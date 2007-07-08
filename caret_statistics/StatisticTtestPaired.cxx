
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

#include "StatisticDataGroup.h"
#include "StatisticTtestOneSample.h"
#include "StatisticTtestPaired.h"

/**
 * constructor.
 */
StatisticTtestPaired::StatisticTtestPaired()
{
   varianceOverride = 0.0;
   varianceOverrideFlag = false;
}

/**
 * destructor.
 */
StatisticTtestPaired::~StatisticTtestPaired()
{
}

/**
 * use this value for the variance override.
 */
void 
StatisticTtestPaired::setVarianceOverride(const float varianceOverrideIn,
                                          const bool varianceOverrideFlagIn)
{
   varianceOverride = varianceOverrideIn;
   varianceOverrideFlag = varianceOverrideFlagIn;
}
                                    
/**
 * execute the algorithm.
 */
void 
StatisticTtestPaired::execute() throw (StatisticException)
{
   tValue = 0.0;
   degreesOfFreedom = 0;
   pValue = 0.0;
   
   //
   // Verify that there are groups
   //
   const int numGroups = getNumberOfDataGroups();
   if (numGroups != 2) {
      throw StatisticException("Paired T-Test requires exactly two data groups.");
   }
   
   //
   // Get the data group
   //
   StatisticDataGroup* dataGroupA = getDataGroup(0);
   StatisticDataGroup* dataGroupB = getDataGroup(1);
   
   //
   // Get the number of items in the data groups
   //
   const int numDataA = dataGroupA->getNumberOfData();
   const int numDataB = dataGroupB->getNumberOfData();
   if (numDataA != numDataB) {
      throw StatisticException("Data groups sent to Paired T-Test contains a different number of values.");
   }
   if (numDataA <= 0) {
      throw StatisticException("Data groups sent to Paired T-Test contains no data.");
   }
   
   StatisticDataGroup diffDataGroup = *dataGroupA - *dataGroupB;
   
   StatisticTtestOneSample tTestOneSample(0.0);
   tTestOneSample.setVarianceOverride(varianceOverride, varianceOverrideFlag);
   tTestOneSample.addDataGroup(&diffDataGroup);
   try {
      tTestOneSample.execute();
   }
   catch (StatisticException& e) {
      std::string msg("Failure while using one-sample T-Test: "
                  + e.whatStdString());
      throw StatisticException(msg);
   }
   
   tValue = tTestOneSample.getTValue();
   degreesOfFreedom = tTestOneSample.getDegreesOfFreedom();
   pValue = tTestOneSample.getPValue();
}

