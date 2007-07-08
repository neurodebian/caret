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

#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <cmath>
#include <limits>
#include <iostream>
#include <set>
#include <sstream>

#include <QDir>

#include "BorderFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformationMeasurement.h"
#include "BrainModelSurfaceGeodesic.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "GeodesicDistanceFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceDeformationMeasurement::BrainModelSurfaceDeformationMeasurement(BrainSet* bs,
                                     BrainModelSurface* bmsIn,
                                     BorderFile* borderFileIn,
                                     MetricFile* metricFileIn,
                                     const int sphereNumberIn)
   : BrainModelAlgorithm(bs)
{
   bms = bmsIn;
   borderFile = borderFileIn;
   userMetricFile = metricFileIn;
   sphereNumber = sphereNumberIn;
}
                                     
/**
 * Destructor.
 */
BrainModelSurfaceDeformationMeasurement::~BrainModelSurfaceDeformationMeasurement()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceDeformationMeasurement::execute() throw (BrainModelAlgorithmException)
{
   if (bms == NULL) {
      throw BrainModelAlgorithmException("The surface is invalid.");
   }
   if (bms->getNumberOfNodes() <= 0) {
      throw BrainModelAlgorithmException("The surface has no nodes.");
   }
   if (bms->getTopologyFile() == NULL) {
      throw BrainModelAlgorithmException("The surface has no topology.");
   }
   if (borderFile == NULL) {
      throw BrainModelAlgorithmException("The border file is invalid.");
   }
   if (borderFile->getNumberOfBorders() == 0) {
      throw BrainModelAlgorithmException("The border file contains no borders.");
   }
   if (userMetricFile == NULL) {
      throw BrainModelAlgorithmException("The metric file is invalid.");
   }
   
   //
   // Set up user's metric file
   //
   const int numNodes = bms->getNumberOfNodes();
   int metricColumn = 0;
   if (userMetricFile->getNumberOfNodes() <= 0) {
      userMetricFile->setNumberOfNodesAndColumns(numNodes, 1);
   }
   else {
      userMetricFile->addColumns(1);
      metricColumn = userMetricFile->getNumberOfColumns() - 1;
   }
   userMetricFile->setColumnName(metricColumn, "Nearest Spherical Border Distance");
   
   //
   // Create a topology helper so that only nodes with neighbors are used
   //
   const TopologyHelper* th = bms->getTopologyFile()->getTopologyHelper(false, true, false);
   
   CoordinateFile* cf = bms->getCoordinateFile();

   const float radius = bms->getSphericalSurfaceRadius();
   
   //
   // Loop through all of the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      userMetricFile->setValue(i, metricColumn, 0.0);
      
      //
      // Only do nodes with neighbors
      //
      if (th->getNodeHasNeighbors(i)) {
         // 
         // This node's XYZ
         //
         float nodeXYZ[3];
         cf->getCoordinate(i, nodeXYZ);
         MathUtilities::normalize(nodeXYZ);
         
         //
         // Find the nearest border point
         //
         float nearestGreatCircleDistance = std::numeric_limits<float>::max();
         const int numBorders = borderFile->getNumberOfBorders();
         for (int j = 0; j < numBorders; j++) {
            const Border* b = borderFile->getBorder(j);
            const int numLinks = b->getNumberOfLinks();
            for (int k = 0; k < numLinks; k++) {
               float linkXYZ[3];
               b->getLinkXYZ(k, linkXYZ);
               MathUtilities::normalize(linkXYZ);
               const float dotProduct = MathUtilities::dotProduct(nodeXYZ, linkXYZ);
               const float theta = std::acos(dotProduct);
               const float greatCircleDistance = theta * radius;
               nearestGreatCircleDistance = std::min(nearestGreatCircleDistance, 
                                                     greatCircleDistance);
            }
         }
         if (nearestGreatCircleDistance <= 0.0) {
            nearestGreatCircleDistance = 0.0001;
         }
         userMetricFile->setValue(i, metricColumn, nearestGreatCircleDistance);
      }
      if ((i % 100) == 0) {
         //std::cout << i << " of " << numNodes << std::endl;
      }
   }

   //
   // Write the user's metric file
   //
   try {
  //    brainSet->writeMetricFile(userMetricFile->getFileName());   
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
}

/*
 * This does geodesic distance.
void 
BrainModelSurfaceDeformationMeasurement::execute() throw (BrainModelAlgorithmException)
{
   if (bms == NULL) {
      throw BrainModelAlgorithmException("The surface is invalid.");
   }
   if (bms->getNumberOfNodes() <= 0) {
      throw BrainModelAlgorithmException("The surface has no nodes.");
   }
   if (bms->getTopologyFile() == NULL) {
      throw BrainModelAlgorithmException("The surface has no topology.");
   }
   if (borderFile == NULL) {
      throw BrainModelAlgorithmException("The border file is invalid.");
   }
   if (borderFile->getNumberOfBorders() == 0) {
      throw BrainModelAlgorithmException("The border file contains no borders.");
   }
   if (userMetricFile == NULL) {
      throw BrainModelAlgorithmException("The metric file is invalid.");
   }
   
   //
   // Load the standard sphere
   //
   QString currentPath = QDir::currentPath();
   std::ostringstream str;
   str << brainSet->getCaretHomeDirectory()
       << "/"
       << "REGISTER.SPHERE"
       << "/"
       << "sphere.v5."
       << sphereNumber
       << ".spec";
   BrainSet sphereBrainSet(str.str().c_str());
   if (sphereBrainSet.getNumberOfBrainModels() <= 0) {
      QString msg("Unable to read standard sphere spec file ");
      msg.append(str.str().c_str());
      throw BrainModelAlgorithmException(msg);
   }
   BrainModelSurface* sphereSurface = sphereBrainSet.getBrainModelSurface(0);
   if (sphereSurface == NULL) {
      QString msg("Unable to read standard sphere from");
      msg.append(str.str().c_str());
      throw BrainModelAlgorithmException(msg);
   }
   QDir::setCurrent(currentPath);
   
   //
   // Get the radius of the border file
   //
   float sphereRadius = 1.0;
   const int numBorders = borderFile->getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      const Border* border = borderFile->getBorder(i);
      const int numLinks = border->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         const float* xyz = border->getLinkXYZ(j);
         sphereRadius = std::sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]);
         break;
      }
   }
   
   //
   // Set the radius of the spherical surface
   //
   sphereSurface->convertToSphereWithRadius(sphereRadius);
   
   //
   // Find the nearest node for each borders point
   //
   std::set<int> nodesNearBordersSet;
   BrainModelSurfacePointLocator sphereNodeLocator(sphereSurface,
                                             true);
   for (int i = 0; i < numBorders; i++) {
      const Border* border = borderFile->getBorder(i);
      const int numLinks = border->getNumberOfLinks();
      for (int j = 0; j < numLinks; j++) {
         const int node = sphereNodeLocator.getNearestPoint(border->getLinkXYZ(j));
         if (node >= 0) {
            nodesNearBordersSet.insert(node);
            if (DebugControl::getDebugOn()) {
               std::cout << border->getName().toAscii().constData()
                         << " link "
                         << j
                         << " is closest to node " 
                         << node
                         << std::endl;
            }
         }
         else {
            std::cout << "Unable to find node nearest to border "
                      << border->getName().toAscii().constData()
                      << "link "
                      << j
                      << std::endl;
         }
      }
   }
   
   //
   // Put the nodes near borders in a array for faster access
   //
   const int numNodesNearBorders = static_cast<int>(nodesNearBordersSet.size());
   if (numNodesNearBorders <= 0) {
      throw BrainModelAlgorithmException("No nodes near borders were found.");
   }
   int* nodesNearBorders = new int[numNodesNearBorders];
   int cnt = 0;
   for (std::set<int>::iterator iter = nodesNearBordersSet.begin();
        iter != nodesNearBordersSet.end(); iter++) {
      nodesNearBorders[cnt] = *iter;
      cnt++;
   }
   
   //
   // Create a topology helper so that only nodes with neighbors are used
   //
   const TopologyHelper* th = sphereSurface->getTopologyFile()->getTopologyHelper(false, true, false);
   
   const int sphereNumNodes = sphereSurface->getNumberOfNodes();
   CoordinateFile* cf = sphereSurface->getCoordinateFile();

   //
   // Create a geodesic distance file with one column
   //
   GeodesicDistanceFile geoDistFile;
   geoDistFile.setNumberOfNodesAndColumns(sphereNumNodes, 1);
   
   //
   // Set up metric file
   //
   MetricFile sphereMetricFile;
   sphereMetricFile.setNumberOfNodesAndColumns(sphereNumNodes, 1);
   const int sphereMetricColumn = 0;
   
   //
   // Loop through all of the nodes
   //
   for (int i = 0; i < sphereNumNodes; i++) {
      sphereMetricFile.setValue(i, sphereMetricColumn, 0.0);
      
      //
      // Only do nodes with neighbors
      //
      if (th->getNodeHasNeighbors(i)) {
         //
         // Determine the geodesic distances for the node
         //
         BrainModelSurfaceGeodesic bmsg(&sphereBrainSet,
                                        sphereSurface,
                                        NULL,
                                        -3,
                                        "",
                                        &geoDistFile,
                                        0,
                                        "node",
                                        i,
                                        NULL);
         try {
            bmsg.execute();
         }
         catch (BrainModelAlgorithmException& e) {
            throw e;
         }
         
         //
         // Find the closest node in the node's nearest to the borders
         //
         float minDistSQ = std::numeric_limits<float>::max();
         const float* myXYZ = cf->getCoordinate(i);
         for (int j = 0; j < numNodesNearBorders; j++) {
            const int node = nodesNearBorders[j];
            const float* nodeXYZ = cf->getCoordinate(node);
            const float dx = myXYZ[0] - nodeXYZ[0];
            const float dy = myXYZ[1] - nodeXYZ[1];
            const float dz = myXYZ[2] - nodeXYZ[2];
            const float distSQ = dx*dx + dy*dy + dz*dz;
            minDistSQ = std::min(minDistSQ,
                                 distSQ);
         }
         const float dist = std::max(std::sqrt(minDistSQ), 0.001);
         sphereMetricFile.setValue(i, sphereMetricColumn, dist);
      }
      if ((i % 100) == 0) {
         std::cout << i << " of " << sphereNumNodes << std::endl;
      }
   }
   
   delete[] nodesNearBorders;

   //
   // Set up user's metric file
   //
   const int numNodes = bms->getNumberOfNodes();
   int metricColumn = 0;
   if (userMetricFile->getNumberOfNodes() <= 0) {
      userMetricFile->setNumberOfNodesAndColumns(numNodes, 1);
   }
   else {
      userMetricFile->addColumns(1);
      metricColumn = userMetricFile->getNumberOfColumns() - 1;
   }
   userMetricFile->setColumnName(metricColumn, "Deform Distance");
   
   //
   // Set distances for the users sphere
   //
   const CoordinateFile* userCoordFile = bms->getCoordinateFile();
   for (int i = 0; i < numNodes; i++) {
      const int sphereNode = sphereNodeLocator.getNearestPoint(userCoordFile->getCoordinate(i));
      userMetricFile->setValue(i, metricColumn, 
                               sphereMetricFile.getValue(sphereNode, sphereMetricColumn));
   }

   //
   // Write the user's metric file
   //
   try {
      brainSet->writeMetricFile(userMetricFile->getFileName());   
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
}
*/
