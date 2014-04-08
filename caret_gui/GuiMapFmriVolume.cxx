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

#include "FileUtilities.h"
#include "GuiMapFmriVolume.h"
#include "VolumeFile.h"

/**
 * Constructor for a volume file.
 */
GuiMapFmriVolume::GuiMapFmriVolume(const QString& fileNameIn,
                                   const float negThreshIn,
                                   const float posThreshIn)
{
   fmriVolumeType = FMRI_VOLUME_TYPE_FILE_ON_DISK;
   fileName = fileNameIn;
   descriptiveName = "FILE - ";
   descriptiveName.append(FileUtilities::basename(fileName));
   
   volumeValid = false;
   VolumeFile vf;
   try {
      vf.readFile(fileName, VolumeFile::VOLUME_READ_HEADER_ONLY);
      if (vf.getNumberOfSubVolumes() > 0) {
         volumeValid = true;
         createSubVolumeNames(&vf, -1, negThreshIn, posThreshIn);
         fileComment = vf.getFileComment();
      }
   }
   catch (FileException&) {
   }
}

/**
 * Constructor for a volume in memory.
 */
GuiMapFmriVolume::GuiMapFmriVolume(VolumeFile* volumeFileIn,
                                   const float negThreshIn,
                                   const float posThreshIn)
{
   fmriVolumeType = FMRI_VOLUME_TYPE_FILE_IN_MEMORY;
   volumeFile = volumeFileIn;
   descriptiveName = "VOLUME - ";
   descriptiveName.append(FileUtilities::basename(volumeFile->getDescriptiveLabel()));
   
   volumeValid = (volumeFile != NULL);
   if (volumeValid) {
      createSubVolumeNames(volumeFile, 1, negThreshIn, posThreshIn);
      fileComment = volumeFile->getFileComment();
   }
}

/**
 * Destructor.
 */
GuiMapFmriVolume::~GuiMapFmriVolume()
{
}

/**
 * Create sub-volume names.
 */
void
GuiMapFmriVolume::createSubVolumeNames(const VolumeFile* vf, 
                                       const int num,
                                       const float negThreshIn,
                                       const float posThreshIn)
{
   int numSubVol = num;
   if (num <= 0) {
      numSubVol = vf->getNumberOfSubVolumes();
   }
   std::vector<QString> names;
   vf->getSubVolumeNames(names);
   for (int i = 0; i < numSubVol; i++) {
      subVolumeNames.push_back(names[i]);
      negativeThreshold = negThreshIn;
      positiveThreshold = posThreshIn;
   }
}

/**
 * get a subvolume name.
 */
QString 
GuiMapFmriVolume::getSubVolumeName(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfSubVolumes())) {
      return subVolumeNames[indx];
   }
   return "";
}

/**
 * set a subvolume name.
 */
void 
GuiMapFmriVolume::setSubVolumeName(const int indx, const QString& name)
{
   if ((indx >= 0) && (indx < getNumberOfSubVolumes())) {
      subVolumeNames[indx] = name;
   }
}

/**
 * get subvolume thresholds.
 */
void 
GuiMapFmriVolume::getThresholds(float& negThreshOut,
                                float& posThreshOut) const
{
  negThreshOut = negativeThreshold;
  posThreshOut = positiveThreshold;
}

/**
 * set subvolume thresholds.
 */
void 
GuiMapFmriVolume::setThresholds(const float negThreshIn,
                                const float posThreshIn)
{
   negativeThreshold = negThreshIn;
   positiveThreshold = posThreshIn;
}
                                  

