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

#include "StatisticDataGroup.h"
#include "StatisticRankTransformation.h"

/**
 * constructor.
 *
 * IMPORTANT: The output data replaces the input data so be sure to create
 * the input StatisticDataGroup with DATA_STORAGE_MODE_POINT.
 */
StatisticRankTransformation::StatisticRankTransformation()
{
}

/**
 * destructor.
 */
StatisticRankTransformation::~StatisticRankTransformation()
{
   for (unsigned int i = 0; i < outputDataGroupContainingRankValues.size(); i++) {
      if (outputDataGroupContainingRankValues[i] != NULL) {
         delete outputDataGroupContainingRankValues[i];
         outputDataGroupContainingRankValues[i] = NULL;
      }
   }
}

/**
 * convert the input arrays to ranks.
 */
void 
StatisticRankTransformation::execute() throw (StatisticException)
{
   const int numDataGroups = getNumberOfDataGroups();
   if (numDataGroups <= 0) {
      throw StatisticException("StatisticRankTransformation requires at least one data group.");
   }
   
   
   //
   // Loop through data groups
   //
   for (int i = 0; i < numDataGroups; i++) {
      //
      // Get array and count
      //
      const StatisticDataGroup* sdg = getDataGroup(i);
      const float* ptr = sdg->getPointerToData();
      const int numElements = sdg->getNumberOfData();
      
      //
      // load array values into rank sum structure
      //
      for (int j = 0; j < numElements; j++) {
         ranks.push_back(RankOrder(i, ptr[j], j));
      }
   }
   
   //
   // Make sure there is data
   //
   if (ranks.empty()) {
      throw StatisticException("No data supplied to RankTransformation.");
   }
   
   //
   // Sort by the array values
   //
   std::sort(ranks.begin(), ranks.end());
   
   //
   // set the ranks which start at a value of 1.0
   //
   const int numRanks = static_cast<int>(ranks.size());
   for (int i = 0; i < numRanks; i++) {
      ranks[i].rank = i + 1.0;
   }
   
   //
   // Adjust ranks for duplicates
   //
   processDuplicates(ranks);
   
   
   //
   // Create output data groups
   //
   for (int dataArrayNumber = 0; dataArrayNumber < numDataGroups; dataArrayNumber++) {
      //
      // Create an array for input data group
      //
      const int numElements = getDataGroup(dataArrayNumber)->getNumberOfData();
      float* rankValues = new float[numElements];
      
      //
      // load the output array
      //
      for (int j = 0; j < numRanks; j++) {
         const RankOrder& rank = ranks[j];
         if (rank.arrayNumber == dataArrayNumber) {
            rankValues[rank.valuesArrayIndex] = rank.rank;
         }
      }
      StatisticDataGroup* sdg = new StatisticDataGroup(rankValues,
                                                       numElements,
                                                       StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP);
      outputDataGroupContainingRankValues.push_back(sdg);
   }
}

/**
 * Constructor.
 */
StatisticRankTransformation::RankOrder::RankOrder(const int arrayNumberIn,
                     const float valueIn,
                     const int valuesArrayIndexIn)
{
   value       = valueIn;
   arrayNumber = arrayNumberIn;
   valuesArrayIndex = valuesArrayIndexIn;
   rank = 0;
}

//
// All duplicate ranks receive the same ranking
//
void
StatisticRankTransformation::processDuplicates(std::vector<RankOrder>& ranks)
{
   const int num = static_cast<int>(ranks.size());   
   if (num <= 1) {
      return;
   }
   
   //
   // Keep track of starting and ending indices of ranks with same value
   //
   float currentValue = ranks[0].value;
   int currentValueStart = 0;
   int currentValueEnd   = 0;
   
   //
   // loop through remaining items
   //
   for (int i = 1; i < num; i++) {
      //
      // Is this value same as previous values
      //
      if (ranks[i].value == currentValue) {
         //
         // Update ending index
         //
         currentValueEnd = i;
      }
      else {
         //
         // new value different than last?
         //
         if (currentValueStart != currentValueEnd) {
            //
            // Determine the average rank and assign it
            // to all ranks with same value
            //
            float sum = 0;
            for (int j = currentValueStart; j <= currentValueEnd; j++) {
               sum += ranks[j].rank;
            }
            const float numRanks = currentValueEnd - currentValueStart + 1;
            const float averageRank = sum / numRanks;
            for (int j = currentValueStart; j <= currentValueEnd; j++) {
               ranks[j].rank = averageRank;
            }
         }
         
         //
         // New value encountered
         //
         currentValue = ranks[i].value;
         currentValueStart = i;
         currentValueEnd   = i;
      }
   }
   
   //
   // Handle instance when the last few values are the same
   //
   if (currentValueStart != currentValueEnd) {
      float sum = 0;
      for (int j = currentValueStart; j <= currentValueEnd; j++) {
         sum += ranks[j].rank;
      }
      const float numRanks = currentValueEnd - currentValueStart + 1;
      const float averageRank = sum / numRanks;
      for (int j = currentValueStart; j <= currentValueEnd; j++) {
         ranks[j].rank = averageRank;
      }
   }
}
