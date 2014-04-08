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

#include <QGlobalStatic>

#include <iostream>
#include <limits>

#define __GUI_CONTOUR_RECONSTRUCTION_DIALOG_MAIN__
#include "GuiContourReconstructionDialog.h"
#undef __GUI_CONTOUR_RECONSTRUCTION_DIALOG_MAIN__

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelContourToSurfaceConverter.h"
#include "BrainModelContours.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiStructureComboBox.h"
#include "QtUtilities.h"
#include "global_variables.h"

/// Constructor
GuiContourReconstructionDialog::GuiContourReconstructionDialog(QWidget* parent,
                                                               bool modalFlag, Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   setModal(modalFlag);
   setWindowTitle("Contour Reconstruction Into Surface");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Group box for structure
   //
   QGroupBox* structureGroup = new QGroupBox("Structure");
   QVBoxLayout* structureLayout = new QVBoxLayout(structureGroup);
   dialogLayout->addWidget(structureGroup);
   
   //
   // left and right hemisphere checkboxes
   //
   structureComboBox = new GuiStructureComboBox;
   structureComboBox->setStructure(theMainWindow->getBrainSet()->getStructure().getType());
   structureLayout->addWidget(structureComboBox);
   
   //
   // Group box for contour cells
   //
   QGroupBox* cellGroup = new QGroupBox("Contour Cells");
   QVBoxLayout* cellLayout = new QVBoxLayout(cellGroup);
   dialogLayout->addWidget(cellGroup);
   
   //
   // convert contour cells
   //
   convertContourCellsCheckBox = new QCheckBox("Convert Contour Cells");
   cellLayout->addWidget(convertContourCellsCheckBox);
   
   //
   // Group box for reconstruction parameters
   //                                        
   QGroupBox* reconstructionGroupBox = new QGroupBox("Reconstruction Parameters");    
   dialogLayout->addWidget(reconstructionGroupBox);
   QGridLayout* reconstructionLayout = new QGridLayout(reconstructionGroupBox);
   
   //
   // reconstruction voxel dimensions
   //
   reconstructionLayout->addWidget(new QLabel("Voxel Dimension"), 0, 0);
   voxelDimensionSpinBox = new QSpinBox;
   voxelDimensionSpinBox->setMinimum(1);
   voxelDimensionSpinBox->setMaximum(2048);
   voxelDimensionSpinBox->setSingleStep(1);
   reconstructionLayout->addWidget(voxelDimensionSpinBox, 0, 1);
   
   //
   // reconstruction max polygons
   //
   reconstructionLayout->addWidget(new QLabel("Polygon Limit"), 1, 0);
   polygonLimitSpinBox = new QSpinBox;
   polygonLimitSpinBox->setMinimum(1);
   polygonLimitSpinBox->setMaximum(std::numeric_limits<int>::max());
   polygonLimitSpinBox->setSingleStep(1000);
   reconstructionLayout->addWidget(polygonLimitSpinBox, 1, 1);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout; 
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   //
   // OK button 
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   loadSavedValues();
}

/**
 * Destructor.
 */
GuiContourReconstructionDialog::~GuiContourReconstructionDialog()
{
}

/** 
 * Load the saved values into the dialog
 */
void
GuiContourReconstructionDialog::loadSavedValues()
{
      convertContourCellsCheckBox->setChecked(savedConvertCells);
      voxelDimensionSpinBox->setValue(savedVoxelDimensions);
      polygonLimitSpinBox->setValue(savedPolygonLimit);
}

/**
 * Called when OK or Cancel button pressed.
 */
void
GuiContourReconstructionDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      savedConvertCells    = convertContourCellsCheckBox->isChecked();
      savedVoxelDimensions = voxelDimensionSpinBox->value();
      savedPolygonLimit    = polygonLimitSpinBox->value();
      
      BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours(-1);
      if (bmc == NULL) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Convert to Surface Error", 
                               "There are no contours in the main window.");
         return;
      }
      BrainModelContourToSurfaceConverter c2s(theMainWindow->getBrainSet(),
                                              bmc->getContourFile(),
                                              savedVoxelDimensions,
                                              savedPolygonLimit,
                                              structureComboBox->getSelectedStructure(),
                                              savedConvertCells);
      try {
         c2s.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Convert to Surface Error", e.whatQString());
         return;
      }
      
      GuiFilesModified fm;
      fm.setCellModified();
      fm.setCellColorModified();
      fm.setCoordinateModified();
      fm.setTopologyModified();
      theMainWindow->fileModificationUpdate(fm);

      GuiBrainModelOpenGL::updateAllGL(NULL);
      
      QApplication::restoreOverrideCursor();
      
      theMainWindow->speakText("Contour reconstruction complete.", false);
   }
   
   QDialog::done(r);
}
