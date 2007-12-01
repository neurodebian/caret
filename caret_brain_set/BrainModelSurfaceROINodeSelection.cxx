
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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceConnectedSearchMetric.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "DebugControl.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfaceROINodeSelection::BrainModelSurfaceROINodeSelection(BrainSet* brainSetIn)
{
   brainSet = brainSetIn;
   displaySelectedNodes = true;
   update();
}

/**
 * destructor.
 */
BrainModelSurfaceROINodeSelection::~BrainModelSurfaceROINodeSelection()
{
}

/**
 * get the nodes selection types and names.
 */
void 
BrainModelSurfaceROINodeSelection::getNodeSelectionTypesAndNames(
                                                     std::vector<SELECTION_LOGIC>& typesOut,
                                                     std::vector<QString>& namesOut)
{  
   typesOut.clear();
   namesOut.clear();
   
   typesOut.push_back(SELECTION_LOGIC_NORMAL);    
      namesOut.push_back("Normal Selection");
   typesOut.push_back(SELECTION_LOGIC_AND);       
      namesOut.push_back("And Selection (Intersection)");
   typesOut.push_back(SELECTION_LOGIC_OR);        
      namesOut.push_back("Or Selection (Union)");
   typesOut.push_back(SELECTION_LOGIC_AND_NOT);   
      namesOut.push_back("And-Not Selection");
}
                                                
/**
 * deselect all nodes.
 */
void 
BrainModelSurfaceROINodeSelection::deselectAllNodes()
{
   update();
   std::fill(nodeSelectedFlags.begin(), nodeSelectedFlags.end(), 0);
}

/**
 * select all nodes (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectAllNodes(const BrainModelSurface* selectionSurface)
{
   update();
   
   const unsigned int numNodes = nodeSelectedFlags.size();
   std::vector<int> nodeFlags(numNodes, 1);
   
   return processNewNodeSelections(SELECTION_LOGIC_NORMAL,
                                   selectionSurface,
                                   nodeFlags);
}

/**
 * update (usually called if number of nodes changes).
 */
void 
BrainModelSurfaceROINodeSelection::update()
{
   const unsigned int numNodes = brainSet->getNumberOfNodes();
   if (numNodes != nodeSelectedFlags.size()) {
      nodeSelectedFlags.resize(numNodes, false);
   }
}

/**
 * see if any nodes are selected.
 */
bool 
BrainModelSurfaceROINodeSelection::anyNodesSelected() const
{
   const unsigned int numNodes = nodeSelectedFlags.size();
   for (unsigned int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] != 0) {
         return true;
      }
   }
   
   return false;
}
      
/**
 * get the selection status of a node.
 */
bool 
BrainModelSurfaceROINodeSelection::getNodeSelected(const int nodeNumber) const
{
   return (nodeSelectedFlags[nodeNumber] != 0);
}

/**
 * get the number of nodes selected.
 */
int 
BrainModelSurfaceROINodeSelection::getNumberOfNodesSelected() const
{
   const unsigned int numNodes = nodeSelectedFlags.size();
   int cnt = 0;
   for (unsigned int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i]) {
         cnt++;
      }
   }
   return cnt;
}
      
/**
 * set the selection status of a node.
 */
void 
BrainModelSurfaceROINodeSelection::setNodeSelected(const int nodeNumber,
                                                                const bool nodeSelectedFlag)
{
   nodeSelectedFlags[nodeNumber] = nodeSelectedFlag;
}

/**
 * get node in ROI flags.
 */
void 
BrainModelSurfaceROINodeSelection::getNodesInROI(std::vector<bool>& nodesAreInROI) const
{
   const unsigned int num = nodeSelectedFlags.size();
   nodesAreInROI.resize(num);
   for (unsigned int i = 0; i < num; i++) {
      nodesAreInROI[i] = (nodeSelectedFlags[i] != 0);
   }
}
      
/**
 * get the tiles that have at least one node in the ROI.
 */
void 
BrainModelSurfaceROINodeSelection::getTilesInRegionOfInterest(
                                                      const BrainModelSurface* bms,
                                                      std::vector<int>& tilesInROI) const
{
   tilesInROI.clear();
   
   if (bms == NULL) {
      return;
   }
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      return;
   }
   const int numTiles = tf->getNumberOfTiles();
   
   tilesInROI.resize(numTiles, 0);
   for (int i = 0; i < numTiles; i++) { 
      int n1, n2, n3;
      tf->getTile(i, n1, n2, n3);
      if (nodeSelectedFlags[n1] ||
          nodeSelectedFlags[n2] ||
          nodeSelectedFlags[n3]) {
         tilesInROI[i] = 1;
      }
   }
}

/**
 * invert selected nodes.
 */
QString 
BrainModelSurfaceROINodeSelection::invertSelectedNodes(const BrainModelSurface* selectionSurface)
{
   const unsigned int numNodes = nodeSelectedFlags.size();
   std::vector<int> newNodeSelections(numNodes, 0);
   for (unsigned int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] == 0) {
         newNodeSelections[i] = 1;
      }
   }
   return processNewNodeSelections(SELECTION_LOGIC_NORMAL,
                                   selectionSurface,
                                   newNodeSelections);
}
      
/**
 * select nodes with paint (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithPaint(
                                               const SELECTION_LOGIC selectionLogic,
                                               const BrainModelSurface* selectionSurface,
                                               const PaintFile* pf,
                                               const int paintFileColumnNumber,
                                               const QString& paintName)
{
   if (pf == NULL) {
      return "ERROR: Paint File is invalid.";
   }

   if ((paintFileColumnNumber < 0) ||
       (paintFileColumnNumber > pf->getNumberOfColumns())) {
      return "ERROR: Paint File Column is invalid.";
   }
   const int paintIndex = pf->getPaintIndexFromName(paintName);
   if (paintIndex < 0) {
      return "ERROR: Paint name not found in paint file.";
   }
   
   const int numNodes = pf->getNumberOfNodes();
   std::vector<int> newNodeSelections(numNodes, 0);
   
   for (int i = 0; i < numNodes; i++) {
      if (pf->getPaint(i, paintFileColumnNumber) == paintIndex) {
         newNodeSelections[i] = 1;
      }
   }
   
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections);
}
                          
/**
 * select nodes within border (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithinBorder(
                                                  const SELECTION_LOGIC selectionLogic,
                                                  const BrainModelSurface* selectionSurface,
                                                  const BrainModelSurface* flatSurface,
                                                  const BrainModelBorderSet* bmbs,
                                                  const QString& borderName)
{
   if (flatSurface == NULL) {
      return "ERROR: Flat surface is invalid.";
   }
   const TopologyFile* tf = flatSurface->getTopologyFile();
   if (tf == NULL) {
      return "ERROR: Flat Surface has no topology.";
   }
   if (bmbs == NULL) {
      return "ERROR: Borders are invalid.";
   }
   
   const CoordinateFile* cf = flatSurface->getCoordinateFile();
   const float* coords = cf->getCoordinate(0);
   
   //
   // Get the border file for the surface type
   //
   BorderFile bf;
   bmbs->copyBordersToBorderFile(flatSurface, bf);
   const int numBorders = bf.getNumberOfBorders();
   if (bf.getNumberOfBorders() <= 0) {
      return "ERROR: Flat surface has no borders.";
   }
   
   const int numNodes = brainSet->getNumberOfNodes();
   std::vector<int> newNodeSelections(numNodes, 0);
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Need to check all borders since there may be more than one 
   // with the same name.
   //
   for (int i = 0; i < numBorders; i++) {
      Border* b = bf.getBorder(i);
      if (b->getName() == borderName) {
         std::vector<bool> insideFlags;
         b->pointsInsideBorder2D(coords, numNodes, insideFlags);
         
         for (int j = 0; j < numNodes; j++) {
            if (th->getNodeHasNeighbors(j)) {
               if (insideFlags[j]) {
                  newNodeSelections[j] = 1;
               }
            }
         }
      }
   }

   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections);
}
                             
/**
 * select nodes within lat/long range (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithLatLong(
                                                 const SELECTION_LOGIC selectionLogic,
                                                 const BrainModelSurface* selectionSurface,
                                                 const LatLonFile* latLonFile,
                                                 const int latLonFileColumnNumber,
                                                 const float minimumLatitude,
                                                 const float maximumLatitude,
                                                 const float minimumLongitude,
                                                 const float maximumLongitude)
{
   if (latLonFile == NULL) {
      return "ERROR: Lat/Long file is invalid.";
   }
   if ((latLonFileColumnNumber < 0) ||
       (latLonFileColumnNumber >= latLonFile->getNumberOfColumns())) {
      return "ERROR: Lat/Long file column number is invalid.";
   }
   
   const int numNodes = latLonFile->getNumberOfNodes();
   std::vector<int> newNodeSelections(numNodes, 0);
   
   for (int i = 0; i < numNodes; i++) {
      float lat, lon;
      latLonFile->getLatLon(i, 
                            latLonFileColumnNumber,
                            lat,
                            lon);
      if ((lat >= minimumLatitude) &&
          (lat <= maximumLatitude) &&
          (lon >= minimumLongitude) &&
          (lon <= maximumLongitude)) {
         newNodeSelections[i] = 1;
      }
   }
   
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections);
}
                            
/**
 * select nodes within metric range (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithMetric(
                                                   const SELECTION_LOGIC selectionLogic,
                                                   const BrainModelSurface* selectionSurface,
                                                   const MetricFile* metricFile,
                                                   const int metricFileColumnNumber,
                                                   const float minimumMetricValue,
                                                   const float maximumMetricValue)
{
   QString fileType("Metric");
   if (dynamic_cast<const SurfaceShapeFile*>(metricFile) != NULL) {
      fileType = "Shape";
   }
   if (metricFile == NULL) {
      return "ERROR: " + fileType + " file is invalid.";
   }
   if ((metricFileColumnNumber < 0) ||
       (metricFileColumnNumber >= metricFile->getNumberOfColumns())) {
      return "ERROR: " + fileType + " file column number is invalid.";
   }
   
   const int numNodes = metricFile->getNumberOfNodes();
   std::vector<int> newNodeSelections(numNodes, 0);
   
   for (int i = 0; i < numNodes; i++) {
      const float value = metricFile->getValue(i, metricFileColumnNumber);
      if ((value >= minimumMetricValue) &&
          (value <= maximumMetricValue)) {
         newNodeSelections[i] = 1;
      }
   }
      
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections);
}
                           
/**
 * select nodes within metric range and connected to specified node (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectConnectedNodesWithMetric(
                                                   const SELECTION_LOGIC selectionLogic,
                                                   const BrainModelSurface* selectionSurface,
                                                   const MetricFile* metricFile,
                                                   const int metricFileColumnNumber,
                                                   const float minimumMetricValue,
                                                   const float maximumMetricValue,
                                                   const int connectedToNodeNumber)
{
   QString fileType("Metric");
   if (dynamic_cast<const SurfaceShapeFile*>(metricFile) != NULL) {
      fileType = "Shape";
   }
   
   if (metricFile == NULL) {
      return "ERROR: " + fileType + " file is invalid.";
   }
   if ((metricFileColumnNumber < 0) ||
       (metricFileColumnNumber >= metricFile->getNumberOfColumns())) {
      return "ERROR: " + fileType + " file column number is invalid.";
   }

   const int numNodes = metricFile->getNumberOfNodes();
   if ((connectedToNodeNumber < 0) ||
       (connectedToNodeNumber >= numNodes)) {
      return "ERROR: selected node number is invalid.";
   }
   
   //
   // Find metrics connected to selected node that are within metric threshold values
   //
   std::vector<int> newNodeSelections(numNodes, 0);
   BrainModelSurfaceConnectedSearchMetric metricSearch(brainSet, 
                                                       (BrainModelSurface*)selectionSurface, 
                                                       connectedToNodeNumber,
                                                       metricFile,
                                                       metricFileColumnNumber, 
                                                       minimumMetricValue, 
                                                       maximumMetricValue);
   try {
      metricSearch.execute();
      for (int i = 0; i < numNodes; i++) {
         if (metricSearch.getNodeConnected(i)) {
            newNodeSelections[i] = 1;
         }
      }
   }
   catch (BrainModelAlgorithmException& bmae) {
      return bmae.whatQString();
   }
   
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections);
}
                           
/**
 * select nodes within surface shape range (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithSurfaceShape(
                                                   const SELECTION_LOGIC selectionLogic,
                                                   const BrainModelSurface* selectionSurface,
                                                   const SurfaceShapeFile* surfaceShapeFile,
                                                   const int surfaceShapeFileColumnNumber,
                                                   const float minimumShapeValue,
                                                   const float maximumShapeValue)
{
   //
   // Shape is derived from Metric
   //
   return selectNodesWithMetric(selectionLogic,
                                selectionSurface,
                                surfaceShapeFile,
                                surfaceShapeFileColumnNumber,
                                minimumShapeValue,
                                maximumShapeValue);
}
                           
/**
 * select nodes within surface shape range and connected to specified node (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectConnectedNodesWithSurfaceShape(
                                                   const SELECTION_LOGIC selectionLogic,
                                                   const BrainModelSurface* selectionSurface,
                                                   const SurfaceShapeFile* surfaceShapeFile,
                                                   const int surfaceShapeFileColumnNumber,
                                                   const float minimumShapeValue,
                                                   const float maximumShapeValue,
                                                   const int connectedToNodeNumber)
{
   //
   // Shape is derived from Metric
   //
   return selectConnectedNodesWithMetric(selectionLogic,
                                         selectionSurface,
                                         surfaceShapeFile,
                                         surfaceShapeFileColumnNumber,
                                         minimumShapeValue,
                                         maximumShapeValue,
                                         connectedToNodeNumber);
}
                           
/**
 * select nodes that are crossovers (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesThatAreCrossovers(
                                                  const SELECTION_LOGIC selectionLogic,
                                                  const BrainModelSurface* selectionSurface)
{
   const BrainSetNodeAttribute* nodeAttributes = brainSet->getNodeAttributes(0);
   const int numNodes = brainSet->getNumberOfNodes();

   std::vector<int> newNodeSelections(numNodes, 0);
   for (int i = 0; i < numNodes; i++) {
      if (nodeAttributes[i].getCrossover() == BrainSetNodeAttribute::CROSSOVER_YES) {
         newNodeSelections[i] = 1;
      }
   }
   
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections);
}

/**
 * dilate the selected nodes.
 */
void 
BrainModelSurfaceROINodeSelection::dilate(const BrainModelSurface* selectionSurface,
                                          int numberOfIterations)
{
   QString topologyHelperErrorMessage;
   const TopologyHelper* th = getSelectionSurfaceTopologyHelper(selectionSurface,
                                                                topologyHelperErrorMessage);
   if (th == NULL) {
      return;
   }
   
   update();
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   
   //
   // For specified number of dilation iterations
   //
   for (int iter = 0; iter < numberOfIterations; iter++) {
      //
      // Output for dilation
      //
      std::vector<int> nodesDilated = nodeSelectedFlags;
   
      //
      // Check each node
      //
      for (int i = 0; i < numNodes; i++) {
         // 
         // Is this node in the ROI
         //
         if (nodeSelectedFlags[i]) {
            //
            // Get node's neighbors
            //
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
            
            //
            // Add neighbors to the ROI
            //
            for (int j = 0; j < numNeighbors; j++) {
               nodesDilated[neighbors[j]] = 1;
            }
         }
      }
   
      //
      // Output dilation
      //
      nodeSelectedFlags = nodesDilated;
   }
}

/**
 * dilate but only add nodes with selected paint.
 */
void 
BrainModelSurfaceROINodeSelection::dilatePaintConstrained(const BrainModelSurface* selectionSurface,
                                          const PaintFile* paintFile,
                                          const int paintColumnNumber,
                                          const QString& paintName,
                                          const int numberOfIterations)
{
   update();
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   
   if ((paintColumnNumber < 0) ||
       (paintColumnNumber > paintFile->getNumberOfColumns()) ||
       (paintFile->getNumberOfNodes() != numNodes)) {
      return;
   }
   
   const int paintNameIndex = paintFile->getPaintIndexFromName(paintName);

   QString topologyHelperErrorMessage;
   const TopologyHelper* th = getSelectionSurfaceTopologyHelper(selectionSurface,
                                                                topologyHelperErrorMessage);
   if (th == NULL) {
      return;
   }
   
   //
   // Identify nodes that may be dilated
   //
   std::vector<int> nodeMayBeDilated(numNodes, 0);
   for (int i = 0; i < numNodes; i++) {
      if (paintFile->getPaint(i, paintColumnNumber) == paintNameIndex) {
         nodeMayBeDilated[i] = 1;
      }
   }
   
   //
   // For specified number of dilation iterations
   //
   for (int iter = 0; iter < numberOfIterations; iter++) {
      //
      // Output for dilation
      //
      std::vector<int> nodesDilated = nodeSelectedFlags;
   
      //
      // Check each node
      //
      for (int i = 0; i < numNodes; i++) {
         // 
         // Is this node in the ROI
         //
         if (nodeSelectedFlags[i]) {
            //
            // Get node's neighbors
            //
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
            
            //
            // Add neighbors to the ROI
            //
            for (int j = 0; j < numNeighbors; j++) {
               const int nodeNum = neighbors[j];
               if (nodeMayBeDilated[nodeNum] != 0) {
                  nodesDilated[nodeNum] = 1;
               }
            }
         }
      }
   
      //
      // Output dilation
      //
      nodeSelectedFlags = nodesDilated;
   }
}
                  
/**
 * boundary only (keeps nodes in ROI that have at least one neighbor NOT in ROI).
 */
void 
BrainModelSurfaceROINodeSelection::boundaryOnly(const BrainModelSurface* bms)
{
   QString topologyHelperErrorMessage;
   const TopologyHelper* th = getSelectionSurfaceTopologyHelper(bms,
                                                                topologyHelperErrorMessage);
   if (th == NULL) {
      return;
   }
   
   update();
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   
   //
   // Output for boundary
   //
   std::vector<int> boundaryNodes(numNodes, 0);

   //
   // Check each node
   //
   for (int i = 0; i < numNodes; i++) {
      // 
      // Is this node in the ROI
      //
      if (nodeSelectedFlags[i]) {
         //
         // Get node's neighbors
         //
         int numNeighbors = 0;
         const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
         
         //
         // If a neighbor is not in the ROI, this is a boundary node ROI
         //
         for (int j = 0; j < numNeighbors; j++) {
            if (nodeSelectedFlags[neighbors[j]] == 0) {
               boundaryNodes[i] = 1;
               break;
            }
         }
      }
   }
   
   //
   // Output boundary
   //
   nodeSelectedFlags = boundaryNodes;
}

/**
 * erode the selected nodes.
 */
void 
BrainModelSurfaceROINodeSelection::erode(const BrainModelSurface* selectionSurface,
                                         int numberOfIterations)
{
   QString topologyHelperErrorMessage;
   const TopologyHelper* th = getSelectionSurfaceTopologyHelper(selectionSurface,
                                                                topologyHelperErrorMessage);
   if (th == NULL) {
      return;
   }
   
   update();
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   
   //
   // For specified number of erosion iterations
   //
   for (int iter = 0; iter < numberOfIterations; iter++) {
      //
      // Output for eroding
      //
      std::vector<int> nodesEroded = nodeSelectedFlags;
   
      //
      // Check each node
      //
      for (int i = 0; i < numNodes; i++) {
         // 
         // Is this node in the ROI
         //
         if (nodeSelectedFlags[i]) {
            //
            // Get node's neighbors
            //
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
            
            //
            // If a neighbor is not in the ROI, take "this" node out of ROI
            //
            for (int j = 0; j < numNeighbors; j++) {
               if (nodeSelectedFlags[neighbors[j]] == 0) {
                  nodesEroded[i] = 0;
                  break;
               }
            }
         }
      }
   
      //
      // Output erosion
      //
      nodeSelectedFlags = nodesEroded;
   }
}
  
/**
 * limit the extent of the ROI.
 */
void 
BrainModelSurfaceROINodeSelection::limitExtent(const BrainModelSurface* selectionSurface,
                                               const float extent[6])
{
   update();
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   
   const CoordinateFile* cf = selectionSurface->getCoordinateFile();
   
   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] != 0) {
         const float* xyz = cf->getCoordinate(i);
         if ((xyz[0] < extent[0]) ||
             (xyz[0] > extent[1]) ||
             (xyz[1] < extent[2]) ||
             (xyz[1] > extent[3]) ||
             (xyz[2] < extent[4]) ||
             (xyz[2] > extent[5])) {
            nodeSelectedFlags[i] = 0;
         }
      }
   }
}      

/**
 * process the new node selections.
 */
QString 
BrainModelSurfaceROINodeSelection::processNewNodeSelections(
                                             const SELECTION_LOGIC selectionLogic,
                                             const BrainModelSurface* selectionSurface,
                                             std::vector<int>& newNodeSelections)
{
   QString topologyHelperErrorMessage;
   const TopologyHelper* th = getSelectionSurfaceTopologyHelper(selectionSurface,
                                                                topologyHelperErrorMessage);
   if (th == NULL) {
      return topologyHelperErrorMessage;
   }
   
   update();
   const unsigned int numNodes = nodeSelectedFlags.size();
   newNodeSelections.resize(numNodes, 0);
   
   for (unsigned int i = 0; i < numNodes; i++) {
      if (th->getNodeHasNeighbors(i)) {
         switch (selectionLogic) {
            case SELECTION_LOGIC_NORMAL:
               nodeSelectedFlags[i] = newNodeSelections[i];
               break;
            case SELECTION_LOGIC_AND:
               if (nodeSelectedFlags[i] &&
                   newNodeSelections[i]) {
                  nodeSelectedFlags[i] = 1;
               }
               else {
                  nodeSelectedFlags[i] = 0;
               }
               break;
            case SELECTION_LOGIC_OR:
               if (nodeSelectedFlags[i] ||
                   newNodeSelections[i]) {
                  nodeSelectedFlags[i] = 1;
               }
               else {
                  nodeSelectedFlags[i] = 0;
               }
               break;
            case SELECTION_LOGIC_AND_NOT:
               if (nodeSelectedFlags[i] &&
                   (newNodeSelections[i] == 0)) {
                  nodeSelectedFlags[i] = 1;
               }
               else {
                  nodeSelectedFlags[i] = 0;
               }
               break;
         }
      }
      else {
         nodeSelectedFlags[i] = 0;   // not in topology
      }
   }
   
   return "";
}

/**
 * get the topolgy helper for selection surface (returns NULL if unavailable).
 */
const TopologyHelper* 
BrainModelSurfaceROINodeSelection::getSelectionSurfaceTopologyHelper(
                                                 const BrainModelSurface* selectionSurface,
                                                 QString& errorMessageOut) const
{
   errorMessageOut = "";
   
   if (selectionSurface == NULL) {
      errorMessageOut = "ERROR: Selection surface is invalid.";
      return NULL;
   }
   const TopologyFile* tf = selectionSurface->getTopologyFile();
   if (tf == NULL) {
      errorMessageOut = "ERROR: Selection Surface has no topology.";
      return NULL;
   }
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   if (th == NULL) {
      errorMessageOut = "ERROR: Failed to create topology helper.";
   }
   return th;
}
                                                              
/**
 * get the center of gravity of the selection.
 */
void 
BrainModelSurfaceROINodeSelection::getCenterOfGravityOfSelectedNodes(
                                       const BrainModelSurface* bms,
                                       float cogOut[3]) const
{
   cogOut[0] = 0.0;
   cogOut[1] = 0.0;
   cogOut[2] = 0.0;
   
   if (bms == NULL) {
      return;
   }
   
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes != static_cast<int>(nodeSelectedFlags.size())) {
      return;
   }
   
   double sum[3] = { 0.0, 0.0, 0.0 };
   const CoordinateFile* cf = bms->getCoordinateFile();
   float numSelected = 0.0;
   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] != 0) {
         const float* xyz = cf->getCoordinate(i);
         sum[0] += xyz[0];
         sum[1] += xyz[1];
         sum[2] += xyz[2];
         numSelected += 1.0;
      }
   }
   
   if (numSelected >= 1.0) {
      cogOut[0] = sum[0] / numSelected;
      cogOut[1] = sum[1] / numSelected;
      cogOut[2] = sum[2] / numSelected;
   }
}
      
/**
 * get the extent of the selection.
 */
void 
BrainModelSurfaceROINodeSelection::getExtentOfSelectedNodes(
                                             const BrainModelSurface* bms,
                                             float extentOut[6]) const
{
   extentOut[0] =  std::numeric_limits<float>::max();
   extentOut[1] = -std::numeric_limits<float>::max();
   extentOut[2] =  std::numeric_limits<float>::max();
   extentOut[3] = -std::numeric_limits<float>::max();
   extentOut[4] =  std::numeric_limits<float>::max();
   extentOut[5] = -std::numeric_limits<float>::max();
   
   if (bms == NULL) {
      return;
   }
   
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes != static_cast<int>(nodeSelectedFlags.size())) {
      return;
   }
   
   const CoordinateFile* cf = bms->getCoordinateFile();
   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] != 0) {
         const float* xyz = cf->getCoordinate(i);
         extentOut[0] = std::min(extentOut[0], xyz[0]);
         extentOut[1] = std::max(extentOut[1], xyz[0]);
         extentOut[2] = std::min(extentOut[2], xyz[1]);
         extentOut[3] = std::max(extentOut[3], xyz[1]);
         extentOut[4] = std::min(extentOut[4], xyz[2]);
         extentOut[5] = std::max(extentOut[5], xyz[2]);
      }
   }
}      

/**
 * get the nodes that have min/max x/y/z values.
 * If structure is invalid medial/lateral nodes will too be invalid (-1)
 */
void 
BrainModelSurfaceROINodeSelection::getNodesWithMinMaxXYZValues(
                                             const BrainModelSurface* bms,
                                             int& xMostMedial,
                                             int& xMostLateral,
                                             int& minXNode,
                                             int& maxXNode,
                                             int& minYNode,
                                             int& maxYNode,
                                             int& minZNode,
                                             int& maxZNode,
                                             int& absMinXNode,
                                             int& absMaxXNode,
                                             int& absMinYNode,
                                             int& absMaxYNode,
                                             int& absMinZNode,
                                             int& absMaxZNode) const
{
   xMostMedial = -1;
   xMostLateral = -1;
   minXNode = -1;
   maxXNode = -1;
   minYNode = -1;
   maxYNode = -1;
   minZNode = -1;
   maxZNode = -1;
   absMinXNode = -1;
   absMaxXNode = -1;
   absMinYNode = -1;
   absMaxYNode = -1;
   absMinZNode = -1;
   absMaxZNode = -1;
   
   float minX =  std::numeric_limits<float>::max();
   float maxX = -std::numeric_limits<float>::max();
   float minY =  std::numeric_limits<float>::max();
   float maxY = -std::numeric_limits<float>::max();
   float minZ =  std::numeric_limits<float>::max();
   float maxZ = -std::numeric_limits<float>::max();
   
   float absMinX =  std::numeric_limits<float>::max();
   float absMaxX = -std::numeric_limits<float>::max();
   float absMinY =  std::numeric_limits<float>::max();
   float absMaxY = -std::numeric_limits<float>::max();
   float absMinZ =  std::numeric_limits<float>::max();
   float absMaxZ = -std::numeric_limits<float>::max();
   
   if (bms == NULL) {
      return;
   }
   
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes != static_cast<int>(nodeSelectedFlags.size())) {
      return;
   }
   
   const CoordinateFile* cf = bms->getCoordinateFile();
   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] != 0) {
         float x, y, z;
         cf->getCoordinate(i, x, y, z);
         
         if (x < minX) {
            minX = x;
            minXNode = i;
         }
         if (x >= maxX) {
            maxX = x;
            maxXNode = i;
         }
         if (y < minY) {
            minY = y;
            minYNode = i;
         }
         if (y >= maxY) {
            maxY = y;
            maxYNode = i;
         }
         if (z < minZ) {
            minZ = z;
            minZNode = i;
         }
         if (z >= maxZ) {
            maxZ = z;
            maxZNode = i;
         }
         
         //
         // Do abs values
         //
         x = std::fabs(x);
         y = std::fabs(y);
         z = std::fabs(z);
         
         if (x < absMinX) {
            absMinX = x;
            absMinXNode = i;
         }
         if (x > absMaxX) {
            absMaxX = x;
            absMaxXNode = i;
         }
         if (y < absMinY) {
            absMinY = y;
            absMinYNode = i;
         }
         if (y > absMaxY) {
            absMaxY = y;
            absMaxYNode = i;
         }
         
         if (z < absMinZ) {
            absMinZ = z;
            absMinZNode = i;
         }
         if (z > absMaxZ) {
            absMaxZ = z;
            absMaxZNode = i;
         }
      }
   }
   
   //
   // Only set X medial and lateral if structure is valid
   //
   if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
      xMostMedial  = maxXNode;
      xMostLateral = minXNode;
   }
   else if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
      xMostMedial  = minXNode;
      xMostLateral = maxXNode;
   }
}                                       

/**
 * set the ROI selection into file.
 */
void 
BrainModelSurfaceROINodeSelection::setRegionOfInterestIntoFile(NodeRegionOfInterestFile& nroi) const
{
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   nroi.setNumberOfNodes(numNodes);
   for (int i = 0; i < numNodes; i++) {
      nroi.setNodeSelected(i, (nodeSelectedFlags[i] != 0));
   }
}

/**
 * get the ROI selection from file.
 */
void 
BrainModelSurfaceROINodeSelection::getRegionOfInterestFromFile(const NodeRegionOfInterestFile& nroi) throw (FileException)
{
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   if (numNodes == 0) {
      nodeSelectedFlags.resize(numNodes, false);
   }
   else if (numNodes != nroi.getNumberOfNodes()) {
      throw FileException("ROI and ROI-File have a different number of nodes.");
   }
   for (int i = 0; i < numNodes; i++) {
      if (nroi.getNodeSelected(i)) {
         nodeSelectedFlags[i] = 1;
      }
      else {
         nodeSelectedFlags[i] = 0;
      }
   }
}      

/**
 * expand the ROI so that these nodes are within the ROI and connected.
 */
void 
BrainModelSurfaceROINodeSelection::expandSoNodesAreWithinAndConnected(const BrainModelSurface* selectionSurface,
                                                                      const int node1,
                                                                      const int node2)
{
   update();
   
   //
   // If the ROI is empty, start with the two nodes
   //
   if (getNumberOfNodesSelected() <= 0) {
      nodeSelectedFlags[node1] = 1;
      nodeSelectedFlags[node2] = 1;
   }
   
   //
   // Dilate the ROI until node1 and node2 are both in the ROI
   //
   int dilateCounterGetNodesIntoROI = 0;
   while ((nodeSelectedFlags[node1] == 0) ||
          (nodeSelectedFlags[node2] == 0)) {
      dilate(selectionSurface, 1);
      dilateCounterGetNodesIntoROI++;
   }   
   //if (DebugControl::getDebugOn()) {
      std::cout << dilateCounterGetNodesIntoROI << " iterations needed to get nodes "
                << node1 << " and " << node2
                << " into the ROI" << std::endl;
   //}
   
   //
   // do until the two nodes are connected
   //
   bool nodesConnected = false;
   int dilateCounterConnectNodes = 0;
   while (nodesConnected == false) {
      //
      // Are the nodes connected?
      //
      if (areNodesConnected(selectionSurface,
                            nodeSelectedFlags,
                            node1,
                            node2)) {
         //
         // Done searching!
         //
         nodesConnected = true;
      }                       
      else {
         //
         // Dilate one iteration
         //
         dilate(selectionSurface, 1);
         dilateCounterConnectNodes++;
      }
   }
   
   //if (DebugControl::getDebugOn()) {
      std::cout << dilateCounterConnectNodes << " iterations needed to connect nodes "
                << node1 << " and " << node2
                << " in the ROI" << std::endl;
   //}
   
   //
   // Was the ROI dilated in order to get the nodes into the ROI
   // or connect the nodes
   //
   const int numberOfErodeIterations = dilateCounterGetNodesIntoROI
                                     + dilateCounterConnectNodes;
   if (numberOfErodeIterations > 0) {
      //
      // Erode an equal number of iterations which essentially make this a 
      // closing operation (dilation followed by erosion)
      //
      for (int i = 0; i < numberOfErodeIterations; i++) {
         //
         // Save selected nodes
         //
         const std::vector<int> savedSelection = nodeSelectedFlags;
         
         //
         // Erode one itereation
         //
         erodeButMaintainNodeConnection(selectionSurface, 1, node1, node2);
/*
         erode(selectionSurface, 1);
         
         //
         // Erosion could cause nodes to be disconnected
         //
         if (areNodesConnected(selectionSurface,
                               nodeSelectedFlags,
                               node1,
                               node2) == false) {
            //
            // restore nodes selected and stop eroding
            //
            nodeSelectedFlags = savedSelection;
            break;
         }
*/
      }
   }
   
   //if (DebugControl::getDebugOn()) {
      std::cout << numberOfErodeIterations << " erosion iterations were performed "
                << std::endl;
   //}
}

/**
 * see if two nodes are connected in the ROI.
 */
bool 
BrainModelSurfaceROINodeSelection::areNodesConnected(const BrainModelSurface* bms,
                                                     const std::vector<int>& connectionFlags,
                                                     const int node1,
                                                     const int node2)
{
   //
   // Find nodes connected to first node
   //
   BrainModelSurfaceConnectedSearch connectedSearch(brainSet,
                                                    (BrainModelSurface*)bms,
                                                    node1,
                                                    &connectionFlags);
   try {
      connectedSearch.execute();
   }
   catch (BrainModelAlgorithmException&) {
   }
   
   //
   // Are the nodes connected?
   //
   if (connectedSearch.getNodeConnected(node2)) {
      //
      // Done searching!
      //
      return true;
   }
   
   return false;
}                                              

/**
 * erode the selected nodes but maintain a connection between two nodes.
 */
void 
BrainModelSurfaceROINodeSelection::erodeButMaintainNodeConnection(const BrainModelSurface* selectionSurface,
                                                                  int numberOfIterations,
                                                                  const int node1,
                                                                  const int node2)
{
   QString topologyHelperErrorMessage;
   const TopologyHelper* th = getSelectionSurfaceTopologyHelper(selectionSurface,
                                                                topologyHelperErrorMessage);
   if (th == NULL) {
      return;
   }
   
   update();
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   
   //
   // Keeps track of nodes that cannot be eroded without breaking the connection
   //
   std::vector<int> nodesThatMayNotBeEroded(numNodes, 0);
   nodesThatMayNotBeEroded[node1] = 1;
   nodesThatMayNotBeEroded[node2] = 1;
   
   //
   // For specified number of dilation iterations
   //
   for (int iter = 0; iter < numberOfIterations; iter++) {
      //
      // Output for eroding
      //
      std::vector<int> nodesSelectedThisIteration = nodeSelectedFlags;
   
      //
      // Keeps track of nodes that were eroded this iteration
      //
      std::vector<int> nodesErodedThisIteration;
      
      //
      // Check each node
      //
      for (int i = 0; i < numNodes; i++) {
         // 
         // Is this node in the ROI
         //
         if (nodeSelectedFlags[i]) {
            //
            // Get node's neighbors
            //
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
            
            //
            // If a neighbor is not in the ROI, take "this" node out of ROI
            //
            for (int j = 0; j < numNeighbors; j++) {
               if (nodeSelectedFlags[neighbors[j]] == 0) {
                  //
                  // Make sure node can be eroded
                  //
                  if (nodesThatMayNotBeEroded[i] == 0) {
                     nodesSelectedThisIteration[i] = 0;
                     nodesErodedThisIteration.push_back(i);
                     break;
                  }
               }
            }
         }
      }
   
      //
      // See if nodes are NOT connected
      //
      if (areNodesConnected(selectionSurface, nodesSelectedThisIteration, node1, node2) == false) {
         //
         // Reset to status before this iterations erosion
         //
         nodesSelectedThisIteration = nodeSelectedFlags;

         //
         // Examine each node that was eroded this pass to see if it caused the 
         // break in the connection
         //
         const int num = static_cast<int>(nodesErodedThisIteration.size());
         for (int m = 0; m < num; m++) {
            //
            // Get node that was eroded
            //
            const int nodeNum = nodesErodedThisIteration[m];
            
            //
            // Can this node be eroded
            //
            if (nodesThatMayNotBeEroded[nodeNum] == 0) {
               //
               // Remove the node from the ROI
               //
               nodesSelectedThisIteration[nodeNum] = 0;
               
               //
               // See if nodes are NOT connected
               //
               if (areNodesConnected(selectionSurface, nodesSelectedThisIteration, node1, node2) == false) {
                  //
                  // Put node back in ROI and do not allow node to be eroded
                  //
                  nodesSelectedThisIteration[nodeNum] = 1;
                  nodesThatMayNotBeEroded[nodeNum] = 1;
               }
            }
         }
      }
      
      //
      // Output erosion
      //
      nodeSelectedFlags = nodesSelectedThisIteration;
   }
}

   
/**
 * select nodes within border (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithinBorder(
                                         const SELECTION_LOGIC selectionLogic,
                                         const BrainModelSurface* selectionSurface,
                                         const Border* borderIn,
                                         const bool surface3DFlag,
                                         const float zMinimum)
{
   //
   // Make all border Z zero
   //
   Border border = *borderIn;
   const int numLinks = border.getNumberOfLinks();
   for (int i = 0; i < numLinks; i++) {
      float xyz[3];
      border.getLinkXYZ(i, xyz);
      xyz[2] = 0.0;
      border.setLinkXYZ(i, xyz);
   }
   
   //
   // Test nodes within border
   //
   const int numNodes = selectionSurface->getNumberOfNodes();
   std::vector<bool> insideBorderFlags(numNodes);
   border.pointsInsideBorder2D(selectionSurface->getCoordinateFile()->getCoordinate(0),
                                numNodes,
                                insideBorderFlags,
                                surface3DFlag,
                                zMinimum);
   
   //
   // Get nodes within border
   //
   std::vector<int> nodeFlags(numNodes, 0);
   for (int i = 0; i < numNodes; i++) {
      if (insideBorderFlags[i]) {
         nodeFlags[i] = 1;
      }
   }
   
   //
   // Update the selected nodes
   //
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   nodeFlags);
}
