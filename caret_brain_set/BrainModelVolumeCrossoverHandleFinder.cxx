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

#include <iostream>
#include <stack>

#include "BrainModelSurface.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeCrossoverHandleFinder.h"
#include "BrainModelVolumeToSurfaceConverter.h"
#include "BrainSetNodeAttribute.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "RgbPaintFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"

/**
 * Constructor.
 */
BrainModelVolumeCrossoverHandleFinder::BrainModelVolumeCrossoverHandleFinder(BrainSet* bs,
                                                           const VolumeFile* segmentationIn,
                                                           const bool addHandlesVolumeToBrainSetIn,
                                                           const bool rgbPaintSurfaceHandlesIn)
   : BrainModelAlgorithm(bs)
{
   segmentationVolume = new VolumeFile(*segmentationIn);
   handlesRgbVolume = NULL;
   crossoversVolume = NULL;
   addHandlesVolumeToBrainSet = addHandlesVolumeToBrainSetIn;   
   rgbPaintSurfaceHandles = rgbPaintSurfaceHandlesIn;
}
      
/**
 * Destructor
 */
BrainModelVolumeCrossoverHandleFinder::~BrainModelVolumeCrossoverHandleFinder()
{
   if (crossoversVolume != NULL) {
      delete crossoversVolume;
      crossoversVolume = NULL;
   }
   
   if (segmentationVolume != NULL) {
      delete segmentationVolume;
      segmentationVolume = NULL;
   }
}

/**
 * execute the algorithm
 */
void 
BrainModelVolumeCrossoverHandleFinder::execute() throw (BrainModelAlgorithmException)
{
   handlesFound.clear();
   status = STATUS_HAS_HANDLES;
   
   //
   // Remove any islands in the segmentation
   //
   segmentationVolume->removeIslandsFromSegmentation();
   
   //
   // Generate a surface
   //
   BrainSet bs;
   BrainModelVolumeToSurfaceConverter bmvsc(&bs,
                                      segmentationVolume,
                                      BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_SUREFIT_SURFACE,
                                      false,
                                      true,
                                      false);
   try {
      bmvsc.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      throw BrainModelAlgorithmException("ERROR: Unable to generate a temporary surface\n"
                                         + e.whatQString());
   }
   
   //
   // Find the fiducial surface created by surface generation
   //
   BrainModelSurface* fiducialBMS = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialBMS == NULL) {
      throw BrainModelAlgorithmException("ERROR: unable to find the temporary fiducial surface.");
   }
   
   //
   // Get the topology file
   //
   TopologyFile* fiducialTF = fiducialBMS->getTopologyFile();
   if (fiducialTF == NULL) {
      throw BrainModelAlgorithmException("ERROR: unable to find the temporary topology file.");
   }
   
   //
   // Remove islands
   //
   fiducialTF->disconnectIslands();
   
   //
   // Generate an euler count
   //
   int faces, vertices, edges, eulerCount, numHoles, numObjects;
   fiducialTF->getEulerCount(false, faces, vertices, edges, eulerCount, numHoles, numObjects);
   
   //
   // Euler count is 2 for a topologically correct closed surface
   //
   if (eulerCount == 2) {
      status = STATUS_NO_HANDLES;
      return;
   }
   
   //
   // Smooth into a sphere DO NOT DELETE SINCE PART OF BRAIN SET
   //
   BrainModelSurface* sphereSurface = new BrainModelSurface(*fiducialBMS);
   bs.addBrainModel(sphereSurface);
   sphereSurface->translateToCenterOfMass();
   sphereSurface->convertToSphereWithSurfaceArea();
   sphereSurface->arealSmoothing(1.0,
                                 1000,
                                 0,
                                 NULL,
                                 10);
   if (DebugControl::getDebugOn()) {
      try {
         sphereSurface->getCoordinateFile()->writeFile("crossover_sphere.coord");
         sphereSurface->getTopologyFile()->writeFile("crossover_sphere.topo");
      }
      catch (FileException&) {
      }
   }
   
   //
   // Do crossover check
   //
   int tileCrossovers = 0;
   int nodeCrossovers = 0;
   sphereSurface->crossoverCheck(tileCrossovers,
                                 nodeCrossovers,
                                 BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (nodeCrossovers == 0) {
      status = STATUS_NO_HANDLES;
      return;
   }
      
   //
   // Create the crossovers volume
   //
   crossoversVolume = new VolumeFile(*segmentationVolume);
   crossoversVolume->setAllVoxels(0.0);
   crossoversVolume->setFileComment("Crossovers");
   
   //
   // Set voxels that contain a crossover using fiducial surface
   //
   const int numNodes = fiducialBMS->getNumberOfNodes();
   const CoordinateFile* fiducialCoords = fiducialBMS->getCoordinateFile();
   for (int i = 0; i < numNodes; i++) {
      BrainSetNodeAttribute* bna = bs.getNodeAttributes(i);
      if (bna->getCrossover() == BrainSetNodeAttribute::CROSSOVER_YES) {
         const float* xyz = fiducialCoords->getCoordinate(i);
         int ijk[3];
         if (crossoversVolume->convertCoordinatesToVoxelIJK(xyz, ijk)) {
            crossoversVolume->setVoxel(ijk, 0, 255.0);
         }
      }
   }
   
   if (DebugControl::getDebugOn()) {
      try {
         crossoversVolume->writeFile("crossovers_volume.nii");
      }
      catch (FileException&) {
      }
   }
   
   //
   // If surface should be painted around the handles with rgb paint
   //
   if (rgbPaintSurfaceHandles) {
      highlightHandlesInSurface();
   }
   
   //
   // Dilate the crossovers volume a little bit
   //
   crossoversVolume->doVolMorphOps(1, 0);

   //
   // Find objects in crossovers volume (the approximate handles)
   //
   createVoxelHandleList();
   
   //
   // Add handles volume to brain set if requested
   //
   if (addHandlesVolumeToBrainSet && 
       (brainSet != NULL)) {      
      //
      // Get information about the input volume
      //
      int dim[3];
      segmentationVolume->getDimensions(dim);
      float origin[3];
      segmentationVolume->getOrigin(origin);
      float spacing[3];
      segmentationVolume->getSpacing(spacing);
      VolumeFile::ORIENTATION orientation[3];
      segmentationVolume->getOrientation(orientation);
      
      //
      // Create the handles volume 
      //
      handlesRgbVolume = new VolumeFile;
      handlesRgbVolume->initialize(VolumeFile::VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED, dim, 
                                orientation, origin, spacing);
      
      //
      // Dimensions of input volume
      //
      volumeDimX = dim[0];
      volumeDimY = dim[1];
      volumeDimZ = dim[2];
      
      //
      // copy the handles found along each axis into the handles volume
      //
      for (int k = 0; k < volumeDimZ; k++) {
         for (int i = 0; i < volumeDimX; i++) {
            for (int j = 0; j < volumeDimY; j++) {
               const int ijk[3] = { i, j, k };
               if (crossoversVolume->getVoxel(ijk) != 0.0) {
                  handlesRgbVolume->setVoxel(ijk, 0, 255.0);
                  handlesRgbVolume->setVoxel(ijk, 1, 0.0);
                  handlesRgbVolume->setVoxel(ijk, 2, 255.0);
               }
               else {
                  handlesRgbVolume->setVoxel(ijk, 0, 0.0);
                  handlesRgbVolume->setVoxel(ijk, 1, 0.0);
                  handlesRgbVolume->setVoxel(ijk, 2, 0.0);
               }
            }
         }
      }
   
      const QString directory = FileUtilities::dirname(segmentationVolume->getFileName());
      const QString filename  = FileUtilities::basename(segmentationVolume->getFileName());
      if (filename.isEmpty() == false) {
         QString s(directory);
         if (s.isEmpty() == false) {
            s.append("/");
         }
         s.append("handles_");
         s.append(filename);
         handlesRgbVolume->setFileName(s);
      }
      handlesRgbVolume->clearModified();
      brainSet->addVolumeFile(VolumeFile::VOLUME_TYPE_RGB,
                              handlesRgbVolume,
                              handlesRgbVolume->getFileName(),
                              true,
                              false);
   }
}

/**
 * create a list of the handles in voxel IJK.
 */
void 
BrainModelVolumeCrossoverHandleFinder::createVoxelHandleList()
{
   //
   // Find the objects in the crossovers volume
   //
   std::vector<VolumeFile::VoxelGroup> crossoverHandles;
   crossoversVolume->findObjectsWithinSegmentationVolume(crossoverHandles);
   
   //
   // Loop through the objects in the crossovers volume
   //
   const int num = static_cast<int>(crossoverHandles.size());
   for (int i = 0; i < num; i++) {
      const VolumeFile::VoxelGroup& vg = crossoverHandles[i];
      const int numVoxels = vg.getNumberOfVoxels();
      
      //
      // Find center of gravity of voxel IJKs
      //
      int ii = 0, jj = 0, kk = 0;
      for (int j = 0; j < numVoxels; j++) {
         const VolumeFile::VoxelIJK voxelIJK = vg.getVoxel(j);
         ii += voxelIJK.getI();
         jj += voxelIJK.getJ();
         kk += voxelIJK.getK();
      }
      ii /= numVoxels;
      jj /= numVoxels;
      kk /= numVoxels;
      
      const int ijk[3] = { ii, jj, kk };
      BrainModelVolumeTopologicalError h(ijk, numVoxels);
      handlesFound.push_back(h);
   }
}

/**
 * Highlight nodes in the surface around the handles.
 */
void
BrainModelVolumeCrossoverHandleFinder::highlightHandlesInSurface()
{
   //
   // Find the active fiducial surface
   //
   BrainModelSurface* fiducialSurface = brainSet->getActiveFiducialSurface();
   if (fiducialSurface == NULL) {
      return;
   }
   
   //
   // Get the Coordinate file
   //
   CoordinateFile* cf = fiducialSurface->getCoordinateFile();
   const int numCoords = cf->getNumberOfCoordinates();
   if (numCoords <= 0) {
      return;
   }
   
   //
   // Get pointer to voxels
   //
   int dim[3];
   crossoversVolume->getDimensions(dim);
   if ((dim[0] <= 0) || (dim[1] <= 0) || (dim[2] <= 0)) {
      return;
   }
   
   //
   // Get the RGB Paint File
   //
   RgbPaintFile* rgbFile = brainSet->getRgbPaintFile();
   
   //
   // Find/Create Handles column
   //
   const QString columnName("Handles");
   int columnNumber = rgbFile->getColumnWithName(columnName);
   if ((columnNumber < 0) || (columnNumber >= rgbFile->getNumberOfColumns())) {
      if (rgbFile->getNumberOfColumns() == 0) {
         rgbFile->setNumberOfNodesAndColumns(numCoords, 1);
      }
      else {
         rgbFile->addColumns(1);
      }
      columnNumber = rgbFile->getNumberOfColumns() - 1;
   }
   rgbFile->setColumnName(columnNumber, columnName);
  
   bool* handlesFlag = new bool[numCoords];

   //
   // See if node is around a handle
   //
   for (int i = 0; i < numCoords; i++) {
      float xyz[3];
      cf->getCoordinate(i, xyz);
      
      handlesFlag[i] = false;
      
      int ijk[3];
      float pcoords[3];
      if (crossoversVolume->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords)) {
         if (crossoversVolume->getVoxel(ijk, 0) != 0.0) {
            handlesFlag[i] = true;
         }
      }      
   }
   
   //
   // Get a topology helper for the fiducial surface to access neighbors
   //
   const TopologyFile* tf = fiducialSurface->getTopologyFile();
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Highlight nodes that are neighbors of nodes marked has handles
   //
   bool* handlesFlag2 = new bool[numCoords];
   const int neighborDepth = 2;
   for (int k = 0; k < neighborDepth; k++) {
      for (int i = 0; i < numCoords; i++) {
         handlesFlag2[i] = handlesFlag[i];
      }

      for (int i = 0; i < numCoords; i++) {
         if (handlesFlag[i]) {
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(i, numNeighbors);
            for (int j = 0; j < numNeighbors; j++) {
               handlesFlag2[neighbors[j]] = true;
            }
         }
      }
      
      for (int i = 0; i < numCoords; i++) {
         handlesFlag[i] = handlesFlag2[i];
      }
   }
   
   for (int i = 0; i < numCoords; i++) {
      if (handlesFlag[i]) {
         rgbFile->setRgb(i, columnNumber, 255.0, 0.0, 255.0);
      }
      else {
         rgbFile->setRgb(i, columnNumber, 0.0, 0.0, 0.0);
      }
   }
   
   rgbFile->clearModified();
   
   delete[] handlesFlag;
   delete[] handlesFlag2;
}
