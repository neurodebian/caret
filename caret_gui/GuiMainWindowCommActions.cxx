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

#include <QAction>
#include <QMessageBox>

#include "BrainSet.h"
#include "CommunicatorClientAFNI.h"
#include "CommunicatorClientFIV.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowCommActions.h"
#include "GuiSumsDialog.h"

#include "global_variables.h"

/**
 * Constructor
 */
GuiMainWindowCommActions::GuiMainWindowCommActions(GuiMainWindow* parent) :
   QObject(parent)
{
   setObjectName("GuiMainWindowCommActions");
   
   sumsDialog = NULL;
   
   afniConnectAction = new QAction(parent);
   afniConnectAction->setText("Connect to AFNI...");
   afniConnectAction->setObjectName("afniConnectAction");
   QObject::connect(afniConnectAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotAfniConnect()));

   afniDisconnectAction = new QAction(parent);
   afniDisconnectAction->setText("Close AFNI Connection");
   afniDisconnectAction->setObjectName("afniDisconnectAction");
   QObject::connect(afniDisconnectAction, SIGNAL(triggered(bool)),
                    parent->getAfniClientCommunicator(), SLOT(closeConnection()));

   fivConnectAction = new QAction(parent);
   fivConnectAction->setText("Connect to FIV...");
   fivConnectAction->setObjectName("fivConnectAction");
   QObject::connect(fivConnectAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFivConnect()));

   fivDisconnectAction = new QAction(parent);
   fivDisconnectAction->setText("Close FIV Connection");
   fivDisconnectAction->setObjectName("fivDisconnectAction");
   QObject::connect(fivDisconnectAction, SIGNAL(triggered(bool)),
                    parent->getFivClientCommunicator(), SLOT(closeConnection()));

   sumsDatabaseAction = new QAction(parent);
   sumsDatabaseAction->setText("Connect to SumsDB Database...");
   sumsDatabaseAction->setObjectName("sumsDatabaseAction");
   QObject::connect(sumsDatabaseAction, SIGNAL(triggered(bool)),
                    this, SLOT(sumsDatabaseSlot()));
}

/**
 * Destructor
 */
GuiMainWindowCommActions::~GuiMainWindowCommActions()
{
}

/**
 * Called if connect to AFNI is selected.
 */
void
GuiMainWindowCommActions::slotAfniConnect()
{
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   if (tmf->getNumberOfMatrices() == 0) {
      QMessageBox::critical(theMainWindow, "Communication Error", 
                            "There are no transformation matrix files loaded which \n"
                            "are necessary for successful communication with AFNI.");
      return;
   }
   
   theMainWindow->getAfniClientCommunicator()->openConnection();
}

/**
 * Called if connect to FIV is selected.
 */
void
GuiMainWindowCommActions::slotFivConnect()
{
   theMainWindow->getFivClientCommunicator()->openConnection();
}

/**
 * SuMS database.
 */
void 
GuiMainWindowCommActions::sumsDatabaseSlot()
{
   if (sumsDialog == NULL) {
      sumsDialog = new GuiSumsDialog(theMainWindow, theMainWindow->getBrainSet()->getPreferencesFile());
   }
   sumsDialog->show();
   sumsDialog->activateWindow();
}

/**
 * update the actions (typically called when menu is about to show)
 */
void 
GuiMainWindowCommActions::updateActions()
{
}
