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

#ifndef __GUI_LOAD_NODE_ATTRIBUTE_FILE_COLUMN_SELECTION_DIALOG_H__
#define __GUI_LOAD_NODE_ATTRIBUTE_FILE_COLUMN_SELECTION_DIALOG_H__

#include <vector>

#include "WuQDialog.h"

#include "AbstractFile.h"

class GuiNodeAttributeColumnSelectionComboBox;
class GiftiNodeDataFile;
class NodeAttributeFile;
class QComboBox;
class QCheckBox;
class QLineEdit;

/// class for selectively loading node attribute file columns
class GuiLoadNodeAttributeFileColumnSelectionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiLoadNodeAttributeFileColumnSelectionDialog(QWidget* parent,
                                                NodeAttributeFile* newFileIn,
                                                NodeAttributeFile* currentFileIn);
                                                
      /// Constructor
      GuiLoadNodeAttributeFileColumnSelectionDialog(QWidget* parent,
                                                GiftiNodeDataFile* newFileIn,
                                                GiftiNodeDataFile* currentFileIn);
                                                
      /// Destructor
      ~GuiLoadNodeAttributeFileColumnSelectionDialog();

      /// Get the destination column in the existing file for a column from 
      std::vector<int> getDestinationColumns() const;
       
      /// Get the erase all existing columns flag
      bool getEraseAllExistingColumns() const;
      
      /// Get the new file's updated column names.
      std::vector<QString> getNewFileColumnNames() const;

      /// get append file comment selection
      AbstractFile::FILE_COMMENT_MODE getAppendFileCommentSelection() const;
      
   protected slots:
      /// called when set all to do not load button pressed
      void slotAllToDoNotLoadPushButton();
      
      /// called when erase all columns check box is selected
      void slotEraseAllExistingColumnsCheckBox(bool b);
      
   protected:
      /// called when OK or Cancel button pressed
      void done(int r);
      
      /// existing file column selection combo boxes
      std::vector<GuiNodeAttributeColumnSelectionComboBox*> columnComboBoxes;
      
      /// new file column names line edits
      std::vector<QLineEdit*> columnNameLineEdits;
      
      /// new file that is to be loaded
      NodeAttributeFile* newFileNAF;
      
      /// file to which new file is append or replaces
      NodeAttributeFile* currentFileNAF;
      
      /// new file that is to be loaded
      GiftiNodeDataFile* newFileNDF;
      
      /// file to which new file is append or replaces
      GiftiNodeDataFile* currentFileNDF;
      
      /// erase all existing columns
      QCheckBox* eraseAllExistingColumnsCheckBox;
      
      /// combo box for file comment append mode
      QComboBox* appendFileCommentComboBox;
};

#endif // __GUI_LOAD_NODE_ATTRIBUTE_FILE_COLUMN_SELECTION_DIALOG_H__

