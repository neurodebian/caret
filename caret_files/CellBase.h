
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
#include "StudyMetaDataLink.h"

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
      
      /// get the study metadata link
      StudyMetaDataLink getStudyMetaDataLink() const { return studyMetaDataLink; }
      
      /// set the study metadata link
      void setStudyMetaDataLink(const StudyMetaDataLink smdl);
            
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
      
   protected:
      /// called to read from an XML structure
      virtual void readXML(QDomNode& node) throw (FileException);
      
      /// called to write to an XML structure
      virtual void writeXML(QDomDocument& xmlDoc,
                            QDomElement&  parentElement);
                    
      /// called when a "set" method is called
      virtual void setModified() = 0;
      
      /// position of cell
      float xyz[3];
      
      /// section number
      int sectionNumber;
      
      /// name of cell
      QString name;
      
      /// study number
      int studyNumber;
      
      /// the link to study metadata
      StudyMetaDataLink studyMetaDataLink;
      
      /// geography
      QString geography;
      
      /// area
      QString area;
      
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
     
      /// signed distance above the surface (positive if above surface, negative if below)
      float signedDistanceAboveSurface;
      
      /// structure of cell
      Structure structure;
      
      /// highlight flag
      bool highlightFlag;
};

#endif // __CELL_BASE__

