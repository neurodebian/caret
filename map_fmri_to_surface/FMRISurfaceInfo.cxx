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



#include <qdatetime.h>

#include "FMRISurfaceInfo.h"

/**
 * Constructor
 */
FMRISurfaceInfo::FMRISurfaceInfo(const std::string& subdir,
                                 const std::string& surfaceName,
                                 const std::string& descriptiveNameIn,
                                 const std::string& anatomyName,
                                 const std::string& specFileNameIn) {
   surfaceFileName.assign(surfaceName);
   descriptiveName.assign(descriptiveNameIn);
   specFileName.assign(specFileNameIn);
   
   metricNameCounter++;
   char nameCounterStr[32];
   sprintf(nameCounterStr, "%d", metricNameCounter);
   
   if (subdir.length() > 0) {
      metricFileName.assign(subdir);
      metricFileName.append("/");
   }
   else {
      metricFileName.assign("");
   }
   metricFileName.append("map_fmri_");
   if (anatomyName.length() > 0) {
      metricFileName.append(anatomyName);
      metricFileName.append("_");
   }
   metricFileName.append(nameCounterStr);
   metricFileName.append("_");
   QDateTime dt = QDateTime::currentDateTime();
   const QString dts = dt.toString("dd_MMM_yyyy_hh_mm_ss");
   metricFileName.append(dts.latin1());
   metricFileName.append(".metric");
   
   metricTitle.assign(descriptiveName);
}

/**
 * Add a column (volume/subvolume) to the metrics
 */
void 
FMRISurfaceInfo::addMetricColumnInfo(const std::string& name, const int volumeIndex,
                                     const int subVolumeIndex) {
   metricColumnNames.push_back(name);
   metricColumnComments.push_back("");
   metricColumnVolumeIndex.push_back(volumeIndex);
   metricColumnSubVolumeIndex.push_back(subVolumeIndex);
}

/**
 * Delete a column (volume/subvolume) from a metric
 */
void 
FMRISurfaceInfo::deleteMetricColumnName(const int volumeIndex) {
   //
   // A volume may be listed several times due to it having subvolumes
   //
   bool deletedItems = true;
   while(deletedItems) { 
      deletedItems = false;
      
      for (unsigned int i = 0; i < metricColumnNames.size(); i++) {
         if (metricColumnVolumeIndex[i] == volumeIndex) {
            std::vector<std::string>::iterator iter = metricColumnNames.begin();
            iter += i;
            metricColumnNames.erase(iter);
            
            std::vector<int>::iterator iter2 = metricColumnVolumeIndex.begin();
            iter2 += i;
            metricColumnVolumeIndex.erase(iter2);

            std::vector<int>::iterator iter3 = metricColumnSubVolumeIndex.begin();
            iter3 += i;
            metricColumnSubVolumeIndex.erase(iter3);

            std::vector<std::string>::iterator iter4 = metricColumnComments.begin();
            iter4 += i;
            metricColumnComments.erase(iter4);
            
            deletedItems = true;
            break;
         }
      }
   }
               
   //
   // Take care of any volumes that are above the one being deleted
   // so that their index is correct.
   //
   for (unsigned int j = 0; j < metricColumnVolumeIndex.size(); j++) {
      if (metricColumnVolumeIndex[j] > volumeIndex) {
         metricColumnVolumeIndex[j] -= 1;
      }
   }

}
