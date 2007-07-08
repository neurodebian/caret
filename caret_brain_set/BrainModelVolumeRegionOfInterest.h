
#ifndef __BRAIN_MODEL_VOLUME_REGION_OF_INTEREST_H__
#define __BRAIN_MODEL_VOLUME_REGION_OF_INTEREST_H__

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

#include <vector>

#include "BrainModelAlgorithmException.h"

class BrainSet;
class VolumeFile;

/// keeps track of the volume used interactively for region of interest operations
/// and performs operations using the region of interest
class BrainModelVolumeRegionOfInterest {
   public:
      // constructor
      BrainModelVolumeRegionOfInterest(BrainSet* brainSetIn);
      
      // destructor
      ~BrainModelVolumeRegionOfInterest();

      // set the volume ROI to all voxels in volume
      void setVolumeROIToAllVoxels(const VolumeFile* vf);
      
      // set the volume ROI to all displayed voxels in volume
      void setVolumeROIToAllDisplayedVoxels(const VolumeFile* vf);
      
      // set the volume ROI to all voxels within a range in volume
      void setVolumeROIToVoxelsInRangeOfValues(const VolumeFile* vf,
                                               const float minValue,
                                               const float maxValue);
      
      // set the volume ROI to all paint voxels with the specified name
      void setVolumeROIToPaintIDVoxels(const VolumeFile* vf,
                                       const QString& paintName);
                                       
      /// get the display volume ROI in graphics windows flag
      bool getDisplayVolumeROI() const { return displayVolumeROI; }
      
      /// set display the volume ROI in graphics windows flag
      void setDisplayVolumeROI(const bool b) { displayVolumeROI = b; }
      
      // get the number of voxels in the ROI
      int getNumberOfVoxelsInROI() const;
            
      /// get the region of interest volume
      VolumeFile* getROIVolume() { return roiVolume; }
      
      // assign paint id to paint volume for ROI voxels
      void operationAssignPaintVolumeID(VolumeFile* paintVolume,
                               const QString& paintName) const throw (BrainModelAlgorithmException);
       
      // assign function value to functional volume for ROI voxels
      void operationAssignFunctionalVolumeValue(VolumeFile* functionalVolume,
                                    const float value) const throw (BrainModelAlgorithmException);
      
      // paint region center of gravity report
      void operationPaintRegionCenterOfGravity(const VolumeFile* paintVolume,
                                      QString& reportTextOut) const throw (BrainModelAlgorithmException);
      
      // paint percentage report
      void operationPaintPercentageReport(const VolumeFile* paintVolume,
                                 QString& reportTextOut) const throw (BrainModelAlgorithmException);

      // probabilistic paint volume overlap analysis
      void operationPaintOverlapAnalysis(const std::vector<VolumeFile*>& paintVolumes,
                                QString& reportTextOut) const throw (BrainModelAlgorithmException); 
      
      // segmentation region center of gravity report
      void operationSegmentationRegionCenterOfGravity(const VolumeFile *segmentVolume,
                                             QString& reportTextOut) const throw (BrainModelAlgorithmException);

   protected:
      // reset the ROI volume
      void resetROIVolume(const VolumeFile* vf,
                          const bool colorTheVolumesVoxelsFlag);
      
      // see if a stereotaxic coordinate is inside the ROI
      bool insideVolumeROI(const float xyz[3]) const;
      
      // determine which voxels are within the ROI volume
      // use std::vector<int> since std::vector<bool> packs the bools into bits
      int determineVoxelsWithinVolumeROI(const VolumeFile* volume,
                                      const float minValue,
                                      const float maxValue,
                                      std::vector<int>& voxelInROI) const;
                                      
      // create the report
      QString createReport(const VolumeFile* operatingVolume,
                        const QString& description,
                        const QString& operationText,
                        const int totalNumberOfVoxels,
                        const int numVoxelsInROI) const;
                        
      // create the report
      QString createReport(const std::vector<VolumeFile*> operatingVolumes,
                        const QString& description,
                        const QString& operationText,
                        const int totalNumberOfVoxels,
                        const int numVoxelsInROI) const;
                        
      /// the brain set using this ROI 
      BrainSet* brainSet;
      
      /// the volume containing the region of interest
      VolumeFile* roiVolume;
      
      /// display the volume ROI
      bool displayVolumeROI;
      
      /// text containing information about the ROI selection
      QString reportROIVolumeInfoText;
};
#endif // __BRAIN_MODEL_VOLUME_REGION_OF_INTEREST_H__

