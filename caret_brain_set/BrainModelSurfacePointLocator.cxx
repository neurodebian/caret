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



#include "BrainModelSurface.h"
#include "BrainModelSurfacePointLocator.h"
#include "TopologyFile.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkPointLocator.h"

/**
 * Constructor.
 *
 * If "limitToConnectedNodes" is set, only those nodes who are used in the
 * BrainModelSurface's tiles will be searched.
 *
 * If "limitToTheseNodes" is not NULL and has the same number of elements as
 * there are nodes in the BrainModelSurface, nodes with limitToTheseNodes[]
 * equal to false will not be searched.
 *
 * "nodesMayBeAddedToSurfaceIn" should be set if nodes will be added to the 
 * surface after this object is constructed.  
 */
BrainModelSurfacePointLocator::BrainModelSurfacePointLocator(const BrainModelSurface* bms,
                                                             const bool limitToConnectedNodes,
                                                       const bool nodesMayBeAddedToSurfaceIn,
                                                       const std::vector<bool>* limitToTheseNodes)
   : coordFile(bms->getCoordinateFile())
{
   locator  = NULL;
   points   = NULL;
   polyData = NULL;
   
   //const CoordinateFile* coordFile = bms->getCoordinateFile();
   const int numPoints = coordFile->getNumberOfCoordinates();
   if (numPoints <= 0) {
      return;
   }
   
   //
   // Find out if surface will have nodes added to it after this object is constructed
   //
   nodesMayBeAddedToSurface = nodesMayBeAddedToSurfaceIn;
   originalNumberOfNodes   = numPoints;
   
   //
   // If necessary, keep track of which nodes are connected
   //
   std::vector<bool> useThisNode(numPoints, false);
   if (limitToConnectedNodes) {
      const TopologyFile* topoFile = bms->getTopologyFile();
      const int numTiles = topoFile->getNumberOfTiles();
      for (int i = 0; i < numTiles; i++) {
         int v1, v2, v3;
         topoFile->getTile(i, v1, v2, v3);
         useThisNode[v1] = true;
         useThisNode[v2] = true;
         useThisNode[v3] = true;
      }
   }
   else {
      std::fill(useThisNode.begin(), useThisNode.end(), true);
   }

   //
   // if additional node restrictions
   //
   if (limitToTheseNodes != NULL) {
      if (static_cast<int>(limitToTheseNodes->size()) == numPoints) {
         for (int i = 0; i < numPoints; i++) {
            if ((*limitToTheseNodes)[i] == false) {
               useThisNode[i] = false;
            }
         }
      }
   }
   
   //
   // Add the nodes to the point locator
   //   
   int counter = 0;
   points = vtkPoints::New();
   for (int i = 0; i < numPoints; i++) {
      if (useThisNode[i]) {
         float xyz[3];
         coordFile->getCoordinate(i, xyz);
         points->InsertPoint(counter, xyz);
         counter++;
         pointIndexToNodeIndex.push_back(i);
      }
   }
   
   polyData = vtkPolyData::New();
   polyData->SetPoints(points);
   
   locator = vtkPointLocator::New();
   locator->Initialize();
   locator->SetDataSet(polyData);
   locator->BuildLocator();
}

/*
 * Destructor
 */
BrainModelSurfacePointLocator::~BrainModelSurfacePointLocator()
{
   if (locator != NULL)  {
      locator->Delete();
      locator = NULL;
   }
   if (polyData != NULL) {
      polyData->Delete();
      polyData = NULL;
   }
   if (points != NULL) {
      points->Delete();
      points = NULL;
   }
}

/**      
 * find point nearest to location (returns negative BrainModelSurface is empty)
 */
int 
BrainModelSurfacePointLocator::getNearestPoint(const float xyz[3])
{
   int closestNodeIndex = -1;
   if (locator != NULL) {
#ifdef HAVE_VTK5
      double d[3] = { xyz[0], xyz[1], xyz[2] };
      const int num = locator->FindClosestPoint(d);
#else // HAVE_VTK5
      const int num = locator->FindClosestPoint((float*)xyz);
#endif // HAVE_VTK5
      if (num >= 0) {
         closestNodeIndex = pointIndexToNodeIndex[num];
      }
   }
   
   //
   // Is is possible that nodes have been added to the surface
   //
   if (nodesMayBeAddedToSurface) {
      //
      // Have nodes been added to the surface
      //
      if (originalNumberOfNodes < coordFile->getNumberOfCoordinates()) {
         //
         // Use coordinate file to find nearest node in the nodes that have been added
         //
         const int closestNewNodeIndex = 
            coordFile->getCoordinateIndexClosestToPoint(xyz[0], xyz[1], xyz[2], originalNumberOfNodes);
            
         //
         // Figure out which closest node to use
         //
         if (closestNewNodeIndex >= 0) {
            if (closestNodeIndex < 0) {
               closestNodeIndex = closestNewNodeIndex;
            }
            else {
               const float newCoordDist = vtkMath::Distance2BetweenPoints(xyz, 
                                                       coordFile->getCoordinate(closestNewNodeIndex));
               const float oldCoordDist = vtkMath::Distance2BetweenPoints(xyz, 
                                                       coordFile->getCoordinate(closestNodeIndex));
               if (newCoordDist < oldCoordDist) {
                  closestNodeIndex = closestNewNodeIndex;
               }
            }
         }
      }
   }
   
   return closestNodeIndex;
}

/**
 * Find points within a specified radius of the location.
 */
void
BrainModelSurfacePointLocator::getPointsWithinRadius(const float xyz[3],
                                                     const float radius,
                                                     std::vector<int>& nearbyPointsOut)
{
   nearbyPointsOut.clear();
   
   vtkIdList* idList = vtkIdList::New();
   
   //
   // Find nearby points with the vtk locator
   //
#ifdef HAVE_VTK5
   double xyzD[3] = { xyz[0], xyz[1], xyz[2] };
   locator->FindPointsWithinRadius(radius, xyzD, idList);   
#else // HAVE_VTK5
   locator->FindPointsWithinRadius(radius, xyz, idList);   
#endif // HAVE_VTK5
   const int numIDs = idList->GetNumberOfIds();
   if (numIDs > 0) {
      for (int i = 0; i < numIDs; i++) {
         nearbyPointsOut.push_back(pointIndexToNodeIndex[idList->GetId(i)]);
      }
   }
   
   // Is is possible that nodes have been added to the surface
   //
   if (nodesMayBeAddedToSurface) {
      //
      // Have nodes been added to the surface
      //
      const int newNumberOfNodes = coordFile->getNumberOfCoordinates();
      if (originalNumberOfNodes < newNumberOfNodes) {
         const float radiusSquared = radius * radius;
         
         //
         // Examine nodes that have been added to the surface
         //
         for (int i = originalNumberOfNodes; i < newNumberOfNodes; i++) {
            const float* coordXYZ = coordFile->getCoordinate(i);
            const float distSquared = vtkMath::Distance2BetweenPoints(xyz, coordXYZ);
            if (distSquared < radiusSquared) {
               nearbyPointsOut.push_back(i);
            }
         }
      }
   }
   
   if (idList != NULL) idList->Delete();
}



