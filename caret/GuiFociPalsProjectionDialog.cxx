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
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "FociFileToPalsProjector.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiFociPalsProjectionDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiFociPalsProjectionDialog::GuiFociPalsProjectionDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Foci Projection to PALS Atlas");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
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
      "Caret determines the hemisphere of foci using the\n"
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
GuiFociPalsProjectionDialog::~GuiFociPalsProjectionDialog()
{
}

/**
 * Called when help button is pressed.
 */
void
GuiFociPalsProjectionDialog::slotHelpButton()
{
   theMainWindow->showHelpPageOverModalDialog(this, "dialogs/project_foci_to_pals_dialog.html");
}

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiFociPalsProjectionDialog::done(int r)
{
   //
   // Was OK button pressed
   //
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
      //
      // Save name and clear cell/foci projection file and get cell/foci files
      //
      FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
      
      //
      // Project the foci
      //
      FociFileToPalsProjector fociProjector(theMainWindow->getBrainSet(),
                                            fpf,
                                            0,
                                            surfaceOffsetDoubleSpinBox->value(),
                                            projectOntoSurfaceRadioButton->isChecked());
      try {
         fociProjector.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
      }
      GuiFilesModified fm;
      
      fm.setFociModified();
      fm.setFociProjectionModified();
      
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


