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
#include <QLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiDeleteBordersByNameDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiDeleteBordersByNameDialog::GuiDeleteBordersByNameDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Delete borders by name");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // List box for border names
   //
   borderListWidget = new QListWidget;
   dialogLayout->addWidget(borderListWidget);
   borderListWidget->setSelectionMode(QListWidget::ExtendedSelection);
   
   //
   // Delete Selected Borders button
   //
   QPushButton* deleteBordersButton = new QPushButton("Delete Selected Borders");
   dialogLayout->addWidget(deleteBordersButton);
   deleteBordersButton->setFixedSize(deleteBordersButton->sizeHint());
   deleteBordersButton->setAutoDefault(false);
   QObject::connect(deleteBordersButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteButton()));
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   //
   // close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setFixedSize(closeButton->sizeHint());
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   loadBorderNameListBox();
}

/**
 * Destructor
 */
GuiDeleteBordersByNameDialog::~GuiDeleteBordersByNameDialog()
{
}

/**
 * Load the border list box
 */
void
GuiDeleteBordersByNameDialog::loadBorderNameListBox()
{
   borderListWidget->clear();
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->getAllBorderNames(borderNames);
   
   const int numNames = static_cast<int>(borderNames.size());
   for (int i = 0; i < numNames; i++) {
      borderListWidget->addItem(borderNames[i]);
   }
}

/**
 *
 */
void
GuiDeleteBordersByNameDialog::slotDeleteButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   std::vector<QString> bordersToDelete;
   const int numNames = static_cast<int>(borderNames.size());
   for (int i = 0; i < numNames; i++) {
      if (borderListWidget->isItemSelected(borderListWidget->item(i))) {
         bordersToDelete.push_back(borderNames[i]);
      }
   }
   
   if (bordersToDelete.empty() == false) {
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->deleteBordersWithNames(bordersToDelete);
   }
   
   loadBorderNameListBox();
   
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
   
   QApplication::restoreOverrideCursor();
}

