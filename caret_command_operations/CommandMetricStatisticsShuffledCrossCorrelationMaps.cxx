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

#include "CommandMetricStatisticsShuffledCrossCorrelationMaps.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsShuffledCrossCorrelationMaps::CommandMetricStatisticsShuffledCrossCorrelationMaps()
   : CommandBase("-metric-statistics-shuffled-cross-correlation-map",
                 "METRIC STATISTICS SHUFFLED CROSS CORRELATION MAP")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsShuffledCrossCorrelationMaps::~CommandMetricStatisticsShuffledCrossCorrelationMaps()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsShuffledCrossCorrelationMaps::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addInt("Iteration", 50);
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsShuffledCrossCorrelationMaps::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<iterations>\n"
       + indent9 + "\n"
       + indent9 + "Randomly select two data columns and multiply them together\n"
       + indent9 + "to produce a new column.  Repeat for the specified number\n"
       + indent9 + "of iterations.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsShuffledCrossCorrelationMaps::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   const int iterations = 
      parameters->getNextParameterAsInt("Iterations");
   checkForExcessiveParameters();
   
   MetricFile metricFile;
   metricFile.readFile(inputMetricFileName);
   
   MetricFile* outputMetricFile = metricFile.computeShuffledCrossCorrelationsMap(iterations);

   outputMetricFile->writeFile(outputMetricFileName);
   
   delete outputMetricFile;
}

      

