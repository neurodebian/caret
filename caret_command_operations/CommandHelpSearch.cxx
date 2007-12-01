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

#include "CommandHelp.h"
#include "CommandHelpSearch.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandHelpSearch::CommandHelpSearch()
   : CommandBase("-help-search",
                 "HELP SEARCH")
{
}

/**
 * destructor.
 */
CommandHelpSearch::~CommandHelpSearch()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandHelpSearch::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addVariableListOfParameters("Search Keywords");
}

/**
 * get full help information.
 */
QString 
CommandHelpSearch::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<one-or-more-keywords>\n"
       + indent9 + "   Search Help Information.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandHelpSearch::executeCommand() throw (BrainModelAlgorithmException,
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
   
   //
   // Set the parameters for each command since needed for help
   //
   const int numCommands = static_cast<int>(commands.size());
   for (int i = 0; i < numCommands; i++) {
      commands[i]->setParameters(parameters);
   }
   
   std::vector<QString> keywords;
   while (parameters->getParametersAvailable()) {
      keywords.push_back(parameters->getNextParameterAsString("Keywords").toLower());
   }
   const int numKeywords = static_cast<int>(keywords.size());
   if (numKeywords <= 0) {
      throw CommandException("Keywords are missing.");
   }
   
   //
   // Loop through the commands
   //
   for (int i = 0; i < numCommands; i++) {
      //
      // Get the command
      //
      CommandBase* command = commands[i];
      const QString text = command->getHelpInformation().toLower();
      for (int j = 0; j < numKeywords; j++) {
         if (text.indexOf(keywords[j]) >= 0) {
            CommandHelp::printCommandShortHelpInformation(command);
            break;
         }
      }
   }
}

      

