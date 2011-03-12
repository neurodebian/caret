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

#include <QDomDocument>
#include <QDomElement>

#define __COMMA_SEPARATED_FILE_MAIN__
#include "CommaSeparatedValueFile.h"
#undef __COMMA_SEPARATED_FILE_MAIN__

#include "DebugControl.h"
#include "StringTable.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommaSeparatedValueFile::CommaSeparatedValueFile()
   : AbstractFile("Comma Separated Values File", 
                  SpecFile::getCommaSeparatedValueFileExtension(),
                  false,  // no header
                  FILE_FORMAT_OTHER,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_READ_AND_WRITE)
{
}

/**
 * destructor.
 */
CommaSeparatedValueFile::~CommaSeparatedValueFile()
{
   clear();
}

/**
 * clear the file.
 */
void 
CommaSeparatedValueFile::clear()
{
   clearAbstractFile();
   
   for (unsigned int i = 0; i < dataSections.size(); i++) {
      delete dataSections[i];
      dataSections[i] = NULL;
   }
   dataSections.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
CommaSeparatedValueFile::empty() const
{
   return dataSections.empty();
}

/**
 * get a data section (const method).
 */
const StringTable* 
CommaSeparatedValueFile::getDataSectionByName(const QString& name) const
{
   for (unsigned int i = 0; i < dataSections.size(); i++) {
      if (dataSections[i]->getTableTitle().compare(name, Qt::CaseInsensitive) == 0) {
         return dataSections[i];
      }
   }
   return NULL;
}

/**
 * read from the data to the text stream
 */
void 
CommaSeparatedValueFile::readFromTextStream(QFile& file,
                                             QTextStream& stream) throw (FileException)
{  
   QDataStream dummyDataStream(&file);
   dummyDataStream.setVersion(QDataStream::Qt_4_3);
   QDomElement dummyDomElement;
   readFileData(file, stream, dummyDataStream, dummyDomElement);
}

/**
 * write the data to the text stream.
 */
void 
CommaSeparatedValueFile::writeToTextStream(QTextStream& stream) throw (FileException)
{
   QDataStream dummyDataStream;
   dummyDataStream.setVersion(QDataStream::Qt_4_3);
   QDomDocument dummyDomDocument;
   QDomElement dummyDomElement;
   
   writeFileData(stream,
                 dummyDataStream,
                 dummyDomDocument,
                 dummyDomElement);
}
      
/**
 * get a data section (const method).
 */
StringTable* 
CommaSeparatedValueFile::getDataSectionByName(const QString& name)
{
   for (unsigned int i = 0; i < dataSections.size(); i++) {
      if (dataSections[i]->getTableTitle().compare(name, Qt::CaseInsensitive) == 0) {
         return dataSections[i];
      }
   }
   return NULL;
}
      
/**
 * read the file.
 */
void 
CommaSeparatedValueFile::readFileData(QFile& /*file*/,
                                      QTextStream& stream, 
                                      QDataStream& /*binStream*/,
                                      QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Notes about reading a Comma Separated Value File:
   //
   //  * There may be newline characters (\n or \r) within a string delimeted
   //    by double quotes
   //  * If an element contains a double quote a second double quote is added
   //  * Each element is separated by commas (but not commas embedded in strings
   //    delimeted by double quotes
   //  * each record is delimeted by newlines (but not newlines in string delimeted
   //    by double quotes
   //
   const qint64 bufferSize = 4096;
   QString buffer;

   //
   // Some flags for reading file
   //
   bool endOfItem = false;
   bool endOfRecord = false;
   bool inString = false;
   QChar nextCharacter = 0;
   bool firstRecordFlag = true;
   int recordCount = 0;
   
   //
   // Table currently being loaded
   //
   StringTable* currentTable = NULL;
   bool currentTableColumnNamesHaveBeenSet = false;
   
   //
   // QString for current item and vector for items in record
   //
   QString currentItem;
   std::vector<QString> currentRecord;
   
   //
   // Read until done
   //
   QString nextBuffer;
   bool done = false;
   while (done == false) {
      //
      // read some data
      //
      if (nextBuffer.isEmpty() == false) {
         buffer = nextBuffer;
         nextBuffer.clear();
      }
      else {
         buffer = stream.read(bufferSize);
      }
      const qint64 numRead = buffer.length();
      
      if (numRead <= 0) {
         done = true;
      }
      else {
         //
         // Process characters in the buffer
         //
         for (int i = 0; i < numRead; i++) {
            //
            // get character from the buffer
            //
            const QChar ch = buffer[i];
            
            //
            // Also want to know the next character
            //
            nextCharacter = 0;
            if (i < (numRead - 1)) {
               nextCharacter = buffer[i+1];
            }
            else if (stream.atEnd() == false) {
               nextBuffer = stream.read(bufferSize);
               if (nextBuffer.isEmpty() == false) {
                  nextCharacter = nextBuffer[0];
               }
              /*
               const qint64 oldPos = stream.pos();
               const QString temp = stream.read(1);
               if (temp.isEmpty() == false) {
                  nextCharacter = temp[0];
               }
               stream.seek(oldPos);
              */
            }
            
            //
            // By default we want to use the current character
            //
            bool useCharacter = true;
            
            //
            // Under some circumstances, a character may be skipped
            //
            bool skipNextCharacter = false;
         
            //
            // Check for characters that delineate lines or elements
            //
            if (ch.toAscii() == QChar('"')) {
               //         double quotes in a string are two consecutive double quotes
               // If currently reading the contents of a string
               //
               if (inString) {
                  //
                  // If Double quotes within a string then do not need two of them so skip next 
                  //
                  if (nextCharacter.toAscii() == QChar('"')) {
                     skipNextCharacter = true;
                  }
                  else {
                     //
                     // Just a single quote so at end of the string
                     //
                     inString = false;
                  }
               }
               else {
                  //
                  // Starting a string
                  //
                  inString = true;
               }
            }
            else if (ch.toAscii() == QChar(',')) {  // comma
               //
               // Commas not within a string delineate elements
               //
               if (inString == false) {
                  endOfItem = true;
                  useCharacter = false;
               }
            }
            else if (ch.toAscii() == QChar('\r')) {  // carriage return 0x0d 
               if (inString == false) {
                  //
                  // carriage return not in a string denotes end of element and end of line
                  //
                  endOfItem = true;
                  endOfRecord = true;
                  useCharacter = false;
               }
               //
               // Some operating systems follow a carriage return with a newline
               //
               if (nextCharacter.toAscii() == QChar('\n')) {
                  skipNextCharacter = true;
               }
            }
            else if (ch.toAscii() == QChar('\n')) {  // line feed 0x0a 
               if (inString == false) {
                  //
                  // Line feed not in a string denotes end of element and end of line
                  //
                  endOfItem = true;
                  endOfRecord = true;
                  useCharacter = false;
               }
            }
            
            //
            // Save the character just parsed
            //
            if (useCharacter) {
               currentItem.append(ch);
            }
            
            //
            // At end of item?
            //
            if (endOfItem) {
               //
               // Add to current record
               //
               cleanupItem(currentItem);
               currentRecord.push_back(currentItem);
               
               //
               // clear current item
               //
               currentItem = "";
               endOfItem = false;
               
               //
               // At end of record?
               //
               if (endOfRecord) {
                  //
                  // is there data in the record
                  //
                  const int numItems = static_cast<int>(currentRecord.size());
                  if (numItems > 0) {
                     if (DebugControl::getDebugOn()) {
                        if (currentRecord[0] == "305") {
                           std::cout << "At focus 305" << std::endl;
                        }
                     }
                  
                     //
                     // The first record must be the CSVF identifier
                     //
                     if (firstRecordFlag) {
                        if (currentRecord[0] != getFirstLineCommaSeparatedValueFileIdentifier()) {
                           QString msg("First line in file must be \"");
                           msg += getFirstLineCommaSeparatedValueFileIdentifier();
                           msg += ".\"";
                           throw FileException(msg);
                        }
                        
                        //
                        // Check file version
                        //
                        int fileVersion = 0;
                        if (numItems > 1) {
                           fileVersion = currentRecord[1].toInt();
                        }
                        if (fileVersion > currentFileVersion) {
                           QString msg = "CSVF version read is " + currentRecord[1] +
                                         " but this version of Caret only supports version " +
                                         QString::number(currentFileVersion) +
                                         " and earlier Comma Separated Value Files.";
                           throw FileException(msg);
                        }
                        
                        firstRecordFlag = false;
                     }
                     else if (currentRecord[0].compare(sectionStartTag, Qt::CaseInsensitive) == 0) {
                        //
                        // Start a new StringTable
                        //
                        if (numItems >= 3) {
                           const int numColumns = currentRecord[2].toInt();
                           currentTable = new StringTable(0, numColumns, currentRecord[1]);
                           currentTableColumnNamesHaveBeenSet = false;
                        }
                        else {
                           QString msg("CSVF File section start must have table title and "
                                       "number of columns for record number=");
                           msg += QString::number(recordCount);
                           throw FileException(msg);
                        }
                     }
                     else if (currentRecord[0].compare(sectionEndTag, Qt::CaseInsensitive) == 0) {
                        //
                        // Add current StringTable to the file
                        //
                        if (currentTable != NULL) {
                           addDataSection(currentTable);
                           currentTable = NULL;
                        }
                        else {
                           throw FileException("CSVF File has currentTable NULL at end of section.");
                        }
                     }
                     else {
                        if (currentTable != NULL) {
                           //
                           // First row is the column titles
                           //
                           if (currentTableColumnNamesHaveBeenSet == false) {
                              for (int jj = 0; jj < std::min(currentTable->getNumberOfColumns(),
                                                             numItems); jj++) {
                                 currentTable->setColumnTitle(jj, currentRecord[jj]);
                              }
                              currentTableColumnNamesHaveBeenSet = true;
                           }
                           else {
                              //
                              // Add a row of data
                              //
                              currentTable->addRow(currentRecord);
                           }
                        }
                        else {
                           bool allEmpty = true;
                           for (unsigned int im = 0; im < currentRecord.size(); im++) {
                              if (currentRecord[im].trimmed().isEmpty() == false) {
                                 allEmpty = false;
                                 break;
                              }
                           }
                           if (allEmpty == false) {
                              throw FileException("CSVF File has currentTable NULL when adding data.");
                           }
                        }
                     }
                  }
                  
                  if (DebugControl::getDebugOn()) {
                     const int numItems = static_cast<int>(currentRecord.size());
                     std::cout << "CSVF: Read record with count items: " << numItems << std::endl;
                     //QStringList sl;
                     //for (int mm = 0; mm < numItems; mm++) {
                     //   sl << currentRecord[mm];
                     //}
                     //std::cout << sl.join("|").toAscii().constData() << std::endl;
                  }
                  currentRecord.clear();
                  endOfRecord = false;
                  recordCount++;
               }
            }
            
            //
            // Should next character be skipped
            //
            if (skipNextCharacter) {
               //
               // At end of buffer?
               //
               if (i >= (numRead - 1)) {
                  //
                  // Read and ignore one character
                  //
                  (void)stream.read(1);
               }
               else {
                  //
                  // Skip one character forward in the buffer
                  //
                  i++;
               }
            }
         }
      }
   }
   
   //
   // Was something stil being read?
   //
   if (currentItem.isEmpty() == false) {
      cleanupItem(currentItem);
      currentRecord.push_back(currentItem);
   }
   
   //
   // Is there data for another line
   //
   if (currentRecord.empty() == false) {
      if (DebugControl::getDebugOn()) {
         std::cout << "LAST Read record with count items: " << currentRecord.size() << std::endl;
      }
         
      if (currentRecord[0].compare(sectionEndTag, Qt::CaseInsensitive) == 0) {
         //
         // Add current StringTable to the file
         //
         if (currentTable != NULL) {
            addDataSection(currentTable);
            currentTable = NULL;
            currentTableColumnNamesHaveBeenSet = false;
         }
         else {
            throw FileException("CSVF File has currentTable NULL at end of section.");
         }
      }
   }
   
   //
   // If a table was being read then it is missing its end tag
   //  
   if (currentTable != NULL) {
      const QString msg("Data section named "
                        + currentTable->getTableTitle()
                        + "\n"
                        + "is missing its \""
                        + CommaSeparatedValueFile::sectionEndTag
                        + "\" tag.");
      throw FileException(msg);
   }
}

/**
 * clean up read item that was read (remove quotes, trim whitespace, etc).
 */
void 
CommaSeparatedValueFile::cleanupItem(QString& item)
{
   //
   // Get rid of leading and trailing whitespace
   //
   item = item.trimmed();
   
   const int len = item.length();
   if (len >= 2) {
      //
      // Check for and remove double quotes at beggining and end
      //
      if (item.startsWith('"') && item.endsWith('"')) {
         item = item.mid(1, len - 2);
      }
   }
}
      
/**
 * write  file.
 */
void 
CommaSeparatedValueFile::writeFileData(QTextStream& stream, 
              QDataStream& /*binStream*/,
              QDomDocument& /* xmlDoc */,
              QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Determine the maximum columns of all of the data sections
   //
   const int numSections = getNumberOfDataSections();
   int maximumColumns = 0;
   for (int i = 0; i < numSections; i++) {
      maximumColumns = std::max(maximumColumns, getDataSection(i)->getNumberOfColumns());
   }
   
   if (maximumColumns <= 0) {
      throw FileException("No data for writing.");
   }
   
   //
   // Must have at least 3 columns since section start records have
   // three items, csvf-section-start,header,#columns-in-data
   //
   maximumColumns = std::max(maximumColumns, 3);
   
   //
   // Write the first line indicating this is a CSVF format
   //
   stream << getFirstLineCommaSeparatedValueFileIdentifier() << ","
          << currentFileVersion;
   addExtraCommasAndNewline(stream, maximumColumns - 2);
   
   //
   // Write the data sections
   //
   for (int i = 0; i < numSections; i++) {
      writeDataSection(stream, getDataSection(i), maximumColumns);
   }
}

/**
 * write a section to a file.
 */
void 
CommaSeparatedValueFile::writeDataSection(QTextStream& stream,
                                           const StringTable* section,
                                           const int maximumColumns) throw (FileException)
{
   //
   // Make sure there is data to write
   //
   const int numRows    = section->getNumberOfRows();
   const int numColumns = section->getNumberOfColumns();
   if ((numRows <= 0) || (numColumns <= 0)) {
      return;
   }
   
   //
   // Write the start of the section
   //
   stream << sectionStartTag << "," << section->getTableTitle() << "," << numColumns;
   addExtraCommasAndNewline(stream, maximumColumns - 3);
   
   //
   // write the column titles
   //
   for (int i = 0; i < numColumns; i++) {
      if (i > 0) {
         stream << ",";
      }
      stream << section->getColumnTitle(i);
   }
   addExtraCommasAndNewline(stream, maximumColumns - numColumns);
   
   //
   // Write the data
   //
   for (int i = 0; i < numRows; i++) {
      for (int j = 0; j < numColumns; j++) {
         if (j > 0) {
            stream << ",";
         }
         writeDataElement(stream, section->getElement(i, j));
      }
      addExtraCommasAndNewline(stream, maximumColumns - numColumns);
   }
   
   //
   // Write the end of the section
   //
   stream << sectionEndTag << "," << section->getTableTitle();
   addExtraCommasAndNewline(stream, maximumColumns - 2);
}

/**
 * write a data element.
 */
void 
CommaSeparatedValueFile::writeDataElement(QTextStream& stream,
                                           const QString& dataElementIn)
{
   QString dataElement = dataElementIn;
   bool needsQuotes = false;
   
   //
   // In a CSV file, a double quote is replaced by two double quotes
   // and the whole thing must be enclosed in double quotes
   //
   if (dataElement.indexOf('"') >= 0) {
      needsQuotes = true;
      dataElement = dataElement.replace('"', "\"\"");
   }
   
   //
   // If the element contains commas the whole thing must be put in double quotes
   //
   if (dataElement.indexOf(',') >= 0) {
      needsQuotes = true;
   }
      
   //
   // If it has carriage return, convert to line feed
   //
   if (dataElement.indexOf('\r') >= 0) {
      dataElement = dataElement.replace('\r', '\n');
   }
   
   //
   // If it has line feed surround with quotes
   //
   if (dataElement.indexOf('\n') >= 0) {
      needsQuotes = true;
   }
   
   //
   // If needed enclose in double quotes
   //
   if (needsQuotes) {
      stream << "\"";
      stream << dataElement;
      stream << "\"";
   }
   else {
      stream << dataElement;
   }
}
                            
/**
 * add any extra commas and then a newline.
 */
void 
CommaSeparatedValueFile::addExtraCommasAndNewline(QTextStream& stream, 
                                                   const int numCommas)
{
   for (int i = 0; i < numCommas; i++) {
      stream << ",";
   }
   stream << endl;
}
      
/**
 * add a data section (returns its index).
 */
int 
CommaSeparatedValueFile::addDataSection(StringTable* dataSectionToAdd)
{
   dataSections.push_back(dataSectionToAdd);
   return (dataSections.size() - 1);
}      
