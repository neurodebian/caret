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

#include <QGLWidget>

#include "BrainSet.h"
#include "BrainModelSurfaceAndVolume.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainModelVolume.h"
#include "DisplaySettingsVolume.h"
#include "MathUtilities.h"
#include "TopologyHelper.h"
#include "TransformationMatrixFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelSurfaceAndVolume::BrainModelSurfaceAndVolume(BrainSet* bs)
 : BrainModelSurface(bs, BrainModel::BRAIN_MODEL_SURFACE_AND_VOLUME)
{
   voxelCloudDisplayListNumber = 0;
   reset();
   setSurface();
}
   
/**
 * Destructor.
 */
BrainModelSurfaceAndVolume::~BrainModelSurfaceAndVolume()
{
   clearVoxelCloudDisplayList();
}

/**
 * Get a descriptive name of the model.
 */
QString
BrainModelSurfaceAndVolume::getDescriptiveName() const
{
   return "SURFACE & VOLUME";
}

/**
 * Copy active fiducial surface into this surface
 */
void
BrainModelSurfaceAndVolume::setSurface()
{
   coordinates.clear();
   topology = NULL;
   normals.clear();
   surfaceType = SURFACE_TYPE_UNKNOWN;
   
   BrainModelSurface* bms = brainSet->getActiveFiducialSurface();
   if (bms != NULL) {
      coordinates = *(bms->getCoordinateFile());
      topology    = bms->getTopologyFile();
      setSurfaceType(SURFACE_TYPE_FIDUCIAL);
      computeNormals();
      setStructure(bms->getStructure());
   }
}

/**
 * Reset the volume.
 */
void
BrainModelSurfaceAndVolume::reset()
{
   BrainModelSurface::reset();
   
   previousFunctionalVolumeFile = NULL;
   previousSegmentationVolumeFile = NULL;
   selectedSlices[0] = 0;
   selectedSlices[1] = 0;
   selectedSlices[2] = 0;
   displayHorizontalSlice = true;
   displayParasagittalSlice = true;
   displayCoronalSlice = true;
   displayViewDependentSlice = false;
   displaySurface = true;
   displayFunctionalVolumeCloud = false;
   displaySegmentationVolumeCloud = false;
   displayVectorVolumeCloud = false;
   displaySecondaryOverlayVolumeOnSlices = false;
   displayPrimaryOverlayVolumeOnSlices = false;
   functionalVolumeCloudOpacity = 0.5;
   functionalVolumeCloudOpacityEnabled = false;
   functionalVolumeDistanceThreshold = 1000.0;
   drawAnatomyBlackVoxels = true;
   
   clearVoxelCloudDisplayList();
}

/**
 * Apply a transformation matrix to the surface.
 */
void
BrainModelSurfaceAndVolume::applyTransformationMatrix(TransformationMatrix& tm)
{
   const TopologyHelper* th = topology->getTopologyHelper(false, true, false);
   const int numNodes = getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      float xyz[3] = { 0.0, 0.0, 0.0 };
      if (th->getNodeHasNeighbors(i)) {
         coordinates.getCoordinate(i, xyz);
         double p[4] = { xyz[0], xyz[1], xyz[2], 1.0 };
         tm.multiplyPoint(p);
         xyz[0] = p[0];
         xyz[1] = p[1];
         xyz[2] = p[2];
      }
      coordinates.setCoordinate(i, xyz);
   }
   computeNormals();
}

/**
 * Initialize the selected slices
 */
void
BrainModelSurfaceAndVolume::initializeSelectedSlices()
{
   VolumeFile* vf = getAnatomyVolumeFile();
   if (vf != NULL) {
      int voxelDimensions[3];
      vf->getDimensions(voxelDimensions);
      selectedSlices[0] = voxelDimensions[0] / 2;
      selectedSlices[1] = voxelDimensions[1] / 2;
      selectedSlices[2] = voxelDimensions[2] / 2;
   }
}

/**
 * Get the selected slices.
 */
void
BrainModelSurfaceAndVolume::getSelectedSlices(int slices[3]) 
{
   slices[0] = selectedSlices[0];
   slices[1] = selectedSlices[1];
   slices[2] = selectedSlices[2];

   VolumeFile* vf = getAnatomyVolumeFile();
   if (vf != NULL) {
      int dim[3];
      vf->getDimensions(dim);
      if ((slices[0] >= dim[0]) ||
          (slices[1] >= dim[1]) ||
          (slices[2] >= dim[2])) {
         initializeSelectedSlices();
         slices[0] = selectedSlices[0];
         slices[1] = selectedSlices[1];
         slices[2] = selectedSlices[2];
      }
   }
}

/**
 * Set the selected slices.
 */
void
BrainModelSurfaceAndVolume::setSelectedSlices(const int slices[3])
{
   selectedSlices[0] = slices[0];
   selectedSlices[1] = slices[1];
   selectedSlices[2] = slices[2];
}

/**
 * get the selected anatomy volume file.
 */
VolumeFile* 
BrainModelSurfaceAndVolume::getAnatomyVolumeFile()
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
 * get the selected functional volume file.
 */
VolumeFile* 
BrainModelSurfaceAndVolume::getFunctionalVolumeFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* functionalVolumeFile = NULL;
   if ((dsv->getSelectedFunctionalVolumeView() >= 0) &&
       (dsv->getSelectedFunctionalVolumeView() < brainSet->getNumberOfVolumeFunctionalFiles())) {
      functionalVolumeFile = brainSet->getVolumeFunctionalFile(dsv->getSelectedFunctionalVolumeView());
   }
   
   if (functionalVolumeFile != previousFunctionalVolumeFile) {
      clearVoxelCloudDisplayList();
   }
   previousFunctionalVolumeFile = functionalVolumeFile;
   
   return functionalVolumeFile;
}

/**
 * get the selected segmentation volume file.
 */
VolumeFile* 
BrainModelSurfaceAndVolume::getSegmentationVolumeFile()
{
   DisplaySettingsVolume* dsv = brainSet->getDisplaySettingsVolume();
   VolumeFile* segmentationVolumeFile = NULL;
   if ((dsv->getSelectedSegmentationVolume() >= 0) &&
       (dsv->getSelectedSegmentationVolume() < brainSet->getNumberOfVolumeSegmentationFiles())) {
      segmentationVolumeFile = brainSet->getVolumeSegmentationFile(dsv->getSelectedSegmentationVolume());
   }
   
   if (segmentationVolumeFile != previousSegmentationVolumeFile) {
      //clearVoxelCloudDisplayList();
   }
   previousSegmentationVolumeFile = segmentationVolumeFile;
   
   return segmentationVolumeFile;
}

/**
 * get the selected vector volume file.
 */
VolumeFile* 
BrainModelSurfaceAndVolume::getVectorVolumeFile()
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
 * get the secondary overlay volume file.
 */
VolumeFile* 
BrainModelSurfaceAndVolume::getOverlaySecondaryVolumeFile()
{
   BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   if (bmv != NULL) {
      return bmv->getOverlaySecondaryVolumeFile();
   }
   
   return NULL;
}

/**
 * get the voxel cloud display list.
 */
unsigned int 
BrainModelSurfaceAndVolume::getVoxelCloudDisplayListNumber() const
{
   return voxelCloudDisplayListNumber;
}

/**
 * set the voxel cloud display list.
 */
void 
BrainModelSurfaceAndVolume::setVoxelCloudDisplayListNumber(const unsigned int num)
{
   voxelCloudDisplayListNumber = num;
}

/**
 * clear the voxel cloud display list.
 */
void 
BrainModelSurfaceAndVolume::clearVoxelCloudDisplayList()
{
   if (voxelCloudDisplayListNumber > 0) {
      glDeleteLists(voxelCloudDisplayListNumber, 1);
      voxelCloudDisplayListNumber = 0;
   }
}      

/**
 * set display functional volume data on slices.
 */
void 
BrainModelSurfaceAndVolume::setDisplayFunctionalVolumeCloud(const bool fvc) 
{ 
   displayFunctionalVolumeCloud = fvc; 
   clearVoxelCloudDisplayList();
}
         
/**
 * set display segmentation volume data cloud.
 */
void 
BrainModelSurfaceAndVolume::setDisplaySegmentationVolumeCloud(const bool fvc)
{
   displaySegmentationVolumeCloud = fvc;
}
         
/**
 * set functional volume cloud opacity enabled.
 */
void 
BrainModelSurfaceAndVolume::setFunctionalVolumeCloudOpacityEnabled(const bool fcoe)
{ 
   functionalVolumeCloudOpacityEnabled = fcoe;
   clearVoxelCloudDisplayList();
}
         
/**
 * set the functional volume distance threshold.
 */
void 
BrainModelSurfaceAndVolume::setFunctionalVolumeDistanceThreshold(const float fvdt) 
{ 
   functionalVolumeDistanceThreshold = fvdt; 
   clearVoxelCloudDisplayList();
}
         

/**
 * get the primary overlay volume file.
 */
VolumeFile* 
BrainModelSurfaceAndVolume::getOverlayPrimaryVolumeFile()
{
   BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   if (bmv != NULL) {
      return bmv->getOverlayPrimaryVolumeFile();
   }
   
   return NULL;
}

/**
 * Update functional volume file voxel to surface distances
 */
void
BrainModelSurfaceAndVolume::updateFunctionalVolumeSurfaceDistances()
{
   BrainModelSurface* bms = brainSet->getActiveFiducialSurface();
   if (bms != NULL) {
      VolumeFile* vf = getFunctionalVolumeFile();
      if (vf->getVoxelToSurfaceDistancesValid() == false) {
         if (vf != NULL) {
            float* voxelToSurfaceDistances = vf->getVoxelToSurfaceDistances();
            if (voxelToSurfaceDistances != NULL) {
               //
               // Point locator for finding node nearest to voxels
               //
               BrainModelSurfacePointLocator pl(bms, true);
               
               //
               // Get volume dimensions
               //
               int dim[3];
               vf->getDimensions(dim);
               
               //
               // Get volume spacing 
               //
               float spacing[3];
               vf->getSpacing(spacing);
               const float sx = spacing[0];
               const float sy = spacing[1];
               const float sz = spacing[2];
               
               //
               // Get volume origin in center of voxel
               //
               float origin[3];
               vf->getOrigin(origin);
               const float ox = origin[0] + sx * 0.5;
               const float oy = origin[1] + sy * 0.5;
               const float oz = origin[2] + sz * 0.5;
               
               const CoordinateFile* cf = bms->getCoordinateFile();
               
               //
               // do all voxels
               //
               for (int i = 0; i < dim[0]; i++) {
                  for (int j = 0; j < dim[1]; j++) {
                     for (int k = 0; k < dim[2]; k++) {
                        int ijk[3] = { i, j, k };
                        const int id = vf->getVoxelNumber(ijk);
                        if (id >= 0) {
                           //
                           // XYZ of voxel center
                           //
                           const float xyz[3] = {
                                                  ox + sx * i,
                                                  oy + sy * j,
                                                  oz + sz * k
                                                 };
                           const int node = pl.getNearestPoint(xyz);
                           float dist = 0.0;
                           if (node >= 0) {
                              const float* nodeXYZ = cf->getCoordinate(node);
                              dist = MathUtilities::distance3D(nodeXYZ, xyz);
                           }
                           voxelToSurfaceDistances[id] = dist;
                        }
                     }
                  }
               }
               vf->setVoxelToSurfaceDistancesValid(true);
            }
         }
      }
   }
}

/**
 * set the display surface flag.
 */
void 
BrainModelSurfaceAndVolume::setDisplaySurface(const bool ds) 
{ 
   //
   // If turning surface on/off need to delete display lists
   //
   if (displaySurface != ds) {
      brainSet->clearAllDisplayLists();
   }
   
   displaySurface = ds;
}

/**
 * apply a scene (set display settings).
 */
void 
BrainModelSurfaceAndVolume::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/)
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "BrainModelSurfaceAndVolume") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();

            if (infoName == "displaySurface") {
               si->getValue(displaySurface);
            }
            else if (infoName == "displaySecondaryOverlayVolumeOnSlices") {
               si->getValue(displaySecondaryOverlayVolumeOnSlices);
            }
            else if (infoName == "displayPrimaryOverlayVolumeOnSlices") {
               si->getValue(displayPrimaryOverlayVolumeOnSlices);
            }
            else if (infoName == "displayFunctionalVolumeCloud") {
               si->getValue(displayFunctionalVolumeCloud);
            }
            else if (infoName == "displaySegmentationVolumeCloud") {
               si->getValue(displaySegmentationVolumeCloud);
            }
            else if (infoName == "selectedSlices0") {
               si->getValue(selectedSlices[0]);
            }
            else if (infoName == "selectedSlices1") {
               si->getValue(selectedSlices[1]);
            }
            else if (infoName == "selectedSlices2") {
               si->getValue(selectedSlices[2]);
            }
            else if (infoName == "displayHorizontalSlice") {
               si->getValue(displayHorizontalSlice);
            }
            else if (infoName == "displayParasagittalSlice") {
               si->getValue(displayParasagittalSlice);
            }
            else if (infoName == "displayCoronalSlice") {
               si->getValue(displayCoronalSlice);
            }
            else if (infoName == "functionalVolumeCloudOpacity") {
               si->getValue(functionalVolumeCloudOpacity);
            }
            else if (infoName == "functionalVolumeCloudOpacityEnabled") {
               si->getValue(functionalVolumeCloudOpacityEnabled);
            }
            else if (infoName == "drawAnatomyBlackVoxels") {
               si->getValue(drawAnatomyBlackVoxels);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
BrainModelSurfaceAndVolume::saveScene(SceneFile::Scene& scene,
                                      const bool /*onlyIfSelectedFlag*/)
{
   SceneFile::SceneClass sc("BrainModelSurfaceAndVolume");
   
   sc.addSceneInfo(SceneFile::SceneInfo("displaySurface",
                                        displaySurface));
   sc.addSceneInfo(SceneFile::SceneInfo("displaySecondaryOverlayVolumeOnSlices",
                                        displaySecondaryOverlayVolumeOnSlices));
   sc.addSceneInfo(SceneFile::SceneInfo("displayPrimaryOverlayVolumeOnSlices",
                                        displayPrimaryOverlayVolumeOnSlices));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFunctionalVolumeCloud",
                                        displayFunctionalVolumeCloud));
   sc.addSceneInfo(SceneFile::SceneInfo("displaySegmentationVolumeCloud",
                                        displaySegmentationVolumeCloud));
   sc.addSceneInfo(SceneFile::SceneInfo("selectedSlices0",
                                        selectedSlices[0]));
   sc.addSceneInfo(SceneFile::SceneInfo("selectedSlices1",
                                        selectedSlices[1]));
   sc.addSceneInfo(SceneFile::SceneInfo("selectedSlices2",
                                        selectedSlices[2]));
   sc.addSceneInfo(SceneFile::SceneInfo("displayHorizontalSlice",
                                        displayHorizontalSlice));
   sc.addSceneInfo(SceneFile::SceneInfo("displayParasagittalSlice",
                                        displayParasagittalSlice));
   sc.addSceneInfo(SceneFile::SceneInfo("displayCoronalSlice",
                                        displayCoronalSlice));
   sc.addSceneInfo(SceneFile::SceneInfo("functionalVolumeCloudOpacity",
                                        functionalVolumeCloudOpacity));
   sc.addSceneInfo(SceneFile::SceneInfo("functionalVolumeCloudOpacityEnabled",
                                        functionalVolumeCloudOpacityEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("drawAnatomyBlackVoxels",
                                        drawAnatomyBlackVoxels));
                                        
   scene.addSceneClass(sc);             
}
                       
         

