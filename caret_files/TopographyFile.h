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


#ifndef __VE_TOPOGRAPHY_FILE_H__
#define __VE_TOPOGRAPHY_FILE_H__

#include "NodeAttributeFile.h"

class DeformationMapFile;
class TopographyFile;

//#include "DeformationMapFile.h"

/// Class for storing topography data associated with each node.
class NodeTopography {
   private:
      /// topography file this Node Topography belongs to
      TopographyFile* topographyFile;
      
      /// name of topographic area
      QString name;

      /// mean of eccentricity
      float eMean;
  
      /// low of eccentricity
      float eLow;

      /// high of eccentricity
      float eHigh;

      /// mean of polar angle
      float pMean; 

      /// low of polar angle
      float pLow;

      /// high of polar angle
      float pHigh;
      
   public:
      /// constructor
      NodeTopography();
      
      /// constructor
      NodeTopography(const float eccentricityMean, const float eccentricityLow,
                     const float eccentricityHigh,
                     const float polarMean, const float polarLow,
                     const float polarHigh, const QString& areaName);
                 
      /// get topography data
      void getData(float& eccentricityMean, float& eccentricityLow,
                   float& eccentricityHigh,
                   float& polarMean, float& polarLow,
                   float& polarHigh, QString& areaName) const;

      /// set topography data
      void setData(const float eccentricityMean, const float eccentricityLow,
                   const float eccentricityHigh,
                   const float polarMean, const float polarLow,
                   const float polarHigh, const QString& areaName);
   
   friend class TopographyFile;              
};

/// class for reading/writing/storing a Topography File
class TopographyFile : public NodeAttributeFile {
   private:
      /// Store the topography data here
      std::vector<NodeTopography> topography;
      
      /// number of nodes for reading a version 0 file
      int numberOfNodesVersion0;
      
      /// get an offset into topography
      //int getOffset(const int nodeNumber, const int columnNumber) const;
      
      /// read a version 0 file
      void readFileDataVersion0(QTextStream& stream) throw (FileException);  

      /// read a version 1 file
      void readFileDataVersion1(QTextStream& stream) throw (FileException);   
      
      /// read a Topography File
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write a Topography File
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);

   public:
      /// constructor for TopographyFile
      TopographyFile();
           
      /// destructor for TopographyFile
      ~TopographyFile();
      
      /// append a node attribute  file to this one
      void append(NodeAttributeFile& naf) throw (FileException);

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      /// add columns to this topography file
      void addColumns(const int numberOfNewColumns);
      
      /// Add nodes to the file
      void addNodes(const int numberOfNodesToAdd);
      
      /// add topography for a node
      void setNodeTopography(const int nodeNumber, const int columnNumber, const NodeTopography& nt);
      
      /// clear the current data
      void clear();
      
      /// deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                      NodeAttributeFile& deformedFile,
                      const DEFORM_TYPE dt) const throw (FileException);
      
      /// reset a column of data
      void resetColumn(const int columnNumber);
      
      /// remove a column of data
      void removeColumn(const int columnNumber);
      
      /// get the minimum and maximum values for each category
      void getMinMaxTopography(const int columnNumber,
                               float eccentricityMean[2], 
                               float eccentricityLow[2],
                               float eccentricityHigh[2],
                               float polarMean[2], 
                               float polarLow[2],
                               float polarHigh[2]) const;
                               
      /// set the number of nodes and columns
      void setNumberOfNodesAndColumns(const int numNodes, const int numColumns);
      
      /// set the number of nodes for reading a version 0 topography file
      void setNumberOfNodesVersion0File(const int numNodes) { numberOfNodesVersion0 = numNodes; }
      
      /// get topography for a specified node and column
      NodeTopography getNodeTopography(const int nodeNumber, const int columnNumber) const;
      
   friend class NodeTopography;
};

#ifdef _TOPOGRAPHY_FILE_MAIN_

#endif // _TOPOGRAPHY_FILE_MAIN_


#endif // __VE_TOPOGRAPHY_FILE_H__
