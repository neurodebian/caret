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
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "BrainModelSurfaceDeformDataFile.h"
#include "BrainSet.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiApplyDeformationMapDialog.h"
#include "GuiMainWindow.h"
#include "DeformationMapFile.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "WuQFileDialog.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiApplyDeformationMapDialog::GuiApplyDeformationMapDialog(QWidget* parent, Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   //
   // initialize some variables
   //
   sourceBrainSet = NULL;
   targetBrainSet = NULL;
   
   const int lineEditWidth = 400;
   
   setWindowTitle("Apply Deformation Map");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);

   //
   // Create a group widget to hold all deformation map fileitems
   //  
   QGroupBox* defMapGroupBox = new QGroupBox("Deformation Map File");
   dialogLayout->addWidget(defMapGroupBox);
   QGridLayout* defMapGridLayout = new QGridLayout(defMapGroupBox);

   //
   // Deformation map push button and line edit
   //
   QPushButton* deformationMapPushButton = new QPushButton("Deformation Map File...");
   defMapGridLayout->addWidget(deformationMapPushButton, 0, 0);
   deformationMapPushButton->setAutoDefault(false);
   QObject::connect(deformationMapPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeformationMapButton()));
   deformationMapFileLineEdit = new QLineEdit;
   defMapGridLayout->addWidget(deformationMapFileLineEdit, 0, 1);
   deformationMapFileLineEdit->setMinimumWidth(lineEditWidth);
   deformationMapFileLineEdit->setReadOnly(true);
   const QString defMapName(theMainWindow->getBrainSet()->getDeformationMapFileName());
   if (defMapName.isEmpty() == false) {
      deformationMapFileLineEdit->setText(defMapName);
   }
         
   //
   // Source directory
   //
   sourceDirectoryPushButton = new QPushButton("Individual Directory...");
   defMapGridLayout->addWidget(sourceDirectoryPushButton, 1, 0);
   sourceDirectoryPushButton->setAutoDefault(false);
   sourceDirectoryPushButton->setEnabled(false);
   QObject::connect(sourceDirectoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSourceDirectoryButton()));
   sourceDirectoryLineEdit = new QLineEdit;
   defMapGridLayout->addWidget(sourceDirectoryLineEdit, 1, 1);
   sourceDirectoryLineEdit->setMinimumWidth(lineEditWidth);
   //sourceDirectoryLineEdit->setReadOnly(true);
   
   //
   // target directory
   //
   targetDirectoryPushButton = new QPushButton("Atlas Directory...");
   defMapGridLayout->addWidget(targetDirectoryPushButton, 2, 0);
   targetDirectoryPushButton->setAutoDefault(false);
   targetDirectoryPushButton->setEnabled(false);
   QObject::connect(targetDirectoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotTargetDirectoryButton()));
   targetDirectoryLineEdit = new QLineEdit;
   defMapGridLayout->addWidget(targetDirectoryLineEdit, 2, 1);
   targetDirectoryLineEdit->setMinimumWidth(lineEditWidth);
   //targetDirectoryLineEdit->setReadOnly(true);
   
   //
   // Create a group widget to hold all data file items
   //  
   QGroupBox* dataFileGroupBox = new QGroupBox("Data File");
   dialogLayout->addWidget(dataFileGroupBox);
   QGridLayout* dataFileGridLayout = new QGridLayout(dataFileGroupBox);

   //
   // File Type label and combo box
   //
   dataFileGridLayout->addWidget(new QLabel("File Type"), 0, 0);
   fileTypeComboBox = new QComboBox;
   dataFileGridLayout->addWidget(fileTypeComboBox, 0, 1);
   QObject::connect(fileTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotFileTypeComboBox(int)));
                    
   //
   // Load the file type combo box
   //
   fileTypeComboBox->addItem("Areal Estimation", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION));
   fileTypeComboBox->addItem("Atlas (Probabilistic)", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS));
   fileTypeComboBox->addItem("Border - Flat", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT));
   fileTypeComboBox->addItem("Border - Spherical", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL));
   fileTypeComboBox->addItem("Border Projection", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION));         
   fileTypeComboBox->addItem("Cell", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_CELL));
   fileTypeComboBox->addItem("Cell Projection", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION));
   fileTypeComboBox->addItem("Coordinate File (3D)",
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE));
   fileTypeComboBox->addItem("Coordinate File (FLAT)",
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT));
   fileTypeComboBox->addItem("Foci", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI));
   fileTypeComboBox->addItem("Foci Projection", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION));
   fileTypeComboBox->addItem("Lat/Long", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON));
   fileTypeComboBox->addItem("Metric", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC));
   fileTypeComboBox->addItem("Paint", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT));
   fileTypeComboBox->addItem("RGB Paint", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT));
   fileTypeComboBox->addItem("Surface Shape", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE));
   fileTypeComboBox->addItem("Topography", 
                                static_cast<int>(BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY));
   
   //
   // Atlas topo file 
   //
   atlasTopoFilePushButton = new QPushButton("Target Closed Topo File...");
   dataFileGridLayout->addWidget(atlasTopoFilePushButton, 1, 0);
   atlasTopoFilePushButton->setAutoDefault(false);
   atlasTopoFilePushButton->setEnabled(false);
   QObject::connect(atlasTopoFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotAtlasTopoFileButton()));
   atlasTopoFileLineEdit = new QLineEdit;
   dataFileGridLayout->addWidget(atlasTopoFileLineEdit, 1, 1);
   atlasTopoFileLineEdit->setMinimumWidth(lineEditWidth);
   atlasTopoFileLineEdit->setReadOnly(true);
   
   //
   // File push button and line edit
   //
   filePushButton = new QPushButton("Data File...");
   dataFileGridLayout->addWidget(filePushButton, 2, 0);
   filePushButton->setAutoDefault(false);
   filePushButton->setEnabled(false);
   QObject::connect(filePushButton, SIGNAL(clicked()),
                    this, SLOT(slotFileButton()));
   dataFileNameLineEdit = new QLineEdit;
   dataFileGridLayout->addWidget(dataFileNameLineEdit, 2, 1);
   dataFileNameLineEdit->setMinimumWidth(lineEditWidth);
   dataFileNameLineEdit->setReadOnly(true);
   
   //
   // Deformed file push button and line edit
   //
   deformedFilePushButton = new QPushButton("Deformed File...");
   dataFileGridLayout->addWidget(deformedFilePushButton, 3, 0);
   deformedFilePushButton->setAutoDefault(false);
   deformedFilePushButton->setEnabled(false);
   QObject::connect(deformedFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeformedFileButton()));
   deformedFileNameLineEdit = new QLineEdit;
   dataFileGridLayout->addWidget(deformedFileNameLineEdit, 3, 1);
   deformedFileNameLineEdit->setMinimumWidth(lineEditWidth);
   deformedFileNameLineEdit->setReadOnly(false);
   
   //
   // indiv topo button and line edit
   //
   indivTopoFilePushButton = new QPushButton("Source Cut Topo File...");
   dataFileGridLayout->addWidget(indivTopoFilePushButton, 4, 0);
   indivTopoFilePushButton->setAutoDefault(false);
   indivTopoFilePushButton->setEnabled(false);
   QObject::connect(indivTopoFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotIndivTopoFileButton()));
   indivTopoFileLineEdit = new QLineEdit;
   dataFileGridLayout->addWidget(indivTopoFileLineEdit, 4, 1);
   indivTopoFileLineEdit->setMinimumWidth(lineEditWidth);
   indivTopoFileLineEdit->setReadOnly(true);
   
   //
   // deformed topo button and line edit
   //
   deformedTopoFilePushButton = new QPushButton("Deformed Topo File...");
   dataFileGridLayout->addWidget(deformedTopoFilePushButton, 5, 0);
   deformedTopoFilePushButton->setAutoDefault(false);
   deformedTopoFilePushButton->setEnabled(false);
   QObject::connect(deformedTopoFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeformedTopoFileButton()));
   deformedTopoFileLineEdit = new QLineEdit;
   dataFileGridLayout->addWidget(deformedTopoFileLineEdit, 5, 1);
   deformedTopoFileLineEdit->setMinimumWidth(lineEditWidth);
   deformedTopoFileLineEdit->setReadOnly(false);
   
   //
   // Flat coord max edge length
   //
   flatCoordMaxEdgeLengthLabel = new QLabel("Flat Max Edge Length");
   dataFileGridLayout->addWidget(flatCoordMaxEdgeLengthLabel, 6, 0);
   flatCoordMaxEdgeLengthDoubleSpinBox = new QDoubleSpinBox;
   flatCoordMaxEdgeLengthDoubleSpinBox->setMinimum(0.0);
   flatCoordMaxEdgeLengthDoubleSpinBox->setMaximum(10000.0);
   flatCoordMaxEdgeLengthDoubleSpinBox->setSingleStep(1.0);
   flatCoordMaxEdgeLengthDoubleSpinBox->setDecimals(2);
   dataFileGridLayout->addWidget(flatCoordMaxEdgeLengthDoubleSpinBox, 6, 1);
   flatCoordMaxEdgeLengthDoubleSpinBox->setValue(10.0);
   
   //
   // column prefix
   //
   QLabel* deformedColumnNamePrefixLabel = new QLabel("Column Prefix");
   dataFileGridLayout->addWidget(deformedColumnNamePrefixLabel, 7, 0);
   deformedColumnNamePrefixLineEdit = new QLineEdit;
   dataFileGridLayout->addWidget(deformedColumnNamePrefixLineEdit, 7, 1);
   
   //
   // Metric data deformation method
   //
   QGroupBox* metricGroupBox = new QGroupBox("Metric Deformation");
   dialogLayout->addWidget(metricGroupBox);
   QVBoxLayout* metricGroupLayout = new QVBoxLayout(metricGroupBox);
   metricNearestNodeRadioButton = new QRadioButton("Nearest Node");
   metricGroupLayout->addWidget(metricNearestNodeRadioButton);
   metricAverageTileNodesRadioButton = new QRadioButton("Average of Tile's Nodes");
   metricGroupLayout->addWidget(metricAverageTileNodesRadioButton);
   metricGroupBox->setFixedHeight(metricGroupBox->sizeHint().height());
   QButtonGroup* metricButtonGroup = new QButtonGroup(this);
   metricButtonGroup->addButton(metricNearestNodeRadioButton, 0);
   metricButtonGroup->addButton(metricAverageTileNodesRadioButton, 1);

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
   dialogLayout->addWidget(coordGroupBox);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);

   slotFileTypeComboBox(fileTypeComboBox->currentIndex());
}

/**
 * Destructor
 */
GuiApplyDeformationMapDialog::~GuiApplyDeformationMapDialog()
{
}

/**
 * called when a file type is selected.
 */
void 
GuiApplyDeformationMapDialog::slotFileTypeComboBox(int item)
{
   //
   // default labels and enabled
   //
   bool enableFlatItems = false;
   
   //
   // If valid def map loaded
   //
   if (filePushButton->isEnabled()) {
      //
      // Read the brain sets if needed
      //
      switch(item) {
         case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
            enableFlatItems = true;
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
            break;
      }
      
      atlasTopoFilePushButton->setEnabled(enableFlatItems);
      atlasTopoFileLineEdit->setEnabled(enableFlatItems);
      indivTopoFilePushButton->setEnabled(enableFlatItems);
      indivTopoFileLineEdit->setEnabled(enableFlatItems);
      deformedTopoFilePushButton->setEnabled(enableFlatItems);
      deformedTopoFileLineEdit->setEnabled(enableFlatItems);
      flatCoordMaxEdgeLengthDoubleSpinBox->setEnabled(enableFlatItems);
      flatCoordMaxEdgeLengthLabel->setEnabled(enableFlatItems);
      
      if (enableFlatItems) {
         filePushButton->setText("Source Flat Coord File...");
         deformedFilePushButton->setText("Deformed Coord File...");
      }
      else {
         filePushButton->setText("Data File...");
         deformedFilePushButton->setText("Deformed File...");
      }
   }
}
      
/**
 * called when source directory pushbutton is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotSourceDirectoryButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   fd.setWindowTitle("Select Source Directory");
   fd.setFileMode(WuQFileDialog::DirectoryOnly);
   
   //
   // Popup the dialog
   //
   if (fd.exec() == QDialog::Accepted) {   
      if (fd.selectedFiles().count() > 0) {
         sourceDirectoryLineEdit->setText(fd.selectedFiles().at(0));
         deformationMapFile.setSourceDirectory(fd.selectedFiles().at(0));
      }
   }
}

/**
 * called when target directory pushbutton is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotTargetDirectoryButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   fd.setWindowTitle("Select Target Directory");
   fd.setFileMode(WuQFileDialog::DirectoryOnly);
   
   //
   // Popup the dialog
   //
   if (fd.exec() == QDialog::Accepted) {   
      if (fd.selectedFiles().count() > 0) {
         targetDirectoryLineEdit->setText(fd.selectedFiles().at(0));
         deformationMapFile.setTargetDirectory(fd.selectedFiles().at(0));
      }
   }
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotApplyButton()
{
   const QString dataFileName(dataFileNameLineEdit->text());
   const QString deformedFileName(deformedFileNameLineEdit->text());
   const QString indivTopoFileName(indivTopoFileLineEdit->text());
   const QString indivDeformTopoFileName(deformedTopoFileLineEdit->text());
   const QString atlasTopoFileName(atlasTopoFileLineEdit->text());
   
   QString errorMessage;
   if (deformationMapFile.empty()) {
      errorMessage.append("Is deformation map file valid ?\n");
   }
   if (dataFileName.isEmpty()) {
      errorMessage.append("You must select a data file for deformation\n");
   }
   if (deformedFileName.isEmpty()) {
      errorMessage.append("You must select the output name for the deformed file.\n");
   }
   
   //
   // Set deformed column name prefix
   //
   deformationMapFile.setDeformedColumnNamePrefix(deformedColumnNamePrefixLineEdit->text());

   //
   // Get file type
   //
   const int itemIndex = fileTypeComboBox->currentIndex();
   const BrainModelSurfaceDeformDataFile::DATA_FILE_TYPE dft =
      static_cast<BrainModelSurfaceDeformDataFile::DATA_FILE_TYPE>(
         fileTypeComboBox->itemData(itemIndex).toInt());

   switch(dft) {
      case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
         if (indivTopoFileName.isEmpty()) {
            errorMessage.append("You must select an individual's topo file.\n");
         }
         if (indivDeformTopoFileName.isEmpty()) {
            errorMessage.append("You must select the name for the deformed indiv topo file.\n");
         }
         if (atlasTopoFileName.isEmpty()) {
            errorMessage.append("You must select the atlas topology file.\n");
         }
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
          break;
   }
   
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this, "Deform Error", errorMessage);
      return;
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   const QString savedDirectory(QDir::currentPath());

   if (deformationMapFile.getFileVersion() >= 2) {
      //
      // Deformation map file is modified is source or target directories are changed.
      //
      if (deformationMapFile.getModified()) {         
         try {
            deformationMapFile.writeFile(deformationMapFile.getFileName());
         }
         catch (FileException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "ERROR",
                              "Unable to update deformation map file with\n"
                              "individual and/or atlas directory changes.");
            return;
         }
      }
      
      QString msg;
      if (QFile::exists(deformationMapFile.getSourceDirectory()) == false) {
         msg.append("Individual directory is invalid.  Change the\n"
                    "individual directory to the directory containing\n"
                    "the individual spec file.");
      }
      if (QFile::exists(deformationMapFile.getTargetDirectory()) == false) {
         msg.append("Atlas directory is invalid.  Change the\n"
                    "atlas directory to the directory containing\n"
                    "the atlas spec file.");
      }
      if (msg.isEmpty() == false) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Directory Error", msg);
         return;
      }
   }
   
   //
   // Read the brain sets if needed
   //
   switch(dft) {
      case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
         {
            if (readBrains(errorMessage)) {
               QDir::setCurrent(savedDirectory);
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "Deform Error", errorMessage);
               return;
            }
         }
   }
 
   const QString savedOutputSpecFileName(deformationMapFile.getOutputSpecFileName());
   if (deformationMapFile.getFileVersion() >= 2) {
      //
      // Prepend with path
      //
      QString name(deformationMapFile.getTargetDirectory());
      name.append("/");
      name.append(deformationMapFile.getOutputSpecFileName());
      deformationMapFile.setOutputSpecFileName(name);
   }

   //
   // set metric deformation
   //
   if (metricNearestNodeRadioButton->isChecked()) {
      deformationMapFile.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_NEAREST_NODE);
   }
   else if (metricAverageTileNodesRadioButton->isChecked()) {
      deformationMapFile.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES);
   }

   //
   // Deform the data file
   //
   try {
      switch(dft) {
         case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
            BrainModelSurfaceDeformDataFile::deformNodeAttributeFile(&deformationMapFile,
                                                                     dft,
                                                                     dataFileName,
                                                                     deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
            BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFile(&deformationMapFile,
                                                                     dft,
                                                                     dataFileName,
                                                                     deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
            {
               QString deformedFileName2(deformedFileName);
               BrainModelSurfaceDeformDataFile::deformCoordinateFile(&deformationMapFile,
                                                                     dataFileName,
                                                                     deformedFileName2,
                                                                     smoothCoordsOneIterationCheckBox->isChecked());
            }
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
            BrainModelSurfaceDeformDataFile::deformFlatCoordinateFile(
                                    &deformationMapFile,
                                    atlasTopoFileName,
                                    dataFileName,
                                    indivTopoFileName,
                                    deformedFileName,
                                    indivDeformTopoFileName,
                                    flatCoordMaxEdgeLengthDoubleSpinBox->value());
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
            BrainModelSurfaceDeformDataFile::deformBorderFile(
                             sourceBrainSet,
                             targetBrainSet,
                             &deformationMapFile,
                             dft,
                             dataFileName,
                             deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
            BrainModelSurfaceDeformDataFile::deformCellOrFociFile(
                                 sourceBrainSet,
                                 targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 false,
                                 deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
            BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFile(
                                 sourceBrainSet,
                                 targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 false,
                                 deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
            BrainModelSurfaceDeformDataFile::deformCellOrFociFile(
                                 sourceBrainSet,
                                 targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 true,
                                 deformedFileName);
         case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
            BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFile(
                                 sourceBrainSet,
                                 targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 true,
                                 deformedFileName);
            break;
      }
   }
   catch (BrainModelAlgorithmException& e) {
      QDir::setCurrent(savedDirectory);
            QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "Deform Error", e.whatQString());
      //
      // Restore output spec file name
      //   
      if (deformationMapFile.getFileVersion() >= 2) {
         deformationMapFile.setOutputSpecFileName(savedOutputSpecFileName);
      }
   
      return;
   }
   
   //
   // Restore output spec file name
   //   
   if (deformationMapFile.getFileVersion() >= 2) {
      deformationMapFile.setOutputSpecFileName(savedOutputSpecFileName);
   }
   
   QDir::setCurrent(savedDirectory);
   QApplication::restoreOverrideCursor();
   
   QMessageBox::information(this, "Deform Successful",
                            "File successfully deformed.");
}

/**
 * called when deformation map button is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotDeformationMapButton()
{
   //
   // Create the file dialog
   //
   WuQFileDialog fd(this);
   fd.setDirectory(QDir::currentPath());
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setWindowTitle("Select Deformation Map File");
   fd.setFileMode(WuQFileDialog::ExistingFile);
   //QString fileFilter("Deformation Map File (*");
   //fileFilter.append(SpecFile::getDeformationMapFileExtension());
   //fileFilter.append(")");
   fd.setFilter(FileFilters::getDeformationMapFileFilter());
   fd.selectFilter(FileFilters::getDeformationMapFileFilter());
   
   //
   // Popup the dialog
   //
   if ((fd.exec() == QDialog::Accepted) &&
       (fd.selectedFiles().count() > 0)) {   
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      //
      // Since deformation map file may have changed delete the source and target
      // brain sets if they are loaded.
      //
      if (sourceBrainSet != NULL) {
         delete sourceBrainSet;
      }
      if (targetBrainSet != NULL) {
         delete targetBrainSet;
      }

      //
      // Disable file buttons and clear deformation map file name
      //
      sourceDirectoryPushButton->setEnabled(false);
      targetDirectoryPushButton->setEnabled(false);
      filePushButton->setEnabled(false);
      deformedFilePushButton->setEnabled(false);
      deformationMapFileLineEdit->setText("");
      atlasTopoFilePushButton->setEnabled(false);
      indivTopoFilePushButton->setEnabled(false);
      deformedTopoFilePushButton->setEnabled(false);
      
      //
      // Read the deformation map file
      //
      try {
         deformationMapFile.readFile(fd.selectedFiles().at(0));
      }
      catch (FileException& e) {
         QMessageBox::critical(this, "Deformation Map File Read Error", e.whatQString());
         return;
      }
      QApplication::restoreOverrideCursor();

      //
      // Set column prefix
      //
      deformedColumnNamePrefixLineEdit->setText(deformationMapFile.getDeformedColumnNamePrefix());
      
      //
      // Enable file buttons and display deformation map file name
      //
      filePushButton->setEnabled(true);
      deformedFilePushButton->setEnabled(true);
      deformationMapFileLineEdit->setText(fd.selectedFiles().at(0));
      
      //
      // Load source directory
      //
      bool needSourceDirectory = false;
      if (deformationMapFile.getFileVersion() >= 2) {
         sourceDirectoryLineEdit->setText(deformationMapFile.getSourceDirectory());
         sourceDirectoryPushButton->setEnabled(true);
         if (QFile::exists(deformationMapFile.getSourceDirectory()) == false) {
            needSourceDirectory = true;
         }
      }
      else {
         sourceDirectoryLineEdit->setText("");
      }
      sourceDirectoryLineEdit->setModified(false);
      
      //
      // Load target directory
      //
      bool needTargetDirectory = false;
      if (deformationMapFile.getFileVersion() >= 2) {
         targetDirectoryLineEdit->setText(deformationMapFile.getTargetDirectory());
         targetDirectoryPushButton->setEnabled(true);
         if (QFile::exists(deformationMapFile.getTargetDirectory()) == false) {
            needTargetDirectory = true;
         }
      }
      else {
         targetDirectoryLineEdit->setText("");
      }
      targetDirectoryLineEdit->setModified(false);
      
      //
      // Load the atlas closed topology file
      //
      if (needTargetDirectory == false) {
         QString topoName(deformationMapFile.getTargetClosedTopoFileName());
         if (deformationMapFile.getFileVersion() >= 2) {
            QString tempName(deformationMapFile.getTargetDirectory());
            tempName.append(QDir::separator());
            tempName.append(topoName);
            topoName = tempName;
         }
         if (QFile::exists(topoName)) {
            atlasTopoFileLineEdit->setText(topoName);
         }
      }
      
      QString message;
      if (needSourceDirectory) {
         message.append("Individual directory is invalid.  Please set it to the DIRECTORY\n"
                        "containing the individual spec file.\n");
      }
      if (needTargetDirectory) {
         message.append("Atlas directory is invalid.  Please set it to the DIRECTORY\n"
                        "containing the atlas spec file.");
      }
      if (message.isEmpty() == false) {
         QMessageBox::critical(this, "ERROR", message);
      }
      
      slotFileTypeComboBox(fileTypeComboBox->currentIndex());
      
      if (deformationMapFile.getMetricDeformationType() == 
          DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES) {
         metricAverageTileNodesRadioButton->setChecked(true);
      }
      else {
         metricNearestNodeRadioButton->setChecked(true);
      }
      
      smoothCoordsOneIterationCheckBox->setChecked(deformationMapFile.getSmoothDeformedSurfacesFlag());
   }
}

/**
 * called when indiv topo file button is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotIndivTopoFileButton()
{
   dataFileDialog(FILE_DIALOG_INDIV_TOPO_FILE);
}

/**
 * called when deformed topo file button is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotDeformedTopoFileButton()
{
   dataFileDialog(FILE_DIALOG_DEFORMED_TOPO_FILE);
}

/**
 * called when atlas topo file button is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotAtlasTopoFileButton()
{
   dataFileDialog(FILE_DIALOG_ATLAS_TOPO_FILE);
}

/**
 * called when file button is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotFileButton()
{
   dataFileDialog(FILE_DIALOG_DATA_FILE);
}

/**
 * called when deformed file button is pressed.
 */
void 
GuiApplyDeformationMapDialog::slotDeformedFileButton()
{
   dataFileDialog(FILE_DIALOG_DEFORMED_FILE);
}

/**
 * data file dialog for data file and deformed file.
 */
void 
GuiApplyDeformationMapDialog::dataFileDialog(const FILE_DIALOG_TYPE fdt)
{
   //
   // Set file filter based upon data file type
   //
   QString fileType;
   QString fileExtension;
   QString fileExtension2;
   const int itemIndex = fileTypeComboBox->currentIndex();
   switch(static_cast<BrainModelSurfaceDeformDataFile::DATA_FILE_TYPE>(fileTypeComboBox->itemData(itemIndex).toInt())) {
      case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
         fileType = "Areal Estimation File";
         fileExtension = SpecFile::getArealEstimationFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
         fileType = "Atlas File";
         fileExtension = SpecFile::getProbabilisticAtlasFileExtension();
         fileExtension2 = SpecFile::getProbabilisticAtlasFileExtension() +
                          SpecFile::getPaintFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
         fileType = "Flat Border File";
         fileExtension = SpecFile::getBorderFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
         fileType = "Spherical Border File";
         fileExtension = SpecFile::getBorderFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
         fileType = "Border Projection File";
         fileExtension = SpecFile::getBorderProjectionFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
         fileType = "Cell File";
         fileExtension = SpecFile::getCellFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
         fileType = "Cell Projection File";
         fileExtension = SpecFile::getCellProjectionFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
         fileType = "Coordinate File";
         fileExtension = SpecFile::getCoordinateFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
         fileType = "Flat Coordinate File";
         fileExtension = SpecFile::getCoordinateFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
         fileType = "Foci File";
         fileExtension = SpecFile::getFociFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
         fileType = "Foci Projection File";
         fileExtension = SpecFile::getFociProjectionFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
         fileType = "Lat/Long File";
         fileExtension = SpecFile::getLatLonFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
         fileType = "Metric File";
         fileExtension = SpecFile::getMetricFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
         fileType = "Paint File";
         fileExtension = SpecFile::getPaintFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
         fileType = "RGB Paint File";
         fileExtension = SpecFile::getRgbPaintFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
         fileType = "Surface Shape File";
         fileExtension = SpecFile::getSurfaceShapeFileExtension();
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
         fileType = "Topography File";
         fileExtension = SpecFile::getTopographyFileExtension();
         break;
   }
   
   //
   // Create the file dialog and override file type/ext in some cases
   //
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   switch(fdt) {
      case FILE_DIALOG_ATLAS_TOPO_FILE:
         fileType = "Topology File";
         fileExtension = SpecFile::getTopoFileExtension();
         fd.setWindowTitle("Select Target Topo File For Deformation");
         fd.setFileMode(WuQFileDialog::ExistingFile);
         break;
      case FILE_DIALOG_DATA_FILE:
         fd.setWindowTitle("Select Data File For Deformation");
         fd.setFileMode(WuQFileDialog::ExistingFile);
         break;
      case FILE_DIALOG_DEFORMED_FILE:
         fd.setWindowTitle("Select Deformed Data File (Output File)");
         fd.setFileMode(WuQFileDialog::AnyFile);
         break;
      case FILE_DIALOG_INDIV_TOPO_FILE:
         fileType = "Topology File";
         fileExtension = SpecFile::getTopoFileExtension();
         fd.setWindowTitle("Select Source Topo File For Deformation");
         fd.setFileMode(WuQFileDialog::ExistingFile);
         break;
      case FILE_DIALOG_DEFORMED_TOPO_FILE:
         fileType = "Topology File";
         fileExtension = SpecFile::getTopoFileExtension();
         fd.setWindowTitle("Select Deformed Topology File (Output File)");
         fd.setFileMode(WuQFileDialog::AnyFile);
         break;
   }
   
   //
   // Create the file filter
   //
   QString filter(fileType);
   filter.append(" (*");
   filter.append(fileExtension);
   if (fileExtension2.isEmpty() == false) {
      filter.append(" *");
      filter.append(fileExtension2);
   }
   filter.append(")");
   fd.setFilter(filter);
   
   //
   // Popup the dialog
   //
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         const QString fileName(fd.selectedFiles().at(0));
         switch(fdt) {
            case FILE_DIALOG_ATLAS_TOPO_FILE:
               atlasTopoFileLineEdit->setText(fileName);
               break;
            case FILE_DIALOG_DATA_FILE:
               dataFileNameLineEdit->setText(fileName);
               setDeformedFileName(fdt);
               break;
            case FILE_DIALOG_DEFORMED_FILE:
               deformedFileNameLineEdit->setText(fileName);
               break;
            case FILE_DIALOG_INDIV_TOPO_FILE:
               indivTopoFileLineEdit->setText(fileName);
               setDeformedFileName(fdt);
               break;
            case FILE_DIALOG_DEFORMED_TOPO_FILE:
               deformedTopoFileLineEdit->setText(fileName);
               break;
         }
      }
   }
}      

/**
 * Set the deformed file name using the input data file name and deformation map file.
 */
void
GuiApplyDeformationMapDialog::setDeformedFileName(const FILE_DIALOG_TYPE fdt)
{
   //
   // Name of deformation map file and data file that is to be deformed
   //
   const QString dmfName(deformationMapFileLineEdit->text());
   QString filename;
   QLineEdit* lineEdit = NULL;
   switch (fdt) {
      case FILE_DIALOG_ATLAS_TOPO_FILE:
         break;
      case FILE_DIALOG_DATA_FILE:
         filename = dataFileNameLineEdit->text();
         lineEdit = deformedFileNameLineEdit;
         break;
      case FILE_DIALOG_DEFORMED_FILE:
         break;
      case FILE_DIALOG_INDIV_TOPO_FILE:
         filename = indivTopoFileLineEdit->text();
         lineEdit = deformedTopoFileLineEdit;
         break;
      case FILE_DIALOG_DEFORMED_TOPO_FILE:
         break;
   }
   if (lineEdit == NULL) {
      return;
   }
   
   //
   // Create the deformed file name
   //
   QString outputSpecFileName;
   if (deformationMapFile.getFileVersion() >= 2) {
      outputSpecFileName = deformationMapFile.getTargetDirectory();
      outputSpecFileName.append("/");
      outputSpecFileName.append(FileUtilities::basename(deformationMapFile.getOutputSpecFileName()));
   }
   else {
      outputSpecFileName.append(deformationMapFile.getOutputSpecFileName());
   }
   QString defName =
         BrainModelSurfaceDeformDataFile::createDeformedFileName(filename,
                                       outputSpecFileName,
                                       deformationMapFile.getDeformedFileNamePrefix(),
                                       deformationMapFile.getNumberOfNodes(),
                                       false);
   
   const QString fileDir(FileUtilities::dirname(defName));
   if (fileDir.isEmpty() == false) {
      if (fileDir != ".") {
         if (QFile::exists(fileDir) == false) {
            defName = FileUtilities::basename(defName);
            QString msg("Deformation output directory not found: ");
            msg.append(fileDir);
            msg.append("\n");
            msg.append("Output file location defaults to current directory.");
            QMessageBox::information(this, "File Info", msg);
         }
      }
   }
   
   //
   // Put deformed name into line edit
   //
   lineEdit->setText(defName);
}

/**
 * Read in the source and target brains needed for some deformations
 */
bool
GuiApplyDeformationMapDialog::readBrains(QString& errorMessage)
{
   errorMessage = "";
   
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Does the source brain set need to be created
   //
   if (sourceBrainSet == NULL) {
      //
      // Read in the source spec file
      //
      bool sourceSpecMissing = true;
      SpecFile sourceSpecFile;
      try {
         QString specFileName;
         if (deformationMapFile.getFileVersion() >= 2) {
            specFileName.append(deformationMapFile.getSourceDirectory());
            specFileName.append("/");
         }
         specFileName.append(deformationMapFile.getSourceSpecFileName());
         sourceSpecFile.readFile(specFileName);
         sourceSpecMissing = false;
      }
      catch (FileException& e) {
         //
         // David has a bad habit of renaming spec files, so just hope the
         // data files are still the same name and in the same location.
         //
         QDir::setCurrent(deformationMapFile.getSourceDirectory());
         //errorMessage = e.whatQString());
         //return true;
      }
      
      //
      // Select the deformation files
      //
      sourceSpecFile.setDeformationSelectedFiles(
            deformationMapFile.getSourceClosedTopoFileName(),
            deformationMapFile.getSourceCutTopoFileName(),
            deformationMapFile.getSourceFiducialCoordFileName(),
            deformationMapFile.getSourceSphericalCoordFileName(),
            deformationMapFile.getSourceFlatCoordFileName(),
            "",
            sourceSpecMissing,
            sourceSpecFile.getStructure());
      
      //
      // Read in the source brain set
      //
      std::vector<QString> errorMessages;
      sourceBrainSet = new BrainSet;
      sourceBrainSet->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                   sourceSpecFile,
                                   deformationMapFile.getSourceSpecFileName(),
                                   errorMessages, NULL, NULL);
      if (errorMessages.empty() == false) {
         if (sourceSpecMissing) {
            errorMessages.push_back("Source spec file was not found.\n"
                                    "Tried to load data files.\n");
         }
         errorMessage = StringUtilities::combine(errorMessages, "\n");
         delete sourceBrainSet;
         sourceBrainSet = NULL;
         return true;
      }
      
      //
      // Read in the deformed coordinate file
      //
      if (deformationMapFile.getInverseDeformationFlag() == false) {
         QString coordFileName;
         BrainModelSurface::SURFACE_TYPES surfaceType =
                            BrainModelSurface::SURFACE_TYPE_UNKNOWN;
         switch(deformationMapFile.getFlatOrSphereSelection()) {
            case DeformationMapFile::DEFORMATION_TYPE_FLAT:
               coordFileName = 
                  deformationMapFile.getSourceDeformedFlatCoordFileName();
               surfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
               break;
            case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
               coordFileName = 
                  deformationMapFile.getSourceDeformedSphericalCoordFileName();
               surfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
               break;
         }
         if (coordFileName.isEmpty()) {
            errorMessage = "Deformed source coordinate file is missing.";
            delete sourceBrainSet;
            sourceBrainSet = NULL;
            return true;
         }
         try {
            sourceBrainSet->readCoordinateFile(coordFileName,
                                               surfaceType,
                                               false,
                                               true,
                                               false);
         }
         catch (FileException& e) {
            errorMessage = e.whatQString();
            delete sourceBrainSet;
            sourceBrainSet = NULL;
            return true;
         }
      }
   }
   
   QDir::setCurrent(savedDirectory);
   
   //
   // Does the target brain set need to be created
   //
   if (targetBrainSet == NULL) {
      //
      // Read in the target spec file
      //
      bool targetSpecMissing = true;
      SpecFile targetSpecFile;
      try {
         QString specFileName;
         if (deformationMapFile.getFileVersion() >= 2) {
            specFileName.append(deformationMapFile.getTargetDirectory());
            specFileName.append("/");
         }
         specFileName.append(deformationMapFile.getTargetSpecFileName());
         targetSpecFile.readFile(specFileName);
         targetSpecMissing = false;
      }
      catch (FileException& e) {
         //
         // David has a bad habit of renaming spec files, so just hope the
         // data files are still the same name and in the same location.
         //
         QDir::setCurrent(deformationMapFile.getSourceDirectory());
         //errorMessage = e.whatQString());
         //return true;
      }
      
      //
      // Select the deformation files
      //
      targetSpecFile.setDeformationSelectedFiles(
            deformationMapFile.getTargetClosedTopoFileName(),
            deformationMapFile.getTargetCutTopoFileName(),
            deformationMapFile.getTargetFiducialCoordFileName(),
            deformationMapFile.getTargetSphericalCoordFileName(),
            deformationMapFile.getTargetFlatCoordFileName(),
            "",
            targetSpecMissing,
            targetSpecFile.getStructure());
      
      //
      // Read in the target brain set
      //
      std::vector<QString> errorMessages;
      targetBrainSet = new BrainSet;
      targetBrainSet->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                   targetSpecFile,
                                   deformationMapFile.getTargetSpecFileName(),
                                   errorMessages, NULL, NULL);
      if (errorMessages.empty() == false) {
         if (targetSpecMissing) {
            errorMessages.push_back("Target spec file was not found.\n"
                                    "Tried to load data files.\n");
         }
         errorMessage = StringUtilities::combine(errorMessages, "\n");
         delete targetBrainSet;
         targetBrainSet = NULL;
         return true;
      }
      
      //
      // Read in the deformed coordinate file
      //
      if (deformationMapFile.getInverseDeformationFlag()) {
         QString coordFileName;
         BrainModelSurface::SURFACE_TYPES surfaceType =
                            BrainModelSurface::SURFACE_TYPE_UNKNOWN;
         switch(deformationMapFile.getFlatOrSphereSelection()) {
            case DeformationMapFile::DEFORMATION_TYPE_FLAT:
               coordFileName = 
                  deformationMapFile.getSourceDeformedFlatCoordFileName();
               surfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
               break;
            case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
               coordFileName = 
                  deformationMapFile.getSourceDeformedSphericalCoordFileName();
               surfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
               break;
         }
         if (coordFileName.isEmpty()) {
            errorMessage = "Deformed source coordinate file is missing.";
            delete sourceBrainSet;
            sourceBrainSet = NULL;
            return true;
         }
         try {
            targetBrainSet->readCoordinateFile(coordFileName,
                                               surfaceType,
                                               false,
                                               true,
                                               false);
         }
         catch (FileException& e) {
            errorMessage = e.whatQString();
            delete sourceBrainSet;
            sourceBrainSet = NULL;
            return true;
         }
      }
   }
   
   QDir::setCurrent(savedDirectory);

   return false;
}
