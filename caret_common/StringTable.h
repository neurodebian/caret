#ifndef __STRING_TABLE_H__
#define __STRING_TABLE_H__

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

#include <vector>
#include <QString>

/// class for storing a table of QString's
class StringTable {
   public:
      // constructor
      StringTable(const int numRowsIn, const int numColumnsIn,
                   const QString& tableTitleIn = "");
      
      // destructor
      ~StringTable();
      
      // clear the table
      void clear();
      
      // add a row to the table
      void addRow(const std::vector<QString>& newRow);
      
      // get an element
      QString getElement(const int row, const int col) const;
      
      // get an element as an int
      int getElementAsInt(const int row, const int col) const;
      
      // get an element as a float
      float getElementAsFloat(const int row, const int col) const;
      
      // get an element as a double
      double getElementAsDouble(const int row, const int col) const;
      
      // get a point to an element
      const QString* getElementPointer(const int row, const int col) const;
      
      // set an element
      void setElement(const int row, const int col, const QString& s);
      
      // set an element
      void setElement(const int row, const int col, const int i);
      
      // set an element
      void setElement(const int row, const int col, const float f);
      
      // set an element
      void setElement(const int row, const int col, const double f);
      
      // set array element
      void setElement(const int row, const int col, const int i[], const int num);
      
      // set array element
      void setElement(const int row, const int col, const float f[], const int num);
      
      // get array element
      void getElement(const int row, const int col, int i[], const int num) const;
      
      // get array element
      void getElement(const int row, const int col, float f[], const int num) const;
      
      /// get a column title
      QString getColumnTitle(const int col) const { return columnTitles[col]; }
      
      /// get the index of a column with the specified title
      int getColumnIndexFromName(const QString& columnTitle) const;
      
      /// set a column title
      void setColumnTitle(const int col, const QString& s) { columnTitles[col] = s; }
      
      /// get the number of rows
      int getNumberOfRows() const { return numberOfRows; }
      
      /// get the number of columns
      int getNumberOfColumns() const { return numberOfCols; }
      
      // set the number of rows and columns
      void setNumberOfRowsAndColumns(const int numRows, 
                                     const int numCols,
                                     const QString& tableTitleIn);
                                     
      // set max integer for column
      void setColumnMaxInteger(const int col, const int maxInt);
      
      /// get the table title
      QString getTableTitle() const { return tableTitle; }
      
      /// set the table title
      void setTableTitle(const QString& tt) { tableTitle = tt; }
      
   protected:
      /// compute an index to an element
      int getIndex(const int row, const int col) const { return (row * numberOfCols + col); }
      
      /// the string table
      std::vector<QString> stringTable;
      
      /// number of rows in the string table
      int numberOfRows;
      
      /// number of columns in the string table
      int numberOfCols;
      
      /// titles of columns
      QString* columnTitles;
      
      /// max number of integer digits in a column
      int* colMaxIntDigits;
      
      /// title of table
      QString tableTitle;
};

#endif // __STRING_TABLE_H__

