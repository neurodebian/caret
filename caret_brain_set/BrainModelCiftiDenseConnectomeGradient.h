
#ifndef __BRAIN_MODEL_CIFTI_DENSE_CONNECTOME_GRADIENT_H__
#define __BRAIN_MODEL_CIFTI_DENSE_CONNECTOME_GRADIENT_H__

class BrainModelSurface;
class CiftiFile;

#include "BrainModelAlgorithm.h"

/**
 * Compute gradient of each metric column.
 */
class BrainModelCiftiDenseConnectomeGradient : public BrainModelAlgorithm {
public:
   // constructor for files that have already been read
   BrainModelCiftiDenseConnectomeGradient(
      BrainSet* bs,
      BrainModelSurface* leftSurf,
      BrainModelSurface* rightSurf,
      CiftiFile* inputCiftiFile,
      float surfaceKernel,
      float volumeKernel,
      bool averageNormals,
      bool debug = false);
   
   // execute the algorithm                                                                                                   
   void execute() throw (BrainModelAlgorithmException);
   
private:
   
   BrainModelSurface* m_leftSurf, *m_rightSurf;
   
   CiftiFile* m_inputCiftiFile;
   
   float m_volumeKernel, m_surfaceKernel;
   
   bool m_averageNormals, m_debug;
   
};

#endif //  __BRAIN_MODEL_CIFTI_DENSE_CONNECTOME_GRADIENT_H__

