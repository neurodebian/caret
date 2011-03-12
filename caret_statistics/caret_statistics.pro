######################################################################
# Automatically generated by qmake (1.04a) Wed Jan 15 08:28:52 2003
######################################################################


TARGET	= CaretStatistics
CONFIG	+= staticlib
INCLUDEPATH += .

include(../caret_qmake_include.pro)

!vs:TEMPLATE = lib
vs:TEMPLATE=vclib
dll {
	CONFIG -= staticlib
	CONFIG += plugin
}

# turn off unicode for displaying windows web browser
win32 {
   DEFINES	-= UNICODE
}

# Input  
HEADERS += \
      StatisticAlgorithm.h \
      StatisticAnovaOneWay.h \
      StatisticAnovaTwoWay.h \
      StatisticConvertToZScore.h \
      StatisticCorrelationCoefficient.h \
      StatisticDataGroup.h \
      StatisticDcdflib.h \
      StatisticDescriptiveStatistics.h \
      StatisticException.h \
      StatisticFalseDiscoveryRate.h \
      StatisticGeneratePValue.h \
      StatisticHistogram.h \
      StatisticKruskalWallis.h \
      StatisticLeveneVarianceEquality.h \
      StatisticLinearRegression.h \
      StatisticMatrix.h \
      StatisticMeanAndDeviation.h \
      StatisticMultipleRegression.h \
      StatisticNormalizeDistribution.h \
      StatisticNumericalRecipes.h \
      StatisticPermutation.h \
      StatisticRandomNumber.h \
      StatisticRandomNumberOperator.h \
      StatisticRankTransformation.h \
      StatisticTestNames.h \
      StatisticTtestOneSample.h \
      StatisticTtestPaired.h \
      StatisticTtestTwoSample.h \
      StatisticUnitTesting.h \
      StatisticValueIndexSort.h \
      StatisticVtkMath.h 

SOURCES += \
      StatisticAlgorithm.cxx \
      StatisticAnovaOneWay.cxx \
      StatisticAnovaTwoWay.cxx \
      StatisticConvertToZScore.cxx \
      StatisticCorrelationCoefficient.cxx \
      StatisticDataGroup.cxx \
      StatisticDcdflib.cxx \
      StatisticDcdflibIpmpar.cxx \
      StatisticDescriptiveStatistics.cxx \
      StatisticException.cxx \
      StatisticFalseDiscoveryRate.cxx \
      StatisticGeneratePValue.cxx \
      StatisticHistogram.cxx \
      StatisticKruskalWallis.cxx \
      StatisticLeveneVarianceEquality.cxx \
      StatisticLinearRegression.cxx \
      StatisticMatrix.cxx \
      StatisticMeanAndDeviation.cxx \
      StatisticMultipleRegression.cxx \
      StatisticNormalizeDistribution.cxx \
      StatisticNumericalRecipes.cxx \
      StatisticPermutation.cxx \
      StatisticRandomNumber.cxx \
      StatisticRandomNumberOperator.cxx \
      StatisticRankTransformation.cxx \
      StatisticTestNames.cxx \
      StatisticTtestOneSample.cxx \
      StatisticTtestPaired.cxx \
      StatisticTtestTwoSample.cxx \
      StatisticUnitTesting.cxx \
      StatisticValueIndexSort.cxx \
      StatisticVtkMath.cxx

