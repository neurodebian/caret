
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

#include "CommaSeparatedValueFile.h"
#include "SpecFile.h"
#include "StringTable.h"
#include "StudyMetaAnalysisFile.h"

/**
 * constructor.
 */
StudyMetaAnalysisFile::StudyMetaAnalysisFile()
   : AbstractFile("Study Meta-Analysis File",
                  SpecFile::getStudyMetaAnalysisFileExtension(),
                  true,           // has header
                  FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE,    // default file format
                  FILE_IO_NONE,   // ascii
                  FILE_IO_NONE,   // binary
                  FILE_IO_NONE,   // xml
                  FILE_IO_NONE,   // xml base64
                  FILE_IO_NONE,   // xml base64 gzip
                  FILE_IO_NONE,   // other
                  FILE_IO_READ_ONLY)   // CSV
                  
{
   clear();
}

/**
 * destructor.
 */
StudyMetaAnalysisFile::~StudyMetaAnalysisFile()
{
   clear();
}

/**
 * call AbstractFile::clearAbstractFile() from its clear method.
 */
void 
StudyMetaAnalysisFile::clear()
{
   clearAbstractFile();
   name = "";
   title = "";
   authors = "";
   citation = "";
   doiURL = "";
   metaAnalysisStudies.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
StudyMetaAnalysisFile::empty() const
{
   return name.isEmpty();
}

/**
 * set name.
 */
void 
StudyMetaAnalysisFile::setName(const QString& s)
{
   name = s;
   setModified();
}

/**
 * set title.
 */
void 
StudyMetaAnalysisFile::setTitle(const QString& s)
{
   title = s;
   setModified();
}

/**
 * set authors.
 */
void 
StudyMetaAnalysisFile::setAuthors(const QString& s)
{
   authors = s;
   setModified();
}

/**
 * set citation.
 */
void 
StudyMetaAnalysisFile::setCitation(const QString& s)
{
   citation = s;
   setModified();
}

/**
 * set DOI or URL.
 */
void 
StudyMetaAnalysisFile::setDoiURL(const QString& s)
{
   doiURL = s;
   setModified();
}

/**
 * find out if comma separated file conversion supported.
 */
void 
StudyMetaAnalysisFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                                    bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = false;
}

/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
StudyMetaAnalysisFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& /*csv*/) throw (FileException)
{
   throw FileException("Writing to Comma Separated Value File not supported.");
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
StudyMetaAnalysisFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   clear();
   
   // 
   // Do header
   //    
   const StringTable* stHeader = csv.getDataSectionByName("header");
   if (stHeader != NULL) {
      readHeaderDataFromStringTable(*stHeader);
   }  

   const StringTable* st = csv.getDataSectionByName("MetaAnalysis");   
   if (st == NULL) {
      throw FileException("No MetaAnalysis data found");
   }
   
   const int numRows = st->getNumberOfRows();
   if (numRows != 1) {
      throw FileException("MetaAnalysis section must contain one and only one row of data."); 
   }
   int numCols = st->getNumberOfColumns();
   
   int nameCol = -1;
   int titleCol = -1;
   int authorsCol = -1;
   int citationCol = -1;
   int doiURLCol = -1;
   
   for (int i = 0; i < numCols; i++) {
      const QString columnTitle = st->getColumnTitle(i).toLower();
      if (columnTitle == "name") {
         nameCol = i;
      }
      else if (columnTitle == "title") {
         titleCol = i;   
      }
      else if (columnTitle == "authors") {
         authorsCol = i;   
      }
      else if (columnTitle == "citation") {
         citationCol = i;   
      }
      else if (columnTitle == "doi-url") {
         doiURLCol = i;   
      }
   }
   
   for (int i = 0; i < numRows; i++) {
      if (nameCol >= 0) {
         setName(st->getElement(i, nameCol));
      }
      if (titleCol >= 0) {
         setTitle(st->getElement(i, titleCol));
      }
      if (authorsCol >= 0) {
         setAuthors(st->getElement(i, authorsCol));
      }
      if (citationCol >= 0) {
         setCitation(st->getElement(i, citationCol));
      }
      if (doiURLCol >= 0) {
         setDoiURL(st->getElement(i, doiURLCol));
      }
   }
   
   //
   // Name and PubMed ID pairs
   //
   const StringTable* pmids = csv.getDataSectionByName("Name_PMID");
   if (pmids != NULL) {
      metaAnalysisStudies.readDataFromStringTable(*pmids);
   }
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
StudyMetaAnalysisFile::readFileData(QFile& file,
                                    QTextStream& stream,
                                    QDataStream& /*binStream*/,
                                    QDomElement& /*rootElement*/) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) { 
      return;
   }
 
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Reading in ASCII format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Reading in XML format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            csvf.readFromTextStream(file, stream);
            readDataFromCommaSeparatedValuesTable(csvf);
         }
         break;
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
StudyMetaAnalysisFile::writeFileData(QTextStream& /*stream*/,
                                     QDataStream& /*binStream*/,
                                     QDomDocument& /*xmlDoc*/,
                                     QDomElement& /*rootElement*/) throw (FileException)
{
   throw FileException("Writing of StudyMetaAnalysisFile not supported.");
}
