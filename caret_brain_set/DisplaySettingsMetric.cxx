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

#include <algorithm>

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsMetric.h"
#include "MetricFile.h"
#include "PaletteFile.h"

/**
 * The constructor.
 */
DisplaySettingsMetric::DisplaySettingsMetric(BrainSet* bs)
   : DisplaySettings(bs)
{
   applySelectionToLeftAndRightStructuresFlag = false;
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsMetric::~DisplaySettingsMetric()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsMetric::reset()
{
   displayColumn.clear();
   thresholdColumn.clear();
   overlayScale = METRIC_OVERLAY_SCALE_AUTO_METRIC;
   userScaleNegativeMaximum = -1.0;
   userScaleNegativeMinimum =  0.0;
   userScalePositiveMinimum =  0.0;
   userScalePositiveMaximum =  1.0;
   interpolateColors = false;
   displayMode = METRIC_DISPLAY_MODE_POSITIVE_AND_NEGATIVE;
   paletteIndex = 0;
   displayColorBar = false;
   metricDataPlot = METRIC_DATA_PLOT_OFF;
   metricDataPlotManualScaleFlag = false;
   metricDataPlotManualScaleMinimum = 0.0;
   metricDataPlotManualScaleMaximum = 1000.0;
   showSpecialColorForThresholdedNodes = false;
   userNegativeThreshold = 0.0;
   userPositiveThreshold = 0.0;
   thresholdType = METRIC_THRESHOLDING_TYPE_USER_VALUES;
}

/**
 * get manual data plot scaling.
 */
bool 
DisplaySettingsMetric::getDataPlotManualScaling(float& minPlot, float& maxPlot) const
{
   minPlot = metricDataPlotManualScaleMinimum;
   maxPlot = metricDataPlotManualScaleMaximum;
   return metricDataPlotManualScaleFlag;
}

/**
 * set manual data plot scaling.
 */
void 
DisplaySettingsMetric::setDataPlotManualScaling(const bool b, const float minPlot,
                                                const float maxPlot)
{
   metricDataPlotManualScaleFlag = b;
   metricDataPlotManualScaleMinimum = minPlot;
   metricDataPlotManualScaleMaximum = maxPlot;
}

/**
 * Update any selections due to changes in loaded metric file.
 */
void
DisplaySettingsMetric::update()
{
   updateSelectedColumnIndices(brainSet->getMetricFile(), displayColumn);
   updateSelectedColumnIndices(brainSet->getMetricFile(), thresholdColumn);
   if (paletteIndex >= brainSet->getPaletteFile()->getNumberOfPalettes()) {
      paletteIndex = 0;
   }
}

/**
 * Get the column selected for display.
 * Returns -1 if there are no metric columns available.
 */
int
DisplaySettingsMetric::getSelectedDisplayColumn(const int modelIn)
{
   if (displayColumn.empty()) {
      return -1;
   }
   
   int model = modelIn;
   if (model < 0) {
      model = 0;
   }

   return displayColumn[model];
}

/**
 * set column for display.
 */
void 
DisplaySettingsMetric::setSelectedDisplayColumn(const int model,
                                                const int sdc) 
{ 
   if (applySelectionToLeftAndRightStructuresFlag) {
      MetricFile* mf = brainSet->getMetricFile();
      if ((sdc >= 0) && (sdc < mf->getNumberOfColumns())) {
         int leftCol = -1;
         int rightCol = -1;
         QString name = mf->getColumnName(sdc).toLower().trimmed();
         if (name.indexOf("left") >= 0) {
            leftCol = sdc;
            const QString rightName = name.replace("left", "right");
            for (int i = 0; i < mf->getNumberOfColumns(); i++) {
               if (mf->getColumnName(i).toLower().trimmed() == rightName) {
                  rightCol = i;
                  break;
               }
            }
         }
         else if (name.indexOf("right") >= 0) {
            rightCol = sdc;
            const QString leftName = name.replace("right", "left");
            for (int i = 0; i < mf->getNumberOfColumns(); i++) {
               if (mf->getColumnName(i).toLower().trimmed() == leftName) {
                  leftCol = i;
                  break;
               }
            }
         }
         
         for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
            const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
            if (bms != NULL) {
               switch (bms->getStructure().getType()) {
                  case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                     if (leftCol >= 0) {
                        displayColumn[i] = leftCol;
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                     if (rightCol >= 0) {
                        displayColumn[i] = rightCol;
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                     break;
                  case Structure::STRUCTURE_TYPE_CEREBELLUM:
                     break;
                  case Structure::STRUCTURE_TYPE_INVALID:
                     break;
               }
            }
         }
      }
   }
   else {
      if (model < 0) {
         std::fill(displayColumn.begin(), displayColumn.end(), sdc);
      }
      else {
         displayColumn[model] = sdc; 
      }
   }
}
      
/**
 * Get the column selected for threshold.
 * Returns -1 if there are no metric columns available.
 */
int
DisplaySettingsMetric::getSelectedThresholdColumn(const int modelIn)
{
   if (thresholdColumn.empty()) {
      return -1;
   }
   
   int model = modelIn;
   if (model < 0) {
      model = 0;
   }

   return thresholdColumn[model];
}

/**
 * set column for thresholding.
 */
void 
DisplaySettingsMetric::setSelectedThresholdColumn(const int model,
                                                  const int sdc) 
{
   if (applySelectionToLeftAndRightStructuresFlag) {
      MetricFile* mf = brainSet->getMetricFile();
      if ((sdc >= 0) && (sdc < mf->getNumberOfColumns())) {
         int leftCol = -1;
         int rightCol = -1;
         QString name = mf->getColumnName(sdc).toLower().trimmed();
         if (name.indexOf("left") >= 0) {
            leftCol = sdc;
            const QString rightName = name.replace("left", "right");
            for (int i = 0; i < mf->getNumberOfColumns(); i++) {
               if (mf->getColumnName(i).toLower().trimmed() == rightName) {
                  rightCol = i;
                  break;
               }
            }
         }
         else if (name.indexOf("right") >= 0) {
            rightCol = sdc;
            const QString leftName = name.replace("right", "left");
            for (int i = 0; i < mf->getNumberOfColumns(); i++) {
               if (mf->getColumnName(i).toLower().trimmed() == leftName) {
                  leftCol = i;
                  break;
               }
            }
         }
         
         for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
            const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
            if (bms != NULL) {
               switch (bms->getStructure().getType()) {
                  case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                     if (leftCol >= 0) {
                        thresholdColumn[i] = leftCol;
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                     if (rightCol >= 0) {
                        thresholdColumn[i] = rightCol;
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                     break;
                  case Structure::STRUCTURE_TYPE_CEREBELLUM:
                     break;
                  case Structure::STRUCTURE_TYPE_INVALID:
                     break;
               }
            }
         }
      }
   }
   else {
      if (model < 0) {
         std::fill(thresholdColumn.begin(), thresholdColumn.end(), sdc);
      }
      else {
         thresholdColumn[model] = sdc; 
      }
   }
}
      
/**
 * Get the user scale minimum and maximum values.
 */
void 
DisplaySettingsMetric::getUserScaleMinMax(float& posMinValue, 
                                          float& posMaxValue,
                                          float& negMinValue,
                                          float& negMaxValue) const
{
   posMinValue = userScalePositiveMinimum;
   posMaxValue = userScalePositiveMaximum;
   negMinValue = userScaleNegativeMinimum;
   negMaxValue = userScaleNegativeMaximum;
}

/**
 * Set the user scale minimum and maximum values.
 */
void 
DisplaySettingsMetric::setUserScaleMinMax(const float posMinValue,
                                          const float posMaxValue,
                                          const float negMinValue,
                                          const float negMaxValue)
{
   userScalePositiveMinimum = posMinValue;
   userScalePositiveMaximum = posMaxValue;
   userScaleNegativeMinimum = negMinValue;
   userScaleNegativeMaximum = negMaxValue;
}

/**
 * get special color for thresholded nodes.
 */
void 
DisplaySettingsMetric::getSpecialColorsForThresholdedNodes(unsigned char negThreshColor[3],
                                                           unsigned char posThreshColor[3]) const
{
   negThreshColor[0] = 180;
   negThreshColor[1] = 255;
   negThreshColor[2] = 115;
   posThreshColor[0] = 115;
   posThreshColor[1] = 255;
   posThreshColor[2] = 180;
}                                               

/**
 * get the user thresholding values.
 */
void 
DisplaySettingsMetric::getUserThresholdingValues(float& negThresh, float& posThresh) const
{
   negThresh = userNegativeThreshold;
   posThresh = userPositiveThreshold;
}

/**
 * set the user thresholding values.
 */
void 
DisplaySettingsMetric::setUserThresholdingValues(const float negThresh, const float posThresh)
{
   userNegativeThreshold = negThresh;
   userPositiveThreshold = posThresh;
}

static const QString metricViewID("metric-view-column");
static const QString metricThreshID("metric-thresh-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsMetric::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsMetric") {
         showSceneNodeAttribute(*sc,
                                metricViewID,
                                brainSet->getMetricFile(),
                                "Metric File",
                                displayColumn,
                                errorMessage);
         showSceneNodeAttribute(*sc,
                                metricThreshID,
                                brainSet->getMetricFile(),
                                "Metric File",
                                thresholdColumn,
                                errorMessage);
                                
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "thresholdType") {
               int val;
               si->getValue(val);
               thresholdType = static_cast<METRIC_THRESHOLDING_TYPE>(val);
            }
            else if (infoName == "applySelectionToLeftAndRightStructuresFlag") {
               si->getValue(applySelectionToLeftAndRightStructuresFlag);
            }
            else if (infoName == "userNegativeThreshold") {
               si->getValue(userNegativeThreshold);
            }
            else if (infoName == "userPositiveThreshold") {
               si->getValue(userPositiveThreshold);
            }
            else if (infoName == "overlayScale") {
               int val;
               si->getValue(val);
               overlayScale = static_cast<METRIC_OVERLAY_SCALE>(val);
            }
            else if (infoName == "userScalePositiveMinimum") {
               si->getValue(userScalePositiveMinimum);
            }
            else if (infoName == "userScalePositiveMaximum") {
               si->getValue(userScalePositiveMaximum);
            }
            else if (infoName == "userScaleNegativeMinimum") {
               si->getValue(userScaleNegativeMinimum);
            }
            else if (infoName == "userScaleNegativeMaximum") {
               si->getValue(userScaleNegativeMaximum);
            }
            else if (infoName == "interpolateColors") {
               si->getValue(interpolateColors);
            }
            else if (infoName == "displayColorBar") {
               si->getValue(displayColorBar);
            }
            else if (infoName == "displayMode") {
               int val;
               si->getValue(val);
               displayMode = static_cast<METRIC_DISPLAY_MODE>(val);
            }
            else if (infoName == "metricDataPlot") {
               int val;
               si->getValue(val);
               metricDataPlot = static_cast<METRIC_DATA_PLOT>(val);
            }
            else if (infoName == "metricDataPlotManualScaleFlag") {
               si->getValue(metricDataPlotManualScaleFlag);
            }
            else if (infoName == "metricDataPlotManualScaleMinimum") {
               si->getValue(metricDataPlotManualScaleMinimum);
            }
            else if (infoName == "metricDataPlotManualScaleMaximum") {
               si->getValue(metricDataPlotManualScaleMaximum);
            }
            else if (infoName == "showSpecialColorForThresholdedNodes") {
               si->getValue(showSpecialColorForThresholdedNodes);
            }
            else if (infoName == "metricPaletteIndex") {
               const QString paletteName = si->getValueAsString();
               PaletteFile* pf = brainSet->getPaletteFile();
               const int num = pf->getNumberOfPalettes();
               bool found = false;
               for (int j = 0; j < num; j++) {
                  const Palette* pal = pf->getPalette(j);
                  if (paletteName == pal->getName()) {
                     paletteIndex = j;
                     found = true;
                     break;
                  }
               }
               if (found == false) {
                  errorMessage.append("Unable to find palette named: ");
                  errorMessage.append(paletteName);
                  errorMessage.append("\n");
               }
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsMetric::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   MetricFile* mf = brainSet->getMetricFile();
   
   if (onlyIfSelected) {
      if (mf->getNumberOfColumns() <= 0) {
         return;
      }
      
      BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
      BrainModelVolumeVoxelColoring* bvvc = brainSet->getVoxelColoring();
      if ((bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::UNDERLAY_METRIC,
                                    BrainModelSurfaceNodeColoring::OVERLAY_METRIC) == false) &&
          (bvvc->isUnderlayOrOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL) == false)) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsMetric");
   
   saveSceneNodeAttribute(sc,
                          metricViewID,
                          mf,
                          displayColumn);
   saveSceneNodeAttribute(sc,
                          metricThreshID,
                          mf,
                          thresholdColumn);

   sc.addSceneInfo(SceneFile::SceneInfo("applySelectionToLeftAndRightStructuresFlag",
                                        applySelectionToLeftAndRightStructuresFlag));
   sc.addSceneInfo(SceneFile::SceneInfo("thresholdType",
                                        thresholdType));
   sc.addSceneInfo(SceneFile::SceneInfo("userNegativeThreshold",
                                        userNegativeThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("userPositiveThreshold",
                                        userPositiveThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("overlayScale",
                                        overlayScale));
   sc.addSceneInfo(SceneFile::SceneInfo("userScalePositiveMinimum",
                                        userScalePositiveMinimum));
   sc.addSceneInfo(SceneFile::SceneInfo("userScalePositiveMaximum",
                                        userScalePositiveMaximum));
   sc.addSceneInfo(SceneFile::SceneInfo("userScaleNegativeMinimum",
                                        userScaleNegativeMinimum));
   sc.addSceneInfo(SceneFile::SceneInfo("userScaleNegativeMaximum",
                                        userScaleNegativeMaximum));
   sc.addSceneInfo(SceneFile::SceneInfo("interpolateColors",
                                        interpolateColors));
   sc.addSceneInfo(SceneFile::SceneInfo("displayColorBar",
                                        displayColorBar));
   sc.addSceneInfo(SceneFile::SceneInfo("displayMode",
                                        displayMode));
   sc.addSceneInfo(SceneFile::SceneInfo("metricDataPlot",
                                        metricDataPlot));
   sc.addSceneInfo(SceneFile::SceneInfo("metricDataPlotManualScaleFlag",
                                        metricDataPlotManualScaleFlag));
   sc.addSceneInfo(SceneFile::SceneInfo("metricDataPlotManualScaleMinimum",
                                        metricDataPlotManualScaleMinimum));
   sc.addSceneInfo(SceneFile::SceneInfo("metricDataPlotManualScaleMaximum",
                                        metricDataPlotManualScaleMaximum));
   sc.addSceneInfo(SceneFile::SceneInfo("showSpecialColorForThresholdedNodes",
                                        showSpecialColorForThresholdedNodes));
   PaletteFile* pf = brainSet->getPaletteFile();
   if ((paletteIndex >= 0) && (paletteIndex < pf->getNumberOfPalettes())) {
      const Palette* pal = pf->getPalette(paletteIndex);
      sc.addSceneInfo(SceneFile::SceneInfo("metricPaletteIndex",
                                           pal->getName()));
   }
   
   scene.addSceneClass(sc);
}
                       
/**
 * for node attribute files - all column selections for each surface are the same.
 */
/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsMetric::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   if (displayColumn[bm1] != displayColumn[bm2]) {
      return false;
   }
   return (thresholdColumn[bm1] == thresholdColumn[bm2]);
}      

