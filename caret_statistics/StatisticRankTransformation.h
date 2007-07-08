
#ifndef __RANK_TRANSFORMATION_H__
#define __RANK_TRANSFORMATION_H__

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

/// class for rank transforming values used with non-parametric statistical procedures
class StatisticRankTransformation : public StatisticAlgorithm {
   public:
      // constructor
      StatisticRankTransformation();
      
      // destructor
      ~StatisticRankTransformation();
      
      // convert the input arrays to ranks
      void execute() throw (StatisticException);
      
      /// get the number of output data groups
      int getNumberOfOutputDataGroups() const { return outputDataGroupContainingRankValues.size(); }
      
      /// get the output data group that contains the ranked values
      const StatisticDataGroup* getOutputDataGroupContainingRankValues(const int inputArrayNum) const 
                                     { return outputDataGroupContainingRankValues[inputArrayNum]; }
   protected:
      // class used for rank ordering items
      class RankOrder {
         public:
            
            // Constructor
            RankOrder(const int arrayNumberIn,
                      const float valueIn,
                      const int   valuesArrayIndexIn);
         
            // Destructor
            ~RankOrder() { }
            
            // comparison operator for sorting
            bool operator<(const RankOrder& r) const { return this->value < r.value; }
            
            /// value from array
            float value;
            
            /// array value belongs to
            int arrayNumber;
            
            /// index into array
            int valuesArrayIndex;
            
            /// the rank
            float rank;
      };
      /// process any duplicates in the arrays
      void processDuplicates(std::vector<RankOrder>& ranks);

      /// used for assigning ranks
      std::vector<RankOrder> ranks;
      
      // the output data group containing the ranks
      std::vector<StatisticDataGroup*> outputDataGroupContainingRankValues;
};

#endif // __RANK_TRANSFORMATION_H__
