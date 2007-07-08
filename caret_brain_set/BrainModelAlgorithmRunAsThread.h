
#ifndef __BRAIN_MODEL_ALGORITHM_RUN_AS_THREAD_H__
#define __BRAIN_MODEL_ALGORITHM_RUN_AS_THREAD_H__

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

#include <QThread>

class BrainModelAlgorithm;

/// class that runs a brain model algorithm in a thread
class BrainModelAlgorithmRunAsThread : public QThread {
   public:
      // constructor
      BrainModelAlgorithmRunAsThread(BrainModelAlgorithm* algorithmToRunIn,
                                     const bool deleteBrainModelAlgorithmInDestructorFlagIn);
      
      // destructor
      ~BrainModelAlgorithmRunAsThread();
      
      // call after thread finishes to see if an exception occurred during algorithm execution
      bool getAlgorithmThrewAnException() const { return exceptionThrownFlag; }
      
      // get the exception error message
      QString getExceptionErrorMessage() const { return exceptionMessage; }
      
      // get the algorithm
      BrainModelAlgorithm* getBrainModelAlgorithm() { return algorithmToRun; }
      
      // get the algorithm (const method)
      const BrainModelAlgorithm* getBrainModelAlgorithm() const { return algorithmToRun; }
      
   protected:
      /// runs the algorithm
      void run();
      
      /// the algorithm that is to be run in a thread
      BrainModelAlgorithm* algorithmToRun;
      
      /// will get set if algorithm throws an exception
      bool exceptionThrownFlag;
      
      /// will get set if algorithm throws an exception
      QString exceptionMessage;
      
      /// delete the brain model algorithm in the destructor
      bool deleteBrainModelAlgorithmInDestructorFlag;
};

#endif // __BRAIN_MODEL_ALGORITHM_RUN_AS_THREAD_H__
