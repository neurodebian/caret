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

#include <iostream>

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "DisplaySettingsVolume.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiToolBar.h"
#include "GuiVolumeResizingDialog.h"
#include "ParamsFile.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumeResizingDialog::GuiVolumeResizingDialog(QWidget* parent)
   : WuQDialog(parent)
{   
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   int slices[6];
   slices[0] = 0;
   slices[1] = 0;
   slices[2] = 0;
   slices[3] = 0;
   slices[4] = 0;
   slices[5] = 0;
   dsv->setCroppingSlices(slices);
   dsv->setCroppingSlicesValid(false);
   updatingDialog = true;
   
   setWindowTitle("Resize Volume");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Group box for labels, sliders, and slider values
   //
   QGroupBox* boundsGroupBox = new QGroupBox("Bounds");
   dialogLayout->addWidget(boundsGroupBox);
   QVBoxLayout* boundsGroupLayout = new QVBoxLayout(boundsGroupBox);
   
   //
   // Group box for labels, sliders, and slider values
   //
   QGridLayout* gridLayout = new QGridLayout;
   boundsGroupLayout->addLayout(gridLayout);
   
   const int minSlice = -100000;
   const int maxSlice =  100000;
   //
   // X Minimum slider
   //
   gridLayout->addWidget(new QLabel("X Min "), 0, 0);
   xMinSpinBox = new QSpinBox;
   xMinSpinBox->setMinimum(minSlice);
   xMinSpinBox->setMaximum(maxSlice);
   xMinSpinBox->setSingleStep(1);
   QObject::connect(xMinSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotXMinSpinBox(int)));
   gridLayout->addWidget(xMinSpinBox, 0, 1);
   
   //
   // X Maximum slider
   //
   gridLayout->addWidget(new QLabel("X Max "), 1, 0);
   xMaxSpinBox = new QSpinBox;
   xMaxSpinBox->setMinimum(minSlice);
   xMaxSpinBox->setMaximum(maxSlice);
   xMaxSpinBox->setSingleStep(1);
   QObject::connect(xMaxSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotXMaxSpinBox(int)));
   gridLayout->addWidget(xMaxSpinBox, 1, 1);
   
   //
   // Y Minimum slider
   //
   gridLayout->addWidget(new QLabel("Y Min "), 2, 0);
   yMinSpinBox = new QSpinBox;
   yMinSpinBox->setMinimum(minSlice);
   yMinSpinBox->setMaximum(maxSlice);
   yMinSpinBox->setSingleStep(1);
   QObject::connect(yMinSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotYMinSpinBox(int)));
   gridLayout->addWidget(yMinSpinBox, 2, 1);
   
   //
   // Y Maximum slider
   //
   gridLayout->addWidget(new QLabel("Y Max "), 3, 0);
   yMaxSpinBox = new QSpinBox;
   yMaxSpinBox->setMinimum(minSlice);
   yMaxSpinBox->setMaximum(maxSlice);
   yMaxSpinBox->setSingleStep(1);
   QObject::connect(yMaxSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotYMaxSpinBox(int)));
   gridLayout->addWidget(yMaxSpinBox, 3, 1);
   
   //
   // Z Minimum slider
   //
   gridLayout->addWidget(new QLabel("Z Min "), 4, 0);
   zMinSpinBox = new QSpinBox;
   zMinSpinBox->setMinimum(minSlice);
   zMinSpinBox->setMaximum(maxSlice);
   zMinSpinBox->setSingleStep(1);
   QObject::connect(zMinSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotZMinSpinBox(int)));
   gridLayout->addWidget(zMinSpinBox, 4, 1);
   
   //
   // Z Maximum slider
   //
   gridLayout->addWidget(new QLabel("Z Max "), 5, 0);
   zMaxSpinBox = new QSpinBox;
   zMaxSpinBox->setMinimum(minSlice);
   zMaxSpinBox->setMaximum(maxSlice);
   zMaxSpinBox->setSingleStep(1);
   QObject::connect(zMaxSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotZMaxSpinBox(int)));
   gridLayout->addWidget(zMaxSpinBox, 5, 1);
   
   boundsGroupLayout->addWidget(new QLabel(
      "\n"
      "To make the volume larger, increase the \"max\" values\n"
      "or decrease the \"min\" values (make them negative).\n"
      "Any new voxels are set to zero."));
      
   //
   // Parameters group box
   //
   QGroupBox* paramsGroupBox = new QGroupBox("Parameters File Update");
   dialogLayout->addWidget(paramsGroupBox);
   QVBoxLayout* paramsGroupLayout = new QVBoxLayout(paramsGroupBox);
   
   //
   // x/y/z min check box
   //
   useXyzMinForParametersCheckBox = new QCheckBox("Set Parameters File X/Y/Z Min");
   useXyzMinForParametersCheckBox->setChecked(true);
   paramsGroupLayout->addWidget(useXyzMinForParametersCheckBox);
   
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
                    this, SLOT(slotOKButton()));
    
   //
   // Reset button
   //
   QPushButton* resetButton = new QPushButton("Reset");
   resetButton->setAutoDefault(false);
   buttonsLayout->addWidget(resetButton);
   QObject::connect(resetButton, SIGNAL(clicked()),
                    this, SLOT(slotResetPushButton()));
                    
   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   QtUtilities::makeButtonsSameSize(okButton, resetButton, cancelButton);
}

/**
 * Destructor.
 */
GuiVolumeResizingDialog::~GuiVolumeResizingDialog()
{
}

/**
 * Override to show dialog
 */
void
GuiVolumeResizingDialog::show()
{
   updateDialog(false);
   WuQDialog::show();
}

/**
 * Called when dialog is being closed.
 */
void
GuiVolumeResizingDialog::close()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setCroppingSlicesValid(false);
   GuiBrainModelOpenGL::updateAllGL();
   QDialog::close();
}

/**
 * Reset the cropping lines.
 */
void
GuiVolumeResizingDialog::slotResetPushButton()
{
   updateDialog(false);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Update the dialog.
 */
void
GuiVolumeResizingDialog::updateDialog(const bool fileChangeUpdateFlag)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   updatingDialog = true;
   
   //
   // Make sure there is still an underlay volume displayed in the main window 
   //
   bool haveValidVolume = false;
   dsv->setCroppingSlicesValid(false);
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv != NULL) {
      VolumeFile* vf = bmv->getUnderlayVolumeFile();
      if (vf != NULL) {
         haveValidVolume = true;
         if (fileChangeUpdateFlag == false) {
            dsv->setCroppingSlicesValid(true);
         }
         
         //
         // Initialize the valid slices
         //
         int dim[3];
         vf->getDimensions(dim);
         int slices[6];
         slices[0] = 0;
         slices[1] = dim[0] - 1;
         slices[2] = 0;
         slices[3] = dim[1] - 1;
         slices[4] = 0;
         slices[5] = dim[2] - 1;
         dsv->setCroppingSlices(slices);
         
         //
         // Set the spin box min & max value
         //
         //xMinSpinBox->setMaxValue(dim[0] - 1);
         //xMaxSpinBox->setMaxValue(dim[0] - 1);
         //yMinSpinBox->setMaxValue(dim[1] - 1);
         //yMaxSpinBox->setMaxValue(dim[1] - 1);
         //zMinSpinBox->setMaxValue(dim[2] - 1);
         //zMaxSpinBox->setMaxValue(dim[2] - 1);
         
         //
         // Update with values from paramaters files
         //
         ParamsFile* paramsFile = theMainWindow->getBrainSet()->getParamsFile();
         if (paramsFile != NULL) {
            QString cropValue;
            if (paramsFile->getParameter(ParamsFile::keyCropped, cropValue)) {
               if (cropValue == ParamsFile::keyValueYes) {
                  int slicesTemp[6];
                  if (paramsFile->getParameter(ParamsFile::keyCropMinX, slicesTemp[0]) &&
                      paramsFile->getParameter(ParamsFile::keyCropMaxX, slicesTemp[1]) &&
                      paramsFile->getParameter(ParamsFile::keyCropMinY, slicesTemp[2]) &&
                      paramsFile->getParameter(ParamsFile::keyCropMaxY, slicesTemp[3]) &&
                      paramsFile->getParameter(ParamsFile::keyCropMinZ, slicesTemp[4]) &&
                      paramsFile->getParameter(ParamsFile::keyCropMaxZ, slicesTemp[5])) {
                     slices[0] = slicesTemp[0];
                     slices[1] = slicesTemp[1];
                     slices[2] = slicesTemp[2];
                     slices[3] = slicesTemp[3];
                     slices[4] = slicesTemp[4];
                     slices[5] = slicesTemp[5];
                  }
               }
            }
         }
         
         //
         // Set the spin boxes current values
         //
         xMinSpinBox->setValue(slices[0]);
         xMaxSpinBox->setValue(slices[1]);
         yMinSpinBox->setValue(slices[2]);
         yMaxSpinBox->setValue(slices[3]);
         zMinSpinBox->setValue(slices[4]);
         zMaxSpinBox->setValue(slices[5]);
      }
   }
   
   if (haveValidVolume == false) {
      //
      // Close the dialog
      //
      close();
   }
   
   updatingDialog = false;
}

/**
 * Called when OK button pressed.
 */
void 
GuiVolumeResizingDialog::slotOKButton()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   
   //
   // Go ahead and crop the underlay volume.
   //
   if (dsv->getCroppingSlicesValid()) {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         VolumeFile* vf = bmv->getUnderlayVolumeFile();
         if (vf != NULL) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            const int slices[6] = {
               xMinSpinBox->value(),
               xMaxSpinBox->value(),
               yMinSpinBox->value(),
               yMaxSpinBox->value(),
               zMinSpinBox->value(),
               zMaxSpinBox->value()
            };
            dsv->setCroppingSlices(slices);
            ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
            vf->resize(slices, pf);
            
            if (useXyzMinForParametersCheckBox->isChecked()) {
               pf->setParameter(ParamsFile::keyXmin, xMinSpinBox->value());
               pf->setParameter(ParamsFile::keyYmin, yMinSpinBox->value());
               pf->setParameter(ParamsFile::keyZmin, zMinSpinBox->value());
               try {
                  theMainWindow->getBrainSet()->writeParamsFile(pf->getFileName());
               }
               catch (FileException& e) {
                  QString msg("Unable to write parameters file: ");
                  msg.append(e.whatQString());
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "ERROR", msg);
               }
               
               GuiFilesModified fm;
               fm.setParameterModified();
               theMainWindow->fileModificationUpdate(fm);
            }
            
            bmv->resetViewingTransform(0);
            GuiToolBar::updateAllToolBars(false);
            GuiBrainModelOpenGL::updateAllGL();
            QApplication::restoreOverrideCursor();
         }
      }
      close();
   }
}

/**
 * called when x minimum spin box value changed.
 */
void 
GuiVolumeResizingDialog::slotXMinSpinBox(int value)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   int slices[6];
   dsv->getCroppingSlices(slices);
   slices[0] = value;
   dsv->setCroppingSlices(slices);
   
   if (updatingDialog == false) {
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when x maximum spin box value changed.
 */
void 
GuiVolumeResizingDialog::slotXMaxSpinBox(int value)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   int slices[6];
   dsv->getCroppingSlices(slices);
   slices[1] = value;
   dsv->setCroppingSlices(slices);

   if (updatingDialog == false) {
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when y minimum spin box value changed.
 */
void 
GuiVolumeResizingDialog::slotYMinSpinBox(int value)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   int slices[6];
   dsv->getCroppingSlices(slices);
   slices[2] = value;
   dsv->setCroppingSlices(slices);
   
   if (updatingDialog == false) {
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when y maximum spin box value changed.
 */
void 
GuiVolumeResizingDialog::slotYMaxSpinBox(int value)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   int slices[6];
   dsv->getCroppingSlices(slices);
   slices[3] = value;
   dsv->setCroppingSlices(slices);

   if (updatingDialog == false) {
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when z minimum spin box value changed.
 */
void 
GuiVolumeResizingDialog::slotZMinSpinBox(int value)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   int slices[6];
   dsv->getCroppingSlices(slices);
   slices[4] = value;
   dsv->setCroppingSlices(slices);

   if (updatingDialog == false) {
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when z maximum spin box value changed.
 */
void 
GuiVolumeResizingDialog::slotZMaxSpinBox(int value)
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   int slices[6];
   dsv->getCroppingSlices(slices);
   slices[5] = value;
   dsv->setCroppingSlices(slices);

   if (updatingDialog == false) {
      GuiBrainModelOpenGL::updateAllGL();
   }
}
      

