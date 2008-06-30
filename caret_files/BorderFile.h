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


#ifndef __VE_BORDER_FILE_H__
#define __VE_BORDER_FILE_H__

#include <QGLWidget>

#include "AbstractFile.h"
#include "FileException.h"
#include "SpecFile.h"

class BorderFile;
class ColorFile;
class CoordinateFile;
class TopologyFile;
class TransformationMatrix;

/// class for storing a single border
class Border {
   private:   
      /// Border File this border belongs to
      BorderFile* borderFile;
      
      /// the link coordinates in this border (3 per link)
      std::vector<float> linkXYZ;
      
      /// the link radii (1 per link)
      std::vector<float> linkRadii;
      
      /// the link flat normal vectors (3 per link)
      std::vector<float> linkFlatNormal;
      
      /// the link section (1 per link)
      std::vector<int> linkSection;
      
      /// name of this border
      QString name;
      
      /// center of border (obsolete and unused)
      float center[3];
      
      /// sampline density of this border
      float samplingDensity;
      
      /// variance of this border
      float variance;
      
      /// topography of this border
      float topographyValue;
      
      /// uncertainty of this border
      float arealUncertainty;
      
      /// index into border color file
      int borderColorIndex;
      
      /// border projection identifier for this border (negative if not from border proj file)
      int borderProjectionID;
      
      /// border display flag (show the border)
      bool displayFlag;
      
      /// border name selected for display flag
      bool nameDisplayFlag;
      
      /// resample a border
      void resampleBorder(const float* xorig, const float* yorig, 
                          const float* zorig, const int numPointsIn,
                          const float density,
                          float* xout, float* yout, float* zout,
                          const int numPointsOut);
   public:
      
      /// constructor
      Border(const QString& nameIn = "", const float* centerIn = NULL, 
             const float samplingDensityIn = 25.0,
             const float varianceIn = 1.0, 
             const float topographyIn = 0.0,
             const float arealUncertaintyIn = 0.0);
      
      /// determine if two borders are the same (same name and links)
      bool operator==(const Border& b) const;
      
      /// Apply transformation matrix to border
      void applyTransformationMatrix(TransformationMatrix& tm);
      
      /// get the position for a link
      const float* getLinkXYZ(const int linkNumber) const { return &linkXYZ[linkNumber * 3]; }
      
      /// get the position for a link
      void getLinkXYZ(const int linkNumber, float xyz[3]) const;

      /// set the position of a link
      void setLinkXYZ(const int linkNumber, const float xyz[3]);
      
      /// get the radius for a link
      float getLinkRadius(const int linkNumber) const;
      
      /// set the radius for a link
      void setLinkRadius(const int linkNumber, const float radius);
      
      /// create a border with a subset of the border's links
      Border* getSubSet(const int linkStart, const int linkEnd) const;
      
      /// get the flat normal for a link
      const float* getLinkFlatNormal(const int linkNumber) const { 
         return &linkFlatNormal[linkNumber * 3]; 
      }
      
      /// set the flat normal for a link
      void setLinkFlatNormal(const int linkNumber, const float normal[3]);
      
      /// get the section number for a link
      int getLinkSectionNumber(const int linkNumber) const { return linkSection[linkNumber]; }
      
      /// compute flat normals for each link
      void computeFlatNormals();
      
      /// set the section number of a link
      void setLinkSectionNumber(const int linkNumber, const int section);
      
      /// get the topography value
      float getTopographyValue() const { return topographyValue; }

      /// clear the border links
      void clearLinks();
      
      /// add a border link
      void addBorderLink(const float xyz[3], const int section = 0, const float radius = 0.0);

      /// insert a border link before the specified link number (use number of links for end)
      void insertBorderLink(const int linkIndex,
                            const float xyz[3], 
                            const int section = 0, 
                            const float radius = 0.0);
                            
      /// find links where another border intersects this border (2D borders in X-Y plane)
      bool intersection2D(const Border* otherBorder,
                          const bool thisBorderIsClosed, const bool otherBorderIsClosed,
                          int& myLinkIntersect, int& otherLinkIntersect) const;
      
      /// find first link in "this" border that is within "tolerance" distance
      /// of a link in "other border"
      bool intersection3D(const Border* otherBorder,
                          const float intersectionTolerance,
                          int& myLinkIntersect, int& otherLinkIntersect) const;
      
      /// get total length of a border      
      float getBorderLength() const;
      
      /// get borders attribute data
      void getData(QString& nameOut, float centerOut[3], 
                   float& samplingDensityOut, float& varianceOut,
                   float& topographyOut, float& arealUncertaintyOut) const;
                   
      /// get the uncertainty
      float getArealUncertainty() const { return arealUncertainty; }

      /// get the sampling density for a border
      float getSamplingDensity() const { return samplingDensity; }
      
      /// get the variance
      float getVariance() const { return variance; }

      /// set a borders attribute data
      void setData(const QString& nameIn, const float centerIn[3], 
                   const float samplingDensityIn, const float varianceIn,
                   const float topographyIn, const float arealUncertaintyIn);

      /// return a pointer to a border link
      //BorderLink* getBorderLink(const int i) { return &links[i]; }
      
      /// get border color index
      int getBorderColorIndex() const { return borderColorIndex; }
      
      /// set border color index
      void setBorderColorIndex(const int bci) { borderColorIndex = bci; }
      
      /// get border projection identifier
      int getBorderProjectionID() const { return borderProjectionID; }
      
      /// set the border projection identifier
      void setBorderProjectionID(const int indx) { borderProjectionID = indx; }
      
      /// distance between two border links
      float distanceBetweenLinks(const int link1, const int link2) const;
      
      /// get a border's name
      QString getName() const { return name; }
      
      /// set a border's name
      void setName(const QString& nameIn) { name = nameIn; }

      /// get border selected for display (show the border)
      bool getDisplayFlag() const { return displayFlag; }
      
      /// set border selected for display (show the border)
      void setDisplayFlag(const bool df) { displayFlag = df; }
      
      /// get name selected for display flag
      bool getNameDisplayFlag() { return nameDisplayFlag; }
      
      /// set name selected for display flag
      void setNameDisplayFlag(const bool df) { nameDisplayFlag = df; }
      
      /// return the number of links in this border
      int getNumberOfLinks() const { return (linkXYZ.size() / 3); }
      
      /// get the bounds of border
      void getBounds(float bounds[6]) const;
      
      /// get the center of gravity of the border (returns true if valid)
      bool getCenterOfGravity(float cogXYZOut[3]) const;
      
      /// get the link number nearest to a coordinate
      int getLinkNumberNearestToCoordinate(const float xyz[3]) const;
      
      /// get the link number furthest from a coordinate
      int getLinkNumberFurthestFromCoordinate(const float xyz[3]) const;
      
      /// remove links on negative side of plane
      void removePointsOnNegativeSideOfPlane(const float planeNormal[3],
                                             const float pointInPlane[3]);
      
      /// orient the links clockwise
      void orientLinksClockwise();
      
      /// orient the links counter-clockwise
      void orientLinksCounterClockwise();
      
      /// See if points are inside a border (border assumed flat in X-Y plane)
      void pointsInsideBorder2D(const float* points, const int numPoints,
                                std::vector<bool>& insideFlags,
                                const bool checkNonNegativeZPointsOnly = false,
                                const float zMinimum = 0.0) const;

      /// See if 3D points are inside a 3D border (transform all to screen axis)
      void pointsInsideBorder3D(const GLdouble* modelMatrix,
                                const GLdouble* projectionMatrix,
                                const GLint* viewport,
                                const float* points, 
                                const int numPoints,
                                std::vector<bool>& insideFlags,
                                const bool checkNonNegativeScreenZPointsOnly = true) const;

      /// remove a border link
      void removeLink(const int linkNumber);
      
      // resample a border to the specified density (distance between links)
      void resampleBorderToDensity(const float density, 
                                   const int minimumNumberOfLinks,
                                   int& newNumberOfLinks);
      
      /// resample a border so that it has the specified number of links
      void resampleBorderToNumberOfLinks(const int numberOfLinks);
      
      /// reverse the order of the links in a border
      void reverseBorderLinks();
      
      /// remove landmark rater from name of border
      void removeLandmarkRaterInfoFromBorderName();
      
      /// get the character that begins the landmark rater into in a border name
      static char getLandmarkRaterFirstChar() { return ':'; }
      
      /// smooth the border links
      void smoothBorderLinks(const int numberOfIterations,
                             const bool closedBorderFlag,
                             const std::vector<bool>* smoothTheseLinksOnly = NULL);
      
      /// remove intersecting loops in a border
      void removeIntersectingLoops(const char axisXYZ) throw (FileException);
      
      /// compare the name of two landmark borders
      /// only compares text before the semi-colon in the name
      static bool compareLandmarkBorderNames(const QString& name1, const QString& name2);

      /// interpolate two borders to create new borders (input borders will be resampled too)
      static void createInterpolatedBorders(Border* b1,
                                            Border* b2,
                                            const QString& namePrefix,
                                            const int numberOfNewBorders,
                                            const float sampling,
                                            std::vector<Border*>& outputInterpolatedBorders) throw (FileException);
                                     
   friend class BorderFile;
};

/// class for storing a border file
class BorderFile : public AbstractFile {
   private:
      /// The borders
      std::vector<Border> borders;

   public:
      /// constructor      
      BorderFile(const QString& descriptiveName = "Border File",
                 const QString& defaultExtensionIn = SpecFile::getBorderFileExtension());
      
      /// constructor that creates a border file from the tiles and coordinates
      BorderFile(const TopologyFile* tf, const CoordinateFile* cf);
      
      /// Constructor that creates a BorderFile from the coordinates in a coordinate file.
      BorderFile(const CoordinateFile* cf, const int maxLinksPerBorder = -1);
      
      /// destructor
      virtual ~BorderFile();
      
      /// clear a border file
      void clear();

      /// copy all the links into a coordinate file
      void copyLinksToCoordinateFile(CoordinateFile* cf);
      
      /// Create a border file that is an average of a group of border files
      static void createAverageBorderFile(const std::vector<BorderFile*>& inputBorderFiles,
                                          const float borderResampling,
                                          const bool sphereFlag,
                                          BorderFile& averageBorderFileOut)
                                                 throw (FileException);
      
      /// add a border      
      void addBorder(const Border& b);
      
      /// append a border file to this one
      void append(BorderFile& bf);
      
      /// assign colors to the borders
      void assignColors(const ColorFile& colorFile);
      
      /// Apply transformation matrix to border file
      void applyTransformationMatrix(TransformationMatrix& tm);
      
      /// compute the flat border file normals
      void computeFlatNormals();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfBorders() == 0); }
      
      /// get the indices of borders that area duplicates of other borders
      void getDuplicateBorderIndices(std::vector<int>& duplicateBorderIndices) const;
      
      /// set status of all border display flags
      void setAllNameDisplayFlags(const bool status);
      
      /// set display flag for borders with specified  name
      void setNameDisplayFlagForBordersWithName(const QString& name, const bool flag);
      
      /// get a specified border by index
      Border* getBorder(const int i) { return &borders[i]; }

      /// get a specified border by index (const method)
      const Border* getBorder(const int i) const { return &borders[i]; }

      /// get a specified border by its name
      Border* getBorderByName(const QString& nameToFind,
                              const bool landmarkBordersFlag = false);

      /// get a specified border by its name (const method)
      const Border* getBorderByName(const QString& nameToFind,
                                    const bool landmarkBordersFlag = false) const;

      /// get a specified border index by its name
      int getBorderIndexByName(const QString& nameToFind,
                               const bool landmarkBordersFlag = false) const;

      /// get a border with a projection ID
      Border* getBorderWithProjectionID(const int projectionID);
      
      /// get border index for border with projectionID
      int getBorderIndexForBorderWithProjectionID(const int projectionID) const;
      
      /// get number of borders in this file
      int getNumberOfBorders() const { return borders.size(); }
      
      /// get total number of links in a border file
      int getTotalNumberOfLinks();
      
      /// remove a border from the border file
      void removeBorder(const int borderNumber);
      
      /// remove borders with the specified indices
      void removeBordersWithIndices(const std::vector<int>& borderIndices);
      
      /// remove a border with the projection identifier
      void removeBorderWithProjectionID(const int projectionID);
      
      /// remove all projected borders (projection borders have projection ID >= 0)
      void removeAllProjectedBorders();
      
      /// reset the border projection IDs so not linked to border projections
      void resetBorderProjectionIDs();

      /// resample displayed borders
      void resampleDisplayedBorders(const float density);

      /// resample to match landmark border file (borders with same names have same number of points)
      void resampleToMatchLandmarkBorders(const BorderFile& landmarkBorderFile) throw (FileException);
      
      /// orient displayed borders clockwise
      void orientDisplayedBordersClockwise();

      /// reverse displayed borders
      void reverseDisplayedBorders();

      /// set the radius of a spherical border file
      void setSphericalBorderRadius(const float radius);
      
   protected:
      /// read the file's data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                        QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the file's data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                         QDomElement& /* rootElement */) throw (FileException);
   
   friend class Border;
};

#endif
