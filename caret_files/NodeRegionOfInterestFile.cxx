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

#include "CoordinateFile.h"
#include "NodeRegionOfInterestFile.h"
#include "SpecFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
NodeRegionOfInterestFile::NodeRegionOfInterestFile()
   : PaintFile("Node Region Of Interest",
               SpecFile::getRegionOfInterestFileExtension())
{
   clear();
}

/**
 * destructor.
 */
NodeRegionOfInterestFile::~NodeRegionOfInterestFile()
{
}

/**
 * set number of nodes.
 */
void 
NodeRegionOfInterestFile::setNumberOfNodes(const int numNodes)
{
   if (numNodes > 0) {
      setNumberOfNodesAndColumns(numNodes, 1);
   }
   else {
      setNumberOfNodesAndColumns(numNodes, 0);
   }
}

/**
 * set the number of nodes and columns in the file.
 */
void 
NodeRegionOfInterestFile::setNumberOfNodesAndColumns(const int numNodes, 
                                                     const int numCols,
                                                     const int numElementsPerCol)
{
   PaintFile::setNumberOfNodesAndColumns(numNodes, numCols, numElementsPerCol);
   selectedPaintIndex = addPaintName("Selected");
   deselectedPaintIndex = addPaintName("Deselected");
   
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         setPaint(i, j, deselectedPaintIndex);
      }
   }
}

/**
 * get node selected.
 */
bool 
NodeRegionOfInterestFile::getNodeSelected(const int nodeNumber) const
{
   if (getPaint(nodeNumber, 0) == selectedPaintIndex) {
      return true;
   }
   return false;
}

/**
 * set node selected.
 */
void 
NodeRegionOfInterestFile::setNodeSelected(const int nodeNumber,
                                          const bool status)
{
   if (status) {
      setPaint(nodeNumber, 0, selectedPaintIndex);
   }
   else {
      setPaint(nodeNumber, 0, deselectedPaintIndex);
   }
}

/**
 * assign nodes in ROI by intersecting with a volume file.
 * If a coordinate is within a non-zero voxel, the node is.
 * set to selected in the ROI.
 */
void 
NodeRegionOfInterestFile::assignSelectedNodesWithVolumeFile(const VolumeFile* vf,
                                                            const CoordinateFile* cf,
                                                            const TopologyFile* tf) throw (FileException)
{
   if (vf == NULL) {
      throw FileException("Invalid volume file (NULL).");
   }
   if (cf == NULL) {
      throw FileException("Invalid coordinate file (NULL).");
   }
   if (tf == NULL) {
      throw FileException("Invalid topology file (NULL).");
   }
   if (getNumberOfNodes() <= 0) {
      throw FileException("Region of Interest file has not been allocated (no nodes).");
   }
   if (getNumberOfNodes() != cf->getNumberOfCoordinates()) {
      throw FileException("ROI file has different number of coordinates that coordinate file.");
   }
   
   //
   // Get a topology helper
   //
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   //
   // Loop through coordinates
   //
   const int numCoords = cf->getNumberOfCoordinates();
   for (int i = 0; i < numCoords; i++) {
      //
      // If node has neighbors
      //
      if (th->getNodeHasNeighbors(i)) {
         //
         // Convert coordinate to a voxel index
         //
         int ijk[3];
         if (vf->convertCoordinatesToVoxelIJK(cf->getCoordinate(i), ijk)) {
            //
            // assign the roi for the node if the voxel is non-zero
            //
            if (vf->getVoxel(ijk) != 0) {
               setNodeSelected(i, true);
            }
         }
      }
   }
}

/**
 * get the ROI description.
 */
QString 
NodeRegionOfInterestFile::getRegionOfInterestDescription() const
{
   if (getNumberOfColumns() > 0) {
      return getColumnComment(0);
   }
   return "";
}

/**
 * set the ROI description (number of nodes must have been set).
 */
void 
NodeRegionOfInterestFile::setRegionOfInterestDescription(const QString& s)
{
   if (getNumberOfColumns() > 0) {
      setColumnComment(0, s);
   }
}
      
