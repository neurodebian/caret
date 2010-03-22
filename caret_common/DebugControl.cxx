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
 * abc
 */
/*LICENSE_END*/



#include <cstdlib>

#include "vtkObject.h"

#define __DEBUG_CONTROL_MAIN__ 
#include "DebugControl.h"
#undef __DEBUG_CONTROL_MAIN__
#include "StatisticAlgorithm.h"

/**
 * Get debugging on
 */
bool
DebugControl::getDebugOn()
{
   return debugOn;
}

/**
 * Set debugging on/off
 */
void
DebugControl::setDebugOn(const bool onOff)
{
   debugOn = onOff;
   StatisticAlgorithm::setDebugOn(debugOn);
   vtkObject::SetGlobalWarningDisplay(debugOn);
}

/**
 * set debug flags using environment variables.
 */
void 
DebugControl::setDebugFlagsFromEnvironmentVariables()
{
   if (std::getenv("CARET_DEBUG") != NULL) {
      setDebugOn(true);
   }
   if (std::getenv("CARET_TEST1") != NULL) {
      setTestFlag1(true);
   }
   if (std::getenv("CARET_TEST2") != NULL) {
      setTestFlag2(true);
   }
}
      
/**
 * If the environment variable name passed in is valid, debugging is
 * set to on, otherwise it is set off.
 */
void
DebugControl::setDebugOnWithEnvironmentVariable(const QString& envVarName)
{
   debugOn = (std::getenv(envVarName.toAscii().constData()) != NULL);
   StatisticAlgorithm::setDebugOn(debugOn);
   vtkObject::SetGlobalWarningDisplay(debugOn);
}

