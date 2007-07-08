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
#include <set>

#include "DebugControl.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "WustlRegionFile.h"

/**
 * Constructor.
 */
WustlRegionFile::WustlRegionFile()
     : AbstractFile("Wustl Region File",
                    SpecFile::getWustlRegionFileExtension(),
                    false,
                    AbstractFile::FILE_FORMAT_ASCII,
                    FILE_IO_READ_AND_WRITE,
                    FILE_IO_NONE,
                    FILE_IO_NONE,
                    FILE_IO_NONE)
{
   clear();
}

/**
 * Destructor.
 */
WustlRegionFile::~WustlRegionFile()
{
   clear();
}

/**
 * add a time course.
 */
void 
WustlRegionFile::addTimeCourse(const TimeCourse tc) 
{ 
   timeCourses.push_back(tc);
   setModified(); 
}
      
/**
 * append a region file to this file.
 */
void 
WustlRegionFile::append(const WustlRegionFile wrf)
{
   appendToFileComment(wrf.getFileComment());
   timeCourses.insert(timeCourses.end(),
                      wrf.timeCourses.begin(), wrf.timeCourses.end());
}
      
/**
 * clear the file.
 */
void 
WustlRegionFile::clear()
{
   clearAbstractFile();
   
   timeCourses.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
WustlRegionFile::empty() const
{
   return (timeCourses.empty());
}

/**
 * Read the contents of the file (header has already been read).
 */
void
WustlRegionFile::readFileData(QFile& /*file*/, 
                                        QTextStream& stream,
                                        QDataStream&,
                                  QDomElement& /* rootElement */ ) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   bool readingRegionData = false;
   
   TimeCourse* timeCourseBeingRead = NULL;
   Region* regionBeingRead = NULL;
   
   QString line;
   readLine(stream, line);
   while (stream.atEnd() == false) {
      if (readingRegionData == false) {
         if (StringUtilities::startsWith(line, "TIMECOURSE")) {
            std::vector<QString> tokens;
            StringUtilities::token(line, " ", tokens);
            TimeCourse timeCourse;
            if (tokens.size() >= 3) {
               timeCourse.setName(tokens[2]);
               addTimeCourse(timeCourse);
               const int num = getNumberOfTimeCourses();
               timeCourseBeingRead = getTimeCourse(num - 1);
            }
         }
         else if (StringUtilities::startsWith(line, "REGION")) {
            std::vector<QString> tokens;
            StringUtilities::token(line, " ", tokens);
            Region region;
            if (tokens.size() >= 5) {
               region.setNumber(StringUtilities::toInt(tokens[2]));
               region.setName(tokens[3]);
               region.setNumberOfVoxels(StringUtilities::toInt(tokens[4]));
               if (timeCourseBeingRead == NULL) {
                  QString msg("Error reading file.  Have region named ");
                  msg.append(region.getName());
                  msg.append(" but no timecourse.");
                  throw FileException(filename, msg);
               }
               timeCourseBeingRead->addRegion(region);
               const int num = timeCourseBeingRead->getNumberOfRegions();
               regionBeingRead = timeCourseBeingRead->getRegionByIndex(num - 1);
               readingRegionData = true;
            }
            else {
               QString msg("REGION line has fewer than 5 items: ");
               msg.append(line);
               throw FileException(filename, msg);
            }
         }
      }
      else {
         std::vector<QString> tokens;
         StringUtilities::token(line, " ", tokens);
         
         if (tokens.size() >= 2) {
            RegionCase rc;
            rc.setName(tokens[0]);
            for (unsigned int i = 1; i < tokens.size(); i++) {
               rc.addTimePoint(StringUtilities::toFloat(tokens[i]));
            }
            if (regionBeingRead == NULL) {
               QString msg("Have RegionCase but no current region for line: ");
               msg.append(line);
               throw FileException(filename, msg);
            }
            regionBeingRead->addRegionCase(rc);
         }
         else {
            readingRegionData = false;
            regionBeingRead = NULL;
         }
      }
      
      readLine(stream, line);
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "WustlRegionFile" << std::endl;
      std::cout << "   Number of timecourses: " << getNumberOfTimeCourses() << std::endl;
      for (int i = 0; i < getNumberOfTimeCourses(); i++) {
         std::cout << "   Timecourse " << i << " has ";
         const TimeCourse* tc = getTimeCourse(i);
         std::cout << tc->getNumberOfRegions() << " regions." << std::endl;
      }
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
WustlRegionFile::writeFileData(QTextStream&,
                                  QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename,
                         "Writing of Wu NIL Region of Interest Files not supported.");
}

//
//---------------------------------------------------------------------------------------
//
/**
 * constructor.
 */
WustlRegionFile::TimeCourse::TimeCourse()
{
   name = "";
}

/**
 * constructor.
 */
WustlRegionFile::TimeCourse::~TimeCourse()
{
}
            
/**
 * get a region by its name (returns NULL if not found).
 */
WustlRegionFile::Region* 
WustlRegionFile::TimeCourse::getRegionByName(const QString name) 
{
   const int num = getNumberOfRegions();
   for (int i = 0; i < num; i++) {
      Region* reg = getRegionByIndex(i);
      if (reg->getName() == name) {
         return reg;
      }
   }
   
   return NULL;
}

/**
 * get all of the case names in a time course.
 */
void 
WustlRegionFile::TimeCourse::getAllRegionCaseNames(std::vector<QString>& names) const
{
   std::set<QString> uniqueNames;
   
   const int num = getNumberOfRegions();
   for (int i = 0; i < num; i++) {
      const Region* reg = getRegionByIndex(i);
      const int numCase = reg->getNumberOfRegionCases();
      for (int j = 0; j < numCase; j++) {
         const RegionCase* rc = reg->getRegionCase(j);
         uniqueNames.insert(rc->getName());
      }
   }
   
   names.clear();
   names.insert(names.end(), uniqueNames.begin(), uniqueNames.end());
}
            
//
//---------------------------------------------------------------------------------------
//

/**
 * Constructor.
 */
WustlRegionFile::Region::Region()
{
   regionNumber = -1;
   name = "";
   numberOfVoxels = 0;
}

/**
 * Constructor.
 */
WustlRegionFile::Region::~Region()
{
}

/**
 * get a region case by name.
 */
const WustlRegionFile::RegionCase* 
WustlRegionFile::Region::getRegionCaseByName(const QString& name) const
{
   const int numCases = getNumberOfRegionCases();
   for (int i = 0; i < numCases; i++) {
      const RegionCase* rc = getRegionCase(i);
      if (rc->getName() == name) {
         return rc;
      }
   }
   return NULL;
}

/**
 * get a region case by name (const method).
 */
WustlRegionFile::RegionCase* 
WustlRegionFile::Region::getRegionCaseByName(const QString& name)
{
   const int numCases = getNumberOfRegionCases();
   for (int i = 0; i < numCases; i++) {
      RegionCase* rc = getRegionCase(i);
      if (rc->getName() == name) {
         return rc;
      }
   }
   return NULL;
}

//
//---------------------------------------------------------------------------------------
//

/**
 * Constructor.
 */
WustlRegionFile::RegionCase::RegionCase()
{
   name = "";
   timePoints.clear();
}

/**
 * Constructor.
 */
WustlRegionFile::RegionCase::~RegionCase()
{
}

//
//---------------------------------------------------------------------------------------
//
