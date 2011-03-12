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
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROIOperation.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfaceROIOperation::BrainModelSurfaceROIOperation(BrainSet* bs,
                                           const BrainModelSurface* bmsIn,
                                           const BrainModelSurfaceROINodeSelection* inputSurfaceROIIn)
   : BrainModelAlgorithm(bs),
     bms(bmsIn),
     inputSurfaceROI(inputSurfaceROIIn)
{
   operationSurfaceROI = NULL;
}                                  

/**
 * destructor.
 */
BrainModelSurfaceROIOperation::~BrainModelSurfaceROIOperation()
{
   if (operationSurfaceROI != NULL) {
      delete operationSurfaceROI;
      operationSurfaceROI = NULL;
   }
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIOperation::execute() throw (BrainModelAlgorithmException)
{
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Surface is invalid (NULL).");
   }
   if (bms->getTopologyFile() == NULL) {
      throw BrainModelAlgorithmException("Surface has no topology.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface contains no nodes.");
   }
   if (inputSurfaceROI == NULL) {
      throw BrainModelAlgorithmException("The input ROI is invalid.");
   }
   if (inputSurfaceROI->getNumberOfNodes() != numNodes) {
      throw BrainModelAlgorithmException("The surface and the ROI contain a different number of nodes.");
   }
   const int numberOfSelectedNodes = inputSurfaceROI->getNumberOfNodesSelected();
   if (numberOfSelectedNodes <= 0) {
      throw BrainModelAlgorithmException("No nodes are selected in the ROI.");
   }
   
   reportText = "";
   
   //
   // Copy the input ROI to the operation ROI
   //
   operationSurfaceROI = new BrainModelSurfaceROINodeSelection(*inputSurfaceROI);
   
   //
   // Remove any nodes from the operation ROI if they are not connected
   //
   const TopologyHelper* th = getTopologyHelper();
   if (th == NULL) {
      throw BrainModelAlgorithmException("Operation surface topology invalid.");
   }
   for (int i = 0; i < numNodes; i++) {
      if (th->getNodeHasNeighbors(i) == false) {
         operationSurfaceROI->setNodeSelected(i, false);
      }
   }
   
   executeOperation();
}

/**
 * Create the report header.
 */
void 
BrainModelSurfaceROIOperation::createReportHeader(float& roiAreaOut)
{
   //
   // Add the header describing the node selection
   //
   reportText.append("Node Selection: "
                     + inputSurfaceROI->getSelectionDescription());
   reportText.append("\n\n");
   
   const TopologyFile* tf = bms->getTopologyFile();
   
   const int numNodes = bms->getNumberOfNodes();
   
   //
   // Determine total area and selected area.
   //
   const CoordinateFile* cf = bms->getCoordinateFile();
   double totalArea = 0.0;
   roiAreaOut = 0.0;
   const int numTiles = tf->getNumberOfTiles();
   tileArea.resize(numTiles);
   std::fill(tileArea.begin(), tileArea.end(), 0.0);
   tileInROI.resize(numTiles);
   std::fill(tileInROI.begin(), tileInROI.end(), false);
   
   double centerOfGravity[3] = { 0.0, 0.0, 0.0 };
   
   for (int i = 0; i < numTiles; i++) {
      int nodes[3];
      tf->getTile(i, nodes);
      tileArea[i] = MathUtilities::triangleArea(cf->getCoordinate(nodes[0]),
                                                cf->getCoordinate(nodes[1]),
                                                cf->getCoordinate(nodes[2]));
      totalArea += tileArea[i];
      
      double numMarked = 0.0;
      if (operationSurfaceROI->getNodeSelected(nodes[0])) numMarked += 1.0;
      if (operationSurfaceROI->getNodeSelected(nodes[1])) numMarked += 1.0;
      if (operationSurfaceROI->getNodeSelected(nodes[2])) numMarked += 1.0;
      
      if (tileArea[i] > 0.0) {
         roiAreaOut += (numMarked / 3.0) * tileArea[i];
      }
      tileInROI[i] = (numMarked > 0.0);
   }
   
   for (int m = 0; m < numNodes; m++) {
      if (operationSurfaceROI->getNodeSelected(m)) {
         const float* xyz = cf->getCoordinate(m);
         centerOfGravity[0] += xyz[0];
         centerOfGravity[1] += xyz[1];
         centerOfGravity[2] += xyz[2];
      }
   }
      
   if (headerText.isEmpty() == false) {
      reportText.append(headerText);
      reportText.append("\n\n");
   }
   QString surf("Surface: ");
   surf.append(bms->getDescriptiveName());
   surf.append("\n");
   reportText.append(surf);
   reportText.append("\n");
   
   QString topo("Topology: ");
   topo.append(tf->getDescriptiveName());
   topo.append("\n");
   reportText.append(topo);
   reportText.append("\n");
   
   reportText.append("\n");
   const int count = operationSurfaceROI->getNumberOfNodesSelected();
   QStringList sl;
   sl << QString::number(count) << " of " << QString::number(numNodes) << " nodes in region of interest\n";
   reportText.append(sl.join(""));
   reportText.append("\n");

   sl.clear();
   sl << "Total Surface Area: " << QString::number(totalArea, 'f', 1);
   reportText.append(sl.join(""));
   reportText.append("\n");
   
   sl.clear();
   sl << "Region of Interest Surface Area: " << QString::number(roiAreaOut, 'f', 1);
   reportText.append(sl.join(""));
   reportText.append("\n");
   
   centerOfGravity[0] = centerOfGravity[0] / static_cast<float>(count);
   centerOfGravity[1] = centerOfGravity[1] / static_cast<float>(count);
   centerOfGravity[2] = centerOfGravity[2] / static_cast<float>(count);
   sl.clear();
   sl << "Region of Interest Center of Gravity: " 
       << QString::number(centerOfGravity[0], 'f', 4) << " "
       << QString::number(centerOfGravity[1], 'f', 4) << " "
       << QString::number(centerOfGravity[2], 'f', 4);
   reportText.append(sl.join(""));
   reportText.append("\n");
   
   float meanDistance, minDist, maxDist;
   bms->getMeanDistanceBetweenNodes(operationSurfaceROI, meanDistance, minDist, maxDist);
   sl.clear();
   sl << "Region Mean Distance Between Nodes: "
//       << QString::number(bms->getMeanDistanceBetweenNodes(operationSurfaceROI), 'f', 5);
   << QString::number(meanDistance, 'f', 5);
   reportText.append(sl.join(""));
   reportText.append("\n");
   reportText.append(" \n");
}                                                     

/**
 * set the header text.
 */
void 
BrainModelSurfaceROIOperation::setHeaderText(const QString& headerTextIn)
{
   headerText = headerTextIn;
} 

/**
 * get the topology helper.
 */
const TopologyHelper* 
BrainModelSurfaceROIOperation::getTopologyHelper() const
{
   if (bms == NULL) {
      return NULL;
   }
   
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      return NULL;
   }
   
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   return th;
}
     
