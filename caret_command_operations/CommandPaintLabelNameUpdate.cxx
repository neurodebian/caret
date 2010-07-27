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

#include "CommandPaintLabelNameUpdate.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPaintLabelNameUpdate::CommandPaintLabelNameUpdate()
   : CommandBase("-paint-label-name-update",
                 "PAINT LABEL NAME UPDATE")
{
}

/**
 * destructor.
 */
CommandPaintLabelNameUpdate::~CommandPaintLabelNameUpdate()
{
}

/**
 * get the script builder parameters.
 */
void
CommandPaintLabelNameUpdate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Paint File Name",
                     FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File Name",
                     FileFilters::getPaintFileFilter());
   paramsOut.addVariableListOfParameters("Label Name Updates");
}

/**
 * get full help information.
 */
QString
CommandPaintLabelNameUpdate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-paint-file-name>\n"
       + indent9 + "<output-paint-file-name>\n"
       + indent9 + "[-remove-prefixes]\n"
       + indent9 + "[-remove-suffixes]\n"
       + indent9 + "\n"
       + indent9 + "Update label names in a paint file.\n"
       + indent9 + "\n"
       + indent9 + "\"-remove-prefixes\" removes all characters up to\n"
       + indent9 + "and including the first period in the names of .\n"
       + indent9 + "all labels.\n"
       + indent9 + "\n"
       + indent9 + "\"-remove-suffixes\" removes all characters starting\n"
       + indent9 + "at and including the last period in the names of\n"
       + indent9 + "all labels.\n"
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandPaintLabelNameUpdate::executeCommand() throw (BrainModelAlgorithmException,
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
   bool removePrefixesFlag = false;
   bool removeSuffixesFlag = false;
   while (parameters->getParametersAvailable()) {
      //
      // Get column identifier and new name
      //
      QString paramName = parameters->getNextParameterAsString("Label Update Parameter");
      if (paramName == "-remove-prefixes") {
          removePrefixesFlag = true;
      }
      else if (paramName == "-remove-suffixes") {
          removeSuffixesFlag = true;
      }
      else {
          throw CommandException("Unrecognized parameter: " + paramName);
      }
   }


   //
   // Update labels names
   //
   paintFile.removePrefixesAndSuffixesFromNames(removePrefixesFlag,
                                                removeSuffixesFlag);
   //
   // Write paint file
   //
   paintFile.writeFile(outputPaintFileName);
}



