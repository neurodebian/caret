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

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDir>
#include "WuQFileDialog.h"
#include <QImage>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>

#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelViewingWindow.h"
#include "GuiCaptureWindowImageDialog.h"
#include "GuiChooseSpecFileDialog.h"
#include "GuiConvertDataFileDialog.h"
#include "GuiCopySpecFileDialog.h"
#include "GuiDataFileOpenDialog.h"
#include "GuiDataFileSaveDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowFileActions.h"
#include "GuiPreferencesDialog.h"
#include "GuiSpecFileDialog.h"
#include "GuiLoadedFileManagementDialog.h"
#include "GuiZipSpecFileDialog.h"
#include "QtUtilities.h"
#include "StringUtilities.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowFileActions::GuiMainWindowFileActions(GuiMainWindow* parent) :
   QObject(parent)
{
   setObjectName("GuiMainWindowFileActions");
   
   fastOpenDataFileAction = new QAction(parent);
   fastOpenDataFileAction->setText("Fast Open Data File...");
   fastOpenDataFileAction->setShortcut(Qt::CTRL+Qt::Key_F);
   fastOpenDataFileAction->setObjectName("fastOpenDataFileAction");
   QObject::connect(fastOpenDataFileAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayFastOpenDataFileDialog()));

   showPreferencesDialogAction = new QAction(parent);
   showPreferencesDialogAction->setText("Preferences...");
   showPreferencesDialogAction->setObjectName("showPreferencesDialogAction");
   QObject::connect(showPreferencesDialogAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayPreferencesDialog()));

   setCurrentDirectoryAction = new QAction(parent);
   setCurrentDirectoryAction->setText("Set Current Directory...");
   setCurrentDirectoryAction->setObjectName("setCurrentDirectoryAction");
   QObject::connect(setCurrentDirectoryAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotSetCurrentDirectory()));

   convertDataFileAction = new QAction(parent);
   convertDataFileAction->setText("Convert Data File Formats...");
   convertDataFileAction->setObjectName("convertDataFileAction");
   QObject::connect(convertDataFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(convertDataFileSlot()));

   copySpecFileAction = new QAction(parent);
   copySpecFileAction->setText("Copy Spec File...");
   copySpecFileAction->setObjectName("copySpecFileAction");
   QObject::connect(copySpecFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(copySpecFileSlot()));

   validateSpecFileAction = new QAction(parent);
   validateSpecFileAction->setText("Validate a Spec File...");
   validateSpecFileAction->setObjectName("validateSpecFileAction");
   QObject::connect(validateSpecFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(validateSpecFileSlot()));

   zipSpecFileAction = new QAction(parent);
   zipSpecFileAction->setText("Zip a Spec File...");
   zipSpecFileAction->setObjectName("zipSpecFileAction");
   QObject::connect(zipSpecFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(zipSpecFileSlot()));

   recordAsMpegAction = new QAction(parent);
   recordAsMpegAction->setText("Record Main Window Images as Movie...");
   recordAsMpegAction->setObjectName("recordAsMpegAction");
   QObject::connect(recordAsMpegAction, SIGNAL(triggered(bool)),
                    this, SLOT(recordAsMpegSlot()));

   dataFileOpenAction = new QAction(parent);
   dataFileOpenAction->setText("Open Data File...");
   dataFileOpenAction->setShortcut(Qt::CTRL+Qt::Key_O);
   dataFileOpenAction->setObjectName("dataFileOpenAction");
   QObject::connect(dataFileOpenAction, SIGNAL(triggered(bool)),
                    this, SLOT(dataFileOpenSlot()));
                    
   dataFileSaveAction = new QAction(parent);
   dataFileSaveAction->setText("Save Data File...");
   dataFileSaveAction->setShortcut(Qt::CTRL+Qt::Key_S);
   dataFileSaveAction->setObjectName("dataFileSaveAction");
   QObject::connect(dataFileSaveAction, SIGNAL(triggered(bool)),
                    this, SLOT(dataFileSaveSlot()));                    
   
   copyMainWindowToClipboardAction = new QAction(parent);
   copyMainWindowToClipboardAction->setText("Copy Main Window Image to Clipboard");
   copyMainWindowToClipboardAction->setObjectName("copyMainWindowToClipboardAction");
   QObject::connect(copyMainWindowToClipboardAction, SIGNAL(triggered(bool)),
                    this, SLOT(copyMainWindowToClipboard()));

   printMainWindowAction = new QAction(parent);
   printMainWindowAction->setText("Print Main Window...");
   printMainWindowAction->setObjectName("printMainWindowAction");
   QObject::connect(printMainWindowAction, SIGNAL(triggered(bool)),
                    this, SLOT(printMainWindowImage()));

   captureMainWindowImageAction = new QAction(parent);
   captureMainWindowImageAction->setText("Capture Image of Window...");
   captureMainWindowImageAction->setObjectName("captureMainWindowImageAction");
   QObject::connect(captureMainWindowImageAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayCaptureWindowImageDialog()));
                    
   openSpecFileAction = new QAction(parent);
   openSpecFileAction->setText("Open Spec File...");
   openSpecFileAction->setShortcut(Qt::CTRL+Qt::Key_N);
   openSpecFileAction->setObjectName("openSpecFileAction");
   QObject::connect(openSpecFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(openSpecFileSlot()));

   addDocumentToSpecFileAction = new QAction(parent);
   addDocumentToSpecFileAction->setText("Add Document File to Spec File...");
   addDocumentToSpecFileAction->setObjectName("addDocumentToSpecFileAction");
   QObject::connect(addDocumentToSpecFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(addDocumentToSpecFileSlot()));
                    
   closeSpecFileAction = new QAction(parent);
   closeSpecFileAction->setText("Close Current Spec File");
   closeSpecFileAction->setObjectName("closeSpecFileAction");
   QObject::connect(closeSpecFileAction, SIGNAL(triggered(bool)),
                    parent, SLOT(slotCloseSpecFile()));

   loadedFileManagementAction = new QAction(parent);
   loadedFileManagementAction->setText("Manage Loaded Files...");
   loadedFileManagementAction->setShortcut(Qt::CTRL+Qt::Key_M);
   loadedFileManagementAction->setObjectName("loadedFileManagementAction");
   QObject::connect(loadedFileManagementAction, SIGNAL(triggered(bool)),
                    this, SLOT(loadedFileManagementSlot()));

   exitCaretAction = new QAction(parent);
   exitCaretAction->setText("Exit...");
   exitCaretAction->setShortcut(Qt::CTRL+Qt::Key_Q);
   exitCaretAction->setObjectName("exitCaretAction");
   QObject::connect(exitCaretAction, SIGNAL(triggered(bool)),
                    parent, SLOT(slotCloseProgram()));  //SLOT(close()));
}

/**
 * Destructor.
 */
GuiMainWindowFileActions::~GuiMainWindowFileActions()
{
}

/**
 * Called to set the current directory.
 */
void 
GuiMainWindowFileActions::slotSetCurrentDirectory()
{
   WuQFileDialog fd(theMainWindow);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   fd.setWindowTitle("Choose Directory");
   fd.setFileMode(WuQFileDialog::DirectoryOnly);
   if (fd.exec() == QDialog::Accepted) {
      const QString path(fd.directory().absolutePath());
      QDir::setCurrent(path); // selectedFiles().at(0));
   }
}
      
/**
 * convert data files slot.
 */
void 
GuiMainWindowFileActions::convertDataFileSlot()
{
   static GuiConvertDataFileDialog* convertDialog = NULL;
   
   if (convertDialog == NULL) {
      convertDialog = new GuiConvertDataFileDialog(theMainWindow);
   }
   convertDialog->show();
   convertDialog->activateWindow();
}
      
/**
 * copy spec file slot.
 */
void 
GuiMainWindowFileActions::copySpecFileSlot()
{
   static GuiCopySpecFileDialog* copySpecDialog = NULL;
   
   if (copySpecDialog == NULL) {
      copySpecDialog = new GuiCopySpecFileDialog(theMainWindow, theMainWindow->getBrainSet()->getPreferencesFile());
   }
   copySpecDialog->show();
   copySpecDialog->activateWindow();
}      

/**
 * validate spec file slot.
 */
void 
GuiMainWindowFileActions::validateSpecFileSlot()
{
   GuiChooseSpecFileDialog csfd(theMainWindow, theMainWindow->getBrainSet()->getPreferencesFile(), true);
   if (csfd.exec() == QDialog::Accepted) {
      SpecFile sf;
      try {
         sf.readFile(csfd.getSelectedSpecFile());
         QString msg;
         if (sf.validate(msg)) {
            QMessageBox::information(theMainWindow, "SUCCESS", "Spec File is Valid.");
         }
         else {
            QMessageBox::critical(theMainWindow, "ERROR", msg);
         }
      }
      catch (FileException& e) {
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
   }
}      

/**
 * zip spec file slot.
 */
void 
GuiMainWindowFileActions::zipSpecFileSlot()
{
   static GuiZipSpecFileDialog* zipSpecDialog = NULL;
   
   if (zipSpecDialog == NULL) {
      zipSpecDialog = new GuiZipSpecFileDialog(theMainWindow, theMainWindow->getBrainSet()->getPreferencesFile());
   }
   zipSpecDialog->show();
   zipSpecDialog->activateWindow();
}      

/**
 * Record as MPEG slot.
 */
void
GuiMainWindowFileActions::recordAsMpegSlot()
{
   theMainWindow->getRecordingDialog(true);
}

/**
 * data file open slot.
 */
void 
GuiMainWindowFileActions::dataFileOpenSlot()
{
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   
   static GuiDataFileOpenDialog* fd = NULL;
   if (fd == NULL) {
      fd = new GuiDataFileOpenDialog(theMainWindow);
   }
   fd->setHistory(pf->getRecentDataFileDirectories());
   fd->setDirectory(QDir::currentPath());
   fd->show();
   fd->raise();
   fd->activateWindow();
}
      
/**
 * data file save slot.
 */
void 
GuiMainWindowFileActions::dataFileSaveSlot()
{
   static QString lastFileFilter;
   
   GuiDataFileSaveDialog fd(theMainWindow);
   if (lastFileFilter.isEmpty() == false) {
      fd.selectFileType(lastFileFilter);
   }
   fd.exec();
   lastFileFilter = fd.getFileTypeFilter();
}

/**
 * copy main window image to clipboard.
 */
void 
GuiMainWindowFileActions::copyMainWindowToClipboard()
{
   QImage image;
   theMainWindow->getBrainModelOpenGL()->captureImage(image);
   QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
}      

/**
 * print main window image.
 */
void 
GuiMainWindowFileActions::printMainWindowImage()
{
#ifdef Q_OS_WIN32
   QTime timer;
   timer.start();
#endif // Q_OS_WIN32
   
   QPrinter printer;
   QPrintDialog pd(&printer, theMainWindow);
   if (pd.exec() == QDialog::Accepted) {
      QImage image;
      theMainWindow->getBrainModelOpenGL()->captureImage(image);
      
      QPainter painter(&printer);
      painter.drawImage(0, 0, image);
   }
#ifdef Q_OS_WIN32
   if (timer.elapsed() < 2000) {
      QString msg = "If you did not see the Print Dialog, then printing\n"
                    "will not work on your computer.  This is a problem\n"
                    "seen on Windows versions of Caret5 and there is no\n"
                    "solution to this problem.";
      QMessageBox::critical(theMainWindow, "ERROR", msg);
   }
#endif // Q_OS_WIN32
}      

/**
 * add document to spec file action.
 */
void 
GuiMainWindowFileActions::addDocumentToSpecFileSlot()
{
   QStringList allFileFilters;
   FileFilters::getAllFileFilters(allFileFilters);

   WuQFileDialog fd(theMainWindow);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(".");
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilters(allFileFilters);
   fd.setLabelText(WuQFileDialog::Accept, "Add");
   if (fd.exec() == WuQFileDialog::Accepted) {
      if (fd.selectedFiles().size() > 0) {
         const QString name = fd.selectedFiles().at(0);
         theMainWindow->getBrainSet()->addDocumentFile(name);
      }
   }
}
      
/**
 * Launch a file selection dialog to choose a spec file.
 */
void
GuiMainWindowFileActions::openSpecFileSlot()
{
   GuiChooseSpecFileDialog csfd(theMainWindow, 
                                theMainWindow->getBrainSet()->getPreferencesFile(), 
                                true,
                                true);
   if (csfd.exec() == QDialog::Accepted) {
      std::vector<QString> specFileNames = csfd.getSelectedSpecFiles();
      const int numSpecFiles = static_cast<int>(specFileNames.size());
      if (numSpecFiles == 1) {
         theMainWindow->readSpecFile(specFileNames[0]);
      }
      else {
         //
         // If data files loaed allow append or replace
         //
         QMessageBox msgBox(theMainWindow);
         msgBox.setWindowTitle("Spec File Open");
         msgBox.setText("You have selected multiple spec files.  Do "
                                     "you want to load all files in each of the "
                                     "spec files or pick and choose files in each"
                                     "of the spec files?");
         QPushButton* loadAllPushButton = msgBox.addButton("Load All", 
                                                           QMessageBox::ActionRole);
         QPushButton* pickAndChoosePushButton = msgBox.addButton("Pick and Choose", 
                                                           QMessageBox::ActionRole);
         QPushButton* cancelPushButton = msgBox.addButton("Cancel", 
                                                           QMessageBox::ActionRole);
         msgBox.exec();
         int answer = 0;
         if (msgBox.clickedButton() == loadAllPushButton) {
            answer = 0;
         }
         else if (msgBox.clickedButton() == pickAndChoosePushButton) {
            answer = 1;
         }
         else if (msgBox.clickedButton() == cancelPushButton) {
            answer = 2;
         }
         if (answer == 0) {
            QString msg;
            
            for (int i = 0; i < numSpecFiles; i++) {
               try {
                  SpecFile sf;
                  QString path(FileUtilities::dirname(specFileNames[i]));
                  if (QDir::isRelativePath(path)) {
                     path = QDir::currentPath();
                     path.append("/"); 
                     path.append(specFileNames[i]);
                     specFileNames[i] = QDir::cleanPath(path);
                  }
                  sf.readFile(specFileNames[i]);
                  sf.setAllFileSelections(SpecFile::SPEC_TRUE);
                  theMainWindow->loadSpecFilesDataFiles(sf, NULL, true);
               }
               catch (FileException& e) {
                  msg += e.whatQString() + "\n";
               }
            }  
            
            if (msg.isEmpty() == false) {
               QMessageBox::warning(theMainWindow, "ERROR", msg);
            }
         }
         else if (answer == 1) {
            for (int i = 0; i < numSpecFiles; i++) {
               theMainWindow->readSpecFile(specFileNames[i]);
            }
         }
      }
   }
}

/**
 * Popup the view current files dialog.
 */
void
GuiMainWindowFileActions::loadedFileManagementSlot()
{
   GuiLoadedFileManagementDialog* lfmd = new GuiLoadedFileManagementDialog(theMainWindow);
   lfmd->show();
}

/**
 * update the actions (typically called when menu is about to show)
 */
void 
GuiMainWindowFileActions::updateActions()
{
   QString name("Close Spec File");
   if (theMainWindow != NULL) {
      BrainSet* activeBrainSet = theMainWindow->getBrainSet();
      if (activeBrainSet != NULL) {
         QString specName(activeBrainSet->getSpecFileName());
         if (specName.length() > 1) {
            name.append(" ");
            name.append(FileUtilities::basename(specName));
         }
      }
   }
   closeSpecFileAction->setText(name);
}

