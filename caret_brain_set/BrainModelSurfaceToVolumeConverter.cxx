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


#include <QGlobalStatic>
#include <QTextStream>

#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>

#include <QApplication>
#include <QDateTime>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainSet.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "DebugControl.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "SurfaceShapeFile.h"
#include "vtkPolygon.h"
#include "vtkTriangle.h"

/**
 * Constructor.
 */
BrainModelSurfaceToVolumeConverter::BrainModelSurfaceToVolumeConverter(
                                 BrainSet* bs,
                                 BrainModelSurface* surfaceIn,
                                 const StereotaxicSpace volumeSpaceHintIn,
                                 const float surfaceOffsetIn[3],
                                 const int volumeDimensionsIn[3],
                                 const float voxelSizeIn[3],
                                 const float volumeOriginIn[3],
                                 const float innerBoundaryIn,
                                 const float outerBoundaryIn,
                                 const float thicknessStepIn,
                                 const CONVERSION_MODE convertModeIn)
   : BrainModelAlgorithm(bs)
{
   progressDialogTotalSteps = 0;
   progressDialogCurrentSteps = 0;
   
   nodeToVoxelMappingEnabled = false;
   
   volumeSpaceHint = volumeSpaceHintIn;
   nodeAttributeColumn = 0;
   surface = surfaceIn;
   surfaceOffset[0] = surfaceOffsetIn[0];
   surfaceOffset[1] = surfaceOffsetIn[1];
   surfaceOffset[2] = surfaceOffsetIn[2];
   volumeDimensions[0] = volumeDimensionsIn[0];
   volumeDimensions[1] = volumeDimensionsIn[1];
   volumeDimensions[2] = volumeDimensionsIn[2];
   voxelSize[0] = voxelSizeIn[0];
   voxelSize[1] = voxelSizeIn[1];
   voxelSize[2] = voxelSizeIn[2];
   volumeOrigin[0] = volumeOriginIn[0];
   volumeOrigin[1] = volumeOriginIn[1];
   volumeOrigin[2] = volumeOriginIn[2];
   innerBoundary = innerBoundaryIn;
   outerBoundary = outerBoundaryIn;
   thicknessStep = thicknessStepIn;
   conversionMode = convertModeIn;
   roiVoxelValue = 255.0;

   //
   // set voxel dimensions and sizes for volumes that will
   // be resampled later.
   //
   switch(volumeSpaceHint.getSpace()) {
      case StereotaxicSpace::SPACE_UNKNOWN:
         break;
      case StereotaxicSpace::SPACE_OTHER:
         break;
      case StereotaxicSpace::SPACE_AFNI_TALAIRACH:
         break;
      case StereotaxicSpace::SPACE_FLIRT:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F6:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F99:
         break;
      case StereotaxicSpace::SPACE_MRITOTAL:
         break;
      case StereotaxicSpace::SPACE_SPM_99:
         break;
      case StereotaxicSpace::SPACE_SPM:
         break;
      case StereotaxicSpace::SPACE_SPM_95:
         break;
      case StereotaxicSpace::SPACE_SPM_96:
         break;
      case StereotaxicSpace::SPACE_SPM_2:
         break;
      case StereotaxicSpace::SPACE_SPM_5:
         break;
      case StereotaxicSpace::SPACE_T88:
         break;
      case StereotaxicSpace::SPACE_WU_7112B:
      case StereotaxicSpace::SPACE_WU_7112B_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112B_222:
         volumeDimensions[0] = 256;
         volumeDimensions[1] = 256;
         volumeDimensions[2] = 150;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_WU_7112B_333:
         volumeDimensions[0] = 144;
         volumeDimensions[1] = 192;
         volumeDimensions[2] = 144;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_WU_7112C:
      case StereotaxicSpace::SPACE_WU_7112C_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112C_222:
         volumeDimensions[0] = 256;
         volumeDimensions[1] = 256;
         volumeDimensions[2] = 150;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_WU_7112C_333:
         volumeDimensions[0] = 144;
         volumeDimensions[1] = 192;
         volumeDimensions[2] = 144;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_WU_7112O:
      case StereotaxicSpace::SPACE_WU_7112O_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112O_222:
         volumeDimensions[0] = 256;
         volumeDimensions[1] = 256;
         volumeDimensions[2] = 150;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_WU_7112O_333:
         volumeDimensions[0] = 144;
         volumeDimensions[1] = 192;
         volumeDimensions[2] = 144;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_WU_7112Y:
      case StereotaxicSpace::SPACE_WU_7112Y_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112Y_222:
         volumeDimensions[0] = 256;
         volumeDimensions[1] = 256;
         volumeDimensions[2] = 150;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_WU_7112Y_333:
         volumeDimensions[0] = 144;
         volumeDimensions[1] = 192;
         volumeDimensions[2] = 144;
         voxelSize[0] = 1.0;
         voxelSize[1] = 1.0;
         voxelSize[2] = 1.0;
         break;
      case StereotaxicSpace::SPACE_NUMBER_OF_SPACES:
         break;
   }
}

/**
 * Destructor.
 */
BrainModelSurfaceToVolumeConverter::~BrainModelSurfaceToVolumeConverter()
{
}

/**
 * Execute.
 */
void
BrainModelSurfaceToVolumeConverter::execute() throw (BrainModelAlgorithmException)
{
   if ((volumeDimensions[0] <= 0) || 
       (volumeDimensions[1] <= 0) ||
       (volumeDimensions[2] <= 0)) {
      throw BrainModelAlgorithmException("Volume dimensions must be greater than zero.");
   }
   if ((voxelSize[0] <= 0.0) || 
       (voxelSize[1] <= 0.0) ||
       (voxelSize[2] <= 0.0)) {
      throw BrainModelAlgorithmException("Voxel sizes must be greater than zero.");
   }
   if (innerBoundary > outerBoundary) {
      throw BrainModelAlgorithmException("Inner boundary must be less than outer boundary.");
   }
   
   nodeToVoxelMapping.clear();
   
   QTime timer;
   timer.start();
   
   //
   // See if a progress dialog should be created
   //
   createProgressDialog("Converting Surface to Volume",
                        10,
                        "surfaceToVolumeProgressDialog");
   //
   // Create a volume 
   //
   volume = new VolumeFile;
   
   //
   // the study meta data link
   //
   StudyMetaDataLinkSet studyMetaDataLinkSet;
   
   //
   // Set the type of volume and voxels
   //
   bool createSegmentationFlag = false;
   VolumeFile::VOXEL_DATA_TYPE voxelDataType = VolumeFile::VOXEL_DATA_TYPE_UNKNOWN;
   VolumeFile::VOLUME_TYPE volumeType = VolumeFile::VOLUME_TYPE_UNKNOWN;
   switch(conversionMode) {
      case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
         volumeType = VolumeFile::VOLUME_TYPE_RGB; 
         voxelDataType = VolumeFile::VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED;
         break;
      case CONVERT_TO_ROI_VOLUME_USING_PAINT:
         volumeType = VolumeFile::VOLUME_TYPE_PAINT; 
         voxelDataType = VolumeFile::VOXEL_DATA_TYPE_FLOAT;
         {
            PaintFile* pf = brainSet->getPaintFile();
            if ((nodeAttributeColumn < 0) || (nodeAttributeColumn >= pf->getNumberOfColumns())) {
               removeProgressDialog();
               throw BrainModelAlgorithmException("Invalid paint column");
            }
            studyMetaDataLinkSet = pf->getColumnStudyMetaDataLinkSet(nodeAttributeColumn);
         }
         break;
      case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
      case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
         volumeType = VolumeFile::VOLUME_TYPE_FUNCTIONAL; 
         voxelDataType = VolumeFile::VOXEL_DATA_TYPE_FLOAT;
         {
            MetricFile* mf = brainSet->getMetricFile();
            if ((nodeAttributeColumn < 0) || (nodeAttributeColumn >= mf->getNumberOfColumns())) {
               removeProgressDialog();
               throw BrainModelAlgorithmException("Invalid metric column");
            }
            studyMetaDataLinkSet = mf->getColumnStudyMetaDataLinkSet(nodeAttributeColumn);
         }
         break;
      case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
         volumeType = VolumeFile::VOLUME_TYPE_FUNCTIONAL; 
         voxelDataType = VolumeFile::VOXEL_DATA_TYPE_FLOAT;
         {
            SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
            if ((nodeAttributeColumn < 0) || (nodeAttributeColumn >= ssf->getNumberOfColumns())) {
               removeProgressDialog();
               throw BrainModelAlgorithmException("Invalid surface shape column");
            }
            studyMetaDataLinkSet = ssf->getColumnStudyMetaDataLinkSet(nodeAttributeColumn);
         }
         break;
      case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
         volumeType = VolumeFile::VOLUME_TYPE_SEGMENTATION; 
         voxelDataType = VolumeFile::VOXEL_DATA_TYPE_FLOAT;
         break;
      case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
         volumeType = VolumeFile::VOLUME_TYPE_SEGMENTATION; 
         voxelDataType = VolumeFile::VOXEL_DATA_TYPE_FLOAT;
         createSegmentationFlag = true;
         break;
   }

   //
   // Update/Create the volume
   //
   VolumeFile::ORIENTATION orient[3] = { 
                                 VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                 VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                 VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR };
   volume->initialize(voxelDataType,
                  volumeDimensions,
                  orient,
                  volumeOrigin,
                  voxelSize,
                  false,
                  true);
   volume->setVolumeType(volumeType);
   volume->setStudyMetaDataLinkSet(studyMetaDataLinkSet);
   
   const int numberOfVoxels = volume->getTotalNumberOfVoxels();
   voxelSet.resize(numberOfVoxels, false);
   bsnc = brainSet->getNodeColoring();
   
   const TopologyFile* tf = surface->getTopologyFile();
   const int numTiles = tf->getNumberOfTiles();
   
   const QString surfaceFileName(surface->getFileName());
   
   try {
      //
      // Determing thickness of surfaces
      //
      const float thickness = outerBoundary - innerBoundary;

      //
      // Creating a segmentation volume ?
      if (createSegmentationFlag) {
         //
         // Copy the user's surface since we don't want to modify it
         //
         BrainModelSurface thickSurface(*surface);
         surface = &thickSurface;
         cf = surface->getCoordinateFile();
         
         //
         // See if surface in native space
         //
         float bounds[6];
         surface->getBounds(bounds);
         
         //
         // Offset based on structure
         //
         float offset = 1.0;
         switch (surface->getStructure().getType()) {
            case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
               offset = -10.0;
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
               offset = 10.0;
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
               offset = 1.0;
               break;
            case Structure::STRUCTURE_TYPE_CEREBELLUM:
               offset = 1.0;
               break;
            case Structure::STRUCTURE_TYPE_INVALID:
               offset = 1.0;
               break;
         }
         
         //
         // Guess at location inside surface
         //
         const float middleOfSurface[3] = {
            (bounds[0] + bounds[1]) * 0.5,
            (bounds[2] + bounds[3]) * 0.5,
            (bounds[4] + bounds[5]) * 0.5
         };

         //
         // Get the middle voxel
         //
         int middleIJK[3];
         //volume->convertCoordinatesToVoxelIJK(insideSurface, middleIJK);
         volume->convertCoordinatesToVoxelIJK(middleOfSurface, middleIJK);
         
         //
         // Get the X voxel start and stop used to find interior voxel
         //
         int dim[3];
         volume->getDimensions(dim);
         int startX = 0;
         int endX   = dim[0] - 1;
         int deltaX = 1;
         switch (surface->getStructure().getType()) {
            case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
               startX = dim[0] - 1;
               endX   = 0;
               deltaX = -1;
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
            case Structure::STRUCTURE_TYPE_CEREBELLUM:
            case Structure::STRUCTURE_TYPE_INVALID:
               break;
         }
         
         //
         // Start at original surface and shrink until surface is closed
         //
         const float nodeDelta = -0.5;
         progressDialogTotalSteps = 3 * numTiles;
         for (int i = 1; i <= 20; i++) {
            //
            // Intersect volume with surface
            //
            conversionIntersectTilesAndVoxels();
            
            //
            // Wait until few iterations until checking for a closed boundary
            //
            if (i >= 3) {
               //
               // Make a copy of the volume
               //
               VolumeFile volumeCopy(*volume);
               
               //
               // Find a  voxel inside the volume
               //
               bool found = false;
               int seedVoxel[3] = {
                  middleIJK[0],
                  middleIJK[1],
                  middleIJK[2],
               };
               float lastVoxel = 0.0;
               for (int k = startX; k != endX; k += deltaX) {
                  int ijk[3] = {
                     k,
                     middleIJK[1],
                     middleIJK[2]
                  };
                  
                  const float newVoxel = volumeCopy.getVoxel(ijk);
                  if (k == startX) {
                     lastVoxel = newVoxel;
                  }
                  
                  if ((newVoxel == 0.0) &&
                      (lastVoxel != 0.0)) {
                     seedVoxel[0] = ijk[0];
                     seedVoxel[1] = ijk[1];
                     seedVoxel[2] = ijk[2];
                     found = true;
                     break;
                  }
                  
                  lastVoxel = newVoxel;
               }
               
               if (DebugControl::getDebugOn()) {
                  std::cout << "Surface to Volume Inside volume seed: "
                            << seedVoxel[0] << ", "
                            << seedVoxel[1] << ", "
                            << seedVoxel[2] << std::endl;
               }
                         
               //
               // flood fill interior 
               //
               //volumeCopy.convertCoordinatesToVoxelIJK(insideSurface, seedVoxel);
               const unsigned char red[4] = { 255, 0, 0, 1 };
               volumeCopy.performSegmentationOperation(VolumeFile::SEGMENTATION_OPERATION_FLOOD_FILL_3D,
                                                       VolumeFile::VOLUME_AXIS_Z,
                                                       true,
                                                       seedVoxel,
                                                       seedVoxel,
                                                       255.0,
                                                       red);
               //
               // If corner voxel not set, then surface outline in volume must be closed
               //
               if (volumeCopy.getVoxel(0, 0, 0) == 0) {
                  volumeCopy.fillSegmentationCavities();
                  for (int j = 0; j < volume->getTotalNumberOfVoxelElements(); j++) {
                     volume->setVoxelWithFlatIndex(j, 0, volumeCopy.getVoxelWithFlatIndex(j));
                  }
                  break;
               }
            }
            
            surface->expandSurface(nodeDelta);
         }
      }
      else if (thickness > 0.0) {
         //
         // Copy the user's surface since we don't want to modify it
         //
         BrainModelSurface thickSurface(*surface);
         surface = &thickSurface;
         cf = surface->getCoordinateFile();
         
         //
         // Shrink nodes "inward" by half of thickness then
         // intersect and expand surface
         //
         float nodeDelta = innerBoundary;  //-(thickness * 0.5);
         surface->expandSurface(nodeDelta);
         const int steps = static_cast<int>(thickness / thicknessStep);
         progressDialogTotalSteps = (steps + 1) * numTiles;
         for (int i = 0; i <= steps; i++) {
            conversionIntersectTilesAndVoxels();
            if (i < steps) {
               surface->expandSurface(thicknessStep);
            }
         }
      }
      else {
         progressDialogTotalSteps = numTiles;
         cf = surface->getCoordinateFile();
         conversionIntersectTilesAndVoxels();
      }
   }
   catch (BrainModelAlgorithmException& e) {
      removeProgressDialog();
      throw e;
   }
   
   switch(conversionMode) {
      case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
         break;
      case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
         break;
      case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
         break;
      case CONVERT_TO_ROI_VOLUME_USING_PAINT:
         {
            //
            // Find out which paint names were used and add them to the ROI names
            //
            PaintFile* pf = brainSet->getPaintFile();
            std::vector<int> paintNamesUsed(pf->getNumberOfPaintNames(), 0);
            for (int i = 0; i < numberOfVoxels; i++) {
               const int index = static_cast<int>(volume->getVoxelWithFlatIndex(i));
               if (index > 0) {
                  if (paintNamesUsed[index] == 0) {
                     paintNamesUsed[index] = 
                        volume->addRegionName(pf->getPaintNameFromIndex(index));
                  }
               }
            }
            
            //
            // Update voxel values for updated region indices
            //
            for (int i = 0; i < numberOfVoxels; i++) {
               const int index = static_cast<int>(volume->getVoxelWithFlatIndex(i));
               if (index > 0) {
                  volume->setVoxelWithFlatIndex(i, 0, static_cast<float>(paintNamesUsed[index]));
               }
            }
         }
         break;
      case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
         break;
      case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
         break;
      case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
         break;
   }
   //if (DebugControl::getDebugOn()) {
      std::cout << "Time to create volume was "
                << (static_cast<float>(timer.elapsed()) / 1000.0)
                << " seconds." << std::endl;
   //}
   
   switch(volumeSpaceHint.getSpace()) {
      case StereotaxicSpace::SPACE_UNKNOWN:
         break;
      case StereotaxicSpace::SPACE_OTHER:
         break;
      case StereotaxicSpace::SPACE_AFNI_TALAIRACH:
         break;
      case StereotaxicSpace::SPACE_FLIRT:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F6:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F99:
         break;
      case StereotaxicSpace::SPACE_MRITOTAL:
         break;
      case StereotaxicSpace::SPACE_SPM_99:
         break;
      case StereotaxicSpace::SPACE_SPM_2:
         break;
      case StereotaxicSpace::SPACE_SPM:
         break;
      case StereotaxicSpace::SPACE_SPM_95:
         break;
      case StereotaxicSpace::SPACE_SPM_96:
         break;
      case StereotaxicSpace::SPACE_SPM_5:
         break;
      case StereotaxicSpace::SPACE_T88:
         break;
      case StereotaxicSpace::SPACE_WU_7112B:
      case StereotaxicSpace::SPACE_WU_7112B_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112B_222:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_WU_7112B_333:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_WU_7112C:
      case StereotaxicSpace::SPACE_WU_7112C_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112C_222:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_WU_7112C_333:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_WU_7112O:
      case StereotaxicSpace::SPACE_WU_7112O_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112O_222:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_WU_7112O_333:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_WU_7112Y:
      case StereotaxicSpace::SPACE_WU_7112Y_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112Y_222:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_WU_7112Y_333:
         resampleVolumeToStandardSpace();
         break;
      case StereotaxicSpace::SPACE_NUMBER_OF_SPACES:
         break;
   }
   
   QString comm("Volume intersected with surface: ");
   comm.append(surfaceFileName);
   comm.append("\n");
   volume->appendToFileComment(comm);

   brainSet->addVolumeFile(volume->getVolumeType(), volume, "", true, false);
   
   if (nodeToVoxelMappingEnabled) {
      if (nodeToVoxelMappingFileName.isEmpty()) {
         nodeToVoxelMappingFileName = "node_to_voxel_mapping.txt";
      }
      QFile file(nodeToVoxelMappingFileName);
      if (file.open(QIODevice::WriteOnly)) {
         QTextStream stream(&file);
         
         for (std::set<NodeToVoxelMapping>::iterator iter = nodeToVoxelMapping.begin();
            iter != nodeToVoxelMapping.end(); iter++) {
            const NodeToVoxelMapping nvm = *iter;
            stream << nvm.nodeNumber
                   << " (" << nvm.voxelIJK[0]
                   << ", " << nvm.voxelIJK[1]
                   << ", " << nvm.voxelIJK[2] << ")" << "\n";
         }
         
         file.close();
      }
   }
   
   removeProgressDialog();
}

/**
 * Resample the volume to a standard space.
 */
void 
BrainModelSurfaceToVolumeConverter::resampleVolumeToStandardSpace()
{
   int voxelStep = 0;
   
   switch(volumeSpaceHint.getSpace()) {
      case StereotaxicSpace::SPACE_UNKNOWN:
         break;
      case StereotaxicSpace::SPACE_OTHER:
         break;
      case StereotaxicSpace::SPACE_AFNI_TALAIRACH:
         break;
      case StereotaxicSpace::SPACE_FLIRT:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F6:
         break;
      case StereotaxicSpace::SPACE_MACAQUE_F99:
         break;
      case StereotaxicSpace::SPACE_MRITOTAL:
         break;
      case StereotaxicSpace::SPACE_SPM_99:
         break;
      case StereotaxicSpace::SPACE_SPM_2:
         break;
      case StereotaxicSpace::SPACE_SPM:
         break;
      case StereotaxicSpace::SPACE_SPM_95:
         break;
      case StereotaxicSpace::SPACE_SPM_96:
         break;
      case StereotaxicSpace::SPACE_SPM_5:
         break;
      case StereotaxicSpace::SPACE_T88:
         break;
      case StereotaxicSpace::SPACE_WU_7112B:
      case StereotaxicSpace::SPACE_WU_7112B_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112B_222:
         voxelStep = 2;
         break;
      case StereotaxicSpace::SPACE_WU_7112B_333:
         voxelStep = 3;
         break;
      case StereotaxicSpace::SPACE_WU_7112C:
      case StereotaxicSpace::SPACE_WU_7112C_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112C_222:
         voxelStep = 2;
         break;
      case StereotaxicSpace::SPACE_WU_7112C_333:
         voxelStep = 3;
         break;
      case StereotaxicSpace::SPACE_WU_7112O:
      case StereotaxicSpace::SPACE_WU_7112O_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112O_222:
         voxelStep = 2;
         break;
      case StereotaxicSpace::SPACE_WU_7112O_333:
         voxelStep = 3;
         break;
      case StereotaxicSpace::SPACE_WU_7112Y:
      case StereotaxicSpace::SPACE_WU_7112Y_111:
         break;
      case StereotaxicSpace::SPACE_WU_7112Y_222:
         voxelStep = 2;
         break;
      case StereotaxicSpace::SPACE_WU_7112Y_333:
         voxelStep = 3;
         break;
      case StereotaxicSpace::SPACE_NUMBER_OF_SPACES:
         break;
   }

   if (voxelStep <= 0) {
      return;
   }
   
   //
   // Half the number of voxels in the 111 surface when convert to new voxel size
   //
   const float halfVoxelCube = std::pow(static_cast<double>(voxelStep), 
                                        3.0) * 0.5;
   
   //
   // Save current volume and create a new volume
   //
   VolumeFile* oldVolume = volume;
   volume = new VolumeFile(*volume);
   
   //
   // Set the new volumes parameters
   //
   float origin[3];
   StereotaxicSpace theSpace(volumeSpaceHint);
   const QString name = theSpace.getName();
   theSpace.getDimensions(volumeDimensions);
   theSpace.getOrigin(origin);
   theSpace.getVoxelSize(voxelSize);

   VolumeFile::ORIENTATION orient[3];
   oldVolume->getOrientation(orient);
   volume->initialize(oldVolume->getVoxelDataType(),
                      volumeDimensions,
                      orient,
                      origin,
                      voxelSize);
   volume->setVolumeType(oldVolume->getVolumeType());
                   
   //
   // Copy the region names
   //
   volume->deleteAllRegionNames();
   for (int i = 0; i < oldVolume->getNumberOfRegionNames(); i++) {
      volume->addRegionName(oldVolume->getRegionNameFromIndex(i));
   }

   //
   // Get access to old voxels
   //
   const int oldNumberOfScalars = oldVolume->getTotalNumberOfVoxels();
   if (oldNumberOfScalars == 0) {
      return;
   }
   
   //
   // loop through dimensions of new volume
   //
   for (int i = 0; i < volumeDimensions[0]; i++) {
      for (int j = 0; j < volumeDimensions[1]; j++) {
         for (int k = 0; k < volumeDimensions[2]; k++) {
            
            float voxelCount = 0.0;
            float sumRed     = 0.0;
            float sumGreen   = 0.0;
            float sumBlue    = 0.0;
            std::map<int,int> usedPaints;
            float sumMetric  = 0.0;
            float sumSurfaceShape = 0.0;
            
            
            for (int ii = (i * voxelStep); ii < (i * voxelStep + voxelStep); ii++) {
               for (int jj = (j * voxelStep); jj < (j * voxelStep + voxelStep); jj++) {
                  for (int kk = (k * voxelStep); kk < (k * voxelStep + voxelStep); kk++) {
                     
                     switch(conversionMode) {
                        case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
                           {
                              const float red = oldVolume->getVoxel(ii, jj, kk, 0);
                              const float green = oldVolume->getVoxel(ii, jj, kk, 1);
                              const float blue = oldVolume->getVoxel(ii, jj, kk, 2);
                              if ((red > 0.0) || (green > 0.0) || (blue > 0.0)) {
                                 sumRed   += red;
                                 sumGreen += green;
                                 sumBlue  += blue;
                                 voxelCount += 1.0;
                              }
                           }
                           break;
                        case CONVERT_TO_ROI_VOLUME_USING_PAINT:
                           {
                              const int paintIndex = static_cast<int>(oldVolume->getVoxel(ii, jj, kk));
                              if (paintIndex > 0) {
                                 usedPaints[paintIndex]++;
                                 voxelCount += 1.0;
                              }
                           }
                           break;
                        case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
                           {
                              const float value = oldVolume->getVoxel(ii, jj, kk);
                              if (value != 0.0) {
                                 sumMetric += value;
                                 voxelCount += 1.0;
                              }
                           }
                           break;
                        case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
                           {
                              const float value = oldVolume->getVoxel(ii, jj, kk);
                              if (value != 0.0) {
                                 if (value > sumMetric) {
                                    sumMetric = value;
                                    voxelCount = 1.0; // NOT INTERPOLATING
                                 }
                              }
                           }
                           break;
                        case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
                           {
                              const float value = oldVolume->getVoxel(ii, jj, kk);
                              if (value != 0.0) {
                                 sumSurfaceShape += value;
                                 voxelCount += 1.0;
                              }
                           }
                           break;
                        case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
                           if (oldVolume->getVoxel(ii, jj, kk) > 0) {
                              voxelCount += 1.0;
                           }
                           break;
                        case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
                           if (oldVolume->getVoxel(ii, jj, kk) > 0) {
                              voxelCount += 1.0;
                           }
                           break;
                     }
                  } // kk
               } // jj
            } // ii
            
            //
            // Are enough voxels set ?
            //
            switch(conversionMode) {
               case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
                  if (voxelCount >= halfVoxelCube) {
                     volume->setVoxel(i, j, k, 0, (sumRed   / voxelCount));
                     volume->setVoxel(i, j, k, 1, (sumGreen / voxelCount));
                     volume->setVoxel(i, j, k, 2, (sumBlue  / voxelCount));
                  }
                  else {
                     volume->setVoxel(i, j, k, 0, 0.0);
                     volume->setVoxel(i, j, k, 1, 0.0);
                     volume->setVoxel(i, j, k, 2, 0.0);
                  }
                  break;
               case CONVERT_TO_ROI_VOLUME_USING_PAINT:
                  if (voxelCount >= halfVoxelCube) {
                     int mostUsedIndex = -1;
                     int mostUsedQuantity = 0;
                     for (std::map<int,int>::iterator iter = usedPaints.begin();
                          iter != usedPaints.end(); iter++) {
                        if (iter->second > mostUsedQuantity) {
                           mostUsedQuantity = iter->second;
                           mostUsedIndex    = iter->first;
                        }
                     }
                     volume->setVoxel(i, j, k, 0, mostUsedIndex);
                  }
                  else {
                      volume->setVoxel(i, j, k, 0, 0.0);
                  }
                  break;
               case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
                  if (voxelCount >= halfVoxelCube) {
                      volume->setVoxel(i, j, k, 0, roiVoxelValue);
                  }
                  else {
                      volume->setVoxel(i, j, k, 0, 0.0);
                  }
                  break;
               case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
                  if (voxelCount >= halfVoxelCube) {
                      volume->setVoxel(i, j, k, 0, roiVoxelValue);
                  }
                  else {
                      volume->setVoxel(i, j, k, 0, 0.0);
                  }
                  break;
               case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
                  if (voxelCount >= halfVoxelCube) {
                      volume->setVoxel(i, j, k, 0, sumMetric / voxelCount);
                  }
                  else {
                      volume->setVoxel(i, j, k, 0, 0.0);
                  }
                  break;
               case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
                  if (voxelCount > 0) {
                      volume->setVoxel(i, j, k, 0, sumMetric / voxelCount);
                  }
                  else {
                      volume->setVoxel(i, j, k, 0, 0.0);
                  }
                  break;
               case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
                  if (voxelCount >= halfVoxelCube) {
                      volume->setVoxel(i, j, k, 0, sumSurfaceShape / voxelCount);
                  }
                  else {
                      volume->setVoxel(i, j, k, 0, 0.0);
                  }
                  break;
            }
         }  // k
      }  // j
   }  // i
   
   //
   // Get rid of the old volume
   //
   delete oldVolume;
}

/**
 * Perform conversion by intersecting voxels and tiles.
 */
void
BrainModelSurfaceToVolumeConverter::conversionIntersectTilesAndVoxels() 
                                            throw (BrainModelAlgorithmException)
{
   const TopologyFile* tf = surface->getTopologyFile();
   const int numTiles = tf->getNumberOfTiles();
   PaintFile* paintFile = brainSet->getPaintFile();
   MetricFile* metricFile = brainSet->getMetricFile();
   SurfaceShapeFile* surfaceShapeFile = brainSet->getSurfaceShapeFile();
   
   const BrainModelSurfaceROINodeSelection* surfaceROI =
           brainSet->getBrainModelSurfaceRegionOfInterestNodeSelection();
           
   for (int m = 0; m < numTiles; m++) {
      //
      // Get the nodes in the tiles
      //
      int n1, n2, n3;
      tf->getTile(m, n1, n2, n3);
      
      bool useTile = false;
      float rgbFloat[3] = { 0.0, 0.0, 0.0 };
      int paintIndex = 0;
      float metricValue = 0.0;
      float surfaceShapeValue = 0.0;
      switch(conversionMode) {
         case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
            //
            // If converting to RGB, get color for tile
            //
            getTilesRgbColor(n1, n2, n3, rgbFloat);
            useTile = true;
            break;
         case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
            //
            // See if any of the tile's nodes are part of the query
            //
            {
               if (surfaceROI->getNodeSelected(n1) ||
                   surfaceROI->getNodeSelected(n2) ||
                   surfaceROI->getNodeSelected(n3)) {
                  useTile = true;
               }
            }
            break;
         case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
            //
            // Use all tiles
            //
            useTile = true;
            break;
         case CONVERT_TO_ROI_VOLUME_USING_PAINT:
            {
               //
               // Get paint indices for the nodes of the tile
               //
               const int p1 = paintFile->getPaint(n1, nodeAttributeColumn);
               const int p2 = paintFile->getPaint(n2, nodeAttributeColumn);
               const int p3 = paintFile->getPaint(n3, nodeAttributeColumn);
               
               //
               // Do any of the nodes have valid paint indices
               //
               if ((p1 > 0) || (p2 > 0) || (p3 > 0)) {
                  //
                  // Use the tile and find a paint index to use for the tile
                  //
                  useTile = true;
                  if (p1 == p2) {
                     paintIndex = p1;
                  }
                  else if (p1 == p3) {
                     paintIndex = p1;
                  }
                  else if (p2 == p3) {
                     paintIndex = p2;
                  }
                  else if (p1 > 0) {
                     paintIndex = p1;
                  }
                  else if (p2 > 0) {
                     paintIndex = p2;
                  }
                  else if (p3 > 0) {
                     paintIndex = p3;
                  }
                  else {
                     useTile = false;
                  }
               }
            }
            break;
         case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
            metricValue = (metricFile->getValue(n1, nodeAttributeColumn) +
                           metricFile->getValue(n2, nodeAttributeColumn) +
                           metricFile->getValue(n3, nodeAttributeColumn)) / 3.0;
            useTile = true;
            break;
         case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
            metricValue = std::max(metricFile->getValue(n1, nodeAttributeColumn),
                                   std::max(metricFile->getValue(n2, nodeAttributeColumn),
                                            metricFile->getValue(n3, nodeAttributeColumn)));
            useTile = true;
            break;
         case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
            surfaceShapeValue = (surfaceShapeFile->getValue(n1, nodeAttributeColumn) +
                           surfaceShapeFile->getValue(n2, nodeAttributeColumn) +
                           surfaceShapeFile->getValue(n3, nodeAttributeColumn)) / 3.0;
            useTile = true;
            break;
      }
      
      if (useTile) {
         //
         // Get the coordinates of the tile's nodes
         //
         float v1[3], v2[3], v3[3];
         cf->getCoordinate(n1, v1);
         cf->getCoordinate(n2, v2);
         cf->getCoordinate(n3, v3);
         
         //
         // Add the offsets to each node
         //
         for (int j = 0; j < 3; j++) {
            v1[j] += surfaceOffset[j];
            v2[j] += surfaceOffset[j];
            v3[j] += surfaceOffset[j];
         }
   
         //
         // Min and Max of coordinates
         //
         float minCoord[3];
         minCoord[0] = std::min(v1[0], std::min(v2[0], v3[0]));
         minCoord[1] = std::min(v1[1], std::min(v2[1], v3[1]));
         minCoord[2] = std::min(v1[2], std::min(v2[2], v3[2]));
         float maxCoord[3];
         maxCoord[0] = std::max(v1[0], std::max(v2[0], v3[0]));
         maxCoord[1] = std::max(v1[1], std::max(v2[1], v3[1]));
         maxCoord[2] = std::max(v1[2], std::max(v2[2], v3[2]));
         
         //
         // Convert the min & max of the coordinates into voxel indices
         // ComputeStructuredCoordinates returns 0 if not in the volume
         //
         int minIJK[3];
         int maxIJK[3];         
         float pcoords[3];
         if ((volume->convertCoordinatesToVoxelIJK(minCoord, minIJK, pcoords) != 0) &&
             (volume->convertCoordinatesToVoxelIJK(maxCoord, maxIJK, pcoords) != 0)) {
            const int minX = minIJK[0];
            const int minY = minIJK[1];
            const int minZ = minIJK[2];
            const int maxX = maxIJK[0];
            const int maxY = maxIJK[1];
            const int maxZ = maxIJK[2];
            //
            // All nodes withing the same voxel ?
            //
            if ((minX == maxX) && (minY == maxY) && (minZ == maxZ)) {
               int ijk[3] = { minX, minY, minZ };
               
               //
               // Keep track of node voxel assignments
               //
               if (nodeToVoxelMappingEnabled) {
                  nodeToVoxelMapping.insert(NodeToVoxelMapping(n1, ijk));
                  nodeToVoxelMapping.insert(NodeToVoxelMapping(n2, ijk));
                  nodeToVoxelMapping.insert(NodeToVoxelMapping(n3, ijk));
               }
               
               const int voxNum = volume->getVoxelNumber(ijk);
               if (voxelSet[voxNum] == false) {
                  voxelSet[voxNum] = true;
                  switch(conversionMode) {
                     case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
                        volume->setVoxel(ijk, 0, rgbFloat[0]);
                        volume->setVoxel(ijk, 1, rgbFloat[1]);
                        volume->setVoxel(ijk, 2, rgbFloat[2]);
                        break;
                     case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
                        volume->setVoxel(ijk, 0, roiVoxelValue);
                        break;
                     case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
                        volume->setVoxel(ijk, 0, roiVoxelValue);
                        break;
                     case CONVERT_TO_ROI_VOLUME_USING_PAINT:
                        volume->setVoxel(ijk, 0, paintIndex);
                        break;
                     case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
                        volume->setVoxel(ijk, 0, metricValue);
                        break;
                     case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
                        volume->setVoxel(ijk, 0, metricValue);
                        break;
                     case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
                        volume->setVoxel(ijk, 0, surfaceShapeValue);
                        break;
                  }
               }
            }
            else {
               //
               // First, set the voxel that each of the nodes falls within
               //
               for (int j = 0; j < 3; j++) {
                  float* xyz = NULL;
                  switch(j) {
                     case 0:
                        xyz = v1;
                        break;
                     case 1:
                        xyz = v2;
                        break;
                     case 2:
                        xyz = v3;
                        break;
                  }
                  if (xyz != NULL) {
                     int ijk[3];
                     float pcoords[3];
                     if (volume->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords) != 0) {
                        //
                        // Keep track of node voxel assignments
                        //
                        if (nodeToVoxelMappingEnabled) {
                           nodeToVoxelMapping.insert(NodeToVoxelMapping(n1, ijk));
                           nodeToVoxelMapping.insert(NodeToVoxelMapping(n2, ijk));
                           nodeToVoxelMapping.insert(NodeToVoxelMapping(n3, ijk));
                        }
               
                        const int voxNum = volume->getVoxelNumber(ijk);
                        if (voxelSet[voxNum] == false) {
                           voxelSet[voxNum] = true;
                           switch(conversionMode) {
                              case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
                                 volume->setVoxel(ijk, 0, rgbFloat[0]);
                                 volume->setVoxel(ijk, 1, rgbFloat[1]);
                                 volume->setVoxel(ijk, 2, rgbFloat[2]);
                                 break;
                              case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
                                 volume->setVoxel(ijk, 0, roiVoxelValue);
                                 break;
                              case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
                                 volume->setVoxel(ijk, 0, roiVoxelValue);
                                 break;
                              case CONVERT_TO_ROI_VOLUME_USING_PAINT:
                                 volume->setVoxel(ijk, 0, paintIndex);
                                 break;
                              case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
                                 volume->setVoxel(ijk, 0, metricValue);
                                 break;
                              case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
                                 volume->setVoxel(ijk, 0, metricValue);
                                 break;
                              case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
                                 volume->setVoxel(ijk, 0, surfaceShapeValue);
                                 break;
                           }
                        }
                     }
                  }
               }
               
               
               //
               // Create a vtkTriangle and see if voxel edges pass through it
               //
               vtkPoints* pts = vtkPoints::New();
               pts->SetNumberOfPoints(3);
               pts->InsertPoint(0, v1);
               pts->InsertPoint(1, v2);
               pts->InsertPoint(2, v3);
               vtkIdType ids[3] = { 0, 1, 2 };
               vtkTriangle* triangle = vtkTriangle::New();
               triangle->Initialize(3, ids, pts);
               for (int k = minZ; k <= maxZ; k++) {
                  for (int j = minY; j <= maxY; j++) {
                     for (int i = minX; i <= maxX; i++) {
                        int ijk[3] = { i, j, k };
                        const int voxNum = volume->getVoxelNumber(i, j, k);
                        if (voxelSet[voxNum] == false) {
                           if (intersectTriangleWithVoxel(triangle, v1, v2, v3, ijk)) {
                              voxelSet[voxNum] = true;
                              switch(conversionMode) {
                                 case CONVERT_TO_RGB_VOLUME_USING_NODE_COLORING:
                                    volume->setVoxel(ijk, 0, rgbFloat[0]);
                                    volume->setVoxel(ijk, 1, rgbFloat[1]);
                                    volume->setVoxel(ijk, 2, rgbFloat[2]);
                                    break;
                                 case CONVERT_TO_ROI_VOLUME_USING_ROI_NODES:
                                    volume->setVoxel(ijk, 0, roiVoxelValue);
                                    break;
                                 case CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES:
                                    volume->setVoxel(ijk, 0, roiVoxelValue);
                                    break;
                                 case CONVERT_TO_ROI_VOLUME_USING_PAINT:
                                    volume->setVoxel(ijk, 0, paintIndex);
                                    break;
                                 case CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE:
                                    volume->setVoxel(ijk, 0, metricValue);
                                    break;
                                 case CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE:
                                    volume->setVoxel(ijk, 0, metricValue);
                                    break;
                                 case CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE:
                                    volume->setVoxel(ijk, 0, surfaceShapeValue);
                                    break;
                              }
                              //
                              // Keep track of node voxel assignments
                              //
                              if (nodeToVoxelMappingEnabled) {
                                 nodeToVoxelMapping.insert(NodeToVoxelMapping(n1, ijk));
                                 nodeToVoxelMapping.insert(NodeToVoxelMapping(n2, ijk));
                                 nodeToVoxelMapping.insert(NodeToVoxelMapping(n3, ijk));
                              }
               
                           }
                        }
                        else {  // if (voxelSet[pointID])
                           //
                           // Keep track of node voxel assignments.
                           // Note: Voxel has already been set so its check is skipped above
                           // but just see if the tile also intersects this voxel.
                           //
                           if (nodeToVoxelMappingEnabled) {
                              if (intersectTriangleWithVoxel(triangle, v1, v2, v3, ijk)) {
                                 nodeToVoxelMapping.insert(NodeToVoxelMapping(n1, ijk));
                                 nodeToVoxelMapping.insert(NodeToVoxelMapping(n2, ijk));
                                 nodeToVoxelMapping.insert(NodeToVoxelMapping(n3, ijk));
                              }
                           }
                        }
                     }
                  }
               }
               triangle->Delete();
               pts->Delete();
            }
         } // ComputeStructuredCoordinates
      }  // if (useTile)
      
      //
      // Update progress dialog
      //
      if (m > 0) {
         const int stepInterval = 1000;
         if ((m % stepInterval) == 0) {
            progressDialogCurrentSteps += stepInterval;
            int percent = static_cast<int>( 100.0 * 
                 (static_cast<float>(progressDialogCurrentSteps) / 
                  static_cast<float>(progressDialogTotalSteps)));
            if (percent > 99) {
               percent = 99;
            }
            QString text = QString("%1% Complete.").arg(percent);
            updateProgressDialog(text,
                                     progressDialogCurrentSteps,
                                     progressDialogTotalSteps);
         }
      }      
   }  // for
}

/**
 * See if a triangle and a voxel intersect.
 */
bool
BrainModelSurfaceToVolumeConverter::intersectTriangleWithVoxel(vtkTriangle* triangle, 
                                                               float t1In[3],
                                                               float t2In[3],
                                                               float t3In[3],
                                                               const int ijk[3])
{
#ifdef HAVE_VTK5
   double t1[3] = { t1In[0], t1In[1], t1In[2] };
   double t2[3] = { t2In[0], t2In[1], t2In[2] };
   double t3[3] = { t3In[0], t3In[1], t3In[2] };
#else // HAVE_VTK5
   float t1[3] = { t1In[0], t1In[1], t1In[2] };
   float t2[3] = { t2In[0], t2In[1], t2In[2] };
   float t3[3] = { t3In[0], t3In[1], t3In[2] };
#endif // HAVE_VTK5

   const float tolerance = 0.01;
   
   //
   // Voxel sizes
   //
   const float dx = voxelSize[0];
   const float dy = voxelSize[1];
   const float dz = voxelSize[2];
   
   //
   // Convert the volume index into a coordinate
   //
   float xyz[3];
   volume->getVoxelCoordinate(ijk, false, xyz);
   
   //
   // Vertices of the voxel, Lower and Upper
   //
   float al[] = { xyz[0],      xyz[1],      xyz[2] };
   float bl[] = { xyz[0] + dx, xyz[1],      xyz[2] };
   float cl[] = { xyz[0] + dx, xyz[1] + dy, xyz[2] };
   float dl[] = { xyz[0],      xyz[1] + dy, xyz[2] };
   float au[] = { xyz[0],      xyz[1],      xyz[2] + dz };
   float bu[] = { xyz[0] + dx, xyz[1],      xyz[2] + dz };
   float cu[] = { xyz[0] + dx, xyz[1] + dy, xyz[2] + dz };
   float du[] = { xyz[0],      xyz[1] + dy, xyz[2] + dz };
   
   //
   // Test all 12 voxel edges for intersection with the triangle
   //
   for (int i = 0; i < 12; i++) {
      float *p1 = NULL, *p2 = NULL;
      switch(i) {
         case 0:
            p1 = al;
            p2 = bl;
            break;
         case 1:
            p1 = bl;
            p2 = cl;
            break;
         case 2:
            p1 = cl;
            p2 = dl;
            break;
         case 3:
            p1 = dl;
            p2 = al;
            break;
         case 4:
            p1 = au;
            p2 = bu;
            break;
         case 5:
            p1 = bu;
            p2 = cu;
            break;
         case 6:
            p1 = cu;
            p2 = du;
            break;
         case 7:
            p1 = du;
            p2 = au;
            break;
         case 8:
            p1 = al;
            p2 = au;
            break;
         case 9:
            p1 = bl;
            p2 = bu;
            break;
         case 10:
            p1 = cl;
            p2 = cu;
            break;
         case 11:
            p1 = dl;
            p2 = du;
            break;
      }
      
      int subid;
#ifdef HAVE_VTK5
      double t, x[3], pcoords[3];
      double p1d[3] = { p1[0], p1[1], p1[2] };
      double p2d[3] = { p2[0], p2[1], p2[2] };
      double toleranced = tolerance;
      if (triangle->IntersectWithLine(p1d, p2d, toleranced, t, x, pcoords, subid) != 0) {
#else // HAVE_VTK5
      float t, x[3], pcoords[3];
      if (triangle->IntersectWithLine(p1, p2, tolerance, t, x, pcoords, subid) != 0) {
#endif // HAVE_VTK5
         if ((t >= 0.0) && (t <= 1.0)) {
            return true;
         }
      }
   }
   
   //
   // Test triangle edges for intersection with voxel faces
   //
   for (int i = 0; i < 6; i++) {
      float *p1 = NULL, *p2 = NULL, *p3 = NULL, *p4 = NULL;
      switch(i) {
         case 0:
            p1 = al;
            p2 = bl;
            p3 = cl;
            p4 = dl;
            break;
         case 1:
            p1 = au;
            p2 = bu;
            p3 = cu;
            p4 = du;
            break;
         case 2:
            p1 = al;
            p2 = bl;
            p3 = bu;
            p4 = au;
            break;
         case 3:
            p1 = bl;
            p2 = cl;
            p3 = cu;
            p4 = bu;
            break;
         case 4:
            p1 = cl;
            p2 = dl;
            p3 = du;
            p4 = cu;
            break;
         case 5:
            p1 = dl;
            p2 = al;
            p3 = au;
            p4 = du;
            break;
      }

      //
      // Create a vtkPolygon of voxel face and see if triangle edges pass through it
      //
      vtkPoints* pts = vtkPoints::New();
      pts->SetNumberOfPoints(4);
      pts->InsertPoint(0, p1);
      pts->InsertPoint(1, p2);
      pts->InsertPoint(2, p3);
      pts->InsertPoint(3, p4);
      vtkIdType ids[4] = { 0, 1, 2, 3 };
      vtkPolygon* polygon = vtkPolygon::New();
      polygon->Initialize(4, ids, pts);

#ifdef HAVE_VTK5
      double t, x[3], pcoords[3];
#else // HAVE_VTK5
      float t, x[3], pcoords[3];
#endif // HAVE_VTK5
      int subid;
      bool found = false;
      double toleranced = tolerance;
      if (polygon->IntersectWithLine(t1, t2, toleranced, t, x, pcoords, subid) != 0) {
         if ((t >= 0.0) && (t <= 1.0)) {
            found = true;
         }
      }
      if (found == false) {
         if (polygon->IntersectWithLine(t2, t3, toleranced, t, x, pcoords, subid) != 0) {
            if ((t >= 0.0) && (t <= 1.0)) {
               found = true;
            }
         }
         if (found == false) {
            if (polygon->IntersectWithLine(t3, t1, toleranced, t, x, pcoords, subid) != 0) {
               if ((t >= 0.0) && (t <= 1.0)) {
                  found = true;
               }
            }
         }
      }
      polygon->Delete();
      pts->Delete();
      
      if (found) {
         return true;
      }
   }
   
   return false;
}

/**
 * Determine if a voxel index is valid
 */
bool
BrainModelSurfaceToVolumeConverter::getVoxelIndexValid(const int i, const int j, const int k) const
{
   if ((i >= 0) && (i < volumeDimensions[0]) &&
       (j >= 0) && (j < volumeDimensions[1]) &&
       (k >= 0) && (k < volumeDimensions[2])) {
      return true;
   }
   return false;
}

/**
 * Get the RGB average of a tile's nodes.
 */
void
BrainModelSurfaceToVolumeConverter::getTilesRgbColor(const int n1, const int n2,
                                                     const int n3, float rgbFloat[3])
{
   const float float255 = 255.0;
   
   //
   // Average the RGB values from the three nodes of the tile
   //
   const int modelNumber = surface->getBrainModelIndex();
   const unsigned char* rgb1 = bsnc->getNodeColor(modelNumber, n1);
   const unsigned char* rgb2 = bsnc->getNodeColor(modelNumber, n2);
   const unsigned char* rgb3 = bsnc->getNodeColor(modelNumber, n3);
   rgbFloat[0] = (static_cast<float>(rgb1[0]) +
                  static_cast<float>(rgb2[0]) +
                  static_cast<float>(rgb3[0])) / 3.0;
   rgbFloat[1] = (static_cast<float>(rgb1[1]) +
                  static_cast<float>(rgb2[1]) +
                  static_cast<float>(rgb3[1])) / 3.0;
   rgbFloat[2] = (static_cast<float>(rgb1[2]) +
                  static_cast<float>(rgb2[2]) +
                  static_cast<float>(rgb3[2])) / 3.0;
   rgbFloat[0] = std::min(rgbFloat[0], float255);
   rgbFloat[1] = std::min(rgbFloat[1], float255);
   rgbFloat[2] = std::min(rgbFloat[2], float255);
}

/**
 * get node to voxel mapping enabled and file name.
 */
void 
BrainModelSurfaceToVolumeConverter::getNodeToVoxelMappingEnabled(bool& enabled, 
                                                                 QString& fileName) const 
{ 
   enabled = nodeToVoxelMappingEnabled; 
   fileName = nodeToVoxelMappingFileName;
}

/**
 * set node to voxel mapping enabled and file name.
 */
void 
BrainModelSurfaceToVolumeConverter::setNodeToVoxelMappingEnabled(const bool enabled, 
                                                                 const QString& fileName) 
{ 
   nodeToVoxelMappingEnabled = enabled; 
   nodeToVoxelMappingFileName = fileName;
}

