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

#include <QAction>
#include <QApplication>

#include "AreaColorFile.h"
#include "BorderToTopographyConverter.h"
#include "BrainModelBorderSet.h"
#include "BrainModelRunCaretUtilityProgram.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiChooseNodeAttributeColumnDialog.h"
#include "GuiColorFileEditorDialog.h"
#include "GuiCurrentColoringToRgbPaintDialog.h"
#include "GuiDeformationFieldDialog.h"
#include "GuiGenerateArealEstimationDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowAttributesActions.h"
#include "GuiMapFmriDialog.h"
#include "GuiMessageBox.h"
#include "GuiMetricShapeStatisticsDialog.h"
#include "GuiMetricShapeOneAndPairedTTestDialog.h"
#include "GuiMetricShapeStatisticalAlgorithmDialog.h"
#include "GuiChooseNodeAttributeColumnDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiNodeAttributeFileClearResetDialog.h"
#include "GuiPaintNamesEditDialog.h"
#include "GuiMetricShapeInterHemClustersDialog.h"
#include "GuiMetricShapeTwoSampleTTestDialog.h"
#include "GuiSurfaceToVolumeDialog.h"
#include "GuiShapeOrVectorsFromCoordinateSubtractionDialog.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "QtTableDialog.h"
#include "QtCheckBoxSelectionLineEditDialog.h"
#include "RgbPaintFile.h"
#include "StudyMetaDataFile.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "VocabularyFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowAttributesActions::GuiMainWindowAttributesActions(GuiMainWindow* parent)
   : QObject(parent)
{
   setObjectName("GuiMainWindowAttributesActions");
   
   generateTopographyAction = new QAction(parent);
   generateTopographyAction->setText("Generate Topography From Borders and Paint");
   //generateTopographyAction->setName("generateTopographyAction");
   QObject::connect(generateTopographyAction, SIGNAL(activated()),
                    this, SLOT(slotGenerateTopography()));

   generateArealEstimationMapAction = new QAction(parent);
   generateArealEstimationMapAction->setText("Generate Areal Estimation Map...");
   //generateArealEstimationMapAction->setName("generateArealEstimationMapAction");
   QObject::connect(generateArealEstimationMapAction, SIGNAL(activated()),
                    this, SLOT(slotGenerateArealEstimationMap()));

   displayParamsFileEditorAction = new QAction(parent);
   displayParamsFileEditorAction->setText("Edit...");
   //displayParamsFileEditorAction->setName("displayParamsFileEditorAction");
   QObject::connect(displayParamsFileEditorAction, SIGNAL(activated()),
                    parent, SLOT(displayParamsFileEditorDialog()));

   displayPaletteEditorAction = new QAction(parent);
   displayPaletteEditorAction->setText("Edit...");
   //displayPaletteEditorAction->setName("displayPaletteEditorAction");
   QObject::connect(displayPaletteEditorAction, SIGNAL(activated()),
                    parent, SLOT(displayPaletteEditorDialog()));

   areaColorsEditAction = new QAction(parent);
   areaColorsEditAction->setText("Edit...");
   //areaColorsEditAction->setName("areaColorsEditAction");
   QObject::connect(areaColorsEditAction, SIGNAL(activated()),
                    this, SLOT(slotAreaColorsEdit()));

   metricMathAction = new QAction(parent);
   metricMathAction->setText("Mathematical Operations...");
   //metricMathAction->setName("metricMathAction");
   QObject::connect(metricMathAction, SIGNAL(activated()),
                    parent, SLOT(displayMetricMathDialog()));

   metricToRgbPaintAction = new QAction(parent);
   metricToRgbPaintAction->setText("Convert Metric to RGB Paint...");
   //metricToRgbPaintAction->setName("metricToRgbPaintAction");
   QObject::connect(metricToRgbPaintAction, SIGNAL(activated()),
                    this, SLOT(slotMetricToRgbPaint()));

   metricAverageDeviationAction = new QAction(parent);
   metricAverageDeviationAction->setText("Compute Average and Sample Standard Deviation of All Columns...");
   //metricAverageDeviationAction->setName("metricAverageDeviationAction");
   QObject::connect(metricAverageDeviationAction, SIGNAL(activated()),
                    this, SLOT(slotMetricAverageDeviation()));

   metricToVolumeAction = new QAction(parent);
   metricToVolumeAction->setText("Convert Metric Column to Functional Volume...");
   //metricToVolumeAction->setName("metricToVolumeAction");
   QObject::connect(metricToVolumeAction, SIGNAL(activated()),
                    this, SLOT(slotMetricToVolume()));

   metricClearAllOrPartAction = new QAction(parent);
   metricClearAllOrPartAction->setText("Clear All or Part of Metric File...");
   //metricClearAllOrPartAction->setName("metricClearAllOrPartAction");
   QObject::connect(metricClearAllOrPartAction, SIGNAL(activated()),
                    this, SLOT(slotMetricClearAllOrPart()));

   volumeToSurfaceMapperAction = new QAction(parent);
   volumeToSurfaceMapperAction->setText("Map Volume(s) to Surface(s)...");
   //volumeToSurfaceMapperAction->setName("volumeToSurfaceMapperAction");
   QObject::connect(volumeToSurfaceMapperAction, SIGNAL(activated()),
                    this, SLOT(slotVolumeToSurfaceMapper()));

   metricModificationAction = new QAction(parent);
   metricModificationAction->setText("Clustering and Smoothing...");
   //metricModificationAction->setName("metricModificationAction");
   QObject::connect(metricModificationAction, SIGNAL(activated()),
                    this, SLOT(slotMetricModification()));

   surfaceShapeToVolumeAction = new QAction(parent);
   surfaceShapeToVolumeAction->setText("Convert Surface Shape Column to Functional Volume...");
   //surfaceShapeToVolumeAction->setName("surfaceShapeToVolumeAction");
   QObject::connect(surfaceShapeToVolumeAction, SIGNAL(activated()),
                    this, SLOT(slotSurfaceShapeToVolume()));

   surfaceShapeClearAllOrPartAction = new QAction(parent);
   surfaceShapeClearAllOrPartAction->setText("Clear All or Part of Surface Shape File...");
   //surfaceShapeClearAllOrPartAction->setName("surfaceShapeClearAllOrPartAction");
   QObject::connect(surfaceShapeClearAllOrPartAction, SIGNAL(activated()),
                    this, SLOT(slotSurfaceShapeClearAllOrPart()));

   surfaceShapeAverageDeviationAction = new QAction(parent);
   surfaceShapeAverageDeviationAction->setText("Compute Average and Sample Standard Deviation of All Columns...");
   //surfaceShapeAverageDeviationAction->setName("surfaceShapeAverageDeviationAction");
   QObject::connect(surfaceShapeAverageDeviationAction, SIGNAL(activated()),
                    this, SLOT(slotSurfaceShapeAverageDeviation()));

   shapeMathAction = new QAction(parent);
   shapeMathAction->setText("Mathematical Operations...");
   //shapeMathAction->setName("shapeMathAction");
   QObject::connect(shapeMathAction, SIGNAL(activated()),
                    parent, SLOT(displayShapeMathDialog()));

   shapeModificationAction = new QAction(parent);
   shapeModificationAction->setText("Clustering and Smoothing...");
   //shapeModificationAction->setName("shapeModificationAction");
   QObject::connect(shapeModificationAction, SIGNAL(activated()),
                    this, SLOT(slotShapeModification()));

   metricShapeCoordinateDifferenceAction = new QAction(parent);
   metricShapeCoordinateDifferenceAction->setText("Coordinate Difference...");
   //metricShapeCoordinateDifferenceAction->setName("metricShapeCoordinateDifferenceAction");
   QObject::connect(metricShapeCoordinateDifferenceAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeCoordinateDifference()));

   metricShapeNormalizationAction = new QAction(parent);
   metricShapeNormalizationAction->setText("Normalization of File...");
   //metricShapeNormalizationAction->setName("shapeNormalizationAction");
   QObject::connect(metricShapeNormalizationAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeNormalization()));

   metricShapeRootMeanSquareAction = new QAction(parent);
   metricShapeRootMeanSquareAction->setText("Root Mean Square...");
   QObject::connect(metricShapeRootMeanSquareAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeRootMeanSquare()));
                    
   metricShapeLeveneMapAction = new QAction(parent);
   metricShapeLeveneMapAction->setText("Levene Map on File...");
   //metricShapeLeveneMapAction->setName("metricShapeLeveneMapAction");
   QObject::connect(metricShapeLeveneMapAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeLeveneMap()));

   metricShapeZMapAction = new QAction(parent);
   metricShapeZMapAction->setText("Z-Map on File...");
   //shapeZMapAction->setName("shapeZMapAction");
   QObject::connect(metricShapeZMapAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeZMap()));

   metricShapeTMapAction = new QAction(parent);
   metricShapeTMapAction->setText("T-Map on Files...");
   //shapeTMapAction->setName("shapeTMapAction");
   QObject::connect(metricShapeTMapAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeTMap()));

   metricShapeSubtractAverageAction = new QAction(parent);
   metricShapeSubtractAverageAction->setText("Subtract Group Average on Files...");
   //shapeSubtractAverageAction->setName("shapeSubtractAverageAction");
   QObject::connect(metricShapeSubtractAverageAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeSubtractAverage()));

   metricShapeTMapShuffledColumnsAction = new QAction(parent);
   metricShapeTMapShuffledColumnsAction->setText("T-Maps on Shuffled Columns on Files...");
   //shapeTMapShuffledColumnsAction->setName("shapeTMapShuffledColumnsAction");
   QObject::connect(metricShapeTMapShuffledColumnsAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeTMapShuffledColumns()));

   metricShapeShuffledCorrelationColumnsAction = new QAction(parent);
   metricShapeShuffledCorrelationColumnsAction->setText("Shuffled Cross-Correlation Maps...");
   //shapeShuffledCorrelationColumnsAction->setName("shapeShuffledCorrelationColumnsAction");
   QObject::connect(metricShapeShuffledCorrelationColumnsAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeShuffledCorrelationColumns()));

   metricShapeOneSampleTTestAction = new QAction(parent);
   metricShapeOneSampleTTestAction->setText("One Sample T-Test...");
   //shapeInterHemClustersAction->setName("shapeInterHemClustersAction");
   QObject::connect(metricShapeOneSampleTTestAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeOneSampleTTestDialog()));

   metricShapePairedTTestAction = new QAction(parent);
   metricShapePairedTTestAction->setText("Paired T-Test...");
   //shapeInterHemClustersAction->setName("shapeInterHemClustersAction");
   QObject::connect(metricShapePairedTTestAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapePairedTTestDialog()));

   metricShapeInterHemClustersAction = new QAction(parent);
   metricShapeInterHemClustersAction->setText("Interhemispheric Clusters...");
   //shapeInterHemClustersAction->setName("shapeInterHemClustersAction");
   QObject::connect(metricShapeInterHemClustersAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeInterHemClustersDialog()));

   metricShapeTwoSampleTTestAction = new QAction(parent);
   metricShapeTwoSampleTTestAction->setText("Two Sample T-Test...");
   //shapeSignificantClustersAction->setName("shapeSignificantClustersAction");
   QObject::connect(metricShapeTwoSampleTTestAction, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeTwoSampleTTestDialog()));

   metricShapeWilcoxonRankSum = new QAction(parent);
   metricShapeWilcoxonRankSum->setText("Wilcoxon Rank-Sum Test...");
   QObject::connect(metricShapeWilcoxonRankSum, SIGNAL(activated()),
                    this, SLOT(slotMetricShapeWilcoxonRankSumDialog()));

   paintCleanNamesAction = new QAction(parent);
   paintCleanNamesAction->setText("Cleanup Paint Names");
   //paintCleanNamesAction->setName("paintCleanNamesAction");
   QObject::connect(paintCleanNamesAction, SIGNAL(activated()),
                    this, SLOT(slotPaintCleanNames()));

   paintToVolumeAction = new QAction(parent);
   paintToVolumeAction->setText("Convert Paint Column to Paint Volume...");
   //paintToVolumeAction->setName("paintToVolumeAction");
   QObject::connect(paintToVolumeAction, SIGNAL(activated()),
                    this, SLOT(slotPaintToVolume()));

   paintNamesEditAction = new QAction(parent);
   paintNamesEditAction->setText("Edit Paint Names...");
   //paintNamesEditAction->setName("paintNamesEditAction");
   QObject::connect(paintNamesEditAction, SIGNAL(activated()),
                    this, SLOT(slotPaintNamesEdit()));

   paintAssignWithinDisplayedBordersAction = new QAction(parent);
   paintAssignWithinDisplayedBordersAction->setText("Assign Nodes Within Displayed Borders...");
   //paintAssignWithinDisplayedBordersAction->setName("paintAssignWithinDisplayedBordersAction");
   QObject::connect(paintAssignWithinDisplayedBordersAction, SIGNAL(activated()),
                    this, SLOT(slotPaintAssignWithinDisplayedBorders()));

   paintClearAllOrPartAction = new QAction(parent);
   paintClearAllOrPartAction->setText("Clear All or Part of Paint File...");
   //paintClearAllOrPartAction->setName("paintClearAllOrPartAction");
   QObject::connect(paintClearAllOrPartAction, SIGNAL(activated()),
                    this, SLOT(slotPaintClearAllOrPart()));

   copyColoringToRgbPaintAction = new QAction(parent);
   copyColoringToRgbPaintAction->setText("Copy Current Coloring to RGB Paint...");
   //copyColoringToRgbPaintAction->setName("copyColoringToRgbPaintAction");
   QObject::connect(copyColoringToRgbPaintAction, SIGNAL(activated()),
                    this, SLOT(slotCopyColoringToRgbPaint()));

   copyNormalsToRgbPaintAction = new QAction(parent);
   copyNormalsToRgbPaintAction->setText("Copy Surface Normals to RGB Paint");
   //copyNormalsToRgbPaintAction->setName("copyNormalsToRgbPaintAction");
   QObject::connect(copyNormalsToRgbPaintAction, SIGNAL(activated()),
                    this, SLOT(slotCopyNormalsToRgbPaint()));

   copyColoringToVolumeAction = new QAction(parent);
   copyColoringToVolumeAction->setText("Copy Current Coloring to RGB Volume...");
   //copyColoringToVolumeAction->setName("copyColoringToVolumeAction");
   QObject::connect(copyColoringToVolumeAction, SIGNAL(activated()),
                    this, SLOT(slotCopyColoringToVolume()));

   latLonClearAllOrPartAction = new QAction(parent);
   latLonClearAllOrPartAction->setText("Clear All or Part of Lat/Long File...");
   //latLonClearAllOrPartAction->setName("latLonClearAllOrPartAction");
   QObject::connect(latLonClearAllOrPartAction, SIGNAL(activated()),
                    this, SLOT(slotLatLonClearAllOrPart()));

   latLonGenerateOnSphereAction = new QAction(parent);
   latLonGenerateOnSphereAction->setText("Generate Lat/Long on Spherical Surface...");
   //latLonGenerateOnSphereAction->setName("latLonGenerateOnSphereAction");
   QObject::connect(latLonGenerateOnSphereAction, SIGNAL(activated()),
                    this, SLOT(slotLatLonGenerateOnSphere()));

   generateDeformationFieldAction = new QAction(parent);
   generateDeformationFieldAction->setText("Generate Deformation Field...");
   //generateDeformationFieldAction->setName("generateDeformationFieldAction");
   QObject::connect(generateDeformationFieldAction, SIGNAL(activated()),
                    this, SLOT(slotGenerateDeformationField()));

   probAtlasThresholdToPaintAction = new QAction(parent);
   probAtlasThresholdToPaintAction->setText("Convert Threshold Coloring to Paint...");
   //probAtlasThresholdToPaintAction->setName("probAtlasThresholdToPaintAction");
   QObject::connect(probAtlasThresholdToPaintAction, SIGNAL(activated()),
                    this, SLOT(slotProbAtlasThresholdToPaint()));

   coordsToVectorsAction = new QAction(parent);
   coordsToVectorsAction->setText("Create Vectors From Surface Subtraction...");
   //coordsToVectorsAction->setName("coordsToVectorsAction");
   QObject::connect(coordsToVectorsAction, SIGNAL(activated()),
                    this, SLOT(slotCoordsToVectors()));

   normalsToVectorsAction = new QAction(parent);
   normalsToVectorsAction->setText("Create Vectors From Surface Normals...");
   QObject::connect(normalsToVectorsAction, SIGNAL(activated()),
                    this, SLOT(slotNormalsToVectors()));
                    
   modelsEditorAction = new QAction(parent);
   modelsEditorAction->setText("Edit...");
   //modelsEditorAction->setName("modelsEditorAction");
   QObject::connect(modelsEditorAction, SIGNAL(activated()),
                    parent, SLOT(displayModelsEditorDialog()));
         
   studyMetaDataEditorDialogAction = new QAction(parent);
   studyMetaDataEditorDialogAction->setText("Edit...");
   QObject::connect(studyMetaDataEditorDialogAction, SIGNAL(activated()),
                    parent, SLOT(displayStudyMetaDataFileEditorDialog()));
                    
   vocabularyFileEditorDialogAction = new QAction(parent);
   vocabularyFileEditorDialogAction->setText("Edit...");
   QObject::connect(vocabularyFileEditorDialogAction, SIGNAL(activated()),
                    parent, SLOT(displayVocabularyFileEditorDialog()));
                    
   vocabularyMoveStudyInfoToStudyMetaDataAction = new QAction(parent);
   vocabularyMoveStudyInfoToStudyMetaDataAction->setText("Move Vocabulary Study Info to Study Metadata File");
   QObject::connect(vocabularyMoveStudyInfoToStudyMetaDataAction, SIGNAL(activated()),
                    this, SLOT(slotVocabularyMoveStudyInfoToStudyMetaData()));
}

/**
 * Destructor.
 */
GuiMainWindowAttributesActions::~GuiMainWindowAttributesActions()
{
}

/**
 * convert vocabulary study info to study metadata file.
 */
void 
GuiMainWindowAttributesActions::slotVocabularyMoveStudyInfoToStudyMetaData()
{
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
   smdf->append(*vf);

   GuiFilesModified fm;
   fm.setFociModified();
   fm.setVocabularyModified();
   theMainWindow->fileModificationUpdate(fm);

   const QString msg("The StudyInfo from the Vocabulary File has been moved\n"
                     "to the Study Metadata File.  As a result, both the \n"
                     "StudyMetaData and Vocabulary Files need to be saved.");
   GuiMessageBox::information(theMainWindow, "INFO", msg, "OK");
}

/**
 * Called to map volume data to a surface data file.
 */
void
GuiMainWindowAttributesActions::slotVolumeToSurfaceMapper()
{
   static GuiMapFmriDialog* mfd = NULL;
   if (mfd == NULL) {
      mfd = new GuiMapFmriDialog(theMainWindow, theMainWindow->getBrainSet(), true, false);
      QObject::connect(mfd, SIGNAL(signalMappingComplete()),
                       this, SLOT(slotVolumeToSurfaceMapperComplete()));
   }
   mfd->initializeDialog();
   mfd->show();
}

/**
 * slot for volume to surface mapper complete.
 */
void 
GuiMainWindowAttributesActions::slotVolumeToSurfaceMapperComplete()
{
   GuiFilesModified fm;
   fm.setStatusForAll(true);
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL(NULL);
}
      
/**
 * generate a deformation field.
 */
void 
GuiMainWindowAttributesActions::slotGenerateDeformationField()
{
   GuiDeformationFieldDialog gdfd(theMainWindow);
   gdfd.exec();
}      

/**
 * Generate topography.
 */
void
GuiMainWindowAttributesActions::slotGenerateTopography()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   BorderFile borders;
   bmbs->copyBordersToBorderFile(bms, borders);
   BorderToTopographyConverter btc(theMainWindow->getBrainSet(),
                                   bms,
                                   &borders,
                                   theMainWindow->getBrainSet()->getPaintFile(),
                                   theMainWindow->getBrainSet()->getTopographyFile(),
                                   -1,
                                   "Topography");
   try {
      btc.execute();
   }
   catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(theMainWindow, "Error", e.whatQString(), "OK");
   }
}

/**
 * Generate an areal estimation map.
 */
void
GuiMainWindowAttributesActions::slotGenerateArealEstimationMap()
{
   GuiGenerateArealEstimationDialog aed(theMainWindow);
   aed.exec();
}

/**
 * Edit and Add area colors
 */
void
GuiMainWindowAttributesActions::slotAreaColorsEdit()
{
   GuiColorFileEditorDialog* gfed = new GuiColorFileEditorDialog(theMainWindow,
                                                                 theMainWindow->getBrainSet()->getAreaColorFile(),
                                                                 false, 
                                                                 false,
                                                                 false,
                                                                 false,
                                                                 false);
   QObject::connect(gfed, SIGNAL(redrawRequested()),
                    this, SLOT(slotAreaColorsChanged()));
   gfed->show();
}

/**
 * This slot is called by the color editor when user pressed apply or dialog isclosed.
 */
void
GuiMainWindowAttributesActions::slotAreaColorsChanged()
{
   GuiFilesModified fm;
   fm.setAreaColorModified();
   theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Called to modify metric data.
 */
void
GuiMainWindowAttributesActions::slotMetricModification()
{
   theMainWindow->getMetricModificationDialog(true);
}

/**
 * Clear all or part of the file.
 */
void
GuiMainWindowAttributesActions::slotMetricClearAllOrPart()
{
   GuiFilesModified fm;
   fm.setMetricModified();
   GuiNodeAttributeFileClearResetDialog d(theMainWindow, GUI_NODE_FILE_TYPE_METRIC);
   d.exec();
}

/**
 * Called to create a volume using a metric column.
 */
void
GuiMainWindowAttributesActions::slotMetricToVolume()
{
   GuiSurfaceToVolumeDialog svd(theMainWindow, GuiSurfaceToVolumeDialog::DIALOG_MODE_METRIC,
                                "Copy Surface Metric Column to Functional Volume");
   if (svd.exec() == QDialog::Accepted) {
      const int metricColumn = svd.getSelectedNodeAttributeColumn();
      if ((metricColumn < 0) || (metricColumn >= theMainWindow->getBrainSet()->getMetricFile()->getNumberOfColumns())) {
         GuiMessageBox::critical(theMainWindow, "Error", "Invalid metric Column", "OK");
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      float offset[3];
      float voxelSize[3];
      float origin[3];
      int dim[3];
      svd.getSurfaceOffset(offset);
      svd.getVolumeDimensions(dim);
      svd.getVolumeVoxelSizes(voxelSize);
      svd.getVolumeOrigin(origin);
      BrainModelSurfaceToVolumeConverter stv(theMainWindow->getBrainSet(),
                                             svd.getSelectedSurface(),
                                             svd.getStandardVolumeSpace(),
                                             offset,
                                             dim,
                                             voxelSize,
                                             origin,
                                             svd.getSurfaceInnerBoundary(),
                                             svd.getSurfaceOuterBoundary(),
                                             svd.getSurfaceThicknessStep(),
                                             svd.getMetricConversionMode());
      stv.setNodeAttributeColumn(metricColumn);
      stv.setNodeToVoxelMappingEnabled(svd.getNodeToVoxelMappingEnabled(),
                                       svd.getNodeToVoxelMappingFileName());
      
      try {
         stv.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(theMainWindow, "Error", e.whatQString(), "OK");
         return;
      }
      
      theMainWindow->speakText("The metric data has been converted to a volume.", false);
      
      GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
      openGL->displayBrainModelVolume();
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called to display metric to rgb paint dialog
 */
void
GuiMainWindowAttributesActions::slotMetricToRgbPaint()
{
   theMainWindow->getMetricsToRgbPaintDialog(true);
}

/**
 * Called to compute average and deviation of metrics.
 */
void
GuiMainWindowAttributesActions::slotMetricAverageDeviation()
{
   std::vector<bool> checkBoxValues;
   std::vector<QString> lineEditValues;
   
   checkBoxValues.push_back(true); lineEditValues.push_back("Mean");
   checkBoxValues.push_back(true); lineEditValues.push_back("Sample Standard Deviation");
   checkBoxValues.push_back(true); lineEditValues.push_back("Standard Error");
   checkBoxValues.push_back(true); lineEditValues.push_back("Minimum Absolute Value");
   checkBoxValues.push_back(true); lineEditValues.push_back("Maximum Absolute Value");
   
   QtCheckBoxSelectionLineEditDialog cbsled(theMainWindow,
                                            "Metric Statistics",
                                            "Choose Statistical Measurements",
                                            checkBoxValues,
                                            lineEditValues);
   if (cbsled.exec() == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      QString meanName, devName, errorName, minAbsName, maxAbsName;
      if (cbsled.getCheckBoxStatus(0)) {
         meanName = cbsled.getLineEditValue(0);
      }
      if (cbsled.getCheckBoxStatus(1)) {
         devName = cbsled.getLineEditValue(1);
      }
      if (cbsled.getCheckBoxStatus(2)) {
         errorName = cbsled.getLineEditValue(2);
      }
      if (cbsled.getCheckBoxStatus(3)) {
         minAbsName = cbsled.getLineEditValue(3);
      }
      if (cbsled.getCheckBoxStatus(4)) {
         maxAbsName = cbsled.getLineEditValue(4);
      }
      
      MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
      mf->computeStatistics(meanName,
                             devName,
                             errorName,
                             minAbsName,
                             maxAbsName);
                             
      theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
      GuiBrainModelOpenGL::updateAllGL(NULL);
      GuiFilesModified fm;
      fm.setMetricModified();
      theMainWindow->fileModificationUpdate(fm);
      QApplication::restoreOverrideCursor();
   }
}

/**
 * convert prob atlas threshold coloring to paint.
 */
void 
GuiMainWindowAttributesActions::slotProbAtlasThresholdToPaint()
{
   ProbabilisticAtlasFile* probAtlasFile = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   if (probAtlasFile->getNumberOfColumns() < 1) {
      GuiMessageBox::critical(theMainWindow, "ERROR", "Probabilistic Atlas File is Empty.", "OK");
      return;
   }
   
   //
   // Get node coloring
   //
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   
   //
   // An overlay or underlay must be prob atlas
   //
   bool error = false;
   if ((bsnc->getPrimaryOverlay(0) != BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS) &&
       (bsnc->getSecondaryOverlay(0) != BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS) &&
       (bsnc->getUnderlay(0) != BrainModelSurfaceNodeColoring::UNDERLAY_PROBABILISTIC_ATLAS)) {
      error = true;
   }
   
   //
   // Threshold display mode must be set for prob atlas
   //
   DisplaySettingsProbabilisticAtlas* dspa = theMainWindow->getBrainSet()->getDisplaySettingsProbabilisticAtlasSurface();
   if (dspa->getDisplayType() != 
       DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_THRESHOLD) {
      error = true;
   }
   
   if (error) {
      GuiMessageBox::critical(theMainWindow, "ERROR",
         "The overlay or underlay must be set to Probabilistic\n"
         "Atlas with the Display Mode set to Threshold.", "OK");
      return;
   }
   
   //
   // Use dialog to choose paint column and its name
   //
   GuiChooseNodeAttributeColumnDialog cacd(theMainWindow,
                                           GUI_NODE_FILE_TYPE_PAINT,
                                           "",
                                           true,
                                           false);
   if (cacd.exec() == QDialog::Accepted) {
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      //
      // Modify paint file for new column or update
      //
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      int columnNumber = cacd.getSelectedColumnNumber();
      if (pf->getNumberOfNodes() == 0) {
         pf->setNumberOfNodesAndColumns(theMainWindow->getBrainSet()->getNumberOfNodes(), 1);
         columnNumber = 0;
      }
      else if (columnNumber < 0) {
         pf->addColumns(1);
         columnNumber = pf->getNumberOfColumns() - 1;
      }
      pf->setColumnName(columnNumber, cacd.getColumnName());
            
      //
      // Update paint file with colors
      //
      bsnc->addProbAtlasThresholdingToPaintFile(pf, columnNumber);
      
      GuiFilesModified fm;
      fm.setPaintModified();
      theMainWindow->fileModificationUpdate(fm);
      bsnc->assignColors();
      GuiBrainModelOpenGL::updateAllGL(NULL);
      
      QApplication::restoreOverrideCursor();
   }
}      

/**
 * slot for assigning paints within displayed borders.
 */
void 
GuiMainWindowAttributesActions::slotPaintAssignWithinDisplayedBorders()
{
   //
   // Get the surface in the main window and its brain model index
   //
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      const int modelIndex = theMainWindow->getBrainSet()->getBrainModelIndex(bms);
      if (modelIndex >= 0) {
         //
         // Get the coordinates
         //
         CoordinateFile* cf = bms->getCoordinateFile();
         const float* coords = cf->getCoordinate(0);
         const int numCoords = bms->getNumberOfNodes();
         
         //
         // Allocate the inside flags
         //
         std::vector<bool> nodeInsideFlag(numCoords);
         
         //
         // Get the paint file
         //
         PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
         
         //
         // Popup a dialog to choose the paint column that is to be assigned
         //
         GuiChooseNodeAttributeColumnDialog nacd(theMainWindow,
                                                 GUI_NODE_FILE_TYPE_PAINT,
                                                 "",
                                                 true,
                                                 false);
         nacd.setWindowTitle("Choose Paint Column");
         if (nacd.exec() == QDialog::Accepted) {
         
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            
            //
            // Modify paint file for column change
            //
            int paintColumn = nacd.getSelectedColumnNumber();
            if (paintColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
               if (pf->getNumberOfColumns() <= 0) {
                  pf->setNumberOfNodesAndColumns(numCoords, 1);
               }
               else {
                  pf->addColumns(1);
               }
               paintColumn = pf->getNumberOfColumns() - 1;
            }
            pf->setColumnName(paintColumn, nacd.getColumnName());
            
            //
            // Find borders that are valid for this surface and displayed
            //
            BrainModelBorderSet* borderSet = theMainWindow->getBrainSet()->getBorderSet();
            const int numBorders = borderSet->getNumberOfBorders();
            for (int i = 0; i < numBorders; i++) {
               BrainModelBorder* border = borderSet->getBorder(i);
               if (border->getDisplayFlag() && 
                  border->getValidForBrainModel(modelIndex)) {
                  //
                  // Find nodes within the border
                  //
                  border->pointsInsideBorder(bms, 
                                             coords,
                                             numCoords,
                                             nodeInsideFlag,
                                             false);
                                             
                  //
                  // Assign paint nodes for nodes that are within the border
                  //
                  int paintIndex = -1;
                  for (int j = 0; j < numCoords; j++) {
                     if (nodeInsideFlag[j]) {
                        if (paintIndex < 0) {
                           paintIndex = pf->addPaintName(border->getName());
                        }
                        pf->setPaint(j, paintColumn, paintIndex);
                     }
                  }
               }
            }
            
            //
            // Assign colors and update graphics
            //
            GuiFilesModified fm;
            fm.setPaintModified();
            theMainWindow->fileModificationUpdate(fm);
            theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
            GuiBrainModelOpenGL::updateAllGL(NULL);
            QApplication::restoreOverrideCursor();
         }
      }
   }
}

/**
 * Called to edit paint names.
 */
void
GuiMainWindowAttributesActions::slotPaintNamesEdit()
{
   GuiPaintNamesEditDialog pned(theMainWindow);
   pned.exec();
}

/**
 * Clear all or part of the paint file.
 */
void
GuiMainWindowAttributesActions::slotPaintClearAllOrPart()
{
   GuiFilesModified fm;
   fm.setPaintModified();
   GuiNodeAttributeFileClearResetDialog d(theMainWindow, GUI_NODE_FILE_TYPE_PAINT);
   d.exec();
}

/**
 * Called to create a volume using a paint column.
 */
void
GuiMainWindowAttributesActions::slotPaintToVolume()
{
   GuiSurfaceToVolumeDialog svd(theMainWindow, GuiSurfaceToVolumeDialog::DIALOG_MODE_PAINT,
                                "Copy Surface Paint Column to Paint Volume");
   if (svd.exec() == QDialog::Accepted) {
      const int paintColumn = svd.getSelectedNodeAttributeColumn();
      if ((paintColumn < 0) || (paintColumn >= theMainWindow->getBrainSet()->getPaintFile()->getNumberOfColumns())) {
         GuiMessageBox::critical(theMainWindow, "Error", "Invalid paint Column", "OK");
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      float offset[3];
      float voxelSize[3];
      float origin[3];
      int dim[3];
      svd.getSurfaceOffset(offset);
      svd.getVolumeDimensions(dim);
      svd.getVolumeVoxelSizes(voxelSize);
      svd.getVolumeOrigin(origin);
      BrainModelSurfaceToVolumeConverter stv(theMainWindow->getBrainSet(),
                                             svd.getSelectedSurface(),
                                             svd.getStandardVolumeSpace(),
                                             offset,
                                             dim,
                                             voxelSize,
                                             origin,
                                             svd.getSurfaceInnerBoundary(),
                                             svd.getSurfaceOuterBoundary(),
                                             svd.getSurfaceThicknessStep(),
                   BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_PAINT);
      stv.setNodeAttributeColumn(paintColumn);
      stv.setNodeToVoxelMappingEnabled(svd.getNodeToVoxelMappingEnabled(),
                                       svd.getNodeToVoxelMappingFileName());

      try {
         stv.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(theMainWindow, "Error", e.whatQString(), "OK");
         return;
      }
      
      theMainWindow->speakText("The paint data has been converted to a volume.", false);
      
      GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
      openGL->displayBrainModelVolume();
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called to copy current coloring to a volume.
 */
void
GuiMainWindowAttributesActions::slotCopyColoringToVolume()
{
   GuiSurfaceToVolumeDialog svd(theMainWindow, GuiSurfaceToVolumeDialog::DIALOG_MODE_NORMAL,
                                "Copy Coloring to RGB Volume...");
   if (svd.exec() == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      float offset[3];
      float voxelSize[3];
      float origin[3];
      int dim[3];
      svd.getSurfaceOffset(offset);
      svd.getVolumeDimensions(dim);
      svd.getVolumeVoxelSizes(voxelSize);
      svd.getVolumeOrigin(origin);
      BrainModelSurfaceToVolumeConverter stv(theMainWindow->getBrainSet(),
                                             svd.getSelectedSurface(),
                                             svd.getStandardVolumeSpace(),
                                             offset,
                                             dim,
                                             voxelSize,
                                             origin,
                                             svd.getSurfaceInnerBoundary(),
                                             svd.getSurfaceOuterBoundary(),
                                             svd.getSurfaceThicknessStep(),
                   BrainModelSurfaceToVolumeConverter::CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING);
      stv.setNodeToVoxelMappingEnabled(svd.getNodeToVoxelMappingEnabled(),
                                       svd.getNodeToVoxelMappingFileName());

      try {
         stv.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(theMainWindow, "Error", e.whatQString(), "OK");
         return;
      }
      
      theMainWindow->speakText("The R G B data has been converted to a volume.", false);
      
      GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
      openGL->displayBrainModelVolume();
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called to copy current coloring to RGB Paint.
 */
void
GuiMainWindowAttributesActions::slotCopyColoringToRgbPaint()
{
   GuiCurrentColoringToRgbPaintDialog* d = new GuiCurrentColoringToRgbPaintDialog(theMainWindow);
   d->exec();
}

/**
 * Called to copy surface normals to RGB Paint.
 */
void
GuiMainWindowAttributesActions::slotCopyNormalsToRgbPaint()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   bms->convertNormalsToRgbPaint(theMainWindow->getBrainSet()->getRgbPaintFile());
   GuiFilesModified fm;
   fm.setRgbPaintModified();
   theMainWindow->fileModificationUpdate(fm);
}

/**
 * Called to clean paint file names
 */
void
GuiMainWindowAttributesActions::slotPaintCleanNames()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   pf->cleanUpPaintNames();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
}

/**
 * slot for shuffled correlation columns.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeShuffledCorrelationColumns()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_SHUFF_CROSS_CORRELATION);
   mssd.exec();
}
      
/**
 * slot for shape normalization.
 */
void
GuiMainWindowAttributesActions::slotMetricShapeNormalization()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_NORMALIZE);
   mssd.exec();
}
      
/**
 * slot for metric and shape root mean square.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeRootMeanSquare()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_ROOT_MEAN_SQUARE);
   mssd.exec();
}
      
/**
 * slot for shape Levene Map computation.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeLeveneMap()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_LEVENE);
   mssd.exec();
}

/**
 * slot for shape Z-Map computation.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeZMap()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_ZMAP);
   mssd.exec();
}

/**
 * slot for shape T-Map computation.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeTMap()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_TMAP);
   mssd.exec();
}
      
/**
 * slot for shape subtract average computation.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeSubtractAverage()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_SUBTRACT_GROUP_AVERAGE);
   mssd.exec();
}

/**
 * slot for shape T-Map shuffled columns.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeTMapShuffledColumns()
{
   GuiMetricShapeStatisticsDialog mssd(theMainWindow,
                                       GuiMetricShapeStatisticsDialog::DIALOG_MODE_SHUFFLED_TMAP);
   mssd.exec();
}
      
/**
 * Called to modify metric data.
 */
void
GuiMainWindowAttributesActions::slotShapeModification()
{
   theMainWindow->getShapeModificationDialog(true);
}

/**
 * Called to compute average and deviation of metrics.
 */
void
GuiMainWindowAttributesActions::slotSurfaceShapeAverageDeviation()
{
   std::vector<bool> checkBoxValues;
   std::vector<QString> lineEditValues;
   
   checkBoxValues.push_back(true); lineEditValues.push_back("Mean");
   checkBoxValues.push_back(true); lineEditValues.push_back("Sample Standard Deviation");
   checkBoxValues.push_back(true); lineEditValues.push_back("Standard Error");
   checkBoxValues.push_back(true); lineEditValues.push_back("Minimum Absolute Value");
   checkBoxValues.push_back(true); lineEditValues.push_back("Maximum Absolute Value");
   
   QtCheckBoxSelectionLineEditDialog cbsled(theMainWindow,
                                            "Surface Shape Statistics",
                                            "Choose Statistical Measurements",
                                            checkBoxValues,
                                            lineEditValues);
   if (cbsled.exec() == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      QString meanName, devName, errorName, minAbsName, maxAbsName;
      if (cbsled.getCheckBoxStatus(0)) {
         meanName = cbsled.getLineEditValue(0);
      }
      if (cbsled.getCheckBoxStatus(1)) {
         devName = cbsled.getLineEditValue(1);
      }
      if (cbsled.getCheckBoxStatus(2)) {
         errorName = cbsled.getLineEditValue(2);
      }
      if (cbsled.getCheckBoxStatus(3)) {
         minAbsName = cbsled.getLineEditValue(3);
      }
      if (cbsled.getCheckBoxStatus(4)) {
         maxAbsName = cbsled.getLineEditValue(4);
      }
      
      SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
      ssf->computeStatistics(meanName,
                             devName,
                             errorName,
                             minAbsName,
                             maxAbsName);
                             
      theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
      GuiBrainModelOpenGL::updateAllGL(NULL);
      GuiFilesModified fm;
      fm.setSurfaceShapeModified();
      theMainWindow->fileModificationUpdate(fm);
      QApplication::restoreOverrideCursor();
   }
}


/**
 * Clear all or part of the surface shape file.
 */
void
GuiMainWindowAttributesActions::slotSurfaceShapeClearAllOrPart()
{
   GuiFilesModified fm;
   fm.setSurfaceShapeModified();
   GuiNodeAttributeFileClearResetDialog d(theMainWindow, GUI_NODE_FILE_TYPE_SURFACE_SHAPE);
   d.exec();
}

/**
 * Called to create a volume using a surface shape column.
 */
void
GuiMainWindowAttributesActions::slotSurfaceShapeToVolume()
{
   GuiSurfaceToVolumeDialog svd(theMainWindow, GuiSurfaceToVolumeDialog::DIALOG_MODE_SURFACE_SHAPE,
                                "Copy Surface Shape Column to Functional Volume");
   if (svd.exec() == QDialog::Accepted) {
      const int shapeColumn = svd.getSelectedNodeAttributeColumn();
      if ((shapeColumn < 0) || (shapeColumn >= theMainWindow->getBrainSet()->getSurfaceShapeFile()->getNumberOfColumns())) {
         GuiMessageBox::critical(theMainWindow, "Error", "Invalid surface shape Column", "OK");
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      float offset[3];
      float voxelSize[3];
      float origin[3];
      int dim[3];
      svd.getSurfaceOffset(offset);
      svd.getVolumeDimensions(dim);
      svd.getVolumeVoxelSizes(voxelSize);
      svd.getVolumeOrigin(origin);
      BrainModelSurfaceToVolumeConverter stv(theMainWindow->getBrainSet(),
                                             svd.getSelectedSurface(),
                                             svd.getStandardVolumeSpace(),
                                             offset,
                                             dim,
                                             voxelSize,
                                             origin,
                                             svd.getSurfaceInnerBoundary(),
                                             svd.getSurfaceOuterBoundary(),
                                             svd.getSurfaceThicknessStep(),
                   BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE);
      stv.setNodeAttributeColumn(shapeColumn);
      stv.setNodeToVoxelMappingEnabled(svd.getNodeToVoxelMappingEnabled(),
                                       svd.getNodeToVoxelMappingFileName());

      try {
         stv.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(theMainWindow, "Error", e.whatQString(), "OK");
         return;
      }
      
      theMainWindow->speakText("The surface shape data has been converted to a volume.", false);
      
      GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
      openGL->displayBrainModelVolume();
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Clear all or part of the surface shape file.
 */
void
GuiMainWindowAttributesActions::slotLatLonClearAllOrPart()
{
   GuiFilesModified fm;
   fm.setLatLonModified();
   GuiNodeAttributeFileClearResetDialog d(theMainWindow, GUI_NODE_FILE_TYPE_LAT_LON);
   d.exec();
}

/**
 * Called to create a volume using a surface shape column.
 */
void
GuiMainWindowAttributesActions::slotLatLonGenerateOnSphere()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      GuiMessageBox::critical(theMainWindow, "Error", "There is no surface in the main window.", "OK");
      return;
   }
   
   if (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
      if (GuiMessageBox::warning(theMainWindow, "Warning", "The surface in the main window does\n"
                                                "not appear to be a sphere.", 
                                                "Continue", "Cancel") != 0) {
         return;
      }
   }
   
   GuiChooseNodeAttributeColumnDialog cnacd(theMainWindow,
                                            GUI_NODE_FILE_TYPE_LAT_LON,
                                            "",
                                            true,
                                            false);
   if (cnacd.exec() == QDialog::Accepted) {
      const int columnNumber = cnacd.getSelectedColumnNumber();
      const QString columnName = cnacd.getColumnName();
      bms->createLatitudeLongitude(theMainWindow->getBrainSet()->getLatLonFile(),
                                   columnNumber,
                                   columnName,
                                   false,
                                   true);
      GuiFilesModified fm;
      fm.setLatLonModified();
      theMainWindow->fileModificationUpdate(fm);
   }
}

/**
 * convert coord files to vectors.
 */
void 
GuiMainWindowAttributesActions::slotCoordsToVectors()
{
   GuiShapeOrVectorsFromCoordinateSubtractionDialog csd(theMainWindow,
                       GuiShapeOrVectorsFromCoordinateSubtractionDialog::MODE_VECTOR);
   csd.exec();
}

/**
 * convert normals to vectors.
 */
void 
GuiMainWindowAttributesActions::slotNormalsToVectors()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      GuiChooseNodeAttributeColumnDialog acd(theMainWindow,
                                             GUI_NODE_FILE_TYPE_SURFACE_VECTOR,
                                             QString(""),
                                             true,
                                             false);
      if (acd.exec() == GuiChooseNodeAttributeColumnDialog::Accepted) {
         bms->copyNormalsToSurfaceVectorFile(theMainWindow->getBrainSet()->getSurfaceVectorFile(),
                                             acd.getSelectedColumnNumber(),
                                             acd.getColumnName());
         GuiFilesModified fm;
         fm.setSurfaceVectorModified();
         theMainWindow->fileModificationUpdate(fm);
      }
   }
}

/**
 * slot for one-sample t-test dialog.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeOneSampleTTestDialog()
{
   static GuiMetricShapeOneAndPairedTTestDialog* shapeOneSampleTTestDialog = NULL;
   if (shapeOneSampleTTestDialog == NULL) {
      shapeOneSampleTTestDialog = new GuiMetricShapeOneAndPairedTTestDialog(theMainWindow, false);
   }
   shapeOneSampleTTestDialog->show();
   shapeOneSampleTTestDialog->raise();
   shapeOneSampleTTestDialog->activateWindow();
}      

/**
 * slot for two-sample t-test dialog.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapePairedTTestDialog()
{
   static GuiMetricShapeOneAndPairedTTestDialog* shapePairedTTestDialog = NULL;
   if (shapePairedTTestDialog == NULL) {
      shapePairedTTestDialog = new GuiMetricShapeOneAndPairedTTestDialog(theMainWindow, true);
   }
   shapePairedTTestDialog->show();
   shapePairedTTestDialog->raise();
   shapePairedTTestDialog->activateWindow();
}      

/**
 * slot for the shape significant clusters dialog.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeTwoSampleTTestDialog()
{
   static GuiMetricShapeTwoSampleTTestDialog* shapeSignificantClustersDialog = NULL;
   
   if (shapeSignificantClustersDialog == NULL) {
      shapeSignificantClustersDialog = new GuiMetricShapeTwoSampleTTestDialog(theMainWindow,
                  BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_NONE);
   }
   shapeSignificantClustersDialog->show();
   shapeSignificantClustersDialog->raise();
   shapeSignificantClustersDialog->activateWindow();
}
      
/**
 * slot for metric shape coordinate difference.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeCoordinateDifference()
{
   static GuiMetricShapeStatisticalAlgorithmDialog* statAlgDialog = NULL;
   if (statAlgDialog == NULL) {
      statAlgDialog = new GuiMetricShapeStatisticalAlgorithmDialog(theMainWindow);
   }
   statAlgDialog->show();
   statAlgDialog->raise();
   statAlgDialog->activateWindow();
}

/**
 * slot for the wilcoxon rank-sum dialog.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeWilcoxonRankSumDialog()
{
   static GuiMetricShapeTwoSampleTTestDialog* shapeSignificantClustersDialog = NULL;
   
   if (shapeSignificantClustersDialog == NULL) {
      shapeSignificantClustersDialog = new GuiMetricShapeTwoSampleTTestDialog(theMainWindow,
                   BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST);
   }
   shapeSignificantClustersDialog->show();
   shapeSignificantClustersDialog->raise();
   shapeSignificantClustersDialog->activateWindow();
}
      
/**
 * slot shape inter-hem clusters dialog.
 */
void 
GuiMainWindowAttributesActions::slotMetricShapeInterHemClustersDialog()
{
   static GuiMetricShapeInterHemClustersDialog* shapeInterHemClustersDialog = NULL;
   
   if (shapeInterHemClustersDialog == NULL) {
      shapeInterHemClustersDialog = new GuiMetricShapeInterHemClustersDialog(theMainWindow);
   }
   shapeInterHemClustersDialog->show();
   shapeInterHemClustersDialog->raise();
   shapeInterHemClustersDialog->activateWindow();
}

/**
 * update the actions (typically called when menu is about to show)
 */
void 
GuiMainWindowAttributesActions::updateActions()
{
   const VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
   vocabularyMoveStudyInfoToStudyMetaDataAction->setEnabled(vf->getNumberOfStudyInfo() > 0);
}

      
