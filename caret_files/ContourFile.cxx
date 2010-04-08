/*LICENSE_START*/

#include <StringUtilities.h>

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
#include <iostream>
#include <limits>

#include "BorderFile.h"
#define _CONTOUR_FILE_MAIN_
#include "ContourFile.h"
#undef _CONTOUR_FILE_MAIN_
#include "DebugControl.h"
#include "MathUtilities.h"
#include "MDPlotFile.h"
#include "NeurolucidaFile.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CaretContour::ContourPoint::ContourPoint()
{
   x = 0.0;
   y = 0.0;
   z = 0.0;
   specialFlag = false;
   highlightFlag = false;
}
      
/**
 * constructor.
 */
CaretContour::ContourPoint::ContourPoint(const float xi,
             const float yi,
             const float zi,
             const bool special)
{
   x = xi;
   y = yi;
   z = zi;
   specialFlag = special;
   highlightFlag = false;
}
      
/**
 * destructor.
 */
CaretContour::ContourPoint::~ContourPoint()
{
}

/** 
 * Constructor
 */
CaretContour::CaretContour(const int estimatedNumberOfPointsInContour)
{
   contourFile   = NULL;
   sectionNumber = -1;
   if (estimatedNumberOfPointsInContour > 0) {
      points.reserve(estimatedNumberOfPointsInContour);
   }
}

/** 
 * Destructor
 */
CaretContour::~CaretContour()
{
   clearPoints();
   contourFile = NULL;
}

/**
 * copy constructor.
 */
CaretContour::CaretContour(const CaretContour& cc)
{
   copyHelper(cc);
}

/**
 * assignment operator.
 */
CaretContour& 
CaretContour::operator=(const CaretContour& cc)
{
   if (this != &cc) {
      copyHelper(cc);
   }
   
   return *this;
}

/**
 * copy helper.
 */
void 
CaretContour::copyHelper(const CaretContour& cc)
{
   points = cc.points;
   sectionNumber = cc.sectionNumber;
   contourFile = NULL;
}

/**
 * operator for comparing contours by section number.
 */
bool 
CaretContour::operator<(const CaretContour& cc) const 
{
   const bool lessThan = (this->sectionNumber < cc.sectionNumber);
   return lessThan;
}

/** 
 * Add a point to this contour.
 */
void
CaretContour::addPoint(const float xIn, const float yIn, const float zIn)
{
   ContourPoint cp(xIn, yIn, zIn, false);
   points.push_back(cp);
   if (contourFile != NULL) {
      contourFile->setModified();
   }
}

/** 
 * Add a number of points to this contour.
 */
void
CaretContour::addPoints(const float* xIn, const float* yIn, const int numPoints)
{
   for (int i = 0; i < numPoints; i++) {
      ContourPoint cp(xIn[i], yIn[i], sectionNumber, false);
      points.push_back(cp);
   }
   if (contourFile != NULL) {
      contourFile->setModified();
   }
}

/** 
 * delete a point from this contour.
 */
void 
CaretContour::deletePoint(const int pointIndex)
{
   if ((pointIndex >= 0) && (pointIndex < getNumberOfPoints())) {
      points.erase(points.begin() + pointIndex);
   }
   if (contourFile != NULL) {
      contourFile->setModified();
   }
}

/** 
 * get the number of points in this contour.
 */
int
CaretContour::getNumberOfPoints() const
{
   return static_cast<int>(points.size());
}

/** 
 * See if any "special flags" are set.
 */
bool 
CaretContour::getAnySpecialFlagSet() const
{
   const int num = getNumberOfPoints();
   for (int i = 0; i < num; i++) {
      if (points[i].specialFlag) {
         return true;
      }
   }
   return false;
}

/** 
 * Get the coordinates of a point in this contour.
 */
void
CaretContour::getPointXY(const int pointIndex, float& xOut, float& yOut) const
{
   xOut = points[pointIndex].x;
   yOut = points[pointIndex].y;
}

/** 
 * Get the coordinates of a point in this contour.
 */
void
CaretContour::getPointXYZ(const int pointIndex, float& xOut, float& yOut, float& zOut) const
{
   xOut = points[pointIndex].x;
   yOut = points[pointIndex].y;
   zOut = points[pointIndex].z;
}

/** 
 * Get the coordinates of a point in this contour.
 */
void
CaretContour::getPointXYZ(const int pointIndex, float xyzOut[3]) const
{
   xyzOut[0] = points[pointIndex].x;
   xyzOut[1] = points[pointIndex].y;
   xyzOut[2] = points[pointIndex].z;
}

/**
 * get the estimated distance between each point in the contour.
 */
float 
CaretContour::getAverageDistanceBetweenPoints() const
{
   float dist = 0.0;
   
   const int num = getNumberOfPoints() - 1;
   for (int i = 0; i < num; i++) {
      float x1, x2, y1, y2, z1, z2;
      getPointXYZ(i, x1, y1, z1);
      getPointXYZ(i + 1, x2, y2, z2);
      const float dx = x2 - x1;
      const float dy = y2 - y1;
      const float dz = z2 - z1;
      const float d = std::sqrt(dx*dx + dy*dy + dz*dz);
      dist += d;
   }
   
   if (num > 0) {
      dist = dist / static_cast<float>(num);
   }
   
   return dist;
}

/**
 * resample the contour.
 */
void 
CaretContour::resample(const float distanceBetweenPoints)
{
   const int num = getNumberOfPoints() - 1;
   if (num > 1) {
      //
      // Convert to border
      //
      Border b;
      for (int i = 0; i < num; i++) {
         float x, y, z;
         getPointXYZ(i, x, y, z);
         const float xyz[3] = { x, y, z };
         b.addBorderLink(xyz);
      }
      
      //
      // Resample the border
      //
      int newNum = 0;
      b.resampleBorderToDensity(distanceBetweenPoints, 2, newNum);
      
      //
      // Clear this contour
      //
      clearPoints();
      
      //
      // Convert border back to contours
      //
      const int numLinks = b.getNumberOfLinks();
      for (int i = 0; i < numLinks; i++) {
         const float* xyz = b.getLinkXYZ(i);
         addPoint(xyz[0], xyz[1], xyz[2]);
      }
   }
}
      
/**
 * clear the contour points.
 */
void 
CaretContour::clearPoints()
{
   points.clear();
}

/** 
 * Get the section number of this contour.
 */
int
CaretContour::getSectionNumber() const
{
   return sectionNumber;
}

/** 
 * Set the coordinates for a point in this contour.
 */
void 
CaretContour::setPointXY(const int pointIndex, const float xIn, const float yIn)
{
   points[pointIndex].x = xIn;
   points[pointIndex].y = yIn;
   if (contourFile != NULL) {
      contourFile->setModified();
   }
}

/** 
 * Set the coordinates for a point in this contour.
 */
void 
CaretContour::setPointXYZ(const int pointIndex, const float xIn, const float yIn, const float zIn)
{
   if ((pointIndex >= 0) && (pointIndex < getNumberOfPoints())) {
      points[pointIndex].x = xIn;
      points[pointIndex].y = yIn;
      points[pointIndex].z = zIn;
      if (contourFile != NULL) {
         contourFile->setModified();
      }
   }
}

/**
 * clear the highlight flags.
 */
void 
CaretContour::clearHighlightFlags()
{
   for (int i = 0; i < getNumberOfPoints(); i++) {
      points[i].highlightFlag = false;
   }
}

/**
 * get the highlight flag for a point.
 */
bool 
CaretContour::getHighlightFlag(const int pointIndex) const
{
   return points[pointIndex].highlightFlag; 
}

/**
 * set the special flag for a point.
 */
void 
CaretContour::setHighlightFlag(const int pointIndex, const bool flag)
{
   if ((pointIndex >= 0) && (pointIndex < getNumberOfPoints())) {
      points[pointIndex].highlightFlag = flag; 
   }
}

/**
 * Set the special flag for a point.
 */
void 
CaretContour::setSpecialFlag(const int pointIndex, const bool flag) 
{ 
   if ((pointIndex >= 0) && (pointIndex < getNumberOfPoints())) {
      points[pointIndex].specialFlag = flag; 
   }
}

/**
 * Get the special flag for a point.
 */
bool 
CaretContour::getSpecialFlag(const int pointIndex) const 
{ 
   return points[pointIndex].specialFlag; 
}

/**
 * Clear the special flag for all points.
 */
void 
CaretContour::clearSpecialFlags() 
{
   for (int i = 0; i < getNumberOfPoints(); i++) {
      points[i].specialFlag = false;
   }
}

/** 
 * Set the section number for this contour.
 */
void
CaretContour::setSectionNumber(const int sectionNumberIn)
{
   sectionNumber = sectionNumberIn;
   for (int i = 0; i < getNumberOfPoints(); i++) {
      points[i].z = sectionNumber;
   }
   if (contourFile != NULL) {
      contourFile->updateMinMaxSections(sectionNumber);
      contourFile->setModified();
   }
}

/**
 * reverse the order of the points in a contour.
 */
void 
CaretContour::reversePointOrder()
{
   const int num = getNumberOfPoints();
   if (num > 1) {
      std::reverse(points.begin(), points.end());

      if (contourFile != NULL) {
         contourFile->setModified();
      }
   }
}

/**
 * remove consecutive duplicate points (same x/y).
 */
void 
CaretContour::removeDuplicatePoints()
{
   const int num = getNumberOfPoints();
   if (num > 0) {
      //
      // containers for retained contour points
      //
      std::vector<ContourPoint> newPoints;
      
      //
      // get first point in contour
      //
      float xLast, yLast, zLast;
      getPointXYZ(0, xLast, yLast, zLast);
      bool specialLast = getSpecialFlag(0);
      newPoints.push_back(points[0]);
      
      //
      // loop through remaining contours
      //
      for (int i = 1; i < num; i++) {
         float x, y, z;
         getPointXYZ(i, x, y, z);
         bool special = getSpecialFlag(i);
         
         //
         // Did the X or Y change from the previous point
         //
         if ((x != xLast) || (y != yLast)) {
            xLast = x;
            yLast = y;
            zLast = z;
            specialLast = special;
            newPoints.push_back(points[i]);
         }
      }
      
      //
      // replace the contour
      //
      const int numPts = static_cast<int>(newPoints.size());
      if (num != numPts) {
         points = newPoints;
         if (contourFile != NULL) {
            contourFile->setModified();
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "INFO: contour had " << (num - newPoints.size()) << " duplicate points." << std::endl;
         }
      }
   }
}
      
/** 
 * Constructor.
 */
ContourFile::ContourFile()
   : AbstractFile("Contour File", SpecFile::getContourFileExtension())
{
   clear();
}

/** 
 * Destructor.
 */
ContourFile::~ContourFile()
{
   clear();
}

/**
 * Append a contour file to this contour file.
 */
void
ContourFile::append(ContourFile& cf, QString& errorMessage)
{
   errorMessage = "";
   const int num = cf.getNumberOfContours();
   
   for (int i = 0; i < num; i++) {
      CaretContour* cc = cf.getContour(i);
      addContour(*cc);
   }

   //
   // transfer the file's comment
   //
   appendFileComment(cf);

   setMinMaxSections();
   minimumSelectedSection = minimumSection;
   maximumSelectedSection = maximumSection;
   setSectionType(SECTION_TYPE_ALL);

   setModified();
}

/** 
 * Clear this contour file.
 */
void
ContourFile::clear()
{
   clearAbstractFile();
   contours.clear();
   sectionSpacing = 1.0;
   sectionType = SECTION_TYPE_ALL;
   setMinMaxSections();
   mainWindowScaling[0] = -1.0;
   mainWindowScaling[1] = -1.0;
   mainWindowScaling[2] = -1.0;
}

/**
 * cleanup contours - remove duplicate points in contours and contours with less than 3 points.
 */
bool 
ContourFile::cleanupContours()
{
   const int num = getNumberOfContours();
   std::vector<CaretContour> newContours;
   
   bool changeFlag = false;
   for (int i = 0; i < num; i++) {
      CaretContour& cc = contours[i];
      //
      // Remove duplicate points from the contours
      //
      const int oldPts = cc.getNumberOfPoints();
      cc.removeDuplicatePoints();
      
      //
      // Only keep those contours with more than 2 points
      //
      int numPts = cc.getNumberOfPoints();
      if (numPts > 2) {
         newContours.push_back(cc);
      }
      else {
         numPts = 0;
      }
      
      if (oldPts != numPts) {
         changeFlag = true;
      }
   }
   
   if (changeFlag) {
      contours = newContours;
      setModified();
      
      if (DebugControl::getDebugOn()) {
         std::cout << "INFO: " << (num - newContours.size()) << " contours were removed during cleanup." << std::endl;
      }
   }
   newContours.clear();
   
   return changeFlag;
}
      
/** 
 * Clear all of the special flags.
 */
void 
ContourFile::clearSpecialFlags()
{
   const int num = getNumberOfContours();
   
   for (int i = 0; i < num; i++) {
      contours[i].clearSpecialFlags();
   }
}

/** 
 * Clear all of the highlight flags.
 */
void 
ContourFile::clearHighlightFlags()
{
   const int num = getNumberOfContours();
   
   for (int i = 0; i < num; i++) {
      contours[i].clearHighlightFlags();
   }
}

/** 
 * Add a contour to this contour file.
 */
void
ContourFile::addContour(const CaretContour& contourIn)
{
   contours.push_back(contourIn);
   const int contourIndex = getNumberOfContours() - 1;
   CaretContour* cc = getContour(contourIndex);
   cc->contourFile = this;
   if (contourIndex == 0) {
      minimumSection = cc->sectionNumber;
      maximumSection = cc->sectionNumber;
   }
   updateMinMaxSections(cc->sectionNumber);
   setModified();
}

/**
 * set the special flag for all contour points in the section range and box
 */
void 
ContourFile::setSpecialFlags(const int sectionLow,
                             const int sectionHigh,
                             const float bounds[4])
{
   const int num = getNumberOfContours();
   
   const float minX = std::min(bounds[0], bounds[2]);
   const float maxX = std::max(bounds[0], bounds[2]);
   const float minY = std::min(bounds[1], bounds[3]);
   const float maxY = std::max(bounds[1], bounds[3]);

   for (int i = 0; i < num; i++) {
      if ((contours[i].sectionNumber >= sectionLow) &&
          (contours[i].sectionNumber <= sectionHigh)) {
         const int numPoints = contours[i].getNumberOfPoints();
         for (int j = 0; j < numPoints; j++) {
            const float x = contours[i].points[j].x;
            const float y = contours[i].points[j].y;
            
            if ((x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY)) {
               contours[i].points[j].specialFlag = true;
            }
         }
      }
   }            
}

/**
 * find contour point within distance of coordinate.
 */
void 
ContourFile::findContourPoint(const float xyz[3], 
                              const float withinDistance,
                              int& contourNumber, 
                              int& contourPointNumber) const
{
   const float withinDistanceSQ = withinDistance * withinDistance;
   float closestDistanceSQ = std::numeric_limits<float>::max();
   contourNumber = -1;
   contourPointNumber = -1;
   
   const int numContours = getNumberOfContours();
   for (int i = 0; i < numContours; i++) {
      const CaretContour* cc = getContour(i);
      const int numPoints = cc->getNumberOfPoints();
      for (int j = 0; j < numPoints; j++) {
         float pointXYZ[3];
         cc->getPointXYZ(j, pointXYZ[0], pointXYZ[1], pointXYZ[2]);
         const float distSQ = MathUtilities::distanceSquared3D(xyz, pointXYZ);
         if (distSQ < withinDistanceSQ) {
            if (distSQ < closestDistanceSQ) {
               contourNumber = i;
               contourPointNumber = j;
               closestDistanceSQ = distSQ;
            }
         }
      }
   }
}
      
/** 
 * Apply a transformation matrix to this contour file.
 */
void 
ContourFile::applyTransformationMatrix(const int sectionLow, 
                                       const int sectionHigh,
                                       const TransformationMatrix& matrixIn,
                                       const bool limitToSpecialFlagNodes)
{
   TransformationMatrix& matrix = const_cast<TransformationMatrix&>(matrixIn);
   
   const int num = getNumberOfContours();
   
   for (int i = 0; i < num; i++) {
      if ((contours[i].sectionNumber >= sectionLow) &&
          (contours[i].sectionNumber <= sectionHigh)) {
         const int numPoints = contours[i].getNumberOfPoints();
         for (int j = 0; j < numPoints; j++) {
            bool transformIt = true;
            if (limitToSpecialFlagNodes) {
               transformIt = contours[i].points[j].specialFlag;
            }
            
            if (transformIt) {
               double p[4] = { contours[i].points[j].x,
                               contours[i].points[j].y, 
                               contours[i].points[j].z, 1.0 };
               matrix.multiplyPoint(p);
               contours[i].points[j].x = p[0];
               contours[i].points[j].y = p[1];
               contours[i].points[j].z = p[2];
            }
         }
      }
   }
   setModified();
}

/** 
 * delete a contour from this contour file.
 */
void 
ContourFile::deleteContour(const int indexNumber)
{   
   const int num = getNumberOfContours();
   if ((indexNumber >= 0) && (indexNumber < num)) {
      contours.erase(contours.begin() + indexNumber);
      setModified();
      setMinMaxSections();
   }
}

/** 
 * Get a contour from this contour file.
 */
CaretContour*
ContourFile::getContour(const int indexNumber)
{
   return &contours[indexNumber];
}

/** 
 * Get a contour from this contour file (const method).
 */
const CaretContour*
ContourFile::getContour(const int indexNumber) const
{
   return &contours[indexNumber];
}

/** 
 * Get the extent of this contour file.
 */
void 
ContourFile::getExtent(float& minX, float& maxX,
                       float& minY, float& maxY) const
{
   const int numContours = getNumberOfContours();
   
   minX =  std::numeric_limits<float>::max();
   maxX = -std::numeric_limits<float>::max();
   minY =  std::numeric_limits<float>::max();
   maxY = -std::numeric_limits<float>::max();

   for (int i = 0; i < numContours; i++) {
      const int numPoints = contours[i].getNumberOfPoints();
      for (int j = 0; j < numPoints; j++) {
         if (contours[i].points[j].x < minX) {
            minX = contours[i].points[j].x;
         }
         if (contours[i].points[j].x > maxX) {
            maxX = contours[i].points[j].x;
         }
         if (contours[i].points[j].y < minY) {
            minY = contours[i].points[j].y;
         }
         if (contours[i].points[j].y > maxY) {
            maxY = contours[i].points[j].y;
         }
      }
   }
}

/** 
 * Get the number of contours in this contour file.
 */
int
ContourFile::getNumberOfContours() const
{
   return static_cast<int>(contours.size());
}

/** 
 * For a specified section, get the center of gravity of its contour points.
 */
int 
ContourFile::getSectionCOG(const int sectionNumber,
                           float& cogX, float& cogY) const
{
   
   const int numContours = getNumberOfContours();
   
   cogX = 0.0;
   cogY = 0.0;
   int cnt = 0;
   
   for (int i = 0; i < numContours; i++) {
      if (contours[i].sectionNumber == sectionNumber) {
         const int numPoints = contours[i].getNumberOfPoints();
         
         for (int j = 0; j < numPoints; j++) {
            cogX += contours[i].points[j].x;
            cogY += contours[i].points[j].y;
            cnt++;
         }
      }
   }
   
   if (cnt > 0) {
      cogX /= static_cast<float>(cnt);
      cogY /= static_cast<float>(cnt);
   }
   
   return cnt;
}

/**
 * resample all of the contours.
 */
void 
ContourFile::resampleAllContours(const float distanceBetweenPoints)
{
   const int numContours = getNumberOfContours();
   for (int i = 0; i < numContours; i++) {
      contours[i].resample(distanceBetweenPoints);
   }
}
      
/** 
 * Get the extents of a single section.
 */
void 
ContourFile::getSectionExtent(int& lowest, int& highest) const
{
   lowest = minimumSection;
   highest = maximumSection;
}

/**
 *  Set the minimum and maximum section numbers.
 */
void
ContourFile::setMinMaxSections()
{
   const int numContours = getNumberOfContours();
   if (numContours <= 0) {
      minimumSection = std::numeric_limits<int>::max();
      maximumSection = std::numeric_limits<int>::min();;
      return;
   }
   
   minimumSection  = contours[0].sectionNumber;
   maximumSection = contours[0].sectionNumber;
   
   for (int i = 0; i < numContours; i++) {
      maximumSection = std::max(contours[i].sectionNumber, maximumSection);
      minimumSection = std::min(contours[i].sectionNumber, minimumSection);
   }
}

/** 
 * Get the spacing between sections (millimeters).
 */
float 
ContourFile::getSectionSpacing() const
{
   return sectionSpacing;
}

/** 
 * Merge two contours (append contour2 to contour1).
 */
void
ContourFile::mergeContours(const int contour1, const int contour2)
{
   if ((contour1 >= 0) && (contour1 < getNumberOfContours()) &&
       (contour2 >= 0) && (contour2 < getNumberOfContours())) {
      CaretContour* first = getContour(contour1);
      CaretContour* second = getContour(contour2);
      const float dist = std::max(first->getAverageDistanceBetweenPoints(),
                                  second->getAverageDistanceBetweenPoints());
      const int num = second->getNumberOfPoints();
      for (int i = 0; i < num; i++) {
         float x, y, z;
         second->getPointXYZ(i, x, y, z);
         first->addPoint(x, y, z);
      }
      if (dist > 0.0) {
         first->resample(dist);
      }
      deleteContour(contour2);
   }
   setModified();
   setMinMaxSections();
}

/** 
 * Set the spacing between sections (millimeters).
 */
void 
ContourFile::setSectionSpacing(const float sectionSpacingIn)
{
   sectionSpacing = sectionSpacingIn;
   setModified();
}

/** 
 * Sort the contours by section number.
 */
void
ContourFile::sortBySectionNumber()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Before sorting: ";
      const int num = getNumberOfContours();
      for (int i = 0; i < num; i++) {
         std::cout << " " << getContour(i)->getSectionNumber();
      }
      std::cout << std::endl;
   }

   std::sort(contours.begin(), contours.end());

   const int num1 = getNumberOfContours();
   for (int i = 0; i < num1; i++) {
      getContour(i)->contourFile = this;
   }

   if (DebugControl::getDebugOn()) {
      std::cout << "After sorting: ";
      const int num2 = getNumberOfContours();
      for (int i = 0; i < num2; i++) {
         std::cout << " " << getContour(i)->getSectionNumber();
      }
      std::cout << std::endl;
   }
}

/**
 * import contours from a Nuerolucida File.
 */
void 
ContourFile::importNeurolucidaFile(const NeurolucidaFile& nf) throw (FileException)
{
   const int num = nf.getNumberOfContours();
   for (int i = 0; i < num; i++) {
      addContour(*(nf.getContour(i)));
   }
}
      
/**
 * import contours from an MD Plot File.
 */
void 
ContourFile::importMDPlotFile(const MDPlotFile& mdf) throw (FileException)
{
   const int numLines = mdf.getNumberOfLines();
   const int numVerticesInFile = mdf.getNumberOfVertices();
   for (int i = 0; i < numLines; i++) {
      const MDPlotLine* mdl = mdf.getLine(i);
      
      bool sectionHasBeenSetFlag = false;
      const int numVerticesInLine = mdl->getNumberOfVertices();
      CaretContour cc;  //(numVerticesInLine);
      for (int j = 0; j < numVerticesInLine; j++) {
         const int vertexIndex = mdl->getVertexIndex(j);
         if ((vertexIndex < 0) ||
             (vertexIndex >= numVerticesInFile)) {
            std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid MDPlot Vertex (" << j << ") Index in ContourFile::importMDPlotFile: "
                      << vertexIndex << std::endl;
            continue;
         }
         
         const MDPlotVertex* mdv = mdf.getVertex(vertexIndex);
         if (mdv == NULL) {
            std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: NULL MDPlot Vertex in ContourFile::importMDPlotFile" << std::endl;
            continue;
         }
         float x, y, z;
         mdv->getXYZ(x, y, z);
         
         if (sectionHasBeenSetFlag == false) {
            sectionHasBeenSetFlag = true;
            cc.setSectionNumber(static_cast<int>(z));
         }
         cc.addPoint(x, y, z);
      }
      
      if (cc.getNumberOfPoints() > 0) {
         addContour(cc);
      }
   }
}      

/**
 * Get main window scaling (invalid if negative values).
 */
void
ContourFile::getMainWindowScaling(float scaleOut[3]) const
{
   scaleOut[0] = mainWindowScaling[0];
   scaleOut[1] = mainWindowScaling[1];
   scaleOut[2] = mainWindowScaling[2];
}

/**
 * Set the main window scaling
 */
void
ContourFile::setMainWindowScaling(const float scaleIn[3])
{
   mainWindowScaling[0] = scaleIn[0];
   mainWindowScaling[1] = scaleIn[1];
   mainWindowScaling[2] = scaleIn[2];
   setModified();
}

/** 
 * Read a version 1 contour file.
 */
void
ContourFile::readFileDataVersion1(QTextStream& stream) throw (FileException)
{
   int numContours = -1;
   
   bool readingTags = true;
   while(readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      
      if (tag == tagBeginData) {
         readingTags = false;
      }
      else if (tag == tagNumberOfContours) {
         numContours = tagValue.toInt();
      }
      else if (tag == tagSectionSpacing) {
         sectionSpacing = tagValue.toFloat();
      }
      else if (tag == tagMainWindowScaling) {
         std::vector<float> scaling;
         StringUtilities::token(tagValue, " ", scaling);
         if (scaling.size() >= 3) {
            mainWindowScaling[0] = scaling[0];
            mainWindowScaling[1] = scaling[1];
            mainWindowScaling[2] = scaling[2];
         }
      }
   }
   
   if (numContours <= 0) {
      throw FileException(filename, "Contour File contains 0 contours.");
   }

   QString line, lastLine;
   std::vector<QString> tokens;
      
   for (int i = 0; i < numContours; i++) {
      lastLine = line;
      readLineIntoTokens(stream, line, tokens);
      
      //if (DebugControl::getDebugOn()) {
      //   std::cout << "reading contour i: " << i;
      //   if (tokens.empty() == false) {
      //      std::cout << " contour num: " << tokens[0].toInt();
      //   }
      //   std::cout << std::endl;
      //}
      
      if (tokens.size() == 3) {
         const int numPoints = tokens[1].toInt();
         const int section = tokens[2].toInt();
         //if (DebugControl::getDebugOn()) {
            std::cout << "contour " << i
                      << ", section " << section
                      << ", contains " << numPoints
                      << " points." << std::endl;
         //}
         
         CaretContour cc;
         cc.setSectionNumber(section);
         
         for (int j = 0; j < numPoints; j++) {
            lastLine = line;
            readLineIntoTokens(stream, line, tokens);
            if (tokens.size() == 2) {
               const float x = tokens[0].toFloat();
               const float y = tokens[1].toFloat();
               if (sectionSpacing != 0.0) {
                  cc.addPoint(x, y, section * sectionSpacing);
               }
               else {
                  cc.addPoint(x, y, section);
               }
            }
            else {
               QString msg("ERROR: Reading contour file data line ");
               msg.append(line);
               msg.append("\nPrevious line read: ");
               msg.append(lastLine);
               throw FileException(filename, msg);
            }
         }
         
         if (cc.getNumberOfPoints() > 0) {
            addContour(cc);
         }
      }
      else {
         QString msg("ERROR: Reading contour file data line: ");
         msg.append(line);
         msg.append("\nPrevious line read: ");
         msg.append(lastLine);
         throw FileException(filename, msg);
      }
   }
}

/** 
 * Read a version 0 contour file.
 */
void
ContourFile::readFileDataVersion0(QTextStream& /*stream*/) throw (FileException)
{
   throw FileException(filename, "Reading version 0 contour files not supported.");
}

/** 
 * Read a contour file's data.
 */
void
ContourFile::readFileData(QFile& file, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   //
   // save file position since version 0 has no tags
   //
   const qint64 oldFilePosition = stream.pos(); //file.pos();
   
   int fileVersion = 0;
   
   QString versionStr, versionNumberStr;
   readTagLine(stream, versionStr, versionNumberStr);
   if (versionStr == tagFileVersion) {
      fileVersion = versionNumberStr.toInt();
   }
   switch(fileVersion) {
      case 0:
         file.seek(oldFilePosition);
         stream.seek(oldFilePosition);
         readFileDataVersion0(stream);
         break;
      case 1:
         readFileDataVersion1(stream);
         break;
      default:
         throw FileException(filename,
               "Invalid Contour File version.  Perhaps you need a newer version of Caret.");
         break;
   }

   setMinMaxSections();
   minimumSelectedSection = minimumSection;
   maximumSelectedSection = maximumSection;
   setSectionType(SECTION_TYPE_ALL);
}

/** 
 * write a contour file's data.
 */
void
ContourFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   const int numContours = getNumberOfContours();
   
   stream << tagFileVersion << " 1\n";
   stream << tagNumberOfContours << " " << numContours << "\n";
   stream << tagSectionSpacing << " " << sectionSpacing << "\n";
   stream << tagMainWindowScaling
          << " " << mainWindowScaling[0]
          << " " << mainWindowScaling[1]
          << " " << mainWindowScaling[2]
          << "\n";
   
   stream << tagBeginData << "\n";
   
   for (int i = 0; i < numContours; i++) {
      CaretContour* cc = getContour(i);
      
      const int numPoints = cc->getNumberOfPoints();
      stream << i << " " << numPoints << " " << cc->sectionNumber << "\n";
      
      for (int j = 0; j < numPoints; j++) {
         stream << cc->points[j].x << " " << cc->points[j].y << "\n";
      }
   }
}

/**
 * get the section type.
 */
ContourFile::SECTION_TYPE 
ContourFile::getSectionType() const
{
   return sectionType;
}

/**
 * Set the selection type.
 */
void 
ContourFile::setSectionType(const SECTION_TYPE type)
{
   sectionType = type;
   
   switch(sectionType) {
      case SECTION_TYPE_SINGLE:
         maximumSelectedSection = minimumSelectedSection;
         break;
      case SECTION_TYPE_MULTIPLE:
         break;
      case SECTION_TYPE_ALL:
         minimumSelectedSection = minimumSection;
         maximumSelectedSection = maximumSection;
         break;
      case SECTION_TYPE_HIDE:
         break;
   }
}

/**
 * get the minimum selected section
 */
int
ContourFile::getMinimumSelectedSection() const
{
   return minimumSelectedSection;
}

/**
 * get the maximum selected section
 */
int
ContourFile::getMaximumSelectedSection() const
{
   return maximumSelectedSection;
}

/**
 * set the minimum selected section
 */
void
ContourFile::setMinimumSelectedSection(const int sect)
{
   minimumSelectedSection = sect;
}

/**
 * set the maximum selected section
 */
void
ContourFile::setMaximumSelectedSection(const int sect)
{
   maximumSelectedSection = sect;
}

/**
 * Update minimum and maximum section numbers.
 */
void
ContourFile::updateMinMaxSections(const int sectionNumber)
{
   if (sectionNumber < minimumSection) {
      minimumSection = sectionNumber;
   }
   if (sectionNumber > maximumSection) {
      maximumSection = sectionNumber;
   }
   if (sectionType == SECTION_TYPE_ALL) {
      minimumSelectedSection = minimumSection;
      maximumSelectedSection = maximumSection;
   }
}
