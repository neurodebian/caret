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



#ifndef __FILE_UTILITIES_H__
#define __FILE_UTILITIES_H__

#include <map>
#include <QString>
#include <vector>

#include <QString>

/// class contains static methods for operations dealing with files
class FileUtilities {
   public:

      /// copy (or move) a file  (returns true if an error occurs)
      static bool copyFile(const QString& inputName,
                           const QString& outputName,
                           const bool moveFileFlag = false,
                           const bool verboseMode = false);

      /// download a file specified by a URL (returns true if successful)
      static bool downloadFileWithHttpGet(const QString& fileUrl, 
                               const QString& saveFileInDirectoryName,
                               const int maxTimeToWait,
                               QString& errorMessageOut,
                               std::map<QString,QString>* headerTags = NULL,
                               int* returnCode = NULL);
                               
      /// download file to a QString
      static bool downloadFileWithHttpGet(const QString& fileUrl,
                        const int maxTimeToWait,
                        QString& fileContentsOut,
                        QString& errorMessageOut,
                        std::map<QString,QString>* headerTags = NULL,
                        int* returnCode = NULL);
                                              
      /// download file to a C++ string
      static bool downloadFileWithHttpPost(const QString& fileUrl,
                        const QString& postContentIn,
                        const int maxTimeToWait,
                        QString& fileContentsOut,
                        QString& errorMessageOut,
                        const std::map<QString,QString>* additionalHeaderTagsIn = NULL,
                        std::map<QString,QString>* headerTags = NULL,
                        int* returnCode = NULL);
                                              
      /// find files int the specified directory
      static void findFilesInDirectory(const QString& dirName, 
                                       const QStringList& regExpMatchList,
                                       std::vector<QString>& matchingFiles);
                               
      /// return result of making "myPathIn" relative to "otherPathIn"
      static void relativePath(const QString& otherPathIn,
                           const QString& myPathIn,
                           QString& result);
   
   
      /// return filename with path removed.
      static QString basename(const QString& s);
   
      /// return path of a filename.
      static QString dirname(const QString& s);
   
      /// return the filename's extension
      static QString filenameExtension(const QString& s);
      
      /// return the filename without extension
      static QString filenameWithoutExtension(const QString& s);
      
      /// parse a data file name to see if it is a valid Caret file name.
      /// Returns true if the file's name is a valid Caret name.
      static bool parseCaretDataFileName(const QString& filenameIn,
                                         QString& directory,
                                         QString& species,
                                         QString& casename,
                                         QString& anatomy,
                                         QString& hemisphere,
                                         QString& description,
                                         QString& descriptionNoTypeName,
                                         QString& theDate,
                                         QString& numNodes,
                                         QString& extension);
   
      /// Reassemble the Caret data file name from the components.
      static QString reassembleCaretDataFileName(
                                         const QString& directory,
                                         const QString& species,
                                         const QString& casename,
                                         const QString& anatomy,
                                         const QString& hemisphere,
                                         const QString& description,
                                         const QString& theDate,
                                         const QString& numNodes,
                                         const QString& extension);
   
      /// Change the type name in the description string of the caret file name.
      static QString changeCaretDataFileDescriptionType(const QString& filename,
                                                     const QString& newDescription);

      /// Change the description in the name of a caret file name.
      static QString changeCaretDataFileDescription(const QString& filename,
                                                    const QString& newDescription);

      /// turn "path/file" into "file (path)"
      static QString rearrangeFileName(const QString& fileName,
                                           const int maxPathLength = 10000);
   
      /// create a directory (returns true if successful)
      static bool createDirectory(const QString& dirPath);
      
      /// see if a directory contains files 
      static bool directoryContainsFiles(const QString& dirPath);
      
      /// gunzip a file (returns true if successful)
      static bool gunzipFile(const QString& inputName, const QString& outputName);
      
      /// Get subdirectory prefix.
      static QString getSubdirectoryPrefix(const QString& s);
      
      /// Return the directory separator for the current platform
      static QString directorySeparator();
      
      /// deterimine if a path is an absolute path
      static bool isAbsolutePath(const QString& name);
      
      // return the name of a temporary directory (empty if invalid)
      static QString temporaryDirectory();
      
      // see if a file contains the specified text
      static bool findTextInFile(const QString& fileName,
                                 const QString& text,
                                 const bool caseSensitiveFlag);

      // Remove the old extension if it is present.  Add new extension.
      static QString replaceExtension(const QString fileName,
                                      const QString oldExtension,
                                      const QString newExtension);

};

#endif // __FILE_UTILITIES_H__

