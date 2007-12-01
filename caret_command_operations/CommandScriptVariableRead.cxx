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

#include "CommandScriptVariableRead.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandScriptVariableRead::CommandScriptVariableRead()
   : CommandBase("-script-variable-read",
                 "SCRIPT VARIABLE READ")
{
}

/**
 * destructor.
 */
CommandScriptVariableRead::~CommandScriptVariableRead()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandScriptVariableRead::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addString("Variable Name");
   paramsOut.addString("Prompt Message");
}

/**
 * get full help information.
 */
QString 
CommandScriptVariableRead::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<name-of-variable-that-is-to-be-set>\n"
       + indent9 + "<prompt-message>\n"
       + indent9 + "\n"
       + indent9 + "Request the user to enter a value.\n"
       + indent9 + "\n"
       + indent9 + "If the \"prompt-message\" contains spaces, it must be\n"
       + indent9 + "enclosed in double quotes.\n"
       + indent9 + "\n"
       + indent9 + "When entering variable values, Bource and C-Shell scripts\n"
       + indent9 + "do not allow spaces in the value that is entered.  DOS and\n"
       + indent9 + "Python allow spaces only if the value is enclosed in \n"
       + indent9 + "double quotes.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandScriptVariableRead::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   variableName = parameters->getNextParameterAsString("Variable Name");
   promptMessage = parameters->getNextParameterAsString("Prompt Message");
   checkForExcessiveParameters();
}

      

