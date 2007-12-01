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

#include "BrainModelSurfaceMetricTwinComparison.h"
#include "BrainSet.h"
#include "CommandMetricTwinComparison.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricTwinComparison::CommandMetricTwinComparison()
   : CommandBase("-metric-twin-comparison",
                 "METRIC TWIN COMPARISON")
{
}

/**
 * destructor.
 */
CommandMetricTwinComparison::~CommandMetricTwinComparison()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricTwinComparison::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
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
CommandMetricTwinComparison::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name-A>\n"
       + indent9 + "<input-metric-file-name-B>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Metric/Shape File A and Metric/Shape File B contain twin subjects\n"
       + indent9 + "such that one of the twins is in File A and its corresonding twin\n"
       + indent9 + "is in the same column but in File B.\n"
       + indent9 + "The output file contains the twin-paired expected variance,\n"
       + indent9 + "the expected variance of all non-twin pairs, and the the difference\n"
       + indent9 + "of the two expected variances.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricTwinComparison::executeCommand() throw (BrainModelAlgorithmException,
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

   BrainSet bs;
   BrainModelSurfaceMetricTwinComparison bmsmtc(&bs,
                                                inputMetricFileNameA,
                                                inputMetricFileNameB,
                                                outputMetricFileName);
   bmsmtc.execute();
}

      

