/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

//
// This file contains the class methods that create the OpenGL widget
// for drawing a brain surface.
//

#include <QGlobalStatic>
#include <QMouseEvent>
#include <QKeyEvent>

#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include <iostream>
#include <limits>

#include <QApplication>
#include <QCursor>
#include <QDir>
#include <QFont>
#include <QFontMetrics>
#include <QInputDialog>
#include <QMessageBox>
#include <QMutex>
#include <QRegExp>

#include "vtkMath.h"

#define __GUI_BRAIN_SURFACE_OPENGL_INIT_STATIC__
#include "GuiBrainModelOpenGL.h"
#undef  __GUI_BRAIN_SURFACE_OPENGL_INIT_STATIC__

#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelIdentification.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceAndVolume.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderManager.h"
#include "CellFile.h"
#include "CommunicatorClientAFNI.h"
#include "CommunicatorClientFIV.h"
#include "ContourCellFile.h"
#include "CutsFile.h"
#include "DebugControl.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsCoCoMac.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsSurface.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsVolume.h"
#include "DisplaySettingsWustlRegion.h"
#include "FileUtilities.h"
#include "GuiAddCellsDialog.h"
#include "GuiAlignSurfaceToStandardOrientationDialog.h"
#include "GuiBordersCreateInterpolatedDialog.h"
#include "GuiBorderDrawUpdateDialog.h"
#include "GuiBorderOperationsDialog.h"
#include "GuiBrainModelOpenGLKeyEvent.h"
#include "GuiBrainModelOpenGLMouseEvent.h"
#include "GuiBrainModelOpenGLPopupMenu.h"
#include "GuiBrainSetAndModelSelectionControl.h"
#include "GuiContourAlignmentDialog.h"
#include "GuiContourDrawDialog.h"
#include "GuiContourSetScaleDialog.h"
#include "GuiDrawBorderDialog.h"
#include "GuiFilesModified.h"
#include "GuiFlattenFullHemisphereDialog.h"
#include "GuiGraphWidget.h"
#include "GuiGraphWidgetDialog.h"
#include "GuiIdentifyDialog.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowVolumeActions.h"
#include "GuiRecordingDialog.h"
#include "GuiSurfaceRegionOfInterestDialog.h"
#include "GuiSurfaceRegionOfInterestDialogOLD.h"
#include "GuiToolBar.h"
#include "GuiTransformationMatrixDialog.h"
#include "GuiVolumePaintEditorDialog.h"
#include "GuiVolumeResizingDialog.h"
#include "GuiVolumeSegmentationEditorDialog.h"
#include "ImageFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "SectionFile.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "TransformationMatrixFile.h"
#include "VolumeFile.h"
#include "VtkModelFile.h"
#include "WustlRegionFile.h"

#include "global_variables.h"
#include "vtkTransform.h"

// Prevent more than one window from painting simultaneously   
static QMutex paintMutex;

/**
 * The Constructor.
 */
GuiBrainModelOpenGL::GuiBrainModelOpenGL(QWidget* parent, 
                                         GuiBrainModelOpenGL* sharedBrainModelOpenGL,
                                         const char* name,
                                         const BrainModel::BRAIN_MODEL_VIEW_NUMBER svn)
                  : QGLWidget(parent, sharedBrainModelOpenGL)
{
   setObjectName(name);
   
   static bool firstTime = true;
   if (firstTime) {      
      //
      // OpenGL drawing
      //
      openGL = NULL;
      //
      // Mouse button move masks
      // Note: On Macintoshes, Qt::ControlButton is the Apple key
      //
      leftMouseButtonMoveMask        = Qt::NoModifier;
      leftShiftMouseButtonMoveMask   = Qt::ShiftModifier;
      leftControlMouseButtonMoveMask = Qt::ControlModifier;
      leftAltMouseButtonMoveMask     = Qt::AltModifier;

      //
      // Mouse button press masks
      // Note: On Macintoshes, Qt::ControlButton is the Apple key
      //
      leftMouseButtonPressMask        = Qt::NoButton;
      leftShiftMouseButtonPressMask   = Qt::ShiftModifier;
      leftControlMouseButtonPressMask = Qt::ControlModifier;

      firstTime = false;
   }
   drawLinearObjectOnly = false;
   imageSubRegionBoxValid = false;
   imageSubRegionBox[0] = 0;
   imageSubRegionBox[1] = 0;
   imageSubRegionBox[2] = 0;
   imageSubRegionBox[3] = 0;
   viewingWindowIndex = svn;
   allBrainSurfaceOpenGL[viewingWindowIndex] = this;
   
   mouseMode = MOUSE_MODE_VIEW;
   rotationAxis = BRAIN_MODEL_ROTATION_AXIS_XY;
   yokeView = false;
   borderPointBeingMoved[0] = -1;
   
   popupMenu = new GuiBrainModelOpenGLPopupMenu(this);
      
   keyUpLastTime = false;

   setFocusPolicy(Qt::StrongFocus);
}

/**
 * The Destructor.
 */
GuiBrainModelOpenGL::~GuiBrainModelOpenGL()
{
   makeCurrent();
   allBrainSurfaceOpenGL[viewingWindowIndex] = NULL;
   
   //
   // Delete the BrainModelOpenGL when all GuiBrainModelOpenGL are gone
   //
   bool allNull = true;
   for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      if (allBrainSurfaceOpenGL[i] != NULL) {
         allNull = false;
      }
   }
   if (allNull) {
      delete openGL;
      openGL = NULL;
   }
}

/**
 *initialize the OpenGL.
 */
void 
GuiBrainModelOpenGL::initializeGL()
{
   if (openGL == NULL) {
      openGL = new BrainModelOpenGL;
      
      float sizes[2];
      glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
      minPointSize = sizes[0];
      maxPointSize = sizes[1];

      glGetFloatv(GL_LINE_WIDTH_RANGE, sizes);
      minLineSize = sizes[0];
      maxLineSize = sizes[1];
   }
   openGL->initializeOpenGL(false);
}
       
/**
 * Reset the linear object being drawn (border, contour, cut).
 */
void
GuiBrainModelOpenGL::resetLinearObjectBeingDrawn()
{
   linearObjectBeingDrawn.clearLinks();
   linearObjectAugmentBorderCount = 0;
}

/**
 * Called by QT to redraw the OpenGL widget.
 */
void
GuiBrainModelOpenGL::paintGL()
{
   // painting is typically disabled when loading the spec file since the progress dialog
   // may cause a paint event with only part of a coordinate files loaded.
   if (paintingEnabled == false) {
      return;
   }
   
   int viewport[4] = { 0, 0, windowWidth[viewingWindowIndex], windowHeight[viewingWindowIndex] };
   
   //
   // Handle aux windows yoked to main window
   //
   if (viewingWindowIndex != BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
      if (getYokeView()) {
         BrainModelSurface* mainBMS = 
            allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->getDisplayedBrainModelSurface();
         BrainModelSurface* bms = getDisplayedBrainModelSurface();
         if ((mainBMS != NULL) && (bms != NULL)) {
            float translation[3];
            mainBMS->getTranslation(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW, translation);
            bms->setTranslation(viewingWindowIndex, translation);
            float scale[3];
            mainBMS->getScaling(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW, scale);
            bms->setScaling(viewingWindowIndex, scale);
            float matrix[16];
            mainBMS->getRotationMatrix(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW, matrix);
            bms->setRotationMatrix(viewingWindowIndex, matrix);
         }
         else if (bms != NULL) {
            //
            // See if oblique volume view in main window
            //
            BrainModelVolume* mainBMV = 
               allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->getDisplayedBrainModelVolume();
            if (mainBMV != NULL) {
               if (mainBMV->getSelectedAxis(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) == 
                   VolumeFile::VOLUME_AXIS_OBLIQUE) {
                  float matrix[16];
                  mainBMV->getObliqueRotationMatrix(matrix);
                  bms->setRotationMatrix(viewingWindowIndex, matrix);
               }
            }
         }
      }
   }
   
   std::vector<int> specialNodes;
   if (mouseMode == MOUSE_MODE_EDIT_ADD_TILE) {
      for (int ti = 0; ti < newTileNodeCounter; ti++) {
         specialNodes.push_back(newTileNodeNumbers[ti]);      
      }
   }
   openGL->setNodeSpecialHighlighting(specialNodes);
   openGL->setLinearObject(linearObjectBeingDrawn);
   openGL->setDrawLinearObjectOnly(drawLinearObjectOnly);

   BrainModel* bm = getDisplayedBrainModel();
   BrainSet* bs = NULL;
   if (bm != NULL) {
      bs = bm->getBrainSet();
   }
   if (bs == NULL) {
      bs = theMainWindow->getBrainSet(getModelViewNumber());
   }

   openGL->setImageSubRegion(imageSubRegionBox, imageSubRegionBoxValid);
   
   bool drawAllFiducialFlag = false;
   if (modelSelectionControl != NULL) {
      drawAllFiducialFlag = modelSelectionControl->getAllFiducialSelected();
   }
   if (drawAllFiducialFlag) {
      std::vector<BrainSet*> brainSets;
      theMainWindow->getAllBrainSets(brainSets);
      openGL->drawAllFiducialSurfaceBrainModels(brainSets,
                                                viewingWindowIndex,
                                                viewport,
                                                this);
   }
   else {
      openGL->drawBrainModel(bs,
                             bm,
                             viewingWindowIndex,
                             viewport,
                             this);
   }
}

/**
 * Called by QT when the OpenGL widget is resized.
 */
void
GuiBrainModelOpenGL::resizeGL(int w, int h)
{
   windowWidth[viewingWindowIndex]  = w;
   windowHeight[viewingWindowIndex] = h;

   openGL->updateOrthoSize(viewingWindowIndex, w, h);
}

/**
 * Get the dimensions of the surface orthographic projection
 */
void
GuiBrainModelOpenGL::getOrthographicBox(double& orthoLeft,   double& orthoRight,
                                        double& orthoBottom, double& orthoTop,
                                        double& orthoNear,   double& orthoFar) const
{
   openGL->getOrthographicBox(viewingWindowIndex,
                              orthoLeft, orthoRight,
                              orthoBottom, orthoTop,
                              orthoNear, orthoFar);
}

/**
 *  Returns a pointer to the displayed model (NULL if not available)
 */
BrainModel*
GuiBrainModelOpenGL::getDisplayedBrainModel()
{
   if (theMainWindow != NULL) {
      const int numModels = theMainWindow->getBrainSet(getModelViewNumber())->getNumberOfBrainModels();
      if (numModels > 0) {
         if (theMainWindow->getBrainSet(getModelViewNumber())->getDisplayedModelIndexForWindow(viewingWindowIndex) < numModels) {
            return theMainWindow->getBrainSet(getModelViewNumber())->getBrainModel(theMainWindow->getBrainSet(getModelViewNumber())->getDisplayedModelIndexForWindow(viewingWindowIndex));
         }
      }
   }
   return NULL;
}

/**
 *  Returns a pointer to the displayed surface (NULL if a surface is not displayed)
 */
BrainModelSurface*
GuiBrainModelOpenGL::getDisplayedBrainModelSurface()
{
   BrainModel* bm = getDisplayedBrainModel();
   if (bm != NULL) {
      if (bm->getModelType() == BrainModel::BRAIN_MODEL_SURFACE) {
         BrainModelSurface* bms = dynamic_cast<BrainModelSurface*>(bm);
         return bms;
      }
   }
   return NULL;
}

/**
 *  Returns a pointer to the displayed surface and volume (NULL if a surface and volume is not displayed)
 */
BrainModelSurfaceAndVolume*
GuiBrainModelOpenGL::getDisplayedBrainModelSurfaceAndVolume()
{
   BrainModel* bm = getDisplayedBrainModel();
   if (bm != NULL) {
      if (bm->getModelType() == BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME) {
         BrainModelSurfaceAndVolume* bms = dynamic_cast<BrainModelSurfaceAndVolume*>(bm);
         return bms;
      }
   }
   return NULL;
}

/**
 *  Returns a pointer to the displayed contours (NULL if a surface is not displayed)
 */
BrainModelContours*
GuiBrainModelOpenGL::getDisplayedBrainModelContours()
{
   BrainModel* bm = getDisplayedBrainModel();
   if (bm != NULL) {
      if (bm->getModelType() == BrainModel::BRAIN_MODEL_CONTOURS) {
         BrainModelContours* bms = dynamic_cast<BrainModelContours*>(bm);
         return bms;
      }
   }
   return NULL;
}

/**
 *  Returns a pointer to the displayed volume (NULL if a volume is not displayed)
 */
BrainModelVolume*
GuiBrainModelOpenGL::getDisplayedBrainModelVolume()
{
   BrainModel* bm = getDisplayedBrainModel();
   if (bm != NULL) {
      if (bm->getModelType() == BrainModel::BRAIN_MODEL_VOLUME) {
         BrainModelVolume* bmv = dynamic_cast<BrainModelVolume*>(bm);
         return bmv;
      }
   }
   return NULL;
}

/**
 * Display the volume
 */
void
GuiBrainModelOpenGL::displayBrainModelVolume()
{
   for (int i = 0; i < theMainWindow->getBrainSet(getModelViewNumber())->getNumberOfBrainModels(); i++) {
      if (theMainWindow->getBrainSet(getModelViewNumber())->getBrainModelVolume(i) != NULL) {
         setDisplayedBrainModelIndex(i);
         break;
      }
   }
}

/**
 * Called when an Identify mouse click has been made on a brain model.
 */
void
GuiBrainModelOpenGL::selectBrainModelItem(const int selectionX, 
                                          const int selectionY,
                                          const unsigned long selectionMask,
                                          const bool identifyFlag)
{
   makeCurrent();   // 9/13/2005
   
   int viewport[4] = { 0, 0, windowWidth[viewingWindowIndex], windowHeight[viewingWindowIndex] };
   
   BrainModel* bm = getDisplayedBrainModel();
   BrainSet* bs = NULL;
   if (bm != NULL) {
      bs = bm->getBrainSet();
   }
   if (bs == NULL) {
      bs = theMainWindow->getBrainSet(getModelViewNumber());
   }

   if (identifyFlag) {
      BrainModel* allWindowBrainModels[BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
      for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
         allWindowBrainModels[i] = NULL;
         if (allBrainSurfaceOpenGL[i] != NULL) {
            allWindowBrainModels[i] = allBrainSurfaceOpenGL[i]->getDisplayedBrainModel();
         }
      }
      
      QString idText = openGL->identifyBrainModelItem(bs,
                                                      bm,
                                                      allWindowBrainModels,
                                                      viewingWindowIndex,
                                                      viewport,
                                                      this,
                                                      selectionMask,
                                                      selectionX,
                                                      selectionY,
                                                      getInViewMode(),
                                                      true,
                                                      true);
      if (idText.isEmpty() == false) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->appendHtml(idText);
      }
      GuiToolBar::updateAllToolBars(false);
   }
   else {
      openGL->selectBrainModelItem(bs,
                                   bm,
                                   viewingWindowIndex,
                                   viewport,
                                   this,
                                   selectionMask,
                                   selectionX,
                                   selectionY,
                                   getInViewMode());
   }
                          
   selectedSurfaceTile = openGL->getSelectedSurfaceTile();
   selectedNode = openGL->getSelectedNode();
   selectedBorder1 = openGL->getSelectedBorder1();
   selectedBorder2 = openGL->getSelectedBorder2();
   selectedVolumeBorder = openGL->getSelectedVolumeBorder();
   selectedCellProjection = openGL->getSelectedCellProjection();
   selectedVolumeCell = openGL->getSelectedVolumeCell();
   selectedCut = openGL->getSelectedCut();
   selectedFocusProjection = openGL->getSelectedFocusProjection();
   selectedVolumeFoci = openGL->getSelectedVolumeFoci();
   selectedPaletteMetric = openGL->getSelectedPaletteMetric();
   selectedPaletteShape = openGL->getSelectedPaletteShape();
   selectedContour = openGL->getSelectedContour();
   selectedContourCell = openGL->getSelectedContourCell();
   selectedVoxelUnderlay = openGL->getSelectedVoxelUnderlay();
   selectedVoxelOverlaySecondary = openGL->getSelectedVoxelOverlaySecondary();
   selectedVoxelOverlayPrimary = openGL->getSelectedVoxelOverlayPrimary();
   selectedVoxelFunctionalCloud = openGL->getSelectedVoxelFunctionalCloud();
   selectedLink = openGL->getSelectedLink();   
   selectedTransformationAxes = openGL->getSelectedTransformationMatrixAxes();
   selectedVtkModel = openGL->getSelectedVtkModel();
   selectedTransformCell = openGL->getSelectedTransformationCell();
   selectedTransformFocus = openGL->getSelectedTransformationFoci();
   
   if (mouseMode == MOUSE_MODE_VIEW) {
   
      //bool cocomacFlag = false;
      
      if ((selectedNode.getItemIndex1() >= 0) && 
          identifyFlag) {
         const int nodeNum = selectedNode.getItemIndex1();
         if (nodeNum >= 0) {
            BrainSetNodeAttribute* bna = bs->getNodeAttributes(nodeNum);
            if (bna->getHighlighting() == BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE) {
               bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
            }
            else {
               bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE);
            }
            bs->clearAllDisplayLists();
         }
      }

      if ((selectedContour.getItemIndex1() >= 0) &&
          (selectedContour.getItemIndex2() >= 0) &&
          identifyFlag) {
         BrainModelContours* bmc = bs->getBrainModelContours();
         if (bmc != NULL) {
            ContourFile* cf = bmc->getContourFile();
            CaretContour* cc = cf->getContour(selectedContour.getItemIndex1());
            cc->setHighlightFlag(selectedContour.getItemIndex2(),
                                 (! cc->getHighlightFlag(selectedContour.getItemIndex2())));
         }
      }
      
      if (identifyFlag) {
         const int nodeNumber = selectedNode.getItemIndex1();
         if (nodeNumber >= 0) {
            //
            // Pass it on to AFNI & FIV
            //
            theMainWindow->getAfniClientCommunicator()->sendNodeHighlightToAFNI(nodeNumber);
            theMainWindow->getFivClientCommunicator()->sendNodeHighlightToFIV(nodeNumber);
         }
      }
      
      //
      // In surface and volume, display ID for only node/voxel nearest viewer
      //
      /*
      if (getDisplayedBrainModelSurfaceAndVolume() != NULL) {
         if ((selectedNode.getItemIndex1() >= 0) &&
             (selectedVoxelUnderlay.getItemIndex1() >= 0)) {
            if (selectedNode.getDepth() < selectedVoxelUnderlay.getDepth()) {
               selectedVoxelUnderlay.setItemIndex1(-1);
            }
            else {
               selectedNode.setItemIndex1(-1);
            }
         }
      }
      */
      
      const BrainModelSurface* bms = getDisplayedBrainModelSurface();
      if (bms == NULL) {
         bms = dynamic_cast<BrainModelSurface*>(getDisplayedBrainModelSurfaceAndVolume());
      }
      int modelIndex = -1;
      if (bms != NULL) {
         modelIndex = bms->getBrainModelIndex();
      }
      
      /*
      if (selectionMask & BrainModelOpenGL::SELECTION_MASK_NODE) {
         //
         // Update node colors if displaying cocomac
         //
         BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet(getModelViewNumber())->getNodeColoring();
         DisplaySettingsCoCoMac* dsc = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySettingsCoCoMac();
         if ((bsnc->getPrimaryOverlay(modelIndex) == BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC) ||
             (bsnc->getSecondaryOverlay(modelIndex) == BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC) ||
             (bsnc->getUnderlay(modelIndex) == BrainModelSurfaceNodeColoring::UNDERLAY_COCOMAC)) {
            if (selectedNode.getItemIndex1() >= 0) {
               dsc->setSelectedNode(selectedNode.getItemIndex1());
               cocomacFlag = true;
            }
            else {
               dsc->setSelectedNode(-1);
            }
            bsnc->assignColors();
            updateAllGL(NULL);
         }
         else {
            dsc->setSelectedNode(-1);
         }
      }
      */
      
      //
      // If axes selected in a view mode disable any node or voxel identification
      //
      /*
      if (mouseMode == MOUSE_MODE_VIEW) {
         if (selectedTransformationAxes.getItemIndex1() >= 0) {
            selectedNode.setItemIndex1(-1);
            selectedVoxelUnderlay.setItemIndex1(-1);
            selectedVoxelOverlaySecondary.setItemIndex1(-1);
            selectedVoxelOverlayPrimary.setItemIndex1(-1);
            selectedVoxelFunctionalCloud.setItemIndex1(-1);
         }
      }
      */
      
      //
      // Metric graph plotting
      //
      if (selectedNode.getItemIndex1() >= 0) {
         //GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         BrainModelSurface* s = getDisplayedBrainModelSurface();
         if (s == NULL) {
            s = dynamic_cast<BrainModelSurface*>(getDisplayedBrainModelSurfaceAndVolume());
         }
         if (s != NULL) {
            /*
            id->displayNode(s, getModelViewNumber(), selectedNode.getItemIndex1(), 
                            BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL, 
                            true, cocomacFlag);
            */                
            DisplaySettingsMetric* dsm = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySettingsMetric();
            if (dsm->getMDataPlotOnNodeID() != DisplaySettingsMetric::METRIC_DATA_PLOT_OFF) {
               if (theMainWindow->getBrainSet()->isASurfaceOverlayForAnySurface(
                        BrainModelSurfaceOverlay::OVERLAY_METRIC)) {
                  const MetricFile* mf = theMainWindow->getBrainSet(getModelViewNumber())->getMetricFile();
                  const int col = dsm->getFirstSelectedColumnForBrainModel(modelIndex);
                  const int numCols = mf->getNumberOfColumns();
                  if ((col >= 0) && (col < numCols)) {
                     GuiGraphWidgetDialog* graphDialog = new GuiGraphWidgetDialog("Metric Plot",
                                                                                  this);
                     graphDialog->setAttribute(Qt::WA_DeleteOnClose);
                     GuiGraphWidget* graphWidget = graphDialog->getGraphWidget();
                     graphWidget->setLegends("Timepoints", "", "", "Functional Data");
                     std::vector<float> dataYFloat(numCols);
                     mf->getAllColumnValuesForNode(selectedNode.getItemIndex1(), &dataYFloat[0]);
                     std::vector<double> dataX(numCols);
                     std::vector<double> dataY(numCols);
                     for (int i = 0; i < numCols; i++) {
                        dataX[i] = i + 1;
                        dataY[i] = dataYFloat[i];
                     }
                     graphWidget->addData(dataX, dataY, QColor(255, 0, 0),
                                          GuiGraphWidget::DRAW_DATA_TYPE_LINES);
                     if (dsm->getMDataPlotOnNodeID() == DisplaySettingsMetric::METRIC_DATA_PLOT_NODE_AND_NEIGHBORS){
                        const TopologyFile* tf = s->getTopologyFile();
                        if (tf != NULL) {
                           const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
                           int numNeighbors;
                           const int* neighbors = th->getNodeNeighbors(selectedNode.getItemIndex1(),
                                                                       numNeighbors);
                           for (int i = 0; i < numNeighbors; i++) {
                              std::vector<float> dataYFloat(numCols);
                              mf->getAllColumnValuesForNode(neighbors[i], &dataYFloat[0]);
                              std::vector<double> dataY(numCols);
                              for (int i = 0; i < numCols; i++) {
                                 dataY[i] = dataYFloat[i];
                              }
                              graphWidget->addData(dataX, dataY, QColor(255, 0, 0),
                                                   GuiGraphWidget::DRAW_DATA_TYPE_LINES);
                           }
                        }
                     }
                     float minScale, maxScale;
                     if (dsm->getDataPlotManualScaling(minScale, maxScale)) {
                        graphWidget->setScaleYMinimum(minScale);
                        graphWidget->setScaleYMaximum(maxScale);
                     }
                     graphDialog->show();
                     graphDialog->activateWindow();
                  }
               }
            }
            
            //
            // See if region and paint files are loaded
            //
            WustlRegionFile* wrf = theMainWindow->getBrainSet(getModelViewNumber())->getWustlRegionFile();
            const int numTimeCourses = wrf->getNumberOfTimeCourses();
            const int numPaintVolumes = theMainWindow->getBrainSet(getModelViewNumber())->getNumberOfVolumePaintFiles();
            if ((numTimeCourses > 0) && (numPaintVolumes > 0)) {
               const int NUM_COLORS = 23;
               int colorCtr = 0;
               const int colors[NUM_COLORS][3] = {
                  { 255,   0,   0 },
                  { 255,   0, 127 },
                  { 255,   0, 255 },
                  { 255, 127,   0 },
                  { 255, 127, 127 },
                  { 255, 127, 255 },
                  { 255, 255,   0 },
                  { 255, 255, 127 },
                  { 127,   0,   0 },
                  { 127,   0, 127 },
                  { 127,   0, 255 },
                  { 127, 127,   0 },
                  { 127, 127, 127 },
                  { 127, 127, 255 },
                  { 127, 255,   0 },
                  { 127, 255, 127 },
                  { 127, 255, 255 },
                  {   0, 127,   0 },
                  {   0, 127, 127 },
                  {   0, 127, 255 },
                  {   0, 255,   0 },
                  {   0, 255, 127 },
                  {   0, 255, 255 }
               };
               
               //
               // If a popup graph should be displayed
               //
               const DisplaySettingsWustlRegion* dswr = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySettingsWustlRegion();
               if (dswr->getPopupGraphEnabled()) {
                  //
                  // if a valid paint volume is selected
                  //
                  const int paintVolumeIndex = dswr->getSelectedPaintVolume();
                  if ((numPaintVolumes >= 0) && (paintVolumeIndex < numPaintVolumes)) {
                     //
                     // If fiducial surface is valid
                     //
                     BrainModelSurface* fiducial = theMainWindow->getBrainSet(getModelViewNumber())->getActiveFiducialSurface();
                     if (fiducial != NULL) {
                        //
                        // Get the fiducial coordinate
                        //
                        float xyz[3];
                        const CoordinateFile* cf = fiducial->getCoordinateFile();
                        cf->getCoordinate(selectedNode.getItemIndex1(), xyz);
                        
                        // 
                        // Get the paint volume
                        //
                        VolumeFile* paintVolume = theMainWindow->getBrainSet(getModelViewNumber())->getVolumePaintFile(paintVolumeIndex);
                        
                        //
                        // Find the voxel containing the coordinate
                        //
                        int ijk[3];
                        float pcoords[3];
                        if (paintVolume->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords) != 0) {
                           //
                           // Get the voxel
                           //
                           const float voxelFloat = paintVolume->getVoxel(ijk, 0);
                           
                           //
                           // Convert voxel to int
                           //
                           const int roiNumber = static_cast<int>(voxelFloat);
                           
                           //
                           // Get the name of the ROI
                           //
                           if ((roiNumber >= 0) && (roiNumber < paintVolume->getNumberOfRegionNames())) {
                              const QString regionName(paintVolume->getRegionNameFromIndex(roiNumber));
                              if (DebugControl::getDebugOn()) {
                                 std::cout << "Region name identified: " << regionName.toAscii().constData() << std::endl;
                              }  
                              
                              //
                              // The graph widget
                              //
                              GuiGraphWidgetDialog* graph = NULL;
                              GuiGraphWidget* graphWidget = NULL;
                              QString graphTitle;
                              
                              //
                              // Loop through the timecourses
                              //
                              for (int itc = 0; itc < numTimeCourses; itc++) {
                                 //
                                 //
                                 //
                                 bool useTC = false;
                                 switch (dswr->getTimeCourseSelection()) {
                                    case DisplaySettingsWustlRegion::TIME_COURSE_SELECTION_ALL:
                                       graphTitle = "All Time Courses";
                                       useTC = true;
                                       break;
                                    case DisplaySettingsWustlRegion::TIME_COURSE_SELECTION_SINGLE:
                                       if (itc == dswr->getSelectedTimeCourse()) {
                                          useTC = true;
                                       }
                                       break;
                                 }
                                 
                                 //
                                 // Should this time course be displayed ?
                                 //
                                 if (useTC) {
                                    //
                                    // Get the index to the region
                                    //
                                    WustlRegionFile::TimeCourse* tc = wrf->getTimeCourse(itc);
                                    const WustlRegionFile::Region* region = tc->getRegionByName(regionName);
                                    if (region != NULL) {
                                       //
                                       // Get the selected case name
                                       //
                                       const QString caseName = dswr->getSelectedCaseName();
                                       if (caseName.isEmpty() == false) {
                                          //
                                          // Get the region case
                                          //
                                          const WustlRegionFile::RegionCase* rc = 
                                             region->getRegionCaseByName(caseName);
                                          
                                          if (rc != NULL) {
                                             //
                                             // Create the graph if it has not already been created
                                             //
                                             if (graph == NULL) {
                                                graph = new GuiGraphWidgetDialog("", 
                                                                                 this);
                                                graphWidget->setAttribute(Qt::WA_DeleteOnClose);
                                                graphWidget = graph->getGraphWidget();
                                                if (graphTitle.isEmpty() == false) {
                                                   graph->setWindowTitle(graphTitle);
                                                }
                                                else {
                                                   graph->setWindowTitle(tc->getName());
                                                }
                                          
                                                if (dswr->getGraphMode() ==
                                                    DisplaySettingsWustlRegion::GRAPH_MODE_USER_SCALE) {
                                                   float minScale, maxScale;
                                                   dswr->getUserScale(minScale, maxScale);
                                                   graphWidget->setScaleYMinimum(minScale);
                                                   graphWidget->setScaleYMaximum(maxScale);
                                                }
                                             } // if (graph == NULL) 
                                             //
                                             // Get the time points
                                             //
                                             const std::vector<float> dataYFloat = rc->getTimePoints();
                                             std::vector<double> dataX, dataY;
                                             for (int i = 0; i < static_cast<int>(dataY.size()); i++) {
                                                dataX.push_back(i);
                                                dataY.push_back(dataYFloat[i]);
                                             }
                                             
                                             colorCtr++;
                                             if (colorCtr >= NUM_COLORS) {
                                                colorCtr = 0;
                                             }

                                             QColor qcolor(colors[colorCtr][0],
                                                           colors[colorCtr][1],
                                                           colors[colorCtr][2]);

                                             graphWidget->addData(dataX, dataY, qcolor, GuiGraphWidget::DRAW_DATA_TYPE_LINES);                                 
                                             graphWidget->setLegends(regionName, "", "", caseName);
                                          }  // if (rc != NULL)
                                       }  // if (casename.empty() == false)
                                    } // if (region != NULL)
                                 }  // if (useTC)
                              }  // for (int itc...
                              
                              if (graph != NULL) {
                                 graph->show();
                                 graph->activateWindow();
                              } // if (roiNumber
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      else if (selectedSurfaceTile.getItemIndex1() >= 0) {
         //
         // Node not found but tile is so pick a node from the tile
         //
      }

/*      
      if (selectedBorder1.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayBorder(selectedBorder1, getModelViewNumber());
      }
   
      if (selectedBorder2.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayBorder(selectedBorder2, getModelViewNumber());
      }
      
      if (selectedCellProjection.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayCell(selectedCellProjection, getModelViewNumber());
      }
   
      if (selectedVolumeCell.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayVolumeCell(selectedVolumeCell, getModelViewNumber());
      }
   
      if (selectedFocusProjection.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayFocus(getDisplayedBrainModelSurface(), 
                          selectedFocusProjection,
                          getModelViewNumber());
      }
      
      if (selectedVolumeFoci.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayVolumeFocus(selectedVolumeFoci, getModelViewNumber());
      }
      
      if (selectedTransformCell.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayTransformCell(selectedTransformCell, getModelViewNumber());
      }
      
      if (selectedTransformFocus.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayTransformFoci(selectedTransformFocus, getModelViewNumber());
      }
      
      if (selectedPaletteMetric.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
         id->displayPalette(selectedPaletteMetric, getModelViewNumber());
      }
      //if (selectedPaletteShape.getItemIndex1() >= 0) {
      //   GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
      //   id->displayPalette(selectedPaletteShape, getModelViewNumber());
      //}

      BrainModelVolume* bmv = theMainWindow->getBrainSet(getModelViewNumber())->getBrainModelVolume(-1);
      if (bmv != NULL) {
         if (selectedVoxelUnderlay.getItemIndex1() >= 0) {
            TransformationMatrix* tm = NULL;
            if (bmv->getSelectedAxis(viewingWindowIndex) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
               tm = new TransformationMatrix;
               tm->setMatrix(bmv->getObliqueRotationMatrix());
            }
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            id->displayVoxel(bmv->getUnderlayVolumeFile(), selectedVoxelUnderlay, tm, viewingWindowIndex);
            if (tm != NULL) {
               delete tm;
               tm = NULL;
            }
         }
         else if (selectedVoxelOverlaySecondary.getItemIndex1() >= 0) {
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            id->displayVoxel(bmv->getOverlaySecondaryVolumeFile(), selectedVoxelOverlaySecondary, NULL, 
                             getModelViewNumber());
         }
         else if (selectedVoxelOverlayPrimary.getItemIndex1() >= 0) {
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            id->displayVoxel(bmv->getOverlayPrimaryVolumeFile(), selectedVoxelOverlayPrimary, NULL,
                             getModelViewNumber());
         }

         if (selectedVoxelFunctionalCloud.getItemIndex1() >= 0) {
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            id->displayVoxelCloudFunctional(selectedVoxelFunctionalCloud, getModelViewNumber());
         }
         if (selectedVolumeBorder.getItemIndex1() >= 0) {
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            id->displayVolumeBorder(selectedVolumeBorder, getModelViewNumber());
         } 
      }
               
      //
      // If VTK model is selected
      //
      if (selectedVtkModel.getItemIndex1() >= 0) {
         GuiIdentifyDialog* id= theMainWindow->getIdentifyDialog(true);
         id->displayVtkModel(selectedVtkModel.getItemIndex1(), selectedVtkModel.getItemIndex2(), 
                             getModelViewNumber());
      }
      
      BrainModelContours* bmc = theMainWindow->getBrainSet(getModelViewNumber())->getBrainModelContours();
      if (bmc != NULL) {
         if (selectedContour.getItemIndex1() >= 0) {
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            id->displayContour(selectedContour, getModelViewNumber());
         }
         if (selectedContourCell.getItemIndex1() >= 0) {
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            id->displayContourCell(selectedContourCell, getModelViewNumber());
         }
      }
*/
   } // if (mouseMode == VIEW_MODE)

   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * key processing for translation axes.
 */
void 
GuiBrainModelOpenGL::keyTranslationAxes(GuiBrainModelOpenGLKeyEvent& ke)
{
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet(getModelViewNumber())->getTransformationMatrixFile();
   const int indx = tmf->getSelectedTransformationAxesIndex();
   if ((indx < 0) || (indx >= tmf->getNumberOfMatrices())) {
      return;
   }
   
   if (keyUpLastTime) {
      GuiTransformationMatrixDialog* tmd = theMainWindow->getTransformMatrixEditor();
      if (tmd != NULL) {
         tmd->axesEventInMainWindow();
      }
   }

   TransformationMatrix* tm = tmf->getTransformationMatrix(indx);
   
   BrainModelSurface* bms = getDisplayedBrainModelSurface();
   if (bms == NULL) {
      bms = getDisplayedBrainModelSurfaceAndVolume();
   }
   
   vtkTransform* surfRotMatrix = NULL;
   if (bms != NULL) {
      surfRotMatrix = bms->getRotationTransformMatrix(viewingWindowIndex);
   }
   else {
      BrainModelVolume* bmv = getDisplayedBrainModelVolume();
      if (bmv != NULL) {
         surfRotMatrix = bmv->getRotationTransformMatrix(viewingWindowIndex);
      }
      //std::cout << "ERROR: No surface but doing translation axes." << std::endl;
   }
   
   //
   // Panning and zooming
   //
   float dt[4] = { 0.0, 0.0, 0.0, 1.0 };
   if ((ke.key == Qt::Key_Left) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = -1;
      }
      else {
         dt[0] = -5;
      }
   }
   if ((ke.key == Qt::Key_Right) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = 1;
      }
      else {
         dt[0] = 5;
      }
   }
   if ((ke.key == Qt::Key_Up) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = 1;
      }
      else {
         dt[1] = 5;
      }
   }
   if ((ke.key == Qt::Key_Down) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = -1;
      }
      else {
         dt[1] = -5;
      }
   }
   if ((ke.key == Qt::Key_Up) &&
       (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dt[2] = -1;
      }
      else {
         dt[2] = -5;
      }
   }
   if ((ke.key == Qt::Key_Down) &&
       (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dt[2] = 1;
      }
      else {
         dt[2] = 5;
      }
   }
   if ((dt[0] != 0.0) || (dt[1] != 0.0) || (dt[2] != 0.0)) {
/*
      if (surfRotMatrix != NULL) {
         float dt2[4];
         surfRotMatrix->MultiplyPoint(dt, dt2);
         dt[0] = dt2[0];
         dt[1] = dt2[1];
         dt[2] = dt2[2];
      }
      tm->translate(dt[0], dt[1], dt[2]);
*/
      const double txyz[3] = { dt[0], dt[1], dt[2] };
      tm->translate(txyz, surfRotMatrix);
      theMainWindow->updateTransformationMatrixEditor(tm);
      updateAllGL();
      ke.setKeyEventUsed(true);
      return;
   }

   //
   // Rotation
   //
   if ((ke.shiftKeyDown == false) &&
       (ke.controlKeyDown == false)) {
      float rotX = 0.0, rotY = 0.0, rotZ = 0.0;
      if (ke.key == Qt::Key_Left) {
         if (ke.altKeyDown) {
            rotY = -1;
         }
         else {
            rotY = -5;
         }
      }
      if (ke.key == Qt::Key_Right) {
         if (ke.altKeyDown) {
            rotY = 1;
         }
         else {
            rotY = 5;
         }
      }
      if (ke.key == Qt::Key_Up) {
         if (ke.altKeyDown) {
            rotX = -1;
         }
         else {
            rotX = -5;
         }
      }
      if (ke.key == Qt::Key_Down) {
         if (ke.altKeyDown) {
            rotX = 1;
         }
         else {
            rotX = 5;
         }
      }
      if (ke.key == Qt::Key_PageUp) {
         if (ke.altKeyDown) {
            rotZ = 1;
         }
         else {
            rotZ = 5;
         }
      }
      if (ke.key == Qt::Key_PageDown) {
         if (ke.altKeyDown) {
            rotZ = -1;
         }
         else {
            rotZ = -5;
         }
      }
      if ((rotX != 0.0) || (rotY != 0.0) || (rotZ != 0.0)) {
/*
         if (surfRotMatrix != NULL) {
            float dt[4] = { rotX, rotY, rotZ, 1.0 };
            float dt2[4];
            surfRotMatrix->MultiplyPoint(dt, dt2);
            rotX = dt2[0];
            rotY = dt2[1];
            rotZ = dt2[2];
         }

         if (rotX != 0.0) {
            tm->rotateX(rotX);
         }
         if (rotY != 0.0) {
            tm->rotateY(rotY);
         }      
         if (rotZ != 0.0) {
            tm->rotateZ(rotZ);
         }
*/
         //
         // Alter the rotation angles using the surface viewing matrix
         //
         TransformationMatrix t2;
         if (surfRotMatrix != NULL) {
            t2.setMatrix(surfRotMatrix);
         }
         float r[3] = { rotX, rotY, rotZ };
         t2.multiplyPoint(r);
         
         //
         // Create a matrix containing the rotations
         //
         TransformationMatrix t3;
         t3.rotateZ(r[2]);
         t3.rotateX(r[0]);
         t3.rotateY(r[1]);
         
         //
         // Remove translation from axes matrix,
         // Multiply axes matrix by rotation matrix
         // Add translation back to axes matrix
         //
         float tx, ty, tz;
         tm->getTranslation(tx, ty, tz);
         tm->translate(-tx, -ty, -tz);
         tm->preMultiply(t3);
         tm->translate(tx, ty, tz);
               
         theMainWindow->updateTransformationMatrixEditor(tm);
         updateAllGL();
         ke.setKeyEventUsed(true);
         return;
      }
      
      //
      // Reset view
      //
      if (ke.key == Qt::Key_Home) {
         tm->identity();
         theMainWindow->updateTransformationMatrixEditor(tm);
         ke.setKeyEventUsed(true);
         updateAllGL();
         return;
      }
   }
}
       
/**
 * key processing for contours.
 */
void 
GuiBrainModelOpenGL::keyContourView(GuiBrainModelOpenGLKeyEvent& ke)
{
   BrainModelContours* bmc = getDisplayedBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   
   //
   // Panning
   //
   float dt[3] = { 0, 0, 0 };
   if ((ke.key == Qt::Key_Left) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = -1;
      }
      else {
         dt[0] = -5;
      }
   }
   if ((ke.key == Qt::Key_Right) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = 1;
      }
      else {
         dt[0] = 5;
      }
   }
   if ((ke.key == Qt::Key_Up) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = 1;
      }
      else {
         dt[1] = 5;
      }
   }
   if ((ke.key == Qt::Key_Down) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = -1;
      }
      else {
         dt[1] = -5;
      }
   }
   if ((dt[0] != 0.0) || (dt[1] != 0.0) || (dt[2] != 0.0)) {
      float translate[3];
      bmc->getTranslation(viewingWindowIndex, translate);
      translate[0] += dt[0];
      translate[1] += dt[1];
      translate[2] =  dt[2];
      bmc->setTranslation(viewingWindowIndex, translate);
      updateAllGL(this);
      ke.setKeyEventUsed(true);
      return;
   }

   //
   // Do zooming
   //
   float dZoom = 0.0;
   if ((ke.key == Qt::Key_Up) && 
       (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = 1;
      }
      else {
         dZoom = 5;
      }
   }
   else if ((ke.key == Qt::Key_Down) && 
            (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = -1;
      }
      else {
         dZoom = -5;
      }
   }
   if (dZoom != 0.0) {
      float scale[3];
      bmc->getScaling(viewingWindowIndex, scale);
      scale[0] += dZoom * scale[0] * 0.01;
      scale[1] += dZoom * scale[1] * 0.01;
      scale[0] = std::max(scale[0], 0.01f);
      scale[1] = std::max(scale[1], 0.01f);
      scale[2] = 1.0;
      bmc->setScaling(viewingWindowIndex, scale);
      ke.setKeyEventUsed(true);
      updateAllGL(this);
      return;
   }

   //
   // Reset view
   //
   if (ke.key == Qt::Key_Home) {
      bmc->setToStandardView(viewingWindowIndex, BrainModelSurface::VIEW_RESET);
      ke.setKeyEventUsed(true);
      updateAllGL(this);
      return;
   }   
}
       
/**
 * key processing for surface.
 */
void 
GuiBrainModelOpenGL::keySurfaceView(GuiBrainModelOpenGLKeyEvent& ke)
{
   BrainModelSurface* bms = getDisplayedBrainModelSurface();
   DisplaySettingsSurface* dss = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySettingsSurface();

   //
   // The Caret Main Window BrainModelSurface is used when yoking.  It is NULL
   // if the Caret Main Window contains a volume or contours.  When doing a yoked     
   // surface, the transformations associated with the Caret Main Window are upated.     
   //    
   BrainModelSurface* mainWindowModelSurface = theMainWindow->getBrainModelSurface();
   int mainWindowModelViewNumber = -1;
   bool yokeIt = false;
   if (mainWindowModelSurface != NULL) {
      mainWindowModelViewNumber =
         allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->getModelViewNumber();         
      yokeIt = getYokeView(); 
   }           
   
   //
   // Panning
   //
   float dt[3] = { 0, 0, 0 };
   if ((ke.key == Qt::Key_Left) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = -1;
      }
      else {
         dt[0] = -5;
      }
   }
   if ((ke.key == Qt::Key_Right) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = 1;
      }
      else {
         dt[0] = 5;
      }
   }
   if ((ke.key == Qt::Key_Up) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = 1;
      }
      else {
         dt[1] = 5;
      }
   }
   if ((ke.key == Qt::Key_Down) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = -1;
      }
      else {
         dt[1] = -5;
      }
   }
   if ((dt[0] != 0.0) || (dt[1] != 0.0) || (dt[2] != 0.0)) {
      float translate[3];
      bms->getTranslation(viewingWindowIndex, translate);
      //    
      // If yoked, use main window brain surface model for translation
      //    
      if (yokeIt) {
         mainWindowModelSurface->getTranslation(mainWindowModelViewNumber, translate);      
      }     
      translate[0] += dt[0];
      translate[1] += dt[1];
      translate[2] =  dt[2];
      //    
      // If yoked, use main window brain surface model for translation
      //    
      if (yokeIt) {
         mainWindowModelSurface->setTranslation(mainWindowModelViewNumber, translate);
      }
      else {
         bms->setTranslation(viewingWindowIndex, translate);
      }
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      updateAllGL(this);
      ke.setKeyEventUsed(true);
      return;
   }

   //
   // Do zooming
   //
   float dZoom = 0.0;
   if ((ke.key == Qt::Key_Up) && 
       (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = 1;
      }
      else {
         dZoom = 5;
      }
   }
   else if ((ke.key == Qt::Key_Down) && 
            (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = -1;
      }
      else {
         dZoom = -5;
      }
   }
   if (dZoom != 0.0) {
      switch (dss->getViewingProjection()) {
         case DisplaySettingsSurface::VIEWING_PROJECTION_ORTHOGRAPHIC:
            {
               float scale[3];
               bms->getScaling(viewingWindowIndex, scale);
               //
               // If yoked, use main window brain surface model for scaling
               //
               if (yokeIt) {
                  mainWindowModelSurface->getScaling(mainWindowModelViewNumber, scale);
               }
               
               scale[0] += dZoom * scale[0] * 0.01;
               scale[1] += dZoom * scale[1] * 0.01;
               scale[2] += dZoom * scale[2] * 0.01;
               scale[0] = std::max(scale[0], 0.01f);
               scale[1] = std::max(scale[1], 0.01f);
               scale[2] = std::max(scale[2], 0.01f);
               //if (scale < 0.01) scale = 0.01;   // negative would flip surface
               //
               // If yoked, use main window brain surface model for scaling
               //
               if (yokeIt) {
                  mainWindowModelSurface->setScaling(mainWindowModelViewNumber, scale);
               }
               else {
                  bms->setScaling(viewingWindowIndex, scale);
               }
            }
            break;
         case DisplaySettingsSurface::VIEWING_PROJECTION_PERSPECTIVE:
            {
               float zoom = bms->getPerspectiveZooming(viewingWindowIndex);
               //
               // If yoked, use main window brain surface model for scaling
               //
               if (yokeIt) {
                  zoom = mainWindowModelSurface->getPerspectiveZooming(mainWindowModelViewNumber);
               }
               zoom -= (dZoom * 5.0);
               if (yokeIt) {
                  mainWindowModelSurface->setPerspectiveZooming(mainWindowModelViewNumber, zoom);
               }
               else {
                  bms->setPerspectiveZooming(viewingWindowIndex, zoom);
               }
            }
            break;
      }
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      ke.setKeyEventUsed(true);
      updateAllGL(this);
      return;
   }

   //
   // Rotation
   //
   if ((ke.shiftKeyDown == false) &&
       (ke.controlKeyDown == false)) {
      float rotX = 0.0, rotY = 0.0, rotZ = 0.0;
      if (ke.key == Qt::Key_Left) {
         if (ke.altKeyDown) {
            rotY = 1;
         }
         else {
            rotY = 5;
         }
      }
      if (ke.key == Qt::Key_Right) {
         if (ke.altKeyDown) {
            rotY = -1;
         }
         else {
            rotY = -5;
         }
      }
      if (ke.key == Qt::Key_Up) {
         if (ke.altKeyDown) {
            rotX = 1;
         }
         else {
            rotX = 5;
         }
      }
      if (ke.key == Qt::Key_Down) {
         if (ke.altKeyDown) {
            rotX = -1;
         }
         else {
            rotX = -5;
         }
      }
      if (ke.key == Qt::Key_PageUp) {
         if (ke.altKeyDown) {
            rotZ = 1;
         }
         else {
            rotZ = 5;
         }
      }
      if (ke.key == Qt::Key_PageDown) {
         if (ke.altKeyDown) {
            rotZ = -1;
         }
         else {
            rotZ = -5;
         }
      }
      if ((rotX != 0.0) || (rotY != 0.0) || (rotZ != 0.0)) {
         vtkTransform* matrix = bms->getRotationTransformMatrix(viewingWindowIndex);
         
         //
         // If yoked, use main window brain surface model for rotation
         //
         if (yokeIt) {
            matrix = mainWindowModelSurface->getRotationTransformMatrix(mainWindowModelViewNumber);
         }
         
         if (rotX != 0.0) {
            matrix->RotateX(rotX);
         }
         if (rotY != 0.0) {
            matrix->RotateY(rotY);
         }
         if (rotZ != 0.0) {
            matrix->RotateZ(rotZ);
         }
         
         if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
            theMainWindow->updateTransformationMatrixEditor(NULL);
         }
         updateAllGL(this);
         ke.setKeyEventUsed(true);
         return;
      }
   }
   
   //
   // Reset view
   //
   if (ke.key == Qt::Key_Home) {
      bms->setToStandardView(viewingWindowIndex, BrainModelSurface::VIEW_RESET);
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      ke.setKeyEventUsed(true);
      updateAllGL(this);
      return;
   }
}

/**
 * key processing for volume.
 */
void 
GuiBrainModelOpenGL::keyVolumeView(GuiBrainModelOpenGLKeyEvent& ke)
{
   BrainModelVolume* bmv = getDisplayedBrainModelVolume();
   VolumeFile* vf = bmv->getMasterVolumeFile();

   const bool drawAll = 
      (bmv->getSelectedAxis(viewingWindowIndex) == VolumeFile::VOLUME_AXIS_OBLIQUE);

   //
   // Panning
   //
   float dt[3] = { 0, 0, 0 };
   if ((ke.key == Qt::Key_Left) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = -1;
      }
      else {
         dt[0] = -5;
      }
   }
   if ((ke.key == Qt::Key_Right) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = 1;
      }
      else {
         dt[0] = 5;
      }
   }
   if ((ke.key == Qt::Key_Up) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = 1;
      }
      else {
         dt[1] = 5;
      }
   }
   if ((ke.key == Qt::Key_Down) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = -1;
      }
      else {
         dt[1] = -5;
      }
   }
   if ((dt[0] != 0.0) || (dt[1] != 0.0) || (dt[2] != 0.0)) {
      float translate[3];
      bmv->getTranslation(viewingWindowIndex, translate);
      translate[0] += dt[0];
      translate[1] += dt[1];
      translate[2] =  dt[2];
      bmv->setTranslation(viewingWindowIndex, translate);
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      if (drawAll) {
         updateAllGL();
      }
      else {
         updateAllGL(this);
      }
      ke.setKeyEventUsed(true);
      return;
   }

   //
   // Do zooming
   //
   float dZoom = 0.0;
   if ((ke.key == Qt::Key_Up) &&
       (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = 1;
      }
      else {
         dZoom = 5;
      }
   }
   else if ((ke.key == Qt::Key_Down) &&
            (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = -1;
      }
      else {
         dZoom = -5;
      }
   }
   if (dZoom != 0.0) {
      float scale[3];
      bmv->getScaling(viewingWindowIndex, scale);
      scale[0] += dZoom * scale[0] * 0.01;
      scale[1] += dZoom * scale[1] * 0.01;
      scale[2] += dZoom * scale[2] * 0.01;
      scale[0] = std::max(scale[0], 0.01f);
      scale[1] = std::max(scale[1], 0.01f);
      scale[2] = std::max(scale[2], 0.01f);
      //if (scale < 0.01) scale = 0.01;   // negative would flip surface
      bmv->setScaling(viewingWindowIndex, scale);
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      if (drawAll) {
         updateAllGL();
      }
      else {
         updateAllGL(this);
      }
      ke.setKeyEventUsed(true);
      return;
   }


   if (vf != NULL) {
      //
      // get dimensions of volume
      //
      int dim[3];
      vf->getDimensions(dim);
      
      if ((ke.key == Qt::Key_PageUp) || (ke.key == Qt::Key_PageDown)) {
         //
         // Get the current slices displayed
         //
         int orthogonalSlices[3];
         bmv->getSelectedOrthogonalSlices(viewingWindowIndex, orthogonalSlices);
         int obliqueSlices[3];
         bmv->getSelectedObliqueSlices(obliqueSlices);
         int obliqueSliceOffsets[3];
         bmv->getSelectedObliqueSliceOffsets(viewingWindowIndex, obliqueSliceOffsets);
         
         //
         // Change the selected slices
         //
         int indx = -1;
         switch (bmv->getSelectedAxis(viewingWindowIndex)) {
            case VolumeFile::VOLUME_AXIS_X:
               indx = 0;
               break;
            case VolumeFile::VOLUME_AXIS_Y:
               indx = 1;
               break;
            case VolumeFile::VOLUME_AXIS_Z:
               indx = 2;
               break;
            case VolumeFile::VOLUME_AXIS_ALL:
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
               indx = 3;
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
               indx = 4;
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
               indx = 5;
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
               indx = 6;
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         if ((indx >= 0) && (indx <= 2)) {
            int delta = -1;
            if (ke.key == Qt::Key_PageUp) {
               delta = 1;
            }
            orthogonalSlices[indx] += delta;
            orthogonalSlices[indx] = std::max(0, orthogonalSlices[indx]);
            orthogonalSlices[indx] = std::min(dim[indx] - 1, orthogonalSlices[indx]);
            bmv->setSelectedOrthogonalSlices(viewingWindowIndex, orthogonalSlices);
         }
         else if (indx == 3) {
         }
         else if ((indx >= 4) && (indx <= 6)) {
            int delta = -1;
            if (ke.key == Qt::Key_PageUp) {
               delta = 1;
            }
            if (indx == 4) {
               obliqueSliceOffsets[0] += delta;
            }
            else if (indx == 5) {
               obliqueSliceOffsets[1] += delta;
            }
            else if (indx == 6) {
               obliqueSliceOffsets[2] += delta;
            }
            bmv->setSelectedObliqueSliceOffsets(viewingWindowIndex, obliqueSliceOffsets);
         }
         
         
         //
         // Update the toolbars
         //
         GuiToolBar::updateAllToolBars(false);
         
         //
         // Update the display
         //
         if (drawAll) {
            updateAllGL();
         }
         else {
            updateAllGL(this);
         }
         ke.setKeyEventUsed(true);
         return;
      }
   }

   //
   // Reset view
   //
   if (ke.key == Qt::Key_Home) {
      bmv->resetViewingTransform(viewingWindowIndex);
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      if (drawAll) {
         updateAllGL();
      }
      else {
         updateAllGL(this);
      }
      ke.setKeyEventUsed(true);
      return;
   }
}

/**
 * key processing for surface and volume.
 */
void 
GuiBrainModelOpenGL::keySurfaceAndVolumeView(GuiBrainModelOpenGLKeyEvent& ke)
{
   BrainModelSurfaceAndVolume* bmsv = getDisplayedBrainModelSurfaceAndVolume();
   DisplaySettingsSurface* dss = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySettingsSurface();

   //
   // Panning
   //
   float dt[3] = { 0, 0, 0 };
   if ((ke.key == Qt::Key_Left) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = -1;
      }
      else {
         dt[0] = -5;
      }
   }
   if ((ke.key == Qt::Key_Right) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[0] = 1;
      }
      else {
         dt[0] = 5;
      }
   }
   if ((ke.key == Qt::Key_Up) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = 1;
      }
      else {
         dt[1] = 5;
      }
   }
   if ((ke.key == Qt::Key_Down) &&
       ke.shiftKeyDown) {
      if (ke.altKeyDown) {
         dt[1] = -1;
      }
      else {
         dt[1] = -5;
      }
   }
   if ((dt[0] != 0.0) || (dt[1] != 0.0) || (dt[2] != 0.0)) {
      float translate[3];
      bmsv->getTranslation(viewingWindowIndex, translate);
      translate[0] += dt[0];
      translate[1] += dt[1];
      translate[2] =  dt[2];
      bmsv->setTranslation(viewingWindowIndex, translate);
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      updateAllGL(this);
      ke.setKeyEventUsed(true);
      return;
   }

   //
   // Rotation
   //
   if ((ke.shiftKeyDown == false) && (ke.controlKeyDown == false)) {
      float rotX = 0.0, rotY = 0.0, rotZ = 0.0;
      if (ke.key == Qt::Key_Left) {
         if (ke.altKeyDown) {
            rotY = -1;
         }
         else {
            rotY = -5;
         }
      }
      if (ke.key == Qt::Key_Right) {
         if (ke.altKeyDown) {
            rotY = 1;
         }
         else {
            rotY = 5;
         }
      }
      if (ke.key == Qt::Key_Up) {
         if (ke.altKeyDown) {
            rotX = 1;
         }
         else {
            rotX = 5;
         }
      }
      if (ke.key == Qt::Key_Down) {
         if (ke.altKeyDown) {
            rotX = -1;
         }
         else {
            rotX = -5;
         }
      }
      if (ke.key == Qt::Key_PageUp) {
         if (ke.altKeyDown) {
            rotZ = 1;
         }
         else {
            rotZ = 5;
         }
      }
      if (ke.key == Qt::Key_PageDown) {
         if (ke.altKeyDown) {
            rotZ = -1;
         }
         else {
            rotZ = -5;
         }
      }
      if ((rotX != 0.0) || (rotY != 0.0) || (rotZ != 0.0)) {
         vtkTransform* matrix = bmsv->getRotationTransformMatrix(viewingWindowIndex);
         if (rotX != 0.0) {
            matrix->RotateX(rotX);
         }
         if (rotY != 0.0) {
            matrix->RotateY(rotY);
         }      
         if (rotZ != 0.0) {
            matrix->RotateZ(rotZ);
         }
         if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
            theMainWindow->updateTransformationMatrixEditor(NULL);
         }
         updateAllGL(this);
         ke.setKeyEventUsed(true);
         return;
      }
   }

   //
   // Do zooming
   //
   float dZoom = 0.0;
   if ((ke.key == Qt::Key_Up) && 
       (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = 1;
      }
      else {
         dZoom = 5;
      }
   }
   else if ((ke.key == Qt::Key_Down) &&
            (ke.controlKeyDown)) {
      if (ke.altKeyDown) {
         dZoom = -1;
      }
      else {
         dZoom = -5;
      }
   }
   if (dZoom != 0.0) {
      switch (dss->getViewingProjection()) {
         case DisplaySettingsSurface::VIEWING_PROJECTION_ORTHOGRAPHIC:
            {
               float scale[3];
               bmsv->getScaling(viewingWindowIndex, scale);
               scale[0] += dZoom * scale[0] * 0.01;
               scale[1] += dZoom * scale[1] * 0.01;
               scale[2] += dZoom * scale[2] * 0.01;
               scale[0] = std::max(scale[0], 0.01f);
               scale[1] = std::max(scale[1], 0.01f);
               scale[2] = std::max(scale[2], 0.01f);
               //if (scale < 0.01) scale = 0.01;   // negative would flip surface
               bmsv->setScaling(viewingWindowIndex, scale);
            }
            break;
         case DisplaySettingsSurface::VIEWING_PROJECTION_PERSPECTIVE:
            {
               float zoom = bmsv->getPerspectiveZooming(viewingWindowIndex);
               zoom -= (dZoom * 5.0);
               bmsv->setPerspectiveZooming(viewingWindowIndex, zoom);
            }
            break;
      }
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      updateAllGL(this);
      ke.setKeyEventUsed(true);
      return;
   }
   
   //
   // Reset view
   //
   if (ke.key == Qt::Key_Home) {
      bmsv->setToStandardView(viewingWindowIndex, BrainModelSurfaceAndVolume::VIEW_RESET);
      if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         theMainWindow->updateTransformationMatrixEditor(NULL);
      }
      updateAllGL(this);
      ke.setKeyEventUsed(true);
      return;
   }
}

/** 
 * Gets called by QT when a key is released.
 */
void 
GuiBrainModelOpenGL::keyReleaseEvent(QKeyEvent* /*ke*/)
{
   keyUpLastTime = true;
}       

/**
 * Called by QT when a key is pressed.
 */
void
GuiBrainModelOpenGL::keyPressEvent(QKeyEvent* ke)
{
   GuiBrainModelOpenGLKeyEvent kv(ke->key(),
                                  (ke->modifiers() & Qt::ShiftModifier),
                                  (ke->modifiers() & Qt::AltModifier),
                                  (ke->modifiers() & Qt::ControlModifier));
   kv.debug();
                                  
   if (kv.controlKeyDown && 
       (kv.key >= Qt::Key_F1) && 
       (kv.key <= Qt::Key_F15)) {
      const int modelIndex = kv.key - Qt::Key_F1;
      if ((modelIndex >= 0) && (modelIndex < theMainWindow->getBrainSet(getModelViewNumber())->getNumberOfBrainModels())) {
         setDisplayedBrainModelIndex(modelIndex);

         //
         // Update the toolbars
         //
         GuiToolBar::updateAllToolBars(false);
                  
         updateGL();
      }
      
      kv.setKeyEventUsed(true);
   }

   if (kv.getKeyEventUsed() == false) {
      switch(getMouseMode()) {
         case GuiBrainModelOpenGL::MOUSE_MODE_NONE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_VIEW:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DRAW:
            break;
         case MOUSE_MODE_BORDER_DRAW_NEW:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE_POINT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_INTERPOLATE:
            break;
         case MOUSE_MODE_BORDER_INTERPOLATE_NEW:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_MOVE_POINT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_REVERSE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_UPDATE:
            break;
         case MOUSE_MODE_BORDER_UPDATE_NEW:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_BORDER_RENAME:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CUT_DRAW:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CUT_DELETE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_FOCI_DELETE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CELL_ADD:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CELL_DELETE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_BORDER_SELECT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_PAINT_INDEX_SELECT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_METRIC_NODE_SELECT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SHAPE_NODE_SELECT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_GEODESIC_NODE_SELECT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_ALIGN_STANDARD_ORIENTATION:
            break;
         case MOUSE_MODE_ALIGN_STANDARD_ORIENTATION_FULL_HEM_FLATTEN:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_SET_SCALE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DRAW:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN_REGION:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_MOVE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_DELETE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DELETE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_REVERSE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_MERGE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_ADD:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_DELETE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_MOVE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_SEGMENTATION_EDIT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_ADD_NODE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_ADD_TILE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_DELETE_TILE_BY_LINK:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_DISCONNECT_NODE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_EDIT_MOVE_NODE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_TRANSFORMATION_MATRIX_AXES:
            keyTranslationAxes(kv);
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_TRANSFORMATION_MATRIX_SET_TRANSLATE:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_VOLUME_PAINT_EDIT:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_START:
            break;
         case GuiBrainModelOpenGL::MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_END:
            break;
      }
   }

   if (kv.getKeyEventUsed() == false) {
      if (getDisplayedBrainModelSurfaceAndVolume() != NULL) {
         keySurfaceAndVolumeView(kv);
      }
      else if (getDisplayedBrainModelSurface() != NULL) {
         keySurfaceView(kv);
      }
      else if (getDisplayedBrainModelVolume() != NULL) {
         keyVolumeView(kv);
      }
      else if (getDisplayedBrainModelContours() != NULL) {
         keyContourView(kv);
      }
   }
   
   if (kv.getKeyEventUsed() == false) {
      //
      // Key processing with no modifiers - standard views
      //
      int modelNum = viewingWindowIndex;
      BrainModelSurface* bms = getDisplayedBrainModelSurfaceAndVolume();
      if (bms == NULL) {
         bms = getDisplayedBrainModelSurface();
         if (bms != NULL) {
            if (viewingWindowIndex != BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               if (getYokeView()) {
                  bms = allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->getDisplayedBrainModelSurface();
                  modelNum = BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW;
               }
            }
         }
      }
      if (bms != NULL) {
         switch (kv.key) {
            case Qt::Key_A:
               bms->setToStandardView(modelNum,
                                      BrainModelSurface::VIEW_ANTERIOR);
               kv.setKeyEventUsed(true);
               break;
            case Qt::Key_D:
               bms->setToStandardView(modelNum,
                                      BrainModelSurface::VIEW_DORSAL);
               kv.setKeyEventUsed(true);
               break;
            case Qt::Key_L:
               bms->setToStandardView(modelNum,
                                      BrainModelSurface::VIEW_LATERAL);
               kv.setKeyEventUsed(true);
               break;
            case Qt::Key_M:
               bms->setToStandardView(modelNum,
                                      BrainModelSurface::VIEW_MEDIAL);
               kv.setKeyEventUsed(true);
               break;
            case Qt::Key_P:
               bms->setToStandardView(modelNum,
                                      BrainModelSurface::VIEW_POSTERIOR);
               kv.setKeyEventUsed(true);
               break;
            case Qt::Key_R:
               bms->setToStandardView(modelNum,
                                      BrainModelSurface::VIEW_RESET);
               kv.setKeyEventUsed(true);
               break;
            case Qt::Key_V:
               bms->setToStandardView(modelNum,
                                      BrainModelSurface::VIEW_VENTRAL);
               kv.setKeyEventUsed(true);
               break;
         }
         updateAllGL(this);
      }
   }
   
   keyUpLastTime = false;
   
   if (kv.getKeyEventUsed()) {
      ke->accept();
   }
   else {
      ke->ignore();
   }
}

/**
 * Change the mouse mode
 */
void
GuiBrainModelOpenGL::setMouseMode(const GuiBrainModelOpenGL::MOUSE_MODES mm)
{
   //
   // Ignore mouse mode commands for all but main window
   //
   if (isMainWindowOpenGL() == false) {
      if (mm != MOUSE_MODE_VIEW) {
         return;
      }
   }
   
   bool needReDrawFlag = false;

   newTileNodeNumbers[0] = -1;
   newTileNodeNumbers[1] = -1;
   newTileNodeNumbers[2] = -1;
   newTileNodeCounter = 0;
   
   const MOUSE_MODES oldMouseMode = mouseMode;
   if ((mouseMode == MOUSE_MODE_BORDER_DRAW) ||
       (mouseMode == MOUSE_MODE_BORDER_DRAW_NEW) ||
       (mouseMode == MOUSE_MODE_BORDER_MOVE_POINT)) {
      QApplication::restoreOverrideCursor(); 
   }
   mouseMode = mm;
   if (linearObjectBeingDrawn.getNumberOfLinks() > 0) {
      needReDrawFlag = true;
   }
   resetLinearObjectBeingDrawn();
   theMainWindow->getToolBar()->updateMouseModeComboBox();
   theMainWindow->updateStatusBarLabel();

   if (mouseMode == MOUSE_MODE_TRANSFORMATION_MATRIX_AXES) {
      needReDrawFlag = true;
   }
   else if (oldMouseMode == MOUSE_MODE_TRANSFORMATION_MATRIX_AXES) {
      TransformationMatrixFile* tmf = theMainWindow->getBrainSet(getModelViewNumber())->getTransformationMatrixFile();
      tmf->setSelectedTransformationAxesIndex(-1);
      needReDrawFlag = true;
   }
   
   if (mouseMode != MOUSE_MODE_IMAGE_SUBREGION) {
      imageSubRegionBoxValid = false;
   }
   else if (mouseMode == MOUSE_MODE_IMAGE_SUBREGION) {
      imageSubRegionBoxValid = getCaptureImageSubRegionValid();
   }
   
   if (needReDrawFlag) {
      updateAllGL();
   }
}

/**
 * Called by QT when the mouse button pressed.
 */
void 
GuiBrainModelOpenGL::mousePressEvent(QMouseEvent* me)
{
   const bool splashImageWasOn = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySplashImage();
   theMainWindow->getBrainSet(getModelViewNumber())->setDisplaySplashImage(false);
   
   Qt::KeyboardModifiers bs = me->modifiers();
   Qt::MouseButton button = me->button();
   
   const int mouseX = me->x();
   const int mouseY = me->y();
   
   if (button == Qt::LeftButton) {
      //
      // Create the mouse event
      //
      GuiBrainModelOpenGLMouseEvent::BMO_MOUSE_EVENT theEvent = 
                                 GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE;
      bool validMode = false;
      
      //
      // Mouse moved with just left button down
      //
      if (bs == leftMouseButtonPressMask) {
         validMode = true;
         theEvent = GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS;
      }
      //
      // Mouse moved with control key and left mouse button down
      //
      else if (bs == leftControlMouseButtonPressMask) {
         validMode = true;
         theEvent = GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS;
      }
      //
      // Mouse moved with shift key and left mouse button down
      //
      else if (bs == leftShiftMouseButtonPressMask) {
         validMode = true;
         theEvent = GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS;
      }
      
      //
      // If valid mouse/key combination
      //
      if (validMode) {
         //
         // Process the mouse event
         //
         GuiBrainModelOpenGLMouseEvent mouseEvent(theEvent, mouseX, mouseY, 0, 0);
         routeMouseEvent(mouseEvent);
      }
   }
   
   else if (button == Qt::RightButton) {
      if (getDisplayedBrainModelSurface() != NULL) {
         //
         // Perform an ID operation
         // Change the mouse mode to prevent identification from occurring.
         //
         const MOUSE_MODES savedMouseMode = mouseMode;
         mouseMode = MOUSE_MODE_NONE;
         selectBrainModelItem(mouseX, mouseY, BrainModelOpenGL::SELECTION_MASK_ALL);
         mouseMode = savedMouseMode;
         
         //
         // Find the identified items
         //
         std::vector<BrainModelOpenGLSelectedItem> items;
         if (selectedNode.getItemIndex1() >= 0) items.push_back(selectedNode);
         if (selectedBorder1.getItemIndex1() >= 0) items.push_back(selectedBorder1);
         if (selectedBorder2.getItemIndex1() >= 0) items.push_back(selectedBorder2);
         if (selectedCellProjection.getItemIndex1() >= 0) items.push_back(selectedCellProjection);
         if (selectedVolumeCell.getItemIndex1() >= 0) items.push_back(selectedVolumeCell);
         if (selectedCut.getItemIndex1() >= 0) items.push_back(selectedCut);
         if (selectedFocusProjection.getItemIndex1() >= 0) items.push_back(selectedFocusProjection);
         if (selectedVolumeFoci.getItemIndex1() >= 0) items.push_back(selectedVolumeFoci);
         if (selectedPaletteMetric.getItemIndex1() >= 0) items.push_back(selectedPaletteMetric);
         if (selectedPaletteShape.getItemIndex1() >= 0) items.push_back(selectedPaletteShape);
         if (selectedContour.getItemIndex1() >= 0) items.push_back(selectedContour);
         if (selectedContourCell.getItemIndex1() >= 0) items.push_back(selectedContourCell);
         if (selectedVoxelUnderlay.getItemIndex1() >= 0) items.push_back(selectedVoxelUnderlay);
         if (selectedVoxelOverlaySecondary.getItemIndex1() >= 0) items.push_back(selectedVoxelOverlaySecondary);
         if (selectedVoxelOverlayPrimary.getItemIndex1() >= 0) items.push_back(selectedVoxelOverlayPrimary);
         if (selectedVoxelFunctionalCloud.getItemIndex1() >= 0) items.push_back(selectedVoxelFunctionalCloud);
         if (selectedLink.getItemIndex1() >= 0) items.push_back(selectedLink);
         if (selectedTransformationAxes.getItemIndex1() >= 0) items.push_back(selectedTransformationAxes);
         if (selectedVtkModel.getItemIndex1() >= 0) items.push_back(selectedVtkModel);
         if (selectedTransformCell.getItemIndex1() >= 0) items.push_back(selectedTransformCell);
         if (selectedTransformFocus.getItemIndex1() >= 0) items.push_back(selectedTransformFocus);
         
         //
         // Update and popup the popup menu
         //
         popupMenu->setSelectedItems(items);
         popupMenu->exec(QCursor::pos());
         updateGL();
      }
   }

   lastMouseX  = mouseX;
   lastMouseY  = mouseY;
   mousePressX = mouseX;
   mousePressY = mouseY;
   
   mouseMovedBounds[0] = mouseX;
   mouseMovedBounds[1] = mouseY;
   mouseMovedBounds[2] = mouseX;
   mouseMovedBounds[3] = mouseY;
   
   if (splashImageWasOn) {
      updateAllGL(NULL);
   }
}

/**
 * Called by QT when the mouse button is released.
 */
void
GuiBrainModelOpenGL::mouseReleaseEvent(QMouseEvent* me)
{
   const int x = me->x();
   const int y = me->y();
   
   mouseMovedBounds[0] = std::min(mouseMovedBounds[0], x);
   mouseMovedBounds[1] = std::min(mouseMovedBounds[1], y);
   mouseMovedBounds[2] = std::max(mouseMovedBounds[2], x);
   mouseMovedBounds[3] = std::max(mouseMovedBounds[3], y);

   if ((me->button() == Qt::LeftButton) && (me->button() == Qt::LeftButton)) {
      //
      //
      // Perform an ID operation if the minimum and maximum position of the mouse
      // while the mouse button was held down is very small.
      //
      const int dx = static_cast<int>(fabs(static_cast<double>(mouseMovedBounds[0] - mouseMovedBounds[2])));
      const int dy = static_cast<int>(fabs(static_cast<double>(mouseMovedBounds[1] - mouseMovedBounds[3])));
      if ((dx <= mouseMoveTolerance) && (dy <= mouseMoveTolerance)) {
         //
         // Create the mouse event
         //
         GuiBrainModelOpenGLMouseEvent mouseEvent(
               GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK, x, y, 0, 0);
         routeMouseEvent(mouseEvent);                  
      }
      else {
         //
         // Create the mouse event
         //
         GuiBrainModelOpenGLMouseEvent mouseEvent(
               GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE, x, y, 0, 0);
         routeMouseEvent(mouseEvent);                  
      }
   }
}

/**
 * Convert window coords to model coords
 */
bool
GuiBrainModelOpenGL::convertWindowToModelCoords(const int windowX, const int windowY,
                                                const bool useZBufferForWindowZ,
                                                float& modelX, float& modelY, float& modelZ)
{
   GLdouble winx, winy, winz, objx, objy, objz;       

   GLint* selectionViewport = openGL->getSelectionViewport(viewingWindowIndex);
   winx = windowX;
   winy = selectionViewport[3] - windowY;
   winz = 0.0;
   if (useZBufferForWindowZ) {
      updateAllGL(this);
      GLfloat zDepth = 0.0;
      glReadPixels(windowX, selectionViewport[3] - windowY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zDepth);
      winz = zDepth;
   }
   if(gluUnProject(winx, winy, winz, openGL->getSelectionModelviewMatrix(viewingWindowIndex), 
                   openGL->getSelectionProjectionMatrix(viewingWindowIndex), 
                   selectionViewport, &objx, &objy, &objz) == GL_FALSE) {
      std::cerr << "PROGRAM ERROR: gluUnProject failed!" << std::endl;
      modelX = 0;
      modelY = 0;
      modelZ = 0;
      return false;
   }
   modelX = objx;
   modelY = objy;
   modelZ = objz;
   return true;
}

/**
 * Called by QT when the mouse is moved with the mouse button held down.
 */
void
GuiBrainModelOpenGL::mouseMoveEvent(QMouseEvent* me)
{
   Qt::KeyboardModifiers bs = me->modifiers();
   Qt::MouseButtons button = me->buttons();
   const int x = me->x();
   const int y = me->y();
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Button State: " << (int)bs << std::endl;
      std::cout << "Button " << (int)button << std::endl;
   }
   
   mouseMovedBounds[0] = std::min(mouseMovedBounds[0], x);
   mouseMovedBounds[1] = std::min(mouseMovedBounds[1], y);
   mouseMovedBounds[2] = std::max(mouseMovedBounds[2], x);
   mouseMovedBounds[3] = std::max(mouseMovedBounds[3], y);
   
   const int dx = static_cast<int>(fabs(static_cast<double>(mouseMovedBounds[0] - mouseMovedBounds[2])));
   const int dy = static_cast<int>(fabs(static_cast<double>(mouseMovedBounds[1] - mouseMovedBounds[3])));
   
   bool doIt = true;
   if (mouseMode == MOUSE_MODE_VIEW) {
      //
      // Ignore very small movements in view mode to allow ID operation
      //
      if ((dx <= mouseMoveTolerance) && (dy <= mouseMoveTolerance)) {
         doIt = false;
      }
   }
   
   if (doIt) {
      //QT4if (bs & leftMouseButtonMoveMask) {
      if (button == Qt::LeftButton) {
         //
         // Create the mouse event
         //
         GuiBrainModelOpenGLMouseEvent::BMO_MOUSE_EVENT theEvent = 
                                    GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE;
         bool validMode = false;
         
         //
         // Mouse moved with just left button down
         //
         if (bs == leftMouseButtonMoveMask) {
            validMode = true;
            theEvent = GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE;
         }
         //
         // Mouse moved with control key and left mouse button down
         //
         else if (bs == leftControlMouseButtonMoveMask) {
            validMode = true;
            theEvent = GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE;
         }
         //
         // Mouse moved with shift key and left mouse button down
         //
         else if (bs == leftShiftMouseButtonMoveMask) {
            validMode = true;
            theEvent = GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE;
         }
         //
         // Mouse moved with alt key and left mouse button down
         //
         else if (bs == leftAltMouseButtonMoveMask) {
            validMode = true;
            theEvent = GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE;
         }
         
         //
         // If valid mouse/key combination
         //
         if (validMode) {
            //
            // Compute change in mouse position
            //
            PreferencesFile* pf = theMainWindow->getBrainSet(getModelViewNumber())->getPreferencesFile();
            const float multiplier = pf->getMouseSpeed();
            const int dx = static_cast<int>((x - lastMouseX) * multiplier);
            const int dy = static_cast<int>((lastMouseY - y) * multiplier);  // origin at top
            
            //
            // Process the mouse event
            //
            GuiBrainModelOpenGLMouseEvent mouseEvent(theEvent, x, y, dx, dy);
            routeMouseEvent(mouseEvent);
         }
      }
   }

   lastMouseX = x;
   lastMouseY = y;
}

/**
 * Get the brain set for this OpenGL Widget.
 */
BrainSet* 
GuiBrainModelOpenGL::getBrainSet()
{
   return theMainWindow->getBrainSet(getModelViewNumber());
}
       
/**
 * Get the index of the brain model displayed in this widget
 */
int
GuiBrainModelOpenGL::getDisplayedBrainModelIndex() const
{
   return theMainWindow->getBrainSet(getModelViewNumber())->getDisplayedModelIndexForWindow(viewingWindowIndex);
}

/**
 * Set the index of the surface displayed in this widget
 */
void
GuiBrainModelOpenGL::setDisplayedBrainModelIndex(const int newIndex)
{
   if (newIndex < theMainWindow->getBrainSet(getModelViewNumber())->getNumberOfBrainModels()) {
      theMainWindow->getBrainSet(getModelViewNumber())->setDisplayedModelIndexForWindow(viewingWindowIndex, newIndex);
      const BrainModelSurface* bms = theMainWindow->getBrainSet(getModelViewNumber())->getBrainModelSurface(newIndex);
      if (bms != NULL) {
         bool flatFlag = false;
         switch (bms->getSurfaceType()) {
            case BrainModelSurface::SURFACE_TYPE_RAW:
               break;
            case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
               break;
            case BrainModelSurface::SURFACE_TYPE_INFLATED:
               break;
            case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
               break;
            case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
               break;
            case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
                break;
            case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
               break;
            case BrainModelSurface::SURFACE_TYPE_FLAT:
               flatFlag = true;
               break;
            case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
               flatFlag = true;
               break;
            case BrainModelSurface::SURFACE_TYPE_HULL:
               break;
            case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
               break;
            case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
               break;
         }
         if (flatFlag) {
            setRotationAxis(BRAIN_MODEL_ROTATION_AXIS_OFF);
         }
         else if (getRotationAxis() == BRAIN_MODEL_ROTATION_AXIS_OFF) {
            setRotationAxis(BRAIN_MODEL_ROTATION_AXIS_XY);
         }
         GuiToolBar::updateAllToolBars(false);
      }
   }
}

/**
 * Set the brain model displayed in this widget
 */
void
GuiBrainModelOpenGL::setDisplayedBrainModel(const BrainModel* bm)
{
   for (int i = 0; i < theMainWindow->getBrainSet(getModelViewNumber())->getNumberOfBrainModels(); i++) {
      if (theMainWindow->getBrainSet(getModelViewNumber())->getBrainModel(i) == bm) {
         setDisplayedBrainModelIndex(i);
         break;
      }
   }
}

/**
 * Set the index of the surface displayed in all widgets
 */
void
GuiBrainModelOpenGL::setAllDisplayedBrainModelIndices(const int newIndex)
{
   for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      if (allBrainSurfaceOpenGL[i] != NULL) {
         allBrainSurfaceOpenGL[i]->setDisplayedBrainModelIndex(newIndex);
      }
   }
}

/**
 * Redraws OpenGL windows.  If "modelToUpdate" is NULL, all windows are redrawn.
 * If "modelToUpdate" is not NULL, the specified window and any windows yoked to 
 * it are redrawn.
 */
void
GuiBrainModelOpenGL::updateAllGL(GuiBrainModelOpenGL *modelToUpdate)
{
   bool updatedMainWindow = false;
   
   if (modelToUpdate != NULL) {
      modelToUpdate->updateGL();
      
      bool updateAllYoked = false;

      //
      // Is this the main window
      //
      if (modelToUpdate->viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         updateAllYoked = true;
         updatedMainWindow = true;
      }
      else {
         //
         // See if this model is yoked
         //
         if (modelToUpdate->getYokeView()) {
            updateAllYoked = true;
            
            //
            // update the main window too
            //
            allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->updateGL();
            updatedMainWindow = true;
         }
      }
      
      if (updateAllYoked) {
         //
         // Update all yoked view except one that was drawing at beginning of method.
         //
         for (int i = BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2; i < BrainModel::BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
            if (allBrainSurfaceOpenGL[i] != NULL) {
               if (allBrainSurfaceOpenGL[i]->getYokeView()) {
                  if (allBrainSurfaceOpenGL[i] != modelToUpdate) {
                     allBrainSurfaceOpenGL[i]->updateGL();
                  }
               }
            }
         }
      }
   }
   else {
      //
      // Update all models.
      //
      for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
         if (allBrainSurfaceOpenGL[i] != NULL) {
            allBrainSurfaceOpenGL[i]->updateGL();
         }
      }
      updatedMainWindow = true;
   }
   
   //
   // See if recording is turned on
   //
   if (updatedMainWindow) {
      GuiRecordingDialog* rd = theMainWindow->getRecordingDialog(false);
      if (rd->getRecordingOn()) {
#ifdef Q_OS_WIN32
         //
         // Kludge for windows:  If lighting is off (and Caret always draws flat 
         // surfaces with lighting off) the captured images are screwed up.  So, make this
         // flat surface "fiducial" so that its captured image will look normal and 
         // restore the surface type after capturing the image.
         //
         BrainModelSurface::SURFACE_TYPES oldSurfaceType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
         bool surfaceModified = false;
         BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
         if (bms != NULL) {
            if ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT) ||
                (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
               oldSurfaceType = bms->getSurfaceType();
               surfaceModified = bms->getCoordinateFile()->getModified();
               bms->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
            }
         }
#endif // Q_OS_WIN32

         //
         // Capture image of main window
         //
         QImage image;
         allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->captureImage(image);
         rd->addImageToRecording(image);
         
#ifdef Q_OS_WIN32
         if (oldSurfaceType != BrainModelSurface::SURFACE_TYPE_UNKNOWN) {
            bms->setSurfaceType(oldSurfaceType);
            if (surfaceModified == false) {
               bms->getCoordinateFile()->clearModified();
            }
         }
#endif // Q_OS_WIN32
      }
   }

   //
   // Clear display lists for any surface that are not displayed
   //
   for (int j = 0; j < theMainWindow->getBrainSet(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW)->getNumberOfBrainModels(); j++) {
      BrainModelSurface* bms = theMainWindow->getBrainSet(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW)->getBrainModelSurface(j);
      if (bms != NULL) {
         bool found = false;
         for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
            if (allBrainSurfaceOpenGL[i] != NULL) {
               if (allBrainSurfaceOpenGL[i]->getDisplayedBrainModelSurface() == bms) {
                  found = true;
                  break;
               }
            }
         }
         if (found == false) {
            CoordinateFile* cf = bms->getCoordinateFile();
            cf->clearDisplayList();
         }
      }
   }
}

/**
 * Save graphics window to image file.
 * See QImageIO::outputFormats for "format" values.
 */
bool
GuiBrainModelOpenGL::saveImage(const QString& name, 
                               const QString& format,
                               const bool addToSpecFile)
{
  // PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();

   const int imageQuality = 100;   // 0 = highly compressed,  100 = uncompressed high quality

   QImage image;
   captureImage(image);
   if (image.save(name, format.toAscii().constData(), imageQuality) == false) {
      return true;
   }
   
   ImageFile* img = new ImageFile(image);
   img->setFileName(name);
   theMainWindow->getBrainSet(getModelViewNumber())->addImageFile(img);
   if (addToSpecFile) {
      theMainWindow->getBrainSet(getModelViewNumber())->addToSpecFile(SpecFile::getImageFileTag(), name);
   }
   img->clearModified();
   
   //
   // Files have changed
   //
   GuiFilesModified fm;
   fm.setImagesModified();
   theMainWindow->fileModificationUpdate(fm);
      

   return false;
}

/**
 * get image sub region valid.
 */
bool 
GuiBrainModelOpenGL::getCaptureImageSubRegionValid() const
{
   //if (imageSubRegionBoxValid) {
      const int x1 = std::min(imageSubRegionBox[0], imageSubRegionBox[2]) + 1;
      const int x2 = std::max(imageSubRegionBox[0], imageSubRegionBox[2]) - 1;
      const int y1 = std::min(imageSubRegionBox[1], imageSubRegionBox[3]) + 1;
      const int y2 = std::max(imageSubRegionBox[1], imageSubRegionBox[3]) - 1;

      //
      // Width and height of image (do not include subregion lines)
      //
      const int w = x2 - x1;
      const int h = y2 - y1;
      
      //
      // valid if image has width and height
      //
      if ((w > 0) && (h > 0)) {
         return true;
      }
   //}
   return false;
}

/**
 * Capture a portion of the image of the current graphics.
 */
void 
GuiBrainModelOpenGL::captureImageSubRegion(QImage& image)
{
   //
   // Capture the image bug turn off the subregion box so it does not show up in image
   //
   //openGL->setImageSubRegion(imageSubRegionBox, false);
   //updateGL();
   captureImage(image);
   //openGL->setImageSubRegion(imageSubRegionBox, imageSubRegionBoxValid);
   
   if (imageSubRegionBoxValid) {
      //
      // Extent of image
      //
      const int x1 = std::min(imageSubRegionBox[0], imageSubRegionBox[2]) + 1;
      const int x2 = std::max(imageSubRegionBox[0], imageSubRegionBox[2]) - 1;
      const int y1 = std::min(imageSubRegionBox[1], imageSubRegionBox[3]) + 1;
      const int y2 = std::max(imageSubRegionBox[1], imageSubRegionBox[3]) - 1;

      //
      // Width and height of image (do not include subregion lines)
      //
      const int w = x2 - x1;
      const int h = y2 - y1;
      
      if ((w <= 0) || (h <= 0)) {
         return;
      }
      
      //
      // Image origin is at top but subregion has origin at bottom
      //
      const int windowHeight = height();
      const int ypos = windowHeight - (y1 + h);
      
      //
      // Crop the image
      //
      image = image.copy(x1, ypos, w, h);
   }
   
   //ImageFile* img = new ImageFile(image);
   //img->setFileName("subregion.jpg");
   //theMainWindow->getBrainSet(getModelViewNumber())->addImageFile(img);
}
       
/**
 * Capture an image of the current graphics.
 */
void
GuiBrainModelOpenGL::captureImage(QImage& image)
{
   PreferencesFile* pf = theMainWindow->getBrainSet(getModelViewNumber())->getPreferencesFile();
   
   //
   // Display lists do not work during screen capture - do not know why.
   // So disable display lists, capture image, re-enable display lists.
   //
   const bool displayListsValid = pf->getDisplayListsEnabled();
   if (displayListsValid) {
      theMainWindow->getBrainSet(getModelViewNumber())->clearAllDisplayLists();
      pf->setDisplayListsEnabled(false);
   }
   
   switch(pf->getImageCaptureType()) {
      case PreferencesFile::IMAGE_CAPTURE_PIXMAP:
         {
            image = renderPixmap().toImage();
         }
         break;
      case PreferencesFile::IMAGE_CAPTURE_OPENGL_BUFFER:
         {
            updateGL(); 
            image = grabFrameBuffer();
         }
         break;
   }
   
   //
   // Re-enable display lists
   //
   if (displayListsValid) {
      pf->setDisplayListsEnabled(true);
   }
}

 
/**
 * Returns information about the BrainModelSurface displayed in the caret main window.  
 * If contours or a volume is displayed in the caret main window, false is returned. 
 * true is returned if a surface is in the caret main mindow.
 *   mainCaretWindowModelSurface - Will contain the surface in the caret main window.
 *                                 NULL if surface not in caret main window.
 *   mainCaretWindowBrainModelOpenGL - Will contain the GuiGrainModelOpenGL widget in
 *                                   - the caret main window (always valid).
 *   modelViewIndex - surface model index viewed in caret main window.
 */ 
bool
GuiBrainModelOpenGL::getCaretMainWindowModelInfo(
                                   BrainModelSurface*& mainCaretWindowModelSurface,
                                   GuiBrainModelOpenGL*& mainCaretWindowBrainModelOpenGL,
                                   int& modelViewIndex)
{
   mainCaretWindowBrainModelOpenGL = allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW];
   mainCaretWindowModelSurface = mainCaretWindowBrainModelOpenGL->getDisplayedBrainModelSurface();
   if (mainCaretWindowModelSurface != NULL) {
      modelViewIndex = mainCaretWindowBrainModelOpenGL->getModelViewNumber();
      return true;
   }
   return false;
}

/**
 * Route the mouse event to the appropriate handler.
 */
void
GuiBrainModelOpenGL::routeMouseEvent(const GuiBrainModelOpenGLMouseEvent& me)
{
   //
   // Is this the main window
   //
   if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
      switch(mouseMode) {
         case MOUSE_MODE_NONE:
            break;
         case MOUSE_MODE_VIEW:
            {
               const BrainModel* bm = getDisplayedBrainModel();
               if (bm != NULL) {
                  switch(bm->getModelType()) {
                     case BrainModel::BRAIN_MODEL_CONTOURS:
                        mouseContourView(me);
                        break;
                     case BrainModel::BRAIN_MODEL_SURFACE:
                        mouseSurfaceView(me);
                        break;
                     case BrainModel::BRAIN_MODEL_VOLUME:
                        mouseVolumeView(me);
                        break;
                     case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
                        mouseSurfaceAndVolumeView(me);
                        break;
                  }
               }
            }
            break;
         case MOUSE_MODE_BORDER_DRAW:
            mouseBorderDraw(me);
            break;
         case MOUSE_MODE_BORDER_DRAW_NEW:
            mouseBorderDrawNew(me);
            break;
         case MOUSE_MODE_BORDER_DELETE:
            mouseBorderDelete(me);
            break;
         case MOUSE_MODE_BORDER_DELETE_POINT:
            mouseBorderDeletePoint(me);
            break;
         case MOUSE_MODE_BORDER_INTERPOLATE:
            mouseBorderInterpolate(me);
            break;
         case MOUSE_MODE_BORDER_INTERPOLATE_NEW:
            mouseBorderInterpolateNew(me);
            break;
         case MOUSE_MODE_BORDER_MOVE_POINT:
            mouseBorderMovePoint(me);
            break;
         case MOUSE_MODE_BORDER_REVERSE:
            mouseBorderReverse(me);
            break;
         case MOUSE_MODE_BORDER_RENAME:
            mouseBorderRename(me);
            break;
         case MOUSE_MODE_BORDER_UPDATE:
            mouseBorderUpdate(me);
            break;
         case MOUSE_MODE_BORDER_UPDATE_NEW:
            mouseBorderUpdateNew(me);
            break;
         case MOUSE_MODE_CUT_DRAW:
            mouseCutDraw(me);
            break;
         case MOUSE_MODE_CUT_DELETE:
            mouseCutDelete(me);
            break;
         case MOUSE_MODE_FOCI_DELETE:
            mouseFociDelete(me);
            break;
         case MOUSE_MODE_CELL_DELETE:
            mouseCellDelete(me);
            break;
         case MOUSE_MODE_SURFACE_ROI_BORDER_SELECT:
            mouseSurfaceRoiBorderSelect(me);
            break;
         case MOUSE_MODE_SURFACE_ROI_PAINT_INDEX_SELECT:
            mouseSurfaceRoiPaintSelect(me);
            break;
         case MOUSE_MODE_SURFACE_ROI_METRIC_NODE_SELECT:
            mouseSurfaceRoiMetricSelect(me);
            break;
         case MOUSE_MODE_SURFACE_ROI_GEODESIC_NODE_SELECT:
            mouseSurfaceRoiGeodesicNodeSelect(me);
            break;
         case MOUSE_MODE_ALIGN_STANDARD_ORIENTATION:
            mouseAlignStandardOrientation(me);
            break;
         case MOUSE_MODE_ALIGN_STANDARD_ORIENTATION_FULL_HEM_FLATTEN:
            mouseAlignStandardOrientationFullHemFlatten(me);
            break;
         case MOUSE_MODE_CELL_ADD:
            mouseCellAdd(me);
            break;
         case MOUSE_MODE_CONTOUR_SET_SCALE:
            mouseContourSetScale(me);
            break;
         case MOUSE_MODE_CONTOUR_DRAW:
            mouseContourDraw(me);
            break;
         case MOUSE_MODE_CONTOUR_ALIGN:
            mouseContourAlign(me);
            break;
         case MOUSE_MODE_CONTOUR_ALIGN_REGION:
            mouseContourAlignRegion(me);
            break;
         case MOUSE_MODE_CONTOUR_POINT_MOVE:
            mouseContourPointMove(me);
            break;
         case MOUSE_MODE_CONTOUR_POINT_DELETE:
            mouseContourPointDelete(me);
            break;
         case MOUSE_MODE_CONTOUR_DELETE:
            mouseContourDelete(me);
            break;
         case MOUSE_MODE_CONTOUR_REVERSE:
            mouseContourReverse(me);
            break;
         case MOUSE_MODE_CONTOUR_MERGE:
            mouseContourMerge(me);
            break;
         case MOUSE_MODE_CONTOUR_CELL_ADD:
            mouseContourCellAdd(me);
            break;
         case MOUSE_MODE_CONTOUR_CELL_DELETE:
            mouseContourCellDelete(me);
            break;
         case MOUSE_MODE_CONTOUR_CELL_MOVE:
            mouseContourCellMove(me);
            break;
         case MOUSE_MODE_VOLUME_SEGMENTATION_EDIT:
            mouseVolumeSegmentationEdit(me);
            break;
         case MOUSE_MODE_EDIT_ADD_NODE:
            mouseAddNodes(me);
            break;
         case MOUSE_MODE_EDIT_ADD_TILE:
            mouseAddTile(me);
            break;
         case MOUSE_MODE_EDIT_DELETE_TILE_BY_LINK:
            mouseDeleteTileByLink(me);
            break;
         case MOUSE_MODE_EDIT_DISCONNECT_NODE:
            mouseDisconnectNode(me);
            break;
         case MOUSE_MODE_EDIT_MOVE_NODE:
            mouseMoveNode(me);
            break;
         case MOUSE_MODE_SURFACE_ROI_SHAPE_NODE_SELECT:
            mouseSurfaceRoiShapeSelect(me);
            break;
         case MOUSE_MODE_TRANSFORMATION_MATRIX_SET_TRANSLATE:
            mouseTranslationAxesSetTranslate(me);
            break;
         case MOUSE_MODE_TRANSFORMATION_MATRIX_AXES:
            mouseTranslationAxes(me);
            break;
         case MOUSE_MODE_VOLUME_PAINT_EDIT:
            mouseVolumePaintEdit(me);
            break;
         case MOUSE_MODE_IMAGE_SUBREGION:
            mouseImageSubRegion(me);
            break;
         case MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_START:
            mouseSurfaceROISulcalBorderNodeStart(me);
            break;
         case MOUSE_MODE_SURFACE_ROI_SULCAL_BORDER_NODE_END:
            mouseSurfaceROISulcalBorderNodeEnd(me);
            break;
      }
   }
   else {
      //
      // Viewing windows always rotate/scale/pan surface
      //
      const BrainModel* bm = getDisplayedBrainModel();
      if (bm != NULL) {
         switch(bm->getModelType()) {
            case BrainModel::BRAIN_MODEL_CONTOURS:
               mouseContourView(me);
               break;
            case BrainModel::BRAIN_MODEL_SURFACE:
               mouseSurfaceView(me);
               break;
            case BrainModel::BRAIN_MODEL_VOLUME:
               mouseVolumeView(me);
               break;
            case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
               mouseSurfaceAndVolumeView(me);
               break;
         }
      }
   }
}

/**
 * mouse processing for surface view mode
 */
void  
GuiBrainModelOpenGL::mouseSurfaceAndVolumeView(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelSurfaceAndVolume* bmsv = getDisplayedBrainModelSurfaceAndVolume();
   
   //
   // Biggest change in mouse axis
   //
   const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         //
         // Perform auto loading
         //
         {
            BrainSet* bs = theMainWindow->getBrainSet(getModelViewNumber());
            BrainSetAutoLoaderManager* autoLoader = bs->getAutoLoaderManager();
            if (autoLoader->getAnyAutoLoaderSelected()) {
               selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE, false);
               if (selectedNode.getItemIndex1() >= 0) {
                  QString errorMessage =
                     autoLoader->processAutoLoading(selectedNode.getItemIndex1());
                  GuiFilesModified fm;
                  fm.setMetricModified();
                  fm.setVolumeModified();
                  theMainWindow->fileModificationUpdate(fm);
                  if (errorMessage.isEmpty() == false) {
                     QMessageBox::critical(this, "ERROR", errorMessage);
                  }
               }
               else {
                  selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_UNDERLAY, false);
                  if (selectedVoxelUnderlay.getItemIndex1() >= 0) {
                     int ijk[3] = {
                           selectedVoxelUnderlay.getItemIndex1(),
                           selectedVoxelUnderlay.getItemIndex2(),
                           selectedVoxelUnderlay.getItemIndex3()
                     };
                     float xyz[3];
                     BrainModelVolume* bmv = bs->getBrainModelVolume();
                     if (bmv != NULL) {
                        VolumeFile* vf = bmv->getUnderlayVolumeFile();
                        if (vf != NULL) {
                           vf->getVoxelCoordinate(ijk, xyz);
                           QString errorMessage =
                              autoLoader->processAutoLoading(xyz);
                           GuiFilesModified fm;
                           fm.setMetricModified();
                           theMainWindow->fileModificationUpdate(fm);
                           if (errorMessage.isEmpty() == false) {
                              QMessageBox::critical(this, "ERROR", errorMessage);
                           }
                        }
                     }
                  }
               }
               GuiFilesModified fm;
               fm.setMetricModified();
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);

            }
         }

         //
         // Perform an ID operation
         //
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_ALL, true);
         
         //
         // If transformation axis selected, switch to transform axes mouse mode
         //
         if (isMainWindowOpenGL()) {
            if (selectedTransformationAxes.getItemIndex1() >= 0) {
               setMouseMode(MOUSE_MODE_TRANSFORMATION_MATRIX_AXES);
               TransformationMatrixFile* tmf = theMainWindow->getBrainSet(getModelViewNumber())->getTransformationMatrixFile();
               const int indx = selectedTransformationAxes.getItemIndex1();
               if ((indx >= 0) && (indx < tmf->getNumberOfMatrices())) {
                  tmf->setSelectedTransformationAxesIndex(indx);
               }
            }
         }

         //
         // Update all GL so that green/blue ID  node symbols show up in all windows
         //   
         GuiBrainModelOpenGL::updateAllGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            vtkTransform* matrix = bmsv->getRotationTransformMatrix(viewingWindowIndex);
            
            switch(rotationAxis) {
               case BRAIN_MODEL_ROTATION_AXIS_X:
                  matrix->RotateX(biggestChange); //me.dy);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_Y:
                  matrix->RotateY(-biggestChange); //-me.dx);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_Z:
                  matrix->RotateZ(-biggestChange); //-me.dx);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_XY:
                  matrix->RotateX(me.dy);
                  matrix->RotateY(-me.dx);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_OFF:
                  break;
            }
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         {
            float translate[3];
            bmsv->getTranslation(viewingWindowIndex, translate);
            translate[0] += me.dx;
            translate[1] += me.dy;
            translate[2] =  0.0;
            bmsv->setTranslation(viewingWindowIndex, translate);
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         {
            float scale[3];
            bmsv->getScaling(viewingWindowIndex, scale);
            //scale += me.dy * scale * 0.01;
            scale[0] += biggestChange * scale[0] * 0.01;
            scale[1] += biggestChange * scale[1] * 0.01;
            scale[2] += biggestChange * scale[2] * 0.01;
            scale[0] = std::max(scale[0], 0.01f);
            scale[1] = std::max(scale[1], 0.01f);
            scale[2] = std::max(scale[2], 0.01f);
            //if (scale < 0.01) scale = 0.01;   // negative would flip surface
            bmsv->setScaling(viewingWindowIndex, scale);
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for surface view mode
 */
void  
GuiBrainModelOpenGL::mouseSurfaceView(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelSurface* bms = getDisplayedBrainModelSurface();
   BrainSet* bs = theMainWindow->getBrainSet(getModelViewNumber());
   DisplaySettingsSurface* dss = bs->getDisplaySettingsSurface();
   
   //
   // The Caret Main Window BrainModelSurface is used when yoking.  It is NULL
   // if the Caret Main Window contains a volume or contours.  When doing a yoked
   // surface, the transformations associated with the Caret Main Window are upated.
   //
   BrainModelSurface* mainWindowModelSurface = theMainWindow->getBrainModelSurface();
   int mainWindowModelViewNumber = -1;
   bool yokeSurfaceFlag = false;
   if (mainWindowModelSurface != NULL) {
      mainWindowModelViewNumber = 
         allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->getModelViewNumber();
      yokeSurfaceFlag = getYokeView();
   }
   
   //
   // See if yoke to oblique volume view in main window
   //
   bool yokeVolumeFlag = false;
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   vtkTransform* volumeObliqueRotationMatrix = NULL;
   if (bmv != NULL) {
      if (bmv->getSelectedAxis(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
         yokeVolumeFlag = getYokeView();
         volumeObliqueRotationMatrix = bmv->getObliqueRotationMatrix();
      }
   }
   
   //
   // Biggest change in mouse axis
   //
   const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;
            
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
       {
         //BrainSetAutoLoaderManager* autoLoader = bs->getAutoLoaderManager();
         //if (autoLoader->getAnyAutoLoaderSelected()) {
         //   selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE, false);
         //   if (selectedNode.getItemIndex1() >= 0) {
         //      QString errorMessage =
         //         autoLoader->processAutoLoading(selectedNode.getItemIndex1());
         //      GuiFilesModified fm;
         //      fm.setMetricModified();
         //      fm.setVolumeModified();
         //      theMainWindow->fileModificationUpdate(fm);
         //      if (errorMessage.isEmpty() == false) {
         //         QMessageBox::critical(this, "ERROR", errorMessage);
         //      }
         //   }
         //   GuiFilesModified fm;
         //   fm.setMetricModified();
         //   fm.setVolumeModified();
         //   theMainWindow->fileModificationUpdate(fm);
         //}

         //
         // Perform an ID operation
         //
         QTime timer;
         timer.start();
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_ALL, true);
         int total_time = timer.elapsed();
         std::cout << "id time:" << total_time << std::endl;
         BrainSetAutoLoaderManager* autoLoader = bs->getAutoLoaderManager();
         if (autoLoader->getAnyAutoLoaderSelected()) {
            if (selectedNode.getItemIndex1() >= 0) {
               QString errorMessage =
                  autoLoader->processAutoLoading(selectedNode.getItemIndex1());
               GuiFilesModified fm;
               fm.setMetricModified();
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);
               if (errorMessage.isEmpty() == false) {
                  QMessageBox::critical(this, "ERROR", errorMessage);
               }
            }
            GuiFilesModified fm;
            fm.setMetricModified();
            fm.setVolumeModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         
         //
         // Adjust transformation axes only in main window
         //
         if (selectedTransformationAxes.getItemIndex1() >= 0) {
            if (isMainWindowOpenGL()) {
               if (getMouseMode() != MOUSE_MODE_TRANSFORMATION_MATRIX_AXES) {
                  setMouseMode(MOUSE_MODE_TRANSFORMATION_MATRIX_AXES);
                  TransformationMatrixFile* tmf = theMainWindow->getBrainSet(getModelViewNumber())->getTransformationMatrixFile();
                  const int indx = selectedTransformationAxes.getItemIndex1();
                  if ((indx >= 0) && (indx < tmf->getNumberOfMatrices())) {
                     tmf->setSelectedTransformationAxesIndex(indx);
                  }
               }
               else {
                  setMouseMode(MOUSE_MODE_VIEW);
               }
            }
         }

         //
         // Update all GL so that green/blue ID  node symbols show up in all windows
         //   
         updateAllGL(NULL);
       }
       break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            vtkTransform* matrix = bms->getRotationTransformMatrix(viewingWindowIndex);
            
            //
            // If yoked, use main window brain surface model for rotation
            //
            if (yokeSurfaceFlag) {
               matrix = mainWindowModelSurface->getRotationTransformMatrix(mainWindowModelViewNumber);
            }
            
            //
            // If yoke to volume in oblique view in main window
            //
            if (yokeVolumeFlag) {
               matrix = volumeObliqueRotationMatrix;
            }
            
            switch(rotationAxis) {
               case BRAIN_MODEL_ROTATION_AXIS_X:
                  matrix->RotateX(biggestChange); //me.dy);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_Y:
                  matrix->RotateY(-biggestChange); //-me.dx);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_Z:
                  matrix->RotateZ(-biggestChange); //-me.dx);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_XY:
                  matrix->RotateX(me.dy);
                  matrix->RotateY(-me.dx);
                  break;
               case BRAIN_MODEL_ROTATION_AXIS_OFF:
                  break;
            }
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         {
            float translate[3];
            bms->getTranslation(viewingWindowIndex, translate);
            //
            // If yoked, use main window brain surface model for translation
            //
            if (yokeSurfaceFlag) {
               mainWindowModelSurface->getTranslation(mainWindowModelViewNumber, translate);
            }
            translate[0] += me.dx;
            translate[1] += me.dy;
            translate[2] =  0.0;
            //
            // If yoked, use main window brain surface model for translation
            //
            if (yokeSurfaceFlag) {
               mainWindowModelSurface->setTranslation(mainWindowModelViewNumber, translate);
            }
            else {
               bms->setTranslation(viewingWindowIndex, translate);
            }
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         switch (dss->getViewingProjection()) {
            case DisplaySettingsSurface::VIEWING_PROJECTION_ORTHOGRAPHIC:
               {
                  float scale[3];
                  bms->getScaling(viewingWindowIndex, scale);
                  //
                  // If yoked, use main window brain surface model for scaling
                  //
                  if (yokeSurfaceFlag) {
                     mainWindowModelSurface->getScaling(mainWindowModelViewNumber, scale);
                  }
                  
                  scale[0] += biggestChange * scale[0] * 0.01;
                  scale[1] += biggestChange * scale[1] * 0.01;
                  scale[2] += biggestChange * scale[2] * 0.01;
                  scale[0] = std::max(scale[0], 0.01f);
                  scale[1] = std::max(scale[1], 0.01f);
                  scale[2] = std::max(scale[2], 0.01f);
                  //if (scale < 0.01) scale = 0.01;   // negative would flip surface
                  //
                  // If yoked, use main window brain surface model for scaling
                  //
                  if (yokeSurfaceFlag) {
                     mainWindowModelSurface->setScaling(mainWindowModelViewNumber, scale);
                  }
                  else {
                     bms->setScaling(viewingWindowIndex, scale);
                  }
               }
               break;
            case DisplaySettingsSurface::VIEWING_PROJECTION_PERSPECTIVE:
               {
                  float zoom = bms->getPerspectiveZooming(viewingWindowIndex);
                  //
                  // If yoked, use main window brain surface model for scaling
                  //
                  if (yokeSurfaceFlag) {
                     zoom = mainWindowModelSurface->getPerspectiveZooming(mainWindowModelViewNumber);
                  }
                  zoom -= biggestChange * 1.0;
                  if (yokeSurfaceFlag) {
                     mainWindowModelSurface->setPerspectiveZooming(mainWindowModelViewNumber, zoom);
                  }
                  else {
                     bms->setPerspectiveZooming(viewingWindowIndex, zoom);
                  }
               }
               break;
         }
         if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
            theMainWindow->updateTransformationMatrixEditor(NULL);
         }
         updateAllGL(this);
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for border update NEW.
 */
void 
GuiBrainModelOpenGL::mouseBorderUpdateNew(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelSurface* bms = getDisplayedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   GuiBorderOperationsDialog* borderOperationsDialog = 
      theMainWindow->getBorderOperationsDialog(false);
   if (borderOperationsDialog == NULL) {
      return;
   }

   BrainModelBorderSet::UPDATE_BORDER_MODE borderUpdateMode =
      BrainModelBorderSet::UPDATE_BORDER_MODE_NONE;
   float sampling;
   bool twoDimFlag;
   bool autoProjectYesFlag;
   borderOperationsDialog->getBorderUpdateParameters(
      borderUpdateMode,
      sampling,
      twoDimFlag,
      autoProjectYesFlag);
       
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (linearObjectBeingDrawn.getNumberOfLinks() > 1) {
            const int numLinks = linearObjectBeingDrawn.getNumberOfLinks();
            if (numLinks > 1) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
               QString errorMessage;
               bmbs->updateBorder(bms,
                                  borderUpdateMode,
                                  &linearObjectBeingDrawn,
                                  sampling,
                                  autoProjectYesFlag,
                                  errorMessage);
               if (errorMessage.isEmpty() == false) {
                  QMessageBox::critical(this, "ERROR", errorMessage);
               }
            }
         }
         resetLinearObjectBeingDrawn();
         theMainWindow->getBrainSet()->assignBorderColors();
         updateAllGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            float modelXYZ[3];
            bool modelXYZValid = false;
            
            bool doThreeD = (twoDimFlag == false);
            if (doThreeD) {
               //
               // Interpolate point within a tile
               //
               int viewport[4] = { 0, 0, windowWidth[viewingWindowIndex], 
                                         windowHeight[viewingWindowIndex] };
               float tilePos[3];

               BrainSet* bs = bms->getBrainSet();
               if (bs == NULL) {
                  bs = theMainWindow->getBrainSet(getModelViewNumber());
               }

               if (openGL->getSurfacePointAtDisplayXY(bs,
                                                  bms,
                                                  viewingWindowIndex,
                                                  viewport,
                                                  me.x,
                                                  me.y,
                                                  tilePos)) {
                  if (DebugControl::getDebugOn()) {
                     std::cout << "Pos in tile: " 
                               << tilePos[0] << " "
                               << tilePos[1] << " "
                               << tilePos[2] << std::endl;
                  }
                  
                  modelXYZ[0] = tilePos[0];
                  modelXYZ[1] = tilePos[1];
                  modelXYZ[2] = tilePos[2];
                  modelXYZValid = true;
               }
            }
            else {
               //
               // Convert the window coordinate to a surface coordinate
               //
               convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
               if (DebugControl::getDebugOn()) {
                  std::cout << "Window: (" << me.x << ", " << me.y << ")"
                           << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
                           << modelXYZ[2] << ")" << std::endl;
               }
               modelXYZ[2] = 0.0;
               modelXYZValid = true;
            }
                     
            if (modelXYZValid) {
               linearObjectBeingDrawn.addBorderLink(modelXYZ, 0);
               if (linearObjectBeingDrawn.getNumberOfLinks() > 2) {
                  //int dummy = 0;
                  //linearObjectBeingDrawn.resampleBorderToDensity(dbd->getResampling(), 2, dummy);
               }
            }
            
            if (doThreeD) {
               drawLinearObjectOnly = false;
            }
            else {
               drawLinearObjectOnly = true;
            }
            updateGL();
            drawLinearObjectOnly = false;
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         {
            //
            // Allow rotation but only if drawing border in 3D mode
            //
            bool doThreeD = (twoDimFlag == false);
            if ((bms != NULL) &&
                doThreeD) {
               vtkTransform* matrix = bms->getRotationTransformMatrix(viewingWindowIndex);
            
               //
               // Biggest change in mouse axis
               //
               const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;

               switch(rotationAxis) {
                  case BRAIN_MODEL_ROTATION_AXIS_X:
                     matrix->RotateX(biggestChange); //me.dy);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Y:
                     matrix->RotateY(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Z:
                     matrix->RotateZ(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_XY:
                     matrix->RotateX(me.dy);
                     matrix->RotateY(-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_OFF:
                     break;
               }
               if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               updateAllGL(this);
            }
         }
         break;
   }
}

/**
 * mouse processing for border update
 */
void  
GuiBrainModelOpenGL::mouseBorderUpdate(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelSurface* bms = getDisplayedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   GuiBorderDrawUpdateDialog* borderDrawUpdateDialog = 
      theMainWindow->getDrawBorderUpdateDialog();
   if (borderDrawUpdateDialog == NULL) {
      return;
   }

   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (linearObjectBeingDrawn.getNumberOfLinks() > 1) {
            const int numLinks = linearObjectBeingDrawn.getNumberOfLinks();
            if (numLinks > 1) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
               QString errorMessage;
               bmbs->updateBorder(bms,
                                  borderDrawUpdateDialog->getBorderUpdateMode(),
                                  &linearObjectBeingDrawn,
                                  borderDrawUpdateDialog->getResampling(),
                                  borderDrawUpdateDialog->getAutoProjectBorder(),
                                  errorMessage);
               if (errorMessage.isEmpty() == false) {
                  QMessageBox::critical(this, "ERROR", errorMessage);
               }
            }
         }
         resetLinearObjectBeingDrawn();
         theMainWindow->getBrainSet()->assignBorderColors();
         updateAllGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            float modelXYZ[3];
            bool modelXYZValid = false;
            
            bool doThreeD = borderDrawUpdateDialog->getThreeDimensional();
            if (doThreeD) {
               //
               // Interpolate point within a tile
               //
               int viewport[4] = { 0, 0, windowWidth[viewingWindowIndex], 
                                         windowHeight[viewingWindowIndex] };
               float tilePos[3];

               BrainSet* bs = bms->getBrainSet();
               if (bs == NULL) {
                  bs = theMainWindow->getBrainSet(getModelViewNumber());
               }

               if (openGL->getSurfacePointAtDisplayXY(bs,
                                                  bms,
                                                  viewingWindowIndex,
                                                  viewport,
                                                  me.x,
                                                  me.y,
                                                  tilePos)) {
                  if (DebugControl::getDebugOn()) {
                     std::cout << "Pos in tile: " 
                               << tilePos[0] << " "
                               << tilePos[1] << " "
                               << tilePos[2] << std::endl;
                  }
                  
                  modelXYZ[0] = tilePos[0];
                  modelXYZ[1] = tilePos[1];
                  modelXYZ[2] = tilePos[2];
                  modelXYZValid = true;
               }
            }
            else {
               //
               // Convert the window coordinate to a surface coordinate
               //
               convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
               if (DebugControl::getDebugOn()) {
                  std::cout << "Window: (" << me.x << ", " << me.y << ")"
                           << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
                           << modelXYZ[2] << ")" << std::endl;
               }
               modelXYZ[2] = 0.0;
               modelXYZValid = true;
            }
                     
            if (modelXYZValid) {
               linearObjectBeingDrawn.addBorderLink(modelXYZ, 0);
               if (linearObjectBeingDrawn.getNumberOfLinks() > 2) {
                  //int dummy = 0;
                  //linearObjectBeingDrawn.resampleBorderToDensity(dbd->getResampling(), 2, dummy);
               }
            }
            
            if (doThreeD) {
               drawLinearObjectOnly = false;
            }
            else {
               drawLinearObjectOnly = true;
            }
            updateGL();
            drawLinearObjectOnly = false;
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         {
            //
            // Allow rotation but only if drawing border in 3D mode
            //
            bool doThreeD = borderDrawUpdateDialog->getThreeDimensional();
            if ((bms != NULL) &&
                doThreeD) {
               vtkTransform* matrix = bms->getRotationTransformMatrix(viewingWindowIndex);
            
               //
               // Biggest change in mouse axis
               //
               const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;

               switch(rotationAxis) {
                  case BRAIN_MODEL_ROTATION_AXIS_X:
                     matrix->RotateX(biggestChange); //me.dy);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Y:
                     matrix->RotateY(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Z:
                     matrix->RotateZ(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_XY:
                     matrix->RotateX(me.dy);
                     matrix->RotateY(-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_OFF:
                     break;
               }
               if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               updateAllGL(this);
            }
         }
         break;
   }
}

/**
 * mouse processing for border draw mode.
 */
void 
GuiBrainModelOpenGL::mouseBorderDrawNew(const GuiBrainModelOpenGLMouseEvent& me)
{
   GuiBorderOperationsDialog* borderOperationsDialog = 
      theMainWindow->getBorderOperationsDialog(false);
   if (borderOperationsDialog == NULL) {
      return;
   }

   VolumeFile* paintVolumeForVoxelAssignment = NULL;
   QString borderName;
   float borderSampling;
   int borderColorIndex;
   int surfacePaintAssignmentColumnNumber;
   int surfacePaintNameAssignmentIndex;
   int paintVolumeSliceThickness;
   bool closedBorderFlag;
   bool twoDimFlag;
   bool autoProjectYesFlag;
   borderOperationsDialog->getBorderDrawingParameters(borderName,
                                          borderSampling,
                                          borderColorIndex,
                                          surfacePaintAssignmentColumnNumber,
                                          surfacePaintNameAssignmentIndex,
                                          paintVolumeForVoxelAssignment,
                                          paintVolumeSliceThickness,
                                          closedBorderFlag,
                                          twoDimFlag,
                                          autoProjectYesFlag);
                                                      
   
   BrainModelSurface* bms = getDisplayedBrainModelSurface();
   BrainModelVolume* bmv = getDisplayedBrainModelVolume();
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (linearObjectBeingDrawn.getNumberOfLinks() > 1) {
            int dummy = 0;
            linearObjectBeingDrawn.resampleBorderToDensity(borderSampling, 2, 
                                                            dummy);
            const int numLinks = linearObjectBeingDrawn.getNumberOfLinks();
            if (numLinks > 1) {
               //
               // Is this a volume
               //
               if (bmv != NULL) {
                  const VolumeFile::VOLUME_AXIS volumeSliceAxis = bmv->getSelectedAxis(0);
                  const VolumeFile* vf = bmv->getMasterVolumeFile();
                  if (vf != NULL) {
                     float origin[3], spacing[3];
                     vf->getOrigin(origin);
                     vf->getSpacing(spacing);
                     int slices[3];
                     bmv->getSelectedOrthogonalSlices(0, slices);
                     float volumeSliceCoordinate = 0.0;
                     switch (volumeSliceAxis) {
                        case VolumeFile::VOLUME_AXIS_X:
                           volumeSliceCoordinate = slices[0] * spacing[0] + origin[0];
                           break;
                        case VolumeFile::VOLUME_AXIS_Y:
                           volumeSliceCoordinate = slices[1] * spacing[1] + origin[1];
                           break;
                        case VolumeFile::VOLUME_AXIS_Z:
                           volumeSliceCoordinate = slices[2] * spacing[2] + origin[2];
                           break;
                        case VolumeFile::VOLUME_AXIS_ALL:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                        case VolumeFile::VOLUME_AXIS_UNKNOWN:
                           break;
                     }
                     for (int i = 0; i < numLinks; i++) {
                        float xyzIn[3];
                        linearObjectBeingDrawn.getLinkXYZ(i, xyzIn);
                        float xyzOut[3];
                        switch (volumeSliceAxis) {
                           case VolumeFile::VOLUME_AXIS_X:
                              xyzOut[0] = volumeSliceCoordinate;
                              xyzOut[1] = xyzIn[0];
                              xyzOut[2] = xyzIn[1];
                              break;
                           case VolumeFile::VOLUME_AXIS_Y:
                              xyzOut[0] = xyzIn[0];
                              xyzOut[1] = volumeSliceCoordinate;
                              xyzOut[2] = xyzIn[1];
                              break;
                           case VolumeFile::VOLUME_AXIS_Z: 
                              xyzOut[0] = xyzIn[0];
                              xyzOut[1] = xyzIn[1];
                              xyzOut[2] = volumeSliceCoordinate;
                              break;
                           case VolumeFile::VOLUME_AXIS_ALL:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                           case VolumeFile::VOLUME_AXIS_UNKNOWN:
                              break;
                        }
                        linearObjectBeingDrawn.setLinkXYZ(i, xyzOut);
                     }
                  }
                  else {
                     QMessageBox::critical(this, "ERROR",
                         "At least one volume must be displayed as\n"
                         "an overlay or underlay to draw a border.");
                     resetLinearObjectBeingDrawn();
                     updateAllGL();
                     return;
                  }
               }
               
               Border border = linearObjectBeingDrawn;
               resetLinearObjectBeingDrawn();

               if (borderName.isEmpty()) {
                  borderName = "No-Name";
               }
               border.setName(borderName);
               border.setBorderColorIndex(borderColorIndex);
               
               float lastXYZ[3];
               if (closedBorderFlag) {
                  if (border.getNumberOfLinks() > 0) {
                     border.getLinkXYZ(0, lastXYZ);
                     border.addBorderLink(lastXYZ);
                  }
                  if (DebugControl::getDebugOn()) {
                     const int numB = border.getNumberOfLinks();
                     std::cout << "Border drawn has "
                               << numB
                               << " links."
                               << std::endl;
                     for (int i = 0; i < numB; i++) {
                        const float* xyz = border.getLinkXYZ(i);
                        std::cout << i << ": "
                                  << xyz[0] << ", "
                                  << xyz[1] << ", "
                                  << xyz[2] << std::endl;
                     }
                  }
               }
               int dummy = 0;
               border.resampleBorderToDensity(borderSampling, 2, dummy);
               
               
               if (bmv != NULL) {
                  BorderFile* bf = theMainWindow->getBrainSet()->getVolumeBorderFile();
                  bf->addBorder(border);
                  
                  //
                  // Assigning voxels within closed border
                  //
                  if (paintVolumeForVoxelAssignment != NULL) {
                     paintVolumeForVoxelAssignment->assignVoxelsWithinBorder(
                                                     bmv->getSelectedAxis(0),
                                                     borderName,
                                                     &border,
                                                     paintVolumeSliceThickness);
                                                     
                  }
               }
               else if (bms != NULL) {
                  BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
                  BrainModelBorder* b = new BrainModelBorder(theMainWindow->getBrainSet(), &border, bms->getSurfaceType());
                  bmbs->addBorder(b);
                  
                  if (autoProjectYesFlag) {
                     const int borderNumber = bmbs->getNumberOfBorders() - 1;
                     if (borderNumber >= 0) {
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                        bmbs->projectBorders(bms,
                                             true,
                                             borderNumber,
                                             borderNumber);
                        QApplication::restoreOverrideCursor();
                     }
                  }
                  
                  //
                  // If assigning nodes
                  //
                  if ((surfacePaintAssignmentColumnNumber >= 0) &&
                      (surfacePaintNameAssignmentIndex >= 0)) {
                     PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();

                     //
                     // Assign the nodes within the closed border.
                     //
                     const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
                     std::vector<bool> insideFlags(numNodes, false);
                     const CoordinateFile* cf = bms->getCoordinateFile();
                     const float* coords = cf->getCoordinate(0);

                     //
                     // Drawing a 3D border ?
                     //
                     if (twoDimFlag == false) {
                        BrainModelOpenGL* openGL = GuiBrainModelOpenGL::getOpenGLDrawing();
                        border.pointsInsideBorder3D(openGL->getSelectionModelviewMatrix(0),
                                                    openGL->getSelectionProjectionMatrix(0),
                                                    openGL->getSelectionViewport(0),
                                                    coords,
                                                    numNodes,
                                                    insideFlags);
                     }
                     else {
                        border.pointsInsideBorder2D(coords, numNodes, insideFlags);
                     }
                     
                     for (int j = 0; j < numNodes; j++) {
                        if (insideFlags[j]) {
                           pf->setPaint(j, 
                                        surfacePaintAssignmentColumnNumber, 
                                        surfacePaintNameAssignmentIndex);
                        }
                     }
                  }
               }

               //
               // Border File has changed
               //
               DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
               dsb->setDisplayBorders(true);
               GuiFilesModified fm;
               fm.setAreaColorModified();
               fm.setBorderColorModified();
               fm.setBorderModified();
               fm.setPaintModified();
               theMainWindow->fileModificationUpdate(fm);   
            }
         }
         
         resetLinearObjectBeingDrawn();
         updateAllGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         if (bmv != NULL) {
            //
            // Augment volume border being drawn with links from an existing border
            //
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOLUME_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Augment Border: "
                        << selectedVolumeBorder.getItemIndex1() << " "
                        << selectedVolumeBorder.getItemIndex2() << std::endl;
            }
            
            if (selectedVolumeBorder.getItemIndex1() >= 0) {
               if (linearObjectAugmentBorderCount == 0) {
                  linearObjectAugmentBorder1 = selectedVolumeBorder;
                  QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
                  linearObjectAugmentBorderCount++;
               }
               else {
                  linearObjectAugmentBorder2 = selectedVolumeBorder;
                  QApplication::restoreOverrideCursor();
                  
                  const int borderNum = linearObjectAugmentBorder1.getItemIndex1();
                  
                  if (borderNum != linearObjectAugmentBorder1.getItemIndex1()) {
                     QMessageBox::warning(this, "Border Selection Error",
                                          "You have tried to augment with two different "
                                          "borders!  Reselect both links.");
                  }
                  else {
                     const int linkStart = linearObjectAugmentBorder1.getItemIndex2();
                     const int linkEnd   = linearObjectAugmentBorder2.getItemIndex2();
                     BorderFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getVolumeBorderFile();
                     Border* b = bf->getBorder(borderNum);
                     const int totalLinks = b->getNumberOfLinks();
                     if (linkStart < linkEnd) {
                        for (int i = linkStart; i >= 0; i--) {
                           linearObjectBeingDrawn.addBorderLink(b->getLinkXYZ(i));
                        }
                        for (int i = totalLinks - 1; i >= linkEnd; i--) {
                           linearObjectBeingDrawn.addBorderLink(b->getLinkXYZ(i));
                        }
                     }
                     else {
                        for (int i = linkStart; i >= linkEnd; i--) {
                           linearObjectBeingDrawn.addBorderLink(b->getLinkXYZ(i));
                        }
                     }
                  }
                  linearObjectAugmentBorderCount = 0;
               }
            }
         }
         else {
            //
            // Augment surface border being drawn with links from an existing border
            //
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Augment Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               if (linearObjectAugmentBorderCount == 0) {
                  linearObjectAugmentBorder1 = selectedBorder1;
                  QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
                  linearObjectAugmentBorderCount++;
               }
               else {
                  linearObjectAugmentBorder2 = selectedBorder1;
                  QApplication::restoreOverrideCursor();
                  
                  const int borderNum = linearObjectAugmentBorder1.getItemIndex2();
                  
                  if (borderNum != linearObjectAugmentBorder1.getItemIndex2()) {
                     QMessageBox::warning(this, "Border Selection Error",
                                          "You have tried to augment with two different "
                                          "borders!  Reselect both links.");
                  }
                  else {
                     const int linkStart = linearObjectAugmentBorder1.getItemIndex3();
                     const int linkEnd   = linearObjectAugmentBorder2.getItemIndex3();
                     BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
                     BrainModelBorder* b = bmbs->getBorder(borderNum);
                     const int totalLinks = b->getNumberOfBorderLinks();
                     const int brainModelIndex = getDisplayedBrainModelIndex();
                     if (linkStart < linkEnd) {
                        for (int i = linkStart; i >= 0; i--) {
                           BrainModelBorderLink* link = b->getBorderLink(i);
                           linearObjectBeingDrawn.addBorderLink(link->getLinkPosition(brainModelIndex));
                        }
                        for (int i = totalLinks - 1; i >= linkEnd; i--) {
                           BrainModelBorderLink* link = b->getBorderLink(i);
                           linearObjectBeingDrawn.addBorderLink(link->getLinkPosition(brainModelIndex));
                        }
                     }
                     else {
                        for (int i = linkStart; i >= linkEnd; i--) {
                           BrainModelBorderLink* link = b->getBorderLink(i);
                           linearObjectBeingDrawn.addBorderLink(link->getLinkPosition(brainModelIndex));
                        }
                     }
                  }
                  linearObjectAugmentBorderCount = 0;
               }
            }
         }
         updateGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            float modelXYZ[3];
            bool modelXYZValid = false;
            
            bool doThreeD = (twoDimFlag == false);
            if (bmv != NULL) {
               doThreeD = false;
            }
            if (doThreeD) {
               //
               // Find the surface 3d position under the cursor
               //
               if (bms != NULL) {
                  //
                  // Interpolate point within a tile
                  //
                  int viewport[4] = { 0, 0, windowWidth[viewingWindowIndex], 
                                            windowHeight[viewingWindowIndex] };
                  float tilePos[3];

                  BrainSet* bs = NULL;
                  if (bms != NULL) {
                     bs = bms->getBrainSet();
                  }
                  if (bs == NULL) {
                     bs = theMainWindow->getBrainSet(getModelViewNumber());
                  }

                  if (openGL->getSurfacePointAtDisplayXY(bs,
                                                     bms,
                                                     viewingWindowIndex,
                                                     viewport,
                                                     me.x,
                                                     me.y,
                                                     tilePos)) {
                     if (DebugControl::getDebugOn()) {
                        std::cout << "Pos in tile: " 
                                  << tilePos[0] << " "
                                  << tilePos[1] << " "
                                  << tilePos[2] << std::endl;
                     }
                     
                     modelXYZ[0] = tilePos[0];
                     modelXYZ[1] = tilePos[1];
                     modelXYZ[2] = tilePos[2];
                     modelXYZValid = true;
                  }
               }
            }
            else {
               //
               // Convert the window coordinate to a surface coordinate
               //
               convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
               if (DebugControl::getDebugOn()) {
                  std::cout << "Window: (" << me.x << ", " << me.y << ")"
                           << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
                           << modelXYZ[2] << ")" << std::endl;
               }
               modelXYZ[2] = 0.0;
               modelXYZValid = true;
            }
                     
            if (modelXYZValid) {
               linearObjectBeingDrawn.addBorderLink(modelXYZ, 0);
               if (linearObjectBeingDrawn.getNumberOfLinks() > 2) {
                  //int dummy = 0;
                  //linearObjectBeingDrawn.resampleBorderToDensity(dbd->getResampling(), 2, dummy);
               }
            }
            
            if (doThreeD) {
               drawLinearObjectOnly = false;
            }
            else {
               drawLinearObjectOnly = true;
            }
            updateGL();
            drawLinearObjectOnly = false;
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         {
            bool doThreeD = (twoDimFlag == false);
            if ((bms != NULL) &&
                doThreeD) {
               vtkTransform* matrix = bms->getRotationTransformMatrix(viewingWindowIndex);
            
               //
               // Biggest change in mouse axis
               //
               const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;

               switch(rotationAxis) {
                  case BRAIN_MODEL_ROTATION_AXIS_X:
                     matrix->RotateX(biggestChange); //me.dy);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Y:
                     matrix->RotateY(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Z:
                     matrix->RotateZ(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_XY:
                     matrix->RotateX(me.dy);
                     matrix->RotateY(-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_OFF:
                     break;
               }
               if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               updateAllGL(this);
            }
         }
         break;
   }
}
       
/**
 * mouse processing for border draw mode
 */
void  
GuiBrainModelOpenGL::mouseBorderDraw(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelVolume* bmv = getDisplayedBrainModelVolume();
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (linearObjectBeingDrawn.getNumberOfLinks() > 1) {
            int dummy = 0;
            GuiDrawBorderDialog* dbd = theMainWindow->getDrawBorderDialog(false);
            linearObjectBeingDrawn.resampleBorderToDensity(dbd->getResampling(), 2, 
                                                            dummy);
            const int numLinks = linearObjectBeingDrawn.getNumberOfLinks();
            if (numLinks > 1) {
               //
               // Is this a volume
               //
               if (bmv != NULL) {
                  const VolumeFile::VOLUME_AXIS volumeSliceAxis = bmv->getSelectedAxis(0);
                  const VolumeFile* vf = bmv->getMasterVolumeFile();
                  if (vf != NULL) {
                     float origin[3], spacing[3];
                     vf->getOrigin(origin);
                     vf->getSpacing(spacing);
                     int slices[3];
                     bmv->getSelectedOrthogonalSlices(0, slices);
                     float volumeSliceCoordinate = 0.0;
                     switch (volumeSliceAxis) {
                        case VolumeFile::VOLUME_AXIS_X:
                           volumeSliceCoordinate = slices[0] * spacing[0] + origin[0];
                           break;
                        case VolumeFile::VOLUME_AXIS_Y:
                           volumeSliceCoordinate = slices[1] * spacing[1] + origin[1];
                           break;
                        case VolumeFile::VOLUME_AXIS_Z:
                           volumeSliceCoordinate = slices[2] * spacing[2] + origin[2];
                           break;
                        case VolumeFile::VOLUME_AXIS_ALL:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                        case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                        case VolumeFile::VOLUME_AXIS_UNKNOWN:
                           break;
                     }
                     for (int i = 0; i < numLinks; i++) {
                        float xyzIn[3];
                        linearObjectBeingDrawn.getLinkXYZ(i, xyzIn);
                        float xyzOut[3];
                        switch (volumeSliceAxis) {
                           case VolumeFile::VOLUME_AXIS_X:
                              xyzOut[0] = volumeSliceCoordinate;
                              xyzOut[1] = xyzIn[0];
                              xyzOut[2] = xyzIn[1];
                              break;
                           case VolumeFile::VOLUME_AXIS_Y:
                              xyzOut[0] = xyzIn[0];
                              xyzOut[1] = volumeSliceCoordinate;
                              xyzOut[2] = xyzIn[1];
                              break;
                           case VolumeFile::VOLUME_AXIS_Z: 
                              xyzOut[0] = xyzIn[0];
                              xyzOut[1] = xyzIn[1];
                              xyzOut[2] = volumeSliceCoordinate;
                              break;
                           case VolumeFile::VOLUME_AXIS_ALL:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                           case VolumeFile::VOLUME_AXIS_UNKNOWN:
                              break;
                        }
                        linearObjectBeingDrawn.setLinkXYZ(i, xyzOut);
                     }
                  }
                  else {
                     QMessageBox::critical(this, "ERROR",
                         "At least one volume must be displayed as\n"
                         "an overlay or underlay to draw a border.");
                     resetLinearObjectBeingDrawn();
                     updateGL();
                     return;
                  }
               }
               dbd->createNewBorder(getDisplayedBrainModel(), linearObjectBeingDrawn);
               resetLinearObjectBeingDrawn();
               updateGL();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         if (bmv != NULL) {
            //
            // Augment volume border being drawn with links from an existing border
            //
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOLUME_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Augment Border: "
                        << selectedVolumeBorder.getItemIndex1() << " "
                        << selectedVolumeBorder.getItemIndex2() << std::endl;
            }
            
            if (selectedVolumeBorder.getItemIndex1() >= 0) {
               if (linearObjectAugmentBorderCount == 0) {
                  linearObjectAugmentBorder1 = selectedVolumeBorder;
                  QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
                  linearObjectAugmentBorderCount++;
               }
               else {
                  linearObjectAugmentBorder2 = selectedVolumeBorder;
                  QApplication::restoreOverrideCursor();
                  
                  const int borderNum = linearObjectAugmentBorder1.getItemIndex1();
                  
                  if (borderNum != linearObjectAugmentBorder1.getItemIndex1()) {
                     QMessageBox::warning(this, "Border Selection Error",
                                          "You have tried to augment with two different "
                                          "borders!  Reselect both links.");
                  }
                  else {
                     const int linkStart = linearObjectAugmentBorder1.getItemIndex2();
                     const int linkEnd   = linearObjectAugmentBorder2.getItemIndex2();
                     BorderFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getVolumeBorderFile();
                     Border* b = bf->getBorder(borderNum);
                     const int totalLinks = b->getNumberOfLinks();
                     if (linkStart < linkEnd) {
                        for (int i = linkStart; i >= 0; i--) {
                           linearObjectBeingDrawn.addBorderLink(b->getLinkXYZ(i));
                        }
                        for (int i = totalLinks - 1; i >= linkEnd; i--) {
                           linearObjectBeingDrawn.addBorderLink(b->getLinkXYZ(i));
                        }
                     }
                     else {
                        for (int i = linkStart; i >= linkEnd; i--) {
                           linearObjectBeingDrawn.addBorderLink(b->getLinkXYZ(i));
                        }
                     }
                  }
                  linearObjectAugmentBorderCount = 0;
               }
            }
         }
         else {
            //
            // Augment surface border being drawn with links from an existing border
            //
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Augment Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               if (linearObjectAugmentBorderCount == 0) {
                  linearObjectAugmentBorder1 = selectedBorder1;
                  QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
                  linearObjectAugmentBorderCount++;
               }
               else {
                  linearObjectAugmentBorder2 = selectedBorder1;
                  QApplication::restoreOverrideCursor();
                  
                  const int borderNum = linearObjectAugmentBorder1.getItemIndex2();
                  
                  if (borderNum != linearObjectAugmentBorder1.getItemIndex2()) {
                     QMessageBox::warning(this, "Border Selection Error",
                                          "You have tried to augment with two different "
                                          "borders!  Reselect both links.");
                  }
                  else {
                     const int linkStart = linearObjectAugmentBorder1.getItemIndex3();
                     const int linkEnd   = linearObjectAugmentBorder2.getItemIndex3();
                     BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
                     BrainModelBorder* b = bmbs->getBorder(borderNum);
                     const int totalLinks = b->getNumberOfBorderLinks();
                     const int brainModelIndex = getDisplayedBrainModelIndex();
                     if (linkStart < linkEnd) {
                        for (int i = linkStart; i >= 0; i--) {
                           BrainModelBorderLink* link = b->getBorderLink(i);
                           linearObjectBeingDrawn.addBorderLink(link->getLinkPosition(brainModelIndex));
                        }
                        for (int i = totalLinks - 1; i >= linkEnd; i--) {
                           BrainModelBorderLink* link = b->getBorderLink(i);
                           linearObjectBeingDrawn.addBorderLink(link->getLinkPosition(brainModelIndex));
                        }
                     }
                     else {
                        for (int i = linkStart; i >= linkEnd; i--) {
                           BrainModelBorderLink* link = b->getBorderLink(i);
                           linearObjectBeingDrawn.addBorderLink(link->getLinkPosition(brainModelIndex));
                        }
                     }
                     //int dummy = 0;
                     //GuiDrawBorderDialog* dbd = theMainWindow->getDrawBorderDialog(false);
                     //linearObjectBeingDrawn.resampleBorderToDensity(dbd->getResampling(), 2, 
                     //                                                dummy);
                  }
                  linearObjectAugmentBorderCount = 0;
               }
            }
         }
         updateGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            GuiDrawBorderDialog* dbd = theMainWindow->getDrawBorderDialog(false);
            float modelXYZ[3];
            bool modelXYZValid = false;
            
            bool doThreeD = dbd->getThreeDimensional();
            if (bmv != NULL) {
               doThreeD = false;
            }
            if (doThreeD) {
               //
               // Find the surface 3d position under the cursor
               //
               BrainModelSurface* bms = getDisplayedBrainModelSurface();
               if (bms != NULL) {
/*
                  selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
                  const int nodeNum = selectedNode.getItemIndex1();
                  if (nodeNum >= 0) {
                     const CoordinateFile* cf = bms->getCoordinateFile();
                     cf->getCoordinate(nodeNum, modelXYZ);
                     modelXYZValid = true;
                     if (DebugControl::getDebugOn()) {
                        std::cout << "Node: " << nodeNum
                                 << "XYZ: " << modelXYZ[0] << " "
                                 << modelXYZ[1] << " "
                                 << modelXYZ[2] << std::endl;
                     }
                  }
*/                  
                  //
                  // Interpolate point within a tile
                  //
                  int viewport[4] = { 0, 0, windowWidth[viewingWindowIndex], 
                                            windowHeight[viewingWindowIndex] };
                  float tilePos[3];

                  BrainSet* bs = NULL;
                  if (bms != NULL) {
                     bs = bms->getBrainSet();
                  }
                  if (bs == NULL) {
                     bs = theMainWindow->getBrainSet(getModelViewNumber());
                  }

                  if (openGL->getSurfacePointAtDisplayXY(bs,
                                                     bms,
                                                     viewingWindowIndex,
                                                     viewport,
                                                     me.x,
                                                     me.y,
                                                     tilePos)) {
                     if (DebugControl::getDebugOn()) {
                        std::cout << "Pos in tile: " 
                                  << tilePos[0] << " "
                                  << tilePos[1] << " "
                                  << tilePos[2] << std::endl;
                     }
                     
                     modelXYZ[0] = tilePos[0];
                     modelXYZ[1] = tilePos[1];
                     modelXYZ[2] = tilePos[2];
                     modelXYZValid = true;
                  }
               }
            }
            else {
               //
               // Convert the window coordinate to a surface coordinate
               //
               convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
               if (DebugControl::getDebugOn()) {
                  std::cout << "Window: (" << me.x << ", " << me.y << ")"
                           << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
                           << modelXYZ[2] << ")" << std::endl;
               }
               modelXYZ[2] = 0.0;
               modelXYZValid = true;
            }
                     
            if (modelXYZValid) {
               linearObjectBeingDrawn.addBorderLink(modelXYZ, 0);
               if (linearObjectBeingDrawn.getNumberOfLinks() > 2) {
                  //int dummy = 0;
                  //linearObjectBeingDrawn.resampleBorderToDensity(dbd->getResampling(), 2, dummy);
               }
            }
            
            if (doThreeD) {
               drawLinearObjectOnly = false;
            }
            else {
               drawLinearObjectOnly = true;
            }
            updateGL();
            drawLinearObjectOnly = false;
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         {
            //
            // Allow rotation but only if drawing border in 3D mode
            //
            GuiDrawBorderDialog* dbd = theMainWindow->getDrawBorderDialog(false);
            BrainModelSurface* bms = getDisplayedBrainModelSurface();
            bool doThreeD = dbd->getThreeDimensional();
            if ((bms != NULL) &&
                doThreeD) {
               vtkTransform* matrix = bms->getRotationTransformMatrix(viewingWindowIndex);
            
               //
               // Biggest change in mouse axis
               //
               const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;

               switch(rotationAxis) {
                  case BRAIN_MODEL_ROTATION_AXIS_X:
                     matrix->RotateX(biggestChange); //me.dy);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Y:
                     matrix->RotateY(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_Z:
                     matrix->RotateZ(-biggestChange); //-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_XY:
                     matrix->RotateX(me.dy);
                     matrix->RotateY(-me.dx);
                     break;
                  case BRAIN_MODEL_ROTATION_AXIS_OFF:
                     break;
               }
               if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
                  theMainWindow->updateTransformationMatrixEditor(NULL);
               }
               updateAllGL(this);
            }
         }
         break;
   }
}

/**
 * mouse processing for border delete mode
 */
void  
GuiBrainModelOpenGL::mouseBorderDelete(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
               bmbs->deleteBorder(selectedBorder1.getItemIndex2());
            }
         }
         else if (getDisplayedBrainModelVolume() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOLUME_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedVolumeBorder.getItemIndex1() << " "
                        << selectedVolumeBorder.getItemIndex2() << std::endl;
            }
            
            if (selectedVolumeBorder.getItemIndex1() >= 0) {
               BorderFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getVolumeBorderFile();
               bf->removeBorder(selectedVolumeBorder.getItemIndex1());
            }
         }
         updateAllGL(NULL);
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for border delete point mode
 */
void  
GuiBrainModelOpenGL::mouseBorderDeletePoint(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
               BrainModelBorder* b = bmbs->getBorder(selectedBorder1.getItemIndex2());
               b->deleteBorderLink(selectedBorder1.getItemIndex3());
            }
         }
         else if (getDisplayedBrainModelVolume() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOLUME_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedVolumeBorder.getItemIndex1() << " "
                        << selectedVolumeBorder.getItemIndex2() << std::endl;
            }
            
            if (selectedVolumeBorder.getItemIndex1() >= 0) {
               BorderFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getVolumeBorderFile();
               Border* b = bf->getBorder(selectedVolumeBorder.getItemIndex1());
               b->removeLink(selectedVolumeBorder.getItemIndex2());
            }
         }
         updateAllGL(NULL);
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for border interpolate.
 */
void 
GuiBrainModelOpenGL::mouseBorderInterpolate(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               GuiBordersCreateInterpolatedDialog* cid = theMainWindow->getBordersCreateInterpolatedDialog(false);
               cid->updateBorderSelection(GuiBordersCreateInterpolatedDialog::INTERPOLATE_BORDER_1,
                                          selectedBorder1.getItemIndex2());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               GuiBordersCreateInterpolatedDialog* cid = theMainWindow->getBordersCreateInterpolatedDialog(false);
               cid->updateBorderSelection(GuiBordersCreateInterpolatedDialog::INTERPOLATE_BORDER_2,
                                          selectedBorder1.getItemIndex2());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}
       
/**
 * mouse processing for border interpolate NEW.
 */
void 
GuiBrainModelOpenGL::mouseBorderInterpolateNew(const GuiBrainModelOpenGLMouseEvent& me)
{
   GuiBorderOperationsDialog* borderOperationsDialog = 
      theMainWindow->getBorderOperationsDialog(false);
   if (borderOperationsDialog == NULL) {
      return;
   }

   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               borderOperationsDialog->setSelectedInterpolatedBorders(
                                          0,
                                          selectedBorder1.getItemIndex2());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (DebugControl::getDebugOn()) {
               std::cout << "Selected Border: "
                        << selectedBorder1.getItemIndex1() << " "
                        << selectedBorder1.getItemIndex2() << " "
                        << selectedBorder1.getItemIndex3() << std::endl;
            }
            
            if (selectedBorder1.getItemIndex1() >= 0) {
               borderOperationsDialog->setSelectedInterpolatedBorders(
                                          1,
                                          selectedBorder1.getItemIndex2());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}
       
/**
 * mouse processing for border move point
 */
void  
GuiBrainModelOpenGL::mouseBorderMovePoint(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         borderPointBeingMoved[0] = -1;
         QApplication::restoreOverrideCursor(); 
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (selectedBorder1.getItemIndex1() >= 0) {
               borderPointBeingMoved[0] = selectedBorder1.getItemIndex1();
               borderPointBeingMoved[1] = selectedBorder1.getItemIndex2();
               borderPointBeingMoved[2] = selectedBorder1.getItemIndex3();
               QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
            }
         }
         else if (getDisplayedBrainModelVolume() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOLUME_BORDER);
            if (selectedVolumeBorder.getItemIndex1() >= 0) {
               borderPointBeingMoved[0] = selectedVolumeBorder.getItemIndex1();
               borderPointBeingMoved[1] = selectedVolumeBorder.getItemIndex2();
               borderPointBeingMoved[2] = selectedVolumeBorder.getItemIndex3();
               QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         QApplication::restoreOverrideCursor(); 
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         if (borderPointBeingMoved[0] >= 0) {
            if (getDisplayedBrainModelSurface() != NULL) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
               BrainModelBorder* b = bmbs->getBorder(borderPointBeingMoved[1]);
               if (b != NULL) {
                  BrainModelBorderLink* link = b->getBorderLink(borderPointBeingMoved[2]);
                  if (link != NULL) {
                     float pos[3];
                     link->getLinkPosition(borderPointBeingMoved[0], pos);
                     float dummy;
                     convertWindowToModelCoords(me.x, me.y, false, pos[0], pos[1], dummy);
                     link->setLinkPosition(borderPointBeingMoved[0], pos);
                     updateGL();
                  }
               }
            }
            else if (getDisplayedBrainModelVolume() != NULL) {
               BorderFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getVolumeBorderFile();
               Border* b = bf->getBorder(borderPointBeingMoved[0]);
               BrainModelVolume* bmv = getDisplayedBrainModelVolume();
               const VolumeFile::VOLUME_AXIS volumeSliceAxis = bmv->getSelectedAxis(0);
               const VolumeFile* vf = bmv->getMasterVolumeFile();
               float origin[3], spacing[3];
               vf->getOrigin(origin);
               vf->getSpacing(spacing);
               int slices[3];
               bmv->getSelectedOrthogonalSlices(0, slices);
               float volumeSliceCoordinate = 0.0;
               switch (volumeSliceAxis) {
                  case VolumeFile::VOLUME_AXIS_X:
                     volumeSliceCoordinate = slices[0] * spacing[0] + origin[0];
                     break;
                  case VolumeFile::VOLUME_AXIS_Y:
                     volumeSliceCoordinate = slices[1] * spacing[1] + origin[1];
                     break;
                  case VolumeFile::VOLUME_AXIS_Z:
                     volumeSliceCoordinate = slices[2] * spacing[2] + origin[2];
                     break;
                  case VolumeFile::VOLUME_AXIS_ALL:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                  case VolumeFile::VOLUME_AXIS_UNKNOWN:
                     break;
               }
               if (b != NULL) {
                  float xyz[3];
                  b->getLinkXYZ(borderPointBeingMoved[1], xyz);
                  float dummy;
                  float newX, newY;
                  convertWindowToModelCoords(me.x, me.y, false, newX, newY, dummy);
                  switch (volumeSliceAxis) {
                     case VolumeFile::VOLUME_AXIS_X:
                        xyz[0] = volumeSliceCoordinate;
                        xyz[1] = newX;
                        xyz[2] = newY;
                        break;
                     case VolumeFile::VOLUME_AXIS_Y:
                        xyz[0] = newX;
                        xyz[1] = volumeSliceCoordinate;
                        xyz[2] = newY;
                        break;
                     case VolumeFile::VOLUME_AXIS_Z: 
                        xyz[0] = newX;
                        xyz[1] = newY;
                        xyz[2] = volumeSliceCoordinate;
                        break;
                     case VolumeFile::VOLUME_AXIS_ALL:
                     case VolumeFile::VOLUME_AXIS_OBLIQUE:
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                     case VolumeFile::VOLUME_AXIS_UNKNOWN:
                        break;
                  }
                  b->setLinkXYZ(borderPointBeingMoved[1], xyz);
                  updateGL();
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for border reverse
 */
void  
GuiBrainModelOpenGL::mouseBorderReverse(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (selectedBorder1.getItemIndex1() >= 0) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
               BrainModelBorder* b = bmbs->getBorder(selectedBorder1.getItemIndex2());
               b->reverseLinks();
               updateAllGL(NULL);
            }
         }
         else if (getDisplayedBrainModelVolume() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOLUME_BORDER);
            if (selectedVolumeBorder.getItemIndex1() >= 0) {
               BorderFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getVolumeBorderFile();
               Border* b = bf->getBorder(selectedVolumeBorder.getItemIndex1());
               b->reverseBorderLinks();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for border rename
 */
void  
GuiBrainModelOpenGL::mouseBorderRename(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         if (getDisplayedBrainModelSurface() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
            if (selectedBorder1.getItemIndex1() >= 0) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
               BrainModelBorder* b = bmbs->getBorder(selectedBorder1.getItemIndex2());
               bool ok = false;
               QString newName = QInputDialog::getText(this,
                                                       "Rename Border",
                                                       "Border Name",
                                                       QLineEdit::Normal,
                                                       b->getName(),
                                                       &ok);
               if (ok && (newName.isEmpty() == false)) {
                  b->setName(newName);
                  theMainWindow->getBrainSet(getModelViewNumber())->assignBorderColors();
               }
            }
         }
         else if (getDisplayedBrainModelVolume() != NULL) {
            selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOLUME_BORDER);
            if (selectedVolumeBorder.getItemIndex1() >= 0) {
               BorderFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getVolumeBorderFile();
               Border* b = bf->getBorder(selectedVolumeBorder.getItemIndex1());
               bool ok = false;
               QString newName = QInputDialog::getText(this,
                                                       "Rename Border",
                                                       "Border Name",
                                                       QLineEdit::Normal,
                                                       b->getName(),
                                                       &ok);
               if (ok && (newName.isEmpty() == false)) {
                  b->setName(newName);
                  theMainWindow->getBrainSet(getModelViewNumber())->assignBorderColors();
               }
            }
         }
         updateAllGL(NULL);
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for draw cut mode
 */
void  
GuiBrainModelOpenGL::mouseCutDraw(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (linearObjectBeingDrawn.getNumberOfLinks() > 1) {
            int dummy = 0;
            linearObjectBeingDrawn.resampleBorderToDensity(5.0, 2, dummy);
            if (linearObjectBeingDrawn.getNumberOfLinks() > 1) {
               BrainModel* bm = getDisplayedBrainModel();
               if (bm->getModelType() == BrainModel::BRAIN_MODEL_SURFACE) {
                  CutsFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getCutsFile();
                  bf->addBorder(linearObjectBeingDrawn);
               }
               GuiFilesModified fm;
               fm.setCutModified();
               theMainWindow->fileModificationUpdate(fm);
               resetLinearObjectBeingDrawn();
               updateGL();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            //
            // Convert the window coordinate to a surface coordinate
            //
            float modelXYZ[3];
            convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
            if (DebugControl::getDebugOn()) {
               std::cout << "Window: (" << me.x << ", " << me.y << ")"
                        << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
                        << modelXYZ[2] << ")" << std::endl;
            }
            modelXYZ[2] = 0.0;
                  
            linearObjectBeingDrawn.addBorderLink(modelXYZ, 0);
            //if (linearObjectBeingDrawn.getNumberOfLinks() > 2) {
            //   int dummy = 0;
            //   linearObjectBeingDrawn.resampleBorderToDensity(5.0, 2, dummy);
            //}
            
            drawLinearObjectOnly = true;
            updateGL();
            drawLinearObjectOnly = false;
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for delete cut mode
 */
void  
GuiBrainModelOpenGL::mouseCutDelete(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CUT);
         if (DebugControl::getDebugOn()) {
            std::cout << "Selected Cut: "
                     << selectedCut.getItemIndex1() << " "
                     << selectedCut.getItemIndex2() <<std::endl;
         }
         
         if (selectedCut.getItemIndex1() >= 0) {
            CutsFile* bf = theMainWindow->getBrainSet(getModelViewNumber())->getCutsFile();
            bf->removeBorder(selectedCut.getItemIndex1());
            GuiFilesModified fm;
            fm.setCutModified();
            theMainWindow->fileModificationUpdate(fm);
            updateGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for delete foci
 */
void  
GuiBrainModelOpenGL::mouseFociDelete(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_FOCUS_PROJECTION);
         if (DebugControl::getDebugOn()) {
            std::cout << "Selected Foci: "
                     << selectedFocusProjection.getItemIndex1() << std::endl;
         }
         
         if (selectedFocusProjection.getItemIndex1() >= 0) {
            theMainWindow->getBrainSet(getModelViewNumber())->deleteFocus(selectedFocusProjection.getItemIndex1());
            updateAllGL(NULL);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for delete cell
 */
void  
GuiBrainModelOpenGL::mouseCellDelete(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CELL_PROJECTION);
         if (DebugControl::getDebugOn()) {
            std::cout << "Selected Cell: "
                     << selectedCellProjection.getItemIndex1() << std::endl;
         }
         
         if (selectedCellProjection.getItemIndex1() >= 0) {
            theMainWindow->getBrainSet(getModelViewNumber())->deleteCell(selectedCellProjection.getItemIndex1());
            updateAllGL(NULL);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for roi border selection
 */
void  
GuiBrainModelOpenGL::mouseSurfaceRoiBorderSelect(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_BORDER);
         if (selectedBorder1.getItemIndex1() >= 0) {
            GuiSurfaceRegionOfInterestDialog* roi = theMainWindow->getSurfaceRegionOfInterestDialog(false);
            if (roi != NULL) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
               BrainModelBorder* b = bmbs->getBorder(selectedBorder1.getItemIndex2());
               roi->setBorderNameForQuery(b->getName());
            }
            GuiSurfaceRegionOfInterestDialogOLD* roiOLD = theMainWindow->getSurfaceRegionOfInterestDialogOLD(false);
            if (roiOLD != NULL) {
               BrainModelBorderSet* bmbs = theMainWindow->getBrainSet(getModelViewNumber())->getBorderSet();
               BrainModelBorder* b = bmbs->getBorder(selectedBorder1.getItemIndex2());
               roiOLD->setBorderNameForQuery(b->getName());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for roi paint selection
 */
void  
GuiBrainModelOpenGL::mouseSurfaceRoiPaintSelect(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiSurfaceRegionOfInterestDialog* roi = theMainWindow->getSurfaceRegionOfInterestDialog(false);
            if (roi != NULL) {               
               PaintFile* pf = theMainWindow->getBrainSet(getModelViewNumber())->getPaintFile();
               const int index = roi->getQueryPaintColumn();
               if ((index >= 0) && (index < pf->getNumberOfColumns())) {
                  roi->setPaintIndexForQuery(pf->getPaint(selectedNode.getItemIndex1(), index));
               }
            }
            GuiSurfaceRegionOfInterestDialogOLD* roiOLD = theMainWindow->getSurfaceRegionOfInterestDialogOLD(false);
            if (roiOLD != NULL) {               
               PaintFile* pf = theMainWindow->getBrainSet(getModelViewNumber())->getPaintFile();
               const int index = roiOLD->getQueryPaintColumn();
               if ((index >= 0) && (index < pf->getNumberOfColumns())) {
                  roiOLD->setPaintIndexForQuery(pf->getPaint(selectedNode.getItemIndex1(), index));
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for roi metric selection
 */
void  
GuiBrainModelOpenGL::mouseSurfaceRoiMetricSelect(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiSurfaceRegionOfInterestDialog* roi = theMainWindow->getSurfaceRegionOfInterestDialog(false);
            if (roi != NULL) {               
               roi->setMetricNodeForQuery(selectedNode.getItemIndex1());
            }
            GuiSurfaceRegionOfInterestDialogOLD* roiOLD = theMainWindow->getSurfaceRegionOfInterestDialogOLD(false);
            if (roiOLD != NULL) {               
               roiOLD->setMetricNodeForQuery(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for roi shape selection
 */
void  
GuiBrainModelOpenGL::mouseSurfaceRoiShapeSelect(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiSurfaceRegionOfInterestDialog* roi = theMainWindow->getSurfaceRegionOfInterestDialog(false);
            if (roi != NULL) {               
               roi->setShapeNodeForQuery(selectedNode.getItemIndex1());
            }
            GuiSurfaceRegionOfInterestDialogOLD* roiOLD = theMainWindow->getSurfaceRegionOfInterestDialogOLD(false);
            if (roiOLD != NULL) {               
               roiOLD->setShapeNodeForQuery(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for roi geodesic node selection
 */
void  
GuiBrainModelOpenGL::mouseSurfaceRoiGeodesicNodeSelect(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiSurfaceRegionOfInterestDialog* roi = theMainWindow->getSurfaceRegionOfInterestDialog(false);
            if (roi != NULL) {               
               roi->setNodeForGeodesicQuery(selectedNode.getItemIndex1());
            }
            GuiSurfaceRegionOfInterestDialogOLD* roiOLD = theMainWindow->getSurfaceRegionOfInterestDialogOLD(false);
            if (roiOLD != NULL) {               
               roiOLD->setNodeForGeodesicQuery(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for align surface to standard orientation
 */
void 
GuiBrainModelOpenGL::mouseAlignStandardOrientation(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiAlignSurfaceToStandardOrientationDialog* assod = 
               theMainWindow->getAlignSurfaceToStandardOrientationDialog();
            
            if (assod != NULL) {                
                  assod->setVentralTipNode(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiAlignSurfaceToStandardOrientationDialog* assod = 
               theMainWindow->getAlignSurfaceToStandardOrientationDialog();
            
            if (assod != NULL) {                
               assod->setMedialTipNode(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}
       
/**
 * mouse processing for align surface to standard orientation during flatten full hem
 */
void 
GuiBrainModelOpenGL::mouseAlignStandardOrientationFullHemFlatten(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiFlattenFullHemisphereDialog* flattenFullHemDialog =
               theMainWindow->getFlattenFullHemisphereDialog(false);
            if (flattenFullHemDialog != NULL) {                
                  flattenFullHemDialog->setCentralSulcusVentralTip(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiFlattenFullHemisphereDialog* flattenFullHemDialog =
               theMainWindow->getFlattenFullHemisphereDialog(false);
            if (flattenFullHemDialog != NULL) {                
                  flattenFullHemDialog->setCentralSulcusDorsalMedialTip(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}
       
/**
 * mouse process for cell add mode.
 */
void 
GuiBrainModelOpenGL::mouseCellAdd(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiAddCellsDialog* acd = theMainWindow->getAddCellsDialog(false);
            if (acd != NULL) { 
               acd->addCellAtNodeNumber(selectedNode.getItemIndex1());
            }  
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for volume transforms in view mode.
 */
void 
GuiBrainModelOpenGL::mouseVolumeView(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelVolume* bmv = getDisplayedBrainModelVolume();
   const bool drawAll = 
      (bmv->getSelectedAxis(viewingWindowIndex) == VolumeFile::VOLUME_AXIS_OBLIQUE);

   //
   // Biggest change in mouse axis
   //
   const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;

   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         {
            BrainSet* bs = theMainWindow->getBrainSet(getModelViewNumber());
            BrainSetAutoLoaderManager* autoLoader = bs->getAutoLoaderManager();
            if (autoLoader->getAnyAutoLoaderSelected()) {
               selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_UNDERLAY, false);
               if (selectedVoxelUnderlay.getItemIndex1() >= 0) {
                  int ijk[3] = {
                        selectedVoxelUnderlay.getItemIndex1(),
                        selectedVoxelUnderlay.getItemIndex2(),
                        selectedVoxelUnderlay.getItemIndex3()
                  };
                  float xyz[3];
                  VolumeFile* vf = bmv->getUnderlayVolumeFile();
                  if (vf != NULL) {
                     vf->getVoxelCoordinate(ijk, xyz);
                     QString errorMessage =
                        autoLoader->processAutoLoading(xyz);
                     GuiFilesModified fm;
                     fm.setMetricModified();
                     fm.setVolumeModified();
                     theMainWindow->fileModificationUpdate(fm);
                     if (errorMessage.isEmpty() == false) {
                        QMessageBox::critical(this, "ERROR", errorMessage);
                     }
                  }
               }
               GuiFilesModified fm;
               fm.setMetricModified();
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
         }

         //
         // Perform an ID operation
         //
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_ALL, true);
         
         //
         // Was a node selected
         //
         if (selectedNode.getItemIndex1() > 0) {
            getBrainSet()->setDisplayCrossForNode(selectedNode.getItemIndex1(),
                                                  getDisplayedBrainModelSurface());
         }
         
         //
         // See if any voxels were selected
         //
         if (selectedVoxelUnderlay.getItemIndex1() > 0) {
            BrainModelVolume* bmv = theMainWindow->getBrainSet(getModelViewNumber())->getBrainModelVolume(-1);
            if (bmv != NULL) {
               VolumeFile* vf = bmv->getUnderlayVolumeFile();
               if (vf != NULL) {
                  float origin[3];
                  float spacing[3];
                  vf->getOrigin(origin);
                  vf->getSpacing(spacing);
                  
                  //
                  // Set the selected voxel
                  //
                  const int slices[3] = {
                     selectedVoxelUnderlay.getItemIndex1(),
                     selectedVoxelUnderlay.getItemIndex2(),
                     selectedVoxelUnderlay.getItemIndex3()
                  };
                  const int sliceOffsets[3] = {
                     selectedVoxelUnderlay.getItemIndex4(),
                     selectedVoxelUnderlay.getItemIndex5(),
                     selectedVoxelUnderlay.getItemIndex6()
                  };
                  
                  bool selectOrthogonalSlices = true;
                  switch (bmv->getSelectedAxis(viewingWindowIndex)) {
                     case VolumeFile::VOLUME_AXIS_X:
                     case VolumeFile::VOLUME_AXIS_Y:
                     case VolumeFile::VOLUME_AXIS_Z:
                     case VolumeFile::VOLUME_AXIS_ALL:
                        {
                           //
                           // Don't change slices if viewing montage
                           //
                           //const DisplaySettingsVolume* dsv = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySettingsVolume();
                           //if (dsv->getMontageViewSelected()) {
                           //   selectOrthogonalSlices = false;
                           //}
                        }
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE:
                        selectOrthogonalSlices = false;
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_X: 
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                        bmv->setSelectedObliqueSliceOffsets(viewingWindowIndex, sliceOffsets);
                        selectOrthogonalSlices = false;
                        break;
                     case VolumeFile::VOLUME_AXIS_UNKNOWN:
                        break;
                  }
                  if (selectOrthogonalSlices) {
                     //
                     // Montage View ?
                     //
                     DisplaySettingsVolume* dsv = theMainWindow->getBrainSet(getModelViewNumber())->getDisplaySettingsVolume();
                     if (dsv->getMontageViewSelected()) {
                        //
                        // Place selected slice in center of montage
                        //
                        int rows, cols, increment;
                        dsv->getMontageViewSettings(rows, cols, increment);
                        const int centerRow = (rows / 2);
                        const int centerCol = (cols / 2);
                        const int offset = (centerRow * cols + centerCol) * increment;
                        
                        int topLeftSlice[3] = { slices[0], slices[1], slices[2] };
                        switch (bmv->getSelectedAxis(viewingWindowIndex)) {
                           case VolumeFile::VOLUME_AXIS_X:
                              topLeftSlice[0] -= offset;
                              topLeftSlice[0] = std::max(topLeftSlice[0], 0);
                              break;
                           case VolumeFile::VOLUME_AXIS_Y:
                              topLeftSlice[1] -= offset;
                              topLeftSlice[1] = std::max(topLeftSlice[1], 0);
                              break;
                           case VolumeFile::VOLUME_AXIS_Z:
                              topLeftSlice[2] -= offset;
                              topLeftSlice[2] = std::max(topLeftSlice[2], 0);
                              break;
                           case VolumeFile::VOLUME_AXIS_ALL:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_X: 
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                           case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                           case VolumeFile::VOLUME_AXIS_UNKNOWN:
                              break;
                        }
                        bmv->setSelectedOrthogonalSlices(viewingWindowIndex, topLeftSlice);
                     }
                     else {
                        bmv->setSelectedOrthogonalSlices(viewingWindowIndex, slices);
                        
                        //
                        // If main window or yoked to main window
                        // update selected slices in the main and yoked windows
                        //
                        if ((viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) ||
                            getYokeView()) {
                           for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
                              if (allBrainSurfaceOpenGL[i] != NULL) {
                                 BrainModelVolume* bmv2 = allBrainSurfaceOpenGL[i]->getDisplayedBrainModelVolume();
                                 if (bmv2 != NULL) {
                                    if ((i == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) ||
                                        allBrainSurfaceOpenGL[i]->getYokeView()) {
                                       bmv2->setSelectedOrthogonalSlices(i, slices);
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }                 
               }
            }
         }
         
         //
         // If transformation axis selected, switch to transform axes mouse mode
         //
         if (isMainWindowOpenGL()) {
            if (selectedTransformationAxes.getItemIndex1() >= 0) {
               setMouseMode(MOUSE_MODE_TRANSFORMATION_MATRIX_AXES);
               TransformationMatrixFile* tmf = theMainWindow->getBrainSet(getModelViewNumber())->getTransformationMatrixFile();
               const int indx = selectedTransformationAxes.getItemIndex1();
               if ((indx >= 0) && (indx < tmf->getNumberOfMatrices())) {
                  tmf->setSelectedTransformationAxesIndex(indx);
               }
            }
         }
         
         //
         // Update toolbar all GL so that green/blue ID  node symbols show up in all windows
         //   
         GuiToolBar::updateAllToolBars(false);
         GuiBrainModelOpenGL::updateAllGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            GuiMainWindowVolumeActions* volumeActions = theMainWindow->getVolumeActions();

            if (bmv->getSelectedAxis(viewingWindowIndex) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
               vtkTransform* matrix = bmv->getObliqueRotationMatrix();
               matrix->RotateX(me.dy);
               matrix->RotateY(-me.dx);
            }
            else if (volumeActions->getTranformRotationChecked()) {
               bmv->addToDisplayRotation(viewingWindowIndex, -biggestChange);
            }
            else {
               if (bmv->getSelectedAxis(viewingWindowIndex) == VolumeFile::VOLUME_AXIS_ALL) {
                  BrainModelSurface* bms = theMainWindow->getBrainSet(getModelViewNumber())->getActiveFiducialSurface();
                  if (bms != NULL) {
                     vtkTransform* matrix = bms->getRotationTransformMatrix(viewingWindowIndex);
                     matrix->RotateX(me.dy);
                     matrix->RotateY(-me.dx);
                  }
               }
            }
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            if (drawAll) {
               updateAllGL();
            }
            else {
               updateAllGL(this);
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         {
            float translate[3];
            bmv->getTranslation(viewingWindowIndex, translate);
            translate[0] += me.dx;
            translate[1] += me.dy;
            translate[2] =  0.0;
            bmv->setTranslation(viewingWindowIndex, translate);
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            if (drawAll) {
               updateAllGL();
            }
            else {
               updateAllGL(this);
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         {
            float scale[3];
            bmv->getScaling(viewingWindowIndex, scale);
            //scale += me.dy * scale * 0.01;
            scale[0] += biggestChange * scale[0] * 0.01;
            scale[1] += biggestChange * scale[1] * 0.01;
            scale[2] += biggestChange * scale[2] * 0.01;
            scale[0] = std::max(scale[0], 0.01f);
            scale[1] = std::max(scale[1], 0.01f);
            scale[2] = std::max(scale[2], 0.01f);
            //if (scale < 0.01) scale = 0.01;   // negative would flip surface
            bmv->setScaling(viewingWindowIndex, scale);
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            if (drawAll) {
               updateAllGL();
            }
            else {
               updateAllGL(this);
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour transforms in view mode.
 */
void 
GuiBrainModelOpenGL::mouseContourView(const GuiBrainModelOpenGLMouseEvent& me)
{
   //
   // Biggest change in mouse axis
   //
   const int biggestChange = (abs(me.dy) > abs(me.dx)) ? me.dy : me.dx;

   BrainModelContours* bmc = getDisplayedBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         //
         // Perform and ID operation
         //
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_ALL, true);
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            vtkTransform* matrix = bmc->getRotationTransformMatrix(viewingWindowIndex);
            matrix->RotateZ(-biggestChange); //-me.dx);
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         {
            float translate[3];
            bmc->getTranslation(viewingWindowIndex, translate);
            const float multiplier = 0.10;
            translate[0] += me.dx * multiplier;
            translate[1] += me.dy * multiplier;
            translate[2] =  0.0;
            bmc->setTranslation(viewingWindowIndex, translate);
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         {
            float scale[3];
            bmc->getScaling(viewingWindowIndex, scale);
            //scale += me.dy * scale * 0.01;
            scale[0] += biggestChange * scale[0] * 0.01;
            scale[1] += biggestChange * scale[1] * 0.01;
            scale[2] += biggestChange * scale[2] * 0.01;
            scale[0] = std::max(scale[0], 0.0001f);
            scale[1] = std::max(scale[1], 0.0001f);
            scale[2] = std::max(scale[2], 0.0001f);
            //if (scale < 0.01) scale = 0.01;   // negative would flip surface
            bmc->setScaling(viewingWindowIndex, scale);
            if (viewingWindowIndex == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
               theMainWindow->updateTransformationMatrixEditor(NULL);
            }
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}
       
/**
 * mouse process for contour set scale.
 */
void 
GuiBrainModelOpenGL::mouseContourSetScale(const GuiBrainModelOpenGLMouseEvent& me)
{
   GuiContourSetScaleDialog* ssd = theMainWindow->getContourSetScaleDialog(false);
   if (ssd != NULL) {
      //
      // Convert mouse position to model coordinates
      //
      float modelX = me.x;
      float modelY = me.y;
      float modelZ = 0.0;
      convertWindowToModelCoords(me.x, me.y, false, modelX, modelY, modelZ);
      
      switch(me.event) {
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
            ssd->setScaleStartPoint(modelX, modelY);
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
            ssd->setScaleEndPoint(modelX, modelY);
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
            break;
         case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
            break;
      }
   }
}

/**
 * mouse process for contour draw.
 */
void 
GuiBrainModelOpenGL::mouseContourDraw(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         if (linearObjectBeingDrawn.getNumberOfLinks() > 1) {
            //
            // Get the contour model
            //
            BrainModelContours* bmc = getDisplayedBrainModelContours();
            if (bmc != NULL) {
               //
               // Get the contour drawing dialog
               //
               GuiContourDrawDialog* cdd = theMainWindow->getContourDrawDialog(false);
               
               //
               // Create the contour
               //
               CaretContour cc;
               if (cdd != NULL) {
                  cc.setSectionNumber(cdd->getSectionNumber());
               }
               
               //
               // Resample to contour spacing
               //
               if (cdd != NULL) {
                  int newNumLinks;
                  linearObjectBeingDrawn.resampleBorderToDensity(cdd->getPointSpacing(),
                                                                 2, newNumLinks);
               }
               
               //
               // Create the new contour
               //
               ContourFile* cf = bmc->getContourFile();
               float z = cc.getSectionNumber();
               const float spacing = cf->getSectionSpacing();
               if (spacing != 0.0) {
                  z *= spacing;
               }
               const int numPoints = linearObjectBeingDrawn.getNumberOfLinks();
               for (int i = 0; i < numPoints; i++) {
                  const float* xyz = linearObjectBeingDrawn.getLinkXYZ(i);
                  cc.addPoint(xyz[0], xyz[1], z);
               }
               cf->addContour(cc);
               
               //
               // If single sections, set to the section just drawn
               //
               if (cf->getSectionType() == ContourFile::SECTION_TYPE_SINGLE) {
                   cf->setMinimumSelectedSection(cdd->getSectionNumber());
                   cf->setMaximumSelectedSection(cdd->getSectionNumber());
               }
            }
            
            //
            // Update main window
            //
            GuiFilesModified fm;
            fm.setContourModified();
            theMainWindow->fileModificationUpdate(fm);
            resetLinearObjectBeingDrawn();
            updateGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            //
            // Convert the window coordinate to a surface coordinate
            //
            float modelXYZ[3];
            convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
            if (DebugControl::getDebugOn()) {
               std::cout << "Window: (" << me.x << ", " << me.y << ")"
                        << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
                        << modelXYZ[2] << ")" << std::endl;
            }
            modelXYZ[2] = 0.0;
                  
            linearObjectBeingDrawn.addBorderLink(modelXYZ, 0);
/*
            if (linearObjectBeingDrawn.getNumberOfLinks() > 2) {
               //
               // Resample to contour spacing
               //
               GuiContourDrawDialog* cdd = theMainWindow->getContourDrawDialog(false);
               if (cdd != NULL) {
                  int newNumLinks;
                  linearObjectBeingDrawn.resampleBorderToDensity(cdd->getPointSpacing(),
                                                                 2, newNumLinks);
               }
            }
*/            
            drawLinearObjectOnly = true;
            updateGL();
            drawLinearObjectOnly = false;
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour align.
 */
void 
GuiBrainModelOpenGL::mouseContourAlign(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelContours* bmc = getDisplayedBrainModelContours();
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            vtkTransform* matrix = bmc->getAlignmentRotationTransformMatrix();
            matrix->RotateZ(-me.dx * 0.10);
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         {
            float translate[3];
            bmc->getAlignmentTranslation(translate);
            float multiplier = 0.1;  // 0.5
#ifdef OS_MACX
            multiplier = 0.10;
#endif
            translate[0] += me.dx * multiplier;
            translate[1] += me.dy * multiplier;
            translate[2] =  0.0;
            bmc->setAlignmentTranslation(translate);
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         {
            float scale = bmc->getAlignmentScaling();
            scale += me.dy * scale * 0.01;
            if (scale < 0.01) scale = 0.01;   // negative would flip surface
            bmc->setAlignmentScaling(scale);
            updateAllGL(this);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour region align.
 */
void 
GuiBrainModelOpenGL::mouseContourAlignRegion(const GuiBrainModelOpenGLMouseEvent& me)
{
   BrainModelContours* bmc = getDisplayedBrainModelContours();
   GuiContourAlignmentDialog* cad = theMainWindow->getContourAlignmentDialog(false);
   const int sectionNumber = cad->getAlignmentSectionNumber();
   
   //
   // Convert mouse position to contour coordinates
   //
   float modelXYZ[3];
   convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
   if (DebugControl::getDebugOn()) {
      std::cout << "Window: (" << me.x << ", " << me.y << ")"
               << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
               << modelXYZ[2] << ")" << std::endl;
   }
   modelXYZ[2] = 0.0;
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         //
         // Clear the region and Set the start of the region
         //
         {
            bmc->resetAlignmentRegionBox();
            bmc->setAlignmentRegionBoxStart(modelXYZ);
            updateGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         //
         // Select contour points within the region
         //
         {
            bmc->setAlignmentRegionBoxEnd(modelXYZ);
            float bounds[4];
            bmc->getAlignmentRegionBox(bounds);
            ContourFile* contours = bmc->getContourFile();
            contours->setSpecialFlags(sectionNumber, sectionNumber, bounds);
            ContourCellFile* contourCells = theMainWindow->getBrainSet(getModelViewNumber())->getContourCellFile();
            contourCells->setSpecialFlags(sectionNumber, sectionNumber, bounds);            
            updateGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         //
         // Reset contour region and clear selected contour points
         //
         {
            bmc->resetAlignmentRegionBox();
            ContourFile* contours = bmc->getContourFile();
            contours->clearSpecialFlags();
            ContourCellFile* contourCells = theMainWindow->getBrainSet(getModelViewNumber())->getContourCellFile();
            contourCells->clearAllSpecialFlags();
            updateGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         //
         // Switch to alignment mode and clear the region
         //
         bmc->resetAlignmentRegionBox();
         setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN);
         updateGL();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         //
         // Set the end point of the region
         //
         {
            bmc->setAlignmentRegionBoxEnd(modelXYZ);
            updateGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour move point.
 */
void 
GuiBrainModelOpenGL::mouseContourPointMove(const GuiBrainModelOpenGLMouseEvent& me)
{
   static int contourNumber = -1;
   static int pointNumber   = -1;
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         contourNumber = -1;
         pointNumber   = -1;
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR);
         if ((selectedContour.getItemIndex1() >= 0) &&
             (selectedContour.getItemIndex2() >= 0)) {
            BrainModelContours* bmc = theMainWindow->getBrainModelContours();
            if (bmc == NULL) {
               return;
            }
            contourNumber = selectedContour.getItemIndex1();
            pointNumber   = selectedContour.getItemIndex2();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         if ((contourNumber >= 0) && (pointNumber >= 0)) {
            BrainModelContours* bmc = theMainWindow->getBrainModelContours();
            if (bmc == NULL) {
               return;
            }
            ContourFile* cf = bmc->getContourFile();
            if (contourNumber < cf->getNumberOfContours()) {
               CaretContour* cc = cf->getContour(contourNumber);
               const int numPoints = cc->getNumberOfPoints();
               if (pointNumber < numPoints) {
                  float x, y, z;
                  convertWindowToModelCoords(me.x, me.y, false, x, y, z);
                  cc->setPointXY(pointNumber, x, y);
               }
            }
            updateAllGL(NULL);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour delete point.
 */
void 
GuiBrainModelOpenGL::mouseContourPointDelete(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR);
         if ((selectedContour.getItemIndex1() >= 0) &&
             (selectedContour.getItemIndex2() >= 0)) {
            BrainModelContours* bmc = theMainWindow->getBrainModelContours();
            if (bmc == NULL) {
               return;
            }
            ContourFile* cf = bmc->getContourFile();
            CaretContour* cc = cf->getContour(selectedContour.getItemIndex1());
            cc->deletePoint(selectedContour.getItemIndex2());
            updateAllGL(NULL);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour delete.
 */
void 
GuiBrainModelOpenGL::mouseContourDelete(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR);
         if ((selectedContour.getItemIndex1() >= 0) &&
             (selectedContour.getItemIndex2() >= 0)) {
            BrainModelContours* bmc = theMainWindow->getBrainModelContours();
            if (bmc == NULL) {
               return;
            }
            ContourFile* cf = bmc->getContourFile();
            cf->deleteContour(selectedContour.getItemIndex1());
            updateAllGL(NULL);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour reverse.
 */
void 
GuiBrainModelOpenGL::mouseContourReverse(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR);
         if ((selectedContour.getItemIndex1() >= 0) &&
             (selectedContour.getItemIndex2() >= 0)) {
            BrainModelContours* bmc = theMainWindow->getBrainModelContours();
            if (bmc == NULL) {
               return;
            }
            ContourFile* cf = bmc->getContourFile();
            CaretContour* contour = cf->getContour(selectedContour.getItemIndex1());
            if (cf != NULL) {
               contour->reversePointOrder();
               updateAllGL(NULL);
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour delete.
 */
void 
GuiBrainModelOpenGL::mouseContourMerge(const GuiBrainModelOpenGLMouseEvent& me)
{
   static int contourOne = -1;
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR);
         if ((selectedContour.getItemIndex1() >= 0) &&
             (selectedContour.getItemIndex2() >= 0)) {
            BrainModelContours* bmc = theMainWindow->getBrainModelContours();
            if (bmc == NULL) {
               return;
            }
            ContourFile* cf = bmc->getContourFile();
            if (selectedContour.getItemIndex1() < cf->getNumberOfContours()) {
               contourOne = selectedContour.getItemIndex1();
            }
         }
         else {
            QApplication::beep();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR);
         if ((selectedContour.getItemIndex1() >= 0) &&
             (selectedContour.getItemIndex2() >= 0)) {
            BrainModelContours* bmc = theMainWindow->getBrainModelContours();
            if (bmc == NULL) {
               return;
            }
            ContourFile* cf = bmc->getContourFile();
            if (selectedContour.getItemIndex1() < cf->getNumberOfContours()) {
               if (contourOne < cf->getNumberOfContours()) {
                  cf->mergeContours(contourOne, selectedContour.getItemIndex1());
                  GuiFilesModified fm;
                  fm.setContourModified();
                  theMainWindow->fileModificationUpdate(fm);
                  updateAllGL(NULL);
               }
            }
         }
         else {
            QApplication::beep();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for contour cell add mode.
 */
void 
GuiBrainModelOpenGL::mouseContourCellAdd(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         {
            GuiAddCellsDialog* acd = theMainWindow->getAddContourCellsDialog(false);
            if (acd != NULL) { 
               float modelXYZ[3];
               convertWindowToModelCoords(me.x, me.y, false, modelXYZ[0], modelXYZ[1], modelXYZ[2]);
               if (DebugControl::getDebugOn()) {
                  std::cout << "Window: (" << me.x << ", " << me.y << ")"
                           << " Model: (" << modelXYZ[0] << ", " << modelXYZ[1] << ", " 
                           << modelXYZ[2] << ")" << std::endl;
               }
               BrainModelContours* bmc = theMainWindow->getBrainSet(getModelViewNumber())->getBrainModelContours(-1);
               if (bmc != NULL) {
                  ContourFile* cf = bmc->getContourFile();
                  if (cf != NULL) {
                     modelXYZ[2] = cf->getSectionSpacing() * cf->getMinimumSelectedSection();
                     acd->addContourCellAtXYZ(modelXYZ, cf->getMinimumSelectedSection());
                  }
               }
            }  
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for delete contour cell
 */
void  
GuiBrainModelOpenGL::mouseContourCellDelete(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR_CELL);
         if (DebugControl::getDebugOn()) {
            std::cout << "Selected Contour Cell: "
                     << selectedContourCell.getItemIndex1() << std::endl;
         }
         
         if (selectedContourCell.getItemIndex1() >= 0) {
            ContourCellFile* cf = theMainWindow->getBrainSet(getModelViewNumber())->getContourCellFile();
            cf->deleteCell(selectedContourCell.getItemIndex1());
            updateAllGL(NULL);
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for delete contour cell
 */
void  
GuiBrainModelOpenGL::mouseContourCellMove(const GuiBrainModelOpenGLMouseEvent& me)
{
   static int contourCellNumber = -1;
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_CONTOUR_CELL);
         if (DebugControl::getDebugOn()) {
            std::cout << "Selected Contour Cell: "
                     << selectedContourCell.getItemIndex1() << std::endl;
         }
         
         if (selectedContourCell.getItemIndex1() >= 0) {
            contourCellNumber = selectedContourCell.getItemIndex1();
            updateGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         contourCellNumber = -1;
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            ContourCellFile* cf = theMainWindow->getBrainSet(getModelViewNumber())->getContourCellFile();
            if ((contourCellNumber >= 0) && (contourCellNumber < cf->getNumberOfCells())) {
               CellData* cell = cf->getCell(contourCellNumber);
               float xyz[3];
               cell->getXYZ(xyz);
               float x, y, z;
               convertWindowToModelCoords(me.x, me.y, false, x, y, z);
               xyz[0] = x;
               xyz[1] = y; // ignore Z
               cell->setXYZ(xyz);
               updateGL();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse process for roi sulcal border start node.
 */
void 
GuiBrainModelOpenGL::mouseSurfaceROISulcalBorderNodeStart(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiSurfaceRegionOfInterestDialog* roi = theMainWindow->getSurfaceRegionOfInterestDialog(false);
            if (roi != NULL) {               
               roi->setCreateLinearBorderStartNode(selectedNode.getItemIndex1());
            }
            GuiSurfaceRegionOfInterestDialogOLD* roiOLD = theMainWindow->getSurfaceRegionOfInterestDialogOLD(false);
            if (roiOLD != NULL) {               
               roiOLD->setCreateBorderOpenStartNode(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}
 
/**
 * mouse process for roi sulcal border end node.
 */
void 
GuiBrainModelOpenGL::mouseSurfaceROISulcalBorderNodeEnd(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         if (selectedNode.getItemIndex1() >= 0) {
            GuiSurfaceRegionOfInterestDialog* roi = theMainWindow->getSurfaceRegionOfInterestDialog(false);
            if (roi != NULL) {               
               roi->setCreateLinearBorderEndNode(selectedNode.getItemIndex1());
            }
            GuiSurfaceRegionOfInterestDialogOLD* roiOLD = theMainWindow->getSurfaceRegionOfInterestDialogOLD(false);
            if (roiOLD != NULL) {               
               roiOLD->setCreateBorderOpenEndNode(selectedNode.getItemIndex1());
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}       

/**
  * mouse process for image subregion selection.
  */
void 
GuiBrainModelOpenGL::mouseImageSubRegion(const GuiBrainModelOpenGLMouseEvent& me)
{ 
   bool redraw = false;
   
   const int x = me.x;
   const int y = windowHeight[viewingWindowIndex] - me.y;
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         imageSubRegionBoxValid = true;
         imageSubRegionBox[0] = x;
         imageSubRegionBox[1] = y;
         imageSubRegionBox[2] = x;
         imageSubRegionBox[3] = y;
         redraw = true;
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         //imageSubRegionBoxValid = false;
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         imageSubRegionBox[2] = x;
         imageSubRegionBox[3] = y;
         redraw = true;
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
   
   if (redraw) {
      updateGL();
   }
}

/**
 * mouse processing for volume paint editing
 */
void  
GuiBrainModelOpenGL::mouseVolumePaintEdit(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            //
            // Make sure we are looking at a volume
            //
            BrainModelVolume* bmv = getDisplayedBrainModelVolume();
            if (bmv != NULL) {
               //
               // See if a paint volume is selected
               //
               VolumeFile* paintVolume = bmv->getSelectedVolumePaintFile();
               if (paintVolume != NULL) {
                  //
                  // Get the paint volume editing dialog
                  //
                  GuiVolumePaintEditorDialog* volumePaintEditorDialog
                                  = theMainWindow->getVolumePaintEditorDialog(false);
                  if (volumePaintEditorDialog != NULL) {
                     //
                     // Tell rending that a paint is being edited so that 
                     // voxels with value 0 will be reported
                     //
                     openGL->setEditingPaintVolumeFlag(true);

                     //
                     // Process voxel selection
                     //
                     if (paintVolume == bmv->getOverlayPrimaryVolumeFile()) {
                        selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_OVERLAY_PRIMARY);
                        if (selectedVoxelOverlayPrimary.getItemIndex1() >= 0) {
                           volumePaintEditorDialog->processVoxel(
                                                      selectedVoxelOverlayPrimary.getItemIndex1(),
                                                      selectedVoxelOverlayPrimary.getItemIndex2(),
                                                      selectedVoxelOverlayPrimary.getItemIndex3(),
                                static_cast<VolumeFile::VOLUME_AXIS>(selectedVoxelOverlayPrimary.getItemIndex4()));
                        }
                     }
                     else if (paintVolume == bmv->getOverlaySecondaryVolumeFile()) {
                        selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_OVERLAY_SECONDARY);
                        if (selectedVoxelOverlaySecondary.getItemIndex1() >= 0) {
                           volumePaintEditorDialog->processVoxel(
                                                      selectedVoxelOverlaySecondary.getItemIndex1(),
                                                      selectedVoxelOverlaySecondary.getItemIndex2(),
                                                      selectedVoxelOverlaySecondary.getItemIndex3(),
                                static_cast<VolumeFile::VOLUME_AXIS>(selectedVoxelOverlaySecondary.getItemIndex4()));
                        }
                     }
                     else if (paintVolume == bmv->getUnderlayVolumeFile()) {
                        selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_UNDERLAY);
                        if (selectedVoxelUnderlay.getItemIndex1() >= 0) {
                           volumePaintEditorDialog->processVoxel(
                                                      selectedVoxelUnderlay.getItemIndex1(),
                                                      selectedVoxelUnderlay.getItemIndex2(),
                                                      selectedVoxelUnderlay.getItemIndex3(),
                                static_cast<VolumeFile::VOLUME_AXIS>(selectedVoxelUnderlay.getItemIndex4()));
                        }
                     }
                     
                     //
                     // Tell rending that a paint is NO longer being edited 
                     //
                     openGL->setEditingPaintVolumeFlag(false);
                  }
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse processing for volume segmentation editing
 */
void  
GuiBrainModelOpenGL::mouseVolumeSegmentationEdit(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            //
            // Make sure we are looking at a volume
            //
            BrainModelVolume* bmv = getDisplayedBrainModelVolume();
            if (bmv != NULL) {
               //
               // See if a segmentation volume is selected
               //
               VolumeFile* segmentVolume = bmv->getSelectedVolumeSegmentationFile();
               if (segmentVolume != NULL) {
                  //
                  // Get the segmentation volume editing dialog
                  //
                  GuiVolumeSegmentationEditorDialog* volumeSegmentationEditorDialog
                     = theMainWindow->getVolumeSegmentationEditorDialog(false);
                  if (volumeSegmentationEditorDialog != NULL) {
                     //
                     // Tell rending that a segmentation is being edited so that 
                     // voxels with value 0 will be reported
                     //
                     openGL->setEditingSegmentationVolumeFlag(true);

                     //
                     // Process voxel selection
                     //
                     if (segmentVolume == bmv->getOverlayPrimaryVolumeFile()) {
                        selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_OVERLAY_PRIMARY);
                        if (selectedVoxelOverlayPrimary.getItemIndex1() >= 0) {
                           volumeSegmentationEditorDialog->processVoxel(
                                                      selectedVoxelOverlayPrimary.getItemIndex1(),
                                                      selectedVoxelOverlayPrimary.getItemIndex2(),
                                                      selectedVoxelOverlayPrimary.getItemIndex3(),
                                     static_cast<VolumeFile::VOLUME_AXIS>(selectedVoxelOverlayPrimary.getItemIndex4()));
                        }
                     }
                     else if (segmentVolume == bmv->getOverlaySecondaryVolumeFile()) {
                        selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_OVERLAY_SECONDARY);
                        if (selectedVoxelOverlaySecondary.getItemIndex1() >= 0) {
                           volumeSegmentationEditorDialog->processVoxel(
                                                      selectedVoxelOverlaySecondary.getItemIndex1(),
                                                      selectedVoxelOverlaySecondary.getItemIndex2(),
                                                      selectedVoxelOverlaySecondary.getItemIndex3(),
                                      static_cast<VolumeFile::VOLUME_AXIS>(selectedVoxelOverlaySecondary.getItemIndex4()));
                        }
                     }
                     else if (segmentVolume == bmv->getUnderlayVolumeFile()) {
                        selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_VOXEL_UNDERLAY);
                        if (selectedVoxelUnderlay.getItemIndex1() >= 0) {
                           volumeSegmentationEditorDialog->processVoxel(
                                                      selectedVoxelUnderlay.getItemIndex1(),
                                                      selectedVoxelUnderlay.getItemIndex2(),
                                                      selectedVoxelUnderlay.getItemIndex3(),
                                       static_cast<VolumeFile::VOLUME_AXIS>(selectedVoxelUnderlay.getItemIndex4()));
                        }
                     }
                     
                     //
                     // Tell rending that a segmentation is NO longer being edited 
                     //
                     openGL->setEditingSegmentationVolumeFlag(false);
                  }
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse mode for adding a node.
 */
void 
GuiBrainModelOpenGL::mouseAddNodes(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         {
            //
            // Add a node assuming viewing X/Y plane.
            //
            BrainModelSurface* bms = getDisplayedBrainModelSurface();
            if (bms != NULL) {
               float x, y, z;
               convertWindowToModelCoords(me.x, me.y, false, x, y, z);
               theMainWindow->getBrainSet(getModelViewNumber())->addNodes(1);
               CoordinateFile* cf = bms->getCoordinateFile();
               const int lastNode = cf->getNumberOfCoordinates() - 1;
               if (lastNode >= 0) {
                 cf->setCoordinate(lastNode, x, y, 0.0f);
               }
               updateAllGL();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse mode for adding a tile.
 */
void 
GuiBrainModelOpenGL::mouseAddTile(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         {
            BrainModelSurface* bms = getDisplayedBrainModelSurface();
            if (bms != NULL) {
               selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
               const int nodeNumber = selectedNode.getItemIndex1();
               if (nodeNumber >= 0) {
                  if (newTileNodeCounter < 3) {
                     newTileNodeNumbers[newTileNodeCounter] = nodeNumber;
                     newTileNodeCounter++;
                     if (newTileNodeCounter == 3) {
                        TopologyFile* tf = bms->getTopologyFile();
                        if (tf == NULL) {
                           tf = new TopologyFile;
                           theMainWindow->getBrainSet(getModelViewNumber())->addTopologyFile(tf);
                           bms->setTopologyFile(tf);
                        }
                        tf->addTile(newTileNodeNumbers);
                        newTileNodeCounter = 0;
                     }
                  }
                  else {
                     newTileNodeCounter = 0;
                  }
                  theMainWindow->getBrainSet(getModelViewNumber())->clearAllDisplayLists();
                  updateAllGL();
               }
               else {
                  QApplication::beep();
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse mode for deleting tile by clicking link.
 */
void
GuiBrainModelOpenGL::mouseDeleteTileByLink(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         {
            BrainModelSurface* bms = getDisplayedBrainModelSurface();
            if (bms != NULL) {
               selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_LINK);
               const int nodeNumber1 = selectedLink.getItemIndex1();
               const int nodeNumber2 = selectedLink.getItemIndex2();
               TopologyFile* tf = bms->getTopologyFile();
               if (tf != NULL) {
                  tf->deleteTilesWithEdge(nodeNumber1, nodeNumber2);
               }
               updateAllGL();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse mode for disconnect node.
 */
void 
GuiBrainModelOpenGL::mouseDisconnectNode(const GuiBrainModelOpenGLMouseEvent& me)
{
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         {
            BrainModelSurface* bms = getDisplayedBrainModelSurface();
            if (bms != NULL) {
               selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
               const int nodeNumber = selectedNode.getItemIndex1();
               if (nodeNumber >= 0) {
                  TopologyFile* tf = bms->getTopologyFile();
                  if (tf != NULL) {
                     const int numNodes = tf->getNumberOfNodes();
                     if (nodeNumber < numNodes) {
                        std::vector<bool> nodesToDelete(numNodes, false);
                        nodesToDelete[nodeNumber] = true;
                        tf->deleteTilesWithMarkedNodes(nodesToDelete);
                     }
                  }
                  updateAllGL();
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse mode for setting transformation matrix translate.
 */
void
GuiBrainModelOpenGL::mouseTranslationAxesSetTranslate(const GuiBrainModelOpenGLMouseEvent& me)
{
   selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_ALL);
      
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         {
            GuiTransformationMatrixDialog* tmd = theMainWindow->getTransformMatrixEditor();
            if (tmd != NULL) {
               //
               // Find the identified items
               //
               bool validXYZ = false;
               float x, y, z;
               float depth = std::numeric_limits<float>::max();
               if (selectedNode.getItemIndex1() >= 0) {
                  const BrainModelSurface* bms = getDisplayedBrainModelSurface();
                  if (bms != NULL) {
                     const int node = selectedNode.getItemIndex1();
                     if ((node >= 0) && (node < bms->getNumberOfNodes())) {
                        const CoordinateFile* cf = bms->getCoordinateFile();
                        if (selectedNode.getDepth() < depth) {
                           cf->getCoordinate(node, x, y, z);
                           depth = selectedNode.getDepth();
                           validXYZ = true;
                        }
                     }
                  }
               }
               BrainModelVolume* bmv = getDisplayedBrainModelVolume();
               if (bmv != NULL) {
                  VolumeFile* vf = NULL;
                  int ijk[3] = { -1, -1, -1 };
                  float vd = std::numeric_limits<float>::max();
                  if (selectedVoxelUnderlay.getItemIndex1() >= 0) {
                     vf = bmv->getUnderlayVolumeFile();
                     if (vf != NULL) {
                        if (vf != NULL) {
                           ijk[0] = selectedVoxelUnderlay.getItemIndex1();
                           ijk[1] = selectedVoxelUnderlay.getItemIndex2();
                           ijk[2] = selectedVoxelUnderlay.getItemIndex3();
                           vd = selectedVoxelUnderlay.getDepth();
                        }
                     }
                  }
                  if (selectedVoxelOverlaySecondary.getItemIndex1() >= 0) {
                     vf = bmv->getOverlaySecondaryVolumeFile();
                     if (vf != NULL) {
                        if (vf != NULL) {
                           ijk[0] = selectedVoxelOverlaySecondary.getItemIndex1();
                           ijk[1] = selectedVoxelOverlaySecondary.getItemIndex2();
                           ijk[2] = selectedVoxelOverlaySecondary.getItemIndex3();
                           vd = selectedVoxelOverlaySecondary.getDepth();
                        }
                     }
                  }
                  if (selectedVoxelOverlayPrimary.getItemIndex1() >= 0) {
                     vf = bmv->getOverlayPrimaryVolumeFile();
                     if (vf != NULL) {
                        if (vf != NULL) {
                           ijk[0] = selectedVoxelOverlayPrimary.getItemIndex1();
                           ijk[1] = selectedVoxelOverlayPrimary.getItemIndex2();
                           ijk[2] = selectedVoxelOverlayPrimary.getItemIndex3();
                           vd = selectedVoxelOverlayPrimary.getDepth();
                        }
                     }
                  }
                  
                  if (ijk[0] >= 0) {
                     float s[3], o[3];
                     vf->getOrigin(o);
                     vf->getSpacing(s);
                     if (vd < depth) {
                        x = o[0] + s[0] * ijk[0];
                        y = o[1] + s[1] * ijk[1];
                        z = o[2] + s[2] * ijk[2];
                        depth = vd;
                        validXYZ = true;
                     }
                  }
               }

               if (selectedVtkModel.getItemIndex1() >= 0) {
                  const int modelNum = selectedVtkModel.getItemIndex1();
                  const int tileNum = selectedVtkModel.getItemIndex2();
                  if ((modelNum >= 0) && (modelNum < theMainWindow->getBrainSet(getModelViewNumber())->getNumberOfVtkModelFiles())) {
                     const VtkModelFile* vmf = theMainWindow->getBrainSet(getModelViewNumber())->getVtkModelFile(modelNum);
                     float xyz[3];
                     vmf->getTriangleCoordinate(tileNum, xyz);
                     if (selectedVtkModel.getDepth() < depth) {
                        x = xyz[0];
                        y = xyz[1];
                        z = xyz[2];
                        depth = selectedVtkModel.getDepth();
                        validXYZ = true;
                     }
                  }
               }

               //
               // If transformation axis selected, switch to transform axes mouse mode
               //
               if ((selectedTransformationAxes.getItemIndex1() >= 0) &&
                   (selectedTransformationAxes.getDepth() < depth)) {
                  setMouseMode(MOUSE_MODE_TRANSFORMATION_MATRIX_AXES);
                  TransformationMatrixFile* tmf = theMainWindow->getBrainSet(getModelViewNumber())->getTransformationMatrixFile();
                  const int indx = selectedTransformationAxes.getItemIndex1();
                  if ((indx >= 0) && (indx < tmf->getNumberOfMatrices())) {
                     tmf->setSelectedTransformationAxesIndex(indx);
                  }
                  updateAllGL();
                  return;
               }
               else if (validXYZ) {
                  tmd->setTranslation(x, y, z);
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse mode for translation axes.
 */
void 
GuiBrainModelOpenGL::mouseTranslationAxes(const GuiBrainModelOpenGLMouseEvent& me)
{
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet(getModelViewNumber())->getTransformationMatrixFile();
   const int indx = tmf->getSelectedTransformationAxesIndex();
   if ((indx < 0) || (indx >= tmf->getNumberOfMatrices())) {
      return;
   }
   TransformationMatrix* tm = tmf->getTransformationMatrix(indx);
   
   BrainModelSurface* bms = getDisplayedBrainModelSurface();
   if (bms == NULL) {
      bms = getDisplayedBrainModelSurfaceAndVolume();
   }

   vtkTransform* surfRotMatrix = NULL;
   if (bms != NULL) {
      surfRotMatrix = bms->getRotationTransformMatrix(viewingWindowIndex);
   }
   else {
      BrainModelVolume* bmv = getDisplayedBrainModelVolume();
      if (bmv != NULL) {
         surfRotMatrix = bmv->getRotationTransformMatrix(viewingWindowIndex);
      }
      //std::cout << "ERROR: No surface but doing translation axes." << std::endl;
   }
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         {
            selectBrainModelItem(me.x, me.y, 
                                 BrainModelOpenGL::SELECTION_MASK_TRANSFORMATION_MATRIX_AXES);
            const int newItem = selectedTransformationAxes.getItemIndex1();
            if (newItem == indx) {
               setMouseMode(MOUSE_MODE_VIEW);
            }
            else if ((newItem >= 0) && (newItem < tmf->getNumberOfMatrices())) {
               tmf->setSelectedTransformationAxesIndex(newItem);
               updateAllGL();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         {
            GuiTransformationMatrixDialog* tmd = theMainWindow->getTransformMatrixEditor();
            if (tmd != NULL) {
               tmd->axesEventInMainWindow();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         {
            //vtkTransform* matrix = dss->getTransformationAxesRotationMatrix();
            float rotX = -me.dy;
            float rotY = me.dx;
            float rotZ = 0.0;
            if ((rotX != 0.0) || (rotY != 0.0)) {
/*            
               // works if surface NOT rotated
               TransformationMatrix t1;
               t1.rotateZ(rotZ);
               t1.rotateX(rotX);
               t1.rotateY(rotY);
               tm->multiply(t1);
*/

/*
               // works if surface is rotated but NOT if surface is translated
               TransformationMatrix t2;
               if (surfRotMatrix != NULL) {
                  t2.setMatrix(surfRotMatrix);
               }
               float r[3] = { rotX, rotY, rotZ };
               t2.multiplyPoint(r);
               
               std::cout << "rot: " << rotX << ", " << rotY << ", " << rotZ << std::endl;
               std::cout << "r[]: " << r[0] << ", " << r[1] << ", " << r[2] << std::endl;
               std::cout << std::endl;
               
               TransformationMatrix t3;
               t3.rotateZ(r[2]);
               t3.rotateX(r[0]);
               t3.rotateY(r[1]);
               
               tm->multiply(t3);
*/

               //
               // Alter the rotation angles using the surface viewing matrix
               //
               TransformationMatrix t2;
               if (surfRotMatrix != NULL) {
                  t2.setMatrix(surfRotMatrix);
               }
               float r[3] = { rotX, rotY, rotZ };
               t2.multiplyPoint(r);
               
               //
               // Create a matrix containing the rotations
               //
               TransformationMatrix t3;
               t3.rotateZ(r[2]);
               t3.rotateX(r[0]);
               t3.rotateY(r[1]);
               
               //
               // Remove translation from axes matrix,
               // Multiply axes matrix by rotation matrix
               // Add translation back to axes matrix
               //
               float tx, ty, tz;
               tm->getTranslation(tx, ty, tz);
               tm->translate(-tx, -ty, -tz);
               tm->preMultiply(t3);
               tm->translate(tx, ty, tz);

               theMainWindow->updateTransformationMatrixEditor(tm);
               updateAllGL();
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         {
/*
            float dt[4] = { me.dx, me.dy, 0.0, 1.0 };
            if (surfRotMatrix != NULL) {
               float dt2[4];
               surfRotMatrix->MultiplyPoint(dt, dt2);
               dt[0] = dt2[0];
               dt[1] = dt2[1];
               dt[2] = dt2[2];
            }
            tm->translate(dt[0], dt[1], dt[2]);
*/
            const double txyz[3] = { me.dx, me.dy, 0.0 };
            tm->translate(txyz, surfRotMatrix);
            theMainWindow->updateTransformationMatrixEditor(tm);
            updateAllGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         {
/*
            float dt[4] = { 0.0, 0.0, -me.dy, 1.0 };
            if (surfRotMatrix != NULL) {
               float dt2[4];
               surfRotMatrix->MultiplyPoint(dt, dt2);
               dt[0] = dt2[0];
               dt[1] = dt2[1];
               dt[2] = dt2[2];
            }
            tm->translate(dt[0], dt[1], dt[2]);
*/
            const double txyz[3] = { 0.0, 0.0, -me.dy };
            tm->translate(txyz, surfRotMatrix);
            theMainWindow->updateTransformationMatrixEditor(tm);
            updateAllGL();
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * mouse mode for move node.
 */
void 
GuiBrainModelOpenGL::mouseMoveNode(const GuiBrainModelOpenGLMouseEvent& me)
{
   static int nodeNumber = -1;
   
   switch(me.event) {
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CLICK:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_PRESS:
         selectBrainModelItem(me.x, me.y, BrainModelOpenGL::SELECTION_MASK_NODE);
         nodeNumber = selectedNode.getItemIndex1();
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_RELEASE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_PRESS:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_MOVE:
         if (nodeNumber >= 0) {
            BrainModelSurface* bms = getDisplayedBrainModelSurface();
            if (bms != NULL) {
               CoordinateFile* cf = bms->getCoordinateFile();
               if (nodeNumber < cf->getNumberOfCoordinates()) {
                  float x, y, z, zorig;
                  cf->getCoordinate(nodeNumber, x, y, zorig);
                  convertWindowToModelCoords(me.x, me.y, false, x, y, z);
                  cf->setCoordinate(nodeNumber, x, y, zorig);
                  updateGL();
               }
            }
         }
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_SHIFT_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_CONTROL_MOVE:
         break;
      case GuiBrainModelOpenGLMouseEvent::MOUSE_LEFT_ALT_MOVE:
         break;
   }
}

/**
 * get the maximum point size.
 */
void 
GuiBrainModelOpenGL::getPointSizeRange(float& minSize, float& maxSize)
{
   minSize = minPointSize;
   maxSize = maxPointSize;
}

/**
 * get the maximum line width.
 */
void 
GuiBrainModelOpenGL::getLineWidthRange(float& minSize, float& maxSize)
{
   minSize = minLineSize;
   maxSize = maxLineSize;
}
       
/**
 * get caption displayed in main window.
 */
QString 
GuiBrainModelOpenGL::getMainWindowCaption()
{
   return openGL->getMainWindowCaption();
}

/**
 * set caption displayed in main window.
 */
void 
GuiBrainModelOpenGL::setMainWindowCaption(const QString& s)
{
   openGL->setMainWindowCaption(s);
}

/**
 * get main window center model coordinate (returns true if valid).
 */
bool 
GuiBrainModelOpenGL::getMainWindowCenterModelCoordinate(float posOut[3])
{
   return allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->convertWindowToModelCoords(
            allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->windowWidth[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW] / 2, 
            allBrainSurfaceOpenGL[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW]->windowHeight[BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW] / 2, 
            true,
            posOut[0], 
            posOut[1],
            posOut[2]);
}
       
/**
 * are viewing an oblique slice in a window.
 */
bool 
GuiBrainModelOpenGL::viewingObliqueSlice()
{
   for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      if (allBrainSurfaceOpenGL[i] != NULL) {
         BrainModelVolume* bmv = allBrainSurfaceOpenGL[i]->getDisplayedBrainModelVolume();
         if (bmv != NULL) {
            switch (bmv->getSelectedAxis(i)) {
               case VolumeFile::VOLUME_AXIS_X:
                  break;
               case VolumeFile::VOLUME_AXIS_Y:
                  break;
               case VolumeFile::VOLUME_AXIS_Z:
                  break;
               case VolumeFile::VOLUME_AXIS_ALL:
                  break;
               case VolumeFile::VOLUME_AXIS_OBLIQUE:
                  break;
               case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                  return true;
                  break;
               case VolumeFile::VOLUME_AXIS_UNKNOWN:
                  break;
            }
         }
      }
   }
   
   return false;
}

/**
 * redraw window.
 */
void 
GuiBrainModelOpenGL::slotRedrawWindow()
{
   updateAllGL(this);
   qApp->processEvents();   
}
