
#ifndef __SURFACE_VECTOR_FILE_H__
#define __SURFACE_VECTOR_FILE_H__

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

#include "FileException.h"
#include "NodeAttributeFile.h"

class CoordinateFile;
class TopologyFile;

/// file for storing surface vectors
class SurfaceVectorFile : public NodeAttributeFile {
   public:
      // constructor
      SurfaceVectorFile();
      
      // destructor
      ~SurfaceVectorFile();
      
      // creates vectors from coord file differences (A - B)
      void addUpdateColumn(const CoordinateFile* coordA,
                      const CoordinateFile* coordB,
                      const TopologyFile* tf = NULL,
                      const int columnNumber = -1,
                      const QString& columnNameIn = "",
                      const QString& columnCommentIn = "") throw (FileException);
                        
      /// append a node attribute  file to this one
      virtual void append(NodeAttributeFile& naf) throw (FileException);

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      virtual void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      /// add columns to this node attribute file
      virtual void addColumns(const int numberOfNewColumns);

      /// add nodes to this file
      virtual void addNodes(const int numberOfNodesToAdd);
      
      /// deform "this" node attribute file placing the output in "deformedFile".
      virtual void deformFile(const DeformationMapFile& dmf, 
                              NodeAttributeFile& deformedFile,
                              const DEFORM_TYPE dt) const throw (FileException);
                              
      /// set the number of nodes and columns in the file
      virtual void setNumberOfNodesAndColumns(const int numNodes, const int numCols);
      
      /// reset a column of data
      virtual void resetColumn(const int columnNumber);
      
      /// remove a column of data
      virtual void removeColumn(const int columnNumber);
      
      /// Clears current file data in memory.  Deriving classes must override this method and
      /// call AbstractFile::clearAbstractFile() from its clear method.
      virtual void clear();
      
      /// get a vector
      const float* getVector(const int node,
                             const int column) const;
                             
      /// get a vector
      void getVector(const int node,
                     const int column,
                     float xyz[3]) const;
                     
      /// get a vector
      void getVector(const int node,
                     const int column,
                     float& x,
                     float& y,
                     float& z) const;
                     
      /// set a vector
      void setVector(const int node, 
                     const int column,
                     const float xyz[3]);
                     
      /// set a vector
      void setVector(const int node, 
                     const int column,
                     const float x,
                     const float y,
                     const float z);
                     
   protected:
      /// Read the contents of the file (header has already been read)
      virtual void readFileData(QFile& file, 
                                QTextStream& stream,
                                QDataStream& binStream,
                                QDomElement& rootElement) throw (FileException);
      
      /// Write the file's data (header has already been written)
      virtual void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& xmlDoc,
                                 QDomElement& rootElement) throw (FileException);
                                 
      /// storage of the vectors
      std::vector<float> vectors;

};

#endif // __SURFACE_VECTOR_FILE_H__

