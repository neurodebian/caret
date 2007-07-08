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

#include <set>

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QBoxLayout>
#include <QGridLayout>

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiPaintNamesEditDialog.h"
#include "PaintFile.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiPaintNamesEditDialog::GuiPaintNamesEditDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Edit Paint Names");
   
   //
   // Scroll and grid for names widget since there could be many names
   //
   QWidget* nameGridWidget = new QWidget;
   QGridLayout* nameGridLayout = new QGridLayout(nameGridWidget);
   nameGridLayout->setSpacing(3);
   //
   // Column names
   //
   nameGridLayout->addWidget(new QLabel("Old Name"), 0, 0);
   nameGridLayout->addWidget(new QLabel("New Name"), 0, 1);
   
   //
   // Sort the paint names using a set (also removes duplicates)
   //
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int numPaintNames = pf->getNumberOfPaintNames();
   std::set<QString> sortedNames;
   for (int i = 0; i < numPaintNames; i++) {
      sortedNames.insert(pf->getPaintNameFromIndex(i));
   }
   
   //
   // Load the paint names into the dialog
   //
   int cnt = 1;
   for (std::set<QString>::iterator iter = sortedNames.begin();
        iter != sortedNames.end(); iter++) {
      const QString name(*iter);
      nameGridLayout->addWidget(new QLabel(name), cnt, 0);
      
      QLineEdit* lineEdit = new QLineEdit;
      lineEdit->setText(name);
      oldPaintNames.push_back(name);
      paintNameLineEdits.push_back(lineEdit);
      nameGridLayout->addWidget(lineEdit, cnt, 1);
      cnt++;
   }
   
   //
   // Scroll area for paint names
   //
   QScrollArea* sv = new QScrollArea;
   sv->setWidget(nameGridWidget);
   sv->setWidgetResizable(true);

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(3);
   dialogLayout->setSpacing(3);
   dialogLayout->addWidget(sv);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK", this);
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));

   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel", this);
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * Deconstructor.
 */
GuiPaintNamesEditDialog::~GuiPaintNamesEditDialog()
{
}

/**
 * Called when dialog is being closed.
 */
void 
GuiPaintNamesEditDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      //
      // Find names that have changed and update the paint file
      //
      bool nameChanged = false;
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      const int num = static_cast<int>(oldPaintNames.size());
      for (int i = 0; i < num; i++) {
         const QString newName(paintNameLineEdits[i]->text());
         const QString oldName(oldPaintNames[i]);
         if (newName != oldName) {
            const int index = pf->getPaintIndexFromName(oldName);
            if (index >= 0) {
               pf->setPaintName(index, newName);
               nameChanged = true;
            }
         }
      }
      
      //
      // Update if paint names changed
      //
      if (nameChanged) {
         theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
         GuiBrainModelOpenGL::updateAllGL();
         GuiFilesModified fm;
         fm.setPaintModified();
         theMainWindow->fileModificationUpdate(fm);
      }
      
      QApplication::restoreOverrideCursor();
   }
   QDialog::done(r);
}

