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

#include "QtDialog.h"

#include "GuiVolumeSelectionControl.h"

/// dialog for selecting a volume file
class GuiVolumeSelectionDialog : public QtDialog {
   public:
      /// Constructor
      GuiVolumeSelectionDialog(QWidget* parent,
                                const bool showAnatomyVolumes,
                                const bool showFunctionalVolumes,
                                const bool showPaintVolumes,
                                const bool showProbAtlasVolumes,
                                const bool showRgbPaintVolumes,
                                const bool showSegmentationVolumes,
                                const bool showVectorVolumes,
                                const GuiVolumeSelectionControl::LABEL_MODE labelModeIn,
                                const char* name,
                                const bool enableNewVolumeSelectionIn,
                                const bool enableNameTextEntryIn,
                                const bool enableDescriptiveLabelTextEntryIn);
      
      /// Destructor
      ~GuiVolumeSelectionDialog();
      
      /// get the selected volume type
      VolumeFile::VOLUME_TYPE getSelectedVolumeType() const;
      
      /// get the selected volume file (if null NEW might be selected)
      VolumeFile* getSelectedVolumeFile();
      
      /// get the selected volume file (const method)
      const VolumeFile* getSelectedVolumeFile() const;
      
      /// get "New Volume" selected
      bool getNewVolumeSelected() const;
      
      /// set the selected volume file
      void setSelectedVolumeFile(const VolumeFile* vf);
      
      /// get the name for the selected file
      QString getSelectedVolumeFileName() const;
   
      /// get the descriptive label for the selected file
      QString getSelectedVolumeDescriptiveLabel() const;
      
   protected:
      /// the volume selection control
      GuiVolumeSelectionControl* volumeSelectionControl;
      
};
