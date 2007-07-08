
#ifndef __GUI_SPEC_FILE_CREATION_DIALOG_H__
#define __GUI_SPEC_FILE_CREATION_DIALOG_H__

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

class GuiStructureComboBox;
class QLineEdit;
class QTextEdit;

/// Dialog for creating a spec file
class GuiSpecFileCreationDialog : public QtDialog {
   Q_OBJECT
   
   public:
      // constructor (dialog is modal)
      GuiSpecFileCreationDialog(QDialog* parent);
      
      // destructor
      ~GuiSpecFileCreationDialog();
      
      // get the name of the created spec file
      QString getCreatedSpecFileName() const { return createdSpecFileName; }
      
   protected slots:
      // called to set directory
      void slotDirectoryPushButton();

      // called when species button pressed
      void slotSpeciesPushButton();
      
      // called when category button pressed
      void slotCategoryPushButton();
      
      // called when space button pressed
      void slotSpacePushButton();
      
   protected:
      // called when OK or Cancel button pressed
      void done(int r);

      // directory line edit
      QLineEdit* directoryLineEdit;
      
      // species line edit
      QLineEdit* speciesLineEdit;
      
      // subject line edit
      QLineEdit* subjectLineEdit;
      
      // space line edit
      QLineEdit* spaceLineEdit;
      
      // category line edit
      QLineEdit* categoryLineEdit;
      
      // structure combo box
      GuiStructureComboBox* structureComboBox;
      
      // description line edit
      QLineEdit* descriptionLineEdit;
      
      // name of the created spec file
      QString createdSpecFileName;
      
      // comment text edit
      QTextEdit* commentTextEdit;
};

#endif // __GUI_SPEC_FILE_CREATION_DIALOG_H__
