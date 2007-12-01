
#ifndef __BRAIN_SET_MULTI_THREADED_SPEC_FILE_READER_H__
#define __BRAIN_SET_MULTI_THREADED_SPEC_FILE_READER_H__

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

#include "SpecFile.h"

class BrainModelAlgorithm;
class BrainSet;
class QProgressDialog;

/// class for multi-hreaded reading of spec file data files
class BrainSetMultiThreadedSpecFileReader : public QObject {
   Q_OBJECT
   
   public:
      // constructor
      BrainSetMultiThreadedSpecFileReader(BrainSet* brainSetIn);
      
      // destructor
      ~BrainSetMultiThreadedSpecFileReader();
      
      // read the files
      void readDataFiles(const int numberOfThreads,
                         const SpecFile& specFile,
                         QProgressDialog* progressDialog,
                         std::vector<QString>& errorMessagesOut);
                         
   protected slots:
      // update the progress dialog
      void updateProgressDialog(const QString& message);
      
   protected:
      // add data files to the algorithm
      void addDataFiles(const SpecFile::Entry& specFileEntry);
      
      // read files
      void readFiles(const int numberOfThreads,
                     std::vector<QString>& errorMessagesOut);
                      
      // clear the file readers
      void clearFileReaders();
      
      // the brain set
      BrainSet* brainSet;
      
      // the file readers
      std::vector<BrainModelAlgorithm*> fileReaders;
      
      // the progress dialog
      QProgressDialog* progressDialog;
};

#endif // __BRAIN_SET_MULTI_THREADED_SPEC_FILE_READER_H__

