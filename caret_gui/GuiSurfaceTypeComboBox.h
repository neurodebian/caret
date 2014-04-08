
#ifndef __GUI_SURFACE_TYPE_COMBO_BOX_H__
#define __GUI_SURFACE_TYPE_COMBO_BOX_H__

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

#include <QComboBox>

#include "BrainModelSurface.h"

/// combo box for choosing a surface type
class GuiSurfaceTypeComboBox : public QComboBox {
   Q_OBJECT
   
   public:
      // constructor
      GuiSurfaceTypeComboBox(const bool showUnknown = true,
                             QWidget* parent = 0);
                
      // constructor
      GuiSurfaceTypeComboBox(const std::vector<BrainModelSurface::SURFACE_TYPES> showTheseTypes,
                             QWidget* parent = 0);

      // destructor
      ~GuiSurfaceTypeComboBox();
      
      // set surface type
      void setSurfaceType(const BrainModelSurface::SURFACE_TYPES st);
      
      // get surface type
      BrainModelSurface::SURFACE_TYPES getSurfaceType() const;
      
   protected:
};

#endif // __GUI_SURFACE_TYPE_COMBO_BOX_H__

