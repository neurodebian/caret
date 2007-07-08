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



#ifndef __FMRI_VOLUME_INFO_H__
#define __FMRI_VOLUME_INFO_H__

#include <string>
#include <vector>

/// This class is used to store data related to the fMRI volumes.
class FMRIVolumeInfo {
   private:
      /// name of volume
      std::string volumeName;
      
      /// number of sub volumes
      int    numberOfSubVolumes;
      
      /// names of sub volumes
      std::vector<std::string> subVolumeNames;
      
   public:
      /// Constructor
      FMRIVolumeInfo(const std::string& name, const int numSubVols, 
                     const std::vector<std::string>& subVolNames);
                           
      /// Get the name of the volume
      std::string getVolumeName() const { return volumeName; }
      
      /// Get the number of sub volumes
      int getNumberOfSubVolumes() const { return numberOfSubVolumes; }
      
      /// Get the names of the sub volumes
      void getSubVolumeNames(std::vector<std::string>& subVolNames) {
         subVolNames = subVolumeNames;
      }
};

#endif // __FMRI_VOLUME_INFO_H__


