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


#include "BrainModelVolume.h"
#include "BrainModelVolumeHandleFinder.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "RgbPaintFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"

/**
 * Constructor.
 * If the "handlesVolumeIn" is NULL it will be created.  Otherwise, its
 * contents will be overwritten.
 */
BrainModelVolumeHandleFinder::BrainModelVolumeHandleFinder(BrainSet* bs,
                                                           VolumeFile* segmentationIn,
                                                           const bool addHandlesVolumeToBrainSetIn,
                                                           const bool searchAxisXIn,
                                                           const bool searchAxisYIn,
                                                           const bool searchAxisZIn,
                                                           const bool rgbPaintSurfaceHandlesIn)
   : BrainModelAlgorithm(bs)
{
   segmentationVolume = segmentationIn;
   handlesVolume      = NULL;
   addHandlesVolumeToBrainSet = addHandlesVolumeToBrainSetIn;
   
   voxels = NULL;
   visitedVoxels = NULL;
   
   searchAxisX = searchAxisXIn;
   searchAxisY = searchAxisYIn;
   searchAxisZ = searchAxisZIn;
   rgbPaintSurfaceHandles = rgbPaintSurfaceHandlesIn;
}
      
/**
 * Destructor
 */
BrainModelVolumeHandleFinder::~BrainModelVolumeHandleFinder()
{
}

/**
 * execute the algorithm
 */
void 
BrainModelVolumeHandleFinder::execute() throw (BrainModelAlgorithmException)
{
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
   // Dimensions of input volume
   //
   volumeDimX = dim[0];
   volumeDimY = dim[1];
   volumeDimZ = dim[2];
   
   //
   // Get the number of voxels
   //
   numVoxels = segmentationVolume->getTotalNumberOfVoxels();
   if (numVoxels <= 0) {
      throw BrainModelAlgorithmException("Segmentation volume is empty");
   }
   if (numVoxels != (volumeDimX * volumeDimY * volumeDimZ)) {
      throw BrainModelAlgorithmException("Number of voxels does not match dimensions.");
   }
   
   //
   // Copy input voxels since input and output volumes may be the same.
   // Create volumes for searching along the three axis.
   //
   unsigned char* inputVoxels = new unsigned char[numVoxels];
   unsigned char* voxelsX = new unsigned char[numVoxels];
   unsigned char* voxelsY = new unsigned char[numVoxels];
   unsigned char* voxelsZ = new unsigned char[numVoxels];
   for (int ii = 0; ii < numVoxels; ii++) {
      //
      // All non-zero voxels are assumed part of the segmentation
      //
      int voxel = VOXEL_UNSET;
      if (segmentationVolume->getVoxelWithFlatIndex(ii) != 0.0) {
         voxel = VOXEL_SEGMENTATION;
      }
      inputVoxels[ii] = voxel;
      voxelsX[ii] = voxel;
      voxelsY[ii] = voxel;
      voxelsZ[ii] = voxel;
   }
  
   //
   // Keeps track of voxels while searching
   //
   visitedVoxels = new int[numVoxels];

   //
   // Look for voxels along the Z axis
   //
   if (searchAxisZ) {
      voxels = voxelsZ;    
      initialVoxelAssignments(VolumeFile::VOLUME_AXIS_Z);   
      findHandles(VolumeFile::VOLUME_AXIS_Z);
   }
   
   //
   // Look for voxels along the Y axis
   //
   if (searchAxisY) {
      voxels = voxelsY;
      initialVoxelAssignments(VolumeFile::VOLUME_AXIS_Y);   
      findHandles(VolumeFile::VOLUME_AXIS_Y);
   }

   //
   // Look for voxels along the X axis
   //
   if (searchAxisX) {
      voxels = voxelsX;
      initialVoxelAssignments(VolumeFile::VOLUME_AXIS_X);
      findHandles(VolumeFile::VOLUME_AXIS_X);
   }

   //
   // Create the handles volume 
   //
   handlesVolume = new VolumeFile();
   handlesVolume->initialize(VolumeFile::VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED, dim, 
                             orientation, origin, spacing);
   //vtkDataArray* handles = handlesVolume->getVolumeData()->GetPointData()->GetScalars();
   
   //
   // copy the handles found along each axis into the handles volume
   //
   for (int k = 0; k < volumeDimZ; k++) {
      for (int i = 0; i < volumeDimX; i++) {
         for (int j = 0; j < volumeDimY; j++) {
            bool handleFlag = false;
            
            const int ijk[3] = { i, j, k };
            const int voxelID = segmentationVolume->getVoxelNumber(ijk);
            //handlesVolume->setVoxel(voxelID, inputVoxels[voxelID]);
            const int xvoxel = voxelsX[voxelID];
            if (xvoxel == VOXEL_HANDLE) {
               handleFlag = true;
            }
            const int yvoxel = voxelsY[voxelID];
            if (yvoxel == VOXEL_HANDLE) {
               handleFlag = true;
            }
            const int zvoxel = voxelsZ[voxelID];
            if (zvoxel == VOXEL_HANDLE) {
               handleFlag = true;
            }
            
            if (handleFlag) {
               handlesVolume->setVoxel(ijk, 0, 255.0);
               handlesVolume->setVoxel(ijk, 1, 0.0);
               handlesVolume->setVoxel(ijk, 2, 255.0);
            }
            else {
               handlesVolume->setVoxel(ijk, 0, 0.0);
               handlesVolume->setVoxel(ijk, 1, 0.0);
               handlesVolume->setVoxel(ijk, 2, 0.0);
            }
            //voxels[voxelID] = static_cast<unsigned char>(handles->GetComponent(voxelID, 0));
         }
      }
   }
   
   //
   // If surface should be painted around the handles with rgb paint
   //
   if (rgbPaintSurfaceHandles) {
      highlightHandlesInSurface();
   }
   
   //
   // Add handles volume to brain set if requested
   //
   if (addHandlesVolumeToBrainSet && 
       (brainSet != NULL) &&
       (getNumberOfHandles() > 0)) {
      const QString directory = FileUtilities::dirname(segmentationVolume->getFileName());
      const QString filename  = FileUtilities::basename(segmentationVolume->getFileName());
      if (filename.isEmpty() == false) {
         QString s(directory);
         if (s.isEmpty() == false) {
            s.append("/");
         }
         s.append("handles_");
         s.append(filename);
         handlesVolume->setFileName(s);
      }
      handlesVolume->clearModified();
      brainSet->addVolumeFile(VolumeFile::VOLUME_TYPE_RGB,
                              handlesVolume,
                              handlesVolume->getFileName(),
                              true,
                              false);
   }
   else {
      delete handlesVolume;
   }
   
   //
   // Free memory
   //
   delete[] inputVoxels;
   delete[] voxelsX;
   delete[] voxelsY;
   delete[] voxelsZ;
   delete[] visitedVoxels;
}

/**
 * Highlight nodes in the surface around the handles.
 */
void
BrainModelVolumeHandleFinder::highlightHandlesInSurface()
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
   handlesVolume->getDimensions(dim);
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
      if (handlesVolume->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords)) {
         if (handlesVolume->getVoxel(ijk, 0) != 0.0) {
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

/**
 * Clear the visited voxels.
 */
void
BrainModelVolumeHandleFinder::clearVisitedVoxels()
{
   for (int i = 0; i < numVoxels; i++) {
      visitedVoxels[i] = 0;
   }
}

/**
 * Look for any voxels with value VOXEL_UNSET that are six connected
 * to a voxel with value VOXEL_EXTERIOR thus forming a handle.
 */
void
BrainModelVolumeHandleFinder::findHandleSearch(const VolumeFile::VOLUME_AXIS searchAxis,
                                               const int ii, const int jj, const int kk)
{
   std::stack<VoxelIJK> st;
   st.push(VoxelIJK(ii, jj, kk));
   
   while (st.empty() == false) {
      const VoxelIJK v = st.top();
      st.pop();
      
      int i, j, k;
      v.getIJK(i, j, k);
      if ((i >= 0) && (i < volumeDimX) &&
          (j >= 0) && (j < volumeDimY) &&
          (k >= 0) && (k < volumeDimZ)) {
         const int ijk[3] = { i, j, k };
         const int voxelID = segmentationVolume->getVoxelNumber(ijk);
         const int voxel = voxels[voxelID];
         if (voxel == VOXEL_UNSET) {
            if (visitedVoxels[voxelID] == 0) {
               visitedVoxels[voxelID] = 1;
               
               switch (searchAxis) {
                  case VolumeFile::VOLUME_AXIS_X:
                     if (i < (volumeDimX - 1)) {
                        const int ijk2[3] = { i + 1, j, k };
                        const int voxelID2 = segmentationVolume->getVoxelNumber(ijk2);
                        if (voxels[voxelID2] == VOXEL_EXTERIOR) {
                           externalVoxelSlice.insert(i + 1);
                        }
                     }
                     if (i > 0) {
                        const int ijk2[3] = { i - 1, j, k };
                        const int voxelID2 = segmentationVolume->getVoxelNumber(ijk2);
                        if (voxels[voxelID2] == VOXEL_EXTERIOR) {
                           externalVoxelSlice.insert(i - 1);
                        }
                     }
                     break;
                  case VolumeFile::VOLUME_AXIS_Y:
                     if (j < (volumeDimY - 1)) {
                        const int ijk2[3] = { i, j + 1, k };
                        const int voxelID2 = segmentationVolume->getVoxelNumber(ijk2);
                        if (voxels[voxelID2] == VOXEL_EXTERIOR) {
                           externalVoxelSlice.insert(j + 1);
                        }
                     }
                     if (j > 0) {
                        const int ijk2[3] = { i, j - 1, k };
                        const int voxelID2 = segmentationVolume->getVoxelNumber(ijk2);
                        if (voxels[voxelID2] == VOXEL_EXTERIOR) {
                           externalVoxelSlice.insert(j - 1);
                        }
                     }
                     break;
                  case VolumeFile::VOLUME_AXIS_Z:
                     if (k < (volumeDimZ - 1)) {
                        const int ijk2[3] = { i, j, k + 1 };
                        const int voxelID2 = segmentationVolume->getVoxelNumber(ijk2);
                        if (voxels[voxelID2] == VOXEL_EXTERIOR) {
                           externalVoxelSlice.insert(k + 1);
                        }
                     }
                     if (k > 0) {
                        const int ijk2[3] = { i, j, k - 1 };
                        const int voxelID2 = segmentationVolume->getVoxelNumber(ijk2);
                        if (voxels[voxelID2] == VOXEL_EXTERIOR) {
                           externalVoxelSlice.insert(k - 1);
                        }
                     }
                     break;
                  case VolumeFile::VOLUME_AXIS_ALL:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                  case VolumeFile::VOLUME_AXIS_UNKNOWN:
                     break;
               }
               st.push(VoxelIJK(i - 1, j, k));
               st.push(VoxelIJK(i + 1, j, k));
               st.push(VoxelIJK(i, j - 1, k));
               st.push(VoxelIJK(i, j + 1, k));
               st.push(VoxelIJK(i, j, k - 1));
               st.push(VoxelIJK(i, j, k + 1));
            }
         }
      }
   }
}

/**
 * Find handles along the specified axis.
 */
void
BrainModelVolumeHandleFinder::findHandles(const VolumeFile::VOLUME_AXIS searchAxis)
{
   int loop1Start = 0;
   int loop1End   = 0;
   int loop2Start = 0;
   int loop2End   = 0;
   int loop3Start = 0;
   int loop3End   = 0;
   
   switch (searchAxis) {
      case VolumeFile::VOLUME_AXIS_X:
         loop1Start = 1;
         loop1End   = volumeDimX - 1;
         loop2Start = 0;
         loop2End   = volumeDimY;
         loop3Start = 0;
         loop3End   = volumeDimZ;
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         loop1Start = 1;
         loop1End   = volumeDimY - 1;
         loop2Start = 0;
         loop2End   = volumeDimX;
         loop3Start = 0;
         loop3End   = volumeDimZ;
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         loop1Start = 1;
         loop1End   = volumeDimZ - 1;
         loop2Start = 0;
         loop2End   = volumeDimX;
         loop3Start = 0;
         loop3End   = volumeDimY;
         break;
      case VolumeFile::VOLUME_AXIS_ALL:
      case VolumeFile::VOLUME_AXIS_OBLIQUE:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         break;
   }
   
   for (int loop1 = loop1Start; loop1 < loop1End; loop1++) {
      for (int loop2 = loop2Start; loop2 < loop2End; loop2++) {
         for (int loop3 = loop3Start; loop3 < loop3End; loop3++) { 
            int x = 0;
            int y = 0;
            int z = 0;           
            switch (searchAxis) {
               case VolumeFile::VOLUME_AXIS_X:
                  x = loop1;
                  y = loop2;
                  z = loop3;
                  break;
               case VolumeFile::VOLUME_AXIS_Y:
                  x = loop2;
                  y = loop1;
                  z = loop3;
                  break;
               case VolumeFile::VOLUME_AXIS_Z:
                  x = loop2;
                  y = loop3;
                  z = loop1;
                  break;
               case VolumeFile::VOLUME_AXIS_ALL:
               case VolumeFile::VOLUME_AXIS_OBLIQUE:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               case VolumeFile::VOLUME_AXIS_UNKNOWN:
                  break;
            }
            int ijk[3] = { x, y, z };
            
            //
            // Get the value at the voxel indices
            //
            int voxelID = segmentationVolume->getVoxelNumber((int*)ijk);
            const int voxel = voxels[voxelID];
            
            //
            // Is this a potential handle voxel ?
            //
            if (voxel == VOXEL_UNSET) {
	            clearVisitedVoxels();
               
               //
               // Search VOXEL_UNSET voxels finding any that are adjacent in the 
               // search axis to VOXEL_EXTERIOR voxels.
               //
               findHandleSearch(searchAxis, x, y, z);
               int status = VOXEL_NOT_HANDLE;
               
               //
               // If two or VOXEL_EXTERIOR voxels with different slices along
               // the search axis were found, then these voxels are part of a handle
               //
               if (externalVoxelSlice.size() > 1) {
                  status = VOXEL_HANDLE;
               }
               //
               // Voxels are full enclosed by VOXEL_SEGMENTATION voxels
               //
               else if (externalVoxelSlice.size() == 0) {
                  status = VOXEL_CAVITY;
               }
               
               //
               // Make the voxels as not a handle, in a handle, or in a cavity
               //
               int numVoxelsInHandle = 0;
               for (int m = 0; m < numVoxels; m++) {
                  if (visitedVoxels[m]) {
                    voxels[m] = status;
                    numVoxelsInHandle++;
                  }
               }
               
               //
               // Are the voxels in a handle
               //
               if (externalVoxelSlice.size() > 1) {
                  //
                  // Get the range of the slices along the search axis
                  //
                  int firstSlice = -1;
                  int lastSlice  = -1;
                  for (std::set<int>::iterator is = externalVoxelSlice.begin();
                       is != externalVoxelSlice.end(); is++) {
                     if (is == externalVoxelSlice.begin()) {
                        firstSlice = *is + 1;
                     }
                     lastSlice = *is - 1;
                  }
                  
                  if (DebugControl::getDebugOn()) {
                     std::cout << "Handle at voxel (" << x << ", " << y << ", " 
                        << z << ") slices (" << firstSlice << ", " << lastSlice
                        << ") involves " << numVoxelsInHandle << " voxels"
                        << endl;
                  }
      
                  //
                  // Save the handle information
                  //
                  const int slices[2] = { firstSlice, lastSlice };
                  std::vector<int> handleVoxels;
                  for (int m = 0; m < numVoxels; m++) {
                     if (visitedVoxels[m]) {
                        handleVoxels.push_back(m);
                     }
                  }
                  float voxelXYZ[3];
                  segmentationVolume->getVoxelCoordinate(ijk, true, voxelXYZ);
                  handlesFound.push_back(BrainModelVolumeTopologicalError(ijk,
                                                voxelXYZ,
                                                slices,
                                                handleVoxels,
                                                numVoxelsInHandle,
                                                searchAxis));
               }
	            externalVoxelSlice.clear();
	         }
	      }
      }
   }
}

/**
 * Flood fill in 2D all 4-connected neighbors orthogonal specified axis.
 */
void
BrainModelVolumeHandleFinder::floodFill4Connected(const VolumeFile::VOLUME_AXIS searchAxis,
                                                  const int i1, const int j1, const int z1,
                                                  const int replace, const int replaceWith)
{
   std::stack<VoxelIJK> st;
   st.push(VoxelIJK(i1, j1, z1));
   
   while (st.empty() == false) {
      const VoxelIJK v = st.top();
      st.pop();
      int i, j, k;
      v.getIJK(i, j, k);
      
      if ((i >= 0) && (i < volumeDimX) &&
          (j >= 0) && (j < volumeDimY) &&
          (k >= 0) && (k < volumeDimZ)) {
         const int ijk[3] = { i, j, k };
         const int voxelID = segmentationVolume->getVoxelNumber(ijk);
         const int voxel = voxels[voxelID];
         if (voxel == replace) {
            int iDelta = 0;
            int jDelta = 0;
            int kDelta = 0;
            switch (searchAxis) {
               case VolumeFile::VOLUME_AXIS_X:
                  iDelta = 0;
                  jDelta = 1;
                  kDelta = 1;
                  break;
               case VolumeFile::VOLUME_AXIS_Y:
                  iDelta = 1;
                  jDelta = 0;
                  kDelta = 1;
                  break;
               case VolumeFile::VOLUME_AXIS_Z:
                  iDelta = 1;
                  jDelta = 1;
                  kDelta = 0;
                  break;
               case VolumeFile::VOLUME_AXIS_ALL:
               case VolumeFile::VOLUME_AXIS_OBLIQUE:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               case VolumeFile::VOLUME_AXIS_UNKNOWN:
                  break;
            }
            
            voxels[voxelID] = replaceWith;
            
            if (iDelta != 0) {
               st.push(VoxelIJK(i - iDelta, j, k));
               st.push(VoxelIJK(i + iDelta, j, k));
            }
            if (jDelta != 0) {
               st.push(VoxelIJK(i, j - jDelta, k));
               st.push(VoxelIJK(i, j + jDelta, k));
            }
            if (kDelta != 0) {
               st.push(VoxelIJK(i, j, k - kDelta));
               st.push(VoxelIJK(i, j, k + kDelta));
            }
         }
      }
   }
}

/**
 * Move around edges of volume and set all "4 connected" neighbors as 
 * VOXEL_EXTERIOR.  Any voxels remaining with the value VOXEL_UNSET are
 * potential handles.
 */
void
BrainModelVolumeHandleFinder::initialVoxelAssignments(const VolumeFile::VOLUME_AXIS searchAxis)
{
   switch (searchAxis) {
      case VolumeFile::VOLUME_AXIS_X:
         for (int x = 0; x < volumeDimX; x++) {
            for (int y = 0; y < volumeDimY; y++) {
               floodFill4Connected(VolumeFile::VOLUME_AXIS_X, x, y, 0, VOXEL_UNSET, VOXEL_EXTERIOR);
               floodFill4Connected(VolumeFile::VOLUME_AXIS_X, x, y, volumeDimZ - 1, VOXEL_UNSET, VOXEL_EXTERIOR);
            }
            for (int z = 0; z < volumeDimZ; z++) {
               floodFill4Connected(VolumeFile::VOLUME_AXIS_X, x, 0, z, VOXEL_UNSET, VOXEL_EXTERIOR);
               floodFill4Connected(VolumeFile::VOLUME_AXIS_X, x, volumeDimY - 1, z, VOXEL_UNSET, VOXEL_EXTERIOR);
            }
         }
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         for (int y = 0; y < volumeDimY; y++) {
            for (int x = 0; x < volumeDimX; x++) {
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Y, x, y, 0, VOXEL_UNSET, VOXEL_EXTERIOR);
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Y, x, y, volumeDimZ - 1, VOXEL_UNSET, VOXEL_EXTERIOR);
            }
            for (int z = 0; z < volumeDimZ; z++) {
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Y, 0, y, z, VOXEL_UNSET, VOXEL_EXTERIOR);
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Y, volumeDimX - 1, y, z, VOXEL_UNSET, VOXEL_EXTERIOR);
            }
         }
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         for (int z = 0; z < volumeDimZ; z++) {
            for (int x = 0; x < volumeDimX; x++) {
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Z, x, 0, z, VOXEL_UNSET, VOXEL_EXTERIOR);
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Z, x, volumeDimY - 1, z, VOXEL_UNSET, VOXEL_EXTERIOR);
            }
            for (int y = 0; y < volumeDimY; y++) {
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Z, 0, y, z, VOXEL_UNSET, VOXEL_EXTERIOR);
               floodFill4Connected(VolumeFile::VOLUME_AXIS_Z, volumeDimX - 1, y, z, VOXEL_UNSET, VOXEL_EXTERIOR);
            }
         }
         break;
      case VolumeFile::VOLUME_AXIS_ALL:
      case VolumeFile::VOLUME_AXIS_OBLIQUE:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         break;
   }
}

