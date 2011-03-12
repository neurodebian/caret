/*
 *  BrainModelSurfaceMetricCorrelationMatrix.cxx
 *  caret_brain_set
 *
 *  Created by John Harwell on 12/30/10.
 *  Copyright 2010 Washington University School of Medicine. All rights reserved.
 *
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>

#include <QDataStream>
#include <QFile>
#include <QTime>

#include "BrainModelSurfaceMetricCorrelationMatrix.h"
#include "GiftiDataArray.h"
#include "GiftiDataArrayFile.h"
#include "GiftiDataArrayFileStreamReader.h"
#include "MetricFile.h"

#ifdef _OPENMP
#include <omp.h>
#endif

static const bool timingFlag = true;

/**
 * constructor.
 */ 
BrainModelSurfaceMetricCorrelationMatrix::BrainModelSurfaceMetricCorrelationMatrix(
                                                BrainSet* bs,
                                                MetricFile* inputMetricFileIn,
                                                const bool applyFisherZTransformFlagIn,
                                                const bool outputGiftiFlagIn,
                                                const bool parallelFlagIn)
   : BrainModelAlgorithm(bs),
     GiftiDataArrayReadListener(),
     mode(MODE_FILES_IN_MEMORY),
     inputMetricFile(inputMetricFileIn),
     applyFisherZTransformFlag(applyFisherZTransformFlagIn),
     outputGiftiFlag(outputGiftiFlagIn),
     parallelFlag(parallelFlagIn)
{
   this->initialize();
}

// create instance for processing that reads and writes files incrementally
// in order to minimize memory usage
BrainModelSurfaceMetricCorrelationMatrix::BrainModelSurfaceMetricCorrelationMatrix(
            const QString& inputMetricFileName,
            const QString& outputMetricFileName,
            const bool applyFisherZTransformFlagIn,
            const bool outputGiftiFlagIn,
            const bool parallelFlagIn)
    : BrainModelAlgorithm(NULL),
      GiftiDataArrayReadListener(),
      mode(MODE_METRIC_INCREMENTAL),
      inputMetricFile(NULL),
      applyFisherZTransformFlag(applyFisherZTransformFlagIn),
      outputGiftiFlag(outputGiftiFlagIn),
      parallelFlag(parallelFlagIn)
{
   this->initialize();
   this->inputMetricFileName = inputMetricFileName;
   this->outputMetricFileName = outputMetricFileName;
}

/**
 * destructor.
 */ 
BrainModelSurfaceMetricCorrelationMatrix::~BrainModelSurfaceMetricCorrelationMatrix()
{
   if (this->deleteOutputMetricFlag) {
      if (this->outputMetricFile != NULL) {
         delete this->outputMetricFile;
      }
   }
   if (this->deleteOutputGiftiFlag) {
      if (this->outputGiftiFile != NULL) {
         delete this->outputGiftiFile;
      }
   }
   if (this->dataValues != NULL) {
      delete[] this->dataValues;
   }
   if (this->rowMeans != NULL) {
      delete[] this->rowMeans;
   }
   if (this->rowSumSquared != NULL) {
      delete[] this->rowSumSquared;
   }
}

/**
 * initialize variables.
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::initialize()
{
   this->deleteOutputMetricFlag = true;
   this->outputMetricFile = NULL;
   this->deleteOutputGiftiFlag = true;
   this->outputGiftiFile = NULL;
   this->dataValues = NULL;
   this->rowMeans = NULL;
   this->rowSumSquared = NULL;
   this->nextRowToProcess = -1;
}
      
/**
 * execute the algorithm.
 */                                                                                               
void 
BrainModelSurfaceMetricCorrelationMatrix::execute() throw (BrainModelAlgorithmException)
{
   
   QTime loadTimer;
   loadTimer.start();
   switch (this->mode) {
   case MODE_FILES_IN_MEMORY:
      this->inputNumRows = this->inputMetricFile->getNumberOfNodes();
      this->inputNumColumns = this->inputMetricFile->getNumberOfColumns();

      if ((this->inputNumRows <= 0) || (this->inputNumColumns <= 0)) {
         throw BrainModelAlgorithmException(
             "Input metric file is empty: " +
             this->inputMetricFile->getFileName());
      }
      this->loadDataValues();
      break;
   case MODE_METRIC_INCREMENTAL:
      if (this->inputMetricFileName.isEmpty()) {
         throw BrainModelAlgorithmException("Input metric file name is empty.");
      }
      if (this->outputMetricFileName.isEmpty()) {
         throw BrainModelAlgorithmException("Output metric file name is empty.");
      }
      this->loadDataValuesMetricIncremental();
      break;
   }
   if (timingFlag) {
      std::cout << "Loaded data values in "
                << (loadTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }
   
   /*
    * Compute the means
    */
   QTime meanTimer;
   meanTimer.start();
   this->computeMeans();
   if (timingFlag) {
      std::cout << "Computed means in "
                << (meanTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }
    
   /*
    * Compute the sumSquared
    */
   QTime ssTimer;
   ssTimer.start();
   this->computeSumSquared();
   if (timingFlag) {
      std::cout << "Computed sum-squareds in "
                << (ssTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }

   /*
    * Set output dimension output
    */
   this->outputDimension = this->inputNumRows;

   /*
    * Create the output metric file
    */
   QTime createMetricTimer;
   createMetricTimer.start();
   if (this->outputGiftiFlag) {
      this->createOutputGiftiFile();
   }
   else {
      this->createOutputMetricFile();
   }
   if (timingFlag) {
      std::cout << "Create output file in "
                << (createMetricTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }

   /*
    * Comute the correlations
    */
   QTime corrTimer;
   corrTimer.start();
   this->computeCorrelations(MODE_FILES_IN_MEMORY);
   // SEE NOTE  this->computeCorrelations(this->mode);    // Use when CIFTI file is available
   if (timingFlag) {
      std::cout << "Computed correlations in "
                << (corrTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }

   /*
    * Write output file.
    */
   switch (this->mode) {
   case MODE_FILES_IN_MEMORY:
      break;
   case MODE_METRIC_INCREMENTAL:
      try {
         QTime writeTimer;
         writeTimer.start();
         if (this->outputGiftiFlag) {
            AbstractFile::FILE_FORMAT format = AbstractFile::getPreferredMetricWriteTypeCaretCommand();
            this->outputGiftiFile->setFileWriteType(format);
            this->outputGiftiFile->writeFile(this->outputMetricFileName);
         }
         else {
            this->outputMetricFile->writeFile(this->outputMetricFileName);
         }
         if (timingFlag) {
            std::cout << "Write output file in "
                      << (writeTimer.elapsed() * 0.001)
                      << " seconds."
                      << std::endl;
         }
      }
      catch (FileException e) {
         throw BrainModelAlgorithmException(e.whatQString());
      }
      break;
   }
}

/**
 * load the data values.
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::loadDataValues() 
{
   /*
    * Create and load the data values.
    */
   const long numRows = this->inputNumRows;
   const long numCols = this->inputNumColumns;
   const long numElements = numRows * numCols;
   this->dataValues = new float[numElements];
   for (long jCol = 0; jCol < numCols; jCol++) {
      GiftiDataArray* gda = this->inputMetricFile->getDataArray(jCol);
      const float* giftiData = gda->getDataPointerFloat();
      for (long iRow = 0; iRow < numRows; iRow++) {
         const long indx = (iRow * numCols) + jCol;
         this->dataValues[indx] = giftiData[iRow];
      }
   }
}      

/**
 * called for incremental reads.
 */
QString
BrainModelSurfaceMetricCorrelationMatrix::dataArrayWasRead(
                      GiftiDataArray* gda,
                      const int dataArrayIndex,
                      const int numberOfDataArrays)
{
   /*
    * Data array should be one-dimensional with the number of
    * elements being the number of rows (nodes).
    */
   bool twoDimFlag = false;
   long numRows = -1;
   long dataArrayNumberOfColumns = -1;
   if (gda->getNumberOfDimensions() == 1) {
      numRows = gda->getDimension(0);
   }
   else if (gda->getNumberOfDimensions() == 2) {
      if (gda->getDimension(1) == 1) {
         numRows = gda->getDimension(0);
      }
      else if (gda->getDimension(1) > 1) {
         if (numberOfDataArrays > 1) {
            return "Data file may contain only one two-dimensional data array.";
         }
         numRows = gda->getDimension(0);
         dataArrayNumberOfColumns = gda->getDimension(1);
         twoDimFlag = true;
      }
   }
   if (numRows <= 0) {
      return "Data arrays must be one-dimensional for incremental reading";
   }
   if (dataArrayIndex == 0) {
      this->inputNumRows = numRows;
      if (twoDimFlag) {
         this->inputNumColumns = dataArrayNumberOfColumns;

         /*
          * Use the data, as is, and DO NOT delete the data array.
          */
         this->dataValues = (float*)gda->getDataPointerFloat();
      }
      else {
         this->inputNumColumns = numberOfDataArrays;
         const long numElements = this->inputNumRows * this->inputNumColumns;
         this->dataValues = new float[numElements];
      }
   }
   else {
      if (numRows != this->inputNumRows) {
         return "Data arrays must all have the same dimensions";
      }
   }

   if (twoDimFlag == false) {
      const int numCols = this->inputNumColumns;
      const float* giftiData = gda->getDataPointerFloat();
      for (long iRow = 0; iRow < numRows; iRow++) {
         const long indx = (iRow * numCols) + dataArrayIndex;
         this->dataValues[indx] = giftiData[iRow];
      }

      /*
       * Delete the data array since the data was copied.
       */
      delete gda;
   }


   return "";
}


/**
 * load the data values by incrementally reading the input metric file.
 */
void
BrainModelSurfaceMetricCorrelationMatrix::loadDataValuesMetricIncremental() throw (BrainModelAlgorithmException)
{
   try {
      GiftiDataArrayFileStreamReader::readFileAndReportDataArraysAsTheyAreRead(
               this->inputMetricFileName,
               this);
   }
   catch (FileException e) {
      throw BrainModelAlgorithmException(e);
   }
}

/**
 * get the output metric file (if called, caller is reponsible for DELETING
 * returned metric file).
 */
MetricFile* 
BrainModelSurfaceMetricCorrelationMatrix::getOutputMetricFile() 
{
   this->deleteOutputMetricFlag = false;
   return this->outputMetricFile;
}

/**
 * get the output gifti file (if called, caller is reponsible for DELETING
 * returned gifti file).
 */
GiftiDataArrayFile* 
BrainModelSurfaceMetricCorrelationMatrix::getOutputGiftiFile() 
{
   this->deleteOutputGiftiFlag = false;
   return this->outputGiftiFile;
}

/**
 * compute the means.
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::computeMeans()
{
   this->rowMeans = new float[this->inputNumRows];
   
   const long numRows = this->inputNumRows;
   const long numCols = this->inputNumColumns;
   
   if (this->parallelFlag) {
      #pragma omp parallel for
      for (long iRow = 0; iRow < numRows; iRow++) {
         double sum = 0.0;
         
         for (long j = 0; j < numCols; j++) {
            const long indx = iRow * numCols + j;
            sum += this->dataValues[indx];
         }
         
         this->rowMeans[iRow] = sum / numCols;
      }   
   }
   else {
      for (long iRow = 0; iRow < numRows; iRow++) {
         double sum = 0.0;
         
         for (long j = 0; j < numCols; j++) {
            const long indx = iRow * numCols + j;
            sum += this->dataValues[indx];
         }
         
         this->rowMeans[iRow] = sum / numCols;
      }   
   }
}

/**
 * compute the sum squared.  After this, the dataValues array
 * will contain the value after the mean has been subtracted. 
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::computeSumSquared()
{
   const long numRows = this->inputNumRows;
   const long numCols = this->inputNumColumns;
   this->rowSumSquared = new double[numRows];
   
   if (this->parallelFlag) {
      #pragma omp parallel for
      for (long iRow = 0; iRow < numRows; iRow++) {
         double ss = 0.0;
         double mean = this->rowMeans[iRow];
         
         for (long jCol = 0; jCol < numCols; jCol++) {
            const long indx = iRow * numCols + jCol;
            float f = this->dataValues[indx] - mean;
            ss += (f * f);
            
            /*
             * Save difference of node from for use during correlation calculation
             */
            this->dataValues[indx] = f;
         }
         
         this->rowSumSquared[iRow] = ss;
      }   
   }
   else {
      for (long iRow = 0; iRow < numRows; iRow++) {
         double ss = 0.0;
         double mean = this->rowMeans[iRow];
         
         for (long jCol = 0; jCol < numCols; jCol++) {
            const long indx = iRow * numCols + jCol;
            float f = this->dataValues[indx] - mean;
            ss += (f * f);
            
            /*
             * Save difference of node from for use during correlation calculation
             */
            this->dataValues[indx] = f;
         }
         
         this->rowSumSquared[iRow] = ss;
      }   
   }
}

/**
 * compute the correlations.
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::computeCorrelations(const Mode currentMode)
{
   this->nextRowToProcess = -1;

   switch (currentMode) {
   case MODE_FILES_IN_MEMORY:
#ifdef _OPENMP
      if (this->parallelFlag) {
         long numThreads = omp_get_max_threads();
         if (numThreads > 1) {
            #pragma omp parallel for
            for (long i = 0; i < numThreads; i++) {
               this->computeCorrelationsForRows();
            }
         }
         else {
            this->computeCorrelationsForRows();
         }
      }
      else {
         this->computeCorrelationsForRows();
      }
#else
      this->computeCorrelationsForRows();
#endif
      break;
   case MODE_METRIC_INCREMENTAL:
      {
         FILE* file = fopen("DataFile.dat", "w+");
         if (file == NULL) {
            throw BrainModelAlgorithmException("Failed to open output file for writing.");
         }
#ifdef _OPENMP
         if (this->parallelFlag) {
            long numThreads = omp_get_max_threads();
            if (numThreads > 1) {
               #pragma omp parallel for
               for (long i = 0; i < numThreads; i++) {
                  this->computeCorrelationsForRowsMetricIncremental(file);
               }
            }
            else {
               this->computeCorrelationsForRowsMetricIncremental(file);
            }
         }
         else {
            this->computeCorrelationsForRowsMetricIncremental(file);
         }
#else
         this->computeCorrelationsForRowsMetricIncremental(file);
#endif
         fclose(file);
      }
      break;
   }
}

/**
 * Sompute correlations for rows until there are no more rows to process.
 * Since the correlation matrix is symmetric, we only need to calculate
 * the lower half and can then copy the result to the upper half.
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::computeCorrelationsForRows()
{
   const double tinyValue = 1.0e-20;

   const long numJ = this->outputDimension;
   const long numCols = this->inputNumColumns;

   while (true) {
      long iRow = -1;
      
      //
      // Only one thread may execute this block at any time
      //
      #pragma omp critical
      {
         this->nextRowToProcess++;
         iRow = this->nextRowToProcess;

         if (timingFlag) {
            if ((iRow % 1000) == 0) {
               std::cout << "Processing row " << iRow << std::endl;
            }
         }
      }  
       
      if (iRow >= this->outputDimension) {
         break;
      }

      //
      // Compare with remaining rows
      //
      for (long jRow = iRow; jRow < numJ; jRow++) {
         //
         //
         // diff of mean from col I multiplied by diff of mean from column J
         //
         double sumSquaredBothColumns = 0.0;
         for (long jCol = 0; jCol < numCols; jCol++) {
            const long indxI = iRow * numCols + jCol;
            const long indxJ = jRow * numCols + jCol;
            sumSquaredBothColumns 
               += (this->dataValues[indxI] * this->dataValues[indxJ]);
         }
         
         //
         // Compute correlation coefficient
         //
         const double denominator = this->rowSumSquared[iRow]
                                  * this->rowSumSquared[jRow];
         float r = 0.0;
         if (denominator != 0.0) {
            double sd = std::sqrt(denominator);
            r = sumSquaredBothColumns / sd;
         }
         else {
            r = sumSquaredBothColumns / tinyValue;
         }
         
         /*
          * Apply the Fisher Z-Transform?
          */
         if (this->applyFisherZTransformFlag) {
            float denom = (1.0 - r);
            if (denom != 0.0) {
               r = 0.5 * std::log((1.0 + r) / denom);
            }
            else {
               r = 0.5 * std::log((1.0 + r) / tinyValue);
            }
         }

         //
         // Matrix is symmetric !!!!
         //
         //this->outputMetricFile->setValue(iRow, jRow, r);
         //this->outputMetricFile->setValue(jRow, iRow, r);
         *(this->outputDataArrayColumns[iRow] + jRow) = r;
         *(this->outputDataArrayColumns[jRow] + iRow) = r;
      }
   }
}

/**
 * Sompute correlations for rows until there are no more rows to process.
 * Since the correlation matrix is symmetric, we only need to calculate
 * the lower half and can then copy the result to the upper half.
 *
 * THIS is NOT functioning correctly.  Except for first part of output file,
 * the remainder appears to be zeros.
 */
void
BrainModelSurfaceMetricCorrelationMatrix::computeCorrelationsForRowsMetricIncremental(FILE* file)
{
   const double tinyValue = 1.0e-20;
   const long numJ = this->outputDimension;
   const long numCols = this->inputNumColumns;

   float* dataRow = new float[this->outputDimension];

   while (true) {
      long iRow = -1;

      //
      // Only one thread may execute this block at any time
      //
      #pragma omp critical
      {
         this->nextRowToProcess++;
         iRow = this->nextRowToProcess;
      }

      if (iRow >= this->outputDimension) {
         break;
      }

      //
      // Compare with ALL rows
      //
      for (long jRow = 0; jRow < numJ; jRow++) {
         //
         //
         // diff of mean from col I multiplied by diff of mean from column J
         //
         double sumSquaredBothColumns = 0.0;
         for (long jCol = 0; jCol < numCols; jCol++) {
            const long indxI = iRow * numCols + jCol;
            const long indxJ = jRow * numCols + jCol;
            sumSquaredBothColumns
               += (this->dataValues[indxI] * this->dataValues[indxJ]);
         }

         //
         // Compute correlation coefficient
         //
         const double denominator = this->rowSumSquared[iRow]
                                  * this->rowSumSquared[jRow];
         float r = 0.0;
         if (denominator != 0.0) {
            double sd = std::sqrt(denominator);
            r = sumSquaredBothColumns / sd;
         }
         else {
            r = sumSquaredBothColumns / tinyValue;
         }

         /*
          * Apply the Fisher Z-Transform?
          */
         if (this->applyFisherZTransformFlag) {
            float denom = (1.0 - r);
            if (denom != 0.0) {
               r = 0.5 * std::log((1.0 + r) / denom);
            }
            else {
               r = 0.5 * std::log((1.0 + r) / tinyValue);
            }
         }

         dataRow[jRow] = r;
//         //
//         // Matrix is symmetric !!!!
//         //
//         //this->outputMetricFile->setValue(iRow, jRow, r);
//         //this->outputMetricFile->setValue(jRow, iRow, r);
//         if (this->outputGiftiFlag) {
//            const long indx1 = iRow * numJ + jRow;
//            this->outputGiftiArrayData[indx1] = r;
//            const long indx2 = jRow * numJ + iRow;
//            this->outputGiftiArrayData[indx2] = r;
//         }
//         else {
//            *(this->outputMetricArrayColumns[iRow] + jRow) = r;
//            *(this->outputMetricArrayColumns[jRow] + iRow) = r;
//         }
      }

      //
      // Only one thread may execute this block at any time
      //
      #pragma omp critical
      {
         long offset = iRow * this->outputDimension * 4;
         unsigned long numToWrite = this->outputDimension * 4;

         if (fseek(file, offset, SEEK_SET) != 0) {
            throw BrainModelAlgorithmException("Seek into output file failed.");
         }
         if (fwrite((void*)dataRow, 1, numToWrite, file) != numToWrite) {
            throw BrainModelAlgorithmException("Failed to write bytes to output file.");
         }
      }
   }

   delete[] dataRow;
}


/**
 * create output gifti file.
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::createOutputGiftiFile()
{
   this->outputDimension = this->inputNumRows;
   
   this->outputGiftiFile = new GiftiDataArrayFile();
   
   std::vector<int> dims;
   dims.push_back(this->outputDimension);
   dims.push_back(this->outputDimension);
   
   GiftiDataArray* gda = new GiftiDataArray(this->outputGiftiFile,
                                            "NIFTI_INTENT_NONE",
                                            GiftiDataArray::DATA_TYPE_FLOAT32,
                                            dims,
                                            GiftiDataArray::ENCODING_EXTERNAL_FILE_BINARY);
   this->outputGiftiFile->addDataArray(gda);

   float* giftiData = gda->getDataPointerFloat();
   this->outputDataArrayColumns = new float*[this->outputDimension];

   for (long i = 0; i < this->outputDimension; i++) {
      long offset = i * this->outputDimension;
      this->outputDataArrayColumns[i] = (giftiData + offset);
   }
}

/**
 * create output metric file.
 */
void 
BrainModelSurfaceMetricCorrelationMatrix::createOutputMetricFile()
{
   this->outputDimension = this->inputNumRows;
   this->outputMetricFile = new MetricFile();
   this->outputMetricFile->setNumberOfNodesAndColumns(this->outputDimension, this->outputDimension);
   
   this->outputDataArrayColumns = new float*[this->outputDimension];
   
   for (long i = 0; i < this->outputDimension; i++) {
      GiftiDataArray* gda = this->outputMetricFile->getDataArray(i);
      this->outputDataArrayColumns[i] = gda->getDataPointerFloat();
   }   
}
      
