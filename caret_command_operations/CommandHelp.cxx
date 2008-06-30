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

#include <QApplication>

#include "CommandHelp.h"
#include "CommandHelpGlobalOptions.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandHelp::CommandHelp()
   : CommandBase("-help",
                 "HELP")
{
}

/**
 * destructor.
 */
CommandHelp::~CommandHelp()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandHelp::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandHelp::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "\n"
       + indent9 + "   Show help information.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * print short help information.
 */
void 
CommandHelp::printCommandShortHelpInformation(const CommandBase* command)
{
   std::cout << "   "
             << command->getShortDescription().toAscii().constData()
             << "   "
             << command->getOperationSwitch().toAscii().constData()
             << std::endl;
}
      
/**
 * print long help information.
 */
void 
CommandHelp::printCommandLongHelpInformation(const CommandBase* command)
{
   std::cout << command->getHelpInformation().toAscii().constData();
   
   CommandHelpGlobalOptions hgo;
   
   const QString moreHelp = 
        (indent9 + "Run \""
         + QApplication::applicationName()
         + " "
         + hgo.getOperationSwitch() + "\"\n"
         + indent9 + "   for parameters available to all commands.\n"
         + "\n");
/*
   const QString moreHelp = 
        (indent9 + "Adding the parameter \"-CHDIR <directory-name>\" to \n"
       + indent9 + "the command will result in the command running from \n"
       + indent9 + "that directory.  This parameter does not affect the \n"
       + indent9 + "current directory in the shell that executes the \n"
       + indent9 + "command.\n"
       + indent9 + "\n"
       + indent9 + "Adding the parameter \"-CHMOD <permissions>\" to the \n"
       + indent9 + "command will result in all files written by the command\n"
       + indent9 + "receiving the specified permissions.  \"permissions\"\n"
       + indent9 + "is a list of one or move of the following values separated \n"
       + indent9 + "by commas:\n"
       + indent9 + "   UR   - user read\n"
       + indent9 + "   UW   - user write\n"
       + indent9 + "   UX   - user execute\n"
       + indent9 + "   GR   - group read\n"
       + indent9 + "   GW   - group write\n"
       + indent9 + "   GX   - group execute\n"
       + indent9 + "   AR   - all read\n"
       + indent9 + "   AW   - all write\n"
       + indent9 + "   AX   - all execute\n"
       + indent9 + "\n");
*/
   std::cout << moreHelp.toAscii().constData();
}
      
/**
 * execute the command.
 */
void 
CommandHelp::executeCommand() throw (BrainModelAlgorithmException,
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
   // Loop through the commands
   //
   const int numCommands = static_cast<int>(commands.size());
   for (int i = 0; i < numCommands; i++) {
      //
      // Print the command's help information
      //
      printCommandShortHelpInformation(commands[i]);
   }
}

      

