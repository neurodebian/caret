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

#include <QGridLayout>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>

#include "BrainSet.h"
#include "FileFilters.h"
#include "GuiFileSelectionButton.h"
#include "GuiSpecAndSceneFileCreationDialog.h"

/**
 * constructor.
 */
GuiSpecAndSceneFileCreationDialog::GuiSpecAndSceneFileCreationDialog(QWidget* parent,
                                                       BrainSet* brainSetIn,
                                                       const std::vector<int>& sceneIndicesIn)
   : QtDialogModal(parent)
{
   setWindowTitle("Create Spec and Scene File");
   
   brainSet = brainSetIn;
   sceneIndices = sceneIndicesIn;
   
   //
   // Button and line edit for spec file
   //
   GuiFileSelectionButton* specFileButton = new GuiFileSelectionButton(0,
                                                                       "Spec File...",
                                                                       FileFilters::getSpecFileFilter(),
                                                                       false);
   specFileNameLineEdit = new QLineEdit;
   QObject::connect(specFileButton, SIGNAL(fileSelected(const QString&)),
                    specFileNameLineEdit, SLOT(setText(const QString&)));
                    
   //
   // Button and line edit for scene file
   //
   GuiFileSelectionButton* sceneFileButton = new GuiFileSelectionButton(0,
                                                                       "Scene File...",
                                                                       FileFilters::getSceneFileFilter(),
                                                                       false);
   sceneFileNameLineEdit = new QLineEdit;
   QObject::connect(sceneFileButton, SIGNAL(fileSelected(const QString&)),
                    sceneFileNameLineEdit, SLOT(setText(const QString&)));
      
   //
   // layout the selection controls
   //
   QGridLayout* fileGridLayout = new QGridLayout;
   fileGridLayout->addWidget(specFileButton, 0, 0);
   fileGridLayout->addWidget(specFileNameLineEdit, 0, 1);
   fileGridLayout->addWidget(sceneFileButton, 1, 0);
   fileGridLayout->addWidget(sceneFileNameLineEdit, 1, 1);

   //
   // Get the dialog's layout and add to it
   //
   QVBoxLayout* dialogLayout = getDialogLayout();
   dialogLayout->addLayout(fileGridLayout);
}
                                  
/**
 * destructor.
 */
GuiSpecAndSceneFileCreationDialog::~GuiSpecAndSceneFileCreationDialog()
{
}

/**
 * called when OK/Cancel button pressed.
 */
void 
GuiSpecAndSceneFileCreationDialog::done(int r)
{
   //
   // Get name of spec file
   //
   QString specFileName = specFileNameLineEdit->text();
   if (specFileName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "You must enter the name for the Spec File.");
      return;
   }

   //
   // Get name of scene file
   //
   QString sceneFileName = sceneFileNameLineEdit->text();
   if (sceneFileName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "You must enter the name for the Scene File.");
      return;
   }

   //
   // Create the spec file
   //
   QString errorMessage;
   brainSet->createSpecFromScenes(sceneIndices,
                                  specFileName,
                                  sceneFileName,
                                  errorMessage);
    
   //
   // Was there an error ?
   //
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this, "ERROR", errorMessage);
      return;
   }
   
   QtDialogModal::done(r);
}
