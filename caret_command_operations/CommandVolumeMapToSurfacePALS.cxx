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
#include <set>

#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"
#include "BrainModelVolumeToSurfaceMapperPALS.h"
#include "BrainSet.h"
#include "CommandVolumeMapToSurfacePALS.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandVolumeMapToSurfacePALS::CommandVolumeMapToSurfacePALS()
   : CommandBase("-volume-map-to-surface-pals",
                 "VOLUME MAP TO SURFACE PALS ATLAS")
{

}

/**
 * destructor.
 */
CommandVolumeMapToSurfacePALS::~CommandVolumeMapToSurfacePALS()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMapToSurfacePALS::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   //
   // Get the names of the supported stereotaxic spaces
   //
   std::vector<QString> spaceNames;
   BrainModelVolumeToSurfaceMapperPALS::getSupportedStereotaxicSpaceName(spaceNames);

   //
   // Get the names of the mapping algorithms
   //
   std::vector<QString> algNames;
   std::vector<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM> algValues;   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmNamesAndValues(
                                                                        algNames,
                                                                        algValues);

   QStringList metricPaintFileFilters;
   metricPaintFileFilters << FileFilters::getMetricFileFilter();
   metricPaintFileFilters << FileFilters::getPaintFileFilter();
   metricPaintFileFilters << FileFilters::getSurfaceShapeFileFilter();

   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name", metricPaintFileFilters);
   paramsOut.addFile("Output Metric File Name", metricPaintFileFilters);
   paramsOut.addListOfItems("Stereotaxic Space", spaceNames, spaceNames); // yes use names for both
   paramsOut.addStructure("Structure");
   paramsOut.addListOfItems("Algorithm", algNames, algNames); // yes use names for both
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addVariableListOfParameters("Mapping Options");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMapToSurfacePALS::getHelpInformation() const
{
   //
   // Get the names of the supported stereotaxic spaces
   //
   std::vector<QString> spaceNames;
   BrainModelVolumeToSurfaceMapperPALS::getSupportedStereotaxicSpaceName(spaceNames);

   //
   // Get the names of the mapping algorithms
   //
   std::vector<QString> algNames;
   std::vector<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM> algValues;   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmNamesAndValues(
                                                                        algNames,
                                                                        algValues);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-or-paint-file-name>\n"
       + indent9 + "<output-metric-or-paint-file-name>\n"
       + indent9 + "<stereotaxic-space-name>\n"
       + indent9 + "<structure>\n"
       + indent9 + "<algorithm>\n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "[-metric-afm]\n"
       + indent9 + "[-metric-mfm]\n"
       + indent9 + "[-metric-min]\n"
       + indent9 + "[-metric-max]\n"
       + indent9 + "[-metric-std-dev]\n"
       + indent9 + "[-metric-std-error]\n"
       + indent9 + "[-metric-all-cases]\n"
       + indent9 + "[-paint-afm]\n"
       + indent9 + "[-paint-most-common]\n"
       + indent9 + "[-paint-most-common-no-unknown]\n"
       + indent9 + "[-paint-all-cases]\n"
       + indent9 + "[-av  average-voxel-neighbor-cube-size (mm)]\n"
       + indent9 + "[-bf  brain-fish-max-distance\n"
       + indent9 + "      brain-fish-splat factor]\n"
       + indent9 + "[-g   gaussian-neighbor-cube-size (mm)\n"
       + indent9 + "      sigma-norm\n"
       + indent9 + "      sigma-tang\n"
       + indent9 + "      norm below cutoff (mm)\n"
       + indent9 + "      norm above cutoff (mm)\n"
       + indent9 + "      tang-cutoff (mm)]\n"
       + indent9 + "[-mv  maximum-voxel-neighbor-cube-size (mm)]\n"
       + indent9 + "[-sv  strongest-voxel-neighbor-cube-size (mm)]\n"
       + indent9 + "\n"
       + indent9 + "Map a volume to the PALS atlas surfaces.\n"
       + indent9 + "\n"
       + indent9 + "\n"
       + indent9 + "\"algorithm\" is one of:\n");
   for (unsigned int i = 0; i < algNames.size(); i++) {
      helpInfo += (indent9 + "   " + algNames[i] + "\n");
   }
   helpInfo += ("" 
       + indent9 + "\n"
       + indent9 + "METRIC MAPPING OPTIONS\n"
       + indent9 + "   \"-metric-afm\"  Map to the average fiducial surface \n"
       + indent9 + "      (Average Fiducial Mapping).\n"
       + indent9 + "   \"-metric-mfm\"  Multi-Fiducal-Mapping - Output the \n"
       + indent9 + "      average of mapping to all PALS cases (Multi-Fiducial-\n"
       + indent9 + "      Mapping)\n"
       + indent9 + "   \"-metric-min\"  Output the minimum of mapping to all\n"
       + indent9 + "      PALS cases.\n"
       + indent9 + "   \"-metric-max\"  Output the maximum of mapping to all\n"
       + indent9 + "      PALS cases.\n"
       + indent9 + "   \"-metric-std-dev\"  Output the sample standard deviation\n"
       + indent9 + "      of mapping to all PALS cases.\n"
       + indent9 + "   \"-metric-std-error\"  Output the standard error of \n"
       + indent9 + "      mapping to all PALS cases.\n"
       + indent9 + "   \"-metric-all-cases\"  Output the mapping to each of the\n"
       + indent9 + "      PALS cases.\n"
       + indent9 + "\n"
       + indent9 + "\n"
       + indent9 + "PAINT MAPPING OPTIONS\n"
       + indent9 + "   \"-paint-afm\"  Map to the average fiducial surface \n"
       + indent9 + "      (Average Fiducial Mapping).\n"
       + indent9 + "   \"-paint-most-common\"  Output the most common paint\n"
       + indent9 + "      of mapping to all PALS cases. \n"
       + indent9 + "   \"-paint-most-common-no-unknown\"  Output the most common\n"
       + indent9 + "      paint of mapping to all PALS cases but exclude the \n"
       + indent9 + "      unidentified nodes.\n"
       + indent9 + "   \"-paint-all-cases\"  Output the mapping to all PALS \n"
       + indent9 + "      cases.\n"
       + indent9 + "\n"
       + indent9 + "Stereotaxic Spaces Supported \n");
   for (unsigned int i = 0; i < spaceNames.size(); i++) {
      helpInfo += (indent9 + "   " + spaceNames[i] + "\n");
   }
   helpInfo += (""
       + indent9 + "\n"
       + indent9 + "\"structure\" is one of:\n"
       + indent9 + "   LEFT\n"
       + indent9 + "   RIGHT\n"
       + indent9 + "\n"
       + indent9 + "If the input metric or paint file name is not an empty string\n"
       + indent9 + " (\"\"), the newly create metric or paint columns will be \n"
       + indent9 + "appended to the file and then written with the output file \n"
       + indent9 + "name.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeMapToSurfacePALS::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputDataFileName =
      parameters->getNextParameterAsString("Input Metric/Paint File Name");
   const QString outputDataFileName =
      parameters->getNextParameterAsString("Output Metric/Paint File Name");
   const QString stereotaxicSpaceName =
      parameters->getNextParameterAsString("Stereotaxic Space Name").toUpper();
   const QString structureName =
      parameters->getNextParameterAsString("Structure Name").toUpper();
   const QString algorithmName =
      parameters->getNextParameterAsString("Algorithm Name").toUpper();
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters mappingParameters;
   
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Map To PALS Parameter");
      if (paramName == "-metric-afm") {
         mappingParameters.setPalsMetricAverageFiducialMappingEnabled(true);
      }
      else if (paramName == "-metric-mfm") {
         mappingParameters.setPalsMetricMultiFiducialMappingEnabled(true);
      }
      else if (paramName == "-metric-min") {
         mappingParameters.setPalsMetricMinimumEnabled(true);
      }
      else if (paramName == "-metric-max") {
         mappingParameters.setPalsMetricMaximumEnabled(true);
      }
      else if (paramName == "-metric-std-dev") {
         mappingParameters.setPalsMetricStandardDeviationEnabled(true);
      }
      else if (paramName == "-metric-std-error") {
         mappingParameters.setPalsMetricStandardErrorEnabled(true);
      }
      else if (paramName == "-metric-all-cases") {
         mappingParameters.setPalsMetricAllCasesEnabled(true);
      }
      else if (paramName == "-paint-afm") {
         mappingParameters.setPalsPaintAverageFiducialMappingEnabled(true);
      }
      else if (paramName == "-paint-most-common") {
         mappingParameters.setPalsPaintMostCommonEnabled(true);
      }
      else if (paramName == "-paint-most-common-no-unknown") {
         mappingParameters.setPalsPaintMostCommonExcludeUnidentifiedEnabled(true);
      }
      else if (paramName == "-paint-all-cases") {
         mappingParameters.setPalsPaintAllCasesEnabled(true);
      }
      else if (paramName == "-av") {
         mappingParameters.setAlgorithmMetricAverageVoxelParameters(
            parameters->getNextParameterAsFloat("Average Voxel Neighbors (mm)"));
      }
      else if (paramName == "-bf") {
         mappingParameters.setAlgorithmMetricMcwBrainFishParameters(
            parameters->getNextParameterAsFloat("Brain Fish Max Distance"),
            parameters->getNextParameterAsInt("Brain Fish Splat Factor"));
      }
      else if (paramName == "-g") {
         mappingParameters.setAlgorithmMetricGaussianParameters(
            parameters->getNextParameterAsFloat("Guassian Neighbor Cube Size (mm)"),
            parameters->getNextParameterAsFloat("Gaussian Sigma Norm"),
            parameters->getNextParameterAsFloat("Gaussian Sigma Tangent"),
            parameters->getNextParameterAsFloat("Gaussina Norm Below Cutoff"),
            parameters->getNextParameterAsFloat("Gaussian Norm Above Cutoff"),
            parameters->getNextParameterAsFloat("Gaussian Tangent Cutoff"));
      }
      else if (paramName == "-mv") {
         mappingParameters.setAlgorithmMetricMaximumVoxelParameters(
            parameters->getNextParameterAsFloat("Maximum Voxel Neighbors (mm)"));
      }
      else if (paramName == "-sv") {
         mappingParameters.setAlgorithmMetricStrongestVoxelParameters(
            parameters->getNextParameterAsFloat("Strongest Voxel Neighbors (mm)"));
      }
      else {
         throw CommandException("Unrecognized Parameter: " + paramName);
      }
   }
   
   //
   // Set the algorithm
   //
   std::vector<QString> algNames;
   std::vector<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM> algValues;   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmNamesAndValues(
                                                                        algNames,
                                                                        algValues);
   bool algNameValid = false;
   BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM algorithm;
   for (unsigned int i = 0; i < algNames.size(); i++) {
      if (algorithmName == algNames[i]) {
         algorithm = algValues[i];
         algNameValid = true;
         break;
      }
   }
   if (algNameValid == false) {
      throw CommandException("Invalid algorithm name \""
                             + algorithmName
                             + "\".");
   }
   mappingParameters.setAlgorithm(algorithm);

   //
   // Get the structure
   //
   Structure::STRUCTURE_TYPE structure;
   if (structureName == "RIGHT") {
      structure = Structure::STRUCTURE_TYPE_CORTEX_RIGHT;
   }
   else if (structureName == "LEFT") {
      structure = Structure::STRUCTURE_TYPE_CORTEX_LEFT;
   }
   else {
      throw CommandException("Structure must be either RIGHT or LEFT.\n"
                             "       Value entered \"" + structureName + "\"");
   }
   
   //
   // Check Stereotaxic Space
   //
   bool spaceNameFound = false;
   std::vector<QString> spaceNames;
   BrainModelVolumeToSurfaceMapperPALS::getSupportedStereotaxicSpaceName(spaceNames);
   for (unsigned int i = 0; i < spaceNames.size(); i++) {
      if (stereotaxicSpaceName == spaceNames[i]) {
         spaceNameFound = true;
         break;
      }
   }
   if (spaceNameFound == false) {
      throw CommandException("Invalid stereotaxic space \""
                             + stereotaxicSpaceName
                             + "\"");
   }
   
   //
   // Read the volume file
   //
   VolumeFile volumeFile;
   volumeFile.readFile(inputVolumeFileName);
   
   //
   // Files that will get created
   //
   MetricFile mappingMetricFile, outputMetricFile;
   PaintFile mappingPaintFile, outputPaintFile;
   GiftiNodeDataFile* mappingDataFile = NULL;
   GiftiNodeDataFile* outputDataFile = NULL;
   if (outputDataFileName.endsWith(SpecFile::getMetricFileExtension()) ||
       outputDataFileName.endsWith(SpecFile::getSurfaceShapeFileExtension())) {
      mappingDataFile = &mappingMetricFile;
      outputDataFile = &outputMetricFile;
   }
   else if (outputDataFileName.endsWith(SpecFile::getPaintFileExtension())) {
      mappingDataFile = &mappingPaintFile;
      outputDataFile = &outputPaintFile;
   }
   else {
      throw CommandException("Unable to determine type of output file which "
                             "must end with \""
                             + SpecFile::getMetricFileExtension()
                             + "\" or \""
                             + SpecFile::getPaintFileExtension()
                             + ".");
   }
   
   //
   // Run the mapper
   //
   BrainSet bs;
   BrainModelVolumeToSurfaceMapperPALS palsMapper(&bs,
                                                  &volumeFile,
                                                  stereotaxicSpaceName,
                                                  structure,
                                                  mappingParameters,
                                                  mappingDataFile);
   palsMapper.execute();
   
   //
   // If input file present, read it
   //
   if (inputDataFileName.isEmpty() == false) {
      if (QFile::exists(inputDataFileName)) {
         outputDataFile->readFile(inputDataFileName);
      }
   }
   
   //
   // Add new data to output file
   //
   outputDataFile->append(*mappingDataFile);
   
   //
   // Write the output data file
   //
   outputDataFile->writeFile(outputDataFileName);
}

      

