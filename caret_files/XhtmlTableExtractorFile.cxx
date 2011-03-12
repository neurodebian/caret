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

#include <iostream>

#include <QDomElement>
#include <QDomNode>

#include "CommaSeparatedValueFile.h"
#include "DebugControl.h"
#include "SpecFile.h"
#include "StringTable.h"
#include "XhtmlTableExtractorFile.h"

/**
 * constructor.
 */
XhtmlTableExtractorFile::XhtmlTableExtractorFile()
 : AbstractFile("XHTML Table Extractor File",            // description
                SpecFile::getXmlFileExtension(),         // default extension
                true,                                    // has header
                FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE,  // default write type
                FILE_IO_NONE,                            // ascii support
                FILE_IO_NONE,                            // binary support
                FILE_IO_READ_ONLY,                       // XML support
                FILE_IO_NONE,                            // XML Base64 support
                FILE_IO_NONE,                            // XML GZip Base64 support
                FILE_IO_NONE,                            // other support
                FILE_IO_WRITE_ONLY)                      // CSV support
{
   //
   // XHTML table file root is always "html"
   //
   rootXmlElementTagName = "html";
}

/**
 * destructor.
 */
XhtmlTableExtractorFile::~XhtmlTableExtractorFile()
{
}

/**
 * call AbstractFile::clearAbstractFile() from its clear method.
 */
void 
XhtmlTableExtractorFile::clear()
{
   clearAbstractFile();
   clearTables();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
XhtmlTableExtractorFile::empty() const
{
   return true;
}

/**
 * find out if comma separated file conversion supported.
 */
void 
XhtmlTableExtractorFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                                      bool& writeToCSV) const
{
   readFromCSV = false;
   writeToCSV = true;
}

/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
XhtmlTableExtractorFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException)
{
   csv.clear();
   
   const int numTables = getNumberOfTables();
   for (int i = 0; i < numTables; i++) {
      const Table* table = getTable(i);
      int numRows, numCols;
      table->getTableDimensions(numRows, numCols);
      
      if ((numRows > 0) && (numCols > 0)) {
         StringTable* st = new StringTable(numRows, numCols);
         
         for (int j = 0; j < numRows; j++) {
            const TableRow* tr = table->getRow(j);
            const int numElem = tr->getNumberOfElements();
            for (int k = 0; k < numElem; k++) {
               st->setElement(j, k, tr->getElement(k).trimmed());
            }
         }
         csv.addDataSection(st);
      }
   }
}

/**
 * read an Html Table element ("table")
 */
void 
XhtmlTableExtractorFile::readHtmlTable(QDomElement& elementIn) throw (FileException)
{
   //
   // Create a new table and make it the active table
   //
   activeTable = new Table;
   activeTableStack.push(activeTable);
   
   //
   // Process children
   //
   QDomNode node = elementIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         readXHTML(elem);
      }
      node = node.nextSibling();
   }
   
   //
   // Done with table
   //
   activeTable = NULL;
   if (activeTableStack.empty() == false) {
      activeTable = activeTableStack.top();
      activeTableStack.pop();
      tables.push_back(activeTable);
   }
}

/**
 * read an Html Table Row element ("tr")
 */
void 
XhtmlTableExtractorFile::readHtmlTableRow(QDomElement& elementIn) throw (FileException)
{
   if (activeTable == NULL) {
      throw FileException("XhtmlTableExtractorFile read error: have a table row but there is not an active table.");
   }
   
   //
   // Create a new row in the table
   //
   TableRow* row = new TableRow;
   activeTable->addRow(row);

   //
   // Process children
   //
   QDomNode node = elementIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         readXHTML(elem);
      }
      node = node.nextSibling();
   }   
}

/**
 * Recursively get the contents of an HTML "td" element by creating a 
 * string from the "leaves" of this element's children
 */
void 
XhtmlTableExtractorFile::tableDataToText(QDomNode node, const bool doSiblingsFlag, QString& text)
{
   while (node.isNull() == false) {
      //
      // Does this element have children??
      //
      if (node.hasChildNodes()) {
         //
         // process its children
         //
         tableDataToText(node.firstChild(), true, text);
      }
      else {
         //
         // Since this elemenet does not have children, it is a leaf
         // Get its text
         //
         bool addNodeValue = true;
         QDomElement elem = node.toElement();
         if (elem.isNull() == false) {
            //
            // Journal of Neuroscience uses an image for minus signs
            // If found, convert it to a minus sign
            //
            if (DebugControl::getDebugOn()) {
               std::cout << "ELEM TAGNAME: " << elem.tagName().toAscii().constData() << std::endl;
            }
            if (elem.tagName().toLower() == "img") {
               if (elem.attribute("src").contains("minus.gif")) {
                  text += "-";
                  addNodeValue = false;
               }
            }
         }
         if (addNodeValue) {
            //
            // add on to the text string
            //
            text += node.nodeValue();
         }
      }
      //
      // Process siblings
      //
      if (doSiblingsFlag) {
         node = node.nextSibling();
      }
      else {
         return;
      }
   }   
}

/**
 * read an Html Table Data (an HTML "td" element)
 */
void 
XhtmlTableExtractorFile::readHtmlTableData(QDomElement& elementIn) throw (FileException)
{
   if (activeTable == NULL) {
      throw FileException("XhtmlTableExtractorFile read error: have a table data but there is not an active table.");
   }
   TableRow* tableRow = activeTable->getNewestRow();
   if (tableRow == NULL) {
      throw FileException("XhtmlTableExtractorFile read error: have table data but no newest row");
   }
   
   //
   // See if any children are a table
   //
   bool haveTableChild = false;
   QDomNode node = elementIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName().toLower() == "table") {
            haveTableChild = true;
            break;
         }
      }
      node = node.nextSibling();
   }   
   
   //
   // Nested tables ?
   //
   if (haveTableChild) {
      QDomNode node = elementIn.firstChild();
      while (node.isNull() == false) {
         QDomElement elem = node.toElement();
         if (elem.isNull() == false) {
            readXHTML(elem);
         }
         node = node.nextSibling();
      }   
   }
   else {
      //
      // Look for rowspan and colspan
      //
      const int rowspan = elementIn.attribute("rowspan", "0").toInt();
      const int colspan = elementIn.attribute("colspan", "1").toInt();

      if (rowspan > 1) {
         TableRowSpan trs(activeTable->getNumberOfRows(),
                          rowspan - 1,
                          tableRow->getNumberOfElements(),
                          colspan);
         activeTable->addRowSpan(trs);
      }
      
      //
      // Create a string from all of this table data's "leaves"
      //
      QString s;
      tableDataToText(elementIn, false, s);
      if (DebugControl::getDebugOn()) {
         std::cout << "NodeTraversed: " << s.toAscii().constData() << std::endl;
      }
         
      //
      // Replace newlines and line feeds with a blank
      //
      s = s.replace('\n', " ");
      s = s.replace('\r', " ");
      
      //
      // Add to the table
      //
      tableRow->addElement(s, colspan);
      
   }
}
      
/**
 * read an XHTML format file.
 */
void 
XhtmlTableExtractorFile::readXHTML(QDomElement& elementIn) throw (FileException)
{
   const QString tagName = elementIn.tagName().toLower();
   
   if (tagName == "table") {
      readHtmlTable(elementIn);
   }
   else if (tagName == "tr") {
      readHtmlTableRow(elementIn);
   }
   else if ((tagName == "td") || (tagName == "th")) {
      readHtmlTableData(elementIn);
   }
   else {
      QDomNode node = elementIn.firstChild();
      while (node.isNull() == false) {
         QDomElement elem = node.toElement();
         if (elem.isNull() == false) {
            readXHTML(elem);
         }
         node = node.nextSibling();
      }
   }
}

/**
 * clear the tables read.
 */
void 
XhtmlTableExtractorFile::clearTables()
{
   const int numTables = getNumberOfTables();
   for (int i = 0; i < numTables; i++) {
      delete tables[i];
      tables[i] = NULL;
   }
   tables.clear();
}
      
/**
 * get the number of tables.
 */
int 
XhtmlTableExtractorFile::getNumberOfTables() const
{
   return tables.size();
}

/**
 * get a table.
 */
XhtmlTableExtractorFile::Table* 
XhtmlTableExtractorFile::getTable(const int indx)
{
   return tables[indx];
}

/**
 * get a table.
 */
const XhtmlTableExtractorFile::Table* 
XhtmlTableExtractorFile::getTable(const int indx) const
{
   return tables[indx];
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
XhtmlTableExtractorFile::readFileData(QFile& /*file*/,
                                      QTextStream& /*stream*/,
                                      QDataStream& /*binStream*/,
                                      QDomElement& rootElement) throw (FileException)
{
   activeTable = NULL;
   clearTables();
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Reading in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         readXHTML(rootElement);
         for (int i = 0; i < static_cast<int>(tables.size()); i++) {
            tables[i]->finishTable();
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Reading XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Reading in Comma Separated Value File format not supported.");
         break;
   }
   
   if (activeTableStack.empty() == false) {
      std::cout << "Program Error: Active table stack is not empty after reading XhtmlTableExtractorFile" << std::endl;
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
XhtmlTableExtractorFile::writeFileData(QTextStream& stream,
                                       QDataStream& /*binStream*/,
                                       QDomDocument& /*xmlDoc*/,
                                       QDomElement& /*rootElement*/) throw (FileException)
{
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in Ascii format not supported.");           
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");       
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing XML not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            writeDataIntoCommaSeparatedValueFile(csvf);
            csvf.writeToTextStream(stream);
         }
         break;
   }
}



//====================================================================================
// TableRow Class
//====================================================================================

/**
 * constructor.
 */
XhtmlTableExtractorFile::Table::Table()
{
}

/**
 * destructor.
 */
XhtmlTableExtractorFile::Table::~Table()
{
   const int num = getNumberOfRows();
   for (int i = 0; i < num; i++) {
      delete rows[i];
      rows[i] = NULL;
   }
   
   rows.clear();
}

/**
 * add a row to the table.
 */
void 
XhtmlTableExtractorFile::Table::addRow(TableRow* tableRow)
{
   rows.push_back(tableRow);
}

/**
 * add a row span to the table.
 */
void 
XhtmlTableExtractorFile::Table::addRowSpan(const TableRowSpan& rowSpan)
{
   rowSpans.push_back(rowSpan);
}

/**
 * finish the table (handle row spans).
 */
void 
XhtmlTableExtractorFile::Table::finishTable()
{
   const int num = static_cast<int>(rowSpans.size());
   for (int k = 0; k < num; k++) {
      const TableRowSpan& trs = rowSpans[k];
      
      for (int j = trs.rowStart; j <= trs.rowEnd; j++) {
         for (int i = trs.colStart; i <= trs.colEnd; i++) {
            //if (DebugControl::getDebugOn()) {
               TableRow* tr = getRow(j);
               tr->insertElement(i, "");
               std::cout << "Insert empty element at table"
                         << " row " << j
                         << " col " << i << std::endl;
            //}
         }
      }
   }
}

/**
 * get the dimensions of the table.
 */
void 
XhtmlTableExtractorFile::Table::getTableDimensions(int& numRows, int& numCols) const
{
   numRows = getNumberOfRows();
   numCols = 0;
   
   for (int i = 0; i < numRows; i++) {
      const TableRow* tr = getRow(i);
      numCols = std::max(numCols, tr->getNumberOfElements());
   }
}
            
/**
 * get the number of rows in the table.
 */
int 
XhtmlTableExtractorFile::Table::getNumberOfRows() const
{
   return rows.size();
}

/**
 * get a row from the table.
 */
XhtmlTableExtractorFile::TableRow* 
XhtmlTableExtractorFile::Table::getRow(const int indx)
{
   return rows[indx];
}

/**
 * get a row from the table (const method).
 */
const XhtmlTableExtractorFile::TableRow* 
XhtmlTableExtractorFile::Table::getRow(const int indx) const
{
   return rows[indx];
}

/**
 * get the newest row.
 */
XhtmlTableExtractorFile::TableRow* 
XhtmlTableExtractorFile::Table::getNewestRow()
{
   const int num = getNumberOfRows();
   if (num > 0) {
      return rows[num - 1];
   }
   return NULL;
}

//====================================================================================
// TableRow Class
//====================================================================================
/**
 * constructor.
 */
XhtmlTableExtractorFile::TableRow::TableRow()
{
}

/**
 * destructor.
 */
XhtmlTableExtractorFile::TableRow::~TableRow()
{
   elements.clear();
}

/**
 * add an element to the row.
 */
void 
XhtmlTableExtractorFile::TableRow::addElement(const QString& s, const int colSpan)
{
   elements.push_back(s);
   for (int i = 1; i < colSpan; i++) {
      elements.push_back("");
   }
}

/**
 * get the number of elements.
 */
int 
XhtmlTableExtractorFile::TableRow::getNumberOfElements() const 
{ 
   return elements.size(); 
}

/**
 * get an element.
 */
QString 
XhtmlTableExtractorFile::TableRow::getElement(const int indx) const
{ 
   return elements[indx]; 
}

/**
 * insert an element.
 */
void 
XhtmlTableExtractorFile::TableRow::insertElement(const int position, const QString& value)
{
   elements.insert(elements.begin() + position, value);
}

//====================================================================================
// TableRowSpan Class
//====================================================================================
/**
 * constructor.
 */
XhtmlTableExtractorFile::TableRowSpan::TableRowSpan(const int rowStartIn,
                                                    const int numRowsIn,
                                                    const int colStartIn,
                                                    const int numColsIn)
{
   rowStart = rowStartIn;
   rowEnd = rowStart + numRowsIn - 1;
   colStart = colStartIn;
   colEnd = colStart + numColsIn - 1;
}
             
/**
 * destructor.
 */
XhtmlTableExtractorFile::TableRowSpan::~TableRowSpan()
{
}            

