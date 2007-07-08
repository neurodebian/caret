
#ifndef __BRAIN_MODEL_SURFACE_METRIC_COORDINATE_DIFFERENCE_H__
#define __BRAIN_MODEL_SURFACE_METRIC_COORDINATE_DIFFERENCE_H__

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

#include <QString>
#include <vector>

#include "BrainModelSurfaceMetricFindClustersBase.h"

class BrainModelSurface;
class CoordinateFile;
class MetricFile;
class QTextStream;

/// find significant clusters in metric files that contain coordinate differences
class BrainModelSurfaceMetricCoordinateDifference : public BrainModelSurfaceMetricFindClustersBase {
   public:
      /// the mode of the algorithm
      enum MODE {
         /// coordinate difference mode
         MODE_COORDINATE_DIFFERENCE,
         /// t-map difference mode
         MODE_TMAP_DIFFERENCE
      };
      
      // constructor
      BrainModelSurfaceMetricCoordinateDifference(BrainSet* bs,
                                                const MODE modeIn,
                                                const std::vector<QString>& coordFileGroupAIn,
                                                const std::vector<QString>& coordFileGroupBIn,
                                                const QString& fiducialCoordFileNameIn,
                                                const QString& openTopoFileNameIn,
                                                const QString& distortionShapeFileNameIn,
                                                const QString& distanceMetricFileNameIn,
                                                const QString& shuffledDistanceMetricFileNameIn,
                                                const QString& clustersPaintFileNameIn,
                                                const QString& clustersMetricFileNameIn,
                                                const QString& reportFileNameIn,
                                                const int distortionShapeFileColumnIn,
                                                const int iterationsIn,
                                                const float thresholdIn,
                                                const float pValueIn,
                                                const int numberOfThreadsIn);
      
      // destructor
      ~BrainModelSurfaceMetricCoordinateDifference();
      
   protected:
      /// must be implemented by subclasses
      void executeClusterSearch() throw (BrainModelAlgorithmException);
      
      // free memory
      void cleanUp();
      
      /// compute coordinate deviation
      void computeCoordinateDeviation(const std::vector<CoordinateFile*>& coordFiles,
                                      const CoordinateFile& averageCoordFile,
                                      MetricFile& deviationFile);
                                      
      /// mode of the algorithm
      MODE mode;
      
      /// names of coordinate files for group A
      std::vector<QString> coordFileNameGroupA;
      
      /// names of coordinate files for group B
      std::vector<QString> coordFileNameGroupB;
      
      /// group A coordinate files
      std::vector<CoordinateFile*> coordGroupA;
      
      /// group B coordinate files
      std::vector<CoordinateFile*> coordGroupB;
      
      /// iterations for shuffling average coord files
      int iterations;
      
      /// name of distance metric file
      //QString& distanceMetricFileName;
      
      /// name of shuffled distance metric file
      //QString& shuffledDistanceMetricFileName;
      
};

#endif // __BRAIN_MODEL_SURFACE_METRIC_COORDINATE_DIFFERENCE_H__

