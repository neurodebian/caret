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



#ifndef __VE_WUNIL_HEADER_H__
#define __VE_WUNIL_HEADER_H__

#include <QString>
#include <vector>

#include "FileException.h"

/// class for storing a Washingtion University Neuro Imaging Laboratory attribute
class WuNilAttribute {
   public:
      /// constructor for single double
      WuNilAttribute(const QString& name, const double value);
      
      /// constructor for single float
      WuNilAttribute(const QString& name, const float value);
      
      /// constructor for single int
      WuNilAttribute(const QString& name, const int value);
      
      /// constructor for array of floats
      WuNilAttribute(const QString& name, const float values[], const int numValues);
      
      /// constructor for array of ints
      WuNilAttribute(const QString& name, const int values[], const int numValues);
      
      /// constructor for vector of floats
      WuNilAttribute(const QString& name, const std::vector<float>& values);
      
      /// constructor for vector of ints
      WuNilAttribute(const QString& name, const std::vector<int>& values);
      
      /// constructor
      WuNilAttribute(const QString& name, const QString& value);
      
      /// get the values for an int attribute
      void getValue(std::vector<int>& valueOut) const;
      
      /// get the values for a float
      void getValue(std::vector<float>& valueOut) const;
      
      /// get the value for a string
      QString getValue() const { return value; }
      
      /// name of the attribute
      QString attributeName;
      
      /// storage for a string attribute
      QString value;
      
      static const QString NAME_CARET_METADATA;
      static const QString NAME_NUMBER_FORMAT;
      static const QString NAME_NUMBER_OF_BYTES_PER_PIXEL;
      static const QString NAME_ORIENTATION;
      static const QString NAME_NUMBER_OF_DIMENSIONS;
      static const QString NAME_SCALING_FACTOR_1;
      static const QString NAME_SCALING_FACTOR_2;
      static const QString NAME_SCALING_FACTOR_3;
      static const QString NAME_MATRIX_SIZE_1;
      static const QString NAME_MATRIX_SIZE_2;
      static const QString NAME_MATRIX_SIZE_3;
      static const QString NAME_MATRIX_SIZE_4;
      static const QString NAME_DATE;
      static const QString NAME_CENTER;
      static const QString NAME_MMPPIX;
      static const QString NAME_CONVERSION_PROGRAM;
      static const QString NAME_REGION_NAME;
      static const QString NAME_IMAGEDATA_BYTE_ORDER;
};

#ifdef __WUNIL_HEADER_DEFINE__

   const QString WuNilAttribute::NAME_CARET_METADATA = "caret_metadata";
   const QString WuNilAttribute::NAME_NUMBER_FORMAT = "number format";
   const QString WuNilAttribute::NAME_NUMBER_OF_BYTES_PER_PIXEL = "number of bytes per pixel";
   const QString WuNilAttribute::NAME_ORIENTATION = "orientation";
   const QString WuNilAttribute::NAME_NUMBER_OF_DIMENSIONS = "number of dimensions";
   const QString WuNilAttribute::NAME_SCALING_FACTOR_1 = "scaling factor (mm/pixel) [1]";
   const QString WuNilAttribute::NAME_SCALING_FACTOR_2 = "scaling factor (mm/pixel) [2]";
   const QString WuNilAttribute::NAME_SCALING_FACTOR_3 = "scaling factor (mm/pixel) [3]";
   const QString WuNilAttribute::NAME_MATRIX_SIZE_1 = "matrix size [1]";
   const QString WuNilAttribute::NAME_MATRIX_SIZE_2 = "matrix size [2]";
   const QString WuNilAttribute::NAME_MATRIX_SIZE_3 = "matrix size [3]";
   const QString WuNilAttribute::NAME_MATRIX_SIZE_4 = "matrix size [4]";
   const QString WuNilAttribute::NAME_DATE   = "date";
   const QString WuNilAttribute::NAME_CENTER = "center";
   const QString WuNilAttribute::NAME_MMPPIX = "mmppix";
   const QString WuNilAttribute::NAME_CONVERSION_PROGRAM = "conversion program";
   const QString WuNilAttribute::NAME_REGION_NAME = "region names";
   const QString WuNilAttribute::NAME_IMAGEDATA_BYTE_ORDER = "imagedata byte order";

#endif // __WUNIL_HEADER_DEFINE__


/// class for storing a Washingtion University Neuro Imaging Laboratory Header File
class WuNilHeader {
   public:
      /// Constructor
      WuNilHeader();
      
      /// Destructor
      ~WuNilHeader();
      
      /// add an attribute
      void addAttribute(WuNilAttribute& attr);
      
      /// clear the header
      void clear();
      
      /// Get the number of attributes
      int getNumberOfAttributes() const { return attributes.size(); }
      
      /// Get an attribute by its index
      WuNilAttribute* getAttribute(const int index);
      
      /// Get an attribute by its name (returns NULL if not found)
      WuNilAttribute* getAttribute(const QString& name);
      
      /// Get an attributes index from its name (returns -1 if not found)
      int getAttributeIndexFromName(const QString& name) const;
      
      /// get the region names
      void getRegionNames(std::vector<QString>& names) const;
      
      /// set the region names
      void setRegionNames(const std::vector<QString>& names);
      
      /// read a header file
      void readHeader(QFile& file, QTextStream& stream) throw (FileException);
      
      /// Write to a header file
      void writeHeader(QTextStream& stream) throw (FileException);
      
      /// convert a voxel indices into stereotaxic coordinates
      static void voxelIndicesToStereotaxicCoordinates(const int dim[3],
                                                       const float center[3],
                                                       const float mmpix[3],
                                                       const int voxelIndices[3],
                                                       float coordsOut[3]);
                                          
   private:
      /// write an attribute with the specified name
      void writeAttribute(QTextStream& stream, const QString& attributeName,
                            std::vector<bool>& attributeWrittenFlags);      
      
      /// write an attribute with the specified index
      void writeAttribute(QTextStream& stream, const int index);      
      
      /// used for flipping coordinate stuff
      static void vrtflip(const int imgdim[3],
                          const float centeri[3],
                          const float mmppixi[3],
                          float centert[3],
                          float mmppixt[3]);
                          
      /// storage for the attributes
      std::vector<WuNilAttribute> attributes;

      /// the region names
      std::vector<QString> nilRegionNames;
};

#endif //  __VE_WUNIL_HEADER_H__

