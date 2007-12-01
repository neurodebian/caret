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

#include "ValueIndexSort.h"

/**
 * Constructor.
 */
ValueIndexSort::ValueIndexSort()
{
   values.clear();
}
              
/**
 * Constructor.
 */
ValueIndexSort::~ValueIndexSort()
{
}

/**
 * add an index value pair (must call sort() when all added).
 */
void 
ValueIndexSort::addValueIndexPair(const int indx, const float value)
{
   values.push_back(ValueIndexPair(indx, value));
}

/**
 * sort the items (use if addValueIndexPair) was used.
 */
void 
ValueIndexSort::sort()
{
   std::sort(values.begin(), values.end());
}
      
/**
 * get number of value indice pairs.
 */
int 
ValueIndexSort::getNumberOfItems() const 
{ 
   return values.size(); 
}

/**
 * get value and index for an item.
 */
void 
ValueIndexSort::getValueAndIndex(const int itemNum,
                               int& indexOut,
                               float& valueOut) const
{
   indexOut = values[itemNum].indx;
   valueOut  = values[itemNum].value;
}
                           
/**
 * constructor.
 */
ValueIndexSort::ValueIndexPair::ValueIndexPair(const int indexIn, const float valueIn)
{
   indx = indexIn;
   value = valueIn;
}

/**
 * less than operator.
 */
bool 
ValueIndexSort::ValueIndexPair::operator<(const ValueIndexPair& nip) const
{
   return (value < nip.value);
}
