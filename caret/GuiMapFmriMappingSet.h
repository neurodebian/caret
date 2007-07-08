
#ifndef __GUI_MAP_FMRI_MAPPING_SET_H__
#define __GUI_MAP_FMRI_MAPPING_SET_H__

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

#include <QString>
#include <vector>

#include "GuiMapFmriMetricInfo.h"
#include "MetricFile.h"

class BrainModelSurface;
class GuiMapFmriVolume;

/// class for storing mapping surface information
class GuiMapFmriMappingSet {
   public:
      /// type of metric output
      enum METRIC_OUTPUT_TYPE {
         METRIC_OUTPUT_TYPE_NONE,
         METRIC_OUTPUT_TYPE_CARET_METRIC,
         METRIC_OUTPUT_TYPE_SPEC_FILE
      };
      
      /// type of mapping
      enum MAPPING_TYPE {
         MAPPING_TYPE_NONE,
         MAPPING_TYPE_WITH_CARET_SURFACES,
         MAPPING_TYPE_WITH_SPEC_FILES
      };
      
      /// Constructor for mapping to spec files both with and without atlas
      GuiMapFmriMappingSet(const QString& outputSpecFileNameIn,
                           const QString& mappingFilesPathIn,
                           const QString& topoFileNameIn,
                           const std::vector<QString>& coordFileNamesIn,
                           const QString& avgCoordFileNameIn,
                           const std::vector<GuiMapFmriVolume*>& volumes,
                           const QString& fileNameExtensionIn,
                           const QString& descriptionIn,
                           const QString& metricNameHintIn,
                           const QString& structureNameIn,
                           const bool mappingWithAtlasFlagIn,
                           const bool doAvgCoordFileFlagIn,
                           const bool doAvgOfAllCoordFileFlagIn,
                           const bool doStdDevOfAllCoordFileFlagIn,
                           const bool doStdErrorOfAllCoordFileFlagIn,
                           const bool doMinOfAllCoordFileFlagIn,
                           const bool doMaxOfAllCoordFileFlagIn,
                           const bool doAllCasesCoordFileFlagIn);
      
      /// Constructor for mapping to caret surfaces
      GuiMapFmriMappingSet(std::vector<BrainModelSurface*> caretSurfacesIn,
                           const std::vector<GuiMapFmriVolume*>& volumes,
                           const QString& fileNameExtensionIn,
                           const QString& descriptionIn);
     
      /// Constructor for mapping to caret surfaces with an atlas
      GuiMapFmriMappingSet(const QString& mappingFilesPathIn,
                           const QString& mappingTopoFileNameIn,
                           const std::vector<QString>& mappingCoordFileNamesIn,
                           const QString& avgCoordFileNameIn,
                           const std::vector<GuiMapFmriVolume*>& volumes,
                           const QString& fileNameExtensionIn,
                           const QString& descriptionIn,
                           const QString& structureNameIn,
                           const bool doAvgCoordFileFlagIn,
                           const bool doAvgOfAllCoordFileFlagIn,
                           const bool doStdDevOfAllCoordFileFlagIn,
                           const bool doStdErrorOfAllCoordFileFlagIn,
                           const bool doMinOfAllCoordFileFlagIn,
                           const bool doMaxOfAllCoordFileFlagIn,
                           const bool doAllCasesCoordFileFlagIn);
                            
      /// Destructor
      ~GuiMapFmriMappingSet();

      /// get the mapping type
      MAPPING_TYPE getMappingType() const { return mappingType; }
      
      /// get the metric output type
      METRIC_OUTPUT_TYPE getMetricOutputType() const { return metricOutputType; } 

      /// get the output spec file name
      QString getOutputSpecFileName() const { return outputSpecFileName; }
      
      /// get the location of the mapping files
      QString getMappingFilesPath() const { return mappingFilesPath; }
      
      /// get the topology file name for mapping data
      QString getMappingTopoFileName() const { return mappingTopoFileName; }
      
      /// get the coordinate file names for mapping data
      void getMappingCoordFileNames(std::vector<QString>& names) const
                                         { names = mappingCoordFileNames; }
   
      /// name of the avg fiducial file 
      QString getAverageFiducialCoordFileName() const { return averageFiducialFileName; }
      
      /// get the surfaces for mapping data
      void getMappingCaretSurfaces(std::vector<BrainModelSurface*>& surfaces) const
                                         { surfaces = mappingCaretSurfaces; }
      
      /// Get a descriptive label.
      QString getDescriptiveLabel() const;

      /// Get the name of the metric file
      QString getMetricFileName() const { return metricFileName; }
      
      /// Set the name of the metric file
      void setMetricFileName(const QString& name) { metricFileName = name; }
      
      /// get flag indicating mapping with atlas files
      bool getMappingWithAtlasFlag() const { return mappingWithAtlasFlag; }

      /// Get the number of metric mapping info (one per (vol * subvols))
      int getNumberOfMetricMappingInfo() const { return metricMappingInfo.size(); }
      
      /// Get metric mapping info
      GuiMapFmriMetricInfo* getMetricMappingInfo(const int indx)
                                                { return &metricMappingInfo[indx]; }
          
      /// Get metric mapping info (const method)
      const GuiMapFmriMetricInfo* getMetricMappingInfo(const int indx) const
                                                { return &metricMappingInfo[indx]; }
          
      /// flag for average fiducial file
      bool getDoAverageFiducialFileFlag() const { return doAverageFiducialFileFlag; }
      
      /// flag for average of all coord files
      bool getDoAvgOfAllCoordFileFlag() const { return doAvgOfAllCoordFileFlag; }
      
      /// flag for std dev of all coord files
      bool getDoStdDevOfAllCoordFileFlag() const { return doStdDevOfAllCoordFileFlag; }
      
      /// flag for std error of all coord files
      bool getDoStdErrorOfAllCoordFileFlag() const { return doStdErrorOfAllCoordFileFlag; }
      
      /// flag for min of all coord files
      bool getDoMinOfAllCoordFileFlag() const { return doMinOfAllCoordFileFlag; }
      
      /// flag for max of all coord files
      bool getDoMaxOfAllCoordFileFlag() const { return doMaxOfAllCoordFileFlag; }
      
      /// flag for do all indiv case files
      bool getDoAllCasesCoordFileFlag() const { return doAllCasesCoordFileFlag; }
      
      /// get the name for a structure for column naming
      QString getStructureForColumnNaming() const;
      
   protected:
      /// metric mapping information
      std::vector<GuiMapFmriMetricInfo> metricMappingInfo;
      
      /// Initialize the metric file naming
      void initializeMetricFile(const std::vector<GuiMapFmriVolume*>& volumes);

      /// location of the mapping files (topo and coord)
      QString mappingFilesPath;
      
      /// name and path of output spec file (for mappingType == MAPPING_TYPE_TO_SPEC_FILES)
      QString outputSpecFileName;
      
      /// name of topology file (for mappingType == MAPPING_TYPE_TO_SPEC_FILES
      QString mappingTopoFileName;
      
      /// names of coordinate files (for mappingType == MAPPING_TYPE_TO_SPEC_FILES
      std::vector<QString> mappingCoordFileNames;
      
      /// name of the average fiducial file 
      QString averageFiducialFileName;
      
      /// caret surfaces that are to be mapped  (for mappingType == MAPPING_TYPE_TO_CARET_SURFACES)
      std::vector<BrainModelSurface*> mappingCaretSurfaces;
      
      /// the mapping type
      MAPPING_TYPE mappingType;    
      
      /// name of the metric file
      QString metricFileName;
      
      /// counter to ensure that metric files are uniquely named
      static int uniqueNameCounter;
      
      /// the metric output type
      METRIC_OUTPUT_TYPE metricOutputType;
      
      /// metric file name hint
      QString metricNameHint;
      
      /// description of atlas
      QString description;
      
      /// file name extension
      QString fileNameExtension;
      
      /// name of structure
      QString structureName;
      
      /// flag indicating data is being mapped with an atlas
      bool mappingWithAtlasFlag; 
      
      /// flag for average coord file
      bool doAverageFiducialFileFlag;
      
      /// flag for average of all coord files
      bool doAvgOfAllCoordFileFlag;
      
      /// flag for std dev of all coord files
      bool doStdDevOfAllCoordFileFlag;
      
      /// flag for std error of all coord files
      bool doStdErrorOfAllCoordFileFlag;
      
      /// flag for min of all coord files
      bool doMinOfAllCoordFileFlag;
      
      /// flag for max of all coord files
      bool doMaxOfAllCoordFileFlag;
      
      /// flag for do all indiv case files
      bool doAllCasesCoordFileFlag;
      
};

#ifdef __GUI_MAP_FMRI_MAPPING_SET_MAIN__

int GuiMapFmriMappingSet::uniqueNameCounter = 0;

#endif // __GUI_MAP_FMRI_MAPPING_SET_MAIN__

#endif // __GUI_MAP_FMRI_MAPPING_SET_H__

