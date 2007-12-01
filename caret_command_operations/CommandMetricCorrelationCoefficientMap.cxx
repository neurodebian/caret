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

#include "CommandMetricCorrelationCoefficientMap.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricCorrelationCoefficientMap::CommandMetricCorrelationCoefficientMap()
   : CommandBase("-metric-correlation-coefficient-map",
                 "METRIC CORRELATION COEFFICIENT MAP")
{
}

/**
 * destructor.
 */
CommandMetricCorrelationCoefficientMap::~CommandMetricCorrelationCoefficientMap()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricCorrelationCoefficientMap::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name A", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Input Metric File Name B", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricCorrelationCoefficientMap::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name-A>\n"
       + indent9 + "<input-metric-file-name-B>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "\n"
       + indent9 + "For each node, compute a correlation coefficient from the node's values\n"
       + indent9 + "in the two input metric files.  The two input files must have the same\n"
       + indent9 + "number of columns and column 'j' in the two files should contain data\n"
       + indent9 + "for the same subject.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricCorrelationCoefficientMap::executeCommand() throw (BrainModelAlgorithmException,
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
   checkForExcessiveParameters();
   
   MetricFile metricFileA, metricFileB;
   metricFileA.readFile(inputMetricFileNameA);
   metricFileB.readFile(inputMetricFileNameB);
   MetricFile* outputMetricFile = MetricFile::computeCorrelationCoefficientMap(&metricFileA,
                                                                               &metricFileB);
   outputMetricFile->writeFile(outputMetricFileName);
   delete outputMetricFile;
   outputMetricFile = NULL;
}

      

