
#ifndef __STATISTIC_DATA_GROUP_H__
#define __STATISTIC_DATA_GROUP_H__

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

/// class for pointing/storing a data group for statistical processing
/// NOTE the data passed to the StatisticDataGroup is NEVER modified
class StatisticDataGroup {
   public:
      /// data storage mode - how to store data
      enum DATA_STORAGE_MODE {
         /// maintain a pointer to data (data must NOT be deallocated by user until statistical
         /// algorithm that uses data has executed).
         DATA_STORAGE_MODE_POINT,
         /// maintain pointer to data and delete the data in destructor
         /// data MUST have been allocated with new[].
         DATA_STORAGE_MODE_TAKE_OWNERSHIP
      };
      
      // constructor (if taking ownership, data must have been allocated with new[])
      StatisticDataGroup(const float* dataIn,
                         const int numDataIn,
                         const DATA_STORAGE_MODE dataStorageModeIn);
      
      // constructor from vector
      StatisticDataGroup(const std::vector<float>* dataIn,
                         const DATA_STORAGE_MODE dataStorageModeIn);
      
      // copy constructor
      StatisticDataGroup(const StatisticDataGroup& dataGroup);
      
      // destructor
      ~StatisticDataGroup();
      
      // assignment operator
      StatisticDataGroup& operator=(const StatisticDataGroup& dataGroup);
      
      /// get the number of data items
      inline int getNumberOfData() const { return numData; }
      
      /// get a data item
      inline float getData(const int indx) const { return data[indx]; }
      
      /// set a data item
      //inline void setData(const int indx, const float d) { data[indx] = d; }
      
      /// get a pointer to data (const method)
      const float* getPointerToData() const { return data; }
      
      // get the sum of all of the data 
      double getSumOfData() const;
      
      // get mean of data
      float getMeanOfData() const;
      
   protected:
      // copy helper for copy constructor and operator=
      void copyHelper(const StatisticDataGroup& dataGroup);
      
      // delete the data in this data group
      void deleteData();
      
      // constructor helper (if NOT copying data, data must have been allocated with new[])
      void constructorHelper(const float* dataIn,
                             const std::vector<float>* dataVectorIn,
                             const int numDataIn,
                             const DATA_STORAGE_MODE dataStorageModeIn);
      /// the data
      float* data;
      
      /// vector for data
      std::vector<float>* dataVector;
      
      /// number of data items
      int numData;
      
      /// how data is stored and deleted
      DATA_STORAGE_MODE dataStorageMode;
   
   friend class StatisticAlgorithm;
   
};

/// subraction operator
StatisticDataGroup operator-(const StatisticDataGroup& a, const StatisticDataGroup& b);

#endif // __STATISTIC_DATA_GROUP_H__
