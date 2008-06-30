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


#ifndef __BRAIN_MODEL_RUN_EXTERNAL_PROGRAM_H__
#define __BRAIN_MODEL_RUN_EXTERNAL_PROGRAM_H__

#include <QObject>
#include <QStringList>

#include "BrainModelAlgorithm.h"

/// Class to run an external program
class BrainModelRunExternalProgram : public BrainModelAlgorithm  {
   Q_OBJECT
   
   public:
      
      /// Constructor
      BrainModelRunExternalProgram(const QString& programNameIn,
                                   const QStringList& programArgumentsIn,
                                   const bool programIsInCaretSlashBinDirectoryFlagIn);
                                       
      /// Destructor
      ~BrainModelRunExternalProgram();
      
      // execute the utility program.
      void execute() throw (BrainModelAlgorithmException);
      
      /// get output of command
      QString getOutputText() const { return outputText; }
      
   protected:
      /// program name
      QString programName;
      
      /// program arguments
      QStringList programArguments;
      
      /// wait until program is finished before returning from execute()
      bool waitUntilDone;
      
      /// output of program
      QString outputText;
      
      /// program is in caret/bin directory flag
      bool programIsInCaretSlashBinDirectoryFlag;
};

#endif // __BRAIN_MODEL_RUN_EXTERNAL_PROGRAM_H__

