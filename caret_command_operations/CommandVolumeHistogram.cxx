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

#include <iostream>
#include <set>

#include "CommandVolumeHistogram.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StatisticHistogram.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeHistogram::CommandVolumeHistogram()
   : CommandBase("-volume-histogram",
                 "VOLUME HISTOGRAM")
{
   numberOfBuckets = 64;
   yMaximum        = 500000.0;
}

/**
 * destructor.
 */
CommandVolumeHistogram::~CommandVolumeHistogram()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeHistogram::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandVolumeHistogram::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "[-list-peaks]\n"
       + indent9 + "[-number-of-buckets  number-of-buckets]\n"
       + indent9 + "[-y-maximum  maximum-y-value]\n"
       + indent9 + "\n"
       + indent9 + "Display a histogram of the volume in the terminal window.\n"
       + indent9 + "\n"
       + indent9 + "If \"-list-peaks\" is specified, numerical estimates of the\n"
       + indent9 + "gray and white matter peaks will be listed.\n"
       + indent9 + "\n"
       + indent9 + "Use \"-number-of-buckets\" to specify the number of buckets. \n"
       + indent9 + "The default is " + QString::number(numberOfBuckets) + ".\n"
       + indent9 + "\n"
       + indent9 + "Use \"-y-maximum\" to specify the maximum Y-value.  The \n"
       + indent9 + "default is " + QString::number(yMaximum) + ".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeHistogram::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   
   bool listPeaksFlag = false;
   bool yMaxWasSetByUser = false;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Histogram Options");
      if (paramName == "-list-peaks") {
         listPeaksFlag = true;
      }
      else if (paramName == "-number-of-buckets") {
         numberOfBuckets = parameters->getNextParameterAsInt("Number of Buckets");
      }
      else if (paramName == "-y-maximum") {
         yMaximum = parameters->getNextParameterAsFloat("Y-Maximum");
         yMaxWasSetByUser = true;
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // Determine the histogram
   //
   StatisticHistogram* histo = volume.getHistogram(numberOfBuckets);
   
   //
   // Adjust Y-Max
   //
   float maxBucketValue = 0.0;
   if (yMaxWasSetByUser == false) {
      std::set<float> sortedBucketCounts;
      for (int i = 0; i < numberOfBuckets; i++) {
         float bucketDataValue, bucketCount;
         histo->getDataForBucket(i, bucketDataValue, bucketCount);
         sortedBucketCounts.insert(bucketCount);
      }
      int count = 0;
      for (std::set<float>::reverse_iterator iter = sortedBucketCounts.rbegin();
           iter != sortedBucketCounts.rend();
           iter++) {
         
         if (DebugControl::getDebugOn()) {
            std::cout << "Find Y-Max: " << *iter << std::endl;
         }
         
         if (count >= 2) {
            yMaximum = *iter;
            break;
         }
         
         if (count == 0) {
            maxBucketValue = *iter;
         }
         count++;
      }
   }
   
   //
   // Place histogram into a 2D array
   //
   std::vector<QString> bucketValueStrings;
   const int numRows = 40;
   const int arraySize = numRows * numberOfBuckets;
   char* histoArray = new char[arraySize];
   for (int i = 0; i < arraySize; i++) {
      histoArray[i] = ' ' ;
   } 
   for (int i = 0; i < numberOfBuckets; i++) {
      float bucketDataValue, bucketCount;
      histo->getDataForBucket(i, bucketDataValue, bucketCount);
      const float percentToSet = bucketCount / yMaximum;
      const int numYUsed = std::min(static_cast<int>(numRows * percentToSet), numRows);
      for (int j = 0; j < numYUsed; j++) {
         const int offset = (j * numberOfBuckets) + i;
         histoArray[offset] = '*';
      }
      
      if (DebugControl::getDebugOn()) {
         std::cout << "Bucket, count, percentToSet, numYUsed: " << i << ", " << bucketCount
                   << ", " << percentToSet << ", " << numYUsed << std::endl;
      }
      
      bucketValueStrings.push_back(QString::number(bucketDataValue, 'f', 0));
   }
     
   //
   // Print the array
   //
   int maxChars = -1;
   std::cout << std::endl;
   for (int j = (numRows - 1); j >= 0; j--) {
      for (int i = 0; i < numberOfBuckets; i++) {
         const int offset = (j * numberOfBuckets) + i;
         std::cout << histoArray[offset];
      }
      
      //
      // Print the Y-Value on the right side
      //
      const float rowHeight = yMaximum / numRows;
      const float yValue = rowHeight * (j + 1);
      QString yString = QString::number(yValue, 'f', 1);
      if (maxChars < 0) {
         maxChars = yString.length();
      }
      else {
         yString = yString.rightJustified(maxChars, ' ');
      }
      std::cout << " " << yString.toAscii().constData();
      std::cout << std::endl;
   }
   
   //
   // print value at each column vertically
   //
   int maxDigits = 0;
   for (int i = 0; i < numberOfBuckets; i++) {
      maxDigits = std::max(maxDigits, bucketValueStrings[i].length());
   }
   for (int i = 0; i < numberOfBuckets; i++) {
      bucketValueStrings[i] = bucketValueStrings[i].rightJustified(maxDigits, '0');
   }
   for (int i = 0; i < maxDigits; i++) {
      for (int j = 0; j < numberOfBuckets; j++) {
         const QString s(bucketValueStrings[j]);
         const char c = s[i].toAscii();
         std::cout << c;
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
   
   //
   // Print maximum bucket value
   //
   std::cout << "Maximum Y-Value: " << QString::number(maxBucketValue, 'f', 1).toAscii().constData() << std::endl;
   std::cout << "Max Y-Value Displayed: " << QString::number(yMaximum, 'f', 1).toAscii().constData() << std::endl;
   std::cout << std::endl;
   
   //
   // Print the histogram peaks
   //
   if (listPeaksFlag) {
      histo->printHistogramPeaks(std::cout);
   }
   
   delete histo;
   histo = NULL;
   delete histoArray;
   histoArray = NULL;
}

      

