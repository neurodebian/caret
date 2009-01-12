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



#ifndef __VE_BORDER_PROJECTION_FILE_H__
#define __VE_BORDER_PROJECTION_FILE_H__

#include "AbstractFile.h"

class Border;
class BorderProjectionFile;
class ColorFile;
class CoordinateFile;
class TopologyHelper;

/// Class for storing a border projection link.
class BorderProjectionLink {
   public:
      /// Constructor
      BorderProjectionLink(const int sectionIn,
                       const int verticesIn[3],
                       const float areasIn[3],
                       const float radiusIn);
      
      /// Destructor
      ~BorderProjectionLink();
      
      /// determine if two border projection links are the same
      bool operator==(const BorderProjectionLink& bpl) const;
      
      /// set border data
      void setData(const int sectionIn,
                   const int verticesIn[3],
                   const float areasIn[3],
                   const float radiusIn);
                       
      /// get border data
      void getData(int& sectionOut,
                   int verticesOut[3],
                   float areasOut[3],
                   float& radiusOut) const;
                   
      /// unproject a border link
      void unprojectLink(const CoordinateFile* unprojectCoordFile,
                         float xyzOut[3]) const;
   private:
      /// border projection file this border projection belongs to
      BorderProjectionFile* borderProjectionFile;
      
      /// section of this border projection point
      int section;
      
      /// vertices for this border projection point
      int vertices[3];
      
      /// barycentric areas for this border projection point
      float areas[3];
      
      /// radius of border point
      float radius;
      
   friend class BorderProjection;
   friend class BorderProjectionFile;
};


/// Class for storing a border projection.
class BorderProjection {
   public:
      /// constructor
      BorderProjection(const QString& nameIn = "_no_name_",
                       const float* centerIn3 = NULL, 
                       const float samplingDensityIn = 25.0,
                       const float varianceIn = 1.0, 
                       const float topographyIn = 0.0,
                       const float arealUncertaintyIn = 0.0);
      
      /// destructor
      ~BorderProjection();
      
      /// determine if two border projections are the same
      bool operator==(const BorderProjection& bp) const;

      /// append a border projection to this border projection
      void append(const BorderProjection& bp);
      
      /// add a border projection link
      void addBorderProjectionLink(const BorderProjectionLink& bl);

      /// add border projection link directly on node
      void addBorderProjectionLinkOnNode(const int nodeNumber);
      
      /// get borders attribute data
      void getData(QString& nameOut, float centerOut[3], 
                   float& samplingDensityOut, float& varianceOut,
                   float& topographyOut, float& arealUncertaintyOut) const;
                   
      /// get the sampling density for a border projection
      float getSamplingDensity() const { return samplingDensity; }
      
      /// set a borders attribute data
      void setData(const QString& nameIn, const float centerIn[3], 
                   const float samplingDensityIn, const float varianceIn,
                   const float topographyIn, const float arealUncertaintyIn);

      /// return a pointer to a border link
      BorderProjectionLink* getBorderProjectionLink(const int i) { return &links[i]; }
      
      /// return a pointer to a border link
      const BorderProjectionLink* getBorderProjectionLink(const int i) const { return &links[i]; }
      
      /// get border color index
      int getBorderColorIndex() const { return borderColorIndex; }
      
      /// set border color index
      void setBorderColorIndex(const int bci) { borderColorIndex = bci; }
      
      /// get a border's name
      QString getName() const { return name; }
      
      /// set a border's name
      void setName(const QString& s) { name = s; }
      
      // return the number of links in this border projection
      int getNumberOfLinks() const { return links.size(); }
      
      /// get the unique ID
      int getUniqueID() const { return uniqueID; }
      
      /// get center of gravity
      void getCenterOfGravity(const CoordinateFile* coordFile,
                              float centerOfGravityOut[3]) const;
      
      /// insert a border projection link before the specified link number (use number of links for end)
      void insertBorderProjectionLink(const int linkIndex,
                                      const BorderProjectionLink& bl);
                                       
      /// insert a border projection link before the specified link number (use number of links for end)
      void insertBorderProjectionLinkOnNode(const int linkIndex,
                                            const int nodeNumber);
      
      /// get a subset of the links as a border projection
      BorderProjection getSubSetOfBorderProjectionLinks(const int startLinkNumber,
                                                        const int endLinkNumber) const;
                                                        
      /// unproject a border
      void unprojectBorderProjection(const CoordinateFile* cf,
                                     Border& borderOut) const;
                           
      /// unproject a border
      void unprojectBorderProjection(const CoordinateFile* cf,
                                     const TopologyHelper* th,
                                     Border& borderOut) const;
      
      /// get the link number nearest to a coordinate
      int getLinkNumberNearestToCoordinate(const CoordinateFile* cf,
                                           const float xyz[3]) const;
      
      /// get the link number furthest from a coordinate
      int getLinkNumberFurthestFromCoordinate(const CoordinateFile* cf,
                                              const float xyz[3]) const;
      
      /// split a border that is approximately linear in shape
      bool splitClosedBorderProjection(const CoordinateFile* cf,
                                       const int startingLinkNumber,
                                       const QString& newNameSuffix,
                                       BorderProjection& halfOneOut,
                                       BorderProjection& halfTwoOut,
                                       const int endingLinkNumber = -1);
                                       
      /// get the center of gravity for a border (returns true if valid)
      bool getCenterOfGravity(const CoordinateFile* cf,
                              const TopologyHelper* th,
                              float cogXYZOut[3]) const;
                           
      /// change the starting link of a closed border so it is close to a point
      void changeStartingLinkOfClosedBorderToBeNearPoint(const CoordinateFile* cf,
                                                         const float pointXYZ[3]);
      
      /// remove links from border within specified distances of point
      /// if a specified distance is zero or less it is ignored
      void removeLinksNearPoint(const CoordinateFile* unprojectCoordFile,
                                const float pointXYZ[3],
                                const float xDistance,
                                const float yDistance,
                                const float zDistance,
                                const float straightLineDistance);
                                                        
      /// remove links in border before/after link nearest to point
      void removeLinksBeforeAfterLinkNearestPoint(const CoordinateFile* cf,
                                                  const float pointXYZ[3],
                                                  const bool removeAfterFlag,
                                                  const bool removeBeforeFlag);
      
      /// remove links from border that are not within the specified extent 
      void removeLinksOutsideExtent(const CoordinateFile* unprojectCoordFile,
                                    const float extent[6]);
                                                        
      /// remove the border projection link at the specified index
      void removeBorderProjectionLink(const int linkNumber);
   
      /// remove duplicate border projection links
      void removeDuplicateBorderProjectionLinks();
      
      /// remove the last border projection link
      void removeLastBorderProjectionLink();
      
      /// reverse the order of the border projection links
      void reverseOrderOfBorderProjectionLinks();
      
   private:
      /// border projection file this border projection belongs to
      BorderProjectionFile* borderProjectionFile;
      
      /// the links in this border projection
      std::vector<BorderProjectionLink> links;
      
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
      
      /// unique identifier for this border projection
      int uniqueID;
      
      /// unique ID source
      static int uniqueIDSource;
      
   friend class BorderProjectionFile;
};

/// Class for storing a border projection file.
class BorderProjectionFile : public AbstractFile {
   public:
   
      /// constructor      
      BorderProjectionFile();
      
      /// destructor
      ~BorderProjectionFile();
     
      /// append another border projection file to this one.
      void append(BorderProjectionFile& bpf);
      
      /// assign colors to the borders
      void assignColors(const ColorFile& colorFile);
      
      /// clear a border projection file
      void clear();

      /// add a border projection    
      void addBorderProjection(const BorderProjection& b);
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfBorderProjections() == 0); }
      
      /// get the indices of border projections that area duplicates of other border projections.
      void getDuplicateBorderProjectionIndices(std::vector<int>& duplicateBorderProjIndices) const;

      /// get a specified border by index
      BorderProjection* getBorderProjection(const int i) { return &links[i]; }

      /// get a specified border by index
      const BorderProjection* getBorderProjection(const int i) const { return &links[i]; }

      /// get the index of a border projection
      int getBorderProjectionIndex(const BorderProjection* bp) const;
      
      /// get first specified border by name
      BorderProjection* getFirstBorderProjectionByName(const QString& name);

      /// get first specified border by name
      const BorderProjection* getFirstBorderProjectionByName(const QString& name) const;

      /// get last specified border by name
      BorderProjection* getLastBorderProjectionByName(const QString& name);

      /// get last specified border by name
      const BorderProjection* getLastBorderProjectionByName(const QString& name) const;

      /// get number of borders in this file
      int getNumberOfBorderProjections() const { return links.size(); }
      
      /// get a border projection with the specified unique ID
      BorderProjection* getBorderProjectionWithUniqueID(const int uniqueID);
      
      /// get the index of border projection with the specified unique ID
      int getBorderProjectionIndexWithUniqueID(const int uniqueID) const;
      
      /// get the border projection with the largest number of links
      BorderProjection* getBorderProjectionWithLargestNumberOfLinks();
      
      /// remove border projection with the specified unique ID
      void removeBorderProjectionWithUniqueID(const int uniqueID);
      
      /// remove border projection at specified index
      void removeBorderProjection(const int index);
      
      /// remove borders with the specified indices.
      void removeBordersWithIndices(const std::vector<int>& borderProjectionIndicesIn);

      /// remove borders with the specified name.
      void removeBordersWithName(const QString& nameIn);

      /// reverse order of links in all border projections
      void reverseOrderOfAllBorderProjections();
      
      /// read the file's data
      void readFileData(QFile& file, QTextStream& stream, QDataStream&,
                        QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the file's data
      void writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                         QDomElement& /* rootElement */) throw (FileException);
      
      /// compare a file for unit testing (returns true if "within tolerance")
      virtual bool compareFileForUnitTesting(const AbstractFile* af,
                                             const float tolerance,
                                             QString& messageOut) const;
   private:
   
      /// this file's border projections
      std::vector<BorderProjection> links;
      
   friend class BorderProjection;
   friend class BorderProjectionLink;
};

#endif // __VE_BORDER_PROJECTION_FILE_H__

#ifdef __BORDER_PROJECTION_MAIN__
int BorderProjection::uniqueIDSource = 1;
#endif // __BORDER_PROJECTION_MAIN__
