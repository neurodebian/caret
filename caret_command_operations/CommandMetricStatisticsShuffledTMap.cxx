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

#include "CommandMetricStatisticsShuffledTMap.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandMetricStatisticsShuffledTMap::CommandMetricStatisticsShuffledTMap()
   : CommandBase("-metric-statistics-shuffled-t-map",
                 "METRIC STATISTICS SHUFFLED T-MAP")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsShuffledTMap::~CommandMetricStatisticsShuffledTMap()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsShuffledTMap::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addInt("Iterations");
   paramsOut.addInt("Number in First Group");
   paramsOut.addInt("Variance Smoothing Iterations");
   paramsOut.addFloat("Variance Smoothing Strength");
   paramsOut.addBoolean("Pool Variance Flag");
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsShuffledTMap::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<iterations>\n"
       + indent9 + "<number-in-first-group>\n"
       + indent9 + "<variance-smoothing-iterations>\n"
       + indent9 + "<variance-smoothing-strength>\n"
       + indent9 + "<pool-the-variance-flag>\n"
       + indent9 + "\n"
       + indent9 + "Computed a T-Map on shuffled columns split into two groups.\n"
       + indent9 + "If the number in the first group is negative or zero, the\n"
       + indent9 + "columns are split into two groups of the same size.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsShuffledTMap::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const int iterations =
      parameters->getNextParameterAsInt("Iterations");
   const int numberInFirstGroup =
      parameters->getNextParameterAsInt("Number in First Group");
   const int varianceSmoothingIterations =
      parameters->getNextParameterAsInt("Variance Smoothing Iterations");
   const float varianceSmoothingStrength =
      parameters->getNextParameterAsFloat("Variance Smoothing Strength");
   const bool poolTheVarianceFlag =
      parameters->getNextParameterAsBoolean("Pool Variance Flag");
   checkForExcessiveParameters();
   
   MetricFile inputMetricFile;
   inputMetricFile.readFile(inputMetricFileName);
   
   TopologyFile topologyFile;
   topologyFile.readFile(topologyFileName);
   
   MetricFile* outputMetricFile = 
      inputMetricFile.computeStatisticalShuffledTMap(iterations,
                                         numberInFirstGroup,
                                         &topologyFile,
                                         varianceSmoothingIterations,
                                         varianceSmoothingStrength,
                                         poolTheVarianceFlag);

   outputMetricFile->writeFile(outputMetricFileName);
   delete outputMetricFile;
}

      

