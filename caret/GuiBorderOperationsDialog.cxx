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

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolButton>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiBrainModelSurfaceSelectionComboBox.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderLandmarkIdentification.h"
#include "BrainModelSurfacePaintToBorderConverter.h"
#include "BrainSet.h"
#include "DisplaySettingsBorders.h"
#include "GuiBorderOperationsDialog.h"
#include "GuiColorFileEditorWidget.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowLayersActions.h"
#include "GuiNameSelectionDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiStereotaxicSpaceComboBox.h"
#include "GuiVolumeFileSelectionComboBox.h"
#include "PaintFile.h"
#include "SurfaceShapeFile.h"
#include "VocabularyFile.h"
#include "VolumeFile.h"
#include "WuQMessageBox.h"
#include "global_variables.h"

//============================================================================

/**
 * constructor.
 */
GuiBorderOperationsDialog::GuiBorderOperationsDialog(QWidget* parent)
   : WuQMultiPageDialog(WuQMultiPageDialog::PAGE_CREATION_WHEN_DISPLAYED,
                        2,
                        parent)
{
   setWindowTitle("Border Operations");
        
   //
   // Create and Modify operations
   //
   QToolButton* convertVolumeBordersToFiducialToolButton =
                      createToolButton("VF",
                                       "Convert Volume Borders to Fiducial Borders",
                                       this,
                                       SLOT(slotConvertVolumeToFiducialBorders()));
   QToolButton* createLatLonToolButton = createToolButton("LL", 
                 "Create Lon/Lon Spherical Borders",
                 this,
                 SLOT(slotLatLonSphericalBorders()));   
   QToolButton* movePointsToolButton = createToolButton("MV", 
                             "Move Border Points With Mouse.",
                             this,
                             SLOT(slotMoveBorderPointWithMouse()));
   QToolButton* renameWithMouseToolButton = createToolButton("RN", 
                             "Rename Borders With Mouse.",
                             this,
                             SLOT(slotRenameBordersWithMouse()));
   QToolButton* resampleDisplayedToolButton = createToolButton("RS", 
                             "Resample Displayed Borders.",
                             this,
                             SLOT(slotResampleDisplayedBorders()));
   QToolButton* orientClockwiseToolButton = createToolButton("CW", 
                             "Orient Borders Clockwise.",
                             this,
                             SLOT(slotOrientBordersClockwise()));                 
   QToolButton* reverseDisplayedToolButton = createToolButton("RD", 
                             "Reverse Displayed Borders.",
                             this,
                             SLOT(slotReverseDisplayedBorders()));
   QToolButton* reversePointsToolButton = createToolButton("RB", 
                             "Reverse Border Points With Mouse.",
                             this,
                             SLOT(slotReverseBordersWithMouse()));
   QGroupBox* modifyGroupBox = new QGroupBox("Modify");
   QHBoxLayout* modifyGroupLayout = new QHBoxLayout(modifyGroupBox);
   modifyGroupLayout->addWidget(convertVolumeBordersToFiducialToolButton);
   modifyGroupLayout->addWidget(createLatLonToolButton);
   modifyGroupLayout->addWidget(movePointsToolButton);
   modifyGroupLayout->addWidget(renameWithMouseToolButton);
   modifyGroupLayout->addWidget(resampleDisplayedToolButton);
   modifyGroupLayout->addWidget(orientClockwiseToolButton);
   modifyGroupLayout->addWidget(reverseDisplayedToolButton);
   modifyGroupLayout->addWidget(reversePointsToolButton);
       
   //
   // Delete operations
   //
   QToolButton* deleteAllToolButton = createToolButton(
                             "DA", 
                             "Delete All Borders.",
                             this,
                             SLOT(slotDeleteAllBorders()));
   QToolButton* deleteNotDisplayedToolButton = createToolButton(
                             "DN", 
                             "Delete Borders Not Displayed on Main Window Surface.",
                             this,
                             SLOT(slotDeleteBordersNotDisplayed()));
   QToolButton* deleteWithMouseToolButton = createToolButton(
                             "DB", 
                             "Delete Borders With Mouse.",
                             this,
                             SLOT(slotDeleteBordersWithMouse()));
   QToolButton* deletePointsWithMouseToolButton = createToolButton(
                             "DP", 
                             "Delete Border Points With Mouse.",
                             this,
                             SLOT(slotDeleteBorderPointsWithMouse()));
   QToolButton* deleteOutsideSurfaceToolButton = createToolButton(
                             "DO", 
                             "Delete Border Points Outside Flat Surface.",
                             this,
                             SLOT(slotDeleteBorderPointsOutsideSurface()));
   QGroupBox* deleteGroupBox = new QGroupBox("Delete");
   QHBoxLayout* deleteGroupLayout = new QHBoxLayout(deleteGroupBox);
   deleteGroupLayout->addWidget(deleteAllToolButton);
   deleteGroupLayout->addWidget(deleteNotDisplayedToolButton);
   deleteGroupLayout->addWidget(deleteWithMouseToolButton);
   deleteGroupLayout->addWidget(deletePointsWithMouseToolButton);
   deleteGroupLayout->addWidget(deleteOutsideSurfaceToolButton);

   //
   // Setup Toolbar
   //
   addWidgetToToolBar(0, modifyGroupBox);
   addWidgetToToolBar(0, deleteGroupBox);
   
   addPage("Create Borders From Paint Regions",
           new BordersPageCreateFromPaintRegions);

   addPage("Create Flat Analysis Grid Borders",
           new BordersPageCreateFlatAnalysisGridBorders);
           
   addPage("Create Flat Grid Borders",
           new BordersPageCreateFlatGridBorders);
           
   pageInterpolatedBorders = new BordersPageCreateInterpolatedBorders;
   addPage("Create Interpolated Borders",
           pageInterpolatedBorders);
   
   pageCreateLandmarkBorders = new BordersPageCreateLandmarks;
   addPage("Create Flattening and Registration Landmark Borders",
           pageCreateLandmarkBorders);
           
   pageBordersDrawNew = new BordersPageDrawNew;
   addPage("Draw New Borders",
           pageBordersDrawNew);

   pageBordersDrawUpdate = new BordersPageUpdateExisting;
   addPage("Draw Update Existing Borders",
           pageBordersDrawUpdate);

   //addPage("Edit Attributes",
   //        new BordersPageEditAttributes);
           
   addPage("Edit Colors",
           new BordersPageEditColors);

   addPage("Project Borders",
           new BordersPageProjection);
           
   
   setDefaultPage(pageBordersDrawNew);
}

/**
 * destructor.
 */
GuiBorderOperationsDialog::~GuiBorderOperationsDialog()
{
}

/**
 * show the draw borders page.
 */
void 
GuiBorderOperationsDialog::showPageDrawBorders()
{
   showPage(pageBordersDrawNew);
}

/**
 * show the update borders page.
 */
void 
GuiBorderOperationsDialog::showPageUpdateBorders()
{
   showPage(pageBordersDrawUpdate);
}

/**
 * show the create landmark borders page.
 */
void 
GuiBorderOperationsDialog::showPageCreateLandmarkBorders(
                                   const bool checkCreateFlattenLandmarks,
                                   const bool checkCreateRegistrationLandmarks,
                                   const BrainModelSurface* fiducialSurface,
                                   const BrainModelSurface* inflatedSurface,
                                   const BrainModelSurface* veryInflatedSurface,
                                   const BrainModelSurface* sphericalSurface)
{
   showPage(pageCreateLandmarkBorders);
   pageCreateLandmarkBorders->initializePage(checkCreateFlattenLandmarks,
                                             checkCreateRegistrationLandmarks,
                                             fiducialSurface,
                                             inflatedSurface,
                                             veryInflatedSurface,
                                             sphericalSurface);
}
      
/**
 * called when close push button clicked.
 */
bool 
GuiBorderOperationsDialog::close()
{
   if ((theMainWindow->getBrainModelOpenGL()->getMouseMode() ==
        GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DRAW_NEW) ||
       (theMainWindow->getBrainModelOpenGL()->getMouseMode() ==
        GuiBrainModelOpenGL::MOUSE_MODE_BORDER_UPDATE_NEW)) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
                          GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }

   return WuQMultiPageDialog::close();
}

/**
 * called to convert volume to fiducial borders.
 */
void 
GuiBorderOperationsDialog::slotConvertVolumeToFiducialBorders()
{
   theMainWindow->getLayersActions()->slotBordersVolumeToBordersFiducial();
}
      
/**
 * called to create lat/lon spherical borders.
 */
void 
GuiBorderOperationsDialog::slotLatLonSphericalBorders()
{
   theMainWindow->getLayersActions()->slotBordersCreateSpherical();
}

/**
 * called to move border points with mouse
 */
void  
GuiBorderOperationsDialog::slotMoveBorderPointWithMouse()
{
   theMainWindow->getLayersActions()->slotBordersMovePointWithMouse();
}

/**
 * called to rename borders with mouse
 */
void  
GuiBorderOperationsDialog::slotRenameBordersWithMouse()
{
   theMainWindow->getLayersActions()->slotBordersRenameWithMouse();
}

/**
 * called to reverse borders with mouse
 */
void  
GuiBorderOperationsDialog::slotReverseBordersWithMouse()
{
   theMainWindow->getLayersActions()->slotBordersReverseWithMouse();
}

/**
 * called to orient borders clockwise
 */
void  
GuiBorderOperationsDialog::slotOrientBordersClockwise()
{
   theMainWindow->getLayersActions()->slotBordersOrientDisplayedClockwise();
}

/**
 * called to reverse displayed borders
 */
void  
GuiBorderOperationsDialog::slotReverseDisplayedBorders()
{
   theMainWindow->getLayersActions()->slotBordersReverseDisplayed();
}

/**
 * called to delete all borders
 */
void  
GuiBorderOperationsDialog::slotDeleteAllBorders()
{
   theMainWindow->getLayersActions()->slotBordersDeleteAll();
}

/**
 * called to delete borders not displayed.
 */
void 
GuiBorderOperationsDialog::slotDeleteBordersNotDisplayed()
{
   BrainModel* bm = theMainWindow->getBrainModelSurface();
   if (bm != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->deleteBordersNotDisplayedOnBrainModel(bm);

      GuiFilesModified fm;
      fm.setBorderModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * called to resample borders.
 */
void 
GuiBorderOperationsDialog::slotResampleDisplayedBorders()
{
   theMainWindow->getLayersActions()->slotBordersResampleDisplayed();
}      
      
/**
 * called to delete borders with mouse
 */
void  
GuiBorderOperationsDialog::slotDeleteBordersWithMouse()
{
   theMainWindow->getLayersActions()->slotBordersDeleteWithMouse();
}

/**
 * called to delete border points with mouse
 */
void  
GuiBorderOperationsDialog::slotDeleteBorderPointsWithMouse()
{
   theMainWindow->getLayersActions()->slotBordersDeletePointWithMouse();
}

/**
 * called to delete border points outside surface
 */
void  
GuiBorderOperationsDialog::slotDeleteBorderPointsOutsideSurface()
{
   theMainWindow->getLayersActions()->slotBordersDeletePointsOutsideSurface();
}

/**
 * get border drawing parameters.
 */
void 
GuiBorderOperationsDialog::getBorderDrawingParameters(QString& borderNameOut,
                                   float& samplingOut,
                                   int& borderColorIndexOut,
                                   int& surfacePaintAssignmentColumnNumberOut,
                                   int& surfacePaintNameAssignmentIndexOut,
                                   VolumeFile* &paintVolumeForVoxelAssignmentOut,
                                   int& paintVolumeSliceThicknessOut,
                                   bool& closedBorderFlagOut,
                                   bool& twoDimFlagOut,
                                   bool& autoProjectYesFlagOut) const
{
   pageBordersDrawNew->getBorderDrawingParameters(borderNameOut,
                                            samplingOut,
                                            borderColorIndexOut,
                                            surfacePaintAssignmentColumnNumberOut,
                                            surfacePaintNameAssignmentIndexOut,
                                            paintVolumeForVoxelAssignmentOut,
                                            paintVolumeSliceThicknessOut,
                                            closedBorderFlagOut,
                                            twoDimFlagOut,
                                            autoProjectYesFlagOut);
}
                                      
/**
 * get border update parameters.
 */
void 
GuiBorderOperationsDialog::getBorderUpdateParameters(
                   BrainModelBorderSet::UPDATE_BORDER_MODE& borderUpdateModeOut,
                   float& samplingOut,
                   bool& twoDimFlagOut,
                   bool& autoProjectYesFlagOut) const
{
   pageBordersDrawUpdate->getBorderUpdateParameters(
                   borderUpdateModeOut,
                   samplingOut,
                   twoDimFlagOut,
                   autoProjectYesFlagOut);
}

/**
 * set selected interpolated borders.
 */
void 
GuiBorderOperationsDialog::setSelectedInterpolatedBorders(const int borderNumber,
                                                          const int borderIndex)
{
   pageInterpolatedBorders->setSelectedBorders(borderNumber, borderIndex);
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageDrawNew::BordersPageDrawNew()
{
}
                   
/**
 * destructor.
 */
BordersPageDrawNew::~BordersPageDrawNew()
{
}

/**
 * apply the pages settings.
 */
void 
BordersPageDrawNew::applyPage()
{
   theMainWindow->getBrainModelOpenGL()->resetLinearObjectBeingDrawn();
   
   const QString borderName = nameLineEdit->text().trimmed();
   if (borderName.isEmpty()) {
      WuQMessageBox::critical(this, "ERROR", "Border name is missing.");
      return;
   }

   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();

   if (bms != NULL) {
      //
      // Make sure a flat or compressed medial wall surface is not rotated
      //
      if (dimension2DRadioButton->isChecked()) {
         bool haveFlatSurface = false;
         switch (bms->getSurfaceType()) {
            case BrainModelSurface::SURFACE_TYPE_RAW:
            case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
            case BrainModelSurface::SURFACE_TYPE_INFLATED:
            case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
            case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
            case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
               break;
            case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
            case BrainModelSurface::SURFACE_TYPE_FLAT:
            case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
               haveFlatSurface = true;
               break;
            case BrainModelSurface::SURFACE_TYPE_HULL:
            case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
            case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
               break;
         }
      
         if (haveFlatSurface) {
            float matrix[16];
            bms->getRotationMatrix(0, matrix);
            
            if ((matrix[0] != 1.0) || (matrix[5] != 1.0) || (matrix[10] != 1.0)) {
               if (QMessageBox::warning(this, "WARNING",
                       "The flat surface appears to be rotated which will\n"
                       "prevent the border from being drawn correctly.\n"
                       "Would you like to remove the rotation?",
                       (QMessageBox::Yes | QMessageBox::No),
                       QMessageBox::Yes)
                          == QMessageBox::Yes) {
                  //
                  // Setting the view to dorsal resets the rotation matrix without
                  // affecting the translate and scaling.
                  //
                  bms->setToStandardView(0, BrainModelSurface::VIEW_DORSAL);
                  GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
               }
            }
         }
      }
   }

   //
   // Find the matching color
   //
   bool borderColorMatch = false;
   BorderColorFile* borderColorFile = theMainWindow->getBrainSet()->getBorderColorFile();
   borderColorIndex = borderColorFile->getColorIndexByName(borderName, borderColorMatch);
   
   //
   // Border color may need to be created
   //
   bool createBorderColor = false;
   if ((borderColorIndex >= 0) && (borderColorMatch == true)) {
      createBorderColor = false;
   }
   else if ((borderColorIndex >= 0) && (borderColorMatch == false)) {
      QString msg("Use border color \"");
      msg.append(borderColorFile->getColorNameByIndex(borderColorIndex));
      msg.append("\" for border ");
      msg.append(borderName);
      msg.append(" ?");
      QString noButton("No, define color ");
      noButton.append(borderName);

      QMessageBox msgBox(this);
      msgBox.setWindowTitle("Use Partially Matching Color");
      msgBox.setText(msg);
      QPushButton* yesPushButton = msgBox.addButton("Yes", 
                                                    QMessageBox::ActionRole);
      QPushButton* noPushButton = msgBox.addButton(noButton, 
                                                    QMessageBox::ActionRole);
      msgBox.exec();
      if (msgBox.clickedButton() == yesPushButton) {
         createBorderColor = false;
      }
      else if (msgBox.clickedButton() == noPushButton) {
         createBorderColor = true;
      }
   }
   else {
      createBorderColor = true;
   }
   
   if (createBorderColor) {
      QString title("Create Border Color: ");
      title.append(borderName);
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this, 
                                                                 title,
                                                                 false,
                                                                 false,
                                                                 false,
                                                                 false);
      csd->exec();
      
      //
      // Add new border color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      borderColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);
      borderColorIndex = borderColorFile->getNumberOfColors() - 1;
   }
   
   bool doAreaColorFlag = false;
   
   //
   // Initialize do not assign paint
   //
   paintAssignmentColumnNumber = -1;
   paintNameAssignmentIndex = -1;
   
   //
   // Is there a surface in the main window and assigning nodes
   //
   if ((bms != NULL) &&
       typeClosedRadioButton->isChecked() &&
       assignNodesInsideBorderGroupBox->isChecked()) {
      doAreaColorFlag = true;
      
      //
      // Get column number for assignment
      //
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      paintAssignmentColumnNumber = paintColumnComboBox->currentIndex();
      
      //
      // Does column need to be created?
      //
      if ((paintAssignmentColumnNumber < 0) ||
          (paintAssignmentColumnNumber >= pf->getNumberOfColumns()) ||
          (paintColumnComboBox->getNewSelected())) {
         pf->addColumns(1);
         paintAssignmentColumnNumber = pf->getNumberOfColumns() - 1;
         paintColumnComboBox->setCurrentIndex(paintAssignmentColumnNumber);
      }

      //
      // Set name of column
      //
      const QString columnName = paintColumnNameLineEdit->text();
      pf->setColumnName(paintAssignmentColumnNumber, 
                        columnName);
      
      //
      // paint index for node assignment
      //
      paintNameAssignmentIndex = pf->addPaintName(borderName);
   }
   
   //
   // If there is a volume in the main window and assigning voxels
   //
   paintVolumeForVoxelAssignment = NULL;
   if ((bmv != NULL) &&
       typeClosedRadioButton->isChecked() &&
       assignVoxelsInsideBorderGroupBox->isChecked()) {
      paintVolumeForVoxelAssignment = paintVolumeSelectionControl->getSelectedVolumeFile();
      if (paintVolumeForVoxelAssignment != NULL) {
         doAreaColorFlag = true;
      }
   }
   
   if (doAreaColorFlag) {
      //
      // Find the matching color
      //
      bool areaColorMatch = false;
      AreaColorFile* areaColorFile = theMainWindow->getBrainSet()->getAreaColorFile();
      const int areaColorIndex = areaColorFile->getColorIndexByName(borderName, areaColorMatch);
      
      //
      // Area color may need to be created
      //
      if (areaColorMatch == false) {
         QString msg("Matching area color \"");
         msg.append(borderName);
         msg.append("\" not found");
         QString borderButton("Use Border Color");
         QString defineButton("Define Area Color ");
         int result = -1;
         if (areaColorIndex >= 0) {
            QString partialMatchButton("Use ");
            partialMatchButton.append(areaColorFile->getColorNameByIndex(areaColorIndex));
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Set Area Color");
            msgBox.setText(msg);
            QPushButton* useColorPushButton = msgBox.addButton(borderButton, 
                                                          QMessageBox::ActionRole);
            QPushButton* defineColorPushButton = msgBox.addButton(defineButton, 
                                                          QMessageBox::ActionRole);
            QPushButton* usePartialColorPushButton = msgBox.addButton(partialMatchButton, 
                                                          QMessageBox::ActionRole);
            msgBox.exec();
            if (msgBox.clickedButton() == useColorPushButton) {
               result = 0;
            }
            else if (msgBox.clickedButton() == defineColorPushButton) {
               result = 1;
            }
            else if (msgBox.clickedButton() == usePartialColorPushButton) {
               result = 2;
            }
         }
         else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Set Area Color");
            msgBox.setText(msg);
            QPushButton* useColorPushButton = msgBox.addButton(borderButton, 
                                                          QMessageBox::ActionRole);
            QPushButton* defineColorPushButton = msgBox.addButton(defineButton, 
                                                          QMessageBox::ActionRole);
            msgBox.exec();
            if (msgBox.clickedButton() == useColorPushButton) {
               result = 0;
            }
            else if (msgBox.clickedButton() == defineColorPushButton) {
               result = 1;
            }
         }
         
         if (result == 0) {
            //
            // Copy border color to area color
            //
            unsigned char r, g, b, a;
            borderColorFile->getColorByIndex(borderColorIndex, r, g, b, a);
            float pointSize, lineSize;
            borderColorFile->getPointLineSizeByIndex(borderColorIndex, pointSize, lineSize);
            areaColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize);
         }
         else if (result == 1) {
            //
            // define the area color
            //
            QString title("Create Area Color: ");
            title.append(borderName);
            GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this,
                                                                       title,
                                                                       false,
                                                                       false, 
                                                                       false,
                                                                       false);
            csd->exec();
            
            //
            // Add new area color
            //
            float pointSize = 2.0, lineSize = 1.0;
            unsigned char r, g, b, a;
            ColorFile::ColorStorage::SYMBOL symbol;
            csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
            areaColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);            
         }
         else if (result == 2) {
            //
            // do nothing so that partially matching color is used
            //
         }
      }
   }

   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                      GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DRAW_NEW);
}

/**
 * create the page.
 */
QWidget* 
BordersPageDrawNew::createPage()
{
   //
   // Name
   //
   QPushButton* namePushButton = new QPushButton("Name...");
   QObject::connect(namePushButton, SIGNAL(clicked()),
                    this, SLOT(slotNamePushButton()));
   nameLineEdit = new QLineEdit;
   
   //
   // Sampling 
   //
   QLabel* samplingLabel = new QLabel("Sampling (mm)");
   samplingDoubleSpinBox = new QDoubleSpinBox;
   samplingDoubleSpinBox->setMinimum(0.01);
   samplingDoubleSpinBox->setMaximum(9999999.0);
   samplingDoubleSpinBox->setDecimals(2);
   samplingDoubleSpinBox->setSingleStep(0.5);
   samplingDoubleSpinBox->setValue(2.0);
   
   //
   // Type
   //
   typeClosedRadioButton = new QRadioButton("Closed (Boundary)");
   typeOpenRadioButton = new QRadioButton("Open");
   QButtonGroup* typeButtGroup = new QButtonGroup(this);
   typeButtGroup->addButton(typeClosedRadioButton);
   typeButtGroup->addButton(typeOpenRadioButton);
   QObject::connect(typeButtGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotTypeChanged()));
   QGroupBox* typeGroupBox = new QGroupBox("Type");
   QVBoxLayout* typeLayout = new QVBoxLayout(typeGroupBox);
   typeLayout->addWidget(typeClosedRadioButton);
   typeLayout->addWidget(typeOpenRadioButton);
   typeOpenRadioButton->setChecked(true);
   
   //
   // Dimension
   //
   dimension2DRadioButton = new QRadioButton("2D (Flat)");
   dimension2DRadioButton->setToolTip("Draw border on a flat surface.");
   dimension2DAutoProjectRadioButton = new QRadioButton("2D (Flat, Auto Project)");
   dimension2DAutoProjectRadioButton->setToolTip("Draw border on a flat surface and\n"
                                                 "project border to all other surfaces.");
   dimension3DRadioButton = new QRadioButton("3D");
   dimension3DRadioButton->setToolTip("Draw border on any surface and\n"
                                      "project to all other surfaces.");
   QButtonGroup* dimButtGroup = new QButtonGroup(this);
   dimButtGroup->addButton(dimension2DRadioButton);
   dimButtGroup->addButton(dimension2DAutoProjectRadioButton);
   dimButtGroup->addButton(dimension3DRadioButton);
   QGroupBox* dimGroupBox = new QGroupBox("Dimensions");
   QVBoxLayout* dimLayout = new QVBoxLayout(dimGroupBox);
   dimLayout->addWidget(dimension2DRadioButton);
   dimLayout->addWidget(dimension2DAutoProjectRadioButton);
   dimLayout->addWidget(dimension3DRadioButton);
   dimension3DRadioButton->setChecked(true);
      
   //
   // Layout for name and sampling
   //
   QGridLayout* nameSamplingGridLayout = new QGridLayout;
   nameSamplingGridLayout->addWidget(namePushButton, 0, 0);
   nameSamplingGridLayout->addWidget(nameLineEdit, 0, 1, 1, 2);
   nameSamplingGridLayout->addWidget(samplingLabel, 1, 0);
   nameSamplingGridLayout->addWidget(samplingDoubleSpinBox, 1, 1);
   
   //
   // Layout for type/dim/project
   //
   QHBoxLayout* typeDimProjectLayout = new QHBoxLayout;
   typeDimProjectLayout->addWidget(typeGroupBox);
   typeDimProjectLayout->addWidget(dimGroupBox);
   typeDimProjectLayout->addStretch();

   //
   // Node/Voxel Assignment
   //
   nodeVoxelAssignmentWidget = createClosedBorderNodeVoxelAssignmentWidget();
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(nameSamplingGridLayout);
   layout->addLayout(typeDimProjectLayout);
   layout->addWidget(nodeVoxelAssignmentWidget);
   
   slotTypeChanged();
   
   return w;
}

/**
 * called when type changed.
 */
void 
BordersPageDrawNew::slotTypeChanged()
{
   nodeVoxelAssignmentWidget->setEnabled(typeClosedRadioButton->isChecked());
}
      
/**
 * create the closed border assignment widget.
 */
QWidget* 
BordersPageDrawNew::createClosedBorderNodeVoxelAssignmentWidget()      
{
   //
   // Paint Column Selection
   //
   QLabel* paintColumnLabel = new QLabel("Paint Column");
   paintColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
      theMainWindow->getBrainSet()->getPaintFile(),
      true,
      false,
      false);
      
   //
   // Name of paint column
   //
   QLabel* paintColumnNameLabel = new QLabel("Column Name");
   paintColumnNameLineEdit = new QLineEdit;

   //
   // Connect paint column selection to column name line edit
   //
   QObject::connect(paintColumnComboBox, SIGNAL(itemNameSelected(const QString&)),
                    paintColumnNameLineEdit, SLOT(setText(const QString&)));
   
   //
   // Widget for node assignment
   //
   QWidget* nodeAssignmentWidget = new QWidget;
   QGridLayout* gridLayout = new QGridLayout(nodeAssignmentWidget);
   gridLayout->addWidget(paintColumnLabel, 1, 0);
   gridLayout->addWidget(paintColumnComboBox, 1, 1);
   gridLayout->addWidget(paintColumnNameLabel, 2, 0);
   gridLayout->addWidget(paintColumnNameLineEdit, 2, 1);
   
   //
   // Assign nodes group box
   //
   assignNodesInsideBorderGroupBox = 
      new QGroupBox("Assign Paint Identifier to Nodes Inside Border");
   assignNodesInsideBorderGroupBox->setCheckable(true);
   assignNodesInsideBorderGroupBox->setChecked(false);
   QVBoxLayout* assignNodesLayout = new QVBoxLayout(assignNodesInsideBorderGroupBox);
   assignNodesLayout->addWidget(nodeAssignmentWidget);
   QObject::connect(assignNodesInsideBorderGroupBox, SIGNAL(toggled(bool)),
                    nodeAssignmentWidget, SLOT(setEnabled(bool)));
                    
   //-----------------------------------------------------------------
   
   //
   // assign voxels paint volume
   //
   QLabel* paintVolumeLabel = new QLabel("Paint Volume");
   paintVolumeSelectionControl = new GuiVolumeFileSelectionComboBox(
                                        VolumeFile::VOLUME_TYPE_PAINT);

   //
   // Thickness selection
   //
   QLabel* ThicknessLabel = new QLabel("Thickness");
   paintVolumeSliceThicknessComboBox = new QComboBox;
   paintVolumeSliceThicknessComboBox->addItem("Current Slice");
   paintVolumeSliceThicknessComboBox->addItem("+/- 1 Slice");
   paintVolumeSliceThicknessComboBox->addItem("+/- 2 Slices");
   paintVolumeSliceThicknessComboBox->addItem("+/- 3 Slices");
   paintVolumeSliceThicknessComboBox->addItem("+/- 4 Slices");
   paintVolumeSliceThicknessComboBox->addItem("+/- 5 Slices");
   
   //
   // Assign voxels group box
   //
   assignVoxelsInsideBorderGroupBox =
      new QGroupBox("Assign Paint Identifier to Voxels Inside Border");
   assignVoxelsInsideBorderGroupBox->setCheckable(true);
   assignVoxelsInsideBorderGroupBox->setChecked(false);
   QGridLayout* assignVoxelsLayout = new QGridLayout(assignVoxelsInsideBorderGroupBox);
   assignVoxelsLayout->addWidget(paintVolumeLabel, 0, 0);
   assignVoxelsLayout->addWidget(paintVolumeSelectionControl, 0, 1);
   assignVoxelsLayout->addWidget(ThicknessLabel, 1, 0);
   assignVoxelsLayout->addWidget(paintVolumeSliceThicknessComboBox, 1, 1);
   
   //
   // Tab widget for node and voxel assignment
   //
   QTabWidget* assignNodeVoxelTabWidget = new QTabWidget;
   assignNodeVoxelTabWidget->addTab(assignNodesInsideBorderGroupBox, 
                                    "Assign Nodes");
   assignNodeVoxelTabWidget->addTab(assignVoxelsInsideBorderGroupBox, 
                                    "Assign Voxels");
   
   return assignNodeVoxelTabWidget;
}

/**
 * called when name push button clicked.
 */
void 
BordersPageDrawNew::slotNamePushButton()
{
   static GuiNameSelectionDialog::LIST_ITEMS_TYPE itemForDisplay = 
                   GuiNameSelectionDialog::LIST_BORDER_COLORS_ALPHA;
           
   GuiNameSelectionDialog nsd(this, 
                              GuiNameSelectionDialog::LIST_ALL,
                              itemForDisplay);
   if (nsd.exec() == QDialog::Accepted) {
      itemForDisplay = nsd.getSelectedItemType();
      QString name(nsd.getNameSelected());
      if (name.isEmpty() == false) {
         nameLineEdit->setText(name);
         
         if (name.toUpper() == "MEDIAL.WALL") {
            typeClosedRadioButton->setChecked(true);
         }
         else if ((name.toLower().endsWith("cut")) ||
                  (name.indexOf("LANDMARK") >= 0)) {
            typeOpenRadioButton->setChecked(true);
         }
      }
   }
}
      
/**
 * update the page.
 */
void 
BordersPageDrawNew::updatePage()
{
   paintColumnComboBox->updateComboBox();
   paintColumnNameLineEdit->setText(paintColumnComboBox->getCurrentLabel());
   
}

/**
 * see if the page is valid.
 */
bool 
BordersPageDrawNew::getPageValid() const
{
   return true;
}

/**
 * get border drawing parameters.
 */
void 
BordersPageDrawNew::getBorderDrawingParameters(QString& borderNameOut,
                                   float& samplingOut,
                                   int& borderColorIndexOut,
                                   int& surfacePaintAssignmentColumnNumberOut,
                                   int& surfacePaintNameAssignmentIndexOut,
                                   VolumeFile* &paintVolumeForVoxelAssignmentOut,
                                   int& paintVolumeSliceThicknessOut,
                                   bool& closedBorderFlagOut,
                                   bool& twoDimFlagOut,
                                   bool& autoProjectYesFlagOut) const
{
   borderNameOut = nameLineEdit->text();
   borderColorIndexOut = borderColorIndex;
   samplingOut = samplingDoubleSpinBox->value();
   closedBorderFlagOut = typeClosedRadioButton->isChecked();
   twoDimFlagOut = (dimension2DRadioButton->isChecked() ||
                    dimension2DAutoProjectRadioButton->isChecked());
   autoProjectYesFlagOut = (dimension2DAutoProjectRadioButton->isChecked()
                            || dimension3DRadioButton->isChecked());
   surfacePaintAssignmentColumnNumberOut = paintAssignmentColumnNumber;
   surfacePaintNameAssignmentIndexOut = paintNameAssignmentIndex;
   
   paintVolumeForVoxelAssignmentOut = paintVolumeForVoxelAssignment;
   paintVolumeSliceThicknessOut = paintVolumeSliceThicknessComboBox->currentIndex();
}                                      

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageUpdateExisting::BordersPageUpdateExisting()
{
}
                   
/**
 * destructor.
 */
BordersPageUpdateExisting::~BordersPageUpdateExisting()
{
}

/**
 * apply the pages settings.
 */
void 
BordersPageUpdateExisting::applyPage()
{
   
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                      GuiBrainModelOpenGL::MOUSE_MODE_BORDER_UPDATE_NEW);
}

/**
 * create the page.
 */
QWidget* 
BordersPageUpdateExisting::createPage()
{
   //-------------------------------------------------------------------------
   //
   // Mode radio buttons
   //
   extendBorderModeRadioButton = new QRadioButton("Extend From Either End");
   extendBorderModeRadioButton->setToolTip("Extend a border by starting near\n"
                                           "one end of the border and continuing\n"
                                           "to the border's new end.");
   replaceSegmentInBorderModeRadioButton = new QRadioButton("Replace Segment");
   replaceSegmentInBorderModeRadioButton->setToolTip("Correct a border by starting over\n"
                                                     "a point in the border, drawing the\n"
                                                     "correct new segment, and concluding\n"
                                                     "over a point in the border.");
   eraseBorderModeRadioButton = new QRadioButton("Erase Segment From End");
   eraseBorderModeRadioButton->setToolTip("Remove the end from a border by\n"
                                          "starting at an end of the border\n"
                                          "and continue along the border to\n"
                                          "what should be the new end point.");

   //
   // Button Group to keep mode radio buttons mutually exclusive
   //
   QButtonGroup* modeButtGroup = new QButtonGroup(this);
   modeButtGroup->addButton(eraseBorderModeRadioButton);
   modeButtGroup->addButton(extendBorderModeRadioButton);
   modeButtGroup->addButton(replaceSegmentInBorderModeRadioButton);
   replaceSegmentInBorderModeRadioButton->setChecked(true);
   
   //
   // Group box and layout for mode
   //
   QGroupBox* modeGroupBox = new QGroupBox("Mode");
   QVBoxLayout* modeLayout = new QVBoxLayout(modeGroupBox);
   modeLayout->addWidget(eraseBorderModeRadioButton);
   modeLayout->addWidget(extendBorderModeRadioButton);
   modeLayout->addWidget(replaceSegmentInBorderModeRadioButton);
   
   //
   // Sampling 
   //
   QLabel* samplingLabel = new QLabel("Sampling (mm)");
   samplingDoubleSpinBox = new QDoubleSpinBox;
   samplingDoubleSpinBox->setMinimum(0.01);
   samplingDoubleSpinBox->setMaximum(9999999.0);
   samplingDoubleSpinBox->setDecimals(2);
   samplingDoubleSpinBox->setSingleStep(0.5);
   samplingDoubleSpinBox->setValue(2.0);
   QGroupBox* samplingGroupBox = new QGroupBox("Misc");
   QHBoxLayout* samplingLayout = new QHBoxLayout(samplingGroupBox);
   samplingLayout->addWidget(samplingLabel);
   samplingLayout->addWidget(samplingDoubleSpinBox);
   
   //
   // Dimension
   //
   dimension2DRadioButton = new QRadioButton("2D (Flat)");
   dimension2DRadioButton->setToolTip("Update a flat surface border");
   dimension2DAutoProjectRadioButton = new QRadioButton("2D (Flat, Auto Project)");
   dimension2DAutoProjectRadioButton->setToolTip("Update flat surface border and\n"
                                                 "project to all other surfaces.");
   dimension3DRadioButton = new QRadioButton("3D");
   dimension3DRadioButton->setToolTip("Update 3D border and project\n"
                                      "to all other surfaces.");
   QButtonGroup* dimButtGroup = new QButtonGroup(this);
   dimButtGroup->addButton(dimension2DRadioButton);
   dimButtGroup->addButton(dimension2DAutoProjectRadioButton);
   dimButtGroup->addButton(dimension3DRadioButton);
   QGroupBox* dimGroupBox = new QGroupBox("Dimensions");
   QVBoxLayout* dimLayout = new QVBoxLayout(dimGroupBox);
   dimLayout->addWidget(dimension2DRadioButton);
   dimLayout->addWidget(dimension2DAutoProjectRadioButton);
   dimLayout->addWidget(dimension3DRadioButton);
   dimension3DRadioButton->setChecked(true);

   //
   // Layout for dimensions, auto project, and sampling
   //
   QHBoxLayout* row2Layout = new QHBoxLayout;
   row2Layout->addWidget(samplingGroupBox);
   row2Layout->addWidget(dimGroupBox);
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(modeGroupBox);
   layout->addLayout(row2Layout);
   
   return w;
}
                        
/**
 * get border update parameters.
 */
void 
BordersPageUpdateExisting::getBorderUpdateParameters(
                     BrainModelBorderSet::UPDATE_BORDER_MODE& borderUpdateModeOut,
                                float& samplingOut,
                                bool& twoDimFlagOut,
                                bool& autoProjectYesFlagOut) const
{
   borderUpdateModeOut =
      BrainModelBorderSet::UPDATE_BORDER_MODE_NONE;
      
   if (replaceSegmentInBorderModeRadioButton->isChecked()) {
      borderUpdateModeOut = BrainModelBorderSet::UPDATE_BORDER_MODE_REPLACE_SEGMENT_IN_MIDDLE_OF_BORDER;
   }
   else if (eraseBorderModeRadioButton->isChecked()) {
      borderUpdateModeOut = BrainModelBorderSet::UPDATE_BORDER_MODE_ERASE;
   }
   else if (extendBorderModeRadioButton->isChecked()) {
      borderUpdateModeOut = BrainModelBorderSet::UPDATE_BORDER_MODE_EXTEND_BORDER_FROM_END;
   }
   
   samplingOut = samplingDoubleSpinBox->value();
   twoDimFlagOut = dimension2DRadioButton->isChecked();
   autoProjectYesFlagOut = (dimension2DAutoProjectRadioButton->isChecked()
                            || dimension3DRadioButton->isChecked());
}

/**
 * update the page.
 */
void 
BordersPageUpdateExisting::updatePage()
{
}

/**
 * see if the page is valid.
 */
bool 
BordersPageUpdateExisting::getPageValid() const
{
   return true; //(theMainWindow->getBrainSet()->getBorderSet()->getNumberOfBorders() > 0);
}


//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageProjection::BordersPageProjection()
{
}
                   
/**
 * destructor.
 */
BordersPageProjection::~BordersPageProjection()
{
}

/**
 * apply the pages settings.
 */
void 
BordersPageProjection::applyPage()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   BrainModelSurface* bms = openGL->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      //
      // Project borders used by this surface
      //
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->projectBorders(bms, nearestTileRadioButton->isChecked());

      //
      // Notify that borders have been changed.
      //
      GuiFilesModified fm;
      fm.setBorderModified();
      theMainWindow->fileModificationUpdate(fm);

      //
      // Update all displayed surfaces
      //
      GuiBrainModelOpenGL::updateAllGL(NULL);
   }

   QApplication::restoreOverrideCursor();
}

/**
 * create the page.
 */
QWidget* 
BordersPageProjection::createPage()
{
   nearestNodeRadioButton = new QRadioButton("Nearest Node");
   nearestTileRadioButton = new QRadioButton("Nearest Tile");

   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(nearestNodeRadioButton);
   buttGroup->addButton(nearestTileRadioButton);
   nearestTileRadioButton->setChecked(true);
   
   QGroupBox* projGroupBox = new QGroupBox("Projection Method");
   QVBoxLayout* projGroupLayout = new QVBoxLayout(projGroupBox);
   projGroupLayout->addWidget(nearestNodeRadioButton);
   projGroupLayout->addWidget(nearestTileRadioButton);
   projGroupLayout->addStretch();
   
   return projGroupBox;
}
                        
/**
 * update the page.
 */
void 
BordersPageProjection::updatePage()
{
}

/**
 * see if the page is valid.
 */
bool 
BordersPageProjection::getPageValid() const
{
   return true; //(theMainWindow->getBrainSet()->getBorderSet()->getNumberOfBorders() > 0);
}



//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageEditColors::BordersPageEditColors()
{
   colorEditorWidget = NULL;
}
                   
/**
 * destructor.
 */
BordersPageEditColors::~BordersPageEditColors()
{
}

/**
 * apply the pages settings.
 */
void 
BordersPageEditColors::applyPage()
{
   colorEditorWidget->applySlot();
}

/**
 * create the page.
 */
QWidget* 
BordersPageEditColors::createPage()
{
   colorEditorWidget = 
      new GuiColorFileEditorWidget(NULL,
                                   theMainWindow->getBrainSet()->getBorderColorFile(),
                                   true,
                                   true,
                                   false,
                                   false);
   QObject::connect(colorEditorWidget, SIGNAL(redrawRequested()),
                    this, SLOT(slotColorsChanged()));
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(colorEditorWidget);
   layout->addStretch();
   
   return colorEditorWidget;
}
                        
/**
 * called if colors changed.
 */
void 
BordersPageEditColors::slotColorsChanged()
{
   GuiFilesModified fm;
   fm.setBorderColorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * update the page.
 */
void 
BordersPageEditColors::updatePage()
{
   if (colorEditorWidget != NULL) {
      colorEditorWidget->updateColorEditor();
   }
}

/**
 * see if the page is valid.
 */
bool 
BordersPageEditColors::getPageValid() const
{
   return true;
}



//============================================================================
//============================================================================
//============================================================================
/**
 * constructor.
 */
BordersPageCreateFromPaintRegions::BordersPageCreateFromPaintRegions()
{
   paintColumnComboBox = NULL;
}
                   
/**
 * destructor.
 */
BordersPageCreateFromPaintRegions::~BordersPageCreateFromPaintRegions()
{
}

/**
 * apply the pages settings.
 */
void
BordersPageCreateFromPaintRegions::applyPage()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      BrainSet* bs = theMainWindow->getBrainSet();
      const int columnNumber = paintColumnComboBox->currentIndex();
      PaintFile* pf = bs->getPaintFile();
      if ((columnNumber >= 0) &&
          (columnNumber < pf->getNumberOfColumns())) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         BrainModelSurfacePaintToBorderConverter ptb(bs,
                                                     bms,
                                                     pf,
                                                     columnNumber);
         try {
            ptb.execute();
         }
         catch (BrainModelAlgorithmException& e) {
            QApplication::restoreOverrideCursor();
            WuQMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
         }
         
         DisplaySettingsBorders* dsb = bs->getDisplaySettingsBorders();
         dsb->setDisplayBorders(true);
         GuiFilesModified fm;
         fm.setBorderModified();
         fm.setBorderColorModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * create the page.
 */
QWidget* 
BordersPageCreateFromPaintRegions::createPage()
{
   QLabel* paintColumnLabel = new QLabel("Paint Column");
   paintColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                          theMainWindow->getBrainSet()->getPaintFile(),
                                          false,
                                          false,
                                          false);
   QObject::connect(paintColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotPaintColumnComboBox(int)));
                    
   QLabel* borderNameLabel = new QLabel("Border Name");
   borderNameLineEdit = new QLineEdit;
   
   //
   // Layout
   //
   QWidget* w = new QWidget;
   QGridLayout* gridLayout = new QGridLayout(w);
   gridLayout->addWidget(paintColumnLabel, 0, 0);
   gridLayout->addWidget(paintColumnComboBox, 0, 1);
   gridLayout->addWidget(borderNameLabel, 1, 0);
   gridLayout->addWidget(borderNameLineEdit, 1, 1);
   
   return w;
   
}
                        
/**
 * called when paint column selection made.
 */
void 
BordersPageCreateFromPaintRegions::slotPaintColumnComboBox(int item)
{
   const PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   if ((item >= 0) &&
       (item < pf->getNumberOfColumns())) {
      borderNameLineEdit->setText(pf->getColumnName(item));
   }
}

/**
 * update the page.
 */
void 
BordersPageCreateFromPaintRegions::updatePage()
{
   if (paintColumnComboBox != NULL) {
      paintColumnComboBox->updateComboBox();
      if (borderNameLineEdit->text().isEmpty()) {
         borderNameLineEdit->setText(paintColumnComboBox->currentText());
      }
   }
}

/**
 * see if the page is valid.
 */
bool 
BordersPageCreateFromPaintRegions::getPageValid() const
{
/*
   const bool valid =
      (theMainWindow->getBrainSet()->getPaintFile()->getNumberOfColumns() > 0);
   return valid;
*/
   return true;
}


//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageCreateFlatGridBorders::BordersPageCreateFlatGridBorders()
{
}
                   
/**
 * destructor.
 */
BordersPageCreateFlatGridBorders::~BordersPageCreateFlatGridBorders()
{
}

/**
 * apply the pages settings.
 */
void 
BordersPageCreateFlatGridBorders::applyPage()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      //
      // See if a flat surface is in the main window
      //
      if (bms->getIsFlatSurface() == false) {
         WuQMessageBox::critical(theMainWindow, "Surface Type", 
                          "The surface in the main window must be flat for this operation !!!");
         return;
      }
 
      BorderFile borderFile;
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      bms->createFlatGridBorders(borderFile, 
                                 gridSpacingDoubleSpinBox->value(), 
                                 pointsPerGridSquareSpinBox->value());

      theMainWindow->getBrainSet()->deleteAllBorders();
      
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->copyBordersFromBorderFile(bms, &borderFile);
            
      if (restrictPointsToSurfaceCheckBox->isChecked()) {
         theMainWindow->getLayersActions()->slotBordersDeletePointsOutsideSurface();
      }
      
      BorderColorFile* colorFile = theMainWindow->getBrainSet()->getBorderColorFile();
      colorFile->addColorIfItDoesNotExist("GridX", 255, 0, 0);
      colorFile->addColorIfItDoesNotExist("GridY", 0, 255, 0);
      theMainWindow->getBrainSet()->assignBorderColors();
      
      DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
      dsb->setDisplayBorders(true);
      
      GuiFilesModified fm;
      fm.setBorderModified();
      fm.setBorderColorModified();
      theMainWindow->fileModificationUpdate(fm);
      
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
   }
}

/**
 * create the page.
 */
QWidget* 
BordersPageCreateFlatGridBorders::createPage()
{
   //
   // Grid spacing
   //
   QLabel* gridSpacingLabel = new QLabel("Grid Spacing (mm)");
   gridSpacingDoubleSpinBox = new QDoubleSpinBox;
   gridSpacingDoubleSpinBox->setMinimum(0.01);
   gridSpacingDoubleSpinBox->setMaximum(500000.0);
   gridSpacingDoubleSpinBox->setSingleStep(1);
   gridSpacingDoubleSpinBox->setDecimals(2);
   gridSpacingDoubleSpinBox->setValue(50.0);
   
   //
   // Points per grid square
   //
   QLabel* pointsPerGridSquareLabel = new QLabel("Point Per Grid Square");
   pointsPerGridSquareSpinBox = new QSpinBox;
   pointsPerGridSquareSpinBox->setMinimum(0);
   pointsPerGridSquareSpinBox->setMaximum(50000);
   pointsPerGridSquareSpinBox->setSingleStep(1);
   pointsPerGridSquareSpinBox->setValue(5);
   
   //
   // Restrict points to surface check box
   //
   restrictPointsToSurfaceCheckBox = new QCheckBox("Restrict Points to Surface");
   restrictPointsToSurfaceCheckBox->setChecked(true);
   
   QWidget* w = new QWidget;
   QGridLayout* layout = new QGridLayout(w);
   layout->addWidget(gridSpacingLabel, 0, 0);
   layout->addWidget(gridSpacingDoubleSpinBox, 0, 1);
   layout->addWidget(pointsPerGridSquareLabel, 1, 0);
   layout->addWidget(pointsPerGridSquareSpinBox, 1, 1);
   layout->addWidget(restrictPointsToSurfaceCheckBox, 2, 0, 1, 2);
   
   return w;
}
                        
/**
 * update the page.
 */
void 
BordersPageCreateFlatGridBorders::updatePage()
{
}

/**
 * see if the page is valid.
 */
bool 
BordersPageCreateFlatGridBorders::getPageValid() const
{
   return true;
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageCreateFlatAnalysisGridBorders::BordersPageCreateFlatAnalysisGridBorders()
{
}
                   
/**
 * destructor.
 */
BordersPageCreateFlatAnalysisGridBorders::~BordersPageCreateFlatAnalysisGridBorders()
{
}

/**
 * called when update grid extent push button pressed.
 */
void 
BordersPageCreateFlatAnalysisGridBorders::slotUpdateGridExtentPushButton()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      float bounds[6];
      bms->getBounds(bounds);
      
      xMinDoubleSpinBox->setValue(bounds[0]);
      xMaxDoubleSpinBox->setValue(bounds[1]);
      yMinDoubleSpinBox->setValue(bounds[2]);
      yMaxDoubleSpinBox->setValue(bounds[3]);
   }
}
      
/**
 * apply the pages settings.
 */
void 
BordersPageCreateFlatAnalysisGridBorders::applyPage()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      //
      // See if a flat surface is in the main window
      //
      BrainModelSurface::SURFACE_TYPES st = bms->getSurfaceType();
      if ((st != BrainModelSurface::SURFACE_TYPE_FLAT) &&
          (st != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         if (QMessageBox::critical(theMainWindow, "Surface Type", 
                          "The surface in the main window must be flat for this operation.\n"
                          "Do you want to continue?",
                          QMessageBox::Yes | QMessageBox::No)
               == QMessageBox::No) {
            return;
         }
      }
 
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      float bounds[4];
      bounds[0] = xMinDoubleSpinBox->value();
      bounds[1] = xMaxDoubleSpinBox->value();
      bounds[2] = yMinDoubleSpinBox->value();
      bounds[3] = yMaxDoubleSpinBox->value();
      const float spacing = spacingDoubleSpinBox->value();
      
      BorderFile borderFile;
      bms->createFlatGridBordersForAnalysis(borderFile, bounds, spacing);

      theMainWindow->getBrainSet()->deleteAllBorders();
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->copyBordersFromBorderFile(bms, &borderFile);

      if (restrictPointsToSurfaceCheckBox->isChecked()) {
         theMainWindow->getLayersActions()->slotBordersDeletePointsOutsideSurface();
      }
      
      BorderColorFile* colorFile = theMainWindow->getBrainSet()->getBorderColorFile();
      colorFile->addColorIfItDoesNotExist("Grid", 255, 0, 0);
      theMainWindow->getBrainSet()->assignBorderColors();
               
      DisplaySettingsBorders* dsb = 
         theMainWindow->getBrainSet()->getDisplaySettingsBorders();
      dsb->setDisplayBorders(true);

      GuiFilesModified fm;
      fm.setBorderModified();
      fm.setBorderColorModified();
      theMainWindow->fileModificationUpdate(fm);
      
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
   }
}

/**
 * create the page.
 */
QWidget* 
BordersPageCreateFlatAnalysisGridBorders::createPage()
{
   //
   // X-Min
   //
   QLabel* xMinLabel = new QLabel("X-Min");
   xMinDoubleSpinBox = new QDoubleSpinBox;
   xMinDoubleSpinBox->setMinimum(-5000000.0);
   xMinDoubleSpinBox->setMaximum(5000000.0);
   xMinDoubleSpinBox->setSingleStep(1);
   xMinDoubleSpinBox->setDecimals(3);
   xMinDoubleSpinBox->setValue(10.0);
   
   //
   // X-Max
   //
   QLabel* xMaxLabel = new QLabel("X-Max");
   xMaxDoubleSpinBox = new QDoubleSpinBox;
   xMaxDoubleSpinBox->setMinimum(-5000000.0);
   xMaxDoubleSpinBox->setMaximum(5000000.0);
   xMaxDoubleSpinBox->setSingleStep(1);
   xMaxDoubleSpinBox->setDecimals(3);
   xMaxDoubleSpinBox->setValue(10.0);
   
   //
   // Y-Min
   //
   QLabel* yMinLabel = new QLabel("Y-Min");
   yMinDoubleSpinBox = new QDoubleSpinBox;
   yMinDoubleSpinBox->setMinimum(-5000000.0);
   yMinDoubleSpinBox->setMaximum(5000000.0);
   yMinDoubleSpinBox->setSingleStep(1);
   yMinDoubleSpinBox->setDecimals(3);
   yMinDoubleSpinBox->setValue(10.0);
   
   //
   // Y-Max
   //
   QLabel* yMaxLabel = new QLabel("Y-Max");
   yMaxDoubleSpinBox = new QDoubleSpinBox;
   yMaxDoubleSpinBox->setMinimum(-5000000.0);
   yMaxDoubleSpinBox->setMaximum(5000000.0);
   yMaxDoubleSpinBox->setSingleStep(1);
   yMaxDoubleSpinBox->setDecimals(3);
   yMaxDoubleSpinBox->setValue(10.0);
   
   //
   // spacing
   //
   QLabel* spacingLabel = new QLabel("Spacing (mm)");
   spacingDoubleSpinBox = new QDoubleSpinBox;
   spacingDoubleSpinBox->setMinimum(0.01);
   spacingDoubleSpinBox->setMaximum(5000000.0);
   spacingDoubleSpinBox->setSingleStep(1);
   spacingDoubleSpinBox->setDecimals(3);
   spacingDoubleSpinBox->setValue(10.0);
   
   //
   // Restrict points to surface check box
   //
   restrictPointsToSurfaceCheckBox = new QCheckBox("Restrict Points to Surface");
   restrictPointsToSurfaceCheckBox->setChecked(true);
   
   QPushButton* updateGridExtentPushButton = new QPushButton("Update Grid Extent");
   updateGridExtentPushButton->setAutoDefault(false);
   updateGridExtentPushButton->setFixedSize(updateGridExtentPushButton->sizeHint());
   QObject::connect(updateGridExtentPushButton, SIGNAL(clicked()),
                    this, SLOT(slotUpdateGridExtentPushButton()));
                    
   QWidget* w = new QWidget;
   QGridLayout* layout = new QGridLayout(w);
   layout->addWidget(xMinLabel, 0, 0);
   layout->addWidget(xMinDoubleSpinBox, 0, 1);
   layout->addWidget(xMaxLabel, 1, 0);
   layout->addWidget(xMaxDoubleSpinBox, 1, 1);
   layout->addWidget(yMinLabel, 2, 0);
   layout->addWidget(yMinDoubleSpinBox, 2, 1);
   layout->addWidget(yMaxLabel, 3, 0);
   layout->addWidget(yMaxDoubleSpinBox, 3, 1);
   layout->addWidget(spacingLabel, 4, 0);
   layout->addWidget(spacingDoubleSpinBox, 4, 1);
   layout->addWidget(restrictPointsToSurfaceCheckBox, 5, 0, 1, 2);
   layout->addWidget(updateGridExtentPushButton, 6, 0);
   
   slotUpdateGridExtentPushButton();
   
   return w;
}
                        
/**
 * update the page.
 */
void 
BordersPageCreateFlatAnalysisGridBorders::updatePage()
{
}

/**
 * see if the page is valid.
 */
bool 
BordersPageCreateFlatAnalysisGridBorders::getPageValid() const
{
   return true;
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageCreateInterpolatedBorders::BordersPageCreateInterpolatedBorders()
{
   border1Index = -1;
   border2Index = -1;
}
                   
/**
 * destructor.
 */
BordersPageCreateInterpolatedBorders::~BordersPageCreateInterpolatedBorders()
{
}

/**
 * apply the pages settings.
 */
void 
BordersPageCreateInterpolatedBorders::applyPage()
{
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   if ((border1Index < 0) || (border1Index >= numBorders) ||
       (border2Index < 0) || (border2Index >= numBorders)) {
      WuQMessageBox::critical(this, "ERROR", "Borders selected are invalid.  Reselect.");
      return;
   }
   
   //
   // Interpolate the borders
   //
   const QString borderName(namePrefixLineEdit->text());
   QString errorMessage;
   bmbs->createInterpolatedBorders(theMainWindow->getBrainModelSurface(),
                                   border1Index,
                                   border2Index,
                                   borderName,
                                   numberOfInterplatedBordersSpinBox->value(),
                                   samplingDoubleSpinBox->value(),
                                   errorMessage);
   if (errorMessage.isEmpty() == false) {
      WuQMessageBox::critical(this, "ERROR", errorMessage);
      return;
   }      
   
   //
   // Find the matching color
   //
   bool borderColorMatch = false;
   BorderColorFile* borderColorFile = theMainWindow->getBrainSet()->getBorderColorFile();
   const int borderColorIndex = borderColorFile->getColorIndexByName(borderName, borderColorMatch);
  
   //
   // Border color may need to be created
   //
   bool createBorderColor = false;
   if ((borderColorIndex >= 0) && (borderColorMatch == true)) {
      createBorderColor = false;
   }   
   else if ((borderColorIndex >= 0) && (borderColorMatch == false)) {
      QString msg("Use border color \"");
      msg.append(borderColorFile->getColorNameByIndex(borderColorIndex));
      msg.append("\" for border ");
      msg.append(borderName);
      msg.append(" ?");
      QString noButton("No, define color ");
      noButton.append(borderName);
      if (QMessageBox::information(this, 
                                   "Use Partially Matching Color",
                                   msg, 
                                   (QMessageBox::Yes | QMessageBox::No),
                                        QMessageBox::No) == QMessageBox::No) {
         createBorderColor = true;      
      }
   }   
   else {
      createBorderColor = true;
   }

   if (createBorderColor) {
      QString title("Create Border Color: ");
      title.append(borderName);
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this,
                                                                 title,
                                                                 false,
                                                                 false,
                                                                 false,
                                                                 false);
      csd->exec();

      //
      // Add new border color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      borderColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);
   }


   GuiFilesModified fm;
   fm.setBorderModified();
   fm.setBorderColorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();   
}

/**
 * create the page.
 */
QWidget* 
BordersPageCreateInterpolatedBorders::createPage()
{
   const int minWidth = 300;
   
   //
   // Border 1 Labels
   //
   QLabel* border1Label = new QLabel("Border 1 (left click): ");
   border1NameLabel = new QLabel("");
   border1NameLabel->setMinimumWidth(minWidth);
   
   //
   // Border 2 Labels
   //
   QLabel* border2Label = new QLabel("Border 2 (left click): ");
   border2NameLabel = new QLabel("");
   border2NameLabel->setMinimumWidth(minWidth);
   
   //
   // Name prefix
   //
   QLabel* namePrefixLabel = new QLabel("Name Prefix");
   namePrefixLineEdit = new QLineEdit;
   namePrefixLineEdit->setMinimumWidth(minWidth);
   namePrefixLineEdit->setText("LANDMARK.Interpolated");
   
   //
   // Sampling
   //
   QLabel* samplingLabel = new QLabel("Sampling");
   samplingDoubleSpinBox = new QDoubleSpinBox;
   samplingDoubleSpinBox->setMinimum(0.001);
   samplingDoubleSpinBox->setMaximum(500000.0);
   samplingDoubleSpinBox->setSingleStep(1);
   samplingDoubleSpinBox->setDecimals(2);
   samplingDoubleSpinBox->setValue(1.0);

   //
   // Number of interpolated borders
   //
   QLabel* numberOfInterplatedBordersLabel = new QLabel("Number of Interpolated Borders");
   numberOfInterplatedBordersSpinBox = new QSpinBox;
   numberOfInterplatedBordersSpinBox->setMinimum(1);
   numberOfInterplatedBordersSpinBox->setMaximum(100000);
   numberOfInterplatedBordersSpinBox->setSingleStep(1);
   numberOfInterplatedBordersSpinBox->setValue(1);
   
   //
   // Enable mouse push button
   //
   QPushButton* enableMousePushButton = new QPushButton("Enable Mouse");
   enableMousePushButton->setAutoDefault(false);
   enableMousePushButton->setFixedSize(enableMousePushButton->sizeHint());
   QObject::connect(enableMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotEnableMousePushButton()));
   
   QWidget* w = new QWidget;
   QGridLayout* layout = new QGridLayout(w);
   layout->addWidget(border1Label, 0, 0);
   layout->addWidget(border1NameLabel, 0, 1);
   layout->addWidget(border2Label, 1, 0);
   layout->addWidget(border2NameLabel, 1, 1);
   layout->addWidget(namePrefixLabel, 2, 0);
   layout->addWidget(namePrefixLineEdit, 2, 1);
   layout->addWidget(samplingLabel, 3, 0);
   layout->addWidget(samplingDoubleSpinBox, 3, 1);
   layout->addWidget(numberOfInterplatedBordersLabel, 4, 0);
   layout->addWidget(numberOfInterplatedBordersSpinBox, 4, 1);
   layout->addWidget(enableMousePushButton, 5, 0, 1, 2);
   
   return w;
}
                        
/**
 * called to enable mouse mode.
 */
void 
BordersPageCreateInterpolatedBorders::slotEnableMousePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_INTERPOLATE_NEW);   
}
      
/**
 * set selected borders.
 */
void 
BordersPageCreateInterpolatedBorders::setSelectedBorders(const int borderNumber,
                                              const int borderIndex)
{
   //
   // Get index and name of border
   //
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const BrainModelBorder* b = bmbs->getBorder(borderIndex);
   const QString labelValue(QString::number(borderIndex)
                            + "  "
                            + b->getName());
                            
   switch (borderNumber) {
      case 0:
         border1Index = borderIndex;
         border1NameLabel->setText(labelValue);
         break;
      case 1:
         border2Index = borderIndex;
         border2NameLabel->setText(labelValue);
         break;
   }
}
                              
/**
 * update the page.
 */
void 
BordersPageCreateInterpolatedBorders::updatePage()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_INTERPOLATE_NEW);   
}

/**
 * see if the page is valid.
 */
bool 
BordersPageCreateInterpolatedBorders::getPageValid() const
{
   return true;
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageEditAttributes::BordersPageEditAttributes()
{
}
                   
/**
 * destructor.
 */
BordersPageEditAttributes::~BordersPageEditAttributes()
{
}

/**
 * apply the pages settings.
 */
void 
BordersPageEditAttributes::applyPage()
{
}

/**
 * create the page.
 */
QWidget* 
BordersPageEditAttributes::createPage()
{
   QLabel* label = new QLabel("Attributes");
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(label);
   
   return w;
}
                        
/**
 * update the page.
 */
void 
BordersPageEditAttributes::updatePage()
{
}

/**
 * see if the page is valid.
 */
bool 
BordersPageEditAttributes::getPageValid() const
{
   return true;
}

//============================================================================
//============================================================================
//============================================================================

/**
 * constructor.
 */
BordersPageCreateLandmarks::BordersPageCreateLandmarks()
{
}
                   
/**
 * destructor.
 */
BordersPageCreateLandmarks::~BordersPageCreateLandmarks()
{
}

/**
 * set checkboxes for operations.
 */
void 
BordersPageCreateLandmarks::initializePage(const bool checkCreateFlattenLandmarks,
                                           const bool checkCreateRegistrationLandmarks,
                                           const BrainModelSurface* fiducialSurface,
                                           const BrainModelSurface* inflatedSurface,
                                           const BrainModelSurface* veryInflatedSurface,
                                           const BrainModelSurface* sphericalSurface)
{
   createFlattenLandmarksCheckBox->setChecked(checkCreateFlattenLandmarks);
   createRegistrationLandmarksCheckBox->setChecked(checkCreateRegistrationLandmarks);
   fiducialSurfaceComboBox->setSelectedBrainModelSurface(fiducialSurface);
   inflatedSurfaceComboBox->setSelectedBrainModelSurface(inflatedSurface);
   veryInflatedSurfaceComboBox->setSelectedBrainModelSurface(veryInflatedSurface);
   ellipsoidSurfaceComboBox->setSelectedBrainModelSurface(sphericalSurface);
}

/**
 * apply the pages settings.
 */
void 
BordersPageCreateLandmarks::applyPage()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   
   int operationMask = 0;
   if (createFlattenLandmarksCheckBox->isChecked()) {
      operationMask |= 
         BrainModelSurfaceBorderLandmarkIdentification::OPERATION_ID_FLATTENING_LANDMARKS;
   }
   if (createRegistrationLandmarksCheckBox->isChecked()) {
      operationMask |= 
         BrainModelSurfaceBorderLandmarkIdentification::OPERATION_ID_REGISTRATION_LANDMARKS;
   }
   
   BorderProjectionFile borderProjectionFile;
   BrainModelSurfaceBorderLandmarkIdentification 
      landmark(bs,
               stereotaxicSpaceComboBox->getSelectedStereotaxicSpace(),
               anatomicalVolumeComboBox->getSelectedVolumeFile(),
               fiducialSurfaceComboBox->getSelectedBrainModelSurface(),
               inflatedSurfaceComboBox->getSelectedBrainModelSurface(),
               veryInflatedSurfaceComboBox->getSelectedBrainModelSurface(),
               ellipsoidSurfaceComboBox->getSelectedBrainModelSurface(),
               bs->getSurfaceShapeFile(),
               shapeDepthColumnComboBox->currentIndex(),
               bs->getPaintFile(),
               paintGeographyColumnComboBox->currentIndex(),
               bs->getAreaColorFile(),
               &borderProjectionFile,
               bs->getBorderColorFile(),
               bs->getVocabularyFile(),
               operationMask);
   try {
      WuQDialog::showWaitCursor();
      landmark.execute();

      BrainModelBorderSet* bmbs = bs->getBorderSet();
      bmbs->copyBordersFromBorderProjectionFile(&borderProjectionFile);
      bmbs->setAllBordersModifiedStatus(false);
      bmbs->setProjectionsModified(true);
      
      DisplaySettingsBorders* dsb = bs->getDisplaySettingsBorders();
      dsb->setDisplayBorders(true);
      GuiFilesModified fm;
      fm.setAreaColorModified();
      fm.setBorderModified();
      fm.setBorderColorModified();
      fm.setPaintModified(),
      fm.setVocabularyModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();

      WuQDialog::showNormalCursor();
      WuQMessageBox::information(this, "OK", "Borders generated successfully.\n"
                                             "Data files need to be saved.  See\n"
                                             "File Menu->Manage Loaded Files.");
   }
   catch (BrainModelAlgorithmException& e) {
      WuQDialog::showNormalCursor();
      WuQMessageBox::critical(this, "ERROR", e.whatQString());
   }
}

/**
 * create the page.
 */
QWidget* 
BordersPageCreateLandmarks::createPage()
{
   const int maxWidth = 400;
   
   //
   // Create flatten landmarks check box
   //
   createFlattenLandmarksCheckBox = new QCheckBox("Create Flattening Landmarks");
   createFlattenLandmarksCheckBox->setChecked(true);
   
   //
   // Create flatten landmarks check box
   //
   createRegistrationLandmarksCheckBox = new QCheckBox("Create Registration Landmarks");
   createRegistrationLandmarksCheckBox->setChecked(true);
   
   //
   // stereotaxic space
   //
   QLabel* stereotaxicSpaceLabel = new QLabel("Stereotaxic Space");
   stereotaxicSpaceComboBox = new GuiStereotaxicSpaceComboBox;
   stereotaxicSpaceComboBox->setMaximumWidth(maxWidth);
    
   //
   // anatomical volume
   //
   QLabel* anatomicalVolumeLabel = new QLabel("Anatomical Volume");
   anatomicalVolumeComboBox = new GuiVolumeFileSelectionComboBox(
                                        VolumeFile::VOLUME_TYPE_ANATOMY);
   anatomicalVolumeComboBox->setMaximumWidth(maxWidth);
   anatomicalVolumeComboBox->setToolTip(
           "If the anatomical volume contains ONLY the corpus callosum\n"
           "the name MUST contain both the words \"corpus\" and \"callosum\"");
    
   //
   // fiducial surface
   //
   QLabel* fiducialSurfaceLabel = new QLabel("Fiducial Surface");
   fiducialSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
       BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   fiducialSurfaceComboBox->setMaximumWidth(maxWidth);
    
   //
   // inflated surface
   //
   QLabel* inflatedSurfaceLabel = new QLabel("Inflated Surface");
   inflatedSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
       BrainModelSurface::SURFACE_TYPE_INFLATED);
   inflatedSurfaceComboBox->setMaximumWidth(maxWidth);
    
   //
   // very inflated surface
   //
   QLabel* veryInflatedSurfaceLabel = new QLabel("Very Inflated Surface");
   veryInflatedSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
       BrainModelSurface::SURFACE_TYPE_VERY_INFLATED);
   veryInflatedSurfaceComboBox->setMaximumWidth(maxWidth);
    
   //
   // ellipsoid surface
   //
   std::vector<BrainModelSurface::SURFACE_TYPES> sphereEllipsoidTypes;
   sphereEllipsoidTypes.push_back(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL);
   sphereEllipsoidTypes.push_back(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   QLabel* ellipsoidSurfaceLabel = new QLabel("Sphere/Ellipsoid Surface");
   ellipsoidSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
                                                       sphereEllipsoidTypes);
   ellipsoidSurfaceComboBox->setMaximumWidth(maxWidth);
    
   //
   // paint geography column
   //
   QLabel* paintGeographyLabel = new QLabel("Paint Geography Column");
   paintGeographyColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                     GUI_NODE_FILE_TYPE_PAINT,
                                     false,
                                     false,
                                     false);
   paintGeographyColumnComboBox->setMaximumWidth(maxWidth);

   //
   // surface shape depth column
   //
   QLabel* surfaceShapeDepthLabel = new QLabel("Shape Depth Column");
   shapeDepthColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                     GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                     false,
                                     false,
                                     false);
   shapeDepthColumnComboBox->setMaximumWidth(maxWidth);
    
   QWidget* w = new QWidget;
   QGridLayout* layout = new QGridLayout(w);
   layout->addWidget(createFlattenLandmarksCheckBox, 0, 0, 1, 2);
   layout->addWidget(createRegistrationLandmarksCheckBox, 1, 0, 1, 2);
   layout->addWidget(stereotaxicSpaceLabel, 2, 0);
   layout->addWidget(stereotaxicSpaceComboBox, 2, 1);
   layout->addWidget(anatomicalVolumeLabel, 3, 0);
   layout->addWidget(anatomicalVolumeComboBox, 3, 1);
   layout->addWidget(fiducialSurfaceLabel, 4, 0);
   layout->addWidget(fiducialSurfaceComboBox, 4, 1);
   layout->addWidget(inflatedSurfaceLabel, 5, 0);
   layout->addWidget(inflatedSurfaceComboBox, 5, 1);
   layout->addWidget(veryInflatedSurfaceLabel, 6, 0);
   layout->addWidget(veryInflatedSurfaceComboBox, 6, 1);
   layout->addWidget(ellipsoidSurfaceLabel, 7, 0);
   layout->addWidget(ellipsoidSurfaceComboBox, 7, 1);
   layout->addWidget(paintGeographyLabel, 8, 0);
   layout->addWidget(paintGeographyColumnComboBox, 8, 1);
   layout->addWidget(surfaceShapeDepthLabel, 9, 0);
   layout->addWidget(shapeDepthColumnComboBox, 9, 1);
   layout->setColumnStretch(0, 0);
   layout->setColumnStretch(0, 1);
   
   return w;
}
                        
/**
 * update the page.
 */
void 
BordersPageCreateLandmarks::updatePage()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   stereotaxicSpaceComboBox->setSelectedStereotaxicSpace(bs->getStereotaxicSpace());
   anatomicalVolumeComboBox->updateComboBox();
   fiducialSurfaceComboBox->updateComboBox();
   inflatedSurfaceComboBox->updateComboBox();
   veryInflatedSurfaceComboBox->updateComboBox();
   ellipsoidSurfaceComboBox->updateComboBox();
   shapeDepthColumnComboBox->updateComboBox();
   const int depthColumn = bs->getSurfaceShapeFile()->getColumnWithName(
                                  SurfaceShapeFile::sulcalDepthColumnName);
   if (depthColumn >= 0) {
      shapeDepthColumnComboBox->setCurrentIndex(depthColumn);
   }
   paintGeographyColumnComboBox->updateComboBox();
   const int geographyColumn = bs->getPaintFile()->getColumnWithName(
                                  PaintFile::columnNameGeography);
   if (geographyColumn >= 0) {
      paintGeographyColumnComboBox->setCurrentIndex(geographyColumn);
   }
}

/**
 * see if the page is valid.
 */
bool 
BordersPageCreateLandmarks::getPageValid() const
{
   return true;
}
