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

#ifndef __GUI_MAP_FMRI_VOLUME_H__
#define __GUI_MAP_FMRI_VOLUME_H__

#include <QString>
#include <vector>

class VolumeFile;

/// This class stores information about an fmri volume
class GuiMapFmriVolume {
   public:
      /// type of volume
      enum FMRI_VOLUME_TYPE {
         FMRI_VOLUME_TYPE_FILE_ON_DISK,
         FMRI_VOLUME_TYPE_FILE_IN_MEMORY
      };
      
      /// Constructor for a volume file
      GuiMapFmriVolume(const QString& fileNameIn,
                       const float negThreshIn,
                       const float posThreshIn);
      
      /// Constructor for a volume in memory
      GuiMapFmriVolume(VolumeFile* volumeFileIn,
                       const float negThreshIn,
                       const float posThreshIn);
      
      /// Destructor
      ~GuiMapFmriVolume();

      /// get the name of the volume file
      QString getVolumeFileName() const { return fileName; }
      
      /// get the type of volume
      FMRI_VOLUME_TYPE getFmriVolumeType() const { return fmriVolumeType; }
      
      /// get the descriptive name
      QString getDescriptiveName() const { return descriptiveName; }
      
      /// get number of subvolumes
      int getNumberOfSubVolumes() const { return subVolumeNames.size(); }
      
      /// get a subvolume name
      QString getSubVolumeName(const int indx) const;
      
      /// set a subvolume name
      void setSubVolumeName(const int indx, const QString& name);
      
      /// get thresholds
      void getThresholds(float& negThreshOut,
                         float& posThreshOut) const;
                                  
      /// set thresholds
      void setThresholds(const float negThreshIn,
                         const float posThreshIn);
                                  
      /// get volume is valid flag
      bool getVolumeValid() const { return volumeValid; }
      
      /// get the in-memory volume file
      VolumeFile* getInMemoryVolumeFile() { return volumeFile; }
      
      /// get the file's comment
      QString getFileComment() const { return fileComment; }
      
   protected:
      /// Create sub-volume names (use -1 for num if unknown)
      void createSubVolumeNames(const VolumeFile* vf, const int num,
                       const float negThreshIn, const float posThreshIn);

      /// the type of volume
      FMRI_VOLUME_TYPE fmriVolumeType;
      
      /// the name of the volume file used when (
      QString fileName;
      
      /// the VolumeFile used when ()
      VolumeFile* volumeFile;
      
      /// the descriptive name
      QString descriptiveName;
      
      /// names of sub-volumes
      std::vector<QString> subVolumeNames;
      
      /// the file's comment
      QString fileComment;
      
      /// negative thresholds per sub-volume
      float negativeThreshold;
      
      /// positive thresholds per sub-volume
      float positiveThreshold;
      
      /// volume is valid
      bool volumeValid;
};

#endif // __GUI_MAP_FMRI_VOLUME_H__

