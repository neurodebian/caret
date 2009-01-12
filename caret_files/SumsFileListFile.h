
#ifndef __SUMS_FILE_LIST_FILE_H__
#define __SUMS_FILE_LIST_FILE_H__

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

class QDomElement;
class QDomNode;

#include "AbstractFile.h"

/// Class for storing download info for a file
class SumsFileInfo {
   public:
      /// file sorting
      enum SORTING_KEY {
         SORTING_KEY_DATE,
         SORTING_KEY_NAME,
         SORTING_KEY_TYPE
      };
      
      /// Constructor
      SumsFileInfo();
      
      /// Constructor
      ~SumsFileInfo();
      
      /// get the file's name without a path
      QString getNameWithoutPath() const { return fileNameWithoutPath; }
      
      /// set the file's name without a path
      void setNameWithoutPath(const QString& s);
      
      /// get the file's long name
      QString getNameWithPath() const { return fileNameWithPath; }
      
      /// set the file's long name
      void setNameWithPath(const QString& s) { fileNameWithPath = s; }
      
      /// get the file's URL
      QString getURL() const { return fileURL; }
      
      /// set the file's URL
      void setURL(const QString& s) { fileURL = s; }
      
      /// get the file's date
      QString getDate() const { return fileDate; }
      
      /// set the file's date
      void setDate(const QString& s) { fileDate = s; }
      
      /// get the file's comment
      QString getComment() const { return fileComment; }
      
      /// set the file's comment
      void setComment(const QString& s) { fileComment = s; }
      
      /// get validity
      bool isValid() const;
      
      /// get the file's size
      int getSize() const { return fileSize; }
      
      /// set the file's size
      void setSize(const int s) { fileSize = s; }
      
      /// get the file's state
      QString getState() const { return fileState; }
      
      /// set the file's state
      void setState(const QString& s) { fileState = s; }
      
      /// get the file's type name
      QString getTypeName() const { return fileTypeName; }
      
      /// set the file's type name
      void setTypeName(const QString& s) { fileTypeName = s; }
      
      /// get the file's selection status
      bool getSelected() const { return fileSelected; }
      
      /// set the file's selection status
      void setSelected(const bool s) { fileSelected = s; }
      
      /// get the ID
      QString getID() const { return fileID; }
      
      /// set the ID
      void setID(const QString& idin) { fileID = idin; }
      
      /// set the sorting key
      static void setSortingKey(const SORTING_KEY sk) { sortingKey = sk; }
      
      /// Comparison for sorting
      bool operator<(const SumsFileInfo& sfi) const;
      
   protected:
      /// name of file with path
      QString fileNameWithPath;
      
      /// name of file
      QString fileNameWithoutPath;
      
      /// URL of file
      QString fileURL;
      
      /// date of file
      QString fileDate;
      
      /// comment of file
      QString fileComment;
      
      /// size of the file
      int fileSize;
      
      /// state of the file
      QString fileState;
      
      /// type name of the file
      QString fileTypeName;
      
      /// the SuMS ID
      QString fileID;
      
      /// file selected
      bool fileSelected;
      
      /// sorting key
      static SORTING_KEY sortingKey;
};

/// Class for stroring file listings produced by the SuMS database
class SumsFileListFile : public AbstractFile {
   public:
      /// file sorting
      enum SORT_ORDER {
         SORT_ORDER_DATE,
         SORT_ORDER_NAME,
         SORT_ORDER_TYPE
      };
      
      /// Constructor
      SumsFileListFile();
      
      /// Destructor
      ~SumsFileListFile();
      
      /// clear the file
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return sumsFileInfo.empty(); }
      
      /// Add a file info to this file.
      void addSumsFile(const SumsFileInfo& sfi);

      /// get number of file info's
      int getNumberOfSumsFiles() const { return sumsFileInfo.size(); }
      
      /// get a sums file
      const SumsFileInfo* getSumsFileInfo(const int index) const;
      
      /// get a sums file
      SumsFileInfo* getSumsFileInfo(const int index);
      
      /// See if there is a common subdirectory prefix for all of the files
      QString getCommonSubdirectoryPrefix() const;      
      
      /// read the file from a string
      //void readFileFromString(const QString& s) throw (FileException);
      
      /// sort the files
      void sort(const SORT_ORDER so);
      
      /// set the selection status of all files.
      void setAllFileSelectionStatus(const bool status);

      /// Remove subdirectory prefix from all files
      void removeSubdirectoryPrefix();

      /// get exclude spec file flag
      bool getExcludeSpecFileFlag() const { return excludeSpecFileFlag; }
      
      /// set exclude spec file flag
      void setExcludeSpecFileFlag(const bool flag) { excludeSpecFileFlag = flag; }
      
      /// Remove paths from all files.
      void removePathsFromAllFiles();
      
   protected:
      /// read the file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write the file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// Process the file version.
      void processFileVersion(QDomElement& elem) throw (FileException);

      /// Process a file element
      void processFile(QDomElement& elem) throw (FileException);

      /// Process a files element
      void processFiles(QDomElement& elem) throw (FileException);

      /// info about each file
      std::vector<SumsFileInfo> sumsFileInfo;
      
      /// file's version
      int fileVersion;
      
      /// exclude spec file flag
      bool excludeSpecFileFlag;
};


#endif // __SUMS_FILE_LIST_FILE_H__

#ifdef __SUMS_FILE_LIST_FILE_MAIN__
SumsFileInfo::SORTING_KEY SumsFileInfo::sortingKey = SumsFileInfo::SORTING_KEY_TYPE;
#endif // __SUMS_FILE_LIST_FILE_MAIN__

