
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

#include <cmath>
#include <iostream>

#include "StatisticAnovaOneWay.h"
#include "StatisticAnovaTwoWay.h"
#include "StatisticConvertToZScore.h"
#include "StatisticCorrelationCoefficient.h"
#include "StatisticDataGroup.h"
#include "StatisticDescriptiveStatistics.h"
#include "StatisticException.h"
#include "StatisticFalseDiscoveryRate.h"
#include "StatisticHistogram.h"
#include "StatisticLeveneVarianceEquality.h"
#include "StatisticMeanAndDeviation.h"
#include "StatisticNormalizeDistribution.h"
#include "StatisticPermutation.h"
#include "StatisticRankTransformation.h"
#include "StatisticRandomNumber.h"
#include "StatisticTtestOneSample.h"
#include "StatisticTtestPaired.h"
#include "StatisticTtestTwoSample.h"
#include "StatisticUnitTesting.h"
#include "StatisticValueIndexSort.h"

/**
 * constructor.
 */
StatisticUnitTesting::StatisticUnitTesting(const bool printTestValuesFlagIn)
{
   printTestValuesFlag = printTestValuesFlagIn;
}

/**
 * destructor.
 */
StatisticUnitTesting::~StatisticUnitTesting()
{
}

/**
 * run the tests.
 */
void 
StatisticUnitTesting::execute() throw (StatisticException)
{
   problemFlag = false;
   
   std::cout << std::endl;
   
   problemFlag |= testStatisticAnovaOneWay();
   std::cout << std::endl;
   
   problemFlag |= testStatisticAnovaTwoWayFixedEffectCase1();
   std::cout << std::endl;
   
   problemFlag |= testStatisticAnovaTwoWayFixedEffectCase2();
   std::cout << std::endl;
   
   problemFlag |= testStatisticAnovaTwoWayRandomEffect();
   std::cout << std::endl;
   
   problemFlag |= testStatisticAnovaTwoWayMixedEffect();
   std::cout << std::endl;
   
   problemFlag |= testConvertToZScore();
   std::cout << std::endl;
   
   problemFlag |= testCorrelationCoefficient();
   std::cout << std::endl;
   
   problemFlag |= testStatisticDescriptive();
   std::cout << std::endl;
   
   problemFlag |= testFalseDiscoveryRate();
   std::cout << std::endl;
   
   problemFlag |= testHistogram();
   std::cout << std::endl;
   
   problemFlag |= testLevenesTest();
   std::cout << std::endl;
   
   problemFlag |= testNormalizeDistributionSorted();
   std::cout << std::endl;
   
   problemFlag |= testNormalizeDistributionUnsorted();
   std::cout << std::endl;
   
   problemFlag |= testStatisticMeanAndDeviation();
   std::cout << std::endl;
   
   problemFlag |= testPermutationRandomShuffle();
   std::cout << std::endl;
   
   problemFlag |= testPermutationSignFlipping();
   std::cout << std::endl;
   
   problemFlag |= testRankTransformation();
   std::cout << std::endl;
   
   problemFlag |= testStatisticTtestOneSample();
   std::cout << std::endl;
   
   problemFlag |= testStatisticTtestPaired();
   std::cout << std::endl;
   
   problemFlag |= testStatisticTtestTwoSamplePooledVariance();
   std::cout << std::endl;
   
   problemFlag |= testStatisticTtestTwoSampleUnpooledVariance();
   std::cout << std::endl;
   
   problemFlag |= testValueIndexSort();
   std::cout << std::endl;
   
   if (problemFlag == false) {
      std::cout << "SUCCESSFUL Statistic Unit Test." << std::endl;
   }
   else {
      std::cout << "FAILURES in Statistic Unit Test." << std::endl;
   }
   std::cout << std::endl;
}

/**
 * test value/index sorting.
 */
bool 
StatisticUnitTesting::testValueIndexSort()
{
   const int numData = 10;
   const float data[numData] = { 3, 5, 7, 2, 4, 9, 1, 13, 12, 6 };
   const float dataSorted[numData] = { 1, 2, 3, 4, 5, 6, 7, 9, 12, 13 };
   const float indicesSorted[numData] = { 6, 3, 0, 4, 1, 9, 2, 5, 8, 7 };
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticValueIndexSort vis;
   vis.addDataGroup(&sdg);
   
   try {
      vis.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticValueIndexSort threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   if (vis.getNumberOfItems() != numData) {
      std::cout << "FAILED StatisticValueIndexSort number of output items should be "
                << numData 
                << " but is "
                << vis.getNumberOfItems()
                << std::endl;
      return true;
   }
   
   bool problem = false;
   
   for (int i = 0; i < numData; i++) {
      int indx;
      float value;
      vis.getValueAndOriginalIndex(i, indx, value);
      problem |= verify("StatisticValueIndexSort value " + StatisticAlgorithm::numberToString(i),
                        value,
                        dataSorted[i]);
      problem |= verify("StatisticValueIndexSort original index " + StatisticAlgorithm::numberToString(i),
                        indx,
                        indicesSorted[i]);
   }
   
   if (problem == false) {
      std::cout << "PASSED StatisticValueIndexSort" << std::endl;
   }
   
   return problem;
}
      
/**
 * test descriptive statistics.
 */
bool 
StatisticUnitTesting::testStatisticDescriptive()
{
   //
   // Data from page 43, table 2-2, Statistics For Psychology, 2nd, Aron & Aron
   //
   const int numData = 10;
   const float data[numData] = {
      7.0,
      8.0,
      8.0,
      7.0,
      3.0,
      1.0,
      6.0,
      9.0,
      3.0,
      8.0
   };
   
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticDescriptiveStatistics sds;
   sds.addDataGroup(&sdg);
   
   try {
      sds.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticDescriptiveStatistics threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticDescriptiveStatistics SumOfSquares",
                     sds.getSumOfSquares(),
                     66.0);
                     
   problem |= verify("StatisticDescriptiveStatistics Mean",
                     sds.getMean(),
                     6.0);
   
   problem |= verify("StatisticDescriptiveStatistics Variance",
                     sds.getVariance(),
                     6.6);
   
   problem |= verify("StatisticDescriptiveStatistics Population Sample Variance",
                     sds.getPopulationSampleVariance(),
                     7.333333);
   
   problem |= verify("StatisticDescriptiveStatistics Standard Deviation",
                     sds.getStandardDeviation(),
                     2.57);
   
   problem |= verify("StatisticDescriptiveStatistics Population Sample Standard Deviation",
                     sds.getPopulationSampleStandardDeviation(),
                     2.708);
   
   problem |= verify("StatisticDescriptiveStatistics Root Mean Square",
                     sds.getRootMeanSquare(),
                     6.52687);
   
   problem |= verify("StatisticDescriptiveStatistics Standard Error of the Mean",
                     sds.getStandardErrorOfTheMean(),
                     0.856349);
   
   float minValue, maxValue;
   sds.getMinimumAndMaximum(minValue, maxValue);
   problem |= verify("StatisticDescriptiveStatistics Minimum Value",
                     minValue,
                     1.0);
   
   problem |= verify("StatisticDescriptiveStatistics Maximum Value",
                     maxValue,
                     9.0);
   
   problem |= verify("StatisticDescriptiveStatistics Median",
                     sds.getMedian(),
                     7.0);
   
   problem |= verify("StatisticDescriptiveStatistics Skewness",
                     sds.getSkewness(),
                     -0.784397);
   
   problem |= verify("StatisticDescriptiveStatistics Kurtosis",
                     sds.getKurtosis(),
                     3.80165);
   
   if (problem == false) {
      std::cout << "PASSED StatisticDescriptiveStatistics" << std::endl;
   }
   
   return problem;
}

/**
 * test mean, variance, deviation.
 */
bool 
StatisticUnitTesting::testStatisticMeanAndDeviation()
{
   //
   // Data from page 43, table 2-2, Statistics For Psychology, 2nd, Aron & Aron
   //
   const int numData = 10;
   float data[numData] = {
      7.0,
      8.0,
      8.0,
      7.0,
      3.0,
      1.0,
      6.0,
      9.0,
      3.0,
      8.0
   };
   
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticMeanAndDeviation smad;
   smad.addDataGroup(&sdg);
   
   try {
      smad.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticMeanAndDeviation threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticMeanAndDeviation SumOfSquares",
                     smad.getSumOfSquares(),
                     66.0);
                     
   problem |= verify("StatisticMeanAndDeviation Mean",
                     smad.getMean(),
                     6.0);
   
   problem |= verify("StatisticMeanAndDeviation Variance",
                     smad.getVariance(),
                     6.6);
   
   problem |= verify("StatisticMeanAndDeviation Population Sample Variance",
                     smad.getPopulationSampleVariance(),
                     7.333333);
   
   problem |= verify("StatisticMeanAndDeviation Standard Deviation",
                     smad.getStandardDeviation(),
                     2.57);
   
   problem |= verify("StatisticMeanAndDeviation Population Sample Standard Deviation",
                     smad.getPopulationSampleStandardDeviation(),
                     2.708);
   
   if (problem == false) {
      std::cout << "PASSED StatisticMeanAndDeviation" << std::endl;
   }
   
   return problem;
}

/**
 * test one-sample T-Test.
 */
bool 
StatisticUnitTesting::testStatisticTtestOneSample()
{
   //
   // Data from page 264, table 9-3, Statistics For Psychology, 2nd, Aron & Aron
   //
   const int numData = 10;
   float data[numData] = {
      5.0,
      3.0,
      6.0,
      2.0,
      7.0,
      6.0,
      7.0,
      4.0,
      2.0,
      5.0
   };
   
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   const float knownMeanMu = 4.0;
   StatisticTtestOneSample tTest(knownMeanMu);
   tTest.addDataGroup(&sdg);

   try {
      tTest.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticTtestOneSample threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticTtestOneSample T-Value",
                     tTest.getTValue(),
                     1.17211);
                     
   problem |= verify("StatisticTtestOneSample Degrees Of Freedom",
                     tTest.getDegreesOfFreedom(),
                     9.0);
                     
   problem |= verify("StatisticTtestOneSample P-Value",
                     tTest.getPValue(),
                     0.135623);
                     
   if (problem == false) {
      std::cout << "PASSED StatisticTtestOneSample" << std::endl;
   }
   
   return problem;
}

/**
 * test Paired T-Test.
 */
bool 
StatisticUnitTesting::testStatisticTtestPaired()
{
   //
   // Data from page 269, table 9-5, Statistics For Psychology, 2nd, Aron & Aron
   //
   const int numData = 19;
   float dataBefore[numData] = {
      126,
      133,
      126,
      115,
      108,
      109,
      124,
      98,
      95,
      120,
      118,
      126,
      121,
      116,
      94,
      105,
      123,
      125,
      128,
   };
   
   float dataAfter[numData] = {
      115,
      125,
      96,
      115,
      119,
      82,
      93,
      109,
      72,
      104,
      107,
      118,
      102,
      115,
      83,
      87,
      121,
      100,
      118,
   };
   
   
   StatisticDataGroup sdgAfter(dataAfter, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgBefore(dataBefore, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticTtestPaired tTest;
   tTest.addDataGroup(&sdgAfter);
   tTest.addDataGroup(&sdgBefore);

   try {
      tTest.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticTtestPaired threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticTtestPaired T-Value",
                     tTest.getTValue(),
                     -4.24042);
                     
   problem |= verify("StatisticTtestPaired Degrees Of Freedom",
                     tTest.getDegreesOfFreedom(),
                     18.0);
                     
   problem |= verify("StatisticTtestPaired P-Value",
                     tTest.getPValue(),
                     0.000246008);
                     
   if (problem == false) {
      std::cout << "PASSED StatisticTtestPaired" << std::endl;
   }
   
   return problem;
}

/**
 * test Paired T-Test.
 */
bool 
StatisticUnitTesting::testStatisticTtestTwoSamplePooledVariance()
{
   //
   // Data from http://www.statsdirect.com/help/parametric_methods/utt.htm
   //
   const int numDataA = 12;
   float dataA[numDataA] = {
      134,
      146,
      104,
      119,
      124,
      161,
      107,
      83,
      113,
      129,
      97,
      123
   };
   
   const int numDataB = 7;
   float dataB[numDataB] = {
      70,
      118,
      101,
      85,
      107,
      132,
      94
   };
   
   
   StatisticDataGroup sdgA(dataA, numDataA, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgB(dataB, numDataB, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticTtestTwoSample tTest(StatisticTtestTwoSample::VARIANCE_TYPE_POOLED);
   tTest.addDataGroup(&sdgA);
   tTest.addDataGroup(&sdgB);

   try {
      tTest.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticTtestTwoSample PooledVariance threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticTtestTwoSample PooledVariance T-Value",
                     tTest.getTValue(),
                     1.891436);
                     
   problem |= verify("StatisticTtestTwoSample PooledVariance Degrees Of Freedom",
                     tTest.getDegreesOfFreedom(),
                     17.0);
                     
   problem |= verify("StatisticTtestTwoSample PooledVariance P-Value",
                     tTest.getPValue(),
                     0.0379);
                     
   if (problem == false) {
      std::cout << "PASSED StatisticTtestTwoSample PooledVariance" << std::endl;
   }
   
   return problem;
}

/**
 * test Paired T-Test.
 */
bool 
StatisticUnitTesting::testStatisticTtestTwoSampleUnpooledVariance()
{
   //
   // Data from http://www.statsdirect.com/help/parametric_methods/utt.htm
   //
   const int numDataA = 12;
   float dataA[numDataA] = {
      134,
      146,
      104,
      119,
      124,
      161,
      107,
      83,
      113,
      129,
      97,
      123
   };
   
   const int numDataB = 7;
   float dataB[numDataB] = {
      70,
      118,
      101,
      85,
      107,
      132,
      94
   };
   
   
   StatisticDataGroup sdgA(dataA, numDataA, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgB(dataB, numDataB, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticTtestTwoSample tTest(StatisticTtestTwoSample::VARIANCE_TYPE_UNPOOLED);
   tTest.addDataGroup(&sdgA);
   tTest.addDataGroup(&sdgB);

   try {
      tTest.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticTtestTwoSample UnpooledVariance threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticTtestTwoSample UnpooledVariance T-Value",
                     tTest.getTValue(),
                     1.9107);
                     
   problem |= verify("StatisticTtestTwoSample UnpooledVariance Degrees Of Freedom",
                     tTest.getDegreesOfFreedom(),
                     13.081702);
                     
   problem |= verify("StatisticTtestTwoSample UnpooledVariance P-Value",
                     tTest.getPValue(),
                     0.0391);
                     
   if (problem == false) {
      std::cout << "PASSED StatisticTtestTwoSample UnpooledVariance " << std::endl;
   }
   
   return problem;
}

/**
 * test two-way fixed-effect ANOVA.
 */
bool 
StatisticUnitTesting::testStatisticAnovaTwoWayFixedEffectCase1()
{
   //
   // Data from Statistics for Psychology
   //           Arthur Aron & Elaine Aron
   //           2nd Edition, 1999
   //           Table 17-3, page 397
   //
   const int numData = 10;
   const float boysLowAffiliation[numData] = {
      12.1,
      11.4,
      11.2,
      10.9,
      10.3,
      9.8,
      9.7,
      9.5,
      9.3,
      8.8
   };
   
   const float boysHighAffiliation[numData] = {
      11.1,
      10.4,
      10.2,
      9.8,
      9.2,
      9.1,
      8.9,
      8.7,
      8.2,
      6.6
   };
   
   const float girlsLowAffiliation[numData] = {
      17.4,
      17.1,
      16.8,
      16.7,
      15.5,
      15.3,
      15.0,
      15.4,
      14.3,
      14.0
   };
   
   const float girlsHighAffiliation[numData] = {
      22.0,
      20.5,
      19.9,
      19.1,
      18.5,
      17.4,
      17.0,
      17.1,
      17.1,
      16.5
   };
   
   //        Affiliation
   //Gender   Low  High
   //Boys
   //Girls
   
   StatisticDataGroup sdgBoysLow(boysLowAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgBoysHigh(boysHighAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgGirlsLow(girlsLowAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgGirlsHigh(girlsHighAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_FIXED_EFFECT);
   anova.setNumberOfFactorLevels(2, 2);
   anova.setDataGroup(0, 0, &sdgBoysLow);
   anova.setDataGroup(0, 1, &sdgBoysHigh);
   anova.setDataGroup(1, 0, &sdgGirlsLow);
   anova.setDataGroup(1, 1, &sdgGirlsHigh);
   
   try {
      anova.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticAnovaTwoWay Fixed Case 1 threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 SSTR",
                     anova.getSumOfSquaresTreatmentSSTR(),
                     587.089);
                     
   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 SSE",
                     anova.getSumOfSquaresErrorSSE(),
                     67.25);
                     
   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 SSTO",
                     anova.getSumOfSquaresTotalSSTO(),
                     654.339);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 SSA",
                     anova.getSumOfSquaresSSA(),
                     543.169);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 SSB",
                     anova.getSumOfSquaresSSB(),
                     7.056);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 SSAB",
                     anova.getSumOfSquaresSSAB(),
                     36.864);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Mean of All",
                     anova.getMeanOfAllValues(),
                     13.445);
      
   const float cellMeans[4] = { 10.3, 9.22, 15.75, 18.51 };
   for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
         const std::string text("StatisticAnovaTwoWay Fixed Case 1 Cell "
                            + StatisticAlgorithm::numberToString(i)
                            + ", "
                            + StatisticAlgorithm::numberToString(j)
                            + " Mean");
         problem |= verify(text,
                           anova.getCellMean(i, j),
                           cellMeans[i*2 + j]);
      }
   }

   const float meansA[2] = { 9.76, 17.13 };
   for (int i = 0; i < 2; i++) {
      const std::string text("StatisticAnovaTwoWay Fixed Case 1 Factor Level A "
                         + StatisticAlgorithm::numberToString(i)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelA(i),
                           meansA[i]);
   }

   const float meansB[2] = { 13.025, 13.865 };
   for (int j = 0; j < 2; j++) {
      const std::string text("StatisticAnovaTwoWay Fixed Case 1 Factor Level B "
                         + StatisticAlgorithm::numberToString(j)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelB(j),
                           meansB[j]);
   }

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Degrees of Freedom Factor A",
                     anova.getDegreesOfFreedomFactorA(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Degrees of Freedom Factor B",
                     anova.getDegreesOfFreedomFactorB(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Degrees of Freedom Interactions",
                     anova.getDegreesOfFreedomInteractions(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Degrees of Freedom Between Treatments",
                     anova.getDegreesOfFreedomBetweenTreatments(),
                     3.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Degrees of Freedom Error",
                     anova.getDegreesOfFreedomError(),
                     36.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Degrees of Freedom Total",
                     anova.getDegreesOfFreedomTotal(),
                     39.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Mean Square Factor A",
                     anova.getMeanSquareFactorA_MSA(),
                     543.169);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Mean Square Factor B",
                     anova.getMeanSquareFactorB_MSB(),
                     7.056);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Mean Square Interaction",
                     anova.getMeanSquareInteractionMSAB(),
                     36.864);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Mean Square Between Treatments",
                     anova.getMeanSquareBetweenTreatmentsMSTR(),
                     195.696);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 Mean Square Error",
                     anova.getMeanSquareErrorMSE(),
                     1.868);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 F-Statistic Factor A",
                     anova.getFStatisticFactorA(),
                     290.767);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 F-Statistic Factor B",
                     anova.getfStatisticFactorB(),
                     3.77719);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 F-Statistic Interaction",
                     anova.getfStatisticInteraction(),
                     19.7339);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 P-Value Factor A",
                     anova.getPValueFactorA(),
                     0.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 P-Value Factor B",
                     anova.getPValueFactorB(),
                     0.0598056);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 1 P-Value Interaction",
                     anova.getPValueInteraction(),
                     0.0000814);
   if (problem) {
      std::cout << "StatisticAnovaTwoWay Fixed Case 1 Factor A: Gender" << std::endl;
      std::cout << "StatisticAnovaTwoWay Fixed Case 1 Factor B: Affiliation" << std::endl;
   }
   
   if (problem == false) {
      std::cout << "PASSED StatisticAnovaTwoWay Fixed Case 1 " << std::endl;
   }
   
   return problem;
}

/**
 * test two-way fixed-effect ANOVA.
 */
bool 
StatisticUnitTesting::testStatisticAnovaTwoWayFixedEffectCase2()
{
   //
   // Data from Applied Linear Statistical Models
   //           John Neter, William Wasserman, and Michael H. Kutner
   //           Third Edition, 1990
   //           Data from Table 18.7, page 695 Castle Bakery Example
   //                     Table 18.10, page 705
   //                     Figure 18.7, page 711
   //
   const int numData = 2;
   const float bottomRegular[numData] = { 47.0, 43.0 };
   const float middleRegular[numData] = { 62.0, 68.0};
   const float topRegular[numData]    = { 41.0, 39.0};
   const float bottomWide[numData] = { 46.0, 40.0 };
   const float middleWide[numData] = { 67.0, 71.0 };
   const float topWide[numData]    = { 42.0, 46.0 };
   
   //         Display Width
   //Height   Regular  Wide
   //Bottom
   //Middle
   //Top
   
   StatisticDataGroup sdgBottomRegular(bottomRegular, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgMiddleRegular(middleRegular, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgTopRegular(topRegular, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgBottomWide(bottomWide, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgMiddleWide(middleWide, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgTopWide(topWide, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_FIXED_EFFECT);
   anova.setNumberOfFactorLevels(3, 2);
   anova.setDataGroup(0, 0, &sdgBottomRegular);
   anova.setDataGroup(1, 0, &sdgMiddleRegular);
   anova.setDataGroup(2, 0, &sdgTopRegular);
   anova.setDataGroup(0, 1, &sdgBottomWide);
   anova.setDataGroup(1, 1, &sdgMiddleWide);
   anova.setDataGroup(2, 1, &sdgTopWide);
   
   try {
      anova.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticAnovaTwoWay Fixed Case 2 threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 SSTR",
                     anova.getSumOfSquaresTreatmentSSTR(),
                     1580.0);
                     
   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 SSE",
                     anova.getSumOfSquaresErrorSSE(),
                     62.0);
                     
   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 SSTO",
                     anova.getSumOfSquaresTotalSSTO(),
                     1642.0);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 SSA",
                     anova.getSumOfSquaresSSA(),
                     1544.0);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 SSB",
                     anova.getSumOfSquaresSSB(),
                     12.0);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 SSAB",
                     anova.getSumOfSquaresSSAB(),
                     24.0);
      
   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Mean of All",
                     anova.getMeanOfAllValues(),
                     51.0);
      
   const float cellMeans[6] = { 45.0, 43.0, 65.0, 69.0, 40.0, 44.0 };
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 2; j++) {
         const std::string text("StatisticAnovaTwoWay Fixed Case 2 Cell "
                            + StatisticAlgorithm::numberToString(i)
                            + ", "
                            + StatisticAlgorithm::numberToString(j)
                            + " Mean");
         problem |= verify(text,
                           anova.getCellMean(i, j),
                           cellMeans[i*2 + j]);
      }
   }

   const float meansA[3] = { 44.0, 67.0, 42.0 };
   for (int i = 0; i < 3; i++) {
      const std::string text("StatisticAnovaTwoWay Fixed Case 2 Factor Level A "
                         + StatisticAlgorithm::numberToString(i)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelA(i),
                           meansA[i]);
   }

   const float meansB[2] = { 50.0, 52.0 };
   for (int j = 0; j < 2; j++) {
      const std::string text("StatisticAnovaTwoWay Fixed Case 2 Factor Level B "
                         + StatisticAlgorithm::numberToString(j)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelB(j),
                           meansB[j]);
   }

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Degrees of Freedom Factor A",
                     anova.getDegreesOfFreedomFactorA(),
                     2.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Degrees of Freedom Factor B",
                     anova.getDegreesOfFreedomFactorB(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Degrees of Freedom Interactions",
                     anova.getDegreesOfFreedomInteractions(),
                     2.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Degrees of Freedom Between Treatments",
                     anova.getDegreesOfFreedomBetweenTreatments(),
                     5.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Degrees of Freedom Error",
                     anova.getDegreesOfFreedomError(),
                     6.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Degrees of Freedom Total",
                     anova.getDegreesOfFreedomTotal(),
                     11.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Mean Square Factor A",
                     anova.getMeanSquareFactorA_MSA(),
                     772.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Mean Square Factor B",
                     anova.getMeanSquareFactorB_MSB(),
                     12.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Mean Square Interaction",
                     anova.getMeanSquareInteractionMSAB(),
                     12.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Mean Square Between Treatments",
                     anova.getMeanSquareBetweenTreatmentsMSTR(),
                     316.0);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 Mean Square Error",
                     anova.getMeanSquareErrorMSE(),
                     10.33333);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 F-Statistic Factor A",
                     anova.getFStatisticFactorA(),
                     74.71);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 F-Statistic Factor B",
                     anova.getfStatisticFactorB(),
                     1.16129);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 F-Statistic Interaction",
                     anova.getfStatisticInteraction(),
                     1.16129);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 P-Value Factor A",
                     anova.getPValueFactorA(),
                     0.0001);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 P-Value Factor B",
                     anova.getPValueFactorB(),
                     0.3226);

   problem |= verify("StatisticAnovaTwoWay Fixed Case 2 P-Value Interaction",
                     anova.getPValueInteraction(),
                     0.3747);
   if (problem) {
      std::cout << "StatisticAnovaTwoWay Fixed Case 2 Factor A: Display Height" << std::endl;
      std::cout << "StatisticAnovaTwoWay Fixed Case 2 Factor B: Width" << std::endl;
   }
   
   if (problem == false) {
      std::cout << "PASSED StatisticAnovaTwoWay Fixed Case 2 " << std::endl;
   }
   
   return problem;
}

/**
 * test two-way random-effect ANOVA.
 */
bool 
StatisticUnitTesting::testStatisticAnovaTwoWayRandomEffect()
{
   //
   // Data from Statistics for Psychology
   //           Arthur Aron & Elaine Aron
   //           2nd Edition, 1999
   //           Table 17-3, page 397
   //
   const int numData = 10;
   const float boysLowAffiliation[numData] = {
      12.1,
      11.4,
      11.2,
      10.9,
      10.3,
      9.8,
      9.7,
      9.5,
      9.3,
      8.8
   };
   
   const float boysHighAffiliation[numData] = {
      11.1,
      10.4,
      10.2,
      9.8,
      9.2,
      9.1,
      8.9,
      8.7,
      8.2,
      6.6
   };
   
   const float girlsLowAffiliation[numData] = {
      17.4,
      17.1,
      16.8,
      16.7,
      15.5,
      15.3,
      15.0,
      15.4,
      14.3,
      14.0
   };
   
   const float girlsHighAffiliation[numData] = {
      22.0,
      20.5,
      19.9,
      19.1,
      18.5,
      17.4,
      17.0,
      17.1,
      17.1,
      16.5
   };
   
   //        Affiliation
   //Gender   Low  High
   //Boys
   //Girls
   
   StatisticDataGroup sdgBoysLow(boysLowAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgBoysHigh(boysHighAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgGirlsLow(girlsLowAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgGirlsHigh(girlsHighAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_RANDOM_EFFECT);
   anova.setNumberOfFactorLevels(2, 2);
   anova.setDataGroup(0, 0, &sdgBoysLow);
   anova.setDataGroup(0, 1, &sdgBoysHigh);
   anova.setDataGroup(1, 0, &sdgGirlsLow);
   anova.setDataGroup(1, 1, &sdgGirlsHigh);
   
   try {
      anova.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticAnovaTwoWay Random threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticAnovaTwoWay Random SSTR",
                     anova.getSumOfSquaresTreatmentSSTR(),
                     587.089);
                     
   problem |= verify("StatisticAnovaTwoWay Random SSE",
                     anova.getSumOfSquaresErrorSSE(),
                     67.25);
                     
   problem |= verify("StatisticAnovaTwoWay Random SSTO",
                     anova.getSumOfSquaresTotalSSTO(),
                     654.339);
      
   problem |= verify("StatisticAnovaTwoWay Random SSA",
                     anova.getSumOfSquaresSSA(),
                     543.169);
      
   problem |= verify("StatisticAnovaTwoWay Random SSB",
                     anova.getSumOfSquaresSSB(),
                     7.056);
      
   problem |= verify("StatisticAnovaTwoWay Random SSAB",
                     anova.getSumOfSquaresSSAB(),
                     36.864);
      
   problem |= verify("StatisticAnovaTwoWay Random Mean of All",
                     anova.getMeanOfAllValues(),
                     13.445);
      
   const float cellMeans[4] = { 10.3, 9.22, 15.75, 18.51 };
   for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
         const std::string text("StatisticAnovaTwoWay Random Cell "
                            + StatisticAlgorithm::numberToString(i)
                            + ", "
                            + StatisticAlgorithm::numberToString(j)
                            + " Mean");
         problem |= verify(text,
                           anova.getCellMean(i, j),
                           cellMeans[i*2 + j]);
      }
   }

   const float meansA[2] = { 9.76, 17.13 };
   for (int i = 0; i < 2; i++) {
      const std::string text("StatisticAnovaTwoWay Random Factor Level A "
                         + StatisticAlgorithm::numberToString(i)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelA(i),
                           meansA[i]);
   }

   const float meansB[2] = { 13.025, 13.865 };
   for (int j = 0; j < 2; j++) {
      const std::string text("StatisticAnovaTwoWay Random Factor Level B "
                         + StatisticAlgorithm::numberToString(j)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelB(j),
                           meansB[j]);
   }

   problem |= verify("StatisticAnovaTwoWay Random Degrees of Freedom Factor A",
                     anova.getDegreesOfFreedomFactorA(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Random Degrees of Freedom Factor B",
                     anova.getDegreesOfFreedomFactorB(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Random Degrees of Freedom Interactions",
                     anova.getDegreesOfFreedomInteractions(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Random Degrees of Freedom Between Treatments",
                     anova.getDegreesOfFreedomBetweenTreatments(),
                     3.0);

   problem |= verify("StatisticAnovaTwoWay Random Degrees of Freedom Error",
                     anova.getDegreesOfFreedomError(),
                     36.0);

   problem |= verify("StatisticAnovaTwoWay Random Degrees of Freedom Total",
                     anova.getDegreesOfFreedomTotal(),
                     39.0);

   problem |= verify("StatisticAnovaTwoWay Random Mean Square Factor A",
                     anova.getMeanSquareFactorA_MSA(),
                     543.169);

   problem |= verify("StatisticAnovaTwoWay Random Mean Square Factor B",
                     anova.getMeanSquareFactorB_MSB(),
                     7.056);

   problem |= verify("StatisticAnovaTwoWay Random Mean Square Interaction",
                     anova.getMeanSquareInteractionMSAB(),
                     36.864);

   problem |= verify("StatisticAnovaTwoWay Random Mean Square Between Treatments",
                     anova.getMeanSquareBetweenTreatmentsMSTR(),
                     195.696);

   problem |= verify("StatisticAnovaTwoWay Random Mean Square Error",
                     anova.getMeanSquareErrorMSE(),
                     1.868);

   problem |= verify("StatisticAnovaTwoWay Random F-Statistic Factor A",
                     anova.getFStatisticFactorA(),
                     14.7344);

   problem |= verify("StatisticAnovaTwoWay Random F-Statistic Factor B",
                     anova.getfStatisticFactorB(),
                     0.191406);

   problem |= verify("StatisticAnovaTwoWay Random F-Statistic Interaction",
                     anova.getfStatisticInteraction(),
                     19.7339);

   problem |= verify("StatisticAnovaTwoWay Random P-Value Factor A",
                     anova.getPValueFactorA(),
                     0.000481591);

   problem |= verify("StatisticAnovaTwoWay Random P-Value Factor B",
                     anova.getPValueFactorB(),
                     0.664362);

   problem |= verify("StatisticAnovaTwoWay Random P-Value Interaction",
                     anova.getPValueInteraction(),
                     0.0000814022);
   if (problem) {
      std::cout << "StatisticAnovaTwoWay Random Factor A: Gender" << std::endl;
      std::cout << "StatisticAnovaTwoWay Random Factor B: Affiliation" << std::endl;
   }
   
   if (problem == false) {
      std::cout << "PASSED StatisticAnovaTwoWay Random " << std::endl;
   }
   
   return problem;
}

/**
 * test two-way mixed-effect ANOVA.
 */
bool 
StatisticUnitTesting::testStatisticAnovaTwoWayMixedEffect()
{
   //
   // Data from Statistics for Psychology
   //           Arthur Aron & Elaine Aron
   //           2nd Edition, 1999
   //           Table 17-3, page 397
   //
   const int numData = 10;
   const float boysLowAffiliation[numData] = {
      12.1,
      11.4,
      11.2,
      10.9,
      10.3,
      9.8,
      9.7,
      9.5,
      9.3,
      8.8
   };
   
   const float boysHighAffiliation[numData] = {
      11.1,
      10.4,
      10.2,
      9.8,
      9.2,
      9.1,
      8.9,
      8.7,
      8.2,
      6.6
   };
   
   const float girlsLowAffiliation[numData] = {
      17.4,
      17.1,
      16.8,
      16.7,
      15.5,
      15.3,
      15.0,
      15.4,
      14.3,
      14.0
   };
   
   const float girlsHighAffiliation[numData] = {
      22.0,
      20.5,
      19.9,
      19.1,
      18.5,
      17.4,
      17.0,
      17.1,
      17.1,
      16.5
   };
   
   //        Affiliation
   //Gender   Low  High
   //Boys
   //Girls
   
   StatisticDataGroup sdgBoysLow(boysLowAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgBoysHigh(boysHighAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgGirlsLow(girlsLowAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgGirlsHigh(girlsHighAffiliation, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVE_MODEL_TYPE_MIXED_EFFECT);
   anova.setNumberOfFactorLevels(2, 2);
   anova.setDataGroup(0, 0, &sdgBoysLow);
   anova.setDataGroup(0, 1, &sdgBoysHigh);
   anova.setDataGroup(1, 0, &sdgGirlsLow);
   anova.setDataGroup(1, 1, &sdgGirlsHigh);
   
   try {
      anova.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticAnovaTwoWay Mixed threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticAnovaTwoWay Mixed SSTR",
                     anova.getSumOfSquaresTreatmentSSTR(),
                     587.089);
                     
   problem |= verify("StatisticAnovaTwoWay Mixed SSE",
                     anova.getSumOfSquaresErrorSSE(),
                     67.25);
                     
   problem |= verify("StatisticAnovaTwoWay Mixed SSTO",
                     anova.getSumOfSquaresTotalSSTO(),
                     654.339);
      
   problem |= verify("StatisticAnovaTwoWay Mixed SSA",
                     anova.getSumOfSquaresSSA(),
                     543.169);
      
   problem |= verify("StatisticAnovaTwoWay Mixed SSB",
                     anova.getSumOfSquaresSSB(),
                     7.056);
      
   problem |= verify("StatisticAnovaTwoWay Mixed SSAB",
                     anova.getSumOfSquaresSSAB(),
                     36.864);
      
   problem |= verify("StatisticAnovaTwoWay Mixed Mean of All",
                     anova.getMeanOfAllValues(),
                     13.445);
      
   const float cellMeans[4] = { 10.3, 9.22, 15.75, 18.51 };
   for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
         const std::string text("StatisticAnovaTwoWay Mixed Cell "
                            + StatisticAlgorithm::numberToString(i)
                            + ", "
                            + StatisticAlgorithm::numberToString(j)
                            + " Mean");
         problem |= verify(text,
                           anova.getCellMean(i, j),
                           cellMeans[i*2 + j]);
      }
   }

   const float meansA[2] = { 9.76, 17.13 };
   for (int i = 0; i < 2; i++) {
      const std::string text("StatisticAnovaTwoWay Mixed Factor Level A "
                         + StatisticAlgorithm::numberToString(i)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelA(i),
                           meansA[i]);
   }

   const float meansB[2] = { 13.025, 13.865 };
   for (int j = 0; j < 2; j++) {
      const std::string text("StatisticAnovaTwoWay Mixed Factor Level B "
                         + StatisticAlgorithm::numberToString(j)
                         + " Mean");
         problem |= verify(text,
                           anova.getMeanFactorLevelB(j),
                           meansB[j]);
   }

   problem |= verify("StatisticAnovaTwoWay Mixed Degrees of Freedom Factor A",
                     anova.getDegreesOfFreedomFactorA(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Mixed Degrees of Freedom Factor B",
                     anova.getDegreesOfFreedomFactorB(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Mixed Degrees of Freedom Interactions",
                     anova.getDegreesOfFreedomInteractions(),
                     1.0);

   problem |= verify("StatisticAnovaTwoWay Mixed Degrees of Freedom Between Treatments",
                     anova.getDegreesOfFreedomBetweenTreatments(),
                     3.0);

   problem |= verify("StatisticAnovaTwoWay Mixed Degrees of Freedom Error",
                     anova.getDegreesOfFreedomError(),
                     36.0);

   problem |= verify("StatisticAnovaTwoWay Mixed Degrees of Freedom Total",
                     anova.getDegreesOfFreedomTotal(),
                     39.0);

   problem |= verify("StatisticAnovaTwoWay Mixed Mean Square Factor A",
                     anova.getMeanSquareFactorA_MSA(),
                     543.169);

   problem |= verify("StatisticAnovaTwoWay Mixed Mean Square Factor B",
                     anova.getMeanSquareFactorB_MSB(),
                     7.056);

   problem |= verify("StatisticAnovaTwoWay Mixed Mean Square Interaction",
                     anova.getMeanSquareInteractionMSAB(),
                     36.864);

   problem |= verify("StatisticAnovaTwoWay Mixed Mean Square Between Treatments",
                     anova.getMeanSquareBetweenTreatmentsMSTR(),
                     195.696);

   problem |= verify("StatisticAnovaTwoWay Mixed Mean Square Error",
                     anova.getMeanSquareErrorMSE(),
                     1.868);

   problem |= verify("StatisticAnovaTwoWay Mixed F-Statistic Factor A",
                     anova.getFStatisticFactorA(),
                     14.7344);

   problem |= verify("StatisticAnovaTwoWay Mixed F-Statistic Factor B",
                     anova.getfStatisticFactorB(),
                     3.77719);

   problem |= verify("StatisticAnovaTwoWay Mixed F-Statistic Interaction",
                     anova.getfStatisticInteraction(),
                     19.7339);

   problem |= verify("StatisticAnovaTwoWay Mixed P-Value Factor A",
                     anova.getPValueFactorA(),
                     0.000481591);

   problem |= verify("StatisticAnovaTwoWay Mixed P-Value Factor B",
                     anova.getPValueFactorB(),
                     0.0598056);

   problem |= verify("StatisticAnovaTwoWay Mixed P-Value Interaction",
                     anova.getPValueInteraction(),
                     0.0000814022);
   if (problem) {
      std::cout << "StatisticAnovaTwoWay Mixed Factor A: Gender" << std::endl;
      std::cout << "StatisticAnovaTwoWay Mixed Factor B: Affiliation" << std::endl;
   }
   
   if (problem == false) {
      std::cout << "PASSED StatisticAnovaTwoWay Mixed " << std::endl;
   }
   
   return problem;
}

/**
 * test one-way ANOVA.
 * Data from http://www.statsdirect.com/help/analysis_of_variance/oneway.htm
 */
bool 
StatisticUnitTesting::testStatisticAnovaOneWay()
{
   float data1[] = { 279, 338, 334, 198, 303 };
   const int numData1 = sizeof(data1) / sizeof(float);
   float data2[] = { 378, 275, 412, 265, 286 };
   const int numData2 = sizeof(data2) / sizeof(float);
   float data3[] = { 172, 335, 335, 282, 250 };
   const int numData3 = sizeof(data3) / sizeof(float);
   float data4[] = { 381, 346, 340, 471, 318 };
   const int numData4 = sizeof(data4) / sizeof(float);

   StatisticDataGroup sdg1(data1, numData1, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdg2(data2, numData2, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdg3(data3, numData3, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdg4(data4, numData4, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticAnovaOneWay anova;
   anova.addDataGroup(&sdg1);
   anova.addDataGroup(&sdg2);
   anova.addDataGroup(&sdg3);
   anova.addDataGroup(&sdg4);

   try {
      anova.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticAnovaOneWay threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticAnovaOneWay SSTR",
                     anova.getSumOfSquaresTreatmentSSTR(),
                     27234.2);
                     
   problem |= verify("StatisticAnovaOneWay SSE",
                     anova.getSumOfSquaresErrorSSE(),
                     63953.6);
                     
   problem |= verify("StatisticAnovaOneWay SSTO",
                     anova.getSumOfSquaresTotalSSTO(),
                     91187.8);
                     
   problem |= verify("StatisticAnovaOneWay MSTR",
                     anova.getMeanSumOfSquaresTreatmentMSTR(),
                     9078.066667);
                     
   problem |= verify("StatisticAnovaOneWay MSE",
                     anova.getMeanSumOfSquaresErrorMSE(),
                     3997.1);
                     
   problem |= verify("StatisticAnovaOneWay DOF Between",
                     anova.getDegreesOfFreedomBetweenTreatments(),
                     3.0);
                     
   problem |= verify("StatisticAnovaOneWay DOF Within",
                     anova.getDegreesOfFreedomWithinTreatments(),
                     16.0);
                     
   problem |= verify("StatisticAnovaOneWay DOF Total",
                     anova.getDegreesOfFreedomTotal(),
                     19.0);
                     
   problem |= verify("StatisticAnovaOneWay F-Statistic",
                     anova.getFStatistic(),
                     2.271163);
                     
   problem |= verify("StatisticAnovaOneWay P-Value",
                     anova.getPValue(),
                     .1195);
                     
                     
   if (problem == false) {
      std::cout << "PASSED StatisticAnovaOneWay " << std::endl;
   }
   
   return problem;
}      

/**
 * verify that two floating point numbers are nearly identical (false if ok).
 */
bool 
StatisticUnitTesting::verify(const std::string& testName,
                             const float computedValue,
                             const float correctValue,
                             const float acceptableDifference)
{
   bool printThem = false;
   bool errorFlag = false;
   std::string printWord;
   const float diff = std::fabs(computedValue - correctValue);
   if (diff > acceptableDifference) {
      printWord = "FAILED";
      printThem = true;
      errorFlag = true;
   }
   else if (printTestValuesFlag) {
      printThem = true;
   }
   
   if (printThem) {
      std::cout << printWord << " "
                << testName << std::endl;
      std::cout << "   Computed Value = " << computedValue << std::endl;
      if (errorFlag) {
         std::cout << "   Correct Value  = " << correctValue << std::endl;
      }
   }
   
   return errorFlag;
}

/**
 * test convert to z-score.
 */
bool 
StatisticUnitTesting::testConvertToZScore()
{
   //
   // Data from Statistics for Psychology
   //           Arthur Aron & Elaine Aron
   //           2nd Edition, 1999
   //           Table 2-2, page 43
   //           Table 2-5, page 53
   //           Mean = 6
   //           SD = 2.57
   //
   
   bool problem = false;

   const int numData = 10;
   const float data[numData] = { 7, 8, 8, 7, 3, 1, 6, 9, 3, 8 };
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   
   StatisticConvertToZScore convertToZ;
   convertToZ.addDataGroup(&sdg);
   
   try {
      convertToZ.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticConvertToZScore threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   float value = 7.0;
   convertToZ.convertToZScore(value);
   
   problem |= verify("StatisticConvertToZScore ",
                     value,
                     0.38910);  // 0.39 in example
                     
   if (problem == false) {
      std::cout << "PASSED StatisticConvertToZScore " << std::endl;
   }
   
   return problem;
}

/**
 * test correlation coefficient.
 */
bool 
StatisticUnitTesting::testCorrelationCoefficient()
{
   bool problem = false;

   //
   // Data from Statistics for Psychology
   //           Arthur Aron & Elaine Aron
   //           2nd Edition, 1999
   //           Table 3-2, page 78
   //           T-stats page 99
   const int numData = 5;
   const float numEmployees[numData] = { 6, 8, 3, 10, 8 };
   const float stressLevel[numData]  = { 7, 8, 1,  8, 6 };
   StatisticDataGroup sdgEmployees(numEmployees, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgStress(stressLevel, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticCorrelationCoefficient correlate;
   correlate.addDataGroup(&sdgEmployees);
   correlate.addDataGroup(&sdgStress);
   
   try {
      correlate.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticCorrelationCoefficient threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   problem |= verify("StatisticCorrelationCoefficient Correlation Coefficient R2",
                     correlate.getCorrelationCoefficient(),
                     0.765756); 
   problem |= verify("StatisticCorrelationCoefficient T-Value",
                     correlate.getTValue(),
                     3.13164); // 3.317 in book
   problem |= verify("StatisticCorrelationCoefficient Degrees of Freedom",
                     correlate.getDegreesOfFreedom(),
                     3.0); 
   problem |= verify("StatisticCorrelationCoefficient P-Value",
                     correlate.getPValue(),
                     0.02599); 
                     
   if (problem == false) {
      std::cout << "PASSED StatisticCorrelationCoefficient " << std::endl;
   }
   
   return problem;
}

/**
 * test false discovery rate.
 */
bool 
StatisticUnitTesting::testFalseDiscoveryRate()
{
   //
   // This data is made up and not known to be valid.
   //
   const int numData = 12;
   const float data[numData] = { 0.8, 0.01, 0.07, 0.12, 0.15, 0.0015,
                                 0.3, 0.02, 0.03, 0.03, 0.34, 0.0375 };
   const float q = 0.05;
   StatisticDataGroup sdg(data,
                          numData,
                          StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticFalseDiscoveryRate fdr(q,
                                   StatisticFalseDiscoveryRate::C_CONSTANT_1);
   fdr.addDataGroup(&sdg);
   try {
      fdr.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticFalseDiscoveryRate threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }

   bool problem = false;

   problem |= verify("StatisticFalseDiscoveryRate P-Cutoff",
                     fdr.getPCutoff(),
                     0.0015); 

   if (problem == false) {
      std::cout << "PASSED StatisticFalseDiscoveryRate " << std::endl;
   }
   
   return problem;
}

/**
 * test histogram.
 */
bool 
StatisticUnitTesting::testHistogram()
{
   const int numData = 15;
   const float data[numData] = { 1, 2, 9, 4, 3, 7, 5, 4, 5, 8, 2, 5, 3, 4, 4};
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   const int numBuckets = 5;
   StatisticHistogram hist(numBuckets);
   hist.addDataGroup(&sdg);
   try {
      hist.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticHistogram threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   if (numBuckets != hist.getNumberOfBuckets()) {
      std::cout << "FAILED StatisticHistogram should have produced "
                << numBuckets 
                << " buckets but produced "
                << hist.getNumberOfBuckets();
      return true;
   }
   
   const float bucketValues[numBuckets] = { 1.0, 2.6, 4.2, 5.8, 7.4 };
   const float bucketCounts[numBuckets] = { 3.0, 6.0, 3.0, 1.0, 2.0 };
   bool problem = false;
   for (int i = 0; i < numBuckets; i++) {
      float buckVal = 1.0;
      float buckCnt = 1.0;
      hist.getDataForBucket(i, buckVal, buckCnt);
      const std::string msg1("StatisticHistogram bucket data value " + StatisticAlgorithm::numberToString(i));
      problem |= verify(msg1,
                        buckVal,
                        bucketValues[i]);
      const std::string msg2("StatisticHistogram bucket count value " + StatisticAlgorithm::numberToString(i));
      problem |= verify(msg2,
                        buckCnt,
                        bucketCounts[i]);
   }

   if (problem == false) {
      std::cout << "PASSED StatisticHistogram " << std::endl;
   }
   
   return problem;
}

/**
 * test levene's test.
 */
bool 
StatisticUnitTesting::testLevenesTest()
{
   //
   // Data is made upt
   //
   const int numData = 5;
   const float dataA[numData] = { 1.4, 2.6, 0.8, 1.3, 1.9 };
   const float dataB[numData] = { 2.4, 1.8, 2.7, 2.3, 1.6 };
   StatisticDataGroup sdgA(dataA, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgB(dataB, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticLeveneVarianceEquality levene;
   levene.addDataGroup(&sdgA);
   levene.addDataGroup(&sdgB);
   
   try {
      levene.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticLeveneVarianceEquality threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;

   problem |= verify("StatisticLeveneVarianceEquality F-Statistic",
                     levene.getLeveneF(),
                     0.0982914); 
   problem |= verify("StatisticLeveneVarianceEquality DOF 1",
                     levene.getDegreesOfFreedom1(),
                     1.0);
   problem |= verify("StatisticLeveneVarianceEquality DOF 2",
                     levene.getDegreesOfFreedom2(),
                     8.0); 
   problem |= verify("StatisticLeveneVarianceEquality P-Value",
                     levene.getPValue(),
                     0.761908); 
                     
   if (problem == false) {
      std::cout << "PASSED StatisticLeveneVarianceEquality " << std::endl;
   }
   
   return problem;
}

/**
 * test rank transformation.
 */
bool 
StatisticUnitTesting::testRankTransformation()
{
   const int numData = 7;
   const float groupA[numData] = { 3, 9, 1, 7, 5, 1, 10 };
   const float groupB[numData]  = { 8, 10, 2, 7, 3, 10, 15 };
   const float resultA[numData] = { 4.5, 10, 1.5, 7.5, 6, 1.5, 12 };
   const float resultB[numData] = { 9, 12, 3, 7.5, 4.5, 12, 14 };
   
   StatisticDataGroup sdgA(groupA, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup sdgB(groupB, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticRankTransformation srt;
   srt.addDataGroup(&sdgA, false);
   srt.addDataGroup(&sdgB, false);
   
   try {
      srt.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticRankTransformation threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;

   if (srt.getNumberOfOutputDataGroups() != 2) {
      std::cout << "FAILED StatisticRankTransformation number of output data groups is "
                << srt.getNumberOfOutputDataGroups()
                << " but should be 2." << std::endl;
      return problem;
   }
   
   const StatisticDataGroup* aOut = srt.getOutputDataGroupContainingRankValues(0);
   for (int i = 0; i < numData; i++) {
      if (resultA[i] != aOut->getData(i)) {
         std::cout << "FAILED StatisticRankTransformation groupe A element "
                   << groupA[i]
                   << " should have rank "
                   << resultA[i]
                   << " but is ranked "
                   << aOut->getData(i) << std::endl;
         problem = true;
      }
   }
   
   const StatisticDataGroup* bOut = srt.getOutputDataGroupContainingRankValues(1);
   for (int i = 0; i < numData; i++) {
      if (resultB[i] != bOut->getData(i)) {
         std::cout << "FAILED StatisticRankTransformation groupe B element "
                   << groupB[i]
                   << " should have rank "
                   << resultB[i]
                   << " but is ranked "
                   << bOut->getData(i) << std::endl;
         problem = true;
      }
   }
   
   if (problem == false) {
      std::cout << "PASSED StatisticRankTransformation " << std::endl;
   }
   return problem;
}
 
/**
 * test normalization of a distribution.
 */
bool 
StatisticUnitTesting::testNormalizeDistributionSorted()
{
   //
   // Data is made upt
   //
   const int numData = 15;
   const float data[numData] = { 1, 3, 3, 3, 4, 4, 5, 7, 7, 8, 9, 10, 12, 14, 16 };
   const float normValues[numData] = { -5.7, -1.2, -0.85, -0.55, -0.3, -0.1, 0.0, 0.1, 0.3, 0.5, 0.75, 1.05, 1.45, 1.7, 10.0 };
   //const float normValues[numData] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticNormalizeDistribution normal(0.0, 1.0);
   normal.addDataGroup(&sdg);
   
   try {
      normal.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticNormalizeDistribution Sorted Data threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   const StatisticDataGroup* sdgOut = normal.getOutputDataGroupContainingNormalizeValues();
   if (sdgOut->getNumberOfData() != numData) {
      std::cout << "FAILED StatisticNormalizeDistribution Sorted Data output has wrong number of values." << std::endl;
      return true;
   }
   
   bool problem = false;

   for (int i = 0; i < numData; i++) {
      const std::string msg("StatisticNormalizeDistribution Sorted Data output value["
                        + StatisticAlgorithm::numberToString(i)
                        + "]");
      problem |= verify(msg,
                        sdgOut->getData(i),
                        normValues[i]); 
   }
                     
   if (problem == false) {
      std::cout << "PASSED StatisticNormalizeDistribution Sorted Data " << std::endl;
   }
   
   return problem;
} 

/**
 * test normalization of a distribution.
 */
bool 
StatisticUnitTesting::testNormalizeDistributionUnsorted()
{
   //
   // Data is made upt
   //
   const int numData = 15;
   const float data[numData] = { 1, 3, 7, 4, 12, 8, 5, 4, 9, 10, 3, 14, 3, 7, 16 };
   const float normValues[numData] = { -5.7, -1.2, 0.1, -0.3, 1.45, 0.5, 0.0, -0.1, 0.75, 1.05, -0.85, 1.7, -0.55, 0.3, 10 };
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticNormalizeDistribution normal(0.0, 1.0);
   normal.addDataGroup(&sdg);
   
   try {
      normal.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticNormalizeDistribution Unsorted Data threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   const StatisticDataGroup* sdgOut = normal.getOutputDataGroupContainingNormalizeValues();
   if (sdgOut->getNumberOfData() != numData) {
      std::cout << "FAILED StatisticNormalizeDistribution Unsorted Data output has wrong number of values." << std::endl;
      return true;
   }
   
   bool problem = false;

   for (int i = 0; i < numData; i++) {
      const std::string msg("StatisticNormalizeDistribution Unsorted Data output value["
                        + StatisticAlgorithm::numberToString(i)
                        + "]");
      problem |= verify(msg,
                        sdgOut->getData(i),
                        normValues[i]); 
   }
                     
   if (problem == false) {
      std::cout << "PASSED StatisticNormalizeDistribution Unsorted Data" << std::endl;
   }
   
   return problem;
}      
     
/**
 * test permutation random shuffle.
 */
bool 
StatisticUnitTesting::testPermutationRandomShuffle()
{
   //
   // Setting the seed results in the same set of random numbers being generated
   // and the same order each time this test is run
   //
   StatisticRandomNumber::setRandomSeed(1234567);
   
   //
   // Data is made up
   //
   const int numData = 10;
   const float data[numData] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
   const float shuffledData[numData] = { 3, 5, 2, 1, 7, 8, 6, 9, 10, 4 };
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticPermutation perm(StatisticPermutation::PERMUTATION_METHOD_RANDOM_ORDER);
   perm.addDataGroup(&sdg);
   
   //
   // Run the algorithm
   //
   try {
      perm.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticPermutation Random Shuffle threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   const StatisticDataGroup* sdgOut = perm.getOutputData();
   if (sdgOut->getNumberOfData() != numData) {
      std::cout << "FAILED StatisticPermutation Random Shuffle output has wrong number of values." << std::endl;
      return true;
   }
   
   bool problem = false;

   for (int i = 0; i < numData; i++) {
      const std::string msg("StatisticPermutation Random Shuffle output value["
                        + StatisticAlgorithm::numberToString(i)
                        + "]");
      problem |= verify(msg,
                        sdgOut->getData(i),
                        shuffledData[i]); 
   }

   if (problem == false) {
      std::cout << "PASSED StatisticPermutation Random Shuffle" << std::endl;
   }
   
   return problem;
}

/**
 * test permutation sign flipping.
 */
bool 
StatisticUnitTesting::testPermutationSignFlipping()
{
   //
   // Setting the seed results in the same set of random numbers being generated
   // and the same order each time this test is run
   //
   StatisticRandomNumber::setRandomSeed(1234567);
   
   //
   // Data is made up
   //
   const int numData = 10;
   const float data[numData] = { -1, 2, -3, 4, -5, 6, -7, 8, -9, 10 };
   const float signFlippedData[numData] = { -1, -2, 3, -4, -5, 6, -7, 8, -9, -10 };
   StatisticDataGroup sdg(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);

   StatisticPermutation perm(StatisticPermutation::PERMUTATION_METHOD_RANDOM_SIGN_FLIP);
   perm.addDataGroup(&sdg);
   
   //
   // Run the algorithm
   //
   try {
      perm.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticPermutation Sign Flip threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   const StatisticDataGroup* sdgOut = perm.getOutputData();
   if (sdgOut->getNumberOfData() != numData) {
      std::cout << "FAILED StatisticPermutation Sign Flip output has wrong number of values." << std::endl;
      return true;
   }
   
   bool problem = false;

   for (int i = 0; i < numData; i++) {
      const std::string msg("StatisticPermutation Sign Flip output value["
                        + StatisticAlgorithm::numberToString(i)
                        + "]");
      problem |= verify(msg,
                        sdgOut->getData(i),
                        signFlippedData[i]); 
   }

   if (problem == false) {
      std::cout << "PASSED StatisticPermutation Sign Flip" << std::endl;
   }
   
   return problem;
}      
