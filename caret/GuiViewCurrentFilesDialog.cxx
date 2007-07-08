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

#include <sstream>

#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QToolTip>

#include "AreaColorFile.h"
#include "ArealEstimationFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "CocomacConnectivityFile.h"
#include "CommaSeparatedValueFile.h"
#include "ContourCellFile.h"
#include "ContourCellColorFile.h"
#include "CutsFile.h"
#include "DeformationFieldFile.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GeodesicDistanceFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiCommaSeparatedValueFileEditor.h"
#include "GuiDataFileCommentDialog.h"
#include "GuiFilesModified.h"
#include "GuiMessageBox.h"
#include "GuiViewCurrentFilesDialog.h"
#include "ImageFile.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "PaletteFile.h"
#include "ParamsFile.h"
#include "ProbabilisticAtlasFile.h"
#include "RgbPaintFile.h"
#include "SceneFile.h"
#include "SectionFile.h"
#include "StudyMetaDataFile.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "TopographyFile.h"
#include "TopologyFile.h"
#include "VocabularyFile.h"
#include "VolumeFile.h"
#include "VtkModelFile.h"
#include "WustlRegionFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiViewCurrentFilesDialog::GuiViewCurrentFilesDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setAttribute(Qt::WA_DeleteOnClose);

   QString captionStr("View Current Files");
   const QString specFileName = theMainWindow->getBrainSet()->getSpecFileName();
   if (specFileName.isEmpty() == false) {
      captionStr.append(": ");
      captionStr.append(FileUtilities::basename(specFileName));
   }
   setWindowTitle(captionStr);
   
   const SpecFile* sf = theMainWindow->getBrainSet()->getLoadedFilesSpecFile();
   
   GuiFilesModified fm;
   
   //
   // width of view/info/clear buttons
   // 
   buttonWidth = 60;
   
   //
   // Layout indices
   //
   int numCols = 0;
   columnFileTypeIndex = numCols++;
   columnModifiedIndex = numCols++;
   columnViewIndex = numCols++;
   columnCommentIndex = numCols++;
   columnClearIndex = numCols++;
   columnFileNameIndex = numCols++;

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // widget for files widget since there could be many files
   //
   fileGridWidget = new QWidget;
   fileGridLayout = new QGridLayout(fileGridWidget);
   fileGridLayout->setSpacing(5);

   // QVBox* vbox = new QVBox(sv, "vbox");  // prevents background from showing through
   //fileGrid = new QGrid(2, Qt::Horizontal, vbox, "fileGrid");
   //sv->addChild(vbox);
   
   //
   // Button group for data files "V" push button
   //
   viewButtonGroup = new QButtonGroup(this);
   QObject::connect(viewButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotViewButtonGroup(int)));
   
   //
   // Button group for data files "?" push button
   //
   infoButtonGroup = new QButtonGroup(this);
   QObject::connect(infoButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotInfoButtonGroup(int)));
   
   //
   // Button group for border files "?" push button
   //
   borderInfoButtonGroup = new QButtonGroup(this);
   QObject::connect(borderInfoButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotBorderInfoButtonGroup(int)));
   
   //
   // Clear button group for data files "X" push button
   //
   clearButtonGroup = new QButtonGroup(this);
   QObject::connect(clearButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotClearButtonGroup(int)));
   
   //
   // column labels
   //
   fileGridLayout->addWidget(new QLabel("File Type"), 0, columnFileTypeIndex);
   fileGridLayout->addWidget(new QLabel(" Mod "), 0, columnModifiedIndex);
   fileGridLayout->addWidget(new QLabel("View"), 0, columnViewIndex);
   fileGridLayout->addWidget(new QLabel("Comment"), 0, columnCommentIndex);
   fileGridLayout->addWidget(new QLabel("Clear"), 0, columnClearIndex);
   fileGridLayout->addWidget(new QLabel("File Name"), 0, columnFileNameIndex);
   
   fm.setStatusForAll(false);
   fm.setAreaColorModified();
   addFileToGrid(theMainWindow->getBrainSet()->getAreaColorFile(), 
                 fm,
                 sf->areaColorFile,
                 "Area Color File");

   fm.setStatusForAll(false);
   fm.setArealEstimationModified();
   addFileToGrid(theMainWindow->getBrainSet()->getArealEstimationFile(), 
                 fm,
                 sf->arealEstimationFile,
                 "Areal Estimation File");

   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_RAW,
                                               "Border - Raw");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                               "Border - Fiducial");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_INFLATED,
                                               "Border - Inflated");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_VERY_INFLATED,
                                               "Border - Very Inflated");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                               "Border - Spherical");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL,
                                               "Border - Ellipsoid");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
                                               "Border - Comp Med Wall");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_FLAT,
                                               "Border - Flat");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR,
                                               "Border - Flat Lobar");
   addBorderFileToGrid(bmbs, BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                                               "Border - Unknown");
                                               
   
   fm.setStatusForAll(false);
   fm.setBorderColorModified();
   addFileToGrid(theMainWindow->getBrainSet()->getBorderColorFile(),
                 fm,
                  "Border Color File");

   addBorderProjectionFileToGrid(bmbs);
                       
   fm.setStatusForAll(false);
   fm.setCellColorModified();
   addFileToGrid(theMainWindow->getBrainSet()->getCellColorFile(),
                 fm,
                 sf->cellColorFile,
                  "Cell Color File");
                  
   fm.setStatusForAll(false);
   fm.setCellProjectionModified();
   addFileToGrid(theMainWindow->getBrainSet()->getCellProjectionFile(), fm, 
                 sf->cellProjectionFile);

   fm.setStatusForAll(false);
   fm.setCellModified();
   addFileToGrid(theMainWindow->getBrainSet()->getVolumeCellFile(), fm, 
                 sf->volumeCellFile,
                 "Cell File (Volume)");

   fm.setStatusForAll(false);
   fm.setCocomacModified();
   addFileToGrid(theMainWindow->getBrainSet()->getCocomacFile(), fm,
                 sf->cocomacConnectivityFile);

   fm.setStatusForAll(false);
   fm.setContourCellModified();
   addFileToGrid(theMainWindow->getBrainSet()->getContourCellFile(), 
                 fm,
                 sf->contourCellFile,
                 "Contour Cells File");

   fm.setStatusForAll(false);
   fm.setCellColorModified();
   addFileToGrid(theMainWindow->getBrainSet()->getContourCellColorFile(),
                 fm,
                 sf->contourCellColorFile,
                 "Contour Cell Color File");
                 
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      BrainModel* bm = theMainWindow->getBrainSet()->getBrainModel(i);
      switch(bm->getModelType()) {
         case BrainModel::BRAIN_MODEL_CONTOURS:
            {
               BrainModelContours* bmc = dynamic_cast<BrainModelContours*>(bm);
               ContourFile* cf = bmc->getContourFile();
               fm.setStatusForAll(false);
               fm.setContourModified();
               addFileToGrid(cf, fm); 
            }
            break;
         case BrainModel::BRAIN_MODEL_SURFACE:
            {
               BrainModelSurface* bms = dynamic_cast<BrainModelSurface*>(bm);
               CoordinateFile* cf = bms->getCoordinateFile();
               fm.setStatusForAll(false);
               fm.setCoordinateModified();
               addFileToGrid(cf, fm);
            }
            break;
         case BrainModel::BRAIN_MODEL_VOLUME:
            break;
         case BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME:
            break;
      }
   }   
   
   fm.setStatusForAll(false);
   fm.setCutModified();
   addFileToGrid(theMainWindow->getBrainSet()->getCutsFile(), 
                 fm,
                 sf->cutsFile,
                 "Cuts File");
   
   fm.setStatusForAll(false);
   fm.setDeformationFieldModified();
   addFileToGrid(theMainWindow->getBrainSet()->getDeformationFieldFile(),
                 fm,
                 sf->deformationFieldFile,
                 "Deformation Field File");
   
   fm.setStatusForAll(false);
   fm.setFociColorModified();
   addFileToGrid(theMainWindow->getBrainSet()->getFociColorFile(),
                 fm,
                 sf->fociColorFile,
                 "Foci Color File");
   
   fm.setStatusForAll(false);
   fm.setFociProjectionModified();
   addFileToGrid(theMainWindow->getBrainSet()->getFociProjectionFile(),
                 fm,
                 sf->fociProjectionFile,
                 "Foci Projection File");
   
   fm.setStatusForAll(false);
   fm.setFociModified();
   addFileToGrid(theMainWindow->getBrainSet()->getVolumeFociFile(), fm, 
                 sf->volumeFociFile,
                 "Foci File (Volume)");

   fm.setStatusForAll(false);
   fm.setGeodesicModified();
   addFileToGrid(theMainWindow->getBrainSet()->getGeodesicDistanceFile(), fm,
                 sf->geodesicDistanceFile);
   
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfImageFiles(); i++) {
      fm.setStatusForAll(false);
      fm.setImagesModified();
      addFileToGrid(theMainWindow->getBrainSet()->getImageFile(i), fm);
   }
   
   fm.setStatusForAll(false);
   fm.setLatLonModified();
   addFileToGrid(theMainWindow->getBrainSet()->getLatLonFile(), fm,
                 sf->latLonFile);
   
   fm.setStatusForAll(false);
   fm.setMetricModified();
   addFileToGrid(theMainWindow->getBrainSet()->getMetricFile(), fm, sf->metricFile);
   
   fm.setStatusForAll(false);
   fm.setPaintModified();
   addFileToGrid(theMainWindow->getBrainSet()->getPaintFile(), fm, sf->paintFile);
   
   fm.setStatusForAll(false);
   fm.setParameterModified();
   addFileToGrid(theMainWindow->getBrainSet()->getParamsFile(), fm, sf->paramsFile);
   
   fm.setStatusForAll(false);
   fm.setProbabilisticAtlasModified();
   addFileToGrid(theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile(), 
                 fm,
                 sf->atlasFile,
                 "Probabilistic Atlas File");
   
   fm.setStatusForAll(false);
   fm.setPaletteModified();
   addFileToGrid(theMainWindow->getBrainSet()->getPaletteFile(), fm, sf->paletteFile);
   
   fm.setStatusForAll(false);
   fm.setRgbPaintModified();
   addFileToGrid(theMainWindow->getBrainSet()->getRgbPaintFile(), fm, sf->rgbPaintFile);
   
   fm.setStatusForAll(false);
   fm.setSectionModified();
   addFileToGrid(theMainWindow->getBrainSet()->getSectionFile(), fm, sf->sectionFile);
   
   fm.setStatusForAll(false);
   fm.setSceneModified();
   addFileToGrid(theMainWindow->getBrainSet()->getSceneFile(), fm, sf->sceneFile);
   
   fm.setStatusForAll(false);
   fm.setStudyMetaDataModified();
   addFileToGrid(theMainWindow->getBrainSet()->getStudyMetaDataFile(), fm, sf->studyMetaDataFile);
   
   fm.setStatusForAll(false);
   fm.setSurfaceShapeModified();
   addFileToGrid(theMainWindow->getBrainSet()->getSurfaceShapeFile(), 
                 fm,
                 sf->surfaceShapeFile,
                 "Surface Shape File");
   
   fm.setStatusForAll(false);
   fm.setSurfaceVectorModified();
   addFileToGrid(theMainWindow->getBrainSet()->getSurfaceVectorFile(), 
                 fm,
                 sf->surfaceVectorFile,
                 "Surface Vector File");
   
   fm.setStatusForAll(false);
   fm.setTopographyModified();
   addFileToGrid(theMainWindow->getBrainSet()->getTopographyFile(), fm);

   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfTopologyFiles(); i++) {
      TopologyFile* tf = theMainWindow->getBrainSet()->getTopologyFile(i);   
      fm.setStatusForAll(false);
      fm.setTopologyModified();
      addFileToGrid(tf, fm);
   }   
   
   fm.setStatusForAll(false);
   fm.setVocabularyModified();
   addFileToGrid(theMainWindow->getBrainSet()->getVocabularyFile(), fm);
   
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {   
      fm.setStatusForAll(false);
      fm.setVolumeModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVolumeAnatomyFile(i), 
                 fm,
                 "Volume File - Anatomy");
   }
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
      fm.setStatusForAll(false);
      fm.setVolumeModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVolumeFunctionalFile(i), 
                 fm,
                 "Volume File - Functional");
   }
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {   
      fm.setStatusForAll(false);
      fm.setVolumeModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVolumePaintFile(i), 
                 fm,
                 "Volume File - Paint");
   }
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {   
      fm.setStatusForAll(false);
      fm.setVolumeModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i), 
                 fm,
                 "Volume File - Prob Atlas");
   }
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
      fm.setStatusForAll(false);
      fm.setVolumeModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVolumeRgbFile(i), 
                 fm,
                 "Volume File - RGB");
   }
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {   
      fm.setStatusForAll(false);
      fm.setVolumeModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVolumeSegmentationFile(i), 
                 fm,
                 "Volume File - Segmentation");
   }
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {   
      fm.setStatusForAll(false);
      fm.setVolumeModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVolumeVectorFile(i), 
                 fm,
                 "Volume File - Vector");
   }

   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVtkModelFiles(); i++) {   
      fm.setStatusForAll(false);
      fm.setVtkModelModified();
      addFileToGrid(theMainWindow->getBrainSet()->getVtkModelFile(i), fm);
   }
   
   fm.setStatusForAll(false);
   fm.setWustlRegionModified();
   addFileToGrid(theMainWindow->getBrainSet()->getWustlRegionFile(), fm, sf->wustlRegionFile);
   
   //
   // Scrolled widget for files 
   //
   QScrollArea* sv = new QScrollArea(this);
   dialogLayout->addWidget(sv);
   sv->setWidget(fileGridWidget);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close", this);
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
}

/**
 * Called when an info button is pressed.
 */
void
GuiViewCurrentFilesDialog::slotInfoButtonGroup(int item)
{
   AbstractFile* af = dataFiles[item];
   if (af != NULL) {
      GuiDataFileCommentDialog* dfc = new GuiDataFileCommentDialog(this, af);
      dfc->show();
   }
}

/**
 * Called when a view button is pressed.
 */
void
GuiViewCurrentFilesDialog::slotViewButtonGroup(int item)
{
   AbstractFile* af = dataFiles[item];
   if (af != NULL) {
      //CommaSeparatedValueFile csv;
      //af->writeDataIntoCommaSeparatedValueFile(csv);
      //GuiCommaSeparatedValueFileEditor* csfv = new GuiCommaSeparatedValueFileEditor(this, &csv, af);
      GuiCommaSeparatedValueFileEditor* csfv = 
                new GuiCommaSeparatedValueFileEditor(this, af, dataFileTypeMask[item]);
      csfv->show();
   }
}

/**
 * Called when a border info button is pressed.
 */
void
GuiViewCurrentFilesDialog::slotBorderInfoButtonGroup(int item)
{
   BrainModelBorderFileInfo* bfi = borderInfoFileInfo[item];
   if (bfi != NULL) {
      GuiDataFileCommentDialog* dfc = new GuiDataFileCommentDialog(this, bfi);
      dfc->show();
   }
}

/**
 * Called when an clear button is pressed.
 */
void
GuiViewCurrentFilesDialog::slotClearButtonGroup(int item)
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   AbstractFile* af = dataFiles[item];
   
   if (af != NULL) {
      const GuiFilesModified fm = dataFileTypeMask[item];
      
      if (fm.getCoordinateModified()) {
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
            BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
            if (bms != NULL) {
               if (bms->getCoordinateFile() == af) {
                  theMainWindow->getBrainSet()->deleteBrainModelSurface(bms);
                  break;
               }
            }
         }
      }
      else if (fm.getVolumeModified()) {
         theMainWindow->getBrainSet()->deleteVolumeFile(dynamic_cast<VolumeFile*>(af));
      }
      else if(fm.getContourModified()) {
         BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours();
         if (bmc != NULL) {
            theMainWindow->getBrainSet()->deleteBrainModel(bmc);
         }
      }
      else if (fm.getTopologyModified()) {
         theMainWindow->getBrainSet()->deleteTopologyFile(dynamic_cast<TopologyFile*>(af));
      }
      else if (fm.getVtkModelModified()) {
         theMainWindow->getBrainSet()->deleteVtkModelFile(dynamic_cast<VtkModelFile*>(af));
      }
      else if (fm.getImagesModified()) {
         theMainWindow->getBrainSet()->deleteImageFile(dynamic_cast<ImageFile*>(af));
      }
      else if (dynamic_cast<AreaColorFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearAreaColorFile();
      }
      else if (dynamic_cast<ArealEstimationFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearArealEstimationFile();
      }
      else if (dynamic_cast<BorderColorFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearBorderColorFile();
      }
      else if (dynamic_cast<CellFile*>(af) != NULL) {  // COULD BE FOCI or CELLS
         CellFile* cf = dynamic_cast<CellFile*>(af);
         if (cf == theMainWindow->getBrainSet()->getVolumeCellFile()) {
            theMainWindow->getBrainSet()->deleteAllCells(false, true);
         }
         else if (cf == theMainWindow->getBrainSet()->getVolumeFociFile()) {
            theMainWindow->getBrainSet()->deleteAllFoci(false, true);
         }
      }
      else if (dynamic_cast<CellProjectionFile*>(af) != NULL) {
         CellProjectionFile* cpf = dynamic_cast<CellProjectionFile*>(af);
         if (cpf == theMainWindow->getBrainSet()->getCellProjectionFile()) {
            theMainWindow->getBrainSet()->deleteAllCells(true, false);
         }
         else if (cpf == theMainWindow->getBrainSet()->getFociProjectionFile()) {
            theMainWindow->getBrainSet()->deleteAllFoci(true, false);
         }
      }
      else if (dynamic_cast<CellColorFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearCellColorFile();
      }
      else if (dynamic_cast<CocomacConnectivityFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearCocomacConnectivityFile();
      }
      else if (dynamic_cast<ContourCellColorFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearContourCellColorFile();
      }
      else if (dynamic_cast<ContourCellFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearContourCellFile();
      }
      else if (dynamic_cast<CutsFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearCutsFile();
      }
      else if (dynamic_cast<DeformationFieldFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearDeformationFieldFile();
      }
      else if (dynamic_cast<DeformationMapFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearDeformationMapFile();
      }
      else if (dynamic_cast<FociColorFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearFociColorFile();
      }
      else if (dynamic_cast<GeodesicDistanceFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearGeodesicDistanceFile();
      }
      else if (dynamic_cast<LatLonFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearLatLonFile();
      }    // shape must be before metric since shape derived from metric
      else if (dynamic_cast<SurfaceShapeFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearSurfaceShapeFile();
      }
      else if (dynamic_cast<SurfaceVectorFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearSurfaceVectorFile();
      }
      else if (dynamic_cast<MetricFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearMetricFile();
      }
      else if ((dynamic_cast<PaintFile*>(af) != NULL) && // not prob atlas derived from paint
               (dynamic_cast<ProbabilisticAtlasFile*>(af) == NULL)) {
         theMainWindow->getBrainSet()->clearPaintFile();
      }
      else if (dynamic_cast<PaletteFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearPaletteFile();
      }
      else if (dynamic_cast<ParamsFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearParamsFile();
      }
      else if (dynamic_cast<ProbabilisticAtlasFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearProbabilisticAtlasFile();
      }
      else if (dynamic_cast<RgbPaintFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearRgbPaintFile();
      }
      else if (dynamic_cast<SceneFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearSceneFile();
      }
      else if (dynamic_cast<SectionFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearSectionFile();
      }
      else if (dynamic_cast<StudyMetaDataFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearStudyMetaDataFile();
      }
      else if (dynamic_cast<TopographyFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearTopographyFile();
      }
      else if (dynamic_cast<TransformationMatrixFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearTransformationMatrixFile();
      }
      else if (dynamic_cast<VocabularyFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearVocabularyFile();
      }
      else if (dynamic_cast<WustlRegionFile*>(af) != NULL) {
         theMainWindow->getBrainSet()->clearWustlRegionFile();
      }
      else {
         std::ostringstream str;
         str << "PROGRAM ERROR: Unrecognized file type for deleting: "
             << af->getDescriptiveName().toAscii().constData();
         GuiMessageBox::critical(this, "PROGRAM ERROR", str.str().c_str(), "OK");
            //af->clear();
      }
      dataFiles[item] = NULL;
      
      //
      // Disable info & clear button
      //
      QAbstractButton* clearButton = clearButtonGroup->button(item);
      if (clearButton != NULL) {
         clearButton->setEnabled(false);
      }
      QAbstractButton* infoButton = infoButtonGroup->button(item);
      if (infoButton != NULL) {
         infoButton->setEnabled(false);
      }
   }
   
   theMainWindow->fileModificationUpdate(dataFileTypeMask[item]);
   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
}

/**
 * Add a border file to the grid.
 */
void
GuiViewCurrentFilesDialog::addBorderFileToGrid(BrainModelBorderSet* bmbs,
                                               const BrainModelSurface::SURFACE_TYPES surfaceType,
                                               const char* typeName)
{
   BrainModelBorderFileInfo* bmi = bmbs->getBorderFileInfo(surfaceType);
   if (bmi != NULL) {
      const QString fileName(bmi->getFileName());
      if (fileName.isEmpty() == false) {
         const BorderFile* bf = bmbs->copyBordersOfSpecifiedType(surfaceType);
         if (bf != NULL) {
            if (bf->getNumberOfBorders() > 0) {
               const int rowNum = fileGridLayout->rowCount() + 1;
               fileGridLayout->addWidget(new QLabel(typeName), rowNum, columnFileTypeIndex);
               fileGridLayout->addWidget(new QLabel("     "), rowNum, columnModifiedIndex);
               
               QPushButton* infoPB = new QPushButton("?");
               infoPB->setAutoDefault(false);
               infoPB->setFixedWidth(buttonWidth);
               borderInfoButtonGroup->addButton(infoPB);
               infoPB->setToolTip("Show file information.");
               fileGridLayout->addWidget(infoPB, rowNum, columnCommentIndex);
               borderInfoFileInfo.push_back(bmi);
               
               fileGridLayout->addWidget(new QLabel(" "), rowNum, columnClearIndex);    // X button
               fileGridLayout->addWidget(new QLabel(fileName), rowNum, columnFileNameIndex);
            }
            if (bf != NULL) {
               delete bf;
            }
         }
      }
   }
}

/**
 * Add a border projection file to the grid.
 */
void
GuiViewCurrentFilesDialog::addBorderProjectionFileToGrid(BrainModelBorderSet* bmbs)
{
   BrainModelBorderFileInfo* bmi = bmbs->getBorderProjectionFileInfo();
   if (bmi != NULL) {
      const QString fileName(bmi->getFileName());
      if (fileName.isEmpty() == false) {
         BorderProjectionFile bpf;
         bmbs->copyBordersToBorderProjectionFile(bpf);
         if (bpf.getNumberOfBorderProjections() > 0) {
            const int rowNum = fileGridLayout->rowCount() + 1;
            fileGridLayout->addWidget(new QLabel("Border Projection"), rowNum, columnFileTypeIndex);
            fileGridLayout->addWidget(new QLabel("     "), rowNum, columnModifiedIndex);
            
            borderProjInfoPB = new QPushButton("?");
            borderProjInfoPB->setAutoDefault(false);
            borderProjInfoPB->setFixedWidth(buttonWidth);
            borderInfoButtonGroup->addButton(borderProjInfoPB);
            borderProjInfoPB->setToolTip("Show file information.");
            fileGridLayout->addWidget(borderProjInfoPB, rowNum, columnCommentIndex);
            borderInfoFileInfo.push_back(bmi);
            
            borderProjClearPB = new QPushButton("X");
            borderProjClearPB->setAutoDefault(false);
            borderProjClearPB->setFixedWidth(buttonWidth);
            QObject::connect(borderProjClearPB, SIGNAL(clicked()),
                             this, SLOT(slotDeleteBorderProjections()));
            borderProjClearPB->setToolTip("Delete this file from memory.\n"
                                   "The file is NOT deleted.");
            fileGridLayout->addWidget(borderProjClearPB, rowNum, columnClearIndex);

            fileGridLayout->addWidget(new QLabel(fileName), rowNum, columnFileNameIndex);
         }
      }
   }
}

/**
 * called to delete border projections.
 */
void 
GuiViewCurrentFilesDialog::slotDeleteBorderProjections()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->deleteBorderProjections();
   
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
   
   borderProjInfoPB->setEnabled(false);
   borderProjClearPB->setEnabled(false);
   
   QApplication::restoreOverrideCursor();
}
      
/**
 * Add a file to the grid.
 */
void
GuiViewCurrentFilesDialog::addFileToGrid(AbstractFile* af,
                                         const GuiFilesModified& fileMask,
                                         const char* typeName)
{
   SpecFile::Entry dummyEntry;
   addFileToGrid(af, fileMask, dummyEntry, typeName);
}

/**
 * add a file to the list of files.
 */
void 
GuiViewCurrentFilesDialog::addFileToGrid(AbstractFile* af,
                               const GuiFilesModified& fileMask,
                               const SpecFile::Entry& dataFileInfo,
                               const char* typeName)
{
   if (af != NULL) {
      if (af->empty() == false) {
         QString label(af->getDescriptiveName());
         if (typeName != NULL) {
            label = typeName;
         }
         QString modifiedLabel("     ");
         if (af->getModified()) {
            modifiedLabel = "  *  ";
         }
         const int rowNum = fileGridLayout->rowCount() + 1;
         
         fileGridLayout->addWidget(new QLabel(label), rowNum, columnFileTypeIndex);
         
         fileGridLayout->addWidget(new QLabel(modifiedLabel), rowNum, columnModifiedIndex);
         
         QPushButton* viewPB = new QPushButton("V");
         viewPB->setAutoDefault(false);
         viewPB->setFixedWidth(buttonWidth);
         viewButtonGroup->addButton(viewPB, dataFiles.size());
         viewPB->setToolTip("View data file in spreadsheet");
         fileGridLayout->addWidget(viewPB, rowNum, columnViewIndex);
         bool readCSV, writeCSV;
         af->getCommaSeparatedFileSupport(readCSV, writeCSV);
         if (writeCSV == false) {
            viewPB->hide();
         }
         
         QPushButton* infoPB = new QPushButton("?");
         infoPB->setAutoDefault(false);
         infoPB->setFixedWidth(buttonWidth);
         infoButtonGroup->addButton(infoPB, dataFiles.size());
         infoPB->setToolTip("Show file information.");
         fileGridLayout->addWidget(infoPB, rowNum, columnCommentIndex);
         
         QPushButton* clearPB = new QPushButton("X");
         clearPB->setAutoDefault(false);
         clearPB->setFixedWidth(buttonWidth);
         clearButtonGroup->addButton(clearPB, dataFiles.size());
         clearPB->setToolTip("Delete this file from memory.\n"
                                "The file is NOT deleted.");
         fileGridLayout->addWidget(clearPB, rowNum, columnClearIndex);
         
         dataFiles.push_back(af);
         dataFileTypeMask.push_back(fileMask);
         
         const QString filename(FileUtilities::basename(af->getFileName()));
         fileGridLayout->addWidget(new QLabel(filename), rowNum, columnFileNameIndex);
         
         for (int i = 0; i < dataFileInfo.getNumberOfFiles(); i++) {
            if (dataFileInfo.files[i].selected == SpecFile::SPEC_TRUE) {
               const QString name(FileUtilities::basename(dataFileInfo.files[i].filename));
               if (name != filename) {
                  const int rowNum = fileGridLayout->rowCount() + 1;
/*
                  fileGridLayout->addWidget(new QLabel(" "), rowNum, 0);
                  fileGridLayout->addWidget(new QLabel(" "), rowNum, 1);
                  fileGridLayout->addWidget(new QLabel(" "), rowNum, 2);
                  fileGridLayout->addWidget(new QLabel(" "), rowNum, 3);
                  fileGridLayout->addWidget(new QLabel(" "), rowNum, 4);
*/
                  fileGridLayout->addWidget(new QLabel(name), rowNum, columnFileNameIndex);
               }
            }
         }
      }
   }
}                         

/**
 * Destructor
 */
GuiViewCurrentFilesDialog::~GuiViewCurrentFilesDialog()
{
}
