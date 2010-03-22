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

#include <algorithm>

#include "BrainSet.h"
#include "DisplaySettingsVectors.h"
#include "FileUtilities.h"
#include "StringUtilities.h"
#include "VectorFile.h"

/**
 * The constructor.
 */
DisplaySettingsVectors::DisplaySettingsVectors(BrainSet* bs)
   : DisplaySettings(bs)
{
   lengthMultiplier = 10.0;
   sparseDistance = 50;
   colorMode = DisplaySettingsVectors::COLOR_MODE_XYZ_AS_RGB;
   displayModeSurface = DisplaySettingsVectors::DISPLAY_MODE_NONE;
   displayModeVolume = DisplaySettingsVectors::DISPLAY_MODE_NONE;
   vectorType = DisplaySettingsVectors::VECTOR_TYPE_BIDIRECTIONAL;
   surfaceSymbol = DisplaySettingsVectors::SURFACE_SYMBOL_2D_LINE;
   drawWithMagnitude = true;
   magnitudeThreshold = 0.05;
   surfaceVectorLineWidth = 1.0;
   volumeSliceDistanceAboveLimit = 1.05;
   volumeSliceDistanceBelowLimit = -1.05;
   segmentationMaskingVolumeEnabled = false;
   functionalMaskingVolumeEnabled = false;
   functionalMaskingVolumePositiveThreshold =  5.0;
   functionalMaskingVolumeNegativeThreshold = -5.0;

   displayOrientation = DISPLAY_ORIENTATION_ANY;
   displayOrientationAngle = 45.0;

   reset();
}

/**
 * The destructor.
 */
DisplaySettingsVectors::~DisplaySettingsVectors()
{
}

/**
 * Reinitialize all display settings
 */
void
DisplaySettingsVectors::reset()
{
   segmentationMaskingVolumeFile = NULL;
   functionalMaskingVolumeFile = NULL;
}

/**
 * Update any selections due to changes in loaded areal estimation file
 */
void
DisplaySettingsVectors::update()
{
}

/**
 * set the length multiplier.
 */
void 
DisplaySettingsVectors::setLengthMultiplier(const float len)
{
   lengthMultiplier = len;
}

/**
 * set the color mode.
 */
void
DisplaySettingsVectors::setColorMode(const COLOR_MODE colorModeIn)
{
   this->colorMode = colorModeIn;
}

/**
 * set draw with magnitude.
 */
void
DisplaySettingsVectors::setDrawWithMagnitude(const bool dwm)
{
   this->drawWithMagnitude = dwm;
}

/**
 * set the display mode.
 */ 
void 
DisplaySettingsVectors::setDisplayModeSurface(const DISPLAY_MODE dm)
{ 
   displayModeSurface = dm;
}

/**
 * set the display mode.
 */
void
DisplaySettingsVectors::setDisplayModeVolume(const DISPLAY_MODE dm)
{
   displayModeVolume = dm;
}

/**
 * set the sparse distance.
 */
void 
DisplaySettingsVectors::setSparseDisplayDistance(const int dist)
{ 
   sparseDistance = dist; 
}

/**
 * Get display a vector file.
 */
bool
DisplaySettingsVectors::getDisplayVectorFile(const int indx) const
{
   displayVectorFileFlag.resize(brainSet->getNumberOfVectorFiles(), true);
   if ((indx >= 0) && (indx < static_cast<int>(displayVectorFileFlag.size()))) {
      return displayVectorFileFlag[indx];
   }
   return false;
}

/**
 * Set display a vector file.
 */
void
DisplaySettingsVectors::setDisplayVectorFile(const int indx, const bool dispFlag)
{
   displayVectorFileFlag.resize(brainSet->getNumberOfVectorFiles(), true);
   if ((indx >= 0) && (indx < static_cast<int>(displayVectorFileFlag.size()))) {
      displayVectorFileFlag[indx] = dispFlag;
   }
}

static const QString surfaceVectorID("surface-vector-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsVectors::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/)
{
   segmentationMaskingVolumeFile = NULL;

   int numVectorFiles = brainSet->getNumberOfVectorFiles();
   for (int i = 0; i < numVectorFiles; i++) {
      this->setDisplayVectorFile(i, false);
   }

   surfaceSymbol = SURFACE_SYMBOL_2D_LINE;
   vectorType = VECTOR_TYPE_BIDIRECTIONAL;

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsVectors") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();
            if (infoName == "colorMode") {
               colorMode = static_cast<COLOR_MODE>(si->getValueAsInt());
            }
            else if (infoName == "surfaceSymbol") {
               const QString surfaceSymbolString = si->getValueAsString();
               if (surfaceSymbolString == "SURFACE_SYMBOL_3D") {
                  surfaceSymbol = SURFACE_SYMBOL_3D;
               }
               else if (surfaceSymbolString == "SURFACE_SYMBOL_2D_LINE") {
                  surfaceSymbol = SURFACE_SYMBOL_2D_LINE;
               }
            }
            else if (infoName == "vectorType") {
               const QString vectorTypeString = si->getValueAsString();
               if (vectorTypeString == "VECTOR_TYPE_BIDIRECTIONAL") {
                  vectorType = VECTOR_TYPE_BIDIRECTIONAL;
               }
               else if ((vectorTypeString == "VECTOR_TYPE_UNIDIRECTIONAL") ||
                        (vectorTypeString == "VECTOR_TYPE_UNIDIRECTIONAL_ARROW")) {
                  vectorType = VECTOR_TYPE_UNIDIRECTIONAL_ARROW;
               }
               else if (vectorTypeString == "VECTOR_TYPE_UNIDIRECTIONAL_CYLINDER") {
                  vectorType = VECTOR_TYPE_UNIDIRECTIONAL_CYLINDER;
               }
            }
            else if (infoName == "drawMode") {
               switch (si->getValueAsInt()) {
                  case 0:
                     surfaceSymbol = SURFACE_SYMBOL_3D;
                     vectorType    = VECTOR_TYPE_UNIDIRECTIONAL_ARROW;
                     break;
                  case 1:
                     surfaceSymbol = SURFACE_SYMBOL_3D;
                     vectorType    = VECTOR_TYPE_BIDIRECTIONAL;
                     break;
                  case 2:
                     surfaceSymbol = SURFACE_SYMBOL_2D_LINE;
                     vectorType    = VECTOR_TYPE_BIDIRECTIONAL;
                     break;
                  case 3:
                     surfaceSymbol = SURFACE_SYMBOL_2D_LINE;
                     vectorType    = VECTOR_TYPE_UNIDIRECTIONAL_ARROW;
                     break;
               }
            }
            else if  (infoName == "displayModeSurface") {
               displayModeSurface = static_cast<DISPLAY_MODE>(si->getValueAsInt());
            }
            else if  (infoName == "displayModeVolume") {
               displayModeVolume = static_cast<DISPLAY_MODE>(si->getValueAsInt());
            }
            else if (infoName == "sparseDistance") {
               si->getValue(sparseDistance);
            }
            else if (infoName == "drawWithMagnitude") {
               si->getValue(drawWithMagnitude);
            }
            else if (infoName == "lengthMultiplier") {
               si->getValue(lengthMultiplier);
            }
            else if (infoName == "magnitudeThreshold") {
               si->getValue(magnitudeThreshold);
            }
            else if (infoName == "volumeSliceDistanceBelowLimit") {
               si->getValue(volumeSliceDistanceBelowLimit);
            }
            else if (infoName == "volumeSliceDistanceAboveLimit") {
               si->getValue(volumeSliceDistanceAboveLimit);
            }
            else if (infoName == "surfaceVectorLineWidth") {
               si->getValue(surfaceVectorLineWidth);
            }
            else if (infoName == "segmentationMaskingVolumeEnabled") {
               si->getValue(segmentationMaskingVolumeEnabled);
            }
            else if (infoName == "segmentationMaskingVolumeFile") {
               const QString name = si->getValueAsString();
               for (int m = 0; m < brainSet->getNumberOfVolumeSegmentationFiles(); m++) {
                  VolumeFile* vf = brainSet->getVolumeSegmentationFile(m);
                  if (name == FileUtilities::basename(vf->getFileName())) {
                     segmentationMaskingVolumeFile = vf;
                     break;
                  }
               }
            }
            else if (infoName == "functionalMaskingVolumeEnabled") {
               si->getValue(functionalMaskingVolumeEnabled);
            }
            else if (infoName == "functionalMaskingVolumeFile") {
               const QString name = si->getValueAsString();
               for (int m = 0; m < brainSet->getNumberOfVolumeFunctionalFiles(); m++) {
                  VolumeFile* vf = brainSet->getVolumeFunctionalFile(m);
                  if (name == FileUtilities::basename(vf->getFileName())) {
                     functionalMaskingVolumeFile = vf;
                     break;
                  }
               }
            }
            else if (infoName == "functionalMaskingVolumePositiveThreshold") {
               si->getValue(functionalMaskingVolumePositiveThreshold);
            }
            else if (infoName == "functionalMaskingVolumeNegativeThreshold") {
               si->getValue(functionalMaskingVolumeNegativeThreshold);
            }
            else if (infoName == "displayVectorFileFlag") {
               const QString fileName = si->getValueAsString();
               for (int m = 0; m < numVectorFiles; m++) {
                  VectorFile* vf = brainSet->getVectorFile(m);
                  if (vf->getFileNameNoPath() == fileName) {
                     this->setDisplayVectorFile(m, true);
                     break;
                  }
               }
            }
            else if (infoName == "displayOrientationAngle") {
               si->getValue(displayOrientationAngle);
            }
            else if (infoName == "displayOrientation") {
               QString orientString = si->getValueAsString();
               DISPLAY_ORIENTATION orient = DISPLAY_ORIENTATION_ANY;
               if (orientString == "DISPLAY_ORIENTATION_ANY") {
                  orient = DISPLAY_ORIENTATION_ANY;
               }
               else if (orientString == "DISPLAY_ORIENTATION_LEFT_RIGHT") {
                  orient = DISPLAY_ORIENTATION_LEFT_RIGHT;
               }
               else if (orientString == "DISPLAY_ORIENTATION_POSTERIOR_ANTERIOR") {
                  orient = DISPLAY_ORIENTATION_POSTERIOR_ANTERIOR;
               }
               else if (orientString == "DISPLAY_ORIENTATION_INFERIOR_SUPERIOR") {
                  orient = DISPLAY_ORIENTATION_INFERIOR_SUPERIOR;
               }
               else {
                  std::cout << "ERROR: Invalid display orientation for vectors: \""
                            << orientString.toAscii().constData()
                            << std::endl;
               }
               setDisplayOrientation(orient);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsVectors::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   int numVectorFiles = brainSet->getNumberOfVectorFiles();
   if (onlyIfSelected) {
      if (numVectorFiles <= 0) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsVectors");

   sc.addSceneInfo(SceneFile::SceneInfo("colorMode", colorMode));
   QString vectorTypeString;
   switch (vectorType) {
      case VECTOR_TYPE_BIDIRECTIONAL:
         vectorTypeString = "VECTOR_TYPE_BIDIRECTIONAL";
         break;
      case VECTOR_TYPE_UNIDIRECTIONAL_ARROW:
         vectorTypeString = "VECTOR_TYPE_UNIDIRECTIONAL_ARROW";
         break;
      case VECTOR_TYPE_UNIDIRECTIONAL_CYLINDER:
         vectorTypeString = "VECTOR_TYPE_UNIDIRECTIONAL_CYLINDER";
         break;
   }
   sc.addSceneInfo(SceneFile::SceneInfo("vectorType", vectorTypeString));
   QString surfaceSymbolString;
   switch (surfaceSymbol) {
      case SURFACE_SYMBOL_3D:
         surfaceSymbolString = "SURFACE_SYMBOL_3D";
         break;
      case SURFACE_SYMBOL_2D_LINE:
         surfaceSymbolString = "SURFACE_SYMBOL_2D_LINE";
         break;
   }
   sc.addSceneInfo(SceneFile::SceneInfo("surfaceSymbol", surfaceSymbolString));
   sc.addSceneInfo(SceneFile::SceneInfo("displayModeSurface", displayModeSurface));
   
   sc.addSceneInfo(SceneFile::SceneInfo("displayModeVolume", displayModeVolume));

   sc.addSceneInfo(SceneFile::SceneInfo("sparseDistance",
                                        sparseDistance));
   sc.addSceneInfo(SceneFile::SceneInfo("lengthMultiplier",
                                        lengthMultiplier));
   sc.addSceneInfo(SceneFile::SceneInfo("drawWithMagnitude",
                                        drawWithMagnitude));
   sc.addSceneInfo(SceneFile::SceneInfo("magnitudeThreshold",
                                        magnitudeThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("volumeSliceDistanceAboveLimit",
                                        volumeSliceDistanceAboveLimit));
   sc.addSceneInfo(SceneFile::SceneInfo("volumeSliceDistanceBelowLimit",
                                        volumeSliceDistanceBelowLimit));
   sc.addSceneInfo(SceneFile::SceneInfo("surfaceVectorLineWidth",
                                        surfaceVectorLineWidth));
   sc.addSceneInfo(SceneFile::SceneInfo("segmentationMaskingVolumeEnabled",
                                        segmentationMaskingVolumeEnabled));
   if (getSegmentationMaskingVolumeFile() != NULL) {
      sc.addSceneInfo(SceneFile::SceneInfo("segmentationMaskingVolumeFile",
        FileUtilities::basename(getSegmentationMaskingVolumeFile()->getFileName())));
   }
   sc.addSceneInfo(SceneFile::SceneInfo("functionalMaskingVolumeEnabled",
                                        functionalMaskingVolumeEnabled));
   if (getFunctionalMaskingVolumeFile() != NULL) {
      sc.addSceneInfo(SceneFile::SceneInfo("functionalMaskingVolumeFile",
        FileUtilities::basename(getFunctionalMaskingVolumeFile()->getFileName())));
   }
   sc.addSceneInfo(SceneFile::SceneInfo("functionalMaskingVolumePositiveThreshold",
                                        functionalMaskingVolumePositiveThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("functionalMaskingVolumeNegativeThreshold",
                                        functionalMaskingVolumeNegativeThreshold));
   for (int i = 0; i < numVectorFiles; i++) {
      if (this->getDisplayVectorFile(i)) {
         VectorFile* vf = brainSet->getVectorFile(i);
         sc.addSceneInfo(SceneFile::SceneInfo("displayVectorFileFlag",
                                              vf->getFileNameNoPath()));
      }
   }
   sc.addSceneInfo(SceneFile::SceneInfo("displayOrientationAngle",
                                        displayOrientationAngle));
   QString orientString;
   switch (displayOrientation) {
      case DISPLAY_ORIENTATION_ANY:
         orientString = "DISPLAY_ORIENTATION_ANY";
         break;
      case DISPLAY_ORIENTATION_LEFT_RIGHT:
         orientString = "DISPLAY_ORIENTATION_LEFT_RIGHT";
         break;
      case DISPLAY_ORIENTATION_POSTERIOR_ANTERIOR:
         orientString = "DISPLAY_ORIENTATION_POSTERIOR_ANTERIOR";
         break;
      case DISPLAY_ORIENTATION_INFERIOR_SUPERIOR:
         orientString = "DISPLAY_ORIENTATION_INFERIOR_SUPERIOR";
         break;
   }
   sc.addSceneInfo(SceneFile::SceneInfo("displayOrientation",
                                        orientString));

   scene.addSceneClass(sc);
}

/**
 * set the volume slice above limit.
 */
void
DisplaySettingsVectors::setVolumeSliceDistanceAboveLimit(const float limit)
{
   this->volumeSliceDistanceAboveLimit = limit;
}

/**
 * set the volume slice below limit.
 */
void
DisplaySettingsVectors::setVolumeSliceDistanceBelowLimit(const float limit)
{
   this->volumeSliceDistanceBelowLimit = limit;
}

/**
 * set the surface vector line width.
 */
void
DisplaySettingsVectors::setSurfaceVectorLineWidth(const float lineWidth)
{
   this->surfaceVectorLineWidth = lineWidth;
}

/**
 * set the magnitude threshold.
 */
void
DisplaySettingsVectors::setMagnitudeThreshold(const float magThresh)
{
   this->magnitudeThreshold = magThresh;
}

/**
 * set enable segmentation masking volume.
 */
void
DisplaySettingsVectors::setSegmentationMaskingVolumeEnabled(bool enableIt)
{
   this->segmentationMaskingVolumeEnabled = enableIt;
}

/**
 * get the segmentation volume used for masking.
 */
VolumeFile*
DisplaySettingsVectors::getSegmentationMaskingVolumeFile()
{
   bool found = false;
   for (int i = 0; i < brainSet->getNumberOfVolumeSegmentationFiles(); i++) {
      if (brainSet->getVolumeSegmentationFile(i) == this->segmentationMaskingVolumeFile) {
         found = true;
         break;
      }
   }

   if (found == false) {
      this->segmentationMaskingVolumeFile = NULL;
   }
   if (this->segmentationMaskingVolumeFile == NULL) {
      if (brainSet->getNumberOfVolumeSegmentationFiles() > 0) {
         this->segmentationMaskingVolumeFile = brainSet->getVolumeSegmentationFile(0);
      }
   }
   
   return segmentationMaskingVolumeFile;
}

/**
 * set the segmentation volume used for masking.
 */
void
DisplaySettingsVectors::setSegmentationMaskingVolumeFile(VolumeFile* vf)
{
   this->segmentationMaskingVolumeFile = vf;
}

/**
 * Set enable functional masking volume.
 */
void
DisplaySettingsVectors::setFunctionalMaskingVolumeEnabled(bool enableIt)
{
   functionalMaskingVolumeEnabled = enableIt;
}

/**
 * Get the functional volume used for masking.
 */
VolumeFile*
DisplaySettingsVectors::getFunctionalMaskingVolumeFile()
{
   bool found = false;
   for (int i = 0; i < brainSet->getNumberOfVolumeFunctionalFiles(); i++) {
      if (brainSet->getVolumeFunctionalFile(i) == this->functionalMaskingVolumeFile) {
         found = true;
         break;
      }
   }

   if (found == false) {
      this->functionalMaskingVolumeFile = NULL;
   }
   if (this->functionalMaskingVolumeFile == NULL) {
      const int numFunctionalVolumes =
         brainSet->getNumberOfVolumeFunctionalFiles();
      if (numFunctionalVolumes > 0) {
         this->functionalMaskingVolumeFile =
                 brainSet->getVolumeFunctionalFile(numFunctionalVolumes - 1);
      }
   }

   return functionalMaskingVolumeFile;
}

/**
 *  set the functional volume used for masking.
 */
void
DisplaySettingsVectors::setFunctionalMaskingVolumeFile(VolumeFile* vf)
{
   functionalMaskingVolumeFile = vf;
}

/**
 * Set the functional masking volume positive theshold.
 */
void
DisplaySettingsVectors::setFunctionalMaskingVolumePositiveThreshold(const float value)
{
   functionalMaskingVolumePositiveThreshold = value;
}

/**
 * Set the functional masking volume negative.
 */
void
DisplaySettingsVectors::setFunctionalMaskingVolumeNegativeThreshold(const float value)
{
   functionalMaskingVolumeNegativeThreshold = value;
}
