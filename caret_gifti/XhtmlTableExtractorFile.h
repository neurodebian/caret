#ifndef __XHTML_TABLE_EXTRACTOR_FILE_H__
#define __XHTML_TABLE_EXTRACTOR_FILE_H__

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

#include <stack>

#include "AbstractFile.h"

/// file for extracting tables from XHTML files
class XhtmlTableExtractorFile : public AbstractFile {
   public:
      // stores a row from a table
      class TableRow {
         public:
            // constructor
            TableRow();
            
            // destructor
            ~TableRow();
            
            // add an element to the row
            void addElement(const QString& s, const int colSpan = 1);
            
            // insert an element
            void insertElement(const int position, const QString& value);
            
            // get the number of elements
            int getNumberOfElements() const;
            
            // get an element
            QString getElement(const int indx) const;
            
         protected:
            /// the elements
            std::vector<QString> elements;
      };
      
      // class for handling row span
      class TableRowSpan {
         public:
            // constructor
            TableRowSpan(const int rowStartIn,
                         const int numRowsIn,
                         const int colStartIn,
                         const int numColsIn);
                         
            // destructor
            ~TableRowSpan();
            
            /// starting row for rowspan
            int rowStart;
            
            /// last row for rowspan
            int rowEnd;
            
            /// starting column for rowspan
            int colStart;
            
            /// ending column for rowspan
            int colEnd;
      };
      
      // stores a group of TableRows
      class Table {
         public:
            // constructor
            Table();
            
            // destructor
            ~Table();
            
            // add a row to the table
            void addRow(TableRow* tableRow);
            
            // add a row span to the table
            void addRowSpan(const TableRowSpan& rowSpan);
            
            // finish the table (handle row spans)
            void finishTable();
            
            // get the dimensions of the table
            void getTableDimensions(int& numRows, int& numCols) const;
            
            // get the number of rows in the table
            int getNumberOfRows() const;
            
            // get a row from the table
            TableRow* getRow(const int indx);

            // get a row from the table (const method)
            const TableRow* getRow(const int indx) const;

            // get the newest row
            TableRow* getNewestRow();
            
         protected:
            /// the rows
            std::vector<TableRow*> rows;
            
            /// row span storage
            std::vector<TableRowSpan> rowSpans;
      };
      
      // constructor
      XhtmlTableExtractorFile();
      
      // destructor
      ~XhtmlTableExtractorFile();
      
      // call AbstractFile::clearAbstractFile() from its clear method.
      void clear();
      
      // returns true if the file is isEmpty (contains no data)
      bool empty() const;

      // get the number of tables
      int getNumberOfTables() const;
      
      // get a table
      Table* getTable(const int indx);
      
      // get a table
      const Table* getTable(const int indx) const;
      
      // find out if comma separated file conversion supported
      void getCommaSeparatedFileSupport(bool& readFromCSV,
                                        bool& writeToCSV) const;

      // write the file's data into a comma separated values file (throws exception if not supported)
      void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);

   protected:
      // Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                        QDomElement& rootElement) throw (FileException);

      // Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
                         
      // read an XHTML format file
      void readXHTML(QDomElement& elementIn) throw (FileException);
      
      // read an Html Table
      void readHtmlTable(QDomElement& elementIn) throw (FileException);
      
      // read an Html Table Row
      void readHtmlTableRow(QDomElement& elementIn) throw (FileException);
      
      // read an Html Table Data
      void readHtmlTableData(QDomElement& elementIn) throw (FileException);
      
      // Recursively get the contents of a table data node
      void tableDataToText(QDomNode node, const bool doSiblingsFlag, QString& text);

      // clear the tables read
      void clearTables();
      
      /// tables
      std::vector<Table*> tables;
      
      /// active table stack used to assist with file reading
      std::stack<Table*> activeTableStack;
      
      /// active table used to assist with file reading
      Table* activeTable;

};

#endif // __XHTML_TABLE_EXTRACTOR_FILE_H__

