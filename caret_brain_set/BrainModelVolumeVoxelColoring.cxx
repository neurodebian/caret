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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <QDateTime>
#include <iostream>

#include "AreaColorFile.h"
#include "BrainSet.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "DebugControl.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "DisplaySettingsVolume.h"
#include "MetricFile.h"
#include "PaletteFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelVolumeVoxelColoring::BrainModelVolumeVoxelColoring(BrainSet* bs)
{
   brainSet = bs;
   
   underlay         = UNDERLAY_OVERLAY_ANATOMY;
   primaryOverlay   = UNDERLAY_OVERLAY_NONE;
   secondaryOverlay = UNDERLAY_OVERLAY_NONE;
}

/**
 * destructor.
 */
BrainModelVolumeVoxelColoring::~BrainModelVolumeVoxelColoring()
{
}

/**
 * Initialize the underlay selection
 */
void
BrainModelVolumeVoxelColoring::initializeUnderlay()
{
   if (brainSet->getNumberOfVolumeAnatomyFiles() > 0) {
      underlay = UNDERLAY_OVERLAY_ANATOMY;
   }
   else if (brainSet->getNumberOfVolumeFunctionalFiles() > 0) {
      underlay = UNDERLAY_OVERLAY_FUNCTIONAL;
   }
   else if (brainSet->getNumberOfVolumePaintFiles() > 0) {
      underlay = UNDERLAY_OVERLAY_PAINT;
   }
   else if (brainSet->getNumberOfVolumeProbAtlasFiles() > 0) {
      underlay = UNDERLAY_OVERLAY_PROB_ATLAS;
   }
   else if (brainSet->getNumberOfVolumeRgbFiles() > 0) {
      underlay = UNDERLAY_OVERLAY_RGB;
   }
   else if (brainSet->getNumberOfVolumeSegmentationFiles() > 0) {
      underlay = UNDERLAY_OVERLAY_SEGMENTATION;
   }
   else if (brainSet->getNumberOfVolumeVectorFiles() > 0) {
      underlay = UNDERLAY_OVERLAY_VECTOR;
   }
}

/**
 * Set all functional volume file coloring invalid.
 */
void
BrainModelVolumeVoxelColoring::setVolumeFunctionalColoringInvalid()
{
   for (int i = 0; i < brainSet->getNumberOfVolumeFunctionalFiles(); i++) {
      VolumeFile* vf = brainSet->getVolumeFunctionalFile(i);
      vf->setVoxelColoringInvalid();
   }
}

/**
 * Set all anatomy volume file coloring invalid.
 */
void
BrainModelVolumeVoxelColoring::setVolumeAnatomyColoringInvalid()
{
   for (int i = 0; i < brainSet->getNumberOfVolumeAnatomyFiles(); i++) {
      VolumeFile* vf = brainSet->getVolumeAnatomyFile(i);
      vf->setVoxelColoringInvalid();
   }
}

/**
 * Set all paint volume file coloring invalid.
 */
void
BrainModelVolumeVoxelColoring::setVolumePaintColoringInvalid()
{
   for (int i = 0; i < brainSet->getNumberOfVolumePaintFiles(); i++) {
      VolumeFile* vf = brainSet->getVolumePaintFile(i);
      vf->setVoxelColoringInvalid();
   }
}

/**
 * Set all prob atlas volume file coloring invalid.
 */
void
BrainModelVolumeVoxelColoring::setVolumeProbAtlasColoringInvalid()
{
   for (int i = 0; i < brainSet->getNumberOfVolumeProbAtlasFiles(); i++) {
      VolumeFile* vf = brainSet->getVolumeProbAtlasFile(i);
      vf->setVoxelColoringInvalid();
   }
}

/**
 * Set all segmenatation volume file coloring invalid.
 */
void
BrainModelVolumeVoxelColoring::setVolumeSegmentationColoringInvalid()
{
   for (int i = 0; i < brainSet->getNumberOfVolumeSegmentationFiles(); i++) {
      VolumeFile* vf = brainSet->getVolumeSegmentationFile(i);
      vf->setVoxelColoringInvalid();
   }
}

/**
 * Set all vector volume file coloring invalid.
 */
void
BrainModelVolumeVoxelColoring::setVolumeVectorColoringInvalid()
{
   for (int i = 0; i < brainSet->getNumberOfVolumeVectorFiles(); i++) {
      VolumeFile* vf = brainSet->getVolumeVectorFile(i);
      vf->setVoxelColoringInvalid();
   }
}

/**
 * Set all rgb paint volume file coloring invalid.
 */
void
BrainModelVolumeVoxelColoring::setVolumeRgbPaintColoringInvalid()
{
   for (int i = 0; i < brainSet->getNumberOfVolumeRgbFiles(); i++) {
      VolumeFile* vf = brainSet->getVolumeRgbFile(i);
      vf->setVoxelColoringInvalid();
   }
}

/**
 * set all volume coloring invalid.
 */
void 
BrainModelVolumeVoxelColoring::setVolumeAllColoringInvalid()
{
    setVolumeAnatomyColoringInvalid();
    setVolumeFunctionalColoringInvalid();
    setVolumePaintColoringInvalid();
    setVolumeProbAtlasColoringInvalid();
    setVolumeRgbPaintColoringInvalid();
    setVolumeSegmentationColoringInvalid();
    setVolumeVectorColoringInvalid();
}
      
/**
 * Color all of the volumes voxels.
 */
void 
BrainModelVolumeVoxelColoring::colorAllOfTheVolumesVoxels(VolumeFile* vf)
{
   const unsigned char invalidColor[4] = { 0, 0, 0, VolumeFile::VOXEL_COLOR_STATUS_INVALID };
   int dim[3] = { 0, 0, 0 };
   vf->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            vf->setVoxelColor(i, j, k, invalidColor);
            unsigned char rgb[4];
            getVoxelColoring(vf, i, j, k, rgb);
            vf->setVoxelColor(i, j, k, rgb);
         }
      }
   }
}
      
/**
 * Get a voxel's coloring.
 */
void
BrainModelVolumeVoxelColoring::getVoxelColoring(VolumeFile* vf,
                                                const int i,
                                                const int j,
                                                const int k,
                                                unsigned char rgb[4])
{
   //
   // If voxel color NOT invalid, we are done
   //
   vf->getVoxelColor(i, j, k, rgb);
   if (rgb[3] != VolumeFile::VOXEL_COLOR_STATUS_INVALID) {
      return;
   }
   
   float voxel = vf->getVoxel(i, j, k);
   const DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   
   
   switch(vf->getVolumeType()) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         {
            float voxelOffset = 0.0;
            float voxelScale  = 1.0;
            
            switch (dsv->getAnatomyVolumeColoringType()) {
               case DisplaySettingsVolume::ANATOMY_COLORING_TYPE_0_255:
                  voxelOffset = 0.0;
                  voxelScale  = 1.0;
                  break;
               case DisplaySettingsVolume::ANATOMY_COLORING_TYPE_MIN_MAX:
                  {
                     float minValue = 0.0, maxValue = 0.0;
                     vf->getMinMaxVoxelValues(minValue, maxValue);
                     const float range = maxValue - minValue;
                     if (range != 0.0) {
                        voxelOffset = -minValue;
                        voxelScale  = 255.0 / range;
                     }
                  }
                  break;
               case DisplaySettingsVolume::ANATOMY_COLORING_TYPE_2_98:
                  {
                     
                     float blackValue = 0.0, whiteValue = 0.0;
                     vf->getTwoToNinetyEightPercentMinMaxVoxelValues(blackValue, whiteValue);
                     //
                     // Black/White range of the voxels
                     //
                     const float bwRange = whiteValue - blackValue;
                     
                     voxelOffset = -blackValue;
                     voxelScale  = 255.0 / bwRange;
                  }
            }
            
            const float brightness = dsv->getAnatomyVolumeBrightness();
            const float contrast   = dsv->getAnatomyVolumeContrast();
            
            const float shift = brightness - 128.0;
            const float scale = (100.0 + contrast) / (100.0 - contrast);
            
            voxel += voxelOffset;
            voxel *= voxelScale;
            
            float intensity = 128.0 + (voxel + shift) * scale;
            if (intensity > 255.0) {
               intensity = 255.0;
            }
            else if (intensity < 0.0) {
               intensity = 0.0;
            }
            rgb[0] = static_cast<unsigned char>(intensity);
            rgb[1] = rgb[0];
            rgb[2] = rgb[0];
            rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
            vf->setVoxelColor(i, j, k, rgb);
         }
         break;
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         {
            //
            // Get the volume used for thresholding
            //
            VolumeFile* threshVolume = vf;
            const int threshIndex = dsv->getSelectedFunctionalVolumeThreshold();
            if ((threshIndex >= 0) &&
                (threshIndex < brainSet->getNumberOfVolumeFunctionalFiles())) {
               threshVolume = brainSet->getVolumeFunctionalFile(threshIndex);
            }
            
            //
            // Get the metric settings
            //
            MetricFile* mf = brainSet->getMetricFile();
            DisplaySettingsMetric* dsm = brainSet->getDisplaySettingsMetric();
            
            //
            // Get the palette file
            //
            const PaletteFile* pf = brainSet->getPaletteFile();
            if (pf->getNumberOfPalettes() == 0) {
               std::cerr << "There are no palette files loaded, cannot color metrics." << std::endl;
               return;
            }
            const Palette* palette = pf->getPalette(dsm->getSelectedPaletteIndex());
            
            //
            // Get the minimum and maximum metric 
            //
            float posMinMetric = 0.0, posMaxMetric = 0.0, negMinMetric = 0.0, negMaxMetric = 0.0;
            int metricDisplayColumnNumber, metricThresholdColumnNumber;
            dsm->getMetricsForColoringAndPalette(metricDisplayColumnNumber,
                                                metricThresholdColumnNumber,
                                                negMaxMetric,
                                                negMinMetric,
                                                posMinMetric,
                                                posMaxMetric,
                                                true);                                          

            //
            // Get thresholding
            //
            float thresholdNegativeValue = 0.0, thresholdPositiveValue = 0.0;
            dsm->getUserThresholdingValues(thresholdNegativeValue,
                                           thresholdPositiveValue);
            switch (dsm->getMetricThresholdingType()) {
               case DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_FILE_COLUMN:
                  if ((metricThresholdColumnNumber >= 0) && (metricThresholdColumnNumber < mf->getNumberOfColumns())) {
                     mf->getColumnThresholding(metricThresholdColumnNumber,
                                      thresholdNegativeValue,
                                      thresholdPositiveValue);
                  }
                  break;
               case DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_FILE_COLUMN_AVERAGE:
                  if ((metricThresholdColumnNumber >= 0) && (metricThresholdColumnNumber < mf->getNumberOfColumns())) {
                     mf->getColumnAverageThresholding(metricThresholdColumnNumber,
                                         thresholdNegativeValue,
                                         thresholdPositiveValue);
                  }
                  break;
               case DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_USER_VALUES:
                  dsm->getUserThresholdingValues(thresholdNegativeValue,
                                                 thresholdPositiveValue);
                  break;
            }
            
            //
            // Always interpolate if the palette has only two colors
            //
            bool interpolateColor = dsm->getInterpolateColors();
            if (palette->getNumberOfPaletteEntries() == 2) {
               interpolateColor = true;
            }
            
            rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;
            //const float voxel = vf->getVoxelWithFlatIndex(i, 0);
            //const float threshVoxel = threshVolume->getVoxelWithFlatIndex(i);
            const float threshVoxel = threshVolume->getVoxel(i, j, k);
            
            unsigned char negThreshColor[3], posThreshColor[3];
            dsm->getSpecialColorsForThresholdedNodes(negThreshColor,
                                                     posThreshColor);
            const bool showThreshVoxels = dsm->getShowSpecialColorForThresholdedNodes();
            
            //
            // when activation assignment dialog is active only display
            // those metrics that exceed the threshold value
            //
            enum DISPLAY_VOXEL {
               DISPLAY_VOXEL_NORMAL,
               DISPLAY_VOXEL_POS_THRESH_COLOR,
               DISPLAY_VOXEL_NEG_THRESH_COLOR,
               DISPLAY_VOXEL_DO_NOT
            };
   
            DISPLAY_VOXEL displayVoxel = DISPLAY_VOXEL_NORMAL;
            if (threshVoxel >= 0.0) {
               if (threshVoxel < thresholdPositiveValue) {
                  displayVoxel = DISPLAY_VOXEL_DO_NOT;
                  if (showThreshVoxels) {
                     if (threshVoxel != 0.0) {
                        displayVoxel = DISPLAY_VOXEL_POS_THRESH_COLOR;
                     }
                  }
               }
            }
            if (threshVoxel <= 0.0) {
               if (threshVoxel > thresholdNegativeValue) {
                  displayVoxel = DISPLAY_VOXEL_DO_NOT;
                  if (showThreshVoxels) {
                     if (threshVoxel != 0.0) {
                        displayVoxel = DISPLAY_VOXEL_NEG_THRESH_COLOR;
                     }
                  }
               }
            }
            
            switch(dsm->getDisplayMode()) {
               case DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_AND_NEGATIVE:
                  break;
               case DisplaySettingsMetric::METRIC_DISPLAY_MODE_NEGATIVE_ONLY:
                  if (voxel >= 0.0) {
                     displayVoxel = DISPLAY_VOXEL_DO_NOT;
                  }
                  break;
               case DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_ONLY:
                  if (voxel <= 0.0) {
                     displayVoxel = DISPLAY_VOXEL_DO_NOT;
                  }
                  break;
            }
               
            switch (displayVoxel) {
               case DISPLAY_VOXEL_NORMAL:
                  {
                     float normalized = 0.0;
                     
                     if ((palette->getNumberOfPaletteEntries() == 2) &&
                        interpolateColor) {
                        //
                        // Normalize between [0, 1.0] when two color palette interpolate
                        //
                        float diffMetric = posMaxMetric - negMaxMetric;
                        if (diffMetric == 0.0) {
                           diffMetric = 1.0;
                        }
                        normalized = (voxel - negMaxMetric) / diffMetric;
                     }
                     else {
                        if (voxel >= posMinMetric) {
                           const float numerator = voxel - posMinMetric;
                           float denominator = posMaxMetric - posMinMetric;
                           if (denominator == 0.0) {
                              denominator = 1.0;
                           }
                           normalized = numerator / denominator; 
                        }
                        else if (voxel <= negMinMetric) {
                           const float numerator = voxel - negMinMetric;
                           float denominator = negMaxMetric - negMinMetric;
                           if (denominator == 0.0) {
                              denominator = 1.0;
                           }
                           else if (denominator < 0.0) {
                              denominator = -denominator;
                           }
                           normalized = numerator / denominator; 
                           //
                           // allow a "Postive Only" palette with "Negative Only" displayed
                           //
                           if (palette->getPositiveOnly() &&
                              (dsm->getDisplayMode() == 
                                    DisplaySettingsMetric::METRIC_DISPLAY_MODE_NEGATIVE_ONLY)) {
                              normalized = -normalized;
                           }
                        }  
                     }
                        
                     bool isNoneColor = false;
                     unsigned char colors[3];
                     palette->getColor(normalized, interpolateColor,
                                       isNoneColor, colors);
                     
                     if (isNoneColor == false) { 
                        rgb[0] = colors[0];
                        rgb[1] = colors[1]; 
                        rgb[2] = colors[2]; 
                        rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
                     }
                  }
                  break;
               case DISPLAY_VOXEL_POS_THRESH_COLOR:
                  rgb[0] = posThreshColor[0];
                  rgb[1] = posThreshColor[1]; 
                  rgb[2] = posThreshColor[2]; 
                  rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
                  break;
               case DISPLAY_VOXEL_NEG_THRESH_COLOR:
                  rgb[0] = negThreshColor[0];
                  rgb[1] = negThreshColor[1]; 
                  rgb[2] = negThreshColor[2]; 
                  rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
                  break;
               case DISPLAY_VOXEL_DO_NOT:
                  break;
            } // switch
            
            vf->setVoxelColor(i, j, k, rgb);
         }
         break;
      case VolumeFile::VOLUME_TYPE_PAINT:
         {
            const int numRegionNames = vf->getNumberOfRegionNames();
            if (numRegionNames > 0) {
               AreaColorFile* cf = brainSet->getAreaColorFile();
               
               //
               // Get first volume
               //
               VolumeFile* firstVolumeFile = brainSet->getVolumePaintFile(0);
               
               //
               // Assing colors to the voxels
               //
               rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;
               if (voxel > 0) {
                  const int paintIndex = static_cast<int>(voxel);
                  const QString name = vf->getRegionNameFromIndex(paintIndex);
                  bool match;
                  const int colorFileIndex = cf->getColorIndexByName(name, match);
                  if (colorFileIndex >= 0) {
                     cf->getColorByIndex(colorFileIndex, rgb[0], rgb[1], rgb[2]);
                     if (firstVolumeFile->getHighlightRegionNameByIndex(paintIndex)) {
                        rgb[0] = 0;
                        rgb[1] = 255;
                        rgb[2] = 0;
                     }
                     rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
                  }
               }
            }
            else {
               rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;
            }
            vf->setVoxelColor(i, j, k, rgb);
         }
         break;
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         {
            DisplaySettingsProbabilisticAtlas* dspa = 
                      brainSet->getDisplaySettingsProbabilisticAtlasVolume();
            switch (dspa->getDisplayType()) {
               case DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_NORMAL:
                  assignNormalProbAtlasColor(i, j, k, rgb);
                  break;
               case DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_THRESHOLD:
                  assignThresholdProbAtlasColor(i, j, k, rgb);
                  break;
            }
            
            if ((rgb[0] > 0) || (rgb[1] > 0) || (rgb[2] > 0)) {
               rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
            }
            else {
               rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;
            }
            vf->setVoxelColor(i, j, k, rgb);
         }
         break;
      case VolumeFile::VOLUME_TYPE_RGB:     
         rgb[0] = static_cast<unsigned char>(vf->getVoxel(i, j, k, 0));
         rgb[1] = static_cast<unsigned char>(vf->getVoxel(i, j, k, 1));
         rgb[2] = static_cast<unsigned char>(vf->getVoxel(i, j, k, 2));
         if ((rgb[0] > 0) || (rgb[1] > 0) || (rgb[2] > 0)) {
            rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
         }
         else {
            rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;
         }
         vf->setVoxelColor(i, j, k, rgb);
         break;
      case VolumeFile::VOLUME_TYPE_ROI:
         if (voxel != 0) {
            rgb[0] = 0;
            rgb[1] = 255;
            rgb[2] = 0;
            rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
         }
         else {
            rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;
         }
         vf->setVoxelColor(i, j, k, rgb);
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION:
         {
            unsigned char onColor[4];
            getSegmentationVoxelOnColor(onColor);
            
            if (voxel != 0) {
               rgb[0] = onColor[0];
               rgb[1] = onColor[1];
               rgb[2] = onColor[2];
               rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
            }
            else {
               rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;
            }
            vf->setVoxelColor(i, j, k, rgb);
         }
         break;
      case VolumeFile::VOLUME_TYPE_VECTOR:
         //
         // Do not use colors for voxels
         //
         rgb[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;
         vf->setVoxelColor(i, j, k, rgb);
         break;
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;
   }   
}

/**
 * Assign normal probabilistic coloring to a voxel.
 */
void
BrainModelVolumeVoxelColoring::assignNormalProbAtlasColor(const int iv,
                                                          const int jv,
                                                          const int kv,
                                                          unsigned char rgb[4])
{
   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlasVolume();
   AreaColorFile* cf = brainSet->getAreaColorFile();
   
   const int numberOfVolumes = brainSet->getNumberOfVolumeProbAtlasFiles();
   const int numSelectedChannels = dspa->getNumberOfChannelsSelected();

   //BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   
   //
   // Initialize to background color
   //
   rgb[0] = 0;
   rgb[1] = 0;
   rgb[2] = 0;
   
   bool highlightFlag = false;
   
   if (numSelectedChannels > 0) {
      int* paintIndices = new int[numberOfVolumes];
      int count = 0;
      VolumeFile* firstVolumeFile = NULL;
      for (int fileNum = 0; fileNum < numberOfVolumes; fileNum++) {
         VolumeFile* vf = brainSet->getVolumeProbAtlasFile(fileNum);
         if (fileNum == 0) {
            firstVolumeFile = vf;
         }
         if (dspa->getChannelSelected(fileNum)) {
            const int voxel = static_cast<int>(vf->getVoxel(iv, jv, kv));
            
            //
            // Convert voxel into its place in the prob atlas name table
            //
            const int paintIndex = voxel; //vf->getRegionNameFromIndex(voxel);
            
            // check > 0 since ??? is always first and is not a valid atlas index
            if ((paintIndex > 0) && 
                (paintIndex < vf->getNumberOfRegionNames())) {
               if (dspa->getAreaSelected(paintIndex)) {            
                  paintIndices[count] = paintIndex;
                  count++;
               }
               
               if (firstVolumeFile->getHighlightRegionNameByIndex(paintIndex)) {
                  highlightFlag = true;
               }
            }
         }
      }
      
      if (count > 0) {
         const VolumeFile* firstVolumeFile = brainSet->getVolumeProbAtlasFile(0);
         // clear colors since we have probabilistic data for this voxel
         rgb[0] = 0; 
         rgb[1] = 0; 
         rgb[2] = 0; 
         
         if (highlightFlag) {
            rgb[1] = 255;
         }
         else {
            for (int m = 0; m < count; m++) {
               QString colorName(firstVolumeFile->getRegionNameFromIndex(paintIndices[m]));
               if (dspa->getTreatQuestColorAsUnassigned()) {
                  if (colorName == "???") {
                     colorName = "Unassigned";
                  }
               }
               bool exactMatch;
               const int areaColorIndex = cf->getColorIndexByName(colorName, exactMatch);
               if (areaColorIndex >= 0) {
                  unsigned char r, g, b;
                  cf->getColorByIndex(areaColorIndex, r, g, b);
                  rgb[0] += (unsigned char)((r / (float)(numSelectedChannels)));
                  rgb[1] += (unsigned char)((g / (float)(numSelectedChannels)));
                  rgb[2] += (unsigned char)((b / (float)(numSelectedChannels)));
               }
            }
         }
      }
      
      if (highlightFlag) {
         rgb[0] = 0;
         rgb[1] = 255; 
         rgb[2] = 0;
      }
      delete[] paintIndices;
   }
}

/**
 * Assign threshold probabilistic coloring to a voxel.
 */
void
BrainModelVolumeVoxelColoring::assignThresholdProbAtlasColor(const int iv,
                                                             const int jv,
                                                             const int kv,
                                                             unsigned char rgb[4])
{
   rgb[0] = 0;
   rgb[1] = 0;
   rgb[2] = 0;

   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlasVolume();
   AreaColorFile* cf = brainSet->getAreaColorFile();
   //BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   
   const int numberOfVolumes = brainSet->getNumberOfVolumeProbAtlasFiles();
   if (numberOfVolumes < 0) {
      return;
   }
   const VolumeFile* firstVolumeFile = brainSet->getVolumeProbAtlasFile(0);
   const int numSelectedChannels = dspa->getNumberOfChannelsSelected();

   unsigned char anyAreaColor[3] = { 100, 100, 100 };
   bool anyAreaColorValid = false;
   cf->getColorByName("ANYAREA", anyAreaColorValid, anyAreaColor[0], anyAreaColor[1], anyAreaColor[2]);

   if (numSelectedChannels > 0) {
      std::map<int,int> indexCounterMap;
      
      const int numPaintNames = firstVolumeFile->getNumberOfRegionNames();
      bool atLeastOneNonZero = false;
      for (int volNum = 0; volNum < numberOfVolumes; volNum++) {
         VolumeFile* vf = brainSet->getVolumeProbAtlasFile(volNum);
         if (volNum == 0) {
            firstVolumeFile = vf;
         }
         int cntIndex = 0;
         if (dspa->getChannelSelected(volNum)) {
            const int voxel = static_cast<int>(vf->getVoxel(iv, jv, kv));
            cntIndex = voxel; //bmv->getProbAtlasNameTableIndex(volNum, voxel);
         }
         if ((cntIndex > 0) && (cntIndex < numPaintNames)) {
            if (dspa->getAreaSelected(cntIndex) == false) {
               cntIndex = -1;
            }
         }
         if (cntIndex > 0) {
            //
            // Skip non-sulci
            //
            bool useIt = true;
            if ((vf->getRegionNameFromIndex(cntIndex) == "???") ||
                (vf->getRegionNameFromIndex(cntIndex) == "GYRAL") ||
                (vf->getRegionNameFromIndex(cntIndex) == "GYRUS")) {
               useIt = false;
            }
            
            if (useIt) {
               atLeastOneNonZero = true;

               std::map<int,int>::iterator iter = indexCounterMap.find(cntIndex);
               if (iter != indexCounterMap.end()) {
                  iter->second++;
               }
               else {
                  indexCounterMap[cntIndex] = 1;
               }
            }
         }
      }

      int paintColIndex = -1;
      if (indexCounterMap.empty() == false) {
         int maxIndex = -1;
         int maxCount = -1;
         for (std::map<int,int>::iterator iter = indexCounterMap.begin();
              iter != indexCounterMap.end(); iter++) {
            if (iter->second > maxCount) {
               maxIndex = iter->first;
               maxCount = iter->second;
            }
         }
         
         if (maxCount >= 0) {
            const float percentSelected = static_cast<float>(maxCount)
                                        / static_cast<float>(numSelectedChannels);
            if (percentSelected >= dspa->getThresholdDisplayTypeRatio()) {
               paintColIndex = maxIndex;
            }
         }
      }

      if (paintColIndex >= 0) {
         const QString paintName = firstVolumeFile->getRegionNameFromIndex(paintColIndex);
         bool match = false;
         const int areaColorIndex = cf->getColorIndexByName(paintName, match);
         if (areaColorIndex >= 0) {
            unsigned char r, g, b;
            cf->getColorByIndex(areaColorIndex, r, g, b);
            rgb[0] = r;
            rgb[1] = g;
            rgb[2] = b;
            
            if (firstVolumeFile->getHighlightRegionNameByIndex(paintColIndex)) {
               rgb[0] = 0;
               rgb[1] = 255;
               rgb[2] = 0;
            }
         }
         else {
             rgb[0] = anyAreaColor[0];
             rgb[1] = anyAreaColor[1];
             rgb[2] = anyAreaColor[2];
         }
      }
      else if (atLeastOneNonZero && anyAreaColorValid) {
          rgb[0] = anyAreaColor[0];
          rgb[1] = anyAreaColor[1];
          rgb[2] = anyAreaColor[2];
      }
   }
}

/**
 * Get the colors for an "on" segmentation voxel
 */
void
BrainModelVolumeVoxelColoring::getSegmentationVoxelOnColor(unsigned char rgbs[4])
{
   rgbs[0] = 255;
   rgbs[1] = 0;
   rgbs[2] = 0;
   rgbs[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID;  // causes voxel to display
}

/**
 * Get the colors for an "off" segmentation voxel
 */
void
BrainModelVolumeVoxelColoring::getSegmentationVoxelOffColor(unsigned char rgbs[4])
{
   rgbs[0] = 0;
   rgbs[1] = 0;
   rgbs[2] = 0;
   rgbs[3] = VolumeFile::VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL;  // causes voxel to NOT display
}

//
// Strings for showing and saving scenes
//
static const QString underlayNameID("Volume-Underlay-Name");
static const QString secondaryOverlayNameID("Volume-Secondary-Overlay-Name");
static const QString primaryOverlayNameID("Volume-Primary-Overlay-Name");

static const QString ouNoneName("none");
static const QString ouAnatomyName("anatomy");
static const QString ouFunctionalName("functional");
static const QString ouPaintName("paint");
static const QString ouProbabilisticAtlasName("probabilistic-atlas");
static const QString ouRgbName("rgb");
static const QString ouSegmentationName("segmentation");
static const QString ouVectorName("vector");

/**
 * apply a scene (set display settings).
 */
void 
BrainModelVolumeVoxelColoring::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "BrainModelVolumeVoxelColoring") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            //
            // Is this for underlay or overlay ?
            //
            if ((infoName == underlayNameID) ||
                (infoName == secondaryOverlayNameID) ||
                (infoName == primaryOverlayNameID)) {
               UNDERLAY_OVERLAY_TYPE  selection  = UNDERLAY_OVERLAY_NONE;
               
               const QString value = si->getValueAsString();
               if (value == ouAnatomyName) {
                  selection = UNDERLAY_OVERLAY_ANATOMY;
                  if (brainSet->getNumberOfVolumeAnatomyFiles() <= 0) {
                     errorMessage.append("No anatomy volumes are loaded\n");
                  }
               }
               else if (value == ouFunctionalName) {
                  selection = UNDERLAY_OVERLAY_FUNCTIONAL;
                  if (brainSet->getNumberOfVolumeFunctionalFiles() <= 0) {
                     errorMessage.append("No functional volumes are loaded\n");
                  }
               }
               else if (value == ouPaintName) {
                  selection = UNDERLAY_OVERLAY_PAINT;
                  if (brainSet->getNumberOfVolumePaintFiles() <= 0) {
                     errorMessage.append("No paint volumes are loaded\n");
                  }
               }
               else if (value == ouProbabilisticAtlasName) {
                  selection = UNDERLAY_OVERLAY_PROB_ATLAS;
                  if (brainSet->getNumberOfVolumeProbAtlasFiles() <= 0) {
                     errorMessage.append("No prob atlas volumes are loaded\n");
                  }
               }
               else if (value == ouRgbName) {
                  selection = UNDERLAY_OVERLAY_RGB;
                  if (brainSet->getNumberOfVolumeRgbFiles() <= 0) {
                     errorMessage.append("No rgb volumes are loaded\n");
                  }
               }
               else if (value == ouSegmentationName) {
                  selection = UNDERLAY_OVERLAY_SEGMENTATION;
                  if (brainSet->getNumberOfVolumeSegmentationFiles() <= 0) {
                     errorMessage.append("No segmentation volumes are loaded\n");
                  }
               }
               else if (value == ouVectorName) {
                  selection = UNDERLAY_OVERLAY_VECTOR;
                  if (brainSet->getNumberOfVolumeVectorFiles() <= 0) {
                     errorMessage.append("No vector volumes are loaded\n");
                  }
               }
               
               // 
               // Set the overlay or underlay
               //
               if (infoName == underlayNameID) {
                  setUnderlay(selection);
               }
               else if (infoName == secondaryOverlayNameID) {
                  setSecondaryOverlay(selection);
               }
               else if (infoName == primaryOverlayNameID) {
                  setPrimaryOverlay(selection);
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
BrainModelVolumeVoxelColoring::saveScene(SceneFile::Scene& scene,
                                         const bool /*onlyIfSelectedFlag*/)
{
   //if (onlyIfSelectedFlag) {
      const int numVolumes =
           brainSet->getNumberOfVolumeAnatomyFiles()
         + brainSet->getNumberOfVolumeFunctionalFiles()
         + brainSet->getNumberOfVolumePaintFiles()
         + brainSet->getNumberOfVolumeProbAtlasFiles()
         + brainSet->getNumberOfVolumeRgbFiles()
         + brainSet->getNumberOfVolumeSegmentationFiles()
         + brainSet->getNumberOfVolumeVectorFiles();
      if (numVolumes <= 0) {
         return;
      }
   //}
   
   SceneFile::SceneClass sc("BrainModelVolumeVoxelColoring");
   
   //
   // Do underlay and both overlays
   //
   for (int i = 0; i < 3; i++) {
      QString ouName;
      QString ouValue;
      
      UNDERLAY_OVERLAY_TYPE selection = UNDERLAY_OVERLAY_NONE;
      switch (i) {
         case 0:
            ouName = underlayNameID;
            selection = getUnderlay();
            break;
         case 1:
            ouName = secondaryOverlayNameID;
            selection = getSecondaryOverlay();
            break;
         case 2:
            ouName = primaryOverlayNameID;
            selection = getPrimaryOverlay();
            break;
      }
      switch(selection) {
         case UNDERLAY_OVERLAY_NONE:
            ouValue = ouNoneName;
            break;
         case UNDERLAY_OVERLAY_ANATOMY:
            ouValue = ouAnatomyName;
            break;
         case UNDERLAY_OVERLAY_FUNCTIONAL:
            ouValue = ouFunctionalName;
            break;
         case UNDERLAY_OVERLAY_PAINT:
            ouValue = ouPaintName;
            break;
         case UNDERLAY_OVERLAY_PROB_ATLAS:
            ouValue = ouProbabilisticAtlasName;
            break;
         case UNDERLAY_OVERLAY_RGB:
            ouValue = ouRgbName;
            break;
         case UNDERLAY_OVERLAY_SEGMENTATION:
            ouValue = ouSegmentationName;
            break;
         case UNDERLAY_OVERLAY_VECTOR:
            ouValue = ouVectorName;
            break;
      }
      SceneFile::SceneInfo si(ouName, ouValue);
      sc.addSceneInfo(si);      
   }
   
   scene.addSceneClass(sc);
}

/**
 * see if an overlay or underlay is of a specific type.
 */
bool 
BrainModelVolumeVoxelColoring::isUnderlayOrOverlay(const UNDERLAY_OVERLAY_TYPE uo) const
{
   if ((underlay == uo) ||
       (secondaryOverlay == uo) ||
       (primaryOverlay == uo)) {
      return true;
   }
   return false;
}
                               

