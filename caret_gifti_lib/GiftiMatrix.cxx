
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

#include <QTextStream>

#include "GiftiCommon.h"
#include "GiftiMatrix.h"

/**
 * constructor.
 */
GiftiMatrix::GiftiMatrix()
{
   clear();
}

/**
 * copy constructor.
 */
GiftiMatrix::GiftiMatrix(const GiftiMatrix& gm)
{
   copyHelperGiftiMatrix(gm);
}

/**
 * destructor.
 */
GiftiMatrix::~GiftiMatrix()
{
    clear();
}

/**
 * assignment operator.
 */
GiftiMatrix& 
GiftiMatrix::operator=(const GiftiMatrix& gm)
{
   if (this != &gm) {
      copyHelperGiftiMatrix(gm);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignement operators.
 */
void 
GiftiMatrix::copyHelperGiftiMatrix(const GiftiMatrix& gm)
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m[i][j] = gm.m[i][j];
      }
   }
   
   dataSpaceName     = gm.dataSpaceName;
   transformedSpaceName = gm.transformedSpaceName;
}

/**
 * clear the matrix (removes spaces, sets matrix to identity).
 */
void 
GiftiMatrix::clear()
{
   m[0][0] = 1.0;
   m[0][1] = 0.0;
   m[0][2] = 0.0;
   m[0][3] = 0.0;
   m[1][0] = 0.0;
   m[1][1] = 1.0;
   m[1][2] = 0.0;
   m[1][3] = 0.0;
   m[2][0] = 0.0;
   m[2][1] = 0.0;
   m[2][2] = 1.0;
   m[2][3] = 0.0;
   m[3][0] = 0.0;
   m[3][1] = 0.0;
   m[3][2] = 0.0;
   m[3][3] = 1.0;
   
   dataSpaceName     = "";
   transformedSpaceName = "";
}

/**
 * is the matrix empty (identity matrix with no spaces specified).
 */
bool 
GiftiMatrix::isEmpty() const
{
   return (dataSpaceName.isEmpty() &&
           transformedSpaceName.isEmpty() &&
           isIdentity());
}
      
/**
 * is the matrix the identity matrix ?
 */
bool 
GiftiMatrix::isIdentity() const
{
   if ((m[0][0] == 1.0) &&
       (m[0][1] == 0.0) &&
       (m[0][2] == 0.0) &&
       (m[0][3] == 0.0) &&
       (m[1][0] == 0.0) &&
       (m[1][1] == 1.0) &&
       (m[1][2] == 0.0) &&
       (m[1][3] == 0.0) &&
       (m[2][0] == 0.0) &&
       (m[2][1] == 0.0) &&
       (m[2][2] == 1.0) &&
       (m[2][3] == 0.0) &&
       (m[3][0] == 0.0) &&
       (m[3][1] == 0.0) &&
       (m[3][2] == 0.0) &&
       (m[3][3] == 1.0)) {
      return true;
   }
   return false;
}

/**
 * get the data (pre-transformed) space name.
 */
QString 
GiftiMatrix::getDataSpaceName() const 
{ 
   return dataSpaceName; 
}

/**
 * set the data (pre-transformed) space name.
 */
void 
GiftiMatrix::setDataSpaceName(const QString& spaceName)
{
   dataSpaceName = spaceName;
}

/**
 * get the transformed space name.
 */
QString 
GiftiMatrix::getTransformedSpaceName() const 
{ 
   return transformedSpaceName; 
}

/**
 * set the transformed space name.
 */
void 
GiftiMatrix::setTransformedSpaceName(const QString& spaceName)
{
   transformedSpaceName = spaceName;
}

/**
 * get the matrix.
 */
void 
GiftiMatrix::getMatrix(double matrix[4][4]) const
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[i][j] = m[i][j];
      }
   }
}

/**
 * set the matrix.
 */
void 
GiftiMatrix::setMatrix(const double matrix[4][4])
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m[i][j] = matrix[i][j];
      }
   }
}

/**
 * multiply a point.
 */
void 
GiftiMatrix::multiplyPoint(double& x, double& y, double& z) const
{
   const double xp = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
   const double yp = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
   const double zp = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
   x = xp;
   y = yp;
   z = zp;
}

/**
 * multiply a point.
 */
void 
GiftiMatrix::multiplyPoint(double xyz[3]) const
{
   multiplyPoint(xyz[0], xyz[1], xyz[2]);
}
      
/**
 * write metadata (used by other classes so static).
 */
void 
GiftiMatrix::writeAsXML(QTextStream& stream,
                        const int indentOffset) const
{
   if (isEmpty()) {
      return;
   }
   
   int indent = indentOffset;
   
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagMatrix << ">" << "\n";
   indent++;
   
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagMatrixDataSpace << "><![CDATA["
                 << dataSpaceName 
          << "]]></" << GiftiCommon::tagMatrixDataSpace << ">" << "\n";
          
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagMatrixTransformedSpace << "><![CDATA["
                 << transformedSpaceName 
          << "]]></" << GiftiCommon::tagMatrixTransformedSpace << ">" << "\n";
          
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "<" << GiftiCommon::tagMatrixData << ">" << "\n";
          
   indent++;
   for (int i = 0; i < 4; i++) {
      GiftiCommon::writeIndentationXML(stream, indent);
      for (int j = 0; j < 4; j++) {
         stream << m[i][j] << " ";
      }
      stream << "\n";
   }
   indent--;
   
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "</" << GiftiCommon::tagMatrixData << ">" << "\n";

   indent--;
   GiftiCommon::writeIndentationXML(stream, indent);
   stream << "</" << GiftiCommon::tagMatrix << ">" << "\n";
}
