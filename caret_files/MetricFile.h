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


#ifndef __METRIC_FILE__CLASS_H__
#define __METRIC_FILE__CLASS_H__

#include "CoordinateFile.h"
#include "FileException.h"
#include "GiftiCommon.h"
#include "GiftiNodeDataFile.h"
#include "SpecFile.h"

class DeformationMapFile;
class TopologyFile;

/// MetricMappingInfo is a class used when mapping functional volumes to a metric file.
/// It is not stored in the MetricFile.
class MetricMappingInfo {
   public:
      /// Constructor
      MetricMappingInfo();
      
      /// Constructor
      MetricMappingInfo(const QString& surfaceNameIn,
                        const int surfaceIndexNumberIn,
                        const QString& volumeNameIn,
                        const QString& subVolumeNameIn,
                        const int volumeNumberIn,
                        const int subVolumeNumberIn);
      
      /// Destructor
      virtual ~MetricMappingInfo();
      
      /// reset the data
      void reset();
      
      /// set the data
      void setData(const QString& surfaceNameIn,
                   const int surfaceIndexNumberIn,
                   const QString& volumeNameIn,
                   const QString& subVolumeNameIn,
                   const int volumeNumberIn,
                   const int subVolumeNumberIn);
      
      /// get the data
      void getData(QString& surfaceNameOut,
                   int& surfaceIndexNumberOut,
                   QString& volumeNameOut,
                   QString& subVolumeNameOut,
                   int& volumeNumberOut,
                   int& subVolumeNumberOut) const;
                   
   protected:
      /// the volume number
      int volumeNumber;
      
      /// the sub-volume number
      int subVolumeNumber;
      
      /// the name of the volume
      QString volumeName;
      
      /// the sub volume name
      QString subVolumeName;
      
      /// the name of the surface
      QString surfaceName;
      
      /// the surface index number
      int surfaceIndexNumber;
};

/// MetricFile - a class that associates one or more floating point values with each surface node
class MetricFile : public GiftiNodeDataFile {
   public:
      /// smoothing algorithms
      enum SMOOTH_ALGORITHM {
         SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS,
         SMOOTH_ALGORITHM_GAUSSIAN,
         SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS,
         SMOOTH_ALGORITHM_NONE
      };
      
      /// concatenate columns mode
      enum CONCATENATE_COLUMNS_MODE {
         /// concatenate all columns from all files
         CONCATENATE_COLUMNS_MODE_ALL,
         /// concatenate columns with exact matching name
         CONCATENATE_COLUMNS_MODE_NAME_EXACT
      };
      
      // constructor
      MetricFile(const QString& descriptiveName = "MetricFile",
                 const QString& defaultDataArrayCategoryIn = GiftiCommon::categoryFunctional,
                 const QString& defaultExt = SpecFile::getMetricFileExtension());

      // copy constructor
      MetricFile(const MetricFile& nndf);
      
      // destructor
      virtual ~MetricFile();
      
      // assignment operator
      MetricFile& operator=(const MetricFile& mf);
      
      // clear the metric file
      void clear();
      
      // add a data array
      virtual void addDataArray(GiftiDataArray* nda);
            
      // append a data array file to this one
      virtual void append(const GiftiDataArrayFile& naf) throw (FileException);

      // append a data array file to this one but selectively load/overwrite arraysumns
      // arrayDestination is where naf's arrays should be (-1=new, -2=do not load)
      virtual void append(const GiftiDataArrayFile& naf, 
                          std::vector<int>& indexDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);

      /// reset a data array
      virtual void resetDataArray(const int arrayIndex);
      
      /// remove a data array
      virtual void removeDataArray(const int arrayIndex);
      
      /// add a column that is distance between nodes in two coordinate files (c1 - c2)
      void addColumnOfCoordinateDifference(const CoordinateFile* c1,
                                           const CoordinateFile* c2,
                                           const TopologyFile* topologyFile,
                                           const int column,
                                           const QString& columnName,
                                           const QString& columnComment,
                                           const int xDiffColumn = -1,
                                           const int yDiffColumn = -1,
                                           const int zDiffColumn = -1) throw (FileException);
                                           
      /// add a column that is T-Map of distances in two coordinate files (c1 - c2)
      void addColumnOfCoordinateDifferenceTMap(const CoordinateFile* c1,
                                               const CoordinateFile* c2,
                                               const TopologyFile* topologyFile,
                                               const int column,
                                               const QString& columnName,
                                               const QString& columnComment,
                                               const MetricFile* deviationSquaredDividedByN_1,
                                               const int deviation1Column,
                                               const MetricFile* deviationSquaredDividedByN_2,
                                               const int deviation2Column,
                                               const bool addCoordinateDifferencesFlag) throw (FileException);
      
      // return a metric file that contains descriptive statistics about "this" metric file
      MetricFile* descriptiveStatistics(const bool keepDataColumns = false,
                                        const QString& meanColumnName = "Mean",
                                        const QString& varianceColumnName = "Variance",
                                        const QString& sampleVarianceColumnName = "Sample Variance",
                                        const QString& standardDeviationColumnName = "Standard Deviation",
                                        const QString& sampleStandardDeviationColumnName = "Sample Standard Deviation",
                                        const QString& standardErrorOfTheMeanColumnName = "Standard Error of the Mean",
                                        const QString& rootMeanSquareColumnName = "Root Mean Square",
                                        const QString& minimumColumnName = "Minimum",
                                        const QString& maximumColumnName = "Maximum",
                                        const QString& medianColumnName = "Median",
                                        const QString& skewnessColumnName = "Skewness",
                                        const QString& kurtosisColumnName = "Kurtosis") const throw (FileException);

      // compute the average, deviation, error for all columns for each node
      void computeStatistics(const QString& averageColumnName = "Average",
                             const QString& standardDeviationColumnName = "Deviation",
                             const QString& standardErrorColumnName = "Standard Error",
                             const QString& minimumAbsColumnName = "MinimumAbs",
                             const QString& maximumAbsColumnName = "MaximumAbs");
      
      // compute the average, deviation, error for specified columns for each node
      void computeStatistics(const std::vector<bool>& useColumn,
                             const QString& averageColumnName = "Average",
                             const QString& standardDeviationColumnName = "Deviation",
                             const QString& standardErrorColumnName = "Standard Error",
                             const QString& minimumAbsColumnName = "MinimumAbs",
                             const QString& maximumAbsColumnName = "MaximumAbs");
      
      // Compute the correlation coefficient for a column in relation to all other columns.
      void correlationCoefficient(const int columnOfInterest,
                                  std::vector<float>& correlationCoefficients,
                                  const std::vector<bool>* limitToTheseNodes = NULL) const;
         
      // compute and return a metric file that is a Z-map of "this" metric file
      // Z-map is (Xi - Mean)/Dev for all elements in each row
      MetricFile* computeStatisticalZMap() const throw (FileException);
      
      // compute and return a metric file that contains permuted T-Values of "this" metric file
      // permutation is perfomred by genernating a random plus or minus one for each column
      // and multiplying the row by +/-1 and then computing the T-Value
      MetricFile* computePermutedTValues(const float constant,
                                         const int iterations,
                                         const TopologyFile* topologyFile,
                                         const int varianceSmoothingIterations,
                                         const float varianceSmoothingStrength) const throw (FileException);
                                         
      // compute and return a metric file that computes T-Values of "this" metric file
      // T-Value = (mean - constant) / (sample-dev / sqrt(N))
      MetricFile* computeTValues(const float constant,
                                 const TopologyFile* topologyFile,
                                 const int varianceSmoothingIterations,
                                 const float varianceSmoothingStrength) const throw (FileException);
      
      // compute and return a T-map for the rows of two metric files
      static MetricFile* computeStatisticalTMap(const MetricFile* m1,
                                                const MetricFile* m2,
                                                const TopologyFile* topologyFile,
                                                const int varianceSmoothingIterations,
                                                const float varianceSmoothingStrength,
                                                const bool poolTheVariance,
                                                const float falseDiscoveryRateQ,
                                                const bool doFalseDiscoveryRateFlag,
                                                const bool doDegreesOfFreedomFlag,
                                                const bool doPValuesFlag) throw (FileException);
      
      // compute and return a Levene map for the rows of the metric files
      static MetricFile* computeStatisticalLeveneMap(const std::vector<MetricFile*>& inputFiles)
                                                        throw (FileException);
                                                        
      // subract the average of both files form each file (output files are replaced)
      static void subtractMeanFromRowElements(const MetricFile* inputFile1,
                                              const MetricFile* inputFile2,
                                              MetricFile* outputFile1,
                                              MetricFile* outputFile2) throw (FileException);
      
      // concatenate columns from a group of metric files (all must have same number of nodes)
      static void concatenateColumnsFromFiles(const QString& outputFileName,
                                              const std::vector<QString>& inputFileNames,
                                              const CONCATENATE_COLUMNS_MODE mode,
                                              const QString& columnName) throw (FileException);
          
      // extract columns (indices starting at zero) from a metric file and store in another metric file
      static void extractColumnsFromFile(const QString& inputFileName,
                                         const QString& outputFileName,
                                         const std::vector<int>& columnsToExtract) throw (FileException);
                                         
      // shuffle the values amongst group of metric files (input and output files must be same dimensions)
      static void shuffle(const std::vector<MetricFile*>& metricFilesIn,
                          std::vector<MetricFile*>& metricFilesOut) throw (FileException);

      // compute T-map on shuffled columns split into two groups
      MetricFile* computeStatisticalShuffledTMap(const int numberOfRepetitions,
                                                 const int numberInGroup1,
                                                 const TopologyFile* topologyFile,
                                                 const int varianceSmoothingIterations,
                                                 const float varianceSmoothingStrength,
                                                 const bool poolTheVariance) const throw (FileException);
      
      // compute shuffled cross correlation maps
      MetricFile* computeShuffledCrossCorrelationsMap(const int numberOfRepetitions) const throw (FileException);
      
      // compute normalization of all columns
      MetricFile* computeNormalization(const float mean,
                                       const float standardDeviation) const throw (FileException);
                                       
      // remap values in a column so that they fit a normal distribution
      // with the media at the normal distribution's mean
      void remapColumnToNormalDistribution(const int inputColumnNumber,
                                           const int outputColumnNumber,
                                           const QString& outputColumnName,
                                           const float normalDistributionMean,
                                           const float normalDistributionDeviation) throw (FileException);
                                           
      // deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                  GiftiNodeDataFile& deformedFile,
                  const DEFORM_TYPE dt) const throw (FileException);

      
      /// get the mapping information
      MetricMappingInfo* getColumnMappingInfo(const int columnNumber);
      
      /// get the mapping information (const method)
      const MetricMappingInfo* getColumnMappingInfo(const int columnNumber) const;
      
      /// set the color mapping min/max for all columns to the column min/max values
      void setColorMappingToColumnMinMax();
      
      /// get column color mapping min/max
      void getColumnColorMappingMinMax(const int columnNumber,
                                      float& minValue, float& maxValue) const;
                                      
      /// get column color mapping min/max
      void setColumnColorMappingMinMax(const int columnNumber,
                                       const float minValue, 
                                       const float maxValue);
      
      /// get min/max for a data column
      void getDataColumnMinMax(const int columnNumber, 
                               float& minValue, float& maxValue);
      
      /// get thresholding for a column
      void getColumnThresholding(const int columnNumber,
                                 float& negThreshOut,
                                 float& posThreshOut) const;
                                 
      /// set thresholding for a column
      void setColumnThresholding(const int columnNumber,
                                 const float negThreshIn,
                                 const float posThreshIn);
                                 
      /// get average thresholding for a column
      void getColumnAverageThresholding(const int columnNumber,
                                 float& negThreshOut,
                                 float& posThreshOut) const;
                                 
      /// set average thresholding for a column
      void setColumnAverageThresholding(const int columnNumber,
                                 const float negThreshIn,
                                 const float posThreshIn);
      
      /// get the number of nodes that exceed each of the thresholds
      void getThresholdExceededCounts(const int columnNumber,
                                      const float negThresh,
                                      const float posThresh,
                                      int& numNegExceeded,
                                      int& numPosExceeded) const;
                                      
      /// get a metric for specifed node and column
      float getValue(const int nodeNumber, const int columnNumber) const; 

      /// get metrics for a specified node
      void getValue(const int nodeNumber, float* metrics) const;
      
      /// get metrics for a specified node
      void getValue(const int nodeNumber, std::vector<float>& metrics) const;
      
      /// set a metric for specified node and column                         
      void setValue(const int nodeNumber, const int columnNumber,
                     const float metric);

      /// set a metrics for a specified node
      void setValue(const int nodeNumber, const float* metrics);
                          
      /// Get a column of values for all nodes
      void getColumnForAllNodes(const int columnNumber,
                                std::vector<float>& values) const;
                                 
      /// Get a column of values for all nodes (values must be allocated to getNumberOfNodes() elements)
      void getColumnForAllNodes(const int columnNumber,
                                float* values) const;
                                 
      /// Set a column of values for all nodes
      void setColumnForAllNodes(const int columnNumber,
                                const std::vector<float>& values);
      
      /// Set a column of values for all nodes
      void setColumnForAllNodes(const int columnNumber,
                                const float* values);
      
      /// get the column number for the column with the specified name
      int getNamedColumnNumber(const QString& name) const;
      
      /// change a range of values to zero
      void setRangeOfValuesToZero(const int inputColumnNumber,
                                  const int outputColumnNumber,
                                  const QString& outputColumnName,
                                  const float minValue,
                                  const float maxValue,
                                  const bool inclusiveRangeFlag)
                                                    throw (FileException);
                                  
      /// perform binary opertion on columns of metric file
      void performBinaryOperation(const BINARY_OPERATION operation,
                                  const int columnA,
                                  const int columnB,
                                  const int resultColumn,
                                  const QString& resultColumnName)
                                                    throw (FileException);

                                  
      /// perform unary opertion on columns of metric file
      void performUnaryOperation(const UNARY_OPERATION operation,
                                 const int column,
                                 const int resultColumn,
                                 const QString& resultColumnName,
                                 const float scalar)
                                                    throw (FileException);
                                  
      /// smooth a metric column (if output column is negative a new column is created)
      void smooth(const SMOOTH_ALGORITHM algorithm,
                  const int column, 
                  const int outputColumnIn,
                  const QString& outputColumnName,
                  const float strength,
                  const int iterations,
                  const TopologyFile* topologyFile,
                  const CoordinateFile* coordinateFile,
                  const CoordinateFile* nodeNormalVectors,
                  const float gaussNormBelowCutoff = 2.0,
                  const float gaussNormAboveCutoff = 2.0,
                  const float gaussSigmaNorm = 2.0,
                  const float gaussSigmaTang = 1.0,
                  const float gaussTangentCutoff = 3.0);
                        
      /// neighbor value smoothing
      void smoothNeighbors(const TopologyFile* tf, const int column);
      
      /// log 10 scale a column (do all columns if column number is negative)
      void scaleColumnLog10(const int columnIn);
      
      /// read just the column names from the file
      //void readFileColumnNames(const QString& filename,
      //                          std::vector<QString>& columnNames) throw (FileException);

      /// Export to a free surfer functional file.
      void exportFreeSurferAsciiFunctionalFile(const int columnNumber,
                                              const QString& filename) throw (FileException);
                                              
      /// Import free surfer functional as metric
      void importFreeSurferFunctionalFile(const int numNodes, 
                                          const QString& filename,
                                          const FILE_FORMAT fileFormat = AbstractFile::FILE_FORMAT_ASCII) throw (FileException);

   protected:
      // copy helper used by assignment operator and copy constructor
      void copyHelperMetric(const MetricFile& mf);
      
      /// used when mapping volumes to metric file (not stored in metric file)
      mutable std::vector<MetricMappingInfo> columnMappingInfo;
   
      /// column minimum/maximum values valid
      std::vector<int> columnMinimumMaximumValid;
      
      // copy a column of values to another column
      void copyColumn(const int inputColumnNumber, const int outputColumnNumber);
      
      // read metric file version 0
      void readFileVersion_0(QFile& file, QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      // read metric file version 1
      void readFileVersion_1(QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      // read metric file version 2
      void readFileVersion_2(QFile& file,
                             QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      // read the metric data for the nodes
      void readMetricNodeData(QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      // read the metric file's data
      void readLegacyNodeFileData(QFile& file, QTextStream& stream, QDataStream& binStream)
                                                                 throw (FileException);
      
      // write metric file
      void writeLegacyNodeFileData(QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      static const QString tagColumnColorMapping;
      static const QString tagColumnThreshold;
      static const QString tagColumnAverageThreshold;
      
      static const QString metaDataColumnColorMapping;
      static const QString metaDataColumnThreshold;
      static const QString metaDataColumnAverageThreshold;
};

#ifdef _METRIC_MAIN_
   const QString MetricFile::tagColumnColorMapping = "tag-column-color-mapping";
   const QString MetricFile::tagColumnThreshold    = "tag-column-threshold";
   const QString MetricFile::tagColumnAverageThreshold = "tag-column-average-threshold";
 
   const QString MetricFile::metaDataColumnColorMapping = "column-color-mapping";
   const QString MetricFile::metaDataColumnThreshold    = "column-threshold";
   const QString MetricFile::metaDataColumnAverageThreshold = "column-average-threshold";
#endif  // _METRIC_MAIN_

#endif  // __METRIC_FILE__CLASS_H__
