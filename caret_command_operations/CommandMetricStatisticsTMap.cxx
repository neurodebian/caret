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

#include "CommandMetricStatisticsTMap.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandMetricStatisticsTMap::CommandMetricStatisticsTMap()
   : CommandBase("-metric-statistics-t-map",
                 "METRIC STATISTICS T-MAP")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsTMap::~CommandMetricStatisticsTMap()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsTMap::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name A", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Input Metric File Name B", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addInt("Variance Smoothing Iterations");
   paramsOut.addFloat("Variance Smoothing Strength");
   paramsOut.addFloat("False Discovery Rate Q");
   paramsOut.addBoolean("Pool Variance Flag");
   paramsOut.addBoolean("Do False Discovery Rate Flag");
   paramsOut.addBoolean("Do Degrees of Freedom Flag");
   paramsOut.addBoolean("Do P-Values Flag");
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsTMap::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name-A>\n"
       + indent9 + "<input-metric-file-name-B>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<variance-smoothing-iterations>\n"
       + indent9 + "<variance-smoothing-strength>\n"
       + indent9 + "<falseDiscoveryRateQ>\n"
       + indent9 + "<pool-the-variance-flag>\n"
       + indent9 + "<do-false-discovery-rate-flag>\n"
       + indent9 + "<do-degrees-of-freedom-flag>\n"
       + indent9 + "<do-p-values-flag>\n"
       + indent9 + "\n"
       + indent9 + "Compute a T-Map using the two input files.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsTMap::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputMetricFileNameA =
      parameters->getNextParameterAsString("Input Metric File Name A");
   const QString inputMetricFileNameB =
      parameters->getNextParameterAsString("Input Metric File Name B");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const int varianceSmoothingIterations =
      parameters->getNextParameterAsInt("Variance Smoothing Iterations");
   const float varianceSmoothingStrength =
      parameters->getNextParameterAsFloat("Variance Smoothing Strength");
   const float falseDiscoveryRateQ =
      parameters->getNextParameterAsFloat("False Discovery Rate Q");
   const bool poolTheVarianceFlag =
      parameters->getNextParameterAsBoolean("Pool Variance Flag");
   const bool doFalseDiscoveryRateFlag =
      parameters->getNextParameterAsBoolean("Do False Discovery Rate");
   const bool doDegreesOfFreedomFlag =
      parameters->getNextParameterAsBoolean("Do Degrees of Freedom");
   const bool doPValuesFlag =
      parameters->getNextParameterAsBoolean("Do P-Values Flag");
   checkForExcessiveParameters();

   MetricFile inputMetricFileA, inputMetricFileB;
   inputMetricFileA.readFile(inputMetricFileNameA);
   inputMetricFileB.readFile(inputMetricFileNameB);
   
   TopologyFile topologyFile;
   topologyFile.readFile(topologyFileName);
   
   MetricFile* outputMetricFile = 
      MetricFile::computeStatisticalTMap(&inputMetricFileA,
                                         &inputMetricFileB,
                                         &topologyFile,
                                         varianceSmoothingIterations,
                                         varianceSmoothingStrength,
                                         poolTheVarianceFlag,
                                         falseDiscoveryRateQ,
                                         doFalseDiscoveryRateFlag,
                                         doDegreesOfFreedomFlag,
                                         doPValuesFlag);

   outputMetricFile->writeFile(outputMetricFileName);
   delete outputMetricFile;
}

      

