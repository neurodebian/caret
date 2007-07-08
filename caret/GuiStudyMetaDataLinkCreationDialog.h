
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

#include "QtDialogModal.h"
#include "StudyMetaDataFile.h"

class QComboBox;
class QButtonGroup;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QTableWidget;
class QtWidgetGroup;

/// class for creating a link to a study in a study meta data file
class GuiStudyMetaDataLinkCreationDialog : public QtDialogModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiStudyMetaDataLinkCreationDialog(QWidget* parent);

      // destructor
      ~GuiStudyMetaDataLinkCreationDialog();
      
      // get the link that was created
      StudyMetaDataLink getLinkCreated() const;
      
      // initialize the selected link
      void initializeSelectedLink(const StudyMetaDataLink& smdl);
      
   protected slots:
      // called when a study table item is changed
      void studyTableItemChanged(int row,int column);
      
      // enable/disable OK button
      void slotEnableDisableOkButton();
      
      // called when a link widget selection is made
      void slotLinkWidgetSelection();
      
      // called when a figure is selected
      void slotLinkFigureSelectionComboBox(int item);
      
      // called when a table is selected
      void slotLinkTableSelectionComboBox(int item);
      
      // called when a page reference is selected
      void slotLinkPageReferenceSelectionComboBox(int item);
      
   protected:
      
      // called when OK or Cancel button pressed
      void done(int r);
      
      // get the index of the selected study
      int getSelectedStudyIndex() const;
      
      // create the study table widget
      QWidget* createStudyTableWidget();
      
      // create the type of link widget
      QWidget* createStudyLinkWidget();
      
      // load the study table widget
      void loadStudyTableWidget();
      
      // load the link widget
      void loadStudyLinkWidget();
      
      /// the study meta data link
      StudyMetaDataLink studyMetaDataLink;
      
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
      QtWidgetGroup* linkFigureWidgetsGroup;
      
      /// link page ref selection combo box
      QComboBox* linkPageReferenceSelectionComboBox;
      
      /// link page ref sub-header combo box
      QComboBox* linkPageReferenceSubHeaderSelectionComboBox;
      
      /// contains widgets enabled if linking to a page ref
      QtWidgetGroup* linkPageReferenceWidgetsGroup;
      
      /// link table selection combo box
      QComboBox* linkTableSelectionComboBox;
      
      /// link table sub-header combo box
      QComboBox* linkTableSubHeaderSelectionComboBox;
      
      /// contains widgets enabled if linking to a table
      QtWidgetGroup* linkTableWidgetsGroup;
      
      /// page number widget group
      QtWidgetGroup* pageNumberWidgetGroup;
      
      /// link page number line edit
      QLineEdit* linkPageNumberLineEdit;
};

#endif // __GUI_STUDY_META_DATA_LINK_CREATION_DIALOG_H__
