
#ifndef __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_ALL_H__
#define __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_ALL_H__

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

#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class MetricFile;
class PaintFile;
class SurfaceShapeFile;

/// class for identifying sulci
class BrainModelSurfaceSulcalIdentificationAll : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceSulcalIdentificationAll(BrainSet* bs,
                                            BrainModelSurface* fiducialSurfaceIn,
                                            BrainModelSurface* veryInflatedSurfaceIn,
                                            PaintFile* inputPaintFileIn,
                                            const int paintFileGeographyColumnNumberIn,
                                            SurfaceShapeFile* surfaceShapeFileIn,
                                            const int surfaceShapeFileDepthColumnNumberIn,
                                            const QString& probabilisticDepthVolumeCSVFileNameIn,
                                            const float postCentralSulcusOffsetIn,
                                            const float postCentralSulcusStdDevSquareIn,
                                            const float postCentralSulcusSplitIn);
      
      // destructor
      ~BrainModelSurfaceSulcalIdentificationAll();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      // get the output paint file (will be delete'd by this algorithm, so copy if needed)
      PaintFile* getOutputPaintFile() { return outputPaintFile; }
      
   protected:
      /// class for sulcal names and matching volume file
      class SulcalNameAndVolume {
         public:
            /// constructor
            SulcalNameAndVolume(const QString& sulcusNameIn,
                                const float& depthThresholdIn,
                                const QString& volumeNameIn,
                                const int maximumClustersIn) {
               sulcusName = sulcusNameIn;
               depthThreshold = depthThresholdIn;
               volumeName = volumeNameIn;
               maximumClusters = maximumClustersIn;
            }
            
            /// get name of sulcus
            QString getSulcusName() const { return sulcusName; }
            
            /// get the depth threshold
            float getDepthThreshold() const { return depthThreshold; }
            
            /// get name of volume
            QString getVolumeName() const { return volumeName; }
         
            /// get the maximum number of clusters
            int getMaximumClusters() const { return maximumClusters; }
            
         protected:
            /// name of sulcus
            QString sulcusName;
            
            /// depth threshold 
            float depthThreshold;
            
            /// name of volume
            QString volumeName;
            
            /// maximum clusters
            int maximumClusters;
      };
      
      /// add casename to name
      QString addCaseNameToName(const QString& name) const;
      
      /// map the probabilistic functional volumes to the metric file
      void mapProbabilisticFunctionalVolumes() throw (BrainModelAlgorithmException);
      
      /// create the initial sulcal identification
      void createInitialSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException);
      
      /// dilate the sulcal identification
      void dilateSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException);
      
      /// read the probabilistic volume file list
      void readProbabilisticVolumeFileList() throw (BrainModelAlgorithmException);
      
      /// multiply probabilistic volumes by depth
      void multiplyProbabilisticFunctionalDataByDepth() throw (BrainModelAlgorithmException);
      
      /// rotate the very inflated surface
      void rotateVeryInflatedSurface() throw (BrainModelAlgorithmException);
      
      /// sulcal names and volumes
      std::vector<SulcalNameAndVolume> sulcalNamesAndVolumes;
      
      /// the fiducial surface
      BrainModelSurface* fiducialSurface;
      
      /// the very inflated surface
      BrainModelSurface* veryInflatedSurface;
      
      /// the input paint file
      PaintFile* inputPaintFile;
      
      /// the output paint file
      PaintFile* outputPaintFile;
      
      /// the metric file for mapping functional probabilistic volumes
      MetricFile* probabilisticMetricFile;
      
      /// the paint file geography column number
      int paintFileGeographyColumnNumber;

      /// the surface shape file
      SurfaceShapeFile* surfaceShapeFile;
      
      /// the surface shape file depth column number
      int surfaceShapeFileDepthColumnNumber;
      
      /// name of file listing sulcal names, depth thresholds, and volume file names
      QString probabilisticDepthVolumeCSVFileName;

      /// post central sulcus offset from central sulcus
      float postCentralSulcusOffset;
      
      /// post central sulcus offset from central sulcus std dev squared
      float postCentralSulcusStdDevSquare;
      
      /// post central sulcus split between two largest post central sulci
      float postCentralSulcusSplit;
};

#endif // __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_ALL_H__
