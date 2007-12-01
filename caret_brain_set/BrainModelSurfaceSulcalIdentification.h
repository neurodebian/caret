
#ifndef __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_H__
#define __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_H__

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
class BrainModelSurfaceSulcalIdentification : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceSulcalIdentification(BrainSet* bs,
                                            BrainModelSurface* fiducialSurfaceIn,
                                            PaintFile* inputPaintFileIn,
                                            const int paintFileGeographyColumnNumberIn,
                                            SurfaceShapeFile* surfaceShapeFileIn,
                                            const int surfaceShapeFileDepthColumnNumberIn,
                                            const float clusterNodeCountRatioIn);
      
      // destructor
      ~BrainModelSurfaceSulcalIdentification();
      
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
                                const QString& volumeNameIn) {
               sulcusName = sulcusNameIn;
               volumeName = volumeNameIn;
            }
            
            /// get name of sulcus
            QString getSulcusName() const { return sulcusName; }
            
            /// get name of volume
            QString getVolumeName() const { return volumeName; }
         
         protected:
            /// name of sulcus
            QString sulcusName;
            
            /// name of volume
            QString volumeName;
      };
      
      /// add casename to name
      QString addCaseNameToName(const QString& name) const;
      
      /// map the probabilistic functional volumes to the metric file
      void mapProbabilisticFunctionalVolumes() throw (BrainModelAlgorithmException);
      
      /// create the initial sulcal identification
      void createInitialSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException);
      
      /// dilate the sulcal identification
      void dilateSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException);
      
      /// sulcal names and volumes
      std::vector<SulcalNameAndVolume> sulcalNamesAndVolumes;
      
      /// the fiducial surface
      BrainModelSurface* fiducialSurface;
      
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
      
      /// ratio of nodes that must be identified in the cluster (0.0 to 1.0)
      float clusterNodeCountRatio;
};

#endif // __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_H__
