
#ifndef __STATISTIC_ALGORITHM_H__
#define __STATISTIC_ALGORITHM_H__

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

#include <string>
#include <vector>

#include "StatisticException.h"

class StatisticDataGroup;

/// abstract class for a statistical operation
class StatisticAlgorithm {
   public:
      // constructor
      StatisticAlgorithm();
      
      // destructor
      virtual ~StatisticAlgorithm();
      
      // add a data group to this algorithm
      int addDataGroup(StatisticDataGroup* dataGroup,
                       const bool takeOwnershipOfThisDataGroup = false);
                       
      // execute the algorithm
      virtual void execute() throw (StatisticException) = 0;
      
      // get the number of data groups
      inline int getNumberOfDataGroups() const { return dataGroups.size(); }
      
      // get a data group
      StatisticDataGroup* getDataGroup(const int indx) { return dataGroups[indx]; }
      
      // get a data group (const method)
      const StatisticDataGroup* getDataGroup(const int indx) const { return dataGroups[indx]; }
      
      /// turn on/off debugging of ALL algorithms
      static void setDebugOn(const bool onOff) { debugOnFlag = onOff; }
      
      /// see if debugging is on
      static bool getDebugOn() { return debugOnFlag; }
      
   protected:
      // get all of the data values in a vector
      void getAllDataValues(std::vector<float>& values,
                            const bool sortTheValues) const;
      
      // set the number of data groups (note: "dataGroups" is resized but still contains NULL pointers)
      void setNumberOfDataGroups(const int numDataGroups);
      
      // set a data group
      void setDataGroup(const int indx,
                        StatisticDataGroup* dataGroup,
                        const bool takeOwnershipOfThisDataGroup = false);
                      
      // convert an integer to a standard string
      static std::string numberToString(const int i);
      
      // convert a double to a standard string
      static std::string numberToString(const double d, const int digitsRightOfDecimal = 3);
      
      /// the data groups
      std::vector<StatisticDataGroup*> dataGroups;
      
      /// own the data group
      std::vector<bool> ownDataGroup;
      
      /// debug flag for all algorithms
      static bool debugOnFlag;
};

#ifdef __STATISTIC_ALGORITHM_MAIN__
bool StatisticAlgorithm::debugOnFlag = false;
#endif // __STATISTIC_ALGORITHM_MAIN__

#endif // __STATISTIC_ALGORITHM_H__

