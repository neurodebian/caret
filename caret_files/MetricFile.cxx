/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>

#include "FileUtilities.h"
#include "MathUtilities.h"
#define _METRIC_MAIN_
#include "MetricFile.h"
#undef  _METRIC_MAIN_

#include "CoordinateFile.h"
#include "DeformationMapFile.h"
#include "StatisticDataGroup.h"
#include "StatisticFalseDiscoveryRate.h"
#include "FreeSurferFunctionalFile.h"
#include "GaussianComputation.h"
#include "MathUtilities.h"
#include "StatisticConvertToZScore.h"
#include "StatisticCorrelationCoefficient.h"
#include "StatisticDataGroup.h"
#include "StatisticDescriptiveStatistics.h"
#include "StatisticsUtilities.h"
#include "StatisticGeneratePValue.h"
#include "StatisticLeveneVarianceEquality.h"
#include "StatisticMeanAndDeviation.h"
#include "StatisticMultipleRegression.h"
#include "StatisticNormalizeDistribution.h"
#include "StatisticPermutation.h"
#include "StatisticRandomNumber.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * The constructor
 */
MetricFile::MetricFile(const QString& descriptiveName,
                       const QString& defaultDataArrayCategoryIn,
                       const QString& defaultExt)
   : GiftiNodeDataFile(descriptiveName, 
                       defaultDataArrayCategoryIn,
                       GiftiDataArray::DATA_TYPE_FLOAT32,
                       1,
                       defaultExt,
                       AbstractFile::FILE_FORMAT_ASCII,
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_NONE,
                       FILE_IO_READ_AND_WRITE)
{
   clear();
}

/**
 * constructor.
 */
MetricFile::MetricFile(const int initialNumberOfNodes,
                       const int initialNumberOfColumns,
                       const QString& descriptiveName,
                       const QString& defaultDataArrayCategoryIn,
                       const QString& defaultExt)
   : GiftiNodeDataFile(descriptiveName, 
                       defaultDataArrayCategoryIn,
                       GiftiDataArray::DATA_TYPE_FLOAT32,
                       1,
                       defaultExt,
                       AbstractFile::FILE_FORMAT_ASCII,
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_NONE,
                       FILE_IO_READ_AND_WRITE)
{
   setNumberOfNodesAndColumns(initialNumberOfNodes, initialNumberOfColumns);
   for (int j = 0; j < initialNumberOfColumns; j++) {
      setColumnAllNodesToScalar(j, 0.0);
   }
}
                 
/**
 * copy constructor.
 */
MetricFile::MetricFile(const MetricFile& mf)
   : GiftiNodeDataFile(mf)
{
   copyHelperMetric(mf);
}

/**
 * The destructor
 */
MetricFile::~MetricFile()
{
   clear();
}

/**
 * assignment operator.
 */
MetricFile& 
MetricFile::operator=(const MetricFile& mf)
{
   if (this != &mf) {
      GiftiNodeDataFile::operator=(mf);
      copyHelperMetric(mf);
   }
   
   return *this;
}

/**
 * copy helper used by assignment operator and copy constructor.
 */
void 
MetricFile::copyHelperMetric(const MetricFile& mf)
{
   columnMappingInfo = mf.columnMappingInfo;
}
      
/**
 * Clear the contents of the metric file
 */
void
MetricFile::clear()
{
   GiftiNodeDataFile::clear();
   setNumberOfNodesAndColumns(0, 0);
   //readColumnNamesOnly = false;
}

/**
 * add a data array.
 */
void 
MetricFile::addDataArray(GiftiDataArray* nda)
{
   GiftiNodeDataFile::addDataArray(nda);
   columnMappingInfo.resize(getNumberOfDataArrays());
}

/**
 * append a data array file to this one.
 */
void 
MetricFile::append(const GiftiDataArrayFile& naf) throw (FileException)
{
   const MetricFile& mf = dynamic_cast<const MetricFile&>(naf);
   int num = getNumberOfDataArrays();
   GiftiNodeDataFile::append(naf);
   int newNum = getNumberOfDataArrays();
   columnMappingInfo.resize(newNum);
   for (int i = num; i < newNum; i++) {
      columnMappingInfo[i] = mf.columnMappingInfo[i - num];
   }
}

/**
 * append a data array file to this one but selectively load/overwrite arraysumns
 * arrayDestination is where naf's arrays should be (-1=new, -2=do not load).
 * "indexDestination" will be updated with the columns actually used.
 */
void 
MetricFile::append(const GiftiDataArrayFile& naf, 
                    std::vector<int>& indexDestination,
                    const FILE_COMMENT_MODE fcm) throw (FileException)
{
   const MetricFile& mf = dynamic_cast<const MetricFile&>(naf);
   const int numAppendArrays = naf.getNumberOfDataArrays();
   if (numAppendArrays <= 0) {
      return;
   }

   GiftiNodeDataFile::append(naf, indexDestination, fcm);
   columnMappingInfo.resize(getNumberOfDataArrays());

   //
   // Note: indexDestination is modified by GiftiDataArrayFile so that it always
   // contains the desination colummn indices
   //
   for (int i = 0; i < numAppendArrays; i++) {
      const int arrayIndex = indexDestination[i];
      if (arrayIndex >= 0) {
         columnMappingInfo[arrayIndex] = mf.columnMappingInfo[i];
      }
   }
}

/**
 * reset a data array.
 */
void 
MetricFile::resetDataArray(const int arrayIndex)
{
   GiftiNodeDataFile::resetDataArray(arrayIndex);
   columnMappingInfo[arrayIndex].reset();
}

/**
 * remove a data array.
 */
void 
MetricFile::removeDataArray(const int arrayIndex)
{
   GiftiNodeDataFile::removeDataArray(arrayIndex);
   
   for (int i = arrayIndex; i < (arrayIndex - 1); i++) {
      columnMappingInfo[i] = columnMappingInfo[i + 1];
   }
   columnMappingInfo.resize(getNumberOfDataArrays());
}
      
/**
 * get the mapping information.
 */
MetricMappingInfo* 
MetricFile::getColumnMappingInfo(const int columnNumber)
{
   columnMappingInfo.resize(getNumberOfColumns());
   if (columnNumber < getNumberOfColumns()) {
      return &columnMappingInfo[columnNumber];
   }
   return NULL;
}

/**
 * get the mapping information (const method).
 */
const MetricMappingInfo* 
MetricFile::getColumnMappingInfo(const int columnNumber) const
{
   if (columnNumber < getNumberOfColumns()) {
      return &columnMappingInfo[columnNumber];
   }
   return NULL;
}      


/**
 * Deform the metric file
 */
void 
MetricFile::deformFile(const DeformationMapFile& dmf, 
                       GiftiNodeDataFile& deformedFile,
                       const DEFORM_TYPE dt) const throw (FileException)
{
   MetricFile& deformedMetricFile = dynamic_cast<MetricFile&>(deformedFile);
   
   const int numNodes = dmf.getNumberOfNodes();
   
   const int numCols = getNumberOfColumns();
   deformedMetricFile.setNumberOfNodesAndColumns(numNodes, numCols);
   
   //
   // Transfer stuff in AbstractFile and NodeAttributeFile
   //
   transferFileDataForDeformation(dmf, deformedMetricFile);

   //
   // Transfer metric file title and column names/comments
   //
   for (int k = 0; k < numCols; k++) {
      float minValue, maxValue;
      getColumnColorMappingMinMax(k, minValue, maxValue);
      deformedMetricFile.setColumnColorMappingMinMax(k, minValue, maxValue);
      float negThresh, posThresh;
      getColumnThresholding(k, negThresh, posThresh);
      deformedMetricFile.setColumnThresholding(k, negThresh, posThresh);
   }
   
   //
   // transfer the metric columns
   //
   float* data0 = new float[numCols];
   float* data1 = new float[numCols];
   float* data2 = new float[numCols];
   float* dataOut = new float[numCols];
   int tileNodes[3];
   float tileAreas[3];
   for (int i = 0; i < numNodes; i++) {
      dmf.getDeformDataForNode(i, tileNodes, tileAreas);
      if (dt == DEFORM_NEAREST_NODE) {
         if (tileNodes[0] > -1) {
            getAllColumnValuesForNode(tileNodes[0], data0);
            for (int j = 0; j < numCols; j++) {
               dataOut[j] = data0[j];
            }
         }
         else {
            for (int j = 0; j < numCols; j++) {
               dataOut[j] = 0.0;
            }
         }
      }
      else {
         if ((tileNodes[0] > -1) && (tileNodes[1] > -1) &&
             (tileNodes[2] > -1)) {
            getAllColumnValuesForNode(tileNodes[0], data0);
            getAllColumnValuesForNode(tileNodes[1], data1);
            getAllColumnValuesForNode(tileNodes[2], data2);
            for (int j = 0; j < numCols; j++) {
               const float totalArea = tileAreas[0] + tileAreas[1] + tileAreas[2];
               if (totalArea > 0.0) {
                  //
                  // Note: Caret has historically done its barycentric calculations
                  // skewed one index
                  //
                  dataOut[j] = data0[j] * (tileAreas[1] / totalArea) 
                             + data1[j] * (tileAreas[2] / totalArea)
                             + data2[j] * (tileAreas[0] / totalArea);
               }
            }
         }
         else {
            for (int j = 0; j < numCols; j++) {
               dataOut[j] = 0.0;
            }
         }
      }
      deformedMetricFile.setAllColumnValuesForNode(i, dataOut);
   }
   
   delete[] data0;
   delete[] data1;
   delete[] data2;
   delete[] dataOut;
}      

/**
 * Get the number of a column with the specified name.
 */
int 
MetricFile::getNamedColumnNumber(const QString& name) const
{
   return getColumnWithName(name);
}

/**
 * Get the color mapping min/max for a column.
 */
void 
MetricFile::getColumnColorMappingMinMax(const int columnNumber,
                                        float& minValue, 
                                        float& maxValue) const
{
   minValue = -1.0;
   maxValue =  1.0;
   std::vector<float> f;
   
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfDataArrays())) {
      if (dataArrays[columnNumber]->getMetaData()->get(metaDataColumnColorMapping, f)) {
         if (f.size() == 2) {
            minValue = f[0];
            maxValue = f[1];
            return;
         }
      }
   
      f.clear();
      f.push_back(minValue);
      f.push_back(maxValue);
      dataArrays[columnNumber]->getMetaData()->set(metaDataColumnColorMapping, f);
   }
}

/**
 * set the color mapping min/max for all columns to the column min/max values.
 */
void 
MetricFile::setColorMappingToColumnMinMax()
{
   const int numCols = getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      float minVal, maxVal;
      getDataColumnMinMax(i, minVal, maxVal);
      setColumnColorMappingMinMax(i, minVal, maxVal);
   }
}
      
/**
 * set color mapping min/max for a column.
 */
void 
MetricFile::setColumnColorMappingMinMax(const int columnNumber,
                                        const float minValue, 
                                        const float maxValue)
{
   float neg, pos;
   getColumnColorMappingMinMax(columnNumber, neg, pos);
   bool saveIt = false;
   if (neg != minValue) {
      saveIt = true;
   }
   if (pos != maxValue) {
      saveIt = true;
   }
   if (saveIt) {
      std::vector<float> f;
      f.push_back(minValue);
      f.push_back(maxValue);
      dataArrays[columnNumber]->getMetaData()->set(metaDataColumnColorMapping, f);
      setModified();
   }
}

/**
 * Get the min/max values for a column
 */
void 
MetricFile::getDataColumnMinMax(const int columnNumber, 
                                float& minValue, float& maxValue) 
{
   minValue = -10000.0;
   maxValue =  10000.0;
   if ((columnNumber >= 0) &&
       (columnNumber < getNumberOfDataArrays())) {
      dataArrays[columnNumber]->getMinMaxValues(minValue, maxValue);
   }
/*
   if (columnMinimumMaximumValid[columnNumber] == false) {
      float minVal =  std::numeric_limits<float>::max();
      float maxVal = -minVal;
      for (int i = 0; i < numberOfNodes; i++) {
         const int offset = getOffset(i, columnNumber);
         if (data[offset] < minVal) minVal = data[offset];
         if (data[offset] > maxVal) maxVal = data[offset];
      }
      columnMinimumValue[columnNumber] = minVal;
      columnMaximumValue[columnNumber] = maxVal;
      columnMinimumMaximumValid[columnNumber] = true;
   }
   minValue = columnMinimumValue[columnNumber];
   maxValue = columnMaximumValue[columnNumber];
*/
}

/**
 * get thresholding for a column.
 */
void 
MetricFile::getColumnThresholding(const int columnNumber,
                                  float& negThreshOut,
                                  float& posThreshOut) const
{
   std::vector<float> f;
   if (dataArrays[columnNumber]->getMetaData()->get(metaDataColumnThreshold, f)) {
      if (f.size() == 2) {
         negThreshOut = f[0];
         posThreshOut = f[1];
         return;
      }
   }
   
   f.clear();
   negThreshOut = 0.0;
   posThreshOut =  0.0;
   f.push_back(negThreshOut);
   f.push_back(posThreshOut);
   dataArrays[columnNumber]->getMetaData()->set(metaDataColumnThreshold, f);
}
                           
/**
 * set thresholding for a column.
 */
void 
MetricFile::setColumnThresholding(const int columnNumber,
                                  const float negThreshIn,
                                  const float posThreshIn)
{
   float oldNeg, oldPos;
   getColumnThresholding(columnNumber, oldNeg, oldPos);
   
   bool saveIt = false;
   if (oldNeg != negThreshIn) {
      const float diff = std::fabs(oldNeg - negThreshIn);
      if (diff > 0.001) {
         saveIt = true;
      }
   }
   if (oldPos != posThreshIn) {
      const float diff = std::fabs(oldPos - posThreshIn);
      if (diff > 0.001) {
         saveIt = true;
      }
   }
   
   if (saveIt) {
      std::vector<float> f;
      f.push_back(negThreshIn);
      f.push_back(posThreshIn);
      dataArrays[columnNumber]->getMetaData()->set(metaDataColumnThreshold, f);
      setModified();
   }
}
                                 
/**
 * get average thresholding for a column.
 */
void 
MetricFile::getColumnAverageThresholding(const int columnNumber,
                                  float& negThreshOut,
                                  float& posThreshOut) const
{
   std::vector<float> f;
   if (dataArrays[columnNumber]->getMetaData()->get(metaDataColumnAverageThreshold, f)) {
      if (f.size() == 2) {
         negThreshOut = f[0];
         posThreshOut = f[1];
         return;
      }
   }
   
   f.clear();
   negThreshOut = 0.0;
   posThreshOut =  0.0;
   f.push_back(negThreshOut);
   f.push_back(posThreshOut);
   dataArrays[columnNumber]->getMetaData()->set(metaDataColumnAverageThreshold, f);
}
                           
/**
 * set average thresholding for a column.
 */
void 
MetricFile::setColumnAverageThresholding(const int columnNumber,
                                  const float negThreshIn,
                                  const float posThreshIn)
{
   float oldNeg, oldPos;
   getColumnAverageThresholding(columnNumber, oldNeg, oldPos);
   
   bool saveIt = false;
   if (oldNeg != negThreshIn) {
      const float diff = std::fabs(oldNeg - negThreshIn);
      if (diff > 0.001) {
         saveIt = true;
      }
   }
   if (oldPos != posThreshIn) {
      const float diff = std::fabs(oldPos - posThreshIn);
      if (diff > 0.001) {
         saveIt = true;
      }
   }
   
   if (saveIt) {
      std::vector<float> f;
      f.push_back(negThreshIn);
      f.push_back(posThreshIn);
      dataArrays[columnNumber]->getMetaData()->set(metaDataColumnAverageThreshold, f);
      setModified();
   }
}
                                 
/**
 * get the number of nodes that exceed each of the thresholds.
 */
void 
MetricFile::getThresholdExceededCounts(const int columnNumber,
                                       const float negThresh,
                                       const float posThresh,
                                       int& numNegExceeded,
                                       int& numPosExceeded) const
{
   numNegExceeded = 0;
   numPosExceeded = 0;
   
   const int numNodes = getNumberOfNodes();
   if (numNodes <= 0) {
      return;
   }
   if ((columnNumber < 0) || (columnNumber >= getNumberOfColumns())) {
      return;
   }
   
   for (int i = 0; i < numNodes; i++) {
      const float v = getValue(i, columnNumber);
      if (v > posThresh) {
         numPosExceeded++;
      }
      if (v < negThresh) {
         numNegExceeded++;
      }
   }
}

                                      
/**
 * Get the metric value for a node's column.
 */
float 
MetricFile::getValue(const int nodeNumber, const int columnNumber) const
{
   float* data = dataArrays[columnNumber]->getDataPointerFloat();
   return data[nodeNumber];
} 

/**
 * get metrics for all columns of a specified node
 *"metrics" must be allocated by user to contain getNumberOfColumns() elements.
 */
void 
MetricFile::getAllColumnValuesForNode(const int nodeNumber, float* metrics) const
{
   for (int i = 0; i < getNumberOfColumns(); i++) {
      float* data = dataArrays[i]->getDataPointerFloat();
      metrics[i] = data[nodeNumber];
   }
}

/**
 * Get the metrics (all columns) for a node.
 */
void 
MetricFile::getAllColumnValuesForNode(const int nodeNumber, std::vector<float>& metrics) const
{
   const int num = getNumberOfColumns();
   if (num > 0) {
      metrics.resize(num);
      for (int i = 0; i < num; i++) {
         float* data = dataArrays[i]->getDataPointerFloat();
         metrics[i] = data[nodeNumber];
      }
   }
   else {
      metrics.clear();
   }
}

/**
 * Set the metric value for a node's column.
 */
void 
MetricFile::setValue(const int nodeNumber, const int columnNumber,
               const float metric)
{
   float* data = dataArrays[columnNumber]->getDataPointerFloat();
   data[nodeNumber] = metric;
   dataArrays[columnNumber]->clearMinMaxFloatValuesValid();
   setModified(); 
}

/**
 * Set all column metrics for a specified node.
 */
void 
MetricFile::setAllColumnValuesForNode(const int nodeNumber, const float* metrics)
{
   for (int i = 0; i < getNumberOfColumns(); i++) {
      float* data = dataArrays[i]->getDataPointerFloat();
      data[nodeNumber] = metrics[i];
      dataArrays[i]->clearMinMaxFloatValuesValid();
   }
   setModified();
}

/**
 * Compute the correlation coefficient for a column in relation to all other columns.
 */
void
MetricFile::correlationCoefficient(const int columnOfInterestNumber,
                                   std::vector<float>& correlationCoefficients,
                                   const std::vector<bool>* limitToTheseNodes) const
{
   const int numberOfColumns = getNumberOfColumns();
   const int numberOfNodes = getNumberOfNodes();
   if ((numberOfColumns <= 0) || (numberOfNodes <= 0)) {
      return;
   }
   
   correlationCoefficients.resize(numberOfColumns);
   
   std::vector<float> columnOfInterest;
   if (limitToTheseNodes == NULL) {
      getColumnForAllNodes(columnOfInterestNumber, columnOfInterest);
   }
   else {
      for (int j = 0; j < numberOfNodes; j++) {
         if ((*limitToTheseNodes)[j]) {
            columnOfInterest.push_back(getValue(j, columnOfInterestNumber));
         }
      }
   }
   
   for (int i = 0; i < numberOfColumns; i++) {
      std::vector<float> otherColumn;
      if (limitToTheseNodes == NULL) {
         getColumnForAllNodes(i, otherColumn);
      }
      else {
         for (int j = 0; j < numberOfNodes; j++) {
            if ((*limitToTheseNodes)[j]) {
               otherColumn.push_back(getValue(j, i));
            }
         }
      }
      
      StatisticDataGroup colX(&columnOfInterest, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDataGroup colY(&otherColumn, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticCorrelationCoefficient coeff;
      coeff.addDataGroup(&colX);
      coeff.addDataGroup(&colY);
      try {
         coeff.execute();
      }
      catch (StatisticException& e) {
         const FileException fe(e);
         std::cout << "Correlation coeffiecient error: " << fe.whatQString().toAscii().constData() << std::endl;
      }
      correlationCoefficients[i] = coeff.getCorrelationCoefficientR2();
   }
}

/**
 * return a metric file that contains descriptive statistics about "this" metric file.
 * If the column name is not empty, it corresponding statistic will be computed.
 */
MetricFile* 
MetricFile::descriptiveStatistics(const bool keepDataColumns,
                                  const QString& meanColumnName,
                                  const QString& varianceColumnName,
                                  const QString& sampleVarianceColumnName,
                                  const QString& standardDeviationColumnName,
                                  const QString& sampleStandardDeviationColumnName,
                                  const QString& standardErrorOfTheMeanColumnName,
                                  const QString& rootMeanSquareColumnName,
                                  const QString& minimumColumnName,
                                  const QString& maximumColumnName,
                                  const QString& medianColumnName,
                                  const QString& skewnessColumnName,
                                  const QString& kurtosisColumnName) const throw (FileException)
{
   if (empty()) {
      throw FileException("Metric file contains no data.");
   }
   
   int numCols = 0;
   
   if (keepDataColumns) {
      numCols = getNumberOfColumns();
   }

   int meanColumn = -1;
   if (meanColumnName.isEmpty() == false) {
      meanColumn = numCols;
      numCols++;
   }

   int varianceColumn = -1;
   if (varianceColumnName.isEmpty() == false) {
      varianceColumn = numCols;
      numCols++;
   }

   int sampleVarianceColumn = -1;
   if (sampleVarianceColumnName.isEmpty() == false) {
      sampleVarianceColumn = numCols;
      numCols++;
   }

   int standardDeviationColumn = -1;
   if (standardDeviationColumnName.isEmpty() == false) {
      standardDeviationColumn = numCols;
      numCols++;
   }

   int sampleStandardDeviationColumn = -1;
   if (sampleStandardDeviationColumnName.isEmpty() == false) {
      sampleStandardDeviationColumn = numCols;
      numCols++;
   }

   int standardErrorOfTheMeanColumn = -1;
   if (standardErrorOfTheMeanColumnName.isEmpty() == false) {
      standardErrorOfTheMeanColumn = numCols;
      numCols++;
   }

   int rootMeanSquareColumn = -1;
   if (rootMeanSquareColumnName.isEmpty() == false) {
      rootMeanSquareColumn = numCols;
      numCols++;
   }

   int minimumColumn = -1;
   if (minimumColumnName.isEmpty() == false) {
      minimumColumn = numCols;
      numCols++;
   }

   int maximumColumn = -1;
   if (maximumColumnName.isEmpty() == false) {
      maximumColumn = numCols;
      numCols++;
   }

   int medianColumn = -1;
   if (medianColumnName.isEmpty() == false) {
      medianColumn = numCols;
      numCols++;
   }

   int skewnessColumn = -1;
   if (skewnessColumnName.isEmpty() == false) {
      skewnessColumn = numCols;
      numCols++;
   }

   int kurtosisColumn = -1;
   if (kurtosisColumnName.isEmpty() == false) {
      kurtosisColumn = numCols;
      numCols++;
   }
   
   //
   // Create the metric file
   //
   if (numCols <= 0) {
      throw FileException("No statistics were selected.");
   }
   const int numNodes = getNumberOfNodes();
   MetricFile* mf = new MetricFile;
   mf->setNumberOfNodesAndColumns(numNodes, numCols);
   
   //
   // Name the columns
   //
   if (meanColumn >= 0) {
      mf->setColumnName(meanColumn, meanColumnName);
   }
   if (varianceColumn >= 0) {
      mf->setColumnName(varianceColumn, varianceColumnName);
   }
   if (sampleVarianceColumn >= 0) {
      mf->setColumnName(sampleVarianceColumn, sampleVarianceColumnName);
   }
   if (standardDeviationColumn >= 0) {
      mf->setColumnName(standardDeviationColumn, standardDeviationColumnName);
   }
   if (sampleStandardDeviationColumn >= 0) {
      mf->setColumnName(sampleStandardDeviationColumn, sampleStandardDeviationColumnName);
   }
   if (standardErrorOfTheMeanColumn >= 0) {
      mf->setColumnName(standardErrorOfTheMeanColumn, standardErrorOfTheMeanColumnName);
   }
   if (rootMeanSquareColumn >= 0) {
      mf->setColumnName(rootMeanSquareColumn, rootMeanSquareColumnName);
   }
   if (minimumColumn >= 0) {
      mf->setColumnName(minimumColumn, minimumColumnName);
   }
   if (maximumColumn >= 0) {
      mf->setColumnName(maximumColumn, maximumColumnName);
   }
   if (medianColumn >= 0) {
      mf->setColumnName(medianColumn, medianColumnName);
   }
   if (skewnessColumn >= 0) {
      mf->setColumnName(skewnessColumn, skewnessColumnName);
   }
   if (kurtosisColumn >= 0) {
      mf->setColumnName(kurtosisColumn, kurtosisColumnName);
   }

   //
   // Number of data columns in input data
   //   
   const int inputDataNumberOfColumns = getNumberOfColumns();

   //
   // copy existing data and set column names?
   //
   if (keepDataColumns) {
      for (int j = 0; j < inputDataNumberOfColumns; j++) {
         //
         // Copy metadata including column names
         //
         const GiftiDataArray* gdaInput = getDataArray(j);
         const GiftiMetaData* mdInput = gdaInput->getMetaData();
         GiftiDataArray* gdaOutput = mf->getDataArray(j);
         GiftiMetaData* mdOutput = gdaOutput->getMetaData();
         *mdOutput = *mdInput;
      }
      
      //
      // Copy data
      //
      for (int i = 0; i < numNodes; i++) {
         for (int j = 0; j < inputDataNumberOfColumns; j++) {
            mf->setValue(i, j, getValue(i, j));
         }
      }
   }
   
   //
   // Array for data
   //
   float* theData = new float[inputDataNumberOfColumns];
   
   //
   // Loop through nodes
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Load the data array
      //
      for (int j = 0; j < inputDataNumberOfColumns; j++) {
         theData[j] = getValue(i, j);
      }
      
      //
      // Compute the descriptive statistics
      //
      StatisticDataGroup sdg(theData, 
                             inputDataNumberOfColumns,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDescriptiveStatistics sds;
      sds.addDataGroup(&sdg);
      try {
         sds.execute();
      }
      catch (StatisticException& e) {
         throw FileException(e);
      }
      
      //
      // set the data for selected statistics
      //
      if (meanColumn >= 0) {
         mf->setValue(i, meanColumn, sds.getMean());
      }
      if (varianceColumn >= 0) {
         mf->setValue(i, varianceColumn, sds.getVariance());
      }
      if (sampleVarianceColumn >= 0) {
         mf->setValue(i, sampleVarianceColumn, sds.getPopulationSampleVariance());
      }
      if (standardDeviationColumn >= 0) {
         mf->setValue(i, standardDeviationColumn, sds.getStandardDeviation());
      }
      if (sampleStandardDeviationColumn >= 0) {
         mf->setValue(i, sampleStandardDeviationColumn, sds.getPopulationSampleStandardDeviation());
      }
      if (standardErrorOfTheMeanColumn >= 0) {
         mf->setValue(i, standardErrorOfTheMeanColumn, sds.getStandardErrorOfTheMean());
      }
      if (rootMeanSquareColumn >= 0) {
         mf->setValue(i, rootMeanSquareColumn, sds.getRootMeanSquare());
      }
      float minValue, maxValue;
      sds.getMinimumAndMaximum(minValue, maxValue);
      if (minimumColumn >= 0) {
         mf->setValue(i, minimumColumn, minValue);
      }
      if (maximumColumn >= 0) {
         mf->setValue(i, maximumColumn, maxValue);
      }
      if (medianColumn >= 0) {
         mf->setValue(i, medianColumn, sds.getMedian());
      }
      if (skewnessColumn >= 0) {
         mf->setValue(i, skewnessColumn, sds.getSkewness());
      }
      if (kurtosisColumn >= 0) {
         mf->setValue(i, kurtosisColumn, sds.getKurtosis());
      }
   }
   
   delete[] theData;
   
   return mf;
}

/**
 * Compute the average and standard deviation for all metric values for each node.
 * If the column name is blank, that statistical measurement is not calculated.  If a
 * columns exists with one of the column names, that column is replaced.
 */
void
MetricFile::computeStatistics(const QString& averageColumnName,
                              const QString& standardDeviationColumnName,
                              const QString& standardErrorColumnName,
                              const QString& minimumAbsColumnName,
                              const QString& maximumAbsColumnName)
{
   if (getNumberOfColumns() < 1) {
      return;
   }
   std::vector<bool> colFlag(getNumberOfColumns(), true);
   computeStatistics(colFlag,
                     averageColumnName,
                     standardDeviationColumnName,
                     standardErrorColumnName,
                     minimumAbsColumnName,
                     maximumAbsColumnName);
}


/**
 * Compute the average and standard deviation for all metric values for each node.
 * If the column name is blank, that statistical measurement is not calculated.  If a
 * columns exists with one of the column names, that column is replaced.
 */
void
MetricFile::computeStatistics(const std::vector<bool>& useColumn,
                              const QString& averageColumnName,
                              const QString& standardDeviationColumnName,
                              const QString& standardErrorColumnName,
                              const QString& minimumAbsColumnName,
                              const QString& maximumAbsColumnName)
{
   if (getNumberOfColumns() < 1) {
      return;
   }
   
   //
   // Find or set the column for the average
   //
   int averageColumn = -1;
   if (averageColumnName.isEmpty() == false) {
      averageColumn = getColumnWithName(averageColumnName);
      if (averageColumn < 0) {
         averageColumn = getNumberOfColumns();
         addColumns(1);
         setColumnName(averageColumn, averageColumnName);
      }
   }
   
   //
   // Find or set the column for the standard deviation
   //
   int deviationColumn = -1;
   if (standardDeviationColumnName.isEmpty() == false) {
      deviationColumn = getColumnWithName(standardDeviationColumnName);
      if (deviationColumn < 0) {
         deviationColumn = getNumberOfColumns();
         addColumns(1);
         setColumnName(deviationColumn, standardDeviationColumnName);
      }
   }
   
   //
   // Find or set the column for the standard error
   //
   int standardErrorColumn = -1;
   if (standardErrorColumnName.isEmpty() == false) {
      standardErrorColumn = getColumnWithName(standardErrorColumnName);
      if (standardErrorColumn < 0) {
         standardErrorColumn = getNumberOfColumns();
         addColumns(1);
         setColumnName(standardErrorColumn, standardErrorColumnName);
      }
   }
   
   //
   // Find or set the column for the minimum
   //
   int minimumAbsColumn = -1;
   if (minimumAbsColumnName.isEmpty() == false) {
      minimumAbsColumn = getColumnWithName(minimumAbsColumnName);
      if (minimumAbsColumn < 0) {
         minimumAbsColumn = getNumberOfColumns();
         addColumns(1);
         setColumnName(minimumAbsColumn, minimumAbsColumnName);
      }
   }
   
   //
   // Find or set the column for the maximum
   //
   int maximumAbsColumn = -1;
   if (maximumAbsColumnName.isEmpty() == false) {
      maximumAbsColumn = getColumnWithName(maximumAbsColumnName);
      if (maximumAbsColumn < 0) {
         maximumAbsColumn = getNumberOfColumns();
         addColumns(1);
         setColumnName(maximumAbsColumn, maximumAbsColumnName);
      }
   }
   
   int numNodes = getNumberOfNodes();
   const int numCols = getNumberOfColumns();
   for (int i= 0; i < numNodes; i++) {
      std::vector<float> values;
      values.reserve(numCols);
      for (int j = 0; j < numCols; j++) {
         if ((j != averageColumn) && (j != deviationColumn) && (j != standardErrorColumn) &&
             (j != minimumAbsColumn) && (j != maximumAbsColumn) &&
             useColumn[j]) {
            values.push_back(getValue(i, j));
         }
      }
      
      if (values.empty()) {
         continue;
      }
      
      //
      // Compute the statistics
      //
/*
      StatisticDescriptiveStatistics ds(StatisticDescriptiveStatistics::DATA_TYPE_SAMPLE);
      StatisticDataGroup sdg(&values, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      ds.addDataGroup(&sdg);
      try {
         ds.execute();
      }
      catch (StatisticException&) {
      }
      const StatisticDescriptiveStatistics::DescriptiveStatistics stats = ds.getDescriptiveStatistics();
*/
      StatisticsUtilities::DescriptiveStatistics stats;
      StatisticsUtilities::computeStatistics(values,
                                             true,
                                             stats);
      if (averageColumn >= 0) {
         setValue(i, averageColumn, stats.average);
      }
      if (deviationColumn >= 0) {
         setValue(i, deviationColumn, stats.standardDeviation);
      }
      if (standardErrorColumn >= 0) {
         setValue(i, standardErrorColumn, stats.standardError);
      }
      if (minimumAbsColumn >= 0) {
         if (stats.leastPositiveValue == 0.0) {
            setValue(i, minimumAbsColumn, stats.leastNegativeValue);
         }
         else if (stats.leastNegativeValue == 0.0) {
            setValue(i, minimumAbsColumn, stats.leastPositiveValue);
         }
         else {
            if (std::fabs(stats.leastPositiveValue) < std::fabs(stats.leastNegativeValue)) {
               setValue(i, minimumAbsColumn, stats.leastPositiveValue);
            }
            else {
               setValue(i, minimumAbsColumn, stats.leastNegativeValue);
            }
         }
      }
      if (maximumAbsColumn >= 0) {
         if (stats.mostPositiveValue == 0.0) {
            setValue(i, maximumAbsColumn, stats.mostNegativeValue);
         }
         else if (stats.mostNegativeValue == 0.0) {
            setValue(i, maximumAbsColumn, stats.mostPositiveValue);
         }
         else {
            if (std::fabs(stats.mostPositiveValue) > std::fabs(stats.mostNegativeValue)) {
               setValue(i, maximumAbsColumn, stats.mostPositiveValue);
            }
            else {
               setValue(i, maximumAbsColumn, stats.mostNegativeValue);
            }
         }
      }
   }

   //
   // set color mapping used for surface shape file
   //
   if (averageColumn >= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(averageColumn, minValue, maxValue);
      setColumnColorMappingMinMax(averageColumn, minValue, maxValue);
   }
   if (deviationColumn>= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(deviationColumn, minValue, maxValue);
      setColumnColorMappingMinMax(deviationColumn, minValue, maxValue);
   }
   if (standardErrorColumn >= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(standardErrorColumn, minValue, maxValue);
      setColumnColorMappingMinMax(standardErrorColumn, minValue, maxValue);
   }
   if (minimumAbsColumn >= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(minimumAbsColumn, minValue, maxValue);
      setColumnColorMappingMinMax(minimumAbsColumn, minValue, maxValue);
   }
   if (maximumAbsColumn >= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(maximumAbsColumn, minValue, maxValue);
      setColumnColorMappingMinMax(maximumAbsColumn, minValue, maxValue);
   }
}

/**
 * Read the metric data for the nodes.
 */
void
MetricFile::readMetricNodeData(QTextStream& stream, QDataStream& binStream) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if (numberOfColumns <= 0) {
      return;
   }
   
   //if (readColumnNamesOnly) {
   //   return;
   //}
   float** dataPtr = new float*[numberOfColumns];
   for (int i = 0; i < numberOfColumns; i++) {
      dataPtr[i] = dataArrays[i]->getDataPointerFloat();
   }
   
   QString line;
   std::vector<QString> tokens;
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         for (int i = 0; i < numberOfNodes; i++) {
            readLineIntoTokens(stream, line, tokens);
            
            if (static_cast<int>(tokens.size()) < (numberOfColumns + 1)) {
               throw FileException(filename, "invalid metric line");
            }
            
            for (int j = 0; j < numberOfColumns; j++) {
               dataPtr[j][i] = tokens[j + 1].toFloat();
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         for (int i = 0; i < numberOfNodes; i++) {
            for (int j = 0; j < numberOfColumns; j++) {
               binStream >> dataPtr[j][i];
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing in XML format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }         
   
   delete[] dataPtr;
}

/**
 * change a range of values to zero.
 */
void 
MetricFile::setRangeOfValuesToZero(const int inputColumnNumber,
                                   const int outputColumnNumberIn,
                                   const QString& outputColumnName,
                                   const float minValue,
                                   const float maxValue,
                                   const bool inclusiveRangeFlag)
                                                    throw (FileException)
{
   //
   // Check for valid input column
   //
   const int numberOfColumns = getNumberOfColumns();
   const int numberOfNodes   = getNumberOfNodes();
   if ((numberOfColumns <= 0) || (numberOfNodes <= 0)) {
      throw FileException("There are no nodes in the metric file.");
   }
   if ((inputColumnNumber < 0) || (inputColumnNumber >= numberOfColumns)) {
      throw FileException("Input column index is invalid.");
   }
   
   //
   // Create a new column if needed.
   //
   int outputColumnNumber = outputColumnNumberIn;
   if ((outputColumnNumber < 0) || (outputColumnNumber >= numberOfColumns)){
      addColumns(1);
      outputColumnNumber = getNumberOfDataArrays() - 1;
   }
   setColumnName(outputColumnNumber, outputColumnName);
   
   for (int i = 0; i < numberOfNodes; i++) {
      float value = getValue(i, inputColumnNumber);
      
      if (inclusiveRangeFlag) {
         if ((value >= minValue) && (value <= maxValue)) {
            value = 0.0;
         }
      }
      else {
         if ((value > minValue) && (value < maxValue)) {
            value = 0.0;
         }
      }
      setValue(i, outputColumnNumber, value);
   }

}
                                  
/**
 * perform binary opertion on columns of metric file.
 */
void 
MetricFile::performBinaryOperation(const BINARY_OPERATION operation,
                                   const int columnA,
                                   const int columnB,
                                   const int resultColumnIn,
                                   const QString& resultColumnName)
                                      throw (FileException)
{
   //
   // Check for valid input column
   //
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if ((numberOfColumns <= 0) || (numberOfNodes <= 0)) {
      throw FileException("There are no nodes in the metric file.");
   }
   if ((columnA < 0) || (columnA >= numberOfColumns)) {
      throw FileException("First column index is invalid.");
   }
   if ((columnB < 0) || (columnB >= numberOfColumns)) {
      throw FileException("Second column index is invalid.");
   }
   
   //
   // Create a new column if needed.
   //
   int resultColumn = resultColumnIn;
   if ((resultColumn < 0) || (resultColumn >= numberOfColumns)){
      addColumns(1);
      resultColumn = getNumberOfDataArrays() - 1;
   }
   setColumnName(resultColumn, resultColumnName);

   QString columnComment;
   QString addverb;
   switch(operation) {
      case BINARY_OPERATION_ADD:
         columnComment = "Added columns ";
         addverb = " and ";
         break;
      case BINARY_OPERATION_SUBTRACT:
         columnComment = "Subtracted columns ";
         addverb = " minus ";
         break;
      case BINARY_OPERATION_MULTIPLY:
         columnComment = "Multiplied columns ";
         addverb = " times ";
         break;
      case BINARY_OPERATION_DIVIDE:
         columnComment = "Divided columns ";
         addverb = " divided by ";
         break;
      case BINARY_OPERATION_AVERAGE:
         columnComment = "Average columns ";
         addverb = " averaged with ";
   }
   columnComment.append(getColumnName(columnA));
   columnComment.append(addverb);
   columnComment.append(getColumnName(columnB));
   setColumnComment(resultColumn, columnComment);

   for (int i = 0; i < numberOfNodes; i++) {
      float valueA = getValue(i, columnA);
      float valueB = getValue(i, columnB);
      float result = 0.0;
      switch(operation) {
         case BINARY_OPERATION_ADD:
            result = valueA + valueB;
            break;
         case BINARY_OPERATION_SUBTRACT:
            result = valueA - valueB;
            break;
         case BINARY_OPERATION_MULTIPLY:
            result = valueA * valueB;
            break;
         case BINARY_OPERATION_DIVIDE:
            if (valueB == 0.0) {
               result = 0.0;
            }
            else {
               result = valueA / valueB;
            }
            break;
         case BINARY_OPERATION_AVERAGE:
            result = (valueA + valueB) * 0.5;
            break;
      }
      setValue(i, resultColumn, result);
   }
}
                            
/**
 * perform unary opertion on columns of metric file.
 */
void 
MetricFile::performUnaryOperation(const UNARY_OPERATION operation,
                                  const int column,
                                  const int resultColumnIn,
                                  const QString& resultColumnName,
                                  const float scalar)
                                      throw (FileException)
{
   //
   // Check for valid input column
   //
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if ((numberOfColumns <= 0) || (numberOfNodes <= 0)) {
      throw FileException("There are no nodes in the metric file.");
   }
   if ((column < 0) || (column >= numberOfColumns)) {
      throw FileException("The column selected is invalid.");
   }
   
   //
   // Create a new column if needed.
   //
   int resultColumn = resultColumnIn;
   if ((resultColumn < 0) || (resultColumn >= numberOfColumns)){
      addColumns(1);
      resultColumn = getNumberOfDataArrays() - 1;
   }
   setColumnName(resultColumn, resultColumnName);
   
   QString columnComment;
   QString adverb;
   switch (operation) {
      case UNARY_OPERATION_ADD:
         columnComment = "Added ";
         adverb = " to ";
         break;
      case UNARY_OPERATION_CEILING:
         columnComment = "Ceiling ";
         break;
      case UNARY_OPERATION_FLOOR:
         columnComment = "Floor ";
         break;
      case UNARY_OPERATION_MULTIPLY:
         columnComment = "Multiplied ";
         adverb = " by ";
         break;
      case UNARY_OPERATION_FIX_NOT_A_NUMBER:
         columnComment = "Fixed not-a-number";
         break;
      case UNARY_OPERATION_ABS_VALUE:
         columnComment = "Abs value";
         break;
      case UNARY_OPERATION_SQUARE_ROOT:
         columnComment = "Square Root";
         break;
      case UNARY_OPERATION_SUBTRACT_FROM_ONE:
         columnComment = "Subtract from One";
         break;
      case UNARY_OPERATION_LOG2:
         columnComment = "Log2";
         break;
   }
   columnComment.append(StringUtilities::fromNumber(scalar));
   columnComment.append(adverb);
   columnComment.append(getColumnName(column));
   setColumnComment(resultColumn, columnComment);

   for (int i = 0; i < numberOfNodes; i++) {
      float value = getValue(i, column);
      switch (operation) {
         case UNARY_OPERATION_ADD:
            value += scalar;
            break;
         case UNARY_OPERATION_ABS_VALUE:
            if (value < 0.0) {
               value = -value;
            }
            break;
         case UNARY_OPERATION_CEILING:
            value = std::min(value, scalar);
            break;
         case UNARY_OPERATION_FLOOR:
            value = std::max(value, scalar);
            break;
         case UNARY_OPERATION_MULTIPLY:
            value *= scalar;
            break;
         case UNARY_OPERATION_FIX_NOT_A_NUMBER:
            if (MathUtilities::isNaN(value)) {
               value = 0.0;
            }
            break;
         case UNARY_OPERATION_SQUARE_ROOT:
            if (value > 0.0) {
               value = std::sqrt(value);
            }
            break;
         case UNARY_OPERATION_SUBTRACT_FROM_ONE:
            value = 1.0 - value;
            break;
         case UNARY_OPERATION_LOG2:  // use scalar as base
            value = MathUtilities::log(scalar, value);
            break;
      }
      setValue(i, resultColumn, value);
   }
}

/**
 * log 10 scale a column (do all columns if column number is negative).
 */
void 
MetricFile::scaleColumnLog10(const int columnIn)
{
   int startColumn = 0;
   int endColumn = getNumberOfColumns() - 1;
   if ((columnIn >= 0) && (columnIn < getNumberOfColumns())) {
      startColumn = columnIn;
      endColumn   = columnIn;
   }
   else if (columnIn >= getNumberOfColumns()) {
      return;
   }
   
   const int numNodes = getNumberOfNodes();
   for (int i = startColumn; i <= endColumn; i++) {
      for (int j = 0; j < numNodes; j++) {
         float value = getValue(j, i);
         if (value < 0.00001) {
            value = -5.0;
         }
         else {
            value = std::log10(value);
         }
         setValue(j, i, value);
      }
   }
}      

/**
 * neighbor value smoothing.
 */
void 
MetricFile::smoothNeighbors(const TopologyFile* tf, const int column)
{
   //
   // Get the topology helper
   //
   if (tf == NULL) return;
   const TopologyHelper* topologyHelper = 
                      tf->getTopologyHelper(false, true, false);
   if ((column < 0) || (column >= getNumberOfColumns())) {
      return;
   }
   
   //
   // Loop until no nodes have their values modified
   //
   bool nodeModified = true;
   while (nodeModified) {
      nodeModified = false;
      
      //
      // Loop through the nodes
      //
      const int numNodes = getNumberOfNodes();
      for (int i = 0; i < numNodes; i++) {
         //
         // Get neighbors
         //
         int numNeighbors = 0;
         const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
         
         //
         // Do I have neighbors
         //
         if (numNeighbors >= 0) {
            //
            // Get my value
            //
            const float value = getValue(i, column);
            
            //
            // If my value is non-zero
            //
            if (value != 0.0) {
               //
               // Loop through neighbors
               //
               for (int j = 0; j < numNeighbors; j++) {
                  //
                  // If neighbor is connected
                  //
                  const int neigh = neighbors[j];
                  if (topologyHelper->getNodeHasNeighbors(neigh)) {
                     //
                     // If the neighbor is zero
                     //
                     if (getValue(neigh, column) == 0.0) {
                        //
                        // Set to my value
                        //
                        setValue(neigh, column, value);
                        
                        //
                        // Mark that a node was modified this iteration
                        //
                        nodeModified = true;
                     }
                  }
               }
            }
         }
      }
   }    
}      

/**
 * average neighbor smooth a metric column.
 * (if output column is negative a new column is created).
 */
void 
MetricFile::smoothAverageNeighbors(const int column, 
                                   const int outputColumnIn,
                                   const QString& outputColumnName,
                                   const float strength,
                                   const int iterations,
                                   const TopologyFile* topologyFile)
{   
   //
   // Check for valid input column
   //
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if ((numberOfColumns <= 0) || (numberOfNodes <= 0)) {
      return;
   }
   if ((column < 0) || (column >= numberOfColumns)) {
      return;
   }
   
   //
   // Inverse of strength is applied to the node's current metric value
   //
   const float oneMinusStrength = 1.0 - strength;
   
   //
   // Create a new column if needed.
   //
   int outputColumn = outputColumnIn;
   if ((outputColumn < 0) || (outputColumn >= numberOfColumns)){
      addColumns(1);
      outputColumn = getNumberOfDataArrays() - 1;
   }
   setColumnName(outputColumn, outputColumnName);
   
   //
   // Copy the input column to the output column
   //
   if (column != outputColumn) {
      std::vector<float> values;
      getColumnForAllNodes(column, values);
      setColumnForAllNodes(outputColumn, values);
   }
   
   //
   // column now being smoothed
   //
   const int smoothColumn = outputColumn;
   
   //
   // Get the topology helper
   //
   const TopologyHelper* topologyHelper = 
                      topologyFile->getTopologyHelper(false, true, false);
    
   //
   // Allocate arrays for storing data of column being smoothed
   //
   float* inputValues = new float[numberOfNodes];
   float* outputValues = new float[numberOfNodes];
   
   //
   // smooth the data for the specified number of iterations
   //
   for (int iter = 0; iter < iterations; iter++) {
      //
      // allow other events to process
      //
      AbstractFile::allowEventsToProcess();

      //
      // load arrays for smoothing data
      //
      //std::vector<float> columnValues(numberOfNodes);
      //getColumnForAllNodes(outputColumn, columnValues);
      getColumnForAllNodes(smoothColumn, inputValues);        
      
      //
      // smooth all of the nodes
      //
      for (int i = 0; i < numberOfNodes; i++) {
         //
         // copy input to output in event this node is not smoothed
         //
         outputValues[i] = inputValues[i];
         
         //
         // Get the neighbors for this node
         //
         //std::vector<int> neighbors;
         //topologyHelper->getNodeNeighbors(i, neighbors);
         int numNeighbors = 0;
         const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
         
         //
         // Does this node have neighbors
         //
         //const int numNeighbors = static_cast<int>(neighbors.size());
         if (numNeighbors > 0) {         
            //
            // smooth metric data for this node
            //
            float neighborSum = 0.0;
            for (int j = 0; j < numNeighbors; j++) {
               //
               // Note: outputColumn has output from last iteration of smoothing
               //
               neighborSum += inputValues[neighbors[j]];
            }
            const float neighborAverage = neighborSum / static_cast<float>(numNeighbors);
            
            //
            // Apply smoothing to the node
            //
            outputValues[i] = (inputValues[i] * oneMinusStrength)
                            + (neighborAverage * strength);
         }
      }
      
      //
      // Copy the smoothed values to the output column
      //
      setColumnForAllNodes(smoothColumn, outputValues);
   }
   
   //
   // Add comments describing smoothing
   //
   QString smoothComment(getColumnComment(smoothColumn));
   if (smoothComment.isEmpty() == false) {
      smoothComment.append("\n");
      smoothComment.append("Average Neighbors Smoothing: \n");
   }
   smoothComment.append("   Stength/Iterations: ");
   smoothComment.append(StringUtilities::fromNumber(strength));
   smoothComment.append(" ");
   smoothComment.append(StringUtilities::fromNumber(iterations));
   smoothComment.append("\n");
   setColumnComment(smoothColumn, smoothComment);
   
   delete[] inputValues;
   delete[] outputValues;
   
   setModified();
}                        
/*
void 
MetricFile::smooth(const SMOOTH_ALGORITHM algorithm,
                   const int column, 
                   const int outputColumnIn,
                   const QString& outputColumnName,
                   const float strength,
                   const int iterations,
                   const TopologyFile* topologyFile,
                   const CoordinateFile* coordinateFile,
                   const CoordinateFile* nodeNormalVectors,
                   const float gaussNormBelowCutoff,
                   const float gaussNormAboveCutoff,
                   const float gaussSigmaNorm,
                   const float gaussSigmaTang,
                   const float gaussTangentCutoff)
{   
   //
   // Check for valid input column
   //
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if ((numberOfColumns <= 0) || (numberOfNodes <= 0)) {
      return;
   }
   if ((column < 0) || (column >= numberOfColumns)) {
      return;
   }
   
   //
   // Inverse of strength is applied to the node's current metric value
   //
   const float oneMinusStrength = 1.0 - strength;
   
   //
   // Create a new column if needed.
   //
   int outputColumn = outputColumnIn;
   if ((outputColumn < 0) || (outputColumn >= numberOfColumns)){
      addColumns(1);
      outputColumn = getNumberOfDataArrays() - 1;
   }
   setColumnName(outputColumn, outputColumnName);
   
   //
   // Copy the input column to the output column
   //
   if (column != outputColumn) {
      std::vector<float> values;
      getColumnForAllNodes(column, values);
      setColumnForAllNodes(outputColumn, values);
   }
   
   //
   // column now being smoothed
   //
   const int smoothColumn = outputColumn;
   
   //
   // Get the topology helper
   //
   const TopologyHelper* topologyHelper = 
                      topologyFile->getTopologyHelper(false, true, false);
    
   //
   // Allocate arrays for storing data of column being smoothed
   //
   float* inputValues = new float[numberOfNodes];
   float* outputValues = new float[numberOfNodes];
   
   //
   // Used if gaussian smoothing is being performed
   //
   GaussianComputation gauss(gaussNormBelowCutoff,
                             gaussNormAboveCutoff,
                             gaussSigmaNorm,
                             gaussSigmaTang,
                             gaussTangentCutoff);
                             
   //
   // smooth the data for the specified number of iterations
   //
   for (int iter = 0; iter < iterations; iter++) {
      //
      // allow other events to process
      //
      AbstractFile::allowEventsToProcess();

      //
      // load arrays for smoothing data
      //
      //std::vector<float> columnValues(numberOfNodes);
      //getColumnForAllNodes(outputColumn, columnValues);
      getColumnForAllNodes(smoothColumn, inputValues);        
      
      //
      // smooth all of the nodes
      //
      for (int i = 0; i < numberOfNodes; i++) {
         //
         // copy input to output in event this node is not smoothed
         //
         outputValues[i] = inputValues[i];
         
         //
         // Get the neighbors for this node
         //
         //std::vector<int> neighbors;
         //topologyHelper->getNodeNeighbors(i, neighbors);
         int numNeighbors = 0;
         const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
         
         //
         // Does this node have neighbors
         //
         //const int numNeighbors = static_cast<int>(neighbors.size());
         if (numNeighbors > 0) {
         
            switch (algorithm) {
               case SMOOTH_ALGORITHM_NONE:
                  break;
               case SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
                  {
                     //
                     // smooth metric data for this node
                     //
                     float neighborSum = 0.0;
                     for (int j = 0; j < numNeighbors; j++) {
                        //
                        // Note: outputColumn has output from last iteration of smoothing
                        //
                        neighborSum += inputValues[neighbors[j]];
                     }
                     const float neighborAverage = neighborSum / static_cast<float>(numNeighbors);
                     
                     //
                     // Apply smoothing to the node
                     //
                     outputValues[i] = (inputValues[i] * oneMinusStrength)
                                     + (neighborAverage * strength);
                  };
                  break;
               case SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
                  {
                     //
                     // Coordinates of all nodes
                     //
                     const float* allNodesCoords = coordinateFile->getCoordinate(0);
                     const float* allNormalVectors = (nodeNormalVectors != NULL)
                                                   ? nodeNormalVectors->getCoordinate(0)
                                                   : NULL;
   
                     std::vector<GaussianComputation::Point3D> points;
                     for (int j = 0; j < numNeighbors; j++) {
                        const int neigh = neighbors[j];
                        points.push_back(GaussianComputation::Point3D(&allNodesCoords[neigh*3],
                                                           inputValues[neigh]));
                     }
                     outputValues[i] = gauss.evaluate(&allNodesCoords[i*3],
                                                      &allNormalVectors[i*3],
                                                      points);
                  }
                  break;
               case SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
                  {
                     //
                     // Distances to each neighbor
                     //
                     std::vector<float> neighborDistance(numNeighbors);
                     float totalDistance = 0.0;
                     for (int j = 0; j < numNeighbors; j++) {
                        neighborDistance[j] = 
                           coordinateFile->getDistanceBetweenCoordinates(i, neighbors[j]);
                        totalDistance += neighborDistance[j];
                     }
                     if (totalDistance == 0.0) {
                        totalDistance = 1.0;
                     }
                     
                     //
                     // compute neighbor weighted average
                     //
                     std::vector<float> neighborWeights(numNeighbors);
                     float totalWeight = 0.0;
                     for (int j = 0; j < numNeighbors; j++) {
                        neighborWeights[j] =  1.0 - (neighborDistance[j] / totalDistance);
                        totalWeight += neighborWeights[j];
                     }
                     if (totalWeight == 0.0) {
                        totalWeight = 1.0;
                     }
                     
                     //
                     // compute neighbor weighted average
                     //
                     float neighborSum = 0.0;
                     for (int j = 0; j < numNeighbors; j++) {
                        const float weight = neighborWeights[j] / totalWeight;
                        neighborSum += inputValues[neighbors[j]] * weight;
                     }
                     
                     //
                     // Apply smoothing to the node
                     //
                     outputValues[i] = (inputValues[i] * oneMinusStrength)
                                     + (neighborSum * strength);
                  }
                  break;
            }
         }
      }
      
      //
      // Copy the smoothed values to the output column
      //
      setColumnForAllNodes(smoothColumn, outputValues);
   }
   
   //
   // Add comments describing smoothing
   //
   QString smoothComment(getColumnComment(smoothColumn));
   if (smoothComment.isEmpty() == false) {
      smoothComment.append("\n");
   }
   switch (algorithm) {
      case SMOOTH_ALGORITHM_NONE:
         smoothComment.append("Invalid smoothing algorithm: \n");
         break;
      case SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
         smoothComment.append("Average Neighbors Smoothing: \n");
         break;
      case SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
         smoothComment.append("Gaussian Smoothing (Surface Normal-Based: \n");
         smoothComment.append("   Norm Below Cutoff: ");
         smoothComment.append(StringUtilities::fromNumber(gaussNormBelowCutoff));
         smoothComment.append("\n");
         smoothComment.append("   Norm Above Cutoff: ");
         smoothComment.append(StringUtilities::fromNumber(gaussNormAboveCutoff));
         smoothComment.append("\n");
         smoothComment.append("   Sigma Norm: ");
         smoothComment.append(StringUtilities::fromNumber(gaussSigmaNorm));
         smoothComment.append("\n");
         smoothComment.append("   Sigma Tang: ");
         smoothComment.append(StringUtilities::fromNumber(gaussSigmaTang));
         smoothComment.append("\n");
         smoothComment.append("   Tangend Cutoff: ");
         smoothComment.append(StringUtilities::fromNumber(gaussTangentCutoff));
         smoothComment.append("\n");
         break;
      case SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
         smoothComment.append("Weighted Average Neighbors Smoothing: \n");
         break;
   }
   smoothComment.append("   Stength/Iterations: ");
   smoothComment.append(StringUtilities::fromNumber(strength));
   smoothComment.append(" ");
   smoothComment.append(StringUtilities::fromNumber(iterations));
   smoothComment.append("\n");
   setColumnComment(smoothColumn, smoothComment);
   
   delete[] inputValues;
   delete[] outputValues;
   
   setModified();
}                        
*/

/**
 * Get a column of values for all nodes
 */
void
MetricFile::getColumnForAllNodes(const int columnNumber,
                                 std::vector<float>& values) const
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if (columnNumber >= numberOfColumns) {
      std::cout << "PROGRAM ERROR: Invalid column number " 
                << columnNumber
                << " in MetricFile::getColumnForAllNodes()."
                << std::endl;
      return;
   }
   values.resize(numberOfNodes);
   for (int i = 0; i < numberOfNodes; i++) {
      values[i] = getValue(i, columnNumber);
   }
}

/**
 * Set a column of values for all nodes
 */
void
MetricFile::setColumnForAllNodes(const int columnNumber,
                                 const std::vector<float>& values) 
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if (columnNumber >= numberOfColumns) {
      std::cout << "PROGRAM ERROR: Invalid column number " 
                << columnNumber
                << " in MetricFile::setColumnForAllNodes()."
                << std::endl;
      return;
   }
   const int numValues = static_cast<int>(values.size());
   const int num = std::min(numValues, numberOfNodes);
   for (int i = 0; i < num; i++) {
      setValue(i, columnNumber, values[i]);
   }
}

/**
 * Set a column with a scalar for all nodes.
 */
void 
MetricFile::setColumnAllNodesToScalar(const int columnNumber,
                                      const float value)
{
   const int numberOfNodes = getNumberOfNodes();
   for (int i = 0; i < numberOfNodes; i++) {
      setValue(i, columnNumber, value);
   }
   setColumnColorMappingMinMax(columnNumber, value, value);
}
      
/**
 * Get a column of values for all nodes (values should have getNumberOfNodes() elements)
 */
void
MetricFile::getColumnForAllNodes(const int columnNumber,
                                 float* values) const
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if (columnNumber >= numberOfColumns) {
      std::cout << "PROGRAM ERROR: Invalid column number " 
                << columnNumber
                << " in MetricFile::getColumnForAllNodes()."
                << std::endl;
      return;
   }
   for (int i = 0; i < numberOfNodes; i++) {
      values[i] = getValue(i, columnNumber);
   }
}

/**
 * Set a column of values for all nodes (values should have getNumberOfNodes() elements)
 */
void
MetricFile::setColumnForAllNodes(const int columnNumber,
                                 const float* values) 
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if (columnNumber >= numberOfColumns) {
      std::cout << "PROGRAM ERROR: Invalid column number " 
                << columnNumber
                << " in MetricFile::setColumnForAllNodes()."
                << std::endl;
      return;
   }
   for (int i = 0; i < numberOfNodes; i++) {
      setValue(i, columnNumber, values[i]);
   }
}

/**
 * Read only the column names from the metric file.
 */
/*
void 
MetricFile::readFileColumnNames(const QString& filename,
                                 std::vector<QString>& columnNamesOut) throw (FileException)
{
   columnNamesOut.clear();
   readColumnNamesOnly = true;
   try {
      readFile(filename);
   }
   catch (FileException& e) {
      throw(e);
   }
   for (int i = 0; i < numberOfColumns; i++) {
      columnNamesOut.push_back(columnNames[i]);
   }
   clear();
   readColumnNamesOnly = false;
}
*/

/**
 * Read metric file version 0
 */
void
MetricFile::readFileVersion_0(QFile& file, QTextStream& stream,
                              QDataStream& binStream) throw (FileException)
{
   int numNodes = 0;
   int numCols  = 0;
   
   // get starting position of data
   qint64 startOfMetricData = stream.pos(); //file.pos();
   
   // read through file once to get number of nodes and number of columns
   QString line;
   while(stream.atEnd() == false) {
      readLine(stream, line);
      if (numNodes == 0) {
         std::vector<QString> tokens;
         StringUtilities::token(line, " ", tokens);
         numCols = tokens.size() - 1;
      }
      numNodes++;
   }
   
   if ((numNodes <= 0) || (numCols <= 0)) {
      throw FileException(filename, "metric file has no data");
   }
   
   setNumberOfNodesAndColumns(numNodes, numCols);
   
   // set file position as data will now be read
   file.seek(startOfMetricData);
   stream.seek(startOfMetricData);
   
   return readMetricNodeData(stream, binStream);
}

/**
 * Read version 1 of the metric file.
 */
void 
MetricFile::readFileVersion_1(QTextStream& stream,
                              QDataStream& binStream) throw (FileException)
{
   int numNodes = 0;
   int numCols  = 0;
   
   QString line;
   readLine(stream, line);
   QTextStream(&line, QIODevice::ReadOnly) >> numNodes >> numCols;

   if ((numNodes <= 0) || (numCols <= 0)) {
      throw FileException(filename, "Number of nodes/columns missing");
   }   
   setNumberOfNodesAndColumns(numNodes, numCols);
   
   readLine(stream, line);
   float userMinimum, userMaximum;
   QTextStream(&line, QIODevice::ReadOnly) >> userMinimum >> userMaximum;
   
   for (int j = 0; j < numCols; j++) {
      QString colNum, value;
      readTagLine(stream, colNum, value);
      setColumnName(j, value);
   }
   
   readMetricNodeData(stream, binStream);
}

/**
 * Read metric file version 2
 */
void
MetricFile::readFileVersion_2(QFile& file,
                              QTextStream& stream,
                              QDataStream& binStream) throw (FileException)
{
   int numNodes = -1;
   int numCols  = -1;
   
   bool readingTags = true;
   while (readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      if (tag == tagNumberOfNodes) {
         if (tagValue.isEmpty()) {
            throw FileException(filename, "Reading line containing number of nodes");
         }
         else {
            numNodes = tagValue.toInt();
            if (numCols > 0) {
               setNumberOfNodesAndColumns(numNodes, numCols);
            }
         }
      }
      else if (tag == tagNumberOfColumns) {
         if (tagValue.isEmpty()) {
            throw FileException(filename, "Reading line containing number of columns");
         }
         else {
            numCols = tagValue.toInt();
            if (numNodes > 0) {
               setNumberOfNodesAndColumns(numNodes, numCols);
            }
         }
      }
      else if (tag == tagFileTitle) {
         fileTitle = tagValue;
      }
      else if (tag == tagBeginData) {
         readingTags = false;
      }
      else if (tag == tagColumnName) {
         QString name;
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         setColumnName(indx, name);
      }
      else if (tag == tagColumnComment) {
         QString name;
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         setColumnComment(indx, StringUtilities::setupCommentForDisplay(name));
      }
      else if (tag == tagColumnStudyMetaData) {
         QString name;
         const int indx = splitTagIntoColumnAndValue(tagValue, name);
         StudyMetaDataLinkSet smdls;
         smdls.setLinkSetFromCodedText(name);
         setColumnStudyMetaDataLinkSet(indx, smdls);
      }
      else if (tag == tagColumnColorMapping) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " ", tokens);
         if (tokens.size() == 3) {
            int columnNumber = tokens[0].toInt();
            if (columnNumber >= 0) {
               setColumnColorMappingMinMax(columnNumber,
                                           tokens[1].toFloat(),
                                           tokens[2].toFloat());
            }
            else {
               throw FileException(filename, "column min/max color index invalid");
            }
         }
         else {
            throw FileException(filename, "reading column min/max color mapping");
         }
      }
      else if (tag == tagColumnThreshold) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " ", tokens);
         if (tokens.size() == 3) {
            int columnNumber = tokens[0].toInt();
            if (columnNumber >= 0) {
               setColumnThresholding(columnNumber,
                                     tokens[1].toFloat(),
                                     tokens[2].toFloat());
            }
            else {
               throw FileException(filename, "column threshold index invalid");
            }
         }
         else {
            throw FileException(filename, "reading column threshold");
         }
      }
      else if (tag == tagColumnAverageThreshold) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " ", tokens);
         if (tokens.size() == 3) {
            int columnNumber = tokens[0].toInt();
            if (columnNumber >= 0) {
               setColumnAverageThresholding(columnNumber,
                                            tokens[1].toFloat(),
                                            tokens[2].toFloat());
            }
            else {
               throw FileException(filename, "column average threshold index invalid");
            }
         }
         else {
            throw FileException(filename, "reading column average thresholding");
         }
      }
      else {
         std::cerr << "WARNING: Unknown Metric File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }
   
#ifdef QT4_FILE_POS_BUG
   if (getFileReadType() == FILE_FORMAT_BINARY) {
      qint64 offset = findBinaryDataOffsetQT4Bug(file, "tag-BEGIN-DATA");
      if (offset > 0) {
         offset++;  
         file.seek(offset);
      }
   }
#else
   file.seek(stream.pos());
#endif // QT4_FILE_POS_BUG
   
   readMetricNodeData(stream, binStream);
}

/**
 * Read the metric file
 */
void
MetricFile::readLegacyNodeFileData(QFile& file, QTextStream& stream, 
                         QDataStream& binStream) throw (FileException)
{
   // get starting position of data
   const qint64 startOfMetricData = stream.pos(); //file.pos();
   
   int version = 0;
   
   // read first line to see if it is version 1 or later
   QString versionStr, versionNumberStr;
   readTagLine(stream, versionStr, versionNumberStr);
   if ((versionStr == "metric-version") || (versionStr == tagFileVersion)) {
      switch(versionNumberStr.toInt()) {
         case 1:
            version = 1;
            break;
         case 2:
            version = 2;
            break;
         default:
            throw FileException(filename, "Unknown version of metric file");
      }
   }
   //printf("Metric File Version: %d\n", version);
   
   switch(version) {
      case 1:
         readFileVersion_1(stream, binStream);
         break;
      case 2:
         readFileVersion_2(file, stream, binStream);
         break;
      default:
         // set file position as data will now be read
         file.seek(startOfMetricData);
         stream.seek(startOfMetricData);
         readFileVersion_0(file, stream, binStream);
         break;
   }
}

/**
 * Write the metric file data
 */
void
MetricFile::writeLegacyNodeFileData(QTextStream& stream, QDataStream& binStream) throw (FileException)
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   
   stream << tagFileVersion << " 2" << "\n";  //"metric-version 2\n";
   stream << tagNumberOfNodes << " " << numberOfNodes << "\n";
   stream << tagNumberOfColumns << " " << numberOfColumns << "\n";
   stream << tagFileTitle << " " << fileTitle << "\n";
   for (int j = 0; j < numberOfColumns; j++) {
      stream << tagColumnName << " " << j << " " << getColumnName(j) << "\n";
   }
   for (int m = 0; m < numberOfColumns; m++) {
      stream << tagColumnComment << " " <<   m 
             << " " << StringUtilities::setupCommentForStorage(getColumnComment(m)) << "\n";
   }
   for (int k = 0; k < numberOfColumns; k++) {
      stream << tagColumnStudyMetaData << " " << k
             << " " << getColumnStudyMetaDataLinkSet(k).getLinkSetAsCodedText().toAscii().constData() << "\n";
   }
   for (int k = 0; k < numberOfColumns; k++) {
      float neg, pos;
      getColumnColorMappingMinMax(k, neg, pos);
      stream << tagColumnColorMapping << " " << k << " "
             << neg << " " << pos << "\n";
   }
   for (int k = 0; k < numberOfColumns; k++) {
      float neg, pos;
      getColumnThresholding(k, neg, pos);
      stream << tagColumnThreshold << " " << k << " "
             << neg << " " << pos << "\n";
   }
   for (int k = 0; k < numberOfColumns; k++) {
      float neg, pos;
      getColumnAverageThresholding(k, neg, pos);
      stream << tagColumnAverageThreshold << " " << k << " "
             << neg << " " << pos << "\n";
   }
   stream << tagBeginData << "\n";
   
   if (numberOfColumns > 0) {
      float** dataPtr = new float*[numberOfColumns];
      for (int i = 0; i < numberOfColumns; i++) {
         dataPtr[i] = dataArrays[i]->getDataPointerFloat();
      }

      switch (getFileWriteType()) {
         case FILE_FORMAT_ASCII:
            for (int i = 0; i < numberOfNodes; i++) {
               stream << i;
               for (int j = 0; j < numberOfColumns; j++) {
                  stream << " " << dataPtr[j][i];
               }
               stream << "\n";
            }
            break;
         case FILE_FORMAT_BINARY:
#ifdef QT4_FILE_POS_BUG
            setBinaryFilePosQT4Bug();
#else  // QT4_FILE_POS_BUG
         //
         // still a bug in QT 4.2.2
         //
         setBinaryFilePosQT4Bug();
#endif // QT4_FILE_POS_BUG
            for (int i = 0; i < numberOfNodes; i++) {
               for (int j = 0; j < numberOfColumns; j++) {
                  binStream << dataPtr[j][i];
               }
            }
            break;
         case FILE_FORMAT_XML:
            throw FileException(filename, "Writing in XML format not supported.");
            break;
         case FILE_FORMAT_XML_BASE64:
            throw FileException(filename, "XML Base64 not supported.");
            break;
         case FILE_FORMAT_XML_GZIP_BASE64:
            throw FileException(filename, "XML GZip Base64 not supported.");
            break;
         case FILE_FORMAT_OTHER:
            throw FileException(filename, "Writing in Other format not supported.");
            break;
         case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
            throw FileException(filename, "Comma Separated Value File Format not supported.");
            break;
      }
      delete[] dataPtr;
   }
}

/**
 * Export to a free surfer functional file.
 */
void
MetricFile::exportFreeSurferAsciiFunctionalFile(const int columnNumber,
                                               const QString& filename) throw (FileException)
{
   if ((columnNumber >= 0) && (columnNumber < getNumberOfColumns())) {
      FreeSurferFunctionalFile fsff;
      const int numNodes = getNumberOfNodes();
      fsff.setNumberOfFunctionalItems(numNodes);
      for (int i = 0; i < numNodes; i++) {
         const float metric = getValue(i, columnNumber);
         if (metric != 0.0) {
            fsff.addFunctionalData(i, metric);
         }
      }
      fsff.writeFile(filename);
   }
   else {
      throw FileException(filename, "Invalid column number for export");
   }
}

/**
 * Import free surfer functional as metric.
 */
void
MetricFile::importFreeSurferFunctionalFile(const int numNodes,
                                           const QString& filename,
                                           const AbstractFile::FILE_FORMAT fileFormat) throw (FileException)
{
   if (numNodes == 0) {
      throw FileException(filename, "Number of nodes must be set prior to importing a \n"
                                     "FreeSurfer functional file.  This is usually\n"
                                     "accomplished by importing an \"orig\" surface prior\n"
                                     "to importing functional data.");
   }

   //
   // Add a column to this surface shape file
   //
   if (getNumberOfColumns() == 0) {
      setNumberOfNodesAndColumns(numNodes, 1);
   }
   else {
      addColumns(1);
   }
   const int columnNumber = getNumberOfColumns() - 1;
   
   //
   // Set the name of the column to the name of the curvature file
   //
   setColumnName(columnNumber, FileUtilities::basename(filename));
   
   setModified();
   
   //
   // Read in the free surfer functional file
   //
   FreeSurferFunctionalFile fsff;
   fsff.setFileReadType(fileFormat);
   fsff.readFile(filename);

   //
   // Read in functional data
   //
   const int numItems = fsff.getNumberOfFunctionalItems();
   int nodeNumber;
   float value;
   for (int i = 0; i < numItems; i++) {
      fsff.getFunctionalData(i, nodeNumber, value);
      if (nodeNumber >= numNodes) {
         throw FileException(filename, "Functional data found for node with index "
                                 "larger than there are nodes in the surface.");
      }
      setValue(nodeNumber, columnNumber, value);
   }
   
   appendToFileComment(" Imported from ");
   appendToFileComment(FileUtilities::basename(filename));   
}

/**
 * compute and return a metric file that is a Z-map of "this" metric file
 * Z-map is (Xi - Mean)/Dev for all elements in each row.
 */
MetricFile* 
MetricFile::computeStatisticalZMap() const throw (FileException)
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   
   if ((numberOfNodes <= 0) ||
       (numberOfColumns <= 0)) {
      throw FileException("Input Metric File is isEmpty.");
   }
   if (numberOfColumns == 1) {
      throw FileException("Input Metric File has only one column.");
   }
   
   //
   // Create the new metric file by making a copy of the existing metric file
   //
   MetricFile* metricOut = new MetricFile(*this);
   
   //
   // Process each row in the metric file
   //
   float* values = new float[numberOfColumns];
   for (int i = 0; i < numberOfNodes; i++) {
      //
      // Convert to Z-score
      //
      getAllColumnValuesForNode(i, values);
      StatisticConvertToZScore zAlg;
      StatisticDataGroup sdg(values, 
                             numberOfColumns, 
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      try {
         zAlg.execute();
      }
      catch (StatisticException&) {
      }
      
      for (int m = 0; m < numberOfColumns; m++) {
         zAlg.convertToZScore(values[m]);
      }
      
      metricOut->setAllColumnValuesForNode(i, values);
   }
   delete[] values;
   
   //
   // Update the column names and color mapping
   //
   for (int j = 0; j < numberOfColumns; j++) {
      QString name("Z-map - ");
      name.append(getColumnName(j));
      metricOut->setColumnName(j, name);
      metricOut->setColumnColorMappingMinMax(j, -5.0, 5.0);
   }
   metricOut->appendToFileComment("\nZ-map of ");
   metricOut->appendToFileComment(FileUtilities::basename(getFileName()));
   metricOut->appendToFileComment("\n");
   
   
   return metricOut;
}

/**
 * compute and return a metric file that has each column fit to a
 * normal distribution.
 */
MetricFile* 
MetricFile::computeNormalization(const float mean,
                                 const float standardDeviation) const throw (FileException)
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();
   if ((numberOfNodes <= 0) ||
       (numberOfColumns <= 0)) {
      throw FileException("Input Metric File is isEmpty.");
   }
   
   //
   // Create the new metric file by making a copy of the existing metric file
   //
   MetricFile* metricOut = new MetricFile(*this);
   
   //
   // For column comment
   //
   std::ostringstream str;
   str << "Mean: "
       << mean
       << "  Std Dev: " 
       << standardDeviation;
       
   //
   // Process each column in the metric file
   //
   for (int j = 0; j < numberOfColumns; j++) {
      QString colName("Normalized - ");
      colName.append(metricOut->getColumnName(j));
      metricOut->remapColumnToNormalDistribution(j, j, colName, mean, standardDeviation);
   }
   
   metricOut->appendToFileComment("\nNormalization of ");
   metricOut->appendToFileComment(FileUtilities::basename(getFileName()));
   metricOut->appendToFileComment("\n");   
   
   return metricOut;
}

/**
 * compute and return a metric file that computes T-Values of "this" metric file
 * T-Value = (mean - constant) / (sample-dev / sqrt(N)).
 */
MetricFile* 
MetricFile::computeTValues(const float constant,
                           const TopologyFile* varianceSmoothingTopologyFile,
                           const int varianceSmoothingIterations,
                           const float varianceSmoothingStrength) const throw (FileException)
{
   //
   // Check input metric file
   //
   const int numNodes = getNumberOfNodes();
   if (numNodes <= 0) {
      throw FileException("Metric file contains no nodes.");
   }
   const int numCols = getNumberOfColumns();
   if (numCols < 2) {
      throw FileException("Metric file contains less than two columns.");
   }
   
   //
   // Create output metric file
   //
   MetricFile* mf = new MetricFile;
   mf->setNumberOfNodesAndColumns(numNodes, 1);
   mf->setColumnName(0, "T-Values");
   mf->setFileComment("T-Value from " + getFileName());
   
   
   const float squareRootN = std::sqrt(numCols);
   
   //
   // Compute mean and deviation for each node
   //
   float* mean = new float[numNodes];
   float* deviation = new float[numNodes];
   float* values = new float[numCols];
   for (int i = 0; i < numNodes; i++) {
      getAllColumnValuesForNode(i, values);
      StatisticDataGroup sdg(values,
                             numCols,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticMeanAndDeviation smad;
      smad.addDataGroup(&sdg);
      try {
         smad.execute();
      }
      catch (StatisticException&) {
      }
      mean[i] = smad.getMean();
      deviation[i] = smad.getPopulationSampleStandardDeviation();
   }
   delete[] values;
   values = NULL;
   
   //
   // Should variance smoothing be done ?
   //
   if (varianceSmoothingIterations > 0) {
      //
      // Do variance smoothing
      // Note: variance is the square of deviation
      //
      MetricFile m;
      m.setNumberOfNodesAndColumns(numNodes, 1);
      for (int i = 0; i < numNodes; i++) {
         const float variance =  deviation[i]*deviation[i];
         m.setValue(i, 0, variance);
      }
      m.smoothAverageNeighbors(0,
                               0,
                               "",
                               varianceSmoothingStrength,
                               varianceSmoothingIterations,
                               varianceSmoothingTopologyFile);
      for (int i = 0; i < numNodes; i++) {
         const float variance = m.getValue(i, 0);
         deviation[i] = std::sqrt(variance);
      }
   }
   
   //
   // Process each node
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Compute the T-Value
      //
      const float denom = deviation[i] / squareRootN;
      float tValue = mean[i] - constant;
      if (denom != 0.0) {
         tValue /= denom;
      }
      
      //
      // Store the T-Value
      //
      mf->setValue(i, 0, tValue);
   }
   
   delete[] mean;
   delete[] deviation;
   
   return mf;
}

/**
 * compute and return a metric file that contains permuted T-Values of "this" metric file
 * permutation is perfomred by genernating a random plus or minus one for each column
 * and multiplying the row by +/-1 and then computing the T-Value.
 */
MetricFile* 
MetricFile::computePermutedTValues(const float constant,
                                   const int iterations,
                                   const TopologyFile* topologyFile,
                                   const int varianceSmoothingIterations,
                                   const float varianceSmoothingStrength) const throw (FileException)
{
   //
   // Check input metric file
   //
   const int numNodes = getNumberOfNodes();
   if (numNodes <= 0) {
      throw FileException("Metric file contains no nodes.");
   }
   const int numCols = getNumberOfColumns();
   if (numCols < 2) {
      throw FileException("Metric file contains less than two columns.");
   }
   if (iterations <= 0) {
      throw FileException("Number of iteration is less than or equal to zero.");
   }
   
   //
   // Create output metric file
   //
   MetricFile* mf = new MetricFile;
   mf->setNumberOfNodesAndColumns(numNodes, iterations);
   mf->setColumnName(0, "Permuted T-Values");
   mf->setFileComment("Sign Flipped Permuted T-Values from " + getFileName());

   float* signFlips = new float[numCols];
   float* values    = new float[numCols];
   float* oneColumn = new float[numNodes];
   
   //
   // For the specified number of iterations
   //
   for (int iter = 0; iter < iterations; iter++) {
      //
      // Generate sign flips by randomly generating an array of plus and minus ones
      //
      for (int j = 0; j < numCols; j++) {
         signFlips[j] = 1.0;
      }
      StatisticDataGroup sdg(signFlips, numCols, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticPermutation permuteSigns(StatisticPermutation::PERMUTATION_METHOD_RANDOM_SIGN_FLIP);
      permuteSigns.addDataGroup(&sdg);
      try {
         permuteSigns.execute();
      }
      catch (StatisticException& e) {
         throw FileException(e);
      }
      const StatisticDataGroup* flipOutDataGroup = permuteSigns.getOutputData();
      for (int j = 0; j < numCols; j++) {
         signFlips[j] = flipOutDataGroup->getData(j);
      }

/*      
      //
      // generate the permutation sign flips
      //
      for (int j = 0; j < numCols; j++) {
         signFlips[j] = 1.0;
         if (MathUtilities::randomInteger(-1000, 1000) < 0) {
            signFlips[j] = -1.0;
         }
      }
*/
      
      //
      // Copy this metric file
      //
      MetricFile metricCopy(*this);
      
      //
      // Get the value for all nodes from the copy metric file and apply sign flips
      //
      for (int k = 0; k < numNodes; k++) {
         metricCopy.getAllColumnValuesForNode(k, values);
         for (int j = 0; j < numCols; j++) {
            values[j] *= signFlips[j];
         }
         metricCopy.setAllColumnValuesForNode(k, values);
      }
      
      //
      // Compute T-Values
      //
      MetricFile* tValuesMetric = metricCopy.computeTValues(constant,
                                                            topologyFile,
                                                            varianceSmoothingIterations,
                                                            varianceSmoothingStrength);
      
      //
      // Place the T-Values into the output permuted metric file
      //
      tValuesMetric->getColumnForAllNodes(0, oneColumn);
      mf->setColumnForAllNodes(iter, oneColumn);
      
      delete tValuesMetric;
   }
   
   delete[] signFlips;
   delete[] values;
   delete[] oneColumn;
   
   return mf;
}

/**
 * compute and return a Levene map for the rows of the metric files.
 */
MetricFile* 
MetricFile::computeStatisticalLeveneMap(const std::vector<MetricFile*>& inputFiles)
                                                                 throw (FileException)
{
   //
   // Need at least two files
   //
   const int numFiles = static_cast<int>(inputFiles.size());
   if (numFiles < 2) {
      throw FileException("A Levene Map requires at least two metric files");
   }
   
   //
   // Make sure all files have same number of nodes
   //
   const int numNodes = inputFiles[0]->getNumberOfNodes();
   for (int i = 0; i < numFiles; i++) {
      if (inputFiles[i]->getNumberOfNodes() != numNodes) {
         throw FileException("Not all files sent to Levene's test have the same number of nodes.");
      }
   }

   //
   // Verify that at least one file has three columns
   //
   bool haveThreeColumns = false;
   for (int i = 0; i < numFiles; i++) {
      if (inputFiles[i]->getNumberOfColumns() <= 0) {
         throw FileException("A file passed to Levene test has no columns (data).");
      }
      if (inputFiles[i]->getNumberOfColumns() >= 3) {
         haveThreeColumns = true;
         break;
      }
   }
   if (haveThreeColumns == false) {
      throw FileException("A Levene Map requires at least one file with three columns of data.");
   }
   
   //
   // Create the output file
   //
   int numCols = 0;
   const int fCol = numCols++;
   const int dofNumCol = numCols++;
   const int dofDenCol = numCols++;
   const int pCol = numCols++;
   MetricFile* outputMetricFile = new MetricFile;
   outputMetricFile->setNumberOfNodesAndColumns(numNodes, numCols);
   outputMetricFile->setColumnName(fCol, "Levene-F");
   outputMetricFile->setColumnName(dofNumCol, "DOF - numerator");
   outputMetricFile->setColumnName(dofDenCol, "DOF - denominator");
   outputMetricFile->setColumnName(pCol, "P-Value");
   
   //
   // Perform levene's test on each node
   //
   for (int i = 0; i < numNodes; i++) {
      StatisticLeveneVarianceEquality levene;
      StatisticDataGroup* dataGroups[numFiles];
      for (int j = 0; j < numFiles; j++) {
         const MetricFile* mf = inputFiles[j];
         std::vector<float>* metricData = new std::vector<float>;
         mf->getAllColumnValuesForNode(i, *metricData);
         dataGroups[j] = new StatisticDataGroup(metricData,
                                                StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP); 
         levene.addDataGroup(dataGroups[j], true); // 2nd arg true => take possession and delete when done
      }
      
      //
      // Peform the test
      //
      try {
         levene.execute();
         outputMetricFile->setValue(i, fCol, levene.getLeveneF());
         outputMetricFile->setValue(i, dofNumCol, levene.getDegreesOfFreedom1());
         outputMetricFile->setValue(i, dofDenCol, levene.getDegreesOfFreedom2());
         outputMetricFile->setValue(i, pCol, levene.getPValue());
      }
      catch (StatisticException& e) {
         delete outputMetricFile;
         throw FileException(e);
      }
   }
   
   //
   // Set the min/max values for the columns
   //
   for (int i = 0; i < numCols; i++) {
      float minVal, maxVal;
      outputMetricFile->getDataColumnMinMax(i, minVal, maxVal);
      outputMetricFile->setColumnColorMappingMinMax(i, minVal, maxVal);
   }
   
   return outputMetricFile;
}

/**
 * compute and return a T-map for the rows of two metric files.
 */
MetricFile* 
MetricFile::computeStatisticalTMap(const MetricFile* m1,
                                   const MetricFile* m2,
                                   const TopologyFile* varianceSmoothingTopologyFile,
                                   const int varianceSmoothingIterations,
                                   const float varianceSmoothingStrength,
                                   const bool poolTheVariance,
                                   const float falseDiscoveryRateQ,
                                   const bool doFalseDiscoveryRateFlag,
                                   const bool doDegreesOfFreedomFlag,
                                   const bool doPValuesFlagIn) throw (FileException)
{
   const bool doPValuesFlag = (doPValuesFlagIn || doFalseDiscoveryRateFlag);
   const int m1NumberOfNodes = m1->getNumberOfNodes();
   const int m1NumberOfColumns = m1->getNumberOfColumns();
   const int m2NumberOfNodes = m2->getNumberOfNodes();
   const int m2NumberOfColumns = m2->getNumberOfColumns();
   if (m1 == NULL) {
      throw FileException("First Input File is isEmpty (NULL).");
   }
   if (m2 == NULL) {
      throw FileException("Second Input File is isEmpty (NULL).");
   }
   if ((m1NumberOfNodes <= 0) ||
       (m1NumberOfColumns <= 0)) {
      throw FileException("First Input File is isEmpty.");
   }
   if ((m2NumberOfNodes <= 0) ||
       (m2NumberOfColumns <= 0)) {
      throw FileException("Second Input File is isEmpty.");
   }
   if (m1NumberOfNodes != m2NumberOfNodes) {
      throw FileException("Input files have different number of nodes.");
   }
   
   //
   // Initialize the output metric file
   //
   int numColumns = 0;
   const int meanGroup1Column = numColumns++;
   const int meanGroup2Column = numColumns++;
   const int tStatColumn      = numColumns++;
   int dofColumn = -1;
   if (doDegreesOfFreedomFlag) {
      dofColumn = numColumns;
      numColumns++;
   }
   int pValueColumn = -1;
   if (doPValuesFlag) {
      pValueColumn = numColumns;
      numColumns++;
   }
   
   //
   // If doing P-Value also compute false discovery rates
   // 
   int fdrC1Column = -1;
   int fdrCSumColumn = -1;
   if (doFalseDiscoveryRateFlag) {
      fdrC1Column = numColumns;
      numColumns++;
      fdrCSumColumn = numColumns;
      numColumns++;
   }

   MetricFile* outputMetric = new MetricFile;
   outputMetric->setNumberOfNodesAndColumns(m1NumberOfNodes, numColumns);
   std::ostringstream str;
   str << "T-map of the metric files "
       << FileUtilities::basename(m1->getFileName()).toAscii().constData()
       << " and " 
       << FileUtilities::basename(m2->getFileName()).toAscii().constData();
   outputMetric->setFileComment(str.str().c_str());
   
   //
   // Set the output metric column names
   //
   str.str("");
   str << "Mean - "
       << FileUtilities::basename(m1->getFileName()).toAscii().constData();
   outputMetric->setColumnName(meanGroup1Column, str.str().c_str());
   outputMetric->setColumnColorMappingMinMax(meanGroup1Column, -30.0, 10.0);
   str.str("");
   str << "Mean - "
       << FileUtilities::basename(m2->getFileName()).toAscii().constData();
   outputMetric->setColumnName(meanGroup2Column, str.str().c_str());
   outputMetric->setColumnColorMappingMinMax(meanGroup2Column, -30.0, 10.0);
   outputMetric->setColumnName(tStatColumn, "T-Map");
   outputMetric->setColumnColorMappingMinMax(tStatColumn, -5.0, 5.0);
   float dofMax = 0.0;
   if (dofColumn >= 0) {
      outputMetric->setColumnName(dofColumn, "Degrees of Freedom");
      outputMetric->setColumnColorMappingMinMax(dofColumn, 0.0, 20.0);
   }
   float pValueMax = 0.0;
   if (pValueColumn >= 0) {
      outputMetric->setColumnName(pValueColumn, "P-Value");
      outputMetric->setColumnColorMappingMinMax(pValueColumn, 0.0, 20.0);
   }
   if (fdrC1Column >= 0) {
      outputMetric->setColumnName(fdrC1Column, "P Thresh->FDR C=1");
      outputMetric->setColumnColorMappingMinMax(fdrC1Column, 0.0, 20.0);
   }
   if (fdrCSumColumn >= 0) {
      outputMetric->setColumnName(fdrCSumColumn, "P Thresh->FDR C-Sum");
      outputMetric->setColumnColorMappingMinMax(fdrCSumColumn, 0.0, 20.0);
   }
   
   //
   // Compute mean and variance for each node in first file
   //
   float* values1 = new float[m1NumberOfColumns];
   float* mean1 = new float[m1NumberOfNodes];
   float* var1 = new float[m1NumberOfNodes];
   for (int i = 0; i < m1NumberOfNodes; i++) {
      m1->getAllColumnValuesForNode(i, values1);
      StatisticDataGroup sdg(values1,
                             m1NumberOfColumns,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticMeanAndDeviation smad;
      smad.addDataGroup(&sdg);
      try {
         smad.execute();
      }
      catch (StatisticException&) {
      }
      mean1[i] = smad.getMean();
      float dev = smad.getPopulationSampleStandardDeviation();
      var1[i] = dev*dev;
   }
   delete[] values1;
   values1 = NULL;
   
   //
   // Compute mean and variance for each node in second file
   //
   float* values2 = new float[m2NumberOfColumns];
   float* mean2 = new float[m2NumberOfNodes];
   float* var2 = new float[m2NumberOfNodes];
   for (int i = 0; i < m2NumberOfNodes; i++) {
      m2->getAllColumnValuesForNode(i, values2);
      StatisticDataGroup sdg(values2,
                             m2NumberOfColumns,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticMeanAndDeviation smad;
      smad.addDataGroup(&sdg);
      try {
         smad.execute();
      }
      catch (StatisticException&) {
      }
      mean2[i] = smad.getMean();
      float dev = smad.getPopulationSampleStandardDeviation();
      var2[i] = dev*dev;
   }
   delete[] values2;
   values2 = NULL;
   
   //
   // Should variance smoothing be done ?
   //
   if (varianceSmoothingIterations > 0) {
      //
      // Variance smooth 1st file
      //
      MetricFile m;
      m.setNumberOfNodesAndColumns(m1NumberOfNodes, 1);
      for (int i = 0; i < m1NumberOfNodes; i++) {
         m.setValue(i, 0, var1[i]);
      }
      m.smoothAverageNeighbors(0,
                               0,
                               "",
                               varianceSmoothingStrength,
                               varianceSmoothingIterations,
                               varianceSmoothingTopologyFile);
      for (int i = 0; i < m1NumberOfNodes; i++) {
         var1[i] = m.getValue(i, 0);
      }
      
      //
      // Variance smooth 2nd file
      //
      m.setNumberOfNodesAndColumns(m2NumberOfNodes, 1);
      for (int i = 0; i < m2NumberOfNodes; i++) {
         m.setValue(i, 0, var2[i]);
      }
      m.smoothAverageNeighbors(0,
                               0,
                               "",
                               varianceSmoothingStrength,
                               varianceSmoothingIterations,
                               varianceSmoothingTopologyFile);
      for (int i = 0; i < m2NumberOfNodes; i++) {
         var2[i] = m.getValue(i, 0);
      }
   }
   
   //
   // Used for pooled variance computation
   //
   const float pooledOneOverSqrtN1N2 = std::sqrt((1.0 / m1NumberOfColumns) +
                                                 (1.0 / m2NumberOfColumns));
                                                   
   //
   // Compute the t-statistic for each node
   //
   for (int i = 0; i < m1NumberOfNodes; i++) {
      //
      // Denominator of t-statistic
      //
      float denom = 1.0;
      if (poolTheVariance) {
         //
         // Pooled variance
         //
         const float s2Pooled = ((m1NumberOfColumns - 1) * var1[i] + (m2NumberOfColumns - 1) * var2[i])
                              / (m1NumberOfColumns + m2NumberOfColumns - 2);
         denom = std::sqrt(s2Pooled) * pooledOneOverSqrtN1N2;
      }
      else {
         //
         // Unpooled variance
         //
         denom = std::sqrt(((var1[i]) / static_cast<float>(m1NumberOfColumns)) +
                           ((var2[i]) / static_cast<float>(m2NumberOfColumns)));
      }
      if (denom == 0.0) {
         denom = 1.0;
      }
      
      //
      // Return the t-statistic
      //
      const float tStat = (mean1[i] - mean2[i]) / denom;

      outputMetric->setValue(i, meanGroup1Column, mean1[i]);
      outputMetric->setValue(i, meanGroup2Column, mean2[i]);
      outputMetric->setValue(i, tStatColumn, tStat);
      
      //
      // Do degrees of freedom ?
      //
      if ((dofColumn >= 0) || (pValueColumn >= 0)) {
         const float num1 = m1NumberOfColumns;
         const float num2 = m2NumberOfColumns;
         const float num1M1 = num1 - 1.0;
         const float num2M1 = num2 - 1.0;
         float numerator = var1[i]/num1 + var2[i]/num2;
         numerator = numerator * numerator;
         float denom = (1.0 / num1M1) * (var1[i]/num1) * (var1[i]/num1)
                     + (1.0 / num2M1) * (var2[i]/num2) * (var2[i]/num2);
         float dof = 0.0;
         if (denom != 0.0) {
            dof = static_cast<int>(numerator / denom);
            dofMax = std::max(dofMax, dof);
         }
         
         if (dofColumn >= 0) {
            outputMetric->setValue(i, dofColumn, dof);
         }
      }      
   }

   //
   // Do P-Value computation
   //
   if ((dofColumn >= 0) && (pValueColumn >= 0)) {
      //
      // Gen mean and degrees of freedom
      //
      std::vector<float> tStats, dofs, pValues(m1NumberOfNodes, 0.0);
      outputMetric->getColumnForAllNodes(tStatColumn, tStats);
      outputMetric->getColumnForAllNodes(dofColumn, dofs);
      
      //
      // Create the data groups
      //
      StatisticDataGroup tStatGroup(&tStats, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDataGroup dofGroup(&dofs, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      
      //
      // Create and execute the algorithm
      //
      StatisticGeneratePValue genP(StatisticGeneratePValue::INPUT_STATISTIC_T_TWO_TALE);
      genP.addDataGroup(&tStatGroup);
      genP.addDataGroup(&dofGroup);
      try {
         genP.execute();
         
         //
         // Put the P-Values back into the metric file
         //
         const StatisticDataGroup* pValuesGroup = genP.getOutputDataGroupContainingPValues();
         outputMetric->setColumnForAllNodes(pValueColumn, pValuesGroup->getPointerToData());
         
         //
         // Set the maximum p value
         //
         for (int i = 0; i < m1NumberOfNodes; i++) {
            pValueMax = std::max(pValueMax, pValues[i]);
         }
      }
      catch (StatisticException& e) {
         const FileException fe(e);
         std::cout << "Generation of P-Values failed." << std::endl
                   << "   " << fe.whatQString().toAscii().constData() << std::endl;
      }      
   }
   
   delete[] mean1;
   mean1 = NULL;
   delete[] var1;
   var1  = NULL;
   delete[] mean2;
   mean2 = NULL;
   delete[] var2;
   var2  = NULL;
   
   //
   // Set Degree of Freedom Max Value
   //
   if (dofColumn >= 0) {
      outputMetric->setColumnColorMappingMinMax(dofColumn, 0.0, dofMax);
   }
   
   //
   // Set P-Value Max
   //
   if (pValueColumn >= 0) {
      outputMetric->setColumnColorMappingMinMax(pValueColumn, 0.0, pValueMax);
   }

   //
   // Do False Discovery Rate ?
   //
   if (fdrC1Column >= 0) {
      //
      // get the P-Values
      //
      std::vector<float> values;
      outputMetric->getColumnForAllNodes(pValueColumn, values);
      
      //
      // Determine the false discovery rate
      //
      StatisticFalseDiscoveryRate fdr(falseDiscoveryRateQ,
                                      StatisticFalseDiscoveryRate::C_CONSTANT_1);
      StatisticDataGroup sdg(&values, 
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      fdr.addDataGroup(&sdg);
      try {
         fdr.execute();
      }
      catch (StatisticException& e) {
         throw FileException(e);
      }
      
      //
      // Set values above "p-cutoff" to -1
      //
      const float pCutoff = fdr.getPCutoff();
      const int numValues = static_cast<int>(values.size());
      for (int m = 0; m < numValues; m++) {
         if (values[m] > pCutoff) {
            values[m] = -1.0;
         }
      }
      
      //
      // place output into metric file
      //
      outputMetric->setColumnForAllNodes(fdrC1Column, values);
      
      //
      // Update color mapping min/max
      //
      float minValue, maxValue;
      outputMetric->getDataColumnMinMax(fdrC1Column, minValue, maxValue);
      outputMetric->setColumnColorMappingMinMax(fdrC1Column, 0.0, maxValue);
   }

   //
   // Do False Discovery Rate ?
   //
   if (fdrCSumColumn >= 0) {
      //
      // get the P-Values
      //
      std::vector<float> values;
      outputMetric->getColumnForAllNodes(pValueColumn, values);
      
      //
      // Determine the false discovery rate
      //
      StatisticFalseDiscoveryRate fdr(falseDiscoveryRateQ,
                                      StatisticFalseDiscoveryRate::C_CONSTANT_SUMMATION);
      StatisticDataGroup sdg(&values, 
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      fdr.addDataGroup(&sdg);
      try {
         fdr.execute();
      }
      catch (StatisticException& e) {
         throw FileException(e);
      }
      
      //
      // Set values above "p-cutoff" to -1
      //
      const float pCutoff = fdr.getPCutoff();
      const int numValues = static_cast<int>(values.size());
      for (int m = 0; m < numValues; m++) {
         if (values[m] > pCutoff) {
            values[m] = -1.0;
         }
      }
      
      //
      // place output into metric file
      //
      outputMetric->setColumnForAllNodes(fdrCSumColumn, values);
      
      //
      // Update color mapping min/max
      //
      float minValue, maxValue;
      outputMetric->getDataColumnMinMax(fdrCSumColumn, minValue, maxValue);
      outputMetric->setColumnColorMappingMinMax(fdrCSumColumn, 0.0, maxValue);
   }
   
   return outputMetric;
}
                                               
/**
 * compute correlation coefficient map.
 */
MetricFile* 
MetricFile::computeMultipleCorrelationCoefficientMap(const MetricFile* dependentMetricFile,
                                                     const std::vector<MetricFile*>& independentMetricFiles) throw (FileException)
{
   //
   // Check inputs
   //
   if (dependentMetricFile == NULL) {
      throw FileException("Dependent metric file is NULL (invalid).");
   }
   const int numIndepMetricFiles = static_cast<int>(independentMetricFiles.size());
   if (numIndepMetricFiles <= 0) {
      throw FileException("No Independent metric files.");
   }   
   const int numNodes = dependentMetricFile->getNumberOfNodes();
   if (numNodes <= 0) {
      throw FileException("Dependent metric file has an invalid number of nodes.");
   }
   const int numCols = dependentMetricFile->getNumberOfColumns();
   if (numCols <= 0) {
      throw FileException("Dependent metric file has an invalid number of columns.");
   }
   for (int i = 0; i < numIndepMetricFiles; i++) {
      if (numNodes != independentMetricFiles[i]->getNumberOfNodes()) {
        const QString msg("Independent metric file "
                          + QString::number(i + 1)
                          + " has a different number of nodes than dependent metric file.");
        throw FileException(msg);
      } 
      else if (numCols != independentMetricFiles[i]->getNumberOfColumns()) {
        const QString msg("Independent metric file "
                          + QString::number(i + 1)
                          + " has a different number of columns than dependent metric file.");
        throw FileException(msg);
      }
   }
   
   //
   // Create output metric file
   //
   int colCtr = 0;
   const int r2ColumnNumber = colCtr++;
   const int rColumnNumber = colCtr++;
   const int fColumnNumber = colCtr++;
   const int pColumnNumber = colCtr++;
   const int dofNumeratorColumnNumber = colCtr++;
   const int dofDenominatorColumnNumber = colCtr++;
   const int numOutputColumns = colCtr;
   MetricFile* metricOut = new MetricFile(numNodes, numOutputColumns);
   if (r2ColumnNumber >= 0) {
      metricOut->setColumnName(r2ColumnNumber, "r2 - Coefficient of Multiple Determination");
   }
   if (rColumnNumber >= 0) {
      metricOut->setColumnName(rColumnNumber, "r - Correlation Coefficient");
   }
   if (fColumnNumber >= 0) {
      metricOut->setColumnName(fColumnNumber, "F-Value");
   }
   if (pColumnNumber >= 0) {
      metricOut->setColumnName(pColumnNumber, "P-Value");
   }
   if (dofNumeratorColumnNumber >= 0) {
      metricOut->setColumnName(dofNumeratorColumnNumber, "DOF (numerator)- Degrees of Freedom");
   }
   if (dofDenominatorColumnNumber >= 0) {
      metricOut->setColumnName(dofDenominatorColumnNumber, "DOF (denomenator)- Degrees of Freedom");
   }
   
   //
   // Compute correlation coefficients
   //
   float* dependentData = new float[numCols];
   std::vector<float*> independentData(numIndepMetricFiles);
   for (int m = 0; m < numIndepMetricFiles; m++) {
      independentData[m] = new float[numCols];
   }
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         dependentData[j] = dependentMetricFile->getValue(i, j);
      }
      for (int m = 0; m < numIndepMetricFiles; m++) {
         float* d = independentData[m];
         for (int j = 0; j < numCols; j++) {
            d[j] = independentMetricFiles[m]->getValue(i, j);
         }
      }

      //
      // Create multiple regression object and load data
      //
      StatisticMultipleRegression smr;      
      smr.setDependentDataArray(dependentData, numCols);
      smr.setNumberOfIndependentDataGroups(numIndepMetricFiles);
      for (int m = 0; m < numIndepMetricFiles; m++) {
         smr.setIndependentDataArray(m, independentData[m], numCols);
      }

      //
      // Execute multiple regression
      //
      try {
         smr.execute();
      }
      catch (StatisticException& e) {
         throw FileException(e);
      }
      float SSTO,
            SSE,
            SSR,
            MSR,
            MSE,
            F,
            pValue,
            R2;
      int regressionDOF,
          errorDOF,
          totalDOF;
      smr.getAnovaParameters(SSTO,
                             SSE,
                             SSR,
                             MSR,
                             MSE,
                             F,
                             pValue,
                             R2,
                             regressionDOF,
                             errorDOF,
                             totalDOF);
                            
      if (r2ColumnNumber >= 0) {
         metricOut->setValue(i, r2ColumnNumber, R2);
      }
      if (rColumnNumber >= 0) {
         metricOut->setValue(i, rColumnNumber, std::sqrt(R2));
      }
      if (fColumnNumber >= 0) {
         metricOut->setValue(i, fColumnNumber, F);
      }
      if (pColumnNumber >= 0) {
         metricOut->setValue(i, pColumnNumber, pValue);
      }
      if (dofNumeratorColumnNumber >= 0) {
         metricOut->setValue(i, dofNumeratorColumnNumber, regressionDOF);
      }
      if (dofDenominatorColumnNumber >= 0) {
         metricOut->setValue(i, dofDenominatorColumnNumber, errorDOF);
      }
   }
   delete[] dependentData;
   dependentData = NULL;
   for (int m = 0;  m < numIndepMetricFiles; m++) {
      delete[] independentData[m];
      independentData[m] = NULL;
   }
   
   return metricOut;
}
                                                   
/**
 * compute correlation coefficient map.
 */
MetricFile* 
MetricFile::computeCorrelationCoefficientMap(const MetricFile* m1,
                                             const MetricFile* m2) throw (FileException)
{
   //
   // Check inputs
   //
   if (m1 == NULL) {
      throw FileException("First metric file is NULL (invalid).");
   }
   if (m2 == NULL) {
      throw FileException("Second metric file is NULL (invalid).");
   }   
   const int numNodes = m1->getNumberOfNodes();
   if (numNodes <= 0) {
      throw FileException("First metric file has an invalid number of nodes.");
   }
   if (numNodes != m2->getNumberOfNodes()) {
      throw FileException("Input metric files have a different number of nodes.");
   }
   
   const int numCols = m1->getNumberOfColumns();
   if (numCols <= 0) {
      throw FileException("First metric file has an invalid number of columns.");
   }
   if (numCols != m2->getNumberOfColumns()) {
      throw FileException("Input metric files have a different number of columns.");
   }
   
   //
   // Create output metric file
   //
   int colCtr = 0;
   const int rColumnNumber = colCtr++;
   const int tColumnNumber = colCtr++;
   const int pColumnNumber = colCtr++;
   const int dofColumnNumber = colCtr++;
   const int numOutputColumns = colCtr;
   MetricFile* metricOut = new MetricFile(numNodes, numOutputColumns);
   if (rColumnNumber >= 0) {
      metricOut->setColumnName(rColumnNumber, "r - Correlation Coefficient");
   }
   if (tColumnNumber >= 0) {
      metricOut->setColumnName(tColumnNumber, "T-Value");
   }
   if (pColumnNumber >= 0) {
      metricOut->setColumnName(pColumnNumber, "P-Value");
   }
   if (dofColumnNumber >= 0) {
      metricOut->setColumnName(dofColumnNumber, "DOF - Degrees of Freedom");
   }
   
   //
   // Compute correlation coefficients
   //
   float* data1 = new float[numCols];
   float* data2 = new float[numCols];
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         data1[j] = m1->getValue(i, j);
         data2[j] = m2->getValue(i, j);
      }   
      StatisticDataGroup sdg1(data1, numCols, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDataGroup sdg2(data2, numCols, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      
      StatisticCorrelationCoefficient scc;
      scc.addDataGroup(&sdg1);
      scc.addDataGroup(&sdg2);
      try {
         scc.execute();
      }
      catch (StatisticException& e) {
         throw FileException(e);
      }
      
      if (rColumnNumber >= 0) {
         metricOut->setValue(i, rColumnNumber, scc.getCorrelationCoefficientR());
      }
      if (tColumnNumber >= 0) {
         metricOut->setValue(i, tColumnNumber, scc.getTValue());
      }
      if (pColumnNumber >= 0) {
         metricOut->setValue(i, pColumnNumber, scc.getPValue());
      }
      if (dofColumnNumber >= 0) {
         metricOut->setValue(i, dofColumnNumber, scc.getDegreesOfFreedom());
      }
   }
   delete[] data1;
   data1 = NULL;
   delete[] data2;
   data2 = NULL;
   
   return metricOut;
}
                                                   
/**
 * subract the average of both files form each file (output files are replaced)
 */
void 
MetricFile::subtractMeanFromRowElements(const MetricFile* inputFile1,
                                        const MetricFile* inputFile2,
                                        MetricFile* outputFile1,
                                        MetricFile* outputFile2)  throw (FileException)
{
   if (inputFile1 == NULL) {
      throw FileException("Input File 1 is invalid (NULL).");
   }
   if (inputFile2 == NULL) {
      throw FileException("Input File 2 is invalid (NULL).");
   }
   if (outputFile1 == NULL) {
      throw FileException("Output File 1 is invalid (NULL).");
   }
   if (outputFile2 == NULL) {
      throw FileException("Output File 2 is invalid (NULL).");
   }
   
   if ((inputFile1->getNumberOfNodes() <= 0) ||
       (inputFile1->getNumberOfColumns() <= 0)) {
      throw FileException("Input File 1 is isEmpty.");
   }
   if ((inputFile2->getNumberOfNodes() <= 0) ||
       (inputFile2->getNumberOfColumns() <= 0)) {
      throw FileException("Input File 2 is isEmpty.");
   }
   if (inputFile1->getNumberOfNodes() != inputFile2->getNumberOfNodes()) {
      throw FileException("Input Files have a different number of nodes.");
   }
   
   const int numNodes = inputFile1->getNumberOfNodes();
   const int col1 = inputFile1->getNumberOfColumns();
   const int col2 = inputFile2->getNumberOfColumns();
   const float totalColumns = col1 + col2;
   
   //
   // Copy the metric files
   //
   *outputFile1 = *inputFile1;
   *outputFile2 = *inputFile2;
   
   //
   // Process each row in the metric file
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Determine mean
      //
      float mean = 0.0;
      for (int j = 0; j < col1; j++) {
         mean += outputFile1->getValue(i, j);
      }
      for (int j = 0; j < col2; j++) {
         mean += outputFile2->getValue(i, j);
      }
      mean /= totalColumns;
      
      //
      // Subtract mean from each element
      //
      for (int j = 0; j < col1; j++) {
         outputFile1->setValue(i, j, outputFile1->getValue(i, j) - mean);
      }
      for (int j = 0; j < col2; j++) {
         outputFile2->setValue(i, j, outputFile2->getValue(i, j) - mean);
      }
   }
   
   std::ostringstream commentStr;
   commentStr << "\nSubtracted group average of "
        << FileUtilities::basename(inputFile1->getFileName()).toAscii().constData()
        << "\nand"
        << FileUtilities::basename(inputFile2->getFileName()).toAscii().constData()
        << ".\n";
        
   //
   // Update the column names and color mapping
   //
   for (int j = 0; j < col1; j++) {
      QString name("Avg-subtracted - ");
      name.append(outputFile1->getColumnName(j));
      outputFile1->setColumnName(j, name);
      outputFile1->setColumnColorMappingMinMax(j, -5.0, 5.0);
      
      QString s(commentStr.str().c_str());
      s.append(outputFile1->getColumnComment(j));
      outputFile1->setColumnComment(j, s);
   }
   for (int j = 0; j < col2; j++) {
      QString name("Avg-subtracted - ");
      name.append(outputFile2->getColumnName(j));
      outputFile2->setColumnName(j, name);
      outputFile2->setColumnColorMappingMinMax(j, -5.0, 5.0);
      
      QString s(commentStr.str().c_str());
      s.append(outputFile2->getColumnComment(j));
      outputFile2->setColumnComment(j, s);
   }
   
   outputFile1->appendToFileComment(commentStr.str().c_str());
   outputFile2->appendToFileComment(commentStr.str().c_str());
}

/**
 * compute shuffled cross correlation maps.
 */
MetricFile* 
MetricFile::computeShuffledCrossCorrelationsMap(const int numberOfRepetitions) const throw (FileException)
{
   if (empty()) {
      throw FileException("Metric file contains no data.");
   }
   if (getNumberOfColumns() < 2) {
      throw FileException("Mmetric file must have at least two columns.");
   }
   if (numberOfRepetitions <= 0) {
      throw FileException("Number of repetitions is less than or equal to zero.");
   }
   
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();

   //
   // Create a new metric file with the number of columns beign the number of repetitions
   //
   MetricFile* metricOut = new MetricFile;
   metricOut->setNumberOfNodesAndColumns(numberOfNodes, numberOfRepetitions);
   metricOut->appendToFileComment("Shuffled cross-correlation maps of: ");
   metricOut->appendToFileComment(getFileComment());
   
   //
   // Each column is the multiplication of two randomly selected columns of the input metric
   //
   for (int j = 0; j < numberOfRepetitions; j++) {
      //
      // allow other events to process
      //
      AbstractFile::allowEventsToProcess();

      //
      // Randomly select the columns
      //
      const int col1 = StatisticRandomNumber::randomInteger(0, numberOfColumns - 1);
      int col2 = col1;
      while (col1 == col2) {
         col2 = StatisticRandomNumber::randomInteger(0, numberOfColumns - 1);
      }
      
      //
      // Multiply the two columns
      //
      for (int i = 0; i < numberOfNodes; i++) {
         metricOut->setValue(i, j, getValue(i, col1) * getValue(i, col2));
      }
      
      //
      // set the column comment
      //
      std::ostringstream str;
      str << "Column "
          << getColumnName(col1).toAscii().constData()
          << " * "
          << " Column " 
          << getColumnName(col2).toAscii().constData();
      metricOut->setColumnName(j, str.str().c_str());
      str << " from "
          << FileUtilities::basename(getFileName()).toAscii().constData();
      metricOut->setColumnComment(j, str.str().c_str());
      
      //
      // Set color mapping
      //
      metricOut->setColumnColorMappingMinMax(j, -16.0, 16.0);
   }
   
   return metricOut;
}
      
/**
 * shuffle the values amongst group of input metric files and place
 * into the output files which must already be allocated.
 */
void 
MetricFile::shuffle(const std::vector<MetricFile*>& metricFilesIn,
                    std::vector<MetricFile*>& metricFilesOut) throw (FileException)
{
   //
   // Check for input files
   //
   const int numFiles = static_cast<int>(metricFilesIn.size());
   if (numFiles <= 0) {
      throw FileException("No input metric files to shuffle.");
   }
   if (static_cast<int>(metricFilesOut.size()) != numFiles) {
      throw FileException("Number of input metric files different than number of output metric files.");
   }
   
   //
   // Check for valid files and count total columns
   //
   const int numNodes = metricFilesIn[0]->getNumberOfNodes();
   int totalColumns = 0;
   for (int i = 0; i < numFiles; i++) {
      const MetricFile* mf = metricFilesIn[i];
      if (mf->empty()) {
         const QString msg("Metric file "
                           + FileUtilities::basename(mf->getFileName())
                           + " contains no data.");
         throw FileException(msg);
      }
      if (mf->getNumberOfNodes() != numNodes) {
         const QString msg("Metric files "
                           + FileUtilities::basename(mf->getFileName())
                           + " and "
                           + FileUtilities::basename(metricFilesIn[0]->getFileName())
                           + " contains a different number of nodes.");
         throw FileException(msg);
      }
      
      if ((mf->getNumberOfNodes() != metricFilesOut[i]->getNumberOfNodes()) ||
          (mf->getNumberOfColumns() != metricFilesOut[i]->getNumberOfColumns())) {
         const QString msg("Input Metric File " + QString::number(i)
                           + " does not match its corresponding output metric file"
                           + " in number of nodes and/or columns.");
         throw FileException(msg);
      }
      
      totalColumns += mf->getNumberOfColumns();
   }

   //
   // Create shuffled columns' indices and randomly shuffle them
   //
   std::vector<float> columnsShuffledFloat(totalColumns);
   for (int i = 0; i < totalColumns; i++) {
      columnsShuffledFloat[i] = i;
   }
   StatisticDataGroup sdg(&columnsShuffledFloat, 
                          StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticPermutation perm(StatisticPermutation::PERMUTATION_METHOD_RANDOM_ORDER);
   perm.addDataGroup(&sdg);
   try {
      perm.execute();
   }
   catch (StatisticException& e) {
      throw FileException(e);
   }
   const StatisticDataGroup* permOut = perm.getOutputData();
   if (permOut->getNumberOfData() != totalColumns) {
      throw FileException("Program error: StatisticPermutation return wrong number of values.");
   }
   int* columnsShuffled = new int[totalColumns];
   for (int i = 0; i < totalColumns; i++) {
      columnsShuffled[i] = static_cast<int>(permOut->getData(i));
   }
   
   //
   // Loop through the nodes
   //
   float* values = new float[totalColumns];
   for (int i = 0; i < numNodes; i++) {
      //
      // Place all of the values for the node in a single array
      //
      int indx = 0;
      for (int j = 0; j < numFiles; j++) {
         const MetricFile* mf = metricFilesIn[j];
         const int numCols = mf->getNumberOfColumns();
         for (int k = 0; k < numCols; k++) {
            values[indx] = mf->getValue(i, k);
            indx++;
         }
      }
      
      //
      // add the metric values using the shuffled indices
      //
      indx = 0;
      for (int j = 0; j < numFiles; j++) {
         MetricFile* mf = metricFilesOut[j];
         const int numCols = mf->getNumberOfColumns();
         for (int k = 0; k < numCols; k++) {
            const int shuffIndex = columnsShuffled[indx];
            mf->setValue(i, k, values[shuffIndex]);
            indx++;
         }
      }
   }
   
   //
   // Free memory
   //
   delete[] columnsShuffled;
   delete[] values;
}

/**
 * compute T-map on shuffled columns split into two groups.
 * If the number in group 1 is negative or zero, the columns are split
 * into two groups of the same size.
 */
MetricFile* 
MetricFile::computeStatisticalShuffledTMap(const int numberOfRepetitions,
                                           const int numberInGroup1,
                                           const TopologyFile* topologyFile,
                                           const int varianceSmoothingIterations,
                                           const float varianceSmoothingStrength,
                                           const bool poolTheVariance) const throw (FileException)
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();

   if ((numberOfNodes <= 0) ||
       (numberOfColumns <= 0)) {
      throw FileException("Input Metric File is isEmpty.");
   }
   if (numberOfColumns == 1) {
      throw FileException("Input Metric File has only one column.");
   }
   if (numberInGroup1 >= numberOfColumns) {
      throw FileException("Size of first group is greater than or equal to the number of colunms.");
   }

   //
   // Create the output metric file that will contain the tmaps
   //
   MetricFile* metricOut = new MetricFile;
   metricOut->setNumberOfNodesAndColumns(numberOfNodes, numberOfRepetitions);
   metricOut->appendToFileComment("Shuffled Columns T-Map from ");
   metricOut->appendToFileComment(FileUtilities::basename(getFileName()));
   
   //
   // Create metric files for the two groups
   //
   int halfIndex = numberOfColumns / 2;
   if (numberInGroup1 > 0) {
      halfIndex = numberInGroup1;
   }
   const int numColumnsFile1 = halfIndex;
   const int numColumnsFile2 = numberOfColumns - halfIndex;
   MetricFile file1, file2;
   file1.setNumberOfNodesAndColumns(numberOfNodes, numColumnsFile1);
   file2.setNumberOfNodesAndColumns(numberOfNodes, numColumnsFile2);
   
   //
   // Do for the number of repetitions
   //
   for (int nr = 0; nr < numberOfRepetitions; nr++) {
      //
      // allow other events to process
      //
      AbstractFile::allowEventsToProcess();

      //
      // Create shuffled columns' indices
      //
      std::vector<float> columnsShuffledFloat(numberOfColumns);
      for (int i = 0; i < numberOfColumns; i++) {
         columnsShuffledFloat[i] = i;
      }
/*
      RandomNumberOp randOp;  // used to rand() is called
      std::random_shuffle(columnsShuffled.begin(), columnsShuffled.end(), randOp);
*/
      //
      // Randomly shuffle the columns
      //      
      StatisticDataGroup sdg(&columnsShuffledFloat, 
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticPermutation perm(StatisticPermutation::PERMUTATION_METHOD_RANDOM_ORDER);
      perm.addDataGroup(&sdg);
      try {
         perm.execute();
      }
      catch (StatisticException& e) {
         throw FileException(e);
      }
      const StatisticDataGroup* permOut = perm.getOutputData();
      if (permOut->getNumberOfData() != numberOfColumns) {
         throw FileException("Program error: StatisticPermutation return wrong number of values.");
      }
      std::vector<int> columnsShuffled(numberOfColumns);
      for (int i = 0; i < numberOfColumns; i++) {
         columnsShuffled[i] = static_cast<int>(permOut->getData(i));
      }

      //
      // Set the values for each of the two split metric files
      //
      for (int j = 0; j < numColumnsFile1; j++) {
         const int indx = columnsShuffled[j];
         for (int i = 0; i < numberOfNodes; i++) {
            file1.setValue(i, j, getValue(i, indx));
         }
      }

      //
      // Set the values for each of the two split metric files
      //
      for (int j = 0; j < numColumnsFile2; j++) {
         const int indx = columnsShuffled[j + halfIndex];
         for (int i = 0; i < numberOfNodes; i++) {
            file2.setValue(i, j, getValue(i, indx));
         }
      }
      
      //
      // Compute the T-Map for the two files
      //
      MetricFile* tMapMetricFile = computeStatisticalTMap(&file1, &file2, 
                                                          topologyFile,
                                                          varianceSmoothingIterations,
                                                          varianceSmoothingStrength,
                                                          poolTheVariance,
                                                          0.05,
                                                          false,
                                                          false, 
                                                          false);
      const int tMapColumn = tMapMetricFile->getColumnWithName("T-Map");
      if (tMapColumn < 0) {
         throw FileException("Unable to find columns named \"T-Map\"");
      }
      
      //
      // Add T-map to output metric file
      //
      for (int i = 0; i < numberOfNodes; i++) {
         metricOut->setValue(i, nr, tMapMetricFile->getValue(i, tMapColumn));
      }
      
      //
      // Update column name and comment
      //
      std::ostringstream str1, str1Com, str2, str2Com;
      for (int j = 0; j < numberOfColumns; j++) {
         if (j < halfIndex) {
            str1 << columnsShuffled[j] << " ";
            str1Com << getColumnName(columnsShuffled[j]).toAscii().constData() << " ";
         }
         else {
            str2 << columnsShuffled[j] << " ";
            str2Com << getColumnName(columnsShuffled[j]).toAscii().constData() << " ";
         }
      }
      std::ostringstream str;
      str << "T-Test on "
          << str1.str()
          << " versus "
          << str2.str();
      metricOut->setColumnName(nr, str.str().c_str());
      std::ostringstream strCom;
      strCom << "T-Test on "
             << str1Com.str()
             << " versus "
             << str2Com.str();
      metricOut->setColumnComment(nr, strCom.str().c_str());
      
      //
      // Set column color mapping
      //
      metricOut->setColumnColorMappingMinMax(nr, -5.0, 5.0);
      
      delete tMapMetricFile;
   }
   
   return metricOut;
}
      
/**
 * remap values in a column so that they fit a normal distribution
 * with the median at the normal distribution's mean.
 */
void 
MetricFile::remapColumnToNormalDistribution(const int inputColumnNumber,
                                            const int outputColumnNumberIn,
                                            const QString& outputColumnName,
                                            const float normalDistributionMean,
                                            const float normalDistributionDeviation) throw (FileException)
{
   const int numberOfNodes = getNumberOfNodes();
   const int numberOfColumns = getNumberOfColumns();

   //
   // Check for valid input column
   //
   if ((numberOfColumns <= 0) || (numberOfNodes <= 0)) {
      throw FileException("There are no nodes in the metric file.");
   }
   if ((inputColumnNumber < 0) || (inputColumnNumber >= numberOfColumns)) {
      throw FileException("Input column index is invalid.");
   }
   
   //
   // Create a new column if needed.
   //
   int outputColumnNumber = outputColumnNumberIn;
   if ((outputColumnNumber < 0) || (outputColumnNumber >= numberOfColumns)){
      addColumns(1);
      outputColumnNumber = getNumberOfDataArrays() - 1;
   }
   setColumnName(outputColumnNumber, outputColumnName);

   //
   // Get the input values
   //   
   std::vector<float> values;
   getColumnForAllNodes(inputColumnNumber, values);

   //
   // Normalize the values
   //
   StatisticNormalizeDistribution normDist(normalDistributionMean,
                                           normalDistributionDeviation);
   StatisticDataGroup sdg(&values, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   normDist.addDataGroup(&sdg);
   try {
      normDist.execute();
      
      const StatisticDataGroup* sdgNormalized = normDist.getOutputDataGroupContainingNormalizeValues();
      setColumnForAllNodes(outputColumnNumber, sdgNormalized->getPointerToData());
   }
   catch (StatisticException& e) {
      throw FileException(e);
   }
   
   //
   // Set color mapping
   //
   setColumnColorMappingMinMax(outputColumnNumber, 
                               -normalDistributionDeviation * 4.0,
                               normalDistributionDeviation  * 4.0);
                               
   //
   // create the new column comment
   //
   std::ostringstream str;
   str << "Mean = " << normalDistributionMean
       << "  Std Dev = " << normalDistributionDeviation
       << "\n"
       << getColumnComment(inputColumnNumber).toAscii().constData();
   setColumnComment(outputColumnNumber, str.str().c_str());
}
                                           
/**
 * copy a column of values to another column.
 */
void 
MetricFile::copyColumn(const int inputColumnNumber, const int outputColumnNumber)
{
   GiftiDataArray* ndaIn  = dataArrays[inputColumnNumber];
   GiftiDataArray* ndaOut = dataArrays[outputColumnNumber];
   
   if (ndaIn->getNumberOfRows() == ndaOut->getNumberOfRows()) {
      float* dataIn  = ndaIn->getDataPointerFloat();
      float* dataOut = ndaOut->getDataPointerFloat();
      const int num = ndaIn->getNumberOfRows();
      for (int i = 0; i < num; i++) {
         dataOut[i] = dataIn[i];
      }
   }
}
      
/**
 * extract columns (indices starting at zero) from a metric file and store in another metric file.
 */
void 
MetricFile::extractColumnsFromFile(const QString& inputFileName,
                                   const QString& outputFileName,
                                   const std::vector<int>& columnsToExtract) throw (FileException)
{
   //
   // Check inputs
   //
   if (inputFileName.isEmpty()) {
      throw FileException("input file name is empty.");
   }
   if (outputFileName.isEmpty()) {
      throw FileException("output file name is empty.");
   }
   if (columnsToExtract.empty()) {
      throw FileException("No column are specified for extraction.");
   }
   
   //
   // Read input file
   //
   MetricFile inputFile;
   inputFile.readFile(inputFileName);
   
   //
   // Check input metric file
   //
   const int numNodes = inputFile.getNumberOfNodes();
   const int numCols  = inputFile.getNumberOfColumns();
   if ((numNodes <= 0) || (numCols <= 0)) {
      throw FileException("Input file contains no data.");
   }
   
   //
   // Validate columns for extraction
   //
   const int numColumnsToExtract = static_cast<int>(columnsToExtract.size());
   for (int i = 0; i < numColumnsToExtract; i++) {
      const int col = columnsToExtract[i];
      if ((col < 0) || (col >= numCols)) {
         QString msg("Trying to extract invalid column=" + QString::number(col) + ".\n"
                     "Column numbers should range 0 to " + QString::number(numCols - 1) + ".");
         throw FileException(msg);
      }
   }
   
   //
   // Create the output file
   //
   MetricFile outputFile;

   //
   // Use append() method to copy the data
   //   
   std::vector<int> indexDestination(numCols, APPEND_ARRAY_DO_NOT_LOAD);
   for (int i = 0; i < numColumnsToExtract; i++) {
      indexDestination[columnsToExtract[i]] = APPEND_COLUMN_NEW;
   }
   outputFile.append(inputFile,
                     indexDestination,
                     FILE_COMMENT_MODE_LEAVE_AS_IS);
   outputFile.setFileComment("Data extracted from " + inputFileName);
   
   //
   // Write the output file
   //
   outputFile.writeFile(outputFileName);
}

/**
 * concatenate columns from a group of metric files (all must have same number of nodes).
 */
void 
MetricFile::concatenateColumnsFromFiles(const QString& outputFileName,
                                        const std::vector<QString>& inputFileNames,
                                        const CONCATENATE_COLUMNS_MODE mode,
                                        const QString& columnName) throw (FileException)
{
   //
   // check input files
   //
   if (outputFileName.isEmpty()) {
      throw FileException("No output file name specified.");
   }
   const int num = static_cast<int>(inputFileNames.size());
   if (num == 0) {
      throw FileException("No input file names specified.");
   }
   for (int i = 0; i < num; i++) {
      if (inputFileNames[i].isEmpty()) {
         throw FileException("One or more input file names is not specified.");
      }
   }
   
   //
   // Check mode specified inputs
   //
   switch (mode) {
      case CONCATENATE_COLUMNS_MODE_ALL:
         break;
      case CONCATENATE_COLUMNS_MODE_NAME_EXACT:
         if (columnName.isEmpty()) {
            throw FileException("Column name not specified.");
         }
         break;
   }
   
   //
   // the output file
   //
   MetricFile outputFile;
   
   //
   // Loop through the input files
   //
   for (int i = 0; i < num; i++) {
      //
      // Name of metric file
      //
      const QString metricName = inputFileNames[i];
      
      //
      // read metric file
      //
      MetricFile m;
      m.readFile(metricName);
      
      //
      // Check append mode
      //
      switch (mode) {
         case CONCATENATE_COLUMNS_MODE_ALL:
            for (int j = 0; j < m.getNumberOfColumns(); j++) {
               //
               // Append name of file to column name
               //
               QString colName = m.getColumnName(j);
               colName.append(" - ");
               colName.append(FileUtilities::basename(metricName));  
               m.setColumnName(j, colName);             
            }
            
            //
            // Append the entire file
            //
            outputFile.append(m);
            break;
         case CONCATENATE_COLUMNS_MODE_NAME_EXACT:
            {
               //
               // Keep track of which column to append
               //
               const int numCols = m.getNumberOfColumns();
               std::vector<int> destination(numCols, APPEND_COLUMN_DO_NOT_LOAD); // initial values

               //
               // Find the columns with the specified name
               //
               bool foundColumn = false;
               for (int j = 0; j < numCols; j++) {
                  if (m.getColumnName(j) == columnName) {
                     //
                     // column matches
                     //
                     destination[j] = APPEND_COLUMN_NEW;
                     foundColumn = true;

                     //
                     // Append name of file to column name
                     //
                     QString colName = m.getColumnName(j);
                     colName.append(" - ");
                     colName.append(FileUtilities::basename(metricName));  
                     m.setColumnName(j, colName);             
                  }
               }
               
               //
               // Make sure columns were found
               //
               if (foundColumn == false) {
                  QString msg(FileUtilities::basename(metricName));
                  msg.append(" does not contain any columns named ");
                  msg.append(columnName);
                  throw FileException(msg);
               }
               
               //
               // append the specified column only
               //
               outputFile.append(m, destination, FILE_COMMENT_MODE_LEAVE_AS_IS);
            }
            break;
      }
   }
   
   //
   // Write the output file 
   //
   outputFile.writeFile(outputFileName);
}
                                              
/**
 * add a column that is distance between nodes in two coordinate files (c1 - c2)
 * NOTE: x/y/z diff columns must already exist, they will not be created
 */
void 
MetricFile::addColumnOfCoordinateDifference(const CoordinateFile* c1,
                                            const CoordinateFile* c2,
                                            const TopologyFile* topologyFile,
                                            const int columnIn,
                                            const QString& columnName,
                                            const QString& columnComment,
                                            const int xDiffColumn,
                                            const int yDiffColumn,
                                            const int zDiffColumn) throw (FileException)
{
   //
   // Check inputs
   //
   if (c1 == NULL) {
      throw FileException("First coordinate file invalid");
   }
   if (c2 == NULL) {
      throw FileException("Second coordinate file invalid");
   }
   if (topologyFile == NULL) {
      throw FileException("Topology file invalid");
   }
   if (columnName.isEmpty()) {
      throw FileException("Column name is empty.");
   }
   
   //
   // Check number of nodes
   //
   const int numNodes = c1->getNumberOfCoordinates();
   if (numNodes <= 0) {
      throw FileException("First coordinate file contains no coordinates");
   }
   if (numNodes != c2->getNumberOfCoordinates()) {
      throw FileException("Coordinate files have a different number of nodes");
   }
   const int metricNodes = getNumberOfNodes();
   if (metricNodes > 0) {
      if (metricNodes != numNodes) {
         throw FileException("Coordinate and metric files have different number of nodes.");
      }
   }

   //
   // create/find desired metric column
   //   
   int column = columnIn;
   if (metricNodes == 0) {
      setNumberOfNodesAndColumns(numNodes, 1);
      column = 0;
   }
   else {
      if ((column < 0) || (column >= getNumberOfColumns())) {
         addColumns(1);
         column = getNumberOfColumns() - 1;
      }
   }
   
   //
   // Set the column name and comment
   //
   setColumnName(column, columnName);
   setColumnComment(column, columnComment);
   
   //
   // X/Y/Z-Diff column names
   //
   if (xDiffColumn >= 0) {
      setColumnName(xDiffColumn, "dX");
   }
   if (yDiffColumn >= 0) {
      setColumnName(yDiffColumn, "dY");
   }
   if (zDiffColumn >= 0) {
      setColumnName(zDiffColumn, "dZ");
   }

   //
   // Create a topoogy helper
   //
   const TopologyHelper* th = topologyFile->getTopologyHelper(false, true, false);
   
   //
   // Set the metric data with distance between coordinates
   //
   float maxDist = 0.0;
   for (int i = 0; i < numNodes; i++) {
      float dist = 0;
      if (th->getNodeHasNeighbors(i)) {
         dist = MathUtilities::distance3D(c1->getCoordinate(i), c2->getCoordinate(i));
      }
      setValue(i, column, dist);
      maxDist = std::max(maxDist, dist);
      
      if ((xDiffColumn >= 0) || (yDiffColumn >= 0) || (zDiffColumn >= 0)) {
         float diff[3];
         MathUtilities::subtractVectors(c1->getCoordinate(i), c2->getCoordinate(i), diff);
         if (xDiffColumn >= 0) {
            setValue(i, xDiffColumn, std::fabs(diff[0]));
         }
         if (yDiffColumn >= 0) {
            setValue(i, yDiffColumn, std::fabs(diff[1]));
         }
         if (zDiffColumn >= 0) {
            setValue(i, zDiffColumn, std::fabs(diff[2]));
         }
      }
   }
   setColumnColorMappingMinMax(column, 0.0, maxDist);
   
   //
   // Set color mapping for x/y/z diff columns
   //
   if (xDiffColumn >= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(xDiffColumn, minValue, maxValue);
      setColumnColorMappingMinMax(xDiffColumn, minValue, maxValue);
   }
   if (yDiffColumn >= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(yDiffColumn, minValue, maxValue);
      setColumnColorMappingMinMax(yDiffColumn, minValue, maxValue);
   }
   if (zDiffColumn >= 0) {
      float minValue, maxValue;
      getDataColumnMinMax(zDiffColumn, minValue, maxValue);
      setColumnColorMappingMinMax(zDiffColumn, minValue, maxValue);
   }

   setModified();
}
                                           
/**
 * add a column that is T-Map of distances in two coordinate files (c1 - c2).
 * NOTE: the deviation input files should contain the deviation squared divided
 * by the number of subjects.
 */
void 
MetricFile::addColumnOfCoordinateDifferenceTMap(const CoordinateFile* c1,
                                                const CoordinateFile* c2,
                                                const TopologyFile* topologyFile,
                                                const int columnIn,
                                                const QString& columnName,
                                                const QString& columnComment,
                                                const MetricFile* deviationSquaredDividedByN_1,
                                                const int deviation1Column,
                                                const MetricFile* deviationSquaredDividedByN_2,
                                                const int deviation2Column,
                                                const bool addCoordinateDifferencesFlag) throw (FileException)
{
   //
   // Check inputs
   //
   if (c1 == NULL) {
      throw FileException("First coordinate file invalid");
   }
   if (c2 == NULL) {
      throw FileException("Second coordinate file invalid");
   }
   if (topologyFile == NULL) {
      throw FileException("Topology file invalid");
   }
   if (columnName.isEmpty()) {
      throw FileException("Column name is empty.");
   }
   
   //
   // Check number of nodes
   //
   const int numNodes = c1->getNumberOfCoordinates();
   if (numNodes <= 0) {
      throw FileException("First coordinate file contains no coordinates");
   }
   if (numNodes != c2->getNumberOfCoordinates()) {
      throw FileException("Coordinate files have a different number of nodes");
   }
   const int metricNodes = getNumberOfNodes();
   if (metricNodes > 0) {
      if (metricNodes != numNodes) {
         throw FileException("Coordinate and metric files have different number of nodes.");
      }
   }

   //
   // create/find desired metric column
   //   
   int column = columnIn;
   if (metricNodes == 0) {
      setNumberOfNodesAndColumns(numNodes, 1);
      column = 0;
   }
   else {
      if ((column < 0) || (column >= getNumberOfColumns())) {
         addColumns(1);
         column = getNumberOfColumns() - 1;
      }
   }
   
   //
   // Set the column name and comment
   //
   setColumnName(column, columnName);
   setColumnComment(column, columnComment);
   
   //
   // Create a topoogy helper
   //
   const TopologyHelper* th = topologyFile->getTopologyHelper(false, true, false);
   
   //
   // Set the metric data with distance between coordinates
   //
   float minT = std::numeric_limits<float>::max();
   float maxT = -std::numeric_limits<float>::max();
   for (int i = 0; i < numNodes; i++) {
      float t = 0.0;
      if (th->getNodeHasNeighbors(i)) {
         const float dist = MathUtilities::distance3D(c1->getCoordinate(i), c2->getCoordinate(i));
         const float denom = std::sqrt(deviationSquaredDividedByN_1->getValue(i, deviation1Column)
                                       + deviationSquaredDividedByN_2->getValue(i, deviation2Column));
         if (denom != 0.0) {
            t = dist / denom;
         }
         maxT = std::max(maxT, t);
         minT = std::min(minT, t);
      }
      setValue(i, column, t);
   }
   setColumnColorMappingMinMax(column, minT, maxT);

   //
   // Are distances desired ?
   //
   if (addCoordinateDifferencesFlag) {
      //
      // Add 4 columns for distance, dx, dy, dz
      //
      const int lastCol = getNumberOfColumns();
      addColumns(4);
      
      //
      // Just use plain coordinate differences method to get distance, and dx, dy, dz
      //
      addColumnOfCoordinateDifference(c1,
                                      c2,
                                      topologyFile,
                                      lastCol,
                                      "Distance",
                                      "",
                                      lastCol + 1,
                                      lastCol + 2,
                                      lastCol + 3);                                      
   }
   
   setModified();
}
                                           
//
//------------------------------------------------------------------------------------------
//

/**
 * Constructor.
 */
MetricMappingInfo::MetricMappingInfo()
{
   reset();
} 

/**
 * Constructor.
 */
MetricMappingInfo::MetricMappingInfo(const QString& surfaceNameIn,
                                     const int surfaceIndexNumberIn,
                                     const QString& volumeNameIn,
                                     const QString& subVolumeNameIn,
                                     const int volumeNumberIn,
                                     const int subVolumeNumberIn)
{
   setData(surfaceNameIn, surfaceIndexNumberIn,
           volumeNameIn, subVolumeNameIn, volumeNumberIn, subVolumeNumberIn);
} 

/**
 * Destructor.
 */
MetricMappingInfo::~MetricMappingInfo()
{
} 

/**
 * reset the data.
 */
void 
MetricMappingInfo::reset()
{
   setData("", -1, "", "", -1, -1);
}
      
/**
 * set the data.
 */
void 
MetricMappingInfo::setData(const QString& surfaceNameIn,
                           const int surfaceIndexNumberIn,
                           const QString& volumeNameIn,
                           const QString& subVolumeNameIn,
                           const int volumeNumberIn,
                           const int subVolumeNumberIn)
{
   surfaceName = surfaceNameIn;
   surfaceIndexNumber = surfaceIndexNumberIn;
   volumeName = volumeNameIn;
   subVolumeName = subVolumeNameIn;
   volumeNumber    = volumeNumberIn;
   subVolumeNumber = subVolumeNumberIn;
} 

/**
 * get the data.
 */
void 
MetricMappingInfo::getData(QString& surfaceNameOut,
                           int& surfaceIndexNumberOut,
                           QString& volumeNameOut,
                           QString& subVolumeNameOut,
                           int& volumeNumberOut,
                           int& subVolumeNumberOut) const
{
   surfaceNameOut = surfaceName;
   surfaceIndexNumberOut = surfaceIndexNumber;
   volumeNameOut = volumeName;
   subVolumeNameOut = subVolumeName;
   volumeNumberOut    = volumeNumber;
   subVolumeNumberOut = subVolumeNumber;
} 
