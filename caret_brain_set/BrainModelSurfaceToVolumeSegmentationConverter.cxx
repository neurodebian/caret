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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainModelSurfaceToVolumeSegmentationConverter.h"
#include "BrainModelVolumeHandleFinder.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelSurfaceToVolumeSegmentationConverter::BrainModelSurfaceToVolumeSegmentationConverter(
                                               BrainSet* bs,
                                               BrainModelSurface* inputSurfaceIn,
                                               VolumeFile* outputSegmentationVolumeIn,
                                               const bool fillCavitiesFlagIn,
                                               const bool fillHandlesFlagIn)
   : BrainModelAlgorithm(bs)
{
   inputSurface = inputSurfaceIn;
   outputSegmentationVolume = outputSegmentationVolumeIn;
   fillCavitiesFlag = fillCavitiesFlagIn;
   fillHandlesFlag = fillHandlesFlagIn;
}

/**
 * destructor.
 */
BrainModelSurfaceToVolumeSegmentationConverter::~BrainModelSurfaceToVolumeSegmentationConverter()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceToVolumeSegmentationConverter::execute() throw (BrainModelAlgorithmException)
{
   if (inputSurface == NULL) {
      throw BrainModelAlgorithmException("Input surface is NULL.");
   }
   if (outputSegmentationVolume == NULL) {
      throw BrainModelAlgorithmException("Output volume is NULL");
   }
   
   //
   // Get volume information
   //
   int dimensions[3];
   outputSegmentationVolume->getDimensions(dimensions);
   float spacing[3];
   outputSegmentationVolume->getSpacing(spacing);
   float origin[3];
   outputSegmentationVolume->getOrigin(origin);

   //
   // Convert the surface to a segmentation volume
   //
   const float surfaceOffset[3] = { 0.0, 0.0, 0.0 };
   BrainModelSurfaceToVolumeConverter bmsv(brainSet,
                                           inputSurface,
                                           StereotaxicSpace::SPACE_UNKNOWN,
                                           surfaceOffset,
                                           dimensions,
                                           spacing,
                                           origin,
                                           -1.5,
                                           0.0,
                                           0.5,
            BrainModelSurfaceToVolumeConverter::CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES,
            BrainModelSurfaceToVolumeConverter::INTERSECTION_MODE_INTERSECT_TILES_AND_VOXELS);
   bmsv.execute();

   //
   // copy the resulting segmentation volume
   //
   VolumeFile* vf = bmsv.getOutputVolume();
   if (vf == NULL) {
      throw BrainModelAlgorithmException("Unable to find volume created by intersecting with surface.");
   }
   const QString filename = outputSegmentationVolume->getFileName();
   const VolumeFile::FILE_READ_WRITE_TYPE fileWriteType = outputSegmentationVolume->getFileWriteType();
   *outputSegmentationVolume = *vf;
   outputSegmentationVolume->setFileWriteType(fileWriteType);
   outputSegmentationVolume->setFileName(filename);
   
   
   //
   // Remove islands in volume
   //
   outputSegmentationVolume->removeIslandsFromSegmentation();
      
   //
   // Fill cavities if requested
   //
   if (fillCavitiesFlag) {
      outputSegmentationVolume->fillSegmentationCavities();
   }
   
   //
   // If handles should be filled
   //
   if (fillHandlesFlag) {
      //
      // Find handles
      //
      BrainModelVolumeHandleFinder bmvh(brainSet,
                                        outputSegmentationVolume,
                                        false,
                                        true,
                                        true,
                                        true,
                                        false);
      bmvh.execute();
      
      //
      // Fill small handles
      //
      const int numberOfVoxelsInSmallHandle = 6;
      for (int i = 0; i < bmvh.getNumberOfHandles(); i++) {
         const BrainModelVolumeTopologicalError* handle = bmvh.getHandleInfo(i);
         std::vector<int> handleVoxels;
         handle->getHandleVoxels(handleVoxels);
         if (static_cast<int>(handleVoxels.size()) <= numberOfVoxelsInSmallHandle) {
            if (handleVoxels.size() > 0) {
               outputSegmentationVolume->setVoxel(handleVoxels, 255.0);
            }
         }
      }
   }
}

