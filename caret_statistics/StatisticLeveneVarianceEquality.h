
#ifndef __STATISTIC_LEVENE_VARIANCE_EQUALITY_H__
#define __STATISTIC_LEVENE_VARIANCE_EQUALITY_H__

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

/// class for Levene's Test for Equality of Variances
/// http://www.itl.nist.gov/div898/handbook/eda/section3/eda35a.htm
/// http://www.people.vcu.edu/~wsstreet/courses/314_20033/Handout.Levene.pdf
class StatisticLeveneVarianceEquality : public StatisticAlgorithm {
   public:
      // constructor
      StatisticLeveneVarianceEquality();
      
      // destructor
      ~StatisticLeveneVarianceEquality();
      
      // execute the algorithm
      void execute() throw (StatisticException);
      
      /// get the levene F-statistic
      float getLeveneF() const { return leveneF; }
      
      /// get degrees of freedom 1
      float getDegreesOfFreedom1() const { return dof1; }
      
      /// get degrees of freedom 2
      float getDegreesOfFreedom2() const { return dof2; }
      
      /// get the P-value
      float getPValue() { return pValue; }
      
   protected:
      /// the levene's F statistic
      float leveneF;
      
      /// the first degree of freedom
      float dof1;
      
      /// the second degree of freedom
      float dof2;
      
      /// the p-value
      float pValue;
};

#endif // __STATISTIC_LEVENE_VARIANCE_EQUALITY_H__
