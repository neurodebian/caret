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

#include <set>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>

#include "FileUtilities.h"
#include "GuiChooseSpecFileDialog.h"
#include "GuiMapFmriAtlasDialog.h"
#include "GuiMessageBox.h"
#include "PreferencesFile.h"
#include "QtUtilities.h"
#include "StringUtilities.h"
#include "SpecFile.h"

/**
 * Constructor.
 */
GuiMapFmriAtlasDialog::GuiMapFmriAtlasDialog(QWidget* parent,
                                  std::vector<MapFmriAtlasSpecFileInfo>* atlasesIn,
                                  PreferencesFile* pf,
                                  const QString& speciesIn,
                                  const QString& structureNameIn,
                                  const bool showOutputSpecFileSelection,
                                  const bool enableFiducialOptionsIn)
   : QtDialog(parent, true)
{
   preferencesFile = pf;
   allAtlases = atlasesIn;
   
   space = "";
   species = StringUtilities::makeLowerCase(speciesIn);
   structureName = StringUtilities::makeLowerCase(structureNameIn);
   
   setWindowTitle("Atlas Surface Selection");
      
   //
   // Spec File selection
   //
   QPushButton* selectSpecPushButton = new QPushButton("Select...");
   selectSpecPushButton->setAutoDefault(false);
   selectSpecPushButton->setFixedSize(selectSpecPushButton->sizeHint());
   QObject::connect(selectSpecPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectSpecPushButton()));
   specFileLineEdit = new QLineEdit;
   specFileLineEdit->setReadOnly(true);
   specFileLineEdit->setMinimumWidth(250);
   
   //
   // Spec File group box and layout
   //
   QGroupBox* specGroupBox = new QGroupBox("Output Spec File");
   QHBoxLayout* specGroupLayout = new QHBoxLayout(specGroupBox);
   specGroupLayout->addWidget(selectSpecPushButton);
   specGroupLayout->addWidget(specFileLineEdit);
   specGroupBox->setHidden(showOutputSpecFileSelection == false);

   //
   // Atlas space selection
   //
   QLabel* spaceLabel = new QLabel("Space ");
   atlasSpaceComboBox = new QComboBox;
   QObject::connect(atlasSpaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotAtlasSpaceComboBox(int)));
                       
   //
   // List atlases with matching characteristics
   //
   QLabel* atlasLabel = new QLabel("Atlas ");
   atlasComboBox = new QComboBox;
   QObject::connect(atlasComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotAtlasComboBox(int)));
   
   //
   // Atlas group box and layout
   //
   QGroupBox* atlasGroupBox = new QGroupBox("Mapping Atlas");
   QGridLayout* atlasGridLayout = new QGridLayout(atlasGroupBox);
   atlasGridLayout->addWidget(spaceLabel, 0, 0);
   atlasGridLayout->addWidget(atlasSpaceComboBox, 0, 1);
   atlasGridLayout->addWidget(atlasLabel, 1, 0);
   atlasGridLayout->addWidget(atlasComboBox, 1, 1);
   atlasGridLayout->setColumnStretch(0, 0);
   atlasGridLayout->setColumnStretch(1, 100);
   
   //
   // multi-fid option check boxes
   //
   multiFidAvgFidCheckBox = new QCheckBox("Show Mapping to Averaged Fiducial Surface");
   multiFidAvgFidCheckBox->setChecked(true);
   multiFidAvgAllCasesCheckBox = new QCheckBox("Show Average of Mapping to All Multi-Fiducial Cases");
   multiFidAvgAllCasesCheckBox->setChecked(true);
   multiFidStdDevAllCheckBox = new QCheckBox("Show Sample Standard Deviation of Mapping to All Multi-Fiducial Cases");
   multiFidStdErrorAllCheckBox = new QCheckBox("Show Standard Error of Mapping to All Multi-Fiducial Cases");
   multiFidMinAllCheckBox = new QCheckBox("Show Minimum of Mapping to All Multi-Fiducial Cases");
   multiFidMaxAllCheckBox = new QCheckBox("Show Maximum of Mapping to All Multi-Fiducial Cases");
   multiFidAllCasesCheckBox = new QCheckBox("Show Mapping to Each Multi-Fiducial Case");
      
   //
   // Group box and layout for multi-fiducial options
   //
   multiFiducialGroupBox = new QGroupBox("Multi-Fiducial Mapping Metric Output");
   multiFiducialGroupBox->setEnabled(false);
   QVBoxLayout* multiFiducialLayout = new QVBoxLayout(multiFiducialGroupBox);
   multiFiducialLayout->addWidget(multiFidAvgFidCheckBox);
   multiFiducialLayout->addWidget(multiFidAvgAllCasesCheckBox);
   multiFiducialLayout->addWidget(multiFidStdDevAllCheckBox);
   multiFiducialLayout->addWidget(multiFidStdErrorAllCheckBox);
   multiFiducialLayout->addWidget(multiFidMinAllCheckBox);
   multiFiducialLayout->addWidget(multiFidMaxAllCheckBox);
   multiFiducialLayout->addWidget(multiFidAllCasesCheckBox);
   
   //
   // User instructions shown in text editor
   //
   QString instructions = 
                   "First, choose the Spec File in which the generated metric files "
                   "should be placed.  Second, choose the appropriate stereotaxic "
                   "space.  Third, choose one atlas.";                   
   if (enableFiducialOptionsIn) {
      instructions +=
                   "If the atlas contains more than one fiducial coordinate \n"
                   "file, the multi-fiducial mapping options will be displayed.";
   }   
   QTextEdit* textEditor = new QTextEdit;
   textEditor->setReadOnly(true);
   textEditor->setPlainText(instructions);
   
   //
   // OK and Cancel buttons
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(okButton);
   buttonsLayout->addWidget(cancelButton);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(specGroupBox);
   dialogLayout->addWidget(atlasGroupBox);
   dialogLayout->addWidget(multiFiducialGroupBox);
   dialogLayout->addWidget(textEditor);
   dialogLayout->addLayout(buttonsLayout);

   if (showOutputSpecFileSelection == false) {
      loadAtlasSpaceComboBox();
      slotAtlasSpaceComboBox(0);
   }
   
   if (enableFiducialOptionsIn == false) {
      multiFiducialGroupBox->hide();
   }
}

                           
/**
 * Destructor.
 */
GuiMapFmriAtlasDialog::~GuiMapFmriAtlasDialog()
{
}

/**
 * Load the atlas space combo box.
 */
void
GuiMapFmriAtlasDialog::loadAtlasSpaceComboBox()
{
   std::set<QString> sortedSpaces;
   
   for (int i = 0; i < static_cast<int>(allAtlases->size()); i++) {
      const MapFmriAtlasSpecFileInfo& asfi = (*allAtlases)[i];
      //
      // Check space
      //
      //const QString atlasSpace(StringUtilities::makeLowerCase(asfi.getSpace()));
      const QString atlasSpace(asfi.getSpace());
      if (atlasSpace.isEmpty() == false) {
         sortedSpaces.insert(atlasSpace);
      }
   }

   int defaultItem = -1;
   
   atlasSpaceComboBox->clear();
   
   int ctr = 0;
   for (std::set<QString>::iterator iter = sortedSpaces.begin();
        iter != sortedSpaces.end(); iter++) {
      atlasSpaceComboBox->addItem(*iter);
      if (StringUtilities::makeLowerCase(*iter) == 
          StringUtilities::makeLowerCase(space)) {
         defaultItem = ctr;
      }
      ctr++;
   }
   
   if (defaultItem >= 0) {
      atlasSpaceComboBox->setCurrentIndex(defaultItem);
   }
}

/**
 * called when an atlas space is selected.
 */
void 
GuiMapFmriAtlasDialog::slotAtlasSpaceComboBox(int item)
{
   space = "";
   
   if (item >= 0) {
      space = StringUtilities::makeLowerCase(atlasSpaceComboBox->currentText());
   }
   loadAtlasListBox();
}
      
/**
 * called when an atlas is selected.
 */
void 
GuiMapFmriAtlasDialog::slotAtlasComboBox(int item)
{
   multiFiducialGroupBox->setEnabled(false);
   multiFidAvgFidCheckBox->setEnabled(false);
   multiFidAvgAllCasesCheckBox->setEnabled(false);
   multiFidStdDevAllCheckBox->setEnabled(false);
   multiFidStdErrorAllCheckBox->setEnabled(false);
   multiFidMinAllCheckBox->setEnabled(false);
   multiFidMaxAllCheckBox->setEnabled(false);
   multiFidAllCasesCheckBox->setEnabled(false);
   if ((item >= 0) && (item < static_cast<int>(matchingAtlases.size()))) {
      const MapFmriAtlasSpecFileInfo& asfi = matchingAtlases[item];
      if (asfi.getCoordinateFiles().size() > 1) {
         multiFiducialGroupBox->setEnabled(true);
         multiFidAvgAllCasesCheckBox->setEnabled(true);
         multiFidStdDevAllCheckBox->setEnabled(true);
         multiFidStdErrorAllCheckBox->setEnabled(true);
         multiFidMinAllCheckBox->setEnabled(true);
         multiFidMaxAllCheckBox->setEnabled(true);
         multiFidAllCasesCheckBox->setEnabled(true);
         if (asfi.getAverageCoordinateFile().isEmpty() == false) {
            multiFidAvgFidCheckBox->setEnabled(true);
         }
      }
   }
}
      
/**
 * load atlas combo box
 */
void
GuiMapFmriAtlasDialog::loadAtlasListBox()
{
   matchingAtlases.clear();
   atlasComboBox->clear();

   matchingAtlases.push_back(MapFmriAtlasSpecFileInfo(""));
   atlasComboBox->addItem("Press This to Choose Atlas");
      
   for (int i = 0; i < static_cast<int>(allAtlases->size()); i++) {
      const MapFmriAtlasSpecFileInfo& asfi = (*allAtlases)[i];
      bool matches = true;
      
      //
      // Check space
      //
      const QString atlasSpace(StringUtilities::makeLowerCase(asfi.getSpace()));
      if ((space.isEmpty() == false) &&
          (atlasSpace.isEmpty() == false)) {
         if (space != atlasSpace) {
            matches = false;
         }
      }
      
      if (matches) {
         matchingAtlases.push_back(asfi);
         atlasComboBox->addItem(asfi.getDescription());
      }
   }
   
   if (atlasComboBox->count() > 0) {
      atlasComboBox->setCurrentIndex(0);
      slotAtlasComboBox(0);
   }
}

/**
 * Get the name of the selected spec file.
 */
QString
GuiMapFmriAtlasDialog::getOutputSpecFileName() const 
{
   return specFileLineEdit->text();
}

/**
 * called when select spec file pushbutton pressed.
 */
void 
GuiMapFmriAtlasDialog::slotSelectSpecPushButton()
{
   //
   // Popup the choose spec file dialog
   //
   GuiChooseSpecFileDialog sfd(this, preferencesFile, true);
   if (sfd.exec() == QDialog::Accepted) {
      const QString specFileName = sfd.getSelectedSpecFile();
      specFileLineEdit->setText(specFileName);
      
      //
      // Read the spec file to get the species and the structure
      // 
      try {
         SpecFile sf;
         sf.readFile(specFileName);
         space = StringUtilities::makeLowerCase(sf.getSpace());
         species = StringUtilities::makeLowerCase(sf.getSpecies());
         structureName = StringUtilities::makeLowerCase(sf.getStructure());
      }
      catch (FileException& e) {
      }
      
      loadAtlasSpaceComboBox();
      slotAtlasSpaceComboBox(0);
   }
}

/**
 * Called when OK or Cancel button pressed.
 */
void
GuiMapFmriAtlasDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      if (atlasComboBox->currentIndex() == 0) {
         GuiMessageBox::critical(this, "ERROR", "You must choose an atlas.", "OK");
         return;
      }
      
      //
      // Make sure an atlas is selected.
      //
      QString atlasPath, topoFileName, description, metricNameHint, avgCoord, structureName;
      std::vector<QString>coordNames;
      bool b1, b2, b3, b4, b5, b6, b7;
      getSelectedAtlasData(atlasPath, topoFileName, description, 
                           coordNames, avgCoord, metricNameHint, structureName,
                           b1, b2, b3, b4, b5, b6, b7);
      if (coordNames.empty() && avgCoord.isEmpty()) {
         GuiMessageBox::critical(this, "ERROR", "No atlas is selected.", "OK");
         return;
      }
   }
   
   QDialog::done(r);
}

/**
 * get the selected atlas information.
 */
void 
GuiMapFmriAtlasDialog::getSelectedAtlasData(QString& atlasPath,
                                            QString& topoFileName,
                                            QString& description,
                                            std::vector<QString>& coordFileNames,
                                            QString& averageCoordFile,
                                            QString& metricNameHint,
                                            QString& structureName,
                                            bool& mapToAvgCoordFileFlag,
                                            bool& mapToAvgOfAllFlag,
                                            bool& mapToStdDevOfAllFlag,
                                            bool& mapToStdErrorOfAllFlag,
                                            bool& mapToMinOfAllFlag,
                                            bool& mapToMaxOfAllFlag,
                                            bool& mapToAllCasesFlag) const
{
   atlasPath = "";
   description = "";
   topoFileName = "";
   coordFileNames.clear();
   averageCoordFile = "";
   metricNameHint = "";
   structureName = "";
   mapToAvgCoordFileFlag = false;
   mapToAvgOfAllFlag = false;
   mapToStdDevOfAllFlag = false;
   mapToStdErrorOfAllFlag = false;
   mapToMinOfAllFlag = false;
   mapToMaxOfAllFlag = false;
   mapToAllCasesFlag = false;

   //
   // Get the selected atlas
   //   
   const int num = static_cast<int>(matchingAtlases.size());
   const int atlasNum = atlasComboBox->currentIndex();
   if ((atlasNum > 0) && (atlasNum < num)) {
      //
      // get selected atlas
      //
      const MapFmriAtlasSpecFileInfo& asfi = matchingAtlases[atlasNum];
      
      //
      // Get atlas path and topo file
      //
      atlasPath = asfi.getSpecFilePath();
      topoFileName = asfi.getTopologyFile();
      
      //
      // Get allcoordinate files
      //
      coordFileNames = asfi.getCoordinateFiles();
      
      //
      // Get avg fiducial coord file
      //
      averageCoordFile = asfi.getAverageCoordinateFile();
      
      //
      // Get metric name hint
      //
      metricNameHint = asfi.getMetricNameHint();
      
      //
      // Add description of atlas
      //
      QString s(asfi.getStructure());
      s.append(" ");
      s.append(asfi.getSpace());
      description = s;
      
      //
      // name of structure
      //
      structureName = asfi.getStructure();
      
      //
      // Set multi-fiducial selections
      //
      if (multiFiducialGroupBox->isEnabled()) {
         if (multiFidAvgFidCheckBox->isEnabled()) {
            mapToAvgCoordFileFlag = multiFidAvgFidCheckBox->isChecked();
         }
         mapToAvgOfAllFlag = multiFidAvgAllCasesCheckBox->isChecked();
         mapToStdDevOfAllFlag = multiFidStdDevAllCheckBox->isChecked();
         mapToStdErrorOfAllFlag = multiFidStdErrorAllCheckBox->isChecked();
         mapToMinOfAllFlag = multiFidMinAllCheckBox->isChecked();
         mapToMaxOfAllFlag = multiFidMaxAllCheckBox->isChecked();
         mapToAllCasesFlag = multiFidAllCasesCheckBox->isChecked();
         
         //
         // If the individual case coord files are not needed
         //
         if ((mapToAvgOfAllFlag == false) &&
             (mapToStdDevOfAllFlag == false) &&
             (mapToStdErrorOfAllFlag == false) &&
             (mapToMinOfAllFlag == false) &&
             (mapToMaxOfAllFlag == false) &&
             (mapToAllCasesFlag == false)) {
            coordFileNames.clear();
         }
      }
      else {
         mapToAllCasesFlag = true;
      }
   }
}


