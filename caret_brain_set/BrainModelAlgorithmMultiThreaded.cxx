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

#include "BrainModelAlgorithmMultiThreaded.h"
#include "BrainSet.h"
#include "PreferencesFile.h"

/**
 * Constructor.
 */
BrainModelAlgorithmMultiThreaded::BrainModelAlgorithmMultiThreaded(BrainSet* bs,
                                       BrainModelAlgorithmMultiThreaded* parentOfThisThreadIn,
                                                                   int threadNumberIn,
                                                                   const bool iAmAThread)
   : BrainModelAlgorithm(bs)
{
   parentOfThisThread = parentOfThisThreadIn;
   threadNumber = threadNumberIn;
   threadFlag = iAmAThread;
   threadKeepLoopingFlag = false;
   
   numberOfThreadsToRun = 1;
   
   //
   // Get number of threads from preferences file
   //
   if (bs != NULL) {
      PreferencesFile* pf = bs->getPreferencesFile();
      numberOfThreadsToRun = pf->getMaximumNumberOfThreads();
   }
   
   numChildThreadStarted = 0;
   if (numberOfThreadsToRun > 1) {
      mutexNumChildThreadStarted.unlock();
   }
   
   resetNumChildThreadDone();
   if (numberOfThreadsToRun > 1) {
      mutexNumChildThreadDone.unlock();
   }
   
   threadedIterationDoneFlag = false;
   if (numberOfThreadsToRun > 1) {
      mutexThreadedIterationDoneFlag.unlock();
   }
}

/**
 * Destructor.
 */
BrainModelAlgorithmMultiThreaded::~BrainModelAlgorithmMultiThreaded()
{
}

/**
 * Get the number of children thread that have started.
 */
int
BrainModelAlgorithmMultiThreaded::getNumChildThreadStarted()
{
   mutexNumChildThreadStarted.lock();
   const int num = numChildThreadStarted;
   mutexNumChildThreadStarted.unlock();
   return num;
}

/**
 * Increment number of children thread that have started.
 */
void
BrainModelAlgorithmMultiThreaded::incrementNumChildThreadStarted()
{
   mutexNumChildThreadStarted.lock();
   numChildThreadStarted++;
   mutexNumChildThreadStarted.unlock();
}

/**
 * Reset the number of children thread done (set it to zero).
 */
void
BrainModelAlgorithmMultiThreaded::resetNumChildThreadDone()
{
   numChildThreadDone = 0;
}

/**
 * Get the number of children thread that are done with their iteration.
 */
int
BrainModelAlgorithmMultiThreaded::getNumChildThreadDone()
{
   mutexNumChildThreadDone.lock();
   const int num = numChildThreadDone;
   mutexNumChildThreadDone.unlock();
   return num;
}

/**
 * Increment number of children thread that are done with their iteration.
 */
void
BrainModelAlgorithmMultiThreaded::incrementNumChildThreadDone()
{
   mutexNumChildThreadDone.lock();
   numChildThreadDone++;
   mutexNumChildThreadDone.unlock();
}

/**
 * Get the threaded iteration done flag
 */
bool
BrainModelAlgorithmMultiThreaded::getThreadedIterationDoneFlag()
{
   mutexThreadedIterationDoneFlag.lock();
   const bool flag = threadedIterationDoneFlag;
   mutexThreadedIterationDoneFlag.unlock();
   return flag;
}

/**
 * Set the threaded iteration done flag
 */
void
BrainModelAlgorithmMultiThreaded::setThreadedIterationDoneFlag(const bool flag)
{
   mutexThreadedIterationDoneFlag.lock();
   threadedIterationDoneFlag = flag;
   mutexThreadedIterationDoneFlag.unlock();
}


