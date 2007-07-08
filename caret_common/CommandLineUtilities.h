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

#ifndef __COMMAND_LINE_UTILITIES_H__
#define __COMMAND_LINE_UTILITIES_H__

#include <QString>

/// class for assistance with command line arguments
class CommandLineUtilities {

   public:
      ///Get the next parameter as a string.  Returns true if the parameter exists.
      static bool getNextParameter(const QString& optionName, 
                                   const int argc, 
                                   char* argv[], 
                                   const bool exitOnError,
                                   int& index,
                                   QString& stringOut);
                                   
      /// Get the next parameter as an int.  Returns true if the parameter exists.
      static bool getNextParameter(const QString& optionName, 
                                   const int argc, 
                                   char* argv[],
                                   const bool exitOnError,
                                   int& index,
                                   int& intOut);


      /// Get the next parameter as a float.  Returns true if the parameter exists.
      static bool getNextParameter(const QString& optionName, 
                                   const int argc, 
                                   char* argv[], 
                                   const bool exitOnError,
                                   int& index,
                                   float& floatOut);
                                   
      /// Get the next parameter as a bool.  Returns true if the parameter exists.
      static bool getNextParameter(const QString& optionName, 
                                   const int argc, 
                                   char* argv[], 
                                   const bool exitOnError,
                                   int& index,
                                   bool& boolOut);
                                   
};

#endif // __COMMAND_LINE_UTILITIES_H__

