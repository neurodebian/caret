
#ifndef __BRAIN_MODEL_ALGORITHM_MULTI_THREAD_EXECUTOR_H__
#define __BRAIN_MODEL_ALGORITHM_MULTI_THREAD_EXECUTOR_H__

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

#include <QObject>
#include <QString>

class BrainModelAlgorithm;

/// class for executing brain model algorithms in parallel
class BrainModelAlgorithmMultiThreadExecutor : public QObject {
   Q_OBJECT
   
   public:
      // constructor
      BrainModelAlgorithmMultiThreadExecutor(std::vector<BrainModelAlgorithm*> algorithmsIn,
                                             const int numberOfThreadsToRunIn,
                                             const bool stopIfAlogorithmThrowsExceptionIn);
      
      // destructor
      BrainModelAlgorithmMultiThreadExecutor();
   
      // start executing the threads
      void startExecution();
      
      // get any exeception messages
      void getExceptionMessages(std::vector<QString>& exceptionMessagesOut) const;
      
   signals:
      // emits algorithm description (if non-blank) when algorithm starts
      void algorithmStartedDescription(const QString&);
      
   protected:
      // the algorithms
      std::vector<BrainModelAlgorithm*> algorithms;
      
      // number of simultaneous threads
      int numberOfThreadsToRun;
      
      // stop executing if an algorithms throws an exception
      bool stopIfAlogorithmThrowsException;
      
      // exeception messages
      std::vector<QString> exceptionMessages;
   
};

#endif // __BRAIN_MODEL_ALGORITHM_MULTI_THREAD_EXECUTOR_H__
