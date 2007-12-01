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

#include "BrainSet.h"
#include "BrainModelSurfaceMetricInterHemClusters.h"
#include "CommandMetricStatisticsInterhemisphericClusters.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsInterhemisphericClusters::CommandMetricStatisticsInterhemisphericClusters()
   : CommandBase("-metric-statistics-interhemispheric-clusters",
                 "METRIC STATISTICS INTERHEMISPHERIC CLUSTERS")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsInterhemisphericClusters::~CommandMetricStatisticsInterhemisphericClusters()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsInterhemisphericClusters::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Right Group A Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Right Group B Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Left Group A Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Left Group B Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Open Topology File Name", FileFilters::getTopologyOpenFileFilter());
   paramsOut.addFile("Distoration Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addInt("Distortion Column Number");
   paramsOut.addString("Output File Names Prefix");
   paramsOut.addInt("Iterations Shuffled T-Map");
   paramsOut.addInt("Iterations Right/Left Shuffled T-Map");
   paramsOut.addFloat("Positive Threshold");
   paramsOut.addFloat("Negative Threshold");
   paramsOut.addFloat("P-Value");
   paramsOut.addInt("Variance Smoothing Iterations");
   paramsOut.addFloat("Variance Smoothing Strength");
   paramsOut.addBoolean("Do T-Map Degrees of Freedom");
   paramsOut.addBoolean("Do T-Map P-Value");
   paramsOut.addInt("Number of Threads", 1);
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsInterhemisphericClusters::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<metric-file-name-right-A>   \n"
       + indent9 + "<metric-file-name-right-B>  \n"
       + indent9 + "<metric-file-name-left-A>   \n"
       + indent9 + "<metric-file-name-left-B>  \n"
       + indent9 + "<fiducial-coord-file>   \n"
       + indent9 + "<open-topo-file>  \n"
       + indent9 + "<distortion-metric-shape-file>   \n"
       + indent9 + "<distortion-column-number>  \n"
       + indent9 + "<output-file-names-prefix>  \n"
       + indent9 + "<i-iterations-shuffled-tmap>   \n"
       + indent9 + "<i-iterations-right-left-shuffled-tmap>  \n" 
       + indent9 + "<f-negative-threshold>   \n"
       + indent9 + "<f-positive-threshold>   \n"
       + indent9 + "<f-p-value>  \n"
       + indent9 + "<i-variance-smoothing-iterations>   \n"
       + indent9 + "<f-variance-smoothing-strength>  \n"
       + indent9 + "<b-do-tmap-DOF>   \n"
       + indent9 + "<b-do-tmap-pvalue>  \n"
       + indent9 + "<number-of-threads>  \n"
       + indent9 + " \n"
       + indent9 + "Search for interhemispheric clusters. \n"
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
CommandMetricStatisticsInterhemisphericClusters::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString rightMetricShapeFileA =
      parameters->getNextParameterAsString("Right Metric or Shape File A");
   const QString rightMetricShapeFileB =
      parameters->getNextParameterAsString("Right Metric or Shape File B");
   const QString leftMetricShapeFileA =
      parameters->getNextParameterAsString("Left Metric or Shape File A");
   const QString leftMetricShapeFileB =
      parameters->getNextParameterAsString("Left Metric or Shape File B");
   const QString fiducialCoordFileName =
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
   const QString openTopoFileName =
      parameters->getNextParameterAsString("Open Topology File Name");
   const QString distortionMetricShapeFileName =
      parameters->getNextParameterAsString("Distortion Metric/Shape File Name");
   int distortionColumnNumber =
      parameters->getNextParameterAsInt("Distortion Column Number");
   const QString outputFileNamePrefix =
      parameters->getNextParameterAsString("Output File Name Prefix");
   const int iterations =
      parameters->getNextParameterAsInt("Iterations");
   const int rightLeftIterations =
      parameters->getNextParameterAsInt("Right/Left Iterations");
   const float negativeThreshold =
      parameters->getNextParameterAsFloat("Negative Threshold");
   const float positiveThreshold =
      parameters->getNextParameterAsFloat("Positive Threshold");
   const float pValue =
      parameters->getNextParameterAsFloat("P-Value");
   const int varianceSmoothingIterations =
      parameters->getNextParameterAsInt("Variance Smoothing Iterations");
   const float varianceSmoothingStrength =
      parameters->getNextParameterAsFloat("Variance Smoothing Strength");
   const bool doTMapDOF =
      parameters->getNextParameterAsBoolean("Do T-Map DOF");
   const bool doTMapPValue =
      parameters->getNextParameterAsBoolean("Do T-Map P-Value");
   const int numberOfThreads =
      parameters->getNextParameterAsInt("Number of Threads");

   if (distortionColumnNumber <= 0) {
      throw CommandException("Distortion column number must be >= 1.");
   }
   distortionColumnNumber--;   // algorithm starts at zero


   const QString ext = rightMetricShapeFileA.endsWith(SpecFile::getMetricFileExtension())
                       ? SpecFile::getMetricFileExtension()
                       : SpecFile::getSurfaceShapeFileExtension();

   QString outputRightTMapFileName(outputFileNamePrefix + "_RIGHT_TMap" + ext);
   QString outputLeftTMapFileName(outputFileNamePrefix + "_LEFT_TMap" + ext);
   QString outputRightShuffledTMapFileName(outputFileNamePrefix + "_RIGHT_ShuffledTMap" + ext);
   QString outputLeftShuffledTMapFileName(outputFileNamePrefix + "_LEFT_ShuffledTMap" + ext);
   const QString outputTMapFileName(outputFileNamePrefix + "_TMap" + ext);
   const QString outputShuffledTMapFileName(outputFileNamePrefix + "_ShuffledTMap" + ext);
   const QString outputPaintFileName(outputFileNamePrefix + "_TMapClusters" + SpecFile::getPaintFileExtension());
   const QString outputMetricFileName(outputFileNamePrefix + "_TMapClusters" + SpecFile::getMetricFileExtension());
   const QString outputReportFileName(outputFileNamePrefix + "_TMap_Significant_Clusters" + SpecFile::getTextFileExtension());

                                          
   BrainSet bs;
   BrainModelSurfaceMetricInterHemClusters
      interHem(&bs,
               rightMetricShapeFileA,
               rightMetricShapeFileB,
               leftMetricShapeFileA,
               leftMetricShapeFileB,
               fiducialCoordFileName,
               openTopoFileName,
               distortionMetricShapeFileName,
               outputRightTMapFileName,
               outputLeftTMapFileName,
               outputRightShuffledTMapFileName,
               outputLeftShuffledTMapFileName,
               outputTMapFileName,
               outputShuffledTMapFileName,
               outputPaintFileName,
               outputMetricFileName,
               outputReportFileName,
               distortionColumnNumber,
               iterations,
               rightLeftIterations,
               negativeThreshold,
               positiveThreshold,
               pValue,
               varianceSmoothingIterations,
               varianceSmoothingStrength,
               doTMapDOF,
               doTMapPValue,
               numberOfThreads);
   interHem.execute();
}

      

