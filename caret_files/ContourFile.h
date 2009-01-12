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


#ifndef __VE_CONTOUR_FILE_H__
#define __VE_CONTOUR_FILE_H__

#include <algorithm>

#include "AbstractFile.h"
#include "TransformationMatrixFile.h"

class ContourFile;
class MDPlotFile;
class NeurolucidaFile;

/// This class holds a single contour.
class CaretContour {
   private:
      /// points in the contour
      class ContourPoint {
         public:
            // constructor
            ContourPoint();
            
            // constructor
            ContourPoint(const float xi,
                  const float yi,
                  const float zi,
                  const bool special = false);
                 
            // destructor
            ~ContourPoint();
            
            /// contour x coordinates
            float x;
            
            /// contour y coordinates
            float y;
            
            /// contour z coordinates
            float z;

            /// highlight flag
            bool highlightFlag;
            
            /// special flag used for some operations
            bool specialFlag;
      
      };
      
      /// points in the contour
      std::vector<ContourPoint> points;
      
      /// section number of this contour
      int sectionNumber;
      
      /// contour file this section is part of
      ContourFile* contourFile;
      
      /// copy helper
      void copyHelper(const CaretContour& cc);
      
   public:
      /// Create the contour
      CaretContour(const int estimatedNumberOfPointsInContour = 0);

      /// Destructor
      ~CaretContour();
      
      /// copy constructor
      CaretContour(const CaretContour& cc);
      
      /// assignment operator
      CaretContour& operator=(const CaretContour& cc);
      
      /// Add a point to the contour
      void addPoint(const float xIn, const float yIn, const float zIn = -10000000);
      
      /// Add points to the contour
      void addPoints(const float* xIn, const float* yIn, const int numPoints);
      
      /// clear the contour points
      void clearPoints();
      
      /// Delete a point from the contour
      void deletePoint(const int pointIndex);
      
      /// Get number of points in the contour
      int getNumberOfPoints() const;
      
      /// Get a point from the contour
      void getPointXY(const int pointIndex, float& xOut, float& yOut) const;
      
      /// Get a point from the contour
      void getPointXYZ(const int pointIndex, float& xOut, float& yOut, float& zOut) const;
      
      /// Get a point from the contour
      void getPointXYZ(const int pointIndex, float xyzOut[3]) const;
      
      /// get the estimated distance between each point in the contour
      float getAverageDistanceBetweenPoints() const;
      
      /// resample the contour
      void resample(const float distanceBetweenPoints);
      
      /// remove consecutive duplicate points (same x/y)
      void removeDuplicatePoints();
      
      /// reverse the order of the points in a contour
      void reversePointOrder();
      
      /// Get the z for a contour
      int getSectionNumber() const;
      
      /// Set a point from the contour
      void setPointXY(const int pointIndex, const float xIn, const float yIn);
      
      /// Set a point from the contour
      void setPointXYZ(const int pointIndex, const float xIn, const float yIn, const float zIn);
      
      /// clear the highlight flags
      void clearHighlightFlags();
      
      /// Clear the special flag for all points
      void clearSpecialFlags();
            
      /// Returns true if any of the contour point's special flag is set
      bool getAnySpecialFlagSet() const;
      
      /// get the highlight flag for a point
      bool getHighlightFlag(const int pointIndex) const;
      
      /// set the special flag for a point
      void setHighlightFlag(const int pointIndex, const bool flag);
      
      /// Get the special flag for a point
      bool getSpecialFlag(const int pointIndex) const;
            
      /// Set the special flag for a point
      void setSpecialFlag(const int pointIndex, const bool flag);
            
      /// Set the section
      void setSectionNumber(const int sectionNumberIn);
      
      /// operator for comparing contours by section number
      bool operator<(const CaretContour& cc) const;
            
   friend class ContourFile;
};

/// This class contains contours
class ContourFile : public AbstractFile {
   public:
      /// section selection type
      enum SECTION_TYPE {     
         SECTION_TYPE_SINGLE,
         SECTION_TYPE_MULTIPLE,
         SECTION_TYPE_ALL,
         SECTION_TYPE_HIDE
      };
      
   private:
   
      static const QString tagNumberOfContours;
      static const QString tagSectionSpacing;

      /// selected section type
      SECTION_TYPE sectionType;
      
      /// minimum section
      int minimumSection;
      
      /// maximumSection
      int maximumSection;
      
      /// storage for the contours (do not change to pointer else sorting will break)
      std::vector<CaretContour> contours;
      
      /// spacing between each consecutive section number (millimeters)
      float sectionSpacing;
      
      /// minimum selected section
      int minimumSelectedSection;
      
      /// maximum selected section
      int maximumSelectedSection;

      /// Set the minimum and maximum section numbers.
      void setMinMaxSections();
      
      /// Update minimum and maximum section numbers.
      void updateMinMaxSections(const int sectionNumber);  
      
      /// read a version 0 contour file
      void readFileDataVersion0(QTextStream& fp) throw (FileException);
      
      /// read a version 1 contour file
      void readFileDataVersion1(QTextStream& fp) throw (FileException);
      
   public:

      /// Constructor
      ContourFile();
      
      /// Destructor
      ~ContourFile();
      
      /// Append a contour file to this one
      void append(ContourFile& cf, QString& errorMessage);
      
      /// Clear the file
      void clear();
      
      /// cleanup contours
      bool cleanupContours();
      
      /// see if file contains no data
      bool empty() const { return contours.empty(); }
      
      /// Add a contour (returns index number of contour)
      void addContour(const CaretContour& contourIn);
      
      /// apply a transformation matrix to the contours
      void applyTransformationMatrix(const int sectionLow, 
                                     const int sectionHigh,
                                     const TransformationMatrix& matrix,
                                     const bool limitToSpecialFlagNodes);
      
      /// set the special flag for all contour points in the section range and box
      void setSpecialFlags(const int sectionLow,
                           const int sectionHigh,
                           const float bounds[4]);
                           
      /// Clear the special flag in all contours
      void clearSpecialFlags();
                    
      /// Clear the highlight flags
      void clearHighlightFlags();
      
      /// Delete a contour
      void deleteContour(const int indexNumber);
      
      /// Get a contour
      CaretContour* getContour(const int indexNumber);

      /// Get a contour (const method)
      const CaretContour* getContour(const int indexNumber) const;
      
      /// Get the number of contours
      int getNumberOfContours() const;
      
      /// Get the extent (min and max X & Y) of the contours
      void getExtent(float& minX, float& maxX,
                     float& minY, float& maxY) const;
      /// Get the center of gravity for a section.
      /// Returns number of points for the section
      int getSectionCOG(const int sectionNumber,
                        float& cogX, float& cogY) const;
                        
      /// Get the range of sections
      void getSectionExtent(int& lowest, int& highest) const;
      
      /// Get the section spacing
      float getSectionSpacing() const;
      
      /// Merge two contours into a single contour
      void mergeContours(const int contour1, const int contour2);
      
      /// Set the section spacing
      void setSectionSpacing(const float sectionSpacingIn);
      
      /// Sort the contours by section number
      void sortBySectionNumber();
      
      /// get the section type
      SECTION_TYPE getSectionType() const;

      /// set the selection type
      void setSectionType(const SECTION_TYPE type);

      /// get the minimum section
      int getMinimumSection() const { return minimumSection; }

      /// get the maximum section
      int getMaximumSection() const { return maximumSection; }

      /// get the minimum selected section
      int getMinimumSelectedSection() const;

      /// get the maximum selected section
      int getMaximumSelectedSection() const;

      /// set the minimum selected section
      void setMinimumSelectedSection(const int sect);

      /// set the maximum selected section
      void setMaximumSelectedSection(const int sect);

      /// resample all of the contours
      void resampleAllContours(const float distanceBetweenPoints);
      
      /// find contour point within distance of coordinate
      void findContourPoint(const float xyz[3], 
                            const float withinDistance,
                            int& contourNumber, 
                            int& contourPointNumber) const;
      
      /// import contours from an MD Plot File
      void importMDPlotFile(const MDPlotFile& mdf) throw (FileException);
      
      /// import contours from a Nuerolucida File
      void importNeurolucidaFile(const NeurolucidaFile& nf) throw (FileException);
      
      /// Read the file's data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the file's data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);

   friend class CaretContour;
};

#endif // __VE_CONTOUR_FILE_H__

#ifdef _CONTOUR_FILE_MAIN_
const QString ContourFile::tagNumberOfContours = "tag-number-of-contours";
const QString ContourFile::tagSectionSpacing = "tag-section-spacing";
#endif // _CONTOUR_FILE_MAIN_

