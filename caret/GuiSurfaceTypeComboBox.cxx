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

#include "GuiSurfaceTypeComboBox.h"

/**
 * constructor.
 */
GuiSurfaceTypeComboBox::GuiSurfaceTypeComboBox(const bool showUnknown,
                                               QWidget* parent)
   : QComboBox(parent)
{
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_RAW), 
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_RAW));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL), 
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_FIDUCIAL));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_INFLATED), 
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_INFLATED));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_VERY_INFLATED), 
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_VERY_INFLATED));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_SPHERICAL), 
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_SPHERICAL));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL), 
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL), 
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_FLAT),  
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_FLAT));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR),  
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR));
   addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_HULL),  
           static_cast<int>(BrainModelSurface::SURFACE_TYPE_HULL));
   if (showUnknown) {
      addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(BrainModelSurface::SURFACE_TYPE_UNKNOWN),  
              static_cast<int>(BrainModelSurface::SURFACE_TYPE_UNKNOWN));
      setSurfaceType(BrainModelSurface::SURFACE_TYPE_UNKNOWN);
   }
}
          
/**
 * constructor.
 */
GuiSurfaceTypeComboBox::GuiSurfaceTypeComboBox(const std::vector<BrainModelSurface::SURFACE_TYPES> showTheseTypes,
                                               QWidget* parent)
   : QComboBox(parent)
{
   for (unsigned int i = 0; i < showTheseTypes.size(); i++) {
      const BrainModelSurface::SURFACE_TYPES st = showTheseTypes[i];
      addItem(BrainModelSurface::getSurfaceConfigurationIDFromType(st),
              static_cast<int>(st));
   }
}

/**
 * destructor.
 */
GuiSurfaceTypeComboBox::~GuiSurfaceTypeComboBox()
{
}

/**
 * set surface type.
 */
void 
GuiSurfaceTypeComboBox::setSurfaceType(const BrainModelSurface::SURFACE_TYPES st)
{
   for (int i = 0; i < count(); i++) {
      if (itemData(i).toInt() == static_cast<int>(st)) {
         setCurrentIndex(i);
         return;
      }
   }
}

/**
 * get surface type.
 */
BrainModelSurface::SURFACE_TYPES 
GuiSurfaceTypeComboBox::getSurfaceType() const
{
   return static_cast<BrainModelSurface::SURFACE_TYPES>(itemData(currentIndex()).toInt());
}
