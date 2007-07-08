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


#ifndef __VE_SYSTEM_UTILITIES_H__
#define __VE_SYSTEM_UTILITIES_H__

#include <QString>  

/// This class contains system independent version of system dependent functions
class SystemUtilities {
   public:
      /// see if external program exists (can be executed - ie: in path)
      static bool externalProgramExists(const QString& programName);
      
      /// Get the number of processors on this computer.
      static int getNumberOfProcessors();
      
      /// display a web page in the web browser (specifying web browser is optional).
      /// Returns non-zero if error.
      static int displayInWebBrowser(const QString& webPage,
                                     const QString& webBrowserPath = "");
                                      
      /// sleep for the specified number of seconds
      static void sleepForSeconds(const int numberOfSeconds);
};

#endif // __VE_SYSTEM_UTILITIES_H__


