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

#include "CommandMetricCompositeIdentifiedColumns.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricCompositeIdentifiedColumns::CommandMetricCompositeIdentifiedColumns()
   : CommandBase("-metric-composite-identified-columns",
                 "METRIC COMPOSITE IDENTIFIED COLUMNS")
{
}

/**
 * destructor.
 */
CommandMetricCompositeIdentifiedColumns::~CommandMetricCompositeIdentifiedColumns()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricCompositeIdentifiedColumns::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Metric File Name", 
                     FileFilters::getMetricFileFilter());
   paramsOut.addVariableListOfParameters("Input Metric Files/Columns");
}

/**
 * get full help information.
 */
QString 
CommandMetricCompositeIdentifiedColumns::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-metric-file-name> \n"
       + indent9 + "[input-metric-file-1  file-1-column] \n"
       + indent9 + "...\n"
       + indent9 + "[input-metric-file-N  file-2-column]\n"
       + indent9 + "\n"
       + indent9 + "Composite the selected columns from the input metric\n"
       + indent9 + "files and place them into the output metric file.\n"
       + indent9 + "\n"
       + indent9 + "\"column\" is either the number of the column, which\n"
       + indent9 + "starts at one, or the name of the column.  If a\n"
       + indent9 + "a name contains spaces, it must be enclosed in double\n"
       + indent9 + "quotes.  Name has priority over number.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricCompositeIdentifiedColumns::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get output file
   //
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   MetricFile outputMetricFile;
   
   //
   // Process input files
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get input file name and column identifier
      //
      const QString inputMetricFileName =
         parameters->getNextParameterAsString("Input Metric File Name");
      const QString columnID =
         parameters->getNextParameterAsString("Column Identifier");
         
      //
      // Read the metric file and get the actual column number
      //
      MetricFile metricFile;
      metricFile.readFile(inputMetricFileName);
      const int numColumns = metricFile.getNumberOfColumns();
      if (numColumns <= 0) {
         throw CommandException(FileUtilities::basename(metricFile.getFileName())
                                + " contains no columns");
      }
      const int metricColumnNumber = metricFile.getColumnFromNameOrNumber(columnID, false);
                                                                      
      //
      // Append only the identified column
      //
      std::vector<int> destination(numColumns, MetricFile::APPEND_COLUMN_DO_NOT_LOAD);
      destination[metricColumnNumber] = MetricFile::APPEND_COLUMN_NEW;
      outputMetricFile.append(metricFile,
                              destination,
                              MetricFile::FILE_COMMENT_MODE_LEAVE_AS_IS);
   }
   
   //
   // Check for data
   //
   if (outputMetricFile.getNumberOfColumns() <= 0) {
      throw CommandException("No columns were appended to "
                             + FileUtilities::basename(outputMetricFileName));
   }
   
   //
   // Write the output metric file
   //
   outputMetricFile.writeFile(outputMetricFileName);
}

      

