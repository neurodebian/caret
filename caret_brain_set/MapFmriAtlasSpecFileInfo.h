
#ifndef __GUI_MAP_FMRI_ATLAS_SPEC_FILE_INFO_H__
#define __GUI_MAP_FMRI_ATLAS_SPEC_FILE_INFO_H__

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

#include <QString>
#include <vector>

class BrainSet;

/// class for storage of atlas spec files and information
class MapFmriAtlasSpecFileInfo {
   public:
      /// Constructor
      MapFmriAtlasSpecFileInfo(const QString& specFileNameIn);
      
      /// Destructor
      ~MapFmriAtlasSpecFileInfo();
      
      /// read the atlases
      static void getAtlases(BrainSet* bs,
                             std::vector<MapFmriAtlasSpecFileInfo>& atlases);
      
      /// get the spec file path
      QString getSpecFilePath() const { return specFilePath; }
      
      /// get the spec file description
      QString getDescription() const { return description; }
      
      /// get the topology files
      QString getTopologyFile() const { return topoFile; }
      
      /// get the coordinate files
      std::vector<QString> getCoordinateFiles() const { return coordFiles; }
      
      /// get the average coordinate file
      QString getAverageCoordinateFile() const { return averageCoordinateFile; }
      
      /// get the validity of the data
      bool getDataValid() const { return dataValid; }
      
      /// get the species
      QString getSpecies() const { return species; }
      
      /// get the structure
      QString getStructure() const { return structure; }
      
      /// get the space
      QString getSpace() const { return space; }
      
      /// get the metric name hint
      QString getMetricNameHint() const { return metricNameHint; }
      
      /// comparison operator for sorting
      bool operator<(const MapFmriAtlasSpecFileInfo& asfi) const
         { return description < asfi.description; }
         
   protected:
      /// path of the spec file and its data files
      QString specFilePath;
      
      /// description from the spec file
      QString description;
      
      /// topology files for mapping
      QString topoFile;
      
      /// coordinate files for mapping
      std::vector<QString> coordFiles;
      
      /// spec file species
      QString species;
      
      /// spec file structure
      QString structure;
      
      /// spec file stereotaxic space
      QString space;
      
      /// metric name hint
      QString metricNameHint;
      
      /// average coordinate file
      QString averageCoordinateFile;
      
      /// data is valid
      bool dataValid;
};

#endif // __GUI_MAP_FMRI_ATLAS_SPEC_FILE_INFO_H__

