
#ifndef __DEFORMATION_FIELD_FILE_H__
#define __DEFORMATION_FIELD_FILE_H__

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

/// class for storing deformed barycentric location
class DeformationFieldNodeInfo {
   public:
      /// constructor
      DeformationFieldNodeInfo();
      
      /// destructor
      ~DeformationFieldNodeInfo();
      
      /// get the data
      void getData(int tileNodesOut[3], float tileBarycentricAreasOut[3]) const;
      
      /// set the data
      void setData(const int tileNodesIn[3], 
                   const float tileBarycentricAreasIn[3]);
      
      /// reset the data
      void reset();
      
   protected:                   
      /// nodes of tile containing deformed position
      int tileNodes[3];
      
      /// barycentric areas of tile containing deformed position
      float tileBarycentricAreas[3];
};
      
/// class stores deformation field for a surface
class DeformationFieldFile : public NodeAttributeFile {
   public:
      /// Constructor
      DeformationFieldFile();
      
      /// Destructor
      ~DeformationFieldFile();
      
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
      
      /// get deformation field info for a node
      DeformationFieldNodeInfo* getDeformationInfo(const int node, const int column);
      
      /// get deformation field info for a node (const method)
      const DeformationFieldNodeInfo* getDeformationInfo(const int node, 
                                                         const int column) const;
      
      /// get the name of the pre-deformed coordinate file
      QString getPreDeformedCoordinateFileName(const int columnNumber) const;
            
      /// set the name of the pre-deformed coordinate file
      void setPreDeformedCoordinateFileName(const int columnNumber, const QString& name);
      
      /// get the name of the deformed coordinate file
      QString getDeformedCoordinateFileName(const int columnNumber) const;
      
      /// set the name of the deformed coordinate file
      void setDeformedCoordinateFileName(const int columnNumber, const QString& name);
      
      /// get the name of the deform topology file
      QString getDeformedTopologyFileName(const int columnNumber) const;
      
      /// set the name of the deform topology file
      void setDeformedTopologyFileName(const int columnNumber, const QString& name);
      
      /// get the name of the topology file
      QString getTopologyFileName(const int columnNumber) const;
      
      /// set the name of the topology file
      void setTopologyFileName(const int columnNumber, const QString& name);
      
      /// get the name of the coordinate file
      QString getCoordinateFileName(const int columnNumber) const;
      
      /// set the name of the coordinate file
      void setCoordinateFileName(const int columnNumber, const QString& name);
      
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

   protected:
      /// vector endpoints
      std::vector<DeformationFieldNodeInfo> deformNodeInfo;
      
      /// the deforming topology files
      std::vector<QString> deformedTopologyFileName;
      
      /// the original coordinate file names
      std::vector<QString> preDeformedCoordinateFileName;
      
      /// the deformed coordinate file names
      std::vector<QString> deformedCoordinateFileName;
      
      /// coordinate file on which map computed
      std::vector<QString> coordinateFileName;
      
      /// topology file on which map computed
      std::vector<QString> topologyFileName;
      
      /// pre-deformed coordinate file tag
      static const QString tagPreDeformedCoordinateFile;
      
      /// deformed coordinate file tag
      static const QString tagDeformedCoordinateFile;
      
      /// deformed topology file tag
      static const QString tagDeformedTopologyFile;

      /// coordinate file tag
      static const QString tagCoordinateFile;
      
      /// topology file tag
      static const QString tagTopologyFile;
};

#ifdef __DEFORMATION_FIELD_FILE_MAIN__
   const QString DeformationFieldFile::tagPreDeformedCoordinateFile = "tag-pre-deform-coord-file";
   const QString DeformationFieldFile::tagDeformedCoordinateFile = "tag-deformed-coord-file";
   const QString DeformationFieldFile::tagDeformedTopologyFile = "tag-deformed-topology-file";
   const QString DeformationFieldFile::tagCoordinateFile = "tag-input-coord-file";
   const QString DeformationFieldFile::tagTopologyFile = "tag-input-topology-file";
#endif // __DEFORMATION_FIELD_FILE_MAIN__

#endif  //  __DEFORMATION_FIELD_FILE_H__

