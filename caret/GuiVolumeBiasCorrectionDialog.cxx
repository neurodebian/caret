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

#include <limits>

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsVolume.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiHistogramDisplayDialog.h"
#include "GuiMainWindow.h"
#include "GuiVolumeBiasCorrectionDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiVolumeBiasCorrectionDialog::GuiVolumeBiasCorrectionDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Anatomy Volume Bias Correction");
   
   //
   // min gray
   //
   QLabel* minGrayLabel = new QLabel("Gray Minimum");
   minGrayValueSpinBox = new QSpinBox;
   minGrayValueSpinBox->setMinimum(std::numeric_limits<int>::min());
   minGrayValueSpinBox->setMaximum(std::numeric_limits<int>::max());
   minGrayValueSpinBox->setSingleStep(1);
   QObject::connect(minGrayValueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMinGrayValueSpinBox(int)));
   minGrayValueSpinBox->setValue(0);

   //
   // max hwite
   //
   QLabel* maxWhiteLabel = new QLabel("White Maximum");
   maxWhiteValueSpinBox = new QSpinBox;
   maxWhiteValueSpinBox->setMinimum(std::numeric_limits<int>::min());
   maxWhiteValueSpinBox->setMaximum(std::numeric_limits<int>::max());
   maxWhiteValueSpinBox->setSingleStep(1);
   QObject::connect(maxWhiteValueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMaxWhiteValueSpinBox(int)));
   maxWhiteValueSpinBox->setValue(255);

   //
   // iterations spin box
   //
   QLabel* iterationsLabel = new QLabel("Iterations");
   iterationsSpinBox = new QSpinBox;
   iterationsSpinBox->setMinimum(1);
   iterationsSpinBox->setMaximum(10000);
   iterationsSpinBox->setSingleStep(1);
   iterationsSpinBox->setValue(5);
   
   //
   // Grid Layout for thesholds
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(minGrayLabel, 0, 0, 1, 1);
   gridLayout->addWidget(minGrayValueSpinBox, 0, 1, 1, 1);
   gridLayout->addWidget(maxWhiteLabel, 1, 0, 1, 1);
   gridLayout->addWidget(maxWhiteValueSpinBox, 1, 1,  1, 1);
   gridLayout->addWidget(iterationsLabel, 2, 0, 1, 1);
   gridLayout->addWidget(iterationsSpinBox, 2, 1, 1, 1);
   
   //
   // Histogram push button
   //
   QPushButton* histogramPushButton = new QPushButton("View Histogram...");
   histogramPushButton->setAutoDefault(false);
   histogramPushButton->setFixedSize(histogramPushButton->sizeHint());
   QObject::connect(histogramPushButton, SIGNAL(clicked()),
                    this, SLOT(slotHistogramPushButton()));
                    
   //
   // Instructions
   //
   const QString helpInstructions = 
      "Set the \"Gray Minimum\" so that it is just below the smallest\n"
      "gray value in the volume.  Set the \"White Maximum\" so it is just \n"
      "above the largest white value in the volume.  You should see all gray\n"
      "and white matter voxels colored in green in the main window.  Viewing\n"
      "the anatomy volume's histogram may be helpful.\n"
      "\n"
      "Normally, 5 iterations is sufficent.  However, if the bias is\n"
      "significant, a value of 30 or greater may be needed.";
   QLabel* instructionsLabel = new QLabel(helpInstructions);
   
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
   dialogLayout->addWidget(histogramPushButton);
   dialogLayout->addWidget(instructionsLabel);
   dialogLayout->addLayout(buttonsLayout);
}

/**
 * destructor.
 */
GuiVolumeBiasCorrectionDialog::~GuiVolumeBiasCorrectionDialog()
{
}

/**
 * called to update the dialog.
 */
void 
GuiVolumeBiasCorrectionDialog::updateDialog()
{
   if (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() <= 0) {
      close();
   }
}

/**
 * called to show the dialog (override's QDialog's show() method).
 */
void 
GuiVolumeBiasCorrectionDialog::show()
{
   slotMinGrayValueSpinBox(minGrayValueSpinBox->value());
   slotMaxWhiteValueSpinBox(maxWhiteValueSpinBox->value());
   WuQDialog::show();
}

/**
 * called to close the dialog (override's QDialog's close() method).
 */
void 
GuiVolumeBiasCorrectionDialog::close()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setAnatomyThresholdValid(false);
   GuiBrainModelOpenGL::updateAllGL();
   QDialog::close();
}

/**
 * called when min spin box value is changed.
 */
void 
GuiVolumeBiasCorrectionDialog::slotMinGrayValueSpinBox(int val)
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
GuiVolumeBiasCorrectionDialog::slotMaxWhiteValueSpinBox(int val)
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
 * called when histogram button pressed.
 */
void 
GuiVolumeBiasCorrectionDialog::slotHistogramPushButton()
{
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv != NULL) {
      VolumeFile* vf = bmv->getSelectedVolumeAnatomyFile();
      if (vf == NULL) {
         QMessageBox::critical(this, "ERROR", "No anatomy volume selected.");
         return;
      }
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
     
      const int numVoxels = vf->getTotalNumberOfVoxels();
      std::vector<float> values(numVoxels);
      float minValue =  std::numeric_limits<float>::max();
      float maxValue = -std::numeric_limits<float>::max();
      for (int i = 0; i < numVoxels; i++) {
         values[i] = vf->getVoxelWithFlatIndex(i, 0);
         minValue = std::min(minValue, values[i]);
         maxValue = std::max(maxValue, values[i]);
      }
      GuiHistogramDisplayDialog* ghd = new GuiHistogramDisplayDialog(theMainWindow,
                                             FileUtilities::basename(vf->getFileName()),
                                             values,
                                             true,
                                             false);
      ghd->show();

      QApplication::restoreOverrideCursor();
   }
}
      
/** 
 * called when apply button is pressed.
 */
void 
GuiVolumeBiasCorrectionDialog::slotApplyPushButton()
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
         vf->biasCorrectionWithAFNI(minGrayValueSpinBox->value(),
                                    maxWhiteValueSpinBox->value(),
                                    iterationsSpinBox->value());
      }
      catch (FileException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
         return;
      }
      
      DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
      dsv->setAnatomyThresholdValid(false);
      
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
      QApplication::beep();
   }
}
