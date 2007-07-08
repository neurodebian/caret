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

#include <sstream>
#include <QString>

#include "BrainModelVolumeThresholdSegmentation.h"
#include "BrainSet.h"
#include "VolumeFile.h"

/**
 * Constructor.
 */
BrainModelVolumeThresholdSegmentation::BrainModelVolumeThresholdSegmentation(BrainSet* bs,
                                                              VolumeFile* anatomyVolumeIn,
                                                              const float thresholdIn[2])
   : BrainModelAlgorithm(bs)
{
   anatomyVolume = anatomyVolumeIn;
   threshold[0]  = thresholdIn[0];
   threshold[1]  = thresholdIn[1];
}                             
                             
/**
 * Destructor.
 */
BrainModelVolumeThresholdSegmentation::~BrainModelVolumeThresholdSegmentation()
{
}

/**
 * execute the algorithm.
 */
void
BrainModelVolumeThresholdSegmentation::execute() throw (BrainModelAlgorithmException)
{
   if (anatomyVolume == NULL) {
      throw BrainModelAlgorithmException("Anatomy volume is invalid (NULL).");
   }
   
   //
   // Create the segmentation volume
   //
   VolumeFile* segVol = new VolumeFile(*anatomyVolume);
   segVol->setVolumeType(VolumeFile::VOLUME_TYPE_SEGMENTATION);
   
   //
   // Set file name and comments
   //
   std::ostringstream str;
   str << "Threshold_"
       << threshold;
   segVol->setFileName(str.str().c_str());
   segVol->setDescriptiveLabel("");
   str.str("");
   str << "Thresholded with " << threshold << "\n";
   segVol->appendToFileComment(str.str().c_str());
   
   //
   // Threshold the volume
   //
   segVol->dualThresholdVolume(threshold[0], threshold[1]);
   
   //
   // Add volume to the brain set
   //
   brainSet->addVolumeFile(VolumeFile::VOLUME_TYPE_SEGMENTATION,
                           segVol,
                           segVol->getFileName(),
                           true,
                           false);
}

