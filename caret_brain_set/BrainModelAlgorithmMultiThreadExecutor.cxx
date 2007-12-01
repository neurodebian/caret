
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

#include "BrainModelAlgorithm.h"
#include "BrainModelAlgorithmMultiThreadExecutor.h"
#include "BrainModelAlgorithmRunAsThread.h"

/**
 * constructor.
 */
BrainModelAlgorithmMultiThreadExecutor::BrainModelAlgorithmMultiThreadExecutor(
                                       std::vector<BrainModelAlgorithm*> algorithmsIn,
                                       const int numberOfThreadsToRunIn,
                                       const bool stopIfAlogorithmThrowsExceptionIn)
{
   algorithms = algorithmsIn;
   numberOfThreadsToRun = numberOfThreadsToRunIn;
   if (numberOfThreadsToRun < 1) {
      numberOfThreadsToRun = 1;
   }
   stopIfAlogorithmThrowsException = stopIfAlogorithmThrowsExceptionIn;
}

/**
 * destructor.
 */
BrainModelAlgorithmMultiThreadExecutor::BrainModelAlgorithmMultiThreadExecutor()
{
}

/**
 * start executing the threads.
 */
void 
BrainModelAlgorithmMultiThreadExecutor::startExecution()
{
   //
   // Count of algorithms 
   //
   const int numAlgorithmsToRun = static_cast<int>(algorithms.size());
   if (numAlgorithmsToRun <= 0) {
      return;
   }
   int nextAlgorithmToRun = 0;
   
   //
   // Pointers for algorithm threads
   //
   std::vector<BrainModelAlgorithmRunAsThread*> algorithmThreads(numberOfThreadsToRun);
   for (int i = 0; i < numberOfThreadsToRun; i++) {
      algorithmThreads[i] = NULL;
   }
   
   //
   // Time to wait for a thread
   //
   const unsigned long threadWaitTimeInMilliseconds = 5;
   
   //
   // Loop for creating and executing threads
   //
   bool done = false;
   while (done == false) {
      //
      // Loop through threads to see if any are done and create new threads
      //
      for (int iThread = 0; iThread < numberOfThreadsToRun; iThread++) {
         //
         // Is thread valid ?
         //
         if (algorithmThreads[iThread] != NULL) {
            //
            // Wait on the thread
            //
            algorithmThreads[iThread]->wait(threadWaitTimeInMilliseconds);
            
            //
            // Is thread finished ?
            //
            if (algorithmThreads[iThread]->isFinished()) {
               //
               // Get any error message
               //
               if (algorithmThreads[iThread]->getAlgorithmThrewAnException()) {
                  exceptionMessages.push_back(algorithmThreads[iThread]->getExceptionErrorMessage());
                  
                  //
                  // Should execution stop ?
                  //
                  if (stopIfAlogorithmThrowsException) {
                     nextAlgorithmToRun = numAlgorithmsToRun;
                  }
               }

               //
               // delete the thread (does not delete the algorithm)
               //
               delete algorithmThreads[iThread];
               algorithmThreads[iThread] = NULL;
            }
         }
         
         //
         // Is thread available
         //
         if (algorithmThreads[iThread] == NULL) {
            //
            // Are there algorithms that still need to run ?
            //
            if (nextAlgorithmToRun < numAlgorithmsToRun) {
               //
               // Create the new thread and start it
               //
               algorithmThreads[iThread] = 
                  new BrainModelAlgorithmRunAsThread(algorithms[nextAlgorithmToRun],
                                                     false);
               algorithmThreads[iThread]->start(QThread::HighestPriority);
               
               //
               // Inform caller of algorithm description
               //
               const QString s = algorithms[nextAlgorithmToRun]->getTextDescription();
               if (s.isEmpty() == false) {
                  emit algorithmStartedDescription(s);
               }
               
               //
               // Increment to next algorithm
               //
               nextAlgorithmToRun++;
            }
         }
      } // for (iThread
      
      //
      // Loop through the threads to see if any are still valid and if time to stop
      //
      done = true;
      for (int iThread = 0; iThread < numberOfThreadsToRun; iThread++) {
         if (algorithmThreads[iThread] != NULL) {
            done = false;
         }
      }

      //
      // Allow other events to process
      //
      QApplication::processEvents();
      
   } // while (done == false)
}

/**
 * get any exeception messages.
 */
void 
BrainModelAlgorithmMultiThreadExecutor::getExceptionMessages(std::vector<QString>& exceptionMessagesOut) const
{
   exceptionMessagesOut = exceptionMessages;
}