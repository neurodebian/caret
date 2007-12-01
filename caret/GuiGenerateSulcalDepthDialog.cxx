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
#include <QCheckBox>
#include "WuQFileDialog.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSpinBox>
#include <QToolTip>

#include "BrainModelSurfaceSulcalDepthWithNormals.h"
#include "BrainSet.h"
#include "FileFilters.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiGenerateSulcalDepthDialog.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "QtUtilities.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiGenerateSulcalDepthDialog::GuiGenerateSulcalDepthDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Generate Sulcal Depth");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // surface group box
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Fiducial Surface");
   dialogLayout->addWidget(surfaceGroupBox);
   QVBoxLayout* surfaceGroupBoxLayout = new QVBoxLayout(surfaceGroupBox);
   //
   // Fiducial Surface comb box
   //
   surfaceComboBox = new GuiBrainModelSelectionComboBox(
                                          false,
                                          true,
                                          false,
                                          "",
                                          0);
   surfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainModelSurface());
   surfaceGroupBoxLayout->addWidget(surfaceComboBox);
   
   //
   // Horizontal group box for hull surface selection
   //
   QGroupBox* hullGroupBox = new QGroupBox("Hull File");
   dialogLayout->addWidget(hullGroupBox);
   QVBoxLayout* hullGroupLayout = new QVBoxLayout(hullGroupBox);
   
   //
   // Horiz layout for hull selection
   //
   QHBoxLayout* hullHBoxLayout = new QHBoxLayout;
   hullGroupLayout->addLayout(hullHBoxLayout);
   
   //
   // Button to select hull file
   //
   QPushButton* hullButton = new QPushButton("VTK Select...");
   QObject::connect(hullButton, SIGNAL(clicked()),
                    this, SLOT(slotHullButton()));
   hullButton->setFixedSize(hullButton->sizeHint());
   hullHBoxLayout->addWidget(hullButton);
                    
   //
   // hull file name line edit
   //
   hullFileNameLineEdit = new QLineEdit(hullGroupBox);
   hullFileNameLineEdit->setReadOnly(true);
   hullFileNameLineEdit->setText(theMainWindow->getBrainSet()->getCerebralHullFileName());
   hullHBoxLayout->addWidget(hullFileNameLineEdit);
   
   //
   // Checkbox for create hull surface
   //
   createHullSurfaceCheckBox = new QCheckBox("Create Hull Surface");
   createHullSurfaceCheckBox->setChecked(true);
   hullGroupLayout->addWidget(createHullSurfaceCheckBox);
   createHullSurfaceCheckBox->setToolTip(
                 "Create a hull surface with the same number\n"
                 "number of nodes as surface in main window.");
   
   //
   // Group Box for line edits and combo boxes
   //
   QGroupBox* groupBox = new QGroupBox("Surface Shape Columns");
   QGridLayout* groupBoxGridLayout = new QGridLayout(groupBox);
   dialogLayout->addWidget(groupBox);
   
   //
   // Depth Name line edit
   //
   depthNameLineEdit = new QLineEdit;
   depthNameLineEdit->setText("Depth");
   groupBoxGridLayout->addWidget(depthNameLineEdit, 0, 0);
   
   //
   // depth column combo box
   //
   depthColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                         GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                         true,
                                         true,
                                         false);
   depthColumnComboBox->setCurrentIndex(
      GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
   groupBoxGridLayout->addWidget(depthColumnComboBox, 0, 1);
      
   //
   // smooth depth Name line edit
   //
   smoothDepthNameLineEdit = new QLineEdit;
   smoothDepthNameLineEdit->setText("Smoothed Depth");
   groupBoxGridLayout->addWidget(smoothDepthNameLineEdit, 1, 0);
   
   //
   // smoothed depth column combo box
   //
   smoothDepthColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                         GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                         true,
                                         true,
                                         false);
   smoothDepthColumnComboBox->setCurrentIndex(
      GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
   groupBoxGridLayout->addWidget(smoothDepthColumnComboBox, 1, 1);
   
   //
   // Group Box for smoothing spin boxes
   //
   QGroupBox* smoothingGroupBox = new QGroupBox("Smoothing");
   dialogLayout->addWidget(smoothingGroupBox);
   QGridLayout* smoothingGroupLayout = new QGridLayout(smoothingGroupBox);
   
   //
   // hull smoothing
   //
   smoothingGroupLayout->addWidget(new QLabel("Hull Smoothing Iterations"), 0, 0);
   hullSmoothingSpinBox = new QSpinBox;
   hullSmoothingSpinBox->setMinimum(0);
   hullSmoothingSpinBox->setMaximum(100000);
   hullSmoothingSpinBox->setSingleStep(10);
   hullSmoothingSpinBox->setValue(5);
   smoothingGroupLayout->addWidget(hullSmoothingSpinBox, 0, 1);
      
   //
   // depth smoothing
   //
   smoothingGroupLayout->addWidget(new QLabel("Depth Smoothing Iterations"), 1, 0);
   depthSmoothingSpinBox = new QSpinBox;
   depthSmoothingSpinBox->setMinimum(0);
   depthSmoothingSpinBox->setMaximum(100000);
   depthSmoothingSpinBox->setSingleStep(10);
   depthSmoothingSpinBox->setValue(100);
   smoothingGroupLayout->addWidget(depthSmoothingSpinBox, 1, 1);
   
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
                    
   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * called when hull file select button is pressed
 */
void 
GuiGenerateSulcalDepthDialog::slotHullButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilter(FileFilters::getVtkSurfaceFileFilter());
   fd.selectFilter(FileFilters::getVtkSurfaceFileFilter());
   fd.setWindowTitle("Select VTK Poly Data File");
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         hullFileNameLineEdit->setText(fd.selectedFiles().at(0));
      }
   }
}
      
/**
 * Destructor
 */
GuiGenerateSulcalDepthDialog::~GuiGenerateSulcalDepthDialog()
{
}

/**
 * Called when OK or Cancel button pressed.
 */
void
GuiGenerateSulcalDepthDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      //
      // Get the surface for depth calculation
      //
      BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
      if (bms == NULL) {
         QMessageBox::critical(this, "Error",
                               "A surface must be selected.");
         return;
      }
      
      //
      // Get the vtk file name
      //
      const QString vtkFileName(hullFileNameLineEdit->text());
      if (vtkFileName.isEmpty()) {
         QMessageBox::critical(this, "Error",
                               "VTK file name is missing.");
         return;
      }
      
      //
      // Get depth selections
      //
      int depthColumn = depthColumnComboBox->currentIndex();
      if (depthColumn == 
          GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
         depthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_CREATE_NEW;
      }
      else if (depthColumn == 
               GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE) {
         depthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE;
      }
      const QString depthName(depthNameLineEdit->text());
      if (depthColumn != BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE) {
         if (depthName.isEmpty()) {
            QMessageBox::critical(this, "Error",
                                  "Name for depth must be provided.");
            return;
         }
      }
      
      //
      // Get smoothed depth selections
      //
      int smoothDepthColumn = smoothDepthColumnComboBox->currentIndex();
      if (smoothDepthColumn == 
          GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
         smoothDepthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_CREATE_NEW;
      }
      else if (smoothDepthColumn == 
               GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE) {
         smoothDepthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE;
      }
      const QString smoothDepthName(smoothDepthNameLineEdit->text());
      if (smoothDepthColumn != BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE) {
         if (smoothDepthName.isEmpty()) {
            QMessageBox::critical(this, "Error",
                                  "Name for smoothed depth must be provided.");
            return;
         }
      }
      
      //
      // Make sure at least one depth is being generated
      //
      if ((depthColumn == BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE) &&
          (smoothDepthColumn == BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE)) {
         QMessageBox::critical(this, "Error", "No shape file columns selected");
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      //
      // Hull surface
      //
      BrainModelSurface* hullSurface = NULL;
      CoordinateFile* hullCoord = NULL;
      if (theMainWindow->getBrainModelSurface()) {
         hullSurface = new BrainModelSurface(*theMainWindow->getBrainModelSurface());
         hullSurface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_UNKNOWN);
         hullCoord = hullSurface->getCoordinateFile();
         hullCoord->makeDefaultFileName("CerebralHull");
         theMainWindow->getBrainSet()->addBrainModel(hullSurface);
      }
      
      try {
         //
         // Generate sulcal depth
         //
         BrainModelSurfaceSulcalDepthWithNormals bmssd(theMainWindow->getBrainSet(),
                                         bms,
                                         vtkFileName,
                                         theMainWindow->getBrainSet()->getSurfaceShapeFile(),
                                         hullSmoothingSpinBox->value(),
                                         depthSmoothingSpinBox->value(),
                                         depthColumn,
                                         smoothDepthColumn,
                                         depthName,
                                         smoothDepthName,
                                         hullCoord);
         bmssd.execute();
         
         //
         // Update surface shape related items and redraw
         //
         GuiFilesModified fm;
         fm.setSurfaceShapeModified();
         fm.setCoordinateModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Error", e.whatQString());
         return;
      }
      
      QApplication::restoreOverrideCursor();
   }
   
   QDialog::done(r);
}

