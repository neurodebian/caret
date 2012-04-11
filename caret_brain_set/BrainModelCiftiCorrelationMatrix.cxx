/*
 *  BrainModelCiftiCorrelationMatrix.cxx
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

#include "BrainModelCiftiCorrelationMatrix.h"
#include <DebugControl.h>

#ifdef _OPENMP
#include <omp.h>
#endif

static const bool timingFlag = false;

/**
 * constructor.
 */ 
BrainModelCiftiCorrelationMatrix::BrainModelCiftiCorrelationMatrix(
                                                BrainSet* bs,
                                                CiftiFile* inputCiftiFile,
                                                const bool applyFisherZTransformFlag,
                                                const bool parallelFlag)
   : BrainModelAlgorithm(bs),
     m_inputCiftiFile(inputCiftiFile),
     m_applyFisherZTransformFlag(applyFisherZTransformFlag),
     m_parallelFlag(parallelFlag)
{
   this->initialize();
}

/**
 * destructor.
 */ 
BrainModelCiftiCorrelationMatrix::~BrainModelCiftiCorrelationMatrix()
{
   if (this->m_deleteOutputCiftiFlag) {
      if (this->m_outputCiftiFile != NULL) {
         delete this->m_outputCiftiFile;
      }
   }
   if (this->m_dataValues != NULL) {
      delete[] this->m_dataValues;
   }
   if (this->m_rowMeans != NULL) {
      delete[] this->m_rowMeans;
   }
   if (this->m_rowSumSquared != NULL) {
      delete[] this->m_rowSumSquared;
   }
}

/**
 * initialize variables.
 */
void 
BrainModelCiftiCorrelationMatrix::initialize()
{
   this->m_deleteOutputCiftiFlag = true;
   this->m_outputCiftiFile = NULL;
   this->m_dataValues = NULL;
   this->m_rowMeans = NULL;
   this->m_rowSumSquared = NULL;
   this->m_nextRowToProcess = -1;
}
      
/**
 * execute the algorithm.
 */                                                                                               
void 
BrainModelCiftiCorrelationMatrix::execute() throw (BrainModelAlgorithmException)
{
   
   QTime loadTimer;
   loadTimer.start();

   Nifti2Header header;
   nifti_2_header head;
   this->m_inputCiftiFile->getHeader(header);
   header.getHeaderStruct(head);
   
   if(DebugControl::getDebugOn()) std::cout << "Input Number of Rows is: " << head.dim[5] << std::endl;
   if(DebugControl::getDebugOn()) std::cout << "Input Number of Columns is: " << head.dim[6] << std::endl;
   this->m_inputNumRows = head.dim[5];
   this->m_inputNumColumns = head.dim[6];

   if ((this->m_inputNumRows <= 0) || (this->m_inputNumColumns <= 0)) {
      throw BrainModelAlgorithmException(
            "Input Cifti file is empty: " );
      //+            this->m_inputCiftiFile->getFileName());
   }
   this->loadDataValues();

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
   this->m_outputDimension = this->m_inputNumRows;

   /*
    * Create the output metric file
    */
   QTime createCiftiTimer;
   createCiftiTimer.start();
   
   this->createOutputCiftiFile();
   
   if (timingFlag) {
      std::cout << "Created output file in "
                << (createCiftiTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }

   /*
    * Comute the correlations
    */
   QTime corrTimer;
   corrTimer.start();
   this->computeCorrelations();
   
   if (timingFlag) {
      std::cout << "Computed correlations in "
                << (corrTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }

   /*
    * Write output file.
    */
   //not needed for in memory calculation
}

/**
 * load the data values.
 */
void 
BrainModelCiftiCorrelationMatrix::loadDataValues() 
{
   /*
    * Create and load the data values.
    */
   CiftiMatrix *matrix=  m_inputCiftiFile->getCiftiMatrix();
   matrix->setCopyData(false);
   std::vector <int> dimensions;
   matrix->getMatrixData(this->m_dataValues,dimensions);   
}      

/**
 * get the output metric file (if called, caller is reponsible for DELETING
 * returned metric file).
 */
CiftiFile* 
BrainModelCiftiCorrelationMatrix::getOutputCiftiFile() 
{
   this->m_deleteOutputCiftiFlag = false;
   if(!this->m_outputCiftiFile)
   {
      this->m_outputCiftiFile = new CiftiFile();
      Nifti2Header header;
      CiftiXML xml;
      CiftiMatrix *matrix = new CiftiMatrix();
      m_inputCiftiFile->getHeader(header);
      m_inputCiftiFile->getCiftiXML(xml);
      nifti_2_header head;
      header.getHeaderStruct(head);
      head.dim[6]=head.dim[5];//dimensions are square now
      head.intent_code = NIFTI_INTENT_CONNECTIVITY_DENSE;
      memset(head.intent_name,0x00,16);
      memcpy(head.intent_name,"ConnDense",9);      
      header.setHeaderStuct(head);
      CiftiRootElement root;
      xml.getXMLRoot(root);
      int mmCount = root.m_matrices.at(0).m_matrixIndicesMap.size();
      
      std::vector <CiftiMatrixIndicesMapElement> mmTemp;
      std::vector <CiftiMatrixIndicesMapElement> *mm;
      mm = &(root.m_matrices.at(0).m_matrixIndicesMap);
      for(int i = 0;i<mmCount;i++)
      {
         if(mm->at(i).m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
            mmTemp.push_back(mm->at(i));
      }
      *mm = mmTemp;
      std::vector <int> appliesToDim (2,0);
      appliesToDim[0]=0;
      appliesToDim[1]=1;
      mm->at(0).m_appliesToMatrixDimension = appliesToDim;
      xml.setXMLRoot(root);
      
      std::vector <int> dim (2,0);
      dim[0]=head.dim[5];
      dim[1]=head.dim[5];
      matrix->setCopyData(false);
      matrix->setMatrixData(this->m_outputDataArrayColumns[0],dim);
      this->m_outputCiftiFile->setHeader(header);
      this->m_outputCiftiFile->setCiftiXML(xml);
      this->m_outputCiftiFile->setCiftiMatrix(*matrix);
   }
   
   return this->m_outputCiftiFile;
}

/**
 * compute the means.
 */
void 
BrainModelCiftiCorrelationMatrix::computeMeans()
{
   this->m_rowMeans = new float[this->m_inputNumRows];
   
   const long numRows = this->m_inputNumRows;
   const long numCols = this->m_inputNumColumns;
   
   if (this->m_parallelFlag) {
      #pragma omp parallel for
      for (long iRow = 0; iRow < numRows; iRow++) {
         double sum = 0.0;
         
         for (long j = 0; j < numCols; j++) {
            const long indx = iRow * numCols + j;
            sum += this->m_dataValues[indx];
         }
         
         this->m_rowMeans[iRow] = sum / numCols;
      }   
   }
   else {
      for (long iRow = 0; iRow < numRows; iRow++) {
         double sum = 0.0;
         
         for (long j = 0; j < numCols; j++) {
            const long indx = iRow * numCols + j;
            sum += this->m_dataValues[indx];
         }
         
         this->m_rowMeans[iRow] = sum / numCols;
      }   
   }
}

/**
 * compute the sum squared.  After this, the dataValues array
 * will contain the value after the mean has been subtracted. 
 */
void 
BrainModelCiftiCorrelationMatrix::computeSumSquared()
{
   const long numRows = this->m_inputNumRows;
   const long numCols = this->m_inputNumColumns;
   this->m_rowSumSquared = new double[numRows];
   
   if (this->m_parallelFlag) {
      #pragma omp parallel for
      for (long iRow = 0; iRow < numRows; iRow++) {
         double ss = 0.0;
         double mean = this->m_rowMeans[iRow];
         
         for (long jCol = 0; jCol < numCols; jCol++) {
            const long indx = iRow * numCols + jCol;
            float f = this->m_dataValues[indx] - mean;
            ss += (f * f);
            
            /*
             * Save difference of node from for use during correlation calculation
             */
            this->m_dataValues[indx] = f;
         }
         
         this->m_rowSumSquared[iRow] = ss;
      }   
   }
   else {
      for (long iRow = 0; iRow < numRows; iRow++) {
         double ss = 0.0;
         double mean = this->m_rowMeans[iRow];
         
         for (long jCol = 0; jCol < numCols; jCol++) {
            const long indx = iRow * numCols + jCol;
            float f = this->m_dataValues[indx] - mean;
            ss += (f * f);
            
            /*
             * Save difference of node from for use during correlation calculation
             */
            this->m_dataValues[indx] = f;
         }
         
         this->m_rowSumSquared[iRow] = ss;
      }   
   }
}

/**
 * compute the correlations.
 */
void 
BrainModelCiftiCorrelationMatrix::computeCorrelations()
{
   this->m_nextRowToProcess = -1;
   
#ifdef _OPENMP
      if (this->m_parallelFlag) {
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

}

/**
 * Sompute correlations for rows until there are no more rows to process.
 * Since the correlation matrix is symmetric, we only need to calculate
 * the lower half and can then copy the result to the upper half.
 */
void 
BrainModelCiftiCorrelationMatrix::computeCorrelationsForRows()
{
   const double tinyValue = 1.0e-20;

   const long numJ = this->m_outputDimension;
   const long numCols = this->m_inputNumColumns;

   while (true) {
      long iRow = -1;
      
      //
      // Only one thread may execute this block at any time
      //
      #pragma omp critical
      {
         this->m_nextRowToProcess++;
         iRow = this->m_nextRowToProcess;

         if (timingFlag) {
            if ((iRow % 1000) == 0) {
               std::cout << "Processing row " << iRow << std::endl;
            }
         }
      }  
       
      if (iRow >= this->m_outputDimension) {
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
               += (this->m_dataValues[indxI] * this->m_dataValues[indxJ]);
         }
         
         //
         // Compute correlation coefficient
         //
         const double denominator = this->m_rowSumSquared[iRow]
                                  * this->m_rowSumSquared[jRow];
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
         if (this->m_applyFisherZTransformFlag) {
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
         //this->outputCiftiFile->setValue(iRow, jRow, r);
         //this->outputCiftiFile->setValue(jRow, iRow, r);
         *(this->m_outputDataArrayColumns[iRow] + jRow) = r;
         *(this->m_outputDataArrayColumns[jRow] + iRow) = r;
      }
   }
}

/**
 * create output metric file.
 */
void 
BrainModelCiftiCorrelationMatrix::createOutputCiftiFile()
{
   if(DebugControl::getDebugOn()) std::cout << "Creating output file of "<< this->m_outputDimension << "x" << this->m_outputDimension << std::endl;
   this->m_outputDataArrayColumns = new float*[this->m_outputDimension];
   unsigned long size = this->m_outputDimension*this->m_outputDimension;
   if(DebugControl::getDebugOn()) std::cout << "Size to allocate is " << size << " floats" << std::endl;
   float *temp = new float [size];
   if(temp == NULL) 
   {
      std::cout << "There was an error allocating enough space for the Cifti Output File" << std::endl;
      exit(1);
   }
   if(DebugControl::getDebugOn()) std::cout << "Initializing output array" <<std::endl;
   for(unsigned long i = 0;i<size;i++) temp[i] = 0.0f;
   if(DebugControl::getDebugOn()) std::cout << "Finished initializing output array" << std::endl;
      
   for (long i = 0; i < this->m_outputDimension; i++) {
      long offset = i*this->m_outputDimension;
      this->m_outputDataArrayColumns[i] = (temp+offset);
   }
}
      
