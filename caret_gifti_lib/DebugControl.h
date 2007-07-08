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
     
      // set the iterative update
      static void setIterativeUpdate(const int iter) { iterativeUpdate = iter; }

      // get the iterative update
      static int getIterativeUpdate() { return iterativeUpdate; }
 
      // get the test flag
      static bool getTestFlag() { return testFlag; }

      // set the test flag
      static void setTestFlag(const bool tf) { testFlag = tf; }

      // get the file read timing flag
      static bool getFileReadTimingFlag() { return fileReadTimingFlag; }
      
      // set the file read timing flag
      static void setFileReadTimingFlag(const bool f) { fileReadTimingFlag = f; }
      
   private:
      /// debugging on/off flag
      static bool debugOn;

      /// node number for debugging
      static int debugNodeNumber;
   
      /// iterative update
      static int iterativeUpdate;

      /// the test flag
      static bool testFlag;
      
      /// the file read timing flag
      static bool fileReadTimingFlag;
};

#ifdef __DEBUG_CONTROL_MAIN__
bool DebugControl::debugOn = false;
int  DebugControl::debugNodeNumber = -1;
int  DebugControl::iterativeUpdate = 10;
bool DebugControl::testFlag = false;
bool DebugControl::fileReadTimingFlag = false;
#endif

#endif // __DEBUG_CONTROL_H__

