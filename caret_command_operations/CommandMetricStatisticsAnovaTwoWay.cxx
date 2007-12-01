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

#include "BrainModelSurfaceMetricAnovaTwoWay.h"
#include "BrainSet.h"
#include "CommandMetricStatisticsAnovaTwoWay.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsAnovaTwoWay::CommandMetricStatisticsAnovaTwoWay()
   : CommandBase("-metric-statistics-anova-two-way",
                 "METRIC STATISTICS TWO-WAY ANOVA")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsAnovaTwoWay::~CommandMetricStatisticsAnovaTwoWay()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsAnovaTwoWay::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("FIXED");   descriptions.push_back("Both Factors are Fixed");
   values.push_back("RANDOM");   descriptions.push_back("Both Factors are Random Effects");
   values.push_back("MIXED");   descriptions.push_back("Row Factor Fixed, Column Factor Random");
   paramsOut.clear();
   paramsOut.addListOfItems("ANOVA Type", values, descriptions);
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Open Topology File Name", FileFilters::getTopologyOpenFileFilter());
   paramsOut.addFile("Distoration Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addInt("Distortion Column Number");
   paramsOut.addString("Output File Names Prefix");
   paramsOut.addInt("Iterations");
   paramsOut.addFloat("Positive Threshold");
   paramsOut.addFloat("P-Value");
   paramsOut.addBoolean("Do F-Map Degrees of Freedom");
   paramsOut.addBoolean("Do F-Map P-Value");
   paramsOut.addInt("Number of Threads", 1);
   paramsOut.addInt("Number of ANOVA Rows", 1);
   paramsOut.addInt("Number of ANOVA Columns", 1);
   paramsOut.addMultipleFiles("Metric File Names", FileFilters::getMetricShapeFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsAnovaTwoWay::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <ANOVA-TYPE>  \n"
       + indent9 + "   <fiducial-coord-file>   \n"
       + indent9 + "   <open-topo-file>  \n"
       + indent9 + "   <distortion-metric-shape-file>   \n"
       + indent9 + "   <distortion-column-number>  \n"
       + indent9 + "   <output-file-names-prefix>  \n"
       + indent9 + "   <i-iterations>   \n"
       + indent9 + "   <f-positive-threshold>  \n"
       + indent9 + "   <f-p-value>  \n"
       + indent9 + "   <b-do-fmap-DOF>   \n"
       + indent9 + "   <b-do-fmap-pvalue>  \n"
       + indent9 + "   <number-of-threads>  \n"
       + indent9 + "   <number-of-ANOVA-cell-rows>  \n"
       + indent9 + "   <number-of-ANOVA-cell-columns>  \n"
       + indent9 + "   <metric-file-row-0-column-0>   \n"
       + indent9 + "   ... \n"
       + indent9 + "   <metric-file-row-0-column-N>   \n"
       + indent9 + "   ... \n"
       + indent9 + "   <metric-file-row-M-column-0>   \n"
       + indent9 + "   ... \n"
       + indent9 + "   <metric-file-row-M-column-N>   \n"
       + indent9 + "    \n"
       + indent9 + "Perform a two-way analysis of variance on the input metric files.  \n"
       + indent9 + "    \n"
       + indent9 + "The number of rows and columns identify the number of factors \n"
       + indent9 + "for each of the two measurements.   \n"
       + indent9 + "    \n"
       + indent9 + " ANOVA-TYPE is one of  \n"
       + indent9 + "    FIXED   - both factor are fixed effent \n"
       + indent9 + "    RANDOM  - both factors are random effects \n"
       + indent9 + "    MIXED   - the row factor is fixed, the column factor is random \n"
       + indent9 + "    \n"
       + indent9 + "The distortion column number starts at 1. \n"
       + indent9 + " \n"
       + indent9 + "Number of threads is the number of concurrent processes run \n"
       + indent9 + "during the cluster search of the shuffled metric file. \n"
       + indent9 + "Use \"1\" if you are running on a single processor system. \n"
       + indent9 + "Users on systems with multiple processors or multi-core systems \n"
       + indent9 + "should set the number of threads to the number of processors \n"
       + indent9 + "and/or cores to reduce execution time. \n"
       + indent9 + "\n"
       + indent9 + " \n"
       + indent9 + "DANGER: THIS HAS NOT BEEN TESTED!!!!!!!!!!!!!!!!!!! \n"
       + indent9 + "DANGER: THIS HAS NOT BEEN TESTED!!!!!!!!!!!!!!!!!!! \n"
       + indent9 + "DANGER: THIS HAS NOT BEEN TESTED!!!!!!!!!!!!!!!!!!! \n"
       + indent9 + "DANGER: THIS HAS NOT BEEN TESTED!!!!!!!!!!!!!!!!!!! \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsAnovaTwoWay::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString anovaTypeString =
      parameters->getNextParameterAsString("ANOVA Type");
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
   const float positiveThreshold =
      parameters->getNextParameterAsFloat("Positive Threshold");
   const float pValue =
      parameters->getNextParameterAsFloat("P-Value");
   const bool doFMapDOF =
      parameters->getNextParameterAsBoolean("Do F-Map Degrees of Freedom");
   const bool doFMapPValue =
      parameters->getNextParameterAsBoolean("Do F-Map P-Value");
   const int numberOfThreads =
      parameters->getNextParameterAsInt("Number of Threads");
   const int numberOfRowFactors =
      parameters->getNextParameterAsInt("Number of Row Factors");
   const int numberOfColumnFactors =
      parameters->getNextParameterAsInt("Number of Column Factors");
   std::vector<QString> metricFileNames;
   while (parameters->getParametersAvailable()) {
      metricFileNames.push_back(
         parameters->getNextParameterAsString("Metric File Names"));
   }

   BrainModelSurfaceMetricAnovaTwoWay::ANOVA_MODEL_TYPE anovaType;
   if (anovaTypeString == "FIXED") {
      anovaType = BrainModelSurfaceMetricAnovaTwoWay::ANOVA_MODEL_TYPE_FIXED_EFFECT;
   }
   else if (anovaTypeString == "RANDOM") {
      anovaType = BrainModelSurfaceMetricAnovaTwoWay::ANOVA_MODEL_TYPE_RANDOM_EFFECT;
   }
   else if (anovaTypeString == "MIXED") {
      anovaType = BrainModelSurfaceMetricAnovaTwoWay::ANOVA_MODEL_TYPE_ROWS_FIXED_EFFECT_COLUMN_RANDOM_EFFECT;
   }
   else {
      throw CommandException("Invalid ANOVA model type: "
                             + anovaTypeString);
   }
   
   if (distortionColumnNumber <= 0) {
      throw CommandException("Distortion column number must be >= 1.");
   }
   distortionColumnNumber--;   // algorithm starts at zero


   if (metricFileNames.empty()) {
      throw CommandException("No Metric/Shape files provided.");
   }
   const int totalNumFiles = numberOfRowFactors * numberOfColumnFactors;
   if (static_cast<int>(metricFileNames.size()) != totalNumFiles) {
      throw CommandException("Number of metric files should be rows * columns");
   }

   const QString ext = metricFileNames[0].endsWith(SpecFile::getMetricFileExtension())
                       ? SpecFile::getMetricFileExtension()
                       : SpecFile::getSurfaceShapeFileExtension();

   const QString outputFMapFileName(outputFileNamePrefix + "_FMap" + ext);
   const QString outputShuffledFMapFileName(outputFileNamePrefix + "_ShuffledFMap" + ext);
   const QString outputPaintFileName(outputFileNamePrefix + "_FMapClusters" + SpecFile::getPaintFileExtension());
   const QString outputMetricFileName(outputFileNamePrefix + "_FMapClusters" + SpecFile::getMetricFileExtension());
   const QString outputReportFileName(outputFileNamePrefix + "_FMap_Significant_Clusters" + SpecFile::getTextFileExtension());

   BrainSet bs;
   BrainModelSurfaceMetricAnovaTwoWay
          anova(&bs,
                anovaType,
                numberOfRowFactors,
                numberOfColumnFactors,
                fiducialCoordFileName,
                openTopoFileName,
                distortionMetricShapeFileName,
                outputFMapFileName,
                outputShuffledFMapFileName,
                outputPaintFileName,
                outputMetricFileName,
                outputReportFileName,
                distortionColumnNumber,
                iterations,
                positiveThreshold,
                pValue,
                doFMapDOF,
                doFMapPValue,
                numberOfThreads);
   for (int i = 0; i <  numberOfRowFactors; i++) {
      for (int j = 0; j < numberOfColumnFactors; j++) {
         const int indx = i * numberOfColumnFactors + j;
         anova.setMetricShapeFileName(i, j, metricFileNames[indx]);
      }
   }            
   anova.execute();
}

      

