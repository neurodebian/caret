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


#ifndef __GUI_MAIN_WINDOW_COMM_ACTIONS_H__
#define __GUI_MAIN_WINDOW_COMM_ACTIONS_H__

#include <QObject>

class GuiMainWindow;
class GuiSumsDialog;
class QAction;

/// This class creates the Main Window's Comm Actions
class GuiMainWindowCommActions : public QObject {
   Q_OBJECT
   
   public:
      /// constructor
      GuiMainWindowCommActions(GuiMainWindow* parent);
      
      /// destructor
      ~GuiMainWindowCommActions();
      
      /// afni connect action
      QAction* getAfniConnectAction() { return afniConnectAction; }
      
      /// afni disconnect action
      QAction* getAfniDisconnectAction() { return afniDisconnectAction; }
      
      /// fiv connect action
      QAction* getFivConnectAction() { return fivConnectAction; }
      
      /// fiv disconnect action
      QAction* getFivDisconnectAction() { return fivDisconnectAction; }
      
      /// sums database action
      QAction* getSumsDatabaseAction() { return sumsDatabaseAction; }
     
   public slots:
      /// update the actions (typically called when menu is about to show)
      void updateActions();
      
   private slots:
      /// this slot is called if the user chooses to connect to afni
      void slotAfniConnect();
      
      /// this slot is called if the user chooses to connect to fiv
      void slotFivConnect();
      
      /// SuMS database
      void sumsDatabaseSlot();
      
   private:
      /// the SuMS database dialog
      GuiSumsDialog* sumsDialog;
      
      /// afni connect action
      QAction* afniConnectAction;
      
      /// afni disconnect action
      QAction* afniDisconnectAction;
      
      /// fiv connect action
      QAction* fivConnectAction;
      
      /// fiv disconnect action
      QAction* fivDisconnectAction;
      
      /// sums database action
      QAction* sumsDatabaseAction;
      
};

#endif // __GUI_MAIN_WINDOW_COMM_ACTIONS_H__
