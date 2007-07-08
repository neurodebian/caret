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

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolTip>

#include "BrainModelVolume.h"
#include "BrainModelVolumeBiasCorrection.h"
#include "BrainSet.h"
#include "Categories.h"
#include "DebugControl.h"
#include "DisplaySettingsVolume.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFileSelectionButton.h"
#include "GuiGraphWidget.h"
#include "GuiHistogramDisplayDialog.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiOpenDataFileDialog.h"
#include "GuiToolBar.h"
#include "GuiVolumeFileOrientationComboBox.h"
#include "GuiVolumeMultiHemSureFitSegmentationDialog.h"
#include "StatisticHistogram.h"
#include "ParamsFile.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "Species.h"
#include "StringUtilities.h"
#include "Structure.h"
#include "SystemUtilities.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiVolumeMultiHemSureFitSegmentationDialog::GuiVolumeMultiHemSureFitSegmentationDialog(QWidget* parent)
   : QtDialog(parent)
{
   checkForExternalPrograms();
   
   imageDirectory = theMainWindow->getBrainSet()->getCaretHomeDirectory()
                  + "/data_files/segmentation/images/";
   
   setWindowTitle("SureFit Multi-Hemisphere Segmentation");
   nextPushButton = NULL;
   startPushButton = NULL;
   prevPushButton = NULL;
   
   resetDialogSelections();
   
   //
   // The help text browswer
   //
   helpBrowser = new QTextBrowser;
   QGroupBox* helpGroupBox = new QGroupBox("Help");
   QVBoxLayout* helpLayout = new QVBoxLayout(helpGroupBox);
   helpLayout->addWidget(helpBrowser);
/*
   helpBrowser = new QTextBrowser;
   QScrollArea* helpScrollArea = new QScrollArea;
   helpScrollArea->setWidget(helpBrowser);
   QGroupBox* helpGroupBox = new QGroupBox("Help");
   QVBoxLayout* helpLayout = new QVBoxLayout(helpGroupBox);
   helpLayout->addWidget(helpScrollArea);
*/      
   //
   // Create the widget stack for the pages
   //
   pageStackedWidget = new QStackedWidget;
   
   //
   // Create the pages
   //
   createAllInstructionsPage();
   createAllUnsavedFilesPage();
   createAllVolumeFileTypePage();
   createAllTaskPage();
   createSegmentationChooseFilesPage();
   createAnatomyChooseFilePage();
   createAnatomyMriTypePage();
   createAnatomyStandardSpacePage();
   createAnatomyOrientationPage();
   createAnatomyVoxelSizePage();
   createAnatomyOriginPage();
   createAnatomyVoxelValuesPage();
   createAnatomyUniformityPage();
   createAnatomyCropMaskPage();
   createAnatomyStripPage();
   createAnatomyChooseSpecPage();
   createAnatomySubjectInformationPage();
   createAnatomyCreateSpecPage();
   createAnatomyHistogramPage();
   createSegmentationOperationsPage();
   createAllSurfaceOperationsPage();
   createAllRunPage();
   createAllProgressPage();
   
   //
   // Prev pushbutton
   //
   prevPushButton = new QPushButton("Prev");
   prevPushButton->setAutoDefault(false);
   QObject::connect(prevPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPrevPushButton()));
                    
   //
   // Next pushbutton
   //
   nextPushButton = new QPushButton("Next");
   nextPushButton->setAutoDefault(false);
   QObject::connect(nextPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNextPushButton()));
                    
   //
   // Start pushbutton
   //
   startPushButton = new QPushButton("Start");
   startPushButton->setAutoDefault(false);
   QObject::connect(startPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStartPushButton()));
                    
   
   //
   // Close pushbutton
   //
   closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(slotClosePushButton()));
                    
   //
   // Make all of the buttons the same size
   //
   QtUtilities::makeButtonsSameSize(prevPushButton,
                                    nextPushButton,
                                    closePushButton,
                                    startPushButton);
                                    
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   buttonsLayout->addWidget(prevPushButton);
   buttonsLayout->addWidget(nextPushButton);
   buttonsLayout->addWidget(startPushButton);
   buttonsLayout->addWidget(closePushButton);
   
   //
   // Splitter of page widgets and help browser
   //
   pageSplitter = new QSplitter;
   pageSplitter->setOrientation(Qt::Vertical);
   pageSplitter->addWidget(pageStackedWidget);
   pageSplitter->addWidget(helpGroupBox);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(pageSplitter);
   dialogLayout->addLayout(buttonsLayout);
   
   showPage(0);
}

/**
 * destructor.
 */
GuiVolumeMultiHemSureFitSegmentationDialog::~GuiVolumeMultiHemSureFitSegmentationDialog()
{
}

/**
 * Reset the dialog.
 */
void
GuiVolumeMultiHemSureFitSegmentationDialog::resetDialogSelections()
{
   anatomyVolume = NULL;
   leftSegmentationVolume = NULL;
   rightSegmentationVolume = NULL;
   cerebellumSegmentationVolume = NULL;
   leftBrainSet = NULL;
   rightBrainSet = NULL;
   cerebellumBrainSet = NULL;
   anatomicalMriType = ANATOMICAL_MRI_TYPE_UNKNOWN;   
}

/**
 * create the instructions page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAllInstructionsPage()
{
   QString s =
      "The SureFit segmentation process involves a sequence of pages.  The top part of the "
      "window (which contains this text) contains controls for performing the operations.  The "
      "bottom part of the windows contains help information for the current page.  In between "
      "operations and help sections is a control that can enlarge one of the sections (at the "
      "expense of the other).\n"
      "Press Next to continue.";
   StringUtilities::lineWrapString(60, s);
      
   pageAllInstructions = new Page("Instructions");
   QVBoxLayout* layout = pageAllInstructions->getLayout();
   
   layout->addWidget(new QLabel(s));
   pageStackedWidget->addWidget(pageAllInstructions);
   
   QString s2 =
      "This <B>Help</B> section will contain information regarding the purpose of and how "
      "to use the current page in the sequence.";
   pageAllInstructions->setHelp(s2);
}
      
/**
 * create the unsaved files page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAllUnsavedFilesPage()
{
   pageAllUnsavedFiles = new Page("Check for Unsaved Files");
   QVBoxLayout* layout = pageAllUnsavedFiles->getLayout();
   
   unsavedFilesTextEdit = new QTextEdit;
   unsavedFilesTextEdit->setReadOnly(true);
   layout->addWidget(unsavedFilesTextEdit);
   
   pageStackedWidget->addWidget(pageAllUnsavedFiles);
   
   QString s2 = 
      "If there are any unsaved files, the changes made to the files will be lost "
      "if you choose to continue";
   pageAllUnsavedFiles->setHelp(s2);
}
      
/**
 * update the unsaved files page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateAllUnsavedFilesPage()
{
   //
   // See if files are modified
   //
   std::vector<BrainSet*> brainSets;
   theMainWindow->getAllBrainSets(brainSets);
   QString msg;
   QString msg2;
   for (unsigned int i = 0; i < brainSets.size(); i++) {
      theMainWindow->checkForModifiedFiles(brainSets[i], msg2, true);
   }
   if (msg2.isEmpty()) {
      msg = "There are no unsaved files.  Press \"Next\" to continue.";
   }
   else {
      msg = "There are unsaved files whose changes will be lost if you continue "
            "without saving the files.\n";
      msg.append(msg2);
   }
   unsavedFilesTextEdit->setPlainText(msg);
}

/**
 * create the task page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAllTaskPage()
{
   pageAllTask = new Page("Choose Task");
   QVBoxLayout* layout = pageAllTask->getLayout();
   
   //
   // Anatomy structure choices
   //
   taskAnatomyProcessLeftCheckBox = new QCheckBox("Left Cerebral Cortex");
   taskAnatomyProcessRightCheckBox = new QCheckBox("Right Cerebral Cortex");
   taskAnatomyProcessCerebellumCheckBox = new QCheckBox("Cerebellum");
   QGridLayout* anatomyGridLayout = new QGridLayout;
   anatomyGridLayout->addWidget(taskAnatomyProcessLeftCheckBox, 0, 1);
   anatomyGridLayout->addWidget(taskAnatomyProcessRightCheckBox, 1, 1);
   anatomyGridLayout->addWidget(taskAnatomyProcessCerebellumCheckBox, 2, 1);
   anatomyGridLayout->setColumnMinimumWidth(0, 25);
   anatomyGridLayout->setColumnStretch(0, 0);
   anatomyGridLayout->setColumnStretch(1, 0);
     
   //
   // Button group to checkboxes read
   //
   QButtonGroup* checkButtonGroup = new QButtonGroup(this);
   checkButtonGroup->setExclusive(false);
   QObject::connect(checkButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisableButtons()));
   checkButtonGroup->addButton(taskAnatomyProcessLeftCheckBox, 0);
   checkButtonGroup->addButton(taskAnatomyProcessRightCheckBox, 1);
   checkButtonGroup->addButton(taskAnatomyProcessCerebellumCheckBox, 2);
   
   //
   // Task selection anatomy/segmentation
   //
   taskAnatomicalVolumeRadioButton = new QRadioButton("Process Anatomical Volume");
   taskSegmentationVolumeRadioButton = new QRadioButton("Process Segmentation Volume(s)");
   layout->addWidget(taskAnatomicalVolumeRadioButton);
   layout->addLayout(anatomyGridLayout, 0);
   layout->addWidget(taskSegmentationVolumeRadioButton);
   layout->addStretch();
   
   //
   // keep anatomy/task selections mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisableButtons()));
   buttGroup->addButton(taskAnatomicalVolumeRadioButton);
   buttGroup->addButton(taskSegmentationVolumeRadioButton);
   
   pageStackedWidget->addWidget(pageAllTask);
   
   QString s2 = 
      "Choose \"Process Anatomical Volume\" if you have an anatomical volume that you "
      "would like to segment<P>"
      "Choose \"Process Segmentation Volume(s)\" if you have one or more segmentation volumes "
      "(genreated from a single anatomical volume) on which you want to complete processing.";
   pageAllTask->setHelp(s2);
}

/**
 * update the task page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateSegmentationChooseFilesPage()
{
   leftSegmentationLineEdit->setText("");
   rightSegmentationLineEdit->setText("");
   cerebellumSegmentationLineEdit->setText("");
   
   
   QString errorMessage;
   
   
   //
   // Loop through loaded brain sets and find any that are already loaded
   //
   int leftCount = 0;
   int rightCount = 0;
   int cerebellumCount = 0;
   bool leftFound = false;
   bool rightFound = false;
   bool cerebellumFound = false;
   for (int i = 0; i < theMainWindow->getNumberOfBrainSets(); i++) {
      //
      // Get the brain set and its structure
      //
      BrainSet* bs = theMainWindow->getBrainSetByIndex(i);
      const Structure structure = bs->getStructure();
      
      switch (structure.getType()) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
            if (bs == leftBrainSet) {
               BrainModelVolume* bmv = leftBrainSet->getBrainModelVolume();
               if (bmv != NULL) {
                  if (leftSegmentationVolume == bmv->getSelectedVolumeSegmentationFile()) {
                     if (leftSegmentationVolume != NULL) {
                        leftFound = true;
                     }
                  }
               }
            }
            leftCount++;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
            if (bs == rightBrainSet) {
               BrainModelVolume* bmv = rightBrainSet->getBrainModelVolume();
               if (bmv != NULL) {
                  if (rightSegmentationVolume == bmv->getSelectedVolumeSegmentationFile()) {
                     if (rightSegmentationVolume != NULL) {
                        rightFound = true;
                     }
                  }
               }
            }
            rightCount++;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
            break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
            if (bs == cerebellumBrainSet) {
               BrainModelVolume* bmv = cerebellumBrainSet->getBrainModelVolume();
               if (bmv != NULL) {
                  if (cerebellumSegmentationVolume == bmv->getSelectedVolumeSegmentationFile()) {
                     if (cerebellumSegmentationVolume != NULL) {
                        cerebellumFound = true;
                     }
                  }
               }
            }
            cerebellumCount++;
            break;
         case Structure::STRUCTURE_TYPE_INVALID:
            break;
      }
   }
   
   if (leftCount > 1) {
      errorMessage.append("There is more than one left spec file loaded, close all but one\n"
                          "of the left spec files.");
   }
   if (rightCount > 1) {
      errorMessage.append("There is more than one right spec file loaded, close all but one\n"
                          "of the right spec files.");
   }
   if (cerebellumCount > 1) {
      errorMessage.append("There is more than one cerebellum spec file loaded, close all but one\n"
                          "of the cerebellum spec files.");
   }
   if (errorMessage.isEmpty() == false) {
      GuiMessageBox::critical(this, "ERROR", errorMessage, "OK");
      return;
   }
   
   if (leftCount == 0) {
      leftSegmentationVolume = NULL;
      leftBrainSet = NULL;
   }
   if (rightCount == 0) {
      rightSegmentationVolume = NULL;
      rightBrainSet = NULL;
   }
   if (cerebellumCount == 0) {
      cerebellumSegmentationVolume = NULL;
      cerebellumBrainSet = NULL;
   }
   
   //
   // Get the selected segmentation volumes
   //
   if (leftBrainSet != NULL) {
      BrainModelVolume* bmv = leftBrainSet->getBrainModelVolume();
      if (bmv != NULL) {
         leftSegmentationLineEdit->setText(leftSegmentationVolume->getFileName());
      }
   }
   if (rightBrainSet != NULL) {
      BrainModelVolume* bmv = rightBrainSet->getBrainModelVolume();
      if (bmv != NULL) {
         rightSegmentationLineEdit->setText(rightSegmentationVolume->getFileName());
      }
   }
   if (cerebellumBrainSet != NULL) {
      BrainModelVolume* bmv = cerebellumBrainSet->getBrainModelVolume();
      if (bmv != NULL) {
         cerebellumSegmentationLineEdit->setText(cerebellumSegmentationVolume->getFileName());
      }
   }
}

/**      
 * create the segmentation page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createSegmentationChooseFilesPage()
{
   pageSegmentationChooseFiles = new Page("Segmentation Volume Verification");
   
   //
   // Button to select left segmentation volume
   //
   QPushButton* leftSegmentationSelectionButton =  new QPushButton("Left...");
   QObject::connect(leftSegmentationSelectionButton, SIGNAL(clicked()),
                    this, SLOT(slotLeftSegmentationVolumePushButton()));

   //
   // Button to select left segmentation volume
   //
   QPushButton* rightSegmentationSelectionButton =  new QPushButton("Right...");
   QObject::connect(rightSegmentationSelectionButton, SIGNAL(clicked()),
                    this, SLOT(slotRightSegmentationVolumePushButton()));

   //
   // Button to select cerebellum segmentation volume
   //
   QPushButton* cerebellumSegmentationSelectionButton =  new QPushButton("Cerebellum...");
   QObject::connect(cerebellumSegmentationSelectionButton, SIGNAL(clicked()),
                    this, SLOT(slotCerebellumSegmentationVolumePushButton()));

   leftSegmentationLineEdit = new QLineEdit;
   leftSegmentationLineEdit->setReadOnly(true);
   rightSegmentationLineEdit = new QLineEdit;
   rightSegmentationLineEdit->setReadOnly(true);
   cerebellumSegmentationLineEdit = new QLineEdit;
   cerebellumSegmentationLineEdit->setReadOnly(true);
   
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->setSpacing(5);
   gridLayout->addWidget(leftSegmentationSelectionButton, 0, 0);
   gridLayout->addWidget(leftSegmentationLineEdit, 0, 1);
   gridLayout->addWidget(rightSegmentationSelectionButton, 1, 0);
   gridLayout->addWidget(rightSegmentationLineEdit, 1, 1);
   gridLayout->addWidget(cerebellumSegmentationSelectionButton, 2, 0);
   gridLayout->addWidget(cerebellumSegmentationLineEdit, 2, 1);
   
   QVBoxLayout* layout = pageSegmentationChooseFiles->getLayout();
   layout->addLayout(gridLayout);
   layout->addStretch();

   QString s2 = 
      "Currently loaded segmentation volumes are shown above.  If no volumes are shown, "
      "you must load them before you can continue.";
   pageSegmentationChooseFiles->setHelp(s2);

   pageStackedWidget->addWidget(pageSegmentationChooseFiles);
}

/**
 * select left hemisphere segmentation volume.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotLeftSegmentationVolumePushButton()
{
   chooseSegmentationVolumeFromSpecFile(0);
}

/**
 * select right hemisphere segmentation volume.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotRightSegmentationVolumePushButton()
{
   chooseSegmentationVolumeFromSpecFile(1);
}

/**
 * select cerebellum hemisphere segmentation volume.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotCerebellumSegmentationVolumePushButton()
{
   chooseSegmentationVolumeFromSpecFile(2);
}
      
/**
 * choose segmentatinon volume from spec file.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::chooseSegmentationVolumeFromSpecFile(const int indx)
{
   //ggjhhj
}
      
/**      
 * create the anatomy page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyChooseFilePage()
{
   pageAnatomyChooseFile = new Page("Anatomy Volume Selection");
   
   anatomyFileNameLineEdit = new QLineEdit;
   anatomyFileNameLineEdit->setReadOnly(true);
   QPushButton* fileNameSelectionButton = 
      new GuiFileSelectionButton(0,
                                 "Choose Anatomical Volume File...",
                                 GuiDataFileDialog::volumeAnatomyFileFilter,
                                 true);
   fileNameSelectionButton->setFixedSize(fileNameSelectionButton->sizeHint());
   QObject::connect(fileNameSelectionButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotAnatomyVolumeSelected(const QString&)));
   
   QVBoxLayout* layout = pageAnatomyChooseFile->getLayout();
   layout->addWidget(fileNameSelectionButton);
   layout->addWidget(anatomyFileNameLineEdit);
   layout->addStretch();
   
   QString s2 = 
      "Choose the anatomical volume that is to be segmented.  The volume will be displayed "
      "in the main window.  In some cases, it may be necessary to use controls on the "
      "<B>Display Control's Volume - Overlay/Underlay Page</B> to adjust the brightness of the "
      "volume.";
   pageAnatomyChooseFile->setHelp(s2);
   pageStackedWidget->addWidget(pageAnatomyChooseFile);
}

/**
 * called when an anatomy volume is selected.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeSelected(const QString& name)
{
   //
   // Remove all loaded files
   //
   theMainWindow->closeAllSpecFiles();   

   //
   // Clear current file name
   //
   anatomyFileNameLineEdit->setText("");
   anatomyVolume = NULL;
   
   //
   // Open the anatomy volume file
   //
   QString errorMessage;
   bool dummyFlag;
   if (GuiOpenDataFileDialog::openDataFile(this,
                                           SpecFile::volumeAnatomyFileTag,
                                           name,
                                           false,
                                           false,
                                           errorMessage,
                                           dummyFlag)) {
      GuiMessageBox::critical(this, "ERROR", errorMessage, "OK");
      return;
   }
   else {
      anatomyFileNameLineEdit->setText(name);
   }
   
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      GuiMessageBox::critical(this, "PROGRAM ERROR", 
                              "PROGRAM ERROR: there should be a volume in the main window.", "OK");
      return;
   }
   
   anatomyVolume = bmv->getSelectedVolumeAnatomyFile();
   theMainWindow->getBrainSet()->setDisplaySplashImage(false);
   
   //
   // Should current directory be changed ?
   //
   const QString currentPath = QDir::current().absolutePath();
   QFileInfo volumeFileInfo(anatomyVolume->getFileName());
   const QString volumePath = volumeFileInfo.absolutePath();
   if (DebugControl::getDebugOn()) {
      std::cout << "Current path: " << currentPath.toAscii().constData() << std::endl;
      std::cout << "File path:    " << volumePath.toAscii().constData() << std::endl;
   }
   if (currentPath != volumePath) {
      if (GuiMessageBox::warning(this,
                                  "Change Directory",
                                  "The volume selected is not in the current directory.\n"
                                  "Caret is going to set the current directory to the\n"
                                  "directory containing the volume.",
                                  "Continue", "Cancel") == 0) {
         QDir::setCurrent(volumePath);
         anatomyVolume->setFileName(FileUtilities::basename(anatomyVolume->getFileName()));
         anatomyVolume->clearModified();
      }
   }
   
   slotEnableDisableButtons();
}
      
/**      
 * create the volume type page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAllVolumeFileTypePage()
{
   pageAllVolumeFileType = new Page("Volume File Type");
   
   volumeFileTypeAfniRadioButton = new QRadioButton("AFNI (.HEAD)");
   volumeFileTypeNiftiRadioButton = new QRadioButton("NIFTI (.nii, .nii.gz)");
   volumeFileTypeSpmRadioButton = new QRadioButton("SPM (.hdr)");
   volumeFileTypeWuNilRadioButton = new QRadioButton("Washington University NIL (.ifh)");
   
   QButtonGroup* buttonGroup = new QButtonGroup(this);
   buttonGroup->addButton(volumeFileTypeAfniRadioButton);
   buttonGroup->addButton(volumeFileTypeNiftiRadioButton);
   buttonGroup->addButton(volumeFileTypeSpmRadioButton);
   buttonGroup->addButton(volumeFileTypeWuNilRadioButton);
   
   volumeFileWriteCompressedCheckBox = new QCheckBox("Write Compressed Volumes (AFNI and NIFTI only)");
   volumeFileWriteCompressedCheckBox->setChecked(true);
   
   QLabel* typeLabel = new QLabel("Choose type for newly created volumes:");
   QVBoxLayout* layout = pageAllVolumeFileType->getLayout();
   layout->addWidget(typeLabel);
   layout->addWidget(volumeFileTypeAfniRadioButton);
   layout->addWidget(volumeFileTypeNiftiRadioButton);
   layout->addWidget(volumeFileTypeSpmRadioButton);
   layout->addWidget(volumeFileTypeWuNilRadioButton);
   layout->addWidget(new QLabel(" "));
   layout->addWidget(volumeFileWriteCompressedCheckBox);
   layout->addStretch();
   
   pageStackedWidget->addWidget(pageAllVolumeFileType);
   
   QString s2 = 
      "Choose the type of volume file that will be written when volume files are saved. "
      "<B>NIFTI</B> is recommended as it is supported in many neuroscience research "
      "softwares. ";
   pageAllVolumeFileType->setHelp(s2);
}

/**
 * update the volume file type page
 */
void
GuiVolumeMultiHemSureFitSegmentationDialog::updateAllVolumeFileTypePage()
{
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   switch (pf->getPreferredVolumeWriteType()) {
      case VolumeFile::FILE_READ_WRITE_TYPE_RAW:
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_AFNI:
         volumeFileTypeAfniRadioButton->setChecked(true);
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_ANALYZE:
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_NIFTI:
         volumeFileTypeNiftiRadioButton->setChecked(true);
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX:
         volumeFileTypeSpmRadioButton->setChecked(true);
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_WUNIL:
         volumeFileTypeWuNilRadioButton->setChecked(true);
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN:
         break;
   }
}

/**
 * update the volume file type page
 */
void
GuiVolumeMultiHemSureFitSegmentationDialog::readAllVolumeFileTypePage()
{
   VolumeFile::FILE_READ_WRITE_TYPE newWriteType = VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN;
   if (volumeFileTypeAfniRadioButton->isChecked()) {
      newWriteType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
   }
   else if (volumeFileTypeNiftiRadioButton->isChecked()) {
      newWriteType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
   }
   else if (volumeFileTypeSpmRadioButton->isChecked()) {
      newWriteType = VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX;
   }
   else if (volumeFileTypeWuNilRadioButton->isChecked()) {
      newWriteType = VolumeFile::FILE_READ_WRITE_TYPE_WUNIL;
   }
   
   if (newWriteType != VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN) {
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      if (newWriteType != pf->getPreferredVolumeWriteType()) {
         pf->setPreferredVolumeWriteType(newWriteType);
         try {
            pf->writeFile(pf->getFileName());
         }
         catch (FileException&) {
         }
      }
   }
}

/**      
 * create the anatomy choice page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyMriTypePage()
{
   pageAnatomyMriType = new Page("Anatomy MRI Type Selection");
   
   //
   // Type of MRI selection
   //
   anatomyMriTypeT1RadioButton = new QRadioButton("T1");   
   QButtonGroup* buttGroup = new QButtonGroup(this);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotMriTypeSelected()));
   buttGroup->addButton(anatomyMriTypeT1RadioButton);
   QGroupBox* mriTypeGroupBox = new QGroupBox("MRI Type");
   QVBoxLayout* mriTypeGroupLayout = new QVBoxLayout(mriTypeGroupBox);
   mriTypeGroupLayout->addWidget(anatomyMriTypeT1RadioButton);
   
   //
   // Example MRI image
   //
   mriTypeImageLabel = new QLabel("");
   QGroupBox* mriImageGroupBox = new QGroupBox("Example MRI Slice");
   QVBoxLayout* mriImageGroupLayout = new QVBoxLayout(mriImageGroupBox);
   mriImageGroupLayout->addWidget(mriTypeImageLabel);
   
   //
   // Default to T1
   //
   anatomyMriTypeT1RadioButton->setChecked(true);
   slotMriTypeSelected();
   
   //
   // Layout the page
   //
   QVBoxLayout* layout = pageAnatomyMriType->getLayout();
   layout->addWidget(mriTypeGroupBox);
   layout->addWidget(mriImageGroupBox);
   layout->addStretch();
   
   pageStackedWidget->addWidget(pageAnatomyMriType);
   QString s2 = 
      "Choose the type of anatomical MRI that is to be processed. At this time, only"
      "T1 volume are supported for segmentation.  When using a T1 volume, your volume "
      "should be similar to that shown above.  In particular, the white matter should "
      "be brighter than the gray matter and the gray matter should be brighter than "
      "the CSF.  If your volume is not of similar quality, it is likely that Caret "
      "will have difficulty segmentation the volume.";
   pageAnatomyMriType->setHelp(s2);
}

/**
 * called when an MRI type is selected.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotMriTypeSelected()
{   
   QString imageFile(imageDirectory);
   if (anatomyMriTypeT1RadioButton->isChecked()) {
      imageFile += "T1-Volume.jpg";
   }
   
   QPixmap pix;
   if (imageFile != imageDirectory) {
      if (pix.load(imageFile) == false) {
         std::cout << "WARNING: Unable to load image file: " 
                   << imageFile.toAscii().constData() << std::endl;
      }
   }
   mriTypeImageLabel->setPixmap(imageFile);
   
   slotEnableDisableButtons();
}

/**
 * create the anatomy standard space page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyStandardSpacePage()
{
   pageAnatomyStandardSpace = new Page("Anatomy Standard Space");
   //QVBoxLayout* layout = pageAnatomyChoice->getLayout();
   
   pageStackedWidget->addWidget(pageAnatomyStandardSpace);
}

/**      
 * create the spec page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyChooseSpecPage()
{
   pageAnatomyChooseSpec = new Page("Create/Select Spec File(s)");

   const int lineEditWidth = 200;
   
   //
   // Left spec choice
   //
   leftAnatomyCreateSpecFileRadioButton = new QRadioButton("Create Spec File");
   leftAnatomyUseSpecFileRadioButton = new QRadioButton("Use Spec File");
   QButtonGroup* leftAnatomyButtonGroup = new QButtonGroup(this);
   QObject::connect(leftAnatomyButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisableButtons()));
   leftAnatomyButtonGroup->addButton(leftAnatomyCreateSpecFileRadioButton);
   leftAnatomyButtonGroup->addButton(leftAnatomyUseSpecFileRadioButton);
   
   leftAnatomySpecFileLineEdit = new QLineEdit;
   leftAnatomySpecFileLineEdit->setMinimumWidth(lineEditWidth);
   leftAnatomySpecFileLineEdit->setReadOnly(true);
   QPushButton* leftAnatomyChooseSpecPushButton = 
                      new GuiFileSelectionButton(0,
                                                 "Select...",
                                                 GuiDataFileDialog::specFileFilter,
                                                 true);
   QObject::connect(leftAnatomyChooseSpecPushButton, SIGNAL(fileSelected(const QString&)),
                    leftAnatomySpecFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(leftAnatomyChooseSpecPushButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisableButtons()));
   anatomyLeftCreateUseSpecGroupBox = new QGroupBox("Left Hemisphere Spec File");
   QGridLayout* leftGridLayout = new QGridLayout(anatomyLeftCreateUseSpecGroupBox);
   leftGridLayout->addWidget(leftAnatomyCreateSpecFileRadioButton, 0, 0);
   leftGridLayout->addWidget(leftAnatomyUseSpecFileRadioButton, 1, 0);
   leftGridLayout->addWidget(leftAnatomyChooseSpecPushButton, 1, 1);
   leftGridLayout->addWidget(leftAnatomySpecFileLineEdit, 1, 2);
   leftGridLayout->setColumnStretch(0, 0);
   leftGridLayout->setColumnStretch(1, 0);
   leftGridLayout->setColumnStretch(2, 100);
   anatomyLeftCreateUseSpecGroupBox->setFixedHeight(anatomyLeftCreateUseSpecGroupBox->sizeHint().height());
   
   //
   // Right spec choice
   //
   rightAnatomyCreateSpecFileRadioButton = new QRadioButton("Create Spec File");
   rightAnatomyUseSpecFileRadioButton = new QRadioButton("Use Spec File");
   QButtonGroup* rightAnatomyButtonGroup = new QButtonGroup(this);
   QObject::connect(rightAnatomyButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisableButtons()));
   rightAnatomyButtonGroup->addButton(rightAnatomyCreateSpecFileRadioButton);
   rightAnatomyButtonGroup->addButton(rightAnatomyUseSpecFileRadioButton);
   
   rightAnatomySpecFileLineEdit = new QLineEdit;
   rightAnatomySpecFileLineEdit->setMinimumWidth(lineEditWidth);
   rightAnatomySpecFileLineEdit->setReadOnly(true);
   QPushButton* rightAnatomyChooseSpecPushButton = 
                      new GuiFileSelectionButton(0,
                                                 "Select...",
                                                 GuiDataFileDialog::specFileFilter,
                                                 true);
   QObject::connect(rightAnatomyChooseSpecPushButton, SIGNAL(fileSelected(const QString&)),
                    rightAnatomySpecFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(rightAnatomyChooseSpecPushButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisableButtons()));
   anatomyRightCreateUseSpecGroupBox = new QGroupBox("Right Hemisphere Spec File");
   QGridLayout* rightGridLayout = new QGridLayout(anatomyRightCreateUseSpecGroupBox);
   rightGridLayout->addWidget(rightAnatomyCreateSpecFileRadioButton, 0, 0);
   rightGridLayout->addWidget(rightAnatomyUseSpecFileRadioButton, 1, 0);
   rightGridLayout->addWidget(rightAnatomyChooseSpecPushButton, 1, 1);
   rightGridLayout->addWidget(rightAnatomySpecFileLineEdit, 1, 2);
   rightGridLayout->setColumnStretch(0, 0);
   rightGridLayout->setColumnStretch(1, 0);
   rightGridLayout->setColumnStretch(2, 100);
   anatomyRightCreateUseSpecGroupBox->setFixedHeight(anatomyRightCreateUseSpecGroupBox->sizeHint().height());
   
   
   //
   // Cerebellum spec choice
   //
   cerebellumAnatomyCreateSpecFileRadioButton = new QRadioButton("Create Spec File");
   cerebellumAnatomyUseSpecFileRadioButton = new QRadioButton("Use Spec File");
   QButtonGroup* cerebellumAnatomyButtonGroup = new QButtonGroup(this);
   QObject::connect(cerebellumAnatomyButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisableButtons()));
   cerebellumAnatomyButtonGroup->addButton(cerebellumAnatomyCreateSpecFileRadioButton);
   cerebellumAnatomyButtonGroup->addButton(cerebellumAnatomyUseSpecFileRadioButton);
   
   cerebellumAnatomySpecFileLineEdit = new QLineEdit;
   cerebellumAnatomySpecFileLineEdit->setMinimumWidth(lineEditWidth);
   cerebellumAnatomySpecFileLineEdit->setReadOnly(true);
   QPushButton* cerebellumAnatomyChooseSpecPushButton = 
                      new GuiFileSelectionButton(0,
                                                 "Select...",
                                                 GuiDataFileDialog::specFileFilter,
                                                 true);
   QObject::connect(cerebellumAnatomyChooseSpecPushButton, SIGNAL(fileSelected(const QString&)),
                    cerebellumAnatomySpecFileLineEdit, SLOT(setText(const QString&)));
   QObject::connect(cerebellumAnatomyChooseSpecPushButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotEnableDisableButtons()));
   anatomyCerebellumCreateUseSpecGroupBox = new QGroupBox("Cerebellum Spec File");
   QGridLayout* cerebellumGridLayout = new QGridLayout(anatomyCerebellumCreateUseSpecGroupBox);
   cerebellumGridLayout->addWidget(cerebellumAnatomyCreateSpecFileRadioButton, 0, 0);
   cerebellumGridLayout->addWidget(cerebellumAnatomyUseSpecFileRadioButton, 1, 0);
   cerebellumGridLayout->addWidget(cerebellumAnatomyChooseSpecPushButton, 1, 1);
   cerebellumGridLayout->addWidget(cerebellumAnatomySpecFileLineEdit, 1, 2);
   cerebellumGridLayout->setColumnStretch(0, 0);
   cerebellumGridLayout->setColumnStretch(1, 0);
   cerebellumGridLayout->setColumnStretch(2, 100);
   anatomyCerebellumCreateUseSpecGroupBox->setFixedHeight(anatomyCerebellumCreateUseSpecGroupBox->sizeHint().height());
   
   //
   // Layout the page
   //
   QVBoxLayout* layout = pageAnatomyChooseSpec->getLayout();
   layout->addWidget(anatomyLeftCreateUseSpecGroupBox);
   layout->addWidget(anatomyRightCreateUseSpecGroupBox);
   layout->addWidget(anatomyCerebellumCreateUseSpecGroupBox);
   layout->addStretch();
   
   QString s2 = 
      "For each structure (left hemisphere, right hemisphere, and cerebellum) "
      "select its segmentation selection.<P>"
      "";
   pageAnatomyChooseSpec->setHelp(s2);
   
   pageStackedWidget->addWidget(pageAnatomyChooseSpec);
}

/**
 * update the anatomy choose spec page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateAnatomyChooseSpecPage()
{
   anatomyLeftCreateUseSpecGroupBox->setEnabled(taskAnatomyProcessLeftCheckBox->isChecked());
   anatomyRightCreateUseSpecGroupBox->setEnabled(taskAnatomyProcessRightCheckBox->isChecked());
   anatomyCerebellumCreateUseSpecGroupBox->setEnabled(taskAnatomyProcessCerebellumCheckBox->isChecked());
}

/**      
 * create spec create page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyCreateSpecPage()
{
   pageAnatomyCreateSpec = new Page("Anatomy - Create Spec File(s)");

   const int minWidth = 300;

   //
   // left spec file
   //
   anatomyCreateLeftSpecDirectoryLineEdit = new QLineEdit;
   anatomyCreateLeftSpecDirectoryLineEdit->setText("LEFT_HEM");
   anatomyCreateLeftSpecFileLineEdit = new QLineEdit;
   anatomyCreateLeftSpecFileLineEdit->setMinimumWidth(minWidth);
   anatomyCreateLeftSpecFileGroupBox = new QGroupBox("Left Hemisphere Spec File Info");
   QGridLayout* anatomyCreateLeftSpecFileLayout = new QGridLayout(anatomyCreateLeftSpecFileGroupBox);
   anatomyCreateLeftSpecFileLayout->addWidget(new QLabel("Subdirectory"), 0, 0);
   anatomyCreateLeftSpecFileLayout->addWidget(anatomyCreateLeftSpecDirectoryLineEdit, 0, 1);
   anatomyCreateLeftSpecFileLayout->addWidget(new QLabel("Spec File Name"), 1, 0);
   anatomyCreateLeftSpecFileLayout->addWidget(anatomyCreateLeftSpecFileLineEdit, 1, 1);
   
   //
   // right spec file
   //
   anatomyCreateRightSpecDirectoryLineEdit = new QLineEdit;
   anatomyCreateRightSpecDirectoryLineEdit->setText("RIGHT_HEM");
   anatomyCreateRightSpecFileLineEdit = new QLineEdit;
   anatomyCreateRightSpecFileLineEdit->setMinimumWidth(minWidth);
   anatomyCreateRightSpecFileGroupBox = new QGroupBox("Right Hemisphere Spec File Info");
   QGridLayout* anatomyCreateRightSpecFileLayout = new QGridLayout(anatomyCreateRightSpecFileGroupBox);
   anatomyCreateRightSpecFileLayout->addWidget(new QLabel("Subdirectory"), 0, 0);
   anatomyCreateRightSpecFileLayout->addWidget(anatomyCreateRightSpecDirectoryLineEdit, 0, 1);
   anatomyCreateRightSpecFileLayout->addWidget(new QLabel("Spec File Name"), 1, 0);
   anatomyCreateRightSpecFileLayout->addWidget(anatomyCreateRightSpecFileLineEdit, 1, 1);
   
   //
   // cerebellum spec file
   //
   anatomyCreateCerebellumDirectoryFileLineEdit = new QLineEdit;
   anatomyCreateCerebellumDirectoryFileLineEdit->setText("CEREBELLUM");
   anatomyCreateCerebellumSpecFileLineEdit = new QLineEdit;
   anatomyCreateCerebellumSpecFileLineEdit->setMinimumWidth(minWidth);
   anatomyCreateCerebellumSpecFileGroupBox = new QGroupBox("Cerebellum Spec File Info");
   QGridLayout* anatomyCreateCerebellumSpecFileLayout = new QGridLayout(anatomyCreateCerebellumSpecFileGroupBox);
   anatomyCreateCerebellumSpecFileLayout->addWidget(new QLabel("Subdirectory"), 0, 0);
   anatomyCreateCerebellumSpecFileLayout->addWidget(anatomyCreateCerebellumDirectoryFileLineEdit, 0, 1);
   anatomyCreateCerebellumSpecFileLayout->addWidget(new QLabel("Spec File Name"), 1, 0);
   anatomyCreateCerebellumSpecFileLayout->addWidget(anatomyCreateCerebellumSpecFileLineEdit, 1, 1);
   
   QVBoxLayout* layout = pageAnatomyCreateSpec->getLayout();
   layout->addWidget(anatomyCreateLeftSpecFileGroupBox);
   layout->addWidget(anatomyCreateRightSpecFileGroupBox);
   layout->addWidget(anatomyCreateCerebellumSpecFileGroupBox);
   layout->addStretch();

   QString s2 = 
      "The spec file names show above will be created when the <B>Next</B> button "
      "is pressed.";
   pageAnatomyCreateSpec->setHelp(s2);

   pageStackedWidget->addWidget(pageAnatomyCreateSpec);
}

/**
 * create the anatomy subject information page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomySubjectInformationPage()
{
   pageAnatomySubjectInformation = new Page("Anatomy - Subject Information");
   
   //
   // Information about the subject
   //
   QPushButton* speciesPushButton = new QPushButton("Species...");
   speciesPushButton->setAutoDefault(false);
   QObject::connect(speciesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomySpeciesPushButton()));
   anatomySpeciesLineEdit = new QLineEdit;
   QObject::connect(anatomySpeciesLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotEnableDisableButtons()));
   anatomySpeciesLineEdit->setText("Human");

   QLabel* subjectLabel = new QLabel("Subject");
   anatomySubjectLineEdit = new QLineEdit;
   QObject::connect(anatomySubjectLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotEnableDisableButtons()));

   QPushButton* categoryPushButton = new QPushButton("Category");
   categoryPushButton->setAutoDefault(false);
   QObject::connect(categoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyCategoryPushButton()));
   anatomyCategoryLineEdit = new QLineEdit;
   QObject::connect(anatomyCategoryLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotEnableDisableButtons()));
   anatomyCategoryLineEdit->setText("INDIVIDUAL");
   
   QGroupBox* subjectGroupBox = new QGroupBox("Subject Information");
   QGridLayout* subjectGroupLayout = new QGridLayout(subjectGroupBox);
   subjectGroupLayout->addWidget(speciesPushButton, 0, 0);
   subjectGroupLayout->addWidget(anatomySpeciesLineEdit, 0, 1);
   subjectGroupLayout->addWidget(subjectLabel, 1, 0);
   subjectGroupLayout->addWidget(anatomySubjectLineEdit, 1, 1);
   subjectGroupLayout->addWidget(categoryPushButton, 2, 0);
   subjectGroupLayout->addWidget(anatomyCategoryLineEdit, 2, 1);
   
   QVBoxLayout* layout = pageAnatomySubjectInformation->getLayout();
   layout->addWidget(subjectGroupBox);
   layout->addStretch();

   QString s2 = 
      "The subject information will be used to create the new spec file names.";
   pageAnatomySubjectInformation->setHelp(s2);

   pageStackedWidget->addWidget(pageAnatomySubjectInformation);
}

/**
 * called when anatomy species button pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomySpeciesPushButton()
{
   std::vector<QString> values;
   Species::getAllSpecies(values);
  
   int defaultIndex = 0;
   for (int i = 0; i < static_cast<int>(values.size()); i++) {
      if ("Human" == values[i]) {
         defaultIndex = i;
         break;
      }
   }

   QtListBoxSelectionDialog lbsd(this,
                                  "Choose Species",
                                  "",
                                  values,
                                  defaultIndex);
   if (lbsd.exec() == QDialog::Accepted) {
      anatomySpeciesLineEdit->setText(lbsd.getSelectedText());
   }
}

/**
 * called when anatomy category button pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyCategoryPushButton()
{
   std::vector<QString> values;
   Categories::getAllCategories(values);

   int defaultIndex = 1;
   for (int i = 0; i < static_cast<int>(values.size()); i++) {
      if ("INDIVIDUAL" == values[i]) {
         defaultIndex = i;
         break;
      }
   }

   QtListBoxSelectionDialog lbsd(this,
                                  "Choose Category",
                                  "",
                                  values,
                                  defaultIndex);
   if (lbsd.exec() == QDialog::Accepted) {
      anatomyCategoryLineEdit->setText(lbsd.getSelectedText());
   }
}      

/**      
 * create the orientation page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyOrientationPage()
{
   pageAnatomyOrientation = new Page("Volume Orientation");
  
   
   //
   // Orientation selections
   //
   QLabel* xLabel = new QLabel("X ");
   anatomyVolumeXOrientationComboBox = new GuiVolumeFileOrientationComboBox(0, VolumeFile::VOLUME_AXIS_X);
   QObject::connect(anatomyVolumeXOrientationComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEnableDisableButtons()));
   QLabel* yLabel = new QLabel("Y ");
   anatomyVolumeYOrientationComboBox = new GuiVolumeFileOrientationComboBox(0, VolumeFile::VOLUME_AXIS_Y);
   QObject::connect(anatomyVolumeYOrientationComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEnableDisableButtons()));
   QLabel* zLabel = new QLabel("Z ");
   anatomyVolumeZOrientationComboBox = new GuiVolumeFileOrientationComboBox(0, VolumeFile::VOLUME_AXIS_Z);
   QObject::connect(anatomyVolumeZOrientationComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEnableDisableButtons()));
   QGridLayout* orientGridLayout = new QGridLayout;
   orientGridLayout->addWidget(xLabel, 0, 0);
   orientGridLayout->addWidget(anatomyVolumeXOrientationComboBox, 0, 1);
   orientGridLayout->addWidget(yLabel, 1, 0);
   orientGridLayout->addWidget(anatomyVolumeYOrientationComboBox, 1, 1);
   orientGridLayout->addWidget(zLabel, 2, 0);
   orientGridLayout->addWidget(anatomyVolumeZOrientationComboBox, 2, 1);
   orientGridLayout->setColumnStretch(0, 0);
   orientGridLayout->setColumnStretch(1, 0);
   orientGridLayout->setColumnStretch(2, 100);
   QGroupBox* orientGroupBox = new QGroupBox("Orientation");
   QVBoxLayout* orientGroupLayout = new QVBoxLayout(orientGroupBox);
   orientGroupLayout->addLayout(orientGridLayout);
   
   //
   // Flip About X-Axis Push Button
   //   
   QPushButton* flipXPushButton = new QPushButton("Flip About Screen X-Axis");
   flipXPushButton->setFixedSize(flipXPushButton->sizeHint());
   flipXPushButton->setAutoDefault(false);
   QObject::connect(flipXPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeFlipX()));
                    
   //
   // Flip About Y-Axis Push Button
   //   
   QPushButton* flipYPushButton = new QPushButton("Flip About Screen Y-Axis");
   flipYPushButton->setFixedSize(flipYPushButton->sizeHint());
   flipYPushButton->setAutoDefault(false);
   QObject::connect(flipYPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeFlipY()));
   
   //
   // Rotate volume button
   //
   QPushButton* rotatePushButton = new QPushButton("Rotate Clockwise");
   rotatePushButton->setAutoDefault(false);
   rotatePushButton->setFixedSize(rotatePushButton->sizeHint());
   QObject::connect(rotatePushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeRotate()));
   //
   // orthogonal buttons
   //
   QGroupBox* orthogGroupBox = new QGroupBox("Orthogonal Controls");
   QVBoxLayout* orthogLayout = new QVBoxLayout(orthogGroupBox);
   orthogLayout->addWidget(flipXPushButton);
   orthogLayout->addWidget(flipYPushButton);
   orthogLayout->addWidget(rotatePushButton);

   //
   // AC-PC align labels
   //
   QLabel* structureLabel = new QLabel("Structure");
   QLabel* xColLabel = new QLabel("X");
   QLabel* yColLabel = new QLabel("Y");
   QLabel* zColLabel = new QLabel("Z");
   
   //
   // AC spin boxes
   //
   QLabel* acLabel = new QLabel("Anterior Commissure");
   alignAcXSpinBox = new QSpinBox;
   alignAcXSpinBox->setMinimum(0);
   alignAcXSpinBox->setMaximum(1000000);
   alignAcXSpinBox->setSingleStep(1);
   alignAcYSpinBox = new QSpinBox;
   alignAcYSpinBox->setMinimum(0);
   alignAcYSpinBox->setMaximum(1000000);
   alignAcYSpinBox->setSingleStep(1);
   alignAcZSpinBox = new QSpinBox;
   alignAcZSpinBox->setMinimum(0);
   alignAcZSpinBox->setMaximum(1000000);
   alignAcZSpinBox->setSingleStep(1);
   QPushButton* acPushButton = new QPushButton("Use Crosshairs");
   acPushButton->setAutoDefault(false);
   QObject::connect(acPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyCrosshairsAlignAC()));
                    
   //
   // PC spin boxes
   //
   QLabel* pcLabel = new QLabel("Posterior Commissure");
   alignPcXSpinBox = new QSpinBox;
   alignPcXSpinBox->setMinimum(0);
   alignPcXSpinBox->setMaximum(1000000);
   alignPcXSpinBox->setSingleStep(1);
   alignPcYSpinBox = new QSpinBox;
   alignPcYSpinBox->setMinimum(0);
   alignPcYSpinBox->setMaximum(1000000);
   alignPcYSpinBox->setSingleStep(1);
   alignPcZSpinBox = new QSpinBox;
   alignPcZSpinBox->setMinimum(0);
   alignPcZSpinBox->setMaximum(1000000);
   alignPcZSpinBox->setSingleStep(1);
   QPushButton* pcPushButton = new QPushButton("Use Crosshairs");
   pcPushButton->setAutoDefault(false);
   QObject::connect(pcPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyCrosshairsAlignPC()));
   
   //
   // LF spin boxes
   //
   QLabel* lfLabel = new QLabel("Longitudinal Fissure");
   alignLfXSpinBox = new QSpinBox;
   alignLfXSpinBox->setMinimum(0);
   alignLfXSpinBox->setMaximum(1000000);
   alignLfXSpinBox->setSingleStep(1);
   alignLfYSpinBox = new QSpinBox;
   alignLfYSpinBox->setMinimum(0);
   alignLfYSpinBox->setMaximum(1000000);
   alignLfYSpinBox->setSingleStep(1);
   alignLfZSpinBox = new QSpinBox;
   alignLfZSpinBox->setMinimum(0);
   alignLfZSpinBox->setMaximum(1000000);
   alignLfZSpinBox->setSingleStep(1);
   QPushButton* lfPushButton = new QPushButton("Use Crosshairs");
   lfPushButton->setAutoDefault(false);
   QObject::connect(lfPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyCrosshairsAlignLF()));
                    
   QGridLayout* alignGrid = new QGridLayout;
   alignGrid->setMargin(3);
   alignGrid->setSpacing(3);
   alignGrid->addWidget(structureLabel, 0, 0, Qt::AlignHCenter); 
   alignGrid->addWidget(xColLabel, 0, 1, Qt::AlignHCenter); 
   alignGrid->addWidget(yColLabel, 0, 2, Qt::AlignHCenter); 
   alignGrid->addWidget(zColLabel, 0, 3, Qt::AlignHCenter); 
   alignGrid->addWidget(acLabel, 1, 0); 
   alignGrid->addWidget(alignAcXSpinBox, 1, 1); 
   alignGrid->addWidget(alignAcYSpinBox, 1, 2); 
   alignGrid->addWidget(alignAcZSpinBox, 1, 3); 
   alignGrid->addWidget(acPushButton, 1, 4); 
   alignGrid->addWidget(pcLabel, 2, 0); 
   alignGrid->addWidget(alignPcXSpinBox, 2, 1); 
   alignGrid->addWidget(alignPcYSpinBox, 2, 2); 
   alignGrid->addWidget(alignPcZSpinBox, 2, 3); 
   alignGrid->addWidget(pcPushButton, 2, 4); 
   alignGrid->addWidget(lfLabel, 3, 0); 
   alignGrid->addWidget(alignLfXSpinBox, 3, 1); 
   alignGrid->addWidget(alignLfYSpinBox, 3, 2); 
   alignGrid->addWidget(alignLfZSpinBox, 3, 3); 
   alignGrid->addWidget(lfPushButton, 3, 4); 
   
   //
   // Apply AC-PC alignment to volume
   //
   QPushButton* acPcAlignPushButton = new QPushButton("Align using AC and PC");
   acPcAlignPushButton->setFixedSize(acPcAlignPushButton->sizeHint());
   acPcAlignPushButton->setAutoDefault(false);
   QObject::connect(acPcAlignPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAcPcAlignAnatomyVolume()));
                    
   //
   // Oblique controls
   //
   QGroupBox* obliqueGroupBox = new QGroupBox("Oblique Controls");
   QVBoxLayout* oblqueGroupLayout = new QVBoxLayout(obliqueGroupBox);
   oblqueGroupLayout->addLayout(alignGrid);
   oblqueGroupLayout->addWidget(acPcAlignPushButton);
   
   // 
   // Save volume push button
   //
   QPushButton* savePushButton = createSaveAnatomyVolumeFilePushButton();
   
   //
   // layout for page
   //
   QVBoxLayout* layout = pageAnatomyOrientation->getLayout();
   layout->addWidget(orientGroupBox);
   layout->addWidget(orthogGroupBox);
   layout->addWidget(savePushButton);
   // disable for now //     layout->addWidget(obliqueGroupBox);
   layout->addStretch();

   QString s2 = 
      "The volume must be oriented as follows:"
      "<UL>"
      "<LI> The left portion of the brain on the negative X-Axis"
      "<LI> The posterior part of the brain on the negative Y-Axis" 
      "<LI> The inferior part of the brain on the negative Z-Axis"
      "</UL>"
      "The <B>Next</B> pushbutton will not be enabled until the X orientation is "
      "X-Left to Right, the Y orientation is Y-Posterior to Anterior, and the Z "
      "orientation is Z-Inferior to Superior.<P>"
      "If the above conditions are not met, the segmentation process will fail.<P>"
      "If the volume needs flipping, use the <B>Flip</B> pushbuttons to flip "
      "volume until it is in LPI orientation.  Once the volume is in an LPI "
      "orientation, set the controls in the Orientation group to LPI.<P>"
/*
      "If the volume is in an obqlue orientation, find the Anterior Commissure in "
      "the volume in the main window, place the crosshairs over the anterior "
      "commissure, and press the Use Crosshairs push button in the Anterior "
      "commissure row.  Do the same operation to find and set the position of "
      "the posterior commissure.  Lastly, find a voxel in the longitudinal fissure "
      "and press the Use Crosshiars pushbutton.  Once the AC, PC, and LF have "
      "been set, press the <B>Align Using AC and PC</B?> push button." */;
   pageAnatomyOrientation->setHelp(s2);

   pageStackedWidget->addWidget(pageAnatomyOrientation);
}

/**
 * called when rotate push button is pressed.
 */
void
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeRotate()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      switch (bmv->getSelectedAxis(0)) {
         case VolumeFile::VOLUME_AXIS_X:
            anatomyVolume->rotate(VolumeFile::VOLUME_AXIS_X);
            break;
         case VolumeFile::VOLUME_AXIS_Y:
            anatomyVolume->rotate(VolumeFile::VOLUME_AXIS_Y);
            break;
         case VolumeFile::VOLUME_AXIS_Z:
            anatomyVolume->rotate(VolumeFile::VOLUME_AXIS_Z);
            break;
         case VolumeFile::VOLUME_AXIS_ALL:
         case VolumeFile::VOLUME_AXIS_OBLIQUE:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
         case VolumeFile::VOLUME_AXIS_UNKNOWN:
            break;
      }
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}


/**
 * called to AC-PC align a volume.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAcPcAlignAnatomyVolume()
{
   const int ac[3] = {
      alignAcXSpinBox->value(),
      alignAcYSpinBox->value(),
      alignAcZSpinBox->value()
   };
   const int pc[3] = {
      alignPcXSpinBox->value(),
      alignPcYSpinBox->value(),
      alignPcZSpinBox->value()
   };
   const int lf[3] = {
      alignLfXSpinBox->value(),
      alignLfYSpinBox->value(),
      alignLfZSpinBox->value()
   };
   
   anatomyVolume->acPcAlign(ac, pc, lf);
   
   GuiToolBar* toolbar = theMainWindow->getToolBar();
   if (toolbar != NULL) {
      toolbar->setViewSelection(BrainModel::VIEW_RESET);
   }
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called when align set AC X-Hairs button pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyCrosshairsAlignAC()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      GuiMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.", "OK");
      return;
   }
   
   int slices[3];
   bmv->getSelectedOrthogonalSlices(0, slices);
   
   alignAcXSpinBox->setValue(slices[0]);
   alignAcYSpinBox->setValue(slices[1]);
   alignAcZSpinBox->setValue(slices[2]);
}

/**
 * called when align set PC X-Hairs button pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyCrosshairsAlignPC()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      GuiMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.", "OK");
      return;
   }
   
   int slices[3];
   bmv->getSelectedOrthogonalSlices(0, slices);
   
   alignPcXSpinBox->setValue(slices[0]);
   alignPcYSpinBox->setValue(slices[1]);
   alignPcZSpinBox->setValue(slices[2]);
}

/**
 * called when align set LF X-Hairs button pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyCrosshairsAlignLF()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      GuiMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.", "OK");
      return;
   }
   
   int slices[3];
   bmv->getSelectedOrthogonalSlices(0, slices);
   
   alignLfXSpinBox->setValue(slices[0]);
   alignLfYSpinBox->setValue(slices[1]);
   alignLfZSpinBox->setValue(slices[2]);
}
/**
 * flip anatomy volume about screen X-Axis.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeFlipX()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      GuiMessageBox::critical(this, "ERROR", "DISPLAY THE VOLUME IN THE MAIN WINDOW", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   switch (bmv->getSelectedAxis(0)) {
      case VolumeFile::VOLUME_AXIS_X:
         anatomyVolume->flip(VolumeFile::VOLUME_AXIS_Z, false);
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         anatomyVolume->flip(VolumeFile::VOLUME_AXIS_Z, false);
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         anatomyVolume->flip(VolumeFile::VOLUME_AXIS_Y, false);
         break;
      case VolumeFile::VOLUME_AXIS_ALL:
      case VolumeFile::VOLUME_AXIS_OBLIQUE:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         break;
   }

   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
   updateAnatomyOrientationPage();
}

/**
 * flip anatomy volume about screen Y-Axis.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeFlipY()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      GuiMessageBox::critical(this, "ERROR", "DISPLAY THE VOLUME IN THE MAIN WINDOW", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   switch (bmv->getSelectedAxis(0)) {
      case VolumeFile::VOLUME_AXIS_X:
         anatomyVolume->flip(VolumeFile::VOLUME_AXIS_Y, false);
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         anatomyVolume->flip(VolumeFile::VOLUME_AXIS_X, false);
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         anatomyVolume->flip(VolumeFile::VOLUME_AXIS_X, false);
         break;
      case VolumeFile::VOLUME_AXIS_ALL:
      case VolumeFile::VOLUME_AXIS_OBLIQUE:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         break;
   }

   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
}

/**
 * update the anatomy orientation page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateAnatomyOrientationPage()
{
   VolumeFile::ORIENTATION orient[3];
   anatomyVolume->getOrientation(orient);
   
   anatomyVolumeXOrientationComboBox->setOrientation(orient[0]);
   anatomyVolumeYOrientationComboBox->setOrientation(orient[1]);
   anatomyVolumeZOrientationComboBox->setOrientation(orient[2]);
}
      
/**      
 * create the voxel size page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyVoxelSizePage()
{
   pageAnatomyVoxelSize = new Page("Volume Voxel Size");
   
   //
   // Voxel X/Y/Z sizes
   //
   QLabel* xLabel = new QLabel("X ");
   anatomyVoxelSizeXSpinBox = new QDoubleSpinBox;
   anatomyVoxelSizeXSpinBox->setMinimum(0.001);
   anatomyVoxelSizeXSpinBox->setMaximum(10000.0);
   anatomyVoxelSizeXSpinBox->setSingleStep(0.5);
   anatomyVoxelSizeXSpinBox->setDecimals(3);
   QLabel* yLabel = new QLabel("Y ");
   anatomyVoxelSizeYSpinBox = new QDoubleSpinBox;
   anatomyVoxelSizeYSpinBox->setMinimum(0.001);
   anatomyVoxelSizeYSpinBox->setMaximum(10000.0);
   anatomyVoxelSizeYSpinBox->setSingleStep(0.5);
   anatomyVoxelSizeYSpinBox->setDecimals(3);
   QLabel* zLabel = new QLabel("Z ");
   anatomyVoxelSizeZSpinBox = new QDoubleSpinBox;
   anatomyVoxelSizeZSpinBox->setMinimum(0.001);
   anatomyVoxelSizeZSpinBox->setMaximum(10000.0);
   anatomyVoxelSizeZSpinBox->setSingleStep(0.5);
   anatomyVoxelSizeZSpinBox->setDecimals(3);
   QGridLayout* sizeGridLayout = new QGridLayout;
   sizeGridLayout->addWidget(xLabel, 0, 0);
   sizeGridLayout->addWidget(anatomyVoxelSizeXSpinBox, 0, 1);
   sizeGridLayout->addWidget(yLabel, 1, 0);
   sizeGridLayout->addWidget(anatomyVoxelSizeYSpinBox, 1, 1);
   sizeGridLayout->addWidget(zLabel, 2, 0);
   sizeGridLayout->addWidget(anatomyVoxelSizeZSpinBox, 2, 1);
   sizeGridLayout->setColumnStretch(0, 0);
   sizeGridLayout->setColumnStretch(1, 0);
   sizeGridLayout->setColumnStretch(2, 100);
   
   //
   // Resample push button
   //
   QPushButton* resamplePushButton = new QPushButton("Resample");
   resamplePushButton->setAutoDefault(false);
   resamplePushButton->setFixedSize(resamplePushButton->sizeHint());
   QObject::connect(resamplePushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyResampleVoxels()));
                    
   // 
   // Save volume push button
   //
   QPushButton* savePushButton = createSaveAnatomyVolumeFilePushButton();

   QGroupBox* sizeGroupBox = new QGroupBox("Voxel Sizes");
   QVBoxLayout* sizeGroupLayout = new QVBoxLayout(sizeGroupBox);
   sizeGroupLayout->addLayout(sizeGridLayout);
   sizeGroupLayout->addWidget(resamplePushButton);
   sizeGroupLayout->addWidget(savePushButton);
   
   anatomyVoxelSizeTextEdit = new QTextEdit;
   
   QVBoxLayout* layout = pageAnatomyVoxelSize->getLayout();
   layout->addWidget(sizeGroupBox);
   layout->addWidget(anatomyVoxelSizeTextEdit);
   layout->addStretch();

   QString s2 = 
      "To resample the voxels, change the voxels to the desired size "
      "and press the <B>Resample</B> push button.  After doing so, it "
      "is recommended that you save the volume using a new name by "
      "pressing the <B>Save Volume File</B> push button.";
   pageAnatomyVoxelSize->setHelp(s2);

   pageStackedWidget->addWidget(pageAnatomyVoxelSize);
}

/**
 * create the save anatomy volume file push button.
 */
QPushButton* 
GuiVolumeMultiHemSureFitSegmentationDialog::createSaveAnatomyVolumeFilePushButton()
{
   QPushButton* savePushButton = new QPushButton("Save Volume File...");
   savePushButton->setAutoDefault(false);
   savePushButton->setFixedSize(savePushButton->sizeHint());
   QObject::connect(savePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSaveAnatomyVolumeFile()));
   return savePushButton;
}
      
/**
 * resample the anatomy voxels.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyResampleVoxels()
{
   const float newSizes[3] = {
      anatomyVoxelSizeXSpinBox->value(),
      anatomyVoxelSizeYSpinBox->value(),
      anatomyVoxelSizeZSpinBox->value()
   };
   
   anatomyVolume->resampleToSpacing(newSizes);
   GuiBrainModelOpenGL::updateAllGL();
   updateAnatomyVoxelSizePage();
}

/**
 * save the anatomy volume file.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotSaveAnatomyVolumeFile()
{
   //
   // Remove extension from name of file
   //
   QString name = anatomyVolume->getFileName();
   const int nameLen = name.length();
   if (name.endsWith(SpecFile::getAfniVolumeFileExtension())) {
      name = name.left(nameLen - 5);
   }
   else if(name.endsWith(SpecFile::getAnalyzeVolumeFileExtension())) {
      name = name.left(nameLen - 4);
   }
   else if(name.endsWith(".4dfp." + SpecFile::getWustlVolumeFileExtension())) {
      name = name.left(nameLen - 9);
   }
   else if(name.endsWith(SpecFile::getWustlVolumeFileExtension())) {
      name = name.left(nameLen - 4);
   }
   else if(name.endsWith(SpecFile::getMincVolumeFileExtension())) {
      name = name.left(nameLen - 4);
   }
   else if(name.endsWith(SpecFile::getNiftiVolumeFileExtension())) {
      name = name.left(nameLen - 4);
   }
   else if(name.endsWith(SpecFile::getNiftiGzipVolumeFileExtension())) {
      name = name.left(nameLen - 7);
   }
   else if(name.endsWith(".vtk")) {
      name = name.left(nameLen - 4);
   }
   
   //
   // Add extension and set file filter
   //
   QString fileFilter;
   bool zipAfniFlag = false;
   if (volumeFileTypeAfniRadioButton->isChecked()) {
      name += SpecFile::getAfniVolumeFileExtension();
      fileFilter = GuiDataFileDialog::volumeFileAfniFilter;
      zipAfniFlag = volumeFileWriteCompressedCheckBox->isChecked();
   }
   else if (volumeFileTypeNiftiRadioButton->isChecked()) {
      if (volumeFileWriteCompressedCheckBox->isChecked()) {
         name += SpecFile::getNiftiGzipVolumeFileExtension();
      }
      else {
         name += SpecFile::getNiftiVolumeFileExtension();
      }
      fileFilter = GuiDataFileDialog::volumeFileNiftiWriteFilter;
   }
   else if (volumeFileTypeSpmRadioButton->isChecked()) {
      name += SpecFile::getAnalyzeVolumeFileExtension();
      fileFilter = GuiDataFileDialog::volumeFileSpmMedxFilter;
   }
   else if (volumeFileTypeWuNilRadioButton->isChecked()) {
      name += ".4dfp." + SpecFile::getWustlVolumeFileExtension();
      fileFilter = GuiDataFileDialog::volumeFileWuNilFilter;
   }
   else {
      name += SpecFile::getNiftiVolumeFileExtension();
      fileFilter = GuiDataFileDialog::volumeFileNiftiWriteFilter;
   }
   
   //
   // Use a file dialog to save the volume file
   //
   QFileDialog fd(this);
   fd.setWindowTitle("Save Volume File");
   fd.setDirectory(QDir::current());
   fd.setAcceptMode(QFileDialog::AcceptSave);
   fd.setFileMode(QFileDialog::AnyFile);
   fd.setFilter(fileFilter);
   fd.selectFilter(fileFilter);
   fd.setConfirmOverwrite(true);
   fd.selectFile(name);
   if (fd.exec() == QFileDialog::Accepted) {
      QStringList files = fd.selectedFiles();
      if (files.isEmpty() == false) {
         name = files.at(0);
         
         anatomyVolume->setDescriptiveLabel(FileUtilities::basename(name));
         std::vector<VolumeFile*> filesToWrite;
         filesToWrite.push_back(anatomyVolume);
         try {
            VolumeFile::writeFile(name,
                                  VolumeFile::VOXEL_DATA_TYPE_FLOAT,
                                  filesToWrite,
                                  zipAfniFlag);
         }
         catch (FileException& e) {
            GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
            return;
         }
         anatomyVolume->setFileName(FileUtilities::basename(name));
         anatomyVolume->clearModified();
      }
   }
}

/**
 * update the voxel size page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateAnatomyVoxelSizePage()
{
   float voxelSize[3];
   anatomyVolume->getSpacing(voxelSize);
   
   anatomyVoxelSizeXSpinBox->setValue(voxelSize[0]);
   anatomyVoxelSizeYSpinBox->setValue(voxelSize[1]);
   anatomyVoxelSizeZSpinBox->setValue(voxelSize[2]);

   //
   // voxel size message
   //
   QString msg;   
   if ((voxelSize[0] >= 0.99) && (voxelSize[0] <= 1.01) &&
       (voxelSize[1] >= 0.99) && (voxelSize[1] <= 1.01) &&
       (voxelSize[2] >= 0.99) && (voxelSize[2] <= 1.01)) {
      msg = "The voxels appear to be 1mm cubic voxels.  "
            "If you are segmenting a human subject, press the \"Next\" "
            "pushbutton to continue.  If you are segmenting a "
            "monkey subject, the voxels should be resampled to 0.5mm "
            "cubic voxels.";
   }
   else if ((voxelSize[0] >= 0.49) && (voxelSize[0] <= 0.51) &&
            (voxelSize[1] >= 0.49) && (voxelSize[1] <= 0.51) &&
            (voxelSize[2] >= 0.49) && (voxelSize[2] <= 0.51)) {
      msg = "The voxels appear to be 0.5mm cubic voxels.  "
            "If you are segmenting a monkey subject, press the \"Next\" "
            "pushbutton to continue.  If you are segmenting "
            "a human subject, the voxels should be resampled to 1.0mm "
            "cubic voxels.";
   }
   else {
      msg = "The voxel sizes appear to be incorrect.  For human "
            "subjects, the voxels should be 1.0mm cubic voxels.  For "
            "monkeys, the voxels should be 0.5mm cubic voxels.";
   }
   anatomyVoxelSizeTextEdit->setPlainText(msg);
}

/**      
 * create the origin page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyOriginPage()
{
   pageAnatomyOrigin = new Page("Volume Origin");

   QPushButton* setOriginPushButton = new QPushButton("Set Origin");
   setOriginPushButton->setAutoDefault(false);
   setOriginPushButton->setFixedSize(setOriginPushButton->sizeHint());
   QObject::connect(setOriginPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeSetOrigin()));
   
   // 
   // Save volume push button
   //
   QPushButton* savePushButton = createSaveAnatomyVolumeFilePushButton();

   QVBoxLayout* layout = pageAnatomyOrigin->getLayout();
   layout->addWidget(setOriginPushButton);
   layout->addWidget(savePushButton);
   layout->addStretch();

   const QString msg = 
      "Press the <B>R</B> button in the main window toolbar.  "
      "The crosshairs should intersect at the anterior commissure "
      "in all slice views (coronal, horizontal, and parasaggittal).  "
      "If they do, press the <B>Next</B> button to continue."
      "If they do not, move the crosshairs to the anterior "
      "commissure using all slices view (coronal, horizontal, and "
      "parasaggittal and then press the <B>Set Origin</B> button.";
   pageAnatomyOrigin->setHelp(msg);
   pageStackedWidget->addWidget(pageAnatomyOrigin);
}

/**
 * called to set origin.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeSetOrigin()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      GuiMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.", "OK");
      return;
   }
   
   int acSlices[3];
   bmv->getSelectedOrthogonalSlices(0, acSlices);
   
   float voxelSize[3];
   anatomyVolume->getSpacing(voxelSize);
   
   const float origin[3] = {
      -(acSlices[0] * voxelSize[0]),
      -(acSlices[1] * voxelSize[1]),
      -(acSlices[2] * voxelSize[2])
   };
   
   anatomyVolume->setOrigin(origin);
   bmv->setToStandardView(0, BrainModel::VIEW_RESET);
   GuiBrainModelOpenGL::updateAllGL();
}

/**      
 * create the voxel values page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyVoxelValuesPage()
{
   pageAnatomyVoxelValues = new Page("Volume Voxel Values");
   
   //
   // Minimum and maximum voxel values
   //
   QLabel* minLabel = new QLabel("Minimum Voxel Value");
   QLabel* maxLabel = new QLabel("Maximum Voxel Value");
   anatomyVolumeVoxelMinValueLineEdit = new QLineEdit;
   anatomyVolumeVoxelMinValueLineEdit->setReadOnly(true);
   anatomyVolumeVoxelMaxValueLineEdit = new QLineEdit;
   anatomyVolumeVoxelMaxValueLineEdit->setReadOnly(true);
   QGridLayout* voxelGridLayout = new QGridLayout;
   voxelGridLayout->addWidget(minLabel, 0, 0);
   voxelGridLayout->addWidget(anatomyVolumeVoxelMinValueLineEdit, 0, 1);
   voxelGridLayout->addWidget(maxLabel, 1, 0);
   voxelGridLayout->addWidget(anatomyVolumeVoxelMaxValueLineEdit, 1, 1);
   voxelGridLayout->setColumnStretch(0, 0);
   voxelGridLayout->setColumnStretch(1, 0);
   voxelGridLayout->setColumnStretch(2, 100);
   
   //
   // Remap voxels 0 to 255 push button
   //
   QPushButton* remapVoxels0to255PushButton = new QPushButton("Remap Voxels to 0 to 255");
   remapVoxels0to255PushButton->setFixedSize(remapVoxels0to255PushButton->sizeHint());
   remapVoxels0to255PushButton->setAutoDefault(false);
   QObject::connect(remapVoxels0to255PushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeRemapVoxelValues()));
                    
   // 
   // Save volume push button
   //
   QPushButton* savePushButton = createSaveAnatomyVolumeFilePushButton();

   QVBoxLayout* layout = pageAnatomyVoxelValues->getLayout();
   layout->addLayout(voxelGridLayout);
   layout->addWidget(remapVoxels0to255PushButton);
   layout->addWidget(savePushButton);
   layout->addStretch();

   const QString msg = 
      "The voxel values must range from 0 to 255 for segmentation algorithm "
      "to successfully segment the cortex.  If the voxels are not in this range "
      "press the <B>Remap Voxels to 0 to 255</B> pushbutton.";
   pageAnatomyVoxelValues->setHelp(msg);
   
   pageStackedWidget->addWidget(pageAnatomyVoxelValues);
}

/**
 * update the voxel values page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateAnatomyVoxelValuesPage()
{
   float minValue, maxValue;
   anatomyVolume->getMinMaxVoxelValues(minValue, maxValue);
   
   anatomyVolumeVoxelMinValueLineEdit->setText(QString::number(minValue, 'f', 3));
   anatomyVolumeVoxelMaxValueLineEdit->setText(QString::number(maxValue, 'f', 3));
}

/**
 * called to remap voxel values.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeRemapVoxelValues()
{
   anatomyVolume->stretchVoxelValues();
   GuiBrainModelOpenGL::updateAllGL();
   updateAnatomyVoxelValuesPage();
}

/**      
 * create the uniformity page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyUniformityPage()
{
   pageAnatomyUniformity = new Page("Volume Non-Uniformity Correction");
   
   //
   // Load an image containing an example of a non-uniform volume
   //
   QString imageFile(imageDirectory);
   imageFile += "Non-Uniform.jpg";   
   QPixmap pix;
   if (pix.load(imageFile) == false) {
      std::cout << "WARNING: Unable to load image file: " 
                << imageFile.toAscii().constData() << std::endl;
   }

   //
   // Label containing an example of a non-uniform image
   // 
   QLabel* nonUniformMessageLabel = new QLabel(
      "The example image on the right\n"
      "has white matter that is much\n"
      "brighter in the frontal lobes\n"
      "than in the remainder of the \n"
      "brain.  A volume such as this\n"
      "example would need non-uniformity\n"
      "correction to segment successfully.");
       
   QLabel* nonUniformLabel = new QLabel("");
   nonUniformLabel->setPixmap(imageFile);
   QHBoxLayout* labelLayout = new QHBoxLayout;
   labelLayout->addWidget(nonUniformMessageLabel);
   labelLayout->addWidget(nonUniformLabel);
   labelLayout->addStretch();
   
   //
   // Non-uniformity algorithm selection
   //
   anatomyVolumeNonUniformAlgorithmAFNIRadioButton = new QRadioButton("AFNI 3duniformize (fast)");
   anatomyVolumeNonUniformAlgorithmITKRadioButton = new QRadioButton("ITK Bias Corrector (very slow)");
   QButtonGroup* algButtGroup = new QButtonGroup(this);
   //QObject::connect(algButtGroup, SIGNAL(buttonClicked(int)),
   //                 this, SLOT(slotAnatomyNonUniformityAlgorithmSelected()));
   algButtGroup->addButton(anatomyVolumeNonUniformAlgorithmAFNIRadioButton, 0);
   algButtGroup->addButton(anatomyVolumeNonUniformAlgorithmITKRadioButton, 1);
   QGroupBox* algGroupBox = new QGroupBox("Algorithm");
   QVBoxLayout* algGroupLayout = new QVBoxLayout(algGroupBox);
   algGroupLayout->addWidget(anatomyVolumeNonUniformAlgorithmAFNIRadioButton);
   algGroupLayout->addWidget(anatomyVolumeNonUniformAlgorithmITKRadioButton);
   algGroupLayout->addStretch();
   
   //
   // Non uniformity correction parameter intensity controls
   //
   anatomyVolumeNonUniformITKGrayMinSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKGrayMinSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKGrayMinSpinBox->setMaximum(255);
   anatomyVolumeNonUniformITKGrayMinSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKGrayMinSpinBox->setValue(80);
   anatomyVolumeNonUniformITKGrayMaxSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKGrayMaxSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKGrayMaxSpinBox->setMaximum(255);
   anatomyVolumeNonUniformITKGrayMaxSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKGrayMaxSpinBox->setValue(125);
   anatomyVolumeNonUniformITKWhiteMinSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKWhiteMinSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKWhiteMinSpinBox->setMaximum(255);
   anatomyVolumeNonUniformITKWhiteMinSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKWhiteMinSpinBox->setValue(140);
   anatomyVolumeNonUniformITKWhiteMaxSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKWhiteMaxSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKWhiteMaxSpinBox->setMaximum(255);
   anatomyVolumeNonUniformITKWhiteMaxSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKWhiteMaxSpinBox->setValue(190);
   anatomyVolumeNonUniformITKTheshLowSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKTheshLowSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKTheshLowSpinBox->setMaximum(255);
   anatomyVolumeNonUniformITKTheshLowSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKTheshLowSpinBox->setValue(30);
   anatomyVolumeNonUniformITKThreshHighSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKThreshHighSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKThreshHighSpinBox->setMaximum(255);
   anatomyVolumeNonUniformITKThreshHighSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKThreshHighSpinBox->setValue(255);
   QGridLayout* intensityGridLayout = new QGridLayout;
   intensityGridLayout->addWidget(new QLabel(""), 0, 0);
   intensityGridLayout->addWidget(new QLabel("Min"), 0, 1);
   intensityGridLayout->addWidget(new QLabel("Max"), 0, 2);
   intensityGridLayout->addWidget(new QLabel("Gray"), 1, 0);
   intensityGridLayout->addWidget(anatomyVolumeNonUniformITKGrayMinSpinBox, 1, 1);
   intensityGridLayout->addWidget(anatomyVolumeNonUniformITKGrayMaxSpinBox, 1, 2);
   intensityGridLayout->addWidget(new QLabel("White"), 2, 0);
   intensityGridLayout->addWidget(anatomyVolumeNonUniformITKWhiteMinSpinBox, 2, 1);
   intensityGridLayout->addWidget(anatomyVolumeNonUniformITKWhiteMaxSpinBox, 2, 2);
   intensityGridLayout->addWidget(new QLabel("Threshold"), 3, 0);
   intensityGridLayout->addWidget(anatomyVolumeNonUniformITKTheshLowSpinBox, 3, 1);
   intensityGridLayout->addWidget(anatomyVolumeNonUniformITKThreshHighSpinBox, 3, 2);
   intensityGridLayout->setColumnStretch(0, 0);
   intensityGridLayout->setColumnStretch(1, 0);
   intensityGridLayout->setColumnStretch(2, 0);
   intensityGridLayout->setColumnStretch(3, 100);

   //
   // Non uniformity correction parameter iterations controls
   //
   anatomyVolumeNonUniformITKXIterSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKXIterSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKXIterSpinBox->setMaximum(100000);
   anatomyVolumeNonUniformITKXIterSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKXIterSpinBox->setValue(25);
   anatomyVolumeNonUniformITKYIterSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKYIterSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKYIterSpinBox->setMaximum(100000);
   anatomyVolumeNonUniformITKYIterSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKYIterSpinBox->setValue(25);
   anatomyVolumeNonUniformITKZIterSpinBox = new QSpinBox;
   anatomyVolumeNonUniformITKZIterSpinBox->setMinimum(0);
   anatomyVolumeNonUniformITKZIterSpinBox->setMaximum(100000);
   anatomyVolumeNonUniformITKZIterSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformITKZIterSpinBox->setValue(25);
   QHBoxLayout* itersLayout = new QHBoxLayout;
   itersLayout->addWidget(new QLabel("Iterations"));
   itersLayout->addWidget(anatomyVolumeNonUniformITKXIterSpinBox);
   itersLayout->addWidget(anatomyVolumeNonUniformITKYIterSpinBox);
   itersLayout->addWidget(anatomyVolumeNonUniformITKZIterSpinBox);
   itersLayout->addStretch();
  
   //
   // ITK parameters widget
   //
   anatomyVolumeNonUniformITKWidget = new QWidget;
   QVBoxLayout* anatomyVolumeNonUniformITKLayout = new QVBoxLayout(anatomyVolumeNonUniformITKWidget);
   anatomyVolumeNonUniformITKLayout->addLayout(intensityGridLayout);
   anatomyVolumeNonUniformITKLayout->addWidget(new QLabel(" "));
   anatomyVolumeNonUniformITKLayout->addLayout(itersLayout);
   anatomyVolumeNonUniformITKLayout->addStretch();
   
   //
   // AFNI algorithm parameters
   //
   anatomyVolumeNonUniformAFNIGrayMinSpinBox = new QSpinBox;
   anatomyVolumeNonUniformAFNIGrayMinSpinBox->setMinimum(0);
   anatomyVolumeNonUniformAFNIGrayMinSpinBox->setMaximum(255);
   anatomyVolumeNonUniformAFNIGrayMinSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformAFNIGrayMinSpinBox->setValue(70);
   anatomyVolumeNonUniformAFNIWhiteMaxSpinBox = new QSpinBox;
   anatomyVolumeNonUniformAFNIWhiteMaxSpinBox->setMinimum(0);
   anatomyVolumeNonUniformAFNIWhiteMaxSpinBox->setMaximum(255);
   anatomyVolumeNonUniformAFNIWhiteMaxSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformAFNIWhiteMaxSpinBox->setValue(210);
   anatomyVolumeNonUniformAFNIIterationsSpinBox = new QSpinBox;
   anatomyVolumeNonUniformAFNIIterationsSpinBox->setMinimum(0);
   anatomyVolumeNonUniformAFNIIterationsSpinBox->setMaximum(255);
   anatomyVolumeNonUniformAFNIIterationsSpinBox->setSingleStep(1);
   anatomyVolumeNonUniformAFNIIterationsSpinBox->setValue(5);
   
   //
   // AFNI parameters widget
   //
   anatomyVolumeNonUniformAFNIWidget = new QWidget;
   QGridLayout* afniLayout = new QGridLayout(anatomyVolumeNonUniformAFNIWidget);
   afniLayout->addWidget(new QLabel("Gray Min"), 0, 0);
   afniLayout->addWidget(anatomyVolumeNonUniformAFNIGrayMinSpinBox, 0, 1);
   afniLayout->addWidget(new QLabel("White Max"), 1, 0);
   afniLayout->addWidget(anatomyVolumeNonUniformAFNIWhiteMaxSpinBox, 1, 1);
   afniLayout->addWidget(new QLabel("Iterations"), 2, 0);
   afniLayout->addWidget(anatomyVolumeNonUniformAFNIIterationsSpinBox, 2, 1);
   anatomyVolumeNonUniformAFNIWidget->setFixedSize(anatomyVolumeNonUniformAFNIWidget->sizeHint());
   
   //
   // Widget stack for algorithm parameters
   //
   anatomyVolumeNonUniformStackedWidget = new QStackedWidget;
   anatomyVolumeNonUniformStackedWidget->addWidget(anatomyVolumeNonUniformAFNIWidget);
   anatomyVolumeNonUniformStackedWidget->addWidget(anatomyVolumeNonUniformITKWidget);
   
   //
   // Run non-uniformity correction
   //
   QPushButton* runNonUniformPushButton = new QPushButton("Run Non-Uniformity Correction");
   runNonUniformPushButton->setAutoDefault(false);
   runNonUniformPushButton->setFixedSize(runNonUniformPushButton->sizeHint());
   QObject::connect(runNonUniformPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeNonUniformityCorrection()));

   //
   // Correction group
   //
   QGroupBox* correctionGroupBox = new QGroupBox("Non-Uniformity Correction");
   QVBoxLayout* correctionGroupLayout = new QVBoxLayout(correctionGroupBox);
   correctionGroupLayout->addWidget(algGroupBox);
   correctionGroupLayout->addWidget(anatomyVolumeNonUniformStackedWidget);
   correctionGroupLayout->addWidget(runNonUniformPushButton);
   
   //
   // Histogram button
   //
   QPushButton* histogramPushButton = new QPushButton("Histogram...");
   histogramPushButton->setAutoDefault(false);
   histogramPushButton->setFixedSize(histogramPushButton->sizeHint());
   QObject::connect(histogramPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAnatomyVolumeShowHistogram()));
                    
   // 
   // Save volume push button
   //
   QPushButton* savePushButton = createSaveAnatomyVolumeFilePushButton();

   QVBoxLayout* layout = pageAnatomyUniformity->getLayout();
   layout->addLayout(labelLayout);
   layout->addWidget(correctionGroupBox);
   layout->addWidget(histogramPushButton);
   layout->addWidget(savePushButton);
   layout->addStretch();
   
   const QString msg = 
      "If the intensity scale is not uniform throughout the volume, the segmentation "
      "algorithm will not function correctly.<P>"
      "Viewing a histogram may assist in determing the gray, white, and theshold values.<P>"
      "To Perform Non-Uniformity Correction with the AFNI Algorithm:"
      "<OL>"
      "<LI>Find the minimum gray matter value and enter as Gray Min"
      "<LI>Find the maximum white matter value and enter as Gray Max"
      "<LI>Press the Run Non-Uniformity Correction push button."
      "</OL>"
      "To Perform Non-Uniformity Correction with the ITK Algorithm:"
      "<OL>"
      "<LI>Find intensities that are clearly less than the gray matter and clearly greater "
           "than the white matter and enter them as Threshold Min and Max."
      "<LI>Find the range of gray matter values and enter them as Gray Min and Gray Max."
      "<LI>Find the range of white matter values and enter them as White Min and White Max."
      "<LI>For volumes with substantial non-uniformity problems, increase the number "
           "of iterations."
      "<LI>Press the Run Non-Uniformity Correction push button.  The process may take "
           "a while (20 minutes) to run."
      "</OL>"
      "More information about the non-uniformity correction algorithm may be obtained from:"
      "<UL>"
      "<LI>http://www.itk.org/Doxygen16/html/classitk_1_1MRIBiasFieldCorrectionFilter.html"
      "<LI>Parametric estimate of intensity inhomogeneities applied to MRI Martin Styner, G. Gerig, Christian Brechbuehler, Gabor Szekely, IEEE TRANSACTIONS ON MEDICAL IMAGING; 19(3), pp. 153-165, 2000, (http://www.cs.unc.edu/~styner/docs/tmi00.pdf)"
      "<LI>Evaluation of 2D/3D bias correction with 1+1ES-optimization Martin Styner, Prof. Dr. G. Gerig (IKT, BIWI, ETH Zuerich), TR-197 (http://www.cs.unc.edu/~styner/docs/StynerTR97.pdf)"
      "</UL>";
   pageAnatomyUniformity->setHelp(msg);
   
   pageStackedWidget->addWidget(pageAnatomyUniformity);

   //
   // Connect the algorithm selection button group to the widget stack
   //
   QObject::connect(algButtGroup, SIGNAL(buttonClicked(int)),
                    anatomyVolumeNonUniformStackedWidget, SLOT(setCurrentIndex(int)));
                    
   //
   // Default the algorithm
   //
   if (haveAfni3dUniformizeProgramFlag) {
      anatomyVolumeNonUniformAlgorithmAFNIRadioButton->setChecked(true);
   }
   else {
      anatomyVolumeNonUniformAlgorithmAFNIRadioButton->setEnabled(false);
   } 
   
#ifdef HAVE_ITK
   if (anatomyVolumeNonUniformAlgorithmAFNIRadioButton->isChecked() == false) {
      anatomyVolumeNonUniformAlgorithmITKRadioButton->setChecked(true);
   }
#else  // HAVE_ITK
      anatomyVolumeNonUniformAlgorithmITKRadioButton->setEnabled(false);
#endif // HAVE_ITK
}

/**
 * called to show a histogram for the anatomy volume.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeShowHistogram()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   const int numVoxels = anatomyVolume->getTotalNumberOfVoxels();
   std::vector<float> values(numVoxels);
   float minValue =  std::numeric_limits<float>::max();
   float maxValue = -std::numeric_limits<float>::max();
   for (int i = 0; i < numVoxels; i++) {
      values[i] = anatomyVolume->getVoxelWithFlatIndex(i, 0);
      minValue = std::min(minValue, values[i]);
      maxValue = std::max(maxValue, values[i]);
   } 
   GuiHistogramDisplayDialog* ghd = new GuiHistogramDisplayDialog(this, 
                                          FileUtilities::basename(anatomyVolume->getFileName()),
                                          values,
                                          true,
                                          false);
   ghd->show();
   
   QApplication::restoreOverrideCursor();
}

/**
 * called to perform non-uniformity correction on the anatomy volume.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyVolumeNonUniformityCorrection()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   if (anatomyVolumeNonUniformAlgorithmAFNIRadioButton->isChecked()) {
      try {
         anatomyVolume->afniUniformityCorrection(
                         anatomyVolumeNonUniformAFNIGrayMinSpinBox->value(),
                         anatomyVolumeNonUniformAFNIWhiteMaxSpinBox->value(),
                         anatomyVolumeNonUniformAFNIIterationsSpinBox->value());
      }
      catch (FileException& e) {
         GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      }
   }
   else if (anatomyVolumeNonUniformAlgorithmITKRadioButton->isChecked()) {
      const float grayWhite[4] = {
         anatomyVolumeNonUniformITKGrayMinSpinBox->value(),
         anatomyVolumeNonUniformITKGrayMaxSpinBox->value(),
         anatomyVolumeNonUniformITKWhiteMinSpinBox->value(),
         anatomyVolumeNonUniformITKWhiteMaxSpinBox->value()
      };
      const float thresholds[2] = {
         anatomyVolumeNonUniformITKTheshLowSpinBox->value(),
         anatomyVolumeNonUniformITKThreshHighSpinBox->value()
      };
      const int iterations[3] = {
         anatomyVolumeNonUniformITKXIterSpinBox->value(),
         anatomyVolumeNonUniformITKYIterSpinBox->value(),
         anatomyVolumeNonUniformITKZIterSpinBox->value()
      };
      
      try {
         BrainModelVolumeBiasCorrection biasCorrector(anatomyVolume, 
                                                      grayWhite,
                                                      thresholds,
                                                      iterations);
         biasCorrector.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      }
   }
   
   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
}
      
/**      
 * create the crop mask page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyCropMaskPage()
{
   pageAnatomyCropMask = new Page("Volume Cropping");
   //QVBoxLayout* layout = pageCropMask->getLayout();
   
   pageStackedWidget->addWidget(pageAnatomyCropMask);
}

/**      
 * create the strip page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyStripPage()
{
   pageAnatomyStrip = new Page("Volume Non-Cortical Stripping");
   //QVBoxLayout* layout = pageStrip->getLayout();
   
   pageStackedWidget->addWidget(pageAnatomyStrip);
}

/**      
 * create the histogram page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAnatomyHistogramPage()
{
   //
   // Histogram page
   //
   pageAnatomyHistogram = new Page("Volume Histogram");
   
   //
   // Create the histogram widget and group box
   //
   anatomyPeakHistogramWidget = new GuiGraphWidget(0, "");
   QGroupBox* histoGroupBox = new QGroupBox("Histogram");
   QVBoxLayout* histoGroupLayout = new QVBoxLayout(histoGroupBox);
   histoGroupLayout->addWidget(anatomyPeakHistogramWidget);
   
   //
   // Gray peak controls
   //
   anatomyGrayPeakRadioButton = new QRadioButton("Gray");
   anatomyGrayPeakRadioButton->setToolTip(
                 "When this item is selected, anatomy volume voxels\n"
                 "greater than the gray peak threshold will be \n"
                 "colored in green.");
   anatomyGrayPeakSpinBox = new QSpinBox;
   anatomyGrayPeakSpinBox->setMinimum(0);
   anatomyGrayPeakSpinBox->setMaximum(255);
   anatomyGrayPeakSpinBox->setSingleStep(1);
   QObject::connect(anatomyGrayPeakSpinBox, SIGNAL(valueChanged(int)),
                    anatomyPeakHistogramWidget, SLOT(slotSetMinimumPeak(int)));
   QObject::connect(anatomyGrayPeakSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotAnatomyPeakRadioButton()));

   //
   // White peak controls
   //
   anatomyWhitePeakRadioButton = new QRadioButton("White");
   anatomyWhitePeakRadioButton->setToolTip(
                 "When this item is selected, anatomy volume voxels\n"
                 "greater than the white peak threshold will be \n"
                 "colored in green.");
   anatomyWhitePeakSpinBox = new QSpinBox;
   anatomyWhitePeakSpinBox->setMinimum(0);
   anatomyWhitePeakSpinBox->setMaximum(255);
   anatomyWhitePeakSpinBox->setSingleStep(1);
   QObject::connect(anatomyWhitePeakSpinBox, SIGNAL(valueChanged(int)),
                    anatomyPeakHistogramWidget, SLOT(slotSetMaximumPeak(int)));
   QObject::connect(anatomyWhitePeakSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotAnatomyPeakRadioButton()));
    
   //
   // Button group for gray/white push buttons
   //
   QButtonGroup* grayWhiteButtonGroup = new QButtonGroup(this);
   QObject::connect(grayWhiteButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotAnatomyPeakRadioButton()));
   grayWhiteButtonGroup->addButton(anatomyGrayPeakRadioButton, 0);
   grayWhiteButtonGroup->addButton(anatomyWhitePeakRadioButton, 1);

   //
   // Override peaks control
   //
   anatomyOverrideThreshSetCheckBox = new QCheckBox("Override");
   anatomyOverrideThreshSetCheckBox->setToolTip(
                 "Enable this item to override the average\n"
                 "of the gray and white matter peaks.");
   anatomyOverridePeakSpinBox = new QSpinBox;
   anatomyOverridePeakSpinBox->setMinimum(0);
   anatomyOverridePeakSpinBox->setMaximum(255);
   anatomyOverridePeakSpinBox->setSingleStep(1);
   QObject::connect(anatomyOverrideThreshSetCheckBox, SIGNAL(toggled(bool)),
                    anatomyOverridePeakSpinBox, SLOT(setEnabled(bool)));
   anatomyOverridePeakSpinBox->setToolTip(
                 "Enter the value for overriding the average\n"
                 "of the gray and white matter peaks here.");
   
   //
   // Peaks group box and layout
   //
   QGroupBox* peaksGroupBox = new QGroupBox("Peaks");
   QGridLayout* peaksGroupLayout = new QGridLayout(peaksGroupBox);
   peaksGroupLayout->addWidget(anatomyGrayPeakRadioButton, 0, 0);
   peaksGroupLayout->addWidget(anatomyGrayPeakSpinBox, 0, 1);
   peaksGroupLayout->addWidget(anatomyWhitePeakRadioButton, 1, 0);
   peaksGroupLayout->addWidget(anatomyWhitePeakSpinBox, 1, 1);
   peaksGroupLayout->addWidget(anatomyOverrideThreshSetCheckBox, 2, 0);
   peaksGroupLayout->addWidget(anatomyOverridePeakSpinBox, 2, 1);
   peaksGroupBox->setFixedSize(peaksGroupBox->sizeHint());
   
   //
   // Layout page
   //
   QHBoxLayout* pageLayout = new QHBoxLayout;
   pageLayout->addWidget(histoGroupBox);
   pageLayout->addWidget(peaksGroupBox);
   pageLayout->addStretch();
   QVBoxLayout* layout = pageAnatomyHistogram->getLayout();
   layout->addLayout(pageLayout);
   layout->addStretch();
   pageStackedWidget->addWidget(pageAnatomyHistogram);
}

/**
 * upate the anatomy histogram page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateAnatomyHistogramPage() 
{
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   int gray = 0;
   int white = 0;
   pf->getParameter(ParamsFile::keyCGMpeak, gray);
   pf->getParameter(ParamsFile::keyWMpeak, white);

   int peakOverride = 0;
   pf->getParameter(ParamsFile::keyWMThreshSet, peakOverride);
   anatomyOverridePeakSpinBox->setValue(peakOverride);
   anatomyOverrideThreshSetCheckBox->setChecked(peakOverride != 0);
   anatomyOverridePeakSpinBox->setEnabled(anatomyOverrideThreshSetCheckBox->isChecked());

   //
   // Clear all data from the graph
   //
   anatomyPeakHistogramWidget->removeAllData();
   
   //
   // Put data in histogram widget
   //
   StatisticHistogram* hist = anatomyVolume->getHistogram();
   const int numBuckets = hist->getNumberOfBuckets();
   std::vector<double> dataX, dataY;
   for (int j = 0; j < numBuckets; j++) {
      float x, y;
      hist->getDataForBucket(j, x, y);
      if (j == 0) {
         if (x > 0.0) {
            dataX.push_back(0.0);
            dataY.push_back(0.0);
         }
      }
      dataX.push_back(x);
      dataY.push_back(y);
      if (j == (numBuckets - 1)) {
         if (x < 255.0) {
            dataX.push_back(255.0);
            dataY.push_back(0.0);
         }
      }
   }
   anatomyPeakHistogramWidget->addData(dataX, dataY, 
                                QColor(255, 0, 0), 
                                GuiGraphWidget::DRAW_DATA_TYPE_BARS);

   //
   // If no peaks yet
   //
   if ((gray == 0) && (white == 0)) {
      //
      // Estimate the peaks
      //
      int grayBucket, whiteBucket, grayMin, whiteMax, grayWhite, csfBucket;
      hist->getGrayWhitePeakEstimates(grayBucket, whiteBucket, grayMin, whiteMax, grayWhite, csfBucket);
      if (grayBucket >= 0) {
         float x, y;
         hist->getDataForBucket(grayBucket, x, y);
         gray = static_cast<int>(x);
         hist->getDataForBucket(whiteBucket, x, y);
         white = static_cast<int>(x);
      }
   }
   
   //
   // Set the controls
   //
   anatomyGrayPeakSpinBox->setValue(gray);
   anatomyWhitePeakSpinBox->setValue(white);
   
   //
   // Update histogram widget
   //
   anatomyPeakHistogramWidget->slotSetMinimumPeak(anatomyGrayPeakSpinBox->value());
   anatomyPeakHistogramWidget->slotSetMaximumPeak(anatomyWhitePeakSpinBox->value());
   
   //
   // Initialize graph scaling
   //
   bool initializeGraphScaleFlag = false;
   if (initializeGraphScaleFlag) {
      const float minX = gray - 5.0;
      const float maxX = white + 10.0;
      initializeGraphScaleFlag = false;
      float theMax = -1.0;
      for (int j = 0; j < numBuckets; j++) {
         float x, y;
         hist->getDataForBucket(j, x, y);
         if ((x >= minX) && (x <= maxX)) {
            theMax = std::max(theMax, y);
         }
      }
      if (theMax > 0.0) {
         anatomyPeakHistogramWidget->setScaleYMaximum(theMax * 1.75);
      }
   }
   
   //
   // No longer need histogram
   //
   delete hist;
   hist = NULL;
   
   //
   // For volume coloring
   //
   if ((anatomyWhitePeakRadioButton->isChecked() == false) && 
       (anatomyGrayPeakRadioButton->isChecked() == false)) {
      anatomyGrayPeakRadioButton->setChecked(true);
   }
   slotAnatomyPeakRadioButton();
}     

/**
 * called when gray or white peak radio button selected.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotAnatomyPeakRadioButton()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   if (anatomyGrayPeakRadioButton->isChecked()) {
      dsv->setAnatomyThreshold(anatomyGrayPeakSpinBox->value());
      dsv->setAnatomyThresholdValid(true);
   }
   else if (anatomyWhitePeakRadioButton->isChecked()) {
      dsv->setAnatomyThreshold(anatomyWhitePeakSpinBox->value());
      dsv->setAnatomyThresholdValid(true);
   }
   else {
      dsv->setAnatomyThresholdValid(false);
   }
   
   GuiBrainModelOpenGL::updateAllGL();
}

/**      
 * create the segmentation operations page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createSegmentationOperationsPage()
{
   pageSegmentationOperations = new Page("Segmentation Operations");
   //QVBoxLayout* layout = pageSegmentationOperations->getLayout();
   
   pageStackedWidget->addWidget(pageSegmentationOperations);
}

/**      
 * create the surface operations page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAllSurfaceOperationsPage()
{
   pageAllSurfaceOperations = new Page("Surface Operations");
   //QVBoxLayout* layout = pageAllSurfaceOperations->getLayout();
   
   pageStackedWidget->addWidget(pageAllSurfaceOperations);
}

/**      
 * create the run page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAllRunPage()
{
   pageAllRun = new Page("Run SureFit Algorithm");
   //QVBoxLayout* layout = pageRun->getLayout();
   
   pageStackedWidget->addWidget(pageAllRun);
}

/**      
 * create the progress page
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::createAllProgressPage()
{
   pageAllProgress = new Page("SureFit Progress");
   //QVBoxLayout* layout = pageProgress->getLayout();
   
   pageStackedWidget->addWidget(pageAllProgress);
}
      
/**
 * update the dialog.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::updateDialog()
{
   slotEnableDisableButtons();
}

/**
 * show the page.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::showPage(const int pageIndex)
{
   const Page* previousPage = dynamic_cast<const Page*>(pageStackedWidget->currentWidget());
   
   pageStackedWidget->setCurrentIndex(pageIndex);
   
   const Page* page = dynamic_cast<const Page*>(pageStackedWidget->currentWidget());
   
   if (page == pageAllUnsavedFiles) {
      updateAllUnsavedFilesPage();
   }
   
   if (page == pageAllVolumeFileType) {
      updateAllVolumeFileTypePage();
   }

   if (page == pageSegmentationChooseFiles) {
      updateSegmentationChooseFilesPage();
   }
   
   if (page == pageAnatomyChooseFile) {
      anatomyVolume = NULL;
      anatomyFileNameLineEdit->setText("");
   }
   
   if (page == pageAnatomyChooseSpec) {
      updateAnatomyChooseSpecPage();
   }
   
   if (page == pageAnatomyCreateSpec) {
      anatomyCreateLeftSpecFileGroupBox->setEnabled(false);
      anatomyCreateRightSpecFileGroupBox->setEnabled(false);
      anatomyCreateCerebellumSpecFileGroupBox->setEnabled(false);
      
      if (leftAnatomyCreateSpecFileRadioButton->isChecked()) {
         QString name = anatomySpeciesLineEdit->text()
                      + "."
                      + anatomySubjectLineEdit->text()
                      + "."
                      + "L"
                      + SpecFile::getSpecFileExtension();
         anatomyCreateLeftSpecFileLineEdit->setText(name);
         anatomyCreateLeftSpecFileGroupBox->setEnabled(true);
      }

      if (rightAnatomyCreateSpecFileRadioButton->isChecked()) {
         QString name = anatomySpeciesLineEdit->text()
                      + "."
                      + anatomySubjectLineEdit->text()
                      + "."
                      + "R"
                      + SpecFile::getSpecFileExtension();
         anatomyCreateRightSpecFileLineEdit->setText(name);
         anatomyCreateRightSpecFileGroupBox->setEnabled(true);
      }

      if (cerebellumAnatomyCreateSpecFileRadioButton->isChecked()) {
         QString name = anatomySpeciesLineEdit->text()
                      + "."
                      + anatomySubjectLineEdit->text()
                      + "."
                      + "LR"
                      + SpecFile::getSpecFileExtension();
         anatomyCreateCerebellumSpecFileLineEdit->setText(name);
         anatomyCreateCerebellumSpecFileGroupBox->setEnabled(true);
      }
   }
   
   if (page == pageAnatomyOrientation) {
      updateAnatomyOrientationPage();
   }
   if (page == pageAnatomyVoxelSize) {
      updateAnatomyVoxelSizePage();
   }
   
   if (page == pageAnatomyVoxelValues) {
      updateAnatomyVoxelValuesPage();
   }
   if (page == pageAnatomyHistogram) {
      updateAnatomyHistogramPage();
   }
   
   if (previousPage == pageAnatomyHistogram) {
      DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
      dsv->setAnatomyThresholdValid(false);
      GuiBrainModelOpenGL::updateAllGL();
   }
   
   QString html("<HTML><BODY>");
   html.append(page->getHelp());
   html.append("</BODY></HTML>");
   helpBrowser->setHtml(html);
   slotEnableDisableButtons();
}

/**
 * called when "Prev" pushbutton is pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotPrevPushButton()
{
   Page* currentPage = dynamic_cast<Page*>(pageStackedWidget->currentWidget());

   //
   // Is this the choose anatomy or segmentation files
   //
   if ((currentPage == pageSegmentationChooseFiles) ||
       (currentPage == pageAnatomyChooseFile)) {
      showPage(pageStackedWidget->indexOf(pageAllTask));
   }
   //
   // Is this the segmentation operations page and operating with segmentation volumes
   //
   else if ((currentPage == pageSegmentationOperations) &&
            taskSegmentationVolumeRadioButton->isChecked()) {
      showPage(pageStackedWidget->indexOf(pageSegmentationChooseFiles));
   }
   //
   // all other pages
   //
   else {
      const int prevIndex = pageStackedWidget->currentIndex() - 1;
      if (prevIndex >= 0) {
         showPage(prevIndex);
      }
   }
}

/**
 * called when "Next" pushbutton is pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotNextPushButton()
{
   Page* currentPage = dynamic_cast<Page*>(pageStackedWidget->currentWidget());

   //
   // If current page is unsaved files page
   //
   if (currentPage == pageAllUnsavedFiles) {
   }
   
   //
   // Read the current page selections
   //
   if (currentPage == pageAllVolumeFileType) {
      readAllVolumeFileTypePage();
   }
   
   //
   // Get index of next page
   //
   int nextIndex = pageStackedWidget->currentIndex() + 1;
   
   if (currentPage == pageAllTask) {
      if (taskAnatomicalVolumeRadioButton->isChecked()) {
         nextIndex = pageStackedWidget->indexOf(pageAnatomyChooseFile);
      }
      else if (taskSegmentationVolumeRadioButton->isChecked()) {
         nextIndex = pageStackedWidget->indexOf(pageSegmentationChooseFiles);
      }
      else {
         nextIndex = pageStackedWidget->indexOf(currentPage);
      }
   }
   
   if (currentPage == pageAnatomyChooseSpec) {
      if ((leftAnatomyCreateSpecFileRadioButton->isChecked() == false) &&
          (rightAnatomyCreateSpecFileRadioButton->isChecked() == false) &&
          (cerebellumAnatomyCreateSpecFileRadioButton->isChecked() == false)) {
         nextIndex = pageStackedWidget->indexOf(pageAnatomyCropMask);
      }
   }
   
   //
   // Show the next page
   //
   if (nextIndex < pageStackedWidget->count()) {
      showPage(nextIndex);
   }
}


/**
 * called when "Close" pushbutton is pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotClosePushButton()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setAnatomyThresholdValid(false);
   
   QtDialog::close();
}

/**
 * called when "Start" pushbutton is pressed.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotStartPushButton()
{
   slotNextPushButton();
}

/**
 * enable/disable buttons.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::slotEnableDisableButtons()
{
   const Page* page = dynamic_cast<Page*>(pageStackedWidget->currentWidget());
   
   bool nextValid = false;
   bool startValid = false;
   
   anatomicalMriType = ANATOMICAL_MRI_TYPE_UNKNOWN;
   if (anatomyMriTypeT1RadioButton->isChecked()) {
      anatomicalMriType = ANATOMICAL_MRI_TYPE_T1;
   }
   
   if (page == pageAllUnsavedFiles) {
      nextValid = true;
   }
   else if (page == pageAllInstructions) {
      nextValid = true;
   }
   else if (page == pageAllVolumeFileType) {
      nextValid = (volumeFileTypeAfniRadioButton->isChecked()  ||
                   volumeFileTypeNiftiRadioButton->isChecked() ||
                   volumeFileTypeSpmRadioButton->isChecked()   ||
                   volumeFileTypeWuNilRadioButton->isChecked());
   }
   else if (page == pageAllTask) {
      const bool anatValid = taskAnatomicalVolumeRadioButton->isChecked() &&
                             (taskAnatomyProcessLeftCheckBox->isChecked() ||
                              taskAnatomyProcessRightCheckBox->isChecked() ||
                              taskAnatomyProcessCerebellumCheckBox->isChecked());
      const bool segValid =  taskSegmentationVolumeRadioButton->isChecked();
      nextValid = anatValid || segValid;
   }
   else if (page == pageSegmentationChooseFiles) {
      nextValid = ((leftSegmentationVolume != NULL) ||
                   (rightSegmentationVolume != NULL) ||
                   (cerebellumSegmentationVolume != NULL));
   }  
   else if (page == pageAnatomyChooseFile) {
      nextValid = (anatomyVolume != NULL);
   }
   else if (page == pageAnatomyMriType) {
      nextValid = (anatomicalMriType != ANATOMICAL_MRI_TYPE_UNKNOWN);
   }
   else if (page == pageAnatomyChooseSpec) {
      const bool leftValid = leftAnatomyCreateSpecFileRadioButton->isChecked() ||
                             (leftAnatomyUseSpecFileRadioButton->isChecked() &&
                              (leftAnatomySpecFileLineEdit->text().isEmpty() == false));
      const bool rightValid = rightAnatomyCreateSpecFileRadioButton->isChecked() ||
                             (rightAnatomyUseSpecFileRadioButton->isChecked() &&
                              (rightAnatomySpecFileLineEdit->text().isEmpty() == false));
      const bool cerebellumValid = cerebellumAnatomyCreateSpecFileRadioButton->isChecked() ||
                             (cerebellumAnatomyUseSpecFileRadioButton->isChecked() &&
                              (cerebellumAnatomySpecFileLineEdit->text().isEmpty() == false));
      nextValid = (leftValid || rightValid || cerebellumValid);
   }
   else if (page == pageAnatomySubjectInformation) {
      nextValid = (anatomySpeciesLineEdit->text().isEmpty() == false) &&
                  (anatomySubjectLineEdit->text().isEmpty() == false) &&
                  (anatomyCategoryLineEdit->text().isEmpty() == false);
   }
   else if (page == pageAnatomyCreateSpec) {
      bool leftOK = true;
      if (anatomyCreateLeftSpecFileGroupBox->isEnabled()) {
         if (anatomyCreateLeftSpecFileLineEdit->text().isEmpty()) {
            leftOK = false;
         }
      }
      bool rightOK = true;
      if (anatomyCreateRightSpecFileGroupBox->isEnabled()) {
         if (anatomyCreateRightSpecFileLineEdit->text().isEmpty()) {
            rightOK = false;
         }
      }
      bool cerebellumOK = true;
      if (anatomyCreateCerebellumSpecFileGroupBox->isEnabled()) {
         if (anatomyCreateCerebellumSpecFileLineEdit->text().isEmpty()) {
            cerebellumOK = false;
         }
      }
      nextValid = (leftOK || leftOK || cerebellumOK);
   }
   else if (page == pageAnatomyStandardSpace) {
      nextValid = true;
   }
   else if (page == pageAnatomyOrientation) {
      nextValid = ((anatomyVolumeXOrientationComboBox->getOrientation() == VolumeFile::ORIENTATION_LEFT_TO_RIGHT) &&
                   (anatomyVolumeYOrientationComboBox->getOrientation() == VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR) &&
                   (anatomyVolumeZOrientationComboBox->getOrientation() == VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR));
   }
   else if (page == pageAnatomyVoxelSize) {
      nextValid = true;
   }
   else if (page == pageAnatomyOrigin) {
      nextValid = true;
   }
   else if (page == pageAnatomyVoxelValues) {
      const float minVoxel = anatomyVolumeVoxelMinValueLineEdit->text().toFloat();
      const float maxVoxel = anatomyVolumeVoxelMaxValueLineEdit->text().toFloat();
      nextValid = ((minVoxel >= 0.0) && (minVoxel <= 20.0)) &&
                  ((maxVoxel >= 240.0) && (maxVoxel <= 255.0));
   }
   else if (page == pageAnatomyUniformity) {
      nextValid = true;
   }
   else if (page == pageAnatomyCropMask) {
      nextValid = true;
   }
   else if (page == pageAnatomyStrip) {
      nextValid = true;
   }
   else if (page == pageAnatomyHistogram) {
      nextValid = true;
   }
   else if (page == pageSegmentationOperations) {
      nextValid = true;
   }
   else if (page == pageAllSurfaceOperations) {
      nextValid = true;
   }
   else if (page == pageAllRun) {
      startValid = true;
   }
   else if (page == pageAllProgress) {
   }
   
   if (nextPushButton != NULL) {
      nextPushButton->setEnabled(nextValid);
      startPushButton->setEnabled(startValid);
   }
}
      
/**
 * check for external programs.
 */
void 
GuiVolumeMultiHemSureFitSegmentationDialog::checkForExternalPrograms()
{
   haveAfni3dUniformizeProgramFlag = SystemUtilities::externalProgramExists("3duniformize");
}
      

//-----------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiVolumeMultiHemSureFitSegmentationDialog::Page::Page(const QString& pageTitle)
   : QWidget(0)
{
   //
   // Place a scrolled area inside a group box
   //
   QWidget* pageWidget = new QWidget;
   pageLayout = new QVBoxLayout(pageWidget);
   QScrollArea* pageScrollArea = new QScrollArea;
   pageScrollArea->setWidget(pageWidget);
   pageScrollArea->setWidgetResizable(true);
   QGroupBox* pageGroupBox = new QGroupBox(pageTitle);
   QVBoxLayout* pageGroupBoxLayout = new QVBoxLayout(pageGroupBox);
   pageGroupBoxLayout->addWidget(pageScrollArea);

/*
   QGroupBox* pageGroupBox = new QGroupBox(pageTitle);
   pageLayout = new QVBoxLayout(pageGroupBox);
*/
   //
   // Attach the group box to this page
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(pageGroupBox);
}

/**
 * destructor.
 */
GuiVolumeMultiHemSureFitSegmentationDialog::Page::~Page()
{
}


