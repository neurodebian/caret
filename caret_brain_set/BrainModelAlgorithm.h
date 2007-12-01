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



#ifndef __VE_BRAIN_MODEL_ALGORITHM_H__
#define __VE_BRAIN_MODEL_ALGORITHM_H__

#include <QObject>

#include "BrainModelAlgorithmException.h"

class QProgressDialog;

class BrainSet;
class QString;

/// Abstract class for algorithms that operate on brain models
class BrainModelAlgorithm : public QObject {
   Q_OBJECT
   
   public:
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException) = 0;
      
      /// Destructor
      virtual ~BrainModelAlgorithm();
      
      /// get warnings from execution
      QString getWarningMessages() const { return warningMessages; }
   
      /// get a text description of algorithm
      virtual QString getTextDescription() const;
      
   signals:
      /// signal emitted to indicate progress
      void signalProgressUpdate(const QString& text,
                                const int progressNumber,
                                const int totalNumberOfSteps);
      
   protected:
      /// Constructor
      BrainModelAlgorithm(BrainSet* bs);
      
      /// Create the progress dialog.
      void createProgressDialog(const QString& title,
                                const int totalNumberOfSteps,
                                const QString& dialogName);
                                
      /// Update the progress dialog.  Throws exception if cancel button pressed by user.
      void updateProgressDialog(const QString& text,
                                const int progressNumber = -1,
                                const int totalNumberOfSteps = -1) throw (BrainModelAlgorithmException);
       
      /// Remove and delete the progress dialog.
      void removeProgressDialog();

      /// add to warning messages (newline automatically added after message)
      void addToWarningMessages(const QString& msg);
      
      /// allow GUI events to process
      void allowEventsToProcess();
      
      /// the brain set
      BrainSet* brainSet;
      
      /// the progress dialog
      QProgressDialog* progressDialog;
      
      /// warnings encountered during algorithm execution
      QString warningMessages;
      
};

#endif // __VE_BRAIN_MODEL_ALGORITHM_H__

