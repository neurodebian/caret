
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
#include <stack>

#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceConnectedSearchMetric.h"
#include "BrainModelSurfaceGeodesic.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "DebugControl.h"
#include "GeodesicDistanceFile.h"
#include "LatLonFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "StringUtilities.h"
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
 * copy constructor.
 */
BrainModelSurfaceROINodeSelection::BrainModelSurfaceROINodeSelection(
                            const BrainModelSurfaceROINodeSelection& roi)
{
   copyHelper(roi);
}

/**
 * Assignment operator.
 */
BrainModelSurfaceROINodeSelection& 
BrainModelSurfaceROINodeSelection::operator=(const BrainModelSurfaceROINodeSelection& roi)
{
   if (this != &roi) {
      copyHelper(roi);
   }
   
   return *this;
}

/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
BrainModelSurfaceROINodeSelection::copyHelper(const BrainModelSurfaceROINodeSelection& roi)
{
   brainSet = roi.brainSet;
   nodeSelectedFlags = roi.nodeSelectedFlags;
   displaySelectedNodes = roi.displaySelectedNodes;
   selectionDescription = roi.selectionDescription;
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
   selectionDescription = "";
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
                                   nodeFlags,
                                   "All Nodes");
}

/**
 * select nodes that are edges.
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesThatAreEdges(const SELECTION_LOGIC selectionLogic,
                                                           const BrainModelSurface* selectionSurface)
{
   update();
   
   brainSet->classifyNodes(selectionSurface->getTopologyFile());
   
   const unsigned int numNodes = nodeSelectedFlags.size();
   std::vector<int> nodeFlags(numNodes, 0);
   
   bool thereAreEdgeNodesFlag = false;
   for (unsigned int i = 0; i < numNodes; i++) {
      const BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(i);
      if (bna->getClassification() ==
          BrainSetNodeAttribute::CLASSIFICATION_TYPE_EDGE) {
         nodeFlags[i] = 1;
         thereAreEdgeNodesFlag = true;
      }
   }
   
   if (thereAreEdgeNodesFlag == false) {
      return "There are no edges in the surface.";
   }
   
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   nodeFlags,
                                   "Edge Nodes");
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
   
   const QString descriptionSaved = selectionDescription;
   const QString msg = processNewNodeSelections(SELECTION_LOGIC_NORMAL,
                                   selectionSurface,
                                   newNodeSelections,
                                   "Invert Selection");
   selectionDescription = descriptionSaved;
   addToSelectionDescription("", "Invert Selection");
   
   return msg;
}

/**
 * select nodes within geodesic distance.
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithinGeodesicDistance(
                                          const SELECTION_LOGIC selectionLogic,
                                          const BrainModelSurface* selectionSurface,
                                          const int nodeNumber,
                                          const float geodesicDistance)
{
   const int numNodes = selectionSurface->getNumberOfNodes();
   if ((nodeNumber < 0) ||
       (nodeNumber >= numNodes)) {
      return "Invalid node number for selecting nodes with geodesic.";
   }
   
   GeodesicDistanceFile geodesicDistanceFile;
   BrainModelSurfaceGeodesic geodesic(brainSet,
                                      (BrainModelSurface*)selectionSurface,
                                      NULL,
                                      -1,
                                      "",
                                      &geodesicDistanceFile,
                                      -1,
                                      "GeoDist",
                                      nodeNumber,
                                      NULL);
   try {
      geodesic.execute();
   }
   catch (BrainModelAlgorithmException&) {
      return ("Selecting nodes with geodesic failed for node number "
              + QString::number(nodeNumber));
   }
        
   if ((geodesicDistanceFile.getNumberOfNodes() == numNodes) &&
       (geodesicDistanceFile.getNumberOfColumns() >= 1)) {
      std::vector<int> newNodeSelections(numNodes, 0);
      
      for (int i = 0; i < numNodes; i++) {
         if (geodesicDistanceFile.getNodeParentDistance(i, 0)
             < geodesicDistance) {
            newNodeSelections[i] = 1;
         }
      }
      newNodeSelections[nodeNumber] = 1;
      
      const QString& description = 
         ("Nodes within  "
          + QString::number(geodesicDistance, 'f', 3)
          + " geodesic distance of node number "
          + QString::number(nodeNumber));
         
      return processNewNodeSelections(selectionLogic,
                                      selectionSurface,
                                      newNodeSelections,
                                      description);
   }
   return ("Selecting nodes with geodesic failed for node number "
           + QString::number(nodeNumber));
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
      return "ERROR: Paint name " + paintName + "not found in paint file.";
   }
   
   const int numNodes = pf->getNumberOfNodes();
   std::vector<int> newNodeSelections(numNodes, 0);
   
   for (int i = 0; i < numNodes; i++) {
      if (pf->getPaint(i, paintFileColumnNumber) == paintIndex) {
         newNodeSelections[i] = 1;
      }
   }
   
   const QString& description = 
      ("Nodes assigned the name "
       + paintName
       + " in column named "
       + pf->getColumnName(paintFileColumnNumber));
      
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections,
                                   description);
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
                                   newNodeSelections,
                                   "Nodes within borders named " + borderName);
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
   
   const QString description =
      ("Node with latitude range ("
       + QString::number(minimumLatitude, 'f', 4)
       + ", "
       + QString::number(maximumLatitude, 'f', 4)
       + ") and longitude range ("
       + QString::number(minimumLongitude, 'f', 4)
       + ", "
       + QString::number(maximumLongitude, 'f', 4)
       + ")");
       
       
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections,
                                   description);
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
      
   const QString description = 
      ("Nodes in range ("
       + QString::number(minimumMetricValue, 'f', 3)
       + ", "
       + QString::number(maximumMetricValue, 'f', 3)
       + ") in "
       + fileType
       + " column named "
       + metricFile->getColumnName(metricFileColumnNumber));
       
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections,
                                   description);
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
   
   const QString description = 
      ("Nodes in range ("
       + QString::number(minimumMetricValue, 'f', 3)
       + ", "
       + QString::number(maximumMetricValue, 'f', 3)
       + ") in "
       + fileType
       + " column named "
       + metricFile->getColumnName(metricFileColumnNumber)
       + " connected to node "
       + QString::number(connectedToNodeNumber));
       
   return processNewNodeSelections(selectionLogic,
                                   selectionSurface,
                                   newNodeSelections,
                                   description);
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
                                   newNodeSelections,
                                   "Nodes identified as crossovers");
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
   
   addToSelectionDescription("",
                             ("Dilated "
                              + QString::number(numberOfIterations)
                              + " iterations"));
}

/**
 * dilate around the node (adds node's neighbors to ROI).
 */
void 
BrainModelSurfaceROINodeSelection::dilateAroundNode(const BrainModelSurface* selectionSurface,
                                                    const int nodeNumber)
{
   QString topologyHelperErrorMessage;
   const TopologyHelper* th = getSelectionSurfaceTopologyHelper(selectionSurface,
                                                                topologyHelperErrorMessage);
   if (th == NULL) {
      return;
   }
   
   update();
   
   //
   // Put node's neighbors into the ROI
   //
   int numNeighbors = 0;
   const int* neighbors = th->getNodeNeighbors(nodeNumber, numNeighbors);
   
   //
   // Add neighbors to the ROI
   //
   for (int j = 0; j < numNeighbors; j++) {
      nodeSelectedFlags[neighbors[j]] = 1;
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
   
   const QString description =
      ("Dilated "
       + QString::number(numberOfIterations)
       + " iterations nodes with paint name "
       + paintName
       + " in paint column "
       + paintFile->getColumnName(paintColumnNumber));
   addToSelectionDescription("",
                             description);
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
   
   addToSelectionDescription("", "Boundary nodes only");
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

   addToSelectionDescription("",
                             ("Eroded "
                              + QString::number(numberOfIterations)
                              + " iterations"));
}

/**
 * exclude nodes in a region.
 */
void
BrainModelSurfaceROINodeSelection::excludeNodesInRegion(
                          const BrainModelSurface* selectionSurface,
                          const float regionExtent[6])
{
   update();
   const int numNodes = static_cast<int>(nodeSelectedFlags.size());
   
   const CoordinateFile* cf = selectionSurface->getCoordinateFile();
   
   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] != 0) {
         const float* xyz = cf->getCoordinate(i);
         if ((xyz[0] > regionExtent[0]) &&
             (xyz[0] < regionExtent[1]) &&
             (xyz[1] > regionExtent[2]) &&
             (xyz[1] < regionExtent[3]) &&
             (xyz[2] > regionExtent[4]) &&
             (xyz[2] < regionExtent[5])) {
            nodeSelectedFlags[i] = 0;
         }
      }
   }
   
   const QString description =
      ("Excluded in region extent "
        "(" + QString::number(regionExtent[0], 'f', 3) 
             + ", " + QString::number(regionExtent[1], 'f', 3) + ") "
         "(" + QString::number(regionExtent[2], 'f', 3) 
             + ", " + QString::number(regionExtent[3], 'f', 3) + ") "
         "(" + QString::number(regionExtent[4], 'f', 3) 
             + ", " + QString::number(regionExtent[5], 'f', 3) + ") ");
   addToSelectionDescription("", description);
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
   
   const QString description =
      ("Limit extent "
        "(" + QString::number(extent[0], 'f', 3) 
             + ", " + QString::number(extent[1], 'f', 3) + ") "
         "(" + QString::number(extent[2], 'f', 3) 
             + ", " + QString::number(extent[3], 'f', 3) + ") "
         "(" + QString::number(extent[4], 'f', 3) 
             + ", " + QString::number(extent[5], 'f', 3) + ") ");
   addToSelectionDescription("", description);
}      

/**
 * process the new node selections.
 */
QString 
BrainModelSurfaceROINodeSelection::processNewNodeSelections(
                                             const SELECTION_LOGIC selectionLogic,
                                             const BrainModelSurface* selectionSurface,
                                             std::vector<int>& newNodeSelections,
                                             const QString& description)
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
   
   //
   // Update description of selection
   //
   QString logicText;
   switch (selectionLogic) {
      case SELECTION_LOGIC_NORMAL:
         logicText = "";
         selectionDescription = "";
         break;
      case SELECTION_LOGIC_AND:
         logicText = "AND";
         break;
      case SELECTION_LOGIC_OR:
         logicText = "OR";
         break;
      case SELECTION_LOGIC_AND_NOT:
         logicText = "AND-NOT";
         break;
   }
   addToSelectionDescription(logicText, description);
   
   return "";
}

/**
 * add to the selection description.
 */
void 
BrainModelSurfaceROINodeSelection::addToSelectionDescription(const QString& selectionLogicText,
                                                             const QString& descriptionIn)
{
   QString description = descriptionIn;
   StringUtilities::lineWrapString(70, description);
   if (selectionDescription.isEmpty() == false) {
      selectionDescription += "\n";
   }
   selectionDescription += (selectionLogicText
                            + " "
                            + description);
                            
   if (DebugControl::getDebugOn()) {
      std::cout << "ROI: " 
                << selectionDescription.toAscii().constData()
                << std::endl
                << std::endl;
   }
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
 * get node with most lateral X Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMostLateralXCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return mostLateralXNode;
}

/**
 * get node with most medial X Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMostMedialXCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return mostMedialXNode;
}

/**
 * get node with minimum X Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMinimumXCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return minXNode;
}

/**
 * get node with maximum X Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMaximumXCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return maxXNode;
}

/**
 * get node with minimum Y Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMinimumYCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return minYNode;
}

/**
 * get node with maximum Y Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMaximumYCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return maxYNode;
}

/**
 * get node with minimum Z Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMinimumZCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return minZNode;
}

/**
 * get node with maximum Z Coordinate.
 */
int 
BrainModelSurfaceROINodeSelection::getNodeWithMaximumZCoordinate(const BrainModelSurface* surface) const
{
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   getNodesWithMinMaxXYZValues(surface,
                               mostMedialXNode,
                               mostLateralXNode,
                               minXNode, 
                               maxXNode,
                               minYNode,
                               maxYNode,
                               minZNode,
                               maxZNode,
                               absMinXNode,
                               absMaxXNode,
                               absMinYNode,
                               absMaxYNode,
                               absMinZNode,
                               absMaxZNode);
                                    
   return maxZNode;
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
 * get the surface area of the ROI.
 */
float 
BrainModelSurfaceROINodeSelection::getSurfaceAreaOfROI(const BrainModelSurface* surface) const
{
   float surfaceArea = 0.0;
   
   const TopologyFile* tf = surface->getTopologyFile();
   const CoordinateFile* cf = surface->getCoordinateFile();

   const int numTiles = tf->getNumberOfTiles();
   for (int i = 0; i < numTiles; i++) {
      int n1, n2, n3;
      tf->getTile(i, n1, n2, n3);
      const float tileArea = MathUtilities::triangleArea(cf->getCoordinate(n1),
                                                         cf->getCoordinate(n2),
                                                         cf->getCoordinate(n3));
     
      double numMarked = 0.0;
      if (nodeSelectedFlags[n1]) numMarked += 1.0;
      if (nodeSelectedFlags[n2]) numMarked += 1.0;
      if (nodeSelectedFlags[n3]) numMarked += 1.0;
      
      if (tileArea > 0.0) {
         surfaceArea += (numMarked / 3.0) * tileArea;
      }
   }
   
   return surfaceArea;
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
   nroi.setRegionOfInterestDescription(selectionDescription);
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
   selectionDescription = nroi.getRegionOfInterestDescription();
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
   const std::vector<int> nodesThatMayNotBeErodedFlags = nodeSelectedFlags;
   
   //
   // Dilate the ROI until node1 and node2 are both in the ROI
   //
   int dilateCounterGetNodesIntoROI = 0;
   while ((nodeSelectedFlags[node1] == 0) ||
          (nodeSelectedFlags[node2] == 0)) {
      dilate(selectionSurface, 1);
      dilateCounterGetNodesIntoROI++;
   }   
   if (DebugControl::getDebugOn()) {
      std::cout << dilateCounterGetNodesIntoROI << " iterations needed to get nodes "
                << node1 << " and " << node2
                << " into the ROI" << std::endl;
   }
   
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
   
   if (DebugControl::getDebugOn()) {
      std::cout << dilateCounterConnectNodes << " iterations needed to connect nodes "
                << node1 << " and " << node2
                << " in the ROI" << std::endl;
   }
   
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
         erodeButMaintainNodeConnection(selectionSurface, 
                                        nodesThatMayNotBeErodedFlags,
                                        1, node1, node2);

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
   
   if (DebugControl::getDebugOn()) {
      std::cout << numberOfErodeIterations << " erosion iterations were performed "
                << std::endl;
   }
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
 * erode the selected nodes but maintain a connection between two nodes.
 */
void 
BrainModelSurfaceROINodeSelection::erodeButMaintainNodeConnection(const BrainModelSurface* selectionSurface,
                                             const std::vector<int>& nodesThatMayNotBeErodedIn,
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
   std::vector<int> nodesThatMayNotBeEroded = nodesThatMayNotBeErodedIn;
   nodesThatMayNotBeEroded.resize(numNodes, 0);
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
                                   nodeFlags,
                                   "Within Border Named " + border.getName());
}

/**
 * select nodes within border (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::selectNodesWithinBorderOnSphere(
                                      const SELECTION_LOGIC selectionLogic,
                                      const BrainModelSurface* sphericalSurface,
                                      const BorderProjection* borderProjection)
{
   //
   // Copy sphere and orient so COG on positive Z-Axis
   //
   BrainModelSurface surface(*sphericalSurface);

   //
   // Create topology helper
   //
   const TopologyHelper* th = 
      surface.getTopologyFile()->getTopologyHelper(false, true, false);

   //
   // Get the center of gravity of the border projection
   //
   float cog[3];
   borderProjection->getCenterOfGravity(surface.getCoordinateFile(),
                                        th,
                                        cog);
   //
   // Copy sphere and orient so COG on positive Z-Axis
   //
   surface.orientPointToPositiveZAxis(cog);
   
   //
   // Unproject the border
   //
   Border border;
   borderProjection->unprojectBorderProjection(surface.getCoordinateFile(),
                                               th,
                                               border);
   
   //
   // Select the nodes
   //
   const QString message = selectNodesWithinBorder(selectionLogic,
                                                 &surface,
                                                 &border,
                                                 true,
                                                 1.0);

   if (DebugControl::getDebugOn()) {
      try {
         CoordinateFile cf = *surface.getCoordinateFile();
         cf.writeFile("Sphere_Orient_For_Border_Inclusion.coord");
         BorderProjectionFile bpf;
         bpf.addBorderProjection(*borderProjection);
         bpf.writeFile("Sphere_Orient_For_Border_Inclusion.borderproj");
         NodeRegionOfInterestFile roiFile;
         setRegionOfInterestIntoFile(roiFile);
         roiFile.writeFile("Sphere_Orient_For_Border_Inclusion.roi");
      }
      catch (FileException&) {
      }
   }
   
   return message;
}                                   

/**
 * discard islands with islands that have fewer than specified number of nodes.
 */
int 
BrainModelSurfaceROINodeSelection::discardIslands(const BrainModelSurface* selectionSurface,
                                                  const int minimumNumberOfNodesInIslandsKept)
{
   std::vector<int> islandRootNode;
   std::vector<int> islandNumNodes;
   std::vector<int> nodeRootNeighbor;
   const int numPieces = findIslands(selectionSurface,
                                     islandRootNode, 
                                     islandNumNodes, 
                                     nodeRootNeighbor);
   
   //
   // See if there are any islands
   //
   if (numPieces <= 1) {
      return 0;
   }

   const int numNodes = selectionSurface->getNumberOfNodes();
   
   //
   // Deselect all nodes that are not 
   // connected to the node with the most connected neighbors
   //
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numPieces; j++) {
         if (islandRootNode[j] == nodeRootNeighbor[i]) {
            if (islandNumNodes[j] < minimumNumberOfNodesInIslandsKept) {
               nodeSelectedFlags[i] = false;
            }
            break;
         }
      }
   }
   
   addToSelectionDescription("",
                             ("Removed "
                              + QString::number(numPieces - 1)
                              + " islands containing less than "
                              + QString::number(minimumNumberOfNodesInIslandsKept)
                              + " nodes."));
   return numPieces - 1;
}      

/**
 * discard islands keeps the largest contiguous piece (returns number of islands removed).
 */
int 
BrainModelSurfaceROINodeSelection::discardIslands(const BrainModelSurface* selectionSurface)
{
   std::vector<int> islandRootNode;
   std::vector<int> islandNumNodes;
   std::vector<int> nodeRootNeighbor;
   const int numPieces = findIslands(selectionSurface,
                                     islandRootNode, 
                                     islandNumNodes, 
                                     nodeRootNeighbor);
   
   //
   // See if there are any islands
   //
   if (numPieces <= 1) {
      return 0;
   }

   const int numNodes = selectionSurface->getNumberOfNodes();

   //
   // find node with most connected neighbors
   //
   int mostNeighborsNode = -1;
   int mostNeighbors = 0;
   for (int j = 0; j < numPieces; j++) {
      if (islandNumNodes[j] > 0) {
         if (DebugControl::getDebugOn()) {
            std::cout << islandRootNode[j] << " is connected to " << islandNumNodes[j]
                      << " nodes." << std::endl;
         }
      }
      if (islandNumNodes[j] > mostNeighbors) {
         mostNeighborsNode = islandRootNode[j];
         mostNeighbors     = islandNumNodes[j];
      }
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << mostNeighborsNode << " has the most neighbors = "
                << mostNeighbors << std::endl;
   }
   
   //
   // Deselect all nodes that are not 
   // connected to the node with the most connected neighbors
   //
   if (mostNeighborsNode >= 0) {
      for (int i = 0; i < numNodes; i++) {
         if (nodeRootNeighbor[i] != mostNeighborsNode) {
            nodeSelectedFlags[i] = false;
         }
      }
   }
   
   addToSelectionDescription("",
                             ("Removed "
                              + QString::number(numPieces - 1)
                              + " islands."));
   return numPieces - 1;
}

/**
 * find islands (number of disjoint groups of nodes).
 * Returns number disjoint connected pieces of surface.
 * Return value of 1 indicates no islands (one connected piece of surface).
 * Return value of 0 indicates no topology.
 * Return value greater than 1 indicates islands.
 *
 * islandRootNode - contains a node in the piece of surface and the number of
 *                  elements is the number returned by this method.
 * islandNumNodes - is the number of nodes in the piece of surface and the
 *                  number of elements is the number returned by this method.
 * nodeRootNeighbor - is the "islandRootNode" for the node "i" and the number
 *                    of elements is the number of nodes in the surface.
 */
int 
BrainModelSurfaceROINodeSelection::findIslands(const BrainModelSurface* selectionSurface,
                                               std::vector<int>& islandRootNode,
                                               std::vector<int>& islandNumNodes,
                                               std::vector<int>& nodeRootNeighbor)
{
   update();

   islandRootNode.clear();
   islandNumNodes.clear();
   nodeRootNeighbor.clear();
    
   //
   // Get this topology file's topology helper (DO NOT DELETE IT)
   //
   QString topologyHelperErrorMessage;
   const TopologyHelper* topologyHelper = getSelectionSurfaceTopologyHelper(selectionSurface,
                                                                topologyHelperErrorMessage);
   const int numNodes = topologyHelper->getNumberOfNodes();
   
   if (numNodes == 0) {
      return 0;
   }
   
   if (numNodes == 1) {
      for (int i = 0; i < numNodes; i++) {
         if (nodeSelectedFlags[i]) {
            if (topologyHelper->getNodeHasNeighbors(i)) {
               islandRootNode.push_back(i);
               islandNumNodes.push_back(1);
               nodeRootNeighbor.push_back(i);
               break;
            }
         }
      }
      return static_cast<int>(islandRootNode.size());
   }
   
   nodeRootNeighbor.resize(numNodes);
   std::fill(nodeRootNeighbor.begin(), nodeRootNeighbor.end(), -1);

   std::vector<int> numConnectedNeighbors(numNodes, 0);
   std::vector<int> visited(numNodes, 0);
   
   //
   // Mark all nodes without neighbors or NOT in ROI as visited
   //
   for (int i = 0; i < numNodes; i++) {
      if ((topologyHelper->getNodeHasNeighbors(i) == false) ||
          (nodeSelectedFlags[i] == 0)) {
         visited[i] = 1;
      }
   } 
 
   //
   // Search the surface marking all connected nodes.
   //
   for (int n = 0; n < numNodes; n++) {
      if (visited[n] == 0) {
         
         const int nodeNumberIn = n;
         const int origNeighbor = n;
         
         std::stack<int> st;
         st.push(nodeNumberIn);
         
         while(!st.empty()) {
            const int nodeNumber = st.top();
            st.pop();
            
            if (visited[nodeNumber] == 0) {
               visited[nodeNumber] = 1;
               nodeRootNeighbor[nodeNumber] = origNeighbor;
               numConnectedNeighbors[origNeighbor]++;
               std::vector<int> neighbors;
               topologyHelper->getNodeNeighbors(nodeNumber, neighbors);
               for (int i = 0; i < static_cast<int>(neighbors.size()); i++) {
                  const int node = neighbors[i];
                  if (visited[node] == 0) {
                     st.push(node);
                  }
               }
            }
         }
      }
   }
   
   //
   // set the islands
   //
   for (int j = 0; j < numNodes; j++) {
      if (numConnectedNeighbors[j] > 0) {
         islandRootNode.push_back(j);
         islandNumNodes.push_back(numConnectedNeighbors[j]);
         if (DebugControl::getDebugOn()) {
            std::cout << j << " is connected to " << numConnectedNeighbors[j]
                      << " nodes." << std::endl;
         }
      }
   }   
   
   return static_cast<int>(islandRootNode.size());
}

/**
 * find islands and place each in an ROI.
 */
std::vector<BrainModelSurfaceROINodeSelection*> 
BrainModelSurfaceROINodeSelection::findIslands(const BrainModelSurface* selectionSurface)
{
   std::vector<BrainModelSurfaceROINodeSelection*> islandsOut;
   
   //
   // Determine islands
   //
   std::vector<int> islandRootNode, islandNumNodes, nodeRootNeighbor;
   const int numIslands = findIslands(selectionSurface,
                                      islandRootNode,
                                      islandNumNodes,
                                      nodeRootNeighbor);
                                      
   if (numIslands > 0) {
      const int numNodes = selectionSurface->getNumberOfNodes();
      
      //
      // Contains mapping of Root node to ROI index
      //
      std::vector<int> rootNodeToRoiIndex(numNodes, -1);

      //
      // Create new ROIs for each island
      //
      for (int i = 0; i < numIslands; i++) {
         rootNodeToRoiIndex[islandRootNode[i]] = i;
         islandsOut.push_back(new BrainModelSurfaceROINodeSelection(brainSet));
      }
      
      //
      // Loop through nodes
      //
      for (int i = 0; i < numNodes; i++) {
         //
         // Assign node to proper ROI
         //
         const int roiNodeNumber = nodeRootNeighbor[i];
         if (roiNodeNumber >= 0) {
            const int roiIndex = rootNodeToRoiIndex[roiNodeNumber];
            islandsOut[roiIndex]->setNodeSelected(i, true);
         }
      }
      
/*
      for (int i = 0; i < numIslands; i++) {
         const int rootNode = islandRootNode[i];
         
         //
         // Create an ROI containing nodes in the island
         //
         BrainModelSurfaceROINodeSelection* island
            = new BrainModelSurfaceROINodeSelection(brainSet);
         for (int j = 0; j < numNodes; j++) {
            if (nodeRootNeighbor[j] == rootNode) {
               island->setNodeSelected(j, true);
            }
         }
         islandsOut.push_back(island);
      }
*/
   }
   
   return islandsOut;
}

/**
 * find node int ROI nearest the XYZ.
 */
int 
BrainModelSurfaceROINodeSelection::getNearestNodeInROI(
                                    const BrainModelSurface* selectionSurface,
                                    const float xyz[3]) const
{
   //update();
   
   const CoordinateFile* cf = selectionSurface->getCoordinateFile();
   
   int nearestNodeNumber = -1;
   float nearestNodeDistanceSquared = std::numeric_limits<float>::max();
   
   const int numNodes = selectionSurface->getNumberOfNodes();

   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i]) {
         const float distSQ = cf->getDistanceToPointSquared(i, xyz);
         if (distSQ < nearestNodeDistanceSquared) {
            nearestNodeNumber = i;
            nearestNodeDistanceSquared = distSQ;
         }
      }
   }

   return nearestNodeNumber;
}

/**
 * find node int ROI nearest the XYZ.
 */
int 
BrainModelSurfaceROINodeSelection::getNearestNodeInROI(
                             const BrainModelSurface* selectionSurface,
                             const float x, 
                             const float y, 
                             const float z) const
{
   const float xyz[3] = { x, y, z };
   return getNearestNodeInROI(selectionSurface, xyz);
}
      
/**
 * logically and this roi with another roi (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::logicallyAND(const BrainModelSurfaceROINodeSelection* otherROI)
{
   update();
   
   const int numNodes = getNumberOfNodes();
   if (numNodes != otherROI->getNumberOfNodes()) {
      return "Unable to AND ROIs because they have a different number of nodes.";
   }
   
   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] &&
          otherROI->nodeSelectedFlags[i]) {
         nodeSelectedFlags[i] = true;
      }
      else {
         nodeSelectedFlags[i] = false;
      }
   }
   
   selectionDescription = 
      "["
      + selectionDescription
      + "] AND ["
      + otherROI->selectionDescription
      + "]";
      
   return "";
}

/**
 * logically or this roi with another roi (returns error message).
 */
QString 
BrainModelSurfaceROINodeSelection::logicallyOR(const BrainModelSurfaceROINodeSelection* otherROI)
{
   update();
   
   const int numNodes = getNumberOfNodes();
   if (numNodes != otherROI->getNumberOfNodes()) {
      return "Unable to AND ROIs because they have a different number of nodes.";
   }
   
   for (int i = 0; i < numNodes; i++) {
      if (nodeSelectedFlags[i] ||
          otherROI->nodeSelectedFlags[i]) {
         nodeSelectedFlags[i] = true;
      }
      else {
         nodeSelectedFlags[i] = false;
      }
   }
   selectionDescription = 
      "["
      + selectionDescription
      + "] AND ["
      + otherROI->selectionDescription
      + "]";
      
   return "";
}
      
      

