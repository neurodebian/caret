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

#include <QGroupBox>
#include <QLayout>
#include <QPushButton>

#include "vtkTransform.h"

#include "BrainModel.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiSetViewDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiSetViewDialog::GuiSetViewDialog(QWidget* parent, 
                                   BrainModel* brainModelIn,
                                   const int viewNumberIn)
   : WuQDialog(parent)
{
   setModal(true);
   creatingDialog = true;
   
   brainModel = brainModelIn;
   viewNumber = viewNumberIn;
   setWindowTitle("Set Surface View");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);

   //
   // Translation
   //
   xTranslateDoubleSpinBox = new QDoubleSpinBox;
   xTranslateDoubleSpinBox->setMinimum(-100000.0);
   xTranslateDoubleSpinBox->setMaximum( 100000.0);
   xTranslateDoubleSpinBox->setSingleStep(5.0);
   xTranslateDoubleSpinBox->setDecimals(1);
   QObject::connect(xTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   yTranslateDoubleSpinBox = new QDoubleSpinBox;
   yTranslateDoubleSpinBox->setMinimum(-100000.0);
   yTranslateDoubleSpinBox->setMaximum( 100000.0);
   yTranslateDoubleSpinBox->setSingleStep(5.0);
   yTranslateDoubleSpinBox->setDecimals(1);
   QObject::connect(yTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   zTranslateDoubleSpinBox = new QDoubleSpinBox;
   zTranslateDoubleSpinBox->setMinimum(-100000.0);
   zTranslateDoubleSpinBox->setMaximum( 100000.0);
   zTranslateDoubleSpinBox->setSingleStep(5.0);
   zTranslateDoubleSpinBox->setDecimals(1);
   QObject::connect(zTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   QGroupBox* translateGroupBox = new QGroupBox("Translate");
   dialogLayout->addWidget(translateGroupBox);
   QHBoxLayout* translateGroupLayout = new QHBoxLayout(translateGroupBox);
   translateGroupLayout->addWidget(xTranslateDoubleSpinBox);
   translateGroupLayout->addWidget(yTranslateDoubleSpinBox);
   translateGroupLayout->addWidget(zTranslateDoubleSpinBox);
                                               
   //
   // Rotation
   //
   xRotateDoubleSpinBox = new QDoubleSpinBox;
   xRotateDoubleSpinBox->setMinimum(-180.0);
   xRotateDoubleSpinBox->setMaximum(180.0);
   xRotateDoubleSpinBox->setSingleStep(5.0);
   xRotateDoubleSpinBox->setDecimals(2);
   QObject::connect(xRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   yRotateDoubleSpinBox = new QDoubleSpinBox;
   yRotateDoubleSpinBox->setMinimum(-180.0);
   yRotateDoubleSpinBox->setMaximum(180.0);
   yRotateDoubleSpinBox->setSingleStep(5.0);
   yRotateDoubleSpinBox->setDecimals(2);
   QObject::connect(yRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   zRotateDoubleSpinBox = new QDoubleSpinBox;
   zRotateDoubleSpinBox->setMinimum(-180.0);
   zRotateDoubleSpinBox->setMaximum(180.0);
   zRotateDoubleSpinBox->setSingleStep(5.0);
   zRotateDoubleSpinBox->setDecimals(2);
   QObject::connect(zRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   QGroupBox* rotateGroupBox = new QGroupBox("Rotate");
   dialogLayout->addWidget(rotateGroupBox);
   QHBoxLayout* rotateGroupLayout = new QHBoxLayout(rotateGroupBox);
   rotateGroupLayout->addWidget(xRotateDoubleSpinBox);
   rotateGroupLayout->addWidget(yRotateDoubleSpinBox);
   rotateGroupLayout->addWidget(zRotateDoubleSpinBox);
                                               
   //
   // Scale
   //
   xScaleDoubleSpinBox = new QDoubleSpinBox;
   xScaleDoubleSpinBox->setMinimum(0.001);
   xScaleDoubleSpinBox->setMaximum(10000.0);
   xScaleDoubleSpinBox->setSingleStep(0.1);
   xScaleDoubleSpinBox->setDecimals(3);
   QObject::connect(xScaleDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   yScaleDoubleSpinBox = new QDoubleSpinBox;
   yScaleDoubleSpinBox->setMinimum(0.001);
   yScaleDoubleSpinBox->setMaximum(10000.0);
   yScaleDoubleSpinBox->setSingleStep(0.1);
   yScaleDoubleSpinBox->setDecimals(3);
   QObject::connect(yScaleDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   zScaleDoubleSpinBox = new QDoubleSpinBox;
   zScaleDoubleSpinBox->setMinimum(0.001);
   zScaleDoubleSpinBox->setMaximum(10000.0);
   zScaleDoubleSpinBox->setSingleStep(0.1);
   zScaleDoubleSpinBox->setDecimals(3);
   QObject::connect(zScaleDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotViewChanged()));
   QGroupBox* scaleGroupBox = new QGroupBox("Scale");
   dialogLayout->addWidget(scaleGroupBox);
   QHBoxLayout* scaleGroupLayout = new QHBoxLayout(scaleGroupBox);
   scaleGroupLayout->addWidget(xScaleDoubleSpinBox);
   scaleGroupLayout->addWidget(yScaleDoubleSpinBox);
   scaleGroupLayout->addWidget(zScaleDoubleSpinBox);
                                               
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
   // Close button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   loadTransformation();
   creatingDialog = false;
}

/**
 * This loads the surface transformations into the dialog.
 */
void
GuiSetViewDialog::loadTransformation()
{
   if (brainModel == NULL) {
      return;
   }
   
   float t[3];
   brainModel->getTranslation(viewNumber, t);
   xTranslateDoubleSpinBox->setValue(t[0]);
   yTranslateDoubleSpinBox->setValue(t[1]);
   zTranslateDoubleSpinBox->setValue(t[2]);
   float r[3];
   vtkTransform* matrix = brainModel->getRotationTransformMatrix(viewNumber);
   matrix->GetOrientation(r);
   xRotateDoubleSpinBox->setValue(r[0]);
   yRotateDoubleSpinBox->setValue(r[1]);
   zRotateDoubleSpinBox->setValue(r[2]);
   float scale[3];
   brainModel->getScaling(viewNumber, scale);
   xScaleDoubleSpinBox->setValue(scale[0]);
   yScaleDoubleSpinBox->setValue(scale[1]);
   zScaleDoubleSpinBox->setValue(scale[2]);
}

/**
 * called when float spin box changed.
 */
void
GuiSetViewDialog::slotViewChanged()
{
   if (creatingDialog) {
      return;
   }
   if (brainModel == NULL) {
      return;
   }
   
   float t[3] = {
      xTranslateDoubleSpinBox->value(),
      yTranslateDoubleSpinBox->value(),
      zTranslateDoubleSpinBox->value()
   };
   brainModel->setTranslation(viewNumber, t);
   
   float r[3] = {
      xRotateDoubleSpinBox->value(),
      yRotateDoubleSpinBox->value(),
      zRotateDoubleSpinBox->value()
   };
   vtkTransform* matrix = brainModel->getRotationTransformMatrix(viewNumber);
   matrix->Identity();
   matrix->RotateZ(r[2]);
   matrix->RotateX(r[0]);
   matrix->RotateY(r[1]);

   brainModel->setScaling(viewNumber, 
                          xScaleDoubleSpinBox->value(),
                          yScaleDoubleSpinBox->value(),
                          zScaleDoubleSpinBox->value());
   if (viewNumber == BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW) {
      theMainWindow->updateTransformationMatrixEditor(NULL);
   }
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Destructor.
 */
GuiSetViewDialog::~GuiSetViewDialog()
{
}

/**
 * Called when OK or Cancel button is pressed.
 */
void 
GuiSetViewDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      slotViewChanged();
   }
   QDialog::done(r);
}
