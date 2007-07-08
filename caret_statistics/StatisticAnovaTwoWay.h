
#ifndef __STATISTIC_ANOVA_TWO_WAY_H__
#define __STATISTIC_ANOVA_TWO_WAY_H__

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

/// this class performs a two way analysis of variance
class StatisticAnovaTwoWay : public StatisticAlgorithm {
   public:
      /// ANOVA model type
      enum ANOVA_MODEL_TYPE {
         /// invalid type
         ANOVA_MODEL_TYPE_INVALID,
         /// fixed effect
         ANOVA_MODEL_TYPE_FIXED_EFFECT,
         /// random effect
         ANOVA_MODEL_TYPE_RANDOM_EFFECT,
         /// mixed effect (A=fixed, B=random)
         ANOVE_MODEL_TYPE_MIXED_EFFECT
      };
      
      // constructor
      StatisticAnovaTwoWay();

      // destructor
      ~StatisticAnovaTwoWay();

      // execute the algorithm
      void execute() throw (StatisticException);

      // set the ANOVA model type
      void setAnovaModelType(const ANOVA_MODEL_TYPE amt);
      
      // set the number of factor levels 
      void setNumberOfFactorLevels(const int factorLevelsInGroupA,
                                   const int factorLevelsInGroupB);
              
      // set a data group (call after "setNumberOfFactorLevels()")
      void setDataGroup(const int factorLevelA,
                        const int factorLevelB,
                        StatisticDataGroup* dataGroup,
                        const bool takeOwnershipOfThisDataGroup = false);
       
      /// get treatment sum of squares
      double getSumOfSquaresTreatmentSSTR() const { return sumOfSquaresTreatmentSSTR; }
      
      /// get error sum of squares
      double getSumOfSquaresErrorSSE() const { return  sumOfSquaresErrorSSE; }
      
      /// get total sum of squares
      double getSumOfSquaresTotalSSTO() const { return  sumOfSquaresTotalSSTO; }
      
      /// get sum of squares for factor A
      double getSumOfSquaresSSA() const { return sumOfSquaresSSA; }
      
      /// get sum of squares for factor B
      double getSumOfSquaresSSB() const { return sumOfSquaresSSB; }
      
      /// get sum of squares for interaction of factors A & B
      double getSumOfSquaresSSAB() const { return sumOfSquaresSSAB; }
      
      /// get the mean of all data values
      double getMeanOfAllValues() const { return meanOfAllY; }
      
      /// get a cell mean
      double getCellMean(const int factorLevelA,
                         const int factorLevelB) const;
            
      /// get a factor level A mean
      double getMeanFactorLevelA(const int factorLevelA) const;
      
      /// get a factor level B mean
      double getMeanFactorLevelB(const int factorLevelB) const;
      
      /// degrees of freedom factor A
      double getDegreesOfFreedomFactorA() const { return degreesOfFreedomFactorA; }

      /// degrees of freedom factor B
      double getDegreesOfFreedomFactorB() const { return degreesOfFreedomFactorB; }

      /// degrees of freedom interactions
      double getDegreesOfFreedomInteractions() const { return degreesOfFreedomInteractions; }

      /// degrees of freedom between treatments
      double getDegreesOfFreedomBetweenTreatments() const { return degreesOfFreedomBetweenTreatments; }

      /// degrees of freedom error
      double getDegreesOfFreedomError() const { return degreesOfFreedomError; }

      /// degrees of freedom total
      double getDegreesOfFreedomTotal() const { return degreesOfFreedomTotal; }
      
      /// mean square factor A
      double getMeanSquareFactorA_MSA() const { return meanSquareFactorA_MSA; }

      /// mean square factor B
      double getMeanSquareFactorB_MSB() const { return meanSquareFactorB_MSB; }

      /// mean square interaction
      double getMeanSquareInteractionMSAB() const { return meanSquareInteractionMSAB; }

      /// mean square between treatments
      double getMeanSquareBetweenTreatmentsMSTR() const { return meanSquareBetweenTreatmentsMSTR; }

      /// mean square error
      double getMeanSquareErrorMSE() const { return meanSquareErrorMSE; }
      
      /// F-Statistic Factor A
      double getFStatisticFactorA() const { return fStatisticFactorA; }

      /// F-Statistic Factor B
      double getfStatisticFactorB() const { return fStatisticFactorB; }

      /// F-Statistic Interation
      double getfStatisticInteraction() const { return fStatisticInteraction; }
      
      /// P-Value for Factor A
      double getPValueFactorA() const { return pValueFactorA; }
      
      /// P-Value for Factor B
      double getPValueFactorB() const { return pValueFactorB; }
      
      /// P-Value for Interaction
      double getPValueInteraction() const { return pValueInteraction; }
      
   protected:
      // get a data group
      StatisticDataGroup* getDataGroup(const int factorLevelA,
                                       const int factorLevelB);
                                       
      // get single data group index
      int getDataGroupIndex(const int factorLevelA,
                            const int factorLevelB) const;

      /// number of factor levels in group A
      int numberOfFactorLevelsGroupA;
      
      /// number of factor levels in group B
      int numberOfFactorLevelsGroupB;
      
      /// ANOVA model type
      ANOVA_MODEL_TYPE anovaModelType;
      
      /// treatment sum of squares
      double sumOfSquaresTreatmentSSTR;
      
      /// error sum of squares
      double sumOfSquaresErrorSSE;
      
      /// total sum of squares
      double sumOfSquaresTotalSSTO;
      
      /// sum of squares for factor A
      double sumOfSquaresSSA;
      
      /// sum of squares for factor B
      double sumOfSquaresSSB;
      
      /// sum of squares for interaction of factors A & B
      double sumOfSquaresSSAB;
      
      /// means of the cells
      double* cellMeans_Yij;
      
      /// mean of all values
      double meanOfAllY;
      
      /// mean of each row (group A factor level)
      double* meanOfFactorLevelsGroupA_Yi;
      
      /// mean of each column (group B factor level)
      double* meanOfFactorLevelsGroupB_Yj;
      
      /// degrees of freedom factor A
      double degreesOfFreedomFactorA;

      /// degrees of freedom factor B
      double degreesOfFreedomFactorB;

      /// degrees of freedom interactions
      double degreesOfFreedomInteractions;

      /// degrees of freedom between treatments
      double degreesOfFreedomBetweenTreatments;

      /// degrees of freedom error
      double degreesOfFreedomError;

      /// degrees of freedom total
      double degreesOfFreedomTotal;
      
      /// mean square factor A
      double meanSquareFactorA_MSA;

      /// mean square factor B
      double meanSquareFactorB_MSB;

      /// mean square interaction
      double meanSquareInteractionMSAB;

      /// mean square between treatments
      double meanSquareBetweenTreatmentsMSTR;

      /// mean square error
      double meanSquareErrorMSE;
      
      /// F-Statistic Factor A
      double fStatisticFactorA;

      /// F-Statistic Factor B
      double fStatisticFactorB;

      /// F-Statistic Interation
      double fStatisticInteraction;
      
      /// P-Value for Factor A
      double pValueFactorA;
      
      /// P-Value for Factor B
      double pValueFactorB;
      
      /// P-Value for Interaction
      double pValueInteraction;

};

#endif // __STATISTIC_ANOVA_TWO_WAY_H__
