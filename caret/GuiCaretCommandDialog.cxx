
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

#include <cstdlib>
#include <iostream>

#include <QApplication>
#include <QButtonGroup>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

#include "BrainModelRunExternalProgram.h"
#include "CommandBase.h"
#include "DebugControl.h"
#include "GuiCaretCommandWidget.h"
#include "GuiCaretCommandDialog.h"
#include "ProgramParameters.h"
#include "QtTextEditDialog.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
GuiCaretCommandDialog::GuiCaretCommandDialog(QWidget* parent,
                                            const QString& /*caretHomeDirectory*/,
                                            const DIALOG_MODE dialogModeIn)
   : WuQDialog(parent)
{
   dialogMode = dialogModeIn;
   
   selectedCommand = NULL;
 
   switch (dialogMode) {
      case DIALOG_MODE_COMMAND_SELECTOR:
      case DIALOG_MODE_COMMAND_SELECTOR_WITH_BEFORE_AFTER:
         setWindowTitle("Caret Command Selector");
         break;
      case DIALOG_MODE_EXECUTOR_NON_MODAL:
         setWindowTitle("Caret Command Executor");
         break;
   }

   //
   // Set path/name of Caret program
   // 
   caretCommandProgramName = "";
/*
   if (caretHomeDirectory.isEmpty() == false) {
      caretCommandProgramName = caretHomeDirectory;
      caretCommandProgramName.append("/");
      caretCommandProgramName.append("bin");
      caretCommandProgramName.append("/");
   }  
*/
   caretCommandProgramName.append("caret_command");
   
   //
   // List box for commands
   //
   commandsListWidget = new QListWidget;
   QObject::connect(commandsListWidget, SIGNAL(currentRowChanged(int)),
                    this, SLOT(slotCommandListWidgetSelection(int)));
   commandsListWidget->setMinimumHeight(50);

   //
   // commands group box and layout
   //
   QGroupBox* commandsGroupBox = new QGroupBox("Commands");
   QVBoxLayout* commandsLayout = new QVBoxLayout(commandsGroupBox);
   commandsLayout->addWidget(commandsListWidget);
   
   //
   // Group box for command description section
   //
   commandDescriptionTextEdit = new QTextEdit;
   commandDescriptionTextEdit->setLineWrapMode(QTextEdit::NoWrap);
   QGroupBox* commandDescriptionGroupBox = new QGroupBox("Command Description");
   QVBoxLayout* commandDescriptionLayout = new QVBoxLayout(commandDescriptionGroupBox);
   commandDescriptionLayout->addWidget(commandDescriptionTextEdit);

   // 
   // Stacked widget for commands
   //
   commandParametersStackedWidget = new QStackedWidget;
   commandParametersScrollArea = new QScrollArea;
   commandParametersScrollArea->setWidget(commandParametersStackedWidget);
   commandParametersScrollArea->setWidgetResizable(true);
   
   //
   // command/help row
   //
   QSplitter* commandHelpSplitter = new QSplitter;
   commandHelpSplitter->setOrientation(Qt::Horizontal);
   commandHelpSplitter->addWidget(commandsGroupBox);
   commandHelpSplitter->addWidget(commandDescriptionGroupBox);   

   //
   // all widgets in dialog
   //
   QWidget* dialogWidgets = NULL;
   addAfterRadioButton = NULL;
   addBeforeRadioButton = NULL;
   switch (dialogMode) {
      case DIALOG_MODE_COMMAND_SELECTOR:
         dialogWidgets = commandHelpSplitter;
         break;
      case DIALOG_MODE_COMMAND_SELECTOR_WITH_BEFORE_AFTER:
         {
            //
            // Add after/before current operation radio buttons
            //
            addAfterRadioButton  = new QRadioButton("Add After Current Operation");
            addBeforeRadioButton = new QRadioButton("Add Before Current Operation");
            
            //
            // Keep radio buttons mutually exclusive
            //
            QButtonGroup* buttGroup = new QButtonGroup(this);
            buttGroup->addButton(addAfterRadioButton);
            buttGroup->addButton(addBeforeRadioButton);
            addAfterRadioButton->setChecked(true);
            
            //
            // Place radio buttons into group box
            //
            QGroupBox* beforeAfterGroupBox = new QGroupBox("Add New Operation Location");
            QVBoxLayout* beforeAfterLayout = new QVBoxLayout(beforeAfterGroupBox);
            beforeAfterLayout->addWidget(addAfterRadioButton);
            beforeAfterLayout->addWidget(addBeforeRadioButton);
            beforeAfterGroupBox->setFixedSize(beforeAfterGroupBox->sizeHint());
            
            //
            // Put group box below operation selection
            //
            dialogWidgets = new QWidget;
            QVBoxLayout* layout = new QVBoxLayout(dialogWidgets);
            layout->addWidget(commandHelpSplitter);
            layout->addWidget(beforeAfterGroupBox);
            layout->addStretch();
         }
         break;
      case DIALOG_MODE_EXECUTOR_NON_MODAL:
         //
         // Keep parameters visible
         //
         commandParametersScrollArea->setMinimumHeight(250);

         //
         // Splitter for top and bottom halves
         //
         QSplitter* horizontalSplitter = new QSplitter;
         horizontalSplitter->setOrientation(Qt::Vertical);
         horizontalSplitter->addWidget(commandHelpSplitter);
         horizontalSplitter->addWidget(commandParametersScrollArea);
         dialogWidgets = horizontalSplitter;         
         break;
   }
   
   //
   // buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   switch (dialogMode) {
      case DIALOG_MODE_COMMAND_SELECTOR:
      case DIALOG_MODE_COMMAND_SELECTOR_WITH_BEFORE_AFTER:
         {
            //
            //
            // OK push button
            //
            QPushButton* okPushButton = new QPushButton("OK");
            okPushButton->setAutoDefault(false);
            QObject::connect(okPushButton, SIGNAL(clicked()),
                             this, SLOT(accept()));
                             
            //
            // Cancel push button
            //
            QPushButton* cancelPushButton = new QPushButton("Cancel");
            cancelPushButton->setAutoDefault(false);
            QObject::connect(cancelPushButton, SIGNAL(clicked()),
                             this, SLOT(reject()));
                            
            buttonsLayout->addWidget(okPushButton);
            buttonsLayout->addWidget(cancelPushButton);
            QtUtilities::makeButtonsSameSize(okPushButton,
                                             cancelPushButton);
         }
         break;
      case DIALOG_MODE_EXECUTOR_NON_MODAL:
         {
            //
            // Execute push button
            //
            QPushButton* executePushButton = new QPushButton("Execute Command");
            executePushButton->setAutoDefault(false);
            QObject::connect(executePushButton, SIGNAL(clicked()),
                             this, SLOT(slotProcessCommandButton()));
                             
            //
            // Close push button
            //
            QPushButton* closePushButton = new QPushButton("Close");
            closePushButton->setAutoDefault(false);
            QObject::connect(closePushButton, SIGNAL(clicked()),
                             this, SLOT(close()));
                            
            buttonsLayout->addWidget(executePushButton);
            buttonsLayout->addWidget(closePushButton);
            QtUtilities::makeButtonsSameSize(executePushButton,
                                             closePushButton);
         }
         break;
   }
   
   //
   // layout the widgets
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(dialogWidgets);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Need to prevent list widget from emitting signals until all is set up
   //
   commandsListWidget->blockSignals(true);

   char* argv[] = { "caret_command", "" };
   params =  new ProgramParameters(1, argv);

   //
   // Get commands and initialize command widgets
   //
   CommandBase::getAllCommandsSortedByDescription(caretCommands);
   const int numCommands = static_cast<int>(caretCommands.size());
   caretCommandWidgets.resize(numCommands, NULL);
   for (int i = 0; i < numCommands; i++) {
      //
      // Get help information
      //
      caretCommands[i]->setParameters(params);
      commandsListWidget->addItem(caretCommands[i]->getShortDescription());
   
      //
      // Initialize widget for commands parameters and add to 
      // parameters stacked widget
      //
      commandParametersStackedWidget->addWidget(new QWidget);
   }
   
   //
   // OK for signals
   //
   commandsListWidget->blockSignals(false);
   
   resize(800, 800);
}

/**
 * destructor.
 */
GuiCaretCommandDialog::~GuiCaretCommandDialog()
{
   if (params != NULL) delete params;
}

/**
 * initialize to command.
 */
void 
GuiCaretCommandDialog::setSelectedCommand(const QString& commandSwitch)
{
   const int numCommands = static_cast<int>(caretCommands.size());
   for (int i = 0; i < numCommands; i++) {
      if (caretCommands[i]->getOperationSwitch() == commandSwitch) {
         commandsListWidget->setCurrentRow(i);
         slotCommandListWidgetSelection(i);
         break;
      }
   }
}

/**
 * get add after button selected (if before/after).
 */
bool 
GuiCaretCommandDialog::getAddAfterWidget() const
{
   bool addAfterFlag = false;
   if (addAfterRadioButton != NULL) {
      addAfterFlag = addAfterRadioButton->isChecked();
   }
   return addAfterFlag;
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiCaretCommandDialog::slotProcessCommandButton()
{
   GuiCaretCommandWidget* guiCommandWidget = 
      dynamic_cast<GuiCaretCommandWidget*>(commandParametersStackedWidget->currentWidget());
   
   if (guiCommandWidget != NULL) {
      QString errorMessage;
      QString commandSwitch;
      QString commandShortDescription;
      QStringList commandParameters;
      guiCommandWidget->getCommandLineForCommandExecution(commandSwitch,
                                       commandParameters,
                                       commandShortDescription,
                                       errorMessage);
      if (errorMessage.isEmpty() == false) {
         QMessageBox::critical(this,
                                 "ERROR",
                                 errorMessage);
      }
      else {
         commandParameters.insert(0, commandSwitch);
         const QString cmdText = caretCommandProgramName
                                + " "
                                + commandParameters.join(" ");
        
         const QString comment(guiCommandWidget->getComment());
         
         if (DebugControl::getDebugOn()) {
            std::cout << "Comment: " 
                     << comment.toAscii().constData()
                     << std::endl;
                     
            std::cout << "Command: \"\""
                     << cmdText.toAscii().constData()
                     << "\"\""
                     << std::endl;
         }
                  
         //
         // Determine operations based upon mode
         //
         bool executeCommandFlag = true;
        
         //
         // Execute command ?
         //
         if (executeCommandFlag) {
            //
            // Too much text output may cause problems
            //
            const QString envVarName("CARET_DEBUG");
            if (std::getenv(envVarName.toAscii().constData()) != NULL) {
               const QString msg =
                  ("WARNING: The environment variable "
                   + envVarName
                   + " is on.\n"
                   + "This may cause the caret_command program to hang if\n"
                   + "there is too much printed output.");
               if (QMessageBox::question(this, 
                                         "WARNING", 
                                         msg, 
                                         (QMessageBox::Ok | 
                                          QMessageBox::Cancel)) 
                                                           != QMessageBox::Ok) {
                  return;
               }
            }
           
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            
            QString txt;
            if (DebugControl::getDebugOn()) {
               txt.append("Command \"");
               txt.append(caretCommandProgramName);
               if (commandParameters.isEmpty() == false) {
                  txt.append(" ");
                  txt.append(commandParameters.join(" "));
               }
               txt.append("\"\n\n");
            }
            
            //
            // Run the program
            //
            BrainModelRunExternalProgram runProgram(caretCommandProgramName,
                                                    commandParameters,
                                                    true);
            try {
               runProgram.execute();
            }
            catch (BrainModelAlgorithmException& /*e*/) {
            }
            txt.append(runProgram.getOutputText());

/*
            //
            // Create a new QProcess and add its arguments
            //
            QProcess process(this);
            
            //
            // Start execution of the command
            //
            process.start(caretCommandProgramName, commandParameters);
            if (!process.waitForStarted()) {
               QString msg("Error starting command: ");
               msg.append(caretCommandProgramName);
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", msg);
               return;
            }
            
            //
            // Wait until the program is complete
            //
            if (!process.waitForFinished(100000000)) {
               QString msg("Error waiting for command to finish: ");
               msg.append(caretCommandProgramName);
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", msg);
               return;
            }

           if (process.exitStatus() == QProcess::NormalExit) {
               if (process.exitCode() == 0) {
                  txt.append("COMMAND SUCCESSFUL\n\n");
               }
               else {
                  executeErrorFlag = true;
                  txt.append("COMMAND FAILED\n\n");
               }
            }
            else {
               executeErrorFlag = true;
               txt.append("COMMAND FAILED\n\n");
            }
            txt.append(process.readAll());
*/
            
            //
            // Display the results of the command
            //
            QtTextEditDialog* te = new QtTextEditDialog(this, true);
             
            te->setText(txt);
            te->show();
            
            QApplication::beep();
            QApplication::restoreOverrideCursor();
         }
      }
   }
}

/**
 * called when a command is selected.
 */
void 
GuiCaretCommandDialog::slotCommandListWidgetSelection(int item)
{
   //
   // Clear the selected command
   //
   selectedCommand = NULL;   

   commandDescriptionTextEdit->setPlainText("");

   //
   // Get the selected command
   //
   if ((item >= 0) && (item < static_cast<int>(commandParametersStackedWidget->count()))) {\
      if (caretCommandWidgets[item] == NULL) {
         caretCommandWidgets[item] = createCommandParametersWidget(caretCommands[item]);
         QWidget* w = commandParametersStackedWidget->widget(item);
         commandParametersStackedWidget->insertWidget(item, caretCommandWidgets[item]);
         delete w;
      }
      GuiCaretCommandWidget* guiCommandWidget = 
         dynamic_cast<GuiCaretCommandWidget*>(commandParametersStackedWidget->widget(item));
      if (guiCommandWidget != NULL) {
         selectedCommand = guiCommandWidget->getCommand();
      }
   
      if (selectedCommand != NULL) {
         //
         // Load the help information
         //
         const QString helpText = selectedCommand->getHelpInformation();
         commandDescriptionTextEdit->setPlainText(helpText);
         
         //
         // Show the gui widget
         //
         commandParametersStackedWidget->setCurrentWidget(guiCommandWidget);
      }
   }
   
   //
   // Scroll to top of widget
   // 
   commandParametersScrollArea->verticalScrollBar()->setSliderPosition(0);
   commandDescriptionTextEdit->verticalScrollBar()->setSliderPosition(0);
}

/**
 * create the command parameters widget.
 */
GuiCaretCommandWidget* 
GuiCaretCommandDialog::createCommandParametersWidget(CommandBase* command)
{
   //
   // Get the commands parameters
   //
   ScriptBuilderParameters parameters;
   command->getScriptBuilderParameters(parameters);
   
   //
   // Add the parameters
   //
/*
   const int numParams = parameters.getNumberOfParameters();
   for (int i = 0; i < numParams; i++) {
      const ScriptBuilderParameters::Parameter* p = parameters.getParameter(i);
      std::cout << "Parameter: "
                << p->getDescription().toAscii().constData() 
                << std::endl;
   }
*/
   
   //
   // Create the command
   //
   bool showCommentEditor = false;
   switch (dialogMode) {
      case DIALOG_MODE_COMMAND_SELECTOR:
      case DIALOG_MODE_COMMAND_SELECTOR_WITH_BEFORE_AFTER:
         showCommentEditor = false;
         break;
      case DIALOG_MODE_EXECUTOR_NON_MODAL:
         showCommentEditor = false;
         break;
   }
   GuiCaretCommandWidget* guiCommand = new GuiCaretCommandWidget(command, showCommentEditor);

   return guiCommand;
}                                  
      
/**
 * get the selected command (if command selector mode).
 */
QString 
GuiCaretCommandDialog::getSelectedCommandSwitch() const
{
   QString switchOut;
   
   const int indx = commandsListWidget->currentRow();
   if ((indx >= 0) &&
       (indx < static_cast<int>(caretCommands.size()))) {
      switchOut = caretCommands[indx]->getOperationSwitch();
   }
   
   return switchOut;
}
      

