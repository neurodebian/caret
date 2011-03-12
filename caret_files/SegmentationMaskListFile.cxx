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

#include <algorithm>
#include <iostream>

#include "CommaSeparatedValueFile.h"
#include "FileUtilities.h"
#include "SegmentationMaskListFile.h"
#include "SpecFile.h"
#include "StringTable.h"

/**
 * constructor.
 */
SegmentationMaskListFile::SegmentationMaskListFile()
   : AbstractFile("Segmentation Mask List File",
                  SpecFile::getCommaSeparatedValueFileExtension(),
                  true,  // has header
                  FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE, // default format
                  FILE_IO_NONE,   // supports ascii
                  FILE_IO_NONE,   // supports binary
                  FILE_IO_NONE,   // supports XML
                  FILE_IO_NONE,   // supports XML Base64
                  FILE_IO_NONE,   // supports XML Base64 GZIP
                  FILE_IO_NONE,   // supports other
                  FILE_IO_READ_ONLY)   // supports CSV
{
}

/**
 * destructor.
 */
SegmentationMaskListFile::~SegmentationMaskListFile()
{
}

/**
 * clear the file.
 */
void 
SegmentationMaskListFile::clear()
{
   clearAbstractFile();
   masks.clear();
}

/**
 * returns true if file contains no data.
 */
bool 
SegmentationMaskListFile::empty() const
{
   return (getNumberOfSegmentationMasks() == 0);
}

/**
 * get name of segmentation mask volume file.
 */
QString 
SegmentationMaskListFile::getSegmentationMaskFileName(const QString& stereotaxicSpaceNameIn,
                                                      const QString& structureNameIn) const
{
   QString stereotaxicSpaceName(stereotaxicSpaceNameIn);
   if (stereotaxicSpaceName.startsWith("711-2")) {
      stereotaxicSpaceName = "711-2C";
   }
   stereotaxicSpaceName = stereotaxicSpaceName.toLower();
   const QString structureName(structureNameIn.toLower());
   
   const int numMasks = getNumberOfSegmentationMasks();
   for (int i = 0; i < numMasks; i++) {
      const SegmentationMask mask = getSegmentationMask(i);
      if ((stereotaxicSpaceName == mask.stereotaxicSpaceName.toLower()) &&
          (structureName == mask.structureName.toLower())) {
         QString name(FileUtilities::dirname(getFileName()));
         if (name.isEmpty() == false) {
            name += "/";
         }
         name += mask.maskVolumeFileName;
         
         return name;
      }
   }
   
   return "";
}
                                         
/**
 * get the available masks.
 */
QString 
SegmentationMaskListFile::getAvailableMasks(const QString& indentation) const
{
   QString msg;
   
   const int numSegentationMasks = getNumberOfSegmentationMasks();
   if (numSegentationMasks <= 0) {
      msg += (indentation
              +"No segmentation masks were found.  They should be in the file \n"
              + indentation 
              + "   "
              + getFileName());
      return msg;
   }
   
   int longestSpaceName = 0;
   for (int i = 0; i < numSegentationMasks; i++) {
      longestSpaceName = std::max(getSegmentationMask(i).stereotaxicSpaceName.length(), longestSpaceName);
   }
   longestSpaceName += 2;
   
   for (int i = 0; i < numSegentationMasks; i++) {
      const SegmentationMask mask = getSegmentationMask(i);
      msg += (indentation 
              + mask.stereotaxicSpaceName.leftJustified(longestSpaceName)
              + "  " 
              + mask.structureName
              + "\n");
   }
   
   return msg;
}
      
/**
 * find out if comma separated file conversion supported.
 */
void 
SegmentationMaskListFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                                       bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = false;
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
SegmentationMaskListFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   masks.clear();
  
   const QString maskTableName("MaskVolumes");
   const StringTable* maskVolumesTable = csv.getDataSectionByName(maskTableName);
   if (maskVolumesTable == NULL) {
      throw FileException("Unable to find table named "
                          + maskTableName
                          + " in "
                          + getFileName());
   }
  
   //
   // Find columns numbers of data
   //
   const int spaceCol = maskVolumesTable->getColumnIndexFromName("Space");
   const int structureCol = maskVolumesTable->getColumnIndexFromName("Structure");
   const int volumeCol = maskVolumesTable->getColumnIndexFromName("MaskVolume");
   if ((spaceCol < 0) ||
       (structureCol < 0) ||
       (volumeCol < 0)) {
      throw FileException("Missing required columns in " +
                          getFileName());
   }
   
   //
   // read the mask volume names
   //
   const int numRows = maskVolumesTable->getNumberOfRows();
   for (int i = 0; i < numRows; i++) {
      SegmentationMask mask;
      mask.stereotaxicSpaceName = maskVolumesTable->getElement(i, spaceCol);
      mask.structureName        = maskVolumesTable->getElement(i, structureCol);
      mask.maskVolumeFileName   = maskVolumesTable->getElement(i, volumeCol);
      masks.push_back(mask);
   }
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
SegmentationMaskListFile::readFileData(QFile& file,
                                       QTextStream& stream,
                                       QDataStream& /*binStream*/,
                                       QDomElement& /*rootElement*/) throw (FileException)
{
   //
   // Should reading data be skipped ?
   // 
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Reading in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Reading XML not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Reading XML-External Binary Encoding file format not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            csvf.readFromTextStream(file, stream);
            readDataFromCommaSeparatedValuesTable(csvf);
         }     
         break;
   }      
   
   std::sort(masks.begin(), masks.end());
}

/**
 * Write the file's data (header has already been written).
 */
void 
SegmentationMaskListFile::writeFileData(QTextStream& /*stream*/,
                                        QDataStream& /*binStream*/,
                                        QDomDocument& /*xmlDoc*/,
                                        QDomElement& /*rootElement*/) throw (FileException)
{
   throw FileException("Writing of SegmentationMaskListFile not supported.");
}

//=================================================================================================

/**
 * less than operator.
 */
bool 
SegmentationMaskListFile::SegmentationMask::operator<(const SegmentationMask& sm) const
{
   if (stereotaxicSpaceName == sm.stereotaxicSpaceName) {
      if (structureName == sm.structureName) {
         return (maskVolumeFileName < sm.maskVolumeFileName);
      }
      
      return (structureName < sm.structureName);
   }
   
   return (stereotaxicSpaceName < sm.stereotaxicSpaceName);
}
