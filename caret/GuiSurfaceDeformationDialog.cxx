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
#include <QSpinBox>
#include <QTabWidget>

#include "GuiBrainModelOpenGL.h"
#include "BrainModelSurfaceDeformationFlat.h"
#include "BrainModelSurfaceDeformationSpherical.h"
#include "BrainSet.h"
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
#include "global_variables.h"

static const QString continuationAndNewLine = " \\\n";

/**
 * constructor
 */
GuiSurfaceDeformationDialog::GuiSurfaceDeformationDialog(
                              QWidget* parent,
                              const DeformationMapFile::DEFORMATION_TYPE deformationTypeIn)
   : WuQDialog(parent)
{
   setModal(true);
   deformationType = deformationTypeIn;
   
   switch (deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         setWindowTitle("Flat Surface Deformation");
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         setWindowTitle("Spherical Surface Deformation");
         break;
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
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   
   QPushButton* batchButton = NULL;
/*
   if (deformationType == DeformationMapFile::DEFORMATION_TYPE_SPHERE) {
      batchButton = new QPushButton("Batch...", this);
      buttonsLayout->addWidget(batchButton);
      QObject::connect(batchButton, SIGNAL(clicked()),
                       this, SLOT(slotBatchButton()));
   }
*/

   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   
   QtUtilities::makeButtonsSameSize(okButton, batchButton, cancelButton);
   
   //
   // Load the parameters into the dialog
   //
   loadParametersIntoDialog();
   
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
 * Called when batch button is pressed.
 */
void 
GuiSurfaceDeformationDialog::slotBatchButton()
{
   std::vector<QString> errorMessages;
   loadDeformationMapFile(errorMessages);
   
   if (errorMessages.empty()) {
      
      std::vector<QString> borderTypeName;
      borderTypeName.push_back("UNKNOWN");
      borderTypeName.push_back("FLAT");
      borderTypeName.push_back("FLAT_LOBAR");
      borderTypeName.push_back("SPHERICAL");
      borderTypeName.push_back("PROJECTION");
      
      //
      // Create the command
      //
      std::ostringstream commandStr;
      commandStr
         << theMainWindow->getBrainSet()->getCaretHomeDirectory().toAscii().constData() << "/bin/caret_sphere_deform";
         
      //addToCommand(commandStr, "-indiv-dir", dmf.getSourceDirectory());
      addToCommand(commandStr, "-indiv-spec", dmf.getSourceSpecFileName());
      QString borderName;
      DeformationMapFile::BORDER_FILE_TYPE bft;
      dmf.getSourceBorderFileName(borderName, bft);
      addToCommand(commandStr, "-indiv-border", borderTypeName[bft], borderName);
      addToCommand(commandStr, "-indiv-closed", dmf.getSourceClosedTopoFileName());
      addToCommand(commandStr, "-indiv-cut", dmf.getSourceCutTopoFileName());
      addToCommand(commandStr, "-indiv-fiducial", dmf.getSourceFiducialCoordFileName());
      addToCommand(commandStr, "-indiv-sphere", dmf.getSourceSphericalCoordFileName());
      addToCommand(commandStr, "-indiv-flat", dmf.getSourceFlatCoordFileName());
      
      //addToCommand(commandStr, "-atlas-dir", dmf.getTargetDirectory());
      addToCommand(commandStr, "-atlas-spec", dmf.getTargetSpecFileName());
      dmf.getTargetBorderFileName(borderName, bft);
      addToCommand(commandStr, "-atlas-border", borderTypeName[bft], borderName);
      addToCommand(commandStr, "-atlas-closed", dmf.getTargetClosedTopoFileName());
      addToCommand(commandStr, "-atlas-cut", dmf.getTargetCutTopoFileName());
      addToCommand(commandStr, "-atlas-fiducial", dmf.getTargetFiducialCoordFileName());
      addToCommand(commandStr, "-atlas-sphere", dmf.getTargetSphericalCoordFileName());
      addToCommand(commandStr, "-atlas-flat", dmf.getTargetFlatCoordFileName());
      
      addToCommand(commandStr, "-sphere-res", StringUtilities::fromNumber(dmf.getSphereResolution()));

      const int numCycles = dmf.getSphericalNumberOfCycles();
      addToCommand(commandStr, "-cycles", StringUtilities::fromNumber(numCycles));
      for (int i = 0; i < numCycles; i++) {
         float strength;
         int cycles, iterations, neighborIterations, finalIterations;
         dmf.getSmoothingParameters(i, strength,
                                    cycles, iterations, neighborIterations, finalIterations);
         commandStr << continuationAndNewLine.toAscii().constData()
                    << "   -smooth "
                    << i << " "
                    << strength << " "
                    << cycles << " "
                    << iterations << " "
                    << neighborIterations << " "
                    << finalIterations;
         
         float linearForce, angularForce, stepSize, landmarkStepSize;
         int smoothIterations;
         dmf.getMorphingParameters(i, cycles, linearForce, angularForce, stepSize,
                                   landmarkStepSize, iterations, smoothIterations);
         commandStr << continuationAndNewLine.toAscii().constData()
                    << "   -morph "
                    << i << " "
                    << cycles << " "
                    << linearForce << " "
                    << angularForce << " "
                    << stepSize << " "
                    << landmarkStepSize << " "
                    << iterations << " "
                    << smoothIterations;
      }
      
      bool ratioOn = false;
      float ratio;
      dmf.getSphereFiducialRatio(ratioOn, ratio);
      if (ratioOn) {
         commandStr << continuationAndNewLine.toAscii().constData()
                    << "   -sphere-dist-correct " << ratio;
      }
      
      DeformationMapFile::BORDER_RESAMPLING_TYPE bst;
      float resampValue;
      QString resampStr[3] = { "NONE", "FILE", "VALUE" };
      dmf.getBorderResampling(bst, resampValue);
      if (bst == DeformationMapFile::BORDER_RESAMPLING_VALUE) {
         resampStr[2] = StringUtilities::fromNumber(resampValue);
      }
      addToCommand(commandStr, "-resample", resampStr[bst]);
      
      QString metricStr[2] = { "NODE", "AVG" };
      addToCommand(commandStr, "-metric-deform", metricStr[dmf.getMetricDeformationType()]);
      
      if (dmf.getDeformBothWays()) {
         commandStr << continuationAndNewLine.toAscii().constData() << "   -both";
      }
      
      addToCommand(commandStr, "-prefix", dmf.getDeformedFileNamePrefix());
      addToCommand(commandStr, "-column-prefix", dmf.getDeformedColumnNamePrefix());
      
      if (dmf.getDeleteIntermediateFiles()) {
         commandStr << continuationAndNewLine.toAscii().constData() << "   -del-temp";
      }  
      
      if (deformIndivFiducialCoordCheckBox->isChecked()) {
         commandStr << continuationAndNewLine.toAscii().constData() << "   -dcfid";
      }
      if (deformIndivInflatedCoordCheckBox->isChecked()) {
         commandStr << continuationAndNewLine.toAscii().constData() << "   -dcinf";
      }
      if (deformIndivVeryInflatedCoordCheckBox->isChecked()) {
         commandStr << continuationAndNewLine.toAscii().constData() << "   -dcvi";
      }
      if (deformIndivSphericalCoordCheckBox->isChecked()) {
         commandStr << continuationAndNewLine.toAscii().constData() << "   -dcsph";
      }
      if (deformIndivFlatCoordCheckBox->isChecked()) {
         commandStr << continuationAndNewLine.toAscii().constData() << "   -dcflat";
      }
      
      commandStr << "\n";
      
/*
      addToCommand(commandStr, "", dmf.());
      addToCommand(commandStr, "", dmf.());
      addToCommand(commandStr, "", dmf.());
      addToCommand(commandStr, "", dmf.());
*/
      QString commandFileName("spherical_deform");
         
      GuiBatchCommandDialog bcd(this,
                                QDir::currentPath(),
                                commandStr.str().c_str(),
                                commandFileName);
      if (bcd.exec() == QDialog::Rejected) {
         return;
      }

      //
      // Allow the dialog to close.
      //
      reject();
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
   dialogTabWidget->addTab(atlasWidgetTop, "Atlas");
   
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
   QPushButton* editBordersPushButton = new QPushButton("Edit...");
   editBordersPushButton->setAutoDefault(false);
   QObject::connect(editBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotBordersEditAtlas()));
   
   //
   // Create the pushbuttons and line edits
   //
   atlasSpecLineEdit           = createFileEntryLine("Spec File...", FILE_TYPE_SPEC, 
                                                     atlasWidgetLayout, atlasButtonGroup);
   atlasBorderLineEdit         = createFileEntryLine("Border File...", FILE_TYPE_BORDER, 
                                                     atlasWidgetLayout, atlasButtonGroup,
                                                     editBordersPushButton);
   atlasClosedTopoLineEdit     = createFileEntryLine("Closed Topo File...", FILE_TYPE_TOPO_CLOSED, 
                                                     atlasWidgetLayout, atlasButtonGroup);
   atlasCutTopoLineEdit        = createFileEntryLine("Cut Topo File...", FILE_TYPE_TOPO_CUT, 
                                                     atlasWidgetLayout, atlasButtonGroup);
   atlasFiducialCoordLineEdit  = createFileEntryLine("Fiducial Coord File...", FILE_TYPE_COORD_FIDUCIAL, 
                                                     atlasWidgetLayout, atlasButtonGroup);
   atlasSphericalCoordLineEdit = createFileEntryLine("Spherical Coord File...", FILE_TYPE_COORD_SPHERICAL, 
                                                     atlasWidgetLayout, atlasButtonGroup);
   atlasFlatCoordLineEdit      = createFileEntryLine("Flat Coord...", FILE_TYPE_COORD_FLAT, 
                                                     atlasWidgetLayout, atlasButtonGroup);
   
   //atlasWidgetTop->setFixedSize(atlasWidgetTop->sizeHint());
   const int numRows = atlasWidgetLayout->rowCount();
   atlasWidgetLayout->addWidget(new QWidget, numRows, 0);
   atlasWidgetLayout->setRowStretch(numRows, 1000);
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
                                                     individualGridLayout, indivButtonGroup);
   indivBorderLineEdit         = createFileEntryLine("Border File...", FILE_TYPE_BORDER, 
                                                     individualGridLayout, indivButtonGroup,
                                                     editBordersPushButton);
   indivClosedTopoLineEdit     = createFileEntryLine("Closed Topo File...", FILE_TYPE_TOPO_CLOSED, 
                                                     individualGridLayout, indivButtonGroup);
   indivCutTopoLineEdit        = createFileEntryLine("Cut Topo File...", FILE_TYPE_TOPO_CUT, 
                                                     individualGridLayout, indivButtonGroup);
   indivFiducialCoordLineEdit  = createFileEntryLine("Fiducial Coord File...", FILE_TYPE_COORD_FIDUCIAL, 
                                                     individualGridLayout, indivButtonGroup);
   indivSphericalCoordLineEdit = createFileEntryLine("Spherical Coord File...", FILE_TYPE_COORD_SPHERICAL, 
                                                     individualGridLayout, indivButtonGroup);
   indivFlatCoordLineEdit      = createFileEntryLine("Flat Coord...", FILE_TYPE_COORD_FLAT, 
                                                     individualGridLayout, indivButtonGroup);
   
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
   switch (deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         indivCoordDeformGroupBox->setHidden(true);
         indivCoordDeformGroupBox->setEnabled(false);
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
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
   if (indivDeformationFiles.borderFileSelected < 0) {
      QMessageBox::critical(this, "ERROR", "There are no individual border files.");
      return;
   }
   editBorderFile(indivSpecFileName,
                  indivDeformationFiles.borderFileNames[indivDeformationFiles.borderFileSelected],
                  indivDeformationFiles.borderFileTypes[indivDeformationFiles.borderFileSelected]);
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
   GuiBorderAttributesDialog::BORDER_FILE_TYPE borderFileType;
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
GuiSurfaceDeformationDialog::slotBordersEditAtlas()
{
   if (atlasDeformationFiles.borderFileSelected < 0) {
      QMessageBox::critical(this, "ERROR", "There are no atlas border files.");
      return;
   }
   editBorderFile(atlasSpecFileName,
                  atlasDeformationFiles.borderFileNames[atlasDeformationFiles.borderFileSelected],
                  atlasDeformationFiles.borderFileTypes[atlasDeformationFiles.borderFileSelected]);
}

/**
 * Create the parameters tab.
 */
void
GuiSurfaceDeformationDialog::createParametersWidget()
{
   QString tabTitle("ERROR");
   QWidget* sphereFlatParmsWidget = NULL;
   switch (deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         //
         // Create the spherical parameters
         //
         tabTitle = "Spherical Parameters";
         sphereFlatParmsWidget = createSphericalParameters();
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
   QPushButton* loadDefMapFilePushButton = new QPushButton("Read Params From Deformation Map File...");
   loadDefMapFilePushButton->setAutoDefault(false);
   loadDefMapFilePushButton->setFixedSize(loadDefMapFilePushButton->sizeHint());
   QObject::connect(loadDefMapFilePushButton, SIGNAL(clicked()),
                    this, SLOT(loadDeformationMapFilePushButton()));
                    
   QGroupBox* setParamsGroupBox = new QGroupBox("Set Parameters");
   QHBoxLayout* setParamsLayout = new QHBoxLayout(setParamsGroupBox);
   setParamsLayout->addWidget(standardParametersPushButton);
   setParamsLayout->addWidget(loadDefMapFilePushButton);

   parametersWidget = new QWidget;
   QVBoxLayout* parametersLayout = new QVBoxLayout(parametersWidget);
   if (sphereFlatParmsWidget != NULL) {
      parametersLayout->addWidget(sphereFlatParmsWidget);
   }
   parametersLayout->addWidget(setParamsGroupBox);
   dialogTabWidget->addTab(parametersWidget, tabTitle);


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
                                             
   //
   // Delete intermediate files check box
   //
   deleteIntermediateFilesCheckBox = new QCheckBox("Delete Intermediate Files");
   
   //
   // label and line edit for deformed file prefix
   //
   QLabel* deformedFileLabel = new QLabel("Deformed File Name Prefix ");
   deformedFilePrefixLineEdit = new QLineEdit;
   QHBoxLayout* deformedFileLayout = new QHBoxLayout;
   deformedFileLayout->addWidget(deformedFileLabel);
   deformedFileLayout->addWidget(deformedFilePrefixLineEdit);
   
   //
   // label and line edit for deformed column name prefix
   //
   QLabel* deformedColumnLabel = new QLabel("Deformed Column Name Prefix ");
   deformedColumnPrefixLineEdit = new QLineEdit;
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
   
   //
   // Coordinate file options
   //
   QGroupBox* coordGroupBox = new QGroupBox("Coordinate Data File Deformation Options");
   QVBoxLayout* coordGroupLayout = new QVBoxLayout(coordGroupBox);
   coordGroupLayout->addWidget(smoothCoordsOneIterationCheckBox);
   
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
   deformationWidgetLayout->addStretch();
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
   
   return metricGroupBox;
}

/**
 *
 */
QWidget*
GuiSurfaceDeformationDialog::createSphericalParameters()
{
   //
   // Sphere resolution combo box and label and HBox
   //
   QLabel* sphereResolutionLabel = new QLabel("Sphere Resolution");
   sphereResolutionComboBox = new QComboBox;
   sphereResolutionComboBox->setFixedSize(sphereResolutionComboBox->sizeHint());
   sphereResolutionComboBox->addItem("   20 Nodes");
   sphereResolutionComboBox->addItem("   74 Nodes");
   sphereResolutionComboBox->addItem("  290 Nodes");
   sphereResolutionComboBox->addItem(" 1154 Nodes");
   sphereResolutionComboBox->addItem(" 4610 Nodes");
   sphereResolutionComboBox->addItem("18434 Nodes");
   sphereResolutionComboBox->addItem("73730 Nodes");
   
   //
   // Number of Cycles combo box and label
   //
   QLabel* numberOfCyclesLabel = new QLabel("Number of Cycles");
   sphereNumberOfCyclesComboBox = new QComboBox;
   sphereNumberOfCyclesComboBox->setFixedSize(sphereNumberOfCyclesComboBox->sizeHint());
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_CYCLES; i++) {
      sphereNumberOfCyclesComboBox->addItem(QString::number(i+1));
   }
   QObject::connect(sphereNumberOfCyclesComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSphereNumberOfCyclesComboBox(int)));
                    
   //
   // Edit Cycles combo box and label
   //
   QLabel* editCycleLabel = new QLabel("Edit Cycle");
   sphereEditCycleComboBox = new QComboBox;
   sphereEditCycleComboBox->setFixedSize(sphereEditCycleComboBox->sizeHint());
   for (int i = 0; i < DeformationMapFile::MAX_SPHERICAL_CYCLES; i++) {
      sphereEditCycleComboBox->addItem(QString::number(i+1));
   }
   QObject::connect(sphereEditCycleComboBox, SIGNAL(highlighted(int)),
                    this, SLOT(slotSphereEditCycleComboBoxOldValue()));
   QObject::connect(sphereEditCycleComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSphereEditCycleComboBoxNewValue()));

   //
   // Fiducial/sphere ratios
   //
   sphereDistortionCorrectionCheckBox = new QCheckBox("Correct For Spherical Distortion\n"
                                                      "Relative to Fiducial");
   sphereDistortionDoubleSpinBox = new QDoubleSpinBox;
   sphereDistortionDoubleSpinBox->setMinimum(0.0);
   sphereDistortionDoubleSpinBox->setMaximum(1.0);
   sphereDistortionDoubleSpinBox->setSingleStep(0.5);
   sphereDistortionDoubleSpinBox->setDecimals(3);
                                            
   //
   // Group for spherical parameters
   //   
   QGroupBox* sphericalParamsGroupBox = new QGroupBox("Spherical Parameters");
   QGridLayout* sphericalParamsGroupLayout = new QGridLayout(sphericalParamsGroupBox);
   sphericalParamsGroupLayout->addWidget(sphereResolutionLabel, 0, 0);
   sphericalParamsGroupLayout->addWidget(sphereResolutionComboBox, 0, 1);
   sphericalParamsGroupLayout->addWidget(numberOfCyclesLabel, 1, 0);
   sphericalParamsGroupLayout->addWidget(sphereNumberOfCyclesComboBox, 1, 1);
   sphericalParamsGroupLayout->addWidget(editCycleLabel, 2, 0);
   sphericalParamsGroupLayout->addWidget(sphereEditCycleComboBox, 2, 1);
   sphericalParamsGroupLayout->addWidget(sphereDistortionCorrectionCheckBox, 3, 0);
   sphericalParamsGroupLayout->addWidget(sphereDistortionDoubleSpinBox, 3, 1);

   //
   //  label and strength spin box
   //
   QLabel* strengthLabel = new QLabel("Strength");
   smoothingStrengthDoubleSpinBox = new QDoubleSpinBox;
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
   smoothingCyclesSpinBox->setMinimum(0);
   smoothingCyclesSpinBox->setMaximum(50000);
   smoothingCyclesSpinBox->setSingleStep(10);
   smoothingCyclesSpinBox->setFixedWidth(120);
   
   //
   //  label and iterations per cycle spin box
   //
   QLabel* iterationsLabel = new QLabel("Iterations/Cycle");
   smoothingIterationsSpinBox = new QSpinBox;
   smoothingIterationsSpinBox->setMinimum(0);
   smoothingIterationsSpinBox->setMaximum(50000);
   smoothingIterationsSpinBox->setSingleStep(5);
   smoothingIterationsSpinBox->setFixedWidth(120);
   
   //
   //  label and smooth neighbors spin box
   //
   QLabel* smoothNeighborsLabel = new QLabel("Smooth Neighbors Every X");
   smoothingNeighborsSpinBox = new QSpinBox;
   smoothingNeighborsSpinBox->setMinimum(0);
   smoothingNeighborsSpinBox->setMaximum(50000);
   smoothingNeighborsSpinBox->setSingleStep(1);
   smoothingNeighborsSpinBox->setFixedWidth(120);
   
   //
   //  label and final smoothing iterations
   //
   QLabel* finalSmoothLabel = new QLabel("Final Smoothing Iterations");
   smoothingFinalSpinBox = new QSpinBox;
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

   //
   //  label and spin box
   //
   QLabel* morphCyclesLabel = new QLabel("Cycles");
   morphingCyclesSpinBox = new QSpinBox;
   morphingCyclesSpinBox->setMinimum(0);
   morphingCyclesSpinBox->setMaximum(5000);
   morphingCyclesSpinBox->setSingleStep(1);
   morphingCyclesSpinBox->setFixedWidth(120);
   
   //
   //  label and spin box
   //
   QLabel* morphingLinearForceLabel = new QLabel("Linear Force");
   morphingLinearForceDoubleSpinBox = new QDoubleSpinBox;
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
   morphingIterationsSpinBox->setMinimum(0);
   morphingIterationsSpinBox->setMaximum(5000);
   morphingIterationsSpinBox->setSingleStep(10);
   morphingIterationsSpinBox->setFixedWidth(120);
   
   //
   //  label and spin box
   //
   QLabel* morphingSmoothLabel = new QLabel("Smooth Iterations");
   morphingSmoothIterationsSpinBox = new QSpinBox;
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
   morphGroupLayout->setRowStretch(7, 100);

   QVBoxLayout* leftColumnLayout = new QVBoxLayout;
   leftColumnLayout->addWidget(sphericalParamsGroupBox);
   leftColumnLayout->addWidget(smoothingParamsGroupBox);
   
   QWidget* sphereParamsPageWidget = new QWidget;
   QHBoxLayout* sphereParamsPageLayout = new QHBoxLayout(sphereParamsPageWidget);
   sphereParamsPageLayout->addLayout(leftColumnLayout);
   sphereParamsPageLayout->addWidget(morphGroupBox);
   
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
    
   //
   // label and line edit
   //
   QLabel* flatBetaLabel = new QLabel("Beta");
   flatBetaLineEdit = new QLineEdit;
   flatBetaLineEdit->setFixedWidth(150);
    
   //
   // label and line edit
   //
   QLabel* flatVarMultLabel = new QLabel("Variance Multiplier");
   flatVarMultDoubleSpinBox = new QLineEdit;
   flatVarMultDoubleSpinBox->setFixedWidth(150);
    
   //
   // label and line edit
   //
   QLabel* flatIterationsLabel = new QLabel("Iterations");
   flatIterationsSpinBox = new QSpinBox;
   flatIterationsSpinBox->setMinimum(0);
   flatIterationsSpinBox->setMaximum(50000);
   flatIterationsSpinBox->setSingleStep(5);
   flatIterationsSpinBox->setFixedWidth(150);
   
   QGroupBox* flatParamsGroupBox = new QGroupBox("Parameters");
   QGridLayout* flatParamsLayout = new QGridLayout(flatParamsGroupBox);
   flatParamsLayout->addWidget(flatSubSamplingLabel, 0, 0);
   flatParamsLayout->addWidget(flatSubSamplingTilesSpinBox, 0, 1);
   flatParamsLayout->addWidget(flatBetaLabel, 1, 0);
   flatParamsLayout->addWidget(flatBetaLineEdit, 1, 1);
   flatParamsLayout->addWidget(flatVarMultLabel, 2, 0);
   flatParamsLayout->addWidget(flatVarMultDoubleSpinBox, 2, 1);
   flatParamsLayout->addWidget(flatIterationsLabel, 3, 0);
   flatParamsLayout->addWidget(flatIterationsSpinBox, 3, 1);
   
   return flatParamsGroupBox;
}

/**
 * Create a file entry line (pushbutton and line edit).
 */
QLineEdit*
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

   return le;
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
      case FILE_TYPE_BORDER:
         displayFileSelection("Select Border File",
                              atlasDeformationFiles.borderFileNames,
                              atlasDeformationFiles.borderFileTypes,
                              atlasDeformationFiles.borderFileSelected);
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
      case FILE_TYPE_BORDER:
         displayFileSelection("Select Border File",
                              indivDeformationFiles.borderFileNames,
                              indivDeformationFiles.borderFileTypes,
                              indivDeformationFiles.borderFileSelected);
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
      switch(deformationType) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            flatDeformFlag = true;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
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
   
   readParametersFromDialog();

   //
   // Set the source border file type
   //
   DeformationMapFile::BORDER_FILE_TYPE bft = DeformationMapFile::BORDER_FILE_UNKNOWN;
   if (indivDeformationFiles.borderFileSelected >= 0) {
      switch(indivDeformationFiles.borderFileTypes[indivDeformationFiles.borderFileSelected]) {
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
   switch(deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         if (dmf.getSourceFlatCoordFileName().isEmpty()) {
            errorMessages.push_back("Required individual flat coord file is missing.");
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         if (dmf.getSourceSphericalCoordFileName().isEmpty()) {
            errorMessages.push_back("Required individual source spherical coord file is missing.");
         }
         break;
   }
   
   //
   // Set the target border file type
   // 
   bft = DeformationMapFile::BORDER_FILE_UNKNOWN;
   if (atlasDeformationFiles.borderFileSelected >= 0) {
      switch(atlasDeformationFiles.borderFileTypes[atlasDeformationFiles.borderFileSelected]) {
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
   const QString targetBorderName(atlasBorderLineEdit->text());
   dmf.setTargetBorderFileName(targetBorderName, bft);
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
   if (targetBorderName.isEmpty()) {
      errorMessages.push_back("Required atlas border file is missing.");
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
   switch(deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         if (dmf.getTargetFlatCoordFileName().isEmpty()) {
            errorMessages.push_back("Required atlas flat coord file is missing.");
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         if (dmf.getTargetSphericalCoordFileName().isEmpty()) {
            errorMessages.push_back("Required individual atlas spherical coord file is missing.");
         }
         break;
   }
   
   //
   // Set flat or sphere
   //
   dmf.setFlatOrSphereSelection(deformationType);
   
   dmf.setSmoothDeformedSurfacesFlag(smoothCoordsOneIterationCheckBox->isChecked());
}

/**
 * Called when user press OK or Cancel buttons
 */
void 
GuiSurfaceDeformationDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      
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
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      DisplaySettingsSurface* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
      
      BrainModelSurfaceDeformation* bma = NULL;
      try {
         switch(deformationType) {
            case DeformationMapFile::DEFORMATION_TYPE_FLAT:
               bma = new BrainModelSurfaceDeformationFlat(theMainWindow->getBrainSet(), &dmf);
               break;
            case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
               dss->setDrawMode(DisplaySettingsSurface::DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL);
               BrainModelSurfaceDeformationSpherical* bmsds = 
                              new BrainModelSurfaceDeformationSpherical(theMainWindow->getBrainSet(), &dmf);
               //
               // Enable coord file deformation for sphere
               //
               bmsds->setDeformSourceFiducialCoordFiles(deformIndivFiducialCoordCheckBox->isChecked());
               bmsds->setDeformSourceInflatedCoordFiles(deformIndivInflatedCoordCheckBox->isChecked());
               bmsds->setDeformSourceVeryInflatedCoordFiles(deformIndivVeryInflatedCoordCheckBox->isChecked());
               bmsds->setDeformSourceSphericalCoordFiles(deformIndivSphericalCoordCheckBox->isChecked());
               bmsds->setDeformSourceFlatCoordFiles(deformIndivFlatCoordCheckBox->isChecked());
               bma = bmsds;
               break;
         }
         
         //
         // Close the Dialog
         //
         QDialog::done(r);
         
         if (bma != NULL) {
            bma->execute();
         }
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Deformation Error", e.whatQString());
         return;
      }

      //
      // Check for deformation errors
      //
      QString msg("The deformation has completed.");
      const QString s2tMsg(bma->getSourceToTargetDeformDataFileErrors());
      if (s2tMsg.isEmpty() == false) {
         msg.append("\nErrors were detected deforming the following data files from\n "
                    "the individual to the atlas:\n");
         msg.append(s2tMsg);
      }
      const QString t2sMsg(bma->getTargetToSourceDeformDataFileErrors());
      if (t2sMsg.isEmpty() == false) {
         msg.append("\nErrors were detected deforming the following data files from\n "
                    "the atlas to the individual:\n");
         msg.append(t2sMsg);
      }
      //
      // Let the user know that the deformation has completed
      //   
      QApplication::restoreOverrideCursor();   
      QMessageBox::information(this, "Deformation Completed", msg);
                                 
      //
      // Free memory
      //
      if (bma != NULL) {
         delete bma;
      }
   }  // QDialog::Accepted
   else {
      QDialog::done(r);
   }
}

/**
 * Display the atlas data files.
 */
void
GuiSurfaceDeformationDialog::displayAtlasFiles()
{
   atlasSpecLineEdit->setText(atlasSpecFileName);
   
   if (atlasDeformationFiles.borderFileNames.size() > 0) {
      atlasBorderLineEdit->setText(
         atlasDeformationFiles.borderFileNames[atlasDeformationFiles.borderFileSelected]);
   }
   else {
      atlasBorderLineEdit->setText("");
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
         indivDeformationFiles.borderFileNames[indivDeformationFiles.borderFileSelected]);
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
   switch (deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
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
   openDefMap.setDirectory(QDir::currentPath());
   openDefMap.setAcceptMode(WuQFileDialog::AcceptOpen);
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
 * Read the parameters from the dialog into the deformation map file.
 */
void
GuiSurfaceDeformationDialog::readParametersFromDialog()
{
   switch(deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         {
            dmf.setFlatParameters(flatSubSamplingTilesSpinBox->value(),
                                  flatBetaLineEdit->text().toFloat(),
                                  flatVarMultDoubleSpinBox->text().toFloat(),
                                  flatIterationsSpinBox->value());
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         {
            //
            // Get the sphere resolution
            //
            switch(sphereResolutionComboBox->currentIndex()) {
               case 0:
                  dmf.setSphereResolution(20);
                  break;
               case 1:
                  dmf.setSphereResolution(74);
                  break;
               case 2:
                  dmf.setSphereResolution(290);
                  break;
               case 3:
                  dmf.setSphereResolution(1154);
                  break;
               case 4:
                  dmf.setSphereResolution(4610);
                  break;
               case 5:
                  dmf.setSphereResolution(18434);
                  break;
               case 6:
                  dmf.setSphereResolution(73730);
                  break;
               default:
                  dmf.setSphereResolution(4610);
                  break;
            }
            
            //
            // Set the number of cycles
            //
            dmf.setSphericalNumberOfCycles(sphereNumberOfCyclesComboBox->currentIndex() + 1);
            
            //
            // Get the current cycle being edited
            //
            const int editCycleNumber = sphereEditCycleComboBox->currentIndex();
            
            //
            // Get the smoothing parameters
            //
            dmf.setSmoothingParameters(editCycleNumber,
                                       smoothingStrengthDoubleSpinBox->value(),
                                       smoothingCyclesSpinBox->value(),
                                       smoothingIterationsSpinBox->value(),
                                       smoothingNeighborsSpinBox->value(),
                                       smoothingFinalSpinBox->value());
                                       
            //
            // Get the morphing parameters
            //
            dmf.setMorphingParameters(editCycleNumber,
                                      morphingCyclesSpinBox->value(),
                                      morphingLinearForceDoubleSpinBox->value(),
                                      morphingAngularForceDoubleSpinBox->value(),
                                      morphingStepSizeDoubleSpinBox->value(),
                                      morphingLandmarkStepSizeDoubleSpinBox->value(),
                                      morphingIterationsSpinBox->value(),
                                      morphingSmoothIterationsSpinBox->value());
                                      
            //
            // Get the fiducial sphere ratios
            //
            dmf.setSphereFiducialRatio(sphereDistortionCorrectionCheckBox->isChecked(),
                                       sphereDistortionDoubleSpinBox->value());
         }
         break;
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
}

/**
 * Load the parameters from a deformation map file.
 */
void
GuiSurfaceDeformationDialog::loadParametersIntoDialog()
{
   switch(deformationType) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         {
            int subSamplingTiles, numIters;
            float beta, varMult;
            dmf.getFlatParameters(subSamplingTiles, beta, varMult, numIters);
            flatSubSamplingTilesSpinBox->setValue(subSamplingTiles);
            flatBetaLineEdit->setText(QString::number(beta, 'f', 6));
            flatVarMultDoubleSpinBox->setText(QString::number(varMult, 'f', 6));
            flatIterationsSpinBox->setValue(numIters);
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         {
            //
            // Load the sphere resolution
            //
            const int numNodesInSphere = dmf.getSphereResolution();
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
            
            //
            // Get the spherical number of cycles
            //
            sphereNumberOfCyclesComboBox->setCurrentIndex(dmf.getSphericalNumberOfCycles() - 1);

            //
            // Update edit cycle combo box
            //
            if (sphereEditCycleComboBox->currentIndex() > dmf.getSphericalNumberOfCycles()) {
               sphereEditCycleComboBox->setCurrentIndex(dmf.getSphericalNumberOfCycles());
            }
            
            //
            // Get current cycle being edited
            //
            const int currentCycle = sphereEditCycleComboBox->currentIndex();
            
            //
            // Load smoothing parameters
            //
            float strength;
            int cycles, iterations, neighbors, finals;
            dmf.getSmoothingParameters(currentCycle,
                                       strength, cycles, iterations, neighbors, finals);
            smoothingStrengthDoubleSpinBox->setValue(strength);
            smoothingCyclesSpinBox->setValue(cycles);
            smoothingIterationsSpinBox->setValue(iterations);
            smoothingNeighborsSpinBox->setValue(neighbors);
            smoothingFinalSpinBox->setValue(finals);
            
            //
            // load morphing parameters
            //
            float linearForce, angularForce, stepSize, landmarkStepSize;
            int smoothIterations;
            dmf.getMorphingParameters(currentCycle,
                                      cycles, linearForce, angularForce, stepSize,
                                      landmarkStepSize, iterations, smoothIterations);
            morphingCyclesSpinBox->setValue(cycles);
            morphingLinearForceDoubleSpinBox->setValue(linearForce);
            morphingAngularForceDoubleSpinBox->setValue(angularForce);
            morphingStepSizeDoubleSpinBox->setValue(stepSize);
            morphingLandmarkStepSizeDoubleSpinBox->setValue(landmarkStepSize);
            morphingIterationsSpinBox->setValue(iterations);
            morphingSmoothIterationsSpinBox->setValue(smoothIterations);
            
            //
            // fiducial sphere ratios
            //
            float ratio;
            bool ratioValid;
            dmf.getSphereFiducialRatio(ratioValid, ratio);
            sphereDistortionCorrectionCheckBox->setChecked(ratioValid);
            sphereDistortionDoubleSpinBox->setValue(ratio);
         }
         break;
   }
   
   //
   // set border resampling
   //
   DeformationMapFile::BORDER_RESAMPLING_TYPE borderResample;
   float resampleValue;
   dmf.getBorderResampling(borderResample, resampleValue);
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
   borderResamplingDoubleSpinBox->setValue(resampleValue);
   
   //
   // set metric deformation
   //
   switch(dmf.getMetricDeformationType()) {
      case DeformationMapFile::METRIC_DEFORM_NEAREST_NODE:
         metricNearestNodeRadioButton->setChecked(true);
         break;
      case DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES:
         metricAverageTileNodesRadioButton->setChecked(true);
         break;
   }
   
   //
   // set deformation stuff
   //
   deformedFilePrefixLineEdit->setText(dmf.getDeformedFileNamePrefix());
   deformedColumnPrefixLineEdit->setText(dmf.getDeformedColumnNamePrefix());
   deformBothWaysCheckBox->setChecked(dmf.getDeformBothWays());
   deleteIntermediateFilesCheckBox->setChecked(dmf.getDeleteIntermediateFiles());
   
   smoothCoordsOneIterationCheckBox->setChecked(dmf.getSmoothDeformedSurfacesFlag());
}

/// called when number of spherical cycles is changed
void 
GuiSurfaceDeformationDialog::slotSphereNumberOfCyclesComboBox(int item)
{
   readParametersFromDialog();
   
   if (item < sphereEditCycleComboBox->currentIndex()) {
      sphereEditCycleComboBox->setCurrentIndex(item);
   }
   loadParametersIntoDialog();
}

/// called before sphere edit cycle is changed
void 
GuiSurfaceDeformationDialog::slotSphereEditCycleComboBoxOldValue()
{
   readParametersFromDialog();
}
      
/// called when sphere edit cycle is changed
void 
GuiSurfaceDeformationDialog::slotSphereEditCycleComboBoxNewValue()
{
   loadParametersIntoDialog();
}
      

//----------------------------------------------------------------------------------------------

/**
 * Constructor.
 */
DeformationDataFiles::DeformationDataFiles()
{
   borderFileSelected         = -1;
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
   borderFileSelected = -1;
   for (int i = 0; i < sf.sphericalBorderFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.sphericalBorderFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_SPHERICAL);
      if (flatDeformFlag == false) {
         if (borderFileSelected < 0) {
            borderFileSelected = static_cast<int>(borderFileNames.size()) - 1;
         }
      }
   }
   for (int i = 0; i < sf.flatBorderFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.flatBorderFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_FLAT);
      if (flatDeformFlag) {
         if (borderFileSelected < 0) {
            borderFileSelected = static_cast<int>(borderFileNames.size()) - 1;
         }
      }
   }
   for (int i = 0; i < sf.lobarFlatBorderFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.lobarFlatBorderFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_FLAT_LOBAR);
      if (flatDeformFlag) {
         if (borderFileSelected < 0) {
            borderFileSelected = static_cast<int>(borderFileNames.size()) - 1;
         }
      }
   }
   for (int i = 0; i < sf.borderProjectionFile.getNumberOfFiles(); i++) {
      borderFileNames.push_back(sf.borderProjectionFile.files[i].filename);
      borderFileTypes.push_back(DATA_FILE_BORDER_PROJECTION);
   }
   if (borderFileNames.size() > 0) {
      if (borderFileSelected < 0) {
         borderFileSelected = 0;
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

