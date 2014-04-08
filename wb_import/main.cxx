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

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <QApplication>
#include <QDir>
#include <QImageIOPlugin>
#include <QTime>

#include "BrainSet.h"
#include "CaretVersion.h"
#include "CommandBase.h"
#include "CommandConvertDataFileToCaret7.h"
#include "CommandConvertSpecFileToCaret7.h"
#include "CommandHelpGlobalOptions.h"
#include "CommandHelp.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "ProgramParameters.h"

/*----------------------------------------------------------------------------------------
 * Globals
 */
static QApplication* myApplication = NULL;

/*----------------------------------------------------------------------------------------
 * Unexpected handler
 */
void unexpectedHandler()
{
  std::cout << "WARNING: wb_import will be terminating due to an unexpected exception." << std::endl
            << "abort() will be called and a core file may be created." << std::endl;
  abort();
}

/*----------------------------------------------------------------------------------------
 * New handler
 */
void newHandler()
{
   std::cout << "\n"
             << "OUT OF MEMORY\n"
             << "\n"
             << "This means that wb_import is unable to get memory that it needs.\n"
             << "Possible causes:\n"
             << "   (1) Your computer lacks sufficient RAM.\n"
             << "   (2) Swap space is too small (you might increase it)."
             << "   (3) Something is wrong with wb_import."
             << std::endl;

   abort();
}

/*----------------------------------------------------------------------------------------
 * The MAIN
 */
int
main(int argc, char* argv[])
{
   //QApplication::setApplicationName(FileUtilities::basename(argv[0]));
   
   //
   // Create a brain set
   // NEED to do this here so that static members are initialized
   //
   //BrainSet brain;
     
   //
   // Program's exit code.
   //
   int programExitCode = 0;
   
   try {
      //
      // The parameters passed to the program
      //
      ProgramParameters params(argc, argv);
      
      const QString programName(params.getProgramNameWithoutPath());
      
      //
      // Set handlers in case sh*t happens
      //
      std::set_unexpected(unexpectedHandler);
      std::set_new_handler(newHandler);

      //
      // Get image plugins so JPEGs can be loaded
      //
#ifndef UBUNTU
#if QT_VERSION < 0x040600
   Q_IMPORT_PLUGIN(qjpeg) //QJpegPlugin)
   Q_IMPORT_PLUGIN(qgif)  //QGifPlugin)
   Q_IMPORT_PLUGIN(qtiff) //QTiffPlugin)
#endif //QT_VERSION
#endif
      //
      // Get all of the available commands
      //
      std::vector<CommandBase*> commands;
      commands.push_back(new CommandConvertDataFileToCaret7);
      commands.push_back(new CommandConvertSpecFileToCaret7);
      const int numCommands = static_cast<int>(commands.size());
      
      const int numParams = params.getNumberOfParameters();
      if (numParams < 2) {
         for (int i = 0; i < numCommands; i++) {
            //if (commands[i]->getOperationSwitch().toLower().startsWith("-help")) {
               CommandHelp::printCommandShortHelpInformation(commands[i]);
            //}
         }
         
         std::cout << std::endl;
         std::cout << "To see help for a specific command, specify the command with no parameters." << std::endl
                   << "If the prefix of a command is supplied, a list of matching commands " << std::endl
                   << "will be displayed." << std::endl;
         std::cout << std::endl;
         
         std::exit(0);
      }
      
      //
      // Get the operation
      //      
      const QString operation(params.getNextParameterAsString("Operation"));
      
      //
      // Set the parameters for each command since needed for help
      // and find the command to run
      //
      bool commandFound = false;
      CommandBase* commandToRun = NULL;
      for (int i = 0; i < numCommands; i++) {
         commands[i]->setParameters(&params);
         if (operation == commands[i]->getOperationSwitch()) {
            commandFound = true;
            commandToRun = commands[i];
         }
      }
      
      //
      // Command line QT application
      //
      bool guiFlag = false;
      QApplication app(argc, argv, guiFlag);
      myApplication = &app;
      QApplication::setApplicationName(FileUtilities::basename(argv[0]));

      //
      // process the global options
      //
      CommandHelpGlobalOptions::processGlobalOptions(params);
            
      //
      // Create the brain set
      //
      BrainSet::initializeStaticStuff();
      BrainSet brain;
      
      //
      // Check for debugging
      //
      DebugControl::setDebugFlagsFromEnvironmentVariables();
      //DebugControl::setDebugOnWithEnvironmentVariable("CARET_DEBUG");
      const bool testFlag1IsOn = DebugControl::getTestFlag1();
      const bool testFlag2IsOn = DebugControl::getTestFlag2();
      const bool debugIsOn = DebugControl::getDebugOn();

      //
      // Brain set may turn debugging on via preferences file but do not let this happen
      //
      DebugControl::setDebugOn(debugIsOn);
      DebugControl::setTestFlag1(testFlag1IsOn);
      DebugControl::setTestFlag2(testFlag2IsOn);

      if (debugIsOn) {
         for (int i = 1; i < argc; i++) {
            std::cout << "arg " << i << ": " 
                      << argv[i] << std::endl;
         }
      }
      
      //
      // execute the command
      //
      if (commandFound) {
         //
         // Should help information be displayed
         //
         if ((params.getNumberOfParameters() == 2) &&
             (commandToRun->commandHasNoParameters() == false)) {
            CommandHelp::printCommandLongHelpInformation(commandToRun);
         }
         else {
            //
            //  Run the command
            //
            QString errorMessage;
            QTime timer;
            timer.start();
            if (commandToRun->execute(errorMessage)) {
               programExitCode = commandToRun->getExitCode();
            }
            else {
               if (errorMessage.isEmpty() == false) {
                  std::cout << std::endl;
                  std::cout << errorMessage.toAscii().constData() << std::endl;
                  std::cout << std::endl;
               }
               std::exit(-1);
            }
            //std::cout << "Execution Time (seconds): "
            //          << (timer.elapsed() * 0.001)
            //          << std::endl;
         }
      }
      
      //
      // If command was not found, see if it partially matches a command name
      // then print short help for all that match
      //
      if ((commandFound == false) &&
          (params.getNumberOfParameters() == 2)) {
         for (int i = 0; i < numCommands; i++) {
            const QString commandSwitch = commands[i]->getOperationSwitch();
            if (commandSwitch.startsWith(operation)) {
               commandFound = true;
               CommandHelp::printCommandShortHelpInformation(commands[i]);
            }
         }
      }
      
      //
      // Was command not found?
      //
      if (commandFound == false) {
         std::cout << "ERROR: operation \""
                   << operation.toAscii().constData()
                   << "\" not found."
                   << std::endl;
         std::exit(-1);
      }
   }
   catch (CommandException& ce) {
      std::cout << ce.whatQString().toAscii().constData() << std::endl;
   }
   catch (ProgramParametersException& ppe) {
      std::cout << ppe.whatQString().toAscii().constData() << std::endl;
   }
   
   return programExitCode;
}
