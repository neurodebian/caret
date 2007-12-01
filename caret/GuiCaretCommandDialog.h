
#ifndef __GUI_CARET_COMMAND_DIALOG_H__
#define __GUI_CARET_COMMAND_DIALOG_H__

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

#include "QtDialogNonModal.h"

#include "ScriptBuilderParameters.h"

class CommandBase;
class GuiCaretCommandWidget;
class ProgramParameters;
class QListWidget;
class QRadioButton;
class QScrollArea;
class QStackedWidget;
class QTextEdit;

//=============================================================================
/// dialog for constructing a caret command operation
class GuiCaretCommandDialog : public QtDialog {
   Q_OBJECT
   
   public:
      // mode of dialog
      enum DIALOG_MODE {
         /// for command selection, modal, "exec()" to run
         DIALOG_MODE_COMMAND_SELECTOR,
         /// for command selection, modal, "exec()" to run with before after
         DIALOG_MODE_COMMAND_SELECTOR_WITH_BEFORE_AFTER,
         /// for executing commands, non-modal, "show()" to run
         DIALOG_MODE_EXECUTOR_NON_MODAL
      };
      
      // constructor
      GuiCaretCommandDialog(QWidget* parent,
                            const QString& caretHomeDirectory,
                            const DIALOG_MODE dialogModeIn);
        
      // destructor
      ~GuiCaretCommandDialog();
      
      // get the selected command (if command selector mode)
      QString getSelectedCommandSwitch() const;
      
      // get add after button selected (if before/after)
      bool getAddAfterWidget() const;
      
      // initialize to command
      void setSelectedCommand(const QString& commandSwitch);
      
   protected slots:
      // called when process command button is pressed
      void slotProcessCommandButton();

      // called when a command is selected
      void slotCommandListWidgetSelection(int item);
      
   protected:
      // create the command parameters widget
      GuiCaretCommandWidget* createCommandParametersWidget(CommandBase* command);
                               
      /// caret command executable
      QString caretCommandProgramName;
      
      /// operations list box
      QListWidget* commandsListWidget;
      
      /// command description text edit
      QTextEdit* commandDescriptionTextEdit;
      
      /// stacked widget for command parameters
      QStackedWidget* commandParametersStackedWidget;
      
      /// the currently selected command
      CommandBase* selectedCommand;
      
      /// needed for initializing commands
      ProgramParameters* params;
      
      /// mode of dialog
      DIALOG_MODE dialogMode;
      
      /// scroll area for parameters
      QScrollArea* commandParametersScrollArea;
      
      /// the caret commands
      std::vector<CommandBase*> caretCommands;
      
      /// the widgets for each command
      std::vector<GuiCaretCommandWidget*> caretCommandWidgets;

      /// add after radio button
      QRadioButton* addAfterRadioButton;
      
      /// add before radio button
      QRadioButton* addBeforeRadioButton;
};

#endif // __GUI_CARET_COMMAND_DIALOG_H__
