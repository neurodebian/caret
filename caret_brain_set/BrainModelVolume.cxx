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

#include <cmath>
#include <iostream>
#include <sstream>

#include "vtkTransform.h"

#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsVolume.h"
#include "FileUtilities.h"

/**
 * Constructor
 */
BrainModelVolume::BrainModelVolume(BrainSet* bs) 
   : BrainModel(bs, BrainModel::BRAIN_MODEL_VOLUME)
{
   obliqueRotationMatrix = vtkTransform::New();
   reset();
}

/**
 * Destructor
 */
BrainModelVolume::~BrainModelVolume()
{
   reset();
   obliqueRotationMatrix->Delete();
}

/**
 * Get a descriptive name of the model.
 */
QString
BrainModelVolume::getDescriptiveName() const
{
   QString name("VOLUME"); 

   const VolumeFile* vf = getMasterVolumeFile();
   if (vf != NULL) {
      name.append(" - ");
      name.append(vf->getDescriptiveLabel());
   }

   return name;
}

/**
 * Reset the volume.
 */
void
BrainModelVolume::reset()
{
   for (int i = 0; i < NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      selectedOrthogonalSlices[i][0] = 0;
      selectedOrthogonalSlices[i][1] = 0;
      selectedOrthogonalSlices[i][2] = 0;
      selectedObliqueSliceOffsets[i][0] = 0;
      selectedObliqueSliceOffsets[i][1] = 0;
      selectedObliqueSliceOffsets[i][2] = 0;
      selectedAxis[i] = VolumeFile::VOLUME_AXIS_Z;
      displayRotation[i] = 0.0;  
      viewStereotaxicCoordinatesFlag[i] = false;
      showUnderlayOnly[i] = false;
   }
   selectedObliqueSlices[0] = 0;
   selectedObliqueSlices[1] = 0;
   selectedObliqueSlices[2] = 0;
   obliqueRotationMatrix->Identity();
   
   updateProbAtlasNameTableIndices();
}

/**
 * view stereotaxic coordinates flag.
 */
bool 
BrainModelVolume::getViewStereotaxicCoordinatesFlag(const int viewNumber) const 
{ 
   return viewStereotaxicCoordinatesFlag[viewNumber]; 
}
 
/**
 * view stereotaxic coordinates flag.
 */
void 
BrainModelVolume::setViewStereotaxicCoordinatesFlag(const int viewNumber, const bool b) 
{ 
   viewStereotaxicCoordinatesFlag[viewNumber] = b; 
}
      
/**
 * Initialize the selected slices for all views.
 */
void
BrainModelVolume::initializeSelectedSlicesAllViews(const bool initializeAxis)
{
   for (int i = 0; i < NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      initializeSelectedSlices(i, initializeAxis);
   }
}

/**
 * Initialize the selected slices.  If the origin is non-zero, the selected slices
 * are set to the voxel containing the coordinate (0, 0, 0).  Otherwise, the selected
 * slices are set to the middle slices (dims / 2).
 */
void
BrainModelVolume::initializeSelectedSlices(const int viewNumber, const bool initializeAxis)
{
   int defaultIJK[3] = { 0, 0, 0 };
   VolumeFile* vf = getMasterVolumeFile();
   if (vf != NULL) {
      //
      // AC should be at stereotaxic coordinate (0, 0, 0)
      //
      float origin[3];
      vf->getOrigin(origin);
      float xyz[3] = { 0.0, 0.0, 0.0 };
      int ijk[3];
      float p[3];
      const bool insideVolumeFlag = vf->convertCoordinatesToVoxelIJK(xyz, ijk, p);

      if (insideVolumeFlag) {
         //
         // Initialize slices to the coordinate (0, 0, 0) which should be the AC
         //
         defaultIJK[0] = ijk[0];
         defaultIJK[1] = ijk[1];
         defaultIJK[2] = ijk[2];
      }
      else {
         //
         // Origin not valid or (0, 0, 0) coordinate not in the volume so
         // initialize to "middle" slices
         //
         int voxelDimensions[3];
         vf->getDimensions(voxelDimensions);
         defaultIJK[0] = voxelDimensions[0] / 2;
         defaultIJK[1] = voxelDimensions[1] / 2;
         defaultIJK[2] = voxelDimensions[2] / 2;
      }
   }

   if (initializeAxis) {
      selectedAxis[viewNumber] = VolumeFile::VOLUME_AXIS_Z;
      selectedOrthogonalSlices[viewNumber][0] = defaultIJK[0];
      selectedOrthogonalSlices[viewNumber][1] = defaultIJK[1];
      selectedOrthogonalSlices[viewNumber][2] = defaultIJK[2];
      selectedObliqueSlices[0] = defaultIJK[0];
      selectedObliqueSlices[1] = defaultIJK[1];
      selectedObliqueSlices[2] = defaultIJK[2];
      selectedObliqueSliceOffsets[viewNumber][0] = 0;
      selectedObliqueSliceOffsets[viewNumber][1] = 0;
      selectedObliqueSliceOffsets[viewNumber][2] = 0;
   }
   else {
      switch (selectedAxis[viewNumber]) {
         case VolumeFile::VOLUME_AXIS_X:
         case VolumeFile::VOLUME_AXIS_Y:
         case VolumeFile::VOLUME_AXIS_Z:
         case VolumeFile::VOLUME_AXIS_ALL:
            selectedOrthogonalSlices[viewNumber][0] = defaultIJK[0];
            selectedOrthogonalSlices[viewNumber][1] = defaultIJK[1];
            selectedOrthogonalSlices[viewNumber][2] = defaultIJK[2];
            break;
         case VolumeFile::VOLUME_AXIS_OBLIQUE:
            selectedObliqueSlices[0] = defaultIJK[0];
            selectedObliqueSlices[1] = defaultIJK[1];
            selectedObliqueSlices[2] = defaultIJK[2];
            break;
         case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
         case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
            selectedObliqueSliceOffsets[viewNumber][0] = 0;
            selectedObliqueSliceOffsets[viewNumber][1] = 0;
            selectedObliqueSliceOffsets[viewNumber][2] = 0;
            break;
         case VolumeFile::VOLUME_AXIS_UNKNOWN:
            break;
      }
   }   
}

/**
 * Get the selected slices.
 */
void
BrainModelVolume::getSelectedOrthogonalSlices(const int volumeViewNumber, int slices[3]) 
{
   slices[0] = selectedOrthogonalSlices[volumeViewNumber][0];
   slices[1] = selectedOrthogonalSlices[volumeViewNumber][1];
   slices[2] = selectedOrthogonalSlices[volumeViewNumber][2];

   VolumeFile* vf = getMasterVolumeFile();
   if (vf != NULL) {
      int dim[3];
      vf->getDimensions(dim);
      if ((slices[0] >= dim[0]) ||
          (slices[1] >= dim[1]) ||
          (slices[2] >= dim[2])) {
         initializeSelectedSlicesAllViews(false);
         slices[0] = selectedOrthogonalSlices[volumeViewNumber][0];
         slices[1] = selectedOrthogonalSlices[volumeViewNumber][1];
         slices[2] = selectedOrthogonalSlices[volumeViewNumber][2];
      }
   }
}

/**
 * get the selected slices for oblique view.
 */
void 
BrainModelVolume::getSelectedObliqueSlices(int slices[3])
{
   slices[0] = selectedObliqueSlices[0];
   slices[1] = selectedObliqueSlices[1];
   slices[2] = selectedObliqueSlices[2];
}

/**
 * set the selected slices for oblique view.
 */
void 
BrainModelVolume::setSelectedObliqueSlices(const int slices[3])
{
   selectedObliqueSlices[0] = slices[0];
   selectedObliqueSlices[1] = slices[1];
   selectedObliqueSlices[2] = slices[2];
}

/**
 * get the selected oblique slice offsets.
 */
void 
BrainModelVolume::getSelectedObliqueSliceOffsets(const int volumeViewNumber, int slices[3])
{
   slices[0] = selectedObliqueSliceOffsets[volumeViewNumber][0];
   slices[1] = selectedObliqueSliceOffsets[volumeViewNumber][1];
   slices[2] = selectedObliqueSliceOffsets[volumeViewNumber][2];
}

/**
 * set the selected oblique slice offsets.
 */
void 
BrainModelVolume::setSelectedObliqueSliceOffsets(const int volumeViewNumber, const int slices[3])
{
   selectedObliqueSliceOffsets[volumeViewNumber][0] = slices[0];
   selectedObliqueSliceOffsets[volumeViewNumber][1] = slices[1];
   selectedObliqueSliceOffsets[volumeViewNumber][2] = slices[2];
}

/**
 * Set the selected slices.
 */
void
BrainModelVolume::setSelectedOrthogonalSlices(const int volumeViewNumber, const int slices[3])
{
   selectedOrthogonalSlices[volumeViewNumber][0] = slices[0];
   selectedOrthogonalSlices[volumeViewNumber][1] = slices[1];
   selectedOrthogonalSlices[volumeViewNumber][2] = slices[2];
}

/**
 * reset the viewing transform.
 */
void 
BrainModelVolume::resetViewingTransform(const int viewNumber)
{
   float m[16];
   m[0]  =  1.0;
   m[1]  =  0.0;
   m[2]  =  0.0;
   m[3]  =  0.0;
   m[4]  =  0.0;
   m[5]  =  1.0;
   m[6]  =  0.0;
   m[7]  =  0.0;
   m[8]  =  0.0;
   m[9]  =  0.0;
   m[10] =  1.0;
   m[11] =  0.0;
   m[12] =  0.0;
   m[13] =  0.0;
   m[14] =  0.0;
   m[15] =  1.0;
   scaling[viewNumber][0] = 1.0;
   scaling[viewNumber][1] = 1.0;
   scaling[viewNumber][2] = 1.0;
   translation[viewNumber][0] = 0.0;
   translation[viewNumber][1] = 0.0;
   translation[viewNumber][2] = 0.0;
   setRotationMatrix(viewNumber, m);
   //initializeSelectedSlicesAllViews(false);
   displayRotation[viewNumber] = 0.0;
   if (getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
      obliqueRotationMatrix->Identity();
   }
}

/**
 * Set the volume to a standard view.
 * This should only be called for volumes when in viewing the "oblique" axis.
 */
void 
BrainModelVolume::setToStandardView(const int viewNumber, const STANDARD_VIEWS view)
{
   float m[16];
   bool valid = true;
   
   const Structure structure = brainSet->getStructure();
   
   switch(view) {
      case VIEW_LATERAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  0.0;
           m[1]  =  0.0;
           m[2]  = -1.0;
           m[3]  =  0.0;
           m[4]  = -1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  0.0;
           m[1]  =  0.0;
           m[2]  =  1.0;
           m[3]  =  0.0;
           m[4]  =  1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_MEDIAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  0.0;       
           m[1]  =  0.0;
           m[2]  =  1.0;
           m[3]  =  0.0;  
           m[4]  =  1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  0.0;
           m[1]  =  0.0;
           m[2]  = -1.0;
           m[3]  =  0.0;
           m[4]  = -1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_POSTERIOR:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  = -1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  = -1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_ANTERIOR:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  =  1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  =  1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_VENTRAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] = -1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] = -1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_DORSAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] =  1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] =  1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_RESET:
         m[0]  =  1.0;
         m[1]  =  0.0;
         m[2]  =  0.0;
         m[3]  =  0.0;
         m[4]  =  0.0;
         m[5]  =  1.0;
         m[6]  =  0.0;
         m[7]  =  0.0;
         m[8]  =  0.0;
         m[9]  =  0.0;
         m[10] =  1.0;
         m[11] =  0.0;
         m[12] =  0.0;
         m[13] =  0.0;
         m[14] =  0.0;
         m[15] =  1.0;
         perspectiveZooming[viewNumber] = 100.0; //defaultPerspectiveZooming;
         scaling[viewNumber][0] = 1.0;
         scaling[viewNumber][1] = 1.0;
         scaling[viewNumber][2] = 1.0;
         translation[viewNumber][0] = 0.0;
         translation[viewNumber][1] = 0.0;
         translation[viewNumber][2] = 0.0;
         setRotationMatrix(viewNumber, m);
         initializeSelectedSlicesAllViews(false);
         displayRotation[viewNumber] = 0.0;
         if(getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
            obliqueRotationMatrix->Identity();
         }
         break;
      case VIEW_ROTATE_X_90:
         if(getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
            obliqueRotationMatrix->RotateX(90.0);
         }
         valid = false;
         break;
      case VIEW_ROTATE_Y_90:
         if(getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
            obliqueRotationMatrix->RotateY(90.0);
         }
         valid = false;
         break;
      case VIEW_ROTATE_Z_90:
         if(getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
            obliqueRotationMatrix->RotateZ(-90.0);
         }
         valid = false;
         break;
      case VIEW_NONE:
      default:
         valid = false;
         break;
   }
   
   if (valid) {
      if(getSelectedAxis(viewNumber) == VolumeFile::VOLUME_AXIS_OBLIQUE) {
         setObliqueRotationMatrix(m);
      }
   }
}

/**
 * get the volume is an underlay or an overlay.
 */
bool 
BrainModelVolume::getVolumeIsAnUnderlayOrAnOverlay(const VolumeFile* vf) const
{
   if (vf != NULL) {
      if ((vf == getUnderlayVolumeFile()) ||
          (vf == getOverlaySecondaryVolumeFile()) ||
          (vf == getOverlayPrimaryVolumeFile())) {
         return true;
      }
   }
   return false;
}
      
/**
 * get the "bottom-most" selected volume file for volume display sizing and control.
 */
VolumeFile* 
BrainModelVolume::getMasterVolumeFile()
{
   VolumeFile *vf = getUnderlayVolumeFile();
   if (vf != NULL) {
      return vf;
   }
   
   vf = getOverlaySecondaryVolumeFile();
   if (vf != NULL) {
      return vf;
   }
   
   vf = getOverlayPrimaryVolumeFile();
   return vf;   
}

/**
 * get the "bottom-most" selected volume file for volume display sizing and control (const).
 */
const VolumeFile* 
BrainModelVolume::getMasterVolumeFile() const
{
   if (getUnderlayVolumeFile() != NULL) {
      return getUnderlayVolumeFile();
   }
   
   if (getOverlaySecondaryVolumeFile() != NULL) {
      return getOverlaySecondaryVolumeFile();
   }
   
   return getOverlayPrimaryVolumeFile();
}

/**
 * get the underlay volume file.
 */
VolumeFile* 
BrainModelVolume::getUnderlayVolumeFile()
{
   BrainModelVolumeVoxelColoring* vvc = brainSet->getVoxelColoring();
   switch (vvc->getUnderlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         return getSelectedVolumeAnatomyFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         return getSelectedVolumeFunctionalViewFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT:
         return getSelectedVolumePaintFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS:
         if (brainSet->getNumberOfVolumeProbAtlasFiles() > 0) {
            return brainSet->getVolumeProbAtlasFile(0);
         }
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         return getSelectedVolumeRgbFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         return getSelectedVolumeSegmentationFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         return getSelectedVolumeVectorFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:
         break;
   }
   return NULL;
}

/**
 * get the underlay volume file (const method).
 */
const VolumeFile* 
BrainModelVolume::getUnderlayVolumeFile() const
{
   BrainModelVolumeVoxelColoring* vvc = brainSet->getVoxelColoring();
   switch (vvc->getUnderlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         return getSelectedVolumeAnatomyFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         return getSelectedVolumeFunctionalViewFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT:
         return getSelectedVolumePaintFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS:
         if (brainSet->getNumberOfVolumeProbAtlasFiles() > 0) {
            return brainSet->getVolumeProbAtlasFile(0);
         }
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         return getSelectedVolumeRgbFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         return getSelectedVolumeSegmentationFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         return getSelectedVolumeVectorFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:
         break;
   }
   return NULL;
}

/**
 * get the secondary overlay volume file.
 */
VolumeFile* 
BrainModelVolume::getOverlaySecondaryVolumeFile()
{
   BrainModelVolumeVoxelColoring* vvc = brainSet->getVoxelColoring();
   switch (vvc->getSecondaryOverlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         return getSelectedVolumeAnatomyFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         return getSelectedVolumeFunctionalViewFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT:
         return getSelectedVolumePaintFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS:
         if (brainSet->getNumberOfVolumeProbAtlasFiles() > 0) {
            return brainSet->getVolumeProbAtlasFile(0);
         }
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         return getSelectedVolumeRgbFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         return getSelectedVolumeSegmentationFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         return getSelectedVolumeVectorFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:
         break;
   }
   return NULL;
}

/**
 * get the secondary overlay volume file (const method).
 */
const VolumeFile* 
BrainModelVolume::getOverlaySecondaryVolumeFile() const
{
   BrainModelVolumeVoxelColoring* vvc = brainSet->getVoxelColoring();
   switch (vvc->getSecondaryOverlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         return getSelectedVolumeAnatomyFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         return getSelectedVolumeFunctionalViewFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT:
         return getSelectedVolumePaintFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS:
         if (brainSet->getNumberOfVolumeProbAtlasFiles() > 0) {
            return brainSet->getVolumeProbAtlasFile(0);
         }
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         return getSelectedVolumeRgbFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         return getSelectedVolumeSegmentationFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         return getSelectedVolumeVectorFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:
         break;
   }
   return NULL;
}

/**
 * get the primary overlay volume file.
 */
VolumeFile* 
BrainModelVolume::getOverlayPrimaryVolumeFile()
{
   BrainModelVolumeVoxelColoring* vvc = brainSet->getVoxelColoring();
   switch (vvc->getPrimaryOverlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         return getSelectedVolumeAnatomyFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         return getSelectedVolumeFunctionalViewFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT:
         return getSelectedVolumePaintFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS:
         if (brainSet->getNumberOfVolumeProbAtlasFiles() > 0) {
            return brainSet->getVolumeProbAtlasFile(0);
         }
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         return getSelectedVolumeRgbFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         return getSelectedVolumeSegmentationFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         return getSelectedVolumeVectorFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:
         break;
   }
   return NULL;
}

/**
 * get the primary overlay volume file (const method).
 */
const VolumeFile* 
BrainModelVolume::getOverlayPrimaryVolumeFile() const
{
   BrainModelVolumeVoxelColoring* vvc = brainSet->getVoxelColoring();
   switch (vvc->getPrimaryOverlay()) {
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY:
         return getSelectedVolumeAnatomyFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL:
         return getSelectedVolumeFunctionalViewFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT:
         return getSelectedVolumePaintFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS:
         if (brainSet->getNumberOfVolumeProbAtlasFiles() > 0) {
            return brainSet->getVolumeProbAtlasFile(0);
         }
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB:
         return getSelectedVolumeRgbFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION:
         return getSelectedVolumeSegmentationFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR:
         return getSelectedVolumeVectorFile();
         break;
      case BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE:
         break;
   }
   return NULL;
}

/**
 * Get the selected anatomy volume file.
 */
VolumeFile*
BrainModelVolume::getSelectedVolumeAnatomyFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* anatomyVolumeFile = NULL;
   if ((dsv->getSelectedAnatomyVolume() >= 0) &&
       (dsv->getSelectedAnatomyVolume() < brainSet->getNumberOfVolumeAnatomyFiles())) {
      anatomyVolumeFile = brainSet->getVolumeAnatomyFile(dsv->getSelectedAnatomyVolume());
   }
   return anatomyVolumeFile;
}

/**
 * Get the selected anatomy volume file (const method).
 */
const VolumeFile*
BrainModelVolume::getSelectedVolumeAnatomyFile() const
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* anatomyVolumeFile = NULL;
   if ((dsv->getSelectedAnatomyVolume() >= 0) &&
       (dsv->getSelectedAnatomyVolume() < brainSet->getNumberOfVolumeAnatomyFiles())) {
      anatomyVolumeFile = brainSet->getVolumeAnatomyFile(dsv->getSelectedAnatomyVolume());
   }
   return anatomyVolumeFile;
}

/**
 * Get the selected functional volume file.
 */
VolumeFile*
BrainModelVolume::getSelectedVolumeFunctionalViewFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* functionalVolumeFile = NULL;
   if ((dsv->getSelectedFunctionalVolumeView() >= 0) &&
       (dsv->getSelectedFunctionalVolumeView() < brainSet->getNumberOfVolumeFunctionalFiles())) {
      functionalVolumeFile = brainSet->getVolumeFunctionalFile(dsv->getSelectedFunctionalVolumeView());
   }
   return functionalVolumeFile;
}

/**
 * Get the selected functional volume file (const method).
 */
const VolumeFile*
BrainModelVolume::getSelectedVolumeFunctionalViewFile() const
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* functionalVolumeFile = NULL;
   if ((dsv->getSelectedFunctionalVolumeView() >= 0) &&
       (dsv->getSelectedFunctionalVolumeView() < brainSet->getNumberOfVolumeFunctionalFiles())) {
      functionalVolumeFile = brainSet->getVolumeFunctionalFile(dsv->getSelectedFunctionalVolumeView());
   }
   return functionalVolumeFile;
}

/**
 * Get the selected functional volume file (const method).
 */
const VolumeFile*
BrainModelVolume::getSelectedVolumeFunctionalThresholdFile() const
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* functionalVolumeFile = NULL;
   if ((dsv->getSelectedFunctionalVolumeThreshold() >= 0) &&
       (dsv->getSelectedFunctionalVolumeThreshold() < brainSet->getNumberOfVolumeFunctionalFiles())) {
      functionalVolumeFile = brainSet->getVolumeFunctionalFile(dsv->getSelectedFunctionalVolumeThreshold());
   }
   return functionalVolumeFile;
}

/**
 * Get the selected functional volume file.
 */
VolumeFile*
BrainModelVolume::getSelectedVolumeFunctionalThresholdFile() 
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* functionalVolumeFile = NULL;
   if ((dsv->getSelectedFunctionalVolumeThreshold() >= 0) &&
       (dsv->getSelectedFunctionalVolumeThreshold() < brainSet->getNumberOfVolumeFunctionalFiles())) {
      functionalVolumeFile = brainSet->getVolumeFunctionalFile(dsv->getSelectedFunctionalVolumeThreshold());
   }
   return functionalVolumeFile;
}

/**
 * Get the selected paint volume file.
 */
VolumeFile*
BrainModelVolume::getSelectedVolumePaintFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* paintVolumeFile = NULL;
   if ((dsv->getSelectedPaintVolume() >= 0) &&
       (dsv->getSelectedPaintVolume() < brainSet->getNumberOfVolumePaintFiles())) {
      paintVolumeFile = brainSet->getVolumePaintFile(dsv->getSelectedPaintVolume());
   }
   return paintVolumeFile;
}

/**
 * Get the selected paint volume file (const method).
 */
const VolumeFile*
BrainModelVolume::getSelectedVolumePaintFile() const
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* paintVolumeFile = NULL;
   if ((dsv->getSelectedPaintVolume() >= 0) &&
       (dsv->getSelectedPaintVolume() < brainSet->getNumberOfVolumePaintFiles())) {
      paintVolumeFile = brainSet->getVolumePaintFile(dsv->getSelectedPaintVolume());
   }
   return paintVolumeFile;
}

/**
 * Get the selected RGB volume file.
 */
VolumeFile*
BrainModelVolume::getSelectedVolumeRgbFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* rgbVolumeFile = NULL;
   if ((dsv->getSelectedRgbVolume() >= 0) &&
       (dsv->getSelectedRgbVolume() < brainSet->getNumberOfVolumeRgbFiles())) {
      rgbVolumeFile = brainSet->getVolumeRgbFile(dsv->getSelectedRgbVolume());
   }
   return rgbVolumeFile;
}

/**
 * Get the selected RGB volume file (const method).
 */
const VolumeFile*
BrainModelVolume::getSelectedVolumeRgbFile() const
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* rgbVolumeFile = NULL;
   if ((dsv->getSelectedRgbVolume() >= 0) &&
       (dsv->getSelectedRgbVolume() < brainSet->getNumberOfVolumeRgbFiles())) {
      rgbVolumeFile = brainSet->getVolumeRgbFile(dsv->getSelectedRgbVolume());
   }
   return rgbVolumeFile;
}

/**
 * Get the selected segmenation volume file.
 */
VolumeFile*
BrainModelVolume::getSelectedVolumeSegmentationFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* segmentationVolumeFile = NULL;
   if ((dsv->getSelectedSegmentationVolume() >= 0) &&
       (dsv->getSelectedSegmentationVolume() < brainSet->getNumberOfVolumeSegmentationFiles())) {
      segmentationVolumeFile = brainSet->getVolumeSegmentationFile(dsv->getSelectedSegmentationVolume());
   }
   return segmentationVolumeFile;
}

/**
 * Get the selected segmenation volume file (const method).
 */
const VolumeFile*
BrainModelVolume::getSelectedVolumeSegmentationFile() const
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* segmentationVolumeFile = NULL;
   if ((dsv->getSelectedSegmentationVolume() >= 0) &&
       (dsv->getSelectedSegmentationVolume() < brainSet->getNumberOfVolumeSegmentationFiles())) {
      segmentationVolumeFile = brainSet->getVolumeSegmentationFile(dsv->getSelectedSegmentationVolume());
   }
   return segmentationVolumeFile;
}

/**
 * Get the selected vector volume file.
 */
VolumeFile*
BrainModelVolume::getSelectedVolumeVectorFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* vectorVolumeFile = NULL;
   if ((dsv->getSelectedVectorVolume() >= 0) &&
       (dsv->getSelectedVectorVolume() < brainSet->getNumberOfVolumeVectorFiles())) {
      vectorVolumeFile = brainSet->getVolumeVectorFile(dsv->getSelectedVectorVolume());
   }
   return vectorVolumeFile;
}

/**
 * Get the selected vector volume file (const method).
 */
const VolumeFile*
BrainModelVolume::getSelectedVolumeVectorFile() const
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* vectorVolumeFile = NULL;
   if ((dsv->getSelectedVectorVolume() >= 0) &&
       (dsv->getSelectedVectorVolume() < brainSet->getNumberOfVolumeVectorFiles())) {
      vectorVolumeFile = brainSet->getVolumeVectorFile(dsv->getSelectedVectorVolume());
   }
   return vectorVolumeFile;
}

/**
 * get the display rotation.
 */
float 
BrainModelVolume::getDisplayRotation(const int volumeViewNumber) const 
{ 
   return displayRotation[volumeViewNumber]; 
}

/**
 * set the display rotation.
 */
void 
BrainModelVolume::setDisplayRotation(const int volumeViewNumber,
                                     const float dr) 
{ 
   displayRotation[volumeViewNumber] = dr; 
}
      
/**
 * add to the display rotation.
 */
void 
BrainModelVolume::addToDisplayRotation(const int volumeViewNumber,
                                       const float delta)
{
   displayRotation[volumeViewNumber] += delta;
   if ((displayRotation[volumeViewNumber] >= 360.0) || 
       (displayRotation[volumeViewNumber] <= -360.0)) {
      displayRotation[volumeViewNumber] = fmod(displayRotation[volumeViewNumber],360.0f);
   }
   if (displayRotation[volumeViewNumber] > 180.0) {
      displayRotation[volumeViewNumber] = -360.0 + displayRotation[volumeViewNumber];
   }
   if (displayRotation[volumeViewNumber] < -180.0) {
      displayRotation[volumeViewNumber] = 360.0 + displayRotation[volumeViewNumber];
   }
}      

/**
 * set the selected axis.
 */
void 
BrainModelVolume::setSelectedAxis(const int volumeViewNumber, const VolumeFile::VOLUME_AXIS axis) 
{ 
   selectedAxis[volumeViewNumber] = axis; 
   displayRotation[volumeViewNumber] = 0.0;
}

/**
 * update prob atlas name table indices (call without arguments to update for all).
 */
void 
BrainModelVolume::updateProbAtlasNameTableIndices(const int probAtlasVolumeIndex)
{
   const int numVolumes = brainSet->getNumberOfVolumeProbAtlasFiles();
   
   //
   // If no volumes, clear the tables
   //
   if (numVolumes <= 0) {
      probAtlasNameTableIndices.clear();
      probAtlasNameTable.clear();
      return;
   }
   
   //
   // Doing all volumes ?
   //
   if (probAtlasVolumeIndex < 0) {
      //
      // Clear the tables
      //
      probAtlasNameTableIndices.clear();
      probAtlasNameTable.clear();

      //
      // Do each volume
      //
      for (int i = 0; i < numVolumes; i++) {
         std::vector<int> indices;
         
         //
         // Get the names from the volume and add to the table
         //
         VolumeFile* vf = brainSet->getVolumeProbAtlasFile(i);
         const int numNames = vf->getNumberOfRegionNames();
         for (int j = 0; j < numNames; j++) {
            indices.push_back(addNameToProbAtlasNameTable(vf->getRegionNameFromIndex(j)));
         }
         
         //
         // Update indices for this volume
         //
         probAtlasNameTableIndices.push_back(indices);
      }   
   }
   else {
      const int numValidVolumes = static_cast<int>(probAtlasNameTableIndices.size());
      int volIndex = -1;
      if ((probAtlasVolumeIndex >= 0) &&
          (probAtlasVolumeIndex < numValidVolumes)) {
         volIndex = probAtlasVolumeIndex;
      }
      
      //
      // Add space for any missing volumes
      //
      for (int i = numValidVolumes; i < (probAtlasVolumeIndex - 1); i++) {
         std::vector<int> dummy;
         probAtlasNameTableIndices.push_back(dummy);
      }
      
      //
      // Get the names from the volume and add to the table
      //
      VolumeFile* vf = brainSet->getVolumeProbAtlasFile(probAtlasVolumeIndex);
      const int numNames = vf->getNumberOfRegionNames();
      std::vector<int> indices;
      for (int j = 0; j < numNames; j++) {
         indices.push_back(addNameToProbAtlasNameTable(vf->getRegionNameFromIndex(j)));
      }
      
      //
      // Update indices for this volume
      //
      probAtlasNameTableIndices.push_back(indices);
   }   
}

/**
 * get the prob atlas name table index for a prob atlas volume voxel.
 */
int 
BrainModelVolume::getProbAtlasNameTableIndex(const int probAtlasVolumeIndex,
                                             const int voxelValue) const
{
   const int numVolumes = static_cast<int>(probAtlasNameTableIndices.size());
   if ((probAtlasVolumeIndex >= 0) &&
       (probAtlasVolumeIndex < numVolumes)) {
      const std::vector<int>& indices = probAtlasNameTableIndices[probAtlasVolumeIndex];
      const int num = static_cast<int>(indices.size());
      if ((voxelValue >= 0) &&
          (voxelValue < num)) {
         return indices[voxelValue];
      }
      else {
         std::cout << "PROGRAM ERROR: Invalid index for prob atlas name table index." << std::endl;
      }
   }
   else {
      std::cout << "PROGRAM ERROR: Invalid index for prob atlas name table volume." << std::endl;
   }
   
   return -1;
}                

/**
 * add name to prob atlas name table (returns its index).
 */
int 
BrainModelVolume::addNameToProbAtlasNameTable(const QString& name)
{
   //
   // See if name already in table
   //
   const int indx = getProbAtlasIndexFromName(name);
   if (indx >= 0) {
      return indx;
   }
   
   //
   // Add name to table
   //
   probAtlasNameTable.push_back(name);
   return (probAtlasNameTable.size() - 1);
}

/**
 * get a name by its index from the prob atlas name table.
 */
QString 
BrainModelVolume::getProbAtlasNameFromIndex(const int indx) const
{
   const int num = getNumberOfProbAtlasNames();
   if ((indx >= 0) && (indx < num)) {
      return probAtlasNameTable[indx];
   }
   
   return "";
}

/**
 * get an index for a name in the prob atlas name table.
 */
int 
BrainModelVolume::getProbAtlasIndexFromName(const QString& name) const
{
   const int num = getNumberOfProbAtlasNames();
   for (int i = 0; i < num; i++) {
      if (probAtlasNameTable[i] == name) {
         return i;
      }
   }
   
   return -1;
}

/**
 * get oblique transformations as string (16 rot, 1 scale).
 */
QString 
BrainModelVolume::getObliqueTransformationsAsString(const int viewNumber) const
{
   
   std::ostringstream str;
   str.precision(2);
   str.setf(std::ios::fixed);

/*   
   const vtkTransform* obliqueTrans = getObliqueRotationMatrix(viewNumber);
   TransformationMatrix tm;
   tm.setMatrix(obliqueTrans);
   float mat[16];
   tm.getMatrix(mat);
*/

   float mat[16];
   getObliqueRotationMatrix(mat);
   for (int i = 0; i < 16; i++) {
      str << mat[i]
          << " ";
   }
   
   float scale[3];
   getScaling(viewNumber, scale);
   str << scale[0] << " " << scale[1] << " " << scale[2];
       
   const QString s(str.str().c_str());

   return s;
}

/**
 * set oblique transformations from string (16 rot, 1 scale).
 */
void 
BrainModelVolume::setObliqueTransformationsAsString(const int viewNumber, const QString s)
{
   QString s2(s);
   QTextStream textStream(&s2, QIODevice::ReadOnly);
   
   float mat[16];
   for (int i = 0; i < 16; i++) {
      textStream >> mat[i];
   }
   setObliqueRotationMatrix(mat);

/*
   TransformationMatrix tm;
   tm.setMatrix(mat);
   vtkTransform* obliqueTrans = getObliqueRotationMatrix(viewNumber);
   tm.getMatrix(obliqueTrans);
*/   
   float scale[3];
   textStream >> scale[0] >> scale[1] >> scale[2];
   setScaling(viewNumber, scale);
}
      
/**
 * get the oblique rotation matrix.
 */
void 
BrainModelVolume::getObliqueRotationMatrix(float matrix[16]) const
{
   vtkTransform* obliqueRot = (vtkTransform*)getObliqueRotationMatrix();
   vtkMatrix4x4* m = vtkMatrix4x4::New(); 
   obliqueRot->GetMatrix(m);
    
   int cnt = 0; 
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) { 
         matrix[cnt] = m->GetElement(i, j);
         cnt++; 
      }
   }
   m->Delete();
}

/**
 * set the oblique rotation matrix.
 */
void 
BrainModelVolume::setObliqueRotationMatrix(const float matrix[16])
{
   vtkTransform* obliqueRot = getObliqueRotationMatrix();
   vtkMatrix4x4* m = vtkMatrix4x4::New();
   obliqueRot->GetMatrix(m);

   int cnt = 0;
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m->SetElement(i, j, matrix[cnt]);
         cnt++;
      }
   }
   obliqueRot->SetMatrix(m);
   m->Delete();
}      

/**
 * get show underlay only in window.
 */
bool 
BrainModelVolume::getShowUnderlayOnlyInWindow(const int viewNumber) const
{
   return showUnderlayOnly[viewNumber];
}

/**
 * set show underlay only in window.
 */
void 
BrainModelVolume::setShowUnderlayOnlyInWindow(const int viewNumber,
                                              const bool underlayOnlyFlag)
{
   showUnderlayOnly[viewNumber] = underlayOnlyFlag;
}
