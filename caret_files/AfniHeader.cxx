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
#include <sstream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QGlobalStatic>
#include <QTextStream>

#define __AFNI_HEADER_DEFINE__
#include "AfniHeader.h"
#undef __AFNI_HEADER_DEFINE__
#include "DateAndTime.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#include "nifti1.h"
#include "GiftiLabelTable.h"

/**
 * Constructor.
 */
AfniHeader::AfniHeader()
{
   clear();
}

/**
 * Destructor.
 */
AfniHeader::~AfniHeader()
{
}

/**
 * Clear the afni header and set to default values
 */
void
AfniHeader::clear()
{
   //
   // Create the required default attributes assuming a single subvolume
   //
   attributes.clear();
   const int ranks[8] = { 3, 1, 0, 0, 0, 0, 0, 0 };
   AfniAttribute rank(AfniAttribute::NAME_DATASET_RANK, ranks, 8);
   addAttribute(rank);
   
   const int dims[3] = { 0, 0, 0 };
   AfniAttribute dim(AfniAttribute::NAME_DATASET_DIMENSIONS, dims, 3);
   addAttribute(dim);
   
   AfniAttribute typeString(AfniAttribute::NAME_TYPESTRING, "3DIM_HEAD_ANAT");
   addAttribute(typeString);
   
   const int sceneData[8] = { 0, 3, 0, -999, -999, -999, -999, -999 };
   AfniAttribute scene(AfniAttribute::NAME_SCENE_DATA, sceneData, 8);
   addAttribute(scene);
   
   const int orientData[3] = { 1, 2, 4 };  // LPI
   AfniAttribute orient(AfniAttribute::NAME_ORIENT_SPECIFIC, orientData, 3);
   addAttribute(orient);
   
   const float origin[3] = { 0.0, 0.0, 0.0 };
   AfniAttribute originData(AfniAttribute::NAME_ORIGIN, origin, 3);
   addAttribute(originData);
   
   const float delta[3] = { 1.0, 1.0, 1.0 };
   AfniAttribute deltaData(AfniAttribute::NAME_DELTA, delta, 3);
   addAttribute(deltaData);
   
   //
   // Create the almost required default attributes
   //
   AfniAttribute icodeString(AfniAttribute::NAME_IDCODE_STRING,
                       DateAndTime::getDateAndTimeForNaming());
                       //QDateTime::currentDateTime().toString("dd_MMMM_yyyy_hh_mm_ss_zzz"));
   addAttribute(icodeString);
   
   AfniAttribute icodeDate(AfniAttribute::NAME_IDCODE_DATE,
                        DateAndTime::getDateAndTimeAsString());
                       //    QDateTime::currentDateTime().toString("ddd MMM d hh:mm::ss yyyy"));
   addAttribute(icodeDate);
   
   const float stats[2] = { 0.0, 255.0 };
   AfniAttribute brickStats(AfniAttribute::NAME_BRICK_STATS, stats, 2);
   addAttribute(brickStats);
   
   AfniAttribute brickTypes(AfniAttribute::NAME_BRICK_TYPES, 3);  // 3 = float
   addAttribute(brickTypes);
   
   AfniAttribute facs(AfniAttribute::NAME_BRICK_FLOAT_FACS,  0.0);
   addAttribute(facs);
   
   //AfniAttribute brickLabs(AfniAttribute::NAME_BRICK_LABS, "#0");
   //addAttribute(brickLabs);
   
   //
   // Not necessary but do them anyway
   //
   
   AfniAttribute history(AfniAttribute::NAME_HISTORY_NOTE, "Created by CARET");
   addAttribute(history);
   
   //
   // Nearly useless attributes
   //   
   AfniAttribute label1(AfniAttribute::NAME_LABEL_1, "orig");
   addAttribute(label1);
   
   AfniAttribute label2(AfniAttribute::NAME_LABEL_2, "Bigfoot Lives");
   addAttribute(label2);
   
   AfniAttribute datasetName(AfniAttribute::NAME_DATASET_NAME, "caret volume");
   addAttribute(datasetName);
}

/**
 * add an attribute (replaces it if it exists)
 */
void 
AfniHeader::addAttribute(AfniAttribute& attr) 
{
   AfniAttribute* a = getAttribute(attr.attributeName);
   if (a != NULL) {
      *a = attr;
   }
   else {
      attributes.push_back(attr);
   }
}

/**
 * Remove an attribute
 */
void 
AfniHeader::removeAttribute(const QString& name) 
{
   for (std::vector<AfniAttribute>::iterator iter = attributes.begin();
        iter != attributes.end(); iter++) {
      if (iter->attributeName == name) {
         attributes.erase(iter);
         break;
      }
   }
}

/**
 * Get an attribute by its index
 */
AfniAttribute* 
AfniHeader::getAttribute(const int index)
{
   if (index < getNumberOfAttributes()) {
      return &attributes[index];
   }
   return NULL;
}

/**
 * get an attribute for the specified name (returns NULL if not found)
 */
AfniAttribute* 
AfniHeader::getAttribute(const QString& name)
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
 * Read an AFNI header.
 */
void
AfniHeader::readHeader(const QString& headerFileName,
                       QFile& /*file*/, QTextStream& stream) throw (FileException)
{
   attributes.clear();
   
   QString name;
   QString type;
   int count = 0;
   
   //
   // Read until end of file
   //
   while(stream.atEnd() == false) {
      const QString line(stream.readLine());
      
      std::vector<QString> tokens;
      StringUtilities::token(line, " \t", tokens);
      
      if (tokens.size() == 3) {
         if (tokens[0] == "type") {
            name = "";
            count = 0;
            type = tokens[2];
         }
         else if (tokens[0] == "name") {
            name = tokens[2];
         }
         else if (tokens[0] == "count") {
            count = tokens[2].toInt();
           
            if ((count > 0) &&
                (name.isEmpty() == false) &&
                (type.isEmpty() == false)) {
               if (DebugControl::getDebugOn()) {
                  std::cout << "Reading AFNI have " << name.toAscii().constData() << " with type " << type.toAscii().constData() 
                            << " and count " << count << std::endl;
               }
               
               if (type == "float-attribute") {
                  float* data = new float[count];
                 
                  int i = 0;
                  while(i < count) {
                     stream >> data[i];
                     i++;
                  }
                 
                  if (DebugControl::getDebugOn()) {
                     std::cout << "AFNI Float name: " << name.toAscii().constData() << " has values ";
                     for (int j = 0; j < count; j++) {
                        std::cout << data[j] << " ";
                     }
                     std::cout << std::endl;
                  }
                  
                  AfniAttribute attr(name, data, count);
                  addAttribute(attr);
                  
                  delete[] data;
               }
               else if (type == "integer-attribute") {
                  int* data = new int[count];
                 
                  int i = 0;
                  while(i < count) {
                     stream >> data[i];
                     i++;
                  }
                 
                  if (DebugControl::getDebugOn()) {
                     std::cout << "AFNI Int name: " << name.toAscii().constData() << " has values ";
                     for (int j = 0; j < count; j++) {
                        std::cout << data[j] << " ";
                     }
                     std::cout << std::endl;
                  }
                  
                  AfniAttribute attr(name, data, count);
                  addAttribute(attr);
                  
                  delete[] data;
               }
               else if (type == "string-attribute") {
                  char* dataChars = new char[count+1];
                  while(stream.atEnd() == false) {
                     QChar c;  // use QChar since ">> char" skips whitespace
                     stream >> c;
                     if (c == '\'') {
                        int i = 0;
                        while (i < (count -1)) {
                           stream >> c;
                           dataChars[i] = c.toLatin1();
                           i++;
                        }
                        dataChars[count - 1] = '\0';
                        break;
                     }
                  }
                  const QString data(dataChars);
                  delete[] dataChars;
                  
                  AfniAttribute attr(name, data);
                  addAttribute(attr);
                  
                  if (DebugControl::getDebugOn()) {
                     std::cout << "AFNI String name: " << name.toAscii().constData() << " has value " 
                               << data.toAscii().constData() << std::endl;
                  }
               }
               else {
                  std::ostringstream str;
                  str << "Unknown AFNI header type: " << type.toAscii().constData() << std::ends;
                  throw FileException(FileUtilities::basename(headerFileName),
                                        str.str().c_str());
               }
            }
         }
      }
   }
}

/**
 * Write an AFNI header.
 */
void
AfniHeader::writeHeader(QTextStream& stream) throw (FileException)
{
   //
   // Set the byte ordering
   //
   //int wordSize;
   //bool bigEndian;       
   //qSysInfo(&wordSize, &bigEndian);
   if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {          
      AfniAttribute byteOrder(AfniAttribute::NAME_BYTEORDER_STRING, "MSB_FIRST");
      addAttribute(byteOrder);
   }
   else {
      AfniAttribute byteOrder(AfniAttribute::NAME_BYTEORDER_STRING, "LSB_FIRST");
      addAttribute(byteOrder);
   }
   
   //
   // Set the date
   //
   AfniAttribute icodeDate(AfniAttribute::NAME_IDCODE_DATE,
                           DateAndTime::getDateAndTimeAsString());
                           //QDateTime::currentDateTime().toString("ddd MMM d hh:mm::ss yyyy"));
   addAttribute(icodeDate);
   
   //
   // Write all of the attributes
   //
   const int numAttr = getNumberOfAttributes();
   for (int i = 0; i < numAttr; i++) {
      stream << "\n";
      
      const AfniAttribute& a = attributes[i];
      
      //
      //  write attribute type
      //
      switch (a.attributeType) {
         case AfniAttribute::ATTRIBUTE_TYPE_FLOAT:
            stream << "type  = float-attribute\n";
            break;
         case AfniAttribute::ATTRIBUTE_TYPE_INTEGER:
            stream << "type  = integer-attribute\n";
            break;
         case AfniAttribute::ATTRIBUTE_TYPE_STRING:
            stream << "type  = string-attribute\n";
            break;
      }
      
      //
      // write attribute name
      //
      stream << "name  = " << a.attributeName << "\n";
      
      //
      // Write the attribute data
      //
      const char* offset = "      ";
      switch (a.attributeType) {
         case AfniAttribute::ATTRIBUTE_TYPE_FLOAT:
            {
               const int count = a.floatValue.size();
               stream << "count = " << count << "\n";
               stream << offset;
               for (int j = 0; j < count; j++) {
                  stream << a.floatValue[j] << " ";
                  if ((j > 0) && ((j % 5) == 0) && (j != (count - 1))) {
                     stream << "\n";
                     stream << offset;
                  }
               }
            }
            break;
         case AfniAttribute::ATTRIBUTE_TYPE_INTEGER:
            {
               const int count = a.intValue.size();
               stream << "count = " << count << "\n";
               stream << offset;
               for (int j = 0; j < count; j++) {
                  stream << a.intValue[j] << " ";
                  if ((j > 0) && ((j % 5) == 0) && (j != (count - 1))) {
                     stream << "\n";
                     stream << offset;
                  }
               }
            }
            break;
         case AfniAttribute::ATTRIBUTE_TYPE_STRING:
            stream << "count  = " << (a.stringValue.length() + 1) << "\n";
            stream << "'" << a.stringValue << "~";
            break;
      }
      stream << "\n";
   }
   
   stream << "\n";
}

/**
 * read the AFNI extension stored in a NIFTI volume file.
 */
void 
AfniHeader::readFromNiftiExtension(const QString& niftiExtension) throw (FileException)
{
   //
   // Place the AFNI extension into a DOM XML Parser
   //
   QDomDocument doc("nifti_afni_extension");
   QString errorMessage;
   int errorLine, errorColumn;
   if (doc.setContent(niftiExtension, &errorMessage, &errorLine, &errorColumn) == false) {
      QString msg("Error parsing afni extension at line ");
      msg += QString::number(errorLine);
      msg += ", column ";
      msg += QString::number(errorColumn);
      throw FileException(msg);
   }
   
   //
   // Get the root element
   //
   QDomElement rootElement = doc.documentElement();
   if (rootElement.isNull()) {
      throw FileException("AFNI extension root element is NULL");
   }
   if (DebugControl::getDebugOn()) {
      std::cout << "AFNI extension root element: " << rootElement.tagName().toAscii().constData() << std::endl;
   }
   
   //
   // Loop through the children of the root element
   //
   QDomNode node = rootElement.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (DebugControl::getDebugOn()) {
         std::cout << "AFNI extension element: " << elem.tagName().toAscii().constData() << std::endl;
      }
  
      const QString niType  = elem.attribute("ni_type");
      const QString niDim   = elem.attribute("ni_dimen");
      const QString atrName = elem.attribute("atr_name");
      QString text   = elem.text();
      if (DebugControl::getDebugOn()) {
         std::cout << "   type: " << niType.toAscii().constData() << std::endl;
         std::cout << "   dim:  " << niDim.toAscii().constData() << std::endl;
         std::cout << "   name: " << atrName.toAscii().constData() << std::endl;
         std::cout << "   text: " << text.toAscii().constData() << std::endl;
      }
  
      //
      // Remove whitespace and skip over empty items
      //
      text = text.trimmed();
      if (text.isEmpty() == false) {
         //
         // Create an attribute
         //
         if (niType == "String") {
            //
            // Remove any leading/trailing double quotes and "~"
            //
            int len = text.length();
            if (text.right(1) == "\"")  {
               text.remove(len - 1, 1);
            }
            if (text.right(1) == "~") {
               len = text.length();
               text.remove(len - 1, 1);
            }
            if (text.left(1) == "\"") {
               text.remove(0, 1);
            }
            if (text.isEmpty() == false) {
               AfniAttribute attr(atrName, text);
               addAttribute(attr);
            }
         }
         else if (niType == "int") {
            std::vector<int> intData;
            StringUtilities::token(text, " ", intData);
            if (intData.empty() == false) {
               AfniAttribute attr(atrName, intData);
               addAttribute(attr);
            }
         }
         else if (niType == "float") {
            std::vector<float> floatData;
            StringUtilities::token(text, " ", floatData);
            if (floatData.empty() == false) {
               AfniAttribute attr(atrName, floatData);
               addAttribute(attr);
            }
         }
         else {
            std::cout << "WARNING: Unrecognized AFNI extension \"ni_type\": "
                      << niType.toAscii().constData() << std::endl;
         }
      }
      
      //
      // Move to next node
      //
      node = node.nextSibling();
   }
   
   //
   // Get the ID code
   //
   const QString idCode = rootElement.attribute("self_idcode");
   if (idCode.isEmpty() == false) {
      AfniAttribute attr(AfniAttribute::NAME_IDCODE_STRING, idCode);
      addAttribute(attr);
   }
}

/**
 * setup the AfniHeader from volume file(s).
 */
void 
AfniHeader::setupFromVolumeFiles(const std::vector<VolumeFile*>& volumesToWrite,
                                 const ColorFile* colorFile) throw (FileException)
{
   if (volumesToWrite.empty()) {
      return;
   }
   VolumeFile* firstVolume = volumesToWrite[0];
   
   //
   // Add comments and label
   //
   QString labelString, commentString;
   for (unsigned int i = 0; i < volumesToWrite.size(); i++) {
      const VolumeFile* vf = volumesToWrite[i];
      labelString += vf->getDescriptiveLabel();
      labelString += "~";
      if (i <= 0) {
         commentString += vf->getFileComment();
      }
   }
   AfniAttribute labs(AfniAttribute::NAME_BRICK_LABS, labelString);
   addAttribute(labs);
   AfniAttribute comms(AfniAttribute::NAME_HISTORY_NOTE, commentString);
   addAttribute(comms);
   
   //
   // Update dimensions
   //
   int dimensions[3];
   firstVolume->getDimensions(dimensions);
   AfniAttribute dims(AfniAttribute::NAME_DATASET_DIMENSIONS, dimensions, 3);
   addAttribute(dims);
   
   //
   // update orientation
   //
   VolumeFile::ORIENTATION orientation[3];
   firstVolume->getOrientation(orientation);
   int orient[3] = { 0, 0, 0 };
   for (int i = 0; i < 3; i++) {
      switch(orientation[i]) {
         case  VolumeFile::ORIENTATION_UNKNOWN:
            orient[i] = 1;
            break;
         case  VolumeFile::ORIENTATION_RIGHT_TO_LEFT:
            orient[i] = 0;
            break;
         case  VolumeFile::ORIENTATION_LEFT_TO_RIGHT:
            orient[i] = 1;
            break;
         case  VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR:
            orient[i] = 2;
            break;
         case  VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR:
            orient[i] = 3;
            break;
         case  VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR:
            orient[i] = 4;
            break;
         case  VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR:
            orient[i] = 5;
            break;
      }
   }
   AfniAttribute ori(AfniAttribute::NAME_ORIENT_SPECIFIC, orient, 3);
   addAttribute(ori);
   
   float spacing[3];
   firstVolume->getSpacing(spacing);

   //
   // Update origin (always LPI) Note that AFNI has origin in center of voxel
   //
   float volumeOrigin[3];
   firstVolume->getOrigin(volumeOrigin);
/*
   float originTemp[3] = {  
                            std::fabs(origin[0] + spacing[0]*0.5),
                            std::fabs(origin[1] + spacing[1]*0.5),
                           -std::fabs(origin[2] + spacing[2]*0.5)
                         }; 
   
   AfniAttribute orig(AfniAttribute::NAME_ORIGIN, originTemp, 3);
*/
   //
   // Caret always writes LPI and in AFNI left is positive-X, posterior
   // is positive Y, and inferior is negative Z
   //
   volumeOrigin[0] = std::fabs(volumeOrigin[0]);
   volumeOrigin[1] = std::fabs(volumeOrigin[1]);
   volumeOrigin[2] = -std::fabs(volumeOrigin[2]);
   AfniAttribute orig(AfniAttribute::NAME_ORIGIN, volumeOrigin, 3);
   addAttribute(orig);
   
   //
   // Update spacing (always LPI)
   //
   const float spacingTemp[3] = { 
                                  -std::fabs(spacing[0]),
                                  -std::fabs(spacing[1]),
                                   std::fabs(spacing[2])
                                };
   AfniAttribute space(AfniAttribute::NAME_DELTA, spacingTemp, 3);
   addAttribute(space);
   
   //
   // Caret volume type
   //
   QString fileTypeString("Unknown");
   switch (firstVolume->getVolumeType()) {
      case  VolumeFile::VOLUME_TYPE_ANATOMY:
         fileTypeString = "Anatomy";
         break;
      case  VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         fileTypeString = "Functional";
         break;
      case  VolumeFile::VOLUME_TYPE_PAINT:
         fileTypeString = "Paint";
         break;
      case  VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         fileTypeString = "Prob Atlas";
         break; 
      case  VolumeFile::VOLUME_TYPE_RGB:
         fileTypeString = "RGB";
         break;
      case  VolumeFile::VOLUME_TYPE_ROI:
         fileTypeString = "ROI";
         break;
      case  VolumeFile::VOLUME_TYPE_SEGMENTATION:
         fileTypeString = "Segmentation";
         break;
      case  VolumeFile::VOLUME_TYPE_VECTOR:
         fileTypeString = "Vector";
         break;
      case  VolumeFile::VOLUME_TYPE_UNKNOWN:
         fileTypeString = "Unknown";
         break;
   }
   AfniAttribute fta(AfniAttribute::NAME_CARET_TYPE, fileTypeString);
   addAttribute(fta);
   
   //
   // Set the voxel data type
   //
   std::vector<int> dataTypeVector;
   switch(firstVolume->getVoxelDataType()) {
      case VolumeFile::VOXEL_DATA_TYPE_UNKNOWN:
         throw FileException(firstVolume->getFileName(), "Unknown data type");
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_CHAR:
         dataTypeVector.push_back(-1);  // invalid type for AFNI
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_CHAR_UNSIGNED:
         dataTypeVector.push_back(0);
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_SHORT:
         dataTypeVector.push_back(1);
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_SHORT_UNSIGNED:
         dataTypeVector.push_back(-1);  // invalid type for AFNI
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_INT:
         dataTypeVector.push_back(2);
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_INT_UNSIGNED:
         dataTypeVector.push_back(-1);  // invalid type for AFNI
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_LONG:
         dataTypeVector.push_back(-1);  // invalid type for AFNI
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_LONG_UNSIGNED:
         dataTypeVector.push_back(-1);  // invalid type for AFNI
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_FLOAT:
         dataTypeVector.push_back(3);
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_DOUBLE:
         dataTypeVector.push_back(4);
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED:
         dataTypeVector.push_back(6);
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_RGB_SLICE_INTERLEAVED:
         dataTypeVector.push_back(6);
         break;
      case  VolumeFile::VOXEL_DATA_TYPE_VECTOR:
         dataTypeVector.push_back(3);
         break;
   }
   
   //
   // Voxel min max values and date types for each sub volume
   //
   int numSubVolumes = static_cast<int>(volumesToWrite.size());
   std::vector<float> minMaxValues;
   for (int i = 0; i < numSubVolumes; i++) {
      float mn, mx;
      volumesToWrite[i]->getMinMaxVoxelValues(mn, mx);
      minMaxValues.push_back(mn);
      minMaxValues.push_back(mx);
      
      if (i >= 1) {
         dataTypeVector.push_back(dataTypeVector[0]);
      }
   }
   AfniAttribute brickType(AfniAttribute::NAME_BRICK_TYPES, dataTypeVector);
   addAttribute(brickType);
   AfniAttribute stats(AfniAttribute::NAME_BRICK_STATS, minMaxValues);
   addAttribute(stats);
   
   //
   // Dataset rank
   //
   const int rankData[2] = { 3, numSubVolumes };
   AfniAttribute rank(AfniAttribute::NAME_DATASET_RANK, rankData, 2);
   addAttribute(rank);
   
   //
   // Is this a functional volume
   //
   if (firstVolume->getVolumeType() == VolumeFile::VOLUME_TYPE_FUNCTIONAL) {
      //
      // Change header to denote functional volume
      //
      AfniAttribute typeString(AfniAttribute::NAME_TYPESTRING, "3DIM_HEAD_FUNC");
      addAttribute(typeString);
      const int sceneData[8] = { 0, 1, 1, -999, -999, -999, -999, -999 };
      AfniAttribute scene(AfniAttribute::NAME_SCENE_DATA, sceneData, 8);
      addAttribute(scene);
   }
   else {
      //
      // Change header to denote anatomical volume
      //
      AfniAttribute typeString(AfniAttribute::NAME_TYPESTRING, "3DIM_HEAD_ANAT");
      addAttribute(typeString);
      const int sceneData[8] = { 0, 3, 0, -999, -999, -999, -999, -999 };
      AfniAttribute scene(AfniAttribute::NAME_SCENE_DATA, sceneData, 8);
      addAttribute(scene);
   }
   
   //
   // Is this a paint or prob atlas volume
   //
   if ((firstVolume->getVolumeType() == VolumeFile::VOLUME_TYPE_PAINT) ||
       (firstVolume->getVolumeType() == VolumeFile::VOLUME_TYPE_PROB_ATLAS)) {
      std::vector<QString> regNames;
      for (int i = 0; i < firstVolume->getNumberOfRegionNames(); i++) {
         //
         // Cannot have '~' character in the name
         //
         QString name(firstVolume->getRegionNameFromIndex(i));
         name = name.replace(QChar('~'), QChar('_'));
         regNames.push_back(name);
      }
      const QString names = StringUtilities::combine(regNames, "~");
      AfniAttribute typeString(AfniAttribute::NAME_LUT_NAMES, names);
      addAttribute(typeString);

      //
      // Label table used by Caret6
      //
      GiftiLabelTable labelTable;
      int numNames = static_cast<int>(regNames.size());
      for (int i = 0; i < numNames; i++) {
         QString name = regNames[i];
         labelTable.addLabel(name);
      }
      if (colorFile != NULL) {
         labelTable.assignColors(*colorFile);
      }
      QString labelString;
      QTextStream labelStream(&labelString, QIODevice::WriteOnly);
      labelTable.writeAsXML(labelStream, 0);
      labelString = labelString.remove(QRegExp("\\n")); // remove newlines

      AfniAttribute labelAttr(AfniAttribute::NAME_LABEL_TABLE, labelString);
      addAttribute(labelAttr);
   }
   
   //
   // Add study meta data link
   //
   QString metaDataString;
   for (unsigned int i = 0; i < volumesToWrite.size(); i++) {
      const VolumeFile* vf = volumesToWrite[i];
      if (i > 0) {
         metaDataString += "~";
      }
      metaDataString += vf->getStudyMetaDataLinkSet().getLinkSetAsCodedText();
   }
   AfniAttribute md(AfniAttribute::NAME_CARET_METADATA_LINK, metaDataString);
   addAttribute(md);
   
   //
   // Add PubMed ID
   //
   AfniAttribute pmid(AfniAttribute::NAME_CARET_PUBMED_ID, volumesToWrite[0]->getFilePubMedID());
   addAttribute(pmid);
}

/**
 * write the AFNI attributes to a NIFTI volume extension.
 */
void 
AfniHeader::writeToNiftiExtension(QString& niftiExtension,
                                  const nifti_1_header* nifti1) throw (FileException)
{
   //
   // Set the date
   //
   AfniAttribute icodeDate(AfniAttribute::NAME_IDCODE_DATE,
                           DateAndTime::getDateAndTimeAsString());
                           //QDateTime::currentDateTime().toString("ddd MMM d hh:mm::ss yyyy"));
   addAttribute(icodeDate);
   
   //
   // Clear the extension
   //
   niftiExtension = "";
   
   //
   // Create the root element
   //
   QDomDocument xmlDoc("nifti_afni_extension");
   QDomElement rootElement = xmlDoc.createElement("AFNI_attributes");
   const AfniAttribute* attr = getAttribute("NAME_IDCODE_STRING");
   if (attr != NULL) {
      rootElement.setAttribute("self_idcode", attr->getValue());
   }
   else {
      rootElement.setAttribute("self_idcode", DateAndTime::getDateAndTimeForNaming()); //QDateTime::currentDateTime().toString("dd_MMMM_yyyy_hh_mm_ss_zzz"));
   }
   rootElement.setAttribute("ni_form", "ni_group");
   QString niftiNums;
   QTextStream(&niftiNums, QIODevice::WriteOnly) << nifti1->dim[1]  // x-dim
                            << ","
                            << nifti1->dim[2]  // y-dim
                            << ","
                            << nifti1->dim[3]  // z-dim
                            << ","
                            << nifti1->dim[4]  // num-time-series
                            << ","
                            << nifti1->dim[5]  // num-components-per-voxel
                            << ","
                            << nifti1->datatype;
   rootElement.setAttribute("NIfTI_nums", niftiNums);
   xmlDoc.appendChild(rootElement);
   
   //
   // Add XML information
   //
   QDomNode xmlNode = xmlDoc.createProcessingInstruction("xml", "version=\"1.0\"");
   xmlDoc.insertBefore(xmlNode, xmlDoc.firstChild());
   
   //
   // Loop through the attributes
   //
   const int num = getNumberOfAttributes();
   for (int i = 0; i < num; i++) {
      const AfniAttribute* attr = getAttribute(i);
      const QString name = attr->getName();
      const AfniAttribute::ATTRIBUTE_TYPE type = attr->getType();
      
      //
      // Should this attribute be excluded (conflicts with NIFTI data)
      // http://nifti.nimh.nih.gov/nifti-1/AFNIextension1
      //
      if ((name == AfniAttribute::NAME_BRICK_FLOAT_FACS) ||
          (name == AfniAttribute::NAME_BRICK_TYPES) ||
          (name == AfniAttribute::NAME_BYTEORDER_STRING) ||
          (name == AfniAttribute::NAME_CARET_TYPE) ||
          (name == AfniAttribute::NAME_DATASET_NAME) ||
          (name == AfniAttribute::NAME_DATASET_DIMENSIONS) ||
          (name == AfniAttribute::NAME_DATASET_RANK) ||
          (name == AfniAttribute::NAME_DELTA) ||
          (name == AfniAttribute::NAME_IDCODE_STRING) ||
          (name == AfniAttribute::NAME_LABEL_1) ||
          (name == AfniAttribute::NAME_LABEL_2) ||
          (name == AfniAttribute::NAME_ORIENT_SPECIFIC) ||
          (name == AfniAttribute::NAME_ORIGIN) ||
          (name == AfniAttribute::NAME_SCENE_DATA) ||
          (name == "STAT_AUX") ||
          (name == "TAXIS_NUMS") ||
          (name == "TAXIS_FLOATS") ||
          (name == "TAXIS_OFFSETS") ||
          (name == AfniAttribute::NAME_TYPESTRING)) {
         continue;
      }
      
      //
      // Get the attribute and element
      //
      QString niType;
      QString niDim;
      QString text;
      switch (type) {
         case AfniAttribute::ATTRIBUTE_TYPE_FLOAT:
            {
               std::vector<float> values;
               attr->getValue(values);
               const int num = static_cast<int>(values.size());
               if (num > 0) {
                  niType = "float";
                  niDim  = QString::number(num);
                  text = StringUtilities::combine(values, " ");
               }
            }
            break;
         case AfniAttribute::ATTRIBUTE_TYPE_INTEGER:
            {
               std::vector<int> values;
               attr->getValue(values);
               const int num = static_cast<int>(values.size());
               if (num > 0) {
                  niType = "int";
                  niDim  = QString::number(num);
                  text = StringUtilities::combine(values, " ");
               }
            }
            break;
         case AfniAttribute::ATTRIBUTE_TYPE_STRING:
            {
               const QString value = attr->getValue();
               if (value.isEmpty() == false) {
                  niType = "String";
                  niDim  = "1";
                  text   = "\"";
                  text   += value;
                  text   += "\"";
               }
            }
            break;
      }
      
      //
      // Convert to DomNode
      //
      if (text.isEmpty() == false) {
         //
         // create the element
         //
         QDomElement elem = xmlDoc.createElement("AFNI_atr");
         elem.setAttribute("ni_type", niType);
         elem.setAttribute("ni_dimen", niDim);
         elem.setAttribute("atr_name", name);
         elem.appendChild(xmlDoc.createTextNode(text));
         
         //
         // Add the element
         //
         rootElement.appendChild(elem);
      }
   }
   
   //
   // Were elements created ?
   //
   if (rootElement.hasChildNodes()) {
      //
      // convert the XML to a string
      //
      niftiExtension = xmlDoc.toString(3).trimmed();
   }
}

//================================================================================
//
// AfniAttribute methods
//

/**
 * constructor for single double but stored as float.  The compiler treats a
 * floating point number (e.g. 4.5) as a double so this contructor is required.
 */ 
AfniAttribute::AfniAttribute(const QString& name, const double value)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_FLOAT;
   floatValue.push_back(static_cast<float>(value));
}

/**
 * constructor for single float
 */ 
AfniAttribute::AfniAttribute(const QString& name, const float value)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_FLOAT;
   floatValue.push_back(value);
}

/**
 * constructor for single int 
 */ 
AfniAttribute::AfniAttribute(const QString& name, const int value)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_INTEGER;
   intValue.push_back(value);
}

/**
 * constructor for float array
 */ 
AfniAttribute::AfniAttribute(const QString& name, const float values[], const int numValues)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_FLOAT;
   for (int i = 0; i < numValues; i++) {
      floatValue.push_back(values[i]);
   }
}

/**
 * constructor for int array 
 */ 
AfniAttribute::AfniAttribute(const QString& name, const int values[], const int numValues)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_INTEGER;
   for (int i = 0; i < numValues; i++) {
      intValue.push_back(values[i]);
   }
}

/**
 * constructor for vector of floats
 */ 
AfniAttribute::AfniAttribute(const QString& name, const std::vector<float>& values)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_FLOAT;
   floatValue = values;
}

/**
 * constructor for vector of ints
 */ 
AfniAttribute::AfniAttribute(const QString& name, const std::vector<int>& values)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_INTEGER;
   intValue = values;
}

/**
 * constructor for string
 */ 
AfniAttribute::AfniAttribute(const QString& name, const QString& value)
{
   attributeName = name;
   attributeType = ATTRIBUTE_TYPE_STRING;
   stringValue = value;
}
      
/**
 * get the value for a string.
 */
QString 
AfniAttribute::getValue() const 
{ 
   return StringUtilities::trimWhitespace(stringValue); 
}
      
