
#ifndef __STATISTIC_PERMUTATION_H__
#define __STATISTIC_PERMUTATION_H__

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

class StatisticDataGroup;

/// class for permuting a group of values
class StatisticPermutation : public StatisticAlgorithm {
   public:
      /// permutation method
      enum PERMUTATION_METHOD {
         /// randomly flip the signs
         PERMUTATION_METHOD_RANDOM_SIGN_FLIP,
         /// randomly reorder the values
         PERMUTATION_METHOD_RANDOM_ORDER
      };
      
      // constructor
      StatisticPermutation(const PERMUTATION_METHOD permutationMethodIn);
      
      // destructor
      ~StatisticPermutation();
      
      // execute the algorithm
      void execute() throw (StatisticException);
      
      /// get the output
      const StatisticDataGroup* getOutputData() const { return outputDataGroup; }
      
      /// generate a random integer
      static int randomInteger(const int minRandomValue,
                               const int maxRandomValue);
                               
   protected:
      /// the output data group
      StatisticDataGroup* outputDataGroup;
      
      /// the permutation method
      PERMUTATION_METHOD permutationMethod;
};

#endif // __STATISTIC_PERMUTATION_H__
