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

#include <QTextStream>

#include "StringTable.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
StringTable::StringTable(const int numRowsIn, const int numColsIn,
                           const QString& tableTitleIn)
{
   columnTitles = NULL;
   colMaxIntDigits = NULL;
   setNumberOfRowsAndColumns(numRowsIn, numColsIn, tableTitleIn);
}

/**
 * destructor.
 */
StringTable::~StringTable()
{
   clear();
}

/**
 * add a row to the table.
 */
void 
StringTable::addRow(const std::vector<QString>& newRow)
{
   const int numElements = static_cast<int>(newRow.size());
   if (numElements > 0) {
      if (numberOfCols <= 0) {
         setNumberOfRowsAndColumns(1, numElements, "");
      }
      else {
         stringTable.insert(stringTable.end(), numberOfCols, QString());
         numberOfRows++;
      }
   }

   const int lastRow = numberOfRows -1;
   for (int i = 0; i < numberOfCols; i++) {
      if (i < numElements) {
         setElement(lastRow, i, newRow[i]);
      }
   }
}
      
/**
 * clear the table.
 */
void 
StringTable::clear()
{   
   stringTable.clear();
   if (columnTitles != NULL) {
      delete[] columnTitles;
      columnTitles = NULL;
   }
   if (colMaxIntDigits != NULL) {
      delete[] colMaxIntDigits;
      colMaxIntDigits = NULL;
   }
   
   numberOfRows = 0;
   numberOfCols = 0;
   tableTitle = "";
}

/**
 * set the number of rows and columns.
 */
void 
StringTable::setNumberOfRowsAndColumns(const int numRowsIn, 
                                        const int numColsIn,
                                        const QString& tableTitleIn)
{
   clear();
   
   tableTitle = tableTitleIn;
   
   numberOfRows = numRowsIn;
   numberOfCols = numColsIn;
   
   const int numElements = numberOfRows * numberOfCols;
   if (numElements > 0) {
      stringTable.resize(numElements);
   }
   
   if (numberOfCols > 0) {
      columnTitles = new QString[numberOfCols];
      colMaxIntDigits = new int[numberOfCols];
      for (int i = 0; i < numberOfCols; i++) {
         colMaxIntDigits[i] = -1;
      }
   }
}

/**
 * get an element.
 */
QString 
StringTable::getElement(const int row, const int col) const
{
   return stringTable[getIndex(row, col)];
}

/**
 * get an element as an int.
 */
int 
StringTable::getElementAsInt(const int row, const int col) const
{
   return getElement(row, col).toInt();
}

/**
 * get an element as a float.
 */
float 
StringTable::getElementAsFloat(const int row, const int col) const
{
   //
   // Watch for "en-dash", "em-dash" -- unicode characters 8208 to 8213
   // so use the StringUtilities::toFloat() which will catch these problems
   //
   return StringUtilities::toFloat(getElement(row, col));
}
      
/**
 * get an element as a double.
 */
double 
StringTable::getElementAsDouble(const int row, const int col) const
{
   //
   // Watch for "en-dash", "em-dash" -- unicode characters 8208 to 8213
   // so use the StringUtilities::toDouble() which will catch these problems
   //
   return StringUtilities::toDouble(getElement(row, col));
}
      
/**
 * get a point to an element.
 */
const QString* 
StringTable::getElementPointer(const int row, const int col) const
{
   return &stringTable[getIndex(row, col)];
}

/**
 * set an element.
 */
void 
StringTable::setElement(const int row, const int col, const QString& s)
{
   stringTable[getIndex(row, col)] = s;
}

/**
 * set an element.
 */
void 
StringTable::setElement(const int row, const int col, const int i)
{
   QString s;
   s.setNum(i);
   if (colMaxIntDigits[col] > 0) {
      const int num = colMaxIntDigits[col] - s.length();
      for (int j = 0; j < num; j++) {
         s.insert(0, '0');
      }
   }
   stringTable[getIndex(row, col)] = s;
}

/**
 * set an element.
 */
void 
StringTable::setElement(const int row, const int col, const float f)
{
   stringTable[getIndex(row, col)].setNum(f, 'f', 6);
}

/**
 * set an element.
 */
void 
StringTable::setElement(const int row, const int col, const double f)
{
   stringTable[getIndex(row, col)].setNum(f, 'f', 24);
}

/**
 * Set max integer for column.  Must be called before setElement().
 * As the integers are converted to strings they are padded with zeros
 * so that they can be sorted correctly when sorted as text strings.
 */
void 
StringTable::setColumnMaxInteger(const int col, const int maxInt)
{
   colMaxIntDigits[col] = static_cast<int>(std::log10(static_cast<double>(maxInt))) + 1;
   if (colMaxIntDigits[col] <= 0) {
      colMaxIntDigits[col] = 1;
   }
}      

/**
 * set array element.
 */
void 
StringTable::setElement(const int row, const int col, const int i[], const int num)
{
   QString s;
   for (int j = 0; j < num; j++) {
      s.append(QString::number(i[j]) + " ");
   }
   setElement(row, col, s);
}

/**
 * set array element.
 */
void 
StringTable::setElement(const int row, const int col, const float f[], const int num)
{
   QString s;
   for (int j = 0; j < num; j++) {
      s.append(QString::number(f[j], 'f', 6) + " ");
   }
   setElement(row, col, s);
}

/**
 * get array element.
 */
void 
StringTable::getElement(const int row, const int col, int i[], const int num) const
{
   QString s = getElement(row, col);
   QTextStream stream(&s, QIODevice::ReadOnly);
   for (int j = 0; j < num; j++) {
      stream >> i[j];
   }
}

/**
 * get array element.
 */
void 
StringTable::getElement(const int row, const int col, float f[], const int num) const
{
   QString s = getElement(row, col);
   QTextStream stream(&s, QIODevice::ReadOnly);
   for (int j = 0; j < num; j++) {
      stream >> f[j];
   }
}

/**
 * get the index of a column with the specified title.
 */
int 
StringTable::getColumnIndexFromName(const QString& columnTitleIn) const
{
   const QString columnTitle(columnTitleIn.toLower());
   
   const int num = getNumberOfColumns();
   for (int i = 0; i < num; i++) {
      if (columnTitle == getColumnTitle(i).toLower()) {
         return i;
      }
   }
   
   return -1;
}
      

