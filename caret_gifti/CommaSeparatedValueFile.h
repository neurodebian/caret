
#ifndef __COMMA_SEPARATED_VALUE_FILE_H__
#define __COMMA_SEPARATED_VALUE_FILE_H__

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

class StringTable;

/// stores data in a CSV (comma separated values) file
class CommaSeparatedValueFile : public AbstractFile {
   public:
      // constructor
      CommaSeparatedValueFile();
      
      // destructor
      ~CommaSeparatedValueFile();
      
      // clear the file
      void clear();

      // returns true if the file is isEmpty (contains no data)
      bool empty() const;

      /// get the number of data sections
      int getNumberOfDataSections() const { return dataSections.size(); }
      
      /// get a data section 
      StringTable* getDataSection(const int indx) { return dataSections[indx]; }
      
      /// get a data section (const method)
      const StringTable* getDataSection(const int indx) const { return dataSections[indx]; }
      
      /// get a data section (const method)
      const StringTable* getDataSectionByName(const QString& name) const;
      
      /// get a data section (const method)
      StringTable* getDataSectionByName(const QString& name);
      
      /// add a data section (returns its index)
      int addDataSection(StringTable* dataSectionToAdd);
      
      /// read from the data to the stream
      void readFromTextStream(QFile& file, QTextStream& stream) throw (FileException);
      
      /// write the data to the stream
      void writeToTextStream(QTextStream& stream) throw (FileException);
      
      /// get the first line in a comma separated value file
      static QString getFirstLineCommaSeparatedValueFileIdentifier() { return "CSVF-FILE"; }
      
   protected:
      // read the file
      void readFileData(QFile& file,
                        QTextStream& stream, 
                        QDataStream& binStream,
                        QDomElement& /* rootElement */) throw (FileException);

      // add any extra commas and then a newline
      void addExtraCommasAndNewline(QTextStream& stream, 
                                    const int numCommas);
      
      // write  file
      void writeFileData(QTextStream& stream, 
                         QDataStream& binStream,
                         QDomDocument& /* xmlDoc */,
                         QDomElement& /* rootElement */) throw (FileException);

      /// write a section to a file
      void writeDataSection(QTextStream& stream,
                            const StringTable* section,
                            const int maximumColumns) throw (FileException);
      
      /// write a data element
      void writeDataElement(QTextStream& stream,
                            const QString& dataElement);
                     
      /// clean up read item that was read (remove quotes, trim whitespace, etc)
      void cleanupItem(QString& element);
      
      /// the sections in the file
      std::vector<StringTable*> dataSections;
      
      /// section start tag
      static const QString sectionStartTag;
      
      /// section end tag
      static const QString sectionEndTag;
      
      /// current file version
      static const int currentFileVersion;
};

#endif // __COMMA_SEPARATED_VALUE_FILE_H__

#ifdef __COMMA_SEPARATED_FILE_MAIN__
   const QString CommaSeparatedValueFile::sectionStartTag = "csvf-section-start";   
   const QString CommaSeparatedValueFile::sectionEndTag   = "csvf-section-end";   
   const int CommaSeparatedValueFile::currentFileVersion = 0;
#endif // __COMMA_SEPARATED_FILE_MAIN__
