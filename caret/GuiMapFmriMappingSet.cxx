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
#include <QDateTime>

#include "BrainModelSurface.h"
#include "FileUtilities.h"
#define __GUI_MAP_FMRI_MAPPING_SET_MAIN__
#include "GuiMapFmriMappingSet.h"
#undef __GUI_MAP_FMRI_MAPPING_SET_MAIN__
#include "GuiMapFmriVolume.h"
#include "StringUtilities.h"

/**
 * Constructor for mapping to spec files.
 */
GuiMapFmriMappingSet::GuiMapFmriMappingSet(const QString& outputSpecFileNameIn,
                     const QString& mappingFilesPathIn,
                     const QString& topoFileNameIn,
                     const std::vector<QString>& coordFileNamesIn,
                     const QString& averageFiducialFileNameIn,
                     const std::vector<GuiMapFmriVolume*>& volumes,
                     const QString& fileNameExtensionIn,
                     const QString& descriptionIn,
                     const QString& metricNameHintIn,
                     const QString& structureNameIn,
                     const bool mappingWithAtlasFlagIn,
                     const bool doAverageFiducialFileFlagIn,
                     const bool doAvgOfAllCoordFileFlagIn,
                     const bool doStdDevOfAllCoordFileFlagIn,
                     const bool doStdErrorOfAllCoordFileFlagIn,
                     const bool doMinOfAllCoordFileFlagIn,
                     const bool doMaxOfAllCoordFileFlagIn,
                     const bool doAllCasesCoordFileFlagIn,
                     const bool doMostCommonOfAllCasesFlagIn,
                     const bool doMostCommonExcludeUnidentifiedOfAllCasesFlagIn)
{
   mappingType = MAPPING_TYPE_WITH_SPEC_FILES;
   mappingFilesPath = mappingFilesPathIn;
   outputSpecFileName = outputSpecFileNameIn;
   mappingTopoFileName   = topoFileNameIn;
   mappingCoordFileNames = coordFileNamesIn;
   mappingWithAtlasFlag = mappingWithAtlasFlagIn;
   metricOutputType = METRIC_OUTPUT_TYPE_SPEC_FILE;
   metricNameHint = metricNameHintIn;
   description = descriptionIn;
   fileNameExtension = fileNameExtensionIn;
   structureName = structureNameIn;
   
   averageFiducialFileName = averageFiducialFileNameIn;
   doAverageFiducialFileFlag = doAverageFiducialFileFlagIn;
   doAvgOfAllCoordFileFlag = doAvgOfAllCoordFileFlagIn;
   doStdDevOfAllCoordFileFlag = doStdDevOfAllCoordFileFlagIn;
   doStdErrorOfAllCoordFileFlag = doStdErrorOfAllCoordFileFlagIn;
   doMinOfAllCoordFileFlag = doMinOfAllCoordFileFlagIn;
   doMaxOfAllCoordFileFlag = doMaxOfAllCoordFileFlagIn;
   doAllCasesCoordFileFlag = doAllCasesCoordFileFlagIn;
   doMostCommonOfAllCasesFlag = doMostCommonOfAllCasesFlagIn;
   doMostCommonExcludeUnidentifiedOfAllCasesFlag = doMostCommonExcludeUnidentifiedOfAllCasesFlagIn;
   
   initializeMetricFile(volumes);
}

/**
 * Constructor for mapping to caret surfaces.
 */
GuiMapFmriMappingSet::GuiMapFmriMappingSet(
                           std::vector<BrainModelSurface*> caretSurfacesIn,
                           const std::vector<GuiMapFmriVolume*>& volumes,
                           const QString& fileNameExtensionIn,
                           const QString& descriptionIn)
{
   mappingType = MAPPING_TYPE_WITH_CARET_SURFACES;
   mappingCaretSurfaces = caretSurfacesIn;
   mappingWithAtlasFlag = false;
   metricOutputType = METRIC_OUTPUT_TYPE_CARET_METRIC;
   metricNameHint = "";
   description = descriptionIn;
   fileNameExtension = fileNameExtensionIn;
   initializeMetricFile(volumes);
   
   averageFiducialFileName = "";
   doAverageFiducialFileFlag = false;
   doAvgOfAllCoordFileFlag = false;
   doStdDevOfAllCoordFileFlag = false;
   doStdErrorOfAllCoordFileFlag = false;
   doMinOfAllCoordFileFlag = false;
   doMaxOfAllCoordFileFlag = false;
   doAllCasesCoordFileFlag = true;
   doMostCommonOfAllCasesFlag = false;
}

/**
 * Constructor for mapping to caret surfaces with an atlas.
 */
GuiMapFmriMappingSet::GuiMapFmriMappingSet(const QString& mappingFilesPathIn,
                     const QString& mappingTopoFileNameIn,
                     const std::vector<QString>& mappingCoordFileNamesIn,
                     const QString& averageFiducialFileNameIn,
                     const std::vector<GuiMapFmriVolume*>& volumes,
                     const QString& fileNameExtensionIn,
                     const QString& descriptionIn,
                     const QString& structureNameIn,
                     const bool doAverageFiducialFileFlagIn,
                     const bool doAvgOfAllCoordFileFlagIn,
                     const bool doStdDevOfAllCoordFileFlagIn,
                     const bool doStdErrorOfAllCoordFileFlagIn,
                     const bool doMinOfAllCoordFileFlagIn,
                     const bool doMaxOfAllCoordFileFlagIn,
                     const bool doAllCasesCoordFileFlagIn,
                     const bool doMostCommonOfAllCasesFlagIn,
                     const bool doMostCommonExcludeUnidentifiedOfAllCasesFlagIn)
{
   mappingType = MAPPING_TYPE_WITH_SPEC_FILES;
   mappingFilesPath = mappingFilesPathIn;
   mappingTopoFileName   = mappingTopoFileNameIn;
   mappingCoordFileNames = mappingCoordFileNamesIn;
   mappingWithAtlasFlag = true;
   metricOutputType = METRIC_OUTPUT_TYPE_CARET_METRIC;
   metricNameHint = "";
   description = descriptionIn;
   fileNameExtension = fileNameExtensionIn;
   structureName = structureNameIn;
   
   averageFiducialFileName = averageFiducialFileNameIn;
   doAverageFiducialFileFlag = doAverageFiducialFileFlagIn;
   doAvgOfAllCoordFileFlag = doAvgOfAllCoordFileFlagIn;
   doStdDevOfAllCoordFileFlag = doStdDevOfAllCoordFileFlagIn;
   doStdErrorOfAllCoordFileFlag = doStdErrorOfAllCoordFileFlagIn;
   doMinOfAllCoordFileFlag = doMinOfAllCoordFileFlagIn;
   doMaxOfAllCoordFileFlag = doMaxOfAllCoordFileFlagIn;
   doAllCasesCoordFileFlag = doAllCasesCoordFileFlagIn;
   doMostCommonOfAllCasesFlag = doMostCommonOfAllCasesFlagIn;
   doMostCommonExcludeUnidentifiedOfAllCasesFlag = doMostCommonExcludeUnidentifiedOfAllCasesFlagIn;
   
   initializeMetricFile(volumes);
}                            

/**
 * Destructor.
 */
GuiMapFmriMappingSet::~GuiMapFmriMappingSet()
{
}

/**
 * Initialize the metric file naming
 */
void
GuiMapFmriMappingSet::initializeMetricFile(const std::vector<GuiMapFmriVolume*>& volumes)
{
   //
   // Initialize the name of the metric file
   //
   QDateTime dt = QDateTime::currentDateTime();
   const QString dts = dt.toString("dd_MMM_yyyy_hh_mm_ss");
 
   std::ostringstream str;
   str << "map_data_"
       << uniqueNameCounter
       << "_";
   if (metricNameHint.isEmpty() == false) {
       str << metricNameHint.toAscii().constData()
           << "_";
   }
   str << dts.toAscii().constData()
       << fileNameExtension.toAscii().constData();
   metricFileName = str.str().c_str(); 
    
   uniqueNameCounter++;   

   // 01 SEP 2004
   //
   // For each volume
   //
   for (int j = 0; j < static_cast<int>(volumes.size()); j++) {
      //
      // Get the number of sub-volumes
      //
      const GuiMapFmriVolume* v = volumes[j];
      const int numSubVolumes = v->getNumberOfSubVolumes();
      
      //
      // For each subvolume
      //
      for (int k = 0; k < numSubVolumes; k++) {
         GuiMapFmriMetricInfo metricInfo(this,
                                         v,
                                         j,
                                         k);
         metricMappingInfo.push_back(metricInfo);
      }
   }

}

/**
 * Get a descriptive label.
 */
QString
GuiMapFmriMappingSet::getDescriptiveLabel() const
{
   QString label;
   
   switch (metricOutputType) {
      case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_NONE:
         label.append("Invalid");
         break;
      case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_CARET_METRIC:
         label.append("Caret (");
         if (mappingWithAtlasFlag) {
            label.append(StringUtilities::fromNumber(static_cast<int>(mappingCoordFileNames.size())));
            label.append(" atlas coord files");
         }
         else {
            label.append(StringUtilities::fromNumber(static_cast<int>(mappingCaretSurfaces.size())));
            label.append(" surfaces");
         }
         if (description.isEmpty() == false) {
            label.append(" - ");
            label.append(description);
         }
         label.append(")");
         break;
      case GuiMapFmriMappingSet::METRIC_OUTPUT_TYPE_SPEC_FILE:
         label.append(FileUtilities::basename(outputSpecFileName));
         label.append(" (");
         label.append(StringUtilities::fromNumber(static_cast<int>(mappingCoordFileNames.size())));
         if (mappingWithAtlasFlag) {
            label.append(" atlas coord files");
         }
         else {
            label.append(" coord files");
         }
         if (description.isEmpty() == false) {
            label.append(" - ");
            label.append(description);
         }
         label.append(")");
         break;
   }
   
   return label;
}

/**
 * get the name for a structure for column naming.
 */
QString 
GuiMapFmriMappingSet::getStructureForColumnNaming() const
{
   Structure s(structureName);
   if (s.isLeftCortex()) {
      return "LEFT";
   }
   else if (s.isRightCortex()) {
      return "RIGHT";
   }
   return "";
}
      
