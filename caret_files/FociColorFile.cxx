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

#include <algorithm>
#include <set>

#include "FociColorFile.h"
#include "FociProjectionFile.h"
#include "NameIndexSort.h"
#include "SpecFile.h"

/**
 * Constructor
 */
FociColorFile::FociColorFile() 
   : ColorFile("Foci Color File", SpecFile::getFociColorFileExtension())
{
}

/**
 * Destructor
 */
FociColorFile::~FociColorFile()
{
}

/**
 * remove colors not matching the names in the cell projection file.
 */
void 
FociColorFile::removeNonMatchingColors(const FociProjectionFile* fpf)
{
   const int numColors = getNumberOfColors();
   if (numColors <= 0) {
      return;
   }
   const int numFoci = fpf->getNumberOfCellProjections();
   
   //
   // Colors that are used
   //
   std::vector<ColorStorage> colorsUsed;

   //
   // Loop through colors
   //
   for (int i = 0; i < numColors; i++) {
      //
      // Get the name ofr the color
      //
      const ColorStorage cs = *(getColor(i));
      const QString colorName = cs.getName();
      
      bool colorUsedFlag = false;

      //
      // Never delete the unknown color
      //
      if (colorName == "???") {
         colorUsedFlag = true;
      }
      else {      
         //
         // loop through foci projections to see if color is used
         //
         for (int j = 0; j < numFoci; j++) {
            //
            // Get the name of the focus
            //
            const CellProjection* cf = fpf->getCellProjection(j);
            const QString focusName = cf->getName();
            
            //
            // Does the focus start with the name of the colors
            //
            if (focusName.startsWith(colorName)) {
               //
               // Keep this color
               //
               colorUsedFlag = true;
               break;
            }
         }
      }
      
      //
      // Was this color used
      //
      if (colorUsedFlag) {
         //
         // Keep this color
         //
         colorsUsed.push_back(cs);
      }
   }
   
   //
   // Were colors removed
   //
   if (static_cast<int>(colorsUsed.size()) != numColors) {
      //
      // Simply replace colors
      //
      colors = colorsUsed;
      setModified();
   }
}      

/**
 * get color indices sorted by name case insensitive.
 */
void 
FociColorFile::getColorIndicesSortedByName(const FociProjectionFile* fpf,
                                 std::vector<int>& indicesSortedByNameOut,
                                 const bool reverseOrderFlag,
                                 const bool limitToColorsUsedByDisplayedFociFlag) const
{
   indicesSortedByNameOut.clear();
  
   //
   // Sort indices by name
   //
   NameIndexSort nis;
   
   if (limitToColorsUsedByDisplayedFociFlag) {
      const int numFoci = fpf->getNumberOfCellProjections();
      const int numColors = getNumberOfColors();
      std::vector<bool> colorIndexUsed(numColors, false);
      
      for (int i = 0; i < numFoci; i++) {
         const CellProjection* focus = fpf->getCellProjection(i);
         if (focus->getDisplayFlag()) {
            const int colorIndex = focus->getColorIndex();
            if (colorIndex >= 0) {
               colorIndexUsed[colorIndex] = true;
            }
         }
      }
      
      for (int i = 0; i < numColors; i++) {
         if (colorIndexUsed[i]) {
            nis.add(i, getColorNameByIndex(i));
         }
      }
   }
   else {
      const int num = getNumberOfColors();
      for (int i = 0; i < num; i++) {
         nis.add(i, getColorNameByIndex(i));
      }
   }

   nis.sortByNameCaseInsensitive();
   const int numItems = nis.getNumberOfItems();
   
   indicesSortedByNameOut.resize(numItems, 0);
   for (int i = 0; i < numItems; i++) {
      indicesSortedByNameOut[i] = nis.getSortedIndex(i);
   }
   
   if (reverseOrderFlag) {
      std::reverse(indicesSortedByNameOut.begin(), indicesSortedByNameOut.end());
   }
}
      
