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

#include "StatisticAnovaTwoWay.h"
#include "StatisticDataGroup.h"
#include "StatisticGeneratePValue.h"

/**
 * constructor.
 * Note: Factor A is in the rows, Factor B is in the columns
 */
StatisticAnovaTwoWay::StatisticAnovaTwoWay()
{
   cellMeans_Yij = NULL;
   meanOfFactorLevelsGroupA_Yi = NULL;
   meanOfFactorLevelsGroupB_Yj = NULL;
   
   meanOfAllY = 0.0;
   numberOfFactorLevelsGroupA = 0;
   numberOfFactorLevelsGroupB = 0;
}

/**
 * destructor.
 */
StatisticAnovaTwoWay::~StatisticAnovaTwoWay()
{
   if (meanOfFactorLevelsGroupA_Yi != NULL) {
      delete[] meanOfFactorLevelsGroupA_Yi;
      meanOfFactorLevelsGroupA_Yi = NULL;
   }
   if (meanOfFactorLevelsGroupB_Yj != NULL) {
      delete[] meanOfFactorLevelsGroupB_Yj;
      meanOfFactorLevelsGroupB_Yj = NULL;
   }
   if (cellMeans_Yij != NULL) {
      delete[] cellMeans_Yij;
      cellMeans_Yij = NULL;
   }
}

/**
 * execute the algorithm.
 * Formulas are from Analysis of Variance for FMRI Data
 * By Douglas Ward (with modifications by Gang Chen)
 * January 10, 2006
 * http://afni.nimh.nih.gov/afni/doc/manual/ANOVA
 */
void 
StatisticAnovaTwoWay::execute() throw (StatisticException)
{
   sumOfSquaresTreatmentSSTR = 0.0;
   sumOfSquaresErrorSSE = 0.0;
   sumOfSquaresTotalSSTO = 0.0;
   sumOfSquaresSSA = 0.0;
   sumOfSquaresSSB = 0.0;
   sumOfSquaresSSAB = 0.0;

   switch (anovaModelType) {
      case ANOVA_MODEL_TYPE_INVALID:
         throw StatisticException("ANOVA model type is invalid.");
         break;
      case ANOVA_MODEL_TYPE_FIXED_EFFECT:
         break;
      case ANOVA_MODEL_TYPE_RANDOM_EFFECT:
         break;
      case ANOVE_MODEL_TYPE_MIXED_EFFECT:
         break;
   }
   
   //
   // verify that there are factor levels
   //
   if ((numberOfFactorLevelsGroupA < 2) ||
       (numberOfFactorLevelsGroupB < 2)) {
      throw StatisticException("Both groups must have at least two factor levels.");
   }
   
   //
   // Verify that all arrays exist
   //
   for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
      for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
         if (getDataGroup(i, j) == NULL) {
            std::string msg("Factor level ("
                        + numberToString(i)
                        + ", "
                        + numberToString(j)
                        + ") is invalid (NULL).");
            throw StatisticException(msg);
         }
      }
   }
   
   //
   // Verify that sample sizes are the same for all factor level combinations
   //
   const int sampleSize = getDataGroup(0, 0)->getNumberOfData();
   for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
      for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
         if (getDataGroup(i, j)->getNumberOfData() != sampleSize) {
            std::string msg("Factor level ("
                        + numberToString(i)
                        + ", "
                        + numberToString(j)
                        + ") has a different sample size than (1, 1).");
            throw StatisticException(msg);
         }
      }
   }

   if (numberOfFactorLevelsGroupA <= 0) {
      throw StatisticException("Number of factor levels in Group A is invalid.");
   }
   if (numberOfFactorLevelsGroupB <= 0) {
      throw StatisticException("Number of factor levels in Group B is invalid.");
   }

   //
   // Number of cells (interactions)
   //
   const int numberOfCells = getNumberOfDataGroups();
   const double numberOfCellsFloat = numberOfCells;
   
   //
   // Determine sums and means for all cells
   //
   double allCellsSum = 0.0;
   double* cellSums  = new double[numberOfCells];
   cellMeans_Yij = new double[numberOfCells];
   for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
      for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
         const int indx = getDataGroupIndex(i, j);
         cellSums[indx] = getDataGroup(i, j)->getSumOfData();
         cellMeans_Yij[indx] = cellSums[indx] / static_cast<double>(sampleSize);
         allCellsSum += cellSums[indx];
      }
   }
   
   //
   // Mean for all group A's factor levels
   //
   meanOfFactorLevelsGroupA_Yi = new double[numberOfFactorLevelsGroupA];
   for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
      double sum = 0.0;
      for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
         const int indx = getDataGroupIndex(i, j);
         sum += cellSums[indx];
      }
      const double numData = numberOfFactorLevelsGroupB * sampleSize;
      meanOfFactorLevelsGroupA_Yi[i] = sum / numData;
   }
   
   //
   // Mean for all group B's factor levels
   //
   meanOfFactorLevelsGroupB_Yj = new double[numberOfFactorLevelsGroupB];
   for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
      double sum = 0.0;
      for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
         const int indx = getDataGroupIndex(i, j);
         sum += cellSums[indx];
      }
      const double numData = numberOfFactorLevelsGroupA * sampleSize;
      meanOfFactorLevelsGroupB_Yj[j] = sum / numData;
   }
   
   //
   // Overall Mean
   //
   meanOfAllY = allCellsSum / static_cast<double>(numberOfCellsFloat * sampleSize);
   
   //
   // Sum of Squares Total, Treatement, Error
   //
   for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
      for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
         const int indx = getDataGroupIndex(i, j);
         const StatisticDataGroup* sdg = getDataGroup(i, j);
         
         for (int k = 0; k < sampleSize; k++) {
            //
            // Get the data element
            //
            const float dataValue = sdg->getData(k);
            
            //
            // Add into total sum of squares
            //
            const double dto = dataValue - meanOfAllY;
            sumOfSquaresTotalSSTO += (dto * dto);
            
            //
            //  Add into error sum of squares
            //
            const double de = dataValue - cellMeans_Yij[indx];
            sumOfSquaresErrorSSE += (de * de);
         }
         
         //
         // Add into treatment sum of squares
         //
         const double d = cellMeans_Yij[indx] - meanOfAllY;
         sumOfSquaresTreatmentSSTR += (d * d);
      }
   }
   //
   // Lastly, multiply by sample size (number of elements in each cell)
   //
   sumOfSquaresTreatmentSSTR *= sampleSize;
   
   //
   // Sum of squares for factor level A
   //
   for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
      const double d = meanOfFactorLevelsGroupA_Yi[i] - meanOfAllY;
      sumOfSquaresSSA += (d * d);
   }
   sumOfSquaresSSA *= (sampleSize * numberOfFactorLevelsGroupB);
   
   //
   // Sum of squares for factor level B
   //
   for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
      const double d = meanOfFactorLevelsGroupB_Yj[j] - meanOfAllY;
      sumOfSquaresSSB += (d * d);
   }
   sumOfSquaresSSB *= (sampleSize * numberOfFactorLevelsGroupA);
   
   //
   // Sum of squares for interaction of factors A & B
   //
   for (int i = 0; i < numberOfFactorLevelsGroupA; i++) {
      for (int j = 0; j < numberOfFactorLevelsGroupB; j++) {
         const float d = (getCellMean(i, j)
                          - getMeanFactorLevelA(i)
                          - getMeanFactorLevelB(j)
                          + meanOfAllY);
         sumOfSquaresSSAB += (d * d);
      }
   }
   sumOfSquaresSSAB *= sampleSize;
   
   //
   // Set degrees of freedom
   //
   degreesOfFreedomFactorA = numberOfFactorLevelsGroupA - 1;
   degreesOfFreedomFactorB = numberOfFactorLevelsGroupB - 1;
   degreesOfFreedomInteractions = degreesOfFreedomFactorA 
                                  * degreesOfFreedomFactorB;
   degreesOfFreedomBetweenTreatments = (numberOfFactorLevelsGroupA
                                        * numberOfFactorLevelsGroupB) 
                                       - 1.0;
   degreesOfFreedomError = (numberOfFactorLevelsGroupA
                            * numberOfFactorLevelsGroupB)
                           * (sampleSize - 1);
   degreesOfFreedomTotal = (sampleSize
                            * numberOfFactorLevelsGroupA
                            * numberOfFactorLevelsGroupB)
                           - 1.0;
                           
   //
   // Mean Squares
   //
   meanSquareFactorA_MSA = sumOfSquaresSSA / degreesOfFreedomFactorA;
   meanSquareFactorB_MSB = sumOfSquaresSSB / degreesOfFreedomFactorB;
   meanSquareInteractionMSAB = sumOfSquaresSSAB / degreesOfFreedomInteractions;
   meanSquareBetweenTreatmentsMSTR = sumOfSquaresTreatmentSSTR / degreesOfFreedomBetweenTreatments;
   meanSquareErrorMSE = sumOfSquaresErrorSSE / degreesOfFreedomError;
   
   //
   // F-Statistics
   // The only difference between the models is the F-Statistic calculation
   //
   switch (anovaModelType) {
      case ANOVA_MODEL_TYPE_INVALID:
         break;
      case ANOVA_MODEL_TYPE_FIXED_EFFECT:
         fStatisticFactorA = meanSquareFactorA_MSA / meanSquareErrorMSE;
         fStatisticFactorB = meanSquareFactorB_MSB / meanSquareErrorMSE;
         fStatisticInteraction = meanSquareInteractionMSAB / meanSquareErrorMSE;
         break;
      case ANOVA_MODEL_TYPE_RANDOM_EFFECT:
         fStatisticFactorA = meanSquareFactorA_MSA / meanSquareInteractionMSAB;
         fStatisticFactorB = meanSquareFactorB_MSB / meanSquareInteractionMSAB;
         fStatisticInteraction = meanSquareInteractionMSAB / meanSquareErrorMSE;
         break;
      case ANOVE_MODEL_TYPE_MIXED_EFFECT:
         fStatisticFactorA = meanSquareFactorA_MSA / meanSquareInteractionMSAB;
         fStatisticFactorB = meanSquareFactorB_MSB / meanSquareErrorMSE;
         fStatisticInteraction = meanSquareInteractionMSAB / meanSquareErrorMSE;
         break;
   }

   //
   // Determine P-Values
   //
   pValueFactorA = 
        StatisticGeneratePValue::getFStatisticPValue(degreesOfFreedomFactorA,
                                                     degreesOfFreedomError,
                                                     fStatisticFactorA);
   pValueFactorB = 
        StatisticGeneratePValue::getFStatisticPValue(degreesOfFreedomFactorB,
                                                     degreesOfFreedomError,
                                                     fStatisticFactorB);
   pValueInteraction = 
        StatisticGeneratePValue::getFStatisticPValue(degreesOfFreedomInteractions,
                                                     degreesOfFreedomError,
                                                     fStatisticInteraction);
/*
   //
   // Determine P-Values
   //
   int which = 1;
   double p = 0.0;
   double q = 0.0;
   double f = fStatisticFactorA;
   double dfn = degreesOfFreedomFactorA;
   double dfd = degreesOfFreedomError;
   int status = 0;
   double bound = 0;
   cdff(&which,
        &p,
        &q,
        &f,
        &dfn,
        &dfd,
        &status,
        &bound);
   pValueFactorA = q;
   
   which = 1;
   p = 0.0;
   q = 0.0;
   f = fStatisticFactorB;
   dfn = degreesOfFreedomFactorB;
   dfd = degreesOfFreedomError;
   status = 0;
   bound = 0;
   cdff(&which,
        &p,
        &q,
        &f,
        &dfn,
        &dfd,
        &status,
        &bound);
   pValueFactorB = q;
   
   which = 1;
   p = 0.0;
   q = 0.0;
   f = fStatisticInteraction;
   dfn = degreesOfFreedomInteractions;
   dfd = degreesOfFreedomError;
   status = 0;
   bound = 0;
   cdff(&which,
        &p,
        &q,
        &f,
        &dfn,
        &dfd,
        &status,
        &bound);
   pValueInteraction = q;
*/

   delete[] cellSums;
}

/**
 * set the ANOVA model type.
 */
void 
StatisticAnovaTwoWay::setAnovaModelType(const ANOVA_MODEL_TYPE amt)
{
   anovaModelType = amt;
}
      
/**
 * set the number of factor levels.
 */
void 
StatisticAnovaTwoWay::setNumberOfFactorLevels(const int factorLevelsInGroupA,
                                              const int factorLevelsInGroupB)
{
   numberOfFactorLevelsGroupA = factorLevelsInGroupA;
   numberOfFactorLevelsGroupB = factorLevelsInGroupB;
   const int numberOfDataGroups = numberOfFactorLevelsGroupA 
                                  * numberOfFactorLevelsGroupB;
   setNumberOfDataGroups(numberOfDataGroups);
}                                   

/**
 * set a data group (call after "setNumberOfFactorLevels()").
 */
void 
StatisticAnovaTwoWay::setDataGroup(const int factorLevelA,
                                   const int factorLevelB,
                                   StatisticDataGroup* dataGroup,
                                   const bool takeOwnershipOfThisDataGroup)
{
   const int indx = getDataGroupIndex(factorLevelA, factorLevelB);
   if (indx >= 0) {
      StatisticAlgorithm::setDataGroup(indx, 
                   dataGroup,
                   takeOwnershipOfThisDataGroup);
   } 
}

/**
 * get a data group.
 */
StatisticDataGroup* 
StatisticAnovaTwoWay::getDataGroup(const int factorLevelA,
                                   const int factorLevelB)
{
   const int indx = getDataGroupIndex(factorLevelA, factorLevelB);
   if (indx >= 0) {
      return StatisticAlgorithm::getDataGroup(indx);
   }
   
   return NULL;
}
                                       
/**
 * get single data group index.
 */
int 
StatisticAnovaTwoWay::getDataGroupIndex(const int factorLevelA,
                                        const int factorLevelB) const
{
   const int indx = (factorLevelA * numberOfFactorLevelsGroupB) 
                    + factorLevelB;
   if ((indx < 0) ||
       (indx >= getNumberOfDataGroups())) {
      return -1;
   }
   
   return indx;
}

/**
 * get a cell mean.
 */
double 
StatisticAnovaTwoWay::getCellMean(const int factorLevelA,
                                  const int factorLevelB) const
{
   const int indx = (factorLevelA * numberOfFactorLevelsGroupB) 
                    + factorLevelB;
   if ((indx < 0) ||
       (indx >= getNumberOfDataGroups())) {
      return 0.0;
   }
   
   return cellMeans_Yij[indx];
}

/**
 * get a factor level A mean.
 */
double 
StatisticAnovaTwoWay::getMeanFactorLevelA(const int factorLevelA) const
{
   return meanOfFactorLevelsGroupA_Yi[factorLevelA];
}

/**
 * get a factor level B mean.
 */
double 
StatisticAnovaTwoWay::getMeanFactorLevelB(const int factorLevelB) const
{
   return meanOfFactorLevelsGroupB_Yj[factorLevelB];
}
