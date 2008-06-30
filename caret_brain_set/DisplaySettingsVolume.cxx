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



#include "DisplaySettingsVolume.h"
#include "BrainModelSurface.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "PreferencesFile.h"
#include "VolumeFile.h"

/**
 * Constructor.
 */
DisplaySettingsVolume::DisplaySettingsVolume(BrainSet* bs)
   : DisplaySettings(bs)
{
   PreferencesFile* pf = BrainSet::getPreferencesFile();
   anatomyVolumeBrightness = pf->getAnatomyVolumeBrightness();
   anatomyVolumeContrast   = pf->getAnatomyVolumeContrast();
   anatomyColoringType     = ANATOMY_COLORING_TYPE_2_98;
   segmentationDrawType    = SEGMENTATION_DRAW_TYPE_BLEND;
   displayCrosshairCoordinates         = true;
   displayCrosshairs                   = true;
   displayOrientationLabels            = true;
   segmentationTranslucency = 0.5;
   for (int i = 0; i < MAXIMUM_OVERLAY_SURFACES; i++) {
      displayOverlaySurfaceOutline[i] = false;
      overlaySurfaceOutlineColor[i] = SURFACE_OUTLINE_COLOR_GREEN;
      if (i == 0) {
         overlaySurfaceOutlineColor[i] = SURFACE_OUTLINE_COLOR_GREEN;
      }
      else if (i == 1) {
         overlaySurfaceOutlineColor[i] = SURFACE_OUTLINE_COLOR_RED;
      }
      else if (i == 2) {
         overlaySurfaceOutlineColor[i] = SURFACE_OUTLINE_COLOR_BLUE;
      }
      else if (i == 3) {
         overlaySurfaceOutlineColor[i] = SURFACE_OUTLINE_COLOR_WHITE;
      }
      else if (i == 4) {
         overlaySurfaceOutlineColor[i] = SURFACE_OUTLINE_COLOR_BLACK;
      }
      overlaySurfaceOutlineThickness[i] = 0.8;
   }
   montageViewNumberOfRows = 3;
   montageViewNumberOfColumns = 3;
   montageSliceIncrement = 5;
   overlayOpacity = 1.0;
   displayColorBar = false;
   vectorVolumeSparsity = 1;
   obliqueSlicesSamplingSize = 1.0;
   
   reset();
}

/**
 * Destructor.
 */
DisplaySettingsVolume::~DisplaySettingsVolume()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsVolume::reset()
{
   selectedAnatomyVolume               = -1;
   selectedFunctionalVolumeView        = -1;
   selectedFunctionalVolumeThreshold   = -1;
   selectedPaintVolume                 = -1;
   selectedRgbVolume                   = -1;
   selectedSegmentationVolume          = -1;
   selectedVectorVolume                = -1;
   croppingSlicesValid                 = false;
   setAnatomyThreshold(256.0);
   anatomyThresholdValid = false;
   montageViewSelected = false;
   obliqueTransform = NULL;
   for (int i = 0; i < MAXIMUM_OVERLAY_SURFACES; i++) {
      overlaySurface[i] = NULL;
   }
}

/**
 * Update selections due to changes in loaded cells.
 */
void
DisplaySettingsVolume::update()
{
   updateFileType(brainSet->getNumberOfVolumeFunctionalFiles(), selectedFunctionalVolumeView);
   updateFileType(brainSet->getNumberOfVolumeFunctionalFiles(), selectedFunctionalVolumeThreshold);
   updateFileType(brainSet->getNumberOfVolumePaintFiles(), selectedPaintVolume);
   updateFileType(brainSet->getNumberOfVolumeRgbFiles(), selectedRgbVolume);
   updateFileType(brainSet->getNumberOfVolumeSegmentationFiles(), selectedSegmentationVolume);
   updateFileType(brainSet->getNumberOfVolumeAnatomyFiles(), selectedAnatomyVolume);
   updateFileType(brainSet->getNumberOfVolumeVectorFiles(), selectedVectorVolume);
   
   if (obliqueTransform != NULL) {
      TransformationMatrixFile* tmf = brainSet->getTransformationMatrixFile();
      if (tmf->getMatrixValid(obliqueTransform) == false) {
         obliqueTransform = NULL;
      }
   }
}

/**
 * set the cropping slices.
 */
void 
DisplaySettingsVolume::setCroppingSlices(const int slices[6])
{
   for (int i = 0; i < 6; i++) {
      croppingSlices[i] = slices[i];
   }
}

/**
 * get the cropping slices.
 */
void 
DisplaySettingsVolume::getCroppingSlices(int slices[6]) const
{
   for (int i = 0; i < 6; i++) {
      slices[i] = croppingSlices[i];
   }
}      

/**
 * update the selected file based upon the number of files
 */
void
DisplaySettingsVolume::updateFileType(const int numFiles, int& selectedIndex)
{
   if (numFiles > 0) {
      if ((selectedIndex < 0) || (selectedIndex >= numFiles)) {
         selectedIndex = 0;
      }
   }
   else {
      selectedIndex = -1;
   }
}

/**
 * get volume montage settings (returns true if montage on).
 */
void 
DisplaySettingsVolume::getMontageViewSettings(int& numRows, int& numSlices,
                                              int& sliceIncrement) const
{
   numRows   = montageViewNumberOfRows;
   numSlices = montageViewNumberOfColumns;
   sliceIncrement = montageSliceIncrement;
}

/**
 * set volume montage settings.
 */
void 
DisplaySettingsVolume::setMontageViewSettings(const int numRows, const int numCols, 
                                              const int sliceIncrement)
{
   montageViewNumberOfRows = numRows;
   montageViewNumberOfColumns = numCols;
   montageSliceIncrement = sliceIncrement;
}
      
const QString volumeFileAnatomyID("volume-file-anatomy");
const QString volumeFileFunctionalViewID("volume-file-func-view");
const QString volumeFileFunctionalThreshID("volume-file-func-thresh");
const QString volumeFilePaintID("volume-file-paint");
const QString volumeFileRgbID("volume-file-rgb");
const QString volumeFileSegmentationID("volume-file-segmentation");
const QString volumeFileVectorID("volume-file-vector");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsVolume::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsVolume") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == volumeFileAnatomyID) {
               setSelectedAnatomyVolume(si->getValueAsString());
            }
            else if (infoName == volumeFileFunctionalViewID) {
               setSelectedFunctionalVolumeView(si->getValueAsString());
            }
            else if (infoName == volumeFileFunctionalThreshID) {
               setSelectedFunctionalVolumeThreshold(si->getValueAsString());
            }
            else if (infoName == volumeFilePaintID) {
               setSelectedPaintVolume(si->getValueAsString());
            }
            else if (infoName == volumeFileRgbID) {
               setSelectedRgbVolume(si->getValueAsString());
            }
            else if (infoName == volumeFileSegmentationID) {
               setSelectedSegmentationVolume(si->getValueAsString());
            }
            else if (infoName == volumeFileVectorID) {
               setSelectedVectorVolume(si->getValueAsString());
            }
            else if (infoName == "anatomyVolumeBrightness") {
               si->getValue(anatomyVolumeBrightness);
            }
            else if (infoName == "anatomyVolumeContrast") {
               si->getValue(anatomyVolumeContrast);
            }
            else if (infoName == "displayCrosshairCoordinates") {
               si->getValue(displayCrosshairCoordinates);
            }
            else if (infoName == "displayOrientationLabels") {
               si->getValue(displayOrientationLabels);
            }
            else if (infoName == "displayCrosshairs") {
               si->getValue(displayCrosshairs);
            }
            else if (infoName == "anatomyColoringType") {
               int val;
               si->getValue(val);
               anatomyColoringType = static_cast<ANATOMY_COLORING_TYPE>(val);
            }
            else if (infoName == "segmentationDrawType") {
               int val;
               si->getValue(val);
               segmentationDrawType = static_cast<SEGMENTATION_DRAW_TYPE>(val);
            }
            else if (infoName == "segmentationTranslucency") {
               si->getValue(segmentationTranslucency);
            }
            else if ((infoName == "displaySurfaceOutline") ||
                     (infoName == "displayFiducialSurfaceOutline")){
               si->getValue(displayOverlaySurfaceOutline[0]);
            }
            else if ((infoName == "surfaceOutlineColor") ||
                     (infoName == "overlaySurfaceOutlineColor")) {
               int val;
               si->getValue(val);
               overlaySurfaceOutlineColor[0] = static_cast<SURFACE_OUTLINE_COLOR>(val);
            }
            else if ((infoName == "surfaceOutlineThickness") ||
                     (infoName == "fiducialSurfaceOutlineThickness")) {
               si->getValue(overlaySurfaceOutlineThickness[0]);
            }
            else if (infoName == "displayHullSurfaceOutline") {
               si->getValue(displayOverlaySurfaceOutline[1]);
            }
            else if (infoName == "hullSurfaceOutlineColor") {
               int val;
               si->getValue(val);
               overlaySurfaceOutlineColor[1] = static_cast<SURFACE_OUTLINE_COLOR>(val);
            }
            else if (infoName == "hullSurfaceOutlineThickness") {
               si->getValue(overlaySurfaceOutlineThickness[1]);
            }
            else if (infoName == "overlaySurface") {
               const int modelNum = si->getModelName().toInt();
               if ((modelNum >= 0) && (modelNum < MAXIMUM_OVERLAY_SURFACES)) {
                  overlaySurface[modelNum] = NULL;
                  if (si->getValueAsString() != "NULL") {
                     bool foundSurface = false;
                     for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
                        BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
                        if (bms != NULL) {
                           const QString coordName = FileUtilities::basename(bms->getCoordinateFile()->getFileName());
                           if (coordName == si->getValueAsString()) {
                              overlaySurface[modelNum] = bms;
                              foundSurface = true;
                              break;
                           }
                        }
                     }
                     if (foundSurface == false) {
                        errorMessage.append("Unable to find surface named ");
                        errorMessage.append(si->getValueAsString());
                        errorMessage.append(" for volume overlay\n");
                     }
                  }
               }
            }
            else if (infoName == "displayOverlaySurfaceOutline") {
               const int modelNum = si->getModelName().toInt();
               if ((modelNum >= 0) && (modelNum < MAXIMUM_OVERLAY_SURFACES)) {
                  si->getValue(displayOverlaySurfaceOutline[modelNum]);
               }
            }
            else if (infoName == "overlaySurfaceOutlineColor") {
               const int modelNum = si->getModelName().toInt();
               if ((modelNum >= 0) && (modelNum < MAXIMUM_OVERLAY_SURFACES)) {
                  int val;
                  si->getValue(val);
                  overlaySurfaceOutlineColor[modelNum] = static_cast<SURFACE_OUTLINE_COLOR>(val);
               }
            }
            else if (infoName == "overlaySurfaceOutlineThickness") {
               const int modelNum = si->getModelName().toInt();
               if ((modelNum >= 0) && (modelNum < MAXIMUM_OVERLAY_SURFACES)) {
                  si->getValue(overlaySurfaceOutlineThickness[modelNum]);
               }
            }
            else if (infoName == "montageViewSelected") {
               si->getValue(montageViewSelected);
            }
            else if (infoName == "montageViewNumberOfRows") {
               si->getValue(montageViewNumberOfRows);
            }
            else if (infoName == "montageViewNumberOfColumns") {
               si->getValue(montageViewNumberOfColumns);
            }
            else if (infoName == "montageSliceIncrement") {
               si->getValue(montageSliceIncrement);
            }
            else if (infoName == "overlayOpacity") {
               si->getValue(overlayOpacity);
            }
            else if (infoName == "displayColorBar") {
               si->getValue(displayColorBar);
            }
            else if (infoName == "obliqueTransform") {
               TransformationMatrixFile* tmf = brainSet->getTransformationMatrixFile();
               obliqueTransform = tmf->getTransformationMatrixWithName(si->getValueAsString());
            }
            else if (infoName == "obliqueSlicesSamplingSize") {
               si->getValue(obliqueSlicesSamplingSize);
            }
         }
      }
   }
}

/// help save volume file for scene
void
DisplaySettingsVolume::sceneSelectedVolumeHelper(SceneFile::SceneClass& sceneClass,
                         const QString& infoName,
                         const VolumeFile* vf)
{
   if (vf != NULL) {
      sceneClass.addSceneInfo(SceneFile::SceneInfo(infoName, vf->getDescriptiveLabel()));
//                                           FileUtilities::basename(vf->getFileName())));
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsVolume::saveScene(SceneFile::Scene& scene, const bool onlyIfSelectedFlag,
                             QString& /*errorMessage*/)
{
   if (onlyIfSelectedFlag) {
      const int numVolumes =
           brainSet->getNumberOfVolumeAnatomyFiles()
         + brainSet->getNumberOfVolumeFunctionalFiles()
         + brainSet->getNumberOfVolumePaintFiles()
         + brainSet->getNumberOfVolumeProbAtlasFiles()
         + brainSet->getNumberOfVolumeRgbFiles()
         + brainSet->getNumberOfVolumeSegmentationFiles()
         + brainSet->getNumberOfVolumeVectorFiles();
      if (numVolumes <= 0) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsVolume");
   
   BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   if (bmv != NULL) {
      sceneSelectedVolumeHelper(sc, volumeFileAnatomyID,
                                bmv->getSelectedVolumeAnatomyFile());
      sceneSelectedVolumeHelper(sc, volumeFileFunctionalViewID,
                                bmv->getSelectedVolumeFunctionalViewFile());
      sceneSelectedVolumeHelper(sc, volumeFileFunctionalThreshID,
                                bmv->getSelectedVolumeFunctionalThresholdFile());
      sceneSelectedVolumeHelper(sc, volumeFilePaintID,
                                bmv->getSelectedVolumePaintFile());
      sceneSelectedVolumeHelper(sc, volumeFileRgbID,
                                bmv->getSelectedVolumeRgbFile());
      sceneSelectedVolumeHelper(sc, volumeFileSegmentationID,
                                bmv->getSelectedVolumeSegmentationFile());
      sceneSelectedVolumeHelper(sc, volumeFileVectorID,
                                bmv->getSelectedVolumeVectorFile());
   }
   sc.addSceneInfo(SceneFile::SceneInfo("anatomyVolumeBrightness",
                                        anatomyVolumeBrightness));
   sc.addSceneInfo(SceneFile::SceneInfo("anatomyVolumeContrast",
                                        anatomyVolumeContrast));
   sc.addSceneInfo(SceneFile::SceneInfo("displayCrosshairCoordinates",
                                        displayCrosshairCoordinates));
   sc.addSceneInfo(SceneFile::SceneInfo("displayCrosshairs",
                                        displayCrosshairs));
   sc.addSceneInfo(SceneFile::SceneInfo("displayOrientationLabels",
                                        displayOrientationLabels));
   sc.addSceneInfo(SceneFile::SceneInfo("anatomyColoringType",
                                        static_cast<int>(anatomyColoringType)));
   sc.addSceneInfo(SceneFile::SceneInfo("segmentationDrawType",
                                        static_cast<int>(segmentationDrawType)));
   sc.addSceneInfo(SceneFile::SceneInfo("segmentationTranslucency",
                                        segmentationTranslucency));
   for (int i = 0; i < MAXIMUM_OVERLAY_SURFACES; i++) {
      const QString modelNum(QString::number(i));
      QString surfaceName("NULL");
      if (overlaySurface[i] != NULL) {
         if (brainSet->getBrainModelIndex(overlaySurface[i]) >= 0) {
            surfaceName = FileUtilities::basename(overlaySurface[i]->getCoordinateFile()->getFileName());
         }
      }
      sc.addSceneInfo(SceneFile::SceneInfo("overlaySurface",
                                           modelNum,
                                           surfaceName));
      sc.addSceneInfo(SceneFile::SceneInfo("displayOverlaySurfaceOutline",
                                           modelNum,
                                           displayOverlaySurfaceOutline[i]));
      sc.addSceneInfo(SceneFile::SceneInfo("overlaySurfaceOutlineColor",
                                           modelNum,
                                           static_cast<int>(overlaySurfaceOutlineColor[i])));
      sc.addSceneInfo(SceneFile::SceneInfo("overlaySurfaceOutlineThickness",
                                           modelNum,
                                           overlaySurfaceOutlineThickness[i]));
   }
   sc.addSceneInfo(SceneFile::SceneInfo("montageViewSelected",
                                        montageViewSelected));
   sc.addSceneInfo(SceneFile::SceneInfo("montageViewNumberOfRows",
                                        montageViewNumberOfRows));
   sc.addSceneInfo(SceneFile::SceneInfo("montageViewNumberOfColumns",
                                        montageViewNumberOfColumns));
   sc.addSceneInfo(SceneFile::SceneInfo("montageSliceIncrement",
                                        montageSliceIncrement));
   sc.addSceneInfo(SceneFile::SceneInfo("overlayOpacity",
                                        overlayOpacity));
   sc.addSceneInfo(SceneFile::SceneInfo("displayColorBar",
                                        displayColorBar));
   if (obliqueTransform != NULL) {
      sc.addSceneInfo(SceneFile::SceneInfo("obliqueTransform",
                                           obliqueTransform->getMatrixName()));
   }
   sc.addSceneInfo(SceneFile::SceneInfo("obliqueSlicesSamplingSize",
                                        obliqueSlicesSamplingSize));
   
   scene.addSceneClass(sc);
}
                       
/**
 * set the selected anatomy volume file.
 */
void 
DisplaySettingsVolume::setSelectedAnatomyVolume(const QString& name)
{
   std::vector<VolumeFile*> files;
   brainSet->getVolumeAnatomyFiles(files);
   const int num = fileSelectionHelper(files, name);
   if (num >= 0) {
      setSelectedAnatomyVolume(num);
   }
}

/**
 * set the selected vector volume file.
 */
void 
DisplaySettingsVolume::setSelectedVectorVolume(const QString& name)
{
   std::vector<VolumeFile*> files;
   brainSet->getVolumeVectorFiles(files);
   const int num = fileSelectionHelper(files, name);
   if (num >= 0) {
      setSelectedVectorVolume(num);
   }
}

/**
 * set the selected segmentation volume file.
 */
void 
DisplaySettingsVolume::setSelectedSegmentationVolume(const QString& name)
{
   std::vector<VolumeFile*> files;
   brainSet->getVolumeSegmentationFiles(files);
   const int num = fileSelectionHelper(files, name);
   if (num >= 0) {
      setSelectedSegmentationVolume(num);
   }
}

/**
 * set the selected rgb volume file.
 */
void 
DisplaySettingsVolume::setSelectedRgbVolume(const QString& name)
{
   std::vector<VolumeFile*> files;
   brainSet->getVolumeRgbFiles(files);
   const int num = fileSelectionHelper(files, name);
   if (num >= 0) {
      setSelectedRgbVolume(num);
   }
}

/**
 * set the selected paint volume file.
 */
void 
DisplaySettingsVolume::setSelectedPaintVolume(const QString& name)
{
   std::vector<VolumeFile*> files;
   brainSet->getVolumePaintFiles(files);
   const int num = fileSelectionHelper(files, name);
   if (num >= 0) {
      setSelectedPaintVolume(num);
   }
}

/**
 * set the selected functional volume file for thresholding.
 */
void 
DisplaySettingsVolume::setSelectedFunctionalVolumeThreshold(const QString& name)
{
   std::vector<VolumeFile*> files;
   brainSet->getVolumeFunctionalFiles(files);
   const int num = fileSelectionHelper(files, name);
   if (num >= 0) {
      setSelectedFunctionalVolumeThreshold(num);
   }
}

/**
 * set the selected functional volume file for viewing.
 */
void 
DisplaySettingsVolume::setSelectedFunctionalVolumeView(const QString& name)
{
   std::vector<VolumeFile*> files;
   brainSet->getVolumeFunctionalFiles(files);
   const int num = fileSelectionHelper(files, name);
   if (num >= 0) {
      setSelectedFunctionalVolumeView(num);
   }
}      

/**
 * file selection helper.
 */
int 
DisplaySettingsVolume::fileSelectionHelper(const std::vector<VolumeFile*>& files,
                                           const QString& fileName)
{
   const QString& name = FileUtilities::basename(fileName);
   for (unsigned int i = 0; i < files.size(); i++) {
      if (name == files[i]->getDescriptiveLabel()) {
         return i;
      }
      if (name == FileUtilities::basename(files[i]->getFileName())) {
         return i;
      }
   }
   return -1;
}

/**
 * Set one of the "overlay" surfaces.
 */
void
DisplaySettingsVolume::setOverlaySurface(const int overlaySurfaceNumber,
                            BrainModelSurface* bms)
{
   overlaySurface[overlaySurfaceNumber] = bms;
}

/**
 * Get one of the "overlay" surfaces.
 */
BrainModelSurface*
DisplaySettingsVolume::getOverlaySurface(const int overlaySurfaceNumber)
{
   bool overlaySurfaceFound = false;
   
   //
   // 1st surface is always active fiducial to avoid breaking older scenes
   //
   if (overlaySurfaceNumber > 0) {
      // 
      // Make sure overlay surface is still valid
      //
      if (overlaySurface[overlaySurfaceNumber] != NULL) {
         const int numBrains = brainSet->getNumberOfBrainModels();
         for (int i = 0; i < numBrains; i++) {
            BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
            if (bms != NULL) {
               if (bms == overlaySurface[overlaySurfaceNumber]) {
                  overlaySurfaceFound = true;
                  break;
               }
            }
         }
      }
   }
   
   //
   // Use the active fiducial surface to make the active surface
   //
   if (overlaySurfaceFound == false) {
      overlaySurface[overlaySurfaceNumber] = brainSet->getActiveFiducialSurface();
   }
   
   return overlaySurface[overlaySurfaceNumber];
}

/**
 * get the anatomy threshold value.
 */
void 
DisplaySettingsVolume::getAnatomyThreshold(float& minThresh, float& maxThresh) const 
{ 
   minThresh = anatomyThreshold[0]; 
   maxThresh = anatomyThreshold[1]; 
}

/**
 * set the anatomy threshold value.
 */
void 
DisplaySettingsVolume::setAnatomyThreshold(const float minThresh,
                                           const float maxThresh) 
{ 
   anatomyThreshold[0] = minThresh; 
   anatomyThreshold[1] = maxThresh; 
}
      
