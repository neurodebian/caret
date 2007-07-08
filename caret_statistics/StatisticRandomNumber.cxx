
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
#include <cstdlib>

#include "StatisticRandomNumber.h"

/**
 * generate a random integer within the specified range.
 */
int 
StatisticRandomNumber::randomInteger(const int minRandomValue,
                                     const int maxRandomValue)
{
   const double dm = maxRandomValue - minRandomValue;
   int v = minRandomValue + static_cast<int>((dm * std::rand()/(RAND_MAX + 1.0)));  
   return v;
}
                         
/**
 * generate a random unsigned integer within the specified range.
 */
int 
StatisticRandomNumber::randomInteger(const unsigned int minRandomValue,
                                     const unsigned int maxRandomValue)
{
   const double dm = maxRandomValue - minRandomValue;
   unsigned int v = minRandomValue + static_cast<unsigned int>((dm * std::rand()/(RAND_MAX + 1.0)));
   return v;
}

/**
 * generate a random integer within the specified range.
 */
float
StatisticRandomNumber::randomFloat(const float minRandomValue,
                                   const float maxRandomValue)
{
   const double dm = maxRandomValue - minRandomValue;
   float v = minRandomValue + ((dm * std::rand()/(RAND_MAX + 1.0)));
   v = std::max(minRandomValue, v);
   v = std::min(maxRandomValue, v);
   return v;
}
                               
/**
 * set the seed for the random number generator.
 */
void 
StatisticRandomNumber::setRandomSeed(const int i)
{
   std::srand(i);
}
