
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

#include "BrainModelSurface.h"
#include "FileUtilities.h"
#include "GuiMapFmriMappingSet.h"
#include "GuiMapFmriMetricInfo.h"
#include "GuiMapFmriVolume.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
GuiMapFmriMetricInfo::GuiMapFmriMetricInfo(const GuiMapFmriMappingSet* surfaceInfo,
                                           const GuiMapFmriVolume* volumeInfo,
                                           const int volumeIndexIn,
                                           const int subVolumeIndexIn)
{
   volumeIndex = volumeIndexIn;
   subVolumeIndex = subVolumeIndexIn;

   QString volumeComment;
   if (volumeInfo != NULL) {
      volumeComment = volumeInfo->getFileComment();
      if (volumeComment.isEmpty() == false) {
         QString s("Volume Comment: ");
         s.append(volumeComment);
         s.append("; ");
         volumeComment = s;
      }
   }
   
   switch (surfaceInfo->getMappingType()) {
      case GuiMapFmriMappingSet::MAPPING_TYPE_NONE:
         return;
         break;
      case GuiMapFmriMappingSet::MAPPING_TYPE_WITH_CARET_SURFACES:
         {
            std::vector<BrainModelSurface*> surfaces;
            surfaceInfo->getMappingCaretSurfaces(surfaces);
            for (unsigned int i = 0; i < surfaces.size(); i++) {
               surfaceNameForMetricColumn.push_back(surfaces[i]->getDescriptiveName());
            }
         }
         break;
      case GuiMapFmriMappingSet::MAPPING_TYPE_WITH_SPEC_FILES:
         {
            std::vector<QString> names;
            surfaceInfo->getMappingCoordFileNames(names);
            for (unsigned int i = 0; i < names.size(); i++) {
               surfaceNameForMetricColumn.push_back(FileUtilities::basename(names[i]));
            }
            
            if (surfaceInfo->getDoAverageFiducialFileFlag()) {
               const QString structName = surfaceInfo->getStructureForColumnNaming();
               
               metricAverageFiducialCoordColumnName = "AFM ";
               if (structName.isEmpty() == false) {
                  metricAverageFiducialCoordColumnName.append(structName);
               }
               metricAverageFiducialCoordColumnName.append(" - ");
               //metricAverageFiducialCoordColumnName.append(surfaceInfo->getAverageFiducialCoordFileName());
               metricAverageFiducialCoordColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
               metricAverageFiducialCoordColumnNameRowNumber = -1;
               metricAverageFiducialCoordComment = "Average Fiducial Mapping (AFM); ";
               metricAverageFiducialCoordComment.append(volumeComment);
               metricAverageFiducialCoordColumnNegativeThreshold = 0.0;
               metricAverageFiducialCoordColumnPositiveThreshold = 0.0;
               float negThresh = 0.0, posThresh = 0.0;
               volumeInfo->getThresholds(negThresh, posThresh);
               metricAverageFiducialCoordColumnNegativeThreshold = negThresh;
               metricAverageFiducialCoordColumnPositiveThreshold = posThresh;
            }
         }
         break;
   }
   
   float negThresh, posThresh;
   volumeInfo->getThresholds(negThresh, posThresh);

   for (unsigned int i = 0; i < surfaceNameForMetricColumn.size(); i++) {
      QString prefix;
      if (surfaceNameForMetricColumn.size() > 1) {
         QString s(StringUtilities::makeLowerCase(surfaceNameForMetricColumn[i]));
         const int caseNameIndex = s.indexOf("case");
         if (caseNameIndex != -1) {
            int periodCount = 0;
            for (int j = caseNameIndex; j < s.length(); j++) {
               if (s[j] == '.') {
                  periodCount++;
                  if (periodCount == 2) {
                     prefix = surfaceNameForMetricColumn[i].mid(caseNameIndex,
                                                        (j - caseNameIndex));
                     break;
                  }
               }
            }
         }
      }
      QString columnName;
      if (prefix.isEmpty() == false) {
         columnName.append("Map to ");
         columnName.append(prefix);
         columnName.append(" - ");
      }
      columnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
      metricColumnNames.push_back(columnName);
      metricColumnNegativeThreshold.push_back(negThresh);
      metricColumnPositiveThreshold.push_back(posThresh);
      metricColumnComments.push_back("");
      metricColumnNamesRowNumber.push_back(-1);
   }
   
   QString allSurfaceNames;
   for (unsigned int i = 0; i < surfaceNameForMetricColumn.size(); i++) {
      allSurfaceNames.append(surfaceNameForMetricColumn[i]);
      allSurfaceNames.append("  ");
   }

   const QString structName = surfaceInfo->getStructureForColumnNaming();

   metricAverageOfAllColumnName = "MFM ";
   if (structName.isEmpty() == false) {
      metricAverageOfAllColumnName.append(structName);
   }
   metricAverageOfAllColumnName.append(" - ");
   metricAverageOfAllColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
   metricAverageOfAllComment.append("Multi-Fiducial Mapping (MFM); ");
   metricAverageOfAllComment.append(volumeComment);
   metricAverageOfAllComment.append("MFM using surface files: ");
   metricAverageOfAllComment.append(allSurfaceNames);
   metricAverageOfAllColumnNameRowNumber = -1;
   metricAverageOfAllColumnNegativeThreshold = negThresh;
   metricAverageOfAllColumnPositiveThreshold = posThresh;
   
   metricStdDevColumnName = "STANDARD DEVIATION (MFM) - ";
   metricStdDevColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
   metricStdDevComment.append("Using surface files: ");
   metricStdDevComment.append(allSurfaceNames);
   metricStdDevColumnNameRowNumber = -1;
   metricStdDevColumnNegativeThreshold = 0.0; //negThresh;
   metricStdDevColumnPositiveThreshold = 0.0; //posThresh;
   
   metricStdErrorColumnName = "STANDARD ERROR (MFM) - ";
   metricStdErrorColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
   metricStdErrorComment.append("Using surface files: ");
   metricStdErrorComment.append(allSurfaceNames);
   metricStdErrorColumnNameRowNumber = -1;
   metricStdErrorColumnNegativeThreshold = 0.0; //negThresh;
   metricStdErrorColumnPositiveThreshold = 0.0; //posThresh;

   metricMinValueColumnName = "MINIMUM (MFM) - ";
   metricMinValueColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
   metricMinValueComment.append("Using surface files: ");
   metricMinValueComment.append(allSurfaceNames);
   metricMinValueColumnNameRowNumber = -1;
   metricMinValueColumnNegativeThreshold = 0.0; //negThresh;
   metricMinValueColumnPositiveThreshold = 0.0; //posThresh;

   metricMaxValueColumnName = "MAXIMUM (MFM) - ";
   metricMaxValueColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
   metricMaxValueComment.append("Using surface files: ");
   metricMaxValueComment.append(allSurfaceNames);
   metricMaxValueColumnNameRowNumber = -1;
   metricMaxValueColumnNegativeThreshold = 0.0; //negThresh;
   metricMaxValueColumnPositiveThreshold = 0.0; //posThresh;

   metricMostCommonValueColumnName = "MFM (Most Common) ";
   if (structName.isEmpty() == false) {
      metricMostCommonValueColumnName.append(structName);
   }
   metricMostCommonValueColumnName.append(" - ");
   metricMostCommonValueColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
   metricMostCommonValueComment.append("Multi-Fiducial Most Common Mapping (MFM); ");
   metricMostCommonValueComment.append(volumeComment);
   metricMostCommonValueComment.append("MFM using surface files: ");
   metricMostCommonValueComment.append(allSurfaceNames);
   metricMostCommonValueColumnNameRowNumber = -1;
   metricMostCommonValueColumnNegativeThreshold = 0.0; //negThresh;
   metricMostCommonValueColumnPositiveThreshold = 0.0; //posThresh;   

   metricMostCommonExcludeUnidentifiedValueColumnName = "MFM (Most Common Exclude No ID) ";
   if (structName.isEmpty() == false) {
      metricMostCommonExcludeUnidentifiedValueColumnName.append(structName);
   }
   metricMostCommonExcludeUnidentifiedValueColumnName.append(" - ");
   metricMostCommonExcludeUnidentifiedValueColumnName.append(volumeInfo->getSubVolumeName(subVolumeIndex));
   metricMostCommonExcludeUnidentifiedValueComment.append("Multi-Fiducial Most Common Exclude No ID Mapping (MFM); ");
   metricMostCommonExcludeUnidentifiedValueComment.append(volumeComment);
   metricMostCommonExcludeUnidentifiedValueComment.append("MFM using surface files: ");
   metricMostCommonExcludeUnidentifiedValueComment.append(allSurfaceNames);
   metricMostCommonExcludeUnidentifiedValueColumnNameRowNumber = -1;
   metricMostCommonExcludeUnidentifiedValueColumnNegativeThreshold = 0.0; //negThresh;
   metricMostCommonExcludeUnidentifiedValueColumnPositiveThreshold = 0.0; //posThresh;   
}      

/**
 * Destructor.
 */
GuiMapFmriMetricInfo::~GuiMapFmriMetricInfo()
{
}

/**
 * get the metric column thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricColumnThresholds(const int indx, float& negThresh, float& posThresh) const
{
   negThresh = metricColumnNegativeThreshold[indx];
   posThresh = metricColumnPositiveThreshold[indx];
}

/**
 * set the metric column thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricColumnThresholds(const int indx, const float negThresh, const float posThresh)
{
   metricColumnNegativeThreshold[indx] = negThresh;
   metricColumnPositiveThreshold[indx] = posThresh;
}

/**
 * get the metric average fiducial coord thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricAverageFiducialCoordThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricAverageFiducialCoordColumnNegativeThreshold;
   posThresh = metricAverageFiducialCoordColumnPositiveThreshold;
}

/**
 * set the metric average fiducial coord thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricAverageFiducialCoordThresholds(const float negThresh, const float posThresh)
{
   metricAverageFiducialCoordColumnNegativeThreshold = negThresh;
   metricAverageFiducialCoordColumnPositiveThreshold = posThresh;
}

/**
 * get the metric average thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricAverageOfAllThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricAverageOfAllColumnNegativeThreshold;
   posThresh = metricAverageOfAllColumnPositiveThreshold;
}

/**
 * set the metric average thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricAverageOfAllThresholds(const float negThresh, const float posThresh)
{
   metricAverageOfAllColumnNegativeThreshold = negThresh;
   metricAverageOfAllColumnPositiveThreshold = posThresh;
}

/**
 * get the metric std dev thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricStdDevThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricStdDevColumnNegativeThreshold;
   posThresh = metricStdDevColumnPositiveThreshold;
}

/**
 * set the metric std dev thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricStdDevThresholds(const float negThresh, const float posThresh)
{
   metricStdDevColumnNegativeThreshold = negThresh;
   metricStdDevColumnPositiveThreshold = posThresh;
}

/**
 * get the metric std error thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricStdErrorThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricStdErrorColumnNegativeThreshold;
   posThresh = metricStdErrorColumnPositiveThreshold;
}

/**
 * set the metric std error thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricStdErrorThresholds(const float negThresh, const float posThresh)
{
   metricStdErrorColumnNegativeThreshold = negThresh;
   metricStdErrorColumnPositiveThreshold = posThresh;
}      

/**
 * get the metric minimum value thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricMinValueThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricMinValueColumnNegativeThreshold;
   posThresh = metricMinValueColumnPositiveThreshold;
}

/**
 * set the metric minimum value thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricMinValueThresholds(const float negThresh, const float posThresh)
{
   metricMinValueColumnNegativeThreshold = negThresh;
   metricMinValueColumnPositiveThreshold = posThresh;
}      
     
/**
 * get the metric maximum value thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricMaxValueThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricMaxValueColumnNegativeThreshold;
   posThresh = metricMaxValueColumnPositiveThreshold;
}

/**
 * set the metric maximum value thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricMaxValueThresholds(const float negThresh, const float posThresh)
{
   metricMaxValueColumnNegativeThreshold = negThresh;
   metricMaxValueColumnPositiveThreshold = posThresh;
}      
     
/**
 * get the metric most common value thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricMostCommonValueThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricMostCommonValueColumnNegativeThreshold;
   posThresh = metricMostCommonValueColumnPositiveThreshold;
}

/**
 * set the metric most common value thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricMostCommonValueThresholds(const float negThresh, const float posThresh)
{
   metricMostCommonValueColumnNegativeThreshold = negThresh;
   metricMostCommonValueColumnPositiveThreshold = posThresh;
}      
     
/**
 * get the metric most common exclude unidentified value thresholds.
 */
void 
GuiMapFmriMetricInfo::getMetricMostCommonExcludeUnidentifiedValueThresholds(float& negThresh, float& posThresh) const
{
   negThresh = metricMostCommonExcludeUnidentifiedValueColumnNegativeThreshold;
   posThresh = metricMostCommonExcludeUnidentifiedValueColumnPositiveThreshold;
}

/**
 * set the metric most common exclude unidentified value thresholds.
 */
void 
GuiMapFmriMetricInfo::setMetricMostCommonExcludeUnidentifiedValueThresholds(const float negThresh, const float posThresh)
{
   metricMostCommonExcludeUnidentifiedValueColumnNegativeThreshold = negThresh;
   metricMostCommonExcludeUnidentifiedValueColumnPositiveThreshold = posThresh;
}      
     
