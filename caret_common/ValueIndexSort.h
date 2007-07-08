
#ifndef __VALUE_INDEX_SORT_H__
#define __VALUE_INDEX_SORT_H__

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

#include <vector>

/// name and index sort
class ValueIndexSort {
   protected:
      /// class for storing names and indices
      class ValueIndexPair {
         public:
            /// constructor
            ValueIndexPair(const int indexIn, const float valueIn);
            
            /// the value
            float value;
            
            /// the index
            int indx;
            
            /// less than operator
            bool operator<(const ValueIndexPair& vip) const;
      };
      
   public:
      /// Constructor
      ValueIndexSort(const std::vector<int>& indicesIn,
                     const std::vector<float>& valuesIn);
                    
      /// Constructor
      ~ValueIndexSort();
      
      /// get number of value index pairs
      int getNumberOfItems() const;
      
      /// get value and index for an item
      void getValueAndIndex(const int itemNum,
                           int& indexOut,
                           float& valueOut) const;
                           
   protected:
      /// the value and indices
      std::vector<ValueIndexPair> values;
      
};

#endif // __NAME_INDEX_SORT_H__

