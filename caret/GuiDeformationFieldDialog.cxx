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
#include <QDir>
#include "WuQFileDialog.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "DeformationFieldFile.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiDeformationFieldDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "QtUtilities.h"
#include "StringUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiDeformationFieldDialog::GuiDeformationFieldDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setModal(true);
   dialogMode = MODE_TYPE_NONE;
   
   setWindowTitle("Create Deformation Field");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Mode vertical button group and radio buttons
   //
   QGroupBox* modeButtonGroupBox = new QGroupBox("Mode");
   QVBoxLayout* modeLayout = new QVBoxLayout(modeButtonGroupBox);
   dialogLayout->addWidget(modeButtonGroupBox);
   dialogModeAtlasIndivRadioButton = new QRadioButton("Atlas To Indiv");
   dialogModeIndivAtlasRadioButton = new QRadioButton("Indiv To Atlas");
   modeLayout->addWidget(dialogModeAtlasIndivRadioButton);
   modeLayout->addWidget(dialogModeIndivAtlasRadioButton);
   QButtonGroup* modeButtonGroup = new QButtonGroup(this);
   modeButtonGroup->addButton(dialogModeAtlasIndivRadioButton, 0);
   modeButtonGroup->addButton(dialogModeIndivAtlasRadioButton, 1);
   QObject::connect(modeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotModeButtonGroup()));

   //
   // Atlas to Indiv Group Box
   //
   atlasIndivGroupBox = new QGroupBox("Atlas to Indiv");
   dialogLayout->addWidget(atlasIndivGroupBox);
   QGridLayout* atlasIndivGridLayout = new QGridLayout(atlasIndivGroupBox);
   
   //
   // atlas surface
   //
   atlasIndivGridLayout->addWidget(new QLabel("Atlas Spherical Surface  "), 0, 0);
   atlasSurfaceComboBox = new GuiBrainModelSelectionComboBox(
                                                        false,
                                                        true,
                                                        false,
                                                        "Select Surface",
                                                        0);
   atlasSurfaceComboBox->setSelectedBrainModelIndex(GuiBrainModelSelectionComboBox::ADD_NEW_INDEX);
   atlasIndivGridLayout->addWidget(atlasSurfaceComboBox, 0, 1);
   
   //
   // indiv source topo file
   //
   QPushButton* indivTopoFilePushButton = new QPushButton("Indiv Closed Topo File...");
   indivTopoFilePushButton->setAutoDefault(false);
   QObject::connect(indivTopoFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotIndivTopoFilePushButton()));
   indivTopoFileLineEdit = new QLineEdit;
   indivTopoFileLineEdit->setReadOnly(true);
   atlasIndivGridLayout->addWidget(indivTopoFilePushButton, 1, 0);
   atlasIndivGridLayout->addWidget(indivTopoFileLineEdit, 1, 1);
   
   //
   // indiv source coord file
   //
   QPushButton* indivCoordFilePushButton = new QPushButton("Indiv Spherical Coord File...");
   indivCoordFilePushButton->setAutoDefault(false);
   QObject::connect(indivCoordFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotIndivCoordFilePushButton()));
   indivCoordFileLineEdit = new QLineEdit;
   indivCoordFileLineEdit->setReadOnly(true);
   atlasIndivGridLayout->addWidget(indivCoordFilePushButton, 2, 0);
   atlasIndivGridLayout->addWidget(indivCoordFileLineEdit, 2, 1);
   //
   // indiv target coord file
   //
   QPushButton* indivDeformedCoordFilePushButton = new QPushButton("Indiv Deformed Spherical Coord File...");
   indivDeformedCoordFilePushButton->setAutoDefault(false);
   QObject::connect(indivDeformedCoordFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotIndivDeformedCoordFilePushButton()));
   indivDeformedCoordFileLineEdit = new QLineEdit;
   indivDeformedCoordFileLineEdit->setReadOnly(true);
   atlasIndivGridLayout->addWidget(indivDeformedCoordFilePushButton, 3, 0);
   atlasIndivGridLayout->addWidget(indivDeformedCoordFileLineEdit, 3, 1);
   
   
   //
   // Indiv to Atlas Group Box
   //
   indivAtlasGroupBox = new QGroupBox("Indiv To Atlas");
   dialogLayout->addWidget(indivAtlasGroupBox);
   QGridLayout* indivAtlasGridLayout = new QGridLayout(indivAtlasGroupBox);
   
   //
   // Source surface
   //
   indivAtlasGridLayout->addWidget(new QLabel("Source Spherical Surface"), 0, 0);
   surfaceComboBox = new GuiBrainModelSelectionComboBox(
                                                        false,
                                                        true,
                                                        false,
                                                        "Select Surface",
                                                        indivAtlasGroupBox);
   surfaceComboBox->setSelectedBrainModelIndex(GuiBrainModelSelectionComboBox::ADD_NEW_INDEX);
   indivAtlasGridLayout->addWidget(surfaceComboBox, 0, 1);
   
   //
   // deformed surface
   //
   indivAtlasGridLayout->addWidget(new QLabel("Deformed Spherical Surface"), 1, 0);
   deformedSurfaceComboBox = new GuiBrainModelSelectionComboBox(
                                                        false,
                                                        true,
                                                        false,
                                                        "Select Surface",
                                                        indivAtlasGroupBox);
   deformedSurfaceComboBox->setSelectedBrainModelIndex(GuiBrainModelSelectionComboBox::ADD_NEW_INDEX);
   indivAtlasGridLayout->addWidget(deformedSurfaceComboBox, 1, 1);
   
   //
   // Deformation Field File Group Box
   //
   QGroupBox* defFileGroupBox = new QGroupBox("Deformation Field File");
   dialogLayout->addWidget(defFileGroupBox);
   QGridLayout* defFileGridLayout = new QGridLayout(defFileGroupBox);
   
   //
   // deformation field file column
   //
   defFileGridLayout->addWidget(new QLabel("Def Field File Column  "), 0, 0);
   deformationFileColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                          GUI_NODE_FILE_TYPE_DEFORMATION_FIELD,
                                                          true,
                                                          false,
                                                          false);
   QObject::connect(deformationFileColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotDeformationFileColumnComboBox(int)));
   defFileGridLayout->addWidget(deformationFileColumnComboBox, 0, 1);
   
   //
   // deformation field file column name
   //
   defFileGridLayout->addWidget(new QLabel("Column Name  "), 1, 0);
   columnNameLineEdit = new QLineEdit(defFileGroupBox);
   columnNameLineEdit->setText("New Column Name");
   defFileGridLayout->addWidget(columnNameLineEdit, 1, 1);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                  this, SLOT(accept()));
                  
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                  this, SLOT(reject()));

   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   deformationFileColumnComboBox->setCurrentIndex(
                       GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
                       
   slotModeButtonGroup();
}

/**
 * destructor.
 */
GuiDeformationFieldDialog::~GuiDeformationFieldDialog()
{
}

/**
 * called when a mode button is selected.
 */
void 
GuiDeformationFieldDialog::slotModeButtonGroup()
{
   dialogMode = MODE_TYPE_NONE;
   if (dialogModeAtlasIndivRadioButton->isChecked()) {
      dialogMode = MODE_TYPE_ATLAS_TO_INDIV;
   }
   else if (dialogModeIndivAtlasRadioButton->isChecked()) {
      dialogMode = MODE_TYPE_INDIV_TO_ATLAS;
   }
   
   atlasIndivGroupBox->setEnabled(false);
   indivAtlasGroupBox->setEnabled(false);
   switch (dialogMode) {
      case MODE_TYPE_NONE:
         break;
      case MODE_TYPE_INDIV_TO_ATLAS:
         indivAtlasGroupBox->setEnabled(true);
         break;
      case MODE_TYPE_ATLAS_TO_INDIV:
         atlasIndivGroupBox->setEnabled(true);
         break;
   }
}
      
/**
 * called when pushbutton is pressed.
 */
void 
GuiDeformationFieldDialog::slotIndivTopoFilePushButton()
{
   //
   // Create a file dialog and get the file.
   //
   WuQFileDialog fd(this);
   fd.setDirectory(QDir::currentPath());
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setWindowTitle("Choose Closed Topology File");
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilter(FileFilters::getTopologyClosedFileFilter());
   fd.selectFilter(FileFilters::getTopologyClosedFileFilter());
   if (fd.exec() == QDialog::Accepted) {
      QStringList sl = fd.selectedFiles();
      if (sl.isEmpty() == false) {
         indivTopoFileLineEdit->setText(sl.at(0));
      }
   }
}

/**
 * called when pushbutton is pressed.
 */
void 
GuiDeformationFieldDialog::slotIndivCoordFilePushButton()
{
   //
   // Create a file dialog and get the file.
   //
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setWindowTitle("Choose Spherical Coordinate File");
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilter(FileFilters::getCoordinateSphericalFileFilter());
   fd.selectFilter(FileFilters::getCoordinateSphericalFileFilter());
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         indivCoordFileLineEdit->setText(fd.selectedFiles().at(0));
      }
   }
}
/**
 * called when pushbutton is pressed.
 */
void 
GuiDeformationFieldDialog::slotIndivDeformedCoordFilePushButton()
{
   //
   // Create a file dialog and get the file.
   //
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setWindowTitle("Choose Deformed Spherical Coordinate File");
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilter(FileFilters::getCoordinateSphericalFileFilter());
   fd.selectFilter(FileFilters::getCoordinateSphericalFileFilter());
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         indivDeformedCoordFileLineEdit->setText(fd.selectedFiles().at(0));
      }
   }
}

/**
 * called when dialog is closed.
 */
void 
GuiDeformationFieldDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      switch (dialogMode) {
         case MODE_TYPE_NONE:
            break;
         case MODE_TYPE_INDIV_TO_ATLAS:
            {
               const BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
               if (bms == NULL) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", "No Surface is selected.");
                  return;
               }
               if (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
                  QMessageBox::critical(this,
                                        "ERROR",
                                        "The source surface must be spherical.");
                  return;
               }
               
               const BrainModelSurface* defBms = deformedSurfaceComboBox->getSelectedBrainModelSurface();
               if (defBms == NULL) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", "No Deformed Surface is selected.");
                  return;
               }
               if (defBms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
                  QMessageBox::critical(this,
                                        "ERROR",
                                        "The deformed surface must be spherical.");
                  return;
               }

               bms->createDeformationField(defBms,
                                           deformationFileColumnComboBox->currentIndex(),
                                           columnNameLineEdit->text(),
                                           *(theMainWindow->getBrainSet()->getDeformationFieldFile()));
            }
            break;
         case MODE_TYPE_ATLAS_TO_INDIV:
            {
               //
               // Make sure inputs are there
               //
               QString indivTopoFileName(indivTopoFileLineEdit->text());
               QString indivCoordFileName(indivCoordFileLineEdit->text());
               QString indivDeformedCoordFileName(indivDeformedCoordFileLineEdit->text());
               if (indivTopoFileName.isEmpty()) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", "Indiv Topo File Name is missing.");
                  return;
               }
               if (indivCoordFileName.isEmpty()) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", "Indiv Coord File Name is missing.");
                  return;
               }
               if (indivDeformedCoordFileName.isEmpty()) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", "Indiv Deformed Coord File Name is missing.");
                  return;
               }
               
               const BrainModelSurface* bms = atlasSurfaceComboBox->getSelectedBrainModelSurface();
               if (bms == NULL) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", "No Atlas Surface is selected.");
                  return;
               }
               
               //
               // Create a spec file for the indiv
               //
               QString specFileName(FileUtilities::dirname(indivTopoFileName));
               specFileName.append("/temp_spec");
               specFileName.append(SpecFile::getSpecFileExtension());
               SpecFile indivSpecFile;
               indivSpecFile.setFileName(specFileName);
               indivSpecFile.addToSpecFile(SpecFile::closedTopoFileTag,
                                           indivTopoFileName, "", false);
               indivSpecFile.addToSpecFile(SpecFile::sphericalCoordFileTag,
                                           indivCoordFileName, "", false);
               indivSpecFile.addToSpecFile(SpecFile::sphericalCoordFileTag,
                                           indivDeformedCoordFileName, "", false);
               indivSpecFile.setAllFileSelections(SpecFile::SPEC_TRUE);                            
               
               //
               // Create a brain set for the indiv
               //
               BrainSet indivBrainSet;
               
               //
               // load the indiv spec file into the indiv brain set
               //
               std::vector<QString> errorMessages;
               QString currentDirectory(QDir::currentPath());
               indivBrainSet.readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                          indivSpecFile,
                                          "indivSpecFile",
                                          errorMessages, NULL, NULL);
               QDir::setCurrent(currentDirectory);
                                          
               //
               // Did an error occur while reading the files
               //
               if (errorMessages.empty() == false) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR reading indiv files",
                                          StringUtilities::combine(errorMessages, "\n"));
                  return;
               }
               
               //
               // Should be two brain models
               //
               if (indivBrainSet.getNumberOfBrainModels() < 2) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR",
                                          "Problems reading indiv files (less than 2 brain models)");
                  return;
               }
               
               //
               // Get the indiv and indiv deformed surfaces
               //
               const BrainModelSurface* indivSurface = indivBrainSet.getBrainModelSurface(0);
               if (indivSurface == NULL) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR",
                                          "Indiv surface missing.");
                  return;
               }
               const BrainModelSurface* indivDeformSurface = indivBrainSet.getBrainModelSurface(1);
               if (indivDeformSurface == NULL) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR",
                                          "Indiv deformed surface missing.");
                  return;
               }
               
               //
               // Create the deformation field
               //
               bms->createDeformationField(indivSurface,
                                           indivDeformSurface,
                                           deformationFileColumnComboBox->currentIndex(),
                                           columnNameLineEdit->text(),
                                           *(theMainWindow->getBrainSet()->getDeformationFieldFile()));
            }
            break;
      }

      GuiFilesModified fm;
      fm.setDeformationFieldModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::beep();
      QApplication::restoreOverrideCursor();
   }
   
   QDialog::done(r);
}

/**
 * called when a column is selected.
 */
void 
GuiDeformationFieldDialog::slotDeformationFileColumnComboBox(int item)
{
   const DeformationFieldFile* dff = theMainWindow->getBrainSet()->getDeformationFieldFile();
   
   columnNameLineEdit->setText("New Column");
   if ((item >= 0) && (item < dff->getNumberOfColumns())) {
      columnNameLineEdit->setText(dff->getColumnName(item));
   }
}
