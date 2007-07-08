
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

#include "GuiVolumeVoxelDataTypeComboBox.h"

/**
 * constructor.
 */
GuiVolumeVoxelDataTypeComboBox::GuiVolumeVoxelDataTypeComboBox(const bool showUnknown,
                                                               QWidget* parent)
   : QComboBox(parent)
{
   //
   // Get the volume types
   //
   std::vector<QString> names;
   VolumeFile::getAllVoxelDataTypesAndNames(types, names, showUnknown);
   
   for (unsigned int i = 0; i < types.size(); i++) {
      addItem(names[i]);
   }
}

/**
 * destructor.
 */
GuiVolumeVoxelDataTypeComboBox::~GuiVolumeVoxelDataTypeComboBox()
{
}

/**
 * get the volume type.
 */
VolumeFile::VOXEL_DATA_TYPE 
GuiVolumeVoxelDataTypeComboBox::getVolumeVoxelDataType() const
{
   const int item = currentIndex();
   if ((item >= 0) && (item < static_cast<int>(types.size()))) {
      return types[item];
   }
   return VolumeFile::VOXEL_DATA_TYPE_UNKNOWN;
}

/**
 * set the volume type.
 */
void 
GuiVolumeVoxelDataTypeComboBox::setVolumeVoxelDataType(const VolumeFile::VOXEL_DATA_TYPE vt)
{
   for (unsigned int i = 0; i < types.size(); i++) {
      if (types[i] == vt) {
         setCurrentIndex(i);
         break;
      }
   }
}

