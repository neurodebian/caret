
#ifndef __BRAIN_MODEL_SURFACE_FLAT_HEXAGONAL_SUBSAMPLE_H__
#define __BRAIN_MODEL_SURFACE_FLAT_HEXAGONAL_SUBSAMPLE_H__

#include "BrainModelAlgorithm.h"

class BrainModelSurface;

/// Class that creates a hexagonal subsample of the flat surface
class BrainModelSurfaceFlatHexagonalSubsample : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceFlatHexagonalSubsample(BrainSet* brainSetIn,
                                              BrainModelSurface* fiducialSurfaceIn,
                                              BrainModelSurface* flatSurfaceIn,
                                              const int numberOfResamplingTiles = -1);
                                              
      /// Destructor
      ~BrainModelSurfaceFlatHexagonalSubsample();
      
      /// create the sub sampled surface
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the subsampled brain set
      BrainSet* getSubsampledBrainSet();
      
   protected:
      /// fiducial of surface that is to be resampled
      BrainModelSurface* fiducialSurfaceForResampling;
      
      /// flat surface that is to be resampled
      BrainModelSurface* flatSurfaceForResampling;
      
      /// the subsampled brain set
      BrainSet* subSampBrainSet;
      
      /// number of resampling tiles goal
      int numberOfResamplingTiles;
      
      /// user retrieved the create brain set so destructor does not need to delete it
      bool brainSetRetrieved;
};

#endif //  __BRAIN_MODEL_SURFACE_FLAT_HEXAGONAL_SUBSAMPLE_H__

