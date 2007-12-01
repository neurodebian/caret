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

#include "BrainModelSurfaceMetricInGroupDifference.h"
#include "BrainSet.h"
#include "CommandMetricInGroupDifference.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricInGroupDifference::CommandMetricInGroupDifference()
   : CommandBase("-metric-in-group-difference",
                 "METRIC IN GROUP DIFFERENCE")
{
}

/**
 * destructor.
 */
CommandMetricInGroupDifference::~CommandMetricInGroupDifference()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricInGroupDifference::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addBoolean("Abs Value Flag");
}

/**
 * get full help information.
 */
QString 
CommandMetricInGroupDifference::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<abs-value-flag>\n"
       + indent9 + "\n"
       + indent9 + "Compute the difference between each column in the metric file. \n"
       + indent9 + "and all other columns in the metric file.  The number of columns \n"
       + indent9 + "in the output metric file is [N! / (K! * (N - K)!)] where N is the. \n"
       + indent9 + "number of columns in the input metric file and K is 2. \n"
       + indent9 + "\n"
       + indent9 + "The abs-value-flag is either \"true\" or \"false\".  If the  \n"
       + indent9 + "abs-value-flag is true, the output is all absolute values of \n"
       + indent9 + "the differences. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricInGroupDifference::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric/Shape File Name");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric/Shape File Name");
   const bool absValueFlag =
      parameters->getNextParameterAsBoolean("Absolute Value Flag");
   checkForExcessiveParameters();
   
   BrainSet bs;
   BrainModelSurfaceMetricInGroupDifference diffAlg(&bs,
                                                    inputMetricFileName,
                                                    outputMetricFileName,
                                                    absValueFlag);
   diffAlg.execute();
}

      

