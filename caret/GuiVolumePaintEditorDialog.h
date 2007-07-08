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

#ifndef __GUI_VOLUME_PAINT_EDITOR_DIALOG_H__
#define __GUI_VOLUME_PAINT_EDITOR_DIALOG_H__

#include <stack>
#include <vector>

#include "QtDialog.h"

#include "VolumeFile.h"
#include "VolumeModification.h"

class QComboBox;
class QLineEdit;
class QPushButton;

/// class dialog that allows editing of a paint volume
class GuiVolumePaintEditorDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiVolumePaintEditorDialog(QWidget* parent);
      
      /// Destructor
      ~GuiVolumePaintEditorDialog();
      
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
      
      /// called when the undo button is pressed
      void slotUndoButton();
      
      /// called when apply button is pressed
      void slotApplyButton();
      
      /// called when name select push button is pressed
      void slotNameSelectionPushButton();

   private:      
      
      /// Create the toggle voxels widget.
      QWidget* createToggleVoxelsSection();

      /// clear the undo stack
      void clearUndoStack();
      
      /// enable the undo button
      void enableUndoButton();
      
      /// voxel region size
      enum TOGGLE_VOXELS_SIZE {
         TOGGLE_VOXELS_SIZE_1_BY_1,
         TOGGLE_VOXELS_SIZE_3_BY_3,
         TOGGLE_VOXELS_SIZE_5_BY_5,
         TOGGLE_VOXELS_SIZE_7_BY_7,
         TOGGLE_VOXELS_SIZE_9_BY_9,
         TOGGLE_VOXELS_SIZE_11_BY_11
      };
      
      /// voxel edit size combo box
      QComboBox* toggleVoxelsSizeComboBox;
      
      /// the undo button
      QPushButton* undoButton;
      
      /// container for storing voxel changes
      std::stack<VolumeModification> undoStack;
      
      /// paint name line edit
      QLineEdit* paintNameLineEdit;
};

#endif // __GUI_VOLUME_SEGMENTATION_EDITOR_DIALOG_H__

