#ifndef __GUI_COMMA_SEPARATED_VALUES_FILE_EDITOR__
#define __GUI_COMMA_SEPARATED_VALUES_FILE_EDITOR__

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

#include <QMainWindow>

#include "CommaSeparatedValueFile.h"
#include "GuiFilesModified.h"
#include "QtDialogNonModal.h"

class QComboBox;
class QStackedWidget;
class QTableWidget;
class StringTable;
class QToolButton;

/// main window for editing comma separated files
class GuiCommaSeparatedValueFileEditorMainWindow : public QMainWindow {
   Q_OBJECT
   
   public:
      // constructor
      GuiCommaSeparatedValueFileEditorMainWindow(AbstractFile* dataFileIn,
                                                 const GuiFilesModified& filesModified,
                                                 QWidget* parent = 0);
     
      // destructor
      ~GuiCommaSeparatedValueFileEditorMainWindow();
   
      // enable/disable row addition and deletion
      void setRowAdditionDeletionEnabled(const bool enabled);
      
      // check for data file changes
      void checkForFileChanges();
      
   protected slots:
      // called if a cell is double clicked.
      void slotTableCellDoubleClicked(int row,int col);
      
      // called if a cell's contents are changed
      void slotCellChanged(int row, int col);
      
      // header selected
      void rowSelected(int row);
      
      // called to add a row
      void addRow();
      
      // called to delete a row
      void deleteRows();
      
   protected:
      // add a data section to the dialog
      void addDataSection(const StringTable* dataSection);
      
      /// data section combo box selection
      QComboBox* dataSectionComboBox;
      
      /// stacked widget for data sections
      QStackedWidget* dataSectionsStackedWidget;
      
      /// the file being edited
      AbstractFile* dataFile;
      
      /// for file modification update
      GuiFilesModified filesModified;
      
      /// the comma separated values version of the input file
      CommaSeparatedValueFile csvFile;
      
      /// the file data file beging edited
      /// the table widgets
      QVector<QTableWidget*> tableWidgets;
      
      /// the table has been modified
      QVector<bool> tableHasBeenModified;
      
      /// add rows tool button
      QToolButton* addRowToolButton;
      
      /// delete rows tool button
      QToolButton* deleteRowToolButton;
      
      /// file editing is allowed
      bool fileEditingAllowed;
};

/// dialog for editing a comma separated values file
class GuiCommaSeparatedValueFileEditor : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiCommaSeparatedValueFileEditor(QWidget* parent,
                                       AbstractFile* dataFileIn,
                                       const GuiFilesModified& filesModified);
     
      // destructor
      ~GuiCommaSeparatedValueFileEditor();
   
      // enable/disable row addition and deletion
      void setRowAdditionDeletionEnabled(const bool enabled);
      
   protected slots:
      // called when close button pressed
      void slotCloseButtonPressed();
      
   protected:
      // editor main window
      GuiCommaSeparatedValueFileEditorMainWindow* editorMainWindow;
};

#endif // __GUI_COMMA_SEPARATED_VALUES_FILE_EDITOR__

