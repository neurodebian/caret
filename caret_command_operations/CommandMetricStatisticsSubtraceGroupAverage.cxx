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

#include "CommandMetricStatisticsSubtraceGroupAverage.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsSubtraceGroupAverage::CommandMetricStatisticsSubtraceGroupAverage()
   : CommandBase("-metric-statistics-subtract-group-average",
                 "METRIC STATISTICS SUBTRACT GROUP AVERAGE")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsSubtraceGroupAverage::~CommandMetricStatisticsSubtraceGroupAverage()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsSubtraceGroupAverage::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name A", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Input Metric File Name B", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name A", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name B", FileFilters::getMetricShapeFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsSubtraceGroupAverage::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name-A>\n"
       + indent9 + "<input-metric-file-name-B>\n"
       + indent9 + "<output-metric-file-name-A>\n"
       + indent9 + "<output-metric-file-name-B>\n"
       + indent9 + "\n"
       + indent9 + "An average is computed each node in both input files.  This\n"
       + indent9 + "average is subtracted from each node's values and stored in\n"
       + indent9 + "the output files.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsSubtraceGroupAverage::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputMetricFileNameA =
      parameters->getNextParameterAsString("Input Metric File Name A");
   const QString inputMetricFileNameB =
      parameters->getNextParameterAsString("Input Metric File Name B");
   const QString outputMetricFileNameA =
      parameters->getNextParameterAsString("Output Metric File Name A");
   const QString outputMetricFileNameB =
      parameters->getNextParameterAsString("Output Metric File Name B");
   checkForExcessiveParameters();
   
   MetricFile inputMetricFileA, inputMetricFileB;
   inputMetricFileA.readFile(inputMetricFileNameA);
   inputMetricFileB.readFile(inputMetricFileNameB);
   
   MetricFile outputMetricFileA, outputMetricFileB;
   MetricFile::subtractMeanFromRowElements(&inputMetricFileA,
                                           &inputMetricFileB,
                                           &outputMetricFileA,
                                           &outputMetricFileB);

   outputMetricFileA.writeFile(outputMetricFileNameA);
   outputMetricFileB.writeFile(outputMetricFileNameB);
}

      

