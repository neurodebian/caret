
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

#ifndef __STATISTIC_RANDOM_NUMBER_H__
#define __STATISTIC_RANDOM_NUMBER_H__

/// class for random number generation
class StatisticRandomNumber {
   public:
      // generate a random integer within the specified range
      static float randomFloat(const float minRandomValue,
                               const float maxRandomValue);
                               
      // generate a random integer within the specified range
      static int randomInteger(const int minRandomValue,
                               const int maxRandomValue);
                               
      // generate a random unsigned integer within the specified range
      static int randomInteger(const unsigned int minRandomValue,
                               const unsigned int maxRandomValue);
                               
      // set the seed for the random number generator
      static void setRandomSeed(const int i);      
};

#endif // __STATISTIC_RANDOM_NUMBER_H__

