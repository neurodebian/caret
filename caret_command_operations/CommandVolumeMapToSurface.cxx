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

#include <QFile>

#include "BrainSet.h"
#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"
#include "BrainModelVolumeToSurfaceMapper.h"
#include "CommandVolumeMapToSurface.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandVolumeMapToSurface::CommandVolumeMapToSurface()
   : CommandBase("-volume-map-to-surface",
                 "VOLUME MAP TO SURFACE")
{
   mappingDataFile = NULL;
}

/**
 * destructor.
 */
CommandVolumeMapToSurface::~CommandVolumeMapToSurface()
{
   if (mappingDataFile != NULL) {
      delete mappingDataFile;
      mappingDataFile = NULL;
   }
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMapToSurface::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
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
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric File Name", metricPaintFileFilters);
   paramsOut.addFile("Output Metric File Name", metricPaintFileFilters);
   paramsOut.addFile("Output Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addListOfItems("Algorithm", algNames, algNames); // yes use names for both
   paramsOut.addMultipleFiles("Input Volume File Names", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addVariableListOfParameters("Mapping Options");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMapToSurface::getHelpInformation() const
{
   std::vector<QString> algNames;
   std::vector<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM> algValues;   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmNamesAndValues(
                                                                        algNames,
                                                                        algValues);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-metric-or-paint-file-name>\n"
       + indent9 + "<output-metric-or-paint-file-name>\n"
       + indent9 + "<algorithm>\n"
       + indent9 + "<input-volume-file-names>\n"
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
       + indent9 + "Map volume(s) to a surface metric or paint file.\n"
       + indent9 + "\n"
       + indent9 + "For successful mapping, both the surface and the volume\n"
       + indent9 + "must be in the same stereotaxic space.\n"
       + indent9 + "\n"
       + indent9 + "\"algorithm\" is one of:\n");
   for (unsigned int i = 0; i < algNames.size(); i++) {
      helpInfo += (indent9 + "   " + algNames[i] + "\n");
   }
   helpInfo += ("" 
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
CommandVolumeMapToSurface::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   std::vector<QString> algNames;
   std::vector<BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM> algValues;   
   BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmNamesAndValues(
                                                                        algNames,
                                                                        algValues);
   BrainModelVolumeToSurfaceMapperAlgorithmParameters mappingParameters;

   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputDataFileName =
      parameters->getNextParameterAsString("Input Metric or Paint File Name");
   const QString outputDataFileName =
      parameters->getNextParameterAsString("Output Metric or PaintFile Name");
   const QString algorithmName =
      parameters->getNextParameterAsString("Algorithm Name");
      
   //
   // Verify algorithm name
   //
   bool algNameValid = false;
   for (unsigned int i = 0; i < algNames.size(); i++) {
      if (algorithmName == algNames[i]) {
         mappingParameters.setAlgorithm(algValues[i]);
         algNameValid = true;
         break;
      }
   }
   if (algNameValid == false) {
      throw CommandException("Invalid algorithm name \""
                             + algorithmName
                             + "\".");
   }
      
   std::vector<QString> inputVolumeFileNames;
   bool readingVolumeFileNamesFlag = true;
   while (parameters->getParametersAvailable()) {
      const QString paramValue = parameters->getNextParameterAsString("Map Volume Parameter");
      if (paramValue.isEmpty() == false) {
         if (paramValue[0] == '-') {
            readingVolumeFileNamesFlag = false;
            if (paramValue == "-av") {
               mappingParameters.setAlgorithmMetricAverageVoxelParameters(
                  parameters->getNextParameterAsFloat("Average Voxel Neighbors (mm)"));
            }
            else if (paramValue == "-bf") {
               const float md = parameters->getNextParameterAsFloat("Brain Fish Max Distance");
               const int sf = parameters->getNextParameterAsInt("Brain Fish Splat Factor");
               mappingParameters.setAlgorithmMetricMcwBrainFishParameters(md, sf);
            }
            else if (paramValue == "-g") {
               const float gn = parameters->getNextParameterAsFloat("Gaussian Neighbor Cube Size (mm)");
               const float sn = parameters->getNextParameterAsFloat("Gaussian Sigma Norm");
               const float st = parameters->getNextParameterAsFloat("Gaussian Sigma Tangent");
               const float nb = parameters->getNextParameterAsFloat("Gaussina Norm Below Cutoff");
               const float na = parameters->getNextParameterAsFloat("Gaussian Norm Above Cutoff");
               const float tc = parameters->getNextParameterAsFloat("Gaussian Tangent Cutoff");
               mappingParameters.setAlgorithmMetricGaussianParameters(gn, sn, st, nb, na, tc);
            }
            else if (paramValue == "-mv") {
               mappingParameters.setAlgorithmMetricMaximumVoxelParameters(
                  parameters->getNextParameterAsFloat("Maximum Voxel Neighbors (mm)"));
            }
            else if (paramValue == "-sv") {
               mappingParameters.setAlgorithmMetricStrongestVoxelParameters(
                  parameters->getNextParameterAsFloat("Strongest Voxel Neighbors (mm)"));
            }
            else {
               throw CommandException("Unrecognized option: " + paramValue);
            }
         }
         else if (readingVolumeFileNamesFlag) {
            inputVolumeFileNames.push_back(paramValue);
         }
         else {
            throw CommandException("Unrecognized parameter: " + paramValue);
         }
      }
   }

   const int numberOfVolumeFiles = static_cast<int>(inputVolumeFileNames.size());
   if (numberOfVolumeFiles <= 0) {
      throw CommandException("No volume file names were provided for mapping.");
   }
   
   enum MODE {
      MODE_NONE,
      MODE_METRIC,
      MODE_PAINT,
   };
   MODE mode = MODE_NONE;
   switch (mappingParameters.getAlgorithm()) {
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_NODES:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_ENCLOSING_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_GAUSSIAN:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_INTERPOLATED_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MAXIMUM_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MCW_BRAINFISH:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_STRONGEST_VOXEL:
         mode = MODE_METRIC;
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_PAINT_ENCLOSING_VOXEL:
         mode = MODE_PAINT;
         break;
   }
   if (mode == MODE_NONE) {
      throw CommandException("Unable determine type of output file from algorithm name.");
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     "",
                     true);
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Metric file for mapping
   //
   switch (mode) {
      case MODE_NONE:
         break;
      case MODE_METRIC:
         mappingDataFile = new MetricFile;
         break;
      case MODE_PAINT:
         mappingDataFile = new PaintFile;
         break;
   }
   
   //
   // Read input metric file if it exists
   //
   if (inputDataFileName.isEmpty() == false) {
      if (QFile::exists(inputDataFileName)) {
         mappingDataFile->readFile(inputDataFileName);
      }
   }
   
   //
   // Map all of the volume files
   //
   for (int j = 0; j < numberOfVolumeFiles; j++) {
      //
      // Read the volume file - all of its subvolumes
      //
      std::vector<VolumeFile*> volumes;
      VolumeFile::readFile(inputVolumeFileNames[j],
                           VolumeFile::VOLUME_READ_SELECTION_ALL,
                           volumes,
                           false);
      for (unsigned int i = 0; i < volumes.size(); i++) {
         VolumeFile* vf = volumes[i];
         
         //
         // Name for output column
         //
         QString columnName(FileUtilities::basename(inputVolumeFileNames[j]));
         if (volumes.size() > 0) {
            columnName += ("["
                           + QString::number(i + 1)
                           + "]");
         }
         //
         // Create the mapper
         //
         BrainModelVolumeToSurfaceMapper mapper(&brainSet,
                                                surface,
                                                vf,
                                                mappingDataFile,
                                                mappingParameters,
                                                -1,
                                                columnName);
               
         //
         // Run the mapper
         //
         mapper.execute();
         
         //
         // Delete the volume
         //
         delete vf;
      }
   }
   
   //
   // Save the metric file
   //
   mappingDataFile->writeFile(outputDataFileName);
}

      

