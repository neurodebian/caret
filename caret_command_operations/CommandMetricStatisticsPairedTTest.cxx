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

#include "BrainModelSurfaceMetricOneAndPairedTTest.h"
#include "BrainSet.h"
#include "CommandMetricStatisticsPairedTTest.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsPairedTTest::CommandMetricStatisticsPairedTTest()
   : CommandBase("-metric-statistics-paired-t-test",
                 "METRIC STATISTICS PAIRED T-TEST")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsPairedTTest::~CommandMetricStatisticsPairedTTest()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsPairedTTest::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Metric File Name A", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Metric File Name B", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Open Topology File Name", FileFilters::getTopologyOpenFileFilter());
   paramsOut.addFile("Distoration Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addInt("Distortion Column Number");
   paramsOut.addString("Output File Names Prefix");
   paramsOut.addFloat("Negative Threshold");
   paramsOut.addFloat("Positive Threshold");
   paramsOut.addFloat("P-Value");
   paramsOut.addInt("Variance Smoothing Iterations");
   paramsOut.addFloat("Variance Smoothing Strength");
   paramsOut.addInt("Iterations");
   paramsOut.addInt("Number of Threads", 1);
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsPairedTTest::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<metric-file-name-A>  \n"
       + indent9 + "<metric-file-name-B>  \n"
       + indent9 + "<fiducial-coordinate-file>  \n"
       + indent9 + "<open-topology-file>  \n"
       + indent9 + "<distortion-metric-file>  \n"
       + indent9 + "<distortion-column-number>  \n"
       + indent9 + "<output-file-names-prefix>  \n"
       + indent9 + "<f-negative-threshold>  \n"
       + indent9 + "<f-positive-threshold>  \n"
       + indent9 + "<f-p-value>  \n"
       + indent9 + "<i-variance-smoothing-iterations>  \n"
       + indent9 + "<f-variance-smoothing-strength>  \n"
       + indent9 + "<i-iterations>  \n"
       + indent9 + "<number-of-threads>  \n"
       + indent9 + " \n"
       + indent9 + "Perform a paired T-Test on the metric file. \n"
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
CommandMetricStatisticsPairedTTest::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString metricShapeFileA =
      parameters->getNextParameterAsString("Metric or Shape File A");
   const QString metricShapeFileB =
      parameters->getNextParameterAsString("Metric or Shape File B");
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
   const int iterations =
      parameters->getNextParameterAsInt("Iterations");
   const int numberOfThreads =
      parameters->getNextParameterAsInt("Number of Threads");
   checkForExcessiveParameters();
   
   if (distortionColumnNumber <= 0) {
      throw CommandException("Distortion column number must be >= 1.");
   }
   distortionColumnNumber--;   // algorithm starts at zero

   const QString ext = metricShapeFileA.endsWith(SpecFile::getMetricFileExtension())
                       ? SpecFile::getMetricFileExtension()
                       : SpecFile::getSurfaceShapeFileExtension();
   const QString outputTMapFileName(outputFileNamePrefix + "_TMap" + ext);
   const QString outputShuffledTMapFileName(outputFileNamePrefix + "_ShuffledTMap" + ext);
   const QString outputPaintFileName(outputFileNamePrefix + "_TMapClusters" + SpecFile::getPaintFileExtension());
   const QString outputMetricFileName(outputFileNamePrefix + "_TMapClusters" + SpecFile::getMetricFileExtension());
   const QString outputReportFileName(outputFileNamePrefix + "_TMap_Significant_Clusters" + SpecFile::getTextFileExtension());


   std::vector<QString> metricAndShapeFiles;
   metricAndShapeFiles.push_back(metricShapeFileA);
   metricAndShapeFiles.push_back(metricShapeFileB);
   
   BrainSet bs;
   BrainModelSurfaceMetricOneAndPairedTTest
      oneSample(&bs,
                BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE_PAIRED,
                metricAndShapeFiles,
                fiducialCoordFileName,
                openTopoFileName,
                distortionMetricShapeFileName,
                outputTMapFileName,
                outputShuffledTMapFileName,
                outputPaintFileName,
                outputMetricFileName,
                outputReportFileName,
                distortionColumnNumber,
                negativeThreshold,
                positiveThreshold,
                pValue,
                varianceSmoothingIterations,
                varianceSmoothingStrength,
                iterations,
                0.0,
                numberOfThreads);
                
   oneSample.execute();
}

      

