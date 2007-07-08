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


#ifndef __QT_TEXT_FILE_EDITOR_DIALOG_H__
#define __QT_TEXT_FILE_EDITOR_DIALOG_H__

#include <QStringList>
#include "QtDialog.h"

class QCheckBox;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QToolButton;
class QtTextFileEditorSearchDialog;

/// Dialog for editing text files
class QtTextFileEditorDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      QtTextFileEditorDialog(QWidget* parent);
      
      /// Destructor
      ~QtTextFileEditorDialog();
      
      /// add additional file filters
      void addAdditionalFileFilters(QStringList& aff) { additionalFileFilters = aff; }
      
      /// load a file
      void loadFile(const QString& fileName,
                    const bool richTextFlag = false);
      
   private slots:
      /// called when open button is pressed
      void slotFileOpen();
      
      /// called when as save as button is pressed
      void slotFileSaveAs();
      
      /// called when save button is pressed
      void slotFileSave();
      
      /// called when print button is pressed
      void slotPrint();
      
      /// called to close the text file editor
      void slotClose();
      
      /// called enable save button
      void slotEnableSaveButton();
      
      /// called when find button pressed
      void slotFind();
      
      /// called when font button pressed
      void slotFont();
      
      /// called when go to button pressed
      void slotGoTo();
      
      /// called to turn on/off text wrapping
      void slotWrap();
      
   protected:
      // save a file
      void saveFile(const QString& name);
      
      /// the find/replace dialog
      QtTextFileEditorSearchDialog* findReplaceDialog;
      
      /// the text display
      QTextEdit* textEditor;
      
      /// the wrap tool button
      QToolButton* wrapToolButton;
      
      /// save button
      QToolButton* saveToolButton;
      
      /// name of file being edited
      QString filename;
      
      /// additional file filters
      QStringList additionalFileFilters;
      
      /// current file filter
      QString currentFileFilter;
      
      /// previously search text
      QString previousSearchText;
      
      /// previously replace text
      QString previousReplaceText;
      
      /// paragraph of last search
      int paragraphNum;
      
      /// letter index in paragraph of last search
      int paragraphChar;
      
      /// previous Go To Line Number
      int previousLineNumber;
};

/// search dialog
class QtTextFileEditorSearchDialog : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      QtTextFileEditorSearchDialog(QTextEdit* editorIn,
                                   QWidget* parent = 0);
      
      // destructor
      ~QtTextFileEditorSearchDialog();
      
   protected slots:
      // called when next button pressed
      void slotNextPushButton();
      
      // called when previous button pressed
      void slotPreviousPushButton();
      
      // called when replace button pressed
      void slotReplacePushButton();
      
      // called when replace & find button pressed
      void slotReplaceAndFindPushButton();
      
      // called when replace all button pressed
      void slotReplaceAllPushButton();
      
   protected:
      // called to replace text (returns true if text was found and replaced)
      bool replaceText();
      
      // called to search for text
      void searchForText(const bool searchBackwards);
      
      /// the text editor
      QTextEdit* editor;
      
      /// the find line edit
      QLineEdit* findLineEdit;
      
      /// the replace line edit
      QLineEdit* replaceLineEdit;
      
      /// case sensitive check box
      QCheckBox* caseSensitiveCheckBox;
      
      /// text previously searched for
      QString previousSearchText;
};

#endif  // __QT_TEXT_FILE_EDITOR_DIALOG_H__

