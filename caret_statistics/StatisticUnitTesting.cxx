
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
#include <sstream>

#include "StatisticAnovaOneWay.h"
#include "StatisticAnovaTwoWay.h"
#include "StatisticConvertToZScore.h"
#include "StatisticCorrelationCoefficient.h"
#include "StatisticDataGroup.h"
#include "StatisticDescriptiveStatistics.h"
#include "StatisticException.h"
#include "StatisticFalseDiscoveryRate.h"
#include "StatisticHistogram.h"
#include "StatisticKruskalWallis.h"
#include "StatisticLeveneVarianceEquality.h"
#include "StatisticLinearRegression.h"
#include "StatisticMatrix.h"
#include "StatisticMeanAndDeviation.h"
#include "StatisticMultipleRegression.h"
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
   : StatisticAlgorithm("Unit Testing")
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
   
   problemFlag |= testKruskalWallis();
   std::cout << std::endl;
   
   problemFlag |= testLevenesTest();
   std::cout << std::endl;
   
   problemFlag |= testLinearRegression();
   std::cout << std::endl;
   
   problemFlag |= testMatrixOperations();
   std::cout << std::endl;
   
   problemFlag |= testMultipleLinearRegression();
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
   
   StatisticValueIndexSort vis;
   vis.addDataArray(data, numData);
   
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
   
   StatisticDescriptiveStatistics sds;
   sds.addDataArray(data, numData);
   
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
   
   StatisticMeanAndDeviation smad;
   smad.addDataArray(data, numData);
   
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
   
   const float knownMeanMu = 4.0;
   StatisticTtestOneSample tTest(knownMeanMu);
   tTest.addDataArray(data, numData);

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
   
   
   StatisticTtestPaired tTest;
   tTest.addDataArray(dataAfter, numData);
   tTest.addDataArray(dataBefore, numData);

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
   
   
   StatisticTtestTwoSample tTest(StatisticTtestTwoSample::VARIANCE_TYPE_POOLED);
   tTest.addDataArray(dataA, numDataA);
   tTest.addDataArray(dataB, numDataB);

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
   
   StatisticTtestTwoSample tTest(StatisticTtestTwoSample::VARIANCE_TYPE_UNPOOLED);
   tTest.addDataArray(dataA, numDataA);
   tTest.addDataArray(dataB, numDataB);

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
   
   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_FIXED_EFFECT);
   anova.setNumberOfFactorLevels(2, 2);
   anova.setDataArray(0, 0, boysLowAffiliation, numData);
   anova.setDataArray(0, 1, boysHighAffiliation, numData);
   anova.setDataArray(1, 0, girlsLowAffiliation, numData);
   anova.setDataArray(1, 1, girlsHighAffiliation, numData);
   
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
   
   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_FIXED_EFFECT);
   anova.setNumberOfFactorLevels(3, 2);
   anova.setDataArray(0, 0, bottomRegular, numData);
   anova.setDataArray(1, 0, middleRegular, numData);
   anova.setDataArray(2, 0, topRegular, numData);
   anova.setDataArray(0, 1, bottomWide, numData);
   anova.setDataArray(1, 1, middleWide, numData);
   anova.setDataArray(2, 1, topWide, numData);
   
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
   
   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_RANDOM_EFFECT);
   anova.setNumberOfFactorLevels(2, 2);
   anova.setDataArray(0, 0, boysLowAffiliation, numData);
   anova.setDataArray(0, 1, boysHighAffiliation, numData);
   anova.setDataArray(1, 0, girlsLowAffiliation, numData);
   anova.setDataArray(1, 1, girlsHighAffiliation, numData);
   
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
   
   StatisticAnovaTwoWay anova;
   anova.setAnovaModelType(StatisticAnovaTwoWay::ANOVA_MODEL_TYPE_MIXED_EFFECT);
   anova.setNumberOfFactorLevels(2, 2);
   anova.setDataArray(0, 0, boysLowAffiliation, numData);
   anova.setDataArray(0, 1, boysHighAffiliation, numData);
   anova.setDataArray(1, 0, girlsLowAffiliation, numData);
   anova.setDataArray(1, 1, girlsHighAffiliation, numData);
   
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

   StatisticAnovaOneWay anova;
   anova.addDataArray(data1, numData1);
   anova.addDataArray(data2, numData2);
   anova.addDataArray(data3, numData3);
   anova.addDataArray(data4, numData4);

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
 * test Kruskal-Wallis.
 * Data from Applied Linear Statistical Models
 *           John Neter, William Wasserman, Michael H. Kutner
 *           3rd Edition
 *           Page 644
 */
bool 
StatisticUnitTesting::testKruskalWallis()
{
   float dataA[] = { 105, 3, 90, 217, 22 };
   const int numDataA = sizeof(dataA) / sizeof(float);
   float dataB[] = { 56, 43, 1, 37, 14 };
   const int numDataB = sizeof(dataB) / sizeof(float);
   float dataC[] = { 183, 144, 219, 86, 39 };
   const int numDataC = sizeof(dataC) / sizeof(float);

   StatisticKruskalWallis kw;
   kw.addDataArray(dataA, numDataA);
   kw.addDataArray(dataB, numDataB);
   kw.addDataArray(dataC, numDataC);

   try {
      kw.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticKruskalWallis threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;
   
   problem |= verify("StatisticKruskalWallis SSTR",
                     kw.getSumOfSquaresTreatmentSSTR(),
                     96.4);
                     
   problem |= verify("StatisticKruskalWallis SSE",
                     kw.getSumOfSquaresErrorSSE(),
                     183.6);
                                          
   problem |= verify("StatisticKruskalWallis MSTR",
                     kw.getMeanSumOfSquaresTreatmentMSTR(),
                     48.2);
                     
   problem |= verify("StatisticKruskalWallis MSE",
                     kw.getMeanSumOfSquaresErrorMSE(),
                     15.3);
                     
   problem |= verify("StatisticKruskalWallis DOF Between",
                     kw.getDegreesOfFreedomBetweenTreatments(),
                     2.0);
                     
   problem |= verify("StatisticKruskalWallis DOF Within",
                     kw.getDegreesOfFreedomWithinTreatments(),
                     12.0);
                     
   problem |= verify("StatisticKruskalWallis DOF Total",
                     kw.getDegreesOfFreedomTotal(),
                     14.0);
                     
   problem |= verify("StatisticKruskalWallis F-Statistic",
                     kw.getFStatistic(),
                     3.15);
                     
   problem |= verify("StatisticKruskalWallis P-Value",
                     kw.getPValue(),
                     0.08);
                     
                     
   if (problem == false) {
      std::cout << "PASSED StatisticKruskalWallis " << std::endl;
   }
   
   return problem;
}      

/*
 * verify that two matrices numbers are nearly identical (false if ok).
 */
bool 
StatisticUnitTesting::verify(const std::string& testName,
            const StatisticMatrix& computedMatrix,
            const StatisticMatrix& correctMatrix,
            const float acceptableDifference)
{
   bool printThem = false;
   bool errorFlag = false;
   std::string printWord;
   const int numRows = computedMatrix.getNumberOfRows();
   const int numCols = computedMatrix.getNumberOfColumns();
   if ((numRows == correctMatrix.getNumberOfRows()) &&
       (numCols == correctMatrix.getNumberOfColumns())) {
      for (int i = 0; i < numRows; i++) {
         for (int j = 0; j < numCols; j++) {
            const float diff = std::fabs(computedMatrix.getElement(i, j)
                                         - correctMatrix.getElement(i, j));
            if (diff > acceptableDifference) {
               printWord = "FAILED";
               printThem = true;
               errorFlag = true;
            }
            else if (printTestValuesFlag) {
               printThem = true;
            }
         }
      }
   }
   else {
      printWord = "FAILED";
      printThem = true;
      errorFlag = true;
   }
   
   if (printThem) {
      std::cout << printWord << " "
                << testName << std::endl;
      computedMatrix.print(std::cout, "      ", "   Computed Matrix: ");
      if (errorFlag) {
         correctMatrix.print(std::cout, "      ", "   Correct Matrix: ");
      }
   }
   
   return errorFlag;
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
 * verify that a group of coefficients are nearly identical (false if ok).
 */
bool
StatisticUnitTesting::verifyCoefficients(const std::string& testName,
                                         const std::vector<float>& computedCoefficients,
                                         const std::vector<float>& correctCoefficients,
                                         const float acceptableDifference)
{   
   bool errorFlag = false;
   
   for (int i = 0; i < static_cast<int>(correctCoefficients.size()); i++) {
      if (i < static_cast<int>(computedCoefficients.size())) {
         std::ostringstream str;
         str << testName
             << "  coefficient["
             << i 
             << "]";
         errorFlag |= verify(str.str(),
                             computedCoefficients[i],
                             correctCoefficients[i],
                             acceptableDifference); 
      }
      else {
         std::cout << testName
                   << " computed coefficient "
                   << i
                   << " is missing."
                   << std::endl;
         errorFlag |= true;
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
   
   StatisticConvertToZScore convertToZ;
   convertToZ.addDataArray(data, numData);
   
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

   StatisticCorrelationCoefficient correlate;
   correlate.addDataArray(numEmployees, numData);
   correlate.addDataArray(stressLevel, numData);
   
   try {
      correlate.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticCorrelationCoefficient threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   problem |= verify("StatisticCorrelationCoefficient Correlation Coefficient R2",
                     correlate.getCorrelationCoefficientR2(),
                     0.765756); 
   problem |= verify("StatisticCorrelationCoefficient Correlation Coefficient R",
                     correlate.getCorrelationCoefficientR(),
                     0.875075); 
   problem |= verify("StatisticCorrelationCoefficient T-Value",
                     correlate.getTValue(),
                     3.13164); // 3.17 in book
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
   StatisticFalseDiscoveryRate fdr(q,
                                   StatisticFalseDiscoveryRate::C_CONSTANT_1);
   fdr.addDataArray(data, numData);
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

   const int numBuckets = 5;
   StatisticHistogram hist(numBuckets);
   hist.addDataArray(data, numData);
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

   StatisticLeveneVarianceEquality levene;
   levene.addDataArray(dataA, numData);
   levene.addDataArray(dataB, numData);
   
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
 * test linear regression.
 */
bool 
StatisticUnitTesting::testLinearRegression()
{
   //
   // Example from Applied Linear Regression Models
   //              John Neter, William Wasserman, and Michael H. Kutner
   //              Second Edition
   //              Page 44
   //
   const int numData = 10;
   const float xi[numData] = { 30, 20, 60, 80, 40, 50, 60, 30, 70, 60 };
   const float yi[numData] = { 73, 50, 128, 170, 87, 108, 135, 69, 148, 132 };
   
   StatisticDataGroup dependentDataGroup(yi, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup independentDataGroup(xi, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticLinearRegression regression;
   regression.setDependentDataArray(yi, numData);
   regression.setIndependentDataArray(xi, numData);

   try {
      regression.execute();
   }
   catch (StatisticException& e) {
      std::cout << "FAILED StatisticLinearRegression threw exception: "
                << e.whatStdString() << std::endl;
      return true;
   }
   
   bool problem = false;

   float b0, b1;
   regression.getRegressionCoefficients(b0, b1);
   problem |= verify("StatisticLinearRegression b0 (intercept)",
                     b0,
                     10.0); 

   problem |= verify("StatisticLinearRegression b1 (slope)",
                     b1,
                     2.0); 

   if (problem == false) {
      std::cout << "PASSED StatisticLinearRegression " << std::endl;
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

   StatisticRankTransformation srt;
   srt.addDataArray(groupA, numData);
   srt.addDataArray(groupB, numData);
   
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
 * test matrix operations.
 */
bool 
StatisticUnitTesting::testMatrixOperations()
{
   bool problem = false;
   
   //
   // Test matrix inverse 2x2
   //
   {
      const float m1Data[4] = { 2, 4, 
                                3, 1 };
      const float m1InverseData[4] = { -0.1, 0.4, 
                                        0.3, -0.2 };
      StatisticMatrix m1(2, 2);
      m1.setMatrixFromOneDimensionalArray(m1Data);
      StatisticMatrix m1Inverse;
      StatisticMatrix m1CorrectInverse(2,2);
      m1CorrectInverse.setMatrixFromOneDimensionalArray(m1InverseData);
      try {
         m1Inverse = m1.inverse();
         problem |= verify("Matrix Inverse (m1)",
                           m1Inverse,
                           m1CorrectInverse);
      }
      catch (StatisticException& e) {
         std::cout << "FAILED: Matrix m1 inverse failed " 
                   << e.whatStdString()
                   << std::endl;
         problem = true;
      }
   }
   
   //
   // Test matrix inverse (4x4)
   //
   {
      const float m2Data[16] = {  2,  0, 1, -1,
                                  1, -1, 0,  2,
                                  0, -1, 2,  1,
                                 -2,  1, 3,  0 };
      const float m2InverseData[16] = { 7,   6,  -5,  1,  // divide all by 18 after
                                        5,  12, -19, 11,  // in StatisticMatrix
                                        3,   0,   3,  3,
                                       -1,  12,  -7,  5 };
      StatisticMatrix m2(4, 4);
      m2.setMatrixFromOneDimensionalArray(m2Data);
      StatisticMatrix m2Inverse;
      StatisticMatrix m2CorrectInverse(4, 4);
      m2CorrectInverse.setMatrixFromOneDimensionalArray(m2InverseData);
      m2CorrectInverse = m2CorrectInverse.multiplyByScalar(1.0 / 18.0);
      //std::cout << "Input matrix: " << std::endl;
      //m2.print(std::cout, "   ");
      try {
         m2Inverse = m2.inverse();
         problem |= verify("Matrix Inverse (m2)",
                           m2Inverse,
                           m2CorrectInverse);
      }
      catch (StatisticException& e) {
         std::cout << "FAILED: Matrix m2 inverse failed " 
                   << e.whatStdString()
                   << std::endl;
         problem = true;
      }
   }
   
   //
   // Test pseudo inverse matrix inverse 2x2
   //
   {
      const float m1Data[4] = {  3.0, -13.0, 
                                -2.0,   9.0 };
      const float m1InverseData[4] = {  9.0, 13.0,
                                        2.0, 3 };
      StatisticMatrix m1(2, 2);
      m1.setMatrixFromOneDimensionalArray(m1Data);
      StatisticMatrix m1Inverse;
      StatisticMatrix m1CorrectInverse(2,2);
      m1CorrectInverse.setMatrixFromOneDimensionalArray(m1InverseData);
      try {
         m1Inverse = m1.inversePseudo();
         problem |= verify("Matrix Pseudo Inverse (m3)",
                           m1Inverse,
                           m1CorrectInverse);
      }
      catch (StatisticException& e) {
         std::cout << "FAILED: Matrix m3 inverse failed " 
                   << e.whatStdString()
                   << std::endl;
         problem = true;
      }
   }
   
   //
   // Test matrix pseudo inverse 2x2 of with linear dependent matrix
   // Inverse values are taken from using Matlab's pinv() on the input matrix
   //
   {
      const float m1Data[4] = { 2, 1, 
                                4, 2 };
      const float m1InverseData[4] = {  0.08, 0.16, 
                                        0.04, 0.08 };
      StatisticMatrix m1(2, 2);
      m1.setMatrixFromOneDimensionalArray(m1Data);
      StatisticMatrix m1Inverse;
      StatisticMatrix m1CorrectInverse(2,2);
      m1CorrectInverse.setMatrixFromOneDimensionalArray(m1InverseData);
      try {
         m1Inverse = m1.inversePseudo();
         problem |= verify("Matrix Pseudo Inverse (m4)",
                           m1Inverse,
                           m1CorrectInverse);
      }
      catch (StatisticException& e) {
         std::cout << "FAILED: Matrix m4 pseudo inverse failed " 
                   << e.whatStdString()
                   << std::endl;
         problem = true;
      }
   }
   
   //
   // Test matrix pseudo inverse 3x3 of with linear dependent matrix
   // Inverse values are taken from using Matlab's pinv() on the input matrix
   //
   {
      const float m1Data[9] = { 1, 2, 3, 
                                7, 4, 7,
                                2, 4, 6 };
      const float m1InverseData[9] = {  -0.0920,  0.2067, -0.1840,
                                         0.0560, -0.0533,  0.1120,
                                         0.0600, -0.0333,  0.1200 };
      StatisticMatrix m1(3, 3);
      m1.setMatrixFromOneDimensionalArray(m1Data);
      StatisticMatrix m1Inverse;
      StatisticMatrix m1CorrectInverse(3,3);
      m1CorrectInverse.setMatrixFromOneDimensionalArray(m1InverseData);
      try {
         m1Inverse = m1.inversePseudo();
         problem |= verify("Matrix Pseudo Inverse (m5)",
                           m1Inverse,
                           m1CorrectInverse);
      }
      catch (StatisticException& e) {
         std::cout << "FAILED: Matrix m5 pseudo inverse failed " 
                   << e.whatStdString()
                   << std::endl;
         problem = true;
      }
   }
   
   //
   // Test Multiply
   //
   {
      const float dataA[4] = { 9, 13,
                               2,  3 };
      StatisticMatrix ma(2, 2);
      ma.setMatrixFromOneDimensionalArray(dataA);
      
      const float dataB[4] = {  7,  5,
                               -3, -2 };
      StatisticMatrix mb(2, 2);
      mb.setMatrixFromOneDimensionalArray(dataB);
      
      const float dataC[4] = { 24, 19,
                                5,  4 };
      StatisticMatrix mc(2, 2);
      mc.setMatrixFromOneDimensionalArray(dataC);
             
      try {
         const StatisticMatrix product = ma.multiply(mb);
         problem |= verify("Matrix Multiply (A*B)",
                           product,
                           mc);
      }
      catch (StatisticException& e) {
         std::cout << "FAILED: Matrix multiply A*B failed " 
                   << e.whatStdString()
                   << std::endl;
         problem = true;
      }
   }
   
   //
   // Test Multiply
   //
   {
      const float dataA[6] = { 3, -1,
                               0,  2,
                               1, -1 };
      StatisticMatrix ma(3, 2);
      ma.setMatrixFromOneDimensionalArray(dataA);
      
      const float dataB[8] = { 2, -1, 0, 1,
                               3,  0, 1, 2 };
      StatisticMatrix mb(2, 4);
      mb.setMatrixFromOneDimensionalArray(dataB);
      
      const float dataC[12] = {  3, -3, -1,  1,
                                 6,  0,  2,  4,
                                -1, -1, -1, -1 };
      StatisticMatrix mc(3, 4);
      mc.setMatrixFromOneDimensionalArray(dataC);
             
      try {
         const StatisticMatrix product = ma.multiply(mb);
         problem |= verify("Matrix Multiply (A*B)",
                           product,
                           mc);
      }
      catch (StatisticException& e) {
         std::cout << "FAILED: Matrix multiply A*B failed " 
                   << e.whatStdString()
                   << std::endl;
         problem = true;
      }
   }
   
   //
   // Test Transpose
   //
   {
      const float data[3] = { 1, 2, 3 };
      StatisticMatrix ma(1, 3);
      ma.setMatrixFromOneDimensionalArray(data);
      StatisticMatrix mb(3, 1);
      mb.setMatrixFromOneDimensionalArray(data);
      
      const StatisticMatrix trans = ma.transpose();
      problem |= verify("Matrix Transpose One-Dimensional",
                        trans,
                        mb);
   }
   
   //
   // Test Transpose
   //
   {
      const float dataA[6] = { 2,  1, -3, 
                               4, -1,  5 };
      StatisticMatrix ma(2, 3);
      ma.setMatrixFromOneDimensionalArray(dataA);
      const float dataB[6] = {  2,  4,
                                1, -1, 
                               -3,  5 };
      StatisticMatrix mb(3, 2);
      mb.setMatrixFromOneDimensionalArray(dataB);
      
      const StatisticMatrix trans = ma.transpose();
      problem |= verify("Matrix Transpose 3x2",
                        trans,
                        mb);
   }
   
   if (problem == false) {
      std::cout << "PASSED StatisticMatrix " << std::endl;
   }
   return problem;
}
      
/**
 * test multiple linear regression.
 */
bool 
StatisticUnitTesting::testMultipleLinearRegression()
{
   bool problem = false;

   {
      //
      // Example from Applied Linear Regression Models
      //              John Neter, William Wasserman, and Michael H. Kutner
      //              Second Edition
      //              Page 44
      //
      // b = Inverse(Xt * X) * Xt * Y
      //
      
      const int numData = 10;
      const float xi[numData] = { 30, 20, 60, 80, 40, 50, 60, 30, 70, 60 };
      const float yi[numData] = { 73, 50, 128, 170, 87, 108, 135, 69, 148, 132 };
      
      StatisticMultipleRegression regression;
      regression.setNumberOfIndependentDataGroups(1);
      regression.setDependentDataArray(yi, numData);
      regression.setIndependentDataArray(0, xi, numData);

      try {
         regression.execute();
      }
      catch (StatisticException& e) {
         std::cout << "FAILED StatisticMultipleRegression 1-X threw exception: "
                   << e.whatStdString() << std::endl;
         return true;
      }
      
      std::vector<float> computedCoefficients, correctCoefficients;
      correctCoefficients.push_back(10.0);
      correctCoefficients.push_back(2.0);
      regression.getRegressionCoefficients(computedCoefficients);

      problem |= verifyCoefficients("Multiple Regression Test 1-X",
                                    computedCoefficients,
                                    correctCoefficients);
   }
                                 
   {
      //
      // Example from Applied Linear Regression Models
      //              John Neter, William Wasserman, and Michael H. Kutner
      //              Second Edition
      //              Page 207, 249-252
      //
      const int numData = 15;
      const float y[numData] = {  162, 120, 223, 131,  67, 
                                  169,  81, 192, 116,  55, 
                                  252, 232, 144, 103, 212 };
      const float x1[numData] = { 274, 180, 375, 205, 86,
                                  265,  98, 330, 195, 53,
                                  430, 372, 236, 157, 370 };
      const float x2[numData] = { 2450, 3254, 3802, 2838, 2347,
                                  3782, 3008, 2450, 2137, 2560,
                                  4020, 4427, 2660, 2088, 2605 };
      
      StatisticMultipleRegression regression;
      regression.setNumberOfIndependentDataGroups(2);
      regression.setDependentDataArray(y, numData);
      regression.setIndependentDataArray(0, x1, numData);
      regression.setIndependentDataArray(1, x2, numData);

      float SSTO, SSE, SSR, MSR, MSE, F, pValue, R2;
      int regressionDOF, errorDOF, totalDOF;
      try {
         regression.execute();
         regression.getAnovaParameters(SSTO, 
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
      }
      catch (StatisticException& e) {
         std::cout << "FAILED StatisticMultipleRegression 2-X threw exception: "
                   << e.whatStdString() << std::endl;
         return true;
      }
      
      std::vector<float> computedCoefficients, correctCoefficients;
      correctCoefficients.push_back(3.452613);
      correctCoefficients.push_back(0.496005);
      correctCoefficients.push_back(0.009199);
      regression.getRegressionCoefficients(computedCoefficients);

      problem |= verifyCoefficients("Multiple Regression Test 2-X",
                                    computedCoefficients,
                                    correctCoefficients);
                                    
      problem |= verify("Multiple Regression Test 2-X SSTO",
                        SSTO,
                        53901.6);
      problem |= verify("Multiple Regression Test 2-X SSE",
                        SSE,
                        56.884);
      problem |= verify("Multiple Regression Test 2-X SSR",
                        SSR,
                        53844.716,
                        0.01);
      problem |= verify("Multiple Regression Test 2-X MSR",
                        MSR,
                        26922.358,
                        0.01);
      problem |= verify("Multiple Regression Test 2-X MSE",
                        MSE,
                        4.740);
      problem |= verify("Multiple Regression Test 2-X F",
                        F,
                        5679.47,  //5680.0);
                        0.01);
      problem |= verify("Multiple Regression Test 2-X P-Value",
                        pValue,
                        0.0);
      problem |= verify("Multiple Regression Test 2-X R2",
                        R2,
                        .9989);
      problem |= verify("Multiple Regression Test 2-X Regression DOF",
                        regressionDOF,
                        2.0);
      problem |= verify("Multiple Regression Test 2-X Error DOF",
                        errorDOF,
                        12.0);
      problem |= verify("Multiple Regression Test 2-X Total DOF",
                        totalDOF,
                        14.0);
   }
                                 
   {
      //
      // Example from Applied Linear Regression Models
      //              John Neter, William Wasserman, and Michael H. Kutner
      //              Second Edition
      //              Page 272
      //
      const int numData = 20;
      const float y[numData] = {  11.9, 22.8, 18.7, 20.1, 12.9, 
                                  21.7, 27.1, 25.4, 21.3, 19.3,
                                  25.4, 27.2, 11.7, 17.8, 12.8,
                                  23.9, 22.6, 25.4, 14.8, 21.1 };
      const float x1[numData] = { 19.5, 24.7, 30.7, 29.8, 19.1,
                                  25.6, 31.4, 27.9, 22.1, 25.5,
                                  31.1, 30.4, 18.7, 19.7, 14.6,
                                  29.5, 27.7, 30.2, 22.7, 25.2 };
      const float x2[numData] = { 43.1, 49.8, 51.9, 54.3, 42.2,
                                  53.9, 58.5, 52.1, 49.9, 53.5,
                                  56.6, 56.7, 46.5, 44.2, 42.7,
                                  54.4, 55.3, 58.6, 48.2, 51.0 };
      const float x3[numData] = { 29.1, 28.2, 37.0, 31.1, 30.9,
                                  23.7, 27.6, 30.6, 23.2, 24.8,
                                  30.0, 28.3, 23.0, 28.6, 21.3,
                                  30.1, 25.7, 24.6, 27.1, 27.5 };
      StatisticMultipleRegression regression;
      regression.setNumberOfIndependentDataGroups(3);
      regression.setDependentDataArray(y, numData);
      regression.setIndependentDataArray(0, x1, numData);
      regression.setIndependentDataArray(1, x2, numData);
      regression.setIndependentDataArray(2, x3, numData);

      try {
         regression.execute();
      }
      catch (StatisticException& e) {
         std::cout << "FAILED StatisticMultipleRegression 3-X threw exception: "
                   << e.whatStdString() << std::endl;
         return true;
      }
      
      std::vector<float> computedCoefficients, correctCoefficients;
      correctCoefficients.push_back(117.084);
      correctCoefficients.push_back(4.334);
      correctCoefficients.push_back(-2.857);
      correctCoefficients.push_back(-2.186);
      regression.getRegressionCoefficients(computedCoefficients);

      problem |= verifyCoefficients("Multiple Regression Test 3-X",
                                    computedCoefficients,
                                    correctCoefficients);
   }
                                 
   {
      //
      // Data from Statistics for Psychology
      //           Arthur Aron & Elaine Aron
      //           2nd Edition, 1999
      //           Table 3-2, page 78
      //           Regression, page 105, 112
      //           T-stats page 99
      const int numData = 5;
      const float numEmployees[numData] = { 6, 8, 3, 10, 8 };
      const float stressLevel[numData]  = { 7, 8, 1,  8, 6 };

      StatisticDataGroup dependentDataGroup(stressLevel, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDataGroup independentDataGroup1(numEmployees, numData, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticMultipleRegression regression;
      regression.setNumberOfIndependentDataGroups(1);
      regression.setDependentDataArray(stressLevel, numData);
      regression.setIndependentDataArray(0, numEmployees, numData);

      float SSTO, SSE, SSR, MSR, MSE, F, pValue, R2;
      int regressionDOF, errorDOF, totalDOF;
      try {
         regression.execute();
         regression.getAnovaParameters(SSTO, 
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
      }
      catch (StatisticException& e) {
         std::cout << "FAILED StatisticMultipleRegression 4-X threw exception: "
                   << e.whatStdString() << std::endl;
         return true;
      }
      
      std::vector<float> computedCoefficients, correctCoefficients;
      correctCoefficients.push_back(-0.75);
      correctCoefficients.push_back(0.964286);
      regression.getRegressionCoefficients(computedCoefficients);

      problem |= verifyCoefficients("Multiple Regression Test 4-X",
                                    computedCoefficients,
                                    correctCoefficients);
                                    
      problem |= verify("Multiple Regression Test 4-X SSTO",
                        SSTO,
                        34);
      problem |= verify("Multiple Regression Test 4-X SSE",
                        SSE,
                        7.96429);
      problem |= verify("Multiple Regression Test 4-X SSR",
                        SSR,
                        26.0357,  
                        0.01);
      problem |= verify("Multiple Regression Test 4-X MSR",
                        MSR,
                        26.04,
                        0.01);
      problem |= verify("Multiple Regression Test 4-X MSE",
                        MSE,
                        2.65476);
      problem |= verify("Multiple Regression Test 4-X F",
                        F,
                        9.80717,  // T=sqrt(9.81) = 3.132 ~= 3.17
                        0.01);
      problem |= verify("Multiple Regression Test 4-X P-Value",
                        pValue,
                        0.052);
      problem |= verify("Multiple Regression Test 4-X R2",
                        R2,
                        0.765756);
      problem |= verify("Multiple Regression Test 4-X Regression DOF",
                        regressionDOF,
                        1.0);
      problem |= verify("Multiple Regression Test 4-X Error DOF",
                        errorDOF,
                        3.0);
      problem |= verify("Multiple Regression Test 4-X Total DOF",
                        totalDOF,
                        4.0);
   }

   if (problem == false) {
      std::cout << "PASSED StatisticMultipleRegression " << std::endl;
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

   StatisticNormalizeDistribution normal(0.0, 1.0);
   normal.addDataArray(data, numData);
   
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

   StatisticNormalizeDistribution normal(0.0, 1.0);
   normal.addDataArray(data, numData);
   
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

   StatisticPermutation perm(StatisticPermutation::PERMUTATION_METHOD_RANDOM_ORDER);
   perm.addDataArray(data, numData);
   
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

   StatisticPermutation perm(StatisticPermutation::PERMUTATION_METHOD_RANDOM_SIGN_FLIP);
   perm.addDataArray(data, numData);
   
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
