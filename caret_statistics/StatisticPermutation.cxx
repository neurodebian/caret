
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
#include <vector>

#include "StatisticDataGroup.h"
#include "StatisticPermutation.h"
#include "StatisticRandomNumber.h"
#include "StatisticRandomNumberOperator.h"

/**
 * constructor.
 */
StatisticPermutation::StatisticPermutation(const PERMUTATION_METHOD permutationMethodIn)
   : StatisticAlgorithm("Permutation")
{
   permutationMethod = permutationMethodIn;
   outputDataGroup = NULL;
}

/**
 * destructor.
 */
StatisticPermutation::~StatisticPermutation()
{
   if (outputDataGroup != NULL) {
      delete outputDataGroup;
      outputDataGroup = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
StatisticPermutation::execute() throw (StatisticException)
{
   //
   // Verify the input data group
   //
   if (getNumberOfDataGroups() != 1) {
      throw StatisticException("Normalization only allows one data group.");
   }
   StatisticDataGroup* sdg = getDataGroup(0);
   const int numValues = sdg->getNumberOfData();
   if (numValues <= 0) {
      throw StatisticException("Normalization data group has no values");
   }
   
   //
   // Copy the data
   //
   std::vector<float>* outputVector = new std::vector<float>;
   for (int i = 0; i < numValues; i++) {
      outputVector->push_back(sdg->getData(i));
   }
   
   //
   // Apply the appropriate algorithm to the values
   //
   switch (permutationMethod) {
      case PERMUTATION_METHOD_RANDOM_SIGN_FLIP:
         { 
            //
            // randomly flip signs of values
            //
            for (int i = 0; i < numValues; i++) {
               if (StatisticRandomNumber::randomInteger(-1000, 1000) < 0) {
                  (*outputVector)[i] = -(*outputVector)[i];
               }
            }
         }
         break;
      case PERMUTATION_METHOD_RANDOM_ORDER:
         {
            //
            // Randomly shuffle the values
            //
            StatisticRandomNumberOperator randOp;
            std::random_shuffle(outputVector->begin(),
                                outputVector->end(),
                                randOp);
         }
         break;
   }
   
   //
   // Create the output data group
   //
   outputDataGroup = new StatisticDataGroup(outputVector,
                                            StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP);
}

