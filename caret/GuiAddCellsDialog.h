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


#ifndef __GUI_ADD_CELLS_DIALOG_H__
#define __GUI_ADD_CELLS_DIALOG_H__

#include <QString>

#include "QtDialog.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QTextEdit;

/// Dialog for adding cells to a surface
class GuiAddCellsDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// dialog mode (adding surface cells or contour cells)
      enum DIALOG_MODE {
         DIALOG_MODE_ADD_CELLS,
         DIALOG_MODE_ADD_CONTOUR_CELLS
      };
      
      /// Constructor
      GuiAddCellsDialog(const DIALOG_MODE dm, QWidget* parent = 0, bool modal = false, Qt::WFlags f = 0);
      
      /// Destructor
      ~GuiAddCellsDialog();
      
      /// add a cell at the specified node number
      void addCellAtNodeNumber(const int nodeNumber);

      /// add a contour cell at the specified position and section number
      void addContourCellAtXYZ(const float xyz[3], const int sectionNumber);

      /// update the file dialog due to file changes
      void updateDialog();
      
   private slots:
      /// Called when apply button is pressed
      void slotApplyButton();
      
      /// Called when button name choose is pressed
      void slotNameButton();
      
      /// Called when button comment choose is pressed
      void slotClassButton();
      
      /// Called when comment combo box item is selected
      void slotCommentComboBox(int item);
      
      /// Called when text is changed in the cell name line edit
      void slotCellNameChanged();

      /// Called when text is changed in the cell class name line edit
      void slotCellClassNameChanged();

   private:
      /// update for cell projections
      void updateCellProjectionMode();
      
      /// update contour cells
      void updateContourCellMode();
      
      /// Get the name of the class.  Returns true if it is missing.
      bool getCellClass();
       
      /// Get the name, the class, and the color.  Returns true if an item is missing.
      bool getNameAndColor();
       
      /// mode of dialog
      DIALOG_MODE dialogMode;
      
      /// cell name line edit
      QLineEdit* nameLineEdit;
      
      /// class name line edit
      QLineEdit* classLineEdit;
      
      /// comment selection combo box
      QComboBox* commentComboBox;
      
      /// comment text edit
      QTextEdit* commentTextEdit;
      
      /// auto project cells check box
      QCheckBox* autoProjectCellsCheckBox;
      
      /// name of the cell
      QString cellName;
      
      /// name of the cell's class
      QString cellClassName;
      
      /// color index for the cell
      int cellColorIndex;
      
      /// name of cell has changed in line edit
      bool cellNameChanged;
      
      /// name of cell class has changed in line edit
      bool cellClassNameChanged;
};

#endif // __GUI_ADD_CELLS_DIALOG_H__

