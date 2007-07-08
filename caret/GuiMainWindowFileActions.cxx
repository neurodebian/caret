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
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>

#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelViewingWindow.h"
#include "GuiCaptureMainWindowImageDialog.h"
#include "GuiChooseSpecFileDialog.h"
#include "GuiConvertDataFileDialog.h"
#include "GuiCopySpecFileDialog.h"
#include "GuiExportDataFileDialog.h"
#include "GuiFilesModified.h"
#include "GuiGiftiFileDialog.h"
#include "GuiImageFileOpenSaveDialog.h"
#include "GuiImportDataFileDialog.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowFileActions.h"
#include "GuiMessageBox.h"
#include "GuiOpenDataFileDialog.h"
#include "GuiSaveDataFileDialog.h"
#include "GuiPreferencesDialog.h"
#include "GuiSpecFileDialog.h"
#include "GuiViewCurrentFilesDialog.h"
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
   QObject::connect(fastOpenDataFileAction, SIGNAL(activated()),
                    parent, SLOT(displayFastOpenDataFileDialog()));

   showPreferencesDialogAction = new QAction(parent);
   showPreferencesDialogAction->setText("Preferences...");
   showPreferencesDialogAction->setObjectName("showPreferencesDialogAction");
   QObject::connect(showPreferencesDialogAction, SIGNAL(activated()),
                    parent, SLOT(displayPreferencesDialog()));

   setCurrentDirectoryAction = new QAction(parent);
   setCurrentDirectoryAction->setText("Set Current Directory...");
   setCurrentDirectoryAction->setObjectName("setCurrentDirectoryAction");
   QObject::connect(setCurrentDirectoryAction, SIGNAL(activated()),
                    this, SLOT(slotSetCurrentDirectory()));

   convertDataFileAction = new QAction(parent);
   convertDataFileAction->setText("Convert Data File Formats...");
   convertDataFileAction->setObjectName("convertDataFileAction");
   QObject::connect(convertDataFileAction, SIGNAL(activated()),
                    this, SLOT(convertDataFileSlot()));

   copySpecFileAction = new QAction(parent);
   copySpecFileAction->setText("Copy Spec File...");
   copySpecFileAction->setObjectName("copySpecFileAction");
   QObject::connect(copySpecFileAction, SIGNAL(activated()),
                    this, SLOT(copySpecFileSlot()));

   validateSpecFileAction = new QAction(parent);
   validateSpecFileAction->setText("Validate a Spec File...");
   validateSpecFileAction->setObjectName("validateSpecFileAction");
   QObject::connect(validateSpecFileAction, SIGNAL(activated()),
                    this, SLOT(validateSpecFileSlot()));

   zipSpecFileAction = new QAction(parent);
   zipSpecFileAction->setText("Zip a Spec File...");
   zipSpecFileAction->setObjectName("zipSpecFileAction");
   QObject::connect(zipSpecFileAction, SIGNAL(activated()),
                    this, SLOT(zipSpecFileSlot()));

   importFileAction = new QAction(parent);
   importFileAction->setText("Import File...");
   importFileAction->setObjectName("importFileAction");
   QObject::connect(importFileAction, SIGNAL(activated()),
                    this, SLOT(importFileSlot()));

   recordAsMpegAction = new QAction(parent);
   recordAsMpegAction->setText("Record Main Window Images as Movie...");
   recordAsMpegAction->setObjectName("recordAsMpegAction");
   QObject::connect(recordAsMpegAction, SIGNAL(activated()),
                    this, SLOT(recordAsMpegSlot()));

   exportFileAction = new QAction(parent);
   exportFileAction->setText("Export File...");
   exportFileAction->setObjectName("exportFileAction");
   QObject::connect(exportFileAction, SIGNAL(activated()),
                    this, SLOT(exportFileSlot()));

   openDataFileAction = new QAction(parent);
   openDataFileAction->setText("Open Data File...");
   openDataFileAction->setShortcut(Qt::CTRL+Qt::Key_O);
   openDataFileAction->setObjectName("openDataFileAction");
   QObject::connect(openDataFileAction, SIGNAL(activated()),
                    this, SLOT(openDataFileSlot()));

   saveDataFileAction = new QAction(parent);
   saveDataFileAction->setText("Save Data File...");
   saveDataFileAction->setShortcut(Qt::CTRL+Qt::Key_S);
   saveDataFileAction->setObjectName("saveDataFileAction");
   QObject::connect(saveDataFileAction, SIGNAL(activated()),
                    this, SLOT(saveDataFileSlot()));

   openImageFileAction = new QAction(parent);
   openImageFileAction->setText("Open Image Files...");
   openImageFileAction->setObjectName("openImageFileAction");
   QObject::connect(openImageFileAction, SIGNAL(activated()),
                    this, SLOT(openImageFileSlot()));

   copyMainWindowToClipboardAction = new QAction(parent);
   copyMainWindowToClipboardAction->setText("Copy Main Window Image to Clipboard");
   copyMainWindowToClipboardAction->setObjectName("copyMainWindowToClipboardAction");
   QObject::connect(copyMainWindowToClipboardAction, SIGNAL(activated()),
                    this, SLOT(copyMainWindowToClipboard()));

   printMainWindowAction = new QAction(parent);
   printMainWindowAction->setText("Print Main Window...");
   printMainWindowAction->setObjectName("printMainWindowAction");
   QObject::connect(printMainWindowAction, SIGNAL(activated()),
                    this, SLOT(printMainWindowImage()));

   saveImageFileAction = new QAction(parent);
   saveImageFileAction->setText("Save Image File...");
   saveImageFileAction->setObjectName("saveImageFileAction");
   QObject::connect(saveImageFileAction, SIGNAL(activated()),
                    this, SLOT(saveImageFileSlot()));

   saveWindowAsImageAction = new QAction(parent);
   saveWindowAsImageAction->setText("Save Main Window to Image File...");
   saveWindowAsImageAction->setObjectName("saveWindowAsImageAction");
   QObject::connect(saveWindowAsImageAction, SIGNAL(activated()),
                    this, SLOT(saveWindowAsImageFileSlot()));

   captureMainWindowImageAction = new QAction(parent);
   captureMainWindowImageAction->setText("Capture Image of Main Window...");
   captureMainWindowImageAction->setObjectName("captureMainWindowImageAction");
   QObject::connect(captureMainWindowImageAction, SIGNAL(activated()),
                    this, SLOT(captureMainWindowImageSlot()));
                    
   openSpecFileAction = new QAction(parent);
   openSpecFileAction->setText("Open Spec File...");
   openSpecFileAction->setShortcut(Qt::CTRL+Qt::Key_N);
   openSpecFileAction->setObjectName("openSpecFileAction");
   QObject::connect(openSpecFileAction, SIGNAL(activated()),
                    this, SLOT(openSpecFileSlot()));

   addDocumentToSpecFileAction = new QAction(parent);
   addDocumentToSpecFileAction->setText("Add Document File to Spec File...");
   addDocumentToSpecFileAction->setObjectName("addDocumentToSpecFileAction");
   QObject::connect(addDocumentToSpecFileAction, SIGNAL(activated()),
                    this, SLOT(addDocumentToSpecFileSlot()));
                    
   closeSpecFileAction = new QAction(parent);
   closeSpecFileAction->setText("Close Current Spec File");
   closeSpecFileAction->setObjectName("closeSpecFileAction");
   QObject::connect(closeSpecFileAction, SIGNAL(activated()),
                    parent, SLOT(slotCloseSpecFile()));

   viewCurrentFilesAction = new QAction(parent);
   viewCurrentFilesAction->setText("View Current Files...");
   viewCurrentFilesAction->setObjectName("viewCurrentFilesAction");
   QObject::connect(viewCurrentFilesAction, SIGNAL(activated()),
                    this, SLOT(viewCurrentFilesSlot()));

   exitCaretAction = new QAction(parent);
   exitCaretAction->setText("Exit...");
   exitCaretAction->setShortcut(Qt::CTRL+Qt::Key_Q);
   exitCaretAction->setObjectName("exitCaretAction");
   QObject::connect(exitCaretAction, SIGNAL(activated()),
                    parent, SLOT(slotCloseProgram()));  //SLOT(close()));

   openGiftiFileDialogAction = new QAction(parent);
   openGiftiFileDialogAction->setText("Open GIFTI File...");
   openGiftiFileDialogAction->setObjectName("openGiftiFile");
   QObject::connect(openGiftiFileDialogAction, SIGNAL(activated()),
                    this, SLOT(openGiftiFileSlot()));

   saveGiftiFileDialogAction = new QAction(parent);
   saveGiftiFileDialogAction->setText("Save GIFTI File...");
   saveGiftiFileDialogAction->setObjectName("saveGiftiFile");
   QObject::connect(saveGiftiFileDialogAction, SIGNAL(activated()),
                    this, SLOT(saveGiftiFileSlot()));
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
   QFileDialog fd(theMainWindow);
   fd.setModal(true);
   fd.setAcceptMode(QFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   fd.setWindowTitle("Choose Directory");
   fd.setFileMode(QFileDialog::DirectoryOnly);
   if (fd.exec() == QDialog::Accepted) {
      QDir::setCurrent(fd.selectedFiles().at(0));
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
            GuiMessageBox::information(theMainWindow, "SUCCESS", "Spec File is Valid.", "OK");
         }
         else {
            GuiMessageBox::critical(theMainWindow, "ERROR", msg, "OK");
         }
      }
      catch (FileException& e) {
         GuiMessageBox::critical(theMainWindow, "ERROR", e.whatQString(), "OK");
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
 * Import file slot.
 */
void
GuiMainWindowFileActions::importFileSlot()
{
   static GuiImportDataFileDialog* fd = NULL;
   if (fd == NULL) {
      fd = new GuiImportDataFileDialog(theMainWindow);
   }
   fd->show();
   fd->activateWindow();
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
 * Export file slot.
 */
void
GuiMainWindowFileActions::exportFileSlot()
{
   static GuiExportDataFileDialog* fd = NULL;
   if (fd == NULL) {
      fd = new GuiExportDataFileDialog(theMainWindow);
   }
   fd->show();
   fd->activateWindow();
}

/**
 * Open Data file slot.
 */
void
GuiMainWindowFileActions::openDataFileSlot()
{
   static GuiOpenDataFileDialog* fd = NULL;
   if (fd == NULL) {
      fd = new GuiOpenDataFileDialog(theMainWindow);
   }
   fd->show();
   fd->activateWindow();
}

/**
 * Save Data file slot.
 */
void
GuiMainWindowFileActions::saveDataFileSlot()
{
   static GuiSaveDataFileDialog* fd = NULL;
   if (fd == NULL) {
      fd = new GuiSaveDataFileDialog(theMainWindow);   
   }
   fd->show();
   fd->activateWindow();
}

/**
 * Open Image File slot.
 */
void
GuiMainWindowFileActions::openImageFileSlot()
{
   static GuiImageFileOpenSaveDialog* sd = NULL;
   if (sd == NULL) {
      sd = new GuiImageFileOpenSaveDialog(theMainWindow, GuiImageFileOpenSaveDialog::DIALOG_MODE_OPEN_FILES);
   }
   sd->show();
   sd->activateWindow();
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
      GuiMessageBox::critical(theMainWindow, "ERROR", msg, "OK");
   }
#endif // Q_OS_WIN32
}      

/**
 * Called to save an image file.
 */
void 
GuiMainWindowFileActions::saveImageFileSlot()
{
   static GuiImageFileOpenSaveDialog* sd = NULL;
   if (sd == NULL) {
      sd = new GuiImageFileOpenSaveDialog(theMainWindow, GuiImageFileOpenSaveDialog::DIALOG_MODE_SAVE_LOADED_IMAGE);
   }
   sd->show();
   sd->activateWindow();
}      

/**
 * Save Window as Image File slot.
 */
void
GuiMainWindowFileActions::saveWindowAsImageFileSlot()
{
   static GuiImageFileOpenSaveDialog* sd = NULL;
   if (sd == NULL) {
      sd = new GuiImageFileOpenSaveDialog(theMainWindow, GuiImageFileOpenSaveDialog::DIALOG_MODE_SAVE_IMAGE_OF_MAIN_WINDOW);
   }
   sd->show();
   sd->activateWindow();
}

/**
 * add document to spec file action.
 */
void 
GuiMainWindowFileActions::addDocumentToSpecFileSlot()
{
   QFileDialog fd(theMainWindow);
   fd.setAcceptMode(QFileDialog::AcceptOpen);
   fd.setDirectory(".");
   fd.setFileMode(QFileDialog::ExistingFile);
   fd.setFilter("Any File (*)");
   fd.setLabelText(QFileDialog::Accept, "Add");
   if (fd.exec() == QFileDialog::Accepted) {
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
   GuiChooseSpecFileDialog csfd(theMainWindow, theMainWindow->getBrainSet()->getPreferencesFile(), true);
   if (csfd.exec() == QDialog::Accepted) {
      theMainWindow->readSpecFile(csfd.getSelectedSpecFile());
   }
}

/**
 * Popup the view current files dialog.
 */
void
GuiMainWindowFileActions::viewCurrentFilesSlot()
{
   GuiViewCurrentFilesDialog* vfd = new GuiViewCurrentFilesDialog(theMainWindow);
   vfd->show();
}

/**
 * capture image of main window.
 */
void 
GuiMainWindowFileActions::captureMainWindowImageSlot()
{
   GuiCaptureMainWindowImageDialog* cmwid = NULL;
   if (cmwid == NULL) {
      cmwid = new GuiCaptureMainWindowImageDialog(theMainWindow);
      QtUtilities::positionWindowOffOtherWindow(theMainWindow, cmwid);
   }
   cmwid->show();
   cmwid->activateWindow();
}

/**
 * open gifti file slot.
 */
void 
GuiMainWindowFileActions::openGiftiFileSlot()
{
   static GuiGiftiFileDialog* gd = NULL;
   if (gd == NULL) {
      gd = new GuiGiftiFileDialog(theMainWindow,
                                  GuiGiftiFileDialog::DIALOG_MODE_FILE_OPEN);
   }
   gd->show();
   gd->activateWindow();
}

/**
 * save gifti file slot.
 */
void 
GuiMainWindowFileActions::saveGiftiFileSlot()
{
   static GuiGiftiFileDialog* gd = NULL;
   if (gd == NULL) {
      gd = new GuiGiftiFileDialog(theMainWindow,
                                  GuiGiftiFileDialog::DIALOG_MODE_FILE_SAVE);
   }
   gd->show();
   gd->activateWindow();
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
   
   saveImageFileAction->setEnabled(theMainWindow->getBrainSet()->getNumberOfImageFiles() > 0);
}

