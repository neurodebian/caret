
#ifndef __GUI_CARET_COMMAND_SCRIPT_BUILDER_DIALOG_H__
#define __GUI_CARET_COMMAND_SCRIPT_BUILDER_DIALOG_H__

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

#include <vector>

#include "WuQDialog.h"

class CaretScriptFile;
class CommandBase;
class GuiCaretCommandWidget;
class GuiCaretCommandContainerWidget;
class QScrollArea;
class QVBoxLayout;

/// class for constructing and editing caret command scripts
class GuiCaretCommandScriptBuilderDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandScriptBuilderDialog(QWidget* parent,
                                         const QString& caretHomeDirectory);
          
      // destructor
      ~GuiCaretCommandScriptBuilderDialog();
   
   protected slots:
      // called when new button pressed
      void slotNewPushButton();
      
      // called when open button pressed
      void slotOpenPushButton();
      
      // called when run button pressed
      void slotRunPushButton();
      
      // called when save button pressed
      void slotSavePushButton();
      
      // called when close button pressed
      void slotClosePushButton();
      
      // called when help button pressed
      void slotHelpPushButton();
      
      // called to add a new command 
      void slotAddCommand(GuiCaretCommandContainerWidget* addAfterWidget);
      
      // called to delete a command
      void slotDeleteCommand(GuiCaretCommandContainerWidget* deleteWidget);
      
      // called to move a command up one position
      void slotMoveCommandUp(GuiCaretCommandContainerWidget* ccw);
      
      // called to move a command down one position
      void slotMoveCommandDown(GuiCaretCommandContainerWidget* ccw);
      
      // called when data is modified
      void slotDataModified();
      
   protected:
      // add initial command to the dialog
      void addInitialCommand(const QString& commandSwitch,
                             const QString& commandComment,
                             const QStringList& commandParameters);
      
      // delete a container widget
      void deleteContainerWidget(GuiCaretCommandContainerWidget* ccw);
      
      // get the container widgets
      void getContainerWidgets(std::vector<GuiCaretCommandContainerWidget*>& containerWidgetsOut);
      
      // get the command widgets
      void getCommandWidgets(std::vector<GuiCaretCommandWidget*>& commandWidgetsOut);
      
      // place the commands into a script file
      void placeCommandsIntoScriptFile(CaretScriptFile& scriptFile);
      
      // scroll area containing commands
      QScrollArea* commandScrollArea;
      
      /// widget containing the command widgets
      QWidget* commandLayoutWidget;
      
      /// layout for commands
      QVBoxLayout* commandLayout;
      
      /// name of caret command program
      QString caretCommandProgramName;
      
      /// the available caret commands
      std::vector<CommandBase*> caretCommands;
      
      /// widgets that have been removed and need to be deleted
      std::vector<GuiCaretCommandContainerWidget*> widgetsThatNeedToBeDeleted;
      
      /// name of script in dialog
      QString scriptFileName;
      
      /// data in dialog is modified
      bool dataIsModified;
};

/// class for a single operation
class GuiCaretCommandContainerWidget : public QWidget {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandContainerWidget(CommandBase* commandOperationIn,
                                     const bool showDeleteButton = true);
      
      // destructor
      ~GuiCaretCommandContainerWidget();
   
      // get the widget
      GuiCaretCommandWidget* getCommandWidget() { return commandWidget; }
      
      // set the comment and parameters
      void setCommentAndParameters(const QString& commentIn,
                                   const QStringList& parametersIn);
                                   
   signals:
      /// add button was pressed signal
      void signalAddButtonPressed(GuiCaretCommandContainerWidget*);
      
      /// delete button was pressed signal
      void signalDeleteButtonPressed(GuiCaretCommandContainerWidget*);

      /// move up button was pressed signal
      void signalMoveUpButtonPressed(GuiCaretCommandContainerWidget*);
      
      /// move down button was pressed signal
      void signalMoveDownButtonPressed(GuiCaretCommandContainerWidget*);
      
      // emitted when data is modified
      void signalDataModified();

   protected slots:
      // called when add button pressed
      void slotAddPushButton();
      
      // called when delete button pressed
      void slotDeletePushButton();
      
      // called when help button pressed
      void slotHelpPushButton();
      
      // called when move up button pressed
      void slotMoveUpPushButton();
      
      // called when move down button pressed
      void slotMoveDownPushButton();
      
   protected:
      /// the command widget
      GuiCaretCommandWidget* commandWidget;
      
      /// the command's help text
      QString commandHelpText;
};

#endif // __GUI_CARET_COMMAND_SCRIPT_BUILDER_DIALOG_H__
