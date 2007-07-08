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
#include <sstream>

#include <QDateTime>
#include <QFile>
#include <QGlobalStatic>
#include <QString>
#include <QTextStream>

#define __WUNIL_HEADER_DEFINE__
#include "WuNilHeader.h"
#undef __WUNIL_HEADER_DEFINE__
#include "DebugControl.h"
#include "FileUtilities.h"
#include "StringUtilities.h"
/**
 * Constructor.
 */
WuNilHeader::WuNilHeader()
{
   clear();
}

/**
 * Destructor.
 */
WuNilHeader::~WuNilHeader()
{
}

/**
 * Clear the afni header and set to default values
 */
void
WuNilHeader::clear()
{
   //
   // Create the required default attributes assuming a single subvolume
   //
   attributes.clear();
   WuNilAttribute format(WuNilAttribute::NAME_NUMBER_FORMAT, "float");
   addAttribute(format);
   
   WuNilAttribute bpp(WuNilAttribute::NAME_NUMBER_OF_BYTES_PER_PIXEL, 4);
   addAttribute(bpp);
   
   WuNilAttribute orient(WuNilAttribute::NAME_ORIENTATION, 2);
   addAttribute(orient);
   
   WuNilAttribute numdim(WuNilAttribute::NAME_NUMBER_OF_DIMENSIONS, 4);
   addAttribute(numdim);

   WuNilAttribute s1(WuNilAttribute::NAME_SCALING_FACTOR_1, 1.0);
   addAttribute(s1);

   WuNilAttribute s2(WuNilAttribute::NAME_SCALING_FACTOR_2, 1.0);
   addAttribute(s2);

   WuNilAttribute s3(WuNilAttribute::NAME_SCALING_FACTOR_3, 1.0);
   addAttribute(s3);

   WuNilAttribute m1(WuNilAttribute::NAME_MATRIX_SIZE_1, 0);
   addAttribute(m1);

   WuNilAttribute m2(WuNilAttribute::NAME_MATRIX_SIZE_2, 0);
   addAttribute(m2);   

   WuNilAttribute m3(WuNilAttribute::NAME_MATRIX_SIZE_3, 0);
   addAttribute(m3);
   
   WuNilAttribute m4(WuNilAttribute::NAME_MATRIX_SIZE_4, 0);
   addAttribute(m4);
   
   WuNilAttribute nd(WuNilAttribute::NAME_DATE, 
                     QDateTime::currentDateTime().toString("ddd MMM d hh:mm::ss yyyy"));
   addAttribute(nd);
   
   //WuNilAttribute nc(WuNilAttribute::NAME_CENTER, "0.0 0.0 0.0");
   //addAttribute(nc);
   
   WuNilAttribute nm(WuNilAttribute::NAME_MMPPIX, "1.0 -1.0 -1.0");
   addAttribute(nm);
   
   WuNilAttribute prog(WuNilAttribute::NAME_CONVERSION_PROGRAM, "caret");
   addAttribute(prog);
   
   nilRegionNames.clear();
}

/**
 * add an attribute (replaces it if it exists)
 */
void 
WuNilHeader::addAttribute(WuNilAttribute& attr) 
{
   WuNilAttribute* a = getAttribute(attr.attributeName);
   if (a != NULL) {
      *a = attr;
   }
   else {
      attributes.push_back(attr);
   }
}

/**
 * Get an attribute by its index
 */
WuNilAttribute* 
WuNilHeader::getAttribute(const int index)
{
   if (index < getNumberOfAttributes()) {
      return &attributes[index];
   }
   return NULL;
}

/**
 * get an attribute for the specified name (returns NULL if not found)
 */
WuNilAttribute* 
WuNilHeader::getAttribute(const QString& name)
{
   const int numAttrs = getNumberOfAttributes();
   for (int i = 0; i < numAttrs; i++) {
      if (attributes[i].attributeName == name) {
         return &attributes[i];
      }
   }
   return NULL;
}

/**
 * Get an attributes index from its name (returns -1 if not found)
 */
int 
WuNilHeader::getAttributeIndexFromName(const QString& name) const
{
   const int numAttrs = getNumberOfAttributes();
   for (int i = 0; i < numAttrs; i++) {
      if (attributes[i].attributeName == name) {
         return i;
      }
   }
   return -1;
}

/**
 * get the region names.
 */
void 
WuNilHeader::getRegionNames(std::vector<QString>& names) const
{
   names = nilRegionNames;
}

/**
 * set the region names.
 */
void 
WuNilHeader::setRegionNames(const std::vector<QString>& names)
{
   nilRegionNames = names;
}

/**
 * Read an WuNil header.
 */
void
WuNilHeader::readHeader(QFile& /*file*/, QTextStream& stream) throw (FileException)
{
   //
   // Read until end of file
   //
   while(stream.atEnd() == false) {
      const QString line(stream.readLine());
      if (line.isEmpty() == false) {
          std::vector<QString> tokens;
          StringUtilities::tokenSingleSeparator(line, ":=", tokens);
          if (tokens.size() == 2) {
             const QString key(StringUtilities::trimWhitespace(tokens[0]));
             if (key == WuNilAttribute::NAME_REGION_NAME) {
                const QString label(tokens[1]);
                StringUtilities::tokenSingleSeparator(label, " ", tokens);
                if (tokens.size() >= 2) {
                   nilRegionNames.push_back(tokens[1]);
                }                
             }
             else {
               const QString value(StringUtilities::trimWhitespace(tokens[1]));
               if (DebugControl::getDebugOn()) {
                  std::cout << "key '" << key.toAscii().constData() << "'" << std::endl;
                  std::cout << "value '" << value.toAscii().constData() << "'" << std::endl;
                  std::cout << std::endl;
               }
               WuNilAttribute attr(key, value);
               addAttribute(attr);
            }
          }
       }
    }
}

/**
 * Write an AFNI header.
 */
void
WuNilHeader::writeHeader(QTextStream& stream) throw (FileException)
{
   //
   // Set the byte ordering
   //
   //int wordSize;
   bool bigEndian = (QSysInfo::ByteOrder == QSysInfo::BigEndian);       
   //qSysInfo(&wordSize, &bigEndian);
   if (bigEndian == false) {        
      // need to swap bytes when writing volume file since 4dfp always MSB  
   }
   
   //
   // Set the date
   //
   WuNilAttribute dat(WuNilAttribute::NAME_DATE,
                      QDateTime::currentDateTime().toString("ddd MMM d hh:mm::ss yyyy"));
   addAttribute(dat);
   
   //
   // Write all of the attributes
   //
   const int numAttr = getNumberOfAttributes();
   std::vector<bool> attributeWritten(numAttr, false);
   
   writeAttribute(stream, WuNilAttribute::NAME_NUMBER_FORMAT, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_NUMBER_OF_BYTES_PER_PIXEL, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_ORIENTATION, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_NUMBER_OF_DIMENSIONS, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_SCALING_FACTOR_1, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_SCALING_FACTOR_2, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_SCALING_FACTOR_3, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_MATRIX_SIZE_1, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_MATRIX_SIZE_2, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_MATRIX_SIZE_3, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_MATRIX_SIZE_4, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_CENTER, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_MMPPIX, attributeWritten);
   writeAttribute(stream, WuNilAttribute::NAME_DATE, attributeWritten);
   
   //
   // Write all attributes that have not already been written
   //
   for (int i = 0; i < numAttr; i++) {
      if (attributeWritten[i] == false) {
         writeAttribute(stream, i);
      }
   }
   
   //
   // Write the region names (start at one to skip "???"
   //
   const int numRegions = static_cast<int>(nilRegionNames.size());
   for (int i = 0; i < numRegions; i++) {
      QString s(WuNilAttribute::NAME_REGION_NAME);
      if (s.length() < 34) { 
         //s.setLength(34);
         //s.fill(' ', 34);
         s = s.leftJustified(34, ' ');
      }
      stream << s << " := " << i << " " << nilRegionNames[i] << "\n";
   }
}

/**
 * Write the specified attribute
 */
void
WuNilHeader::writeAttribute(QTextStream& stream, const QString& attributeName,
                            std::vector<bool>& attributeWrittenFlags)
{
   const int index = getAttributeIndexFromName(attributeName);
   if (index >= 0) {
      writeAttribute(stream, index);
      attributeWrittenFlags[index] = true;
   }
}

/**
 * Write attribute at specified index.
 */
void 
WuNilHeader::writeAttribute(QTextStream& stream, const int index)
{
   if (index >= 0) {
      const WuNilAttribute* attr = getAttribute(index);
      QString s(attr->attributeName);
      if (s.length() < 34) { 
         //s.setLength(34);
         //s.fill(34, ' ');
         s = s.leftJustified(34, ' ');
      }
      stream << s << " := " << attr->value << "\n";
   }
}

/**
 * convert a voxel indices into stereotaxic coordinates.
 * From Avi Snyder's index2at.c and ft4imgo.f.
 */
void 
WuNilHeader::voxelIndicesToStereotaxicCoordinates(const int dim[3],
                                                 const float center[3],
                                                 const float mmpix[3],
                                                 const int voxelIndicesIn[3],
                                                 float coordsOut[3])
{
   float centerr[3], mmppixr[3];
   
   vrtflip(dim, center, mmpix, centerr, mmppixr);
   
   const int voxelIndex[3] = { voxelIndicesIn[0] + 1,
                               voxelIndicesIn[1] + 1,
                               voxelIndicesIn[2] + 1 };
                               
   for (int i = 0; i < 3; i++) {
      coordsOut[i] = voxelIndex[i] * mmppixr[i] - centerr[i];
   }                           
}

/**
 * used for flipping coordinate stuff.
 * From Avi Snyder's index2at.c and ft4imgo.f.
 */
void 
WuNilHeader::vrtflip(const int imgdim[3],
                        const float centeri[3],
                        const float mmppixi[3],
                        float centert[3],
                        float mmppixt[3])
{
   float flips[3] = { -1, 1, -1 };
   
   for (int i = 0; i < 3; i++) {
      mmppixt[i] = mmppixi[i] * flips[i];
      centert[i] = centeri[i] * flips[i];
      if (flips[i] < 0) {
         centert[i] = mmppixt[i] * static_cast<float>(imgdim[i] + 1) - centert[i];
      }
   }
}                          

//================================================================================
//
// WuNilAttribute methods
//

/**
 * constructor for single double but stored as float.  The compiler treats a
 * floating point number (e.g. 4.5) as a double so this contructor is required.
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const double valueIn)
{
   attributeName = name;
   value = QString::number(valueIn, 'f', 6);
}

/**
 * constructor for single float
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const float valueIn)
{
   attributeName = name;
   value = QString::number(valueIn, 'f', 6);
}

/**
 * constructor for single int 
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const int valueIn)
{
   attributeName = name;
   value = QString::number(valueIn);
}

/**
 * constructor for float array
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const float values[], const int numValues)
{
   attributeName = name;
   std::vector<QString> sv;
   for (int i = 0; i < numValues; i++) {
      sv.push_back(QString::number(values[i], 'f', 6));
   }
   value = StringUtilities::combine(sv, " ");
}

/**
 * constructor for int array 
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const int values[], const int numValues)
{
   attributeName = name;
   std::vector<QString> sv;
   for (int i = 0; i < numValues; i++) {
      sv.push_back(QString::number(values[i]));
   }
   value = StringUtilities::combine(sv, " ");
}

/**
 * constructor for vector of floats
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const std::vector<float>& values)
{
   attributeName = name;
   std::vector<QString> sv;
   for (int i = 0; i < static_cast<int>(values.size()); i++) {
      sv.push_back(QString::number(values[i], 'f', 6));
   }
   value = StringUtilities::combine(sv, " ");
}

/**
 * constructor for vector of ints
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const std::vector<int>& values)
{
   attributeName = name;
   std::vector<QString> sv;
   for (int i = 0; i < static_cast<int>(values.size()); i++) {
      sv.push_back(QString::number(values[i]));
   }
   value = StringUtilities::combine(sv, " ");
}

/**
 * constructor for string
 */ 
WuNilAttribute::WuNilAttribute(const QString& name, const QString& valueIn)
{
   attributeName = name;
   value = valueIn;
}
      
/**
 * Get an attribute value.
 */
void
WuNilAttribute::getValue(std::vector<int>& valueOut) const
{
   valueOut.clear();
   std::vector<QString> tokens;
   StringUtilities::token(value, " ", tokens);
   for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
      valueOut.push_back(QString(tokens[i]).toInt());
   }
}

/**
 * Get an attribute value.
 */
void
WuNilAttribute::getValue(std::vector<float>& valueOut) const
{
   valueOut.clear();
   std::vector<QString> tokens;
   StringUtilities::token(value, " ", tokens);
   for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
      valueOut.push_back(QString(tokens[i]).toFloat());
   }
}

