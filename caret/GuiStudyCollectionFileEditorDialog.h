#ifndef __GUI_STUDY_COLLECTION_FILE_EDITOR_DIALOG_H__
#define __GUI_STUDY_COLLECTION_FILE_EDITOR_DIALOG_H__

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

#include "WuQDialog.h"

class QGridLayout;
class QLineEdit;
class QSignalMapper;
class QSpinBox;
class QTextEdit;
class QToolButton;
class StudyCollection;
class WuQWidgetGroup;

/// class for editing a study collection file
class GuiStudyCollectionFileEditorDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiStudyCollectionFileEditorDialog(QWidget* parent = 0);
      
      // destructor
      ~GuiStudyCollectionFileEditorDialog();
      
      // update the dialog
      void updateDialog();
      
   protected slots:
      // when study collection selection spin box value changed
      void slotStudySelectionSpinBox(int);
      
      // called when add new collection push button pressed
      void slotAddCollectionPushButton();
      
      // called when add studies to collection push button pressed
      void slotAddStudiesPushButton();
      
      // called when delete collection push button pressed
      void slotDeleteCollectionPushButton();
      
      /// called when collection name changed
      void slotCollectionNameLineEditChanged(const QString& text);
      
      /// called when collection creator changed
      void slotCollectionCreatorLineEditChanged(const QString& text);
      
      /// called when collection type changed
      void slotCollectionTypeLineEditChanged(const QString& text);
      
      /// called when collection comment changed
      void slotCollectionCommentTextEditChanged();
      
      /// called when collection study name changed
      void slotCollectionStudyNameLineEditChanged(const QString& text);
      
      /// called when collection PMID changed
      void slotCollectionStudyPMIDLineEditChanged(const QString& text);
      
      /// called when collection search ID changed
      void slotCollectionSearchIDLineEditChanged(const QString& text);
      
      /// called when collection focus list ID changed
      void slotCollectionFociListIDLineEditChanged(const QString& text);
      
      /// called when collection focus color list ID changed
      void slotCollectionFociColorListIDLineEditChanged(const QString& text);
      
      /// called when collection topic changed
      void slotCollectionTopicLineEditChanged(const QString& text);
      
      /// called when collection category ID changed
      void slotCollectionCategoryIDLineEditChanged(const QString& text);
      
      /// called when collection ID changed
      void slotCollectionIDLineEditChanged(const QString& text);
      
      /// called when a study name is changed
      void slotCollectionStudyNameChanged(int);
      
      /// called when a study pmid is changed
      void slotCollectionStudyPMIDChanged(int);
      
      /// called when a study mslid is changed
      void slotCollectionStudyMSLIDChanged(int);
      
      /// called when a study delete push button pressed
      void slotCollectionStudyDeletePushButton(int);
      
   protected:
      // load the currently selected collection into the editor
      void loadSelectedCollectionIntoEditor();
      
      // get the selected study collection
      StudyCollection* getSelectedCollection();
      
      // get the index of the selected collection (-1 if invalid)
      int getIndexOfSelectedCollection() const;
      
      // create the study collection selection section
      QWidget* createStudyCollectionSelectionSection();
      
      // create the study collection section
      QWidget* createStudyCollectionSection();
      
      // create the file operations section
      QWidget* createFileOperationsSection();
      
      // create the collection operations section
      QWidget* createCollectionOperationsSection();
      
      /// study selection spin box
      QSpinBox* studySelectionSpinBox;
      
      /// collection name
      QLineEdit* collectionNameLineEdit;
      
      /// collection creator
      QLineEdit* collectionCreatorLineEdit;
      
      /// collection type
      QLineEdit* collectionTypeLineEdit;
      
      /// collection comment
      QTextEdit* collectionCommentTextEdit;
      
      /// collection study name
      QLineEdit* collectionStudyNameLineEdit;
      
      /// collection PMID
      QLineEdit* collectionStudyPMIDLineEdit;
      
      /// collection search ID
      QLineEdit* collectionSearchIDLineEdit;
      
      /// collection focus ID
      QLineEdit* collectionFociListIDLineEdit;
      
      /// collection focus color list ID
      QLineEdit* collectionFociColorListIDLineEdit;
      
      /// collection topic
      QLineEdit* collectionTopicLineEdit;
      
      /// category id line edit
      QLineEdit* collectionCategoryIDLineEdit;
      
      /// collection id line edit
      QLineEdit* collectionIDLineEdit;
      
      /// collection operations widget
      QWidget* collectionOperationsWidget;
      
      /// collection studies widget
      QWidget* collectionStudiesWidget;
      
      /// line edits containing names of studies in collection
      std::vector<QLineEdit*> collectionStudiesNameLineEdits;
      
      /// line edits containing PMIDs of studies in collection
      std::vector<QLineEdit*> collectionStudiesPMIDLineEdits;
      
      /// line edits containing MSL ID of studies in collection
      std::vector<QLineEdit*> collectionStudyMslIDStudyLineEdits;
      
      /// delete push buttons for studies in collection
      std::vector<QToolButton*> collectionStudiesDeletePushButtons;
      
      /// widget group for study in collection
      std::vector<WuQWidgetGroup*> collectionStudiesWidgetGroups;
      
      /// layout for collection study name/pmid
      QGridLayout* collectionStudiesNamePmidLayout;
      
      /// signal mapper for collection study name changed
      QSignalMapper* collectionStudyNameSignalMapper;
      
      /// signal mapper for collection study PMID changed
      QSignalMapper* collectionStudyPMIDSignalMapper;
      
      /// signal mapper for collection study MSLID changed
      QSignalMapper* collectionStudyMSLIDSignalMapper;
      
      /// signal mapper for collection study delete push button
      QSignalMapper* collectionStudyDeleteSignalMapper;
};

#endif // __GUI_STUDY_COLLECTION_FILE_EDITOR_DIALOG_H__
