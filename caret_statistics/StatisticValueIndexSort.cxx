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
#include "StatisticValueIndexSort.h"

/**
 * Constructor.
 */
StatisticValueIndexSort::StatisticValueIndexSort()
{
}
              
/**
 * Constructor.
 */
StatisticValueIndexSort::~StatisticValueIndexSort()
{
}

/**
 * execute the algorithm.
 */
void 
StatisticValueIndexSort::execute() throw (StatisticException)
{
   //
   // Verify the input data group
   //
   if (getNumberOfDataGroups() != 1) {
      throw StatisticException("Value/Indx Sort only allows one data group.");
   }
   const StatisticDataGroup* sdg = getDataGroup(0);
   const int numValues = sdg->getNumberOfData();
   if (numValues <= 0) {
      throw StatisticException("Value/Indx Sort data group has no values");
   }

   //
   // Get the data and sort it
   //   
   values.clear();
   for (int i = 0; i < numValues; i++) {
      values.push_back(ValueIndexPair(i, sdg->getData(i)));
   }
   std::sort(values.begin(), values.end());
}
      
/**
 * get number of value indice pairs.
 */
int 
StatisticValueIndexSort::getNumberOfItems() const 
{ 
   return values.size(); 
}

/**
 * get value and index for an item.
 */
void 
StatisticValueIndexSort::getValueAndOriginalIndex(const int itemNum,
                                                  int& indexOut,
                                                  float& valueOut) const
{
   indexOut = values[itemNum].indx;
   valueOut  = values[itemNum].value;
}
                           
/**
 * constructor.
 */
StatisticValueIndexSort::ValueIndexPair::ValueIndexPair(const int indexIn, const float valueIn)
{
   indx = indexIn;
   value = valueIn;
}

/**
 * less than operator.
 */
bool 
StatisticValueIndexSort::ValueIndexPair::operator<(const ValueIndexPair& nip) const
{
   return (value < nip.value);
}
