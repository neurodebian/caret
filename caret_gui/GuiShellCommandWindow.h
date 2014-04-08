
#ifndef __GUI_SHELL_COMMAND_WINDOW_H__
#define __GUI_SHELL_COMMAND_WINDOW_H__

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

#include <QLineEdit>
#include <QStringList>

#include "WuQDialog.h"

class QComboBox;
class QTextEdit;

/// class for line edit with keys intercepted
class GuiShellCommandLineEdit : public QLineEdit {
   Q_OBJECT
   
   public:
      // constructor
      GuiShellCommandLineEdit();
      
      // destructor
      ~GuiShellCommandLineEdit();

   signals:
      /// emitted when up arrow pressed
      void signalUpArrowKeyPressed();
      
      /// emitted when down arrow key pressed
      void signalDownArrowKeyPressed();
      
      /// emitted when control K pressed
      void signalControlKPressed();
      
   protected:
      // called when a key is pressed
      virtual void keyPressEvent(QKeyEvent* event);
};

/// dialog for executing shell commands
class GuiShellCommandWindow : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiShellCommandWindow(QWidget* parent);
      
      // destructor
      ~GuiShellCommandWindow();
      
   protected slots:
      // called when return pressed in command line edit
      void slotCommandLineReturnPressed();
      
      // called when up arrow is pressed in command line edit
      void slotCommandLineUpArrowPressed();
      
      // called when down arrow is pressed in command line edit
      void slotCommandLineDownArrowPressed();
      
   protected:  
      // convert some unix commands to their window's equivalents
      QString convertUnixToWindows(const QString& s) const;
          
      // line edit for entering commands
      GuiShellCommandLineEdit* commandLineEdit;
      
      // combo box for previous commands
      QComboBox* prevCommandsComboBox;
      
      // text edit for command output
      QTextEdit* commandOutputTextEdit;
      
      /// previous commands
      QStringList previousCommands;
      
      /// previous command index
      int previousCommandIndex;
};

#endif // __GUI_SHELL_COMMAND_WINDOW_H__

