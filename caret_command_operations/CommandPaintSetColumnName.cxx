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

#include "CommandPaintSetColumnName.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPaintSetColumnName::CommandPaintSetColumnName()
   : CommandBase("-paint-set-column-name",
                 "PAINT SET COLUMN NAME")
{
}

/**
 * destructor.
 */
CommandPaintSetColumnName::~CommandPaintSetColumnName()
{
}

/**
 * get the script builder parameters.
 */
void
CommandPaintSetColumnName::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Paint File Name",
                     FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File Name",
                     FileFilters::getPaintFileFilter());
   paramsOut.addVariableListOfParameters("Column Names");
}

/**
 * get full help information.
 */
QString
CommandPaintSetColumnName::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-paint-file-name>\n"
       + indent9 + "<output-paint-file-name>\n"
       + indent9 + "[column  new-name]\n"
       + indent9 + "\n"
       + indent9 + "Rename columns in a paint file.\n"
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
CommandPaintSetColumnName::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get output file
   //
   const QString inputPaintFileName =
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName =
      parameters->getNextParameterAsString("Output Paint File Name");
   PaintFile paintFile;
   paintFile.readFile(inputPaintFileName);

   //
   // Process column names
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get column identifier and new name
      //
      const QString columnID =
         parameters->getNextParameterAsString("Column Identifier");
      const QString newName =
         parameters->getNextParameterAsString("New Column Name");

      //
      // Set the column name
      //
      const int paintColumnNumber = paintFile.getColumnFromNameOrNumber(columnID, false);
      paintFile.setColumnName(paintColumnNumber, newName);
   }

   //
   // Write paint file
   //
   paintFile.writeFile(outputPaintFileName);
}



