
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

#ifndef __GUI_BRAIN_MODEL_SURFACE_SULCAL_DEPTH_WITH_NORMALS_H__
#define __GUI_BRAIN_MODEL_SURFACE_SULCAL_DEPTH_WITH_NORMALS_H__

#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class CoordinateFile;
class SurfaceShapeFile;
class vtkPolyData;

/// Class to generate a sulcal depth map for a surface
class BrainModelSurfaceSulcalDepthWithNormals : public BrainModelAlgorithm {
   public:
      enum DEPTH_COLUMNS {
         DEPTH_COLUMN_CREATE_NEW = -1,
         DEPTH_COLUMN_DO_NOT_GENERATE = -2
      };
      
      /// Constructor
      BrainModelSurfaceSulcalDepthWithNormals(BrainSet* bs,
                                   BrainModelSurface* surfaceIn,
                                   const QString& hullVtkFilenNameIn,
                                   SurfaceShapeFile* shapeFileIn,
                                   const int hullSmoothingIterationsIn,
                                   const int depthSmoothingIterationsIn,
                                   const int depthColumnIn,
                                   const int smoothedDepthColumnIn,
                                   const QString& depthNameIn,
                                   const QString& smoothedDepthNameIn,
                                   CoordinateFile* outputHullCoordFileIn);
      
      /// Destructor
      ~BrainModelSurfaceSulcalDepthWithNormals();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
      
   private:
      // normal check for node (surface/hull normals oriented in similar direction)
      bool normalCheck(const int surfaceNode, const int hullNode);
      
      // determine greatest neighbor distance from each node
      void determineGreatestNeighborDistance(const BrainModelSurface* bms,
                                             std::vector<float>& greatestDistance) const;
      
      /// surface for depth computation
      BrainModelSurface* surface;
      
      /// inflated surface for depth computation
      BrainModelSurface* inflatedSurface;
      
      /// inflated hull surface
      BrainModelSurface* inflatedHullSurface;
      
      /// less inflated hull surface
      BrainModelSurface* lowlySmoothedHullSurface;
      
      /// the hull vtk file name
      QString hullVtkFileName;
      
      /// surface shape file for storage of depth
      SurfaceShapeFile* shapeFile;
      
      /// column for depth
      int depthColumn;
      
      /// column for smoothed depth
      int smoothedDepthColumn;
      
      /// name for depth column
      QString depthName;
      
      /// name for smoothed depth column
      QString smoothedDepthName;
      
      /// hull smoothing interations
      int hullSmoothingIterations;
      
      /// depth smoothing interations
      int depthSmoothingIterations;
      
      /// hull coord file with same number of nodes as input surface
      CoordinateFile* outputHullCoordFile;
};

#endif // __GUI_BRAIN_MODEL_SURFACE_SULCAL_DEPTH_WITH_NORMALS_H__

