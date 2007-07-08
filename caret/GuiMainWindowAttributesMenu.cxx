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

#include "GuiMainWindow.h"
#include "GuiMainWindowAttributesActions.h"
#include "GuiMainWindowAttributesMenu.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowAttributesMenu::GuiMainWindowAttributesMenu(GuiMainWindow* parent)
   : QMenu("Attributes", parent)
{
   setObjectName("GuiMainWindowAttributesMenu");
   
   GuiMainWindowAttributesActions* attributesActions = parent->getAttributesActions();
   
   createAreaColorsSubMenu(attributesActions);
   
   createLatLonSubMenu(attributesActions);
   
   createMetricSubMenu(attributesActions);
   
   createMetricShapeStatisticalSubMenu(attributesActions);
   
   createModelsSubMenu(attributesActions);
   
   createPaintSubMenu(attributesActions);
   
   createPaletteSubMenu(attributesActions);
   
   createParametersSubMenu(attributesActions);
   
   createProbAtlasSubMenu(attributesActions);
   
   createStudyMetaDataSubMenu(attributesActions);
   
   createSurfaceShapeSubMenu(attributesActions);
   
   createVectorsSubMenu(attributesActions);
   
   createVocabularySubMenu(attributesActions);
   
   addSeparator();
   
   addAction(attributesActions->getGenerateArealEstimationMapAction());
    
   addAction(attributesActions->getGenerateDeformationFieldAction());
              
   addAction(attributesActions->getGenerateTopographyAction());
              
   addAction(attributesActions->getCopyColoringToRgbPaintAction());
                            
   //addAction(attributesActions->getCopyNormalsToRgbPaintAction());
   
   addAction(attributesActions->getCopyColoringToVolumeAction());

   addSeparator();
   
   addAction(attributesActions->getVolumeToSurfaceMapperAction());
   
   QObject::connect(this, SIGNAL(aboutToShow()),
                    attributesActions, SLOT(updateActions()));
}

/**
 * Destructor.
 */
GuiMainWindowAttributesMenu::~GuiMainWindowAttributesMenu()
{
}

/**
 * Create the vocabulary sub menu.
 */
void
GuiMainWindowAttributesMenu::createVocabularySubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   vocabularySubMenu = addMenu("Vocabulary");
   
   vocabularySubMenu->addAction(attributesActions->getVocabularyFileEditorDialogAction());
   vocabularySubMenu->addAction(attributesActions->getVocabularyMoveStudyInfoToStudyMetaDataAction());
}

/**
 * Create the area colors sub menu.
 */
void
GuiMainWindowAttributesMenu::createAreaColorsSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   areaColorsSubMenu = addMenu("Area Colors");
   
   areaColorsSubMenu->addAction(attributesActions->getAreaColorsEditAction());
}

/**
 * create the models sub menu
 */
void
GuiMainWindowAttributesMenu::createModelsSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   modelsSubMenu = addMenu("Models");
   
   modelsSubMenu->addAction(attributesActions->getModelsEditorAction());
}

/**
 * create the metrics sub menu
 */
void
GuiMainWindowAttributesMenu::createMetricSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   metricSubMenu = addMenu("Metric");
   
   metricSubMenu->addAction(attributesActions->getMetricClearAllOrPartAction());
   metricSubMenu->addSeparator();

   metricSubMenu->addAction(attributesActions->getMetricModificationAction());
                                 
   metricSubMenu->addAction(attributesActions->getMetricAverageDeviationAction());
   
   metricSubMenu->addAction(attributesActions->getMetricMathAction());
   metricSubMenu->addSeparator();

   metricSubMenu->addAction(attributesActions->getMetricToVolumeAction());
   
   metricSubMenu->addAction(attributesActions->getMetricToRgbPaintAction());
}

/**
 * create the palette sub menu.
 */
void
GuiMainWindowAttributesMenu::createPaletteSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   paletteSubMenu = addMenu("Palette");
   
   paletteSubMenu->addAction(attributesActions->getDisplayPaletteEditor());
}

/**
 * create the parameters file sub menu.
 */
void
GuiMainWindowAttributesMenu::createParametersSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   parametersSubMenu = addMenu("Parameters");
   
   parametersSubMenu->addAction(attributesActions->getDisplayParamsFileEditor());
}


/**
 * create the prob atlas sub menu.
 */
void
GuiMainWindowAttributesMenu::createProbAtlasSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   probAtlasSubMenu = addMenu("Probabilistic Atlas");
   
   probAtlasSubMenu->addAction(attributesActions->getProbAtlasThresholdToPaintAction());
}

/**
 * create the paint sub menu.
 */
void
GuiMainWindowAttributesMenu::createPaintSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   paintSubMenu = addMenu("Paint");
   
   paintSubMenu->addAction(attributesActions->getPaintAssignWithinDisplayedBordersAction());
                            
   paintSubMenu->addAction(attributesActions->getPaintCleanNamesAction());
   paintSubMenu->addAction(attributesActions->getPaintClearAllOrPartAction());
   paintSubMenu->addAction(attributesActions->getPaintToVolumeAction());
   paintSubMenu->addAction(attributesActions->getPaintNamesEditAction());
}

/**
 * create the metric/shape statistical operations menu.
 */
void 
GuiMainWindowAttributesMenu::createMetricShapeStatisticalSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   metricShapeStatisticalSubMenu = addMenu("Metric and Surface Shape Statistical Operations");
   
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeCoordinateDifferenceAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeInterHemClustersAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeLeveneMapAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeNormalizationAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeOneSampleTTestAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapePairedTTestAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeRootMeanSquareAction());   
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeShuffledCorrelationColumnsAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeSubtractAverageAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeTwoSampleTTestAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeTMapAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeTMapShuffledColumnsAction());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeWilcoxonRankSum());
   metricShapeStatisticalSubMenu->addAction(attributesActions->getMetricShapeZMapAction());
}
      
/**
 * create the study metadata sub menu.
 */
void 
GuiMainWindowAttributesMenu::createStudyMetaDataSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   studyMetaDataMenu = addMenu("Study Metadata");
   
   studyMetaDataMenu->addAction(attributesActions->getStudyMetaDataEditorDialogAction());
}
      
/**
 * create the surface shape sub menu.
 */
void
GuiMainWindowAttributesMenu::createSurfaceShapeSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   surfaceShapeSubMenu = addMenu("Surface Shape");
   
   surfaceShapeSubMenu->addAction(attributesActions->getSurfaceShapeClearAllOrPartAction());
   
   surfaceShapeSubMenu->addSeparator();
   
   surfaceShapeSubMenu->addAction(attributesActions->getShapeModificationAction());
   surfaceShapeSubMenu->addAction(attributesActions->getShapeMathAction());
   
   surfaceShapeSubMenu->addSeparator();
                                 
   surfaceShapeSubMenu->addAction(attributesActions->getSurfaceShapeAverageDeviationAction());
   surfaceShapeSubMenu->addSeparator();
   
   surfaceShapeSubMenu->addAction(attributesActions->getSurfaceShapeToVolumeAction());
}

/**
 * create the lat/lon sub menu.
 */
void
GuiMainWindowAttributesMenu::createLatLonSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   latLonSubMenu = addMenu("Lat/Long");
   
   latLonSubMenu->addAction(attributesActions->getLatLonClearAllOrPartAction());
   latLonSubMenu->addAction(attributesActions->getLatLonGenerateOnSphereAction());
}

/**
 * create the vectors sub menu.
 */
void
GuiMainWindowAttributesMenu::createVectorsSubMenu(GuiMainWindowAttributesActions* attributesActions)
{
   vectorsSubMenu = addMenu("Vectors");
   
   vectorsSubMenu->addAction(attributesActions->getCoordsToVectorsAction());
   
   vectorsSubMenu->addAction(attributesActions->getNormalsToVectorsAction());
}

