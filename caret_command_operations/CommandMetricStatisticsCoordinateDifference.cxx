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

#include "BrainModelSurfaceMetricCoordinateDifference.h"
#include "BrainSet.h"
#include "CommandMetricStatisticsCoordinateDifference.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsCoordinateDifference::CommandMetricStatisticsCoordinateDifference()
   : CommandBase("-metric-statistics-coordinate-difference",
                 "METRIC STATISTICS COORDINATE DIFFERENCE")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsCoordinateDifference::~CommandMetricStatisticsCoordinateDifference()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsCoordinateDifference::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("COORD_DIFF");   descriptions.push_back("Use Coordinate Difference");
   values.push_back("TMAP_DIFF");   descriptions.push_back("Perform T-Test of Coordinate Difference");
   
   paramsOut.clear();
   paramsOut.addListOfItems("Mode", values, descriptions);
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Open Topology File Name", FileFilters::getTopologyOpenFileFilter());
   paramsOut.addFile("Distoration Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addInt("Distortion Column Number");
   paramsOut.addString("Output File Names Prefix");
   paramsOut.addInt("Iterations");
   paramsOut.addFloat("Threshold");
   paramsOut.addFloat("P-Value");
   paramsOut.addInt("Number of Threads", 1);
   paramsOut.addVariableListOfParameters("Coordinate Files");
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsCoordinateDifference::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <mode>   \n"
       + indent9 + "   <fiducial-coord-file>   \n"
       + indent9 + "   <open-topo-file>  \n"
       + indent9 + "   <distortion-metric-shape-file>   \n"
       + indent9 + "   <distortion-column-number>  \n"
       + indent9 + "   <output-file-names-prefix>  \n"
       + indent9 + "   <i-iterations>   \n"
       + indent9 + "   <f-threshold>   \n"
       + indent9 + "   <f-p-value>   \n"
       + indent9 + "   <i-number-of-threads>  \n"
       + indent9 + "   -groupA <one-or-more-coord-files-from-group-A>  \n"
       + indent9 + "   -groupB <one-or-more-coord-files-from-group-B>  \n"
       + indent9 + "    \n"
       + indent9 + "Find clusters in groups of coordinate files. \n"
       + indent9 + " \n"
       + indent9 + "mode is one of: \n"
       + indent9 + "   COORD_DIFF - search for clusters in coordinate difference. \n"
       + indent9 + "   TMAP_DIFF  - search for clusters using T-Test of coordinate difference. \n"
       + indent9 + " \n"
       + indent9 + "The distortion column number starts at 1. \n"
       + indent9 + " \n"
       + indent9 + "Number of threads is the number of concurrent processes run \n"
       + indent9 + "during the cluster search of the shuffled metric file. \n"
       + indent9 + "Use \"1\" if you are running on a single processor system. \n"
       + indent9 + "Users on systems with multiple processors or multi-core systems \n"
       + indent9 + "should set the number of threads to the number of processors \n"
       + indent9 + "and/or cores to reduce execution time. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsCoordinateDifference::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString modeName =
      parameters->getNextParameterAsString("Mode");
   const QString fiducialCoordFileName =
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
   const QString openTopoFileName =
      parameters->getNextParameterAsString("Open Topology File Name");
   const QString distortionMetricShapeFileName =
      parameters->getNextParameterAsString("Distortion Metric/Shape File Name");
   int distortionColumnNumber =
      parameters->getNextParameterAsInt("Distortion Column Number");
   QString outputFileNamePrefix =
      parameters->getNextParameterAsString("Output File Name Prefix");
   const int iterations =
      parameters->getNextParameterAsInt("Iterations");
   const float threshold =
      parameters->getNextParameterAsFloat("Threshold");
   const float pValue =
      parameters->getNextParameterAsFloat("P-Value");
   const int numberOfThreads =
      parameters->getNextParameterAsInt("Number of Threads");
   std::vector<QString> coordFileNamesGroupA, coordFileNamesGroupB;
   bool doingGroupA = false;
   bool doingGroupB = false;
   while (parameters->getParametersAvailable()) {
      const QString arg = parameters->getNextParameterAsString("Coordinate File Names");
      if (arg == "-groupA") {
         doingGroupA = true;
         doingGroupB = false;
      }
      else if (arg == "-groupB") {
         doingGroupB = true;
         doingGroupA = false;
      }
      else if (doingGroupA) {
         coordFileNamesGroupA.push_back(arg);
      }
      else if (doingGroupB) {
         coordFileNamesGroupB.push_back(arg);
      }
      else {
         throw CommandException("Invalid parameter in groups of files: " + arg);
      }
   }

      
   BrainModelSurfaceMetricCoordinateDifference::MODE mode;
   if (modeName == "COORD_DIFF") {
      mode = BrainModelSurfaceMetricCoordinateDifference::MODE_COORDINATE_DIFFERENCE;
   }
   else if (modeName == "TMAP_DIFF") {
      mode = BrainModelSurfaceMetricCoordinateDifference::MODE_TMAP_DIFFERENCE;
   }
   else {
      throw CommandException("Invalid mode: " + modeName);
   }
   

   if (distortionColumnNumber <= 0) {
      throw CommandException("Distortion column number must be >= 1.");
   }
   distortionColumnNumber--;   // algorithm starts at zero


   //const QString ext = metricShapeFileA.endsWith(SpecFile::getMetricFileExtension())
   //                    ? SpecFile::getMetricFileExtension()
   //                    : SpecFile::getSurfaceShapeFileExtension();
   const QString ext = SpecFile::getMetricFileExtension();
   const QString outputDistanceFileName(outputFileNamePrefix + "_DistanceOrTMap" + ext);
   const QString outputShuffledDistanceFileName(outputFileNamePrefix + "_Shuffled_DistanceOrTMap" + ext);
   const QString outputPaintFileName(outputFileNamePrefix + "_DistanceOrTMapClusters" + SpecFile::getPaintFileExtension());
   const QString outputMetricFileName(outputFileNamePrefix + "_DistanceOrTMapClusters" + SpecFile::getMetricFileExtension());
   const QString outputReportFileName(outputFileNamePrefix + "_DistanceOrTMap_Significant_Clusters" + SpecFile::getTextFileExtension());

                                          

   BrainSet bs;
   
   BrainModelSurfaceMetricCoordinateDifference 
      coordDiffAlg(&bs,
                   mode,
                   coordFileNamesGroupA,
                   coordFileNamesGroupB,
                   fiducialCoordFileName,
                   openTopoFileName,
                   distortionMetricShapeFileName,
                   outputDistanceFileName,
                   outputShuffledDistanceFileName,
                   outputPaintFileName,
                   outputMetricFileName,
                   outputReportFileName,
                   distortionColumnNumber,
                   iterations,
                   threshold,
                   pValue,
                   numberOfThreads);
   coordDiffAlg.execute();
}

      

