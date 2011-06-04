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
#include <limits>

#include <QGlobalStatic>
#include <QProcess>


#include "BrainModelRunExternalProgram.h"
#include "BrainSet.h"
#include "DebugControl.h"

/**
 * Constructor.
 */
BrainModelRunExternalProgram::BrainModelRunExternalProgram(
                                   const QString& programNameIn,
                                   const QStringList& programArgumentsIn,
                                   const bool programIsInCaretSlashBinDirectoryFlagIn) :
   BrainModelAlgorithm(NULL)
{
   programName = programNameIn;
   programArguments = programArgumentsIn;
   programIsInCaretSlashBinDirectoryFlag = programIsInCaretSlashBinDirectoryFlagIn;
}

/**
 * Destructor.
 */
BrainModelRunExternalProgram::~BrainModelRunExternalProgram()
{
}

/**
 * Execute the program.  If the program is not started successfully, a
 * BrainModelAlgorithmException will be thrown.
 */
void
BrainModelRunExternalProgram::execute() throw (BrainModelAlgorithmException)
{
   outputText = "";
      
   if (DebugControl::getDebugOn()) {
      std::cout << "Caret Home Directory: "
                << BrainSet::getCaretHomeDirectory().toAscii().constData()
                << std::endl;
      std::cout << "Caret Bin Directory: "
                << BrainSet::getBinDirectoryName().toAscii().constData()
                << std::endl;
   }

   //
   // Start path to executable
   //
   QString pgm;
   if (programIsInCaretSlashBinDirectoryFlag) {
      pgm.append(BrainSet::getCaretHomeDirectory());
      pgm.append("/");
      pgm.append(BrainSet::getBinDirectoryName());
      pgm.append("/");

/*
#ifdef Q_OS_MACX
      pgm.append("/bin_macosx/");
#elif  Q_OS_WIN32
      pgm.append("/bin_windows/");
#elif  Q_OS_FREEBSD
      pgm.append("/bin_freebsd/");
#elif  Q_OS_UNIX
      pgm.append("/bin_linux/");
#else
      pgm.append("/bin_other/");
#endif
*/
   }
   pgm.append(programName);
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Program to run is: "
                << pgm.toAscii().constData()
                << std::endl;
   }
/*
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

   bool doingFreeBSDCommandLineProgram = false;
#ifdef Q_OS_FREEBSD
   doingFreeBSDCommandLineProgram = true;
#endif 

   //
   // On a Macintosh, Qt's QProcess expects the program to be a Mac Application bundle so
   // simple command line programs have to be specially handled on the Mac.
   //
   if (doingMacCommandLineProgram ||
       doingFreeBSDCommandLineProgram) {
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
*/
   
   //
   // Create the process object
   //
   QProcess* process = new QProcess(this);

   if (DebugControl::getDebugOn()) {
      std::cout << "Parameters for QProcess" << std::endl;
      std::cout << "   Program to run: " << pgm.toAscii().constData() << std::endl;
   }
   int numArgs = this->programArguments.size();
   for (int i = 0; i < numArgs; i++) {
      QString p = this->programArguments.at(i);
      if (DebugControl::getDebugOn()) {
         std::cout << "   Arg "
                   << i
                   << ": "
                   << p.toAscii().constData()
                   << std::endl;
      }

      if (p.startsWith('"') && p.endsWith('"')) {
         const int len = p.length();
         if (len > 2) {
            p = p.mid(1, len - 2);
            this->programArguments.replace(i, p);
            if (DebugControl::getDebugOn()) {
               std::cout << "      Removed quotes, now: "
                         << p.toAscii().constData()
                         << std::endl;
            }
         }
      }
   }

   //
   // Execute the process
   //
   bool errorFlag = false;
   process->start(pgm, programArguments);
   if (process->waitForStarted()) {
      //
      // See if the program is still running
      //
      process->waitForFinished(std::numeric_limits<int>::max());
      
      if (process->exitStatus() == QProcess::NormalExit) {
         if (process->exitCode() == 0) {
            outputText.append("COMMAND SUCCESSFUL\n\n");
         }
         else {
            errorFlag = true;
            outputText.append("COMMAND Reported Error: \n");
         }
      }
      else {
         errorFlag = true;
         QString str =
            ("Execution of program failed:\n"
             + pgm
             + "\n"
             + "with arguments: "
             + programArguments.join(" ")
             + "\n");
         outputText.append(str);
      }

      outputText += process->readAllStandardError();
      if (outputText.isEmpty() == false) {
         outputText += "\n";
      }
      outputText += process->readAllStandardOutput();
   }
   else {
      errorFlag = true;
      QString str =
         ("Starting of program failed:\n"
          + pgm
          + "\n"
          + "with arguments: "
          + programArguments.join(" ")
          + "\n");
      outputText.append(str);
   }
      
   delete process;

   if (errorFlag) {
      throw BrainModelAlgorithmException(outputText);
   }
}

