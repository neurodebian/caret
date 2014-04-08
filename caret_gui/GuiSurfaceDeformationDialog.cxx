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

#include <iostream>
#include <sstream>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include "WuQFileDialog.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTabWidget>

#include "GuiBrainModelOpenGL.h"
#include "BrainModelSurfaceDeformDataFile.h"
#include "BrainModelSurfaceDeformationFlat.h"
#include "BrainModelSurfaceDeformationMultiStageSphericalVector.h"
#include "BrainModelSurfaceDeformationSpherical.h"
#include "BrainModelSurfaceDeformationSphericalVector.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "DisplaySettingsSurface.h"
#include "FileUtilities.h"
#include "GuiBatchCommandDialog.h"
#include "GuiBorderAttributesDialog.h"
#include "GuiChooseSpecFileDialog.h"
#include "GuiMainWindow.h"
#include "GuiSurfaceDeformationDialog.h"
#include "DeformationMapFile.h"
#include "QtRadioButtonSelectionDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"
#include "GuiStudyMetaDataFileEditorDialog.h"

static const QString continuationAndNewLine = " \\\n";

/**
 * constructor
 */
GuiSurfaceDeformationDialog::GuiSurfaceDeformationDialog(
                              QWidget* parent,
                              const DeformationMapFile::DEFORMATION_TYPE deformationTypeIn)
   : WuQDialog(parent)
{
   dmf.setFlatOrSphereSelection(deformationTypeIn);
   switch (dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         setWindowTitle("Flat Surface Deformation");
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         setWindowTitle("Spherical Surface Deformation");
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         setWindowTitle("Spherical Surface Deformation");
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         setWindowTitle("Spherical Surface Deformation");
         break;
   }

   deformBothWaysCheckBox = NULL;
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
      atlasBorderFileEntryLine[i] = NULL;
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // create the tabbed widgets
   //
   dialogTabWidget = new QTabWidget(this);
   dialogLayout->addWidget(dialogTabWidget);
   createIndividualWidget();
   createAtlasWidget();
   createParametersWidget();
   createDeformationWidget();
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   
   //
   // Cancel button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   //
   // Load the parameters into the dialog
   //
   loadParametersIntoDialog();

   setConnectionsForAutoDefMapFileNameUpdates();
   updateAtlasBorderSelections();
}

/**
 * Add to the command.
 */
void
GuiSurfaceDeformationDialog::addToCommand(std::ostringstream& commandStr, 
                                          const QString& cmd,
                                          const QString& value, 
                                          const QString& value2)
{
   if (value.isEmpty() == false) {
      commandStr << continuationAndNewLine.toAscii().constData();
      commandStr << "   " << cmd.toAscii().constData() << " " << value.toAscii().constData();
      if (value2.isEmpty() == false) {
         commandStr << " " << value2.toAscii().constData();
      }
   }
}

/**
 * destructor
 */
GuiSurfaceDeformationDialog::~GuiSurfaceDeformationDialog()
{
}

/**
 * Create the atlas tab.
 */
void
GuiSurfaceDeformationDialog::createAtlasWidget()
{
   //
   //  Grid to hold pushbuttons and line edits
   //
   atlasWidgetTop = new QWidget;
   QScrollArea* scrollArea = new QScrollArea();
   scrollArea->setWidget(atlasWidgetTop);
   scrollArea->setWidgetResizable(true);
   dialogTabWidget->addTab(scrollArea, "Atlas");
   
   //
   // Put everything in a QGrid
   //
   QGridLayout* atlasWidgetLayout = new QGridLayout(atlasWidgetTop);
   
   //
   // Button group for file selections
   //
   QButtonGroup* atlasButtonGroup = new QButtonGroup(this);
   QObject::connect(atlasButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(atlasFileSelection(int)));
   

   //
   // Edit borders button
   //
   QButtonGroup* editButtonGroup = new QButtonGroup(this);
   QObject::connect(editButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotBordersEditAtlas(int)));
   QPushButton* editBordersPushButton[DeformationMapFile::MAX_SPHERICAL_STAGES];
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
      editBordersPushButton[i] = new QPushButton("Edit...");
      editBordersPushButton[i]->setAutoDefault(false);
      editButtonGroup->addButton(editBordersPushButton[i], i);
   }
   
   //
   // Create the pushbuttons and line edits
   //
   atlasSpecLineEdit           = createFileEntryLine("Spec File...", FILE_TYPE_SPEC, 
                                                     atlasWidgetLayout, atlasButtonGroup)->lineEdit;;
   atlasClosedTopoLineEdit     = createFileEntryLine("Closed Topo File...", FILE_TYPE_TOPO_CLOSED, 
                                                     atlasWidgetLayout, atlasButtonGroup)->lineEdit;;
   atlasCutTopoLineEdit        = createFileEntryLine("Cut Topo File...", FILE_TYPE_TOPO_CUT, 
                                                     atlasWidgetLayout, atlasButtonGroup)->lineEdit;;
   atlasFiducialCoordLineEdit  = createFileEntryLine("Fiducial Coord File...", FILE_TYPE_COORD_FIDUCIAL, 
                                                     atlasWidgetLayout, atlasButtonGroup)->lineEdit;;
   atlasSphericalCoordLineEdit = createFileEntryLine("Spherical Coord File...", FILE_TYPE_COORD_SPHERICAL, 
                                                     atlasWidgetLayout, atlasButtonGroup)->lineEdit;;
   atlasFlatCoordLineEdit      = createFileEntryLine("Flat Coord...", FILE_TYPE_COORD_FLAT, 
                                                     atlasWidgetLayout, atlasButtonGroup)->lineEdit;;

   atlasBorderFileEntryLine[0] = createFileEntryLine("Border File Stage 1...", FILE_TYPE_BORDER_1,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[0]);
   atlasBorderFileEntryLine[1] = createFileEntryLine("Border File Stage 2...", FILE_TYPE_BORDER_2,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[1]);
   atlasBorderFileEntryLine[2] = createFileEntryLine("Border File Stage 3...", FILE_TYPE_BORDER_3,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[2]);
   atlasBorderFileEntryLine[3] = createFileEntryLine("Border File Stage 4...", FILE_TYPE_BORDER_4,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[3]);
   atlasBorderFileEntryLine[4] = createFileEntryLine("Border File Stage 5...", FILE_TYPE_BORDER_5,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[4]);
   atlasBorderFileEntryLine[5] = createFileEntryLine("Border File Stage 6...", FILE_TYPE_BORDER_6,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[5]);
   atlasBorderFileEntryLine[6] = createFileEntryLine("Border File Stage 7...", FILE_TYPE_BORDER_7,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[6]);
   atlasBorderFileEntryLine[7] = createFileEntryLine("Border File Stage 8...", FILE_TYPE_BORDER_8,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[7]);
   atlasBorderFileEntryLine[8] = createFileEntryLine("Border File Stage 9...", FILE_TYPE_BORDER_9,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[8]);
   atlasBorderFileEntryLine[9] = createFileEntryLine("Border File Stage 10...", FILE_TYPE_BORDER_10,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[9]);
   atlasBorderFileEntryLine[10] = createFileEntryLine("Border File Stage 11...", FILE_TYPE_BORDER_11,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[10]);
   atlasBorderFileEntryLine[11] = createFileEntryLine("Border File Stage 12...", FILE_TYPE_BORDER_12,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[11]);
   atlasBorderFileEntryLine[12] = createFileEntryLine("Border File Stage 13...", FILE_TYPE_BORDER_13,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[12]);
   atlasBorderFileEntryLine[13] = createFileEntryLine("Border File Stage 14...", FILE_TYPE_BORDER_14,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[13]);
   atlasBorderFileEntryLine[14] = createFileEntryLine("Border File Stage 15...", FILE_TYPE_BORDER_15,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[14]);
   atlasBorderFileEntryLine[15] = createFileEntryLine("Border File Stage 16...", FILE_TYPE_BORDER_16,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[15]);
   atlasBorderFileEntryLine[16] = createFileEntryLine("Border File Stage 17...", FILE_TYPE_BORDER_17,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[16]);
   atlasBorderFileEntryLine[17] = createFileEntryLine("Border File Stage 18...", FILE_TYPE_BORDER_18,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[17]);
   atlasBorderFileEntryLine[18] = createFileEntryLine("Border File Stage 19...", FILE_TYPE_BORDER_19,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[18]);
   atlasBorderFileEntryLine[19] = createFileEntryLine("Border File Stage 20...", FILE_TYPE_BORDER_20,
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton[19]);
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
      atlasBorderLineEdit[i] = atlasBorderFileEntryLine[i]->lineEdit;
   }

   //atlasWidgetTop->setFixedSize(atlasWidgetTop->sizeHint());
   const int numRows = atlasWidgetLayout->rowCount();
   atlasWidgetLayout->addWidget(new QWidget, numRows, 0);
   atlasWidgetLayout->setRowStretch(numRows, 1000);
}

/**
 * update atlas border selections.
 */
void
GuiSurfaceDeformationDialog::updateAtlasBorderSelections()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Atlas Border Selections"
                << std::endl;
   }
   if (this->atlasBorderFileEntryLine[0] == NULL) {
      return;
   }

   //
   // Default spherical algorithm
   //
   switch (dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         for (int i = 1; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
            this->atlasBorderFileEntryLine[i]->setVisible(false);
         }
         this->atlasBorderFileEntryLine[0]->pushButton->setText("Border File...");
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         this->atlasBorderFileEntryLine[0]->pushButton->setText("Border File Stage 1...");
         for (int i = 1; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
            this->atlasBorderFileEntryLine[i]->setVisible(true);
            bool enableFlag = (i < this->sphereNumberOfStagesSpinBox->value());
            this->atlasBorderFileEntryLine[i]->setEnabled(enableFlag);
         }
         break;
   }
}

/**
 * Create the individual tab.
 */
void
GuiSurfaceDeformationDialog::createIndividualWidget()
{
   //
   // Start with a QVBox to prevent parameters from showing through
   //
   individualWidgetTop = new QWidget;
   dialogTabWidget->addTab(individualWidgetTop, "Individual");
   QVBoxLayout* indivWidgetLayout = new QVBoxLayout(individualWidgetTop);
   
   //
   // Put everything in a QGrid
   //
   QGridLayout* individualGridLayout = new QGridLayout;
   indivWidgetLayout->addLayout(individualGridLayout);

   //
   // Button group for file selections
   //
   QButtonGroup* indivButtonGroup = new QButtonGroup(this);
   QObject::connect(indivButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(indivFileSelection(int)));
   
   //
   // Edit borders button
   //
   QPushButton* editBordersPushButton = new QPushButton("Edit...");
   editBordersPushButton->setAutoDefault(false);
   QObject::connect(editBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotBordersEditIndividual()));
   
   //
   // Create the pushbuttons and line edits
   //
   indivSpecLineEdit           = createFileEntryLine("Spec File...", FILE_TYPE_SPEC, 
                                                     individualGridLayout, indivButtonGroup)->lineEdit;
   indivClosedTopoLineEdit     = createFileEntryLine("Closed Topo File...", FILE_TYPE_TOPO_CLOSED, 
                                                     individualGridLayout, indivButtonGroup)->lineEdit;
   indivCutTopoLineEdit        = createFileEntryLine("Cut Topo File...", FILE_TYPE_TOPO_CUT, 
                                                     individualGridLayout, indivButtonGroup)->lineEdit;
   indivFiducialCoordLineEdit  = createFileEntryLine("Fiducial Coord File...", FILE_TYPE_COORD_FIDUCIAL, 
                                                     individualGridLayout, indivButtonGroup)->lineEdit;
   indivSphericalCoordLineEdit = createFileEntryLine("Spherical Coord File...", FILE_TYPE_COORD_SPHERICAL, 
                                                     individualGridLayout, indivButtonGroup)->lineEdit;
   indivFlatCoordLineEdit      = createFileEntryLine("Flat Coord...", FILE_TYPE_COORD_FLAT, 
                                                     individualGridLayout, indivButtonGroup)->lineEdit;
   indivBorderLineEdit         = createFileEntryLine("Border File...", FILE_TYPE_BORDER_1,
                                                     individualGridLayout, indivButtonGroup,
                                                     editBordersPushButton)->lineEdit;
   
   //individualWidget->setFixedSize(individualWidget->sizeHint());
   
   //
   // Box for control of coordinate file deformation
   //
   QGroupBox* indivCoordDeformGroupBox = new QGroupBox("Deform Coordinate Files To Atlas");
   QGridLayout* indivCoordGridLayout = new QGridLayout(indivCoordDeformGroupBox);
   indivCoordGridLayout->setSpacing(10);
   indivWidgetLayout->addWidget(indivCoordDeformGroupBox);
   
   //
   // Request that fiducial coord files be deformed
   //
   deformIndivFiducialCoordCheckBox = new QCheckBox("Fiducial");
   deformIndivFiducialCoordCheckBox->setChecked(true);
   indivCoordGridLayout->addWidget(deformIndivFiducialCoordCheckBox, 0, 0);
   
   //
   // Request that inflated coord files be deformed
   //
   deformIndivInflatedCoordCheckBox = new QCheckBox("Inflated");
   deformIndivInflatedCoordCheckBox->setChecked(true);
   indivCoordGridLayout->addWidget(deformIndivInflatedCoordCheckBox, 0, 1);
                                            
   //
   // Request that very inflated coord files be deformed
   //
   deformIndivVeryInflatedCoordCheckBox = new QCheckBox("Very Inflated");
   deformIndivVeryInflatedCoordCheckBox->setChecked(true);
   indivCoordGridLayout->addWidget(deformIndivVeryInflatedCoordCheckBox, 1, 0);
                                            
   //
   // Request that spherical coord files be deformed
   //
   deformIndivSphericalCoordCheckBox = new QCheckBox("Spherical");
   deformIndivSphericalCoordCheckBox->setChecked(true);
   indivCoordGridLayout->addWidget(deformIndivSphericalCoordCheckBox, 1, 1);
                                            
   //
   // Request that flat coord files be deformed
   //
   deformIndivFlatCoordCheckBox = new QCheckBox("Flat");
   deformIndivFlatCoordCheckBox->setChecked(true);
   indivCoordGridLayout->addWidget(deformIndivFlatCoordCheckBox, 2, 0);
                                            
   //
   // Request that flat lobar coord files be deformed
   //
   deformIndivFlatLobarCoordCheckBox = new QCheckBox("Flat Lobar");
   deformIndivFlatLobarCoordCheckBox->setChecked(true);
   deformIndivFlatLobarCoordCheckBox->setHidden(true);
   indivCoordGridLayout->addWidget(deformIndivFlatLobarCoordCheckBox, 2, 1);
         
   //
   // Limit box size
   //
   //indivCoordDeformGroupBox->setFixedSize(indivCoordDeformGroupBox->sizeHint());
   
   //
   // Only allow coord deformations if spherical deformation
   //
   switch (dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         indivCoordDeformGroupBox->setHidden(true);
         indivCoordDeformGroupBox->setEnabled(false);
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         indivCoordDeformGroupBox->setHidden(false);
         indivCoordDeformGroupBox->setEnabled(true);
         break;
   }
   
   indivWidgetLayout->addStretch();
}

/**
 * called to edit individual borders.
 */
void 
GuiSurfaceDeformationDialog::slotBordersEditIndividual()
{
   if (indivDeformationFiles.borderFileSelected[0] < 0) {
      QMessageBox::critical(this, "ERROR", "There are no individual border files.");
      return;
   }
   editBorderFile(indivSpecFileName,
                  indivDeformationFiles.borderFileNames[indivDeformationFiles.borderFileSelected[0]],
                  indivDeformationFiles.borderFileTypes[indivDeformationFiles.borderFileSelected[0]]);
}

/**
 * edit a border file.
 */
void 
GuiSurfaceDeformationDialog::editBorderFile(const QString& specFileName,
                                            const QString& borderFileName,
                                            const DeformationDataFiles::DATA_FILE_TYPES fileType)
{
   //
   // Check inputs
   //
   if (specFileName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Invalid spec file name");
      return;
   }
   if (borderFileName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Invalid border file name");
      return;
   }
   
   //
   // Get the type of border file
   //
   GuiBorderAttributesDialog::BORDER_FILE_TYPE borderFileType =
      GuiBorderAttributesDialog::BORDER_FILE_TYPE_BORDER;
   switch(fileType) {
      case DeformationDataFiles::DATA_FILE_BORDER_FLAT:
         borderFileType = GuiBorderAttributesDialog::BORDER_FILE_TYPE_BORDER;
         break;
      case DeformationDataFiles::DATA_FILE_BORDER_FLAT_LOBAR:
         borderFileType = GuiBorderAttributesDialog::BORDER_FILE_TYPE_BORDER;
         break;
      case DeformationDataFiles::DATA_FILE_BORDER_SPHERICAL:
         borderFileType = GuiBorderAttributesDialog::BORDER_FILE_TYPE_BORDER;
         break;
      case DeformationDataFiles::DATA_FILE_BORDER_PROJECTION:
         borderFileType = GuiBorderAttributesDialog::BORDER_FILE_TYPE_BORDER_PROJECTION;
         break;
      case DeformationDataFiles::DATA_FILE_TOPO_CLOSED:
      case DeformationDataFiles::DATA_FILE_TOPO_CUT:
      case DeformationDataFiles::DATA_FILE_TOPO_CUT_LOBAR:
      case DeformationDataFiles::DATA_FILE_COORD_FIDUCIAL:
      case DeformationDataFiles::DATA_FILE_COORD_FLAT:
      case DeformationDataFiles::DATA_FILE_COORD_FLAT_LOBAR:
      case DeformationDataFiles::DATA_FILE_COORD_SPHERICAL:
         QMessageBox::critical(this, "ERROR", "Border file type is invalid");
         return;
         break;
   }
   
   //
   // Save current directory
   //
   const QString savedDirectory = QDir::currentPath();
   
   //
   // Set to directory containing spec file
   //   
   QDir::setCurrent(FileUtilities::dirname(specFileName));
   
   //
   // Launch the border editing dialog
   //
   GuiBorderAttributesDialog bad(this,
                                 borderFileName,
                                 borderFileType,
                                 false);
   bad.exec();
   
   //
   // Restore current directory
   //
   QDir::setCurrent(savedDirectory);
}

/**
 * called to edit atlas borders.
 */
void 
GuiSurfaceDeformationDialog::slotBordersEditAtlas(int index)
{
   if (atlasDeformationFiles.borderFileSelected[index] < 0) {
      QMessageBox::critical(this, "ERROR", "There are no atlas border files.");
      return;
   }
   editBorderFile(atlasSpecFileName,
                  atlasDeformationFiles.borderFileNames[atlasDeformationFiles.borderFileSelected[index]],
                  atlasDeformationFiles.borderFileTypes[atlasDeformationFiles.borderFileSelected[index]]);
}

/**
 * Create the parameters tab.
 */
void
GuiSurfaceDeformationDialog::createParametersWidget()
{
   QString tabTitle("ERROR");
   QWidget* sphereFlatParmsWidget = NULL;
   bool showParamsButtonsFlag = false;
   switch (dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         //
         // Create the spherical parameters
         //
         tabTitle = "Spherical Parameters";
         sphereFlatParmsWidget = createSphericalParameters();
         showParamsButtonsFlag = true;
         break;
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         //
         // Create the spherical parameters
         //
         tabTitle = "Flat Parameters";
         sphereFlatParmsWidget = createFlatParameters();
         break;
   }
   
   //
   // Create a pushbutton to use standard parameters
   //
   QPushButton* standardParametersPushButton = new  QPushButton("Select Standard Parameters...");
   standardParametersPushButton->setAutoDefault(false);
   standardParametersPushButton->setFixedSize(standardParametersPushButton->sizeHint());
   QObject::connect(standardParametersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStandardParametersPushButton()));
   
   //
   // Create a pushbutton to load parameters from a DeformationMapFile
   //
   QPushButton* loadDefMapFilePushButton = new QPushButton("Read Parameter From Def Map File...");
   loadDefMapFilePushButton->setAutoDefault(false);
   loadDefMapFilePushButton->setFixedSize(loadDefMapFilePushButton->sizeHint());
   QObject::connect(loadDefMapFilePushButton, SIGNAL(clicked()),
                    this, SLOT(loadDeformationMapFilePushButton()));

   //
   // Save parameters to a file
   //
   QPushButton* saveDefMapFilePushButton = new QPushButton("Save Parameters to Def Map File...");
   saveDefMapFilePushButton->setAutoDefault(false);
   saveDefMapFilePushButton->setFixedSize(saveDefMapFilePushButton->sizeHint());
   QObject::connect(saveDefMapFilePushButton, SIGNAL(clicked()),
                    this, SLOT(saveDeformationMapFilePushButton()));

   QGroupBox* setParamsGroupBox = new QGroupBox("Set Parameters");
   QHBoxLayout* setParamsLayout = new QHBoxLayout(setParamsGroupBox);
   setParamsLayout->addWidget(standardParametersPushButton);
   setParamsLayout->addWidget(saveDefMapFilePushButton);
   setParamsLayout->addWidget(loadDefMapFilePushButton);

   parametersWidget = new QWidget;
   QVBoxLayout* parametersLayout = new QVBoxLayout(parametersWidget);
   if (sphereFlatParmsWidget != NULL) {
      parametersLayout->addWidget(sphereFlatParmsWidget);
   }
   parametersLayout->addWidget(setParamsGroupBox);
   dialogTabWidget->addTab(parametersWidget, tabTitle);

   if (showParamsButtonsFlag == false) {
      setParamsGroupBox->setVisible(false);
   }
}

/**
 * Create the misc tab.
 */
void
GuiSurfaceDeformationDialog::createDeformationWidget()
{
   //
   // Create the border resampling section
   //
   QWidget* borderWidget = createBorderResampling();
   
   //
   // Create the metric deformation
   //
   QWidget* metricWidget = createMetricDeformation();
   
   //
   // Checkbox to deform in both directions
   //
   deformBothWaysCheckBox = new QCheckBox("Deform Indiv to Atlas and Atlas to Individual");
   deformBothWaysCheckBox->setChecked(true);
   QObject::connect(deformBothWaysCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotDeformationParameterChanged()));
                                             
   //
   // Delete intermediate files check box
   //
   deleteIntermediateFilesCheckBox = new QCheckBox("Delete Intermediate Files");
   QObject::connect(deleteIntermediateFilesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotDeformationParameterChanged()));
   
   //
   // label and line edit for deformed file prefix
   //
   QLabel* deformedFileLabel = new QLabel("Deformed File Name Prefix ");
   deformedFilePrefixLineEdit = new QLineEdit;
   QObject::connect(deformedFilePrefixLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotDeformationParameterChanged()));
   QHBoxLayout* deformedFileLayout = new QHBoxLayout;
   deformedFileLayout->addWidget(deformedFileLabel);
   deformedFileLayout->addWidget(deformedFilePrefixLineEdit);
   
   //
   // label and line edit for deformed column name prefix
   //
   QLabel* deformedColumnLabel = new QLabel("Deformed Column Name Prefix ");
   deformedColumnPrefixLineEdit = new QLineEdit;
   QObject::connect(deformedColumnPrefixLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotDeformationParameterChanged()));
   QHBoxLayout* deformedColumnLayout = new QHBoxLayout;
   deformedColumnLayout->addWidget(deformedColumnLabel);
   deformedColumnLayout->addWidget(deformedColumnPrefixLineEdit);
   
   //
   // Deformation Control Group Box
   //
   QGroupBox* controlGroupBox = new QGroupBox("Deformation Control");
   QVBoxLayout* controlGroupLayout = new QVBoxLayout(controlGroupBox);
   controlGroupLayout->addWidget(deformBothWaysCheckBox);
   controlGroupLayout->addWidget(deleteIntermediateFilesCheckBox);
   controlGroupLayout->addLayout(deformedFileLayout);
   controlGroupLayout->addLayout(deformedColumnLayout);

   //
   // Smooth Coordinate files option
   //   
   smoothCoordsOneIterationCheckBox = new QCheckBox("Smooth One Iteration (Except Flat)");
   smoothCoordsOneIterationCheckBox->setChecked(true);
   QObject::connect(smoothCoordsOneIterationCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotDeformationParameterChanged()));
   
   //
   // Coordinate file options
   //
   QGroupBox* coordGroupBox = new QGroupBox("Coordinate Data File Deformation Options");
   QVBoxLayout* coordGroupLayout = new QVBoxLayout(coordGroupBox);
   coordGroupLayout->addWidget(smoothCoordsOneIterationCheckBox);

   //
   // Deformation map file names
   //
   QLabel* indivToAtlasDeformMapFileNameLabel = new QLabel("Indiv to Atlas");
   indivToAtlasDeformMapFileNameLineEdit = new QLineEdit;
   QObject::connect(indivToAtlasDeformMapFileNameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotDeformationParameterChanged()));
   QLabel* atlasToIndivDeformMapFileNameLabel = new QLabel("Atlas to Indiv");
   atlasToIndivDeformMapFileNameLineEdit = new QLineEdit;
   QObject::connect(atlasToIndivDeformMapFileNameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotDeformationParameterChanged()));
   QGroupBox* defMapNameGroupBox = new QGroupBox("Deformation Map File Names");
   QGridLayout* defMapNameLayout = new QGridLayout(defMapNameGroupBox);
   defMapNameLayout->addWidget(indivToAtlasDeformMapFileNameLabel, 0, 0);
   defMapNameLayout->addWidget(indivToAtlasDeformMapFileNameLineEdit, 0, 1);
   defMapNameLayout->addWidget(atlasToIndivDeformMapFileNameLabel, 1, 0);
   defMapNameLayout->addWidget(atlasToIndivDeformMapFileNameLineEdit, 1, 1);
   defMapNameLayout->setColumnStretch(0, 0);
   defMapNameLayout->setColumnStretch(1, 100);

   //
   // Put border/metric items side by side
   //
   QHBoxLayout* borderMetricLayout = new QHBoxLayout;
   borderMetricLayout->addWidget(borderWidget);
   borderMetricLayout->addWidget(metricWidget);

   //
   // Top widget
   //
   deformationWidgetTop = new QWidget;
   dialogTabWidget->addTab(deformationWidgetTop, "Deformation");
   QVBoxLayout* deformationWidgetLayout = new QVBoxLayout(deformationWidgetTop);
   deformationWidgetLayout->addLayout(borderMetricLayout);
   deformationWidgetLayout->addWidget(controlGroupBox);
   deformationWidgetLayout->addWidget(coordGroupBox);
   deformationWidgetLayout->addWidget(defMapNameGroupBox);
   deformationWidgetLayout->addStretch();
}

/**
 * setup connections for automatic update of deformation map file names.
 */
void
GuiSurfaceDeformationDialog::setConnectionsForAutoDefMapFileNameUpdates()
{
   QObject::connect(this->deformedFilePrefixLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotUpdateDeformationMapFileNames()));
   QObject::connect(this->indivSpecLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotUpdateDeformationMapFileNames()));
   QObject::connect(this->atlasSpecLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotUpdateDeformationMapFileNames()));
}

/**
 * called to update deformation map file names.
 */
void
GuiSurfaceDeformationDialog::slotUpdateDeformationMapFileNames()
{
   QString indivToAtlasSpecFileName =
           BrainModelSurfaceDeformDataFile::createDeformedSpecFileName(
              this->deformedFilePrefixLineEdit->text(),
              this->indivSpecLineEdit->text(),
              this->atlasSpecLineEdit->text());
   this->indivToAtlasDeformMapFileNameLineEdit->blockSignals(true);
   this->indivToAtlasDeformMapFileNameLineEdit->setText(indivToAtlasSpecFileName);
   this->indivToAtlasDeformMapFileNameLineEdit->blockSignals(false);

   QString atlasToIndivSpecFileName =
           BrainModelSurfaceDeformDataFile::createDeformedSpecFileName(
              this->deformedFilePrefixLineEdit->text(),
              this->atlasSpecLineEdit->text(),
              this->indivSpecLineEdit->text());
   this->atlasToIndivDeformMapFileNameLineEdit->blockSignals(true);
   this->atlasToIndivDeformMapFileNameLineEdit->setText(atlasToIndivSpecFileName);
   this->atlasToIndivDeformMapFileNameLineEdit->blockSignals(false);
}


/**
 *
 */
QWidget*
GuiSurfaceDeformationDialog::createBorderResampling()
{
   //
   // "None" resampling
   //
   borderResamplingNoneRadioButton = new QRadioButton("None");
   
   //
   // From border file resampling
   //
   borderResamplingFromFileRadioButton = new QRadioButton("From Border File");
   
   //
   // Horizontal box for resample to button and spin box
   //
   borderResamplingToValueRadioButton = new QRadioButton("Resample To");
   borderResamplingDoubleSpinBox = new QDoubleSpinBox;
   borderResamplingDoubleSpinBox->setMinimum(0.0);
   borderResamplingDoubleSpinBox->setMaximum(10000.0);
   borderResamplingDoubleSpinBox->setSingleStep(1.0);
   borderResamplingDoubleSpinBox->setDecimals(3);
   borderResamplingDoubleSpinBox->setFixedWidth(100);
   QObject::connect(borderResamplingDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotDeformationParameterChanged()));
   
   //
   // Put stuff in a vertical group box
   //
   QGroupBox* borderGroup = new QGroupBox("Border Resampling Interval");
   QGridLayout* borderLayout = new QGridLayout(borderGroup);
   borderLayout->addWidget(borderResamplingNoneRadioButton, 0, 0);
   borderLayout->addWidget(borderResamplingFromFileRadioButton, 1, 0);
   borderLayout->addWidget(borderResamplingToValueRadioButton, 2, 0);
   borderLayout->addWidget(borderResamplingDoubleSpinBox, 2, 1);
   
   //
   // button group so that buttons remain mutually exclusive
   //
   QButtonGroup* borderButtonGroup = new QButtonGroup(borderGroup);
   borderButtonGroup->addButton(borderResamplingNoneRadioButton, 0);
   borderButtonGroup->addButton(borderResamplingFromFileRadioButton, 1);
   borderButtonGroup->addButton(borderResamplingToValueRadioButton, 2);
   QObject::connect(borderButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotDeformationParameterChanged()));

   return borderGroup;
}

/**
 *
 */
QWidget*
GuiSurfaceDeformationDialog::createMetricDeformation()
{
   metricNearestNodeRadioButton = new QRadioButton("Nearest Node");
   metricAverageTileNodesRadioButton = new QRadioButton("Average of Tile's Nodes");
   
   QGroupBox* metricGroupBox = new QGroupBox("Metric Deformation");
   QVBoxLayout* metricGroupLayout = new QVBoxLayout(metricGroupBox);
   metricGroupLayout->addWidget(metricNearestNodeRadioButton);
   metricGroupLayout->addWidget(metricAverageTileNodesRadioButton);
   
   QButtonGroup* metricButtonGroup = new QButtonGroup(this);
   metricButtonGroup->addButton(metricNearestNodeRadioButton);
   metricButtonGroup->addButton(metricAverageTileNodesRadioButton);
   QObject::connect(metricButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotDeformationParameterChanged()));
   
   return metricGroupBox;
}

/**
 * create the landmark vector options.
 */
QWidget*
GuiSurfaceDeformationDialog::createLandmarkVectorOptionsSection()
{
   QLabel* vectorSmoothingLabel = new QLabel("Vector Smoothing Iterations");
   vectorSmoothingIterationsSpinBox = new QSpinBox;
   vectorSmoothingIterationsSpinBox->setMinimum(0);
   vectorSmoothingIterationsSpinBox->setMaximum(100000);
   vectorSmoothingIterationsSpinBox->setSingleStep(1);
   vectorSmoothingIterationsSpinBox->setValue(10);
   QObject::connect(vectorSmoothingIterationsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotLandmarkVectorParameterChanged()));

   QLabel* vectorDisplacementFactorLabel = new QLabel("Displacement Factor");
   vectorDisplacementFactorDoubleSpinBox = new QDoubleSpinBox;
   vectorDisplacementFactorDoubleSpinBox->setMinimum(0.0);
   vectorDisplacementFactorDoubleSpinBox->setMaximum(100000.0);
   vectorDisplacementFactorDoubleSpinBox->setSingleStep(0.1);
   vectorDisplacementFactorDoubleSpinBox->setDecimals(2);
   vectorDisplacementFactorDoubleSpinBox->setValue(1.0);
   QObject::connect(vectorDisplacementFactorDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotLandmarkVectorParameterChanged()));

   QLabel* vectorEndpointFactorLabel = new QLabel("Endpoint Factor");
   vectorEndpointFactorDoubleSpinBox = new QDoubleSpinBox;
   vectorEndpointFactorDoubleSpinBox->setMinimum(0.0);
   vectorEndpointFactorDoubleSpinBox->setMaximum(1000000.0);
   vectorEndpointFactorDoubleSpinBox->setSingleStep(0.1);
   vectorEndpointFactorDoubleSpinBox->setDecimals(2);
   vectorEndpointFactorDoubleSpinBox->setValue(1.0);
   QObject::connect(vectorEndpointFactorDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotLandmarkVectorParameterChanged()));

   pauseForCrossoversConfirmationCheckBox = new QCheckBox("Pause for Crossovers Confirmation");
   QObject::connect(pauseForCrossoversConfirmationCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotLandmarkVectorParameterChanged()));

   QGroupBox* vectorGroupBox = new QGroupBox("Landmark Vector Options");
   QGridLayout* vectorGridLayout = new QGridLayout(vectorGroupBox);
   vectorGridLayout->addWidget(vectorSmoothingLabel, 0, 0);
   vectorGridLayout->addWidget(vectorSmoothingIterationsSpinBox, 0, 1);
   vectorGridLayout->addWidget(vectorDisplacementFactorLabel, 1, 0);
   vectorGridLayout->addWidget(vectorDisplacementFactorDoubleSpinBox, 1, 1);
   vectorGridLayout->addWidget(vectorEndpointFactorLabel, 2, 0);
   vectorGridLayout->addWidget(vectorEndpointFactorDoubleSpinBox, 2, 1);
   vectorGridLayout->addWidget(pauseForCrossoversConfirmationCheckBox, 3, 0, 1, 2);
   vectorGroupBox->setMaximumHeight(vectorGroupBox->sizeHint().height());

   landmarkVectorParametersWidgetGroup = new WuQWidgetGroup(this);
   landmarkVectorParametersWidgetGroup->addWidget(vectorSmoothingIterationsSpinBox);
   landmarkVectorParametersWidgetGroup->addWidget(vectorDisplacementFactorDoubleSpinBox);
   landmarkVectorParametersWidgetGroup->addWidget(vectorEndpointFactorDoubleSpinBox);
   landmarkVectorParametersWidgetGroup->addWidget(pauseForCrossoversConfirmationCheckBox);

   return vectorGroupBox;
}

/**
 * called when a landmark vector parameter is changed.
 */
void
GuiSurfaceDeformationDialog::slotLandmarkVectorParameterChanged()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Landmark Vector Parameter"
                << std::endl;
   }
    const int stageIndex = sphereEditStageSpinBox->value() - 1;
    const int cycleIndex = sphereEditCycleSpinBox->value() - 1;

    dmf.setLandmarkVectorParameters(stageIndex,
                                    cycleIndex,
                                    this->vectorSmoothingIterationsSpinBox->value(),
                                    this->vectorDisplacementFactorDoubleSpinBox->value());
    dmf.setLandmarkVectorStageParameters(stageIndex,
                                         this->vectorEndpointFactorDoubleSpinBox->value());
    dmf.setPauseForCrossoversConfirmation(this->pauseForCrossoversConfirmationCheckBox->isChecked());
}

/**
 * update the landmark vector parameters.
 */
void
GuiSurfaceDeformationDialog::updateLandmarkVectorParameters()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Landmark Vector Parameters"
                << std::endl;
   }
    const int stageIndex = sphereEditStageSpinBox->value() - 1;
    const int cycleIndex = sphereEditCycleSpinBox->value() - 1;

    landmarkVectorParametersWidgetGroup->blockSignals(true);
    int vectorSmoothingIterations = 0;
    float vectorDisplacementFactor = 1.0;
    dmf.getLandmarkVectorParameters(stageIndex, cycleIndex,
                                    vectorSmoothingIterations,
                                    vectorDisplacementFactor);
    this->vectorSmoothingIterationsSpinBox->setValue(vectorSmoothingIterations);
    this->vectorDisplacementFactorDoubleSpinBox->setValue(vectorDisplacementFactor);
    float endpointFactor = 1.0;
    dmf.getLandmarkVectorStageParameters(stageIndex, endpointFactor);
    this->vectorEndpointFactorDoubleSpinBox->setValue(endpointFactor);
    this->pauseForCrossoversConfirmationCheckBox->setChecked(dmf.getPauseForCrossoversConfirmation());
    landmarkVectorParametersWidgetGroup->blockSignals(false);
}

/**
 * called when a spherical algorithm is selected.
 */
void
GuiSurfaceDeformationDialog::slotSphericalAlgorithmSelection()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Spherical Algorithm"
                << std::endl;
   }
   if (this->sphericalLandmarkConstrainedRadioButton->isChecked()) {
      dmf.setFlatOrSphereSelection(DeformationMapFile::DEFORMATION_TYPE_SPHERE);
      this->sphereNumberOfStagesSpinBox->setValue(1);
   }
   else if (this->sphericalLandmarkVectorRadioButton->isChecked()) {
      dmf.setFlatOrSphereSelection(DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR);
   }
   else if (this->sphericalLandmarkVectorSingleStageRadioButton->isChecked()) {
      dmf.setFlatOrSphereSelection(DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR);
      this->sphereNumberOfStagesSpinBox->setValue(1);
   }
   updateAlgorithmSelection();
}

/**
 * update algorithm selection.
 */
void
GuiSurfaceDeformationDialog::updateAlgorithmSelection()
{
   bool enableVectorOptionsFlag = false;
   this->sphericalLandmarkConstrainedRadioButton->blockSignals(true);
   this->sphericalLandmarkVectorRadioButton->blockSignals(true);
   this->sphericalLandmarkVectorSingleStageRadioButton->blockSignals(true);
   switch (dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         this->sphericalLandmarkConstrainedRadioButton->setChecked(true);
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         this->sphericalLandmarkVectorSingleStageRadioButton->setChecked(true);
         enableVectorOptionsFlag = true;
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         this->sphericalLandmarkVectorRadioButton->setChecked(true);
         enableVectorOptionsFlag = true;
         break;
   }
   this->sphericalLandmarkConstrainedRadioButton->blockSignals(false);
   this->sphericalLandmarkVectorRadioButton->blockSignals(false);
   this->sphericalLandmarkVectorSingleStageRadioButton->blockSignals(false);

   landmarkVectorOptionsWidget->setEnabled(enableVectorOptionsFlag);
   sphereStagesWidgetGroup->setEnabled(enableVectorOptionsFlag);

   updateAtlasBorderSelections();
}

/**
 * create the spherical algorithm section.
 */
QWidget*
GuiSurfaceDeformationDialog::createSphericalAlgorithmSection()
{
   //
   // Create the algorithm selection radio buttons
   //
   sphericalLandmarkConstrainedRadioButton =
           new QRadioButton("Landmark Pinned Relaxation");
   sphericalLandmarkVectorRadioButton =
           new QRadioButton("Landmark Vector Difference");
   sphericalLandmarkVectorSingleStageRadioButton =
           new QRadioButton("Landmark Vector Difference Single Stage");

   //
   // NO SINGLE STAGE VECTOR DEFORMATION
   //
   sphericalLandmarkVectorSingleStageRadioButton->setHidden(true);

   //
   // Button group to make buttons mutually exclusive
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(sphericalLandmarkConstrainedRadioButton);
   buttGroup->addButton(sphericalLandmarkVectorRadioButton);
   buttGroup->addButton(sphericalLandmarkVectorSingleStageRadioButton);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotSphericalAlgorithmSelection()));

   //
   // Group box and layout
   //
   QGroupBox* algorithmGroupBox = new QGroupBox("Spherical Algorithm");
   QVBoxLayout* layout = new QVBoxLayout(algorithmGroupBox);
   layout->addWidget(sphericalLandmarkConstrainedRadioButton);
   layout->addWidget(sphericalLandmarkVectorRadioButton);
   layout->addWidget(sphericalLandmarkVectorSingleStageRadioButton);
   algorithmGroupBox->setMaximumHeight(algorithmGroupBox->sizeHint().height());
   return algorithmGroupBox;
}

/**
 * create the morphing parameters section.
 */
QWidget*
GuiSurfaceDeformationDialog::createMorphingParametersSection()
{
   //
   //  label and spin box
   //
   QLabel* morphCyclesLabel = new QLabel("Cycles");
   morphingCyclesSpinBox = new QSpinBox;
   QObject::connect(morphingCyclesSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMorphingParameterChanged()));
   morphingCyclesSpinBox->setMinimum(0);
   morphingCyclesSpinBox->setMaximum(5000);
   morphingCyclesSpinBox->setSingleStep(1);
   morphingCyclesSpinBox->setFixedWidth(120);

   //
   //  label and spin box
   //
   QLabel* morphingLinearForceLabel = new QLabel("Linear Force");
   morphingLinearForceDoubleSpinBox = new QDoubleSpinBox;
   QObject::connect(morphingLinearForceDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMorphingParameterChanged()));
   morphingLinearForceDoubleSpinBox->setMinimum(0.0);
   morphingLinearForceDoubleSpinBox->setMaximum(1.0);
   morphingLinearForceDoubleSpinBox->setSingleStep(0.05);
   morphingLinearForceDoubleSpinBox->setDecimals(3);
   morphingLinearForceDoubleSpinBox->setFixedWidth(120);

   //
   //  label and spin box
   //
   QLabel* morphingAngularForceLabel = new QLabel("Angular Force");
   morphingAngularForceDoubleSpinBox = new QDoubleSpinBox;
   QObject::connect(morphingAngularForceDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMorphingParameterChanged()));
   morphingAngularForceDoubleSpinBox->setMinimum(0.0);
   morphingAngularForceDoubleSpinBox->setMaximum(1.0);
   morphingAngularForceDoubleSpinBox->setSingleStep(0.05);
   morphingAngularForceDoubleSpinBox->setDecimals(3);
   morphingAngularForceDoubleSpinBox->setFixedWidth(120);

   //
   //  label and spin box
   //
   QLabel* morphingStepSizeLabel = new QLabel("Step Size");
   morphingStepSizeDoubleSpinBox = new QDoubleSpinBox;
   QObject::connect(morphingStepSizeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMorphingParameterChanged()));
   morphingStepSizeDoubleSpinBox->setMinimum(0.0);
   morphingStepSizeDoubleSpinBox->setMaximum(1.0);
   morphingStepSizeDoubleSpinBox->setSingleStep(0.05);
   morphingStepSizeDoubleSpinBox->setDecimals(3);
   morphingStepSizeDoubleSpinBox->setFixedWidth(120);

   //
   //  label and spin box
   //
   QLabel* morphingLandmarkLabel = new QLabel("Landmark Step Size");
   morphingLandmarkStepSizeDoubleSpinBox = new QDoubleSpinBox;
   QObject::connect(morphingLandmarkStepSizeDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMorphingParameterChanged()));
   morphingLandmarkStepSizeDoubleSpinBox->setMinimum(0.0);
   morphingLandmarkStepSizeDoubleSpinBox->setMaximum(1.0);
   morphingLandmarkStepSizeDoubleSpinBox->setSingleStep(0.05);
   morphingLandmarkStepSizeDoubleSpinBox->setDecimals(3);
   morphingLandmarkStepSizeDoubleSpinBox->setFixedWidth(120);

   //
   //  label and spin box
   //
   QLabel* morphingIterationsLabel = new QLabel("Iterations");
   morphingIterationsSpinBox = new QSpinBox;
   QObject::connect(morphingIterationsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMorphingParameterChanged()));
   morphingIterationsSpinBox->setMinimum(0);
   morphingIterationsSpinBox->setMaximum(5000);
   morphingIterationsSpinBox->setSingleStep(10);
   morphingIterationsSpinBox->setFixedWidth(120);

   //
   //  label and spin box
   //
   QLabel* morphingSmoothLabel = new QLabel("Smooth Iterations");
   morphingSmoothIterationsSpinBox = new QSpinBox;
   QObject::connect(morphingSmoothIterationsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMorphingParameterChanged()));
   morphingSmoothIterationsSpinBox->setMinimum(0);
   morphingSmoothIterationsSpinBox->setMaximum(5000);
   morphingSmoothIterationsSpinBox->setSingleStep(1);
   morphingSmoothIterationsSpinBox->setFixedWidth(120);

   //
   // Group box for morphing parameters
   //
   QGroupBox* morphGroupBox = new QGroupBox("Morphing Parameters");
   QGridLayout* morphGroupLayout = new QGridLayout(morphGroupBox);
   morphGroupLayout->addWidget(morphCyclesLabel, 0, 0);
   morphGroupLayout->addWidget(morphingCyclesSpinBox, 0, 1);
   morphGroupLayout->addWidget(morphingLinearForceLabel, 1, 0);
   morphGroupLayout->addWidget(morphingLinearForceDoubleSpinBox, 1, 1);
   morphGroupLayout->addWidget(morphingAngularForceLabel, 2, 0);
   morphGroupLayout->addWidget(morphingAngularForceDoubleSpinBox, 2, 1);
   morphGroupLayout->addWidget(morphingStepSizeLabel, 3, 0);
   morphGroupLayout->addWidget(morphingStepSizeDoubleSpinBox, 3, 1);
   morphGroupLayout->addWidget(morphingLandmarkLabel, 4, 0);
   morphGroupLayout->addWidget(morphingLandmarkStepSizeDoubleSpinBox, 4, 1);
   morphGroupLayout->addWidget(morphingIterationsLabel, 5, 0);
   morphGroupLayout->addWidget(morphingIterationsSpinBox, 5, 1);
   morphGroupLayout->addWidget(morphingSmoothLabel, 6, 0);
   morphGroupLayout->addWidget(morphingSmoothIterationsSpinBox, 6, 1);
   //morphGroupLayout->setRowStretch(7, 100);
   morphGroupBox->setMaximumHeight(morphGroupBox->sizeHint().height());

   morphingParametersWidgetGroup = new WuQWidgetGroup(this);
   morphingParametersWidgetGroup->addWidget(morphingCyclesSpinBox);
   morphingParametersWidgetGroup->addWidget(morphingLinearForceDoubleSpinBox);
   morphingParametersWidgetGroup->addWidget(morphingAngularForceDoubleSpinBox);
   morphingParametersWidgetGroup->addWidget(morphingStepSizeDoubleSpinBox);
   morphingParametersWidgetGroup->addWidget(morphingLandmarkStepSizeDoubleSpinBox);
   morphingParametersWidgetGroup->addWidget(morphingIterationsSpinBox);
   morphingParametersWidgetGroup->addWidget(morphingSmoothIterationsSpinBox);

   return morphGroupBox;
}

/**
 * called when a morphing parameter is changed.
 */
void
GuiSurfaceDeformationDialog::slotMorphingParameterChanged()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Morphing Parameters"
                << std::endl;
   }
    const int stageIndex = sphereEditStageSpinBox->value() - 1;
    const int cycleIndex = sphereEditCycleSpinBox->value() - 1;

    //
    // Get the morphing parameters
    //
    dmf.setMorphingParameters(stageIndex,
                              cycleIndex,
                              morphingCyclesSpinBox->value(),
                              morphingLinearForceDoubleSpinBox->value(),
                              morphingAngularForceDoubleSpinBox->value(),
                              morphingStepSizeDoubleSpinBox->value(),
                              morphingLandmarkStepSizeDoubleSpinBox->value(),
                              morphingIterationsSpinBox->value(),
                              morphingSmoothIterationsSpinBox->value());
}

/**
 * update the morphing parameters.
 */
void
GuiSurfaceDeformationDialog::updateMorphingParameters()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Morphing Parameters"
                << std::endl;
   }
    const int stageIndex = sphereEditStageSpinBox->value() - 1;
    const int cycleIndex = sphereEditCycleSpinBox->value() - 1;

    morphingParametersWidgetGroup->blockSignals(true);
    float linearForce, angularForce, stepSize, landmarkStepSize;
    int cycles, iterations, smoothIterations;
    dmf.getMorphingParameters(stageIndex, cycleIndex,
                              cycles, linearForce, angularForce, stepSize,
                              landmarkStepSize, iterations, smoothIterations);
    morphingCyclesSpinBox->setValue(cycles);
    morphingLinearForceDoubleSpinBox->setValue(linearForce);
    morphingAngularForceDoubleSpinBox->setValue(angularForce);
    morphingStepSizeDoubleSpinBox->setValue(stepSize);
    morphingLandmarkStepSizeDoubleSpinBox->setValue(landmarkStepSize);
    morphingIterationsSpinBox->setValue(iterations);
    morphingSmoothIterationsSpinBox->setValue(smoothIterations);
    morphingParametersWidgetGroup->blockSignals(false);
}

/**
 * create the smoothing parameters section.
 */
QWidget*
GuiSurfaceDeformationDialog::createSmoothingParametersSection()
{
   //
   //  label and strength spin box
   //
   QLabel* strengthLabel = new QLabel("Strength");
   smoothingStrengthDoubleSpinBox = new QDoubleSpinBox;
   QObject::connect(smoothingStrengthDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotSmoothingParameterChanged()));
   smoothingStrengthDoubleSpinBox->setMinimum(0.0);
   smoothingStrengthDoubleSpinBox->setMaximum(1.0);
   smoothingStrengthDoubleSpinBox->setSingleStep(0.05);
   smoothingStrengthDoubleSpinBox->setDecimals(3);
   smoothingStrengthDoubleSpinBox->setFixedWidth(120);

   //
   //  label and cycles spin box
   //
   QLabel* cyclesLabel = new QLabel("Cycles");
   smoothingCyclesSpinBox = new QSpinBox;
   QObject::connect(smoothingCyclesSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSmoothingParameterChanged()));
   smoothingCyclesSpinBox->setMinimum(0);
   smoothingCyclesSpinBox->setMaximum(50000);
   smoothingCyclesSpinBox->setSingleStep(10);
   smoothingCyclesSpinBox->setFixedWidth(120);

   //
   //  label and iterations per cycle spin box
   //
   QLabel* iterationsLabel = new QLabel("Iterations/Cycle");
   smoothingIterationsSpinBox = new QSpinBox;
   QObject::connect(smoothingIterationsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSmoothingParameterChanged()));
   smoothingIterationsSpinBox->setMinimum(0);
   smoothingIterationsSpinBox->setMaximum(50000);
   smoothingIterationsSpinBox->setSingleStep(5);
   smoothingIterationsSpinBox->setFixedWidth(120);

   //
   //  label and smooth neighbors spin box
   //
   QLabel* smoothNeighborsLabel = new QLabel("Smooth Neighbors Every X");
   smoothingNeighborsSpinBox = new QSpinBox;
   QObject::connect(smoothingNeighborsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSmoothingParameterChanged()));
   smoothingNeighborsSpinBox->setMinimum(0);
   smoothingNeighborsSpinBox->setMaximum(50000);
   smoothingNeighborsSpinBox->setSingleStep(1);
   smoothingNeighborsSpinBox->setFixedWidth(120);

   //
   //  label and final smoothing iterations
   //
   QLabel* finalSmoothLabel = new QLabel("Final Smoothing Iterations");
   smoothingFinalSpinBox = new QSpinBox;
   QObject::connect(smoothingFinalSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSmoothingParameterChanged()));
   smoothingFinalSpinBox->setMinimum(0);
   smoothingFinalSpinBox->setMaximum(50000);
   smoothingFinalSpinBox->setSingleStep(1);
   smoothingFinalSpinBox->setFixedWidth(120);

   //
   // Group box and layout for smoothing parameters
   //
   QGroupBox* smoothingParamsGroupBox = new QGroupBox("Smoothing Parameters");
   QGridLayout* smoothingParamsLayout = new QGridLayout(smoothingParamsGroupBox);
   smoothingParamsLayout->addWidget(strengthLabel, 0, 0);
   smoothingParamsLayout->addWidget(smoothingStrengthDoubleSpinBox, 0, 1);
   smoothingParamsLayout->addWidget(cyclesLabel, 1, 0);
   smoothingParamsLayout->addWidget(smoothingCyclesSpinBox, 1, 1);
   smoothingParamsLayout->addWidget(iterationsLabel, 2, 0);
   smoothingParamsLayout->addWidget(smoothingIterationsSpinBox, 2, 1);
   smoothingParamsLayout->addWidget(smoothNeighborsLabel, 3, 0);
   smoothingParamsLayout->addWidget(smoothingNeighborsSpinBox, 3, 1);
   smoothingParamsLayout->addWidget(finalSmoothLabel, 4, 0);
   smoothingParamsLayout->addWidget(smoothingFinalSpinBox, 4, 1);
   smoothingParamsGroupBox->setMaximumHeight(smoothingParamsGroupBox->sizeHint().height());

   smoothingParametersWidgetGroup = new WuQWidgetGroup(this);
   smoothingParametersWidgetGroup->addWidget(smoothingStrengthDoubleSpinBox);
   smoothingParametersWidgetGroup->addWidget(smoothingCyclesSpinBox);
   smoothingParametersWidgetGroup->addWidget(smoothingIterationsSpinBox);
   smoothingParametersWidgetGroup->addWidget(smoothingNeighborsSpinBox);
   smoothingParametersWidgetGroup->addWidget(smoothingFinalSpinBox);

   return smoothingParamsGroupBox;
}

/**
 * called when a smoothing parameter is altered.
 */
void
GuiSurfaceDeformationDialog::slotSmoothingParameterChanged()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Smoothing Parameter"
                << std::endl;
   }
    const int stageIndex = sphereEditStageSpinBox->value() - 1;
    const int cycleIndex = sphereEditCycleSpinBox->value() - 1;

    //
    // Get the smoothing parameters
    //
    dmf.setSmoothingParameters(stageIndex,
                               cycleIndex,
                               smoothingStrengthDoubleSpinBox->value(),
                               smoothingCyclesSpinBox->value(),
                               smoothingIterationsSpinBox->value(),
                               smoothingNeighborsSpinBox->value(),
                               smoothingFinalSpinBox->value());
}

/**
 * update the smoothing parameters.
 */
void
GuiSurfaceDeformationDialog::updateSmoothingParameters()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Smoothing Parameters"
                << std::endl;
   }
    const int stageIndex = sphereEditStageSpinBox->value() - 1;
    const int cycleIndex = sphereEditCycleSpinBox->value() - 1;

    smoothingParametersWidgetGroup->blockSignals(true);
    float strength;
    int cycles, iterations, neighbors, finals;
    dmf.getSmoothingParameters(stageIndex, cycleIndex,
                               strength, cycles, iterations, neighbors, finals);
    smoothingStrengthDoubleSpinBox->setValue(strength);
    smoothingCyclesSpinBox->setValue(cycles);
    smoothingIterationsSpinBox->setValue(iterations);
    smoothingNeighborsSpinBox->setValue(neighbors);
    smoothingFinalSpinBox->setValue(finals);
    smoothingParametersWidgetGroup->blockSignals(false);
}

/**
 * update the spherical resolution combo box.
 */
void
GuiSurfaceDeformationDialog::updateSphericalResolutionComboBox()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Spherical Resolution"
                << std::endl;
   }
    const int stageIndex = sphereEditStageSpinBox->value() - 1;
    this->sphereResolutionComboBox->blockSignals(true);
    const int numNodesInSphere = dmf.getSphereResolution(stageIndex);
    switch(numNodesInSphere) {
       case 20:
          sphereResolutionComboBox->setCurrentIndex(0);
          break;
       case 74:
          sphereResolutionComboBox->setCurrentIndex(1);
          break;
       case 290:
          sphereResolutionComboBox->setCurrentIndex(2);
          break;
       case 1154:
          sphereResolutionComboBox->setCurrentIndex(3);
          break;
       case 4610:
          sphereResolutionComboBox->setCurrentIndex(4);
          break;
       case 18434:
          sphereResolutionComboBox->setCurrentIndex(5);
          break;
       case 73730:
          sphereResolutionComboBox->setCurrentIndex(6);
          break;
       default:
          sphereResolutionComboBox->setCurrentIndex(4);
          break;
    }
    this->sphereResolutionComboBox->blockSignals(false);
}

/**
 * called if sphere resolution is changed.
 */
void
GuiSurfaceDeformationDialog::slotSphereResolutionChanged()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Sphere Resolution"
                << std::endl;
   }
    int editStageNumber = sphereEditStageSpinBox->value() - 1;

    switch(sphereResolutionComboBox->currentIndex()) {
       case 0:
          dmf.setSphereResolution(editStageNumber, 20);
          break;
       case 1:
          dmf.setSphereResolution(editStageNumber, 74);
          break;
       case 2:
          dmf.setSphereResolution(editStageNumber, 290);
          break;
       case 3:
          dmf.setSphereResolution(editStageNumber, 1154);
          break;
       case 4:
          dmf.setSphereResolution(editStageNumber, 4610);
          break;
       case 5:
          dmf.setSphereResolution(editStageNumber, 18434);
          break;
       case 6:
          dmf.setSphereResolution(editStageNumber, 73730);
          break;
       default:
          dmf.setSphereResolution(editStageNumber, 4610);
          break;
    }
}

/**
 * create the Spherical parameters section.
 */
QWidget*
GuiSurfaceDeformationDialog::createSphericalParametersSection()
{
   //
   // Number of stages spin box and label
   //
   QLabel* sphereNumberOfStagesLabel = new QLabel("Number of Stages");
   sphereNumberOfStagesSpinBox = new QSpinBox;
   sphereNumberOfStagesSpinBox->setMinimum(1);
   sphereNumberOfStagesSpinBox->setMaximum(DeformationMapFile::MAX_SPHERICAL_STAGES);
   sphereNumberOfStagesSpinBox->setSingleStep(1);
   QObject::connect(sphereNumberOfStagesSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSphereNumberOfStagesSpinBox(int)));

   //
   // Edit Stages spin box and label
   //
   QLabel* sphereEditStagesLabel = new QLabel("Edit Stage");
   sphereEditStageSpinBox = new QSpinBox;
   sphereEditStageSpinBox->setFixedSize(sphereEditStageSpinBox->sizeHint());
   sphereEditStageSpinBox->setMinimum(1);
   sphereEditStageSpinBox->setMaximum(DeformationMapFile::MAX_SPHERICAL_CYCLES);
   QObject::connect(sphereEditStageSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSphereEditStageSpinBox(int)));

   sphereStagesWidgetGroup = new WuQWidgetGroup(this);
   sphereStagesWidgetGroup->addWidget(sphereNumberOfStagesLabel);
   sphereStagesWidgetGroup->addWidget(sphereNumberOfStagesSpinBox);
   sphereStagesWidgetGroup->addWidget(sphereEditStagesLabel);
   sphereStagesWidgetGroup->addWidget(sphereEditStageSpinBox);

   //
   // Sphere resolution combo box and label and HBox
   //
   QLabel* sphereResolutionLabel = new QLabel("Sphere Resolution");
   sphereResolutionComboBox = new QComboBox;
   QObject::connect(sphereResolutionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSphereResolutionChanged()));
   sphereResolutionComboBox->setFixedSize(sphereResolutionComboBox->sizeHint());
   sphereResolutionComboBox->addItem("   20 Nodes");
   sphereResolutionComboBox->addItem("   74 Nodes");
   sphereResolutionComboBox->addItem("  290 Nodes");
   sphereResolutionComboBox->addItem(" 1154 Nodes");
   sphereResolutionComboBox->addItem(" 4610 Nodes");
   sphereResolutionComboBox->addItem("18434 Nodes");
   sphereResolutionComboBox->addItem("73730 Nodes");

   //
   // Number of Cycles spin box and label
   //
   QLabel* numberOfCyclesLabel = new QLabel("Number of Cycles");
   sphereNumberOfCyclesSpinBox = new QSpinBox;
   sphereNumberOfCyclesSpinBox->setMinimum(1);
   sphereNumberOfCyclesSpinBox->setMaximum(DeformationMapFile::MAX_SPHERICAL_CYCLES);
   sphereNumberOfCyclesSpinBox->setSingleStep(1);
   QObject::connect(sphereNumberOfCyclesSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSphereNumberOfCyclesSpinBox(int)));

   //
   // Edit Cycles spin box and label
   //
   QLabel* editCycleLabel = new QLabel("Edit Cycle");
   sphereEditCycleSpinBox = new QSpinBox;
   sphereEditCycleSpinBox->setFixedSize(sphereEditCycleSpinBox->sizeHint());
   sphereEditCycleSpinBox->setMinimum(1);
   sphereEditCycleSpinBox->setMaximum(DeformationMapFile::MAX_SPHERICAL_CYCLES);
   QObject::connect(sphereEditCycleSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSphereEditCycleSpinBox(int)));

   //
   // Fiducial/sphere ratios
   //
   sphereDistortionCorrectionCheckBox = new QCheckBox("Correct For Spherical Distortion\n"
                                                      "Relative to Fiducial");
   QObject::connect(sphereDistortionCorrectionCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotCorrectSphericalDistortionChanged()));
   sphereDistortionDoubleSpinBox = new QDoubleSpinBox;
   QObject::connect(sphereDistortionDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotCorrectSphericalDistortionChanged()));
   sphereDistortionDoubleSpinBox->setMinimum(0.0);
   sphereDistortionDoubleSpinBox->setMaximum(1.0);
   sphereDistortionDoubleSpinBox->setSingleStep(0.5);
   sphereDistortionDoubleSpinBox->setDecimals(3);

   //
   // Group for spherical parameters
   //
   QGroupBox* sphericalParamsGroupBox = new QGroupBox("Spherical Parameters");
   QGridLayout* sphericalParamsGroupLayout = new QGridLayout(sphericalParamsGroupBox);
   sphericalParamsGroupLayout->addWidget(sphereNumberOfStagesLabel, 0, 0);
   sphericalParamsGroupLayout->addWidget(sphereNumberOfStagesSpinBox, 0, 1);
   sphericalParamsGroupLayout->addWidget(sphereEditStagesLabel, 1, 0);
   sphericalParamsGroupLayout->addWidget(sphereEditStageSpinBox, 1, 1);
   sphericalParamsGroupLayout->addWidget(sphereResolutionLabel, 2, 0);
   sphericalParamsGroupLayout->addWidget(sphereResolutionComboBox, 2, 1);
   sphericalParamsGroupLayout->addWidget(numberOfCyclesLabel, 3, 0);
   sphericalParamsGroupLayout->addWidget(sphereNumberOfCyclesSpinBox, 3, 1);
   sphericalParamsGroupLayout->addWidget(editCycleLabel, 4, 0);
   sphericalParamsGroupLayout->addWidget(sphereEditCycleSpinBox, 4, 1);
   sphericalParamsGroupLayout->addWidget(sphereDistortionCorrectionCheckBox, 5, 0);
   sphericalParamsGroupLayout->addWidget(sphereDistortionDoubleSpinBox, 5, 1);
   sphericalParamsGroupBox->setMaximumHeight(sphericalParamsGroupBox->sizeHint().height());

   return sphericalParamsGroupBox;
}

/**
 * update correct spherical distortion correction.
 */
void
GuiSurfaceDeformationDialog::updateCorrectSphericalDistortion()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Spherical Distortion Correction"
                << std::endl;
   }
   bool enabled = false;
   float ratio = 1.0;
   dmf.getSphereFiducialRatio(enabled, ratio);

   sphereDistortionCorrectionCheckBox->blockSignals(true);
   sphereDistortionCorrectionCheckBox->setChecked(enabled);
   sphereDistortionCorrectionCheckBox->blockSignals(false);

   sphereDistortionDoubleSpinBox->blockSignals(true);
   sphereDistortionDoubleSpinBox->setValue(ratio);
   sphereDistortionDoubleSpinBox->blockSignals(false);
}

/**
 * called when a correct for spherical distortion changed.
 */
void
GuiSurfaceDeformationDialog::slotCorrectSphericalDistortionChanged()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Spherical Distortion"
                << std::endl;
   }
    dmf.setSphereFiducialRatio(sphereDistortionCorrectionCheckBox->isChecked(),
                               sphereDistortionDoubleSpinBox->value());
}

/**
 * Create the spherical parameters section.
 */
QWidget*
GuiSurfaceDeformationDialog::createSphericalParameters()
{


   landmarkVectorOptionsWidget = createLandmarkVectorOptionsSection();
   QWidget* sphericalAlgorithmWidget = createSphericalAlgorithmSection();

   QVBoxLayout* leftColumnLayout = new QVBoxLayout;
   leftColumnLayout->addWidget(sphericalAlgorithmWidget);
   leftColumnLayout->addWidget(createSphericalParametersSection());
   leftColumnLayout->addWidget(createSmoothingParametersSection());
   //leftColumnLayout->addStretch();

   QVBoxLayout* rightColumnLayout = new QVBoxLayout;
   rightColumnLayout->addWidget(landmarkVectorOptionsWidget);
   rightColumnLayout->addWidget(createMorphingParametersSection());
   //rightColumnLayout->addStretch();

   QWidget* sphereParamsPageWidget = new QWidget;
   QHBoxLayout* sphereParamsPageLayout = new QHBoxLayout(sphereParamsPageWidget);
   sphereParamsPageLayout->addLayout(leftColumnLayout);
   sphereParamsPageLayout->addLayout(rightColumnLayout);
   
   slotSphericalAlgorithmSelection();

   return sphereParamsPageWidget;
}

/**
 *
 */
QWidget*
GuiSurfaceDeformationDialog::createFlatParameters()
{
   //
   // label and line edit
   //
   QLabel* flatSubSamplingLabel = new QLabel("Sub Sampling Tiles");
   flatSubSamplingTilesSpinBox = new QSpinBox;
   flatSubSamplingTilesSpinBox->setMinimum(1);
   flatSubSamplingTilesSpinBox->setMaximum(50000);
   flatSubSamplingTilesSpinBox->setSingleStep(10);
   flatSubSamplingTilesSpinBox->setFixedWidth(150);
   QObject::connect(flatSubSamplingTilesSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotFlatParameterChanged()));
    
   //
   // label and line edit
   //
   QLabel* flatBetaLabel = new QLabel("Beta");
   flatBetaDoubleSpinBox = new QDoubleSpinBox;
   flatBetaDoubleSpinBox->setFixedWidth(150);
   QObject::connect(flatBetaDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotFlatParameterChanged()));
    
   //
   // label and line edit
   //
   QLabel* flatVarMultLabel = new QLabel("Variance Multiplier");
   flatVarMultDoubleSpinBox = new QDoubleSpinBox;
   flatVarMultDoubleSpinBox->setFixedWidth(150);
   QObject::connect(flatVarMultDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotFlatParameterChanged()));
    
   //
   // label and line edit
   //
   QLabel* flatIterationsLabel = new QLabel("Iterations");
   flatIterationsSpinBox = new QSpinBox;
   flatIterationsSpinBox->setMinimum(0);
   flatIterationsSpinBox->setMaximum(50000);
   flatIterationsSpinBox->setSingleStep(5);
   flatIterationsSpinBox->setFixedWidth(150);
   QObject::connect(flatIterationsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotFlatParameterChanged()));
   
   QGroupBox* flatParamsGroupBox = new QGroupBox("Parameters");
   QGridLayout* flatParamsLayout = new QGridLayout(flatParamsGroupBox);
   flatParamsLayout->addWidget(flatSubSamplingLabel, 0, 0);
   flatParamsLayout->addWidget(flatSubSamplingTilesSpinBox, 0, 1);
   flatParamsLayout->addWidget(flatBetaLabel, 1, 0);
   flatParamsLayout->addWidget(flatBetaDoubleSpinBox, 1, 1);
   flatParamsLayout->addWidget(flatVarMultLabel, 2, 0);
   flatParamsLayout->addWidget(flatVarMultDoubleSpinBox, 2, 1);
   flatParamsLayout->addWidget(flatIterationsLabel, 3, 0);
   flatParamsLayout->addWidget(flatIterationsSpinBox, 3, 1);
   flatParamsGroupBox->setMaximumSize(flatParamsGroupBox->sizeHint());

   return flatParamsGroupBox;
}

/**
 * Create a file entry line (pushbutton and line edit).
 */
FileEntryLine*
GuiSurfaceDeformationDialog::createFileEntryLine(const QString& buttonLabel,
                                                 const FILE_TYPES fileType,
                                                 QGridLayout* parentGridLayout,
                                                 QButtonGroup* buttonGroup,
                                                 QPushButton* extraButton)
{
   //
   // Create the push button
   //
   QPushButton* pb = new QPushButton(buttonLabel);
   pb->setAutoDefault(false);
   buttonGroup->addButton(pb, static_cast<int>(fileType));
   
   //
   // Create the line edit
   //
   const int lineEditWidth = 400;
   QLineEdit* le = new QLineEdit;
   le->setMinimumWidth(lineEditWidth);
   le->setReadOnly(true);
   
   const int rowNum = parentGridLayout->rowCount();
   parentGridLayout->addWidget(pb, rowNum, 0, 1, 1);
   if (extraButton != NULL) {
      parentGridLayout->addWidget(extraButton, rowNum, 1, 1, 1);
      parentGridLayout->addWidget(le, rowNum, 2, 1, 1);
   }
   else {
      parentGridLayout->addWidget(le, rowNum, 1, 1, 2);
   }

   FileEntryLine* fel = new FileEntryLine(pb, le, extraButton);
   return fel;
}

/**
 * Allow user to select atlas files.
 */
void
GuiSurfaceDeformationDialog::atlasFileSelection(int itemNum)
{
   const FILE_TYPES ft = static_cast<FILE_TYPES>(itemNum);
   switch(ft) {
      case FILE_TYPE_SPEC:
         readSpecFile(SELECTION_TYPE_ATLAS);
         break;
      case FILE_TYPE_TOPO_CLOSED:
         displayFileSelection("Select Closed Topo File",
                              atlasDeformationFiles.closedTopoFileNames,
                              atlasDeformationFiles.closedTopoFileTypes,
                              atlasDeformationFiles.closedTopoFileSelected);
         break;
      case FILE_TYPE_TOPO_CUT:
         displayFileSelection("Select Cut Topo File",
                              atlasDeformationFiles.cutTopoFileNames,
                              atlasDeformationFiles.cutTopoFileTypes,
                              atlasDeformationFiles.cutTopoFileSelected);
         break;
      case FILE_TYPE_COORD_FIDUCIAL:
         displayFileSelection("Select Fiducial Coord File",
                              atlasDeformationFiles.fiducialCoordFileNames,
                              atlasDeformationFiles.fiducialCoordFileTypes,
                              atlasDeformationFiles.fiducialCoordFileSelected);
         break;
      case FILE_TYPE_COORD_FLAT:
         displayFileSelection("Select Flat Coord File",
                              atlasDeformationFiles.flatCoordFileNames,
                              atlasDeformationFiles.flatCoordFileTypes,
                              atlasDeformationFiles.flatCoordFileSelected);
         break;
      case FILE_TYPE_COORD_SPHERICAL:
         displayFileSelection("Select Spherical Coord File",
                              atlasDeformationFiles.sphericalCoordFileNames,
                              atlasDeformationFiles.sphericalCoordFileTypes,
                              atlasDeformationFiles.sphericalCoordFileSelected);
         break;
      case FILE_TYPE_BORDER_1:
         displayFileSelection("Select Border File Stage 1",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[0]);
         break;
      case FILE_TYPE_BORDER_2:
         displayFileSelection("Select Border File Stage 2",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[1]);
         break;
      case FILE_TYPE_BORDER_3:
         displayFileSelection("Select Border File Stage 3",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[2]);
         break;
      case FILE_TYPE_BORDER_4:
         displayFileSelection("Select Border File Stage 4",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[3]);
         break;
      case FILE_TYPE_BORDER_5:
         displayFileSelection("Select Border File Stage 5",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[4]);
         break;
      case FILE_TYPE_BORDER_6:
         displayFileSelection("Select Border File Stage 6",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[5]);
         break;
      case FILE_TYPE_BORDER_7:
         displayFileSelection("Select Border File Stage 7",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[6]);
         break;
      case FILE_TYPE_BORDER_8:
         displayFileSelection("Select Border File Stage 8",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[7]);
         break;
      case FILE_TYPE_BORDER_9:
         displayFileSelection("Select Border File Stage 9",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[8]);
         break;
      case FILE_TYPE_BORDER_10:
         displayFileSelection("Select Border File Stage 10",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[9]);
         break;
      case FILE_TYPE_BORDER_11:
         displayFileSelection("Select Border File Stage 11",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[10]);
         break;
      case FILE_TYPE_BORDER_12:
         displayFileSelection("Select Border File Stage 12",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[11]);
         break;
      case FILE_TYPE_BORDER_13:
         displayFileSelection("Select Border File Stage 13",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[12]);
         break;
      case FILE_TYPE_BORDER_14:
         displayFileSelection("Select Border File Stage 14",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[13]);
         break;
      case FILE_TYPE_BORDER_15:
         displayFileSelection("Select Border File Stage 15",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[14]);
         break;
      case FILE_TYPE_BORDER_16:
         displayFileSelection("Select Border File Stage 16",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[15]);
         break;
      case FILE_TYPE_BORDER_17:
         displayFileSelection("Select Border File Stage 17",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[16]);
         break;
      case FILE_TYPE_BORDER_18:
         displayFileSelection("Select Border File Stage 18",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[17]);
         break;
      case FILE_TYPE_BORDER_19:
         displayFileSelection("Select Border File Stage 19",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[18]);
         break;
      case FILE_TYPE_BORDER_20:
         displayFileSelection("Select Border File Stage 20",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected[19]);
         break;
   }
   displayAtlasFiles();
}

/**
 * Select data files for deformation.
 */
void
GuiSurfaceDeformationDialog::displayFileSelection(const QString& title,
                        const std::vector<QString>& labelsIn,
                        const std::vector<DeformationDataFiles::DATA_FILE_TYPES> fileTypes,
                        int& selectedItem)
{
   std::vector<QString> labels;
   
   for (int i = 0; i < static_cast<int>(labelsIn.size()); i++) {
      QString name;
      switch(fileTypes[i]) {
         case DeformationDataFiles::DATA_FILE_BORDER_FLAT:
            name.append("FLAT - ");
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_FLAT_LOBAR:
            name.append("FLAT LOBAR - ");
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_SPHERICAL:
            name.append("SPHERICAL - ");
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_PROJECTION:
            name.append("PROJECTION - ");
            break;
         case DeformationDataFiles::DATA_FILE_TOPO_CLOSED:
            name.append("CLOSED - ");
            break;
         case DeformationDataFiles::DATA_FILE_TOPO_CUT:
            name.append("CUT - ");
            break;
         case DeformationDataFiles::DATA_FILE_TOPO_CUT_LOBAR:
            name.append("CUT LOBAR - ");
            break;
         case DeformationDataFiles::DATA_FILE_COORD_FIDUCIAL:
            break;
         case DeformationDataFiles::DATA_FILE_COORD_FLAT:
            break;
         case DeformationDataFiles::DATA_FILE_COORD_FLAT_LOBAR:
            break;
         case DeformationDataFiles::DATA_FILE_COORD_SPHERICAL:
            break;
      }
      name.append(labelsIn[i]);
      labels.push_back(name);
   }
   
   QtRadioButtonSelectionDialog bsd(this,
                                    title,
                                    "",
                                    labels,
                                    selectedItem);
   if (bsd.exec() == QDialog::Accepted) {
      selectedItem = bsd.getSelectedItemIndex();
   }
}

/**
 * Allow user to select indiv files.
 */
void
GuiSurfaceDeformationDialog::indivFileSelection(int itemNum)
{
   const FILE_TYPES ft = static_cast<FILE_TYPES>(itemNum);
   switch(ft) {
      case FILE_TYPE_SPEC:
         readSpecFile(SELECTION_TYPE_INDIV);
         break;
      case FILE_TYPE_BORDER_1:
      case FILE_TYPE_BORDER_2:
      case FILE_TYPE_BORDER_3:
      case FILE_TYPE_BORDER_4:
      case FILE_TYPE_BORDER_5:
      case FILE_TYPE_BORDER_6:
      case FILE_TYPE_BORDER_7:
      case FILE_TYPE_BORDER_8:
      case FILE_TYPE_BORDER_9:
      case FILE_TYPE_BORDER_10:
      case FILE_TYPE_BORDER_11:
      case FILE_TYPE_BORDER_12:
      case FILE_TYPE_BORDER_13:
      case FILE_TYPE_BORDER_14:
      case FILE_TYPE_BORDER_15:
      case FILE_TYPE_BORDER_16:
      case FILE_TYPE_BORDER_17:
      case FILE_TYPE_BORDER_18:
      case FILE_TYPE_BORDER_19:
      case FILE_TYPE_BORDER_20:
         displayFileSelection("Select Border File",
                              indivDeformationFiles.borderFileNames,
                              indivDeformationFiles.borderFileTypes,
                              indivDeformationFiles.borderFileSelected[0]);
         break;
      case FILE_TYPE_TOPO_CLOSED:
         displayFileSelection("Select Closed Topo File",
                              indivDeformationFiles.closedTopoFileNames,
                              indivDeformationFiles.closedTopoFileTypes,
                              indivDeformationFiles.closedTopoFileSelected);
         break;
      case FILE_TYPE_TOPO_CUT:
         displayFileSelection("Select Cut Topo File",
                              indivDeformationFiles.cutTopoFileNames,
                              indivDeformationFiles.cutTopoFileTypes,
                              indivDeformationFiles.cutTopoFileSelected);
         break;
      case FILE_TYPE_COORD_FIDUCIAL:
         displayFileSelection("Select Fiducial Coord File",
                              indivDeformationFiles.fiducialCoordFileNames,
                              indivDeformationFiles.fiducialCoordFileTypes,
                              indivDeformationFiles.fiducialCoordFileSelected);
         break;
      case FILE_TYPE_COORD_FLAT:
         displayFileSelection("Select Flat Coord File",
                              indivDeformationFiles.flatCoordFileNames,
                              indivDeformationFiles.flatCoordFileTypes,
                              indivDeformationFiles.flatCoordFileSelected);
         break;
      case FILE_TYPE_COORD_SPHERICAL:
         displayFileSelection("Select Spherical Coord File",
                              indivDeformationFiles.sphericalCoordFileNames,
                              indivDeformationFiles.sphericalCoordFileTypes,
                              indivDeformationFiles.sphericalCoordFileSelected);
         break;
   }
   displayIndivFiles();
}

/**
 * Read an atlas or individual spec file
 */
void
GuiSurfaceDeformationDialog::readSpecFile(const SELECTION_TYPE st)
{
   //
   // Use choose spec file dialog
   //
   GuiChooseSpecFileDialog openSpec(this, theMainWindow->getBrainSet()->getPreferencesFile(), true);
   if (openSpec.exec() == QDialog::Accepted) {
      const QString sname(openSpec.getSelectedSpecFile());
      
      //
      // Save the current directory since reading a spec file will change it
      //
      const QString currentDirectory = QDir::currentPath();
      
      //
      // Read the spec file
      //
      SpecFile sf;
      try {
         sf.readFile(sname);
      }
      catch (FileException& e) {
         QMessageBox::critical(this, "Error Reading Spec", e.whatQString());
         QDir::setCurrent(currentDirectory);
         return;
      }
      
      bool flatDeformFlag = false;
      switch(dmf.getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            flatDeformFlag = true;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            flatDeformFlag = false;
            break;
      }
      
      switch(st) {
         case SELECTION_TYPE_ATLAS:
            atlasSpecFileName = sname;
            atlasDeformationFiles.loadSpecFile(sf, flatDeformFlag);
            displayAtlasFiles();
            break;
         case SELECTION_TYPE_INDIV:
            indivSpecFileName = sname;
            indivDeformationFiles.loadSpecFile(sf, flatDeformFlag);
            displayIndivFiles();
            break;
      }
      
      //
      // Restore the current directory after reading the spec file
      //
      QDir::setCurrent(currentDirectory);
   }
}

/**
 * Load the deformation map file.
 */
void 
GuiSurfaceDeformationDialog::loadDeformationMapFile(std::vector<QString>& errorMessages)
{
   errorMessages.clear();
   
   //
   // Set the source border file type
   //
   DeformationMapFile::BORDER_FILE_TYPE bft = DeformationMapFile::BORDER_FILE_UNKNOWN;
   if (indivDeformationFiles.borderFileSelected >= 0) {
      switch(indivDeformationFiles.borderFileTypes[indivDeformationFiles.borderFileSelected[0]]) {
         case DeformationDataFiles::DATA_FILE_BORDER_FLAT:
            bft = DeformationMapFile::BORDER_FILE_FLAT;
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_FLAT_LOBAR:
            bft = DeformationMapFile::BORDER_FILE_FLAT_LOBAR;
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_SPHERICAL:
            bft = DeformationMapFile::BORDER_FILE_SPHERICAL;
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_PROJECTION:
            bft = DeformationMapFile::BORDER_FILE_PROJECTION;
            break;
         case DeformationDataFiles::DATA_FILE_TOPO_CLOSED:
         case DeformationDataFiles::DATA_FILE_TOPO_CUT:
         case DeformationDataFiles::DATA_FILE_TOPO_CUT_LOBAR:
         case DeformationDataFiles::DATA_FILE_COORD_FIDUCIAL:
         case DeformationDataFiles::DATA_FILE_COORD_FLAT:
         case DeformationDataFiles::DATA_FILE_COORD_FLAT_LOBAR:
         case DeformationDataFiles::DATA_FILE_COORD_SPHERICAL:
            break;
      }
   }
   
   //
   // Set the source file names
   //
   dmf.setSourceSpecFileName(indivSpecLineEdit->text());
   const QString sourceBorderName(indivBorderLineEdit->text());
   dmf.setSourceBorderFileName(sourceBorderName, bft);
   dmf.setSourceClosedTopoFileName(indivClosedTopoLineEdit->text());
   dmf.setSourceCutTopoFileName(indivCutTopoLineEdit->text());
   dmf.setSourceFiducialCoordFileName(indivFiducialCoordLineEdit->text());
   dmf.setSourceSphericalCoordFileName(indivSphericalCoordLineEdit->text());
   dmf.setSourceFlatCoordFileName(indivFlatCoordLineEdit->text());
   
   //
   // Check to see that required individual files are provided
   //
   if (dmf.getSourceSpecFileName().isEmpty()) {
      errorMessages.push_back("Required individual spec file is missing.");
   }
   if (sourceBorderName.isEmpty()) {
      errorMessages.push_back("Required individual border file is missing.");
   }
   if (dmf.getSourceClosedTopoFileName().isEmpty()) {
      errorMessages.push_back("Required individual closed topo file is missing.");
   }
   if (dmf.getSourceCutTopoFileName().isEmpty()) {
      errorMessages.push_back("Required individual cut topo file is missing.");
   }
   if (dmf.getSourceFiducialCoordFileName().isEmpty()) {
      errorMessages.push_back("Required individual fiducial coord file is missing.");
   }      
   switch(dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         if (dmf.getSourceFlatCoordFileName().isEmpty()) {
            errorMessages.push_back("Required individual flat coord file is missing.");
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         if (dmf.getSourceSphericalCoordFileName().isEmpty()) {
            errorMessages.push_back("Required individual source spherical coord file is missing.");
         }
         break;
   }
   
   //
   // Set the target border file type
   // 
   bft = DeformationMapFile::BORDER_FILE_UNKNOWN;
   if (atlasDeformationFiles.borderFileSelected[0] >= 0) {
      switch(atlasDeformationFiles.borderFileTypes[atlasDeformationFiles.borderFileSelected[0]]) {
         case DeformationDataFiles::DATA_FILE_BORDER_FLAT:
            bft = DeformationMapFile::BORDER_FILE_FLAT;
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_FLAT_LOBAR:
            bft = DeformationMapFile::BORDER_FILE_FLAT_LOBAR;
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_SPHERICAL:
            bft = DeformationMapFile::BORDER_FILE_SPHERICAL;
            break;
         case DeformationDataFiles::DATA_FILE_BORDER_PROJECTION:
            bft = DeformationMapFile::BORDER_FILE_PROJECTION;
            break;
         case DeformationDataFiles::DATA_FILE_TOPO_CLOSED:
         case DeformationDataFiles::DATA_FILE_TOPO_CUT:
         case DeformationDataFiles::DATA_FILE_TOPO_CUT_LOBAR:
         case DeformationDataFiles::DATA_FILE_COORD_FIDUCIAL:
         case DeformationDataFiles::DATA_FILE_COORD_FLAT:
         case DeformationDataFiles::DATA_FILE_COORD_FLAT_LOBAR:
         case DeformationDataFiles::DATA_FILE_COORD_SPHERICAL:
            break;
      }
   }
   
   //
   // Set the target file names
   //
   dmf.setTargetSpecFileName(atlasSpecLineEdit->text());
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
      const QString targetBorderName(atlasBorderLineEdit[i]->text());
      dmf.setTargetBorderFileName(i, targetBorderName, bft);
      if (i < dmf.getSphericalNumberOfStages()) {
         if (targetBorderName.isEmpty()) {
            errorMessages.push_back(
               "Required atlas border file "
               + QString::number(i + 1)
               + " is missing.");
         }
      }
   }
   dmf.setTargetClosedTopoFileName(atlasClosedTopoLineEdit->text());
   dmf.setTargetCutTopoFileName(atlasCutTopoLineEdit->text());
   dmf.setTargetFiducialCoordFileName(atlasFiducialCoordLineEdit->text());
   dmf.setTargetSphericalCoordFileName(atlasSphericalCoordLineEdit->text());
   dmf.setTargetFlatCoordFileName(atlasFlatCoordLineEdit->text());
   
   //
   // Check to see that required atlas files are provided
   //
   if (dmf.getTargetSpecFileName().isEmpty()) {
      errorMessages.push_back("Required atlas spec file is missing.");
   }
   if (dmf.getTargetClosedTopoFileName().isEmpty()) {
      errorMessages.push_back("Required atlas closed topo file is missing.");
   }
   if (dmf.getTargetCutTopoFileName().isEmpty()) {
      errorMessages.push_back("Required atlas cut topo file is missing.");
   }
   if (dmf.getTargetFiducialCoordFileName().isEmpty()) {
      errorMessages.push_back("Required atlas fiducial coord file is missing.");
   }      
   switch(dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         if (dmf.getTargetFlatCoordFileName().isEmpty()) {
            errorMessages.push_back("Required atlas flat coord file is missing.");
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         if (dmf.getTargetSphericalCoordFileName().isEmpty()) {
            errorMessages.push_back("Required individual atlas spherical coord file is missing.");
         }
         break;
   }
}

/**
 * called when apply button pressed.
 */
void
GuiSurfaceDeformationDialog::slotApplyButton()
{
  std::vector<QString> errorMessages;

  //
  // Copy parameters to the deformation map file.
  //
  loadDeformationMapFile(errorMessages);

  //
  // Are any required files missing ?
  //
  if (errorMessages.size() > 0) {
     const QString msg = StringUtilities::combine(errorMessages, "\n");
     QMessageBox::critical(this, "Files Missing", msg);
     return;
  }
  switch(dmf.getFlatOrSphereSelection()) {
     case DeformationMapFile::DEFORMATION_TYPE_FLAT:
        break;
     case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
     case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
     case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
        if ((this->sphericalLandmarkConstrainedRadioButton->isChecked() == false) &&
            (this->sphericalLandmarkVectorRadioButton->isChecked() == false) &&
            (this->sphericalLandmarkVectorSingleStageRadioButton->isChecked() == false)) {
           QApplication::processEvents();
           QMessageBox::critical(this, "Algorithm Selection",
                 "You must choose the spherical algorithm (landmark "
                 "constrained or landmark vector difference) on the "
                 "Spherical Parameters page.");
           return;
        }
        break;
  }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  DisplaySettingsSurface* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
  QString completeMessage("The deformation has completed.");

  try {
     switch(dmf.getFlatOrSphereSelection()) {
        case DeformationMapFile::DEFORMATION_TYPE_FLAT:
           {
              BrainModelSurfaceDeformationFlat
                 bmsdf(theMainWindow->getBrainSet(), &dmf);
              bmsdf.setDeformationMapFileNames(
                   this->indivToAtlasDeformMapFileNameLineEdit->text(),
                   this->atlasToIndivDeformMapFileNameLineEdit->text());
              bmsdf.execute();

              //
              // Check for deformation errors
              //
              const QString s2tMsg(bmsdf.getSourceToTargetDeformDataFileErrors());
              if (s2tMsg.isEmpty() == false) {
                 completeMessage.append("\nErrors were detected deforming the following data files from\n "
                            "the individual to the atlas:\n");
                 completeMessage.append(s2tMsg);
              }
              const QString t2sMsg(bmsdf.getTargetToSourceDeformDataFileErrors());
              if (t2sMsg.isEmpty() == false) {
                 completeMessage.append("\nErrors were detected deforming the following data files from\n "
                            "the atlas to the individual:\n");
                 completeMessage.append(t2sMsg);
              }
           }
           break;
        case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
           {
               dss->setDrawMode(DisplaySettingsSurface::DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL);
               BrainModelSurfaceDeformationSpherical
                   bmsds(theMainWindow->getBrainSet(), &dmf);
               bmsds.setDeformationMapFileNames(
                       this->indivToAtlasDeformMapFileNameLineEdit->text(),
                       this->atlasToIndivDeformMapFileNameLineEdit->text());
               //
               // Enable coord file deformation for sphere
               //
               bmsds.setDeformSourceFiducialCoordFiles(deformIndivFiducialCoordCheckBox->isChecked());
               bmsds.setDeformSourceInflatedCoordFiles(deformIndivInflatedCoordCheckBox->isChecked());
               bmsds.setDeformSourceVeryInflatedCoordFiles(deformIndivVeryInflatedCoordCheckBox->isChecked());
               bmsds.setDeformSourceSphericalCoordFiles(deformIndivSphericalCoordCheckBox->isChecked());
               bmsds.setDeformSourceFlatCoordFiles(deformIndivFlatCoordCheckBox->isChecked());

               //
               // Main Window Surface is used for Viewing Transformations
               //
               bmsds.setsurfaceWithViewingTransformations(theMainWindow->getBrainModelSurface());
               bmsds.execute();

              //
              // Check for deformation errors
              //
              const QString s2tMsg(bmsds.getSourceToTargetDeformDataFileErrors());
              if (s2tMsg.isEmpty() == false) {
                 completeMessage.append("\nErrors were detected deforming the following data files from\n "
                            "the individual to the atlas:\n");
                 completeMessage.append(s2tMsg);
              }
              const QString t2sMsg(bmsds.getTargetToSourceDeformDataFileErrors());
              if (t2sMsg.isEmpty() == false) {
                 completeMessage.append("\nErrors were detected deforming the following data files from\n "
                            "the atlas to the individual:\n");
                 completeMessage.append(t2sMsg);
              }
           }
           break;
        case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
           {
               dss->setDrawMode(DisplaySettingsSurface::DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL);
               BrainModelSurfaceDeformationMultiStageSphericalVector
                  bmsdsv(theMainWindow->getBrainSet(), &dmf);
               bmsdsv.setDeformationMapFileNames(
                       this->indivToAtlasDeformMapFileNameLineEdit->text(),
                       this->atlasToIndivDeformMapFileNameLineEdit->text());
               //
               // Enable coord file deformation for sphere
               //
               bmsdsv.setDeformSourceFiducialCoordFiles(deformIndivFiducialCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceInflatedCoordFiles(deformIndivInflatedCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceVeryInflatedCoordFiles(deformIndivVeryInflatedCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceSphericalCoordFiles(deformIndivSphericalCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceFlatCoordFiles(deformIndivFlatCoordCheckBox->isChecked());

               //
               // Main Window Surface is used for Viewing Transformations
               //
               bmsdsv.setsurfaceWithViewingTransformations(theMainWindow->getBrainModelSurface());
               bmsdsv.execute();

              //
              // Check for deformation errors
              //
              const QString s2tMsg(bmsdsv.getSourceToTargetDeformDataFileErrors());
              if (s2tMsg.isEmpty() == false) {
                 completeMessage.append("\nErrors were detected deforming the following data files from\n "
                            "the individual to the atlas:\n");
                 completeMessage.append(s2tMsg);
              }
           }
           break;
        case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
           {
               dss->setDrawMode(DisplaySettingsSurface::DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL);
               BrainModelSurfaceDeformationSphericalVector
                  bmsdsv(theMainWindow->getBrainSet(), &dmf);
               bmsdsv.setDeformationMapFileNames(
                       this->indivToAtlasDeformMapFileNameLineEdit->text(),
                       this->atlasToIndivDeformMapFileNameLineEdit->text());
               //
               // Enable coord file deformation for sphere
               //
               bmsdsv.setDeformSourceFiducialCoordFiles(deformIndivFiducialCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceInflatedCoordFiles(deformIndivInflatedCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceVeryInflatedCoordFiles(deformIndivVeryInflatedCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceSphericalCoordFiles(deformIndivSphericalCoordCheckBox->isChecked());
               bmsdsv.setDeformSourceFlatCoordFiles(deformIndivFlatCoordCheckBox->isChecked());

               //
               // Main Window Surface is used for Viewing Transformations
               //
               bmsdsv.setsurfaceWithViewingTransformations(theMainWindow->getBrainModelSurface());
               bmsdsv.execute();

              //
              // Check for deformation errors
              //
              const QString s2tMsg(bmsdsv.getSourceToTargetDeformDataFileErrors());
              if (s2tMsg.isEmpty() == false) {
                 completeMessage.append("\nErrors were detected deforming the following data files from\n "
                            "the individual to the atlas:\n");
                 completeMessage.append(s2tMsg);
              }
           }
           break;
     }
  }
  catch (BrainModelAlgorithmException& e) {
     QApplication::restoreOverrideCursor();
     QMessageBox::critical(this, "Deformation Error", e.whatQString());
     return;
  }

  //
  // Let the user know that the deformation has completed
  //
  QApplication::restoreOverrideCursor();
  QMessageBox::information(this, "Deformation Completed", completeMessage);
}

/**
 * Display the atlas data files.
 */
void
GuiSurfaceDeformationDialog::displayAtlasFiles()
{
   atlasSpecLineEdit->setText(atlasSpecFileName);
   
   if (atlasDeformationFiles.borderFileNames.size() > 0) {
      for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
         atlasBorderLineEdit[i]->setText(
            atlasDeformationFiles.borderFileNames[atlasDeformationFiles.borderFileSelected[i]]);
      }
   }
   else {
      for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
         atlasBorderLineEdit[i]->setText("");
      }
   }
   
   if (atlasDeformationFiles.closedTopoFileNames.size() > 0) {
      atlasClosedTopoLineEdit->setText(
         atlasDeformationFiles.closedTopoFileNames[atlasDeformationFiles.closedTopoFileSelected]);
   }
   else {
      atlasClosedTopoLineEdit->setText("");
   }
   
   if (atlasDeformationFiles.cutTopoFileNames.size() > 0) {
      atlasCutTopoLineEdit->setText(
         atlasDeformationFiles.cutTopoFileNames[atlasDeformationFiles.cutTopoFileSelected]);
   }
   else {
      atlasCutTopoLineEdit->setText("");
   }
   
   if (atlasDeformationFiles.fiducialCoordFileNames.size() > 0) {
      atlasFiducialCoordLineEdit->setText(
         atlasDeformationFiles.fiducialCoordFileNames[atlasDeformationFiles.fiducialCoordFileSelected]);
   }
   else {
      atlasFiducialCoordLineEdit->setText("");
   }
   
   if (atlasDeformationFiles.sphericalCoordFileNames.size() > 0) {
      atlasSphericalCoordLineEdit->setText(
         atlasDeformationFiles.sphericalCoordFileNames[atlasDeformationFiles.sphericalCoordFileSelected]);
   }
   else {
      atlasSphericalCoordLineEdit->setText("");
   }
   
   if (atlasDeformationFiles.flatCoordFileNames.size() > 0) {
      atlasFlatCoordLineEdit->setText(
         atlasDeformationFiles.flatCoordFileNames[atlasDeformationFiles.flatCoordFileSelected]);
   }
   else {
      atlasFlatCoordLineEdit->setText("");
   }
   
}

/**
 * Display the indiv data files.
 */
void
GuiSurfaceDeformationDialog::displayIndivFiles()
{
   indivSpecLineEdit->setText(indivSpecFileName);
   
   if (indivDeformationFiles.borderFileNames.size() > 0) {
      indivBorderLineEdit->setText(
         indivDeformationFiles.borderFileNames[indivDeformationFiles.borderFileSelected[0]]);
   }
   else {
      indivBorderLineEdit->setText("");
   }
   
   if (indivDeformationFiles.closedTopoFileNames.size() > 0) {
      indivClosedTopoLineEdit->setText(
         indivDeformationFiles.closedTopoFileNames[indivDeformationFiles.closedTopoFileSelected]);
   }
   else {
      indivClosedTopoLineEdit->setText("");
   }
   
   if (indivDeformationFiles.cutTopoFileNames.size() > 0) {
      indivCutTopoLineEdit->setText(
         indivDeformationFiles.cutTopoFileNames[indivDeformationFiles.cutTopoFileSelected]);
   }
   else {
      indivCutTopoLineEdit->setText("");
   }
   
   if (indivDeformationFiles.fiducialCoordFileNames.size() > 0) {
      indivFiducialCoordLineEdit->setText(
         indivDeformationFiles.fiducialCoordFileNames[indivDeformationFiles.fiducialCoordFileSelected]);
   }
   else {
      indivFiducialCoordLineEdit->setText("");
   }
   
   if (indivDeformationFiles.sphericalCoordFileNames.size() > 0) {
      indivSphericalCoordLineEdit->setText(
         indivDeformationFiles.sphericalCoordFileNames[indivDeformationFiles.sphericalCoordFileSelected]);
   }
   else {
      indivSphericalCoordLineEdit->setText("");
   }
   
   if (indivDeformationFiles.flatCoordFileNames.size() > 0) {
      indivFlatCoordLineEdit->setText(
         indivDeformationFiles.flatCoordFileNames[indivDeformationFiles.flatCoordFileSelected]);
   }
   else {
      indivFlatCoordLineEdit->setText("");
   }
   
}

/**
 * Called when standard parameters pushbutton pressed.
 */
void
GuiSurfaceDeformationDialog::slotStandardParametersPushButton()  
{
   QString subDirName;
   switch (dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         subDirName = "/data_files/spherical_registration";
         break;
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         subDirName = "/data_files/flat_registration";
         break;
   }
   if (subDirName.isEmpty()) {
      return;
   }
   
   QString templateDirectory(theMainWindow->getBrainSet()->getCaretHomeDirectory());
   templateDirectory.append(subDirName);   
   std::vector<QString> files;
   FileUtilities::findFilesInDirectory(templateDirectory,
                                       QStringList("*.deform_map"),
                                       files);
   
   if (files.empty()) {
      QString msg("No standard deformation map files were found in the directory\n");
      msg.append(templateDirectory);
      QMessageBox::critical(this, "Files Not Found", msg);
      return;
   }
   
   std::vector<QString> templateFiles;
   std::vector<QString> labels;
   for (unsigned int i = 0; i < files.size(); i++) {
      DeformationMapFile df;
      try {
         QString name(templateDirectory);
         name.append("/");
         name.append(files[i]);
         
         df.readFile(name);
         
         const QString descriptiveName(df.getHeaderTag("descriptive_name"));
         if (descriptiveName.isEmpty() == false) {
            labels.push_back(descriptiveName);
         }
         else {
            labels.push_back(files[i]);
         }
         templateFiles.push_back(name);
      }
      catch (FileException& /*e*/) {
      }
   }

   QtRadioButtonSelectionDialog rbd(this,
                           "Choose Registration Parameters",
                           "",
                           labels,
                           -1);
   if (rbd.exec() == QDialog::Accepted) {
      try {
         const int fileNum = rbd.getSelectedItemIndex();
         dmf.clear();
         dmf.readFile(templateFiles[fileNum]);
         loadParametersIntoDialog();
      }
      catch (FileException& e) {
         QMessageBox::critical(this, "Open Error", e.whatQString());
         return;
      }
      
   }
}

/**
 * called when save deformation map file push button is pressed.
 */
void
GuiSurfaceDeformationDialog::saveDeformationMapFilePushButton()
{
   //
   // Create a spec file dialog to select the spec file.
   //
   WuQFileDialog saveDefMap(this);
   saveDefMap.setModal(true);
   saveDefMap.setWindowTitle("Choose Deformation Map File");
   saveDefMap.setFileMode(WuQFileDialog::AnyFile);
   saveDefMap.setAcceptMode(WuQFileDialog::AcceptSave);
   saveDefMap.setHistory(theMainWindow->getBrainSet()->getPreferencesFile()->getRecentDataFileDirectories());
   saveDefMap.setDirectory(QDir::currentPath());
  QString filterString("DeformationMapFile (*");
   filterString.append(SpecFile::getDeformationMapFileExtension());
   filterString.append(")");
   saveDefMap.setFilters(QStringList(filterString));
   if (saveDefMap.exec() == QDialog::Accepted) {
      try {
         if (saveDefMap.selectedFiles().count() > 0) {
            dmf.writeFile(saveDefMap.selectedFiles().at(0));
         }
      }
      catch (FileException& e) {
         QMessageBox::critical(this, "Save Error", e.whatQString());
         return;
      }
   }
}


/**
 * Called to load a deformation map file.
 */
void
GuiSurfaceDeformationDialog::loadDeformationMapFilePushButton()
{
   //
   // Create a spec file dialog to select the spec file.
   //
   WuQFileDialog openDefMap(this);
   openDefMap.setModal(true);
   openDefMap.setWindowTitle("Choose Deformation Map File");
   openDefMap.setFileMode(WuQFileDialog::ExistingFile);
   openDefMap.setAcceptMode(WuQFileDialog::AcceptOpen);
   openDefMap.setHistory(theMainWindow->getBrainSet()->getPreferencesFile()->getRecentDataFileDirectories());
   openDefMap.setDirectory(QDir::currentPath());
   QString filterString("DeformationMapFile (*");
   filterString.append(SpecFile::getDeformationMapFileExtension());
   filterString.append(")");
   openDefMap.setFilters(QStringList(filterString));
   if (openDefMap.exec() == QDialog::Accepted) {
      try {
         dmf.clear();
         if (openDefMap.selectedFiles().count() > 0) {
            dmf.readFile(openDefMap.selectedFiles().at(0));
            loadParametersIntoDialog();
         }
      }
      catch (FileException& e) {
         QMessageBox::critical(this, "Open Error", e.whatQString());
         return;
      }
   }
}

/**
 * called to read the flat parameters from the dialog.
 */
void
GuiSurfaceDeformationDialog::slotFlatParameterChanged()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Flat Parameter"
                << std::endl;
   }
    dmf.setFlatParameters(flatSubSamplingTilesSpinBox->value(),
                          flatBetaDoubleSpinBox->value(),
                          flatVarMultDoubleSpinBox->value(),
                          flatIterationsSpinBox->value());
}

/**
 * update the flat parameters
 */
void
GuiSurfaceDeformationDialog::updateFlatParameters()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Flat Parameters"
                << std::endl;
   }

    int subSamplingTiles, numIters;
    float beta, varMult;
    dmf.getFlatParameters(subSamplingTiles, beta, varMult, numIters);
    flatSubSamplingTilesSpinBox->blockSignals(true);
    flatSubSamplingTilesSpinBox->setValue(subSamplingTiles);
    flatSubSamplingTilesSpinBox->blockSignals(false);
    flatBetaDoubleSpinBox->blockSignals(true);
    flatBetaDoubleSpinBox->setValue(beta);
    flatBetaDoubleSpinBox->blockSignals(false);
    flatVarMultDoubleSpinBox->blockSignals(true);
    flatVarMultDoubleSpinBox->setValue(varMult);
    flatVarMultDoubleSpinBox->blockSignals(false);
    flatIterationsSpinBox->blockSignals(true);
    flatIterationsSpinBox->setValue(numIters);
    flatIterationsSpinBox->blockSignals(false);
}

/**
 * called to read the deformation parameters from the dialog.
 */
void
GuiSurfaceDeformationDialog::slotDeformationParameterChanged()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Deformation Parameter"
                << std::endl;
   }
   //
   // set border resampling
   //
   DeformationMapFile::BORDER_RESAMPLING_TYPE borderResample
      = DeformationMapFile::BORDER_RESAMPLING_NONE;
   if (borderResamplingNoneRadioButton->isChecked()) {
      borderResample = DeformationMapFile::BORDER_RESAMPLING_NONE;
   }
   else if (borderResamplingFromFileRadioButton->isChecked()) {
      borderResample = DeformationMapFile::BORDER_RESAMPLING_FROM_BORDER_FILE;
   }
   else if (borderResamplingToValueRadioButton->isChecked()) {
      borderResample = DeformationMapFile::BORDER_RESAMPLING_VALUE;
   }
   dmf.setBorderResampling(borderResample, borderResamplingDoubleSpinBox->value());

   //
   // set metric deformation
   //
   if (metricNearestNodeRadioButton->isChecked()) {
      dmf.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_NEAREST_NODE);
   }
   else if (metricAverageTileNodesRadioButton->isChecked()) {
      dmf.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES);
   }

   //
   // set deformation stuff
   //
   dmf.setDeformedFileNamePrefix(deformedFilePrefixLineEdit->text());
   dmf.setDeformedColumnNamePrefix(deformedColumnPrefixLineEdit->text());
   dmf.setDeformBothWays(deformBothWaysCheckBox->isChecked());
   dmf.setDeleteIntermediateFiles(deleteIntermediateFilesCheckBox->isChecked());

   dmf.setSmoothDeformedSurfacesFlag(smoothCoordsOneIterationCheckBox->isChecked());
}

/**
 * update the deformation parameters.
 */
void
GuiSurfaceDeformationDialog::updateDeformationParametersPage()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Deformation Parameters"
                << std::endl;
   }
   //
   // set border resampling
   //
   DeformationMapFile::BORDER_RESAMPLING_TYPE borderResample;
   float resampleValue;
   dmf.getBorderResampling(borderResample, resampleValue);
   borderResamplingNoneRadioButton->blockSignals(true);
   borderResamplingFromFileRadioButton->blockSignals(true);
   borderResamplingToValueRadioButton->blockSignals(false);
   switch(borderResample) {
      case DeformationMapFile::BORDER_RESAMPLING_NONE:
         borderResamplingNoneRadioButton->setChecked(true);
         break;
      case DeformationMapFile::BORDER_RESAMPLING_FROM_BORDER_FILE:
         borderResamplingFromFileRadioButton->setChecked(true);
         break;
      case DeformationMapFile::BORDER_RESAMPLING_VALUE:
         borderResamplingToValueRadioButton->setChecked(true);
         break;
   }
   borderResamplingNoneRadioButton->blockSignals(false);
   borderResamplingFromFileRadioButton->blockSignals(false);
   borderResamplingToValueRadioButton->blockSignals(false);

   borderResamplingDoubleSpinBox->blockSignals(true);
   borderResamplingDoubleSpinBox->setValue(resampleValue);
   borderResamplingDoubleSpinBox->blockSignals(false);

   //
   // set metric deformation
   //
   metricNearestNodeRadioButton->blockSignals(true);
   metricAverageTileNodesRadioButton->blockSignals(true);
   switch(dmf.getMetricDeformationType()) {
      case DeformationMapFile::METRIC_DEFORM_NEAREST_NODE:
         metricNearestNodeRadioButton->setChecked(true);
         break;
      case DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES:
         metricAverageTileNodesRadioButton->setChecked(true);
         break;
   }
   metricNearestNodeRadioButton->blockSignals(false);
   metricAverageTileNodesRadioButton->blockSignals(false);

   //
   // set deformation stuff
   //
   deformedFilePrefixLineEdit->blockSignals(true);
   deformedFilePrefixLineEdit->setText(dmf.getDeformedFileNamePrefix());
   deformedFilePrefixLineEdit->blockSignals(false);
   deformedColumnPrefixLineEdit->blockSignals(true);
   deformedColumnPrefixLineEdit->setText(dmf.getDeformedColumnNamePrefix());
   deformedColumnPrefixLineEdit->blockSignals(false);
   deformBothWaysCheckBox->blockSignals(true);
   deformBothWaysCheckBox->setChecked(dmf.getDeformBothWays());
   deformBothWaysCheckBox->blockSignals(false);
   deleteIntermediateFilesCheckBox->blockSignals(true);
   deleteIntermediateFilesCheckBox->setChecked(dmf.getDeleteIntermediateFiles());
   deleteIntermediateFilesCheckBox->blockSignals(false);
   smoothCoordsOneIterationCheckBox->blockSignals(true);
   smoothCoordsOneIterationCheckBox->setChecked(dmf.getSmoothDeformedSurfacesFlag());
   smoothCoordsOneIterationCheckBox->blockSignals(false);
}


/**
 * Load the parameters from a deformation map file.
 */
void
GuiSurfaceDeformationDialog::loadParametersIntoDialog()
{
   switch(dmf.getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         updateFlatParameters();
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         updateAlgorithmSelection();
         sphereNumberOfStagesSpinBox->blockSignals(true);
         sphereNumberOfStagesSpinBox->setValue(dmf.getSphericalNumberOfStages());
         sphereNumberOfStagesSpinBox->blockSignals(false);
         sphereEditStageSpinBox->blockSignals(true);
         sphereEditStageSpinBox->setValue(1);
         sphereEditStageSpinBox->blockSignals(false);
         slotSphereNumberOfStagesSpinBox(sphereNumberOfStagesSpinBox->value());
         updateCorrectSphericalDistortion();
         slotSphericalAlgorithmSelection();
         updateAtlasBorderSelections();
         slotUpdateDeformationMapFileNames();
         break;
   }

   updateDeformationParametersPage();
}

/**
 * update the spherical number of stages spin box.
 */
void
GuiSurfaceDeformationDialog::updateSphereNumberOfStagesSpinBox()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Number of Stages"
                << std::endl;
   }
   sphereNumberOfStagesSpinBox->blockSignals(true);
   sphereNumberOfStagesSpinBox->setValue(dmf.getSphericalNumberOfStages());
   updateSphereEditStageSpinBox();
   sphereNumberOfStagesSpinBox->blockSignals(false);
   updateAtlasBorderSelections();
}

/**
 * called when number of spherical stages is changed.
 */
void
GuiSurfaceDeformationDialog::slotSphereNumberOfStagesSpinBox(int item)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Number of Stages"
                << std::endl;
   }
   dmf.setSphericalNumberOfStages(item);
   updateSphereEditStageSpinBox();
   this->updateSphericalResolutionComboBox();
   this->updateLandmarkVectorParameters();
   this->updateMorphingParameters();
   this->updateSmoothingParameters();
   this->updateAtlasBorderSelections();
}

/**
 * update the sphere edit stage spin box.
 */
void
GuiSurfaceDeformationDialog::updateSphereEditStageSpinBox()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Edit Stage"
                << std::endl;
   }
   sphereEditStageSpinBox->blockSignals(true);
   sphereEditStageSpinBox->setMaximum(
           dmf.getSphericalNumberOfStages());
   sphereEditStageSpinBox->blockSignals(false);
   updateSphereNumberOfCyclesSpinBox();
}

/**
 * called when sphere edit stage is changed.
 */
void
GuiSurfaceDeformationDialog::slotSphereEditStageSpinBox(int /*value*/)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Edit Stage"
                << std::endl;
   }
   updateSphereNumberOfCyclesSpinBox();
   this->updateSphericalResolutionComboBox();
   this->updateLandmarkVectorParameters();
   this->updateMorphingParameters();
   this->updateSmoothingParameters();
}

/**
 * update the sphere number of cycles spin box.
 */
void
GuiSurfaceDeformationDialog::updateSphereNumberOfCyclesSpinBox()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Sphere Number of Cycles"
                << std::endl;
   }
   sphereNumberOfCyclesSpinBox->blockSignals(true);
   int stageIndex = sphereEditStageSpinBox->value() - 1;
   sphereNumberOfCyclesSpinBox->setValue(
         dmf.getSphericalNumberOfCycles(stageIndex));
   sphereNumberOfCyclesSpinBox->blockSignals(false);
   updateSphereEditCycleSpinBox();
}

/**
 * called when number of spherical cycles is changed.
 */
void 
GuiSurfaceDeformationDialog::slotSphereNumberOfCyclesSpinBox(int item)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Number of Cycles"
                << std::endl;
   }
   const int stageIndex = sphereEditStageSpinBox->value() - 1;
   dmf.setSphericalNumberOfCycles(stageIndex, item);
   updateSphereEditCycleSpinBox();
   this->updateLandmarkVectorParameters();
   this->updateMorphingParameters();
   this->updateSmoothingParameters();
}

/**
 * update the edit cycle spin box.  This in turn updates the landmark
 * vector, morphing, and smoothing parameters for the current stage
 * and cycle.
 */
void
GuiSurfaceDeformationDialog::updateSphereEditCycleSpinBox() {
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Update: "
                << "Edit Cycle"
                << std::endl;
   }
   this->sphereEditCycleSpinBox->blockSignals(true);
   const int stageIndex = sphereEditStageSpinBox->value() - 1;
   this->sphereEditCycleSpinBox->setMaximum(
           dmf.getSphericalNumberOfCycles(stageIndex));
   this->sphereEditCycleSpinBox->blockSignals(false);
}

/**
 * called when sphere edit cycle is changed.
 */
void 
GuiSurfaceDeformationDialog::slotSphereEditCycleSpinBox(int /*value*/)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Deformation Dialog Read: "
                << "Sphere Edit Cycle"
                << std::endl;
   }
   this->updateLandmarkVectorParameters();
   this->updateMorphingParameters();
   this->updateSmoothingParameters();
}
      

//----------------------------------------------------------------------------------------------

/**
 * Constructor.
 */
DeformationDataFiles::DeformationDataFiles()
{
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
      borderFileSelected[i] = -1;
   }
   closedTopoFileSelected     = -1;
   cutTopoFileSelected        = -1;
   fiducialCoordFileSelected  = -1;
   flatCoordFileSelected      = -1;
   sphericalCoordFileSelected = -1;
}

/**
 * Destructor.
 */
DeformationDataFiles::~DeformationDataFiles()
{
}

/**
 * Load data file info from a spec file.
 */
void
DeformationDataFiles::loadSpecFile(const SpecFile& sf, const bool flatDeformFlag)
{
   //
   // landmark border files
   //
   borderFileNames.clear();
   borderFileTypes.clear();
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_STAGES; i++) {
      borderFileSelected[i] = -1;
   }
   for (int i = 0; i < sf.sphericalBorderFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.sphericalBorderFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_SPHERICAL);
      if (flatDeformFlag == false) {
         for (int j = 0; j < DeformationMapFile::MAX_SPHERICAL_STAGES; j++) {
            if (borderFileSelected[j] < 0) {
               borderFileSelected[j] = static_cast<int>(borderFileNames.size()) - 1;
            }
         }
      }
   }
   for (int i = 0; i < sf.flatBorderFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.flatBorderFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_FLAT);
      if (flatDeformFlag) {
         for (int j = 0; j < DeformationMapFile::MAX_SPHERICAL_STAGES; j++) {
            if (borderFileSelected[j] < 0) {
               borderFileSelected[j] = static_cast<int>(borderFileNames.size()) - 1;
            }
         }
      }
   }
   for (int i = 0; i < sf.lobarFlatBorderFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.lobarFlatBorderFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_FLAT_LOBAR);
      if (flatDeformFlag) {
         for (int j = 0; j < DeformationMapFile::MAX_SPHERICAL_STAGES; j++) {
            if (borderFileSelected[j] < 0) {
               borderFileSelected[j] = static_cast<int>(borderFileNames.size()) - 1;
            }
         }
      }
   }
   for (int i = 0; i < sf.borderProjectionFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.borderProjectionFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_PROJECTION);
   }
   if (borderFileNames.size() > 0) {
      for (int j = 0; j < DeformationMapFile::MAX_SPHERICAL_STAGES; j++) {
         if (borderFileSelected[j] < 0) {
            borderFileSelected[j] = 0;
         }
      }
   }
   
   //
   // closed topo file
   //
   closedTopoFileNames.clear();
   closedTopoFileTypes.clear();
   for (int i = 0; i < sf.closedTopoFile.getNumberOfFiles(); i++) {
      closedTopoFileNames.push_back(sf.closedTopoFile.files[i].filename);
      closedTopoFileTypes.push_back(DATA_FILE_TOPO_CLOSED);
   }
   for (int i = 0; i < sf.cutTopoFile.getNumberOfFiles(); i++) {
      closedTopoFileNames.push_back(sf.cutTopoFile.files[i].filename);
      closedTopoFileTypes.push_back(DATA_FILE_TOPO_CUT);
   }
   for (int i = 0; i < sf.lobarCutTopoFile.getNumberOfFiles(); i++) {
      closedTopoFileNames.push_back(sf.lobarCutTopoFile.files[i].filename);
      closedTopoFileTypes.push_back(DATA_FILE_TOPO_CUT_LOBAR);
   }
   if (closedTopoFileNames.size() > 0) {
      closedTopoFileSelected = 0;
   }
   else {
      closedTopoFileSelected = -1;
   }
   
   //
   // cut topo file
   //
   cutTopoFileNames.clear();
   cutTopoFileTypes.clear();
   cutTopoFileSelected = -1;
   for (int i = 0; i < sf.closedTopoFile.getNumberOfFiles(); i++) {
      cutTopoFileNames.push_back(sf.closedTopoFile.files[i].filename);
      cutTopoFileTypes.push_back(DATA_FILE_TOPO_CLOSED);
   }
   for (int i = 0; i < sf.cutTopoFile.getNumberOfFiles(); i++) {
      cutTopoFileNames.push_back(sf.cutTopoFile.files[i].filename);
      cutTopoFileTypes.push_back(DATA_FILE_TOPO_CUT);
      if (cutTopoFileSelected < 0) {
         cutTopoFileSelected = static_cast<int>(cutTopoFileNames.size()) - 1;
      }
   }
   for (int i = 0; i < sf.lobarCutTopoFile.getNumberOfFiles(); i++) {
      cutTopoFileNames.push_back(sf.lobarCutTopoFile.files[i].filename);
      cutTopoFileTypes.push_back(DATA_FILE_TOPO_CUT_LOBAR);
      if (cutTopoFileSelected < 0) {
         cutTopoFileSelected = static_cast<int>(cutTopoFileNames.size()) - 1;
      }
   }
   if (cutTopoFileNames.size() > 0) {
      if (cutTopoFileSelected < 0) {
         cutTopoFileSelected = 0;
      }
   }
   
   // 
   // fiducial coord file
   //
   fiducialCoordFileNames.clear();
   fiducialCoordFileTypes.clear();
   for (int i = 0; i < sf.fiducialCoordFile.getNumberOfFiles(); i++) {
      fiducialCoordFileNames.push_back(sf.fiducialCoordFile.files[i].filename);
      fiducialCoordFileTypes.push_back(DATA_FILE_COORD_FIDUCIAL);
   }
   if (fiducialCoordFileNames.size() > 0) {
      fiducialCoordFileSelected = 0;
   }
   else {
      fiducialCoordFileSelected = -1;
   }
   
   //
   // flat coord file
   //
   flatCoordFileNames.clear();
   flatCoordFileTypes.clear();
   for (int i = 0; i < sf.flatCoordFile.getNumberOfFiles(); i++) {
      flatCoordFileNames.push_back(sf.flatCoordFile.files[i].filename);
      flatCoordFileTypes.push_back(DATA_FILE_COORD_FLAT);
   }
   for (int i = 0; i < sf.lobarFlatCoordFile.getNumberOfFiles(); i++) {
      flatCoordFileNames.push_back(sf.lobarFlatCoordFile.files[i].filename);
      flatCoordFileTypes.push_back(DATA_FILE_COORD_FLAT_LOBAR);
   }
   if (flatCoordFileNames.size() > 0) {
      flatCoordFileSelected = 0;
   }
   else {
      flatCoordFileSelected = -1;
   }
   
   //
   // spherical coord file
   //
   sphericalCoordFileNames.clear();
   sphericalCoordFileTypes.clear();
   for (int i = 0; i < sf.sphericalCoordFile.getNumberOfFiles(); i++) {
      sphericalCoordFileNames.push_back(sf.sphericalCoordFile.files[i].filename);
      sphericalCoordFileTypes.push_back(DATA_FILE_COORD_SPHERICAL);
   }
   if (sphericalCoordFileNames.size() > 0) {
      sphericalCoordFileSelected = 0;
   }
   else {
      sphericalCoordFileSelected = -1;
   }
}

