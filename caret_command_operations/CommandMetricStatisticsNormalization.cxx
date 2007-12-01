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

#include "CommandMetricStatisticsNormalization.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsNormalization::CommandMetricStatisticsNormalization()
   : CommandBase("-metric-statistics-normalization",
                 "METRIC STATISTICS NORMALIZATION")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsNormalization::~CommandMetricStatisticsNormalization()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsNormalization::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFloat("Mean", 0.0);
   paramsOut.addFloat("Standard Deviation", 1.0);
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsNormalization::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<mean>\n"
       + indent9 + "<standard-deviation>\n"
       + indent9 + "\n"
       + indent9 + "Remape each column in the metric file so that its fits\n"
       + indent9 + "a normalized (gaussian) distribution using the specified\n"
       + indent9 + "mean an standard deviation.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsNormalization::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   const float mean = 
      parameters->getNextParameterAsFloat("Mean");
   const float standardDeviation = 
      parameters->getNextParameterAsFloat("Mean");
   checkForExcessiveParameters();
   
   MetricFile metricFile;
   metricFile.readFile(inputMetricFileName);
   
   MetricFile* outputMetricFile = metricFile.computeNormalization(mean, standardDeviation);

   outputMetricFile->writeFile(outputMetricFileName);
   
   delete outputMetricFile;
}

      

