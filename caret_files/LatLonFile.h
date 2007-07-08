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


#ifndef __VE_LAT_LON_FILE__
#define __VE_LAT_LON_FILE__

#include "NodeAttributeFile.h"

/// This file contains latitude/longitude information for a surface.
class LatLonFile : public NodeAttributeFile {
   private:
      /// latitude storage
      std::vector<float> latitude;
      
      /// longitude storage
      std::vector<float> longitude;
      
      /// deformed latitude storage
      std::vector<float> deformedLatitude;
      
      /// deformed longitude storage
      std::vector<float> deformedLongitude;
      
      /// deformed lat/lon valid
      std::vector<bool> deformedLatLonValid;
      
      /// read a version 0 file
      void readFileDataVersion0(QTextStream& stream,
                                const bool readNumNodes) throw (FileException);
      
      /// read a version 1 file
      void readFileDataVersion1(QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      /// read an LatLon file data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// write an LatLon file data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
   public:
   
      /// constructor
      LatLonFile();
      
      /// destructor
      ~LatLonFile();
      
      /// append a node attribute  file to this one
      void append(NodeAttributeFile& naf) throw (FileException);

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      
      /// add columns to this lat/lon file
      void addColumns(const int numberOfNewColumns);
      
      /// Add nodes to the file
      void addNodes(const int numberOfNodesToAdd);
      
      /// clears lat/lon memory
      void clear();
      
      /// deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                  NodeAttributeFile& deformedFile,
                  const DEFORM_TYPE dt) const throw (FileException);

                  
      /// reset a column of data
      void resetColumn(const int columnNumber);
      
      /// remove a column of data
      void removeColumn(const int columnNumber);
      
      // set number of nodes
      void setNumberOfNodesAndColumns(const int numNodes, const int numCols);
      
      /// get lat/lon for a node
      void getLatLon(const int nodeNumber, const int columnNumber, float& lat, float& lon) const;
      
      /// get deformed lat lon for a node
      void getDeformedLatLon(const int nodeNumber, const int columnNumber, float& lat, float& lon) const;
      
      /// get deformed lat/lon valid
      bool getDeformedLatLonValid(const int columnNumber) const { 
         return deformedLatLonValid[columnNumber]; 
      }
      
      /// set lat/lon for a node
      void setLatLon(const int nodeNumber, const int columnNumber, const float lat, const float lon);
      
      /// set deformed lat/lon for a node
      void setDeformedLatLon(const int nodeNumber, const int columnNumber, const float lat, const float lon);

};

#ifdef _LAT_LON_FILE_MAIN_

#endif // _LAT_LON_FILE_MAIN_

#endif
