
#ifndef __BRAIN_MODEL_CIFTI_GRADIENT_H__
#define __BRAIN_MODEL_CIFTI_GRADIENT_H__

class BrainModelSurface;
class CiftiFile;

#include "BrainModelAlgorithm.h"

/**
 * Compute gradient of each metric column.
 */
class BrainModelCiftiGradient : public BrainModelAlgorithm {
   public:
      // constructor for files that have already been read
      BrainModelCiftiGradient(
           BrainSet* bs,
           BrainModelSurface* leftSurf,
           BrainModelSurface* rightSurf,
           CiftiFile* inputCiftiFile,
           float surfaceKernel,
           float volumeKernel,
           bool averageNormals,
           bool individualOutput,
           bool parallelFlag = true);
      
      // execute the algorithm                                                                                                   
      void execute() throw (BrainModelAlgorithmException);
         
   private:
      
      BrainModelSurface* m_leftSurf, *m_rightSurf;
      
      CiftiFile* m_inputCiftiFile;
      
      float* m_dataValues;
      
      float m_volumeKernel;
      
      bool m_parallelFlag, m_averageNormals, m_individualOutput;
      
      float m_surfaceKernel;
      
};

#endif //  __BRAIN_MODEL_CIFTI_GRADIENT_H__

