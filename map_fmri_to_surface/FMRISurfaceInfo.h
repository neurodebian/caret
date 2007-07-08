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



#ifndef __FMRI_SURFACE_INFO_H__
#define __FMRI_SURFACE_INFO_H__

#include <string>
#include <vector>

/// This class is used to store data related metric naming for each surface 
/// that will have volume(s) mapped to it. 
class FMRISurfaceInfo {
   private:
      /// name of surface file
      std::string surfaceFileName;
      
      /// descriptive name of surface file
      std::string descriptiveName;
      
      /// name of metric file for surface
      std::string metricFileName;
      
      /// title of metric file for surface
      std::string metricTitle;
      
      /// name of spec file for surface
      std::string specFileName;
      
      /// names of metric columns
      std::vector<std::string> metricColumnNames;
      
      /// comments for metric columns
      std::vector<std::string> metricColumnComments;
      
      /// index to volume
      std::vector<int> metricColumnVolumeIndex;    // volume number 
      
      /// index to volume's subvolume
      std::vector<int> metricColumnSubVolumeIndex; // subvolume in volume
      
      /// counter for metric names
      static int metricNameCounter;
      
   public:
      /// Constructor
      FMRISurfaceInfo(const std::string& subdir,
                      const std::string& surfaceName,
                      const std::string& descriptiveNameIn,
                      const std::string& anatomyName,
                      const std::string& specFileNameIn);
      
      /// Get name of surface filename
      std::string getSurfaceFileName() const { return surfaceFileName; }
      
      /// Get descriptive name
      std::string getDescriptiveName() const { return descriptiveName; }
      
      /// Get spec file name
      std::string getSpecFileName() const { return specFileName; }
      
      /// Set the spec file name
      void setSpecFileName(const std::string& n) { specFileName = n; }
      
      /// Get the metric file name
      std::string getMetricFileName() const { return metricFileName; }
      
      /// Set the metric file name
      void setMetricFileName(const std::string& n) { metricFileName = n; }
      
      /// Get the title of the metric file
      std::string getMetricTitle() const { return metricTitle; }
      
      /// Set the title of the metric file
      void setMetricTitle(const std::string& n) { metricTitle = n; }
      
      /// Add a column (volume) to the metrics
      void addMetricColumnInfo(const std::string& name, const int volumeIndex,
                               const int subVolumeIndex);
       
      /// Delete a column (volume) from the metrics
      void deleteMetricColumnName(const int volumeIndex);
      
      /// Get the number of metric columns
      int getNumberOfMetricColumnNames() const 
                              { return (int)metricColumnNames.size(); }
                              
      /// Get a metric column name
      std::string getMetricColumnName(const int index) const 
                              { return metricColumnNames[index]; }
                              
      /// Set the name of a metric column
      void setMetricColumnName(const int index, const std::string& name)
                              { metricColumnNames[index].assign(name); }
                              
      /// Get metric column comments
      std::string getMetricColumnComments(const int index) const 
                              { return metricColumnComments[index]; }
                              
      /// Set the metric column comments
      void setMetricColumnComments(const int index, const std::string& name)
                              { metricColumnComments[index].assign(name); }

      /// Get the volume index for a metric column
      int getMetricColumnVolumeIndex(const int index) const 
                              { return metricColumnVolumeIndex[index]; }

      /// Get the sub volume index for a metric column
      int getMetricColumnSubVolumeIndex(const int index) const 
                              { return metricColumnSubVolumeIndex[index]; }
};

#endif // __FMRI_SURFACE_INFO_H__

