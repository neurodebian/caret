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
#include <QProgressDialog>
#include <QString>

#include "BrainModelAlgorithm.h"
#include "BrainSet.h"

/**
 * Constructor.
 */
BrainModelAlgorithm::BrainModelAlgorithm(BrainSet* bs) 
   : QObject(0)
{ 
   brainSet = bs; 
   progressDialog = NULL;
   warningMessages = "";
}

/**
 * Destructor.
 */
BrainModelAlgorithm::~BrainModelAlgorithm() 
{ 
   removeProgressDialog();
}
      
/**
 * add to warning messages (newline automatically added after message).
 */
void 
BrainModelAlgorithm::addToWarningMessages(const QString& msg)
{
   if (warningMessages.isEmpty() == false) {
      warningMessages += "\n";
   }
   warningMessages += msg;
}

/**
 * Create the progress dialog.
 */
void
BrainModelAlgorithm::createProgressDialog(const QString& title,
                                          const int totalNumberOfSteps,
                                          const QString& /*dialogName*/)
{
   //
   // See if a progress dialog should be displayed
   //
   progressDialog = NULL;
   QWidget* progressDialogParent = brainSet->getProgressDialogParent();
   if (progressDialogParent != NULL) {      
      progressDialog = new QProgressDialog(title,
                                           "Cancel",
                                           0,
                                           totalNumberOfSteps,
                                           progressDialogParent);
      progressDialog->move(progressDialogParent->pos());
      progressDialog->setWindowTitle(title);
      progressDialog->setValue(0);
      progressDialog->setLabelText("");
      progressDialog->show();
   }
}
   
/**
 * Update the progress dialog.  Returns true if cancel button pressed by user.
 */
void
BrainModelAlgorithm::updateProgressDialog(const QString& text,
                                          const int progressNumber,
                                          const int totalNumberOfSteps) throw (BrainModelAlgorithmException)
{
   //
   // Set progress dialog
   //
   if (progressDialog != NULL) {
      qApp->processEvents();  // note: qApp is global in QApplication
      
      //
      // See if progress dialog was cancelled
      //
      if (progressDialog->wasCanceled()) {
         removeProgressDialog();
         throw BrainModelAlgorithmException("Operation cancelled by user.");
      }

      //
      // update progress dialog
      //
      if (progressNumber >= 0) {
         if (totalNumberOfSteps >= 0) {
            progressDialog->setValue(progressNumber);
            progressDialog->setMaximum(totalNumberOfSteps);
         }
         else {
            progressDialog->setValue(progressNumber);
         }
      }
      progressDialog->setLabelText(text);
      qApp->processEvents();  // note: qApp is global in QApplication
   }
   else if (qApp != NULL) {
      if (qApp->type() == QApplication::Tty) {
        // std::cout << text.toAscii().constData() << std::endl;
      }
   }
   
   emit signalProgressUpdate(text, progressNumber, totalNumberOfSteps);
}

/**
 * Remove and delete the progress dialog.
 */
void
BrainModelAlgorithm::removeProgressDialog()
{
   //
   // Remove progress dialog
   //
   if (progressDialog != NULL) {
      progressDialog->setValue(1000000);
      delete progressDialog;
      progressDialog = NULL;
      qApp->processEvents();  // note: qApp is global in QApplication
   }
}

/**
 * allow GUI events to process.
 */
void 
BrainModelAlgorithm::allowEventsToProcess()
{
   if (brainSet->getProgressDialogParent() != NULL) {
      qApp->processEvents();
   }
}
      
