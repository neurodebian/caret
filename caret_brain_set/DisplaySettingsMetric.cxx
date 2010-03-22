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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsMetric.h"
#include "MetricFile.h"
#include "PaletteFile.h"

/**
 * The constructor.
 */
DisplaySettingsMetric::DisplaySettingsMetric(BrainSet* bs)
   : DisplaySettingsNodeAttributeFile(bs,
                                      bs->getMetricFile(),
                                      NULL,
                                      BrainModelSurfaceOverlay::OVERLAY_METRIC,
                                      true,
                                      true)
{
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
   DisplaySettingsNodeAttributeFile::reset();
   overlayScale = METRIC_OVERLAY_SCALE_AUTO;
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
   overlayScaleSpecifiedColumnNumber = 0;
   autoScalePercentageNegativeMinimum = 2.0;
   autoScalePercentageNegativeMaximum = 98.0;
   autoScalePercentagePositiveMinimum = 2.0;
   autoScalePercentagePositiveMaximum = 98.0;
}

/**
 * get the display and threshold columns for palette (negative if invalid).
 */
void
DisplaySettingsMetric::getMetricsForColoringAndPalette(int& displayColumnOut,
                                                  int& thresholdColumnOut,
                                                  float& negMaxValue,
                                                  float& negMinValue,
                                                  float& posMinValue,
                                                  float& posMaxValue,
                                                  const bool volumeFlag) const
{
   displayColumnOut = -1;
   thresholdColumnOut = -1;
   negMaxValue = 0.0;
   negMinValue = 0.0;
   posMinValue = 0.0;
   posMaxValue = 0.0;

   bool doUpdateFlag = true;
   if (volumeFlag) {
      doUpdateFlag = false;
   }
   
   //
   // Find the first displayed surface with metric as an overlay
   //
   for (int iw = 0; iw < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; iw++) {
      const int brainModelIndex = brainSet->getDisplayedModelIndexForWindow(
         static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(iw));
      if ((brainModelIndex >= 0) &&
          (brainModelIndex < brainSet->getNumberOfBrainModels())) {
         const BrainModelSurface* bms = brainSet->getBrainModelSurface(brainModelIndex);
         if (bms != NULL) {
            for (int j = (brainSet->getNumberOfSurfaceOverlays() - 1); j >= 0; j--) {
               const BrainModelSurfaceOverlay* bmsOverlay = brainSet->getSurfaceOverlay(j);
               if (bmsOverlay->getOverlay(brainModelIndex, doUpdateFlag) ==
                        BrainModelSurfaceOverlay::OVERLAY_METRIC) {
                  displayColumnOut = bmsOverlay->getDisplayColumnSelected(brainModelIndex);
                  thresholdColumnOut = bmsOverlay->getThresholdColumnSelected(brainModelIndex);
                  break;
               }
            }
         }
      }
      
      if (displayColumnOut >= 0) {
         break;
      }
   }
   
   MetricFile* mf = brainSet->getMetricFile();
   bool useFunctionalVolumeFlag = false;
   
   switch (getSelectedOverlayScale()) {
      case METRIC_OVERLAY_SCALE_AUTO:
         if (displayColumnOut >= 0) {
            float minValue, maxValue;
            mf->getDataColumnMinMax(displayColumnOut, minValue, maxValue);
            if (minValue < 0.0) {
               negMaxValue = minValue;
            }
            if (maxValue > 0.0) {
               posMaxValue = maxValue;
            }
         }
         break;
      case METRIC_OVERLAY_SCALE_AUTO_PERCENTAGE:
         if (displayColumnOut >= 0) {
            mf->getMinMaxValuesFromPercentages(displayColumnOut,
                                               autoScalePercentageNegativeMaximum,
                                               autoScalePercentageNegativeMinimum,
                                               autoScalePercentagePositiveMinimum,
                                               autoScalePercentagePositiveMaximum,
                                               negMaxValue,
                                               negMinValue,
                                               posMinValue,
                                               posMaxValue);
         }
         break;
      case METRIC_OVERLAY_SCALE_AUTO_SPECIFIED_COLUMN:
         if (getOverlayScaleSpecifiedColumnNumber() >= 0) {
            float minValue, maxValue;
            mf->getDataColumnMinMax(getOverlayScaleSpecifiedColumnNumber(), minValue, maxValue);
            if (minValue < 0.0) {
               negMaxValue = minValue;
            }
            if (maxValue > 0.0) {
               posMaxValue = maxValue;
            }
         }
         break;
      case METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME:
         useFunctionalVolumeFlag = true;
         break;
      case METRIC_OVERLAY_SCALE_USER:
         getUserScaleMinMax(posMinValue,
                            posMaxValue,
                            negMinValue,
                            negMaxValue);
         break;
   }
   
   //
   // Use volume for min/max values if no metrics
   //
   if (mf->getNumberOfColumns() <= 0) {
      useFunctionalVolumeFlag = true;
   }
   
   if (useFunctionalVolumeFlag) {
      BrainModelVolume* bmv = brainSet->getBrainModelVolume();
      if (bmv != NULL) {
         VolumeFile* vf = bmv->getSelectedVolumeFunctionalViewFile();
         if (vf != NULL) {
            float minValue, maxValue;
            vf->getMinMaxVoxelValues(minValue, maxValue);
            if (minValue < 0.0) {
               negMaxValue = minValue;
            }
            if (maxValue > 0.0) {
               posMaxValue = maxValue;
            }
         }
      }
   }
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
   DisplaySettingsNodeAttributeFile::update();
   if (paletteIndex >= brainSet->getPaletteFile()->getNumberOfPalettes()) {
      paletteIndex = 0;
   }
   updateSelectedColumnIndex(brainSet->getMetricFile(), 
                             overlayScaleSpecifiedColumnNumber);
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

static const QString metricOverlayScaleAutoPriorityColumn("overlay-scale-priority-column");
static const QString metricOverlayScaleAutoPercentageColumn("overlay-scale-percentage-column");
static const QString metricOverlayScaleAutoSpecifiedColumn("overlay-scale-specified-column");
static const QString metricOverlayScaleAutoFunctionalVolume("overlay-scale-functional-volume");
static const QString metricOverlayScaleUser("overlay-scale-user");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsMetric::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   autoScalePercentageNegativeMinimum = 2.0;
   autoScalePercentageNegativeMaximum = 98.0;
   autoScalePercentagePositiveMinimum = 2.0;
   autoScalePercentagePositiveMaximum = 98.0;

   DisplaySettingsNodeAttributeFile::showScene(scene, errorMessage);

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsMetric") {
         showSceneSelectedColumns(*sc,
                                  "Metric File",
                                  metricViewID,
                                  metricThreshID,
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
            else if (infoName == "userNegativeThreshold") {
               si->getValue(userNegativeThreshold);
            }
            else if (infoName == "userPositiveThreshold") {
               si->getValue(userPositiveThreshold);
            }
            else if (infoName == "overlayScale") {
               if (si->getValueAsString() == metricOverlayScaleAutoPriorityColumn) {
                  overlayScale = METRIC_OVERLAY_SCALE_AUTO;
               }
               else if (si->getValueAsString() == metricOverlayScaleAutoPercentageColumn) {
                  overlayScale = METRIC_OVERLAY_SCALE_AUTO_PERCENTAGE;
               }
               else if (si->getValueAsString() == metricOverlayScaleAutoSpecifiedColumn) {
                  overlayScale = METRIC_OVERLAY_SCALE_AUTO_SPECIFIED_COLUMN;
               }
               else if (si->getValueAsString() == metricOverlayScaleAutoFunctionalVolume) {
                  overlayScale = METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME;
               }
               else if (si->getValueAsString() == metricOverlayScaleUser) {
                  overlayScale = METRIC_OVERLAY_SCALE_USER;
               }
               else {
                  overlayScale = METRIC_OVERLAY_SCALE_AUTO;
                  
                  //
                  // Old scene had 3 types stored as int
                  //
                  const int val = si->getValueAsInt();
                  switch (val) {
                     case 0:
                        overlayScale = METRIC_OVERLAY_SCALE_AUTO;
                        break;
                     case 1:
                        overlayScale = METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME;
                        break;
                     case 2:
                        overlayScale = METRIC_OVERLAY_SCALE_USER;
                        break;
                  }
               }
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
            else if (infoName == "autoScalePercentageNegativeMinimum") {
               si->getValue(autoScalePercentageNegativeMinimum);
            }
            else if (infoName == "autoScalePercentageNegativeMaximum") {
               si->getValue(autoScalePercentageNegativeMaximum);
            }
            else if (infoName == "autoScalePercentagePositiveMinimum") {
               si->getValue(autoScalePercentagePositiveMinimum);
            }
            else if (infoName == "autoScalePercentagePositiveMaximum") {
               si->getValue(autoScalePercentagePositiveMaximum);
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
DisplaySettingsMetric::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage)
{
   DisplaySettingsNodeAttributeFile::saveScene(scene, onlyIfSelected, errorMessage);

   MetricFile* mf = brainSet->getMetricFile();
   
   if (onlyIfSelected) {
      if (mf->getNumberOfColumns() <= 0) {
         return;
      }
      
      BrainModelVolumeVoxelColoring* bvvc = brainSet->getVoxelColoring();
      if ((brainSet->isASurfaceOverlayForAnySurface(
                           BrainModelSurfaceOverlay::OVERLAY_METRIC) == false) &&
          (bvvc->isUnderlayOrOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL) == false)) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsMetric");
   
   saveSceneSelectedColumns(sc);

   sc.addSceneInfo(SceneFile::SceneInfo("thresholdType",
                                        thresholdType));
   sc.addSceneInfo(SceneFile::SceneInfo("userNegativeThreshold",
                                        userNegativeThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("userPositiveThreshold",
                                        userPositiveThreshold));
   switch (overlayScale) {
      case METRIC_OVERLAY_SCALE_AUTO:
         sc.addSceneInfo(SceneFile::SceneInfo("overlayScale",
                                              metricOverlayScaleAutoPriorityColumn));
         break;
      case METRIC_OVERLAY_SCALE_AUTO_PERCENTAGE:
         sc.addSceneInfo(SceneFile::SceneInfo("overlayScale",
                                              metricOverlayScaleAutoPercentageColumn));
         break;
      case METRIC_OVERLAY_SCALE_AUTO_SPECIFIED_COLUMN:
         sc.addSceneInfo(SceneFile::SceneInfo("overlayScale",
                                              metricOverlayScaleAutoSpecifiedColumn));
         break;
      case METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME:
         sc.addSceneInfo(SceneFile::SceneInfo("overlayScale",
                                              metricOverlayScaleAutoFunctionalVolume));
         break;
      case METRIC_OVERLAY_SCALE_USER:
         sc.addSceneInfo(SceneFile::SceneInfo("overlayScale",
                                              metricOverlayScaleUser));
         break;
   }
   sc.addSceneInfo(SceneFile::SceneInfo("userScalePositiveMinimum",
                                        userScalePositiveMinimum));
   sc.addSceneInfo(SceneFile::SceneInfo("userScalePositiveMaximum",
                                        userScalePositiveMaximum));
   sc.addSceneInfo(SceneFile::SceneInfo("userScaleNegativeMinimum",
                                        userScaleNegativeMinimum));
   sc.addSceneInfo(SceneFile::SceneInfo("userScaleNegativeMaximum",
                                        userScaleNegativeMaximum));
   sc.addSceneInfo(SceneFile::SceneInfo("autoScalePercentageNegativeMinimum",
                                        autoScalePercentageNegativeMinimum));
   sc.addSceneInfo(SceneFile::SceneInfo("autoScalePercentageNegativeMaximum",
                                        autoScalePercentageNegativeMaximum));
   sc.addSceneInfo(SceneFile::SceneInfo("autoScalePercentagePositiveMinimum",
                                        autoScalePercentagePositiveMinimum));
   sc.addSceneInfo(SceneFile::SceneInfo("autoScalePercentagePositiveMaximum",
                                        autoScalePercentagePositiveMaximum));
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
