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

#include <iostream>

#include "CommandHelp.h"
#include "CommandHelpFull.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandHelpFull::CommandHelpFull()
   : CommandBase("-help-full",
                 "HELP FULL INFORMATION")
{
}

/**
 * destructor.
 */
CommandHelpFull::~CommandHelpFull()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandHelpFull::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandHelpFull::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "\n"
       + indent9 + "   Show full help information.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandHelpFull::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Store the commands
   //
   std::vector<CommandBase*> commands;
   CommandBase::getAllCommandsSortedBySwitch(commands);
   
   std::cout << getGeneralHelpInformation().toAscii().constData() << std::endl;
   
   //
   // Loop through the commands
   //
   const int numCommands = static_cast<int>(commands.size());
   for (int i = 0; i < numCommands; i++) {
      //
      // Get the command
      //
      CommandBase* command = commands[i];
      
      //
      // Print the help information
      //
      std::cout << "------------------------------------------------------------------------------" << std::endl;
      CommandHelp::printCommandLongHelpInformation(command);
   }
   if (numCommands > 0) {
      std::cout << "------------------------------------------------------------------------------" << std::endl;
   }
}

      

