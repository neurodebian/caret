
#ifndef __GUI_PAINT_NAME_EDITOR_DIALOG_H__
#define __GUI_PAINT_NAME_EDITOR_DIALOG_H__

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

#include <vector>

#include <QDialog>

class GuiNodeAttributeColumnSelectionComboBox;
class QLabel;
class QGridLayout;
class QSignalMapper;
class WuQWidgetGroup;

/// class for editing paint names
class GuiPaintNameEditorDialog : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiPaintNameEditorDialog(QWidget* parent = 0);
      
      // destructor
      ~GuiPaintNameEditorDialog();
      
      // update the dialog
      void updateDialog();
      
   protected slots:
      // called when a column is selected
      void slotColumnSelectionComboBox(int col);

      // called when a name button is clicked
      void slotNameButtonClicked(int indx);
            
      // called when delete button is clicked
      void slotDeleteButtonClicked(int indx);
      
      // called when deassign button is clicked
      void slotDeassignButtonClicked(int indx);
      
      // called when reassign button is clicked
      void slotReassignButtonClicked(int indx);
      
      // called when edit color button is clicked
      void slotEditColorButtonClicked(int indx);
      
      /// called to add a new paint name
      void slotAddPaintName();
      
   protected:
      // load the paint names
      void loadPaintNames();
      
      /// update the GUI
      void updateGUI();
      
      /// column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* columnSelectionComboBox;
      
      /// grid layout for paint names
      QGridLayout* paintNameGridLayout;
      
      /// paint file index
      std::vector<int> paintFileIndices;
      
      /// paint name actions
      std::vector<QAction*> paintNameActions;
      
      /// paint count labels
      std::vector<QLabel*> paintCountLabels;
      
      /// paint index labels
      std::vector<QLabel*> paintIndexLabels;
      
      /// paint color widget
      std::vector<QLabel*> paintColorLabels;
      
      /// widget groups for each row
      std::vector<WuQWidgetGroup*> rowWidgetGroups;
      
      /// paint labels needed
      bool addColumnTitlesToPaintNameLayoutFlag;
      
      /// signal mapper for paint name buttons
      QSignalMapper* nameButtonsSignalMapper;
      
      /// signal mapper for delete buttons
      QSignalMapper* deleteButtonsSignalMapper;
      
      /// signal mapper for deassign buttons
      QSignalMapper* deassignButtonsSignalMapper;
      
      /// signal mapper for reassign buttons
      QSignalMapper* reassignButtonsSignalMapper;
      
      /// signal mapper for edit color buttons
      QSignalMapper* editColorButtonsSignalMapper;
};

#endif // __GUI_PAINT_NAME_EDITOR_DIALOG_H__
