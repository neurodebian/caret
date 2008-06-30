
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

#include <iostream>

#include <QGlobalStatic>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRegExp>
#include <QTextCursor>
#include <QTextEdit>

#include "GuiShellCommandWindow.h"

/**
 * constructor.
 */
GuiShellCommandWindow::GuiShellCommandWindow(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Shell Command Window");
   
   //
   // Create the command output section
   //
   commandOutputTextEdit = new QTextEdit;
   commandOutputTextEdit->setReadOnly(true);
   
   //
   // Create the command line edit
   //
   QLabel* commandLabel = new QLabel("Command ");
   commandLineEdit = new GuiShellCommandLineEdit;
   QObject::connect(commandLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(slotCommandLineReturnPressed()));
   QObject::connect(commandLineEdit, SIGNAL(signalUpArrowKeyPressed()),
                    this, SLOT(slotCommandLineUpArrowPressed()));
   QObject::connect(commandLineEdit, SIGNAL(signalDownArrowKeyPressed()),
                    this, SLOT(slotCommandLineDownArrowPressed()));
   QObject::connect(commandLineEdit, SIGNAL(signalControlKPressed()),
                    commandOutputTextEdit, SLOT(clear()));
   QHBoxLayout* commandLayout = new QHBoxLayout;
   commandLayout->addWidget(commandLabel);                    
   commandLayout->addWidget(commandLineEdit);                    
   commandLayout->setStretchFactor(commandLabel, 0);                    
   commandLayout->setStretchFactor(commandLineEdit, 1000);    
                   
   //
   // layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(commandOutputTextEdit);
   dialogLayout->addLayout(commandLayout);
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
   buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(false);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
                    
   //
   // clear previous commands
   //
   previousCommands.clear();
   previousCommandIndex = -1;
}

/**
 * destructor.
 */
GuiShellCommandWindow::~GuiShellCommandWindow()
{
}

/**
 * called when up arrow is pressed in command line edit.
 */
void 
GuiShellCommandWindow::slotCommandLineUpArrowPressed()
{
   previousCommandIndex--;
   if (previousCommandIndex < 0) {
      previousCommandIndex = 0;
   }
   if ((previousCommandIndex >= 0) &&
       (previousCommandIndex < previousCommands.count())) {
      commandLineEdit->setText(previousCommands.at(previousCommandIndex));
   }
}

/**
 * called when down arrow is pressed in command line edit.
 */
void 
GuiShellCommandWindow::slotCommandLineDownArrowPressed()
{
   previousCommandIndex++;
   if (previousCommandIndex >= previousCommands.count()) {
      previousCommandIndex = previousCommands.count();
      commandLineEdit->setText("");
   }
   else if ((previousCommandIndex >= 0) &&
       (previousCommandIndex < previousCommands.count())) {
      commandLineEdit->setText(previousCommands.at(previousCommandIndex));
   }
}

/**
 * convert some unix commands to their window's equivalents.
 */
QString 
GuiShellCommandWindow::convertUnixToWindows(const QString& s) const
{
#ifdef Q_OS_WIN32
   QString s1(s.trimmed());
   if (s1.left(3) == "ls ") {
      s1.replace(0, 2, "dir");
   }
   else if (s1 == "ls") {
      s1 = "dir";
   }
   else if (s1.left(4) == "cat ") {
      s1.replace(0, 3, "type");
   }
   else if (s1.left(3) == "cp ") {
      s1.replace(0, 2, "copy");
   }
   else if (s1.left(3) == "mv ") {
      s1.replace(0, 2, "rename");
   }
   else if (s1.left(3) == "rm") {
      s1.replace(0, 2, "del");
   }
   else if (s1 == "pwd") {
      s1 = "chdir";  // note: chdir without any arguments prints directory
   }
   return s1;
#else  // Q_OS_WIN32
   return s;
#endif // Q_OS_WIN32
}

/**
 * called when return pressed in command line edit.
 */
void 
GuiShellCommandWindow::slotCommandLineReturnPressed()
{
   const QString userCommand(convertUnixToWindows(commandLineEdit->text()));
   if (userCommand.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Command is empty.");
      return;
   }
   
   //
   // Setup the command
   //
   QString processName;
   QStringList processArgs;
#ifdef Q_OS_WIN32
   processName = "cmd.exe";
   processArgs << "/C";
   processArgs << userCommand;
#else // Q_OS_WIN32
   processName = "/bin/sh";
   processArgs << "-c";   // arg to /bin/sh
   processArgs << userCommand;
#endif // Q_OS_WIN32

   //
   // Insert the command into the window in black text
   //
   QString commandForOutputWindow;
   commandForOutputWindow += "<font color=\"#000000\">";   
   commandForOutputWindow += ">";   
   commandForOutputWindow += userCommand;
   commandForOutputWindow += "<font><br>";
   commandOutputTextEdit->insertHtml(commandForOutputWindow);
   
   //
   // Run the command
   //
   QProcess process;   
   process.start(processName, processArgs);
   if (!process.waitForStarted()) {
      QMessageBox::critical(this, "ERROR", "Error starting process.");
      return;
   }
   
   //
   // When process finishes, place its text in window
   //
   if (process.waitForFinished()) {
      //
      // Show command output in blue
      //
      QString txt("<font color=\"#0000FF\">");
      txt += process.readAll();
      txt += "</font><br>";
      txt.replace("\r\n", "<br>");
      txt.replace(QRegExp("[\n\r]"), "<br>");
      commandOutputTextEdit->insertHtml(txt);
   }
   else {
      QMessageBox::critical(this, "ERROR", "Error waiting for process to finish.");
      return;
   }
   
   //
   // Did user command fail?
   //
   if ((process.exitCode() != 0) ||
       (process.exitStatus() != QProcess::NormalExit)) {
      //
      // Show error information in red
      //
      QString commandForOutputWindow;
      commandForOutputWindow += "<font color=\"#FF0000\">";   
      commandForOutputWindow += process.readAllStandardError();
      commandForOutputWindow += "</font><br>";
      commandOutputTextEdit->insertHtml(commandForOutputWindow);
   }
   
   //
   // Save command for access with up arrow
   //
   previousCommands += userCommand;
   previousCommandIndex = previousCommands.count();

   //
   // Clear the last entered command
   //
   commandLineEdit->setText("");
   
   //
   // Scroll to end of window text
   //
   QTextCursor tc = commandOutputTextEdit->textCursor();
   tc.movePosition(QTextCursor::End);
   commandOutputTextEdit->setTextCursor(tc);
}


//-------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiShellCommandLineEdit::GuiShellCommandLineEdit()
   : QLineEdit()
{
}

/**
 * destructor.
 */
GuiShellCommandLineEdit::~GuiShellCommandLineEdit()
{
}

/**
 * called when a key is pressed.
 */
void 
GuiShellCommandLineEdit::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Up) {
      signalUpArrowKeyPressed();
   }
   else if (event->key() == Qt::Key_Down) {
      signalDownArrowKeyPressed();
   }
   
#ifdef Q_OS_MAC_XYZ
   else if ((event->key() == Qt::Key_A) &&
            (event->modifiers() == Qt::MetaModifier)) {
      setCursorPosition(0);
   }
   else if ((event->key() == Qt::Key_E) &&
            (event->modifiers() == Qt::MetaModifier)) {
      setCursorPosition(text().length());
   }
   else if ((event->key() == Qt::Key_K) &&
            (event->modifiers() == Qt::MetaModifier)) {
      signalControlKPressed();
   }
#else // Q_OS_MAC
   else if ((event->key() == Qt::Key_A) &&
            (event->modifiers() == Qt::ControlModifier)) {
      setCursorPosition(0);
   }
   else if ((event->key() == Qt::Key_E) &&
            (event->modifiers() == Qt::ControlModifier)) {
      setCursorPosition(text().length());
   }
   else if ((event->key() == Qt::Key_K) &&
            (event->modifiers() == Qt::ControlModifier)) {
      signalControlKPressed();
   }
#endif // Q_OS_MAC

   QLineEdit::keyPressEvent(event);
}

