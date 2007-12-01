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

#include <QWidget>

#include "CaretScriptFile.h"
#include "CommandScriptRun.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandScriptRun::CommandScriptRun()
   : CommandBase("-script-run",
                 "SCRIPT RUN")
{
}

/**
 * destructor.
 */
CommandScriptRun::~CommandScriptRun()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandScriptRun::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Caret Script File", FileFilters::getCaretScriptFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandScriptRun::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<caret-script-file-name>\n"
       + indent9 + "[-gui] \n"
       + indent9 + "\n"
       + indent9 + "Run a caret script file.\n"
       + indent9 + "\n"
       + indent9 + "If the \"-gui\" option is specified, a progress dialog\n"
       + indent9 + "is shown when the script executes and the user is \n"
       + indent9 + "is prompted for any inputs via a dialog.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandScriptRun::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString scriptFileName =
      parameters->getNextParameterAsString("Script File Name");
   bool useGUI = false;
   while (parameters->getParametersAvailable()) {
      const QString param = parameters->getNextParameterAsString("Script Run Parameter");
      if (param == "-gui") {
         useGUI = true;
      }
      else {
         throw CommandException("Unrecognized parameter: " + param);
      }
   }
   
   //
   // Read the script file
   //
   CaretScriptFile scriptFile;
   scriptFile.readFile(scriptFileName);
   
   //
   // Use GUI?
   //
   QWidget* w = NULL;
   if (useGUI) {
      w = new QWidget;
   }
   
   //
   // Execute the script file
   //
   QString textOutput;
   scriptFile.runCommandsInFile(w,
                                parameters->getProgramNameWithPath(),
                                textOutput);
                                
   //
   // Close widget
   //
   if (w != NULL) {
      w->close();
      delete w;
      w = NULL;
   }

   //
   // Print the output
   //
   std::cout << textOutput.toAscii().constData() << std::endl;
}

      

