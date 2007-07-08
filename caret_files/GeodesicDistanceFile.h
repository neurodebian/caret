
#ifndef __GEODESIC_DISTANCE_FILE_H__
#define __GEODESIC_DISTANCE_FILE_H__

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

#include "NodeAttributeFile.h"

/// class for storing geodesic distance file
class GeodesicDistanceFile : public NodeAttributeFile {
   public:
      /// Constructor
      GeodesicDistanceFile();
      
      /// Destructor
      ~GeodesicDistanceFile();
      
      /// append a node attribute  file to this one
      void append(NodeAttributeFile& naf) throw (FileException);

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      /// add columns to this node attribute file
      void addColumns(const int numberOfNewColumns);

      /// add nodes to this file
      void addNodes(const int numberOfNodesToAdd);
      
      /// Clears current file data in memory. 
      void clear();
      
      /// deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                      NodeAttributeFile& deformedFile,
                      const DEFORM_TYPE dt) const throw (FileException);
      
      /// set the number of nodes and columns in the file
      void setNumberOfNodesAndColumns(const int numNodes, const int numCols);
      
      /// reset a column of data
      void resetColumn(const int columnNumber);
      
      /// remove a column of data
      void removeColumn(const int columnNumber);
      
      /// get the parent for a node
      int getNodeParent(const int nodeNumber, const int columnNumber) const;
      
      /// set the parent for a node
      void setNodeParent(const int nodeNumber, const int columnNumber,
                         const int parent);
                         
      /// get the distance to a parent for a node
      float getNodeParentDistance(const int nodeNumber, const int columnNumber) const;
      
      /// set the distance to a parent for a node
      void setNodeParentDistance(const int nodeNumber, const int columnNumber,
                         const float distance);
      
      /// get the root node
      int getRootNode(const int columnNumber) const;
      
      /// set the root node
      void setRootNode(const int columnNumber, const int node);
      
   protected:
      /// Read the contents of the file (header has already been read)
      void readFileData(QFile& file, 
                                QTextStream& stream,
                                QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// root node of geodesic
      std::vector<int> rootNode;
      
      /// parent for node in path that leads to root node
      std::vector<int> nodeParent;
      
      // distance to parent in path that leads to root node
      std::vector<float> nodeParentDistance;
};

#endif // __GEODESIC_DISTANCE_FILE_H__

