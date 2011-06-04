
#ifndef __BRAIN_MODEL_CIFTI_CORRELATION_MATRIX_H__
#define __BRAIN_MODEL_CIFTI_CORRELATION_MATRIX_H__

#include <cstdio>

#include "BrainModelAlgorithm.h"
#include "CiftiFile.h"

/**
 * Compute correlation of each metric column with all other metric columns.
 */
class BrainModelCiftiCorrelationMatrix : public BrainModelAlgorithm {
   public:
      // constructor for files that have already been read
      BrainModelCiftiCorrelationMatrix(
           BrainSet* bs,
           CiftiFile * inputCiftiFile,
           const bool applyFisherZTransformFlag,
           const bool parallelFlag);
      
      // destructor
      ~BrainModelCiftiCorrelationMatrix();
      
      // execute the algorithm                                                                                                   
      void execute() throw (BrainModelAlgorithmException);                                                
         
      // get the output metric file (if called caller is reponsible for DELETING
      // returned metric file.
      CiftiFile* getOutputCiftiFile();

   private:
      // initialize variables
      void initialize();
      
      // load the data values
      void loadDataValues();

      // create output metric file
      void createOutputCiftiFile();
      
      // compute the means
      void computeMeans();
      
      // compute the sum-squared
      void computeSumSquared();
      
      // compute the correlations
      void computeCorrelations();
      
      // compute correlations for rows until there are no more rows to process
      void computeCorrelationsForRows();

      QString m_inputCiftiFileName;

      CiftiFile* m_inputCiftiFile;
      
      QString m_outputCiftiFileName;

      CiftiFile* m_outputCiftiFile;
      
      float** m_outputDataArrayColumns;
      
      long m_inputNumRows;
      
      long m_inputNumColumns;
      
      long m_outputDimension;
      
      // All values in one-dim array
      float* m_dataValues;
      
      // mean of all column values for each row
      float* m_rowMeans;
      
      // sum-squared of all column values for each row
      double* m_rowSumSquared;
      
      const bool m_applyFisherZTransformFlag;
      
      bool m_deleteOutputCiftiFlag;
      
      long m_nextRowToProcess;
      
      const bool m_parallelFlag;
      
};

#endif //  __BRAIN_MODEL_SURFACE_METRIC_CORRELATION_MATRIX_H__

