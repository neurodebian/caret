
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

#include <cmath>

#include "StatisticCorrelationCoefficient.h"
#include "StatisticDataGroup.h"
#include "StatisticGeneratePValue.h"

/**
 * constructor.
 */
StatisticCorrelationCoefficient::StatisticCorrelationCoefficient()
   : StatisticAlgorithm("Correlation Coefficient")
{
   correlationCoefficientR  = 0.0;
   correlationCoefficientR2 = 0.0;
   pValue = 0.0;
   tValue = -1000000.0;
   tDegreesOfFreedom = 0.0;
}

/**
 * destructor.
 */
StatisticCorrelationCoefficient::~StatisticCorrelationCoefficient()
{
}

/**
 * execute the algorithm.
 */
void 
StatisticCorrelationCoefficient::execute() throw (StatisticException)
{
   correlationCoefficientR2 = 0.0;
   correlationCoefficientR  = 0.0;
   
   pValue = 0.0;
   tValue = -1000000.0;
   tDegreesOfFreedom = 0.0;
   
   //
   // Check the data groups
   //
   if (getNumberOfDataGroups() != 2) {
      throw StatisticException("Correlation coefficient requires two and only two data groups.");
   }
   StatisticDataGroup* xGroup = getDataGroup(0);
   const int numData = xGroup->getNumberOfData();
   StatisticDataGroup* yGroup = getDataGroup(1);
   if (numData != yGroup->getNumberOfData()) {
      throw StatisticException("Groups sent to correlation coefficient must have same number of elements.");
   }

   const float* x = xGroup->getPointerToData();
   const float* y = yGroup->getPointerToData();
   
   const double numFloat = numData;
   double xSum  = 0.0; 
   double x2Sum = 0.0;
   double ySum  = 0.0;
   double y2Sum = 0.0;
   double xySum = 0.0;
   
   for (int i = 0; i < numData; i++) {
      xSum  += x[i];
      x2Sum += x[i] * x[i];
      ySum  += y[i];
      y2Sum += y[i] * y[i];
      xySum += x[i] * y[i];
   }
   
   const double xMean = xSum / numFloat;
   const double yMean = ySum / numFloat;
   
   const double ssxx = x2Sum - (numFloat * xMean * xMean);
   const double ssyy = y2Sum - (numFloat * yMean * yMean);
   const double ssxy = xySum - (numFloat * xMean * yMean);
   
   const double denom = ssxx * ssyy;
   if (denom != 0.0) {
      correlationCoefficientR2 = static_cast<float>((ssxy * ssxy) / denom);
   }
   if (correlationCoefficientR2 >= 0.0) {
      correlationCoefficientR = std::sqrt(correlationCoefficientR2);
   }
      
   //
   // T-Value calculation from:
   //           Statistics for Psychology
   //           Arthur Aron & Elaine Aron
   //           2nd Edition, 1999
   //           page 98-99
   //
   float tDenom = 1.0 - correlationCoefficientR2;
   if (tDenom <= 1.0) {
      tDegreesOfFreedom = numFloat - 2.0;
      if (tDegreesOfFreedom >= 0.0) {
         const float tNum = correlationCoefficientR * std::sqrt(tDegreesOfFreedom);
         tValue = tNum / std::sqrt(tDenom);
         pValue = StatisticGeneratePValue::getOneTailTTestPValue(tDegreesOfFreedom, tValue);
      }
   }
}
