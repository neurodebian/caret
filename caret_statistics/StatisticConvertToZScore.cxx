
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

#include "StatisticConvertToZScore.h"
#include "StatisticDataGroup.h"
#include "StatisticMeanAndDeviation.h"

/**
 * constructor.
 */
StatisticConvertToZScore::StatisticConvertToZScore()
   : StatisticAlgorithm("Convert to Z-Score")
{
   mean = 0.0;
   deviation = 1.0;
}

/**
 * destructor.
 */
StatisticConvertToZScore::~StatisticConvertToZScore()
{
}

/**
 * execute the algorithm.
 */
void 
StatisticConvertToZScore::execute() throw (StatisticException)
{
   //
   // Compute mean and deviation
   //
   StatisticMeanAndDeviation meanAndDev;
   for (int i = 0; i < getNumberOfDataGroups(); i++) {
      meanAndDev.addDataGroup(getDataGroup(i));
   }
   meanAndDev.execute();
   mean = meanAndDev.getMean();
   deviation  = meanAndDev.getStandardDeviation();
   if (deviation == 0.0) {
      deviation = 1.0;
   }
}
   
/**
 * call after execute to convert value to z-score.
 */
void 
StatisticConvertToZScore::convertToZScore(float& value) const
{
   //
   // Convert to Z-Scores
   //
   const float zScore = (value - mean) / deviation;
   value = zScore;
}
