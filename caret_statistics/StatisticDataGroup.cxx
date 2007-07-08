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

#include <iostream>
#include "StatisticDataGroup.h"

/**
 * constructor (if taking ownership, data must have been allocated with new[]).
 */
StatisticDataGroup::StatisticDataGroup(const float* dataIn,
                                       const int numDataIn,
                                       const DATA_STORAGE_MODE dataStorageModeIn)
{
   constructorHelper(dataIn, NULL, numDataIn, dataStorageModeIn);
}

/**
 * constructor.
 */
StatisticDataGroup::StatisticDataGroup(const std::vector<float>* dataVectorIn,
                                       const DATA_STORAGE_MODE dataStorageModeIn)
{
   constructorHelper(NULL, dataVectorIn, dataVectorIn->size(), dataStorageModeIn);
}

/**
 * destructor.
 */
StatisticDataGroup::~StatisticDataGroup()
{
   deleteData();
}

/**
 * delete the data in this data group.
 */
void 
StatisticDataGroup::deleteData()
{
   switch (dataStorageMode) {
      case DATA_STORAGE_MODE_POINT:
         break;
      case DATA_STORAGE_MODE_TAKE_OWNERSHIP:
         if (dataVector != NULL) {
            dataVector->clear();
            delete dataVector;
         }
         else {
            if (data != NULL) {
               delete[] data;
            }
         }
         break;
   }
   data = NULL;
   dataVector = NULL;
   numData = 0;
}
      
/**
 * copy constructor.
 */
StatisticDataGroup::StatisticDataGroup(const StatisticDataGroup& dataGroup)
{
   copyHelper(dataGroup);
}

/**
 * copy helper for copy constructor and operator=.
 */
void 
StatisticDataGroup::copyHelper(const StatisticDataGroup& dataGroup)
{
   //
   // Get rid of existing data
   //
   deleteData();
   
   //
   // copy data
   //
   float* ptr = NULL;
   if (dataGroup.numData > 0) {
      ptr = new float[dataGroup.numData];
      for (int i = 0; i < dataGroup.numData; i++) {
         ptr[i] = dataGroup.data[i];
      }
      constructorHelper(ptr, NULL, dataGroup.numData, DATA_STORAGE_MODE_TAKE_OWNERSHIP);
   }
}
      
/**
 * get the sum of all of the data.
 */
double 
StatisticDataGroup::getSumOfData() const
{
   double sum = 0.0;
   for (int i = 0; i < numData; i++) {
      sum += data[i];
   }
   return sum;
}
      
/**
 * assignment operator.
 */
StatisticDataGroup& 
StatisticDataGroup::operator=(const StatisticDataGroup& dataGroup)
{
   if (this != &dataGroup) {
      copyHelper(dataGroup);
   }
   
   return *this;
}

/**
 * constructor helper (if NOT copying data, data must have been allocated with new[]).
 * One of dataIn and dataVectorIn MUST BE NULL so that memory is processed properly.
 *
 * @param dataIn - this should used if the data was allocated with new[] 
 * @param dataVectorIn - this should be used if the data is stored in a float vector
 */
void 
StatisticDataGroup::constructorHelper(const float* dataIn,
                                      const std::vector<float>* dataVectorIn,
                                      const int numDataIn,
                                      const DATA_STORAGE_MODE dataStorageModeIn)
{
   //
   // Verify that there is data
   //
   data = NULL;
   dataVector = NULL;
   dataStorageMode = DATA_STORAGE_MODE_POINT;
   if (numData < 0) {
      numData = 0;
   }
   
   data = (float*)dataIn;
   dataVector = (std::vector<float>*)dataVectorIn;
   dataStorageMode = dataStorageModeIn;
   numData = numDataIn;
   
   //
   // Process input parameters
   //
   switch (dataStorageMode) {
      case DATA_STORAGE_MODE_POINT:
         //
         // Just point to data
         //
         if (dataVector != NULL) {
            data = (float*)&((*dataVector)[0]); // address of first element
            dataVector = NULL;  // prevents deletion of vector
         }
         break;
      case DATA_STORAGE_MODE_TAKE_OWNERSHIP:
         if (dataVector != NULL) {
            data = (float*)&((*dataVector)[0]);  // address of first element
         }
         break;
   }   
}

/**
 * subraction operator.
 */
StatisticDataGroup 
operator-(const StatisticDataGroup& a, const StatisticDataGroup& b)
{
   float* data = NULL;
   int numData = 0;
   const int numA = a.getNumberOfData();
   const int numB = b.getNumberOfData();
   if (numA == numB) {
      if (numA > 0) {
         numData = numA;
         data = new float[numData];
         for (int i = 0; i < numData; i++) {
            data[i] = a.getData(i) - b.getData(i);
         }
      }
   }
   else {
      std::cout << "PROGRAM ERROR operator-(StatisticDataGroup): different sized data groups." << std::endl;
   }
   
   StatisticDataGroup result(data, numData, StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP);
   
   return result;
}

