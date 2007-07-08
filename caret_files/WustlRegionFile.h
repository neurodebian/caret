
#ifndef __WUSTL_REGION_FILE_H__
#define __WUSTL_REGION_FILE_H__

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

#include "AbstractFile.h"

/// This class is used for the Washington University NeuroImaging Lab's ROI file
class WustlRegionFile : public AbstractFile {
   public:
      /// Class contains a Case for a Region
      class RegionCase {
         public:
            /// Constructor
            RegionCase();
            
            /// Constructor
            ~RegionCase();
         
            /// get the name of the case
            QString getName() const { return name; }
            
            /// set the name of the case
            void setName(const QString& n) { name = n; }
            
            /// get the number of timepoints in the case
            int getNumberOfTimePoints() const { return timePoints.size(); }
            
            /// add a timepoint
            void addTimePoint(const float val) { timePoints.push_back(val); }
            
            /// get the timepoints
            void getTimePoints(std::vector<float>& tp) const { tp = timePoints; }
            
            /// get the timepoints
            std::vector<float> getTimePoints() const { return timePoints; }
            
            /// set the timepoints
            void setTimePoints(const std::vector<float>& tp) { timePoints = tp; }
            
         protected:
            /// name of the case
            QString name;
            
            /// the time points
            std::vector<float> timePoints;
      };

      /// Class contains a Region
      class Region {
         public:
            /// Constructor
            Region();
         
            /// Constructor
            ~Region();
         
            /// get the region number
            int getNumber() const { return regionNumber; }
            
            /// set the region number
            void setNumber(const int num) { regionNumber = num; }
            
            /// get the name of the region
            QString getName() const { return name; }
            
            /// set the name of the region
            void setName(const QString& n) { name = n; }
            
            /// get the number of voxels in the ROI
            int getNumberOfVoxels() const { return numberOfVoxels; }
            
            /// set the number of voxels in the ROI
            void setNumberOfVoxels(const int num) { numberOfVoxels = num; }
            
            /// get the number of region cases
            int getNumberOfRegionCases() const { return regionCases.size(); }
            
            /// add a region case
            void addRegionCase(const RegionCase& rc) { regionCases.push_back(rc); }
            
            /// get a region case
            const RegionCase* getRegionCase(const int indx) const { return &regionCases[indx]; }
            
            /// get a region case (const method)
            RegionCase* getRegionCase(const int indx) { return &regionCases[indx]; }
            
            /// get a region case by name (returns NULL if not found)
            const RegionCase* getRegionCaseByName(const QString& name) const;
            
            /// get a region case by name (returns NULL if not found - const method)
            RegionCase* getRegionCaseByName(const QString& name);
            
         protected:
            /// region number
            int regionNumber;
            
            /// region name
            QString name;
            
            /// number of voxels in the ROI
            int numberOfVoxels;
            
            /// cases in this region
            std::vector<RegionCase> regionCases;
      };
      
      /// Class contains a Timecourse
      class TimeCourse {
         public:
            /// constructor
            TimeCourse();
            
            /// constructor
            ~TimeCourse();
            
            /// get the name of the timecourse
            QString getName() const { return name; }
            
            /// set the name of the timecourse
            void setName(const QString& n) { name = n; }
            
            /// get the number of regions in the timecourse
            int getNumberOfRegions() const { return regions.size(); }
            
            /// get a region by its index
            Region* getRegionByIndex(const int indx) { return &regions[indx]; }
            
            /// get a region by its index (const method)
            const Region* getRegionByIndex(const int indx) const { return &regions[indx]; }
            
            /// get a region by its name (returns NULL if not found)
            Region* getRegionByName(const QString name);
            
            /// get a region (const method)
            const Region* getRegion(const int indx) const { return &regions[indx]; }
            
            /// add a region
            void addRegion(const Region& r) { regions.push_back(r); }
            
            /// get all of the case names in a time course
            void getAllRegionCaseNames(std::vector<QString>& names) const;
            
         protected:
            /// name of timecourse
            QString name;
            
            /// regions in the timecourse
            std::vector<Region> regions;
      };
      
      /// Constructor
      WustlRegionFile();
      
      /// Destructor
      ~WustlRegionFile();
      
      /// clear the file.
      virtual void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      virtual bool empty() const;
      
      /// get the number of timecourses
      int getNumberOfTimeCourses() const { return timeCourses.size(); }
      
      /// get a time course
      TimeCourse* getTimeCourse(const int indx) { return &timeCourses[indx]; }
      
      /// get a time course
      const TimeCourse* getTimeCourse(const int indx) const { return &timeCourses[indx]; }
      
      /// add a time course
      void addTimeCourse(const TimeCourse tc);
      
      /// append a region file to this file
      void append(const WustlRegionFile wrf);
      
   protected:
      /// Read the contents of the file (header has already been read)
      virtual void readFileData(QFile& file, 
                                QTextStream& stream,
                                QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the file's data (header has already been written)
      virtual void writeFileData(QTextStream& stream,
                                 QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);      
      
      /// the time courses
      std::vector<TimeCourse> timeCourses;
};

#endif // __WUSTL_REGION_FILE_H__

