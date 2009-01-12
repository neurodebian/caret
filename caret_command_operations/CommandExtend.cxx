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

#include "CommandExtend.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandExtend::CommandExtend()
   : CommandBase("-extend",
                 "EXTEND CARET COMMAND (HOW-TO)")
{
}

/**
 * destructor.
 */
CommandExtend::~CommandExtend()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandExtend::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandExtend::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "Describes how to extend caret_command.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandExtend::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   std::cout
      << "\n"
      << "HOW TO EXTEND CARET_COMMAND\n"
      << "\n"
      << "View some of the existing commands to see how the commands are \n"
      << "typically written.\n"
      << "\n"
      << "Each command is sub-class of CommandBase.  Each command has both\n"
      << "a \".h\" and a \".cxx\" file.\n"
      << "\n"
      << "* Build caret from the caret source code.  See\n"
      << "   http://brainvis.wustl.edu/CaretHelpAccount/caret5_help/source_code/source_code.html\n"
      << "\n"
      << "* In caret_source/caret_command_operations:\n"
      << "     copy COMMAND_STUB.cxx to \"new-command-name\".cxx\n"
      << "     copy COMMAND_STUB.h   to \"new-command-name\".h\n"
      << "  Example:\n"
      << "     cp COMMAND_STUB.cxx CommandExtend.cxx\n"
      << "     cp COMMAND_STUB.h   CommandExtend.h\n"
      << "\n"
      << "* In the new command's \".h\" file, replace all instances of\n"
      << "     __REPLACE_1__  with __COMMAND\"new-command-name\"_H__\n"
      << "        Example: Replace __REPLACE_1__ with __COMMAND_EXTEND_H__ \n"
      << "\n"
      << "* In the new command's \".h\" and \".cxx\" files, replace all instances\n"
      << "  of _REPLACE_2_  with the new command's class name.  The command's\n"
      << "  name should be the same as the name of the command's .h/.cxx file\n"
      << "  without the extension.\n"
      << "      Example: Replace _REPLACE_2_ with CommandExtend\n"
      << "\n"
      << "* In the new command's \".cxx\" file, find the constructor and add\n"
      << "  the parameters passed to the CommandBase constructor.  The first\n"
      << "  parameter is the \"switch\" used by caret_command and the second\n"
      << "  parameter is a short description of the command in capital letters.\n"
      << "     Example:  CommandBase(\"-\", \"\")   becomes\n"
      << "               CommandBase(\"-extend\", \"EXTEND CARET COMMAND (HOW-TO)\")\n"
      << "\n"
      << "* In the file CommandBase.cxx, add the name of the new command's \n"
      << "  \".h\" file to the \"#include\" section of the file.  In the method\n"
      << "  CommandBase::getAllCommandsUnsorted(CommandBase*>& commandsOut), \n"
      << "  add the new command to the \"commandsOut\" vector.  This step makes\n"
      << "  the new command visible to both caret_command and the Caret GUI's\n"
      << "  Command Executor interface.\n"
      << "\n"
      << "* Add the name of the new command's \".h\" and \".cxx\" file to the \n"
      << "  file caret_command_operations.pro.\n"
      << "\n"
      << "* Run \"qmake\" in the caret_command_operation directory to create\n"
      << "  an updated Makefile from the modified caret_command_operations.pro.\n"
      << "\n"
      << "* Run \"make\" to compile the new command's files.\n"
      << "\n"
      << "* Run \"make build\" to from the caret_source directory to create\n"
      << "  new versions of caret5 and caret_command.\n"
      << "\n"
      << "* Add code to the new command's files and compile.  All parameters\n"
      << "  that the user placed on the command line are availble from the\n" 
      << "  pointer \"parameters\" which is of type ProgramParameters\n"
      << "  The ProgramParameters class is located in the \n"
      << "  caret_source/caret_common directory.  If something goes wrong in the\n"
      << "  command, throw a CommandException.\n"
      << "\n"
      << "* Add information to the new command's getHelpInformation() method\n"
      << "  so that user's know how to use the command.  Please indicate who\n"
      << "  has written the command and include contact information (email .\n"
      << "  address).\n"
      << "\n"
      << "* Update getScriptBuilderParameters() so that the user-interface for\n"
      << "  the command is displayed in the Caret GUI's Command Executor.  The\n"
      << "  class ScriptBuilderParameters is located in the \n"
      << "  caret_command_operations directory.\n"
      << "\n"
      << "* When the command is ready, send the \".h\" and the \".cxx\" file to\n"
      << "  John Harwell, john@brainvis.wustl.edu so that it can be included in\n"
      << "  the next distribution of Caret.\n"
      << "\n"
      << std::endl;
}

      

