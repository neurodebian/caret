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


#ifndef __TOPOLOGY_FILE__H__
#define __TOPOLOGY_FILE__H__

#include "BrainVoyagerFile.h"
#include "FreeSurferSurfaceFile.h"
#include "GiftiDataArrayFile.h"
#include "MniObjSurfaceFile.h"

class PaintFile;
class NodeRegionOfInterestFile;
class TopologyHelper;
class vtkPolyData;

/// Topology File
/**
 * Caret Topology File stores section and category for each node and the
 * surfaces tiles.
 */
class TopologyFile : public GiftiDataArrayFile {
   public:
      /// Types of Topology Files
      enum TOPOLOGY_TYPES {
        TOPOLOGY_TYPE_CLOSED,
        TOPOLOGY_TYPE_OPEN,  
        TOPOLOGY_TYPE_CUT,
        TOPOLOGY_TYPE_LOBAR_CUT,
        TOPOLOGY_TYPE_UNKNOWN,
        TOPOLOGY_TYPE_UNSPECIFIED
      };

      // constructor
      TopologyFile();
      
      // copy constructor
      TopologyFile(const TopologyFile& tf);
      
      /// destructor
      ~TopologyFile();
      
      // assignment operator
      TopologyFile& operator=(const TopologyFile& tf);
      
      // add a tile 
      void addTile(const int v1, const int v2, const int v3);

      // add a tile
      void addTile(const int v[3]);

      // clear the file
      void clear();

      // delete tiles that use any of the marked nodes
      void deleteTilesWithMarkedNodes(const std::vector<bool>& markedNodes);
      
      // delete tiles with the specified indices
      void deleteTiles(const std::vector<int>& tilesToDelete);

      // delete tiles that use both of these nodes
      void deleteTilesWithEdge(const int node1, const int node2);
      
      // get non-manifold nodes (nodes whose triangles do not share and edge)
      void getNonManifoldNodes(std::vector<int>& nodes) const;
      
      // get number of disjoint objects
      int getNumberOfDisjointObjects() const;
      
      // find the number of disjoint pieces of surface
      int findIslands(std::vector<int>& islandRootNode,
                      std::vector<int>& islandNumNodes,
                      std::vector<int>& nodeRootNeighbor) const;   
                          
      // disconnect islands (retain largest number of connected nodes; disconnect others)
      int disconnectIslands();
      
      // disconnect nodes that are in the region of interest
      void disconnectNodesInRegionOfInterest(const NodeRegionOfInterestFile& roiFile) throw (FileException);
      
      // disconnect nodes that are labeled with the specified name and in the specified column
      void disconnectNodesUsingPaint(const PaintFile& paintFile,
                                     const int paintColumn,
                                     const QString& paintName) throw (FileException);
                                     
      // Find corner tiles (corners are tiles that have one or more nodes that are only in one tile).
      void findCornerTiles(const int numCornerNodes, std::vector<int>& cornerTiles) const;

      // remove corners (returns number of tiles deleted)
      int removeCornerTiles(const int numCornerNodes);
      
      // Get the section numbers for the nodes.  Returns true if the sections are valid else false.
      bool getNodeSections(std::vector<int>& nodeSectionsOut) const;
     
      // export topology to a free surfer surface file
      void exportToFreeSurferSurfaceFile(FreeSurferSurfaceFile& fssf);
      
      // import topology from a free surfer surface file
      void importFromFreeSurferSurfaceFile(const FreeSurferSurfaceFile& fssf,
                                           const TopologyFile* tf);
      
      // get the topology from a brain voyager file
      void importFromBrainVoyagerFile(const BrainVoyagerFile& bvf);
      
      /// get the topology from a MNI OBJ surface file
      void importFromMniObjSurfaceFile(const MniObjSurfaceFile& mni) throw (FileException);
      
      // import from a VTK surface file
      void importFromVtkFile(vtkPolyData* polyData);

      // get the topology helper
      const TopologyHelper* getTopologyHelper(const bool needEdgeInfo,
                                          const bool needNodeInfo,
                                          const bool needNodeInfoSorted) const;
                                        
      // Get spec file tag from topology type
      static QString getSpecFileTagFromTopologyType(const TOPOLOGY_TYPES tt);

      // get the perimeter ID from a topology type
      static QString getPerimeterIDFromTopologyType(TOPOLOGY_TYPES tt);

      // get the topology type from a perimeter ID
      static TOPOLOGY_TYPES getTopologyTypeFromPerimeterID(const QString pid);

      // get the topology type
      TOPOLOGY_TYPES getTopologyType() const;

      // get a descriptive name of the topology file
      QString getDescriptiveName() const;
      
      // get the name of the topology type
      QString getTopologyTypeName() const;

      // set the topology type
      void setTopologyType(const TOPOLOGY_TYPES tt);

      // flip the orientation of the tiles
      void flipTileOrientation();
      
      // get a Tile
      const int* getTile(const int indx) const;
      
      // get a Tile
      void getTile(const int tileNumber,
                   int& v1, int& v2, int& v3) const;
      
      // get a Tile
      void getTile(const int tileNumber, int vertices[3]) const;
      
      // set a tile
      void setTile(const int tileNumber,
                   const int v1, const int v2, const int v3);
                   
      // set a tile
      void setTile(const int tileNumber,
                   const int v[3]);
                   
      // find the tile with the two vertices
      int getTileWithVertices(const int v1, const int v2, const int notTileNum) const;

      // get the number of tiles      
      int getNumberOfTiles() const;

      // set the number of tiles
      void setNumberOfTiles(const int numTiles);
      
      // set the number of nodes in coordinate files using this topology file
      void setNumberOfNodes(const int num);
      
      // get the number of nodes in coordinate files using this topology file
      int getNumberOfNodes() const { return numberOfNodes; }      
      
      // get the euler count (V - E + F = 2 for closed surface)
      void getEulerCount(const bool twoDimFlag, int& faces, int& vertices, int& edges,
                         int& eulerCount, int& numberOfHoles, int& numObjects) const;
      
      // Get Edges with 3 or more tiles assigned to them.
      void getDegenerateEdges(int& numDegenerateEdges, 
                              std::vector<int>& nodesUsedByDegenerateEdges) const;
      
      // see if the topology file is equivalent to this one (contains exact same tiles)
      bool equivalent(const TopologyFile& tf) const;
      
   protected:
      // copy helper used by copy constructor and assignment operator
      void copyHelperTopology(const TopologyFile& tf);
      
      // read in tiles from ascii file
      void readTilesAscii(QTextStream& stream, 
                     const bool clockwiseOrientation) throw (FileException);
                        
      // read in tiles from binary file
      void readTilesBinary(QDataStream& stream) throw (FileException);
                        
      // read topology file version 0
      void readFileDataVersion0(QTextStream& stream, 
                                const QString& firstLineRead) throw (FileException);
      
      // read topology file version 1
      void readFileDataVersion1(QFile& file,
                                QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      // read  file
      void readLegacyFileData(QFile& file, QTextStream& stream, QDataStream& binStream)
                                                                 throw (FileException);
      
      // write  file
      void writeLegacyFileData(QTextStream& stream, QDataStream& binStream)
                                                       throw (FileException);
      
      /// topology helper needs to be rebuilt (topology was modified)
      mutable bool topologyHelperNeedsRebuild;
      
      /// topology helper for node/edge info
      mutable TopologyHelper* topologyHelper;
      
      /// number of nodes in coordinate files that use this topology file
      int numberOfNodes;
      
      /// node section number
      std::vector<int> nodeSections;
      
      //
      // NOTE: IF ANY NEW VARIABLES ARE ADDED BE SURE TO UPDATE THE COPY CONSTRUCTOR
      //
      
};

#endif //__TOPOLOGY_FILE__H__


