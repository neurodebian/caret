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
#include <cmath>

#define __BORDER_PROJECTION_MAIN__
#include "BorderProjectionFile.h"
#undef __BORDER_PROJECTION_MAIN__
#include "ColorFile.h"
#include "MathUtilities.h"
#include "SpecFile.h"

/**
 * Constructor.
 */
BorderProjectionLink::BorderProjectionLink(const int sectionIn,
                                           const int verticesIn[3],
                                           const float areasIn[3],
                                           const float radiusIn)
{
   borderProjectionFile = NULL;
   setData(sectionIn, verticesIn, areasIn, radiusIn);
}

/**
 * Destructor.
 */
BorderProjectionLink::~BorderProjectionLink()
{
}
      
/// determine if two border projection links are the same
bool 
BorderProjectionLink::operator==(const BorderProjectionLink& bpl) const
{
   for (int i = 0; i < 3; i++) {
      if (vertices[i] != bpl.vertices[i]) {
         return false;
      }
      if (areas[i] != bpl.areas[i]) {
         return false;
      }
   }
   return true;
}

/**
 * Set the Link's data.
 */
void 
BorderProjectionLink::setData(const int sectionIn,
                              const int verticesIn[3],
                              const float areasIn[3],
                              const float radiusIn)
{
   section     = sectionIn;
   vertices[0] = verticesIn[0];
   vertices[1] = verticesIn[1];
   vertices[2] = verticesIn[2];
   areas[0]    = areasIn[0];
   areas[1]    = areasIn[1];
   areas[2]    = areasIn[2];
   radius      = radiusIn;
   
   if (borderProjectionFile != NULL) {
      borderProjectionFile->setModified();
   }
}
                       
/**
 * Get the link's data.
 */
void 
BorderProjectionLink::getData(int& sectionOut,
                              int verticesOut[3],
                              float areasOut[3],
                              float& radiusOut) const
{
   sectionOut     = section;
   verticesOut[0] = vertices[0];
   verticesOut[1] = vertices[1];
   verticesOut[2] = vertices[2];
   areasOut[0]    = areas[0];
   areasOut[1]    = areas[1];
   areasOut[2]    = areas[2];
   radiusOut      = radius;
}

/**
 * Remove the border projection link at the specified index
 */
void
BorderProjection::removeBorderProjectionLink(const int linkNumber)
{
   if (linkNumber < getNumberOfLinks()) {
      links.erase(links.begin() + linkNumber);
      if (borderProjectionFile != NULL) {
         borderProjectionFile->setModified();
      }
   }
}

/** 
 * Constructor.
 */
BorderProjection::BorderProjection(const QString& nameIn, const float centerIn[3], 
               const float samplingDensityIn, const float varianceIn,
               const float topographyIn, const float arealUncertaintyIn)
{
   uniqueID = uniqueIDSource++;
   borderProjectionFile = NULL;
   borderColorIndex = 0;
   setData(nameIn, centerIn, samplingDensityIn,
           varianceIn, topographyIn, arealUncertaintyIn);
}

/**
 * Destructor
 */
BorderProjection::~BorderProjection()
{
}

/**
 * determine if two border projections are the same.
 */
bool 
BorderProjection::operator==(const BorderProjection& bp) const
{
   const int numLinks = getNumberOfLinks();
   if (numLinks != bp.getNumberOfLinks()) {
      return false;
   }
   
   if (getName() != bp.getName()) {
      return  false;
   }
   
   for (int i = 0; i < numLinks; i++) {
      const BorderProjectionLink* b1 = getBorderProjectionLink(i);
      const BorderProjectionLink* b2 = bp.getBorderProjectionLink(i);
      if ((*b1 == *b2) == false) {
         return false;
      }
   }
   
   return true;
}

/** 
 * Get the data from a border.
 */
void 
BorderProjection::getData(QString& nameOut, float centerOut[3], 
                float& samplingDensityOut, float& varianceOut,
                float& topographyOut, float& arealUncertaintyOut) const
{
   nameOut = name;
   centerOut[0] = center[0];
   centerOut[1] = center[1];
   centerOut[2] = center[2];
   samplingDensityOut = samplingDensity;
   varianceOut = variance;
   topographyOut = topographyValue;
   arealUncertaintyOut = arealUncertainty;
}                  

/** 
 * Set the data in a border.
 */
void
BorderProjection::setData(const QString& nameIn, const float centerIn[3], 
                const float samplingDensityIn, const float varianceIn,
                const float topographyIn, const float arealUncertaintyIn)
{
   name = nameIn;
   center[0] = centerIn[0];
   center[1] = centerIn[1];
   center[2] = centerIn[2];
   samplingDensity = samplingDensityIn;
   variance = varianceIn;
   topographyValue = topographyIn;
   arealUncertainty = arealUncertaintyIn;
   
   if (borderProjectionFile != NULL) {
      borderProjectionFile->setModified();
   }
}

/** 
 * Add a link to the border.
 */
void
BorderProjection::addBorderProjectionLink(const BorderProjectionLink& bl)
{
   //
   // Make sure border projection link is valid
   //
   if ((bl.vertices[0] >= 0) || (bl.vertices[1] >= 0) || (bl.vertices[2] >= 0.0)) {
      links.push_back(bl);
      const int index = static_cast<int>(links.size()) - 1;
      links[index].borderProjectionFile = borderProjectionFile;
   }
}

/**
 * remove duplicate border projection links.
 */
void 
BorderProjection::removeDuplicateBorderProjectionLinks()      
{
   const int num = getNumberOfLinks();
   if (num > 0) {
      std::vector<int> linksToDelete;
      for (int i = 0; i < (num - 1); i++) {
         const BorderProjectionLink* bpl = getBorderProjectionLink(i);
         for (int j = i + 1; j < num; j++) {
            const BorderProjectionLink* bpl2 = getBorderProjectionLink(j);
            if (*bpl == *bpl2) {
               linksToDelete.push_back(j);
            }
         }
      }
      
      std::unique(linksToDelete.begin(), linksToDelete.end());
      std::sort(linksToDelete.begin(), linksToDelete.end());
      std::reverse(linksToDelete.begin(), linksToDelete.end());
      const int numToDelete = static_cast<int>(linksToDelete.size());
      for (int i = 0; i < numToDelete; i++) {
         removeBorderProjectionLink(linksToDelete[i]);
      }
   }
}

/**
 * remove the last border projection link.
 */
void 
BorderProjection::removeLastBorderProjectionLink()
{
   const int num = getNumberOfLinks();
   if (num > 0) {
      removeBorderProjectionLink(num - 1);
   }
}
      
/**
 * Constructor.
 */
BorderProjectionFile::BorderProjectionFile()
   : AbstractFile("Border Projection File",
                  SpecFile::getBorderProjectionFileExtension())
{
   clear();
}

/**
 * Destructor.
 */
BorderProjectionFile::~BorderProjectionFile()
{
   clear();
}

/**
 * Append a border projection file to this one.
 */
void
BorderProjectionFile::append(BorderProjectionFile& bpf)
{
   const int numProj = bpf.getNumberOfBorderProjections();
   for (int i = 0; i < numProj; i++) {
      BorderProjection* bp = bpf.getBorderProjection(i);
      addBorderProjection(*bp);
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(bpf);
}

/**
 * Assign colors to these border projections.
 */
void 
BorderProjectionFile::assignColors(const ColorFile& colorFile)
{
   const int numProj = getNumberOfBorderProjections();
   for (int i = 0; i < numProj; i++) {
      BorderProjection* bp = getBorderProjection(i);
      bool match;
      bp->setBorderColorIndex(colorFile.getColorIndexByName(bp->getName(), match));
   }
}

/**
 * Clear the border projection file's data.
 */
void
BorderProjectionFile::clear()
{
   clearAbstractFile();
   BorderProjection::uniqueIDSource = 1;
   links.clear();
}

/**
 * add a border projection to the file.
 */
void
BorderProjectionFile::addBorderProjection(const BorderProjection& b)
{
   links.push_back(b);
   const int index = static_cast<int>(links.size()) - 1;
   links[index].borderProjectionFile = this;
   for (int i = 0; i < links[index].getNumberOfLinks(); i++) {
      BorderProjectionLink* bpl = links[index].getBorderProjectionLink(i);
      bpl->borderProjectionFile = this;
   }
   setModified();
}

/**
 * get the indices of border projections that area duplicates of other border projections.
 */
void 
BorderProjectionFile::getDuplicateBorderProjectionIndices(
                               std::vector<int>& duplicateBorderProjIndices) const
{
   duplicateBorderProjIndices.clear();
   
   const int numBorders = getNumberOfBorderProjections();
   for (int i = 0; i < (numBorders - 1); i++) {
      const BorderProjection* b = getBorderProjection(i);
      
      for (int j = i + 1; j < numBorders; j++) {
         const BorderProjection* b2 = getBorderProjection(j);
         if (*b == *b2) {
            duplicateBorderProjIndices.push_back(j);
         }
      }
   }
}
      
/**
 * Get the border projection with the unique ID
 */
BorderProjection*
BorderProjectionFile::getBorderProjectionWithUniqueID(const int uniqueID)
{
   const int index = getBorderProjectionIndexWithUniqueID(uniqueID);
   if (index >= 0) {
      return getBorderProjection(index);
   }
   return NULL;
}

/**
 * Get the index of the border projection with the unique ID
 */
int
BorderProjectionFile::getBorderProjectionIndexWithUniqueID(const int uniqueID) const
{
   const int num = getNumberOfBorderProjections();
   for (int i = 0; i < num; i++) {
      const BorderProjection* b = getBorderProjection(i);
      if (b->getUniqueID() == uniqueID) {
         return i;
      }
   }
   return -1;
}

/**
 * Remove the border projection with the specified ID
 */
void
BorderProjectionFile::removeBorderProjectionWithUniqueID(const int uniqueID)
{
   const int index = getBorderProjectionIndexWithUniqueID(uniqueID);
   if (index >= 0) {
      removeBorderProjection(index);
      setModified();
   }
}

/**
 * Remove the border projection at the specified index
 */
void
BorderProjectionFile::removeBorderProjection(const int index)
{
   if (index < getNumberOfBorderProjections()) {
      links.erase(links.begin() + index);
      setModified();
   }
}

/** 
 * remove borders with the specified indices.
 */
void 
BorderProjectionFile::removeBordersWithIndices(const std::vector<int>& borderProjectionIndicesIn)
{
   std::vector<int> borderProjectionIndices = borderProjectionIndicesIn;
   std::sort(borderProjectionIndices.begin(), borderProjectionIndices.end());
   const int num = static_cast<int>(borderProjectionIndices.size());
   for (int i = (num - 1); i >= 0; i--) {
      removeBorderProjection(borderProjectionIndices[i]);
   }
}

/**
 * compare a file for unit testing (returns true if "within tolerance").
 */
bool 
BorderProjectionFile::compareFileForUnitTesting(const AbstractFile* af,
                                                const float tolerance,
                                                QString& messageOut) const
{
   const BorderProjectionFile* bpf = dynamic_cast<const BorderProjectionFile*>(af);
   if (bpf == NULL) {
      messageOut = "File for comparison is not a border projection file.";
      return false;
   }
   
   const int numProj = getNumberOfBorderProjections();
   if (numProj != bpf->getNumberOfBorderProjections()) {
      messageOut = "Files have a different number of border projections.";
      return false;
   }
   
   for (int i = 0; i < numProj; i++) {
      const BorderProjection* bp1 = getBorderProjection(i);
      const BorderProjection* bp2 = bpf->getBorderProjection(i);
      
      if (bp1->getName() != bp2->getName()) {
         messageOut = "Border " 
                    + QString::number(i)
                    + " names to not match "
                    + bp1->getName()
                    + " and " 
                    + bp2->getName();
         return false;
      }
      
      const int numLinks = bp1->getNumberOfLinks();
      if (numLinks != bp2->getNumberOfLinks()) {
         messageOut = "Border Projection has a different number of links.";
         return false;
      }
      
      for (int j = 0; j < numLinks; j++) {
         const BorderProjectionLink* link1 = bp1->getBorderProjectionLink(j);
         const BorderProjectionLink* link2 = bp2->getBorderProjectionLink(j);
         
         int s1, s2;
         int v1[3], v2[3];
         float a1[3], a2[3];
         float r1, r2;
         link1->getData(s1, v1, a1, r1);
         link2->getData(s2, v2, a2, r2);
         
         if ((v1[0] != v2[0]) ||
             (v1[1] != v2[1]) ||
             (v1[2] != v2[2])) {
            messageOut = "border link has non-matching vertices";
            return false;
         }
         
         const float da1 = std::fabs(a1[0] - a2[0]);
         const float da2 = std::fabs(a1[1] - a2[1]);
         const float da3 = std::fabs(a1[2] - a2[2]);
         if ((da1 > tolerance) ||
             (da2 > tolerance) ||
             (da3 > tolerance)) {
            messageOut = "border link has non-matching areas";
            return false;
         }
      }
   }
   
   return true;
}
      
/** 
 * Read the border projection file data.
 */
void 
BorderProjectionFile::readFileData(QFile& /*file*/, 
                                   QTextStream& stream, 
                                   QDataStream&,
                                   QDomElement& /* rootElement */) throw (FileException)
{ 
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }
   QString line;
   
   // 1st line contains number of borders
   readLine(stream, line);
   const int numBorders = line.toInt();
   
   for (int i = 0; i < numBorders; i++) {
      readLine(stream, line);
      int borderNumber;
      int numLinks = 0;
      QString borderName;
      float samplingDensity = 25.0, variance = 1.0, topography = 0.0, uncertainty = 1.0;
      
      QTextStream(&line, QIODevice::ReadOnly) >> borderNumber
                         >> numLinks
                         >> borderName
                         >> samplingDensity
                         >> variance
                         >> topography
                         >> uncertainty;
      
      // line containing the center (ignore)
      float center[3] = { 0.0, 0.0, 0.0 };
      readLine(stream, line);
      
      // create the border
      BorderProjection borderProj(borderName, center, samplingDensity, variance, 
                                 topography, uncertainty);
      
      for (int j = 0; j < numLinks; j++) {
         readLine(stream, line);
         
         int sectionNumber;
         int vertices[3];
         float areas[3];
         float radius = 0.0;
         
         QTextStream(&line, QIODevice::ReadOnly) >> vertices[0]
                             >> vertices[1]
                             >> vertices[2]
                             >> sectionNumber
                             >> areas[0]
                             >> areas[1]
                             >> areas[2]
                             >> radius;

         BorderProjectionLink bl(sectionNumber, vertices, areas, radius);
         bl.borderProjectionFile = this;
         borderProj.addBorderProjectionLink(bl);
      }
      
      addBorderProjection(borderProj);
   }
}

/** 
 * Write the border projection file data.
 */
void 
BorderProjectionFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                    QDomElement& /* rootElement */) throw (FileException)
{
   int numBorders = getNumberOfBorderProjections();

   //
   // only write borders that have links
   //
   int numBordersToWrite = 0;
   for (int i = 0; i < numBorders; i++) {
      if (getBorderProjection(i)->getNumberOfLinks() > 0) {
         numBordersToWrite++;
      }
   }
   stream << numBordersToWrite << "\n";
   
   for (int i = 0; i < numBorders; i++) {
      BorderProjection* border = getBorderProjection(i);
      const int numLinks = border->getNumberOfLinks();
      
      //
      // Only write borders with links
      //
      if (numLinks <= 0) {
         continue;
      }
      
      // write border number, number links, name, sampling, variance, topography
      QString name;
      float center[3];
      float samplingDensity, variance, topography, uncertainty;
      border->getData(name, center, samplingDensity, variance, 
                      topography, uncertainty);
      stream << i << " "
             << numLinks << " "
             << name << " "
             << samplingDensity << " "
             << variance << " "
             << topography << " "
             << uncertainty << "\n";
              
      // write border center (unused)
      stream << center[0] << " " << center[1] << " " << center[2] << "\n";
      
      for (int j = 0; j < numLinks; j++) {
         BorderProjectionLink* bl = border->getBorderProjectionLink(j);
         
         // write vertices, section, and areas
         float areas[3];
         float radius;
         int vertices[3];
         int section;
         bl->getData(section, vertices, areas, radius);
         stream << vertices[0] << " "
                << vertices[1] << " "
                << vertices[2] << " "
                << section << " "
                << areas[0] << " "
                << areas[1] << " "
                << areas[2] << " "
                << radius << "\n";
      }
   }
}


