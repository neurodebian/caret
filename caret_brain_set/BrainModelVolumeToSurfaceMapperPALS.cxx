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

#include <set>

#include <QDir>

#include "BrainModelSurface.h"
#include "BrainModelVolumeToSurfaceMapper.h"
#include "BrainModelVolumeToSurfaceMapperPALS.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "MapFmriAtlasSpecFileInfo.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelVolumeToSurfaceMapperPALS::BrainModelVolumeToSurfaceMapperPALS(BrainSet* bsIn,
                     VolumeFile* volumeFileIn,
                     const QString& stereotaxicSpaceNameIn,
                     const Structure& structureIn,
                     const BrainModelVolumeToSurfaceMapperAlgorithmParameters& mappingParametersIn,
                     GiftiNodeDataFile* dataFileIn)
   : BrainModelAlgorithm(bsIn)
{
   volumeFile = volumeFileIn;
   stereotaxicSpaceName = stereotaxicSpaceNameIn;
   structure  = structureIn;
   mappingParameters = mappingParametersIn;
   dataFile = dataFileIn;
}

/**
 * destructor.
 */
BrainModelVolumeToSurfaceMapperPALS::~BrainModelVolumeToSurfaceMapperPALS()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeToSurfaceMapperPALS::execute() throw (BrainModelAlgorithmException)
{
   if (volumeFile == NULL) {
      throw BrainModelAlgorithmException("Volume is invalid.");
   }
   if (dataFile == NULL) {
      throw BrainModelAlgorithmException("Data file is invalid.");
   }
   if (stereotaxicSpaceName.isEmpty()) {
      throw BrainModelAlgorithmException("Stereotaxic space name is empty.");
   }
   
   QString structureName;
   QString structureAbbreviation;
   switch (structure.getType()) {
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
         structureName = "LEFT";
         structureAbbreviation = "L";
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         structureName = "RIGHT";
         structureAbbreviation = "R";
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
      case Structure::STRUCTURE_TYPE_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_SUBCORTICAL:
      case Structure::STRUCTURE_TYPE_ALL:
      case Structure::STRUCTURE_TYPE_INVALID:
         throw BrainModelAlgorithmException("Structure must be left or right.");
         break;
   }
   
   MetricFile* outputMetricFile = NULL;
   PaintFile*  outputPaintFile  = NULL;
   switch (mappingParameters.getAlgorithm()) {
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_NODES:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_ENCLOSING_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_GAUSSIAN:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_INTERPOLATED_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MAXIMUM_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MCW_BRAINFISH:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_STRONGEST_VOXEL:
         outputMetricFile = dynamic_cast<MetricFile*>(dataFile);
         if (outputMetricFile == NULL) {
            throw BrainModelAlgorithmException("A metric mapping algorithm has been "
                                               "selected but the data file is not a "
                                               "metric file.");
         }
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_PAINT_ENCLOSING_VOXEL:
         outputPaintFile = dynamic_cast<PaintFile*>(dataFile);
         if (outputPaintFile == NULL) {
            throw BrainModelAlgorithmException("A paint mapping algorithm has been "
                                               "selected but the data file is not a "
                                               "paint file.");
         }
         break;
   }
   
   //
   // Get the atlases
   //
   std::vector<MapFmriAtlasSpecFileInfo> atlases;
   MapFmriAtlasSpecFileInfo::getAtlases(brainSet, 
                                        atlases);

   //
   // Find the correct atlas
   //
   MapFmriAtlasSpecFileInfo* mappingAtlas = NULL;
   for (unsigned int i = 0; i < atlases.size(); i++) {
      if (atlases[i].getDataValid()) {
         if (atlases[i].getSpace() == stereotaxicSpaceName) {
            if (atlases[i].getStructure().toUpper() == structureName) {
               mappingAtlas = &atlases[i];
            }
         }
      }
   }
   if (mappingAtlas == NULL) {
      throw BrainModelAlgorithmException("Unable to find an atlas with stereotaxic "
                                         "space = \"" + stereotaxicSpaceName
                                         + "\" and structure = \""
                                         + structureName
                                         + "\"");
   }
   
   //
   // Get the data file names from the atlas
   //
   const QString mappingFilesPath = mappingAtlas->getSpecFilePath();
   const QString topologyFileName = mappingAtlas->getTopologyFile();
   const QString averageCoordFileName = mappingAtlas->getAverageCoordinateFile();
   const std::vector<QString> indivCoordFileNames = mappingAtlas->getCoordinateFiles();
   const int numIndivCoordFiles = static_cast<int>(indivCoordFileNames.size());

   //
   // Save the current directory
   //   
   const QString savedDirectory = QDir::currentPath();
   
   //
   // Change directory to the directory containing the mapping files
   //
   QDir::setCurrent(mappingFilesPath);
   
   try {
      //
      // Maping to a metric file
      //
      if (outputMetricFile != NULL) {
         //
         // Map to average fiducial
         //
         if (mappingParameters.getPalsMetricAverageFiducialMappingEnabled()) {
            mapAverageFiducial(topologyFileName,
                               averageCoordFileName,
                               structureName,
                               outputMetricFile);
         }
         
         //
         // Do metric multi-fiducial cases need to be mapped ?
         //
         if (mappingParameters.getPalsMetricMultiFiducialMappingEnabled() ||
            mappingParameters.getPalsMetricMinimumEnabled()  ||
            mappingParameters.getPalsMetricMaximumEnabled()  ||
            mappingParameters.getPalsMetricStandardDeviationEnabled()  ||
            mappingParameters.getPalsMetricStandardErrorEnabled()  ||
            mappingParameters.getPalsMetricAllCasesEnabled()) {
             
            //
            // Map to each indiv coordinate file
            //
            MetricFile multiFidMetricFile;             
            mapIndividualCases(topologyFileName,
                               indivCoordFileNames,
                               structureAbbreviation,
                               &multiFidMetricFile);
                               
            //
            // Set column names for statistics and MFM
            //
            QString averageColumnName;
            if (mappingParameters.getPalsMetricMultiFiducialMappingEnabled()) {
               averageColumnName = "MFM "
                  + structureName
                  + " - "
                  + FileUtilities::basename(volumeFile->getFileName());
            }
            QString standardDeviationColumnName;
            if (mappingParameters.getPalsMetricStandardDeviationEnabled()) {
               standardDeviationColumnName = "STANDARD DEVIATION (MFM) "
                  + structureName
                  + " - "
                  + FileUtilities::basename(volumeFile->getFileName());
            }
            QString standardErrorColumnName;
            if (mappingParameters.getPalsMetricStandardErrorEnabled()) {
               standardErrorColumnName = "STANDARD ERROR (MFM) "
                  + structureName
                  + " - "
                  + FileUtilities::basename(volumeFile->getFileName());
            }
            QString minimumAbsColumnName;
            if (mappingParameters.getPalsMetricMinimumEnabled()) {
               minimumAbsColumnName = "MINIMUM (MFM) "
                  + structureName
                  + " - "
                  + FileUtilities::basename(volumeFile->getFileName());
            }
            QString maximumAbsColumnName;
            if (mappingParameters.getPalsMetricMaximumEnabled()) {
               maximumAbsColumnName = "MAXIMUM (MFM) "
                  + structureName
                  + " - "
                  + FileUtilities::basename(volumeFile->getFileName());
            }
            
            //
            // Do any stats need to be computed
            //
            if ((averageColumnName.isEmpty() == false) ||
                (standardDeviationColumnName.isEmpty() == false) ||
                (standardErrorColumnName.isEmpty() == false) ||
                (minimumAbsColumnName.isEmpty() == false) ||
                (maximumAbsColumnName.isEmpty() == false)) {
               multiFidMetricFile.computeStatistics(averageColumnName,
                                                    standardDeviationColumnName,
                                                    standardErrorColumnName,
                                                    minimumAbsColumnName,
                                                    maximumAbsColumnName);
               //
               // Append stats
               //
               std::vector<int> columnDestination(multiFidMetricFile.getNumberOfColumns(),
                                                  MetricFile::APPEND_COLUMN_DO_NOT_LOAD);
               for (int i = numIndivCoordFiles; i < multiFidMetricFile.getNumberOfColumns(); i++) {
                  columnDestination[i] = MetricFile::APPEND_COLUMN_NEW;
               }
               outputMetricFile->append(multiFidMetricFile,
                                        columnDestination,
                                        MetricFile::FILE_COMMENT_MODE_LEAVE_AS_IS);
            }
            
            //
            // Append indiv cases to output file
            //
            if (mappingParameters.getPalsMetricAllCasesEnabled()) {
               std::vector<int> columnDestination(multiFidMetricFile.getNumberOfColumns(),
                                                  MetricFile::APPEND_COLUMN_DO_NOT_LOAD);
               for (int i = 0; i < numIndivCoordFiles; i++) {
                  columnDestination[i] = MetricFile::APPEND_COLUMN_NEW;
               }
               outputMetricFile->append(multiFidMetricFile,
                                        columnDestination,
                                        MetricFile::FILE_COMMENT_MODE_LEAVE_AS_IS);
            }
         }
      }
      
      if (outputPaintFile != NULL) {
         //
         // Map to average fiducial
         //
         if (mappingParameters.getPalsPaintAverageFiducialMappingEnabled()) {
            mapAverageFiducial(topologyFileName,
                               averageCoordFileName,
                               structureName,
                               outputPaintFile);
         }
         
         //
         // Do paint multi-fiducial cases need to be mapped ?
         //
         if (mappingParameters.getPalsPaintMostCommonEnabled() ||
             mappingParameters.getPalsPaintMostCommonExcludeUnidentifiedEnabled() ||
             mappingParameters.getPalsPaintAllCasesEnabled()) {

            //
            // Map to each indiv coordinate file
            //
            PaintFile multiFidPaintFile;             
            mapIndividualCases(topologyFileName,
                               indivCoordFileNames,
                               structureAbbreviation,
                               &multiFidPaintFile);
                               
            //
            // Set column names for statistics and MFM
            //
            QString mostCommonColumnName;
            if (mappingParameters.getPalsPaintMostCommonEnabled()) {
               mostCommonColumnName = "Most Common (MFM) "
                  + structureName
                  + " - "
                  + FileUtilities::basename(volumeFile->getFileName());
            }
            QString mostCommonNoIDColumnName;
            if (mappingParameters.getPalsPaintMostCommonExcludeUnidentifiedEnabled()) {
               mostCommonNoIDColumnName = "Most Common Exclude No ID (MFM) "
                  + structureName
                  + " - "
                  + FileUtilities::basename(volumeFile->getFileName());
            }
            
            //
            // Do any most common need to be computed
            //
            if ((mostCommonColumnName.isEmpty() == false) ||
                (mostCommonNoIDColumnName.isEmpty() == false)) {
               multiFidPaintFile.appendMostCommon(mostCommonColumnName,
                                                  mostCommonNoIDColumnName);
                                                  
               //
               // Append most common
               //
               std::vector<int> columnDestination(multiFidPaintFile.getNumberOfColumns(),
                                                  PaintFile::APPEND_COLUMN_DO_NOT_LOAD);
               for (int i = numIndivCoordFiles; i < multiFidPaintFile.getNumberOfColumns(); i++) {
                  columnDestination[i] = PaintFile::APPEND_COLUMN_NEW;
               }
               outputPaintFile->append(multiFidPaintFile,
                                        columnDestination,
                                        PaintFile::FILE_COMMENT_MODE_LEAVE_AS_IS);
            }
            
            //
            // Append indiv cases to output file
            //
            if (mappingParameters.getPalsPaintAllCasesEnabled()) {
               std::vector<int> columnDestination(multiFidPaintFile.getNumberOfColumns(),
                                                  PaintFile::APPEND_COLUMN_DO_NOT_LOAD);
               for (int i = 0; i < numIndivCoordFiles; i++) {
                  columnDestination[i] = PaintFile::APPEND_COLUMN_NEW;
               }
               outputPaintFile->append(multiFidPaintFile,
                                        columnDestination,
                                        PaintFile::FILE_COMMENT_MODE_LEAVE_AS_IS);
            }
         }
      }
   }
   catch (BrainModelAlgorithmException& e) {
      //
      // Restore directory
      //
      QDir::setCurrent(savedDirectory);
      
      throw (e);
   }
   
   //
   // Restore directory
   //
   QDir::setCurrent(savedDirectory);
}

/**
 * map to average fiducial surface.
 */
void 
BrainModelVolumeToSurfaceMapperPALS::mapAverageFiducial(const QString& topologyFileName,
                                 const QString& avgFidCoordFileName,
                                 const QString& structureName,
                                 GiftiNodeDataFile* nodeDataFile) throw (BrainModelAlgorithmException)
{
    //
    // Load the surface
    //
    BrainSet bs(topologyFileName,
                avgFidCoordFileName);
    BrainModelSurface* bms = bs.getBrainModelSurface(0);
    if (bms == NULL) {
       throw BrainModelAlgorithmException("Error loading mapping coord file "
                                          + avgFidCoordFileName);
    }
    
    //
    // Map the surface
    //
    QString columnName("AFM " 
                       + structureName
                       + " - "
                       + FileUtilities::basename(volumeFile->getFileName()));
    BrainModelVolumeToSurfaceMapper mapper(&bs,
                                           bms,
                                           volumeFile,
                                           nodeDataFile,
                                           mappingParameters,
                                           -1,  // new column
                                           columnName);
    mapper.execute(); 
}

/**
 * map all indiv cases.
 */
void 
BrainModelVolumeToSurfaceMapperPALS::mapIndividualCases(const QString& topologyFileName,
                                          const std::vector<QString>& indivCoordFileNames,
                                          const QString& structureAbbreviation,
                                          GiftiNodeDataFile* nodeDataFile) throw (BrainModelAlgorithmException)
{
   const int numIndivCoordFiles = static_cast<int>(indivCoordFileNames.size());

    //
    // Map to each indiv coordinate file
    //
    for (int i = 0; i < numIndivCoordFiles; i++) {
       //
       // Load the surface
       //
       BrainSet bs(topologyFileName,
                   indivCoordFileNames[i]);
       BrainModelSurface* bms = bs.getBrainModelSurface(0);
       if (bms == NULL) {
          throw BrainModelAlgorithmException("Error loading mapping coord file "
                                             + indivCoordFileNames[i]);
       }
       
       //
       // Map the surface
       //
       QString columnName("Map to Case" 
                          + QString::number(i + 1).rightJustified(2, '0')
                          + "."
                          + structureAbbreviation
                          + " - "
                          + FileUtilities::basename(volumeFile->getFileName()));
       BrainModelVolumeToSurfaceMapper mapper(&bs,
                                              bms,
                                              volumeFile,
                                              nodeDataFile,
                                              mappingParameters,
                                              -1,  // new column
                                              columnName);
       mapper.execute(); 
    }
}

/**
 * get the names of the supported stereotaxic spaces.
 */
void 
BrainModelVolumeToSurfaceMapperPALS::getSupportedStereotaxicSpaceName(std::vector<QString>& namesOut)
{
   namesOut.clear();
   
   //
   // read the atlases
   //
   BrainSet bsTemp;
   std::vector<MapFmriAtlasSpecFileInfo> palsAtlases;
   MapFmriAtlasSpecFileInfo::getAtlases(&bsTemp, 
                                        palsAtlases);

   std::set<QString> uniqueSpaceNamesSorted;
   for (unsigned int i = 0; i < palsAtlases.size(); i++) {
      uniqueSpaceNamesSorted.insert(palsAtlases[i].getSpace());
   }
   
   namesOut.insert(namesOut.end(),
                   uniqueSpaceNamesSorted.begin(), uniqueSpaceNamesSorted.end());
}
      
