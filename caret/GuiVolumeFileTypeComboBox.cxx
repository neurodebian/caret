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

#include "GuiVolumeFileTypeComboBox.h"

/**
 * constructor.
 */
GuiVolumeFileTypeComboBox::GuiVolumeFileTypeComboBox(const bool showRaw,
                                                       const bool showUnknown,
                                                       QWidget* parent)
   : QComboBox(parent)
{
   std::vector<VolumeFile::FILE_READ_WRITE_TYPE> fileTypes;
   std::vector<QString> fileTypeNames;
   VolumeFile::getVolumeFileTypesAndNames(fileTypes, fileTypeNames);

   for (unsigned int i = 0; i < fileTypes.size(); i++) {
      bool useIt = true;
      if (fileTypes[i] == VolumeFile::FILE_READ_WRITE_TYPE_RAW) {
         useIt = showRaw;
      }
      if (fileTypes[i] == VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN) {
         useIt = showUnknown;
      }
      
      if (useIt) {
         addItem(fileTypeNames[i]);
         volumeFileTypes.push_back(fileTypes[i]);
      }
   }
}

/**
 * destructor.
 */
GuiVolumeFileTypeComboBox::~GuiVolumeFileTypeComboBox()
{
}

/**
 * get the volume file type name.
 */
QString 
GuiVolumeFileTypeComboBox::getVolumeFileTypeName() const
{
   const VolumeFile::FILE_READ_WRITE_TYPE ft = getVolumeFileType();
   
   std::vector<VolumeFile::FILE_READ_WRITE_TYPE> fileTypes;
   std::vector<QString> fileTypeNames;
   VolumeFile::getVolumeFileTypesAndNames(fileTypes, fileTypeNames);
   for (unsigned int i = 0; i < fileTypes.size(); i++) {
      if (fileTypes[i] == ft) {
         return fileTypeNames[i];
         break;
      }
   }
   
   return "UNKNOWN";
}

/**
 * get the volume type.
 */
VolumeFile::FILE_READ_WRITE_TYPE 
GuiVolumeFileTypeComboBox::getVolumeFileType() const
{
   const int item = currentIndex();
   if ((item >= 0) && (item < static_cast<int>(volumeFileTypes.size()))) {
      return volumeFileTypes[item];
   }
   return VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN;
}

/**
 * set the volume type.
 */
void 
GuiVolumeFileTypeComboBox::setVolumeFileType(const VolumeFile::FILE_READ_WRITE_TYPE ft)
{
   for (unsigned int i = 0; i < volumeFileTypes.size(); i++) {
      if (volumeFileTypes[i] == ft) {
         setCurrentIndex(i);
         break;
      }
   }
}
