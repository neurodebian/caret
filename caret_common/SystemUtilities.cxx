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

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: SystemUtilities.cxx,v $
  Language:  C++
  Date:      $Date: 2007/11/06 16:38:50 $
  Version:   $Revision: 1.4 $


Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include <iostream>
#include <cstdlib>

#include <QApplication>
#include <QGlobalStatic>
#include <QProcess>
#include <QWidget>

#ifdef Q_OS_WIN32
#include <windows.h>
#else  // Q_OS_WIN32
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#endif // Q_OS_WIN32

#ifdef Q_OS_MACX
#include <CoreServices/CoreServices.h>
#endif

#include "SystemUtilities.h"

/**
 * get the user's name.
 */
QString 
SystemUtilities::getUserName()
{
#ifdef Q_OS_WIN32
   const QString name(getenv("USERNAME"));
#else // Q_OS_WIN32
   QString name(getlogin());
   if (name.isEmpty()) {
      name = getenv("USERNAME");
   }
#endif // Q_OS_WIN32
   return name;
}
      
/**
 * see if external program exists (can be executed - ie: in path).
 */
bool 
SystemUtilities::externalProgramExists(const QString& programName)
{
   QProcess p;
   p.start(programName);
   p.waitForStarted();
   p.waitForFinished();
   
   QProcess::ProcessError error = p.error(); 
   //QProcess::ExitStatus status = p.exitStatus();
   const bool programValid = (error == QProcess::UnknownError);
   
   return programValid;
}

/**
 * Get the number of processors on this computer.
 * This code lifted from "vtkMultiThreader.h".
 */
int
SystemUtilities::getNumberOfProcessors()
{
   int numProcessors = 1;
   
#ifdef Q_OS_WIN32
      SYSTEM_INFO sysInfo;
      GetSystemInfo(&sysInfo);
      numProcessors = sysInfo.dwNumberOfProcessors;
#endif  // Q_OS_WIN32

#ifdef Q_OS_MACX
   numProcessors = MPProcessors();
#endif

#ifdef Q_OS_UNIX
#ifndef Q_OS_MACX
#ifdef _SC_NPROCESSORS_ONLN
    numProcessors = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
    numProcessors = sysconf(_SC_NPROC_ONLN);
#endif  // _SC_NPROCESSORS_ONLN

#endif // Q_OS_MACX
#endif // Q_OS_UNIX

   return numProcessors;
}

/**
 * sleep for the specified number of seconds.
 */
void 
SystemUtilities::sleepForSeconds(const int numberOfSeconds)
{
#ifdef Q_OS_WIN32
   SleepEx(numberOfSeconds * 1000, FALSE);
#else  // Q_OS_WIN32
   sleep(numberOfSeconds);
#endif // Q_OS_WIN32
}

/// display a web page in the web browser (specifying web browser is optional).
/// Returns non-zero if error.
int 
SystemUtilities::displayInWebBrowser(const QString& webPage,
                                     const QString& webBrowserPath)
{
   int result = -1;
   
#ifdef Q_OS_WIN32
   result = reinterpret_cast<int>(ShellExecute(
                                     NULL,
                                     "open",
                                     webPage.toAscii().constData(),
                                     NULL,
                                     NULL,
                                     SW_SHOWNORMAL));
   //
   // less than 32 is an error from ShellExecute
   //
   if (result > 32) {
      result = 0;
   }
#endif

#ifdef Q_OS_MAC 
   //
   // Note: web page is placed in quotes so that "&" is not used as background
   //
   QString command(webBrowserPath);
   command = "";  // don't use webBrowserPath
   if (command.isEmpty()) {
      command = "open";
   }
   command.append(" \"");
   command.append(webPage);
   command.append("\" &");
   result =  system(command.toAscii().constData());
#endif

#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
   QString command(webBrowserPath);
   if (command.isEmpty()) {
      const char* browser = getenv("CARET_WEB_BROWSER");
      if (browser == NULL) {
         browser = getenv("BROWSER");
      }
      if (browser != NULL) {
         command = browser;
      }
      else {
         command = "netscape";
      }
   }
   //
   // Note: web page is placed in quotes so that "&" is not used as background
   //
   command.append(" \"");
   command.append(webPage);
   command.append("\" &");
   result =  system(command.toAscii().constData());
#endif
#endif

   return result;
}
                                 
