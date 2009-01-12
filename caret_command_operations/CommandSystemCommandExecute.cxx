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

#include <cstdlib>
#include <iostream>

#include <QProcess>

#include "CommandSystemCommandExecute.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSystemCommandExecute::CommandSystemCommandExecute()
   : CommandBase("-system-command-execute",
                 "SYSTEM COMMAND EXECUTE")
{
}

/**
 * destructor.
 */
CommandSystemCommandExecute::~CommandSystemCommandExecute()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSystemCommandExecute::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addString("System Command");
   paramsOut.addVariableListOfParameters("Parameters");
}

/**
 * get full help information.
 */
QString 
CommandSystemCommandExecute::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<system-command-name>\n"
       + indent9 + "<system-command-parameters>\n"
       + indent9 + "\n"
       + indent9 + "Execute a system command such as another program.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSystemCommandExecute::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   systemCommandName = parameters->getNextParameterAsString("System Command Name");
   systemCommandParameters.clear();
   while (parameters->getParametersAvailable()) {
      systemCommandParameters += parameters->getNextParameterAsString("System Command Parameters");
   }

//
// Use the system() command.  The reason is that when the script executor is run
// it is already in a QProcess and this would also run in another QProcess and 
// running a QProcess in another QProcess does not seem to work.
//
#define USE_SYSTEM_COMMAND
#ifdef USE_SYSTEM_COMMAND
   const QString cmdText(systemCommandName
                         + " "
                         + systemCommandParameters.join(" "));
   const int result = std::system(cmdText.toAscii().constData());
   if (result != 0) {
      throw CommandException("Error Code: " + QString::number(result) 
                             + " running: " + cmdText);
   }
#else  // USE_SYSTEM_COMMAND   
   //
   // Create a new QProcess and add its arguments
   //
   QProcess process(0);
   
   //
   // Start execution of the command
   //
   QString errorMessage;
   const QString cmdText(systemCommandName
                         + " "
                         + systemCommandParameters.join(" "));
   process.start(systemCommandName, systemCommandParameters);
   if (!process.waitForStarted()) {
      QString msg("Error starting command: ");
      msg.append(cmdText);
      errorMessage.append(msg);
   }
   
   //
   // Wait until the program is complete
   //
   if (!process.waitForFinished(100000000)) {
      QString msg("Error waiting for command to finish: ");
      msg.append(cmdText);
      errorMessage.append(msg);
   }
   
   const QString processOutput(process.readAll());
   std::cout << processOutput.toAscii().constData() << std::endl;
   
   if (process.exitStatus() == QProcess::NormalExit) {
      if (process.exitCode() != 0) {
         //std::cout << "Text output: "
         //          << commandsOutputText.toAscii().constData()
         //          << std::endl;
         errorMessage.append("COMMAND FAILED1: ");
         errorMessage.append(cmdText);
         errorMessage.append("\nExit Code " + QString::number(process.exitCode()));
         errorMessage.append("\nError Message" + process.errorString());
         errorMessage.append("\nCommand output: "
                             + processOutput);
      }
   }
   else {
      errorMessage.append("COMMAND FAILED2: ");
      errorMessage.append(cmdText);
   }
   
   if (errorMessage.isEmpty() == false) {
      throw CommandException(errorMessage);
   }
#endif // USE_SYSTEM_COMMAND
}

      

