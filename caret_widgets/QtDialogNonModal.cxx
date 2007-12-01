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

#include <QLayout>
#include <QPushButton>

#include "QtDialogNonModal.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
QtDialogNonModal::QtDialogNonModal(QWidget* parent, 
                                   Qt::WFlags f)
   : QtDialog(parent, false, f)
{
   //
   // Layout for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout;
   dialogLayout->setMargin(3);
   dialogLayout->setSpacing(3);
   setLayout(dialogLayout);
   
   //
   // Layout for user's items
   //
   usersLayout = new QVBoxLayout;
   usersLayout->setSpacing(3);
   dialogLayout->addLayout(usersLayout);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(3);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   applyButton = new QPushButton("Apply");
   buttonsLayout->addWidget(applyButton);
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SIGNAL(signalApplyButtonPressed()));

   //
   // Close button
   //
   closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SIGNAL(signalCloseButtonPressed()));
                    
   //
   // Help button
   //
   helpButton = new QPushButton("Help");
   buttonsLayout->addWidget(helpButton);
   helpButton->setAutoDefault(false);
   QObject::connect(helpButton, SIGNAL(clicked()),
                    this, SIGNAL(signalHelpButtonPressed()));
   helpButton->hide();
   
   resizeButtons();
}
              
/**
 * destructor.
 */
QtDialogNonModal::~QtDialogNonModal()
{
}

/**
 * resize the buttons.
 */
void 
QtDialogNonModal::resizeButtons()
{
   //
   // Make buttons same size
   //
   QtUtilities::makeButtonsSameSize(applyButton, closeButton, helpButton);
}
      
/**
 * show the help button.
 */
void 
QtDialogNonModal::showHelpButton(const bool showIt) 
{ 
   helpButton->setVisible(showIt); 
}
      
