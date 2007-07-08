
#ifndef __GIFTI_META_DATA_H__
#define __GIFTI_META_DATA_H__

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

#include <map>
#include <QString>
#include <vector>

#include <stdint.h>

#include "FileException.h"

class StringTable;
class QTextStream;

/// class for GIFTI metadata
class GiftiMetaData {
   public:
      /// container for metadata
      typedef std::map<QString, QString> MetaDataContainer;
      
      /// iterator for metadata
      typedef MetaDataContainer::iterator MetaDataIterator;
      
      /// const iterator for metadata
      typedef MetaDataContainer::const_iterator ConstMetaDataIterator;
      
      // constructor
      GiftiMetaData();
      
      // copy constructor
      GiftiMetaData(const GiftiMetaData& nmd);
      
      // destructor
      ~GiftiMetaData();
      
      // assignment operator
      GiftiMetaData& operator=(const GiftiMetaData& nmd);
      
      // clear the metadata
      void clear();
      
      // get all of the meta data "names" of the name/value pairs
      void getAllNames(std::vector<QString>& allNames) const;
      
      /// get the metadata
      const MetaDataContainer* getMetaData() const { return &metaData; }
      
      // get metadata as string (returns true if found)
      bool get(const QString& name, QString& value) const;
      
      // set metadata as string
      void set(const QString& name, const QString& value);
      
      // get metadata as int (returns true if found)
      bool get(const QString& name, int32_t& value) const;
      
      // get metadata as vector of ints (returns true if found)
      bool get(const QString& name, std::vector<int32_t>& values) const;
      
      // set metadata as int
      void set(const QString& name, const int32_t value);
      
      // set metadata as vector of ints
      void set(const QString& name, const std::vector<int32_t>& values);
      
      // get metadata as float (returns true if found)
      bool get(const QString& name, float& value) const;
      
      // get metadata as vector of float (returns true if found)
      bool get(const QString& name, std::vector<float>& values) const;
      
      // set metadata as float
      void set(const QString& name, const float value);
      
      // set metadata as vector of float
      void set(const QString& name, const std::vector<float>& values);
      
      /// metadata name element
      static QString getMetaDataName() { return "Name"; }
      
      /// metadata description element
      static QString getMetaDataDescription() { return "Description"; }
      
      // write metadata
      void writeAsXML(QTextStream& stream,
                      const int indentOffset) const;
                            
      /// write the data into a StringTable
      void writeDataIntoStringTable(StringTable& table) const;
      
      /// read the data from a StringTable
      void readDataFromStringTable(const StringTable& table) throw (FileException);
      
   protected:
      // copy helper used by copy constructor and assignment operator
      void copyHelperGiftiMetaData(const GiftiMetaData& nmd);
      
      /// the meta data
      MetaDataContainer metaData;
      
};

#endif // __GIFTI_META_DATA_H__

