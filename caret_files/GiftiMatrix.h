
#ifndef __GIFTI_MATRIX_H__
#define __GIFTI_MATRIX_H__

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

#include <QString>
#include <vector>

class QTextStream;

/// matrix used with coordinates
class GiftiMatrix {
   public:
      // constructor
      GiftiMatrix();
      
      // copy constructor
      GiftiMatrix(const GiftiMatrix& nlt);
      
      // destructor
      ~GiftiMatrix();

      // assignment operator
      GiftiMatrix& operator=(const GiftiMatrix& nlt);
      
      // clear the matrix (removes spaces, sets matrix to identity)
      void clear();
      
      // is the matrix empty (identity matrix with spaces unknown)
      bool isEmpty() const;
      
      // is the matrix the identity matrix ?
      bool isIdentity() const;
      
      /// get the data (pre-transformed) space name
      QString getDataSpaceName() const;
      
      // set the data (pre-transformed) space name
      void setDataSpaceName(const QString& space);
      
      /// get the transformed space name
      QString getTransformedSpaceName() const;
   
      // set the transformed space name
      void setTransformedSpaceName(const QString& space);
      
      // get the matrix
      void getMatrix(double matrix[4][4]) const;
      
      // set the matrix
      void setMatrix(const double matrix[4][4]);
      
      // multiply a point
      void multiplyPoint(double& x, double& y, double& z) const;
      
      // multiply a point
      void multiplyPoint(double xyz[3]) const;
      
      // write the label table
      void writeAsXML(QTextStream& stream,
                      const int indentOffset) const;
                      
   protected:
      // copy helper used by copy constructor and assignement operators
      void copyHelperGiftiMatrix(const GiftiMatrix& gm);
      
      // the matrix
      double m[4][4];
      
      // the data space name
      QString dataSpaceName;
      
      // the transformed space name
      QString transformedSpaceName;
      
      /*** IF ADDITIONAL MEMBERS ARE ADDED UPDATE copyHelperGiftiLabelTable() ***/
};

#endif // __GIFTI_MATRIX_H__
