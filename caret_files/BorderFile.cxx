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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#ifdef CARET_OS_WINDOWS
#include <Windows.h>
#endif
#ifdef CARET_OS_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>

#include "BorderFile.h"
#include "ColorFile.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GiftiLabelTable.h"
#include "MathUtilities.h"
#include "StatisticDataGroup.h"
#include "StatisticMeanAndDeviation.h"
#include "TopologyFile.h"
#include "TransformationMatrixFile.h"
#include "Caret6ProjectedItem.h"

/** 
 * Constructor.
 */
Border::Border(const QString& nameIn, const float* centerIn, 
               const float samplingDensityIn, const float varianceIn,
               const float topographyIn, const float arealUncertaintyIn)
{
   borderFile = NULL;
   displayFlag = true;
   nameDisplayFlag = true;
   borderColorIndex = 0;
   borderProjectionID = -1;
   float center[3] = { 0.0, 0.0, 0.0 };
   if (centerIn != NULL) {
      center[0] = centerIn[0];
      center[1] = centerIn[1];
      center[2] = centerIn[2];
   }
   setData(nameIn, center, samplingDensityIn,
           varianceIn, topographyIn, arealUncertaintyIn);
}

/**
 * determine if two borders are the same (same name and links).
 */
bool 
Border::operator==(const Border& b) const
{
   const int numLinks = getNumberOfLinks();
   if (numLinks != b.getNumberOfLinks()) {
      return false;
   }
   
   if (getName() != b.getName()) {
      return false;
   }
   
   for (int i = 0 ; i < numLinks; i++) {
      float me[3], other[3];
      getLinkXYZ(i, me);
      b.getLinkXYZ(i, other);
      
      for (int j = 0; j < 3; j++) {
         if (me[j] != other[j]) {
            return false;
         }
      }
   }
      
   return true;
}

/**
 * Apply transformation matrix to a border.
 */
void
Border::applyTransformationMatrix(TransformationMatrix& tm)
{ 
   const int numLinks = getNumberOfLinks();
   for (int j = 0; j < numLinks; j++) {
      float xyz[3];
      getLinkXYZ(j, xyz);
      double p[4] = { xyz[0], xyz[1], xyz[2], 1.0 };
      tm.multiplyPoint(p);
      xyz[0]   = p[0];
      xyz[1] = p[1];
      xyz[2] = p[2];
      setLinkXYZ(j, xyz);
   }
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}  

/**
 * append a border this "this" one.
 */
void 
Border::appendBorder(const Border& b)
{
   const int numLinks = b.getNumberOfLinks();
   for (int j = 0; j < numLinks; j++) {
      this->addBorderLink(b.getLinkXYZ(j), b.getLinkSectionNumber(j));
   }
}      

/** 
 * Get the data from a border.
 */
void 
Border::getData(QString& nameOut, float centerOut[3], 
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
Border::setData(const QString& nameIn, const float centerIn[3], 
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
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/** 
 * Get the position of a link.
 */
void
Border::getLinkXYZ(const int linkNumber, float xyz[3]) const
{
   xyz[0] = linkXYZ[linkNumber * 3];
   xyz[1] = linkXYZ[linkNumber * 3 + 1];
   xyz[2] = linkXYZ[linkNumber * 3 + 2];
}

/**
 * Set the position of a link.
 */
void
Border::setLinkXYZ(const int linkNumber, const float xyz[3])
{
   linkXYZ[linkNumber * 3]     = xyz[0];
   linkXYZ[linkNumber * 3 + 1] = xyz[1];
   linkXYZ[linkNumber * 3 + 2] = xyz[2];
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * get the radius for a link.
 */
float 
Border::getLinkRadius(const int linkNumber) const
{
   const float r = linkRadii[linkNumber];
   return r;
}

/**
 * set the radius for a link.
 */
void 
Border::setLinkRadius(const int linkNumber, const float radius)
{
   linkRadii[linkNumber] = radius;
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}      

/**
 * set the uncertainty.
 */
void 
Border::setArealUncertainty(const float uncertainty)
{
   arealUncertainty = uncertainty;
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * find first link in "this" border that is within "tolerance" distance
 * of a link in "other border".
 */
bool 
Border::intersection3D(const Border* otherBorder,
                       const float intersectionTolerance,
                       int& myLinkIntersect, 
                       int& otherLinkIntersect) const
{
   const float intersectionToleranceSquared = intersectionTolerance
                                              * intersectionTolerance;
   myLinkIntersect    = -1;
   otherLinkIntersect = -1;
   
   const int myNumLinks = getNumberOfLinks();
   const int otherNumLinks = otherBorder->getNumberOfLinks();
   for (int i = 0; i < myNumLinks; i++) {
      for (int j = 0; j < otherNumLinks; j++) {
         if (MathUtilities::distanceSquared3D(getLinkXYZ(i),
                                              otherBorder->getLinkXYZ(j))
               < intersectionToleranceSquared) {
            myLinkIntersect = i;
            otherLinkIntersect = j;
            return true;
         }
      }
   }
   
   return false;
}
      
/**
 * find links where another border intersects this border (2D borders in X-Y plane).
 */
bool 
Border::intersection2D(const Border* otherBorder, 
                       const bool thisBorderIsClosed,
                       const bool otherBorderIsClosed,
                       int& myLinkIntersect, 
                       int& otherLinkIntersect) const
{
   myLinkIntersect    = -1;
   otherLinkIntersect = -1;
   
   const int myNumLinks = getNumberOfLinks();
   const int otherNumLinks = otherBorder->getNumberOfLinks();
   float intersection[2];
   
   for (int i = 0; i < myNumLinks; i++) {
      int iNext = i + 1;
      if (i == (myNumLinks - 1)) {
         if (thisBorderIsClosed) {
            iNext = 0;
         }
         else {
            continue;
         }
      }
      for (int j = 0; j < (otherNumLinks - 1); j++) {
         int jNext = j + 1;
         if (j == (otherNumLinks - 1)) {
            if (otherBorderIsClosed) {
               jNext = 0;
            }
            else {
               continue;
            }
         }
         if (MathUtilities::lineIntersection2D(getLinkXYZ(i), 
                                               getLinkXYZ(iNext),
                                               otherBorder->getLinkXYZ(j), 
                                               otherBorder->getLinkXYZ(jNext),
                                               intersection)) {
            myLinkIntersect    = i;
            otherLinkIntersect = j;
            return true;
         }
      }
   }
   
   return false;
}      

/**
 * create a border with a subset of the border's links from linkStart to
 * linkEnd (inclusive).
 */
Border* 
Border::getSubSet(const int linkStart, const int linkEnd) const
{
   Border* b = new Border(name, center, samplingDensity, variance, 
                          topographyValue, arealUncertainty);
   if (linkEnd > linkStart) {
      for (int i = linkStart; i <= linkEnd; i++) {
         b->addBorderLink(getLinkXYZ(i), getLinkSectionNumber(i));
      }
   }
   else {
      const int numLinks = getNumberOfLinks();
      for (int i = linkStart; i < numLinks; i++) {
         b->addBorderLink(getLinkXYZ(i), getLinkSectionNumber(i));
      }
      for (int i = 0; i <= linkEnd; i++) {
         b->addBorderLink(getLinkXYZ(i), getLinkSectionNumber(i));
      }
   }
   
   if (b->getNumberOfLinks() > 0) {
      return b;
   }
   else {
      delete b;
      return NULL;
   }
}      

/**
 * Set the flat normal for a link.
 */
void
Border::setLinkFlatNormal(const int linkNumber, const float normal[3])
{
   linkFlatNormal[linkNumber * 2]     = normal[0];
   linkFlatNormal[linkNumber * 2 + 1] = normal[1];
   linkFlatNormal[linkNumber * 2 + 2] = normal[2];
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * Set the section number of a link.
 */
void
Border::setLinkSectionNumber(const int linkNumber, const int section)
{
   linkSection[linkNumber] = section;
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * Distance between two links.
 */
float
Border::distanceBetweenLinks(const int link1, const int link2) const
{
   const float dx = linkXYZ[link1 * 3]     - linkXYZ[link2 * 3];
   const float dy = linkXYZ[link1 * 3 + 1] - linkXYZ[link2 * 3 + 1];
   const float dz = linkXYZ[link1 * 3 + 2] - linkXYZ[link2 * 3 + 2];
   const float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
   return dist;
}

/** 
 * Add a link to the border.
 */
void
Border::addBorderLink(const float xyz[3],
                      const int section, 
                      const float radius)
{
   //
   // Note: Links are placed at origin when they cannot be projected successfully so ignore them
   //
   if ((xyz[0] != 0.0) || (xyz[1] != 0.0) || (xyz[2] != 0.0)) {
      linkXYZ.push_back(xyz[0]);
      linkXYZ.push_back(xyz[1]);
      linkXYZ.push_back(xyz[2]);
      linkRadii.push_back(radius);
      linkFlatNormal.push_back(0.0);
      linkFlatNormal.push_back(0.0);
      linkFlatNormal.push_back(1.0);
      linkSection.push_back(section);
      if (borderFile != NULL) {
         borderFile->setModified();
      }
   }
}

/**
 * insert a border link before the specified link number (use number of links for end).
 */
void 
Border::insertBorderLink(const int linkIndex,
                         const float xyz[3], 
                         const int section, 
                         const float radius)
{
   if (linkIndex >= getNumberOfLinks()) {
      addBorderLink(xyz, section, radius);
   }
   else if (linkIndex >= 0) {
      const int offset3 = linkIndex * 3;
      linkXYZ.insert(linkXYZ.begin() + offset3, xyz[2]);
      linkXYZ.insert(linkXYZ.begin() + offset3, xyz[1]);
      linkXYZ.insert(linkXYZ.begin() + offset3, xyz[0]);
      linkRadii.insert(linkRadii.begin() + linkIndex, radius);
      linkSection.insert(linkSection.begin() + linkIndex, section);
      linkFlatNormal.insert(linkFlatNormal.begin() + offset3, 0.0);
      linkFlatNormal.insert(linkFlatNormal.begin() + offset3, 0.0);
      linkFlatNormal.insert(linkFlatNormal.begin() + offset3, 1.0);
      if (borderFile != NULL) {
         borderFile->setModified();
      }
   }
}

/** 
 * Get the length of border.
 */
float
Border::getBorderLength() const
{
   float len = 0;
   const int numLinks = getNumberOfLinks();
   for (int i = 1; i < numLinks; i++) {
      len += distanceBetweenLinks(i, i - 1);
   }
   return len;
}

/**
 * Clear border links
 */
void
Border::clearLinks()
{
   linkXYZ.clear();
   linkRadii.clear();
   linkFlatNormal.clear();
   linkSection.clear();
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * remove links on negative side of plane.
 */
void 
Border::removePointsOnNegativeSideOfPlane(const float planeNormal[3],
                                          const float pointInPlane[3])
{
   const int num = getNumberOfLinks();
   for (int i = (num - 1); i >= 0; i--) {
      const float signedDistance = 
         MathUtilities::signedDistanceToPlane(planeNormal,
                                              pointInPlane,
                                              getLinkXYZ(i));
      if (signedDistance < 0.0) {
         removeLink(i);
      }
   }
}
      

/**
 * Remove a border link
 */
void
Border::removeLink(const int linkNumber)
{
   if (linkNumber < getNumberOfLinks()) {
      const int offset3 = linkNumber * 3;
      linkXYZ.erase(linkXYZ.begin() + offset3, linkXYZ.begin() + offset3 + 3);
      const int offset2 = linkNumber * 3;
      linkFlatNormal.erase(linkFlatNormal.begin() + offset2, linkFlatNormal.begin() + offset2 + 3);
      linkSection.erase(linkSection.begin() + linkNumber);
      linkRadii.erase(linkRadii.begin() + linkNumber);
      if (borderFile != NULL) {
         borderFile->setModified();
      }
   }
}

/**
 * remove intersecting loops in a border.
 */
void 
Border::removeIntersectingLoops(const char axisXYZ) throw (FileException)
{
   if ((axisXYZ != 'X') &&
       (axisXYZ != 'Y') &&
       (axisXYZ != 'Z')) {
      throw FileException("PROGRAM ERROR axis passed to \"Border::removeIntersectingLoops\""
                          "must be one of X, Y, or Z");
   }
   
   bool done = false;
   while (done == false) {
      done = true;
      const int numLinks = getNumberOfLinks();
      
      //
      // Loop through links
      //
      bool foundIntersectionFlag = false;
      for (int i = 0; i < (numLinks - 3); i++) {    
         const float* p13D = getLinkXYZ(i);
         const float* p23D = getLinkXYZ(i + 1);     
         
         //
         // Loop through links after "i"
         //
         for (int j = i + 2; j < (numLinks - 1); j++) {
            //
            // Do not test first and last
            //
            if ((i == 0) &&
                (j == (numLinks - 2))) {
               continue;
            }
            
            const float* q13D = getLinkXYZ(j);
            const float* q23D = getLinkXYZ(j + 1);
            
            //
            // Convert 3D to Planar 2D
            //
            float p1[2], p2[2], q1[2], q2[2];
            switch (axisXYZ) {
               case 'X':
                  p1[0] = p13D[1];
                  p1[1] = p13D[2];
                  p2[0] = p23D[1];
                  p2[1] = p23D[2];
                  q1[0] = q13D[1];
                  q1[1] = q13D[2];
                  q2[0] = q23D[1];
                  q2[1] = q23D[2];
                  break;
               case 'Y':
                  p1[0] = p13D[0];
                  p1[1] = p13D[2];
                  p2[0] = p23D[0];
                  p2[1] = p23D[2];
                  q1[0] = q13D[0];
                  q1[1] = q13D[2];
                  q2[0] = q23D[0];
                  q2[1] = q23D[2];
                  break;
               case 'Z':
                  p1[0] = p13D[0];
                  p1[1] = p13D[1];
                  p2[0] = p23D[0];
                  p2[1] = p23D[1];
                  q1[0] = q13D[0];
                  q1[1] = q13D[1];
                  q2[0] = q23D[0];
                  q2[1] = q23D[1];
                  break;
            }
            
            //
            // Do the segments intersect?
            //
            float intersection[2];
            if (MathUtilities::lineIntersection2D(p1, p2, q1, q2, intersection)) {
               //
               // Remove links between intersection
               //
               for (int k = j; k >= (i + 1); k--) {
                  removeLink(k);
               }
               foundIntersectionFlag = true;
               done = false;
               break;
            }
         } // for j...
         
         if (foundIntersectionFlag) {
            break;
         }
      } // for i...
   }
}
      
/**
 * smooth the border links.
 */
void 
Border::smoothBorderLinks(const int numberOfIterations,
                          const bool closedBorderFlag,
                          const std::vector<bool>* smoothTheseLinksOnlyIn)
{
   const int numLinks = getNumberOfLinks();
   if (numLinks <= 2) {
      return;
   }
   std::vector<bool> smoothFlags(numLinks, true);
   if (smoothTheseLinksOnlyIn != NULL) {
      if (static_cast<int>(smoothTheseLinksOnlyIn->size()) != numLinks) {
         return;
      }
      smoothFlags = *smoothTheseLinksOnlyIn;
   }
   
   int jStart = 1;
   int jEnd   = numLinks - 1;
   if (closedBorderFlag) {
      jStart = 0;
      jEnd = numLinks;
   }
  
   if (DebugControl::getDebugOn()) {
      std::cout << "Smoothing: ";
      for (int j = jStart; j < jEnd; j++) {
         if (smoothFlags[j]) {
            std::cout << j << " ";
         }
      }
      std::cout << std::endl;
   }
   
   for (int i = 0; i < numberOfIterations; i++) {
      for (int j = jStart; j < jEnd; j++) {
         if (smoothFlags[j]) {
            int jPrev = j - 1;
            if (jPrev < 0) {
               jPrev = numLinks - 1;
            }
            int jNext = j + 1;
            if (jNext >= numLinks) {
               jNext = 0;
            }
            
            const float* xyzPrev = getLinkXYZ(jPrev);
            const float* xyzNext = getLinkXYZ(jNext);
            float xyz[3];
            getLinkXYZ(j, xyz);
            for (int k = 0; k < 3; k++) {
               xyz[k] = (xyzPrev[k] + xyzNext[k] + xyz[k]) / 3.0;
            }
            
            setLinkXYZ(j, xyz);
         }
      }
   }
}
                             
/** 
 * Resample a border to a specified density.
 */
void
Border::resampleBorderToDensity(const float densityIn,
                                const int minimumNumberOfLinks,
                                int& newNumberOfLinks)
{
   float density = densityIn;
   const float borderLength = getBorderLength();
   newNumberOfLinks = static_cast<int>(borderLength / density) + 1;
   if (minimumNumberOfLinks > 1) {
      if (newNumberOfLinks < minimumNumberOfLinks) {
         newNumberOfLinks = minimumNumberOfLinks;
         //density = borderLength / (minimumNumberOfLinks - 1);
      }
   }
   
   // recompute density so that starting and ending points of border are used
   density = borderLength / (newNumberOfLinks - 1);
   
   const int oldNumberOfLinks = static_cast<int>(getNumberOfLinks());
   
   float* x1 = new float[oldNumberOfLinks];
   float* y1 = new float[oldNumberOfLinks];
   float* z1 = new float[oldNumberOfLinks];
   float* x2 = new float[newNumberOfLinks];
   float* y2 = new float[newNumberOfLinks];
   float* z2 = new float[newNumberOfLinks];
   
   for (int j = 0; j < oldNumberOfLinks; j++) {
      const float* xyz = getLinkXYZ(j);
      x1[j] = xyz[0];
      y1[j] = xyz[1];
      z1[j] = xyz[2];
   }
   
   resampleBorder(x1, y1, z1, oldNumberOfLinks, density,
                  x2, y2, z2, newNumberOfLinks);
   
   Border savedCopy = *this;
   clearLinks();
   for (int i = 0; i < newNumberOfLinks; i++) {
      float xyz[3] = { x2[i], y2[i], z2[i] };
      addBorderLink(xyz, 0, savedCopy.getLinkRadius(
                              savedCopy.getLinkNumberNearestToCoordinate(xyz)));
   }
   
   delete[] x1;
   delete[] y1;
   delete[] z1;
   delete[] x2;
   delete[] y2;
   delete[] z2;
   
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/** 
 * Resample a border to a specified number of links.
 */
void
Border::resampleBorderToNumberOfLinks(const int newNumberOfLinks)
{
   const float borderLength = getBorderLength();
   const int oldNumberOfLinks = static_cast<int>(getNumberOfLinks());
   const float density = borderLength / static_cast<float>(newNumberOfLinks - 1);
   
   float* x1 = new float[oldNumberOfLinks];
   float* y1 = new float[oldNumberOfLinks];
   float* z1 = new float[oldNumberOfLinks];
   float* x2 = new float[newNumberOfLinks];
   float* y2 = new float[newNumberOfLinks];
   float* z2 = new float[newNumberOfLinks];
   
   for (int j = 0; j < oldNumberOfLinks; j++) {
      const float* xyz = getLinkXYZ(j);
      x1[j] = xyz[0];
      y1[j] = xyz[1];
      z1[j] = xyz[2];
   }
   
   resampleBorder(x1, y1, z1, oldNumberOfLinks, density, 
                  x2, y2, z2, newNumberOfLinks);
   
   Border savedCopy = *this;
   clearLinks();
   for (int i = 0; i < newNumberOfLinks; i++) {
      const float xyz[3] = { x2[i], y2[i], z2[i] };
      addBorderLink(xyz, 0, savedCopy.getLinkRadius(
                              savedCopy.getLinkNumberNearestToCoordinate(xyz)));
   }
   
   delete[] x1;
   delete[] y1;
   delete[] z1;
   delete[] x2;
   delete[] y2;
   delete[] z2;
   
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * orient the links counter-clockwise.
 */
void 
Border::orientLinksCounterClockwise()
{
   orientLinksClockwise();
   reverseBorderLinks();
}      

/**
 * Orient the links in the border clockwise.
 */
void
Border::orientLinksClockwise()
{
   float minX = std::numeric_limits<float>::max();
   int minXIndex = -1;
   
   const int num = getNumberOfLinks();
   for (int i = 0; i < num; i++) {
      const float *pos = getLinkXYZ(i);
      if (i == 0) {
         minX = pos[0];
         minXIndex = i;
      }
      else if (pos[0] < minX) {
         minX = pos[0];
         minXIndex = i;
      }
   }
   
   if (minXIndex >= 0) {
      int iNext = minXIndex + 1;
      if (iNext >= num) {
         iNext = 0;
      }
      
      const float *nextPos = getLinkXYZ(iNext);
      const float *minPos  = getLinkXYZ(minXIndex);
      if (nextPos[1] < minPos[1]) {
         reverseBorderLinks();
      }
   }
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * Compute the flat normals for a border.
 */
void
Border::computeFlatNormals()
{
   const int numLinks = getNumberOfLinks();
   for (int i = 0; i < numLinks; i++) {
      int iNext = i + 1;
      if (iNext >= numLinks) iNext = 0;
      int iPrev = i - 1;
      if (iPrev < 0) iPrev = numLinks - 1;
      
      float vec[3];
      MathUtilities::subtractVectors(getLinkXYZ(iNext), getLinkXYZ(iPrev), vec);
      MathUtilities::normalize(vec);
      
      // points 90 to right
      const int i3 = i * 3;
      linkFlatNormal[i3]   =  vec[1];
      linkFlatNormal[i3+1] = -vec[0];
      linkFlatNormal[i3+2] = 0.0;
   }
}

/** 
 * Reverse the order of the links in a border.
 */
void 
Border::reverseBorderLinks()
{
   std::vector<float> xyz;
   std::vector<float> normal;
   std::vector<int> section;
   std::vector<float> radii;
   
   for (int i = getNumberOfLinks() - 1; i >= 0; i--) {
      const float* pos = getLinkXYZ(i);
      const float* norm = getLinkFlatNormal(i);
      const int sect = getLinkSectionNumber(i);
      const float radius = getLinkRadius(i);
      
      xyz.push_back(pos[0]);
      xyz.push_back(pos[1]);
      xyz.push_back(pos[2]);
      radii.push_back(radius);
      normal.push_back(norm[0]);
      normal.push_back(norm[1]);
      normal.push_back(norm[2]);
      section.push_back(sect);
   }
   
   linkXYZ = xyz;
   linkRadii = radii;
   linkFlatNormal = normal;
   linkSection = section;
   
   if (borderFile != NULL) {
      borderFile->setModified();
   }
}

/**
 * remove landmark rater from name of border.
 */
void 
Border::removeLandmarkRaterInfoFromBorderName()
{
   const int landmarkChar = Border::getLandmarkRaterFirstChar();
   const int indx = name.indexOf(landmarkChar);
   if (indx >= 0) {
      name = name.left(indx);
   }
}

/**
 * compare the name of two landmark borders
 * only compares text before the semi-colon in the name.
 */
bool 
Border::compareLandmarkBorderNames(const QString& name1, const QString& name2)
{
   //
   // for comparison, use substring of name up to but not including the colon
   //
   const char landmarkChar = Border::getLandmarkRaterFirstChar();
   int name1Length = name1.indexOf(landmarkChar);
   if (name1Length < 0) {
      name1Length = name1.length();
   }
   int name2Length = name2.indexOf(landmarkChar);
   if (name2Length < 0) {
      name2Length = name2.length();
   }
   
   //
   // Only need to compare if same length
   //
   if (name1Length == name2Length) {
      return (name1.left(name1Length) == name2.left(name2Length));
   }
   return false;
}

/* distance between two points */
static double 
dist (const float x1, const float y1, const float z1, 
      const float x2, const float y2, const float z2)
{
   const float dxyz[3] = { x2 - x1,
                           y2 - y1,	
                           z2 - z1 };	
   return MathUtilities::vectorLength(dxyz);
}

/** 
 * Helpful resample function.
 */
void 
Border::resampleBorder(const float* xorig, const float* yorig, 
                       const float* zorig, 
                        const int numPointsIn, 
                       const float density,
                       float* xout, float* yout, float* zout,
                       const int numPointsOut)
{
   /* Now find the next point dist away from the previous point */
   xout [0] = xorig [0];
   yout [0] = yorig [0];
   zout [0] = zorig [0];

   int j = 0; /* index of the points processed */
   //05/07/01 for (int i = 0; i < numPointsOut-1; i++){
   for (int i = 0; i < numPointsOut-2; i++){
      float x1 = xout [i];
      float y1 = yout [i];
      float z1 = zout [i];

      float dj1 = 0;
      float dj = dist (x1, y1, z1, xorig [j], yorig [j], zorig [j]);
      int k = j;
      /* find the distance to the jth point */
      while (dj < density){
         dj1 = dj;
         dj = dj+dist (xorig [j], yorig [j], zorig [j],
                       xorig [j+1], yorig [j+1], zorig [j+1]);
         j++;
      }
      /* The new point lies in the jth segment */
      float frac = (density - dj1)/(dj-dj1);

      if (k != j) {
         x1 = xorig [j-1];
         y1 = yorig [j-1];
         z1 = zorig [j-1];
      }
      xout [i+1] = (1-frac)*x1 + frac*xorig[j];
      yout [i+1] = (1-frac)*y1 + frac*yorig[j];
      zout [i+1] = (1-frac)*z1 + frac*zorig[j];
   }
   
   // last point
   xout[numPointsOut-1] = xorig[numPointsIn-1];
   yout[numPointsOut-1] = yorig[numPointsIn-1];
   zout[numPointsOut-1] = zorig[numPointsIn-1];
}

/**
 * Determine the bounds of a border.
 */
void
Border::getBounds(float bounds[6]) const
{
   bounds[0] =  std::numeric_limits<float>::max();
   bounds[1] = -std::numeric_limits<float>::max();
   bounds[2] =  std::numeric_limits<float>::max();
   bounds[3] = -std::numeric_limits<float>::max();
   bounds[4] =  std::numeric_limits<float>::max();
   bounds[5] = -std::numeric_limits<float>::max();
   
   const int numLinks = getNumberOfLinks();
   for (int i = 0; i < numLinks; i++) {
      const float* pos = getLinkXYZ(i);
      bounds[0] = std::min(bounds[0], pos[0]);
      bounds[1] = std::max(bounds[1], pos[0]);
      bounds[2] = std::min(bounds[2], pos[1]);
      bounds[3] = std::max(bounds[3], pos[1]);
      bounds[4] = std::min(bounds[4], pos[2]);
      bounds[5] = std::max(bounds[5], pos[2]);
   }   
}

/**
 * get the center of gravity of the border (returns true if valid).
 */
bool 
Border::getCenterOfGravity(float cogXYZOut[3]) const
{
   double sum[3] = { 0.0, 0.0, 0.0 };
   double numSum = 0.0;
   
   const int numLinks = getNumberOfLinks();
   for (int i = 0; i < numLinks; i++) {
      const float* pos = getLinkXYZ(i);
      sum[0] += pos[0];
      sum[1] += pos[1];
      sum[2] += pos[2];
      numSum += 1.0;
   }
   
   if (numSum >= 1.0) {
      cogXYZOut[0] = sum[0] / numSum;
      cogXYZOut[1] = sum[1] / numSum;
      cogXYZOut[2] = sum[2] / numSum;
      return true;
   }
   
   return false;
}      

/**
 * get the link number nearest to a coordinate.
 */
int 
Border::getLinkNumberNearestToCoordinate(const float xyz[3]) const
{
   int nearestLinkNumber = -1;
   float nearestDistanceSQ = std::numeric_limits<float>::max();
   
   const int numLinks = getNumberOfLinks();
   for (int i = 0; i < numLinks; i++) {
      const float* pos = getLinkXYZ(i);
      const float distSQ = MathUtilities::distanceSquared3D(xyz, pos);
      if (distSQ < nearestDistanceSQ) {
         nearestDistanceSQ = distSQ;
         nearestLinkNumber = i;
      }
   }
   
   return nearestLinkNumber;
}
      
/**
 * get the link number furthest from a coordinate.
 */
int 
Border::getLinkNumberFurthestFromCoordinate(const float xyz[3]) const
{
   int furthestLinkNumber = -1;
   float furthestDistanceSQ = -1.0;
   
   const int numLinks = getNumberOfLinks();
   for (int i = 0; i < (numLinks - 1); i++) {
      const float* pos = getLinkXYZ(i);
      const float distSQ = MathUtilities::distanceSquared3D(xyz, pos);
      if (distSQ > furthestDistanceSQ) {
         furthestDistanceSQ = distSQ;
         furthestLinkNumber = i;
      }
   }
   
   return furthestLinkNumber;
}
      
/**
 * See if 3D points are inside a 3D border (transform all to screen axis).
 */
void 
Border::pointsInsideBorder3D(const GLdouble* modelMatrix,
                             const GLdouble* projectionMatrix,
                             const GLint* viewport,
                             const float* points, 
                             const int numPoints,
                             std::vector<bool>& insideFlags,
                             const bool checkNonNegativeScreenZPointsOnly) const
{
   if (static_cast<int>(insideFlags.size()) < numPoints) {
      insideFlags.resize(numPoints);
   }
   std::fill(insideFlags.begin(), insideFlags.end(), false);
   
   //
   // Since the coordinates are typically less than 100, this can cause
   // problems with the point in polygon algorithm (it will report a
   // degenerate polygon).  Scaling all coordinates elminates this problem.
   // Using every other point in the border also helps.
   //
   const float scaleFactor = 1000.0;

   const int numLinks = getNumberOfLinks();

   float bounds[6];
   bounds[0] =  std::numeric_limits<float>::max();
   bounds[1] = -std::numeric_limits<float>::max();
   bounds[2] =  std::numeric_limits<float>::max();
   bounds[3] = -std::numeric_limits<float>::max();
   bounds[4] = -1.0;
   bounds[5] =  1.0;
   
   //
   // Create a polygon consisting of the border links projection to screen space
   //
   std::vector<float> polygon;
   int numToSkip = 1;
   int numInPolygon = 0;
   for (int i = 0; i < (numLinks - 1); i++) {
      const float* pos = getLinkXYZ(i);
      GLdouble x = pos[0];
      GLdouble y = pos[1];
      GLdouble z = pos[2];
      GLdouble sx = 0.0, sy = 0.0, sz = 0.0;
      gluProject(x, y, z,
                 modelMatrix, projectionMatrix, viewport,
                 &sx, &sy, &sz);
      polygon.push_back(sx * scaleFactor);
      polygon.push_back(sy * scaleFactor);
      polygon.push_back(0.0); // test in plane so do not need Z
      numInPolygon++;
      i += numToSkip; 
      
      bounds[0] = std::min(bounds[0], static_cast<float>(sx * scaleFactor));
      bounds[1] = std::max(bounds[1], static_cast<float>(sx * scaleFactor));
      bounds[2] = std::min(bounds[2], static_cast<float>(sy * scaleFactor));
      bounds[3] = std::max(bounds[3], static_cast<float>(sy * scaleFactor));
   }
   if (numInPolygon < 3) {
      return;
   }
   
   float normal[3] = { 0.0, 0.0, 1.0 };
   
   for (int i = 0; i < numPoints; i++) {
      GLdouble x = points[i*3];
      GLdouble y = points[i*3+1];
      GLdouble z = points[i*3+2];
      GLdouble sx = 0.0, sy = 0.0, sz = 0.0;
      gluProject(x, y, z,
                 modelMatrix, projectionMatrix, viewport,
                 &sx, &sy, &sz);
      int result = 0;

      bool checkIt = true;
      if (checkNonNegativeScreenZPointsOnly) {
         if (sz >= 0.5) {  // Z ranges 0 to 1 with 0 closest to viewer
            checkIt = false;
         }
      }
      if (checkIt) {
         float xyz[3] = { sx * scaleFactor, sy * scaleFactor, 0.0 };  // use 0 for Z 
         result = MathUtilities::pointInPolygon(xyz,
                                             numInPolygon,
                                             (float*)&polygon[0],
                                             bounds,
                                             normal);
      }

      if (result > 0) {
         insideFlags[i] = true;
      }
      else if (result < 0)  {
         std::cerr << "Border polygon passed to MathUtilities::pointInPolygon "
                      " is degenerate." << std::endl;
         std::cerr << "Polygon: " << std::endl;
         for (int j = 0; j < numInPolygon; j++) {
            std::cerr << "   " 
                      << j << " "
                      << polygon[j*3] << " "
                      << polygon[j*3+1] << " "
                      << polygon[j*3+2] << std::endl;

         }
         break;
      }
   }
}

/**
 * Determine if points are inside this border.
 * The border is assumed to be flat, in the X-Y plane.
 */
void
Border::pointsInsideBorder2D(const float* points, const int numPoints,
                             std::vector<bool>& insideFlags,
                             const bool checkNonNegativeZPointsOnly,
                             const float zMinimum) const
{
   if (static_cast<int>(insideFlags.size()) < numPoints) {
      insideFlags.resize(numPoints);
   }
   std::fill(insideFlags.begin(), insideFlags.end(), false);
   
   //
   // Since the coordinates are typically less than 100, this can cause
   // problems with the point in polygon algorithm (it will report a
   // degenerate polygon).  Scaling all coordinates elminates this problem.
   // Using every other point in the border also helps.
   //
   const float scaleFactor = 1000.0;

   //
   // Copy the border and orient CCW
   //
   Border border = *this;
   const int numLinks = border.getNumberOfLinks();
   for (int i = 0; i < numLinks; i++) {
      float pos[3];
      border.getLinkXYZ(i, pos);
      pos[2] = 0.0;
      border.setLinkXYZ(i, pos);
   }
   border.orientLinksCounterClockwise();

   std::vector<float> polygon;
   int numToSkip = 1;
   int numInPolygon = 0;
   for (int i = 0; i < (numLinks - 1); i++) {
      const float* pos = border.getLinkXYZ(i);
      polygon.push_back(pos[0] * scaleFactor);
      polygon.push_back(pos[1] * scaleFactor);
      polygon.push_back(0.0); //JWH 5/14/2008 pos[2] * scaleFactor);
      numInPolygon++;
      i += numToSkip; 
   }
   if (numInPolygon < 3) {
      return;
   }
      
   float bounds[6];
   border.getBounds(bounds);
   bounds[0] *= scaleFactor - 1.0;
   bounds[1] *= scaleFactor + 1.0;
   bounds[2] *= scaleFactor - 1.0;
   bounds[3] *= scaleFactor + 1.0;
   bounds[4] = -1.0;
   bounds[5] =  1.0;
   
   float normal[3] = { 0.0, 0.0, 1.0 };
   
   for (int i = 0; i < numPoints; i++) {
      float xyz[3];
      xyz[0] = points[i*3];
      xyz[1] = points[i*3+1];
      xyz[2] = points[i*3+2];

      bool checkIt = true;
      if (checkNonNegativeZPointsOnly) {
         if (xyz[2] < zMinimum) {
            checkIt = false;
         }
      }

      int result = 0;
      if (checkIt) {
         xyz[0] *= scaleFactor;
         xyz[1] *= scaleFactor;
         xyz[2]  = 0.0;
         result = MathUtilities::pointInPolygon(xyz,
                                             numInPolygon,
                                             (float*)&polygon[0],
                                             bounds,
                                             normal);
      }

      if (result > 0) {
         insideFlags[i] = true;
      }
      else if (result < 0)  {
         std::cerr << "Border polygon passed to MathUtilities::pointInPolygon "
                      " is degenerate." << std::endl;
         std::cerr << "Polygon: " << std::endl;
         for (int j = 0; j < numInPolygon; j++) {
            std::cerr << "   " 
                      << j << " "
                      << polygon[j*3] << " "
                      << polygon[j*3+1] << " "
                      << polygon[j*3+2] << std::endl;

         }
         break;
      }
   }
}

/**
 * interpolate two borders to create new borders.
 */
void 
Border::createInterpolatedBorders(Border* b1,
                                  Border* b2,
                                  const QString& namePrefix,
                                  const int numberOfNewBorders,
                                  const float sampling,
                                  std::vector<Border*>& outputInterpolatedBorders) throw (FileException)
{
   outputInterpolatedBorders.clear();
   
   //
   // Check inputs
   //
   if (b1 == NULL) {
      throw FileException("Border 1 is invalid.");
   }
   if (b2 == NULL) {
      throw FileException("Border 2 is invalid.");
   }
   if (b1->getNumberOfLinks() < 2) {
      throw FileException("Border 1 must have at least two links.");
   }
   if (b2->getNumberOfLinks() < 2) {
      throw FileException("Border 2 must have at least two links.");
   }
   if (numberOfNewBorders < 1) {
      throw FileException("Number of new borders must be at least one.");
   }
   if (sampling <= 0.0) {
      throw FileException("Sampling must be greater than zero.");
   }
   if (namePrefix.isEmpty()) {
      throw FileException("Name prefix contains no characters.");
   }
   
   //
   // resample the longest border to the specified sampling and resample the other
   // border to the same number of links
   //
   Border* longestBorder = NULL;
   Border* shortestBorder = NULL;
   if (b1->getBorderLength() > b2->getBorderLength()) {
      longestBorder  = b1;
      shortestBorder = b2;
   }
   else {
      shortestBorder = b1;
      longestBorder  = b2;
   }
   
   //
   // Resample the borders
   //
   int numLinks = 0;
   longestBorder->resampleBorderToDensity(sampling, 2, numLinks);
   if (numLinks < 2) {
      throw FileException("PROGRAM ERROR: Resampling of first border resulted in less than two links.");
   }
   shortestBorder->resampleBorderToNumberOfLinks(numLinks);
   if (b1->getNumberOfLinks() != b2->getNumberOfLinks()) {
      throw FileException("PROGRAM ERROR: After resampling the borders have a different number of links.");
   }
   
   //
   // Create the specified number of interpolated borders
   //
   const float totalSteps = numberOfNewBorders + 1;
   for (int i = 0; i < numberOfNewBorders; i++) {
      const float stepNum = i + 1;
      
      //
      // Create the new border
      //
      const QString borderName(namePrefix + "." + QString::number(i + 1));
      Border* newBorder = new Border(borderName);
      
      //
      // Create and add new links to new border
      //
      const float stepDelta = stepNum / totalSteps;
      for (int j = 0; j < numLinks; j++) {
         //
         // Determine the offset from link in first border to corresponding link in second border
         //
         const float* b1XYZ = b1->getLinkXYZ(j);
         const float* b2XYZ = b2->getLinkXYZ(j);
         const float dxyz[3] = { 
            b2XYZ[0] - b1XYZ[0],
            b2XYZ[1] - b1XYZ[1],
            b2XYZ[2] - b1XYZ[2]
         };
         
         //
         // Create XYZ of point in new border
         //
         const float xyz[3] = {
            b1XYZ[0] + dxyz[0] * stepDelta,
            b1XYZ[1] + dxyz[1] * stepDelta,
            b1XYZ[2] + dxyz[2] * stepDelta
         };
         
         //
         // Add a link to the new border
         //
         newBorder->addBorderLink(xyz);
      }
      
      //
      // Add the new border
      //
      outputInterpolatedBorders.push_back(newBorder);
   }
   
}
                                     
//--------------------------------------------------------------------------------
/** 
 * Constructor.
 */
BorderFile::BorderFile(const QString& descriptiveName,
                       const QString& defaultExtensionIn)
   : AbstractFile(descriptiveName, defaultExtensionIn)
{
   clear();
}

/**
 * Constructor that creates a BorderFile from the coordinates in a coordinate file.
 * If "maxLinksPerBorder" is negative one border containing all coordinates will
 * be created.  Otherwise, multiple borders will be created no more than
 * "maxLinksPerBorder" points.
 */
BorderFile::BorderFile(const CoordinateFile* cf, const int maxLinksPerBorder)
   : AbstractFile("Border File", SpecFile::getBorderFileExtension())
{
   if (cf == NULL) {
      return;
   }
   
   const int numCoords = cf->getNumberOfCoordinates();
   if (maxLinksPerBorder > 0) {
      Border border("Nodes");
      for (int i = 0; i < numCoords; i++) {
         border.addBorderLink(cf->getCoordinate(i));
         if (border.getNumberOfLinks() >= maxLinksPerBorder) {
            addBorder(border);
            border.clearLinks();
         }
      }
      if (border.getNumberOfLinks() > 0) {
         addBorder(border);
      }
   }
   else {
      Border border("Nodes");
      for (int i = 0; i < numCoords; i++) {
         border.addBorderLink(cf->getCoordinate(i));
      }
      addBorder(border);
   }
}

/**
 * Constructor that creates a BorderFile from the tiles in a Topology and Coordinate File.
 */
BorderFile::BorderFile(const TopologyFile* tf, const CoordinateFile* cf)
   : AbstractFile("Border File", SpecFile::getBorderFileExtension())
{
   //
   // Get topology and coordinate information
   //
   if ((tf == NULL) || (cf == NULL)) {
      return;
   }
   const int numTiles = tf->getNumberOfTiles();
   
   for (int i = 0; i < numTiles; i++) {
      //
      // Get the nodes in the tile
      //
      int p1, p2, p3;
      tf->getTile(i, p1, p2, p3);
      
      //
      // Create a closed border with the tiles vertices
      //
      Border b("Tile");
      b.addBorderLink(cf->getCoordinate(p1));
      b.addBorderLink(cf->getCoordinate(p2));
      b.addBorderLink(cf->getCoordinate(p3));
      b.addBorderLink(cf->getCoordinate(p1));  // closes the border
      
      addBorder(b);
   }
}

/**
 * copy all the links into a coordinate file.
 */
void 
BorderFile::copyLinksToCoordinateFile(CoordinateFile* cf)
{
   //
   // Set the number of coordinates
   //
   cf->setNumberOfCoordinates(getTotalNumberOfLinks());
   
   //
   // Set the coordinates with the links
   //
   int ctr = 0;
   for (int i = 0; i < getNumberOfBorders(); i++) {
      Border* border = getBorder(i);
      const int numLinks = border->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         float xyz[3];
         border->getLinkXYZ(j, xyz);
         cf->setCoordinate(ctr, xyz);
         ctr++;
      }
   }
}

/** 
 * Destructor.
 */
BorderFile::~BorderFile()
{
   clear();
}

/**
 * Append a border file to this one.
 */
void
BorderFile::append(BorderFile& bf)
{
   const int numProj = bf.getNumberOfBorders();
   for (int i = 0; i < numProj; i++) {
      Border* b = bf.getBorder(i);
      addBorder(*b);
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(bf);

}

/**
 * Assign colors to these borders.
 */
void 
BorderFile::assignColors(const ColorFile& colorFile)
{
   const int numProj = getNumberOfBorders();
   for (int i = 0; i < numProj; i++) {
      Border* b = getBorder(i);
      bool match;
      b->setBorderColorIndex(colorFile.getColorIndexByName(b->getName(), match));
   }
}

/**
 * resample to match landmark border file (borders with same names have same number of points).
 */
void 
BorderFile::resampleToMatchLandmarkBorders(const BorderFile& landmarkBorderFile) throw (FileException)
{
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      Border* myBorder = getBorder(i);
      const QString name = myBorder->getName();
      
      //
      // Find matching landmark border
      //
      const Border* landmarkBorder = landmarkBorderFile.getBorderByName(name, true);
      if (landmarkBorder == NULL) {
         throw FileException("Matching landmark border not found for border named "
                             + name);
      }
      
      //
      // resample this border to the same number of links as landmark border
      //
      const int numLinks = landmarkBorder->getNumberOfLinks();
      myBorder->resampleBorderToNumberOfLinks(numLinks);
      
      //
      // Set variability ratio for this border
      //
      for (int j = 0; j < numLinks; j++) {
         const float dist = MathUtilities::distance3D(myBorder->getLinkXYZ(j),
                                                      landmarkBorder->getLinkXYZ(j));
         const float landmarkVariability = landmarkBorder->getLinkRadius(j);
         float ratio = 0.0;
         if (landmarkVariability != 0.0) {
            ratio = dist / landmarkVariability;
         }
         myBorder->setLinkRadius(j, ratio);
      }
   }
}

/**
 * compute landmark variability.
 */
void 
BorderFile::evaluateLandmarkVariability(const BorderFile& indivBorderFile,
                                        const BorderFile& atlasBorderFile,
                                        const float badThreshold,
                                        const float extremeThreshold,
                                        const bool useAbsoluteDistanceFlag,
                                        BorderFile& outputBorderFile,
                                        QString& outputTextReport) throw (FileException)
{
   outputBorderFile.clear();
   outputTextReport = "";
   
   //
   // Copy and resample the atlas border
   //
   BorderFile resampledAtlasBorderFile(atlasBorderFile);
   resampledAtlasBorderFile.resampleAllBorders(0.25);
   
   //
   // Set individual border variability
   //
   int longestBorderNameLength = 0;
   int numIndivBorders = indivBorderFile.getNumberOfBorders();
   for (int i = 0; i < numIndivBorders; i++) {
      //
      // Copy indiv border and find atlas border with same name
      //
      Border indivBorder = *(indivBorderFile.getBorder(i));
      const Border* atlasBorder = resampledAtlasBorderFile.getBorderByName(indivBorder.getName());
      if (atlasBorder == NULL) {
         throw FileException("No atlas border named " 
                             + indivBorder.getName()
                             + " found.");
      }
      
      //
      // Track longest border name length
      //
      longestBorderNameLength = std::max(longestBorderNameLength, 
                                         indivBorder.getName().length());
                                         
      //
      // Loop through indiv border points
      //
      int numLinks = indivBorder.getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         //
         // Find nearest atlas link to indiv link
         //
         const float* xyz = indivBorder.getLinkXYZ(j);
         const int atlasLinkNumber = atlasBorder->getLinkNumberNearestToCoordinate(xyz);
         if (atlasLinkNumber >= 0) {
            //
            // Compute and set indiv variability
            //
            const float distance = MathUtilities::distance3D(xyz, 
                                      atlasBorder->getLinkXYZ(atlasLinkNumber));
            if (useAbsoluteDistanceFlag) {
               indivBorder.setLinkRadius(j, distance);
            }
            else {
               float atlasVariability = atlasBorder->getLinkRadius(atlasLinkNumber);
               if (atlasVariability <= 0.0) {
                  atlasVariability = 1.0;
               }
               const float indivVariability = distance / atlasVariability;
               indivBorder.setLinkRadius(j, indivVariability);
            }
         }
         else {
            throw FileException("No nearest point in atlas border named "
                                + atlasBorder->getName());
         }
      }
      
      //
      // Add indiv border to output border file
      //
      outputBorderFile.addBorder(indivBorder);
   }
   
   //
   // Copy and resample the atlas border
   //
   BorderFile resampledIndivBorderFile(indivBorderFile);
   resampledIndivBorderFile.resampleAllBorders(0.25);
   
   //
   // Set atlas border variability
   //
   int numAtlasBorders = atlasBorderFile.getNumberOfBorders();
   for (int i = 0; i < numAtlasBorders; i++) {
      //
      // Copy atlas border and find indiv border with same name
      //
      Border atlasBorder = *(atlasBorderFile.getBorder(i));
      const Border* indivBorder = resampledIndivBorderFile.getBorderByName(atlasBorder.getName());
      if (indivBorder == NULL) {
         throw FileException("No indiv border named " 
                             + atlasBorder.getName()
                             + " found.");
      }
      
      //
      // Loop through atlas border points
      //
      int numLinks = atlasBorder.getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         //
         // Find nearest indiv link to atlas link
         //
         const float* xyz = atlasBorder.getLinkXYZ(j);
         const int indivLinkNumber = indivBorder->getLinkNumberNearestToCoordinate(xyz);
         if (indivLinkNumber >= 0) {
            //
            // Compute and set atlas variability which is distance from
            // indiv to atlas divided by ATLAS (yes ATLAS) variability
            //
            const float distance = MathUtilities::distance3D(xyz, 
                                      indivBorder->getLinkXYZ(indivLinkNumber));
            if (useAbsoluteDistanceFlag) {
               atlasBorder.setLinkRadius(j, distance);
            }
            else {
               float atlasVariability = atlasBorder.getLinkRadius(j);
               if (atlasVariability <= 0.0) {
                  atlasVariability = 1.0;
               }
               atlasVariability = distance / atlasVariability;
               atlasBorder.setLinkRadius(j, atlasVariability);
            }
         }
         else {
            throw FileException("No nearest point in indiv border named "
                                + indivBorder->getName());
         }
      }
      
      //
      // Add atlas border to output border file
      //
      outputBorderFile.addBorder(atlasBorder);
   }
   
   //
   // Header for text report
   //
   const QString borderNameHeader("Border Name");
   const QString badCountHeader("Bad Count");
   const int badCountLength = badCountHeader.length();
   const QString extremeCountHeader("Extreme Count");
   const int extremeCountLength = extremeCountHeader.length();
   const QString overlapPercentageHeader("Overlap Percentage");
   const int overlapPercentageLength = overlapPercentageHeader.length();
   longestBorderNameLength = std::max(longestBorderNameLength,
                                      borderNameHeader.length());
   QString s =
      QString("%1  %2  %3  %4\n").arg(borderNameHeader, longestBorderNameLength)
                              .arg(badCountHeader, badCountLength)
                              .arg(extremeCountHeader, extremeCountLength)
                              .arg(overlapPercentageHeader, overlapPercentageLength);
   outputTextReport.append(s);
   
   //
   // Loop through indiv borders in output border file
   //
   int totalNumberOfBorders = outputBorderFile.getNumberOfBorders();
   for (int i = 0; i < numIndivBorders; i++) {
      Border* indivBorder = outputBorderFile.getBorder(i);
      const QString name = indivBorder->getName();
      
      //
      // Find corresponding atlas border
      //
      Border* atlasBorder = NULL;
      for (int j = numIndivBorders; j < totalNumberOfBorders; j++) {
         Border* b = outputBorderFile.getBorder(j);
         if (b->getName() == name) {
            atlasBorder = b;
            break;
         }
      }
      if (atlasBorder == NULL) {
         throw FileException("Unable to find atlas border named "
                             + name
                             + " in output border file. ");
      }
      
      //
      // Count bad and extreme points in atlas and indiv borders
      //
      int badCount = 0;
      int extremeCount = 0;
      for (int j = 0; j < indivBorder->getNumberOfLinks(); j++) {
         if (indivBorder->getLinkRadius(j) > badThreshold) {
            badCount++;
         }
         if (indivBorder->getLinkRadius(j) > extremeThreshold) {
            extremeCount++;
         }
      }
      for (int j = 0; j < atlasBorder->getNumberOfLinks(); j++) {
         if (atlasBorder->getLinkRadius(j) > badThreshold) {
            badCount++;
         }
         if (atlasBorder->getLinkRadius(j) > extremeThreshold) {
            extremeCount++;
         }
      }
      
      //
      // Determine bad and extreme counts
      //
      const int totalPoints = indivBorder->getNumberOfLinks()
                            + atlasBorder->getNumberOfLinks();
      const float overlapPercentage = 
         (static_cast<float>(totalPoints - badCount)
          / static_cast<float>(totalPoints))
         * 100.0;
         
      //
      // Add to text report
      //
      QString s =
         QString("%1  %2  %3  %4\n").arg(name, longestBorderNameLength)
                                    .arg(badCount, badCountLength)
                                    .arg(extremeCount, extremeCountLength)
                                    .arg(overlapPercentage, overlapPercentageLength, 'f', 1);
      outputTextReport.append(s);
      
      //
      // Change the atlas border name
      //
      atlasBorder->setName("ATLAS_" + name);
   }
}
                                       
/**
 * Create a border file that is an average of a group of border files.
 * Returns NULL if error.
 */
void
BorderFile::createAverageBorderFile(const std::vector<BorderFile*>& inputBorderFiles,
                                    const float borderResampling,
                                    const bool sphereFlag,
                                    BorderFile& averageBorderFileOut)
                                               throw (FileException)
{
   //
   // Need at least two borders files to resample
   //
   if (inputBorderFiles.size() < 2) {
      throw FileException("There must be at least two border files.");
   }
   
   //
   // Clear the output border file
   //
   averageBorderFileOut.clear();
   
   //
   // Stuff for spherical borders
   //
   float sphereRadius = 1.0;
   bool sphereRadiusSet = false;
   
   //
   // Number of border files we have
   //
   const int numFiles = static_cast<int>(inputBorderFiles.size());
   
   //
   // Use the first border files and loop through its borders
   //
   const int numBorders = inputBorderFiles[0]->getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      //
      // Get a border from the first tile
      //
      Border b1 = *(inputBorderFiles[0]->getBorder(i));
      
      //
      // Set sphere radius
      //
      if (sphereRadiusSet == false) {
         for (int j = 0; j < b1.getNumberOfLinks(); j++) {
            float xyz[3];
            b1.getLinkXYZ(j, xyz);
            if ((xyz[0] != 0.0) || (xyz[1] != 0.0) || (xyz[2] != 0.0)) {
               sphereRadius = std::sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]);
               sphereRadiusSet = true;
               break;
            }
         }
      }
      
      //
      // Resample it to the specified amount
      //
      int numLinks = 0;
      b1.resampleBorderToDensity(borderResampling, 2, numLinks);
      
      if (numLinks > 0) {
         //
         // Will contain the additional resampled borders
         //
         std::vector<Border> resampledBorders;
         
         //
         // Loop through the remaining files
         //
         for (int j = 1; j < numFiles; j++) {
            //
            // Find the border with the same name as from first file
            //
            Border* bj = inputBorderFiles[j]->getBorderByName(b1.getName(), true);
            if (bj != NULL) {
               //
               // Resmaple it and add to resampled borders
               //
               Border b = *bj;
               b.removeLandmarkRaterInfoFromBorderName();
               b.resampleBorderToNumberOfLinks(numLinks);
               resampledBorders.push_back(b);
            }
            else {
               //
               // bummer
               //
               QString errorMessage;
               errorMessage = "Unable to find border named ";
               errorMessage.append(b1.getName());
               errorMessage.append("in file ");
               errorMessage.append(inputBorderFiles[j]->getFileName());
               throw FileException(errorMessage);
            }
         }
      
         //
         // Compute the average at each link
         //
         const int numLinks = b1.getNumberOfLinks();
         for (int j = 0; j < numLinks; j++) {
            //
            // Position in first border file
            //
            float xyzSum[3] = { 0.0, 0.0, 0.0 };
            float xyz[3];
            b1.getLinkXYZ(j, xyz);
            xyzSum[0] += xyz[0];
            xyzSum[1] += xyz[1];
            xyzSum[2] += xyz[2];
            
            //
            // Add in positions of other resampled borders
            //
            const int numResampledBorders = static_cast<int>(resampledBorders.size());
            for (int k = 0; k < numResampledBorders; k++) {
               resampledBorders[k].getLinkXYZ(j, xyz);
               xyzSum[0] += xyz[0];
               xyzSum[1] += xyz[1];
               xyzSum[2] += xyz[2];
            }
            
            //
            // Average for this link
            //
            const float num = numResampledBorders + 1;
            const float average[3] = { 
               xyzSum[0] / num, 
               xyzSum[1] / num, 
               xyzSum[2] / num 
            };
            
            //
            // Compute mean distance from average
            //
            std::vector<float> distances;
            distances.push_back(MathUtilities::distance3D(b1.getLinkXYZ(j), average));
            for (int k = 0; k < numResampledBorders; k++) {
               distances.push_back(MathUtilities::distance3D(average, 
                                              resampledBorders[k].getLinkXYZ(j)));
            }
            
            StatisticDataGroup sdg(&distances,
                                   StatisticDataGroup::DATA_STORAGE_MODE_POINT);
            StatisticMeanAndDeviation smad;
            smad.addDataGroup(&sdg);
            try {
               smad.execute();
            }
            catch (StatisticException&) {
            }
            b1.setLinkRadius(j, smad.getMean());
            
            
            //
            // Set the average radius for this link
            //
            b1.setLinkXYZ(j, average);
         }
         
         //
         // add new average border to the border file that will be returned
         //
         averageBorderFileOut.addBorder(b1);
      }
   }
   
   //
   // Project to sphere if needed
   //
   if (averageBorderFileOut.getNumberOfBorders() > 0) {
      if (sphereFlag) {
         averageBorderFileOut.setSphericalBorderRadius(sphereRadius);
      }
   }
   
   QString comment("Average borders from: \n");
   for (int i = 0; i < numFiles; i++) {
      comment.append(FileUtilities::basename(inputBorderFiles[i]->getFileName()));
      comment.append("\n");
   }
   averageBorderFileOut.setFileComment(comment);
   averageBorderFileOut.setHeaderTag(AbstractFile::headerTagConfigurationID,
      inputBorderFiles[0]->getHeaderTag(AbstractFile::headerTagConfigurationID));
}

/** 
 * Clear the border file.
 */
void
BorderFile::clear()
{
   clearAbstractFile();
   borders.clear();
}

/** 
 * Add a border to this file.
 */
void
BorderFile::addBorder(const Border& b)
{
   borders.push_back(b);
   const int index = static_cast<int>(borders.size()) - 1;
   borders[index].borderFile = this;
   setModified();
}

/**
 * Get a border index from its name.
 */
int
BorderFile::getBorderIndexByName(const QString& nameToFind,
                                 const bool landmarkBordersFlag) const
{
   for (unsigned int i = 0; i < borders.size(); i++) {
      if (landmarkBordersFlag) {
         if (Border::compareLandmarkBorderNames(borders[i].getName(), nameToFind)) {
            return i;
         }
      }
      else {
         if (borders[i].getName() == nameToFind) {
            return i;
         }
      }
   }
   return -1;
}

/** 
 * Get a border by name (const method)
 */
const Border* 
BorderFile::getBorderByName(const QString& nameToFind,
                            const bool landmarkBordersFlag) const
{
   const int index = getBorderIndexByName(nameToFind, landmarkBordersFlag);
   if (index >= 0) {
      return &borders[index];
   }
   return NULL;
}

/** 
 * Get a border by name.
 */
Border* 
BorderFile::getBorderByName(const QString& nameToFind,
                            const bool landmarkBordersFlag)
{
   const int index = getBorderIndexByName(nameToFind, landmarkBordersFlag);
   if (index >= 0) {
      return &borders[index];
   }
   return NULL;
}

/**
 * Apply transformation matrix to border file.
 */
void
BorderFile::applyTransformationMatrix(TransformationMatrix& tm)
{ 
   for (int i = 0; i < getNumberOfBorders(); i++) {
      Border* border = getBorder(i);
      border->applyTransformationMatrix(tm);
/*
      const int numLinks = border->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         float xyz[3];
         border->getLinkXYZ(j, xyz);
         double p[4] = { xyz[0], xyz[1], xyz[2], 1.0 };
         tm.multiplyPoint(p);
         xyz[0]   = p[0];
         xyz[1] = p[1];
         xyz[2] = p[2];
         border->setLinkXYZ(j, xyz);
      }
*/
   }
   setModified();
}  

/**
 * Get a border with the specified projection ID
 */
Border*
BorderFile::getBorderWithProjectionID(const int projectionID)
{
   const int index = getBorderIndexForBorderWithProjectionID(projectionID);
   if (index >= 0) {
      return getBorder(index);
   }
   return NULL;
}

/**
 * Get the index for the border with the specified projection ID
 */
int 
BorderFile::getBorderIndexForBorderWithProjectionID(const int projectionID) const
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      const Border* b = getBorder(i);
      if (b->getBorderProjectionID() == projectionID) {
         return i;
      }
   }
   return -1;
}

/**
 * Remove a border at the specified index
 */
void
BorderFile::removeBorder(const int borderNumber)
{
   if (borderNumber < getNumberOfBorders()) {
      borders.erase(borders.begin() + borderNumber);
   }
   setModified();
}

/** 
 * remove borders with the specified indices.
 */
void 
BorderFile::removeBordersWithIndices(const std::vector<int>& borderIndicesIn)
{
   std::vector<int> borderIndices = borderIndicesIn;
   std::sort(borderIndices.begin(), borderIndices.end());
   const int num = static_cast<int>(borderIndices.size());
   for (int i = (num - 1); i >= 0; i--) {
      removeBorder(borderIndices[i]);
   }
}
      
/**
 * get the indices of borders that area duplicates of other borders.
 */
void 
BorderFile::getDuplicateBorderIndices(std::vector<int>& duplicateBorderIndices) const
{
   duplicateBorderIndices.clear();
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < (numBorders - 1); i++) {
      const Border* b = getBorder(i);
      
      for (int j = i + 1; j < numBorders; j++) {
         const Border* b2 = getBorder(j);
         if (*b == *b2) {
            duplicateBorderIndices.push_back(j);
         }
      }
   }
}
      
/**
 * Remove the border with the specified projection ID
 */
void
BorderFile::removeBorderWithProjectionID(const int projectionID)
{
   const int index = getBorderIndexForBorderWithProjectionID(projectionID);
   if (index >= 0) {
      removeBorder(index);
      setModified();
   }
}

/**
 * Reset the border projection IDs on all borders so that they do not
 * appear to be projected borders.
 */
void
BorderFile::resetBorderProjectionIDs()
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      borders[i].setBorderProjectionID(-1);
   } 
}

/**
 * Remove all projected borders from this border file.
 */
void
BorderFile::removeAllProjectedBorders()
{
   std::vector<Border> temp;
   const int num = getNumberOfBorders();
   bool removedBorders = false;
   
   for (int i = 0; i < num; i++) {
      if (borders[i].getBorderProjectionID() >= 0) {
         removedBorders = true;
      }
      else {
         temp.push_back(borders[i]);
      }
   }
   
   if (removedBorders) {
      borders = temp;
      setModified();
   }
}

/**
 * Set the status of all border name display flags
 */
void 
BorderFile::setAllNameDisplayFlags(const bool status)
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      getBorder(i)->setNameDisplayFlag(status);
   }
}

/**
 * Set the status of all border display flags for borders with the specified name.
 */
void
BorderFile::setNameDisplayFlagForBordersWithName(const QString& name, const bool flag)
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      if (borders[i].getName() == name) {
         borders[i].setNameDisplayFlag(flag);
      }
   }
}

/**
 * Compute flat normals for borders.
 */
void
BorderFile::computeFlatNormals()
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      Border* b = getBorder(i);
      b->computeFlatNormals();
   }
}

/**
 * Resample the displayed borders.
 */
void
BorderFile::resampleDisplayedBorders(const float density)
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      Border* b = getBorder(i);
      if (b->getDisplayFlag()) {
         int dummy;
         b->resampleBorderToDensity(density, 2, dummy);
      }
   }
}

/**
 * Resample all borders.
 */
void
BorderFile::resampleAllBorders(const float density)
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      Border* b = getBorder(i);
      int dummy;
      b->resampleBorderToDensity(density, 2, dummy);
   }
}

/**
 * Orient the displayed borders clockwise.
 */
void
BorderFile::orientDisplayedBordersClockwise()
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      Border* b = getBorder(i);
      if (b->getDisplayFlag()) {
         b->orientLinksClockwise();
      }
   }
}

/**
 * Reverse the order of the links in the displayed borders.
 */
void
BorderFile::reverseDisplayedBorders()
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      Border* b = getBorder(i);
      if (b->getDisplayFlag()) {
         b->reverseBorderLinks();
      }
   }
}

/** 
 * Read the border file data.
 */
void 
BorderFile::readFileData(QFile& /*file*/, 
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
      Border border(borderName, center, samplingDensity, variance, 
                    topography, uncertainty);
      
      for (int j = 0; j < numLinks; j++) {
         readLine(stream, line);
         
         int linkNumber, sectionNumber;
         float xyz[3];
         float radius = 0.0;
         
         QTextStream(&line, QIODevice::ReadOnly) >> linkNumber
                             >> sectionNumber
                             >> xyz[0]
                             >> xyz[1]
                             >> xyz[2]
                             >> radius;

         border.addBorderLink(xyz, sectionNumber, radius);
      }
      
      addBorder(border);
   }
}

/** 
 * Get the total number of links in a border.
 */
int 
BorderFile::getTotalNumberOfLinks()
{
   int total = 0;
   for (int i = 0; i < getNumberOfBorders(); i++) {
      Border *border = getBorder(i);
      total += border->getNumberOfLinks();
   }
   return total;
}
      
/** 
 * Write the border file data.
 */
void 
BorderFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                          QDomElement& /* rootElement */) throw (FileException)
{
   int numBorders = getNumberOfBorders();

   //
   // only write borders that have links
   //
   int numBordersToWrite = 0;
   for (int i = 0; i < numBorders; i++) {
      if (getBorder(i)->getNumberOfLinks() > 0) {
         numBordersToWrite++;
      }
   }
   stream << numBordersToWrite << "\n";
   
   for (int i = 0; i < numBorders; i++) {
      Border* border = getBorder(i);
      const int numLinks = border->getNumberOfLinks();
      
      //
      // Only write border if it has links
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
         // write link number, section, and xyz
         const float* xyz = border->getLinkXYZ(j);
         stream << j << " "
                << border->getLinkSectionNumber(j) << " "
                << xyz[0] << " "
                << xyz[1] << " "
                << xyz[2] << " "
                << border->getLinkRadius(j) << "\n";
      }
   }
}

/**
 * Set the radius of all border links in a spherical border file.
 */
void 
BorderFile::setSphericalBorderRadius(const float newRadius)
{
   if (newRadius <= 0.0) {
      return;
   }
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      Border* border = getBorder(i);
      const int numLinks = border->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         float xyz[3];
         border->getLinkXYZ(j, xyz);
         const float oldRadius = sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]);
         if (oldRadius != 0.0) {
            const float scale = newRadius / oldRadius;
            xyz[0] *= scale;
            xyz[1] *= scale;
            xyz[2] *= scale;
            border->setLinkXYZ(j, xyz);
         }
      }
   }
}

/**
 * convert configuration ID to spec file tag.
 */
QString
BorderFile::convertConfigurationIDToSpecFileTag(const QString& nameIn)
{  
   const QString name(nameIn.toUpper());
   if (name == "RAW") return SpecFile::getRawBorderFileTag();
   else if (name == "FIDUCIAL") return SpecFile::getFiducialBorderFileTag();
   else if (name == "INFLATED") return SpecFile::getInflatedBorderFileTag();
   else if (name == "VERY_INFLATED") return SpecFile::getVeryInflatedBorderFileTag();
   else if (name == "SPHERICAL") return SpecFile::getSphericalBorderFileTag();
   else if (name == "ELLIPSOIDAL") return SpecFile::getEllipsoidBorderFileTag();
   else if (name == "CMW") return SpecFile::getCompressedBorderFileTag();
   else if (name == "FLAT") return SpecFile::getFlatBorderFileTag();
   else if (name == "FLAT_LOBAR") return SpecFile::getLobarFlatBorderFileTag();
   else if (name == "HULL") return SpecFile::getHullBorderFileTag();
   else return SpecFile::getUnknownBorderFileMatchTag();
}

/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
BorderFile::writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   int numBorders = this->getNumberOfBorders();
   if (numBorders <= 0) {
      throw FileException("Contains no borders");
   }

   QString name = FileUtilities::filenameWithoutExtension(filenameIn)
                + SpecFile::getBorderProjectionFileExtension();
   QFile file(name);
   if (file.open(QFile::WriteOnly) == false) {
      throw FileException("Unable to open for writing");
   }
   QTextStream stream(&file);

   XmlGenericWriter xmlWriter(stream);
   xmlWriter.writeStartDocument();

   XmlGenericWriterAttributes attributes;
   attributes.addAttribute("CaretFileType", "BorderProjection");
   attributes.addAttribute("xmlns:xsi",
                           "http://www.w3.org/2001/XMLSchema-instance");
   attributes.addAttribute("xsi:noNamespaceSchemaLocation",
                           "http://brainvis.wustl.edu/caret6/xml_schemas/BorderProjectionFileSchema.xsd");
   attributes.addAttribute("Version", "6.0");
   xmlWriter.writeStartElement("CaretDataFile", attributes);

   this->writeHeaderXMLWriter(xmlWriter);

   GiftiLabelTable labelTable;
   if (colorFileIn != NULL) {
      labelTable.createLabelsFromColors(*colorFileIn);
   }
   labelTable.writeAsXML(xmlWriter);

   for (int i = 0; i < numBorders; i++) {
      Border* b = this->getBorder(i);
      int numLinks = b->getNumberOfLinks();
      if (numLinks > 0) {

         XmlGenericWriterAttributes attributes;
         attributes.addAttribute("Index", i);
         xmlWriter.writeStartElement("BorderProjection", attributes);

         xmlWriter.writeElementCData("Name", b->getName());

         for (int j = 0; j < numLinks; j++) {
            Caret6ProjectedItem pi;
            pi.projectionType = Caret6ProjectedItem::UNPROJECTED;
            b->getLinkXYZ(j, pi.xyz);
            pi.structure = structure;
            pi.writeXML(xmlWriter);
         }

         xmlWriter.writeEndElement();
      }
   }

   xmlWriter.writeEndElement();

   xmlWriter.writeEndDocument();

   file.close();

   return name;
}

/**
 * Write the file's memory in caret7 format to the specified name.
 */
QString
BorderFile::writeFileInCaret7Format(const QString& filenameIn, 
                                    Structure structure,
                                    const ColorFile* colorFileIn, 
                                    const bool useCaret7ExtensionFlag) throw (FileException)
{
    int numBorders = this->getNumberOfBorders();
    if (numBorders <= 0) {
        throw FileException("Contains no borders");
    }
    
    QString name = filenameIn;
    QFile file(name);
    if (file.open(QFile::WriteOnly) == false) {
        throw FileException("Unable to open for writing");
    }
    QTextStream stream(&file);
    
    XmlGenericWriter xmlWriter(stream);
    xmlWriter.writeStartDocument();
    
    XmlGenericWriterAttributes attributes;
    attributes.addAttribute("Version", "1.0");
    xmlWriter.writeStartElement("BorderFile", attributes);
    
    this->writeHeaderXMLWriter(xmlWriter);
    
    GiftiLabelTable labelTable;
    if (colorFileIn != NULL) {
        labelTable.createLabelsFromColors(*colorFileIn);
    }
    labelTable.writeAsXML(xmlWriter);
    
    for (int i = 0; i < numBorders; i++) {
        Border* b = this->getBorder(i);
        int numLinks = b->getNumberOfLinks();
        if (numLinks > 0) {
            QString className = "";
            QString colorName = "BLACK";
            const QString borderName = b->getName();
            const int labelIndex = labelTable.getBestMatchingLabelIndex(borderName);
            if (labelIndex >= 0) {
                className = labelTable.getLabel(labelIndex);
                if (className.isEmpty() == false) {
                    colorName = "CLASS";
                }
            }
            
            xmlWriter.writeStartElement("Border");
            
            xmlWriter.writeElementCharacters("Name", 
                                             borderName);
            
            xmlWriter.writeElementCharacters("ClassName", 
                                             className);
            xmlWriter.writeElementCharacters("ColorName",
                                             colorName);
            
            QString structureName = "Invalid";
            if (structure.isLeftCortex()) {
                structureName = "CORTEX_LEFT";
            }
            else if (structure.isRightCortex()) {
                structureName = "CORTEX_RIGHT";
            }
            else if (structure.isCerebellum()) {
                structureName = "CEREBELLUM";
            }
            
            for (int j = 0; j < numLinks; j++) {
                const float* xyz = b->getLinkXYZ(j);
                
                xmlWriter.writeStartElement("SurfaceProjectedItem");
                xmlWriter.writeElementCharacters("Structure", structureName);
                xmlWriter.writeElementCharacters("StereotaxicXYZ", xyz, 3);
                xmlWriter.writeEndElement();
            }
            
            xmlWriter.writeEndElement();
        }
    }
    
    xmlWriter.writeEndElement();
    
    xmlWriter.writeEndDocument();
    
    file.close();
    
    return name;
}

