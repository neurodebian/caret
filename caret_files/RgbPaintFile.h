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


#ifndef __VE_RGB_PAINT_FILE_H__
#define __VE_RGB_PAINT_FILE_H__

#include "NodeAttributeFile.h"

class DeformationMapFile;
class vtkPolyData;

/// RGB Paint File - associates RGB Values with each node
/**
 *
 */
class RgbPaintFile : public NodeAttributeFile {
   private:
    
      /// title for the red component
      std::vector<QString> redComponentTitle;
      
      /// title for the green component
      std::vector<QString> greenComponentTitle;
      
      /// title for the blue component
      std::vector<QString> blueComponentTitle;
      
      /// comment for each red component
      std::vector<QString> redComponentComment;
      
      /// comment for each green component
      std::vector<QString> greenComponentComment;
      
      /// comment for each blue component
      std::vector<QString> blueComponentComment;
      
      /// red column scaling minimum
      std::vector<float> redComponentScaleMin;

      /// green column scaling minimum
      std::vector<float> greenComponentScaleMin;

      /// blue column scaling minimum
      std::vector<float> blueComponentScaleMin;

      /// red column scaling maximum
      std::vector<float> redComponentScaleMax;

      /// green column scaling maximum
      std::vector<float> greenComponentScaleMax;

      /// blue column scaling maximum
      std::vector<float> blueComponentScaleMax;

      /// red color components
      std::vector<float> redComponents;
      
      /// green color components
      std::vector<float> greenComponents;
      
      /// blue color components
      std::vector<float> blueComponents;
      
      /// read rgb paint file data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
   
      /// read rgb paint file data for version 0 file
      void readFileDataVersion0(QFile& file, QTextStream& stream) throw (FileException);
   
      /// read rgb paint file data for version 1 file
      void readFileDataVersion1(QTextStream& stream) throw (FileException);
   
      /// read rgb paint file data for version 2 file
      void readFileDataVersion2(QTextStream& stream,
                                QDataStream& binStream) throw (FileException);
   
      /// write rgb paint file data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
   
      static const QString tagCommentRed;
      static const QString tagCommentGreen;
      static const QString tagCommentBlue;
      static const QString tagTitleRed;
      static const QString tagTitleGreen;
      static const QString tagTitleBlue;
      static const QString tagScaleRed;
      static const QString tagScaleGreen;
      static const QString tagScaleBlue;

   public:
   
      /// Constructor
      RgbPaintFile();
   
      /// Destructor
      ~RgbPaintFile();
   
      /// append a node attribute  file to this one
      void append(NodeAttributeFile& naf) throw (FileException);

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);
      
      /// add columns to this rgb paint file
      void addColumns(const int numberOfNewColumns);
      
      /// Add nodes to the file
      void addNodes(const int numberOfNodesToAdd);
      
      /// clear contents of this Rgb paint file
      void clear();
   
      /// deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                      NodeAttributeFile& deformedFile,
                      const DEFORM_TYPE dt) const throw (FileException);

      /// reset a column of data
      void resetColumn(const int columnNumber);
      
      /// remove a column of data
      void removeColumn(const int columnNumber);
      
      /// get the rgb paints for a node
      void getRgb(const int nodeNumber, const int columnNumber,
                  float& red, float& green, float& blue) const;
   
      /// get the title for red component
      QString getTitleRed(const int columnNumber) const { return redComponentTitle[columnNumber]; }
   
      /// get the title for green component
      QString getTitleGreen(const int columnNumber) const { return greenComponentTitle[columnNumber]; }
   
      /// get the title for blue component
      QString getTitleBlue(const int columnNumber)  const { return blueComponentTitle[columnNumber]; }
   
      /// get the comments for red component
      QString getCommentRed(const int columnNumber) const;
   
      /// get the comments for green component
      QString getCommentGreen(const int columnNumber) const;
   
      /// get the comments for blue component
      QString getCommentBlue(const int columnNumber) const;
   
      /// get the scale for red component
      void getScaleRed(const int columnNumber, float& minScale, float& maxScale) const;
   
      /// get the scale for green component
      void getScaleGreen(const int columnNumber, float& minScale, float& maxScale) const;
   
      /// get the scale for blue component
      void getScaleBlue(const int columnNumber, float& minScale, float& maxScale) const;
      
      /// import colors from vtk poly data
      void importFromVtkFile(vtkPolyData* polyData);
      
      /// Import colors from a SUMA file.
      void importFromSuma(const QString& name) throw (FileException);
      
      /// set the rgb paints for a node
      void setRgb(const int nodeNumber, const int columnNumber,
                  const float red, const float green, const float blue);
   
      /// set the rgb paints for a node
      void setRgb(const int nodeNumber, const int columnNumber,
                  const double red, const double green, const double blue);
   
      /// set the title for a red column
      void setTitleRed(const int columnNumber, const QString& title);  

      /// set the title for a green column
      void setTitleGreen(const int columnNumber, const QString& title); 

      /// set the title for a blue column
      void setTitleBlue(const int columnNumber, const QString& title);  
   
      /// set the comment for a red column
      void setCommentRed(const int columnNumber, const QString& comment); 

      /// set the comment for a green column
      void setCommentGreen(const int columnNumber, const QString& comment); 

      /// set the comment for a blue column
      void setCommentBlue(const int columnNumber, const QString& comment);
      

      /// set the scale for a red column component
      void setScaleRed(const int columnNumber, const float minScale, const float maxScale);

      /// set the scale for a green column component
      void setScaleGreen(const int columnNumber, const float minScale, const float maxScale);

      /// set the scale for a blue column component
      void setScaleBlue(const int columnNumber, const float minScale, const float maxScale);
   
      /// set the number of nodes and column
      void setNumberOfNodesAndColumns(const int numNodes, const int numColumns);
};

#ifdef _RGB_PAINT_FILE_MAIN_

   const QString RgbPaintFile::tagCommentRed = "tag-comment-red";
   const QString RgbPaintFile::tagCommentGreen = "tag-comment-green";
   const QString RgbPaintFile::tagCommentBlue = "tag-comment-blue";
   const QString RgbPaintFile::tagTitleRed = "tag-title-red";
   const QString RgbPaintFile::tagTitleGreen = "tag-title-green";
   const QString RgbPaintFile::tagTitleBlue = "tag-title-blue";
   const QString RgbPaintFile::tagScaleRed = "tag-scale-red";
   const QString RgbPaintFile::tagScaleGreen = "tag-scale-green";
   const QString RgbPaintFile::tagScaleBlue = "tag-scale-blue";

#endif // _RGB_PAINT_FILE_MAIN_

#endif  // __RGB_PAINT_FILE_H__
