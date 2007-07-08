
#ifndef __STATISTIC_CORRELATION_COEFFICIENT_H__
#define __STATISTIC_CORRELATION_COEFFICIENT_H__

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

/// class for computing a correlation coefficent
class StatisticCorrelationCoefficient : public StatisticAlgorithm {
   public:
      // constructor
      StatisticCorrelationCoefficient();
      
      // destructor
      ~StatisticCorrelationCoefficient();
      
      // execute the algorithm
      void execute() throw (StatisticException);
      
      // get the correlation coefficient (r2 or r-squared)
      float getCorrelationCoefficient() const { return correlationCoefficientR2; }
      
      /// get the t-value
      float getTValue() const { return tValue; }
      
      /// get the degrees of freedom
      float getDegreesOfFreedom() const { return tDegreesOfFreedom; }
      
      /// get the p-value
      float getPValue() const { return pValue; }
      
   protected:
      /// the correlation coefficient
      float correlationCoefficientR2;
      
      /// t-value
      float tValue;
      
      /// degrees of freedom
      float tDegreesOfFreedom;
      
      /// p-value
      float pValue;
};

#endif // __STATISTIC_CORRELATION_COEFFICIENT_H__

