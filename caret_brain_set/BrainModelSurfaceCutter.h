
#ifndef __BRAIN_MODEL_SURFACE_CUTTER_H__
#define __BRAIN_MODEL_SURFACE_CUTTER_H__

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class BorderFile;

/// This class applies cuts to a surface
class BrainModelSurfaceCutter : public BrainModelAlgorithm {
   public:
      /// mode of cutting
      enum CUTTING_MODE {
         CUTTING_MODE_NORMAL,
         CUTTING_MODE_NON_NEGATIVE_Z_ONLY
      };
      
      /// Constructor
      BrainModelSurfaceCutter(BrainModelSurface* cuttingSurfaceIn,
                              BorderFile* cutsFileIn,
                              const CUTTING_MODE cuttingModeIn);
      
      /// Destructor
      ~BrainModelSurfaceCutter();
      
      /// Execute the flattening
      virtual void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// surface to which cuts are applied
      BrainModelSurface* cuttingSurface;
      
      /// the cuts file
      BorderFile* theCuts;
      
      /// cutting mode
      CUTTING_MODE cuttingMode;
};

#endif // __BRAIN_MODEL_SURFACE_CUTTER_H__

