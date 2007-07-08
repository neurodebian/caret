
#ifndef __BRAIN_MODEL_SURFACE_SHAPE_INTER_HEM_CLUSTERS_H__
#define __BRAIN_MODEL_SURFACE_SHAPE_INTER_HEM_CLUSTERS_H__

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
class MetricFile;
class QTextStream;

/// find significant clusters between hemisphere surface shape files
class BrainModelSurfaceMetricInterHemClusters : public BrainModelSurfaceMetricFindClustersBase {
   public:
      // constructor
      BrainModelSurfaceMetricInterHemClusters(BrainSet* bs,
                                             const QString& shapeFileRightANameIn,
                                             const QString& shapeFileRightBNameIn,
                                             const QString& shapeFileLeftANameIn,
                                             const QString& shapeFileLeftBNameIn,
                                             const QString& fiducialCoordFileNameIn,
                                             const QString& openTopoFileNameIn,
                                             const QString& areaCorrectionShapeFileNameIn,
                                             const QString& rightTMapFileNameIn,
                                             const QString& leftTMapFileNameIn,
                                             const QString& rightShuffledTMapFileNameIn,
                                             const QString& leftShuffledTMapFileNameIn,
                                             const QString& tMapFileNameIn,
                                             const QString& shuffledTMapFileNameIn,
                                             const QString& clustersPaintFileNameIn,
                                             const QString& clustersMetricFileNameIn,
                                             const QString& reportFileNameIn,
                                             const int areaCorrectionShapeFileColumnIn,
                                             const int iterationsShuffledTMapIn,
                                             const int iterationsRightLeftShuffledTMapIn,
                                             const float negativeThreshIn,
                                             const float positiveThreshIn,
                                             const float pValueIn,
                                             const int tVarianceSmoothingIterationsIn,
                                             const float tVarianceSmoothingStrengthIn,
                                             const bool doTMapDOFIn,
                                             const bool doTMapPValueIn,
                                             const int numberOfThreadsIn);
      
      // destructor
      ~BrainModelSurfaceMetricInterHemClusters();
      
   protected:
      /// must be implemented by subclasses
      void executeClusterSearch() throw (BrainModelAlgorithmException);
      
      // free memory
      void cleanUp();
      
      /// name of shape file Right A
      QString shapeFileRightAName;
   
      /// name of shape file Right B
      QString shapeFileRightBName;
   
      /// name of shape file Left A
      QString shapeFileLeftAName;
   
      /// name of shape file Left B
      QString shapeFileLeftBName;
   
      /// name of right hem tmap file
      QString rightTMapFileName;
      
      /// name of left hem tmap file
      QString leftTMapFileName;
      
      /// name of right hem shuffled tmap file
      QString rightShuffledTMapFileName;
      
      /// name of left hem shuffled tmap file
      QString leftShuffledTMapFileName;
      
      /// iterations for shuffled T-Map
      int iterationsShuffledTMap;  
      
      /// iterations for shuffled right/left T-Maps
      int iterationsRightLeftShuffledTMap;
      
      /// left T-Map shape file
      MetricFile* leftTMapShapeFile;
      
      /// right T-Map shape file
      MetricFile* rightTMapShapeFile;

      /// left shuffled T-Map shape file
      MetricFile* leftShuffledTMapShapeFile;
      
      /// right shuffled T-Map shape file
      MetricFile* rightShuffledTMapShapeFile;

};

#endif // __BRAIN_MODEL_SURFACE_SHAPE_INTER_HEM_CLUSTERS_H__

