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
#include "StatisticFalseDiscoveryRate.h"

/**
 * constructor The input data
 * is expected to be group(s) of P-Values.
 */
StatisticFalseDiscoveryRate::StatisticFalseDiscoveryRate(const float qIn,
                                                         const C_CONSTANT cConstantIn)
{
   pCutoff = 0.0;
   q = qIn;
   cConstant = cConstantIn;
}
                   
/**
 * destructor.
 */
StatisticFalseDiscoveryRate::~StatisticFalseDiscoveryRate()
{
}

/**
 * execute the algorthm (throws exception if an error occurs).
 */
void 
StatisticFalseDiscoveryRate::execute() throw (StatisticException)
{
   if (getNumberOfDataGroups() < 1) {
      throw StatisticException("StatisticFalseDiscoveryRate requires at least one data group.");
   }
   
   pCutoff = 0.0;
   
   if ((q < 0.0) || (q > 1.0)) {
      throw StatisticException("\"q\" must be between 0.0 and 1.0 inclusively");
   }
   
   //
   // copy P-Values and sort them
   //
   std::vector<float> pValuesSorted;
   getAllDataValues(pValuesSorted, true);
   const int numPValues = static_cast<int>(pValuesSorted.size());
   
   if (numPValues <= 0) {
      throw StatisticException("Number of values is less than or equal to zero.");
   }
   
   //
   // Determine the constant "C"
   //
   float c = 0.0;
   switch (cConstant) {
      case C_CONSTANT_1:
         c = 1.0;
         break;
      case C_CONSTANT_SUMMATION:
         {
            c = 0.0;
            for (int i = 1; i <= numPValues; i++) {
               c += (1.0 / static_cast<float>(i));
            }
         }
         break;
   }
   
   //
   // Pre-compute part of the significance cutoff (item 3, page 872)
   //
   const float partOfSig = q / (static_cast<float>(numPValues) * c);
   
   //
   // Determine the significance cutoff
   //
   int pCutoffIndex = 0;
   for (int i = 0; i < numPValues; i++) {
      const float pr = static_cast<float>(i+1) * partOfSig;
      if (pValuesSorted[i] <= pr) {
         pCutoffIndex = i;
      }
   }
   pCutoff = pValuesSorted[pCutoffIndex];
}
