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

#include <QGlobalStatic>

#ifndef Q_OS_WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif  // ifndef Q_OS_WIN32

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <QDateTime>
#include <QFileInfo>
#include <QGroupBox>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>

#include "DebugControl.h"
#include "GuiBatchCommandDialog.h"
#include "QtUtilities.h"
#include "WuQFileDialog.h"

/**
 * Constructor.
 */
GuiBatchCommandDialog::GuiBatchCommandDialog(QWidget* parent,
                                             const QString& directoryToRunIn,
                                             const QString& commandIn,
                                             const QString& commandFileNameIn)
   : QtDialog(parent)
{
   setWindowTitle("Caret Batch Command");
   
   //
   // Directory in which to run command file
   //
   directory = directoryToRunIn;
   command = commandIn;
   
   //
   // Command file name
   //
   QString commandFileName(commandFileNameIn);
   if (commandFileName.isEmpty()) {
      commandFileName = "caret_command";
   }
#ifdef Q_OS_WIN32
   commandFileName.append(".bat");
#else
   commandFileName.append(".sh");
#endif
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Command File Name
   //
   QGroupBox* commandFileGroupBox = new QGroupBox("Command File Name");
   dialogLayout->addWidget(commandFileGroupBox);
   QHBoxLayout* commandFileGroupLayout = new QHBoxLayout(commandFileGroupBox);
   QPushButton* commandFilePushButton = new QPushButton("Select...");
   commandFileGroupLayout->addWidget(commandFilePushButton);
   commandFilePushButton->setAutoDefault(false);
   commandFilePushButton->setFixedSize(commandFilePushButton->sizeHint());
   QObject::connect(commandFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCommandFilePushButton()));
   commandFileNameLineEdit = new QLineEdit;
   commandFileGroupLayout->addWidget(commandFileNameLineEdit);
   commandFileNameLineEdit->setMinimumWidth(300);
   commandFileNameLineEdit->setText(commandFileName);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   
   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}
                        
/**
 * Destructor.
 */
GuiBatchCommandDialog::~GuiBatchCommandDialog()
{
}

/**
 * Called to select the command file name.
 */
void
GuiBatchCommandDialog::slotCommandFilePushButton()
{
   //
   // Create a file dialog to select the command file.
   //
   WuQFileDialog commandFileNameDialog(this);
   commandFileNameDialog.setModal(true);
   commandFileNameDialog.setWindowTitle("Choose Command File");
   commandFileNameDialog.setFileMode(WuQFileDialog::AnyFile);
#ifdef Q_OS_WIN32
   const QString filter("Command File (*.bat)");
#else
   const QString filter("Command File (*.sh)");
#endif
   commandFileNameDialog.setFilter(filter);
   if (commandFileNameDialog.exec() == QDialog::Accepted) {
      commandFileNameLineEdit->setText(commandFileNameDialog.selectedFiles().at(0));
   }
}

/**
 * Called when OK or Cancel button pressed.
 */
void 
GuiBatchCommandDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      if (DebugControl::getDebugOn()) {
         std::cout << std::endl << command.toAscii().constData() << std::endl;
      }
      
      //
      // Make sure command exists
      //
      if (command.isEmpty()) {
         QMessageBox::critical(this, "Error", "PROGRAM ERROR: command is empty.");
         return;
      }
      
      //
      // See if command file exists and, if so, warn user
      //
      const QString commandFileName(commandFileNameLineEdit->text());
      if (QFile::exists(commandFileName)) {
         QString msg("Command file ");
         msg.append(commandFileName);
         msg.append(" exits.  Overwrite it?");
         if (QMessageBox::warning(this, 
                                  "Warning", 
                                  msg,
                                 (QMessageBox::Yes | QMessageBox::No),
                                      QMessageBox::No) == QMessageBox::No) {
            return;
         }
      }
      
      //
      // Open up the command file
      //
      QFile file(commandFileName);
      if (file.open(QIODevice::WriteOnly)) {
         QTextStream stream(&file);
         stream.setRealNumberNotation(QTextStream::FixedNotation);
         stream.setRealNumberPrecision(6);
#ifdef Q_OS_WIN32
         stream << "REM   Created at " 
              << QDateTime::currentDateTime().toString(Qt::TextDate) << "\n"; 
         stream << "\n";
#else
         stream << "#!/bin/sh \n";
         stream << "#   Created at " 
              << QDateTime::currentDateTime().toString(Qt::TextDate) << "\n"; 
         stream << "\n";
#endif
         if (directory.isEmpty() == false) {
            stream << "cd " << directory << "\n";
         }
         stream << "\n";
         stream << command;
         stream << "\n";
         
         file.close();
         
#ifndef Q_OS_WIN32
         //
         // Make command file executable and readable by all
         // Writable by user
         //
         chmod(commandFileName.toAscii().constData(), 
               S_IRUSR + S_IWUSR + S_IXUSR + S_IRGRP + S_IXGRP + S_IROTH + S_IXOTH);
#endif // #ifndef Q_OS_WIN32
      }
      else {
         QString msg("Unable to open for writing: ");
         QFileInfo fileInfo(commandFileName);
         if (fileInfo.exists() && (fileInfo.isWritable() == false)) {
            msg = "File exists and is not writable: ";
         }
         msg.append(commandFileName);
         QMessageBox::critical(this, "Error", msg);
         return;
      }
   }
   
   QDialog::done(r);
}
