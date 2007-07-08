
#ifndef __STUDY_META_ANALYSIS_FILE_H__
#define __STUDY_META_ANALYSIS_FILE_H__

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

#include "AbstractFile.h"
#include "StudyNamePubMedID.h"

/// class for a study meta-analysis file
class StudyMetaAnalysisFile : public AbstractFile {
   public:
      // constructor
      StudyMetaAnalysisFile();
      
      // destructor
      ~StudyMetaAnalysisFile();
      
      /// Clears current file data in memory.  Deriving classes must override this method and
      /// call AbstractFile::clearAbstractFile() from its clear method.
      virtual void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      virtual bool empty() const;
      
      /// find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;


      // get name
      QString getName() const { return name; }
            
      // set name 
      void setName(const QString& s);
      
      // get title
      QString getTitle() const { return title; }
                                 
      // set title
      void setTitle(const QString& s);
      
      // get authors
      QString getAuthors() const { return authors; }
                                 
      // set authors
      void setAuthors(const QString& s);
      
      // get Citation
      QString getCitation() const { return citation; }
                                 
      // set citation
      void setCitation(const QString& s);
      
      // get DOI or URL
      QString getDoiURL() const { return doiURL; }
      
      // set DOI or URL
      void setDoiURL(const QString& s);
      
      /// get a pointer to the meta-analysis studies
      StudyNamePubMedID* getMetaAnalysisStudies() { return &metaAnalysisStudies; }
      
      /// get a pointer to the meta-analysis studies (const method)
      const StudyNamePubMedID* getMetaAnalysisStudies() const { return &metaAnalysisStudies; }
      
   protected:
      /// write the file's data into a comma separated values file (throws exception if not supported)
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);

      /// read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);

      /// Read the contents of the file (header has already been read)
      virtual void readFileData(QFile& file,
                                QTextStream& stream,
                                QDataStream& binStream,
                                QDomElement& rootElement) throw (FileException);

      /// Write the file's data (header has already been written)
      virtual void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException);
                                 
      /// name
      QString name;
                                 
      /// title
      QString title;
                                 
      /// authors
      QString authors;
                                 
      /// Citation
      QString citation;
                                 
      /// DOI or URL
      QString doiURL;
      
      /// the meta-analysis studies
      StudyNamePubMedID metaAnalysisStudies;
};

#endif // __STUDY_META_ANALYSIS_FILE_H__

