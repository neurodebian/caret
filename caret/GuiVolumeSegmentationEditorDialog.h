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

#ifndef __GUI_VOLUME_SEGMENTATION_EDITOR_DIALOG_H__
#define __GUI_VOLUME_SEGMENTATION_EDITOR_DIALOG_H__

#include <list>
#include <vector>

#include "WuQDialog.h"

#include "VolumeFile.h"
#include "VolumeModification.h"

class QComboBox;
class QGroupBox;
class QTextBrowser;
class GuiVolumeSelectionControl;

/// class dialog that allows editing of a segmentation volume
class GuiVolumeSegmentationEditorDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiVolumeSegmentationEditorDialog(QWidget* parent);
      
      /// Destructor
      ~GuiVolumeSegmentationEditorDialog();
      
      /// called when a voxel is selected for editing in the main window
      void processVoxel(const int i, const int j, const int k,
                        const VolumeFile::VOLUME_AXIS pickAxis);
      
      /// called when dialog is popped up
      virtual void show();
      
      /// update the dialog
      void updateDialog();
      
   private slots:
      /// called when dialog is closed
      virtual void close();
      
      /// called when edit mode combo box selection made
      void slotModeComboBox(int item);
      
      /// called when the undo button is pressed
      void slotUndoButton();
      
      /// called when apply button is pressed
      void slotApplyButton();
      
   private:      
      /// the edit mode
      enum EDIT_MODE {
         ///  copy subvolume edit mode
         EDIT_MODE_COPY_SUBVOLUME,
         ///  dilate edit mode
         EDIT_MODE_DILATE,
         ///  erode edit mode
         EDIT_MODE_ERODE,
         ///  flood fill 2D edit mode
         EDIT_MODE_FLOOD_FILL_2D,
         ///  flood fill 3D edit mode
         EDIT_MODE_FLOOD_FILL_3D,
         ///  remove connected 2D edit mode
         EDIT_MODE_REMOVE_CONNECTED_2D,
         ///  remove connected 3D edit mode
         EDIT_MODE_REMOVE_CONNECTED_3D,
         ///  retain all voxels connected to the selected voxel edit mode
         EDIT_MODE_RETAIN_CONNECTED_3D,
         ///  turn voxels on edit mode
         EDIT_MODE_TURN_VOXELS_ON,
         ///  turn voxels off edit mode
         EDIT_MODE_TURN_VOXELS_OFF
      };
      
      /// creates the toggle voxel section
      QWidget* createToggleVoxelsSection();
      
      /// create the other volume section
      QWidget* createOtherVolumeSection();
      
      /// clear the undo stack
      void clearUndoStack();
      
      /// enable the undo button
      void enableUndoButton();
      
      /// Set the help information.
      void loadHelpInformation(const QString& info);
      
      /// get the memory size of the undo container
      int getUndoContainerMemorySize() const;
      
      /// voxel region size
      enum TOGGLE_VOXELS_SIZE {
         TOGGLE_VOXELS_SIZE_1_BY_1,
         TOGGLE_VOXELS_SIZE_3_BY_3,
         TOGGLE_VOXELS_SIZE_5_BY_5,
         TOGGLE_VOXELS_SIZE_7_BY_7,
         TOGGLE_VOXELS_SIZE_9_BY_9,
         TOGGLE_VOXELS_SIZE_11_BY_11,
         TOGGLE_VOXELS_SIZE_ALL
      };
      
      /// the current editing mode
      EDIT_MODE editMode;
      
      /// mode combo box
      QComboBox* modeComboBox;
      
      /// toggle voxels widget
      QWidget* toggleVoxelsWidget;
      
      /// box for horizontal voxel size
      QGroupBox* currentSliceGroupBox;
      
      /// box for three dimensional info
      QGroupBox* threeDimGroupBox;
      
      /// box for other volume
      QGroupBox* otherVolumeVBox;
      
      /// combo box for three dimensional operations
      QComboBox* threeDimSizeComboBox;
      
      /// toggle voxels size combo box
      QComboBox* toggleVoxelsSizeComboBox;
      
      /// the undo button
      QPushButton* undoButton;
      
      /// container for storing voxel changes
      std::list<VolumeModification> undoContainer;
      
      /// text browser for help
      QTextBrowser* helpBrowser;
      
      /// other volume selection box
      GuiVolumeSelectionControl* otherVolumeComboBox;
};

#endif // __GUI_VOLUME_SEGMENTATION_EDITOR_DIALOG_H__

