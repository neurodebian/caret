
#ifndef __GUI_VOCABULARY_FILE_EDITOR_DIALOG_H__
#define __GUI_VOCABULARY_FILE_EDITOR_DIALOG_H__

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

#include "QtDialogNonModal.h"

class GuiStudyInfoEditorWidget;
class QComboBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QTextEdit;

class QtWidgetGroup;

/// dialog for editing a vocabulary file
class GuiVocabularyFileEditorDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiVocabularyFileEditorDialog(QWidget* parent);
      
      // destructor
      ~GuiVocabularyFileEditorDialog();
      
      // update the dialog 
      void updateDialog();
   
   protected slots:
      // called when apply button pressed
      void slotApplyButton();
      
      // called when abbreviation button pressed
      void slotAbbreviationPushButton();
      
      // called when delete button pressed
      void slotDeleteEntryPushButton();
      
      // called when Add or Edit radio button selected
      void slotEntryModeChanged();
      
      // called to load a vocabulary entry
      void slotLoadVocabularyEntry(int indx);
      
      // update the study number combo box
      void slotUpdateStudyNumberComboBox();
      
      // called when vocab study meta data button pressed
      void slotVocabularStudyMetaDataPushButton();
      
   protected:
      // create the vocabulary widget
      QWidget* createVocabularyWidget();
      
      // update edit spin box min/max
      void updateEditEntrySpinBoxMinMax();
      
      /// the tab widget
      QTabWidget* tabWidget;
      
      /// the vocabulary widget
      QWidget* vocabularyWidget;
      
      /// the study info widget
      GuiStudyInfoEditorWidget* studyInfoEditorWidget;
      
      /// abbreviation line edit
      QLineEdit* abbreviationLineEdit;
      
      /// full name line edit
      QLineEdit* fullNameLineEdit;
      
      /// class name line edit
      QLineEdit* classNameLineEdit;
      
      /// vocabulary ID line edit
      QLineEdit* vocabularyIdLineEdit;
      
      /// study meta data line edit
      QLineEdit* vocabularyStudyMetaDataLineEdit;

      /// study number combo box
      QComboBox* studyNumberComboBox;
      
      /// description line edit
      QTextEdit* descriptionTextEdit;
      
      /// add entry radio button
      QRadioButton* entryAddRadioButton;
      
      /// edit entry radio button
      QRadioButton* entryEditRadioButton;
      
      /// editing number spin box
      QSpinBox* entryEditSpinBox;
      
      /// widget group for edit spin box and delete button
      QtWidgetGroup* entryEditWidgetGroup;
      
};

#endif // __GUI_VOCABULARY_FILE_EDITOR_DIALOG_H__
