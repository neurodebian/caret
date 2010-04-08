
#ifndef __GUI_STUDY_META_DATA_LINK_CREATION_DIALOG_H__
#define __GUI_STUDY_META_DATA_LINK_CREATION_DIALOG_H__

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

#include "WuQDialog.h"
#include "StudyMetaDataFile.h"
#include "StudyMetaDataLinkSet.h"

class QAction;
class QComboBox;
class QButtonGroup;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTableWidget;
class QToolButton;
class WuQWidgetGroup;

/// class for creating a link to a study in a study meta data file
class GuiStudyMetaDataLinkCreationDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiStudyMetaDataLinkCreationDialog(QWidget* parent);

      // destructor
      ~GuiStudyMetaDataLinkCreationDialog();
      
      // get the link set that was created
      StudyMetaDataLinkSet getLinkSetCreated() const;
      
      // initialize the selected link set
      void initializeSelectedLinkSet(const StudyMetaDataLinkSet& smdls);
      
   protected slots:
      // called when a study table item is changed
      void slotStudySelected(int row,int column);
      
      // enable/disable OK button
      void slotEnableDisableOkButton();
      
      // called when a link widget selection is made
      void enableLinkSelections();
      
      // called when a figure is selected
      void slotLinkFigureSelectionComboBox(int item);
      
      // called when a table is selected
      void slotLinkTableSelectionComboBox(int item);
      
      // called when a page reference is selected
      void slotLinkPageReferenceSelectionComboBox(int item);

      // called to add a new study meta data link
      void slotAddStudyMetaDataLink();
      
      // called to delete the current study meta data link
      void slotDeleteStudyMetaDataLink();

      // called by link selection up arrow
      void slotLinkSelectionUpArrowAction();
      
      // called by link selection down arrow
      void slotLinkSelectionDownArrowAction();
      
   protected:      
      // called when OK or Cancel button pressed
      void done(int r);
      
      // get the selected study metadata link
      StudyMetaDataLink* getSelectedStudyMetaDataLink();
      
      // set the selected study checkbox
      void setSelectedStudyCheckBox(const int studyIndex,
                                    const bool scrollToStudyFlag);
      
      // get the index of the selected study
      int getSelectedStudyIndex() const;
      
      // create the study table widget
      QWidget* createStudyTableWidget();
      
      // create the type of link widget
      QWidget* createStudyLinkWidget();
      
      // create the link selection widget
      QWidget* createLinkSelectionWidget();
      
      // load the study table widget
      void loadStudyTableWidget();
      
      // called to load the selected study metadata link
      void loadStudyMetaDataLink(const bool scrollToStudyFlag);
      
      // update the link selection controls
      void updateLinkSelectionControls();
           
      /// Save the current study
      void saveCurrentStudy();
      
      /// update the link selection labels and arrows
      void updateLinkSelectionLabelsAndArrows();
      
      /// the study meta data link index
      int studyMetaDataLinkIndex;
      
      /// the study meta data link set
      StudyMetaDataLinkSet studyMetaDataLinkSet;
      
      /// the study table widget
      QTableWidget* studyTableWidget;
      
      /// study table column number for check box
      int studyTableColumnNumberCheckBox;
      
      /// study table column number for title
      int studyTableColumnNumberTitle;
      
      /// study table column number for author
      int studyTableColumnNumberAuthor;
      
      /// study table column number for pubmed id
      int studyTableColumnNumberPubMedID;
      
      /// study table column number for project id
      int studyTableColumnNumberProjectID;
      
      /// study table column number for file index
      int studyTableColumnNumberFileIndex;
      
      /// number of columns in study table
      int studyTableColumnTotal;
      
      /// titles of study table columns
      QStringList studyTableColumnTitles;
      
      /// link to study only radio button
      QRadioButton* linkToStudyOnlyRadioButton;
      
      /// link to study figure radio button
      QRadioButton* linkToFigureRadioButton;
      
      /// link to page reference radio button
      QRadioButton* linkToPageReferenceRadioButton;
      
      /// link to study table radio button
      QRadioButton* linkToTableRadioButton;
      
      /// button group for link to study buttons
      QButtonGroup* linkButtonGroup;
      
      /// link figure selection combo box
      QComboBox* linkFigureSelectionComboBox;
      
      /// link to figure panel combo box
      QComboBox* linkFigurePanelSelectionComboBox;
      
      /// contains widgets enabled if linking to a figure
      WuQWidgetGroup* linkFigureWidgetsGroup;
      
      /// link page ref selection combo box
      QComboBox* linkPageReferenceSelectionComboBox;
      
      /// link page ref sub-header combo box
      QComboBox* linkPageReferenceSubHeaderSelectionComboBox;
      
      /// contains widgets enabled if linking to a page ref
      WuQWidgetGroup* linkPageReferenceWidgetsGroup;
      
      /// link table selection combo box
      QComboBox* linkTableSelectionComboBox;
      
      /// link table sub-header combo box
      QComboBox* linkTableSubHeaderSelectionComboBox;
      
      /// contains widgets enabled if linking to a table
      WuQWidgetGroup* linkTableWidgetsGroup;
      
      /// page number widget group
      //WuQWidgetGroup* pageNumberWidgetGroup;
      
      /// link page number line edit
      //QLineEdit* linkPageNumberLineEdit;
      
      /// link selected number label
      QLabel* currentLinkNumberLabel;
      
      /// link total number label
      QLabel* numberOfLinksLabel;
      
      /// link selection up arrow
      QToolButton* linkSelectionUpArrowToolButton;
      
      /// link selection down arrow
      QToolButton* linkSelectionDownArrowToolButton;
      
      /// action for link selection up arrow
      QAction* linkSelectionUpArrowAction;
      
      /// action for link selection down arrow
      QAction* linkSelectionDownArrowAction;
      
      /// delete link push button
      QPushButton* deleteLinkPushButton;
      
      /// add link push button
      QPushButton* addNewLinkPushButton;
      
      /// the dialog buttons
      QDialogButtonBox* buttonBox;
};

#endif // __GUI_STUDY_META_DATA_LINK_CREATION_DIALOG_H__
