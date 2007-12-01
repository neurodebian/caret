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

#include <QTextStream>

#include "AbstractFile.h"
#include "GiftiCommon.h"
#include "GiftiMetaData.h"
#include "StringTable.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
GiftiMetaData::GiftiMetaData()
{
   clear();
}

/**
 * copy constructor.
 */
GiftiMetaData::GiftiMetaData(const GiftiMetaData& nmd)
{
   copyHelperGiftiMetaData(nmd);
}

/**
 * destructor.
 */
GiftiMetaData::~GiftiMetaData()
{
   clear();
}

/** 
 * assignment operator.
 */
GiftiMetaData& 
GiftiMetaData::operator=(const GiftiMetaData& nmd)
{
   if (this != &nmd) {
      copyHelperGiftiMetaData(nmd);
   }
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
GiftiMetaData::copyHelperGiftiMetaData(const GiftiMetaData& nmd)
{
   metaData = nmd.metaData;
}

/**
 * clear the metadata.
 */
void 
GiftiMetaData::clear()
{
   metaData.clear();
}

/**
 * get metadata as string (returns true if found).
 */
bool 
GiftiMetaData::get(const QString& name,
                            QString& value) const
{
   const QString nameLower(name.toLower());
   
   for (ConstMetaDataIterator iter = metaData.begin();
        iter != metaData.end();
        iter++) {
      if (nameLower == iter->first.toLower()) {
         value = iter->second;
         return true;
      }
   }
   
/*
   ConstMetaDataIterator iter = metaData.find(name);
   if (iter != metaData.end()) { 
      value = iter->second;
      return true;
   }
*/
   return false;
}

/**
 * set metadata as string.
 */
void 
GiftiMetaData::set(const QString& name, const QString& value)
{
   const QString nameLower(name.toLower());
   
   //
   // Since case may vary, remove matching item
   //
   for (MetaDataIterator iter = metaData.begin();
        iter != metaData.end();
        iter++) {
      const QString tagName(iter->first);
      const QString tagNameLower(tagName.toLower());
      if (nameLower == tagNameLower) {
         metaData.erase(iter);
         break;
      }
   }
   
   metaData[name] = value;
}

/**
 * get metadata as int (returns true if found).
 */
bool 
GiftiMetaData::get(const QString& name,
                               int32_t& value) const
{
   QString s;
   if (get(name, s)) {
      StringUtilities::toNumber(s, value);
      return true;
   }
   return false;
}

/**
 * get metadata as vector of ints (returns true if found).
 */
bool 
GiftiMetaData::get(const QString& name,
                               std::vector<int32_t>& values) const
{
   QString md;
   if (get(name, md)) {
      StringUtilities::token(md, " \t", values);
      return true;
   }
   return false;
}

/**
 * set metadata as int.
 */
void 
GiftiMetaData::set(const QString& name, const int32_t value)
{
   metaData[name] = StringUtilities::fromNumber(value);
}

/**
 * set metadata as vector of ints.
 */
void 
GiftiMetaData::set(const QString& name, const std::vector<int32_t>& values)
{
   metaData[name] = StringUtilities::combine(values, " ");
}

/**
 * remove a metadata element.
 */
void 
GiftiMetaData::remove(const QString& name)
{
   metaData.erase(name);
}
      
/**
 * get metadata as float (returns true if found).
 */
bool 
GiftiMetaData::get(const QString& name,
                            float& value) const
{
   QString s;
   if (get(name, s)) {
      StringUtilities::toNumber(s, value);
      return true;
   }
   return false;
}

/**
 * get metadata as vector of float (returns true if found).
 */
bool 
GiftiMetaData::get(const QString& name,
                                 std::vector<float>& values) const
{
   QString md;
   if (get(name, md)) {
      StringUtilities::token(md, " \t", values);
      return true;
   }
   return false;
}

/**
 * set metadata as float.
 */
void 
GiftiMetaData::set(const QString& name, const float value)
{
   metaData[name] = StringUtilities::fromNumber(value);
}

/**
 * set metadata as vector of float.
 */
void 
GiftiMetaData::set(const QString& name, const std::vector<float>& values)
{
   metaData[name] = StringUtilities::combine(values, " ");
}

/**
 * write metadata (used by other classes so static).
 */
void 
GiftiMetaData::writeAsXML(QTextStream& stream,
                                const int indentOffset) const
{
   int indent = indentOffset;
   
   if (metaData.empty()) {
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "<" << GiftiCommon::tagMetaData << "/>" << "\n";
   }
   else {
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "<" << GiftiCommon::tagMetaData << ">" << "\n";
      indent++;
      
      for (ConstMetaDataIterator iter = metaData.begin(); iter != metaData.end(); iter++) {
         GiftiCommon::writeIndentationXML(stream, indent);
         stream << "<" << GiftiCommon::tagMD << ">" << "\n";
         indent++;
         GiftiCommon::writeIndentationXML(stream, indent);
         stream << "<" << GiftiCommon::tagName << "><![CDATA[" 
                << iter->first 
                << "]]></" << GiftiCommon::tagName << ">" << "\n";
         GiftiCommon::writeIndentationXML(stream, indent);
         stream << "<" << GiftiCommon::tagValue << "><![CDATA[" 
                << iter->second 
                << "]]></" << GiftiCommon::tagValue << ">" << "\n";
         indent--;
         GiftiCommon::writeIndentationXML(stream, indent);
         stream << "</" << GiftiCommon::tagMD << ">" << "\n";
      }
      
      indent--;
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "</" << GiftiCommon::tagMetaData << ">" << "\n";
   }
}

/**
 * get all of the meta data "names" of the name/value pairs.
 */
void
GiftiMetaData::getAllNames(std::vector<QString>& allNames) const
{
   allNames.clear();
   
   for (ConstMetaDataIterator iter = metaData.begin(); iter != metaData.end(); iter++) {
      allNames.push_back(iter->first);
   }
}

/**
 * write the data into a StringTable.
 */
void 
GiftiMetaData::writeDataIntoStringTable(StringTable& table) const
{
   const int numRows = metaData.size();
   
   table.setNumberOfRowsAndColumns(numRows, 2, GiftiCommon::tagMetaData);
   
   table.setTableTitle(GiftiCommon::tagMetaData);
   table.setColumnTitle(0, GiftiCommon::tagName);
   table.setColumnTitle(1, GiftiCommon::tagValue);
   
   int rowCount = 0;
   
   for (ConstMetaDataIterator iter = metaData.begin(); iter != metaData.end(); iter++) {
      table.setElement(rowCount, 0, iter->first);
      table.setElement(rowCount, 1, iter->second);
      rowCount++;
   }
}

/**
 * read the data from a StringTable.
 */
void 
GiftiMetaData::readDataFromStringTable(const StringTable& st) throw (FileException)
{
   clear();
   
   int numCols = st.getNumberOfColumns();
   
   int nameCol  = -1;
   int valueCol = -1;
   
   for (int i = 0; i < numCols; i++) {
      const QString columnTitle = st.getColumnTitle(i);
      if (columnTitle == GiftiCommon::tagName) {
         nameCol = i;
      }
      else if (columnTitle == GiftiCommon::tagValue) {
         valueCol = i;
      }
   }
   
   for (int i = 0; i < st.getNumberOfRows(); i++) {
      QString name, value;
      if (nameCol >= 0) {
         name = st.getElement(i, nameCol);
      }
      if (valueCol >= 0) {
         value = st.getElement(i, valueCol);
      }
      set(name, value);
   }
}
      
/**
 * copy the metadata from a caret file.
 */
void 
GiftiMetaData::copyMetaDataFromCaretFile(const AbstractFile* af)
{
   if (af != NULL) {
      AbstractFile::AbstractFileHeaderContainer header = af->getHeader();
      std::map<QString, QString>::iterator iter;
      for (iter = header.begin(); iter != header.end(); iter++) {
         const QString tag(iter->first);
         const QString value(iter->second);
         set(tag, value);
      }
   }
}

/**
 * copy the metadata into a caret file.
 */
void 
GiftiMetaData::copyMetaDataToCaretFile(AbstractFile* af) const
{
   if (af != NULL) {
      for (ConstMetaDataIterator iter = metaData.begin(); iter != metaData.end(); iter++) {
         const QString tag(iter->first);
         const QString value(iter->second);
         af->setHeaderTag(tag, value);
      }
   }
}
      

