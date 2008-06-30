
#ifndef __CELL_BASE__
#define __CELL_BASE__

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

#include <QString>

#include "Structure.h"
#include "StudyMetaDataLinkSet.h"

class QDomDocument;
class QDomElement;
class QDomNode;

/// base class for CellData and CellProjection
class CellBase {
   public:
      /// Coloring mode
      enum CELL_COLOR_MODE {
         /// color by cell name
         CELL_COLOR_MODE_NAME,
         /// color by class name
         CELL_COLOR_MODE_CLASS
      };
      
      // constructor
      CellBase();
      
      // destructor
      virtual ~CellBase();
      
      // copy the data from another cell base class
      void copyData(const CellBase& cb);
      
      // initialize a cell
      virtual void initialize();
      
      /// get xyz
      const float* getXYZ() const { return &xyz[0]; }
      
      /// get xyz
      void getXYZ(float xyzOut[3]) const;
      
      // set xyz
      void setXYZ(const float xyzIn[3]);
      
      // set xyz
      void setXYZ(const float x, const float y, const float z);
      
      /// get search xyz
      const float* getSearchXYZ() const { return &searchXYZ[0]; }
      
      /// get search xyz
      void getSearchXYZ(float xyzOut[3]) const;
      
      // set search xyz
      void setSearchXYZ(const float xyzIn[3]);
      
      // set search xyz
      void setSearchXYZ(const float x, const float y, const float z);
      
      /// get name
      QString getName() const { return name; }
      
      // set name
      virtual void setName(const QString& n);
      
      /// get section number
      int getSectionNumber() const { return sectionNumber; }
      
      // set section number
      void setSectionNumber(const int sn);
      
      /// get study number
      int getStudyNumber() const { return studyNumber; }
      
      // set study number
      void setStudyNumber(const int sn);
      
      /// get the study metadata link set
      StudyMetaDataLinkSet getStudyMetaDataLinkSet() const { return studyMetaDataLinkSet; }
      
      /// set the study metadata link set
      void setStudyMetaDataLinkSet(const StudyMetaDataLinkSet smdls);
            
      /// get region of interest
      QString getRegionOfInterest() const { return regionOfInterest; }
      
      /// set region of interest
      void setRegionOfInterest(const QString& roi);
      
      /// get geography
      QString getGeography() const { return geography; }
      
      // set geography
      void setGeography(const QString& g);
      
      /// get area
      QString getArea() const { return area; }
      
      // set area
      void setArea(const QString& a);
      
      /// get size
      float getSize() const { return size; }
      
      // set size
      void setSize(const float s);
      
      /// get statistic
      QString getStatistic() const { return statistic; }
      
      // set statistic
      void setStatistic(const QString& s);
      
      /// get comment
      QString getComment() const { return comment; }
      
      // set comment
      void setComment(const QString& c);
      
      /// get the display flag
      bool getDisplayFlag() const { return displayFlag; }
      
      // set the display flag 
      void setDisplayFlag(const bool df);
      
      /// get the color index
      int getColorIndex() const { return colorIndex; }
      
      /// set the color index
      void setColorIndex(const int ci);
          
      /// Get the cell's class name
      QString getClassName() const { return className; }
      
      /// Set the cell's class name
      virtual void setClassName(const QString& name) = 0;
      
      /// Get the index of the class
      int getClassIndex() const { return classIndex; }
      
      /// Get the flag used for some operations
      bool getSpecialFlag() const { return specialFlag; }
      
      /// Set the flag used for some operations
      void setSpecialFlag(const bool value) { specialFlag = value; }
      
      /// get flag to indicate if cell is within search parameters
      bool getInSearchFlag() const { return inSearchFlag; }
      
      /// set flag to indicate if cell is within search parameters
      void setInSearchFlag(const bool b) { inSearchFlag = b; }
      
      /// get the signed distance above the surface (positive = above surface)
      float getSignedDistanceAboveSurface() const { return signedDistanceAboveSurface; }
      
      /// set the signed distance above the surface (positive = above surface)
      void setSignedDistanceAboveSurface(const float dist) { signedDistanceAboveSurface = dist; }
      
      /// get the structure
      Structure::STRUCTURE_TYPE getCellStructure() const { return structure.getType(); }
      
      /// set the structure
      void setCellStructure(const Structure::STRUCTURE_TYPE cst);
      
      /// update the cell's invalid structure using the X coordinate if it is not zero
      void updateInvalidCellStructureUsingXCoordinate();
      
      /// copy data
      void copyCellBaseData(const CellBase& cb, const bool copyXYZ = false);
      
      /// get the highlight flag
      bool getHighlightFlag() const { return highlightFlag; }
      
      /// set the highlight flag
      void setHighlightFlag(const bool value) { highlightFlag = value; }
      
      /// get the SuMS repeat number
      QString getSumsRepeatNumber() const { return sumsRepeatNumber; }
      
      /// set the SuMS repeat number
      void setSumsRepeatNumber(const QString& s);
      
      /// get the SuMS ID number
      QString getSumsIDNumber() const { return sumsIDNumber; }
      
      /// set the SuMS ID number
      void setSumsIDNumber(const QString& s);
      
      /// get the SuMS parent cell base ID
      QString getSumsParentCellBaseID() const { return sumsParentCellBaseID; }
      
      /// set the SuMS parent cell base ID
      void setSumsParentCellBaseID(const QString& s);
      
   protected:
      /// called to read from an XML DOM structure
      virtual void readXMLWithDOM(QDomNode& node) throw (FileException);
      
      /// called to write to an XML structure
      virtual void writeXML(QDomDocument& xmlDoc,
                            QDomElement&  parentElement);
                    
      /// called when a "set" method is called
      virtual void setModified() = 0;
      
      /// position of cell
      float xyz[3];
      
      /// position of cell for searching
      float searchXYZ[3];
      
      /// section number
      int sectionNumber;
      
      /// name of cell
      QString name;
      
      /// study number
      int studyNumber;
      
      /// the link to study metadata
      StudyMetaDataLinkSet studyMetaDataLinkSet;
      
      /// geography
      QString geography;
      
      /// area
      QString area;
      
      /// region of interest
      QString regionOfInterest;
      
      /// size
      float size;
      
      /// statistic
      QString statistic;
      
      /// comment
      QString comment;
      
      /// the display flag
      bool displayFlag;
      
      /// the color index
      int colorIndex;
      
      /// class name of cell
      QString className;
      
      /// index of class name
      int classIndex;
      
      /// flag used for some operations
      bool specialFlag;
     
      /// flag to indicate if cell is within search parameters
      bool inSearchFlag;
      
      /// signed distance above the surface (positive if above surface, negative if below)
      float signedDistanceAboveSurface;
      
      /// structure of cell
      Structure structure;
      
      /// highlight flag
      bool highlightFlag;
      
      /// SuMS ID number
      QString sumsIDNumber;
      
      /// SuMS repeat number
      QString sumsRepeatNumber;
      
      /// SuMS parent cell base ID
      QString sumsParentCellBaseID;
      
      /// tag for reading and writing cells
      static const QString tagCellBase;
      
      /// tag for reading and writing cells
      static const QString tagXYZ;
      
      /// tag for reading and writing cells
      static const QString tagSearchXYZ;
      
      /// tag for reading and writing cells
      static const QString tagSectionNumber;
      
      /// tag for reading and writing cells
      static const QString tagName;
      
      /// tag for reading and writing cells
      static const QString tagStudyNumber;
      
      /// tag for reading and writing cells
      static const QString tagGeography;
      
      /// tag for reading and writing cells
      static const QString tagArea;
      
      /// tag for reading and writing cells
      static const QString tagRegionOfInterest;
      
      /// tag for reading and writing cells
      static const QString tagSize;
      
      /// tag for reading and writing cells
      static const QString tagStatistic;
      
      /// tag for reading and writing cells
      static const QString tagComment;
      
      /// tag for reading and writing cells
      static const QString tagClassName;
      
      /// tag for reading and writing cells
      static const QString tagSignedDistanceAboveSurface;
      
      /// tag for reading and writing cells
      static const QString tagStructure; 
      
      /// tag for reading and writing cells
      static const QString tagSumsIDNumber;
      
      /// tag for reading and writing cells
      static const QString tagSumsRepeatNumber;
      
      /// tag for reading and writing cells
      static const QString tagSumsParentCellBaseID;                 
};

#ifdef __CELL_BASE_MAIN__
      const QString CellBase::tagCellBase = "CellBase";
      const QString CellBase::tagXYZ = "xyz";
      const QString CellBase::tagSearchXYZ = "SearchXYZ";
      const QString CellBase::tagSectionNumber = "sectionNumber";
      const QString CellBase::tagName = "name";
      const QString CellBase::tagStudyNumber = "studyNumber";
      const QString CellBase::tagGeography = "geography";
      const QString CellBase::tagArea = "area";
      const QString CellBase::tagRegionOfInterest = "regionOfInterest";
      const QString CellBase::tagSize = "size";
      const QString CellBase::tagStatistic = "statistic";
      const QString CellBase::tagComment = "comment";
      const QString CellBase::tagClassName = "className";
      const QString CellBase::tagSignedDistanceAboveSurface = "signedDistanceAboveSurface";
      const QString CellBase::tagStructure = "structure"; 
      const QString CellBase::tagSumsIDNumber = "tagSumsIDNumber";
      const QString CellBase::tagSumsRepeatNumber = "tagSumsRepeatNumber";
      const QString CellBase::tagSumsParentCellBaseID = "tagSumsParentCellBaseID";
#endif // __CELL_BASE_MAIN__

#endif // __CELL_BASE__

