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



#ifndef __VE_BRAIN_SURFACE_MULTIRESOLUTION_MORPHING_H__
#define __VE_BRAIN_SURFACE_MULTIRESOLUTION_MORPHING_H__

#include <QString>
#include <vector>

#include "BrainSetNodeAttribute.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceMorphing.h"
#include "MultiResMorphFile.h"
#include "StatisticsUtilities.h"
#include "SurfaceShapeFile.h"

class BorderProjection;
class BrainModelSurface;
class CoordinateFile;
class TopologyFile;

/// class for measurements take while multiresolution morphing
class MorphingMeasurements {
   public:
      /// constructor
      MorphingMeasurements(const QString& nameIn,
                           const StatisticsUtilities::DescriptiveStatistics& arealDistortionStatisticsIn,
                           const StatisticsUtilities::DescriptiveStatistics& linearDistortionStatisticsIn,
                           const int numberOfNodeCrossoversIn,
                           const int numberOfTileCrossoversIn,
                           const float elapsedTimeIn);
                           
      /// get the measurements
      void get(QString& nameOut,
               StatisticsUtilities::DescriptiveStatistics& arealDistortionStatisticsOut,
               StatisticsUtilities::DescriptiveStatistics& linearDistortionStatisticsOut,
               int& numberOfNodeCrossoversOut,
               int& numberOfTileCrossoversOut,
               float& elapsedTimeOut) const;
       
   private:
      /// name for this measurement
      QString name;
      
      /// areal distortion measurements
      StatisticsUtilities::DescriptiveStatistics arealDistortionStatistics;
      
      /// linear distortion measurements
      StatisticsUtilities::DescriptiveStatistics linearDistortionStatistics;
      
      /// number of node crossovers
      int numberOfNodeCrossovers;
      
      /// number of tile crossovers
      int numberOfTileCrossovers;
      
      /// time of cycle
      float elapsedTime;
};

/// Class that performs multiresolution morphing on a brain surface
class BrainModelSurfaceMultiresolutionMorphing : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceMultiresolutionMorphing(BrainSet* brainSetIn,
                                          BrainModelSurface* referenceSurfaceIn,
                                          BrainModelSurface* morphingSurfaceIn,
                  const BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingSurfaceTypeIn,
                  const BorderProjection* centralSulcusBorderProjectionIn = NULL);
      
      /// Destructor
      virtual ~BrainModelSurfaceMultiresolutionMorphing();
      
      /// run the multiresolution morphing
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// copy the parameters to this morphing object
      void copyParameters(const BrainModelSurfaceMultiresolutionMorphing& bmsm);

      BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE 
         getMorphingSurfaceType() const { return morphingSurfaceType; }

      /// get the surface measurements taken while morphing
      void getMorphingMeasurements(std::vector<MorphingMeasurements>& mm) const
                                                  { mm = measurements; }

      /// get automatically save all created files
      bool getAutoSaveAllFiles() const { return autoSaveFilesFlag; }
      
      /// set automatically save all created files
      void setAutoSaveAllFiles(const bool b) { autoSaveFilesFlag = b; }
      
      /// get multi-res morph file that contains parameters for editing
      MultiResMorphFile* getMultiResMorphParametersFile() {
          return &this->multiResMorphFile;
      }

      /// get multi-res morph file that contains parameters for reading
      const MultiResMorphFile* getMultiResMorphParametersFile() const {
          return &this->multiResMorphFile;
      }

   protected:
      enum {
         SURFACE_FIDUCIAL_INDEX  = 0,
         SURFACE_MORPHED_INDEX   = 1,
         SURFACE_NOT_MORPHED_INDEX = 2
      };
      
      ///
      void constructTemplateSpheres(std::vector<BrainSet*>& brains)
                                        throw (BrainModelAlgorithmException);
      
      /// create the equilateral grid topology file
      TopologyFile* createEquilateralGridTopology(BrainSet* brain, BrainModelSurface* surface);

      void createSphereDownsampleMapping(std::vector<BrainSet*>& brains)
                                  throw (BrainModelAlgorithmException);
                                  
      /// downsample an equilateral grid surface
      BrainSet* downsampleEquilateralGridSurface(BrainSet* brainIn);
      
      /// measure surface distortions
      void measureSurface(const int cycleNumber, const float elapsedTime);

      /// create downsampled surface(s)
      void multiresolutionDownsample(std::vector<BrainSet*>& brains);
      
      /// perform the multi-resolution morphing
      void multiresolutionMorph(std::vector<BrainSet*>& brains);

      /// upsample fro a lower resolution to a higher resolution brain (spherical surface)
      void sphericalUpsample(std::vector<BrainSet*>& brains,
                             const int targetBrainIndex) 
                                          throw (BrainModelAlgorithmException);
      
      /// write the multi-resolution surfaces
      void writeMultiresolutionSurfaces(std::vector<BrainSet*>& brains);    

      /// upsample from a lower resolution to higher resolution brain (float surface)
      void flatUpsample(BrainSet* fromBrain,
                        BrainSet* toBrain, 
                        BrainModelSurface* toSurface,
                        const bool backsampleFlag);
      
      /// smooth to eliminate crossovers
      void smoothOutCrossovers(BrainModelSurface* bms, const float sphereRadius);
      
      /// setup file naming prefix and suffix
      void setUpOutputFileNaming();  
      
      /// the reference surface
      BrainModelSurface* referenceSurface;
      
      /// the morphing surface
      BrainModelSurface* morphingSurface;
      
      /// type of surface being morphed
      const BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingSurfaceType;
      
      /// central sulcus border projection for alignment
      const BorderProjection* centralSulcusBorderProjection;
      
      /// surface shape file used for distortion measurements
      SurfaceShapeFile shapeMeasurementsFile;

      /// intermediate files
      std::vector<QString> intermediateFiles;
      
      /// prefix of intermediate files names
      QString intermediateFileNamePrefix[MultiResolutionMorphingCycle::MAXIMUM_NUMBER_OF_LEVELS];
      
      /// prefix of intermediate coord files being morphed
      QString intermediateCoordFileNamePrefix[MultiResolutionMorphingCycle::MAXIMUM_NUMBER_OF_LEVELS];
      
      /// name of intermediate spec files
      QString intermediateSpecFileNames[MultiResolutionMorphingCycle::MAXIMUM_NUMBER_OF_LEVELS];
      
      /// name of original morphing surface coord file
      QString origCoordFileName;
      
      /// measurements take while morphing
      std::vector<MorphingMeasurements> measurements;
      
      /// output filename's prefix
      QString outputFileNamePrefix;
      
      /// output filename's suffix
      QString outputFileNameSuffix;
      
      /// the hemisphere being morphed
      Structure::STRUCTURE_TYPE brainStruct;
      
      /// surface type for crossover checks
      BrainModelSurface::SURFACE_TYPES brainModelSurfaceType;
      
      /// cycle currently being iterated
      int currentCycle;
      
      /// crossover smoothing strength
      float crossoverSmoothStrength;
      
      /// crossover smoothing cycles
      int crossoverSmoothCycles;
      
      /// crossover smoothing iterations per cycle
      int crossoverSmoothIterations;
      
      /// crossover smoothing edges every X iterations
      int crossoverSmoothEdgeIterations;
      
      /// crossover smoothing project to sphere every X iterations
      int crossoverSmoothProjectToSphereIterations;
      
      /// crossover smoothing neighbor depth
      int crossoverSmoothNeighborDepth;
      
      /// auto save all created files
      bool autoSaveFilesFlag;

      /// multi-res morph file contains parameters
      MultiResMorphFile multiResMorphFile;
};


#endif // __VE_BRAIN_SURFACE_MULTIRESOLUTION_MORPHING_H__


