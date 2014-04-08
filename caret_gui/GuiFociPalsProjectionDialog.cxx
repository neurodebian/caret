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
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>

#include "GuiFociPalsProjectionDialog.h"

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "FociFileToPalsProjector.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiFociPalsProjectionDialog::GuiFociPalsProjectionDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setModal(true);
   setWindowTitle("Foci Projection to PALS Atlas");
   
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
   QVBoxLayout* projectionTypeLayout = new QVBoxLayout(projectionTypeGroupBox);
   projectionTypeLayout->addLayout(ontoSurfaceHBoxLayout);
   projectionTypeLayout->addWidget(maintainOffsetFromSurfaceRadioButton);
   
   //
   // cerebellum cutoff distances
   //
   QLabel* cerebellarCutoffLabel = new QLabel("Cerebellum Cutoff (R2)");
   cerebellarCutoffDistanceDoubleSpinBox = new QDoubleSpinBox;
   cerebellarCutoffDistanceDoubleSpinBox->setMinimum(0.0);
   cerebellarCutoffDistanceDoubleSpinBox->setMaximum(1000000000.0);
   cerebellarCutoffDistanceDoubleSpinBox->setSingleStep(1.0);
   cerebellarCutoffDistanceDoubleSpinBox->setDecimals(2);
   cerebellarCutoffDistanceDoubleSpinBox->setValue(
      FociFileToPalsProjector::getDefaultCerebellumCutoffDistance());
   QLabel* cerebralCutoffLabel = new QLabel("Cerebral Cutoff (R1)");
   cerebralCutoffDistanceDoubleSpinBox = new QDoubleSpinBox;
   cerebralCutoffDistanceDoubleSpinBox->setMinimum(0.0);
   cerebralCutoffDistanceDoubleSpinBox->setMaximum(1000000000.0);
   cerebralCutoffDistanceDoubleSpinBox->setSingleStep(1.0);
   cerebralCutoffDistanceDoubleSpinBox->setDecimals(2);
   cerebralCutoffDistanceDoubleSpinBox->setValue(
      FociFileToPalsProjector::getDefaultCerebralCutoffDistance());
   
   //
   // Cerebellar Options group box
   //
   cerebellarOptionsGroupBox = new QGroupBox("Allow Projection to Cerebellum");
   cerebellarOptionsGroupBox->setCheckable(true);
   cerebellarOptionsGroupBox->setChecked(true);
   QGridLayout* cerebellaOptionsLayout = new QGridLayout(cerebellarOptionsGroupBox);
   cerebellaOptionsLayout->addWidget(cerebralCutoffLabel, 0, 0);
   cerebellaOptionsLayout->addWidget(cerebralCutoffDistanceDoubleSpinBox, 0, 1);
   cerebellaOptionsLayout->addWidget(cerebellarCutoffLabel, 1, 0);
   cerebellaOptionsLayout->addWidget(cerebellarCutoffDistanceDoubleSpinBox, 1, 1);
   
   QLabel* notesLabel = new QLabel(
      "If Allow Projection to Cerebellum is selected:\n"
      "   ** A ratio of (distance-to-cerebral-cortext divided by\n"
      "                  distance-to-cerebellum) is computed.\n"
      "   ** If this ratio is less than \"Cerebral Cutoff (R1)\",\n"
      "      the focus is assigned to the left or right cerebral\n"
      "      cortex based upon the X-coordinate of the focus.\n"
      "   ** If this ratio is greater than \"Cerebellum Cutoff (R2)\"\n"
      "      the focus is assigned to the cerebellum.\n"
      "   ** If the ratio is between the two cutoffs, the focus is\n"
      "      assigned to both the cerebellum and also the left or  \n"
      "      right cerebral cortex.\n");
   QGroupBox* noteGroupBox = new QGroupBox("Important Information");
   QVBoxLayout* notesLayout = new QVBoxLayout(noteGroupBox);
   notesLayout->addWidget(notesLabel);

   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
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
  
   //
   // Help button
   //
   QPushButton* helpButton = new QPushButton("Help");
   helpButton->setAutoDefault(false);
   buttonsLayout->addWidget(helpButton);
   QObject::connect(helpButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpButton()));
                    
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   dialogLayout->addWidget(cerebellarOptionsGroupBox);
   dialogLayout->addWidget(projectionTypeGroupBox);
   dialogLayout->addWidget(noteGroupBox);
   dialogLayout->addLayout(buttonsLayout);
   
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
      // Get cutoff distances
      //
      const float cerebralCutoffDistance = cerebralCutoffDistanceDoubleSpinBox->value();
      const float cerebellarCutoffDistance = cerebellarCutoffDistanceDoubleSpinBox->value();
      
      //
      // Project the foci
      //
      FociFileToPalsProjector fociProjector(theMainWindow->getBrainSet(),
                                            fpf,
                                            theMainWindow->getBrainSet()->getStudyMetaDataFile(),
                                            0,
                                            -1,
                                            surfaceOffsetDoubleSpinBox->value(),
                                            projectOntoSurfaceRadioButton->isChecked(),
                                            cerebellarOptionsGroupBox->isChecked(),
                                            cerebralCutoffDistance,
                                            cerebellarCutoffDistance);
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


