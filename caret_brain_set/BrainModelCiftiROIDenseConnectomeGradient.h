
#ifndef __BRAIN_MODEL_CIFTI_ROI_DENSE_CONNECTOME_GRADIENT_H__
#define __BRAIN_MODEL_CIFTI_ROI_DENSE_CONNECTOME_GRADIENT_H__

class BrainModelSurface;
class CiftiFile;
class MetricFile;

#include "BrainModelAlgorithm.h"

/**
 * Compute gradient of each metric column.
 */
class BrainModelCiftiROIDenseConnectomeGradient : public BrainModelAlgorithm {
public:
   // constructor for files that have already been read
   BrainModelCiftiROIDenseConnectomeGradient(
      BrainSet* bs,
      BrainModelSurface* leftSurf,
      BrainModelSurface* rightSurf,
      CiftiFile* inputCiftiFile,
      MetricFile* leftROI,
      MetricFile* rightROI,
      float surfaceKernel,
      float volumeKernel,
      bool averageNormals,
      bool debug = false);
   
   // execute the algorithm                                                                                                   
   void execute() throw (BrainModelAlgorithmException);
   
private:
   
   BrainModelSurface* m_leftSurf, *m_rightSurf;
   
   CiftiFile* m_inputCiftiFile;
   
   MetricFile* m_leftROI, *m_rightROI;
   
   float m_volumeKernel, m_surfaceKernel;
   
   bool m_averageNormals, m_debug;
   
};

#endif //  __BRAIN_MODEL_CIFTI_ROI_DENSE_CONNECTOME_GRADIENT_H__

