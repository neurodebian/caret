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

#include <cmath>
#include <iostream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>
#include <QTextStream>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "CellColorFile.h"
#include "ColorFile.h"
#include "CommaSeparatedValueFile.h"
#include "ContourCellColorFile.h"
#include "FociColorFile.h"
#include "StringTable.h"

/**
 * The constructor.
 */
ColorFile::ColorFile(const QString& descriptiveName,
                     const QString& defaultExtensionIn)
   : AbstractFile(descriptiveName, 
                  defaultExtensionIn, 
                  true,
                  FILE_FORMAT_XML, 
                  FILE_IO_READ_ONLY,      // ascii
                  FILE_IO_NONE,           // binary
                  FILE_IO_READ_AND_WRITE, // xml
                  FILE_IO_NONE,           // xml base-64
                  FILE_IO_NONE,           // xml gzip
                  FILE_IO_NONE,           // other
                  FILE_IO_READ_AND_WRITE) // Comma Separated Value File
{
   clear();
}

/**
 * The destructor.
 */
ColorFile::~ColorFile()
{
   clear();
}

/**
 * get a "new" color file from the file name's extension
 * Note: file is not read, just a new file is returned.
 */
ColorFile* 
ColorFile::getColorFileFromFileNameExtension(const QString& fileNameIn)
                                                      throw (FileException)
{
   ColorFile* colorFile = NULL;
   
   if (fileNameIn.endsWith(SpecFile::getAreaColorFileExtension())) {
      colorFile = new AreaColorFile;
   }
   else if (fileNameIn.endsWith(SpecFile::getBorderColorFileExtension())) {
      colorFile = new BorderColorFile;
   }
   else if (fileNameIn.endsWith(SpecFile::getCellColorFileExtension())) {
      colorFile = new CellColorFile;
   }
   else if (fileNameIn.endsWith(SpecFile::getContourCellColorFileExtension())) {
      colorFile = new ContourCellColorFile;
   }
   else if (fileNameIn.endsWith(SpecFile::getFociColorFileExtension())) {
      colorFile = new FociColorFile;
   }
   else {
      const QString errorMessageOut = 
         ("ERROR: Unrecognized color file name extension.  "
          "The input color file name must end with one of: "
          "(Area Color File, \"" + SpecFile::getAreaColorFileExtension() + "\")  "
           + "(Border Color File, \"" + SpecFile::getBorderColorFileExtension() + "\")  "
           + "(Cell Color File, \"" + SpecFile::getCellColorFileExtension() + "\")  "
           + "(Contour Cell Color File, \"" + SpecFile::getContourCellColorFileExtension() + "\")  "
           + "(Foci Color File, \"" + SpecFile::getFociColorFileExtension() + "\")");
      throw FileException(errorMessageOut);
   }

   return colorFile;
}      

/**
 * Clear the file's contents.
 */
void
ColorFile::clear()
{
   clearAbstractFile();
   colors.clear();
}

/**
 * Append a color file to the current color file.
 */
void
ColorFile::append(const ColorFile& cf)
{
   const int numColors = cf.getNumberOfColors();
   
   for (int i = 0; i < numColors; i++) {
      QString name(cf.getColorNameByIndex(i));
      
      unsigned char r, g, b, a;
      cf.getColorByIndex(i, r, g, b, a);

      float pointSize, lineSize;
      cf.getPointLineSizeByIndex(i, pointSize, lineSize);
      
      const ColorStorage::SYMBOL symbol = cf.getSymbolByIndex(i);
      
      /// if color already exists, replace its color components
      bool match;
      const int indx = getColorIndexByName(name, match);
      if ((indx >= 0) && match) {
         setColorByIndex(indx, name, r, g, b, a);
         setPointLineSizeByIndex(indx, pointSize, lineSize);
         setSymbolByIndex(indx, symbol);
      }
      else {
         addColor(name, r, g, b, a, pointSize, lineSize, symbol); 
      }
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(cf);
}

/** 
 * Add a color to the color file.  Returns the index of the color.
 */
int 
ColorFile::addColor(const QString& name,
                    const unsigned char r, const unsigned char g, const unsigned char b,
                    const unsigned char alpha,
                    const float pointSize, const float lineSize,
                    ColorStorage::SYMBOL symbol)
{
   if ((colors.size() == 0) && (name != "???")) {
      colors.push_back(ColorStorage("???", 170, 170, 170, 255, 2.0, 1.0, ColorStorage::SYMBOL_OPENGL_POINT));
   }
   
   //
   // see if color already exists
   //
   bool match = false;
   unsigned char rt, gt, bt, at;
   const int indx = getColorByName(name, match, rt, gt, bt, at);
   if ((indx >= 0) && match) {
      if ((r == rt) && (g == gt) && (b == bt) && (alpha == at)) {
         std::cout << "Color " << name.toAscii().constData()
                   << " specified multiple times with same color components."
                   << std::endl;
      }
      else {
         setColorByIndex(indx, name, r, g, b, alpha);
         std::cout << "Color " << name.toAscii().constData()
                 << " specified multiple times with different color components."
                 << "  Using (" << static_cast<int>(r) << ","
                 << static_cast<int>(g) << ","
                 << static_cast<int>(b) << ","
                 << static_cast<int>(alpha) << ")"
                 << std::endl;
      }
      return indx;
   }

   setModified();
   colors.push_back(ColorStorage(name, r, g, b, alpha, pointSize, lineSize, symbol));
   return colors.size() - 1;
}

/**
 * Get a color by its index (no validity check is made for the index).
 */
void
ColorFile::getColorByIndex(const int indx,
                           unsigned char& red, unsigned char& green, unsigned char& blue) const
{
   if ((indx >= 0) && (indx < static_cast<int>(colors.size()))) {
      colors[indx].getRgb(red, green, blue);
   }
   else {
      red   = 0;
      green = 0;
      blue  = 0;
   }
}

/**
 * Get a color by its index (no validity check is made for the index).
 */
void
ColorFile::getColorByIndex(const int indx,
                           unsigned char& red, unsigned char& green, unsigned char& blue,
                           unsigned char& alpha) const
{
   if ((indx >= 0) && (indx < static_cast<int>(colors.size()))) {
      colors[indx].getRgba(red, green, blue, alpha);
   }
   else {
      red   = 0;
      green = 0;
      blue  = 0;
      alpha = 255;
   }
}

/**
 * Get the line size and point size by index.
 */
void
ColorFile::getPointLineSizeByIndex(const int indx, float& pointSize, float& lineSize) const
{
   pointSize = colors[indx].getPointSize();
   lineSize  = colors[indx].getLineSize();
}

/**
 * Get a color's name by its indx (no validity check is made for the indx).
 */
QString
ColorFile::getColorNameByIndex(const int indx) const
{
   return colors[indx].getName();
}

/**
 * get the symbol by the color index.
 */
ColorFile::ColorStorage::SYMBOL 
ColorFile::getSymbolByIndex(const int indx) const
{
   return colors[indx].getSymbol();
}      
 
/**
 * Get a color by its name.  Returns indx of color if a matching color is 
 * found, else -1.
 * "exactMatch will be true if the color's name was an exact match.  Otherwise,
 * the color is a substring of "name".
 */
int
ColorFile::getColorIndexByName(const QString& name,
                               bool& exactMatch) const
{
   int matchFound = -1;
   int matchLength = -1;
   exactMatch = false;
   
   const int numColors = getNumberOfColors();
   for (int i = 0; i <  numColors; i++) {
      if (colors[i].getName() == name) {
         matchFound = i;
         exactMatch = true;
         break;
      }
      else {
         const int nameLength  = name.length();
         const int colorLength = colors[i].getName().length();
         
         if (colorLength < nameLength) {
            if (colors[i].getName() == name.mid(0, colorLength)) {
               if (matchFound >= 0) {
                  if (colorLength > matchLength) {
                      matchFound = i;
                      matchLength = colorLength;
                  }
               }
               else {
                  matchFound = i;
                  matchLength = colorLength;
               }
            }
         }
      }
   }
   
   return matchFound;
}


 
/**
 * Get a color by its name.  Returns indx of color if a matching color is 
 * found, else -1.
 * "exactMatch will be true if the color's name was an exact match.  Otherwise,
 * the color is a substring of "name".
 */
int
ColorFile::getColorByName(const QString& name,
                          bool& exactMatch,
                          unsigned char& red, unsigned char& green, unsigned char& blue) const
{
   const int matchFound = getColorIndexByName(name, exactMatch);
      
   if (matchFound >= 0) {
      colors[matchFound].getRgb(red, green, blue);
   }
   
   return matchFound;
}

/**
 * Get a color by its name.  Returns indx of color if a matching color is 
 * found, else -1.
 * "exactMatch will be true if the color's name was an exact match.  Otherwise,
 * the color is a substring of "name".
 */
int
ColorFile::getColorByName(const QString& name,
                          bool& exactMatch,
                          unsigned char& red, unsigned char& green, unsigned char& blue,
                          unsigned char& alpha) const
{
   const int matchFound = getColorIndexByName(name, exactMatch);
      
   if (matchFound >= 0) {
      colors[matchFound].getRgba(red, green, blue, alpha);
   }
   
   return matchFound;
}

/**
 * Set a color by it's indx (the index is not checked for validity).
 */
void 
ColorFile::setColorByIndex(const int indx,
                           const QString& nameIn,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue)
{
   colors[indx].setName(nameIn);
   colors[indx].setRgb(red, green, blue);
   setModified();
}

/**
 * Set a color by it's indx (the index is not checked for validity).
 */
void 
ColorFile::setColorByIndex(const int indx,
                           const QString& nameIn,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue, const unsigned char alpha)
{
   colors[indx].setName(nameIn);
   colors[indx].setRgba(red, green, blue, alpha);
   setModified();
}

/**
 * Set a color by it's index (the index is not checked for validity).
 */
void 
ColorFile::setColorByIndex(const int indx,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue)
{
   colors[indx].setRgb(red, green, blue);
   setModified();
}

/**
 * Set a color by it's index (the index is not checked for validity).
 */
void 
ColorFile::setColorByIndex(const int indx,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue, const unsigned char alpha)
{
   colors[indx].setRgba(red, green, blue, alpha);
   setModified();
}

/**
 *
 */
void
ColorFile::setPointLineSizeByIndex(const int indx,
                                   const float pointSize, const float lineSize)
{
   colors[indx].setPointSize(pointSize);
   colors[indx].setLineSize(lineSize);
   setModified();
}

/**
 * set symbol by index.
 */
void 
ColorFile::setSymbolByIndex(const int indx, const ColorFile::ColorStorage::SYMBOL s)
{
   colors[indx].setSymbol(s);
   setModified();
}
      
/**
 * set selection status of all colors
 */
void
ColorFile::setAllSelectedStatus(const bool sel)
{
   const int num = getNumberOfColors();
   for (int i = 0; i < num; i++) {
      setSelected(i, sel);
   }
}

/**
 * Set the number of colors in the file (clears its contents too).
 */
void
ColorFile::setNumberOfColors(const int numColors)
{
   clear();
   colors.resize(numColors);
   setModified();
}

/**
 * Remove color at specified index
 */
void
ColorFile::removeColorByIndex(const int indx)
{
   if (indx < getNumberOfColors()) {
      colors.erase(colors.begin() + indx);
   }
}

/**
 * find out if comma separated file conversion supported.
 */
void 
ColorFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                        bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = true;
}

/**
 * Generate random colors for any name that does not match an area color name. 
 * if "requireExactMatchFlag" is true, a new color is created in there is not 
 * a color that exactly matches name.  If "requireExactMatchFlag" is false
 * and the "name" begins with the color name, a new color is not created.
 * so, for example, if "name" is "SUL.CeS" and there is a color named "SUL",
 * a new color is not created.
 */
void 
ColorFile::generateColorsForNamesWithoutColors(const std::vector<QString>& names,
                                               const bool requireExactMatchFlag)
{
   int ctr = 0;
   const int numNames = static_cast<int>(names.size());
   for (int i = 0; i < numNames; i++) {
      const QString name(names[i]);
      bool exactlyMatchingColorFoundFlag = false;
      const int colorIndex = getColorIndexByName(name, exactlyMatchingColorFoundFlag);
      
      bool createColorFlag = false;
      if (colorIndex < 0) {
         createColorFlag = true;
      }
      else if (requireExactMatchFlag) {
         if (exactlyMatchingColorFoundFlag == false) {
            createColorFlag = true;
         }
      }
      
      if (createColorFlag) {
         if (name == "???") {
            addColor(name, 170, 170, 170);
         }
         else {
            switch (ctr) {
               case 0:
                  addColor(name, 255, 0, 0);
                  break;
               case 1:
                  addColor(name, 255, 0, 127);
                  break;
               case 2:
                  addColor(name, 255, 0, 255);
                  break;
               case 3:
                  addColor(name, 255, 127, 0);
                  break;
               case 4:
                  addColor(name, 255, 127, 127);
                  break;
               case 5:
                  addColor(name, 255, 127, 255);
                  break;
               case 6:
                  addColor(name, 0, 0, 127);
                  break;
               case 7:
                  addColor(name, 0, 0, 255);
                  break;
               case 8:
                  addColor(name, 127, 0, 0);
                  break;
               case 9:
                  addColor(name, 127, 0, 127);
                  break;
               case 10:
                  addColor(name, 127, 0, 255);
                  break;
               case 11:
                  addColor(name, 127, 127, 0);
                  break;
               case 12:
                  addColor(name, 127, 127, 127);
                  break;
               case 13:
                  addColor(name, 127, 127, 255);
                  break;
               case 14:
                  addColor(name, 127, 255, 0);
                  break;
               case 15:
                  addColor(name, 127, 255, 127);
                  break;
               case 16:
                  addColor(name, 127, 255, 255);
                  ctr = -1;  // start over
                  break;
            }
            ctr++;
         }
      }
   }
}
      
/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
ColorFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException)
{
   csv.clear();
   
   const int numColors = getNumberOfColors();
   if (numColors <= 0) {
      return;
   }
   
   //
   // Column numbers for data
   //
   int numCols = 0;
   const int nameCol      = numCols++;
   const int redCol       = numCols++;
   const int greenCol     = numCols++;
   const int blueCol      = numCols++;
   const int alphaCol     = numCols++;
   const int pointSizeCol = numCols++;
   const int lineSizeCol  = numCols++;
   const int symbolCol    = numCols++;
   
   //
   // Create and add to string table
   //
   StringTable* ct = new StringTable(numColors, numCols, "Colors");
   ct->setColumnTitle(nameCol, "Name");
   ct->setColumnTitle(redCol, "Red");
   ct->setColumnTitle(greenCol, "Green");
   ct->setColumnTitle(blueCol, "Blue");
   ct->setColumnTitle(alphaCol, "Alpha");
   ct->setColumnTitle(pointSizeCol, "Point-Size");
   ct->setColumnTitle(lineSizeCol, "Line-Size");
   ct->setColumnTitle(symbolCol, "Symbol");
   
   //
   // Load the data
   //
   for (int i = 0; i < numColors; i++) {
      const ColorStorage* color = getColor(i);
      unsigned char red, green, blue, alpha;
      color->getRgba(red, green, blue, alpha);
      ct->setElement(i, nameCol, color->getName());
      ct->setElement(i, redCol, red);
      ct->setElement(i, greenCol, green);
      ct->setElement(i, blueCol, blue);
      ct->setElement(i, alphaCol, alpha);
      ct->setElement(i, pointSizeCol, color->getPointSize());
      ct->setElement(i, lineSizeCol, color->getLineSize());
      ct->setElement(i, symbolCol, ColorStorage::symbolToText(color->getSymbol()));
   }
   
   StringTable* headerTable = new StringTable(0, 0);
   writeHeaderDataIntoStringTable(*headerTable);
   csv.addDataSection(headerTable);
   csv.addDataSection(ct);
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
ColorFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   clear();
   
   const StringTable* ct = csv.getDataSectionByName("Colors");   
   if (ct == NULL) {
      throw FileException("No colors found");
   }
    
   int nameCol      = -1;
   int redCol       = -1;
   int greenCol     = -1;
   int blueCol      = -1;
   int alphaCol     = -1;
   int pointSizeCol = -1;
   int lineSizeCol  = -1;
   int symbolCol    = -1;

   int numCols = ct->getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      const QString columnTitle = ct->getColumnTitle(i).toLower();
      if (columnTitle == "name") {
         nameCol = i;
      }
      else if (columnTitle == "red") {
         redCol = i;   
      }
      else if (columnTitle == "green") {
         greenCol = i;   
      }
      else if (columnTitle == "blue") {
         blueCol = i;   
      }
      else if (columnTitle == "alpha") {
         alphaCol = i;   
      }
      else if (columnTitle == "point-size") {
         pointSizeCol = i;   
      }
      else if (columnTitle == "line-size") {
         lineSizeCol = i;   
      }
      else if (columnTitle == "symbol") {
         symbolCol = i;   
      }
   }
  
   for (int i = 0; i < ct->getNumberOfRows(); i++) {
      ColorStorage cs;
      unsigned char red, green, blue, alpha;
      cs.getRgba(red, green, blue, alpha);
      float lineSize = cs.getLineSize();
      float pointSize = cs.getPointSize();
      ColorStorage::SYMBOL symbol = cs.getSymbol();
      QString name("unnamed color");
      
      if (nameCol >= 0) {
         name = ct->getElement(i, nameCol);
      }
      if (redCol >= 0) {
         red = ct->getElementAsInt(i, redCol);
      }
      if (greenCol >= 0) {
         green = ct->getElementAsInt(i, greenCol);
      }
      if (blueCol >= 0) {
         blue = ct->getElementAsInt(i, blueCol);
      }
      if (alphaCol >= 0) {
         alpha = ct->getElementAsInt(i, alphaCol);
      }
      if (pointSizeCol >= 0) {
         pointSize = ct->getElementAsFloat(i, pointSizeCol);
      }
      if (lineSizeCol >= 0) {
         lineSize = ct->getElementAsFloat(i, lineSizeCol);
      }
      if (symbolCol >= 0) {
         symbol = ColorStorage::textToSymbol(ct->getElement(i, symbolCol));
      }
      
      addColor(name,
               red, 
               green,
               blue,
               alpha,
               pointSize,
               lineSize,
               symbol);
   }

   //
   // Do header
   //
   const StringTable* stHeader = csv.getDataSectionByName("header");
   if (stHeader != NULL) {
      readHeaderDataFromStringTable(*stHeader);
   }

}
      
/**
 * Read the color file.
 */
void
ColorFile::readFileData(QFile& file, QTextStream& stream, QDataStream&,
                                  QDomElement& rootElement) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         while(stream.atEnd() == false) {
            QString line;
            std::vector<QString> tokens;
            readLineIntoTokens(stream, line, tokens);
            
            if (tokens.size() < 4) {
               QString s("Invalid color file line: ");
               s.append(line);
               throw FileException(filename, s);
            }
            
            const QString name(tokens[0]);
            const int r = tokens[1].toInt();
            const int g = tokens[2].toInt();
            const int b = tokens[3].toInt();
            
            //
            // Fifth item is point size (optional & may be float)
            //
            float pointSize = 1.0;
            if (tokens.size() >= 5) {
               pointSize = tokens[4].toFloat();
               if (pointSize < 1) {
                  pointSize = 1;
               }
            }
            
            //
            // Sixth item is line-width or class-name (both optional and class-name is obsolete)
            //
            float lineSize = 1.0;
            if (tokens.size() >= 6) {
               if ((tokens[5][0] >= '0') && (tokens[5][0] <= '9')) {
                  lineSize = tokens[5].toFloat();
                  if (lineSize < 1) {
                     lineSize = 1;
                  }
               }
            }
            
            colors.push_back(ColorStorage(name, 
                                          static_cast<unsigned char>(r),
                                          static_cast<unsigned char>(g),
                                          static_cast<unsigned char>(b),
                                          pointSize,
                                          lineSize));
         }
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) { 
                  //
                  // Is this a "CellData" element
                  //
                  if (elem.tagName() == "Color") {
                     ColorStorage cs;
                     cs.readXML(node);
                     colors.push_back(cs);
                  }
                  else if ((elem.tagName() == xmlHeaderOldTagName) ||
                           (elem.tagName() == xmlHeaderTagName)) {
                     // ignore, read by AbstractFile::readFile()
                  }
                  else {
                     std::cout << "WARNING: unrecognized Color File element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
                  }
               }
               node = node.nextSibling();
            }
         }
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
 * Write the color file.
 */
void
ColorFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& xmlDoc,
                                  QDomElement& rootElement) throw (FileException)
{
   const int numColors = getNumberOfColors();

   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in Ascii format not supported.");
/*
         for (int i = 0; i < numColors; i++) {
            unsigned char r, g, b;
            getColorByIndex(i, r, g, b);
            float pointSize, lineSize;
            getPointLineSizeByIndex(i, pointSize, lineSize);
            stream << getColorNameByIndex(i) << " "
                   << r << " " << g << " " << b << " " 
                   << pointSize << " " << lineSize << " " << "\n";
         }
*/
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            //
            // Write the colors
            //
            for (int i = 0; i < numColors; i++) {
               ColorStorage* cs = getColor(i);
               cs->writeXML(xmlDoc, rootElement);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
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

//--------------------------------------------------------------------------------------------

/**
 * constructor.
 */
ColorFile::ColorStorage::ColorStorage() {
   name = "";
   rgba[0] = 0;
   rgba[1] = 0;
   rgba[2] = 0;
   rgba[3] = 255;
   symbol = SYMBOL_OPENGL_POINT;
   pointSize = 2.0;
   lineSize = 1;
   selected = true;
}

/**
 * constructor.
 */
ColorFile::ColorStorage::ColorStorage(const QString& nameIn,
             const unsigned char red, const unsigned char green, 
             const unsigned char blue,
             const float pointSizeIn, const float lineSizeIn) {
   name = nameIn;
   rgba[0] = red;
   rgba[1] = green;
   rgba[2] = blue;
   rgba[3] = 255;
   symbol = SYMBOL_OPENGL_POINT;
   pointSize = pointSizeIn;
   lineSize = lineSizeIn;
   selected = true;
}

/**
 * constructor.
 */
ColorFile::ColorStorage::ColorStorage(const QString& nameIn,
             const unsigned char red, const unsigned char green, 
             const unsigned char blue, const unsigned char alpha,
             const float pointSizeIn, const float lineSizeIn,
             const SYMBOL symbolIn) {
   name = nameIn;
   rgba[0] = red;
   rgba[1] = green;
   rgba[2] = blue;
   rgba[3] = alpha;
   symbol = symbolIn;
   pointSize = pointSizeIn;
   lineSize = lineSizeIn;
   selected = true;
}

/**
 * destructor.
 */
ColorFile::ColorStorage::~ColorStorage()
{
}

/**
 * is the color selected.
 */
bool 
ColorFile::ColorStorage::getSelected() const 
{ 
   return selected; 
}

/**
 * set the selection status.
 */
void 
ColorFile::ColorStorage::setSelected(const bool b) 
{ 
   selected = b; 
}

/**
 * get the name.
 */
QString 
ColorFile::ColorStorage::getName() const 
{ 
   return name; 
}

/**
 * set the name.
 */
void 
ColorFile::ColorStorage::setName(const QString& n)
{
   name = n;
}

/**
 * get the line size.
 */
float 
ColorFile::ColorStorage::getLineSize() const
{
   return lineSize;
}

/**
 * set the line size.
 */
void 
ColorFile::ColorStorage::setLineSize(const float s)
{
   lineSize = s;
}

/**
 * get the point size.
 */
float 
ColorFile::ColorStorage::getPointSize() const
{
   return pointSize;
}

/**
 * set the point size.
 */
void 
ColorFile::ColorStorage::setPointSize(const float s)
{
   pointSize = s;
}

/**
 * get the symbol.
 */
ColorFile::ColorStorage::SYMBOL 
ColorFile::ColorStorage::getSymbol() const
{
   return symbol;
}

/**
 * set the symbol.
 */
void 
ColorFile::ColorStorage::setSymbol(const SYMBOL s)
{
   symbol = s;
}
            
/**
 * get rgb colors.
 */
const unsigned char* 
ColorFile::ColorStorage::getRgb() const
{
   return &rgba[0];
}

/**
 * get the rgb colors.
 */
void 
ColorFile::ColorStorage::getRgb(unsigned char& r,
            unsigned char& g,
            unsigned char& b) const
{
   r = rgba[0];
   g = rgba[1];
   b = rgba[2];
}

/**
 * set the rgb colors.
 */
void 
ColorFile::ColorStorage::setRgb(const unsigned char rgbIn[3])
{
   rgba[0] = rgbIn[0];
   rgba[1] = rgbIn[1];
   rgba[2] = rgbIn[2];
   rgba[3] = 255;
}

/**
 * set the rgb colors.
 */
void 
ColorFile::ColorStorage::setRgb(const unsigned char r,
            const unsigned char g,
            const unsigned char b)
{
   rgba[0] = r;
   rgba[1] = g;
   rgba[2] = b;
   rgba[3] = 255;
}

/**
 * get rgba colors.
 */
const unsigned char* 
ColorFile::ColorStorage::getRgba() const
{
   return &rgba[0];
}

/**
 * get the rgba colors.
 */
void 
ColorFile::ColorStorage::getRgba(unsigned char& r,
            unsigned char& g,
            unsigned char& b,
            unsigned char& a) const
{
   r = rgba[0];
   g = rgba[1];
   b = rgba[2];
   a = rgba[3];
}

/**
 * set the rgba colors.
 */
void 
ColorFile::ColorStorage::setRgba(const unsigned char rgbaIn[4])
{
   rgba[0] = rgbaIn[0];
   rgba[1] = rgbaIn[1];
   rgba[2] = rgbaIn[2];
   rgba[3] = rgbaIn[3];
}

/**
 * set the rgba colors.
 */
void 
ColorFile::ColorStorage::setRgba(const unsigned char r,
            const unsigned char g,
            const unsigned char b,
            const unsigned char a)
{
   rgba[0] = r;
   rgba[1] = g;
   rgba[2] = b;
   rgba[3] = a;
}

/**
 * symbol enum to text.
 */
QString 
ColorFile::ColorStorage::symbolToText(const SYMBOL symbol)
{
   QString symbolStr("POINT");
   
   switch (symbol) {
      case SYMBOL_BOX:
         symbolStr = "BOX";
         break;
      case SYMBOL_DIAMOND:
         symbolStr = "DIAMOND";
         break;
      case SYMBOL_DISK:
         symbolStr = "DISK";
         break;
      case SYMBOL_OPENGL_POINT:
         symbolStr = "POINT";
         break;
      case SYMBOL_SPHERE:
         symbolStr = "SPHERE";
         break;
      case SYMBOL_RING:
         symbolStr = "RING";
         break;
      case SYMBOL_NONE:
         symbolStr = "NONE";
         break;
      case SYMBOL_SQUARE:
         symbolStr = "SQUARE";
         break;
   }
   
   return symbolStr;
}

/**
 * text to symbol enum.
 */
ColorFile::ColorStorage::SYMBOL 
ColorFile::ColorStorage::textToSymbol(const QString& symbolStrIn)
{
   SYMBOL symbol = SYMBOL_OPENGL_POINT;
   
   const QString symbolStr(symbolStrIn.toUpper());
   
   if (symbolStr == "BOX") {
      symbol = SYMBOL_BOX;
   }
   else if (symbolStr == "DIAMOND") {
      symbol = SYMBOL_DIAMOND;
   }
   else if (symbolStr == "DISK") {
      symbol = SYMBOL_DISK;
   }
   else if (symbolStr == "POINT") {
      symbol = SYMBOL_OPENGL_POINT;
   }
   else if (symbolStr == "SPHERE") {
      symbol = SYMBOL_SPHERE;
   }
   else if (symbolStr == "RING") {
      symbol = SYMBOL_RING;
   }
   else if (symbolStr == "NONE") {
      symbol = SYMBOL_NONE;
   }
   else if (symbolStr == "SQUARE") {
      symbol = SYMBOL_SQUARE;
   }
   else {
      std::cout << "WARNING: unrecognized symbol type: "
                << symbolStr.toAscii().constData()
                << ".  Defaulting to POINT." 
                << std::endl;
   }
   
   return symbol;
}

/**
 * get all symbol types as strings.
 */
void 
ColorFile::ColorStorage::getAllSymbolTypesAsStrings(std::vector<QString>& symbolStrings)
{
   symbolStrings.clear();
   for (int i = 0; i <= SYMBOL_NONE; i++) {
      symbolStrings.push_back(symbolToText(static_cast<SYMBOL>(i)));
   }
}

/**
 * called to read from an XML structure.
 */
void 
ColorFile::ColorStorage::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "Color") {
      QString msg("Incorrect element type passed to CellData::readXML(): \"");
      msg.append(elem.tagName());
      msg.append("\"");
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "name") {
            name = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "red") {
            rgba[0] = AbstractFile::getXmlElementFirstChildAsInt(elem);
         }
         else if (elem.tagName() == "green") {
            rgba[1] = AbstractFile::getXmlElementFirstChildAsInt(elem);
         }
         else if (elem.tagName() == "blue") {
            rgba[2] = AbstractFile::getXmlElementFirstChildAsInt(elem);
         }
         else if (elem.tagName() == "alpha") {
            rgba[3] = AbstractFile::getXmlElementFirstChildAsInt(elem);
         }
         else if (elem.tagName() == "pointSize") {
            pointSize = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == "lineSize") {
            lineSize = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == "symbol") {
            symbol = textToSymbol(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else {
            std::cout << "WARNING: unrecognized CellData element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      node = node.nextSibling();
   }
}
/**
 * called to write to an XML structure.
 */
void 
ColorFile::ColorStorage::writeXML(QDomDocument& xmlDoc,
                                  QDomElement&  parentElement)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement colorDataElement = xmlDoc.createElement("Color");

   //
   // color elements
   //
   AbstractFile::addXmlCdataElement(xmlDoc, colorDataElement, "name", name);
   AbstractFile::addXmlTextElement(xmlDoc, colorDataElement, "red", rgba[0]);
   AbstractFile::addXmlTextElement(xmlDoc, colorDataElement, "green", rgba[1]);
   AbstractFile::addXmlTextElement(xmlDoc, colorDataElement, "blue", rgba[2]);
   AbstractFile::addXmlTextElement(xmlDoc, colorDataElement, "alpha", rgba[3]);
   AbstractFile::addXmlTextElement(xmlDoc, colorDataElement, "pointSize", pointSize);
   AbstractFile::addXmlTextElement(xmlDoc, colorDataElement, "lineSize", lineSize);
   const QString symbolStr(symbolToText(symbol));
   AbstractFile::addXmlCdataElement(xmlDoc, colorDataElement, "symbol", symbolStr);
   
   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(colorDataElement);
}
