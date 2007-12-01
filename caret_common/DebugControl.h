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



#ifndef __DEBUG_CONTROL_H__
#define __DEBUG_CONTROL_H__

#include <QString>

/// Class for controlling debug information.
class DebugControl {
   public:
      // see if debugging is on
      static bool getDebugOn();
      
      // turn debugging control on/off
      static void setDebugOn(const bool onOff);
      
      // get the debug node number
      static int getDebugNodeNumber() { return debugNodeNumber; }

      // set the debug node number
      static void setDebugNodeNumber(const int node) { debugNodeNumber = node; }

      // turn debugging on/off with an environment variable
      static void setDebugOnWithEnvironmentVariable(const QString& envVarName);
     
      // set debug flags using environment variables
      static void setDebugFlagsFromEnvironmentVariables();
      
      // set the iterative update
      static void setIterativeUpdate(const int iter) { iterativeUpdate = iter; }

      // get the iterative update
      static int getIterativeUpdate() { return iterativeUpdate; }
 
      // get the test flag 1
      static bool getTestFlag1() { return testFlag1; }

      // set the test flag 1
      static void setTestFlag1(const bool tf) { testFlag1 = tf; }

      // get the test flag 2
      static bool getTestFlag2() { return testFlag2; }

      // set the test flag 2
      static void setTestFlag2(const bool tf) { testFlag2 = tf; }

      // get the file read timing flag
      static bool getFileReadTimingFlag() { return fileReadTimingFlag; }
      
      // set the file read timing flag
      static void setFileReadTimingFlag(const bool f) { fileReadTimingFlag = f; }
      
      // see if OpenGL debugging is on
      static bool getOpenGLDebugOn() { return openGLDebugFlag; }

      // set OpenGL debugging on/off
      static void setOpenGLDebugOn(const bool b) { openGLDebugFlag = b; }
      
   private:
      /// debugging on/off flag
      static bool debugOn;

      /// node number for debugging
      static int debugNodeNumber;
   
      /// iterative update
      static int iterativeUpdate;

      /// the test flag 1
      static bool testFlag1;
      
      /// the test flag 2
      static bool testFlag2;
      
      /// OpenGL debug on/off flag
      static bool openGLDebugFlag;
      
      /// the file read timing flag
      static bool fileReadTimingFlag;
};

#ifdef __DEBUG_CONTROL_MAIN__
bool DebugControl::debugOn = false;
int  DebugControl::debugNodeNumber = -1;
int  DebugControl::iterativeUpdate = 10;
bool DebugControl::testFlag1 = false;
bool DebugControl::testFlag2 = false;
bool DebugControl::fileReadTimingFlag = false;
bool DebugControl::openGLDebugFlag = false;
#endif

#endif // __DEBUG_CONTROL_H__

