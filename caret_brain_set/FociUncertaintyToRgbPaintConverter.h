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



#ifndef __VE_FOCI_UNCERTAINTY_TO_RGB_PAINT_CONVERTER_H__
#define __VE_FOCI_UNCERTAINTY_TO_RGB_PAINT_CONVERTER_H__

#include <QString>
#include <vector>

class BrainSet;

/// Class that converts foci uncertainty into an RGB Paint file.
class FociUncertaintyToRgbPaintConverter {
   public:
      /// Constructor
      FociUncertaintyToRgbPaintConverter();
      
      /// Destructor
      ~FociUncertaintyToRgbPaintConverter();
      
      /// convert foci uncertainty to rgb paint (returns true if successful)
      bool convert(BrainSet* brain,
                   const float lowerLimit, const float middleLimit, const float upperLimit,
                   const int rgbPaintColumn, const QString& rgbPaintColumnNewName,
                   QWidget* progressDialogParent, QString& errorMessage);
              
   private:

};

#endif // __VE_FOCI_UNCERTAINTY_TO_RGB_PAINT_CONVERTER_H__

