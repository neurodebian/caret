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

#ifndef __BRAIN_MODEL_ALGORITHM_MULTI_THREADED_H__
#define __BRAIN_MODEL_ALGORITHM_MULTI_THREADED_H__

#include <QMutex>
#include <QThread>

#include "BrainModelAlgorithm.h"

/// Abstract class for multi-threaded algorithms that work on brain models
class BrainModelAlgorithmMultiThreaded : public BrainModelAlgorithm,
                                         protected QThread {
   public:
      /// get the number of thread to run
      int getNumberOfThreadsToRun() const { return numberOfThreadsToRun; }
      
      /// set the number of thread to run
      void setNumberOfThreadsToRun(const int num) { numberOfThreadsToRun = num; }
      
      /// destructor
      virtual ~BrainModelAlgorithmMultiThreaded();
      
      /// Increment the number of children threads that have started their iterations
      void incrementNumChildThreadStarted();
      
      /// Increment the number of children thread that are done with their iteration.
      void incrementNumChildThreadDone();

   protected:
      //
      // initialize
      //
      
      /// constructor
      BrainModelAlgorithmMultiThreaded(BrainSet* bs,
                              BrainModelAlgorithmMultiThreaded* parentOfThisThreadIn,
                              int threadNumberIn,
                              const bool iAmAThread);
      
      /// get the parent of this thread
      BrainModelAlgorithmMultiThreaded* getParentOfThisThread()
                                                     { return parentOfThisThread; }

      /// get the thread keep looping flag
      bool getThreadKeepLoopingFlag() const { return threadKeepLoopingFlag; }
      
      /// set the thread keep looping flag
      void setThreadKeepLoopingFlag(const bool flag) { threadKeepLoopingFlag = flag; }
      
      /// get the number of this thread
      int getThreadNumber() const { return threadNumber; }
      
      /// Get the number of children threads that have started
      int getNumChildThreadStarted();
      
      /// Reset the number of children thread done (set it to zero).
      void resetNumChildThreadDone();
      
      /// Get the number of children thread that are done with their iteration.
      int getNumChildThreadDone();

      /// Get the threaded iteration done flag
      bool getThreadedIterationDoneFlag();

      /// Set the threaded iteration done flag
      void setThreadedIterationDoneFlag(const bool flag);

      /// see if i'm a thread instance
      bool getImAThread() const { return threadFlag; }
      
private:      
      /// number of threads to run
      int numberOfThreadsToRun;

      /// thread flag (set if this instance is run as a thread)
      bool threadFlag;
      
      /// mutex for accessing number of children thread started
      QMutex mutexNumChildThreadStarted;
      
      /// number of children threads started
      int numChildThreadStarted;

      /// number of children thread done
      int numChildThreadDone;
      
      /// mutex for accessing number of children thread done
      QMutex mutexNumChildThreadDone;
      
      /// threaded iteration done flag
      bool threadedIterationDoneFlag;
      
      /// mutex for accessing the threaded iteration done flag
      QMutex mutexThreadedIterationDoneFlag;

      /// number of this thread
      int threadNumber;
      
      /// used to remain in run() when running as a thread
      bool threadKeepLoopingFlag;
      
      /// parent of this thread
      BrainModelAlgorithmMultiThreaded* parentOfThisThread;
      
      
      
};

#endif // __BRAIN_MODEL_ALGORITHM_MULTI_THREADED_H__

