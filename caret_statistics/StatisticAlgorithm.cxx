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
#include <sstream>

#define __STATISTIC_ALGORITHM_MAIN__
#include "StatisticAlgorithm.h"
#undef __STATISTIC_ALGORITHM_MAIN__

#include "StatisticDataGroup.h"

/**
 * constructor.
 */
StatisticAlgorithm::StatisticAlgorithm()
{
}

/**
 * destructor.
 */
StatisticAlgorithm::~StatisticAlgorithm()
{
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      if (ownDataGroup[i]) {
         if (dataGroups[i] != NULL) {
            delete dataGroups[i];
         }
      }
      dataGroups[i] = NULL;
   }

   dataGroups.clear();
   ownDataGroup.clear();
}

/**
 * add a data group to this algorithm.
 */
int 
StatisticAlgorithm::addDataGroup(StatisticDataGroup* dataGroup,
                                 const bool takeOwnershipOfThisDataGroup)
{
   dataGroups.push_back(dataGroup);
   ownDataGroup.push_back(takeOwnershipOfThisDataGroup);
   
   return (getNumberOfDataGroups() - 1);
}

/**
 * set a data group.
 */
void 
StatisticAlgorithm::setDataGroup(const int indx,
                                 StatisticDataGroup* dataGroup,
                                 const bool takeOwnershipOfThisDataGroup)
{
   dataGroups[indx] = dataGroup;
   ownDataGroup[indx] = takeOwnershipOfThisDataGroup;
}
                        
/**
 * set the number of data groups (note: "dataGroups" is resized but still contains NULL pointers).
 */
void 
StatisticAlgorithm::setNumberOfDataGroups(const int numDataGroups)
{
   if (numDataGroups > 0) {
      dataGroups.resize(numDataGroups, NULL);
      ownDataGroup.resize(numDataGroups, false);
   }
}
      
/**
 * get all of the data values in a vector.
 */
void 
StatisticAlgorithm::getAllDataValues(std::vector<float>& values,
                                     const bool sortTheValues) const
{
   values.clear();
   
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      const int num = dataGroups[i]->numData;
      const float* d = dataGroups[i]->data;
      for (int j = 0; j < num; j++) {
         values.push_back(d[j]);
      }
   }
   
   if (sortTheValues) {
      std::sort(values.begin(), values.end());
   }
}

/**
 * convert an integer to a standard string.
 */
std::string 
StatisticAlgorithm::numberToString(const int i)
{
   std::ostringstream str;
   str << i;
   const std::string s = str.str();
   return s;
}

/**
 * convert a double to a standard string.
 */
std::string 
StatisticAlgorithm::numberToString(const double d, const int digitsRightOfDecimal)
{
   std::ostringstream str;
   str.precision(digitsRightOfDecimal);
   str << std::fixed << d;
   const std::string s = str.str();
   return s;
}
                       
