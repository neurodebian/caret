
#
# Name of Project
#
PROJECT(CaretStatistics)


#
# QT include files
#
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTOPENGL TRUE)

INCLUDE(${QT_USE_FILE})

#
# Create a library
#
ADD_LIBRARY(CaretStatistics
      StatisticAlgorithm.h 
      StatisticAnovaOneWay.h 
      StatisticAnovaTwoWay.h 
      StatisticConvertToZScore.h 
      StatisticCorrelationCoefficient.h 
      StatisticDataGroup.h 
      StatisticDcdflib.h 
      StatisticDescriptiveStatistics.h 
      StatisticException.h 
      StatisticFalseDiscoveryRate.h 
      StatisticGeneratePValue.h 
      StatisticHistogram.h 
      StatisticKruskalWallis.h 
      StatisticLeveneVarianceEquality.h 
      StatisticLinearRegression.h 
      StatisticMatrix.h 
      StatisticMeanAndDeviation.h 
      StatisticMultipleRegression.h 
      StatisticNormalizeDistribution.h 
      StatisticNumericalRecipes.h 
      StatisticPermutation.h 
      StatisticRandomNumber.h 
      StatisticRandomNumberOperator.h 
      StatisticRankTransformation.h 
      StatisticTestNames.h 
      StatisticTtestOneSample.h 
      StatisticTtestPaired.h 
      StatisticTtestTwoSample.h 
      StatisticUnitTesting.h 
      StatisticValueIndexSort.h 
      StatisticVtkMath.h 

      StatisticAlgorithm.cxx 
      StatisticAnovaOneWay.cxx 
      StatisticAnovaTwoWay.cxx 
      StatisticConvertToZScore.cxx 
      StatisticCorrelationCoefficient.cxx 
      StatisticDataGroup.cxx 
      StatisticDcdflib.cxx 
      StatisticDcdflibIpmpar.cxx 
      StatisticDescriptiveStatistics.cxx 
      StatisticException.cxx 
      StatisticFalseDiscoveryRate.cxx 
      StatisticGeneratePValue.cxx 
      StatisticHistogram.cxx 
      StatisticKruskalWallis.cxx 
      StatisticLeveneVarianceEquality.cxx 
      StatisticLinearRegression.cxx 
      StatisticMatrix.cxx 
      StatisticMeanAndDeviation.cxx 
      StatisticMultipleRegression.cxx 
      StatisticNormalizeDistribution.cxx 
      StatisticNumericalRecipes.cxx 
      StatisticPermutation.cxx 
      StatisticRandomNumber.cxx 
      StatisticRandomNumberOperator.cxx 
      StatisticRankTransformation.cxx 
      StatisticTestNames.cxx 
      StatisticTtestOneSample.cxx 
      StatisticTtestPaired.cxx 
      StatisticTtestTwoSample.cxx 
      StatisticUnitTesting.cxx 
      StatisticValueIndexSort.cxx 
      StatisticVtkMath.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/caret_brain_set
${CMAKE_SOURCE_DIR}/caret_cifti
${CMAKE_SOURCE_DIR}/caret_common
${CMAKE_SOURCE_DIR}/caret_files
${CMAKE_SOURCE_DIR}/caret_gifti
${CMAKE_SOURCE_DIR}/caret_statistics
${CMAKE_SOURCE_DIR}/caret_command_operations
${CMAKE_SOURCE_DIR}/caret_uniformize
)
