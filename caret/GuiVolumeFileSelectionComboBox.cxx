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

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiMainWindow.h"
#include "GuiVolumeFileSelectionComboBox.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumeFileSelectionComboBox::GuiVolumeFileSelectionComboBox(
                                             const VolumeFile::VOLUME_TYPE volumeTypeIn,
                                             QWidget* parent,
                                             const char* /* name */)
   : QComboBox(parent)
{
   volumeType = volumeTypeIn;
   updateComboBox();
}
                                 
/**
 * Destructor.
 */
GuiVolumeFileSelectionComboBox::~GuiVolumeFileSelectionComboBox()
{
}

/**
 * Get the selected volume file.
 */
VolumeFile* 
GuiVolumeFileSelectionComboBox::getSelectedVolumeFile()
{
   const int index = getSelectedVolumeFileIndex();
   if (index >= 0) {
      return volumeFilePointers[index];
   }
   return NULL;
}

/**
 * Get the selected volume file index.
 */
int 
GuiVolumeFileSelectionComboBox::getSelectedVolumeFileIndex() const
{
   const int index = currentIndex();
   if ((index >= 0) && (index < static_cast<int>(volumeFilePointers.size()))) {
      return index;
   }
   return -1;
}

/**
 * Set the selected volume file index.
 */
void 
GuiVolumeFileSelectionComboBox::setSelectedVolumeFileWithIndex(const int index)
{
   if ((index >= 0) && (index < static_cast<int>(volumeFilePointers.size()))) {
      setCurrentIndex(index);
   }
}

/**
 * Set the selected volume file.
 */
void 
GuiVolumeFileSelectionComboBox::setSelectedVolumeFile(const VolumeFile* vf)
{
   for (int i = 0; i < static_cast<int>(volumeFilePointers.size()); i++) {
      if (volumeFilePointers[i] == vf) {
         setCurrentIndex(i);
         break;
      }
   }
}

/**
 * Update the items in the combo box.
 */
void 
GuiVolumeFileSelectionComboBox::updateComboBox()
{
   //
   // Find currently selected volume file
   //
   VolumeFile* defaultVolumeFile = NULL;
   const int currentIndexVal = currentIndex();
   if ((currentIndexVal >= 0) && (currentIndexVal < count())) {
      if ((currentIndexVal >= 0) && (currentIndexVal < static_cast<int>(volumeFilePointers.size()))) {
         defaultVolumeFile = volumeFilePointers[currentIndexVal];
      }
   }
   
   //
   // Remove everything from the combo box
   //
   clear();
   volumeFilePointers.clear();
   
   //
   // Default selected item
   //
   int defaultIndex = -1;
   
   //
   // Load the combo box
   //
   switch (volumeType) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeAnatomyFile(i);
            volumeFilePointers.push_back(vf);
            if (vf == defaultVolumeFile) {
               defaultIndex = i;
            }
            addItem(FileUtilities::basename(vf->getFileName()));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeFunctionalFile(i);
            volumeFilePointers.push_back(vf);
            if (vf == defaultVolumeFile) {
               defaultIndex = i;
            }
            addItem(FileUtilities::basename(vf->getFileName()));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_PAINT:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumePaintFile(i);
            volumeFilePointers.push_back(vf);
            if (vf == defaultVolumeFile) {
               defaultIndex = i;
            }
            addItem(FileUtilities::basename(vf->getFileName()));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i);
            volumeFilePointers.push_back(vf);
            if (vf == defaultVolumeFile) {
               defaultIndex = i;
            }
            addItem(FileUtilities::basename(vf->getFileName()));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_RGB:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeRgbFile(i);
            volumeFilePointers.push_back(vf);
            if (vf == defaultVolumeFile) {
               defaultIndex = i;
            }
            addItem(FileUtilities::basename(vf->getFileName()));
         }
         break; 
      case VolumeFile::VOLUME_TYPE_ROI:
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeSegmentationFile(i);
            volumeFilePointers.push_back(vf);
            if (vf == defaultVolumeFile) {
               defaultIndex = i;
            }
            addItem(FileUtilities::basename(vf->getFileName()));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_VECTOR:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeVectorFile(i);
            volumeFilePointers.push_back(vf);
            if (vf == defaultVolumeFile) {
               defaultIndex = i;
            }
            addItem(FileUtilities::basename(vf->getFileName()));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;   
   }
   
   //
   // Initialize default item
   //
   if (defaultIndex >= 0) {
      setCurrentIndex(defaultIndex);
   }
}

      
