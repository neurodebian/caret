
#ifndef __GUI_STUDY_META_DATA_FILE_EDITOR_DIALOG_H__
#define __GUI_STUDY_META_DATA_FILE_EDITOR_DIALOG_H__

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

#include <QGroupBox>

#include "QtDialogModal.h"
#include "QtDialogNonModal.h"
#include "StudyMetaDataFile.h"

class QButtonGroup;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QScrollArea;
class QSpinBox;
class QTextEdit;
class WuQWidgetGroup;
class QVBoxLayout;
class StudyFigurePanelWidget;
class StudyFigureWidget;
class StudyMetaAnalysisWidget;
class StudyPageReferenceWidget;
class StudyProvenanceWidget;
class StudyTableWidget;
class StudySubHeaderWidget;
class StudyWidget;

/// class for a GUI used to edit StudyMetaDataFile
class GuiStudyMetaDataFileEditorDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiStudyMetaDataFileEditorDialog(QWidget* parent);
   
      // destructor
      ~GuiStudyMetaDataFileEditorDialog();
   
      // update the dialog
      void updateDialog();
      
   public slots:
      // called to update show meta-analysis check box
      void updateShowMetaAnalysisCheckBox();
      
   protected slots:
      // called when first study button pressed
      void slotFirstStudyPushButton();
      
      // called study selection spin box value changed
      void slotStudySelectionSpinBoxValueChanged(int val);
      
      // called when last study button pressed
      void slotLastStudyPushButton();
      
      // called when study title button pressed
      void slotStudyChooseTitlePushButton();
      
      // called when study author button pressed
      void slotStudyChooseAuthorPushButton();
      
      // called when new study button pressed
      void slotNewStudyPushButton();
      
      // called when delete study button pressed
      void slotDeleteStudyPushButton();
      
      // called to enable/disable selection push buttons
      void slotEnableDisablePushButtons();
      
      // called to close the dialog
      void slotCloseDialog();
      
      // called when add figure button pressed
      void slotFigureAddPushButton();
      
      // called to add table button pressed
      void slotTableAddPushButton();
      
      // called when show meta-analysis check box value is changed
      void slotShowMetaAnalysisCheckBox(bool b);
      
      // called to add page reference button pressed
      void slotPageReferenceAddPushButton();
      
      // called to add provenance button pressed
      void slotProvenanceAddPushButton();
      
      // called when help button pressed
      void slotHelpPushButton();
      
      // called when Fetch All PMID's button pressed
      void slotFetchAllStudiesPushButton();
      
      // called when import meta-analysis button pressed
      void slotImportMetaAnalysisFilePushButton();
      
      // called when delete studies by name button pressed
      void slotDeleteStudiesByNamePushButton();
      
      // called to create new studies out of a study's meta-analysis studies
      void slotCreateStudiesFromMetaAnalysisStudies();
      
      // called when pubmed ID button pressed
      void slotStudyFetchPubMedIDPushButton();
      
   protected:
      // update the study selection spin box
      void updateStudySelectionSpinBox();
      
      // get the current study widget
      StudyWidget* getCurrentStudyWidget();
      
      // load study data into the dialog
      void loadStudyMetaDataIntoDialog(const bool saveCurrentDataFlag);
      
      // save study metadata currently in dialog
      void saveStudyMetaDataFromDialog();
      
      // create the study selection buttons
      QWidget* createStudySelectionButtons();
      
      /// the layout for editing widgets
      QVBoxLayout* editingWidgetsLayout;
      
      /// study selection spin box
      QSpinBox* studySelectionSpinBox;
      
      /// study first push button
      QPushButton* studyFirstPushButton;
      
      /// study last push button
      QPushButton* studyLastPushButton;
      
      /// study choose by title button
      QPushButton* studyChooseTitlePushButton;
      
      /// study choose by author button
      QPushButton* studyChooseAuthorPushButton;
      
      /// study new push button
      QPushButton* studyNewPushButton;
      
      /// study import meta-analysis file
      QPushButton* importMetaAnalysisFilePushButton;
      
      /// study delete push button
      QPushButton* studyDeletePushButton;
      
      /// add figure push button
      QPushButton* figureAddPushButton;
      
      /// add table push button
      QPushButton* tableAddPushButton;
      
      /// add page reference push button
      QPushButton* pageReferenceAddPushButton;
      
      /// add provenance push button
      QPushButton* provenanceAddPushButton;
      
      /// fetch all studies push button
      QPushButton* fetchAllStudiesPushButton;
      
      /// delete studies by name push button
      QPushButton* deleteStudiesByNamePushButton;
      
      /// show meta-analysis check box
      QCheckBox* showMetaAnalysisCheckBox;
      
      /// index of currently displayed study metadata
      int currentStudyMetaDataFileIndex;
      
};

//=========================================================================
//
/// the study widget
class StudyWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudyWidget(GuiStudyMetaDataFileEditorDialog* parentStudyMetaDataFileEditorDialogIn,
                  StudyMetaData* studyMetaDataIn,
                           QWidget* parentIn = 0);
      
      // destructor
      ~StudyWidget();
   
      // add a figure to the study (if NULL add new, empty figure)
      void addFigure(StudyMetaData::Figure* figure);
      
      // remove a figure widget from its layout (does not delete the widget)
      void removeFigureWidget(StudyFigureWidget* figureWidget);

      // add a table to the study (if NULL add new, empty table)
      void addTable(StudyMetaData::Table* table);
      
      // remove a table widget from its layout (does not delete the widget)
      void removeTableWidget(StudyTableWidget* tableWidget);
      
      // add a page reference to the study (if NULL add new, empty page reference)
      void addPageReference(StudyMetaData::PageReference* pageReference);
      
      // add a provenance to the study (if NULL add new, empty provenance)
      void addProvenance(StudyMetaData::Provenance* provenance);
      
      // remove a page reference widget from its layout (does not delete the widget)
      void removePageReferenceWidget(StudyPageReferenceWidget* pageReferenceWidget);
      
      // remove a provenance widget from its layout (does not delete the widget)
      void removeProvenanceWidget(StudyProvenanceWidget* provenanceWidget);
      
      // load data into the widget
      void loadData();
      
      // get study metadata from study metadata file that is in this widget
      StudyMetaData* getCurrentStudyMetaData() { return studyMetaData; }
      
      // show or hide meta-analysis widget
      void showHideMetaAnalysisWidget();
      
   public slots:
      // save the data into the study meta data table subheader
      void slotSaveData();
      
      // called when project ID changed
      void slotStudyProjectIDLineEditChanged();
      
      // called when title changed
      void slotStudyTitleLineEditChanged();
      
      // called when authors changed
      void slotStudyAuthorsLineEditChanged();
      
      // called when citation changed
      void slotStudyCitationLineEditChanged();
      
      // called when mesh changed
      void slotStudyMeshLineEditChanged();
      
      // called when name changed
      void slotStudyNameLineEditChanged();
      
      // called when data format changed
      void slotStudyDataFormatLineEditChanged();
      
      // called when data type changed
      void slotStudyDataTypeLineEditChanged();
      
      // called when pubmed id changed
      void slotStudyPubMedIDSpinBoxChanged();
      
      // called when DOI changed
      void slotStudyDocumentObjectIdentifierLineEditChanged();
      
      // called when space changed
      void slotStudyStereotaxicSpaceLineEditChanged();
      
      // called when space description changed
      void slotStudyStereotaxicSpaceDetailsLineEditChanged();
      
      // called to update quality
      void slotStudyQualityLineEditChanged();
      
      // called to update keywords 
      void slotStudyKeywordsLineEditChanged();
      
      // called when keywords editing finished (return pressed or loses focus)
      void slotStudyKeywordsLineEditFinished();
      
      // called to update keywords in GUI
      void slotUpdateKeywordsAndGUI();
      
      // called when part scheme changed
      void slotStudyPartitioningSchemeAbbreviationLineEditChanged();
      
      // called when part scheme abbreviation changed
      void slotStudyPartitioningSchemeFullNameLineEditChanged();
      
      // called when comment changed
      void slotStudyCommentTextEditChanged();
      
      // called when stereotaxic space button pressed
      void slotStudyStereotaxicSpacePushButton();
      
      // called when DOI/URL "LINK" button is pressed
      void slotStudyDocumentObjectIdentifierPushButton();
      
      // called when PubMed ID "LINK" button is pressed
      void slotStudyPubMedIDPushButton();
      
      // called when study data format ADD button is pressed
      void slotStudyDataFormatAddPushButton();
      
      // called when study data type ADD button is pressed
      void slotStudyDataTypeAddPushButton();
      
   public:
      // save all data including figures and tables
      void saveDataIncludingFiguresAndTables();
      
      // all widgets in widget
      WuQWidgetGroup* allWidgetsGroup;
      
      // parent dialog
      GuiStudyMetaDataFileEditorDialog* parentStudyMetaDataFileEditorDialog;
      
      // study metadata from study metadata file that is in this widget
      StudyMetaData* studyMetaData;
      
      /// layout used by this widget
      QVBoxLayout* layout;
      
      /// study file index label
      QLabel* studyFileIndexDetailsLabel;
      
      /// project ID line edit
      QLineEdit* studyProjectIDLineEdit;
      
      /// title line edit
      QLineEdit* studyTitleLineEdit;
      
      /// mesh line edit
      QLineEdit* studyMeshLineEdit;
      
      /// name line edit
      QLineEdit* studyNameLineEdit;
      
      /// authors line edit
      QLineEdit* studyAuthorsLineEdit;
      
      /// citation line edit
      QLineEdit* studyCitationLineEdit;
      
      /// keywords line edit
      QLineEdit* studyKeywordsLineEdit;
      
      /// stereotaxic space line edit
      QLineEdit* studyStereotaxicSpaceLineEdit;
      
      /// stereotaxic space details line edit
      QLineEdit* studyStereotaxicSpaceDetailsLineEdit;
      
      /// data format line edit
      QLineEdit* studyDataFormatLineEdit;
      
      /// data format type edit
      QLineEdit* studyDataTypeLineEdit;
      
      /// PubMed ID line edit
      QLineEdit* studyPubMedIDLineEdit;
      
      /// Document Object Identifier line edit
      QLineEdit* studyDocumentObjectIdentifierLineEdit;
      
      /// comment text edit
      QTextEdit* studyCommentTextEdit;
      
      /// partitioning scheme abbreviation line edit
      QLineEdit* studyPartitioningSchemeAbbreviationLineEdit;
      
      /// partitioning scheme full name line edit
      QLineEdit* studyPartitioningSchemeFullNameLineEdit;
      
      /// quality line edit
      QLineEdit* studyQualityLineEdit;
      
      /// last time study saved line edit
      QLineEdit* studyLastSaveLineEdit;
      
      /// layout for figures
      QVBoxLayout* figuresLayout;
      
      /// layout for page references
      QVBoxLayout* pageReferenceLayout;
      
      /// layout for provenances
      QVBoxLayout* provenanceLayout;
      
      /// layout for tables
      QVBoxLayout* tablesLayout;
      
      /// the meta-analysis widget
      StudyMetaAnalysisWidget* metaAnalysisWidget;
      
      /// keeps track of changes to keywords
      bool keywordsModifiedFlag;
};

//=========================================================================
//
/// the sub header widget
class StudySubHeaderWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudySubHeaderWidget(StudyMetaData::SubHeader* subHeaderIn,
                           StudyTableWidget* parentStudyTableWidgetIn,
                           StudyPageReferenceWidget* parentPageReferenceWidgetIn,
                           QWidget* parentIn = 0);
      
      // destructor
      ~StudySubHeaderWidget();
   
      // load data into the widget
      void loadData();
      
      /// get the sub header in this widget
      StudyMetaData::SubHeader* getSubHeaderInThisWidget() { return subHeader; }
      
   public slots:
      // save the data into the study meta data table subheader
      void slotSaveData();
      
      // called when delete this sub header button is pressed
      void slotDeleteThisSubHeader();
      
      // called when sub header number changed
      void slotSubHeaderNumberLineEditChanged();
      
      // called when sub header name changed
      void slotSubHeaderNameLineEditChanged();
      
      // called when sub header short name changed
      void slotSubHeaderShortNameLineEditChanged();
      
      // called when sub header short name editing finished
      void slotSubHeaderShortNameEditingFinished();
      
      // called when sub header task description changed
      void slotSubHeaderTaskDescriptionLineEditChanged();
      
      // called when sub header task baseline changed
      void slotSubHeaderTaskBaselineLineEditChanged();
      
      // called when test attributes changed
      void slotSubHeaderTestAttributesLineEditChanged();
      
      // called to update gui
      void slotUpdateShortNamesAndGUI();
      
   public:
      // all widgets in widget
      WuQWidgetGroup* allWidgetsGroup;
      
      // table that is in this widget
      StudyMetaData::SubHeader* subHeader;
      
      /// parent study table widget
      StudyTableWidget* parentStudyTableWidget;
      
      /// parent study page reference widget
      StudyPageReferenceWidget* parentPageReferenceWidget;
      
      /// layout used by this widget
      QVBoxLayout* layout;
      
      /// table subheader number line edit
      QLineEdit* subHeaderNumberLineEdit;
      
      /// table subheader name line edit
      QLineEdit* subHeaderNameLineEdit;
      
      /// table subheader short name line edit
      QLineEdit* subHeaderShortNameLineEdit;
      
      /// subheader short name has been modified
      bool shortNameModifiedFlag;
      
      /// table subheader panel task description
      QLineEdit* subHeaderTaskDescriptionLineEdit;
      
      /// table subheader panel task baseline
      QLineEdit* subHeaderTaskBaselineLineEdit;
      
      /// table subheader panel test attributes
      QLineEdit* subHeaderTestAttributesLineEdit;
};
      
//=========================================================================
//
/// the table widget
class StudyTableWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudyTableWidget(StudyMetaData::Table* tableIn,
                       StudyWidget* parentStudyWidgetIn,
                       QWidget* parentIn = 0);
      
      // destructor
      ~StudyTableWidget();

      // load data into the widget
      void loadData();
      
      // add a sub header to this table widget
      void addSubHeader(StudyMetaData::SubHeader* subHeader);
      
      // remove a sub header widget
      void removeSubHeaderWidget(StudySubHeaderWidget* subHeaderWidget);
      
      // get the table in this table widget
      StudyMetaData::Table* getTableInThisWidget() { return table; }
      
   public slots:
      // save the data into the study meta data table
      void slotSaveData();
      
      // called when add sub header button is pressed
      void slotAddSubHeaderPushButton();
      
      // called when delete this table widget button is pressed.
      void slotDeleteThisTablePushButton();
      
      // called when table number changed
      void slotTableNumberLineEditChanged();
      
      // called when table header changed
      void slotTableHeaderLineEditChanged();
      
      // called when table footer changed
      void slotTableFooterTextEditChanged();
      
      // called when table size units changed
      void slotTableSizeUnitsLineEditChanged();
      
      // called when table voxel size changed
      void slotTableVoxelSizeLineEditChanged();
      
      // called when table statistic changed
      void slotTableStatisticLineEditChanged();
      
      // called when table statistic description changed
      void slotTableStatisticDescriptionLineEditChanged();
      
      // called when statistic push button pressed
      void slotTableStatisticPushButton();
      
   public:
      // all widgets in widget
      WuQWidgetGroup* allWidgetsGroup;
      
      // table that is in this widget
      StudyMetaData::Table* table;
      
      /// layout used by this widget
      QVBoxLayout* layout;
      
      /// table number spin box
      QLineEdit* tableNumberLineEdit;
      
      /// table header line edit
      QLineEdit* tableHeaderLineEdit;
      
      /// table footer text edit
      QTextEdit* tableFooterTextEdit;
      
      /// table size units line edit
      QLineEdit* tableSizeUnitsLineEdit;
      
      /// table voxel size line edit
      QLineEdit* tableVoxelSizeLineEdit;
      
      /// table statistic type line edit
      QLineEdit* tableStatisticLineEdit;
      
      /// table statistic description line edit
      QLineEdit* tableStatisticDescriptionLineEdit;

      /// parent study widget
      StudyWidget* parentStudyWidget;
      
      /// layout for sub headers
      QVBoxLayout* subHeadersLayout;
};
      


//=========================================================================
//
/// the figure widget
class StudyFigureWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudyFigureWidget(StudyMetaData::Figure* figureIn,
                       StudyWidget* parentStudyWidgetIn,
                       QWidget* parentIn = 0);
                   
      // destructor
      ~StudyFigureWidget();
   
      // load data into the widget
      void loadData();
      
      // add a panel to this figure
      void addPanel(StudyMetaData::Figure::Panel* panel);
      
      // remove a panel widget from its layout (does not delete the widget)
      void removePanelWidget(StudyFigurePanelWidget* panelWidget);
      
      // get the figure in this figure widget
      StudyMetaData::Figure* getFigureInThisWidget() { return figure; }
      
   public slots:
      // save the data into the study meta data figure
      void slotSaveData();

      // called when add panel button is pressed
      void slotAddPanelPushButton();
      
      // called when delete this figure widget button is pressed
      void slotDeleteThisFigurePushButton();
      
      // called when figure number changed
      void slotFigureNumberLineEditChanged();
      
      // called when figure legend changed
      void slotFigureLegendLineEditChanged();
      
   public:
      // all widgets in widget
      WuQWidgetGroup* allWidgetsGroup;
      
      // figure that is in this widget
      StudyMetaData::Figure* figure;
      
      /// layout used by this widget
      QVBoxLayout* layout;
      
      /// figure number line edit
      QLineEdit* figureNumberLineEdit;
      
      /// figure legend line edit
      QLineEdit* figureLegendLineEdit;

      /// parent study widget
      StudyWidget* parentStudyWidget;
      
      /// layout for panels
      QVBoxLayout* panelsLayout;
};
      


//=========================================================================
//
/// the figure panel widget
class StudyFigurePanelWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudyFigurePanelWidget(StudyMetaData::Figure::Panel* figurePanelIn,
                             StudyFigureWidget* parentFigureWidgetIn,
                             QWidget* parentIn = 0);
                   
      // destructor
      ~StudyFigurePanelWidget();
   
      // load data into the widget
      void loadData();
      
      /// get the panel in this widget
      StudyMetaData::Figure::Panel* getPanelInThisWidget() { return figurePanel; }
      
   public slots:
      // save the data into the study meta data figure panel
      void slotSaveData();

      // called when delete this panel button pressed
      void slotDeleteThisPanelPushButton();
      
      // called when figure panel identifier changed
      void slotFigurePanelIdentifierLineEditChanged();
      
      // called when figure panel description changed
      void slotFigurePanelDescriptionLineEditChanged();
      
      // called when figure panel task description changed
      void slotFigurePanelTaskDescriptionLineEditChanged();
      
      // called when figure panel task baseline changed
      void slotFigurePanelTaskBaselineLineEditChanged();
      
      // called when figure panel test attributes changed
      void slotFigurePanelTestAttributesLineEditChanged();
      
   public:
      // all widgets in widget
      WuQWidgetGroup* allWidgetsGroup;
      
      // figure that is in this widget
      StudyMetaData::Figure::Panel* figurePanel;
      
      /// parent study figure widget
      StudyFigureWidget* parentFigureWidget;
      
      /// layout used by this widget
      QVBoxLayout* layout;
      
      /// figure panel identifier
      QLineEdit* figurePanelIdentifierLineEdit;
      
      /// figure panel description
      QLineEdit* figurePanelDescriptionLineEdit;
      
      /// figure panel task description
      QLineEdit* figurePanelTaskDescriptionLineEdit;
      
      /// figure panel task baseline
      QLineEdit* figurePanelTaskBaselineLineEdit;
      
      /// figure panel test attributes
      QLineEdit* figurePanelTestAttributesLineEdit;
      

};

//=========================================================================
//
/// the page reference widget
class StudyPageReferenceWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudyPageReferenceWidget(StudyMetaData::PageReference* pageReferenceIn,
                               StudyWidget* parentStudyWidgetIn,
                               QWidget* parentIn = 0);
      
      // destructor
      ~StudyPageReferenceWidget();

      // load data into the widget
      void loadData();
      
      // add a sub header to this page reference widget
      void addSubHeader(StudyMetaData::SubHeader* subHeader);
      
      // remove a sub header widget
      void removeSubHeaderWidget(StudySubHeaderWidget* subHeaderWidget);
      
      // get the page reference in this page reference widget
      StudyMetaData::PageReference* getPageReferenceInThisWidget() { return pageReference; }
      
   public slots:
      // save the data into the study meta data page reference
      void slotSaveData();
      
      // called when add sub header button is pressed
      void slotAddSubHeaderPushButton();
      
      // called when delete this page reference widget button is pressed.
      void slotDeleteThisPageReferencePushButton();
      
      // called when page referencepage number changed
      void slotPageReferencePageNumberLineEditChanged();
      
      // called when page reference header changed
      void slotPageReferenceHeaderLineEditChanged();
      
      // called when page reference comment changed
      void slotPageReferenceCommentTextEditChanged();
      
      // called when page reference size units changed
      void slotPageReferenceSizeUnitsLineEditChanged();
      
      // called when page reference voxel size changed
      void slotPageReferenceVoxelSizeLineEditChanged();
      
      // called when page reference statistic changed
      void slotPageReferenceStatisticLineEditChanged();
      
      // called when page reference statistic description changed
      void slotPageReferenceStatisticDescriptionLineEditChanged();
      
      // called when statistic push button pressed
      void slotPageReferenceStatisticPushButton();
      
   public:
      // all widgets in widget
      WuQWidgetGroup* allWidgetsGroup;
      
      // page reference that is in this widget
      StudyMetaData::PageReference* pageReference;
      
      /// layout used by this widget
      QVBoxLayout* layout;
      
      /// page reference number line edit
      QLineEdit* pageReferenceNumberLineEdit;
      
      /// page reference header line edit
      QLineEdit* pageReferenceHeaderLineEdit;
      
      /// page reference comment text edit
      QTextEdit* pageReferenceCommentTextEdit;
      
      /// page reference size units line edit
      QLineEdit* pageReferenceSizeUnitsLineEdit;
      
      /// page reference voxel size line edit
      QLineEdit* pageReferenceVoxelSizeLineEdit;
      
      /// page reference statistic type line edit
      QLineEdit* pageReferenceStatisticLineEdit;
      
      /// page reference statistic description line edit
      QLineEdit* pageReferenceStatisticDescriptionLineEdit;

      /// parent study widget
      StudyWidget* parentStudyWidget;
      
      /// layout for sub headers
      QVBoxLayout* subHeadersLayout;
};
     
//=========================================================================
//
/// provenance widget
class StudyProvenanceWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudyProvenanceWidget(StudyMetaData::Provenance* provenanceIn,
                            StudyWidget* parentStudyWidgetIn,
                            QWidget* parentIn = 0);
      
      // destructor
      ~StudyProvenanceWidget();
      
      // load data into the widget
      void loadData();
      
      /// get the provenance in this widget
      StudyMetaData::Provenance* getProvenanceInThisWidget() { return provenance; }
      
   public slots:
      // save the data into the study meta data provenance
      void slotSaveData();
      
      // called when delete this provenance widget button is pressed.
      void slotDeleteThisProvenancePushButton();
      
      // called when name changed
      void slotNameLineEditChanged();
      
      // called when date changed
      void slotDateLineEditChanged();
      
      // called when comment changed
      void slotCommentLineEditChanged();
      
   public:
      // all widgets in widget
      WuQWidgetGroup* allWidgetsGroup;
      
      // provenance that is in this widget
      StudyMetaData::Provenance* provenance;
      
      /// layout used by this widget
      QVBoxLayout* layout;
      
      /// provenance name line edit
      QLineEdit* provenanceNameLineEdit;
      
      /// provenance date line edit
      QLineEdit* provenanceDateLineEdit;
      
      /// provenance comment line edit
      QLineEdit* provenanceCommentLineEdit;

      /// parent study widget
      StudyWidget* parentStudyWidget;      
};

//=========================================================================
//
/// meta-analysis widget
class StudyMetaAnalysisWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      StudyMetaAnalysisWidget(StudyNamePubMedID* metaAnalysisStudiesIn,
                              StudyWidget* parentStudyWidgetIn,
                              QWidget* parentIn = 0);
      
      // destructor
      ~StudyMetaAnalysisWidget();

      // load data into the widget
      void loadData();
      
      // get the associated studies in this associated studies widget
      StudyNamePubMedID* getMetaAnalysisInThisWidget() { return metaAnalysisStudies; }
      
   signals:
      // emitted when create studies button is pressed
      void signalCreateStudiesPushButton();
      
   public slots:
      // save the data into the study meta data page reference
      void slotSaveData();
      
      // called when meta-analysis studies changed
      void slotMetaAnalysisStudiesTextEditChanged();
      
   public:
      // page reference that is in this widget
      StudyNamePubMedID* metaAnalysisStudies;
      
      /// meta-analysis studies text edit
      QTextEdit* metaAnalysisStudiesTextEdit;
      
      /// parent study widget
      StudyWidget* parentStudyWidget;
      
      /// all widgets in the group
      WuQWidgetGroup* allWidgetsGroup;
};

/// new study dialog
class GuiStudyMetaDataNewDialog : public QtDialogModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiStudyMetaDataNewDialog(StudyMetaData* currentStudyMetaDataIn,
                                QWidget* parent = 0);
      
      // destructor
      ~GuiStudyMetaDataNewDialog();
      
      // get the new study
      StudyMetaData* getNewStudy() const;
      
      // called when OK/Cancel button pressed
      void done(int r);
      
      // get the PubMed ID
      QString getPubMedID() const;
      
   protected:
      /// new clear entries radio button
      QRadioButton* newEmptyStudyRadioButton;
      
      /// new copy current entries radio button
      QRadioButton* newCopyStudyRadioButton;
      
      /// the current study metadata
      StudyMetaData* currentStudyMetaData;
      
      /// button group for new study
      QButtonGroup* newStudyButtonGroup;
      
      /// new study PubMed ID Line Edit
      QLineEdit* newStudyPubMedIDLineEdit;
      
      /// last checked ID
      static int lastCheckedID;
};

#ifdef __GUI_STUDY_META_DATA_FILE_EDITOR_DIALOG_MAIN_
int GuiStudyMetaDataNewDialog::lastCheckedID = -1;
#endif // __GUI_STUDY_META_DATA_FILE_EDITOR_DIALOG_MAIN_

#endif // __GUI_STUDY_META_DATA_FILE_EDITOR_DIALOG_H__

