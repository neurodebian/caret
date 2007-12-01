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

#include "CommandMetricSetColumnToScalar.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricSetColumnToScalar::CommandMetricSetColumnToScalar()
   : CommandBase("-metric-set-column-to-scalar",
                 "METRIC SET COLUMN TO SCALAR")
{
}

/**
 * destructor.
 */
CommandMetricSetColumnToScalar::~CommandMetricSetColumnToScalar()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricSetColumnToScalar::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Metric File Name", 
                     FileFilters::getMetricFileFilter());
   paramsOut.addVariableListOfParameters("Column Names/Scalars");
}

/**
 * get full help information.
 */
QString 
CommandMetricSetColumnToScalar::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<metric-file-name>\n"
       + indent9 + "[column  scalar-value]\n"
       + indent9 + "\n"
       + indent9 + "Set columns to a scalar value in a metric file.\n"
       + indent9 + "\n"
       + indent9 + "\"column\" is either the number of the column, which\n"
       + indent9 + "starts at one, or the name of the column.  If a\n"
       + indent9 + "a name contains spaces, it must be enclosed in double\n"
       + indent9 + "quotes.  Name has priority over number.  If a name is\n"
       + indent9 + "used for a column and the column does not exist it will\n"
       + indent9 + "be created.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricSetColumnToScalar::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get output file
   //
   const QString metricFileName =
      parameters->getNextParameterAsString("Metric File Name");
   MetricFile metricFile;
   metricFile.readFile(metricFileName);
   
   //
   // Process column names
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get column identifier and new name
      //
      const QString columnID =
         parameters->getNextParameterAsString("Column Identifier");
      const float scalarValue =
         parameters->getNextParameterAsFloat("Scalar Value");
         
      //
      // Set the column
      //
      const int metricColumnNumber = metricFile.getColumnFromNameOrNumber(columnID, true);
      metricFile.setColumnAllNodesToScalar(metricColumnNumber, scalarValue);
   }      

   //
   // Write metric file
   //
   metricFile.writeFile(metricFileName);
}

      

