
#ifndef __GUI_VOLUME_SELECTION_CONTROL_H__
#define __GUI_VOLUME_SELECTION_CONTROL_H__

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

#include <QWidget>

#include "VolumeFile.h"

class QComboBox;
class QLabel;
class QLineEdit;

/// class for selecting a volume loaded in memory
class GuiVolumeSelectionControl : public QWidget {
   Q_OBJECT
   
   public:
      /// type of label for display
      enum LABEL_MODE {
         LABEL_MODE_FILE_NAME,
         LABEL_MODE_FILE_LABEL,
         LABEL_MODE_FILE_LABEL_AND_NAME
      };
      
      /// Constructor
      GuiVolumeSelectionControl(QWidget* parent,
                                const bool showAnatomyVolumes,
                                const bool showFunctionalVolumes,
                                const bool showPaintVolumes,
                                const bool showProbAtlasVolumes,
                                const bool showRgbPaintVolumes,
                                const bool showSegmentationVolumes,
                                const bool showVectorVolumes,
                                const LABEL_MODE labelModeIn,
                                const char* name,
                                const bool enableNewVolumeSelectionIn,
                                const bool enableNameTextEntryIn,
                                const bool enableDescriptiveLabelTextEntryIn);
                                
      /// Destructor
      ~GuiVolumeSelectionControl();
      
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
      
      /// show only the volume type
      void showOnlyVolumeType(const bool b);
      
   signals:
      /// emitted when type or file is selected
      void signalSelectionChanged();
      
   public slots:
      /// Update the control
      void updateControl();
      
      /// Update the volume name line edit
      void updateNameLineEdit();
      
   protected:
      /// volume type label
      QLabel* volumeTypeLabel;
     
      /// volume type combo box
      QComboBox* volumeTypeComboBox;
     
      /// type of volumes to display
      std::vector<VolumeFile::VOLUME_TYPE> volumeTypesToDisplay;
      
      /// the file label
      QLabel* volumeFileLabel;
     
      /// volume file combo box
      QComboBox* volumeFileComboBox;
     
      /// volume files in the combo box
      std::vector<VolumeFile*> volumeFiles;
     
      /// index of "new" volume
      int newVolumeIndex;
     
      /// enable selection of "new" volume
      bool enableNewVolumeSelection;
      
      /// name label
      QLabel* nameLabel;
      
      /// name entry line edit
      QLineEdit* nameLineEdit;
      
      /// the descriptive label
      QLabel* descriptiveLabel;
      
      /// descriptive label line edit
      QLineEdit* descriptiveLabelLineEdit;
      
      /// type of label to display in control
      LABEL_MODE labelMode; 
};

#endif // __GUI_VOLUME_SELECTION_CONTROL_H__

