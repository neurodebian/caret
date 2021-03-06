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

#ifndef __GUI_VOLUME_FILE_SELECTION_COMBO_BOX_H__
#define __GUI_VOLUME_FILE_SELECTION_COMBO_BOX_H__

#include <QComboBox>

#include "VolumeFile.h"

/// combo box for selection of different types of volumes
class GuiVolumeFileSelectionComboBox : public QComboBox {
   public:
      /// Constructor
      GuiVolumeFileSelectionComboBox(const VolumeFile::VOLUME_TYPE volumeTypeIn,
                                     const bool addAllSelectionFlag = false,
                                     QWidget* parent = 0,
                                     const char* name = 0);
                                     
      /// Destructor
      ~GuiVolumeFileSelectionComboBox();
      
      /// get all volumes selected
      bool getAllVolumesSelected() const;
      
      /// Get the selected volume file (NULL if no volumes or ALL selected)
      VolumeFile* getSelectedVolumeFile();
      
      /// Get the selected volume file index (negative if no volumes or ALL selected)
      int getSelectedVolumeFileIndex() const;
      
      /// Set the selected volume file index
      void setSelectedVolumeFileWithIndex(const int index);
      
      /// set the selected volume file
      void setSelectedVolumeFile(const VolumeFile* vf);
      
      /// update the items in the combo box
      void updateComboBox();
      
   protected:
      /// type of volumes for this combo box
      VolumeFile::VOLUME_TYPE volumeType;
      
      /// all selection enabled
      bool allSelectionEnabled;
};

#endif // __GUI_VOLUME_SELECTION_COMBO_BOX_H__

