
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

#include <QDialogButtonBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiMainWindow.h"
#include "GuiModelSelectionControl.h"
#include "GuiModelsEditorDialog.h"
#include "VtkModelFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiModelsEditorDialog::GuiModelsEditorDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setObjectName("GuiModelsEditor");
   setWindowTitle("Models Editor");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   
   //
   // model control
   //
   QLabel* modelLabel = new QLabel("Model: ", this);
   modelControl = new GuiModelSelectionControl;
   
   //
   // Color push button
   //
   QPushButton* colorPushButton = new QPushButton("Set Color...",
                                                  this);
   colorPushButton->setObjectName("colorPushButton");
   colorPushButton->setAutoDefault(false);
   colorPushButton->setFixedSize(colorPushButton->sizeHint());
   QObject::connect(colorPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetColorButton()));
                    
   //
   // Layout items
   //
   QHBoxLayout* modelLayout = new QHBoxLayout;
   dialogLayout->addLayout(modelLayout);
   modelLayout->setObjectName("modelLayout");
   modelLayout->addWidget(modelLabel);
   modelLayout->addWidget(modelControl);
   dialogLayout->addWidget(colorPushButton);
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
   buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(false);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));                    
}

/**
 * destructor.
 */
GuiModelsEditorDialog::~GuiModelsEditorDialog()
{
}

/**
 * update the dialog.
 */
void 
GuiModelsEditorDialog::updateDialog()
{
   modelControl->updateControl();
}

/**
 * called when set color button pressed.
 */
void 
GuiModelsEditorDialog::slotSetColorButton()
{
   VtkModelFile* vmf = modelControl->getSelectedModel();
   if (vmf != NULL) {
      GuiColorSelectionDialog csd(this,
                                  "Set VTK Model Color",
                                  true,
                                  false,
                                  false,
                                  false);
      if (csd.exec() == QDialog::Accepted) {
         unsigned char rgba[4];
         float pointSize, lineSize;
         ColorFile::ColorStorage::SYMBOL symbol;
         csd.getColorInformation(rgba[0], rgba[1], rgba[2], rgba[3],
                                 pointSize, lineSize, symbol);
         vmf->setToSolidColor(rgba);
         GuiBrainModelOpenGL::updateAllGL();
      }
   }
}
