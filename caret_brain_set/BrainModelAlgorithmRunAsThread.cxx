
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

#include "BrainModelAlgorithm.h"
#include "BrainModelAlgorithmRunAsThread.h"

/**
 * constructor.
 */
BrainModelAlgorithmRunAsThread::BrainModelAlgorithmRunAsThread(BrainModelAlgorithm* algorithmToRunIn,
                                            const bool deleteBrainModelAlgorithmInDestructorFlagIn)
{
   algorithmToRun = algorithmToRunIn;
   deleteBrainModelAlgorithmInDestructorFlag = deleteBrainModelAlgorithmInDestructorFlagIn;
   exceptionThrownFlag = false;
   exceptionMessage = "";
}

/**
 * destructor.
 */
BrainModelAlgorithmRunAsThread::~BrainModelAlgorithmRunAsThread()
{
   if (deleteBrainModelAlgorithmInDestructorFlag) {
      delete algorithmToRun;
      algorithmToRun = NULL;
   }
}

/**
 * runs the algorithm.
 */
void 
BrainModelAlgorithmRunAsThread::run()
{
   if (algorithmToRun == NULL) {
      exceptionThrownFlag = true;
      exceptionMessage = "PROGRAM ERROR: Thread passed to constructor was NULL";
      return;
   }
   
   try {
      algorithmToRun->execute();
   }
   catch (BrainModelAlgorithmException& e) {
      exceptionThrownFlag = true;
      exceptionMessage = e.whatQString();
   }
}
