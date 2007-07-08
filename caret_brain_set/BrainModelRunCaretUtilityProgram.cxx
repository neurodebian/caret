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
#include <sstream>
#include <QGlobalStatic>
#include <QProcess>

#ifdef Q_OS_MACX
#include <stdlib.h>
#endif

#include "BrainModelRunCaretUtilityProgram.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "SystemUtilities.h"

/**
 * Constructor.
 */
BrainModelRunCaretUtilityProgram::BrainModelRunCaretUtilityProgram(
                                       const QString& utilityProgramNameIn,
                                       const QString& utilityProgramArgumentsIn,
                                       const bool macApplicationFlagIn,
                                       const bool waitUntilDoneIn) :
   /*QObject(),*/ BrainModelAlgorithm(NULL)
{
   utilityProgramName = utilityProgramNameIn;
   utilityProgramArguments = utilityProgramArgumentsIn;
   macApplicationFlag = macApplicationFlagIn;
   waitUntilDone      = waitUntilDoneIn;
}

/**
 * Destructor.
 */
BrainModelRunCaretUtilityProgram::~BrainModelRunCaretUtilityProgram()
{
}

/**
 * Execute the program.  If the program is not started successfully, a
 * BrainModelAlgorithmException will be thrown.
 */
void
BrainModelRunCaretUtilityProgram::execute() throw (BrainModelAlgorithmException)
{
   bool doingMacCommandLineProgram = false;
   
   //
   // Start path to executable
   //
   QString pgm(brainSet->getCaretHomeDirectory());

#ifdef Q_OS_MACX
   //pgm.append("/apps/map_fmri_to_surface.app/Contents/MacOS/");
   pgm.append("/bin/");
   pgm.append(utilityProgramName);
   if (macApplicationFlag == false) {
      doingMacCommandLineProgram = true;
   }
#endif // Q_OS_MACX

#ifdef Q_OS_WIN32
   pgm.append("\\bin\\");
   pgm.append(utilityProgramName);
   pgm.append(".exe");
#endif // Q_OS_WIN32

#ifdef Q_OS_UNIX
#ifndef Q_OS_MACX
   pgm.append("/bin/");
   pgm.append(utilityProgramName);
#endif // Q_OS_MACX
#endif // Q_OS_UNIX

   //
   // On a Macintosh, Qt's QProcess expects the program to be a Mac Application bundle so
   // simple command line programs have to be specially handled on the Mac.
   //
   if (doingMacCommandLineProgram) {
      if (utilityProgramArguments.isEmpty() == false) {
         pgm.append(" ");
         pgm.append(utilityProgramArguments);
      }
      if (waitUntilDone == false) {
         pgm.append(" &");
      }
      
      if (DebugControl::getDebugOn()) {
         std::cout << "About to run external program: " << pgm.toAscii().constData() << std::endl;
      }
      
      int result = system(pgm.toAscii().constData());
      result = ((result >> 8) & 0xff);
      if (result != 0) {
         std::ostringstream str;
         str << "Execution of program failed:"
             << "\n"
             << pgm.toAscii().constData()
             << "\n"
             << "with arguments: "
             << utilityProgramArguments.toAscii().constData()
             << "\n"
             << "Exit status: "
             << result;
         throw BrainModelAlgorithmException(str.str().c_str());
      }
      return;
   }   
   
   //
   // Create the process object
   //
   QProcess* process = new QProcess(this);

/* QT4
   //
   // Add the program name and arguments to the process
   //
   process->addArgument(pgm);
   if (utilityProgramArguments.isEmpty() == false) {
      process->addArgument(utilityProgramArguments);
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "About to run external program: " << pgm << std::endl;
   }
*/      
   //
   // Execute the process
   //
   process->start(pgm, QStringList(utilityProgramArguments));
   
   //
   // If user wants to wait until the process finishes
   //
   if (waitUntilDone) {
      //
      // See if the program is still running
      //
      process->waitForFinished(std::numeric_limits<int>::max());
      
      if (process->exitStatus() == QProcess::CrashExit) {
         std::ostringstream str;
         str << "Execution of program failed:"
             << "\n"
             << pgm.toAscii().constData()
             << "\n"
             << "with arguments: "
             << utilityProgramArguments.toAscii().constData()
             << "\n";
         throw BrainModelAlgorithmException(str.str().c_str());
      }
   }
}

