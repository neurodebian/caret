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


#ifndef __STRING_UTILITIES_H__
#define __STRING_UTILITIES_H__

#include <QString>
#include <vector>

class QString;

/// class contains static methods for handling C++ strings
class StringUtilities {

   public:

      // replace occurances of string "findThis" in "s" with "replaceWith"
      static QString replace(const QString& s, const QString& findThis, 
                        const QString& replaceWith);
   
      //  replace occurances of character "findThis" in "s" with "replaceWith"
      static QString replace(const QString& s, const char findThis, 
                        const char replaceWith);
   
      // split "s" into "tokens" splitting with any characters in "separators"
      static void token(const QString& s, 
                  const QString& separators,
                  std::vector<QString>& tokens);
   
   
      // split "s" into "float tokens" splitting with any characters in "separators"
      static void token(const QString& s, 
                  const QString& separators,
                  std::vector<float>& tokenFloats);
   
      // split "s" into "double tokens" splitting with any characters in "separators"
      static void token(const QString& s, 
                  const QString& separators,
                  std::vector<double>& tokenDoubles);
   
      // split "s" into "int tokens" splitting with any characters in "separators"
      static void token(const QString& s, 
                  const QString& separators,
                  std::vector<int>& tokenInts);
      
      // split "s" into "tokens" splitting with the literal string "separatorString"
      static void tokenSingleSeparator(const QString& stin,
                                      const QString& separatorString,
                                      std::vector<QString>& tokens);
                                      
      // combine "tokens" into a string delinated by "separators"
      static QString combine(const std::vector<QString>& tokens,
                        const QString& separator);
                        
      // combine "token floats" into a string delinated by "separators"
      static QString combine(const std::vector<float>& tokenFloats,
                        const QString& separator);
                        
      // combine "token doubles" into a string delinated by "separators"
      static QString combine(const std::vector<double>& tokenDoubles,
                        const QString& separator);
                        
      // combine "token ints" into a string delinated by "separators"
      static QString combine(const std::vector<int>& tokenInts,
                        const QString& separator);
                        
      // determine if the string "s1" ends with the string "s2"
      static bool endsWith(const QString& s1, const QString& s2);
   
      // determine if the string "s1" starts with the string "s2"
      static bool startsWith(const QString& s1, const QString& s2);
   
      // return a string made lowercase
      static QString makeLowerCase(const QString& s);
   
      // return a string made uppercase
      static QString makeUpperCase(const QString& s);
   
   
      // setup a comment for storage (replace tabs with spaces, replace newlines with tabs)
      static QString setupCommentForStorage(const QString& stin);

      // setup a comment for display (replace tabs with newlines)
      static QString setupCommentForDisplay(const QString& stin);
      
      // trim the blanks and tabs from both ends of a string
      static QString trimWhitespace(const QString& stin);

      // right justify a string by prepending blanks to it so the
      // string contains "totalSize" characters.
      static QString rightJustify(const QString& s, 
                                      const int totalSize);

      // left justify a string by adding blanks to it so the
      // string contains "totalSize" characters.
      static QString leftJustify(const QString& s, 
                                      const int totalSize);

      // convert any URLs found in the string into an HTML hyperlink
      static QString convertURLsToHyperlinks(const QString& sIn);
      
      // convert a string to an unsigned byte
      static unsigned char toUnsignedByte(const QString& s);
      
      // convert a string to an unsigned byte
      static void toNumber(const QString& s, unsigned char& b);
   
      // convert a unsigned byte to a string
      static QString fromNumber(const unsigned char b);
      
      // convert a string to an integer
      static int toInt(const QString& s);

      // convert a string to an int
      static void toNumber(const QString& s, int& i);
   
      // convert an integer to a string
      static QString fromNumber(const int i);
      
      // Convert an string to a float.  Handles "em" or "en" dash (various width dashes)
      static float toFloat(const QString& s);

      // convert a string to a float
      static void toNumber(const QString& s, float& f);
   
      // convert a float to a string
      static QString fromNumber(const float i);
      
      // Convert an string to a double.  Handles "em" or "en" dash (various width dashes)
      static double toDouble(const QString& s);

      // convert a string to a double
      static void toNumber(const QString& s, double& d);
   
      // convert a double to a string
      static QString fromNumber(const double d);
      
      // convert a string to an bool
      static bool toBool(const QString& s);

      // convert an bool to a string
      static QString fromBool(const bool b);
      
      // get the white space characters
      static QString getWhiteSpaceCharacters();
      
      // set the digits right of decimal for float/string conversions
      static void setFloatDigitsRightOfDecimal(const int numRightOfDecimal);
      
      // find the first of any of the characters in "lookForChars"
      static int findFirstOf(const QString& s, const QString& lookForChars,
                             const int startPos = 0);
                             
      // linewrap a string with no more than maxCharsPerLine 
      static void lineWrapString(const int maxCharsPerLine, QString& s);
      
   protected:
      /// digits right of decimal for float/string conversions
      static int digitsRightOfDecimal;
};

#ifdef __STRING_UTILITIES_MAIN_H__
   int StringUtilities::digitsRightOfDecimal = 6;
#endif // __STRING_UTILITIES_MAIN_H__

#endif  // __STRING_UTILITIES_H__
