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
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "BrainModelSurfaceDistortion.h"
#include "BrainSet.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiDistortionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "QtUtilities.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiDistortionDialog::GuiDistortionDialog(QWidget* parent, BrainModelSurface* bmsIn)
   : QtDialog(parent, true)
{
   bms = bmsIn;
   
   setWindowTitle("Distortion Measurement");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // Group box for surfaces
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surfaces");
   QGridLayout* surfaceGroupGridLayout = new QGridLayout(surfaceGroupBox);
   dialogLayout->addWidget(surfaceGroupBox);
   
   //
   // current surface
   //
   surfaceGroupGridLayout->addWidget(new QLabel("Surface"), 0, 0);
   surfaceGroupGridLayout->addWidget(new QLabel(bms->getDescriptiveName()), 0, 1);

   //
   // Reference surface
   //
   surfaceGroupGridLayout->addWidget(new QLabel("Reference Surface"), 1, 0);
   referenceSurfaceComboBox = new GuiBrainModelSelectionComboBox(false, true, false,
                                                                 "", 0);   
   surfaceGroupGridLayout->addWidget(referenceSurfaceComboBox, 1, 1);
   
   //
   // Group box for distortion name and selection
   //
   QGroupBox* distortionGroupBox = new QGroupBox("Distortion");
   QGridLayout* distortionGridLayout = new QGridLayout(distortionGroupBox);
   dialogLayout->addWidget(distortionGroupBox);
   
   //
   // Names of columns
   //
   distortionGridLayout->addWidget(new QLabel("Distortion Type"), 0, 0);
   distortionGridLayout->addWidget(new QLabel("Column Name"), 0, 1);
   distortionGridLayout->addWidget(new QLabel("Surface Shape Column"), 0, 2);
   
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   
   //
   // Areal distortion
   //
   distortionGridLayout->addWidget(new QLabel("Areal Distortion"), 1, 0);
   arealDistortionNameLineEdit = new QLineEdit;
   distortionGridLayout->addWidget(arealDistortionNameLineEdit, 1, 1);
   arealDistortionColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                         GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                                         true,
                                                         true,
                                                         false);
   distortionGridLayout->addWidget(arealDistortionColumnComboBox, 1, 2);
   QObject::connect(arealDistortionColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotArealDistortion(int)));
   int arealDistortionColumnNumber = ssf->getArealDistortionColumnNumber();
   if (arealDistortionColumnNumber < 0) {
      arealDistortionColumnNumber= GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW;
   }
   arealDistortionColumnComboBox->setCurrentIndex(arealDistortionColumnNumber);
   slotArealDistortion(arealDistortionColumnNumber);
   
   //
   // Linear distortion
   //
   distortionGridLayout->addWidget(new QLabel("Linear Distortion"), 2, 0);
   linearDistortionNameLineEdit = new QLineEdit;
   distortionGridLayout->addWidget(linearDistortionNameLineEdit, 2, 1);
   linearDistortionColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                         GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                                         true,
                                                         true,
                                                         false);
   distortionGridLayout->addWidget(linearDistortionColumnComboBox);
   QObject::connect(linearDistortionColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotLinearDistortion(int)));
   int linearDistortionColumnNumber = ssf->getLinearDistortionColumnNumber();
   if (linearDistortionColumnNumber < 0) {
      linearDistortionColumnNumber= GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW;
   }
   linearDistortionColumnComboBox->setCurrentIndex(linearDistortionColumnNumber);
   slotLinearDistortion(linearDistortionColumnNumber);
   
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
 *
 */
GuiDistortionDialog::~GuiDistortionDialog()
{
}

/**
 * called when linear distortion column selected.
 */
void 
GuiDistortionDialog::slotLinearDistortion(int item)
{
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   if (item >= 0) {
      linearDistortionNameLineEdit->setText(ssf->getColumnName(item));
   }
   else {
      linearDistortionNameLineEdit->setText("Linear Distortion");
   }
}

/**
 * called when areal distortion column selected.
 */
void 
GuiDistortionDialog::slotArealDistortion(int item)
{
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   if (item >= 0) {
      arealDistortionNameLineEdit->setText(ssf->getColumnName(item));
   }
   else {
      arealDistortionNameLineEdit->setText("Areal Distortion");
   }
}
      
/**
 * Called when OK or Cancel buttons pressed
 */
void
GuiDistortionDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      QString arealName(arealDistortionNameLineEdit->text());
      if (arealDistortionColumnComboBox->currentIndex() == 
         GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE) {
         arealName = "";
      }
      
      QString linearName(linearDistortionNameLineEdit->text());
      if (linearDistortionColumnComboBox->currentIndex() == 
         GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE) {
         linearName = "";
      }
   
      BrainModelSurfaceDistortion bmsd(theMainWindow->getBrainSet(),
                                       bms,
                                       referenceSurfaceComboBox->getSelectedBrainModelSurface(),
                                       bms->getTopologyFile(),
                                       theMainWindow->getBrainSet()->getSurfaceShapeFile(),
                                       arealName,
                                       linearName);
      try {
         bmsd.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Error", e.whatQString());
         return;
      }
      GuiFilesModified fm;
      fm.setSurfaceShapeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
   
   QDialog::done(r);
}

