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
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiUserViewSaveDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiUserViewSaveDialog::GuiUserViewSaveDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Set Surface View");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);

   //
   // Name of view
   //
   QHBoxLayout* nameBoxLayout = new QHBoxLayout;
   dialogLayout->addLayout(nameBoxLayout);
   nameBoxLayout->addWidget(new QLabel("View Name "));
   viewNameLineEdit = new QLineEdit;
   nameBoxLayout->addWidget(viewNameLineEdit);
   
   //
   // Transform component check boxes
   //
   saveRotationCheckBox = new QCheckBox("Save Rotation");
   saveRotationCheckBox->setChecked(true);
   dialogLayout->addWidget(saveRotationCheckBox);
   saveScalingCheckBox = new QCheckBox("Save Scale");
   saveScalingCheckBox->setChecked(true);
   dialogLayout->addWidget(saveScalingCheckBox);
   saveTranslationCheckBox = new QCheckBox("Save Translation");
   saveTranslationCheckBox->setChecked(true);
   dialogLayout->addWidget(saveTranslationCheckBox);
   
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
}

/**
 * Destructor.
 */
GuiUserViewSaveDialog::~GuiUserViewSaveDialog()
{
}

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiUserViewSaveDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
      if (bms != NULL) {
         //
         // Check the name of the view
         //
         const QString viewName(viewNameLineEdit->text());
         if (viewName.isEmpty()) {
            GuiMessageBox::critical(this, "ERROR", "Name of view is missing.", "OK");
            return;
         }
         
         //
         // Get the view transformations
         //
         float matrix[16];
         bms->getRotationMatrix(0, matrix);
         float trans[3];
         bms->getTranslation(0, trans);
         float scale[3];
         bms->getScaling(0, scale);

         //
         // Update the preferences file
         //         
         PreferencesFile::UserView uv(viewName, matrix, trans, scale, 
                     saveRotationCheckBox->isChecked(),
                     saveTranslationCheckBox->isChecked(),
                     saveScalingCheckBox->isChecked());
         PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
         
         pf->addUserView(uv);
         try {
            pf->writeFile(pf->getFileName());
         }
         catch (FileException& /*e*/) {
         }
      }
   }
   QDialog::done(r);
}
