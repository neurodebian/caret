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

#ifndef __BRAIN_MODEL_BORDER_SET_H__
#define __BRAIN_MODEL_BORDER_SET_H__

#include <vector>

#include "BorderFile.h"
#include "BrainModelSurface.h"

class Border;
class BorderFile;
class BorderProjection;
class BorderProjectionFile;
class BrainModelSurface;
class BrainSet;

class BrainModelBorder;
class TransformationMatrix;

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/// class for a border link
class BrainModelBorderLink {
   public:
      /// Constructor
      BrainModelBorderLink(const int numBrainModels);
      
      /// Copy Constructor
      BrainModelBorderLink(const BrainModelBorderLink& bmbl);
      
      /// Destructor
      ~BrainModelBorderLink();
      
      /// add a brain model
      void addBrainModel(const float xyzIn[3]);
      
      /// delete a brain model's links
      void deleteBrainModel(const int brainModelIndex);
      
      /// get the section number
      int getSection() const { return section; }
      
      /// set the section number
      void setSection(const int secNum);
      
      /// get the radius
      float getRadius() const { return radius; }
      
      /// set the radius
      void setRadius(const float radiusIn);
      
      /// get the projection information
      void getProjection(int verticesOut[3], float areasOut[3]) const;
      
      /// get the projection information
      void setProjection(const int verticesIn[3], const float areasIn[3]);

      /// get the link position for a brain model
      const float* getLinkPosition(const int brainModelIndex) const;
      
      /// get the link position for a brain model
      void getLinkPosition(const int brainModelIndex, float xyzOut[3]) const;
      
      /// set the link position for a brain model
      void setLinkPosition(const int brainModelIndex, const float xyzIn[3]);
      
      /// get the link position from the border file
      void getLinkFilePosition(float xyzOut[3]) const;
      
      /// set the link position from the border file
      void setLinkFilePosition(const float xyzIn[3]);
      
      /// apply a transformation matrix to a brain model's borders
      void applyTransformationMatrix(const int brainModelIndex,
                                     const TransformationMatrix& tm);
                                     
      /// get the flat normal
      const float* getFlatNormal() const { return flatNormal; }
      
      /// set the flat normal
      void setFlatNormal(const float normal[3]);
      
      /// unproject a border link
      void unprojectLink(const CoordinateFile* cf, const int brainModelIndex);
      
      /// set the BrainModelBorder using this link
      void setBrainModelBorder(BrainModelBorder* bmb);
      
      /// Set the border holding this brain model's link as modified
      void setModified(const int brainModelIndex);
      
      /// Set the border holding this projection as modified
      void setProjectionModified();
      
   protected:
      /// section of this border link
      int section;

      /// vertices for this border projection point
      int vertices[3];

      /// barycentric areas for this border projection point
      float areas[3];

      /// xyz from border file
      float fileXYZ[3];
      
      /// xyz for each surface (3 per surface)
      std::vector<float> xyz;      
      
      /// flat normals
      float flatNormal[3];
      
      /// radius of link
      float radius;
      
      /// the brain model border using this link (do not copy in copy constructor)
      BrainModelBorder* brainModelBorder;
      
      // be sure to update copy constructor if members added
};

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/// class for a single border
class BrainModelBorder {
   public:
      /// source of border
      enum BORDER_TYPE {
         BORDER_TYPE_UNKNOWN,
         BORDER_TYPE_NORMAL,
         BORDER_TYPE_PROJECTION
      };
      
      /// Constructor
      BrainModelBorder(BrainSet* bs,
                       const QString& nameIn, const BORDER_TYPE typeIn,
                       const BrainModelSurface::SURFACE_TYPES borderFileSurfaceTypeIn
                          = BrainModelSurface::SURFACE_TYPE_UNKNOWN);
      
      /// Constructor for a border from a border file
      BrainModelBorder(BrainSet* bs, 
                       const Border* b,
                    const BrainModelSurface::SURFACE_TYPES borderFileSurfaceTypeIn);
      
      /// Constructor for a border associated with a surface
      BrainModelBorder(BrainSet* bs,
                       const BrainModelSurface* bms,
                       const Border* b);
                       
      /// Constructor for a border from a border projection file
      BrainModelBorder(BrainSet* bs,
                       BorderProjection* bp);
      
      /// Copy constructor
      BrainModelBorder(const BrainModelBorder& bmb);
      
      /// Destructor
      ~BrainModelBorder();
      
      /// assignment
      void operator=(const BrainModelBorder& bmb);
      
      /// add a link to this border
      void addBorderLink(const BrainModelBorderLink& bmbl);
      
      /// compute flat normals for this model
      void computeFlatNormals(const BrainModelSurface* bms);

      ///Copy the border to a border file border (user should destroy returned Border*)
      Border* copyToBorderFileBorder(const BrainModelSurface* bms) const;
      
      /// get the display flag
      bool getDisplayFlag() const { return displayFlag; }
      
      /// set the display flag
      void setDisplayFlag(const bool df) { displayFlag = df; }
      
      /// get the name display flag
      bool getNameDisplayFlag() const { return nameDisplayFlag; }
      
      /// set the name display flag
      void setNameDisplayFlag(const bool df) { nameDisplayFlag = df; }
      
      /// update the border for the addition of a new brain model
      void addBrainModel(const BrainModel* bm);
      
      /// update the border for the deletion of a brain model
      void deleteBrainModel(const BrainModel* bm);
      
      /// get the name of the border
      QString getName() const { return name; }
      
      /// set the name of the border
      void setName(const QString& s);
      
      /// get the sampling density
      float getSamplingDensity() const { return samplingDensity; }
      
      /// set the sampling density
      void setSamplingDensity(const float samplingDensity);
      
      /// get the variance
      float getVariance() const { return variance; }
      
      /// set the variance
      void setVariance(const float var);
      
      /// get the topography value
      float getTopography() const { return topographyValue; }
      
      /// set the topography value
      void setTopography(const float topography);
      
      /// get the areal uncertainty
      float getArealUncertainty() const { return arealUncertainty; }
      
      /// set the areal uncertainty
      void setArealUncertainty(const float uncertainty);
      
      /// get the border color index
      int getBorderColorFileIndex() const { return borderFileColorIndex; }
      
      /// set the border color index
      void setBorderColorFileIndex(const int indx) { borderFileColorIndex = indx; }
      
      /// get the area color file index
      int getAreaColorFileIndex() const { return areaColorFileIndex; }
      
      /// set the area color file index
      void setAreaColorFileIndex(const int indx) { areaColorFileIndex = indx; }
      
      /// get the border valid for a brain model
      bool getValidForBrainModel(const int brainModelIndex) const;
      
      /// get the type of the border (projection or border file)
      BORDER_TYPE getType() const { return borderType; }
      
      /// get the surface type of this border
      BrainModelSurface::SURFACE_TYPES getSurfaceType() const 
            { return borderFileSurfaceType; }
            
      /// set the type of the border (projection or border file)
      void setType(const BORDER_TYPE bt);
      
      /// get a border link
      BrainModelBorderLink* getBorderLink(const int index);
      
      /// get a border link (const method)
      const BrainModelBorderLink* getBorderLink(const int index) const;
      
      /// get the number of border links
      int getNumberOfBorderLinks() const { return borderLinks.size(); }
      
      /// delete a border link
      void deleteBorderLink(const int linkNumber);
      
      /// Determine the bounds of a border for a surface.
      void getBounds(const BrainModelSurface* bms, float bounds[6]) const;
      
      /// Determine the bounds of a border for a surface.
      void getBounds(const BrainModelSurface* bms, double bounds[6]) const;
      
      /// Unproject the border links for a surface
      void unprojectLinks(const BrainModelSurface* surface);
      
      /// reverse the order of links in a border
      void reverseLinks();
      
      /// orient the links in a border clockwise
      void orientLinksClockwise(const BrainModelSurface* bms);
      
      /// determine if a set of points are inside a polygon formed by a border
      void pointsInsideBorder(const BrainModelSurface* bms,
                                     const float* points, const int numPoints,
                                     std::vector<bool>& insideFlags,
                                     const bool checkNonNegativeZPointsOnly) const;
                                     
      /// resample the border to the specified density
      void resampleToDensity(const BrainModelSurface* bms,
                             const float density,
                             const int minimumNumberOfLinks,
                             int& newNumberOfLinks);
        
      /// resample the border to the specified number of links
      void resampleToNumberOfLinks(const BrainModelSurface* bms,
                                   const int newNumberOfLinks);
      
      /// set this brain model's link as modified
      void setModified(const int brainModelIndex, const bool mod);
      
      /// Set this projection as modified
      void setProjectionModified(const bool mod);
      
      /// get this brain model's link as modified
      bool getModified(const int brainModelIndex) const;
      
      /// get this projection as modified
      bool getProjectionModified() const { return projectionModified; }
       
      /// apply a transformation matrix to a brain model's borders
      void applyTransformationMatrix(const int brainModelIndex,
                                     const TransformationMatrix& tm);
             
      /// get highlight flag
      bool getHighlightFlag() const { return highlightFlag; }
      
      /// set highlight flag
      void setHighlightFlag(const bool hf)  { highlightFlag = hf; }
      
      /// get the link nearest the coordinate (returns -1 if found)
      int getLinkNearestCoordinate(const int brainModelIndex,
                                   const float xyz[3]) const;
                                    
   protected:
      /// initialize variables in this object
      void initialize(BrainSet* bs);
      
      /// brain set this border is part of
      BrainSet* brainSet;
      
      /// type of the border
      BORDER_TYPE borderType;
      
      /// if BORDER_TYPE_NORMAL type of surface this border associated with
      BrainModelSurface::SURFACE_TYPES borderFileSurfaceType;
      
      /// name of the border
      QString name;
      
      /// sampline density of this border
      float samplingDensity;

      /// variance of this border
      float variance;

      /// topography of this border
      float topographyValue;

      /// uncertainty of this border
      float arealUncertainty;

      /// index into border color file
      int borderFileColorIndex;

      /// index into area color file
      int areaColorFileIndex;
      
      /// border valid for each brain model
      std::vector<bool> brainModelValidity;
      
      /// border modified flags for each brain model
      std::vector<bool> brainModelModified;
      
      /// projection modified flags
      bool projectionModified;
      
      /// the border links
      std::vector<BrainModelBorderLink> borderLinks;
      
      /// display flag
      bool displayFlag;
      
      /// name display flag
      bool nameDisplayFlag;
      
      /// highlight border flag
      bool highlightFlag;
      
      // be sure to update copy constructor and operator = if members added
      
   friend class BrainModelBorderSet;
};

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/// class for info about border files that have been read
class BrainModelBorderFileInfo {
   public:
      /// constructor
      BrainModelBorderFileInfo() {}
      
      /// destructor
      ~BrainModelBorderFileInfo() { clear(); }
      
      /// Set the contents to a border file's values.
      void loadFromBorderFile(const AbstractFile& af);

      /// Load the contents into a border file
      void loadIntoBorderFile(AbstractFile& af) const;
      
      /// clear the contents
      void clear();
      
      /// get the comment
      QString getFileComment() const;

      /// set the file comment
      void setFileComment(const QString& comm);

      /// get the file name
      QString getFileName() const { return fileName; }
      
      /// set the file name
      void setFileName(const QString& name) { fileName = name; }
      
      /// get the file title
      QString getFileTitle() const { return fileTitle; }
      
      /// set the file title
      void setFileTitle(const QString& title) { fileTitle = title; }
      
      /// get the file header
      AbstractFile::AbstractFileHeaderContainer getFileHeader() const
                                                      { return fileHeader; }
      
      /// set the file header
      void setFileHeader(const AbstractFile::AbstractFileHeaderContainer& head) 
                                                      { fileHeader = head; }
      
      /// get the PubMed ID
      QString getPubMedID() const { return pubMedID; }
      
      /// set the PubMed ID
      void setPubMedID(const QString& s) { pubMedID = s; }
      
   protected:
      /// the file's name
      QString fileName;
      
      /// the file's title
      QString fileTitle;
      
      /// the file's header
      AbstractFile::AbstractFileHeaderContainer fileHeader;
      
      /// the file's PubMed ID
      QString pubMedID;
};
      
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/// class for storing all loaded borders and border projections
class BrainModelBorderSet {
   public:
      /// Constructor
      BrainModelBorderSet(BrainSet* bs);
      
      /// Destructor
      ~BrainModelBorderSet();

      /// add a border
      void addBorder(BrainModelBorder* border);
      
      /// add a brain model
      void addBrainModel(const BrainModel* bm);
      
      /// delete a brain model
      void deleteBrainModel(const BrainModel* bm);
      
      /// assign colors to the borders
      void assignColors();
      
      /// clear border highlighting
      void clearBorderHighlighting();
      
      /// compute the flat normals
      void computeFlatNormals(const BrainModel* bm);
      
      /// delete all borders (clears this data)
      void deleteAllBorders();
      
      /// delete a border
      void deleteBorder(const int borderIndex);
      
      /// delete a border link
      void deleteBorderLink(const int borderIndex, const int linkIndex);

      /// delete borders using any of the names.
      void deleteBordersWithNames(const std::vector<QString>& names);
      
      /// delete all projection borders
      void deleteBorderProjections();
      
      /// get the number of borders
      int getNumberOfBorders() const { return borders.size(); }
      
      /// get a border
      BrainModelBorder* getBorder(const int index);
      
      /// get a border (const method)
      const BrainModelBorder* getBorder(const int index) const;
      
      /// Orient all borders displayed on the model clockwise
      void orientDisplayedBordersClockwise(const BrainModel* bm);
      
      /// project borders for a surface
      void projectBorders(const BrainModelSurface* bms,
                          const bool barycentricMode = true,
                          const int firstBorderToProject = -1,
                          const int lastBorderToProject = -1);
      
      /// unproject borders for a surface
      void unprojectBorders(const BrainModelSurface* bms,
                            const int firstBorderToProject = -1,
                            const int lastBorderToProject = -1);
      
      /// unproject borders for all surfaces
      void unprojectBordersForAllSurfaces(const int firstBorderToProject = -1,
                                          const int lastBorderToProject = -1);
      
      /// copy the borders from border file
      void copyBordersFromBorderFile(const BorderFile* borderFile,
                           const BrainModelSurface::SURFACE_TYPES typeIn);
                           
      /// copy the borders from border file
      void copyBordersFromBorderFile(const BrainModelSurface* bms,
                                     const BorderFile* borderFile);
                           
      /// copy the borders from a border projection file
      void copyBordersFromBorderProjectionFile(BorderProjectionFile* borderProjFile);
      
      /// copy borders used by a surface to a border file
      void copyBordersToBorderFile(const BrainModelSurface* bms,
                                   BorderFile& borderFile) const;
         
      /// copy borders that are of the specified type
      BorderFile* copyBordersOfSpecifiedType(const BrainModelSurface::SURFACE_TYPES surfaceType) const;
                                      
      /// copy borders to a border projection file
      void copyBordersToBorderProjectionFile(BorderProjectionFile& borderProjFile) const;
                                   
      /// interpolate two borders to create new borders (input borders will be resampled too)
      void createInterpolatedBorders(const BrainModelSurface* bms,
                                     const int border1Index,
                                     const int border2Index,
                                     const QString& namePrefix,
                                     const int numberOfNewBorders,
                                     const float sampling,
                                     QString& errorMessageOut);
                                     
      /// Resample displayed borders for the model to the specified density.
      void resampleDisplayedBorders(const BrainModel* bm,
                                    const float density);
   
      /// Reverse displayed borders for model.
      void reverseDisplayedBorders(const BrainModel* bm);
   
      /// get the unique names of all borders
      void getAllBorderNames(std::vector<QString>& names,
                             const bool reverseOrderFlag);
      
      /// get indeces of borders with name
      void getAllBordersWithName(const QString& nameIn,
                                 std::vector<int>& indicesOut) const;
                                 
      /// copy a border
      void copyBorder(const int borderToCopyIndex,
                      const QString& nameForCopiedBorder);
                      
      /// See if a surface's borders are modified.
      bool getSurfaceBordersModified(const BrainModelSurface* bms) const; 
      
      ///Set a surface's borders as modified.
      void setSurfaceBordersModified(const BrainModelSurface* bms,
                                               const bool mod);   
      /// See if any projections are modified.
      bool getProjectionsModified() const;
      
      /// Set projections modified.
      void setProjectionsModified(const bool mod);
   
      ///  Set the modification status of all borders and the projections
      void setAllModifiedStatus(const bool mod);
      
      /// Set display flag for borders with specified name
      void setNameDisplayFlagForBordersWithName(const QString& name,
                                                const bool flag);

      /// get the border file info
      BrainModelBorderFileInfo* getBorderFileInfo(const BrainModelSurface::SURFACE_TYPES st);
      
      /// get the border file info
      const BrainModelBorderFileInfo* getBorderFileInfo(const BrainModelSurface::SURFACE_TYPES st) const;
      
      /// set the border file info
      void setBorderFileInfo(const BrainModelSurface::SURFACE_TYPES st,
                             const BrainModelBorderFileInfo& fileInfo);
         
      /// get the border projection file info
      BrainModelBorderFileInfo* getBorderProjectionFileInfo() { return &fileInfoProjection; }
      
      /// get the border projection file info (const method)
      const BrainModelBorderFileInfo* getBorderProjectionFileInfo() const { return &fileInfoProjection; }
      
      /// set the border projection file info
      void setBorderProjectionFileInfo(const BrainModelBorderFileInfo& fileInfo)
                  { fileInfoProjection = fileInfo; }
       
      /// get the volume's borders
      BorderFile* getVolumeBorders() { return &volumeBorders; }
      
      /// get the volume's borders (const method)
      const BorderFile* getVolumeBorders() const { return &volumeBorders; }
      
      /// copy the volume borders to the fiducial borders
      void copyVolumeBordersToFiducialBorders();
      
      /// apply a transformation matrix to a brain model's borders
      void applyTransformationMatrix(const BrainModelSurface* bms,
                                     const TransformationMatrix& tm);
          
      /// set default file names if they are empty
      void setDefaultFileNames();
      
      /// find border and links nearest 3D coordinate (returns true if found)
      bool findBorderAndLinkNearestCoordinate(const BrainModelSurface* bms,
                                              const float xyz[3],
                                              int& borderNumberOut,
                                              int& borderLinkOut) const;
      
      /// update border mode
      enum UPDATE_BORDER_MODE {
         /// update border mode none
         UPDATE_BORDER_MODE_NONE,
         /// update border mode replace segment in middle of border
         UPDATE_BORDER_MODE_REPLACE_SEGMENT_IN_MIDDLE_OF_BORDER,
         /// update border erase
         UPDATE_BORDER_MODE_ERASE,
         /// update border mode extend border from end
         UPDATE_BORDER_MODE_EXTEND_BORDER_FROM_END
      };
      
      /// update a border with a new segment 
      void updateBorder(const BrainModelSurface* bms,
                        const UPDATE_BORDER_MODE updateMode,
                        Border* newBorderSegment,
                        const float samplingDensity,
                        const bool projectBorderFlag,
                        QString& errorMessageOut);
                        
   protected:
      /// brain set using this object
      BrainSet* brainSet;
      
      /// the borders
      std::vector<BrainModelBorder*> borders;
      
      /// header information for border projection file
      BrainModelBorderFileInfo fileInfoProjection;

      /// header information for raw borders
      BrainModelBorderFileInfo fileInfoRaw;

      /// header information for fiducial borders
      BrainModelBorderFileInfo fileInfoFiducial;

      /// header information for inflated borders
      BrainModelBorderFileInfo fileInfoInflated;

      /// header information for very inflated borders
      BrainModelBorderFileInfo fileInfoVeryInflated;

      /// header information for spherical borders
      BrainModelBorderFileInfo fileInfoSpherical;

      /// header information for ellipsoidal borders
      BrainModelBorderFileInfo fileInfoEllipsoidal;

      /// header information for compressed medial wall borders
      BrainModelBorderFileInfo fileInfoCompMedWall;

      /// header information for flat borders
      BrainModelBorderFileInfo fileInfoFlat;

      /// header information for lobar flat borders
      BrainModelBorderFileInfo fileInfoLobarFlat;

      /// header information for lobar flat borders
      BrainModelBorderFileInfo fileInfoHull;

      /// header information for unknown borders
      BrainModelBorderFileInfo fileInfoUnknown;

      /// the volume's borders
      BorderFile volumeBorders;
};

#endif // __BRAIN_MODEL_BORDER_SET_H__

