
#include <QAction>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QToolButton>

#include "BrainSet.h"
#include "GuiMainWindow.h"
#include "GuiStudyMetaDataLinkCreationDialog.h"
#include "QtUtilities.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiStudyMetaDataLinkCreationDialog::GuiStudyMetaDataLinkCreationDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Link to Study Metadata");
   
   //
   // Create the link selection widget
   //
   QWidget* linkSelWidget = createLinkSelectionWidget();
   
   //
   // Create the study table widgets
   //
   QWidget* tableWidget  = createStudyTableWidget();
   
   //
   // Create the type of link widget
   //
   QWidget* linkWidget = createStudyLinkWidget();
   
   //
   // Get the layout for the dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(linkSelWidget);
   dialogLayout->addWidget(tableWidget);
   dialogLayout->addWidget(linkWidget);
   
   //
   // load the table
   //
   loadStudyTableWidget();
   
   //
   // Dialog buttons
   //
   buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok 
                                                      | QDialogButtonBox::Cancel);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(accepted()),
                    this, SLOT(accept()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(reject()));
}

/**
 * destructor.
 */
GuiStudyMetaDataLinkCreationDialog::~GuiStudyMetaDataLinkCreationDialog()
{
}

/** 
 * create the study table widget.
 */
QWidget* 
GuiStudyMetaDataLinkCreationDialog::createStudyTableWidget()
{
   //
   // setup column numbers for study table
   //
   studyTableColumnTotal = 0;
   studyTableColumnNumberCheckBox = studyTableColumnTotal++;
   studyTableColumnNumberTitle = studyTableColumnTotal++;
   studyTableColumnNumberAuthor = studyTableColumnTotal++;
   studyTableColumnNumberPubMedID = studyTableColumnTotal++;
   studyTableColumnNumberProjectID = studyTableColumnTotal++;
   studyTableColumnNumberFileIndex = studyTableColumnTotal++;
   
   //
   // Set the column titles
   //
   for (int i = 0; i < studyTableColumnTotal; i++) {
      if (i == studyTableColumnNumberCheckBox) {
         studyTableColumnTitles << "Select\nStudy";
      }
      else if (i == studyTableColumnNumberTitle) {
         studyTableColumnTitles << "Title";
      }
      else if (i == studyTableColumnNumberAuthor) {
         studyTableColumnTitles << "Authors";
      }
      else if (i == studyTableColumnNumberPubMedID) {
         studyTableColumnTitles << "PubMed ID";
      }
      else if (i == studyTableColumnNumberProjectID) {
         studyTableColumnTitles << "Project ID";
      }
      else if (i == studyTableColumnNumberFileIndex) {
         studyTableColumnTitles << "File Index";
      }
   }
   
   studyTableWidget = new QTableWidget;
   QObject::connect(studyTableWidget, SIGNAL(cellClicked(int,int)),  // was cellChanged in QT 4.1.x
                    this, SLOT(slotStudySelected(int,int)));
   
   QGroupBox* g = new QGroupBox("Study Selection");
   QVBoxLayout* l = new QVBoxLayout(g);
   l->addWidget(studyTableWidget);
   return g;
}
 
/**
 * create the type of link widget.
 */
QWidget* 
GuiStudyMetaDataLinkCreationDialog::createStudyLinkWidget()
{
   const int comboMinWidth = 200;
   
   //
   // Type of link radio buttons
   //
   linkToStudyOnlyRadioButton = new QRadioButton("Link to Study Only");
   linkToFigureRadioButton = new QRadioButton("Link to Figure in Study");
   linkToPageReferenceRadioButton = new QRadioButton("Link to Page Reference");
   linkToTableRadioButton = new QRadioButton("Link to Table in Study");
    
   //
   // Figure combo box, panel label, panel combo box
   //
   linkFigureSelectionComboBox = new QComboBox;
   linkFigureSelectionComboBox->setMinimumWidth(comboMinWidth);
   QObject::connect(linkFigureSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotLinkFigureSelectionComboBox(int)));
   QLabel* linkFigurePanelLabel = new QLabel("Panel");
   linkFigurePanelSelectionComboBox = new QComboBox;
   linkFigurePanelSelectionComboBox->setMinimumWidth(comboMinWidth);
   linkFigureWidgetsGroup = new WuQWidgetGroup(this);
   linkFigureWidgetsGroup->addWidget(linkFigureSelectionComboBox);
   linkFigureWidgetsGroup->addWidget(linkFigurePanelLabel);
   linkFigureWidgetsGroup->addWidget(linkFigurePanelSelectionComboBox);
   
   //
   // Page ref combo box, sub header label, sub header combo box
   //
   linkPageReferenceSelectionComboBox = new QComboBox;
   linkPageReferenceSelectionComboBox->setMinimumWidth(comboMinWidth);
   QObject::connect(linkPageReferenceSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotLinkPageReferenceSelectionComboBox(int)));
   QLabel* linkPageReferenceSubHeaderLabel = new QLabel("Subheader");
   linkPageReferenceSubHeaderSelectionComboBox = new QComboBox;
   linkPageReferenceSubHeaderSelectionComboBox->setMinimumWidth(comboMinWidth);
   linkPageReferenceWidgetsGroup = new WuQWidgetGroup(this);
   linkPageReferenceWidgetsGroup->addWidget(linkPageReferenceSelectionComboBox);
   linkPageReferenceWidgetsGroup->addWidget(linkPageReferenceSubHeaderLabel);
   linkPageReferenceWidgetsGroup->addWidget(linkPageReferenceSubHeaderSelectionComboBox);
   
   //
   // Table combo box, subheader label, subheader combo box
   //
   linkTableSelectionComboBox = new QComboBox;
   linkTableSelectionComboBox->setMinimumWidth(comboMinWidth);
   QObject::connect(linkTableSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotLinkTableSelectionComboBox(int)));
   QLabel* linkTableSubHeaderLabel = new QLabel("Subheader");
   linkTableSubHeaderSelectionComboBox = new QComboBox;
   linkTableSubHeaderSelectionComboBox->setMinimumWidth(comboMinWidth);
   linkTableWidgetsGroup = new WuQWidgetGroup(this);
   linkTableWidgetsGroup->addWidget(linkTableSelectionComboBox);
   linkTableWidgetsGroup->addWidget(linkTableSubHeaderLabel);
   linkTableWidgetsGroup->addWidget(linkTableSubHeaderSelectionComboBox);
   
   //
   // Layout links
   //
   QGridLayout* gl = new QGridLayout;
   gl->addWidget(linkToStudyOnlyRadioButton, 0, 0, 1, 2, Qt::AlignLeft);
   
   gl->addWidget(linkToFigureRadioButton, 1, 0, 1, 2, Qt::AlignLeft);
   gl->addWidget(linkFigureSelectionComboBox, 1, 2, 1, 1, Qt::AlignLeft);
   gl->addWidget(linkFigurePanelLabel, 2, 1, 1, 1, Qt::AlignRight);
   gl->addWidget(linkFigurePanelSelectionComboBox, 2, 2, 1, 1, Qt::AlignLeft);

   gl->addWidget(linkToPageReferenceRadioButton, 3, 0, 1, 2, Qt::AlignLeft);
   gl->addWidget(linkPageReferenceSelectionComboBox, 3, 2, 1, 1, Qt::AlignLeft);
   gl->addWidget(linkPageReferenceSubHeaderLabel, 4, 1, 1, 1, Qt::AlignRight);
   gl->addWidget(linkPageReferenceSubHeaderSelectionComboBox, 4, 2, 1, 1, Qt::AlignLeft);

   gl->addWidget(linkToTableRadioButton, 5, 0, 1, 2, Qt::AlignLeft);
   gl->addWidget(linkTableSelectionComboBox, 5, 2, 1, 1, Qt::AlignLeft);
   gl->addWidget(linkTableSubHeaderLabel, 6, 1, 1, 1, Qt::AlignRight);
   gl->addWidget(linkTableSubHeaderSelectionComboBox, 6, 2, 1, 1, Qt::AlignLeft);
   
   gl->setColumnStretch(0,   0);
   gl->setColumnStretch(1,   0);
   gl->setColumnStretch(2, 100);
   //
   // Button group to keep link radio buttons mutually exclusive
   //
   linkButtonGroup = new QButtonGroup(this);
   QObject::connect(linkButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(enableLinkSelections()));
   linkButtonGroup->addButton(linkToStudyOnlyRadioButton, 0);
   linkButtonGroup->addButton(linkToFigureRadioButton, 1);
   linkButtonGroup->addButton(linkToPageReferenceRadioButton, 2);
   linkButtonGroup->addButton(linkToTableRadioButton, 3);
   
   //
   // Link page number
   //
/*
   QLabel* pageNumberLabel = new QLabel("Page Number");
   linkPageNumberLineEdit = new QLineEdit;
   QHBoxLayout* linkPageLayout = new QHBoxLayout;
   linkPageLayout->addWidget(pageNumberLabel);
   linkPageLayout->addWidget(linkPageNumberLineEdit);
   linkPageLayout->addStretch();
*/   
   //
   // Widget group for page link
   //
/*
   pageNumberWidgetGroup = new WuQWidgetGroup(this);
   pageNumberWidgetGroup->addWidget(pageNumberLabel);
   pageNumberWidgetGroup->addWidget(linkPageNumberLineEdit);
*/

   //
   // Group box and layout for items
   //
   QGroupBox* gb = new QGroupBox("Link Type");
   QVBoxLayout* l = new QVBoxLayout(gb);
   l->addLayout(gl);
   //l->addLayout(linkPageLayout);
   
   return gb;
}

/**
 * create the link selection widget.
 */
QWidget* 
GuiStudyMetaDataLinkCreationDialog::createLinkSelectionWidget()
{
   //
   // link label, current link label, number of links label
   //
   QLabel* linkLabel = new QLabel("Link ");
   currentLinkNumberLabel = new QLabel("  ");
   currentLinkNumberLabel->setFixedSize(currentLinkNumberLabel->sizeHint());
   QLabel* ofLabel = new QLabel(" of ");
   numberOfLinksLabel = new QLabel("  ");
   numberOfLinksLabel->setFixedSize(numberOfLinksLabel->sizeHint());

   //
   // Selection arrow actions
   //
   linkSelectionUpArrowAction = new QAction(this);
   QObject::connect(linkSelectionUpArrowAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotLinkSelectionUpArrowAction()));
   linkSelectionDownArrowAction = new QAction(this);
   QObject::connect(linkSelectionDownArrowAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotLinkSelectionDownArrowAction()));
                    
   //
   // Selection arrow tool buttons
   //   
   linkSelectionUpArrowToolButton = new QToolButton;
   linkSelectionUpArrowToolButton->setArrowType(Qt::UpArrow);
   linkSelectionUpArrowToolButton->setDefaultAction(linkSelectionUpArrowAction);
   linkSelectionDownArrowToolButton = new QToolButton;
   linkSelectionDownArrowToolButton->setArrowType(Qt::DownArrow);
   linkSelectionDownArrowToolButton->setDefaultAction(linkSelectionDownArrowAction);
   
   //
   // Add new link button
   //
   addNewLinkPushButton = new QPushButton("Add");
   addNewLinkPushButton->setToolTip("Press this button to add an \n"
                                    "additional Study Metadata Link.");
   addNewLinkPushButton->setAutoDefault(false);
   QObject::connect(addNewLinkPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddStudyMetaDataLink()));
                    
   //
   // Delete Link
   //
   deleteLinkPushButton = new QPushButton("Delete");
   deleteLinkPushButton->setToolTip("Press this button to delete \n"
                                    "the current link.");
   deleteLinkPushButton->setAutoDefault(false);
   QObject::connect(deleteLinkPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteStudyMetaDataLink()));
   
   QtUtilities::makeButtonsSameSize(addNewLinkPushButton,
                                    deleteLinkPushButton);
                                    
   //
   // Group box and layout
   //
   QGroupBox* linkGroupBox = new QGroupBox("Study Link Selection");
   QHBoxLayout* layout = new QHBoxLayout(linkGroupBox);
   layout->addWidget(linkLabel);
   layout->addWidget(currentLinkNumberLabel);
   layout->addWidget(ofLabel);
   layout->addWidget(numberOfLinksLabel);
   layout->addWidget(linkSelectionDownArrowToolButton);
   layout->addWidget(linkSelectionUpArrowToolButton);
   layout->addWidget(addNewLinkPushButton);
   layout->addWidget(deleteLinkPushButton);
   layout->addStretch();
   
   return linkGroupBox;
}
   
/**
 * called by link selection up arrow.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotLinkSelectionUpArrowAction()
{
   saveCurrentStudy();
   studyMetaDataLinkIndex++;
   if (studyMetaDataLinkIndex >= studyMetaDataLinkSet.getNumberOfStudyMetaDataLinks()) {
      studyMetaDataLinkIndex = studyMetaDataLinkSet.getNumberOfStudyMetaDataLinks() - 1;
   }
   loadStudyMetaDataLink(true);
   updateLinkSelectionLabelsAndArrows();
}

/**
 * called by link selection down arrow.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotLinkSelectionDownArrowAction()
{
   saveCurrentStudy();
   studyMetaDataLinkIndex--;
   if (studyMetaDataLinkIndex < 0) {
      studyMetaDataLinkIndex = 0;
   }
   loadStudyMetaDataLink(true);
   updateLinkSelectionLabelsAndArrows();
}
      
/**
 * called to add a new study meta data link.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotAddStudyMetaDataLink()
{
   saveCurrentStudy();
   StudyMetaDataLink smdl;
   studyMetaDataLinkSet.addStudyMetaDataLink(smdl);
   studyMetaDataLinkIndex = studyMetaDataLinkSet.getNumberOfStudyMetaDataLinks() - 1;
   updateLinkSelectionLabelsAndArrows();
   setSelectedStudyCheckBox(-1, false);
   loadStudyMetaDataLink(true);
}

/**
 * update the link selection spin box.
 */
void 
GuiStudyMetaDataLinkCreationDialog::updateLinkSelectionLabelsAndArrows()
{
   linkSelectionUpArrowAction->setEnabled(false);
   linkSelectionDownArrowAction->setEnabled(false);
   const int num = studyMetaDataLinkSet.getNumberOfStudyMetaDataLinks();
   numberOfLinksLabel->setNum(num);
   currentLinkNumberLabel->setNum(studyMetaDataLinkIndex + 1);
   if (num > 0) {
      if (studyMetaDataLinkIndex > 0) {
         linkSelectionDownArrowAction->setEnabled(true);
      }
      if (studyMetaDataLinkIndex < (num - 1)) {
         linkSelectionUpArrowAction->setEnabled(true);
      }
   }
   else {
      currentLinkNumberLabel->setText("  ");
   }
   
   deleteLinkPushButton->setEnabled(num > 0);
}

/**
 * called to delete the current study meta data link.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotDeleteStudyMetaDataLink()
{
   int num = studyMetaDataLinkSet.getNumberOfStudyMetaDataLinks();
   if (num > 0) {
      if ((studyMetaDataLinkIndex >= 0) &&
          (studyMetaDataLinkIndex < num)) {
         studyMetaDataLinkSet.removeStudyMetaDataLink(studyMetaDataLinkIndex);
         num = studyMetaDataLinkSet.getNumberOfStudyMetaDataLinks();
         if (studyMetaDataLinkIndex >= num) {
            studyMetaDataLinkIndex--;
         }
         if (studyMetaDataLinkIndex < 0) {
            studyMetaDataLinkIndex = 0;
         }
         updateLinkSelectionLabelsAndArrows();
         loadStudyMetaDataLink(true);         
      }
   }
}

/**
 * update the link selection controls.
 */
void 
GuiStudyMetaDataLinkCreationDialog::updateLinkSelectionControls()
{
   StudyMetaDataLink* smdl = getSelectedStudyMetaDataLink();
   if (smdl != NULL) {
      //QTableWidgetItem* item = studyTableWidget->item(indx, studyTableColumnNumberCheckBox);
      //item->setCheckState(Qt::Checked);
      
      //
      // Default to study only
      //
      linkToStudyOnlyRadioButton->setChecked(true);
      
      //
      // Check for link to figure
      //
      const QString figNum = smdl->getFigureNumber();
      if (figNum.isEmpty() == false) {
         linkToFigureRadioButton->setChecked(true);
         for (int i = 0; i < linkFigureSelectionComboBox->count(); i++) {
            if (linkFigureSelectionComboBox->itemData(i).toString() == figNum) {
               linkFigureSelectionComboBox->setCurrentIndex(i);
               slotLinkFigureSelectionComboBox(i);
               
               //
               // Check for link to figure panel
               //
               for (int j = 0; j < linkFigurePanelSelectionComboBox->count(); j++) {
                  if (linkFigurePanelSelectionComboBox->itemData(j).toString() ==
                      smdl->getFigurePanelNumberOrLetter()) {
                     linkFigurePanelSelectionComboBox->setCurrentIndex(j);
                     break;
                  }
               }
               break;
            }
         }
      }
      
      //
      // Check for link to page reference
      //
      const QString pageRefNum = smdl->getPageReferencePageNumber();
      if (pageRefNum.isEmpty() == false) {
         linkToPageReferenceRadioButton->setChecked(true);
         for (int i = 0;  i < linkPageReferenceSelectionComboBox->count(); i++) {
            linkPageReferenceSelectionComboBox->setCurrentIndex(i);
            slotLinkPageReferenceSelectionComboBox(i);
            
            //
            // Check for link to subheader
            //
            for (int j = 0; j < linkPageReferenceSubHeaderSelectionComboBox->count(); j++) {
               if (linkPageReferenceSubHeaderSelectionComboBox->itemData(j).toString() ==
                   smdl->getPageReferenceSubHeaderNumber()) {
                  linkPageReferenceSubHeaderSelectionComboBox->setCurrentIndex(j);
               }
            }
         }
      }
      
      //
      // Check for link to table
      //
      const QString tableNum = smdl->getTableNumber();
      if (tableNum.isEmpty() == false) {
         linkToTableRadioButton->setChecked(true);
         for (int i = 0; i < linkTableSelectionComboBox->count(); i++) {
            if (linkTableSelectionComboBox->itemData(i).toString() == tableNum) {
               linkTableSelectionComboBox->setCurrentIndex(i);
               slotLinkTableSelectionComboBox(i);
               
               //
               // Check for link to sub header 
               //
               for (int j = 0; j < linkTableSubHeaderSelectionComboBox->count(); j++) {
                  if (linkTableSubHeaderSelectionComboBox->itemData(j).toString() ==
                      smdl->getTableSubHeaderNumber()) {
                     linkTableSubHeaderSelectionComboBox->setCurrentIndex(j);
                     break;
                  }
               }
               break;
            }
         }
      }
      
      //linkPageNumberLineEdit->setText(smdl->getPageNumber());
   }
}
            
/**
 * initialize the selected link.
 */
void 
GuiStudyMetaDataLinkCreationDialog::initializeSelectedLinkSet(const StudyMetaDataLinkSet& smdls)
{
   studyMetaDataLinkSet = smdls;
   studyMetaDataLinkIndex = 0;
   
   loadStudyMetaDataLink(true);
   updateLinkSelectionLabelsAndArrows();
}
      
/**
 * get the selected study metadata link.
 */
StudyMetaDataLink* 
GuiStudyMetaDataLinkCreationDialog::getSelectedStudyMetaDataLink()
{
   StudyMetaDataLink* smdl = NULL;
   
   if ((studyMetaDataLinkIndex >= 0) &&
       (studyMetaDataLinkIndex < studyMetaDataLinkSet.getNumberOfStudyMetaDataLinks())) {
      smdl = studyMetaDataLinkSet.getStudyMetaDataLinkPointer(studyMetaDataLinkIndex);
   }
   
   return smdl;
}
      
/**
 * called to load the selected study metadata link.
 */
void 
GuiStudyMetaDataLinkCreationDialog::loadStudyMetaDataLink(const bool scrollToStudyFlag)
{
   //
   // Default to linking to study only
   //
   linkToStudyOnlyRadioButton->setChecked(true);
   
   //
   // Clear figure and table combo boxes
   //
   linkFigureSelectionComboBox->clear();
   linkFigurePanelSelectionComboBox->clear();
   linkPageReferenceSelectionComboBox->clear();
   linkPageReferenceSubHeaderSelectionComboBox->clear();
   linkTableSelectionComboBox->clear();
   linkTableSubHeaderSelectionComboBox->clear();
   //linkPageNumberLineEdit->setText("");
   
   //
   // Get the selected study meta data link
   //
   const StudyMetaDataLink* smdl = getSelectedStudyMetaDataLink();
   if (smdl != NULL) {
      //
      // Load figure and table combo boxes
      //
      StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
      const int indx = smdf->getStudyIndexFromPubMedID(smdl->getPubMedID());
      if ((indx >= 0) && 
          (indx < smdf->getNumberOfStudyMetaData())) {
         //
         // Update the checkbox next to the study
         //
         setSelectedStudyCheckBox(indx, scrollToStudyFlag);
         
         //
         // Get the study metadata
         //
         const StudyMetaData* smd = smdf->getStudyMetaData(indx);
         
         //
         // Update the available figure selections
         //
         const int numFigures = smd->getNumberOfFigures();
         for (int i = 0; i < numFigures; i++) {
            const StudyMetaData::Figure* figure = smd->getFigure(i);
            linkFigureSelectionComboBox->addItem(figure->getNumber(),
                                                 figure->getNumber());
         }
         slotLinkFigureSelectionComboBox(0);
         
         //
         // Update the available page ref selections
         //
         const int numPageRefs = smd->getNumberOfPageReferences();
         for (int i = 0; i < numPageRefs; i++) {
            const StudyMetaData::PageReference* pageRef = smd->getPageReference(i);
            linkPageReferenceSelectionComboBox->addItem(pageRef->getPageNumber(),
                                                        pageRef->getPageNumber());
         }
         slotLinkPageReferenceSelectionComboBox(0);
         
         //
         // Update the available table selections
         //
         const int numTables = smd->getNumberOfTables();
         for (int i = 0; i < numTables; i++) {
            const StudyMetaData::Table* table = smd->getTable(i);
            linkTableSelectionComboBox->addItem(table->getNumber(),
                                                table->getNumber());
         }
         slotLinkTableSelectionComboBox(0);
      }
   }
   
   //
   // Update the link control selections
   //
   updateLinkSelectionControls();
   
   //
   // Enable selection of link controls
   //
   enableLinkSelections();   
}
      
/**
 * called when a figure is selected.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotLinkFigureSelectionComboBox(int item)
{
   linkFigurePanelSelectionComboBox->clear();
   
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int indx = getSelectedStudyIndex();
   if ((indx >= 0) && 
       (indx < smdf->getNumberOfStudyMetaData())) {
      const StudyMetaData* smd = smdf->getStudyMetaData(indx);
      const int numFigures = smd->getNumberOfFigures();
      if ((item >= 0) &&
          (item < numFigures)) {
         linkFigurePanelSelectionComboBox->addItem("No Panel", "");
         
         const StudyMetaData::Figure* figure = smd->getFigure(item);
         const int numPanels = figure->getNumberOfPanels();
         for (int j = 0; j < numPanels; j++) {
            const StudyMetaData::Figure::Panel* panel = figure->getPanel(j);
            linkFigurePanelSelectionComboBox->addItem(panel->getPanelNumberOrLetter(), 
                                                      panel->getPanelNumberOrLetter());
         }
      }
   }
}

/**
 * called when a page reference is selected.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotLinkPageReferenceSelectionComboBox(int item)
{
   linkPageReferenceSubHeaderSelectionComboBox->clear();
   
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int indx = getSelectedStudyIndex();
   if ((indx >= 0) && 
       (indx < smdf->getNumberOfStudyMetaData())) {
      const StudyMetaData* smd = smdf->getStudyMetaData(indx);
      const int numPageReferences = smd->getNumberOfPageReferences();
      if ((item >= 0) &&
          (item < numPageReferences)) {
         linkPageReferenceSubHeaderSelectionComboBox->addItem("No Subheader", QVariant(-1));
         
         const StudyMetaData::PageReference* pageRef = smd->getPageReference(item);
         const int numSubHeaders = pageRef->getNumberOfSubHeaders();
         for (int j = 0; j < numSubHeaders; j++) {
            const StudyMetaData::SubHeader* subHeader = pageRef->getSubHeader(j);
            linkPageReferenceSubHeaderSelectionComboBox->addItem(subHeader->getNumber(),
                                                         subHeader->getNumber());
         }
      }
   }
}      

/**
 * called when a table is selected.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotLinkTableSelectionComboBox(int item)
{
   linkTableSubHeaderSelectionComboBox->clear();
   
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int indx = getSelectedStudyIndex();
   if ((indx >= 0) && 
       (indx < smdf->getNumberOfStudyMetaData())) {
      const StudyMetaData* smd = smdf->getStudyMetaData(indx);
      const int numTables = smd->getNumberOfTables();
      if ((item >= 0) &&
          (item < numTables)) {
         linkTableSubHeaderSelectionComboBox->addItem("No Subheader", QVariant(-1));
         
         const StudyMetaData::Table* table = smd->getTable(item);
         const int numSubHeaders = table->getNumberOfSubHeaders();
         for (int j = 0; j < numSubHeaders; j++) {
            const StudyMetaData::SubHeader* subHeader = table->getSubHeader(j);
            linkTableSubHeaderSelectionComboBox->addItem(subHeader->getNumber(),
                                                         subHeader->getNumber());
         }
      }
   }
}

/**
 * called when a link widget selection is made.
 */
void 
GuiStudyMetaDataLinkCreationDialog::enableLinkSelections()
{
   linkToStudyOnlyRadioButton->setEnabled(false);
   linkToFigureRadioButton->setEnabled(false);
   linkToPageReferenceRadioButton->setEnabled(false);
   linkToTableRadioButton->setEnabled(false);
   
   studyTableWidget->setEnabled(getSelectedStudyMetaDataLink() != NULL);
   
   if ((getSelectedStudyIndex() >= 0) &&
       (getSelectedStudyMetaDataLink() != NULL)) {
      linkToStudyOnlyRadioButton->setEnabled(true);
      linkToFigureRadioButton->setEnabled(linkFigureSelectionComboBox->count() > 0);
      linkToPageReferenceRadioButton->setEnabled(linkPageReferenceSelectionComboBox->count() > 0);
      linkToTableRadioButton->setEnabled(linkTableSelectionComboBox->count() > 0);
   }
   linkFigureWidgetsGroup->setEnabled(linkToFigureRadioButton->isChecked() &&
                                      linkToFigureRadioButton->isEnabled());
   linkPageReferenceWidgetsGroup->setEnabled(linkToPageReferenceRadioButton->isChecked() &&
                                             linkToPageReferenceRadioButton->isEnabled());
   linkTableWidgetsGroup->setEnabled(linkToTableRadioButton->isChecked() &&
                                     linkToTableRadioButton->isEnabled());
   //pageNumberWidgetGroup->setEnabled(linkToStudyOnlyRadioButton->isEnabled());
   
   slotEnableDisableOkButton();
}
      
/**
 * enable/disable OK button.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotEnableDisableOkButton()
{
   QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
   if (okButton != NULL) {
      okButton->setEnabled(getSelectedStudyIndex() >= 0);
   }
}
      
/**
 * get the index of the selected study.
 */
int 
GuiStudyMetaDataLinkCreationDialog::getSelectedStudyIndex() const
{
   for (int i = 0; i < studyTableWidget->rowCount(); i++) {
      QTableWidgetItem* item = studyTableWidget->item(i, studyTableColumnNumberCheckBox);
      if (item->checkState() == Qt::Checked) {
         return i;
      }
   }
   
   return -1;
}
      
/**
 * set the selected study checkbox.
 */
void 
GuiStudyMetaDataLinkCreationDialog::setSelectedStudyCheckBox(const int studyIndex,
                                                             const bool scrollToStudyFlag)
{
   //
   // Block signals otherwise setting a checked status will result
   // in a signal calling this slot again (infinite recursion)
   //
   studyTableWidget->blockSignals(true);
   
   for (int i = 0; i < studyTableWidget->rowCount(); i++) {
      QTableWidgetItem* item = studyTableWidget->item(i, 0);
      if (i == studyIndex) {      
         item->setCheckState(Qt::Checked);
      }
      else {
         item->setCheckState(Qt::Unchecked);
      }
   }
   
   if (scrollToStudyFlag &&
       (studyIndex >= 0)) {
      studyTableWidget->scrollToItem(studyTableWidget->item(studyIndex, studyTableColumnNumberCheckBox),
                                     QAbstractItemView::PositionAtTop);
   }
   
   //
   // Allow signals
   //
   studyTableWidget->blockSignals(false);
}
      
/**
 * called when a study table item is changed.
 */
void 
GuiStudyMetaDataLinkCreationDialog::slotStudySelected(int row,int column)
{
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if ((row >= 0) &&
       (row < smdf->getNumberOfStudyMetaData())) {
      StudyMetaData* smd = smdf->getStudyMetaData(row);
      if (column == studyTableColumnNumberCheckBox) {
         setSelectedStudyCheckBox(row, false);
         StudyMetaDataLink* smdl = getSelectedStudyMetaDataLink();
         if (smdl != NULL) {
            //
            // Replace with new link
            //
            StudyMetaDataLink newLink;
            newLink.setPubMedID(smd->getPubMedID());
            *smdl = newLink;
         }
         loadStudyMetaDataLink(false);
      }
   }
}
      
/**
 * load the study table widget.
 */
void 
GuiStudyMetaDataLinkCreationDialog::loadStudyTableWidget()
{
   //
   // Block signals otherwise, signals will call slotStudySelected()
   // before the table is filled and hence a crash
   //
   studyTableWidget->blockSignals(true);
   
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int numberOfStudyMetaData = smdf->getNumberOfStudyMetaData();
   
   if ((numberOfStudyMetaData > 0) && 
       (studyTableColumnTotal > 0)) {
      //
      // set table size and enable it
      //
      studyTableWidget->setRowCount(numberOfStudyMetaData);
      studyTableWidget->setColumnCount(studyTableColumnTotal);
      studyTableWidget->setEnabled(true);
      studyTableWidget->setHorizontalHeaderLabels(studyTableColumnTitles);
      
      studyTableWidget->setColumnWidth(studyTableColumnNumberCheckBox, 80);
      
      //
      // Load the table
      //
      for (int i = 0; i < numberOfStudyMetaData; i++) {
         const StudyMetaData* smd = smdf->getStudyMetaData(i);
         
         QTableWidgetItem* checkItem = new QTableWidgetItem(" ");
         checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
         checkItem->setCheckState(Qt::Unchecked);
         studyTableWidget->setItem(i, studyTableColumnNumberCheckBox, 
                                   checkItem);
         
         QTableWidgetItem* titleItem = new QTableWidgetItem(smd->getTitle());
         titleItem->setFlags(titleItem->flags() & ~Qt::ItemIsEditable);
         studyTableWidget->setItem(i, studyTableColumnNumberTitle,
                                   titleItem);
         
         QTableWidgetItem* authorsItem = new QTableWidgetItem(smd->getAuthors());
         authorsItem->setFlags(authorsItem->flags() & ~Qt::ItemIsEditable);
         studyTableWidget->setItem(i, studyTableColumnNumberAuthor,
                                   authorsItem);
         
         QTableWidgetItem* pubMedItem = new QTableWidgetItem(smd->getPubMedID());
         pubMedItem->setFlags(pubMedItem->flags() & ~Qt::ItemIsEditable);
         studyTableWidget->setItem(i, studyTableColumnNumberPubMedID, 
                                   pubMedItem);
         
         QTableWidgetItem* projectIDItem = new QTableWidgetItem(smd->getProjectID());
         projectIDItem->setFlags(projectIDItem->flags() & ~Qt::ItemIsEditable);
         studyTableWidget->setItem(i, studyTableColumnNumberProjectID,
                                   projectIDItem);
         
         QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i));
         indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEditable);
         studyTableWidget->setItem(i, studyTableColumnNumberFileIndex,
                                   indexItem);
      }
   }
   else {
      //
      // Clear the table and disable it
      //
      studyTableWidget->clear();
      studyTableWidget->setEnabled(false);
   }
   
   //
   // Allow signals
   //
   studyTableWidget->blockSignals(false);
}      

/**
 * get the link that was created.
 */
StudyMetaDataLinkSet
GuiStudyMetaDataLinkCreationDialog::getLinkSetCreated() const
{
   return studyMetaDataLinkSet;
}

/**
 * Save the current study.
 */
void 
GuiStudyMetaDataLinkCreationDialog::saveCurrentStudy()
{
   //
   // Clear the link
   //
   StudyMetaDataLink* smdl = getSelectedStudyMetaDataLink();
   if (smdl != NULL) {
      smdl->clear();
      
      //
      // See if a study is selected
      //
      StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
      const int indx = getSelectedStudyIndex();
      if ((indx >= 0) && 
          (indx < smdf->getNumberOfStudyMetaData())) {
         const StudyMetaData* smd = smdf->getStudyMetaData(indx);
         
         //
         // Set PubMedID
         //
         smdl->setPubMedID(smd->getPubMedID());
         
         //
         // if linking to figure
         //
         if (linkToFigureRadioButton->isChecked()) {
            const int figureIndex = linkFigureSelectionComboBox->currentIndex();
            if ((figureIndex >= 0) &&
                (figureIndex < smd->getNumberOfFigures())) {
               const QString figureNumber = linkFigureSelectionComboBox->itemData(figureIndex).toString();
               const StudyMetaData::Figure* figure = smd->getFigureByFigureNumber(figureNumber);
               if (figure != NULL) {
                  smdl->setFigureNumber(figureNumber);
                  
                  //
                  // Linking to a panel in the figure ??
                  //
                  if (linkFigurePanelSelectionComboBox->count() > 0) {
                     const QString panelNumberOrLetter = linkFigurePanelSelectionComboBox->itemData(
                                              linkFigurePanelSelectionComboBox->currentIndex()).toString();
                     if (panelNumberOrLetter.isEmpty() == false) {
                        smdl->setFigurePanelNumberOrLetter(panelNumberOrLetter);
                     }
                  }
               }
            }
         }
         
         //
         // If linking to page reference
         //
         if (linkToPageReferenceRadioButton->isChecked()) {
            const int pageRefIndex = linkPageReferenceSelectionComboBox->currentIndex();
            if ((pageRefIndex >= 0) &&
                (pageRefIndex < smd->getNumberOfPageReferences())) {
               const QString pageNumber = linkPageReferenceSelectionComboBox->itemData(pageRefIndex).toString();
               smdl->setPageReferencePageNumber(pageNumber);
               
               //
               // Linking to subheader in the page reference ??
               //
               if (linkPageReferenceSubHeaderSelectionComboBox->count() > 0) {
                  const int subHeaderIndex = linkPageReferenceSubHeaderSelectionComboBox->currentIndex();
                  const QString subHeaderNumber = linkPageReferenceSubHeaderSelectionComboBox->itemData(subHeaderIndex).toString();
                  if (subHeaderNumber.isEmpty() == false) {
                     smdl->setPageReferenceSubHeaderNumber(subHeaderNumber);
                  }
               }
            }
         }
         
         //
         // If linking to table
         //
         if (linkToTableRadioButton->isChecked()) {
            const int tableIndex = linkTableSelectionComboBox->currentIndex();
            if ((tableIndex >= 0) &&
                (tableIndex < smd->getNumberOfTables())) {
               const QString tableNumber = linkTableSelectionComboBox->itemData(tableIndex).toString();
               smdl->setTableNumber(tableNumber);
               
               //
               // Linking to a subheader in the table ??
               //
               if (linkTableSubHeaderSelectionComboBox->count() > 0) {
                  const int subHeaderIndex = linkTableSubHeaderSelectionComboBox->currentIndex();
                  const QString subHeaderNumber = linkTableSubHeaderSelectionComboBox->itemData(subHeaderIndex).toString();
                  if (subHeaderNumber.isEmpty() == false) {
                     smdl->setTableSubHeaderNumber(subHeaderNumber);
                  }
               }
            }            
         }
         
         //
         // set page link
         //
         //smdl->setPageNumber(linkPageNumberLineEdit->text());
      }
   }
}
      
/**
 * called when OK or Cancel button pressed.
 */
void 
GuiStudyMetaDataLinkCreationDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      saveCurrentStudy();
   }

   WuQDialog::done(r);
}
