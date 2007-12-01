
#ifndef __STATISTIC_UNIT_TESTING_H__
#define __STATISTIC_UNIT_TESTING_H__

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

#include "StatisticAlgorithm.h" 

class StatisticMatrix;

/// class that performs unit testing of the statistical routines
class StatisticUnitTesting : public StatisticAlgorithm {
   public:
      // constructor
      StatisticUnitTesting(const bool printTestValuesFlagIn);
      
      // destructor
      ~StatisticUnitTesting();
      
      // run the tests
      void execute() throw (StatisticException);
      
      /// get problems occurred during testing
      bool getProblemsOccurredDuringTesting() const { return problemFlag; }
      
   protected:
      // verify that two floating point numbers are nearly identical (false if ok)
      bool verify(const std::string& testName,
                  const float computedValue,
                  const float correctValue,
                  const float acceptableDifference = 0.001);
                  
      // verify that two matrices numbers are nearly identical (false if ok)
      bool verify(const std::string& testName,
                  const StatisticMatrix& computedMatrix,
                  const StatisticMatrix& correctMatrix,
                  const float acceptableDifference = 0.001);
           
      // verify that a group of coefficients are nearly identical (false if ok)
      bool verifyCoefficients(const std::string& testName,
                   const std::vector<float>& computedCoefficients,
                   const std::vector<float>& correctCoefficients,
                   const float acceptableDifference = 0.001);
                   
      // test mean, variance, deviation
      bool testStatisticMeanAndDeviation();
      
      /// test descriptive statistics
      bool testStatisticDescriptive();
      
      // test one-sample T-Test
      bool testStatisticTtestOneSample();
      
      // test Paired T-Test.
      bool testStatisticTtestPaired();

      // test two-sample T-Test with pooled variance
      bool testStatisticTtestTwoSamplePooledVariance();
      
      // test two-sample T-Test with unpooled variance
      bool testStatisticTtestTwoSampleUnpooledVariance();
      
      // test one-way ANOVA
      bool testStatisticAnovaOneWay();
      
      // test two-way fixed-effect ANOVA
      bool testStatisticAnovaTwoWayFixedEffectCase1();
      
      // test two-way fixed-effect ANOVA
      bool testStatisticAnovaTwoWayFixedEffectCase2();
      
      // test two-way random-effect ANOVA
      bool testStatisticAnovaTwoWayRandomEffect();
      
      // test two-way mixed-effect ANOVA
      bool testStatisticAnovaTwoWayMixedEffect();
      
      // test convert to z-score
      bool testConvertToZScore();
      
      // test correlation coefficient
      bool testCorrelationCoefficient();
      
      // test false discovery rate
      bool testFalseDiscoveryRate();
      
      // test histogram
      bool testHistogram();
      
      // test kruskal-wallis non-parameteric anova
      bool testKruskalWallis();
      
      // test levene's test
      bool testLevenesTest();
      
      // test linear regression
      bool testLinearRegression();
      
      // test matrix operations
      bool testMatrixOperations();
      
      // test multiple linear regression
      bool testMultipleLinearRegression();
      
      // test normalization of a distribution of sorted values
      bool testNormalizeDistributionSorted();
      
      // test normalization of a distribution of unsorted values
      bool testNormalizeDistributionUnsorted();
      
      // test rank transformation
      bool testRankTransformation();
      
      // test permutation random shuffle
      bool testPermutationRandomShuffle();
      
      // test permutation sign flipping
      bool testPermutationSignFlipping();
      
      // test value/index sorting
      bool testValueIndexSort();
      
      // problems encountered in testing
      bool problemFlag;
      
      // print test values even if correct
      bool printTestValuesFlag;
};

#endif // __STATISTIC_UNIT_TESTING_H__
