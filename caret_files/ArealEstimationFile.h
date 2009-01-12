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


#ifndef __AREAL_ESTIMATION_FILE__
#define __AREAL_ESTIMATION_FILE__

class ArealEstimationFile;
class DeformationMapFile;

#include "NodeAttributeFile.h"

///  Areal Estimation Data for a node
class ArealEstimationNode {
   private:
      /// Areal Estimation File this node belongs to
      ArealEstimationFile* arealEstimationFile;
      
      /// area names for a node
      int areaNamesIndex[4];
      
      /// probability of each area for anode
      float areaProbability[4];
   
   public:
      /// Constructor
      ArealEstimationNode();
      
      /// Destructor
      ~ArealEstimationNode();
      
      /// reset the data
      void reset();
      
      /// Get the data
      void getData(int areaNamesIndexOut[4], 
                   float areaProbabilityOut[4]) const;
      
      /// Set the data
      void setData(const int areaNamesIndexIn[4], 
                   const float  areaProbabilityIn[4]);
                   
   friend class ArealEstimationFile;
};

/// Areal Estimation File ("fuzzy" borders)
class ArealEstimationFile : public NodeAttributeFile {
   private:
      /// area names (common to all columns)
      std::vector<QString> areaNames;
      
      /// areal estimation data for all nodes
      std::vector<ArealEstimationNode> nodeData;
      
      /// long name for columns
      std::vector<QString> longName;
      
      /// long name for reading version 1 file
      QString version1LongName;
      
      /// short name for reading version 1 file
      QString version1ShortName;
      
      /// read version 2 of the file
      void readFileDataVersion2(QFile& file, QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      /// read version 1 of the file
      void readFileDataVersion1(QTextStream& stream) throw (FileException);
      
      void readTags(QTextStream& stream, const int fileVersion) throw (FileException);
      
      static const QString tagLongName;

   public:
      /// Constructor
      ArealEstimationFile();
      
      /// Destructor
      ~ArealEstimationFile();
      
      /// Add an area name
      int addAreaName(const QString& name);
      
      /// append a node attribute  file to this one
      void append(NodeAttributeFile& naf) throw (FileException);

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      
      /// Add isEmpty columns to this areal estimation file
      void addColumns(const int numberOfNewColumns);
      
      /// Add nodes to the file
      void addNodes(const int numberOfNodesToAdd);
      
      /// Clear current file in memory
      void clear();
      
      /// deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                  NodeAttributeFile& deformedFile,
                  const DEFORM_TYPE dt) const throw (FileException);

      /// reset a column of data
      void resetColumn(const int columnNumber);
      
      /// remove a column of data
      void removeColumn(const int columnNumber);
      
      /// Get an area name
      QString getAreaName(const int index) const;
      
      /// Get the long name for a column
      QString getLongName(const int columnNumber) const { return longName[columnNumber]; }
      
      /// Get the node's data with string names
      void getNodeData(const int nodeNumber,
                       const int columnNumber,
                       QString areaNamesOut[4], 
                       float areaProbabilityOut[4]) const;
      
      /// Get the node's data with name indices
      void getNodeData(const int nodeNumber,
                       const int columnNumber,
                       int areaNamesIndexOut[4], 
                       float areaProbabilityOut[4]) const;
                       
      /// Get number of area names
      inline int getNumberOfAreaNames() const { return areaNames.size(); }
      
      /// Set the long name
      void setLongName(const int columnNumber, const QString& ln);
      
      /// Set the node's data with names
      void setNodeData(const int nodeNumber,
                       const int columnNumber,
                       const QString areaNamesIn[4], 
                       const float  areaProbabilityIn[4]);
                       
      /// Set the node's data with name indices
      void setNodeData(const int nodeNumber,
                       const int columnNumber,
                       const int areaNamesIndexIn[4], 
                       const float areaProbabilityIn[4]);
                       
      /// Set Number of Nodes - clears node data.  area names unchanged
      void setNumberOfNodesAndColumns(const int numNodes, const int numCols);
      
      /// Read data from the filepointer (header has already been read)
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write data to the filepointer (header has already been written)
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */)  throw (FileException);
      
   friend class ArealEstimationNode;
};

#endif // __AREAL_ESTIMATION_FILE__

#ifdef __AREAL_ESTIMATION_FILE_DEFINE__
const QString ArealEstimationFile::tagLongName = "tag-long-name";
#endif // __AREAL_ESTIMATION_FILE_DEFINE__
