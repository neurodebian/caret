
#ifndef __BRAIN_MODEL_SURFACE_FLATTEN_PARTIAL_HEMISPHERE_H__
#define __BRAIN_MODEL_SURFACE_FLATTEN_PARTIAL_HEMISPHERE_H__

#include "BrainModelAlgorithm.h"

class BrainModelSurface;

/// This class flattens a partial hemisphere
class BrainModelSurfaceFlattenPartialHemisphere : public BrainModelAlgorithm {
   public:
      /// type of surface being flattened
      enum FLATTEN_TYPE {
         FLATTEN_TYPE_ELLIPSOID,
         FLATTEN_TYPE_FIDUCIAL
      };
      
      /// Constructor
      BrainModelSurfaceFlattenPartialHemisphere(BrainModelSurface* fiducialSurfaceIn,
                                                BrainModelSurface* partialHemisphereSurfaceIn,
                                                const FLATTEN_TYPE flattenSurfaceTypeIn);

      /// Destructor
      ~BrainModelSurfaceFlattenPartialHemisphere();
      
      /// Execute the flattening
      virtual void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// fiducial surface
      BrainModelSurface* fiducialSurface;
      
      /// partial hemisphere surface being flattened
      BrainModelSurface* partialHemisphereSurface;
      
      /// type of surface being flattened
      FLATTEN_TYPE flattenSurfaceType;
};

#endif // __BRAIN_MODEL_SURFACE_FLATTEN_PARTIAL_HEMISPHERE_H__

