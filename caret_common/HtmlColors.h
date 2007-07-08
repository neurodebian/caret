
#ifndef __HTML_COLOR_H__
#define __HTML_COLOR_H__

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

#include <QString>
#include <vector>

/// class for html named colors and color values
class HtmlColors { 
   public:
      // get the names of all colors
      static void getAllColorNames(std::vector<QString>& allColorNames);
      
      // get the RGB values for a color name (returns true if color name valid)
      static bool getRgbFromColorName(const QString& colorName,
                                      unsigned char& redOut,
                                      unsigned char& greenOut,
                                      unsigned char& blueOut);
                                      
      // get the number of colors
      static int getNumberOfColors();
      
      // get color information given a color index
      static void getColorInformation(const int colorIndex,
                                      QString& colorNameOut,
                                      unsigned char& redOut,
                                      unsigned char& greenOut,
                                      unsigned char& blueOut);
                                      
   protected:
      // intialize the colors
      static void initializeColors();
      
      // colors initialized flag
      static bool colorsInitializedFlag;
      
      // class for storing names and colors
      class NameRGB {
         public:
            // constructor 
            NameRGB(const QString& nameIn,
                    const unsigned char redIn,
                    const unsigned char greenIn,
                    const unsigned char blueIn);
               
            // comparison operator
            bool operator<(const NameRGB& nrgb) const;
            
            // name
            QString name;
            
            // red
            unsigned char red;
            
            // green
            unsigned char green;
            
            // blue
            unsigned char blue;
      };
      
      /// the colors
      static std::vector<NameRGB> colors;
      
};

#ifdef __HTML_COLORS_MAIN__
   bool HtmlColors::colorsInitializedFlag = false;
   std::vector<HtmlColors::NameRGB> HtmlColors::colors;
#endif // __HTML_COLORS_MAIN__

#endif // __HTML_COLOR_H__
