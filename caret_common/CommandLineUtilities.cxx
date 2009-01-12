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
#include <cstdlib>

#include "CommandLineUtilities.h"
#include "StringUtilities.h"

/**
 * Get the next parameter as a string.
 * Returns true if the parameter exists.
 */
bool
CommandLineUtilities::getNextParameter(const QString& optionName, 
                                       const int argc, 
                                       char* argv[], 
                                       const bool exitOnError,
                                       int& index,
                                       QString& stringOut)
{                
   stringOut = "";
    
   index++;
   if (index < argc) { 
      stringOut = argv[index];
      return true;
   }
    
   std::cout << "Missing parameter for \"" << optionName.toAscii().constData() << "\" option." << std::endl;
   if (exitOnError) {
      std::exit(-1);
   }
   return false;
}

/**
 * Get the next parameter as an int.
 * Returns true if the parameter exists.
 */
bool
CommandLineUtilities::getNextParameter(const QString& optionName, 
                                       const int argc, 
                                       char* argv[],
                                       const bool exitOnError,
                                       int& index,
                                       int& intOut)
{
   intOut = 0;
   QString s;
   if (getNextParameter(optionName, argc, argv, exitOnError, index, s)) {
      intOut = StringUtilities::toInt(s);
      return true;
   }
   return false;
}

/**
 * Get the next parameter as a float.
 * Returns true if the parameter exists.
 */
bool
CommandLineUtilities::getNextParameter(const QString& optionName, 
                                       const int argc, 
                                       char* argv[], 
                                       const bool exitOnError,
                                       int& index,
                                       float& floatOut)
{
   QString s;
   if (getNextParameter(optionName, argc, argv, exitOnError, index, s)) {
      floatOut = StringUtilities::toFloat(s);
      return true;
   }
   return false;
}

/**
 * Get the next parameter as a bool (value should be "true" or "false", or "1" or "0".
 * Returns true if the parameter exists.
 */
bool
CommandLineUtilities::getNextParameter(const QString& optionName, 
                                       const int argc, 
                                       char* argv[], 
                                       const bool exitOnError,
                                       int& index,
                                       bool& boolOut)
{
   boolOut = false;
   QString s;
   if (getNextParameter(optionName, argc, argv, exitOnError, index, s)) {
      if ((StringUtilities::makeLowerCase(s) == "true") || 
          (StringUtilities::makeLowerCase(s) == "t") ||
          (s == "1")) {
         boolOut = true;
      }
      else if ((StringUtilities::makeLowerCase(s) == "false") || 
               (StringUtilities::makeLowerCase(s) == "f") ||
               (s == "0")) {
         boolOut = false;
      }
      else {
         if (exitOnError) {
            std::cout << "Invalid bool value (not \"true\" or \"false\") \"" 
                      << s.toAscii().constData() << "\" for parameter \""
                      << optionName.toAscii().constData() << "\"." << std::endl;
            std::exit(-1);
         }
      }
   }
   return false;
}

