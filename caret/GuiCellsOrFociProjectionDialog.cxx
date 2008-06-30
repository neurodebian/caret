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
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CellFile.h"
#include "CellFileProjector.h"
#include "CellProjectionFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiCellsOrFociProjectionDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "WuQDialog.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiCellsOrFociProjectionDialog::GuiCellsOrFociProjectionDialog(QWidget* parent, FILE_TYPE fileTypeIn)
   : WuQDialog(parent)
{
   setModal(true);
   fileType = fileTypeIn;
   
   switch (fileType) {
      case FILE_TYPE_CELL:
         setWindowTitle("Cell Projection");
         break;
      case FILE_TYPE_FOCI:
         setWindowTitle("Foci Projection");
         break;
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // The radio buttons
   //
   projectAllRadioButton = new QRadioButton("All (no sign flips to match hemisphere)");
   projectFlipToMatchRadioButton = new QRadioButton("All (Flip signs to match Hemisphere; Surface MUST be AC centered)");
   projectHemisphereOnlyRadioButton = new QRadioButton("Hemisphere Only (Surface MUST be AC centered)");

   //
   // Button group for data to project
   //
   QButtonGroup* dataButtonGroup = new QButtonGroup(this);
   dataButtonGroup->addButton(projectAllRadioButton);
   dataButtonGroup->addButton(projectFlipToMatchRadioButton);
   dataButtonGroup->addButton(projectHemisphereOnlyRadioButton);
   
   //
   // Projection type group box
   //
   QGroupBox* projectionGroupBox = new QGroupBox("Data For Projection");
   dialogLayout->addWidget(projectionGroupBox);
   QVBoxLayout* projectionGroupLayout = new QVBoxLayout(projectionGroupBox);
   projectionGroupLayout->addWidget(projectAllRadioButton);
   projectionGroupLayout->addWidget(projectFlipToMatchRadioButton);
   projectionGroupLayout->addWidget(projectHemisphereOnlyRadioButton);
   
   //
   // Project onto surface option
   //
   projectOntoSurfaceRadioButton = new QRadioButton("Project Above Surface");
   surfaceOffsetDoubleSpinBox = new QDoubleSpinBox;
   surfaceOffsetDoubleSpinBox->setMinimum(0.0);
   surfaceOffsetDoubleSpinBox->setMaximum(1000.0);
   surfaceOffsetDoubleSpinBox->setSingleStep(1.0);
   surfaceOffsetDoubleSpinBox->setDecimals(2);
   QHBoxLayout* ontoSurfaceHBoxLayout = new QHBoxLayout;
   ontoSurfaceHBoxLayout->addWidget(projectOntoSurfaceRadioButton);
   ontoSurfaceHBoxLayout->addWidget(surfaceOffsetDoubleSpinBox);
   
   //
   // Maintain offset from surface option
   //
   maintainOffsetFromSurfaceRadioButton = new QRadioButton("Keep Offset From Surface");
   maintainOffsetFromSurfaceRadioButton->setChecked(true);
                                                
   //
   // Button group for spatial projection
   //
   QButtonGroup* projectionButtonGroup = new QButtonGroup(this);
   projectionButtonGroup->addButton(projectOntoSurfaceRadioButton);
   projectionButtonGroup->addButton(maintainOffsetFromSurfaceRadioButton);
   
   //
   // Spatial projection type options box
   //
   QGroupBox* projectionTypeGroupBox = new QGroupBox("Spatial Projection Type");
   dialogLayout->addWidget(projectionTypeGroupBox);
   QVBoxLayout* projectionTypeLayout = new QVBoxLayout(projectionTypeGroupBox);
   projectionTypeLayout->addLayout(ontoSurfaceHBoxLayout);
   projectionTypeLayout->addWidget(maintainOffsetFromSurfaceRadioButton);
   
   QLabel* notesLabel = new QLabel(
      "Caret determines the hemisphere of cells/foci using the\n"
      "sign of the X-coordinate.  Negative implies left\n"
      "hemisphere, positive implies right hemisphere.");
   QGroupBox* noteGroupBox = new QGroupBox("Important Information");
   QVBoxLayout* notesLayout = new QVBoxLayout(noteGroupBox);
   notesLayout->addWidget(notesLabel);
   dialogLayout->addWidget(noteGroupBox);

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
  
   //
   // Help button
   //
   QPushButton* helpButton = new QPushButton("Help");
   helpButton->setAutoDefault(false);
   buttonsLayout->addWidget(helpButton);
   QObject::connect(helpButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpButton()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton, helpButton);
}

/**
 * Destructor.
 */
GuiCellsOrFociProjectionDialog::~GuiCellsOrFociProjectionDialog()
{
}

/**
 * Called when help button is pressed.
 */
void
GuiCellsOrFociProjectionDialog::slotHelpButton()
{
   theMainWindow->showHelpPageOverModalDialog(this, "dialogs/project_cells_foci_dialog.html");
}

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiCellsOrFociProjectionDialog::done(int r)
{
   //
   // Was OK button pressed
   //
   if (r == QDialog::Accepted) {
      //
      // Get projection type
      //
      CellFileProjector::PROJECTION_TYPE projType = CellFileProjector::PROJECTION_TYPE_ALL;
      if (projectAllRadioButton->isChecked()) {
         projType = CellFileProjector::PROJECTION_TYPE_ALL;
      }
      else if (projectHemisphereOnlyRadioButton->isChecked()) {
         projType = CellFileProjector::PROJECTION_TYPE_HEMISPHERE_ONLY;
      }
      else if (projectFlipToMatchRadioButton->isChecked()) {
         projType = CellFileProjector::PROJECTION_TYPE_FLIP_TO_MATCH_HEMISPHERE;
      }
      else {
         QMessageBox::critical(this, "ERROR", "You must select a projection type.");
         return;
      }
      
      //
      // Find the fiducial surface
      //
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getActiveFiducialSurface();
      if (bms == NULL) {
         QMessageBox::critical(this, "ERROR", "There is no fiducial surface.");
         return;
      }

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      //
      // Save name and clear cell/foci projection file and get cell/foci files
      //
      CellProjectionFile* cpf = NULL;
      switch (fileType) {
         case FILE_TYPE_CELL:
            cpf = theMainWindow->getBrainSet()->getCellProjectionFile();
            break;
         case FILE_TYPE_FOCI:
            cpf = theMainWindow->getBrainSet()->getFociProjectionFile();
            break;
      }
      
      //
      // Project all of the cell/foci files
      //
      CellFileProjector projector(bms);
      projector.projectFile(cpf,
                            0,
                            projType,
                            surfaceOffsetDoubleSpinBox->value(),
                            projectOntoSurfaceRadioButton->isChecked(),
                            theMainWindow);
      
      GuiFilesModified fm;
      
      switch (fileType) {
         case FILE_TYPE_CELL:
            fm.setCellModified();
            fm.setCellProjectionModified();
            break;
         case FILE_TYPE_FOCI:
            fm.setFociModified();
            fm.setFociProjectionModified();
            break;
      }
      
      //
      // Update now that cells/foci are changed
      //
      theMainWindow->fileModificationUpdate(fm);
      
      //
      // Update all displayed surfaces
      //
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
   }
   QDialog::done(r);
}


