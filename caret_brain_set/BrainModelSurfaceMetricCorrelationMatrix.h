
#ifndef __BRAIN_MODEL_SURFACE_METRIC_CORRELATION_MATRIX_H__
#define __BRAIN_MODEL_SURFACE_METRIC_CORRELATION_MATRIX_H__

#include <cstdio>

#include "BrainModelAlgorithm.h"
#include "GiftiDataArrayReadListener.h"

class GiftiDataArray;
class GiftiDataArrayFile;
class MetricFile;

/**
 * Compute correlation of each metric column with all other metric columns.
 */
class BrainModelSurfaceMetricCorrelationMatrix : public BrainModelAlgorithm,
                                                        GiftiDataArrayReadListener {
   public:
      // constructor for files that have already been read
      BrainModelSurfaceMetricCorrelationMatrix(
           BrainSet* bs,
           MetricFile* inputMetricFileIn,
           const bool applyFisherZTransformFlagIn,
           const bool outputGiftiFlagIn,
           const bool parallelFlagIn);
      
      // create instance for processing that reads and writes files incrementally
      // in order to minimize memory usage
      BrainModelSurfaceMetricCorrelationMatrix(
                  const QString& inputMetricFileName,
                  const QString& outputMetricFileName,
                  const bool applyFisherZTransformFlagIn,
                  const bool outputGiftiFlagIn,
                  const bool parallelFlagIn);

      // destructor
      ~BrainModelSurfaceMetricCorrelationMatrix();
      
      // execute the algorithm                                                                                                   
      void execute() throw (BrainModelAlgorithmException);                                                
         
      // get the output metric file (if called caller is reponsible for DELETING
      // returned metric file.
      MetricFile* getOutputMetricFile();
      
      // get the output gifti file (if called caller is reponsible for DELETING
      // returned gifti file.
      GiftiDataArrayFile* getOutputGiftiFile();
      
      /// called for incremental reads
      QString dataArrayWasRead(GiftiDataArray* gda,
                            const int dataArrayIndex,
                            const int numberOfDataArrays);

   private:
      // constructor for i
      enum Mode {
         MODE_FILES_IN_MEMORY,
         MODE_METRIC_INCREMENTAL
      };

      // initialize variables
      void initialize();
      
      // load the data values
      void loadDataValues();
      
      // load the data values by incrementally reading the input metric file
      void loadDataValuesMetricIncremental() throw (BrainModelAlgorithmException);

      // create output metric file
      void createOutputMetricFile();
      
      // create output gifti file
      void createOutputGiftiFile();
      
      // compute the means
      void computeMeans();
      
      // compute the sum-squared
      void computeSumSquared();
      
      // compute the correlations
      void computeCorrelations(const Mode currentMode);
      
      // compute correlations for rows until there are no more rows to process
      void computeCorrelationsForRows();
      
      // compute correlations for rows until there are no more rows to process
      void computeCorrelationsForRowsMetricIncremental(FILE* file);

      const Mode mode;

      QString inputMetricFileName;

      MetricFile* inputMetricFile;
      
      GiftiDataArrayFile* outputGiftiFile;
      
      QString outputMetricFileName;

      MetricFile* outputMetricFile;
      
      float** outputDataArrayColumns;
      
      long inputNumRows;
      
      long inputNumColumns;
      
      long outputDimension;
      
      // All values in one-dim array
      float* dataValues;
      
      // mean of all column values for each row
      float* rowMeans;
      
      // sum-squared of all column values for each row
      double* rowSumSquared;
      
      const bool applyFisherZTransformFlag;
      
      bool deleteOutputMetricFlag;
      
      bool deleteOutputGiftiFlag;
      
      long nextRowToProcess;
      
      const bool outputGiftiFlag;

      const bool parallelFlag;
      
};

#endif //  __BRAIN_MODEL_SURFACE_METRIC_CORRELATION_MATRIX_H__

