
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
#include <QDir>
#include "WuQFileDialog.h"
#include <QLayout>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>

#include "BrainSet.h"
#include "CaretScriptFile.h"
#include "CommandBase.h"
#include "CommandScriptComment.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiCaretCommandDialog.h"
#include "GuiCaretCommandScriptBuilderDialog.h"
#include "GuiCaretCommandWidget.h"
#include "GuiMainWindow.h"
#include "QtTextEditDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCaretCommandScriptBuilderDialog::GuiCaretCommandScriptBuilderDialog(QWidget* parent,
                                                       const QString& caretHomeDirectory)
   : WuQDialog(parent)
{
   setWindowTitle("Caret Command Script Builder");
   
   //
   // Set path/name of Caret program
   // 
   caretCommandProgramName = "";
   if (caretHomeDirectory.isEmpty() == false) {
      caretCommandProgramName = caretHomeDirectory;
      caretCommandProgramName.append("/");
      caretCommandProgramName.append(BrainSet::getBinDirectoryName());
      caretCommandProgramName.append("/");
   }  
   caretCommandProgramName.append("caret_command");
   
   //
   // Scroll area for command widgets
   //
   commandLayoutWidget = new QWidget;
   commandLayout = new QVBoxLayout(commandLayoutWidget);
   commandLayout->setSpacing(5);
   commandScrollArea = new QScrollArea;
   commandScrollArea->setWidget(commandLayoutWidget);
   commandScrollArea->setWidgetResizable(true);
   
   //
   // New push button
   //
   QPushButton* newPushButton = new QPushButton("New...");
   newPushButton->setAutoDefault(false);
   newPushButton->setFixedSize(newPushButton->sizeHint());
   QObject::connect(newPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNewPushButton()));
                    
   //
   // Open push button
   //
   QPushButton* openPushButton = new QPushButton("Open...");
   openPushButton->setAutoDefault(false);
   openPushButton->setFixedSize(openPushButton->sizeHint());
   QObject::connect(openPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOpenPushButton()));
                    
   //
   // save push button
   //
   QPushButton* savePushButton = new QPushButton("Save...");
   savePushButton->setAutoDefault(false);
   savePushButton->setFixedSize(savePushButton->sizeHint());
   QObject::connect(savePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSavePushButton()));
                    
   //
   // run push button
   //
   QPushButton* runPushButton = new QPushButton("Run...");
   runPushButton->setAutoDefault(false);
   runPushButton->setFixedSize(runPushButton->sizeHint());
   QObject::connect(runPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRunPushButton()));
                    
   //
   // Close push button
   //
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   closePushButton->setFixedSize(closePushButton->sizeHint());
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(slotClosePushButton()));
                    
   //
   // Size buttons
   //
   QtUtilities::makeButtonsSameSize(newPushButton,
                                    openPushButton,
                                    savePushButton,
                                    runPushButton,
                                    closePushButton);
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout1 = new QHBoxLayout;
   buttonsLayout1->addWidget(newPushButton);
   buttonsLayout1->addWidget(openPushButton);
   buttonsLayout1->addWidget(savePushButton);
   buttonsLayout1->addWidget(runPushButton);
   buttonsLayout1->addWidget(closePushButton);

   //
   // Layout dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(commandScrollArea);   
   dialogLayout->addLayout(buttonsLayout1);

   //
   // Get commands and initialize command widgets
   //
   CommandBase::getAllCommandsSortedByDescription(caretCommands);
   
   //
   // add comment 
   //
   CommandScriptComment commandComment;
   QStringList commandStringList;
   commandStringList.append("Enter comment describing script here.");
   addInitialCommand(commandComment.getOperationSwitch(), "", commandStringList);
   
   dataIsModified = false;
}
    
/**
 * destructor.
 */
GuiCaretCommandScriptBuilderDialog::~GuiCaretCommandScriptBuilderDialog()
{
   const int numCommands = static_cast<int>(caretCommands.size());
   for (int i = 0; i < numCommands; i++) {
      delete caretCommands[i];
      caretCommands[i] = NULL;
   }
   
   const int numWidgets = static_cast<int>(widgetsThatNeedToBeDeleted.size());
   for (int i = 0; i < numWidgets; i++) {
      delete widgetsThatNeedToBeDeleted[i];
      widgetsThatNeedToBeDeleted[i] = NULL;
   }
   widgetsThatNeedToBeDeleted.clear();
}

/**
 * called when new button pressed.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotNewPushButton()
{
   if (dataIsModified) {
      if (QMessageBox::question(this,
                                  "Confirm",
                                  "Current script has been modified but not saved. \n"
                                  "Create new script without saving current script?",
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Cancel)
                                      == QMessageBox::Cancel) {
         return;
      }
   }   
   
   //
   // Remove all widgets in script builder
   //
   std::vector<GuiCaretCommandContainerWidget*> containerWidgets;
   getContainerWidgets(containerWidgets);
   const int numToDelete = static_cast<int>(containerWidgets.size());
   for (int i = (numToDelete - 1); i >= 0; i--) {
      deleteContainerWidget(containerWidgets[i]);
   }
   
   //
   // add comment 
   //
   CommandScriptComment commandComment;
   QStringList commandStringList;
   commandStringList.append("Enter comment describing script here.");
   addInitialCommand(commandComment.getOperationSwitch(), "", commandStringList);
   
   //
   // Data is unmodified
   //
   dataIsModified = false;
   
   scriptFileName = "";
}

/**
 * called when open button pressed.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotOpenPushButton()
{
   if (dataIsModified) {
      if (QMessageBox::question(this,
                                  "Confirm",
                                  "Current script has been modified but not saved. \n"
                                  "Open new script without saving current script?",
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Cancel)
                                      == QMessageBox::Cancel) {
         return;
      }
   }
   
   WuQFileDialog fd(this);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(".");
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilter(FileFilters::getCaretScriptFileFilter());
   fd.selectFilter(FileFilters::getCaretScriptFileFilter());
   if ((fd.exec() == WuQFileDialog::Accepted) &&
       (fd.selectedFiles().count() > 0)) {
      //
      // Get the name of the file
      //
      const QString scriptFileNameRead(fd.selectedFiles().at(0));

      //
      // Read the script file
      //
      CaretScriptFile scriptFile;
      try {
         scriptFile.readFile(scriptFileNameRead);
         scriptFileName = scriptFileNameRead;
      }
      catch (FileException& e) {
         QMessageBox::critical(this, "ERROR", e.whatQString());
         return;
      }
      
      //
      // Remove all widgets in script builder
      //
      std::vector<GuiCaretCommandContainerWidget*> containerWidgets;
      getContainerWidgets(containerWidgets);
      const int numToDelete = static_cast<int>(containerWidgets.size());
      for (int i = (numToDelete - 1); i >= 0; i--) {
         deleteContainerWidget(containerWidgets[i]);
      }
      
      //
      // Get the number of operations in the file
      //
      const int numOperations = scriptFile.getNumberOfCommandOperations();
      if (numOperations > 0) {
         //
         // Loop through the operations
         //
         for (int i = 0; i < numOperations; i++) {
            //
            // get the operation
            //
            CaretScriptFile::CaretCommandOperation* operation =
               scriptFile.getCommandOperation(i);
            const QString commandSwitch = operation->getSwitch();
            
            //
            // 1st operation is always comment
            //
            if (i == 0) {
               CommandScriptComment commandComment;
               if (commandSwitch != commandComment.getOperationSwitch()) {
                  //
                  // add comment 
                  //
                  QStringList commandStringList;
                  commandStringList.append("Enter comment describing script here.");
                  addInitialCommand(commandComment.getOperationSwitch(), "", commandStringList);
               }
            }
            
            //
            // add the operation
            //
            addInitialCommand(commandSwitch,
                              operation->getComment(),
                              operation->getParametersForGUI());
         }
      }
      
      //
      // data is unmodified
      //
      dataIsModified = false;
   }
}

/**
 * called when run button pressed.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotRunPushButton()
{
   if (dataIsModified) {
      if (QMessageBox::question(this,
                                  "Confirm",
                                  "Current script has been modified but not saved. \n"
                                  "and it is recommended to save the script prior\n"
                                  "to running it.\n"
                                  "Run without saving current script?",
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Cancel)
                                      == QMessageBox::Cancel) {
         return;
      }
   }
   
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
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Cancel)
                                      == QMessageBox::Cancel) {
         return;
      }
   }
  
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            
   //
   // The script file
   //
   CaretScriptFile scriptFile;

   //
   // Put the commands into a script file
   //
   placeCommandsIntoScriptFile(scriptFile);
   
   //
   // Run the script file
   //
   QString commandsOutputText;
   try {
      scriptFile.runCommandsInFile(this,
                                   caretCommandProgramName,
                                   commandsOutputText);
   }
   catch (FileException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", e.whatQString());
      return;
   }

   QApplication::beep();
   QApplication::restoreOverrideCursor();
   
   //
   // Display the results of the command
   //
   QtTextEditDialog* te = new QtTextEditDialog(this, true);
   
   te->setText(commandsOutputText);
   te->show();         
}

/**
 * called when save button pressed.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotSavePushButton()
{
   WuQFileDialog fd(this);
   fd.setAcceptMode(WuQFileDialog::AcceptSave);
   fd.setConfirmOverwrite(true);
   fd.setDirectory(".");
   fd.setFileMode(WuQFileDialog::AnyFile);
   fd.setFilter(FileFilters::getCaretScriptFileFilter());
   fd.selectFilter(FileFilters::getCaretScriptFileFilter());
   if (scriptFileName.isEmpty() == false) {
      fd.selectFile(FileUtilities::basename(scriptFileName));
   }
   if ((fd.exec() == WuQFileDialog::Accepted) &&
       (fd.selectedFiles().count() > 0)) {
      //
      // Get the name of the file
      //
      const QString scriptFileNameWrite(fd.selectedFiles().at(0));

      //
      // The script file
      //
      CaretScriptFile scriptFile;

      //
      // Put the commands into a script file
      //
      placeCommandsIntoScriptFile(scriptFile);
                                  
      //
      // Write the script file
      //
      try {
         scriptFile.writeFile(scriptFileNameWrite);
         
         //
         // Clear data modified
         //
         dataIsModified = false;
         
         scriptFileName = scriptFileNameWrite;
      }
      catch (FileException& e) {
         QMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }
}

/**
 * place the commands into a script file.
 */
void 
GuiCaretCommandScriptBuilderDialog::placeCommandsIntoScriptFile(CaretScriptFile& scriptFile)
{
   //
   // Get the command widgets
   //
   std::vector<GuiCaretCommandWidget*> commandWidgets;
   getCommandWidgets(commandWidgets);
   
   //
   // Loop through the commands
   //
   const int num = static_cast<int>(commandWidgets.size());
   for (int i = 0; i < num; i++) {

      //
      // Get the command
      //         
      QString commandSwitch;
      QString commandShortDescription;
      QStringList commandParameters;
      bool lastParameterIsVariableListFlag;
      commandWidgets[i]->getCommandLineForGUI(commandSwitch,
                                        commandParameters,
                                        commandShortDescription,
                                        lastParameterIsVariableListFlag);
      //
      // Add command to the file
      //
      CaretScriptFile::CaretCommandOperation* op =
         new CaretScriptFile::CaretCommandOperation(commandSwitch,
                                                    commandParameters,
                                                    lastParameterIsVariableListFlag,
                                                    commandShortDescription,
                                                    commandWidgets[i]->getComment());
      scriptFile.addCommandOperation(op);
   }
}

/**
 * called when close button pressed.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotClosePushButton()
{
   close();
} 

/**
 * get the caret command operations.
 */
void 
GuiCaretCommandScriptBuilderDialog::getCommandWidgets(std::vector<GuiCaretCommandWidget*>& commandWidgetsOut)
{
   commandWidgetsOut.clear();
   
   std::vector<GuiCaretCommandContainerWidget*> containerWidgets;
   getContainerWidgets(containerWidgets);
   
   const int num = static_cast<int>(containerWidgets.size());
   for (int i = 0; i < num; i++) {
      commandWidgetsOut.push_back(containerWidgets[i]->getCommandWidget());
   }
}
      
/**
 * get the container widgets.
 */
void 
GuiCaretCommandScriptBuilderDialog::getContainerWidgets(std::vector<GuiCaretCommandContainerWidget*>& containerWidgetsOut)
{
   containerWidgetsOut.clear();
   
   const int num = commandLayout->count();
   for (int i = 0; i < num; i++) {
      QLayoutItem* item = commandLayout->itemAt(i);
      QWidget* widget = item->widget();
      if (widget != NULL) {
         GuiCaretCommandContainerWidget* ccw = dynamic_cast<GuiCaretCommandContainerWidget*>(widget);
         if (ccw != NULL) {
            containerWidgetsOut.push_back(ccw);
         }
      }
   }
}
      
/**
 * add a command to the dialog.
 */
void 
GuiCaretCommandScriptBuilderDialog::addInitialCommand(const QString& commandSwitch,
                                                      const QString& commandComment,
                                                      const QStringList& commandParameters)
{
   //
   // Loop through and find the desired command
   //
   bool commandFound = false;
   const int numCommands = static_cast<int>(caretCommands.size());
   for (int i = 0; i < numCommands; i++) {
      if (commandSwitch == caretCommands[i]->getOperationSwitch()) {
         //
         // Create the gui widget for the command
         //
         GuiCaretCommandContainerWidget* ccw =
             new GuiCaretCommandContainerWidget(caretCommands[i],
                                                true);
         ccw->setCommentAndParameters(commandComment,
                                      commandParameters);
                                      
         //
         // connect signals
         //
         QObject::connect(ccw, SIGNAL(signalAddButtonPressed(GuiCaretCommandContainerWidget*)),
                          this, SLOT(slotAddCommand(GuiCaretCommandContainerWidget*)));
         QObject::connect(ccw, SIGNAL(signalDeleteButtonPressed(GuiCaretCommandContainerWidget*)),
                          this, SLOT(slotDeleteCommand(GuiCaretCommandContainerWidget*)));
         QObject::connect(ccw, SIGNAL(signalMoveUpButtonPressed(GuiCaretCommandContainerWidget*)),
                          this, SLOT(slotMoveCommandUp(GuiCaretCommandContainerWidget*)));
         QObject::connect(ccw, SIGNAL(signalMoveDownButtonPressed(GuiCaretCommandContainerWidget*)),
                          this, SLOT(slotMoveCommandDown(GuiCaretCommandContainerWidget*)));
         QObject::connect(ccw, SIGNAL(signalDataModified()),
                          this, SLOT(slotDataModified()));

         //
         // Add to the dialog
         //
         commandLayout->addWidget(ccw);
         commandLayoutWidget->updateGeometry();
         //commandScrollArea->ensureWidgetVisible(ccw);
         commandFound = true;
      }
   }
   
   if (commandFound == false) {
      QMessageBox::critical(this, "ERROR", 
                              "Command " + commandSwitch + " not found.");
   }
}

/**
 * called when data is modified.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotDataModified()
{
   dataIsModified = true;
}
      
/**
 * called to add a new command.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotAddCommand(GuiCaretCommandContainerWidget* addAfterWidget)
{
   static QSize dialogSize(0, 0);
   static QString lastCommandSwitch;
   
   //
   // Choose the command
   //
   GuiCaretCommandDialog ccd(this, 
                             theMainWindow->getBrainSet()->getCaretHomeDirectory(),
                             GuiCaretCommandDialog::DIALOG_MODE_COMMAND_SELECTOR_WITH_BEFORE_AFTER);
   if (dialogSize.width() > 0) {
      ccd.resize(dialogSize);
   }
   if (lastCommandSwitch.isEmpty() == false) {
      ccd.setSelectedCommand(lastCommandSwitch);
   }
   if (ccd.exec() == GuiCaretCommandDialog::Accepted) {
      //
      // Does new widget go after current widget?
      //
      const bool afterFlag = ccd.getAddAfterWidget();

      //
      // Save dialog size
      //
      dialogSize = ccd.size();
      
      //
      // Get the command switch
      //
      const QString commandSwitch = ccd.getSelectedCommandSwitch();
      if (commandSwitch.isEmpty() == false) {
         lastCommandSwitch = commandSwitch;
         
         //
         // Loop through and find the desired command
         //
         CommandBase* command = NULL;
         const int numCommands = static_cast<int>(caretCommands.size());
         for (int i = 0; i < numCommands; i++) {
            if (commandSwitch == caretCommands[i]->getOperationSwitch()) {
               command = caretCommands[i];
               break;
            }
         }
         if (command != NULL) {
            //
            // Create the gui widget for the command
            //
            GuiCaretCommandContainerWidget* ccw =
                new GuiCaretCommandContainerWidget(command);
            
            //
            // connect signals
            //
            QObject::connect(ccw, SIGNAL(signalAddButtonPressed(GuiCaretCommandContainerWidget*)),
                             this, SLOT(slotAddCommand(GuiCaretCommandContainerWidget*)));
            QObject::connect(ccw, SIGNAL(signalDeleteButtonPressed(GuiCaretCommandContainerWidget*)),
                             this, SLOT(slotDeleteCommand(GuiCaretCommandContainerWidget*)));
            QObject::connect(ccw, SIGNAL(signalMoveUpButtonPressed(GuiCaretCommandContainerWidget*)),
                             this, SLOT(slotMoveCommandUp(GuiCaretCommandContainerWidget*)));
            QObject::connect(ccw, SIGNAL(signalMoveDownButtonPressed(GuiCaretCommandContainerWidget*)),
                             this, SLOT(slotMoveCommandDown(GuiCaretCommandContainerWidget*)));
            QObject::connect(ccw, SIGNAL(signalDataModified()),
                             this, SLOT(slotDataModified()));

            //
            // Add to the dialog after the widget that requested the add
            //
            int widgetIndex = commandLayout->indexOf(addAfterWidget);
            if (widgetIndex >= 0) {
               if (afterFlag) {
                  if (widgetIndex >= (commandLayout->count() - 1)) {
                     widgetIndex = -1;
                  }
                  else {
                     widgetIndex++;
                  }
               }
               commandLayout->insertWidget(widgetIndex, ccw);
               commandLayoutWidget->updateGeometry();
               commandScrollArea->ensureWidgetVisible(ccw);
            }
         }
      }
      
      //
      // Data now modified
      //
      slotDataModified();
   }
}

/**
 * called to delete a command.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotDeleteCommand(GuiCaretCommandContainerWidget* deleteWidget)
{
   if (QMessageBox::question(this, 
                               "CONFIRM",
                               "Delete Command?",
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Ok)
                                      == QMessageBox::Ok) {
      deleteContainerWidget(deleteWidget);

      //
      // Data now modified
      //
      slotDataModified();
   }
}
      
/**
 * delete a container widget.
 */
void 
GuiCaretCommandScriptBuilderDialog::deleteContainerWidget(GuiCaretCommandContainerWidget* ccw)
{
      //
      // Remove the widget from the gui
      //
      ccw->hide();
      commandLayout->removeWidget(ccw);  // use takeAt() ????
      commandLayoutWidget->updateGeometry();
      
      //
      // Delete later since this method called from "deleteWidget"
      //
      widgetsThatNeedToBeDeleted.push_back(ccw);
}
      
/**
 * called to move a command up one position.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotMoveCommandUp(GuiCaretCommandContainerWidget* ccw)
{
   const int widgetIndex = commandLayout->indexOf(ccw);
   if (widgetIndex > 0) {
      commandLayout->removeWidget(ccw);
      commandLayout->insertWidget(widgetIndex - 1, ccw);
      commandLayoutWidget->updateGeometry();

      //
      // Data now modified
      //
      slotDataModified();
   }
}

/**
 * called to move a command down one position.
 */
void 
GuiCaretCommandScriptBuilderDialog::slotMoveCommandDown(GuiCaretCommandContainerWidget* ccw)
{
   const int widgetIndex = commandLayout->indexOf(ccw);
   if (widgetIndex < (commandLayout->count() - 1)) {
      commandLayout->removeWidget(ccw);
      commandLayout->insertWidget(widgetIndex + 1, ccw);
      commandLayoutWidget->updateGeometry();

      //
      // Data now modified
      //
      slotDataModified();
   }
}

//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandContainerWidget::GuiCaretCommandContainerWidget(
                      CommandBase* commandOperationIn,
                      const bool showDeleteButton)
{
   //
   // Create the gui widget for the command
   //
   commandWidget = new GuiCaretCommandWidget(commandOperationIn, true);
   QObject::connect(commandWidget, SIGNAL(signalDataModified()),
                    this, SIGNAL(signalDataModified()));
                       
   //
   // Get the help text
   //
   commandHelpText = commandOperationIn->getHelpInformation();
   
   //
   // Create the add button
   //
   QPushButton* addPushButton = new QPushButton("Add");
   addPushButton->setToolTip("Add an operation after\n"
                             "or before this operation.");
   addPushButton->setAutoDefault(false);
   QObject::connect(addPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddPushButton()));
   
   //
   // Create the delete button
   //
   QPushButton* deletePushButton = new QPushButton("Del");
   deletePushButton->setToolTip("Delete this operation.");
   deletePushButton->setAutoDefault(false);
   QObject::connect(deletePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeletePushButton()));
                    
   //
   // Create the help button
   //
   QPushButton* helpPushButton = new QPushButton("Help");
   helpPushButton->setToolTip("Show help information\n"
                              "about this command.");
   helpPushButton->setAutoDefault(false);
   QObject::connect(helpPushButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpPushButton()));
                    
   //
   // Create the move up button
   //
   QPushButton* moveUpPushButton = new QPushButton("Up");
   moveUpPushButton->setToolTip("Move this command\n"
                                "up one position.");
   moveUpPushButton->setAutoDefault(false);
   QObject::connect(moveUpPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMoveUpPushButton()));
                    
   //
   // Create the move down button
   //
   QPushButton* moveDownPushButton = new QPushButton("Down");
   moveDownPushButton->setToolTip("Move this command\n"
                                "up one position.");
   moveDownPushButton->setAutoDefault(false);
   QObject::connect(moveDownPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMoveDownPushButton()));
                    
   //
   // Make buttons same size
   //
   QtUtilities::makeButtonsSameSize(addPushButton,
                                    deletePushButton,
                                    helpPushButton,
                                    moveUpPushButton,
                                    moveDownPushButton);
                                    
   //
   // Layout for buttons
   //
   QVBoxLayout* buttonsLayout = new QVBoxLayout;
   buttonsLayout->addWidget(addPushButton);
   buttonsLayout->addWidget(deletePushButton);
   buttonsLayout->addWidget(helpPushButton);
   buttonsLayout->addStretch();
   buttonsLayout->addWidget(moveUpPushButton);
   buttonsLayout->addWidget(moveDownPushButton);
   
   //
   // Layout for widget
   //
   QHBoxLayout* widgetLayout = new QHBoxLayout(this);
   widgetLayout->addLayout(buttonsLayout);
   widgetLayout->addWidget(commandWidget);
   widgetLayout->setStretchFactor(buttonsLayout, 0);
   widgetLayout->setStretchFactor(commandWidget, 1000);
   
   if (showDeleteButton == false) {
      deletePushButton->hide();
   }
}

/**
 * destructor.
 */
GuiCaretCommandContainerWidget::~GuiCaretCommandContainerWidget()
{
}

/**
 * called when add button pressed.
 */
void 
GuiCaretCommandContainerWidget::slotAddPushButton()
{
   emit signalAddButtonPressed(this);
}

/**
 * called when delete button pressed.
 */
void 
GuiCaretCommandContainerWidget::slotDeletePushButton()
{
   emit signalDeleteButtonPressed(this);
}

/**
 * called when move up button pressed.
 */
void 
GuiCaretCommandContainerWidget::slotMoveUpPushButton()
{
   emit signalMoveUpButtonPressed(this);
}

/**
 * called when move down button pressed.
 */
void 
GuiCaretCommandContainerWidget::slotMoveDownPushButton()
{
   emit signalMoveDownButtonPressed(this);
}

/**
 * called when help button pressed.
 */
void 
GuiCaretCommandContainerWidget::slotHelpPushButton()
{
   QtTextEditDialog* te = new QtTextEditDialog(this,
                                               true,
                                               false);
   te->setText(commandHelpText);
   te->show();
}

/**
 * set the comment and parameters.
 */
void 
GuiCaretCommandContainerWidget::setCommentAndParameters(const QString& commentIn,
                                                        const QStringList& parametersIn)
{
   commandWidget->setComment(commentIn);
   commandWidget->setParameters(parametersIn);
}
                        
