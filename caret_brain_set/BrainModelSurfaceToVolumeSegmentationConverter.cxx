/****************************************************************************
**
** Copyright (C) 1992-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

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
            BrainModelSurfaceToVolumeConverter::CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES);
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

