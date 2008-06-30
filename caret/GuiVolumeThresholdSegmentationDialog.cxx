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
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelVolumeThresholdSegmentation.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsVolume.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiVolumeThresholdSegmentationDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiVolumeThresholdSegmentationDialog::GuiVolumeThresholdSegmentationDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Anatomy Threshold Segmentation");
   
   //
   // min thresh
   //
   QLabel* minThreshLabel = new QLabel("Minimum Threshold");
   minThresholdSpinBox = new QDoubleSpinBox;
   minThresholdSpinBox->setMinimum(-std::numeric_limits<float>::max());
   minThresholdSpinBox->setMaximum(std::numeric_limits<float>::max());
   minThresholdSpinBox->setSingleStep(1.0);
   minThresholdSpinBox->setDecimals(5);
   QObject::connect(minThresholdSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMinThresholdSpinBox(double)));
   minThresholdSpinBox->setValue(175);

   //
   // max thresh
   //
   QLabel* maxThreshLabel = new QLabel("Maximum Threshold");
   maxThresholdSpinBox = new QDoubleSpinBox;
   maxThresholdSpinBox->setMinimum(-std::numeric_limits<float>::max());
   maxThresholdSpinBox->setMaximum(std::numeric_limits<float>::max());
   maxThresholdSpinBox->setSingleStep(1.0);
   maxThresholdSpinBox->setDecimals(5);
   QObject::connect(maxThresholdSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMaxThresholdSpinBox(double)));
   maxThresholdSpinBox->setValue(256);

   //
   // Grid Layout for thesholds
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(minThreshLabel, 0, 0, 1, 1);
   gridLayout->addWidget(minThresholdSpinBox, 0, 1, 1, 1);
   gridLayout->addWidget(maxThreshLabel, 1, 0, 1, 1);
   gridLayout->addWidget(maxThresholdSpinBox, 1, 1,  1, 1);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   buttonsLayout->addWidget(applyButton);
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyPushButton()));
                    
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));

   //
   // Make buttons same size
   //
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
       
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   dialogLayout->addLayout(gridLayout);
   dialogLayout->addLayout(buttonsLayout);
}

/**
 * destructor.
 */
GuiVolumeThresholdSegmentationDialog::~GuiVolumeThresholdSegmentationDialog()
{
}

/**
 * called to update the dialog.
 */
void 
GuiVolumeThresholdSegmentationDialog::updateDialog()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() <= 0) {
      close();
   }
}

/**
 * called to show the dialog (override's QDialog's show() method).
 */
void 
GuiVolumeThresholdSegmentationDialog::show()
{
   slotMinThresholdSpinBox(minThresholdSpinBox->value());
   slotMaxThresholdSpinBox(maxThresholdSpinBox->value());
   WuQDialog::show();
}

/**
 * called to close the dialog (override's QDialog's close() method).
 */
void 
GuiVolumeThresholdSegmentationDialog::close()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setAnatomyThresholdValid(false);
   GuiBrainModelOpenGL::updateAllGL();
   WuQDialog::close();
}

/**
 * called when min spin box value is changed.
 */
void 
GuiVolumeThresholdSegmentationDialog::slotMinThresholdSpinBox(double val)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setAnatomyThresholdValid(true);
   float thresh[2];
   dsv->getAnatomyThreshold(thresh[0], thresh[1]);
   thresh[0] = val;
   dsv->setAnatomyThreshold(thresh[0], thresh[1]);
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * called when max spin box value is changed.
 */
void 
GuiVolumeThresholdSegmentationDialog::slotMaxThresholdSpinBox(double val)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setAnatomyThresholdValid(true);
   float thresh[2];
   dsv->getAnatomyThreshold(thresh[0], thresh[1]);
   thresh[1] = val;
   dsv->setAnatomyThreshold(thresh[0], thresh[1]);
   GuiBrainModelOpenGL::updateAllGL();
}
      
/** 
 * called when apply button is pressed.
 */
void 
GuiVolumeThresholdSegmentationDialog::slotApplyPushButton()
{

   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv != NULL) {
      VolumeFile* vf = bmv->getSelectedVolumeAnatomyFile();
      if (vf == NULL) {
         QMessageBox::critical(this, "ERROR", "No anatomy volume selected.");
         return;
      }

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      try {
         const float thresh[2] = {
            minThresholdSpinBox->value(),
            maxThresholdSpinBox->value()
         };
         BrainModelVolumeThresholdSegmentation bmvts(theMainWindow->getBrainSet(), vf, thresh);
         bmvts.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
         return;
      }
      
      BrainModelVolumeVoxelColoring* bmvvc = theMainWindow->getBrainSet()->getVoxelColoring();
      bmvvc->setPrimaryOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION);
      DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
      dsv->setSelectedSegmentationVolume(theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() - 1);

      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
      QApplication::beep();
   }
}
