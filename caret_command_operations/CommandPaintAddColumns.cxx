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

#include "CommandPaintAddColumns.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPaintAddColumns::CommandPaintAddColumns()
   : CommandBase("-paint-add-columns",
                 "PAINT ADD COLUMNS")
{
}

/**
 * destructor.
 */
CommandPaintAddColumns::~CommandPaintAddColumns()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandPaintAddColumns::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Paint File", FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File", FileFilters::getPaintFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandPaintAddColumns::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-paint-file-name>\n"
       + indent9 + "<output-paint-file-name>\n"
       + indent9 + "[-add  column-name]\n"
       + indent9 + "\n"
       + indent9 + "Add columns to a paint file.  If a paint column with the\n"
       + indent9 + "specified name already exists, a new column is NOT added.\n"
       + indent9 + "The paint file must exist.\n"
       + indent9 + "\n"
       + indent9 + "See the command \"-paint-file-create\" for how to create\n"
       + indent9 + "a paint file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandPaintAddColumns::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get inputs
   //
   const QString inputPaintFileName = 
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName = 
      parameters->getNextParameterAsString("Output Paint File Name");
   std::vector<QString> columnNames;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Paint Add Columns Option");
      if (paramName == "-add") {
         columnNames.push_back(parameters->getNextParameterAsString("Column Name"));
      }
      else {
         throw CommandException("Unknown option.");
      }
   }
   
   //
   // Check input
   //
   const int numColumnsToAdd = static_cast<int>(columnNames.size());
   if (numColumnsToAdd <= 0) {
      throw CommandException("No columns were specified for adding to the paint file.");
   }
   
   //
   // Read the paint file
   //
   PaintFile paintFile;
   paintFile.readFile(inputPaintFileName);

   //
   // Verify data in the paint file.
   //   
   if (paintFile.getNumberOfNodes() <= 0) {
      throw CommandException("Input paint file contains no nodes.");
   }
   
   //
   // Add each new column
   //
   for (int i = 0; i < numColumnsToAdd; i++) {
      //
      // Does paint column NOT exist
      //
      if (paintFile.getColumnWithName(columnNames[i]) < 0) {
         //
         // Add the column
         //
         paintFile.addColumns(1);
         const int col = paintFile.getNumberOfColumns() - 1;
         paintFile.setColumnName(col, columnNames[i]);
      }
   }
   
   //
   // Write the paint file
   //
   paintFile.writeFile(outputPaintFileName);
}

      

