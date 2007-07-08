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



#ifndef __VE_BRAIN_MODEL_SURFACE_POINT_LOCATOR_H__
#define __VE_BRAIN_MODEL_SURFACE_POINT_LOCATOR_H__

#include <vector>

class BrainModelSurface;
class CoordinateFile;
class vtkPointLocator;
class vtkPoints;
class vtkPolyData;

/// This class is used to quickly find the nearest point (node) in a BrainModelSurface.  It
/// should be used when multiple queries will be made.
class BrainModelSurfacePointLocator {
   public:
      /// Constructor
      BrainModelSurfacePointLocator(const BrainModelSurface* bmsin,
                                    const bool limitToConnectedNodes,
                                    const bool nodesMayBeAddedToSurfaceIn = false,
                                    const std::vector<bool>* limitToTheseNodes = NULL);
      
      /// Destructor
      ~BrainModelSurfacePointLocator();
      
      /// find point nearest to location (returns negative BrainModelSurface is empty)
      int getNearestPoint(const float xyz[3]);
      
      /// find points within the specified radius of the location
      void getPointsWithinRadius(const float xyz[3],
                                 const float radius,
                                 std::vector<int>& nearbyPointsOut); 
                                 
   private:
      /// the point locator
      vtkPointLocator* locator;
      
      /// the points
      vtkPoints* points;
      
      /// data set for the points
      vtkPolyData* polyData;
      
      /// If we are limited to connected nodes, only connected nodes will be placed into the
      /// point locator.  "pointIndexToNodeIndex" keeps track of this relationship.
      std::vector<int> pointIndexToNodeIndex;

      /// nodes may be added to the surface after this object is constructed
      bool nodesMayBeAddedToSurface;
      
      /// number of nodes in object passed to constructor
      int originalNumberOfNodes;
      
      /// the surface's coordinate file (DO NOT DELETE IN DESTRUCTOR)
      const CoordinateFile* coordFile;
};

#endif // __VE_BRAIN_MODEL_SURFACE_POINT_LOCATOR_H__

