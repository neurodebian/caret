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

static const qlonglong ALL_VOLUMES_VALUE = 1;

/**
 * Constructor.
 */
GuiVolumeFileSelectionComboBox::GuiVolumeFileSelectionComboBox(
                                             const VolumeFile::VOLUME_TYPE volumeTypeIn,
                                             const bool addAllSelectionFlag,
                                             QWidget* parent,
                                             const char* /* name */)
   : QComboBox(parent)
{
   volumeType = volumeTypeIn;
   allSelectionEnabled = addAllSelectionFlag;
   updateComboBox();
}
                                 
/**
 * Destructor.
 */
GuiVolumeFileSelectionComboBox::~GuiVolumeFileSelectionComboBox()
{
}

/**
 * get all volumes selected.
 */
bool 
GuiVolumeFileSelectionComboBox::getAllVolumesSelected() const
{
   const int indx = currentIndex();
   if ((indx >= 0) &&
       (indx < count())) {
      if (itemData(indx).toLongLong() == ALL_VOLUMES_VALUE) {
         return true;
      }
   }
   
   return false;
}
      
/**
 * Get the selected volume file.
 */
VolumeFile* 
GuiVolumeFileSelectionComboBox::getSelectedVolumeFile()
{
   const int indx = currentIndex();
   if ((indx >= 0) &&
       (indx < count())) {
      if (itemData(indx).toLongLong() != ALL_VOLUMES_VALUE) {
         return (VolumeFile*)(itemData(indx).toLongLong());
      }
   }

   return NULL;
}

/**
 * Get the selected volume file index.
 */
int 
GuiVolumeFileSelectionComboBox::getSelectedVolumeFileIndex() const
{
   const int indx = currentIndex();
   if ((indx >= 0) &&
       (indx < count())) {
      if (itemData(indx).toLongLong() != ALL_VOLUMES_VALUE) {
         return indx;
      }
   }

   return -1;
}

/**
 * Set the selected volume file index.
 */
void 
GuiVolumeFileSelectionComboBox::setSelectedVolumeFileWithIndex(const int indx)
{
   if ((indx >= 0) && (indx < count())) {
      setCurrentIndex(indx);
   }
}

/**
 * Set the selected volume file.
 */
void 
GuiVolumeFileSelectionComboBox::setSelectedVolumeFile(const VolumeFile* vf)
{
   for (int i = 0; i < count(); i++) {
      if (itemData(i).toLongLong() == qlonglong(vf)) {
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
      defaultVolumeFile = (VolumeFile*)(itemData(currentIndexVal).toLongLong());
   }
   
   //
   // Remove everything from the combo box
   //
   clear();
   
   //
   // Load the combo box
   //
   switch (volumeType) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeAnatomyFile(i);
            addItem(FileUtilities::basename(vf->getFileName()),
                    qlonglong(vf));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeFunctionalFile(i);
            addItem(FileUtilities::basename(vf->getFileName()),
                    qlonglong(vf));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_PAINT:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumePaintFile(i);
            addItem(FileUtilities::basename(vf->getFileName()),
                    qlonglong(vf));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i);
            addItem(FileUtilities::basename(vf->getFileName()),
                    qlonglong(vf));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_RGB:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeRgbFile(i);
            addItem(FileUtilities::basename(vf->getFileName()),
                    qlonglong(vf));
         }
         break; 
      case VolumeFile::VOLUME_TYPE_ROI:
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeSegmentationFile(i);
            addItem(FileUtilities::basename(vf->getFileName()),
                    qlonglong(vf));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_VECTOR:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
            VolumeFile* vf = theMainWindow->getBrainSet()->getVolumeVectorFile(i);
            addItem(FileUtilities::basename(vf->getFileName()),
                    qlonglong(vf));
         }
         break;   
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;   
   }
   
   //
   // Add all
   //
   if (allSelectionEnabled) {
      if (count() > 0) {
         addItem("ALL VOLUMES", qlonglong(ALL_VOLUMES_VALUE));
      }
   }
   
   //
   // Initialize default item
   //
   int defaultIndex = 0;
   for (int i = 0; i < count(); i++) {
      if (itemData(i).toLongLong() == qlonglong(defaultVolumeFile)) {
         defaultIndex = i;
         break;
      }
   }
   
   if ((defaultIndex >= 0) && (defaultIndex < count())) {
      setCurrentIndex(defaultIndex);
   }   
}

      
