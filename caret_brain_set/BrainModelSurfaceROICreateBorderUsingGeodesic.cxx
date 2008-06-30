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

#include <cmath>

#include <QStringList>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceGeodesic.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROICreateBorderUsingGeodesic.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GeodesicDistanceFile.h"
#include "MathUtilities.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceROICreateBorderUsingGeodesic::BrainModelSurfaceROICreateBorderUsingGeodesic(BrainSet* bs,
                                                 BrainModelSurface* bmsIn,
                                                 BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                                 const QString& borderNameIn,
                                                 const int startNodeIn,
                                                 const int endNodeIn,
                                                 const float samplingDensityIn)
   : BrainModelSurfaceROIOperation(bs, bmsIn, surfaceROIIn)
{
   borderName      = borderNameIn;
   borderStartNode = startNodeIn;
   borderEndNode   = endNodeIn;
   borderSamplingDensity = samplingDensityIn;
}                                  

/**
 * destructor.
 */
BrainModelSurfaceROICreateBorderUsingGeodesic::~BrainModelSurfaceROICreateBorderUsingGeodesic()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROICreateBorderUsingGeodesic::executeOperation() throw (BrainModelAlgorithmException)
{
   //
   // Check Inputs
   //
   if (borderName.isEmpty()) {
      throw BrainModelAlgorithmException("Name for border is empty.");
   }

   const int numNodesInROI = operationSurfaceROI->getNumberOfNodesSelected();
   
   if (numNodesInROI == 1) {
      throw BrainModelAlgorithmException("There is only one node, the starting node, in the ROI "
                                         " border named " + borderName);
   }
   
   const int numNodes = bms->getNumberOfNodes();
   
   border.clearLinks();
   
   
   if (borderStartNode >= 0) {
      if (borderStartNode >= numNodes) {
         throw BrainModelAlgorithmException("Starting node is invalid for "
                                            + borderName + ".");
      }
      if (operationSurfaceROI->getNodeSelected(borderStartNode) == false) {
         throw BrainModelAlgorithmException("Starting node is not in the ROI for "
                                            + borderName + ".");
      }
   }
   
   if (borderEndNode >= 0) {
      if (borderEndNode >= numNodes) {
         throw BrainModelAlgorithmException("Ending node is invalid for "
                                            + borderName + ".");
      }
      if (operationSurfaceROI->getNodeSelected(borderEndNode) == false) {
         throw BrainModelAlgorithmException("Ending node is not in the ROI for "
                                            + borderName + ".");
      }
      if (borderStartNode < 0) {
         throw BrainModelAlgorithmException("If the end node is specified, the start node must also be specified for "
                                            + borderName + ".");
      }
      if (borderStartNode == borderEndNode) {
         throw BrainModelAlgorithmException("Starting and ending node are the same for "
                                            + borderName + ".");
      }
   }
   
   //
   // If starting node is not specified
   //
   int iterStart = 2;
   if (borderStartNode < 0) {
      //
      // First iteration will find the starting node
      //
      iterStart = 1;
      
      //
      // Just pick the first selected node to use as the starting node
      //
      for (int i = 0; i < numNodes; i++) {
         if (operationSurfaceROI->getNodeSelected(i)) {
            borderStartNode = i;
            break;
         }
      }
   }
   
   //
   // (Iter == 1) is only performed if a starting node was not specified.  In this
   // iteration, find the node that is furthest from the first selected node which 
   // should be at one end of the sulcus.  Use this node as the start node.
   //
   // (Iter == 2) performs the geodesic calculation from the start node.  If the 
   // end node was not specified, just pick the node furthest from the starting
   // node as the end node.
   //
   for (int iter = iterStart; iter <= 2; iter++) {
      //
      // Determine the geodesic distances
      //
      GeodesicDistanceFile geodesicFile;
      BrainModelSurfaceGeodesic geodesic(brainSet,
                                         bms,
                                         NULL,
                                         -2,
                                         "metric-column-name",
                                         &geodesicFile,
                                         -2,
                                         "geodesic-column-name",
                                         borderStartNode,
                                         operationSurfaceROI);
      geodesic.execute();
      
      //
      // Verify geodesic file
      //
      if ((geodesicFile.getNumberOfNodes() != numNodes) ||
          (geodesicFile.getNumberOfColumns() != 1)) {
         throw BrainModelAlgorithmException("PROGRAM ERROR: Geodesic distance file was not properly created for "
                                            + borderName + ".");
      }
      
      //
      // Find node furthest from start node for this iteration
      //
      int furthestNode = -1;
      float furthestNodeDistance = 0.0;
      for (int i = 0; i < numNodes; i++) {
         const float dist = geodesicFile.getNodeParentDistance(i, 0);
         if (dist > furthestNodeDistance) {
            furthestNodeDistance = dist;
            furthestNode = i;
         }
      }
         
      //
      // If searching for the starting node
      //
      if (iter == 1) {
         borderStartNode = furthestNode;
         if (borderStartNode < 0) {
            throw BrainModelAlgorithmException("Unable to determine starting node for "
                                            + borderName + ".");
         }
         if (DebugControl::getDebugOn()) {
            std::cout << "Starting node is " << borderStartNode << std::endl;
         }
      }

      //
      // If finding geodesic path and possibly the end node
      //
      if (iter == 2) {      
         //
         // Was end node NOT specified
         //
         if (borderEndNode < 0) {
            borderEndNode = furthestNode;
            
            if (borderEndNode < 0) {
               throw BrainModelAlgorithmException("Unable to determine the ending node for "
                                            + borderName + ".");
            }
            
            if (DebugControl::getDebugOn()) {
               std::cout << "Ending node is " << borderEndNode << std::endl;
            }
         }
         
         //
         // Make sure end node is connected in geodesic to start node
         //
         if (geodesicFile.getNodeParent(borderEndNode, 0) < 0) {
            throw BrainModelAlgorithmException(
                    "CREATE BORDER ERROR: Start and end nodes are not "
                                               "connected in the ROI for "
                                            + borderName + ".");
         }
         
         //
         // Set border name
         //
         border.setName(borderName);
         
         //
         // Find path for border
         //
         bool done = false;
         int nodeNum = borderEndNode;
         const CoordinateFile* coordFile = bms->getCoordinateFile();
         while (done == false) {
            //
            // Add on to border
            //
            float xyz[3];
            coordFile->getCoordinate(nodeNum, xyz);
            border.addBorderLink(xyz);
            
            //
            // Next node in geodesic path
            //
            nodeNum = geodesicFile.getNodeParent(nodeNum, 0);
            if ((nodeNum == borderStartNode) ||
                (nodeNum < 0)) {
               done = true;
            }
         }
      } // if (iter == 2...
   }  // for (iter...
   
   //
   // resample the border
   //
   if (borderSamplingDensity >= 0) {
      int dummyUnused;
      border.resampleBorderToDensity(borderSamplingDensity,
                                                     2,
                                                     dummyUnused);
   }
   
   //
   // Need to reverse the links since we start from the end node in the geodesic path
   //
   border.reverseBorderLinks();
}

/**
 * get the border that was created by create border mode.
 */
Border 
BrainModelSurfaceROICreateBorderUsingGeodesic::getBorder() const
{
   return border;
}

