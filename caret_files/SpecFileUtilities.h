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



#ifndef __SPEC_FILE_UTILITIES_H__
#define __SPEC_FILE_UTILITIES_H__

#include <QString>
#include <vector>

#include "SpecFile.h"

/// This class contains methods for assistance with spec files
class SpecFileUtilities {
   public:
      enum MODE_COPY_SPEC_FILE {
         MODE_COPY_SPEC_FILE_NONE,
         MODE_COPY_SPEC_FILE_COPY_ALL,
         MODE_COPY_SPEC_FILE_MOVE_ALL,
         MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES
      };
      
      /// copy a spec file (returns true if an error occurred)
      static bool copySpecFile(const QString& sourceSpecFileIn,
                        const QString& targetSpecFileIn,
                        const MODE_COPY_SPEC_FILE copySpecFileModeIn,
                        QString& errorMessage,
                        const bool oneMustBeCurrentDirectory = false,
                        const bool verboseMode = true);
   
      /// zip a spec file (returns true if an error occurred)
      static bool zipSpecFile(const QString& specFileToZip,
                              const QString& zipFileName,
                              const QString& unzipDirName,
                              QString& errorMessage,
                              const QString& zipCommand = "",
                              const QString& temporaryDirectory = "");
      
      /// find spec files in specified directory
      static void findSpecFilesInDirectory(const QString& directory,
                                    std::vector<QString>& files);
                                    
      /// set the volume data file name if it is not already set
      static void setVolumeDataFileName(const QString& directoryName,
                                        const QString& headerFileName,
                                        QString& dataFileName);
                                 
   private:
      /// copy a group of data files
      static void copySpecFileDataFiles(SpecFile::Entry& fileEntry,
                                        const bool verboseMode,
                                        QString& errorMessageOut);
      
      /// copy a spec file data file
      static void copySpecFileCopyDataFile(QString& fileName,
                                           const QString& dataFileNameIn,
                                           const SpecFile::Entry::FILE_TYPE ft,
                                           const bool verboseMode,
                                           QString& errorMessageOut);
      
      /// copy spec file mode
      static MODE_COPY_SPEC_FILE copySpecFileMode;
      
      /// copy spec file mode source path
      static QString copySpecSourcePath;
      
      /// copy spec file mode target path
      static QString copySpecTargetPath;

};


#endif // __SPEC_FILE_UTILITIES_H__

#ifdef __SPEC_FILE_UTILITIES_MAIN__
SpecFileUtilities::MODE_COPY_SPEC_FILE SpecFileUtilities::copySpecFileMode;
QString SpecFileUtilities::copySpecSourcePath;
QString SpecFileUtilities::copySpecTargetPath;
#endif // __SPEC_FILE_UTILITIES_MAIN__

