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



#ifndef __VE_AFNI_ATTRIBUTES_H__
#define __VE_AFNI_ATTRIBUTES_H__

#include <QString>
#include <vector>

#include "FileException.h"

class VolumeFile;
class nifti_1_header;

/// class for storing an AFNI attribute
/// For more info: http://afni.nimh.nih.gov/afni/docREADME/README.attributes
class AfniAttribute {
   public:
      enum ATTRIBUTE_TYPE {
         ATTRIBUTE_TYPE_FLOAT,
         ATTRIBUTE_TYPE_INTEGER,
         ATTRIBUTE_TYPE_STRING
      };
      
      /// constructor for single double but stored as float.
      AfniAttribute(const QString& name, const double value);
      
      /// constructor for single float
      AfniAttribute(const QString& name, const float value);
      
      /// constructor for single int
      AfniAttribute(const QString& name, const int value);
      
      /// constructor for array of floats
      AfniAttribute(const QString& name, const float values[], const int numValues);
      
      /// constructor for array of ints
      AfniAttribute(const QString& name, const int values[], const int numValues);
      
      /// constructor for vector of floats
      AfniAttribute(const QString& name, const std::vector<float>& values);
      
      /// constructor for vector of ints
      AfniAttribute(const QString& name, const std::vector<int>& values);
      
      /// constructor
      AfniAttribute(const QString& name, const QString& value);
      
      /// get the name of the attribute
      QString getName() const { return attributeName; }
      
      /// get the type of the attribute
      ATTRIBUTE_TYPE getType() const { return attributeType; }
      
      /// get the values for an int attribute
      void getValue(std::vector<int>& value) const { value = intValue; }
      
      /// get the values for a float
      void getValue(std::vector<float>& value) const { value = floatValue; }
      
      /// get the value for a string
      QString getValue() const;
      
      /// type of the attribute
      ATTRIBUTE_TYPE attributeType;
      
      /// name of the attribute
      QString attributeName;
      
      /// storage for a string attribute
      QString stringValue;
      
      /// storage for float value(s)
      std::vector<float> floatValue;
      
      /// storage for integer value(s)
      std::vector<int> intValue;
      
      static const QString NAME_BRICK_FLOAT_FACS;
      static const QString NAME_BRICK_LABS;
      static const QString NAME_BRICK_STATS;
      static const QString NAME_BRICK_TYPES;
      static const QString NAME_BRICK_COMMENTS;
      static const QString NAME_BYTEORDER_STRING;
      static const QString NAME_CARET_METADATA_LINK;
      static const QString NAME_CARET_TYPE;
      static const QString NAME_CARET_PUBMED_ID;
      static const QString NAME_DATASET_NAME;
      static const QString NAME_DATASET_DIMENSIONS;
      static const QString NAME_DATASET_RANK;
      static const QString NAME_DELTA;
      static const QString NAME_HISTORY_NOTE;
      static const QString NAME_IDCODE_DATE;
      static const QString NAME_IDCODE_STRING;
      static const QString NAME_LABEL_1;
      static const QString NAME_LABEL_2;
      static const QString NAME_LUT_NAMES;
      static const QString NAME_ORIENT_SPECIFIC;
      static const QString NAME_ORIGIN;
      static const QString NAME_SCENE_DATA;
      static const QString NAME_TYPESTRING;
      static const QString NAME_VOLUME_FILENAMES;
};

/// class for storing the AFNI attributes in an AFNI header
class AfniHeader {
   public:
      /// Constructor
      AfniHeader();
      
      /// Destructor
      ~AfniHeader();
      
      /// add an attribute
      void addAttribute(AfniAttribute& attr);
      
      /// remove an attribute
      void removeAttribute(const QString& name);
      
      /// clear the afni header and reset to default values
      void clear();
      
      /// get the number of attributes
      int getNumberOfAttributes() const { return attributes.size(); }
      
      /// Get an attribute by its index
      AfniAttribute* getAttribute(const int index);
      
      /// Get an attribute for the specified name (returns NULL if attribute not found)
      AfniAttribute* getAttribute(const QString& name);
      
      /// Read a header file
      void readHeader(const QString& headerFileName,
                      QFile& file, QTextStream& stream) throw (FileException);
      
      /// Write to a header file
      void writeHeader(QTextStream& stream) throw (FileException);
      
      /// read the AFNI extension stored in a NIFTI volume file
      void readFromNiftiExtension(const QString& niftiExtension) throw (FileException);
      
      /// setup the AfniHeader from volume file(s)
      void setupFromVolumeFiles(const std::vector<VolumeFile*>& volumes) throw (FileException);
      
      /// write the AFNI attributes to a NIFTI volume extension
      void writeToNiftiExtension(QString& niftiExtension,
                                 const nifti_1_header* hdr) throw (FileException);
      
   private:
      /// storage for the attributes
      std::vector<AfniAttribute> attributes;
};

#endif // __VE_AFNI_ATTRIBUTES_H__

#ifdef __AFNI_HEADER_DEFINE__
   const QString AfniAttribute::NAME_BRICK_FLOAT_FACS   = "BRICK_FLOAT_FACS";
   const QString AfniAttribute::NAME_BRICK_LABS         = "BRICK_LABS";
   const QString AfniAttribute::NAME_BRICK_STATS        = "BRICK_STATS";
   const QString AfniAttribute::NAME_BRICK_TYPES        = "BRICK_TYPES";
   const QString AfniAttribute::NAME_BRICK_COMMENTS     = "BRICK_COMMENTS";
   const QString AfniAttribute::NAME_BYTEORDER_STRING   = "BYTEORDER_STRING";
   const QString AfniAttribute::NAME_CARET_METADATA_LINK = "CARET_METADATA_LINK";
   const QString AfniAttribute::NAME_CARET_TYPE         = "CARET_TYPE";
   const QString AfniAttribute::NAME_CARET_PUBMED_ID    = "CARET_PUBMED_ID";
   const QString AfniAttribute::NAME_DATASET_NAME       = "DATASET_NAME";
   const QString AfniAttribute::NAME_DATASET_DIMENSIONS = "DATASET_DIMENSIONS";
   const QString AfniAttribute::NAME_DATASET_RANK       = "DATASET_RANK";
   const QString AfniAttribute::NAME_DELTA              = "DELTA";
   const QString AfniAttribute::NAME_HISTORY_NOTE       = "HISTORY_NOTE";
   const QString AfniAttribute::NAME_IDCODE_DATE        = "IDCODE_DATE";
   const QString AfniAttribute::NAME_IDCODE_STRING      = "IDCODE_STRING";
   const QString AfniAttribute::NAME_LABEL_1            = "LABEL_1";
   const QString AfniAttribute::NAME_LABEL_2            = "LABEL_2";
   const QString AfniAttribute::NAME_LUT_NAMES          = "LUT_NAMES";
   const QString AfniAttribute::NAME_ORIENT_SPECIFIC    = "ORIENT_SPECIFIC";
   const QString AfniAttribute::NAME_ORIGIN             = "ORIGIN";
   const QString AfniAttribute::NAME_SCENE_DATA         = "SCENE_DATA";
   const QString AfniAttribute::NAME_TYPESTRING         = "TYPESTRING";
   const QString AfniAttribute::NAME_VOLUME_FILENAMES   = "VOLUME_FILENAMES";
#endif // __AFNI_HEADER_DEFINE__


