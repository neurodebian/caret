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
#include <typeinfo>

#include "CommandVerify.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandVerify::CommandVerify()
   : CommandBase("-verify",
                 "VERIFY (development use only)")
{
}

/**
 * destructor.
 */
CommandVerify::~CommandVerify()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVerify::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addBoolean("Anyting", true);
}

/**
 * get full help information.
 */
QString 
CommandVerify::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "\n"
       + indent9 + "Verify that all commands have switches, descriptions,\n"
       + indent9 + "help, and script builder parameters set.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVerify::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   std::vector<CommandBase*> allCommands;
   getAllCommandsSortedByDescription(allCommands);
   
   bool allOKFlag = true;
   
   const int numCommands = static_cast<int>(allCommands.size());
   for (int i = 0; i < numCommands; i++) {
      CommandBase* command = allCommands[i];
      //command->setParameters(parameters);
      const char* className = typeid(*command).name();
      
      QString errorMessage;
      if (command->getOperationSwitch().isEmpty() ||
          (command->getOperationSwitch() == "-")) {
         errorMessage += "   Missing Operation Switch\n";
      }
      if (command->getShortDescription().isEmpty()) {
         errorMessage += "   Missing Short Description\n";
      }
      if (command->getHelpInformation().isEmpty()) {
         errorMessage += "   Missing Help Information.";
      }
      ScriptBuilderParameters params;
      command->getScriptBuilderParameters(params);
      if (params.getNumberOfParameters() <= 0) {
         if (command->commandHasNoParameters() == false) {
            errorMessage += "   Missing script builder parameters";
         }
      }
      
      if (errorMessage.isEmpty() == false) {
         allOKFlag = false;
         std::cout << className 
                   << std::endl
                   << errorMessage.toAscii().constData()
                   << std::endl;
      }
   }
   
   if (allOKFlag) {
      std::cout << "All " << numCommands << " Commands Verified Successfully." << std::endl;
   }
}

      

