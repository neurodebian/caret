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

#include <limits>

#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolTip>

#include "BrainSet.h"
#include "CellFileProjector.h"
#include "DisplaySettingsFoci.h"
#include "FociColorFile.h"
#include "FociFileToPalsProjector.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMapStereotaxicFocusDialog.h"
#include "GuiNameSelectionDialog.h"
#include "GuiStudyInfoEditorWidget.h"
#include "GuiStudyMetaDataLinkCreationDialog.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "global_variables.h"

static const QString noneStudyName("None");

/**
 * Constructor
 */
GuiMapStereotaxicFocusDialog::GuiMapStereotaxicFocusDialog(QWidget* parent)
                          : QtDialog(parent, false)
{
   palsProjector = NULL;
   
   setWindowTitle("Map Stereotaxic Focus");
   
   //
   // Vertical box layout of all items
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(5);
   rows->setSpacing(2);
   
   //
   // Tab widget for foci and studies
   //
   tabWidget = new QTabWidget;
   rows->addWidget(tabWidget);
   
   //
   // Create enter foci page
   //
   fociPageVBox = new QWidget;
   QVBoxLayout* fociPageLayout = new QVBoxLayout(fociPageVBox);
   tabWidget->addTab(fociPageVBox, "Foci");
   createEnterFociTabPage(fociPageLayout);
   
   //
   // Create enter studies page
   //
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   studyInfoEditorWidget = new GuiStudyInfoEditorWidget(fpf->getPointerToStudyInfo());
   studyInfoEditorWidget->hidePartitioningSchemeControls(true);
   tabWidget->addTab(studyInfoEditorWidget, "Studies");
   
   //
   // update dialog with foci file.
   //
   updateDialog();
   
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                  this, SLOT(applyButtonSlot()));
   buttonsLayout->addWidget(applyButton);
   
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                  this, SLOT(close()));
   buttonsLayout->addWidget(closeButton);
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   //
   // If PALS available
   //
   if (projectionPalsAtlasRadioButton->isEnabled() == false) {
      //
      // Use active fiducial for projection
      //
      projectionMainWindowFiducialRadioButton->setChecked(true);
   }
}

/**
 * Destructor
 */
GuiMapStereotaxicFocusDialog::~GuiMapStereotaxicFocusDialog()
{
   if (palsProjector != NULL) {
      delete palsProjector;
   }
}

/**
 * create the enter foci tab page.
 */
void 
GuiMapStereotaxicFocusDialog::createEnterFociTabPage(QVBoxLayout* fociPageLayout)
{
   //
   // Radio buttons for projection surface
   //
   projectionMainWindowFiducialRadioButton = new QRadioButton("Main Window Surface");
   projectionPalsAtlasRadioButton = new QRadioButton("PALS Atlas Surface in Correct Stereotaxic Space");
   
   //
   // Button group to keep radio buttons mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(projectionMainWindowFiducialRadioButton);
   buttGroup->addButton(projectionPalsAtlasRadioButton);
   
   //
   // Group box and layout
   //
   automaticProjectionGroupBox = new QGroupBox("Automatic Projection of Foci to");
   automaticProjectionGroupBox->setCheckable(true);
   automaticProjectionGroupBox->setChecked(true);
   QVBoxLayout* projectionLayout = new QVBoxLayout(automaticProjectionGroupBox);
   projectionLayout->addWidget(projectionMainWindowFiducialRadioButton);
   projectionLayout->addWidget(projectionPalsAtlasRadioButton);
   fociPageLayout->addWidget(automaticProjectionGroupBox);
   
   //
   // Vertical Group Box for New/Edit controls
   //
   QGroupBox* modeGroup = new QGroupBox("Focus Mode");
   fociPageLayout->addWidget(modeGroup);
   QGridLayout* modeGrid = new QGridLayout(modeGroup);
   modeGrid->setSpacing(3);
   
   //
   // Button Group for add & edit buttons
   //
   QButtonGroup* modeButtonGroup = new QButtonGroup(this);
   QObject::connect(modeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotModeButtonSelected()));
   //
   // Add radio button
   //
   newRadioButton = new QRadioButton("Add");
   modeGrid->addWidget(newRadioButton, 0, 0, Qt::AlignLeft);
   modeButtonGroup->addButton(newRadioButton, 0);
   
   //
   // Edit radio button
   //
   editRadioButton = new QRadioButton("Edit Number");
   modeGrid->addWidget(editRadioButton, 1, 0, Qt::AlignLeft);
   modeButtonGroup->addButton(editRadioButton, 1);
   
   //
   // Edit foci number line edit
   //
   editFocusNumberSpinBox = new QSpinBox;
   editFocusNumberSpinBox->setMinimum(0);
   editFocusNumberSpinBox->setMaximum(std::numeric_limits<int>::max());
   editFocusNumberSpinBox->setSingleStep(1);
   editFocusNumberSpinBox->setMinimumWidth(80);
   editFocusNumberSpinBox->setMaximumWidth(80);
   modeGrid->addWidget(editFocusNumberSpinBox, 1, 1, Qt::AlignLeft);
   QObject::connect(editFocusNumberSpinBox, SIGNAL(valueChanged(const QString&)),
                    this, SLOT(editFocusNumberChanged()));
   QObject::connect(editFocusNumberSpinBox, SIGNAL(valueChanged(const QString&)),
                    this, SLOT(loadFocusSlot()));
   
   loadFocusPushButton = new QPushButton("Load");
   loadFocusPushButton->setAutoDefault(false); 
   modeGrid->addWidget(loadFocusPushButton, 1, 2, Qt::AlignLeft);
   QObject::connect(loadFocusPushButton, SIGNAL(clicked()),
                    this, SLOT(loadFocusSlot()));
   loadFocusPushButton->setToolTip(
                 "Press this button to load the focus\n"
                 "whose index is in the box on the left.");
   
   deleteFocusPushButton = new QPushButton("Delete");
   deleteFocusPushButton->setAutoDefault(false); 
   modeGrid->addWidget(deleteFocusPushButton, 1, 3, Qt::AlignLeft);
   QObject::connect(deleteFocusPushButton, SIGNAL(clicked()),
                    this, SLOT(deleteFocusSlot()));
   deleteFocusPushButton->setToolTip(
                 "Press this button to delete the focus\n"
                 "whose index is in the box on the left.");
   
   //
   // Squish the mode section and default to new
   //
   modeGroup->setFixedSize(modeGroup->sizeHint());
   newRadioButton->setChecked(true);
   slotModeButtonSelected();
   
   const int minWidth = 350;

   //
   // Name label, line edit, and choose button
   //
   QPushButton* focusNameButton = new QPushButton("Name...");
   focusNameButton->setAutoDefault(false);
   QObject::connect(focusNameButton, SIGNAL(clicked()),
                    this, SLOT(nameButtonSlot()));
   focusNameLineEdit = new QLineEdit;
   focusNameLineEdit->setMinimumWidth(minWidth);
   
   //
   // Class label, line edit, and choose button
   //
   QPushButton* focusClassNameButton = new QPushButton("Class...");
   focusClassNameButton->setAutoDefault(false);
   QObject::connect(focusClassNameButton, SIGNAL(clicked()),
                    this, SLOT(classNameButtonSlot()));
   focusClassNameLineEdit = new QLineEdit;
   focusClassNameLineEdit->setMinimumWidth(minWidth);
   
   //
   // X, Y, and Z Position label and line edit
   //
   const int coordWidth = 100;
   QLabel* focusCoordLabel = new QLabel("Position (mm)");
   focusXCoordDoubleSpinBox = new QDoubleSpinBox;
   focusXCoordDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   focusXCoordDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   focusXCoordDoubleSpinBox->setDecimals(2);
   focusXCoordDoubleSpinBox->setSingleStep(1.0);
   focusXCoordDoubleSpinBox->setFixedWidth(coordWidth);
   focusYCoordDoubleSpinBox = new QDoubleSpinBox;
   focusYCoordDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   focusYCoordDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   focusYCoordDoubleSpinBox->setDecimals(2);
   focusYCoordDoubleSpinBox->setSingleStep(1.0);
   focusYCoordDoubleSpinBox->setFixedWidth(coordWidth);
   focusZCoordDoubleSpinBox = new QDoubleSpinBox;
   focusZCoordDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   focusZCoordDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   focusZCoordDoubleSpinBox->setDecimals(2);
   focusZCoordDoubleSpinBox->setSingleStep(1.0);
   focusZCoordDoubleSpinBox->setFixedWidth(coordWidth);
   
   //
   // Study label and combo box
   //
   QLabel* focusStudyLabel = new QLabel("Study in Focus File");
   focusStudyComboBox = new QComboBox;
   focusStudyComboBox->setMinimumWidth(minWidth);

   //
   // Study Meta Data Push Button and line edit
   //
   QPushButton* focusStudyMetaDataPushButton = new QPushButton("Study Metadata...");
   focusStudyMetaDataPushButton->setAutoDefault(false);
   QObject::connect(focusStudyMetaDataPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFocusStudyMetaDataPushButton()));
   focusStudyMetaDataLineEdit = new QLineEdit;
   focusStudyMetaDataLineEdit->setMinimumWidth(minWidth);
   focusStudyMetaDataLineEdit->setReadOnly(true);
                    
   //
   // area label and line edit
   //
   QPushButton* focusAreaPushButton = new QPushButton("Area...");  //"Cortical Area...");
   focusAreaPushButton->setAutoDefault(false);
   QObject::connect(focusAreaPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCorticalAreaPushButton()));
   focusAreaLineEdit = new QLineEdit;
   //focusAreaLineEdit->setMinimumWidth(minWidth);
   
   //
   // geography label and line edit
   //
   QPushButton* focusGeographyPushButton = new QPushButton("Geography...");
   focusGeographyPushButton->setAutoDefault(false);
   QObject::connect(focusGeographyPushButton, SIGNAL(clicked()),
                    this, SLOT(slotGeographyPushButton()));
   focusGeographyLineEdit = new QLineEdit;
   //focusGeographyLineEdit->setMinimumWidth(minWidth);
   
   //
   // size label and line edit
   //
   QLabel* focusSizeLabel = new QLabel("Extent");
   focusSizeDoubleSpinBox = new QDoubleSpinBox;
   focusSizeDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   focusSizeDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   focusSizeDoubleSpinBox->setDecimals(3);
   focusSizeDoubleSpinBox->setSingleStep(1.0);
   focusSizeDoubleSpinBox->setFixedWidth(coordWidth);
   
   //
   // statistic label and line edit
   //
   QLabel* focusStatisticLabel = new QLabel("Statistic");
   focusStatisticLineEdit = new QLineEdit;
   //focusStatisticLineEdit->setMinimumWidth(minWidth);
   
   //
   // comment label and text edit
   //
   QLabel* focusCommentLabel = new QLabel("Comment");
   focusCommentTextEdit = new QTextEdit;
   focusCommentTextEdit->setMinimumWidth(minWidth);
   focusCommentTextEdit->setMaximumHeight(60);
   //
   // Vertical Group Box for data entry
   //
   QGroupBox* dataGroup = new QGroupBox("Focus Data");
   fociPageLayout->addWidget(dataGroup);
   int numRows = 0;
   const int NAME_ROW        = numRows++;
   const int CLASS_ROW       = numRows++;
   const int COORD_ROW       = numRows++;
   const int STUDY_ROW       = numRows++;
   const int STUDY_METADATA_ROW = numRows++;
   const int AREA_GEOGRAPHY_ROW        = numRows++;
   const int SIZE_STATISTIC_ROW        = numRows++;
   const int COMMENT_ROW   = numRows++;
   //const int COMMENT_ROW_2   = numRows++;
   //const int COMMENT_ROW_3   = numRows++;

   //
   // Grid Layout for Name and Resampling
   //
   QGridLayout* focusGrid = new QGridLayout(dataGroup);
   focusGrid->addWidget(focusNameButton, NAME_ROW, 0, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusNameLineEdit, NAME_ROW, 1, 1, 3, Qt::AlignLeft);
   focusGrid->addWidget(focusClassNameButton, CLASS_ROW, 0, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusClassNameLineEdit, CLASS_ROW, 1, 1, 3, Qt::AlignLeft);
   focusGrid->addWidget(focusCoordLabel, COORD_ROW, 0, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusXCoordDoubleSpinBox, COORD_ROW, 1, 1, 1, Qt::AlignLeft);
   focusGrid->addWidget(focusYCoordDoubleSpinBox, COORD_ROW, 2, 1, 1, Qt::AlignLeft); 
   focusGrid->addWidget(focusZCoordDoubleSpinBox, COORD_ROW, 3, 1, 1, Qt::AlignLeft);
   focusGrid->addWidget(focusStudyLabel, STUDY_ROW, 0, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusStudyComboBox, STUDY_ROW, 1, 1, 3, Qt::AlignLeft);
   focusGrid->addWidget(focusStudyMetaDataPushButton, STUDY_METADATA_ROW, 0, 1, 1, Qt::AlignLeft);
   focusGrid->addWidget(focusStudyMetaDataLineEdit, STUDY_METADATA_ROW, 1, 1, 3, Qt::AlignLeft);
   focusGrid->addWidget(focusAreaPushButton, AREA_GEOGRAPHY_ROW, 0, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusAreaLineEdit, AREA_GEOGRAPHY_ROW, 1, 1, 1, Qt::AlignLeft);
   focusGrid->addWidget(focusGeographyPushButton, AREA_GEOGRAPHY_ROW, 2, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusGeographyLineEdit, AREA_GEOGRAPHY_ROW, 3, 1, 1, Qt::AlignLeft);
   focusGrid->addWidget(focusSizeLabel, SIZE_STATISTIC_ROW, 0, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusSizeDoubleSpinBox, SIZE_STATISTIC_ROW, 1, 1, 1, Qt::AlignLeft);
   focusGrid->addWidget(focusStatisticLabel, SIZE_STATISTIC_ROW, 2, 1, 1, Qt::AlignRight);
   focusGrid->addWidget(focusStatisticLineEdit, SIZE_STATISTIC_ROW, 3, 1, 1, Qt::AlignLeft);
   focusGrid->addWidget(focusCommentLabel, COMMENT_ROW, 0, Qt::AlignRight | Qt::AlignVCenter);
   focusGrid->addWidget(focusCommentTextEdit, COMMENT_ROW, 1, 1, 3);   
   
}

/**
 * called when study meta data button pressed.
 */
void 
GuiMapStereotaxicFocusDialog::slotFocusStudyMetaDataPushButton()
{
   GuiStudyMetaDataLinkCreationDialog smdlcd(this);
   StudyMetaDataLinkSet smdls;
   smdls.setLinkSetFromCodedText(focusStudyMetaDataLineEdit->text());
   smdlcd.initializeSelectedLinkSet(smdls);
   if (smdlcd.exec() == GuiStudyMetaDataLinkCreationDialog::Accepted) {
      focusStudyMetaDataLineEdit->setText(smdlcd.getLinkSetCreated().getLinkSetAsCodedText());
      focusStudyMetaDataLineEdit->home(false);
   }
}      

/**
 * called when a mode button is selected.
 */
void 
GuiMapStereotaxicFocusDialog::slotModeButtonSelected()
{
   const bool enableThem = editRadioButton->isChecked();
   
   editFocusNumberSpinBox->setEnabled(enableThem);
   loadFocusPushButton->setEnabled(enableThem);
   deleteFocusPushButton->setEnabled(enableThem);
   
   if (enableThem) {
      loadFocusSlot();
   }
}

/**
 * called when cortical area button selected.
 */
void 
GuiMapStereotaxicFocusDialog::slotCorticalAreaPushButton()
{
   GuiNameSelectionDialog nsd(this,
                              (GuiNameSelectionDialog::LIST_AREA_COLORS_ALPHA |
                               GuiNameSelectionDialog::LIST_PAINT_NAMES_ALPHA),
                              GuiNameSelectionDialog::LIST_PAINT_NAMES_ALPHA);
   if (nsd.exec() == QDialog::Accepted) {
      const QString name(nsd.getName());
      if (name.isEmpty() == false) {
         focusAreaLineEdit->setText(name);
      }
   }
}

/**
 * called when Geography button selected.
 */
void 
GuiMapStereotaxicFocusDialog::slotGeographyPushButton()
{
   GuiNameSelectionDialog nsd(this,
                              (GuiNameSelectionDialog::LIST_AREA_COLORS_ALPHA |
                               GuiNameSelectionDialog::LIST_PAINT_NAMES_ALPHA),
                              GuiNameSelectionDialog::LIST_PAINT_NAMES_ALPHA);
   if (nsd.exec() == QDialog::Accepted) {
      const QString name(nsd.getName());
      if (name.isEmpty() == false) {
         focusGeographyLineEdit->setText(name);
      }
   }
}

/**
 * load a focus into the dialog
 */
void 
GuiMapStereotaxicFocusDialog::loadFocusSlot()
{
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   CellProjection dummy;
   CellProjection* cd = &dummy;
   
   if (ff != NULL) {
      const int focusNumber = editFocusNumberSpinBox->value();
      if ((focusNumber >= 0) && (focusNumber < ff->getNumberOfCellProjections())) {
         QString name, className;
         cd = ff->getCellProjection(focusNumber);
      }
      else {
         QMessageBox::critical(this, "Focus Error", "Invalid focus number entered.");
         return;
      }
   }
   
   //
   // Load the focus data
   //
   float xyz[3];
   cd->getXYZ(xyz);
   focusNameLineEdit->setText(cd->getName());
   focusClassNameLineEdit->setText(cd->getClassName());
   focusXCoordDoubleSpinBox->setValue(xyz[0]);
   focusYCoordDoubleSpinBox->setValue(xyz[1]);
   focusZCoordDoubleSpinBox->setValue(xyz[2]);
   focusAreaLineEdit->setText(cd->getArea());
   focusGeographyLineEdit->setText(cd->getGeography());
   focusSizeDoubleSpinBox->setValue(cd->getSize());
   focusStatisticLineEdit->setText(cd->getStatistic());
   focusCommentTextEdit->setPlainText(cd->getComment());
   focusStudyMetaDataLineEdit->setText(cd->getStudyMetaDataLinkSet().getLinkSetAsCodedText());
   focusStudyMetaDataLineEdit->home(false);
   int studyNumber = cd->getStudyNumber();
   if ((studyNumber < 0) ||
       (studyNumber >= ff->getNumberOfStudyInfo())) {
      studyNumber = ff->getNumberOfStudyInfo();
   }
   focusStudyComboBox->setCurrentIndex(studyNumber);
}

/**
 * load a focus into the dialog
 */
void 
GuiMapStereotaxicFocusDialog::deleteFocusSlot()
{
   if (QMessageBox::question(this,
                               "Confirm",
                               "Delete the selected focus?",
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::No) {
      return;
   }
   
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   
   if (ff != NULL) {
      int focusNumber = editFocusNumberSpinBox->value();
      if ((focusNumber >= 0) && (focusNumber < ff->getNumberOfCellProjections())) {
         ff->deleteCellProjection(focusNumber);
         if (focusNumber >= ff->getNumberOfCellProjections()) {
            focusNumber--;
         }
         if (focusNumber < 0) {
            focusNumber = 0;
         }

         GuiFilesModified fm;
         fm.setFociModified();
         theMainWindow->fileModificationUpdate(fm);
         ff->setModified();

         editFocusNumberSpinBox->setValue(focusNumber);
         loadFocusSlot();
      }
      else {
         QMessageBox::critical(this, "Focus Error", "Invalid focus number entered.");
         return;
      }
   }
}

/**
 * Edit the focus.
 */
void
GuiMapStereotaxicFocusDialog::editFocus(const int focusNumber)
{
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   
   if (ff != NULL) {
      if ((focusNumber >= 0) && (focusNumber < ff->getNumberOfCellProjections())) {
         editFocusNumberSpinBox->setValue(focusNumber);
         loadFocusSlot();
      }
   }
}

/**
 * called when the focus number is changed
 */
void 
GuiMapStereotaxicFocusDialog::editFocusNumberChanged()
{
   editRadioButton->setChecked(true);
}

/**
 * Called when apply button pressed.
 */
void
GuiMapStereotaxicFocusDialog::applyButtonSlot()
{
   if (tabWidget->currentWidget() == fociPageVBox) {
      processFocusEntry();
   }
   else {
      studyInfoEditorWidget->slotAcceptEditorContents();
      slotUpdateStudyNumberComboBox();
   }
}

/**
 * process the entry of study information.
 */
void 
GuiMapStereotaxicFocusDialog::processFocusEntry()
{
   //
   // Check automatic projection to surface
   //
   bool projectToMainWindowSurface = false;
   bool projectToPalsSurfaces = false;
   if (automaticProjectionGroupBox->isChecked()) {
      projectToMainWindowSurface = projectionMainWindowFiducialRadioButton->isChecked();
      projectToPalsSurfaces = ((projectionPalsAtlasRadioButton->isChecked()) &&
                               (projectionPalsAtlasRadioButton->isEnabled()));
                                       
      if ((projectToMainWindowSurface == false) && 
          (projectToPalsSurfaces == false)) {
         QMessageBox::critical(this, "ERROR", "You must select a Projection Surface or \n"
                                                "deselect Automatic Projection of Foci.");
         return;
      }
   }

   
   //
   // See if a fiducial surface is in the main window
   //
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   BrainModelSurface* bms = openGL->getDisplayedBrainModelSurface();
   
   if (projectToMainWindowSurface) {
      if (bms == NULL) {
         QMessageBox::critical(this, "ERROR", "There is no surface in the main window.");
         return;
      }
   }
   if (projectToPalsSurfaces) {
      if (theMainWindow->getBrainSet()->getNumberOfNodes() != 73730) {
         QMessageBox::critical(this, "ERROR", "Cannot project to PALS since no PALS compatible surface is loaded.");
         return;
      }
   }
      
   QString msg;
   const QString nameValue = focusNameLineEdit->text();
   if (nameValue.isEmpty()) {
      msg.append("You must enter a foci name.\n");
   }
   
   const QString classNameValue = focusClassNameLineEdit->text();
   if (classNameValue.isEmpty()) {
      //msg.append("You must enter a class name.\n");
   }
   
   const float x = focusXCoordDoubleSpinBox->value();
   const float y = focusYCoordDoubleSpinBox->value();
   const float z = focusZCoordDoubleSpinBox->value();
   if ((x == 0.0) && (y == 0.0) && (z == 0.0)) {
      msg.append("Focus coordinates cannot be all zeros.\n");
   }
   
   if (msg.isEmpty() == false) {
      QMessageBox::critical(this, "Data Entry Error", msg);
      return;
   }

   //
   // Replace any spaces in foci name with underscores
   //
   QString fociName(nameValue);
   //StringUtilities::replace(fociName, ' ', '_');
   
   //
   // Replace any spaces in foci class name with underscores
   //
   QString fociClassName(classNameValue);
   //StringUtilities::replace(fociClassName, ' ', '_');
   
   //
   // Find the matching color
   //
   bool colorMatch = false;
   FociColorFile* colorFile = theMainWindow->getBrainSet()->getFociColorFile();
   int fociColorIndex = colorFile->getColorIndexByName(fociName, colorMatch);
   
   //
   // Foci color may need to be created
   //
   bool createColor = false;
   if ((fociColorIndex >= 0) && (colorMatch == true)) {
      createColor = false;
   }
   else if ((fociColorIndex >= 0) && (colorMatch == false)) {
      QString msg("Use foci color \"");
      msg.append(colorFile->getColorNameByIndex(fociColorIndex));
      msg.append("\" for foci ");
      msg.append(fociName);
      msg.append(" ?");
      const QString noButtonText("No, define color " + fociName);
      QMessageBox msgBox(this);
      msgBox.setWindowTitle("Use Partially Matching Color");
      msgBox.addButton("Yes", QMessageBox::YesRole);
      QPushButton* noPushButton = msgBox.addButton(noButtonText, QMessageBox::NoRole);
      msgBox.exec();
      if (msgBox.clickedButton() == noPushButton) {
         createColor = true;
      }
   }
   else {
      createColor = true;
   }
   
   if (createColor) {
      QString title("Create Foci Color ");
      title.append(fociName);
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this, 
                                                                 title, 
                                                                 true, 
                                                                 true, 
                                                                 false, 
                                                                 true);
      csd->exec();
      
      //
      // Add new foci color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      colorFile->addColor(fociName, r, g, b, a, pointSize, lineSize, symbol);
      fociColorIndex = colorFile->getNumberOfColors() - 1;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Get the foci file
   //   
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   
   //
   // Get the study number
   //
   int studyNumber = focusStudyComboBox->currentIndex();
   if (focusStudyComboBox->currentText() == noneStudyName) {
      studyNumber = -1;
   }
   else if (studyNumber >= ff->getNumberOfStudyInfo()) {
      studyNumber = -1;
   }
   
   int focusNumber = -1;
   
   StudyMetaDataLinkSet studyMetaDataLinkSet;
   studyMetaDataLinkSet.setLinkSetFromCodedText(focusStudyMetaDataLineEdit->text());
   if (newRadioButton->isChecked()) {
      //
      // Create the foci
      //
      CellProjection cd;
      const float xyz[3] = { x, y, z };
      cd.setXYZ(xyz);
      cd.setName(fociName);
      cd.setClassName(fociClassName);
      cd.setStudyNumber(studyNumber);
      cd.setColorIndex(fociColorIndex);
      cd.setComment(focusCommentTextEdit->toPlainText());
      cd.setArea(focusAreaLineEdit->text());
      cd.setGeography(focusGeographyLineEdit->text());
      cd.setSize(focusSizeDoubleSpinBox->text().toFloat());
      cd.setStatistic(focusStatisticLineEdit->text());
      cd.setStudyMetaDataLinkSet(studyMetaDataLinkSet);
      ff->addCellProjection(cd);
      focusNumber = ff->getNumberOfCellProjections() - 1;
   }
   else {
      focusNumber = editFocusNumberSpinBox->value();
      if ((focusNumber >= 0) && (focusNumber < ff->getNumberOfCellProjections())) {
         CellProjection* cd = ff->getCellProjection(focusNumber); 
         cd->setName(fociName);
         const float xyz[3] = { x, y, z };
         cd->setXYZ(xyz);
         cd->setClassName(fociClassName);
         cd->setStudyNumber(studyNumber);
         cd->setComment(focusCommentTextEdit->toPlainText());
         cd->setArea(focusAreaLineEdit->text());
         cd->setGeography(focusGeographyLineEdit->text());
         cd->setSize(focusSizeDoubleSpinBox->value());
         cd->setStatistic(focusStatisticLineEdit->text());
         cd->setStudyMetaDataLinkSet(studyMetaDataLinkSet);
      }
      else {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Focus Number Invalid", 
                               "The focus number entered is invalid");
         return;
      }
   }
   
   if (projectToMainWindowSurface) {
      //
      // Create the cell file projector (foci same as cells)
      //
      CellFileProjector projector(bms);
      
      //
      // Project the new foci
      //
      projector.projectFile(theMainWindow->getBrainSet()->getFociProjectionFile(),
                                focusNumber, 
                                CellFileProjector::PROJECTION_TYPE_FLIP_TO_MATCH_HEMISPHERE,
                                0.0,
                                false,  
                                NULL);
   }
   else if (projectToPalsSurfaces) {
      //
      // Project to PALS atlas
      //
      if (palsProjector == NULL) {
         palsProjector = new FociFileToPalsProjector(theMainWindow->getBrainSet(),
                                                     ff,
                                                     focusNumber,
                                                     0.0,
                                                     false);
      }
      palsProjector->setFirstFocusIndex(focusNumber);
      try {
         palsProjector->execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }
   
   DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
   dsf->setDisplayCells(true);
   
   GuiFilesModified fm;
   fm.setFociModified();
   fm.setFociColorModified();
   theMainWindow->fileModificationUpdate(fm);
   ff->setModified();
   
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
}

/**
 * Update the dialog (typically called when foci file changed).
 */
void
GuiMapStereotaxicFocusDialog::updateDialog()
{
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();   
   
   //
   // Enable projection surface options
   //
   projectionPalsAtlasRadioButton->setEnabled(false);
   const BrainModelSurface* bms = theMainWindow->getBrainSet()->getActiveFiducialSurface();
   if (bms != NULL) {
      projectionMainWindowFiducialRadioButton->setEnabled(true);
      const bool palsValid = (bms->getNumberOfNodes() == 73730);
      projectionPalsAtlasRadioButton->setEnabled(palsValid);
   }

   studyInfoEditorWidget->updateWidget(ff->getPointerToStudyInfo());
   slotUpdateStudyNumberComboBox();
}

/**
 * update the study number combo box.
 */
void 
GuiMapStereotaxicFocusDialog::slotUpdateStudyNumberComboBox()
{
   const bool noneFlag = (focusStudyComboBox->currentText() == noneStudyName);
   const int oldIndx = focusStudyComboBox->currentIndex();
   
   focusStudyComboBox->clear();
   
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   const int numStudys = fpf->getNumberOfStudyInfo();
   for (int i = 0; i < numStudys; i++) {
      const CellStudyInfo* csi = fpf->getStudyInfo(i);
      QString s(csi->getTitle());
      if (s.length() > 30) {
         s.resize(30);
      }
      QString qs(QString::number(i));
      qs.append(" - ");
      qs.append(s);
      focusStudyComboBox->addItem(qs);
   }
   
   const int noneIndex = focusStudyComboBox->count();
   focusStudyComboBox->addItem(noneStudyName);
   
   if (noneFlag) {
      focusStudyComboBox->setCurrentIndex(noneIndex);
   }
   else if ((oldIndx >= 0) && (oldIndx < numStudys)) {
      focusStudyComboBox->setCurrentIndex(oldIndx);
   }
   else {
      focusStudyComboBox->setCurrentIndex(noneIndex);
   }
}      

/**
 * Called when Focus Name button is pressed.
 */
void
GuiMapStereotaxicFocusDialog::nameButtonSlot()
{
   GuiNameSelectionDialog nsd(this,
                              GuiNameSelectionDialog::LIST_ALL,
                              GuiNameSelectionDialog::LIST_FOCI_NAMES_ALPHA);
   if (nsd.exec() == QDialog::Accepted) {
      const QString name(nsd.getName());
      if (name.isEmpty() == false) {
         focusNameLineEdit->setText(name);
      }
   }
}

/**
 * Called when Focus ClassName button is pressed.
 */
void
GuiMapStereotaxicFocusDialog::classNameButtonSlot()
{
   FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
   if (ff != NULL) {
      const int numClasses = ff->getNumberOfCellClasses();
      std::vector<QString> classNames;
      for (int i = 0; i < numClasses; i++) {
         classNames.push_back(ff->getCellClassNameByIndex(i));
      }
      
      if (classNames.size() > 0) {
         QtListBoxSelectionDialog lbsd(this, "Foci Classes", "", classNames, 0);
         if (lbsd.exec()) {
            focusClassNameLineEdit->setText(lbsd.getSelectedText());
         }
      }
      else {
         QMessageBox::information(this, "No Classes", "Cell File has no classes");   
      }
   }
   else {
      QMessageBox::information(this, "No Classes", "Cell File has no classes");   
   }   
}

