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
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "FileUtilities.h"
#include "ProbabilisticAtlasFile.h"

/**
 * Constructor.
 */
DisplaySettingsProbabilisticAtlas::DisplaySettingsProbabilisticAtlas(BrainSet* bs,
                                                 const PROBABILISTIC_TYPE probTypeIn)
   : DisplaySettings(bs)
{
   reset();
   probType = probTypeIn;
}

/**
 * Destructor.
 */
DisplaySettingsProbabilisticAtlas::~DisplaySettingsProbabilisticAtlas()
{
}

/**
 * Get the number of selected channels.
 */
int
DisplaySettingsProbabilisticAtlas::getNumberOfChannelsSelected() const
{
   if (applySelectionToLeftAndRightStructuresFlag) {
      return std::count(channelSelectedForStructure.begin(), channelSelectedForStructure.end(), true);
   }
   else {
      return std::count(channelSelected.begin(), channelSelected.end(), true);
   }
}

/**
 * update structure associated with each channel.
 */
void 
DisplaySettingsProbabilisticAtlas::updateSelectedChannelsForCurrentStructure(const Structure& structure)
{
   switch (probType) {
      case PROBABILISTIC_TYPE_SURFACE:
         {
            ProbabilisticAtlasFile* pf = brainSet->getProbabilisticAtlasSurfaceFile();
            const int numColumns = pf->getNumberOfColumns();
            for (int i = 0; i < numColumns; i++) {
               //
               // Default channel off for structure
               //
               channelSelectedForStructure[i] = false;

               //
               // Is channel on?
               //
               if (channelSelected[i]) {
                  switch (structure.getType()) {
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                        if (pf->getColumnName(i).toLower().indexOf("left") >= 0) {
                           channelSelectedForStructure[i] = true;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                        if (pf->getColumnName(i).toLower().indexOf("right") >= 0) {
                           channelSelectedForStructure[i] = true;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_INVALID:
                        break;
                   }
               }
            }
         }
         break;
      case PROBABILISTIC_TYPE_VOLUME:
         break;
   }
}
      
/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsProbabilisticAtlas::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   if (applySelectionToLeftAndRightStructuresFlag) {
      const BrainModelSurface* bms1 = brainSet->getBrainModelSurface(bm1);
      const BrainModelSurface* bms2 = brainSet->getBrainModelSurface(bm2);
      if ((bms1 != NULL) && (bms2 != NULL)) {
         if (bms1->getStructure().getType() != bms2->getStructure().getType()) {
            //
            // Different structures, probably left/right
            //
            return false;
         }
      }
   }
   
   return true;
}
       
/**
 * get channel selected.
 */
bool 
DisplaySettingsProbabilisticAtlas::getChannelSelected(const int indx) const 
{ 
   return channelSelected[indx]; 
}
      
/**
 * get channel selected.
 */
bool 
DisplaySettingsProbabilisticAtlas::getChannelSelectedForStructure(const int indx) const 
{ 
   if (applySelectionToLeftAndRightStructuresFlag) {
      return channelSelectedForStructure[indx];
   }
   else {
      return channelSelected[indx]; 
   }
}
      
/**
 * Set the selection status of all channels
 */
void
DisplaySettingsProbabilisticAtlas::setAllChannelsSelectedStatus(const bool status)
{
   std::fill(channelSelected.begin(), channelSelected.end(), status);
}

/**
 * Set the selection status of all areas.
 */
void
DisplaySettingsProbabilisticAtlas::setAllAreasSelectedStatus(const bool status)
{
   std::fill(areaSelected.begin(), areaSelected.end(), status);
}

/**
 * Reset the settings.
 */
void
DisplaySettingsProbabilisticAtlas::reset()
{
   displayType = PROBABILISTIC_DISPLAY_TYPE_NORMAL;
   channelSelected.clear();
   channelSelectedForStructure.clear();
   areaSelected.clear();
   treatQuestColorAsUnassigned = false;
   thresholdDisplayTypeRatio = 0.5;
   applySelectionToLeftAndRightStructuresFlag = false;
}

/**
 * Update as a result of new files being loaded.
 */ 
void
DisplaySettingsProbabilisticAtlas::update()
{
   switch (probType) {
      case PROBABILISTIC_TYPE_SURFACE:
         {
            ProbabilisticAtlasFile* pf = brainSet->getProbabilisticAtlasSurfaceFile();
            const int numColumns = pf->getNumberOfColumns();
            channelSelected.resize(numColumns);
            std::fill(channelSelected.begin(), channelSelected.end(), true);
            
            channelSelectedForStructure.resize(numColumns);
            
            const int numNames = pf->getNumberOfPaintNames();
            areaSelected.resize(numNames);
            std::fill(areaSelected.begin(), areaSelected.end(), true);
         }
         break;
      case PROBABILISTIC_TYPE_VOLUME:
         {
            const int numChannels = brainSet->getNumberOfVolumeProbAtlasFiles();
            channelSelected.resize(numChannels);
            std::fill(channelSelected.begin(), channelSelected.end(), true);
            
            channelSelectedForStructure.resize(numChannels);
            
            int numNames = 0;
            BrainModelVolume* bmv = brainSet->getBrainModelVolume();
            if (bmv != NULL) {
               numNames = bmv->getNumberOfProbAtlasNames();
            }
            areaSelected.resize(numNames);
            std::fill(areaSelected.begin(), areaSelected.end(), true);
         }
         break;
   }
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsProbabilisticAtlas::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   update();
   std::fill(channelSelected.begin(), channelSelected.end(), false);
   std::fill(areaSelected.begin(), areaSelected.end(), false);
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (((sc->getName() == "DisplaySettingsProbabilisticAtlasSurface") &&
           (probType == PROBABILISTIC_TYPE_SURFACE)) ||
          ((sc->getName() == "DisplaySettingsProbabilisticAtlasVolume") &&
           (probType == PROBABILISTIC_TYPE_VOLUME))) {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (sc->getName() == "DisplaySettingsProbabilisticAtlasSurface") {
               ProbabilisticAtlasFile* paf = brainSet->getProbabilisticAtlasSurfaceFile();
               
               if (infoName == "surf-prob-atlas-displayType") {
                  displayType = static_cast<PROBABILISTIC_DISPLAY_TYPE>(si->getValueAsInt());
               }
               else if (infoName == "surf-thresholdDisplayTypeRatio") {
                  si->getValue(thresholdDisplayTypeRatio);
               }
               else if (infoName == "surf-treatQuestColorAsUnassigned") {
                  si->getValue(treatQuestColorAsUnassigned);
               }
               else if (infoName == "surf-channelSelected") {
                  const QString name = si->getModelName();
                  bool nameFound = false;
                  for (int i = 0; i < paf->getNumberOfColumns(); i++) {
                     if (paf->getColumnName(i) == name) {
                        if (i < static_cast<int>(channelSelected.size())) {
                           channelSelected[i] = si->getValueAsBool();
                           nameFound = true;
                        }
                     }
                  }
                  if (nameFound == false) {
                     QString msg("Unable to find prob atlas column named \"");
                     msg.append(name);
                     msg.append("\"\n");
                     errorMessage.append(msg);
                  }
               }
               else if (infoName == "surf-areaSelected") {
                  const QString name = si->getModelName();
                  bool nameFound = false;
                  for (int i = 0; i < paf->getNumberOfPaintNames(); i++) {
                     if (name == paf->getPaintNameFromIndex(i)) {
                        if (i < static_cast<int>(areaSelected.size())) {
                           areaSelected[i] = si->getValueAsBool();
                           nameFound = true;
                        }
                     }
                  }
                  if (nameFound == false) {
                     QString msg("Unable to find prob atlas area named \"");
                     msg.append(name);
                     msg.append("\"\n");
                     errorMessage.append(msg);
                  }
               }
               else if (infoName == "surf-applySelectionToLeftAndRightStructuresFlag") {
                  applySelectionToLeftAndRightStructuresFlag = si->getValueAsBool();
               }
            }
            if (sc->getName() == "DisplaySettingsProbabilisticAtlasVolume") {
               if (infoName == "vol-prob-atlas-displayType") {
                  displayType = static_cast<PROBABILISTIC_DISPLAY_TYPE>(si->getValueAsInt());;
               }
               else if (infoName == "vol-thresholdDisplayTypeRatio") {
                  si->getValue(thresholdDisplayTypeRatio);
               }
               else if (infoName == "vol-treatQuestColorAsUnassigned") {
                  si->getValue(treatQuestColorAsUnassigned);
               }
               else if (infoName == "vol-channelSelected") {
                  const QString name = si->getModelName();
                  bool nameFound = false;
                  for (int i = 0; i < brainSet->getNumberOfVolumeProbAtlasFiles(); i++) {
                     if (FileUtilities::basename(brainSet->getVolumeProbAtlasFile(i)->getDescriptiveLabel()) == name) {
                        if (i < static_cast<int>(channelSelected.size())) {
                           channelSelected[i] = si->getValueAsBool();
                           nameFound = true;
                        }
                     }
                  }
                  if (nameFound == false) {
                     QString msg("Unable to find prob atlas volume named \"");
                     msg.append(name);
                     msg.append("\"\n");
                     errorMessage.append(msg);
                  }
               }
               else if (infoName == "vol-areaSelected") {
                  BrainModelVolume* bmv = brainSet->getBrainModelVolume();
                  if (bmv != NULL) {
                     const QString name = si->getModelName();
                     bool nameFound = false;
                     for (int i = 0; i < bmv->getNumberOfProbAtlasNames(); i++) {
                        if (name == bmv->getProbAtlasNameFromIndex(i)) {
                           if (i < static_cast<int>(areaSelected.size())) {
                              areaSelected[i] = si->getValueAsBool();
                              nameFound = true;
                           }
                        }
                     }
                     if (nameFound == false) {
                        QString msg("Unable to find prob atlas volume area named \"");
                        msg.append(name);
                        msg.append("\"\n");
                        errorMessage.append(msg);
                     }
                  }
               }
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsProbabilisticAtlas::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   SceneFile::SceneClass sc("DisplaySettingsProbabilisticAtlas");
   
   switch (probType) {
      case PROBABILISTIC_TYPE_SURFACE:
         {
            sc.setName("DisplaySettingsProbabilisticAtlasSurface");
            
            ProbabilisticAtlasFile* paf = brainSet->getProbabilisticAtlasSurfaceFile();
            
            if (onlyIfSelected) {
               if (paf->getNumberOfColumns() <= 0) {
                  return;
               }
               
               BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
               if (bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::OVERLAY_PROBABILISTIC_ATLAS) == false) {
                  return;
               }
            }
   
            sc.addSceneInfo(SceneFile::SceneInfo("surf-prob-atlas-displayType",
                                                 displayType));
            sc.addSceneInfo(SceneFile::SceneInfo("surf-thresholdDisplayTypeRatio",
                                                 thresholdDisplayTypeRatio));
            sc.addSceneInfo(SceneFile::SceneInfo("surf-treatQuestColorAsUnassigned",
                                                 treatQuestColorAsUnassigned));
                                                 
            const int numChannels = std::min(paf->getNumberOfColumns(),
                                             static_cast<int>(channelSelected.size()));
            for (int i = 0; i < numChannels; i++) {
               sc.addSceneInfo(SceneFile::SceneInfo("surf-channelSelected",
                         paf->getColumnName(i),
                         channelSelected[i]));
            }
            
            const int numNames = std::min(paf->getNumberOfPaintNames(),
                                          static_cast<int>(areaSelected.size()));
            for (int i = 0; i < numNames; i++) {
               sc.addSceneInfo(SceneFile::SceneInfo("surf-areaSelected",
                                                    paf->getPaintNameFromIndex(i),
                                                    areaSelected[i]));
            }
            
            sc.addSceneInfo(SceneFile::SceneInfo("surf-applySelectionToLeftAndRightStructuresFlag",
                                                 applySelectionToLeftAndRightStructuresFlag));
         }
         break;
      case PROBABILISTIC_TYPE_VOLUME:
         {
            if (onlyIfSelected) {
               if (brainSet->getNumberOfVolumeProbAtlasFiles() <= 0) {
                  return;
               }
               
               BrainModelVolumeVoxelColoring* bmvvc = brainSet->getVoxelColoring();
               if (bmvvc->isUnderlayOrOverlay(
                    BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS) == false) {
                  return;
               }
            }
            sc.setName("DisplaySettingsProbabilisticAtlasVolume");
            sc.addSceneInfo(SceneFile::SceneInfo("vol-prob-atlas-displayType",
                                                 displayType));
            sc.addSceneInfo(SceneFile::SceneInfo("vol-thresholdDisplayTypeRatio",
                                                 thresholdDisplayTypeRatio));
            sc.addSceneInfo(SceneFile::SceneInfo("vol-treatQuestColorAsUnassigned",
                                                 treatQuestColorAsUnassigned));
                                                 
            const int numChannels =
               std::min(brainSet->getNumberOfVolumeProbAtlasFiles(),
                        static_cast<int>(channelSelected.size()));
            for (int i = 0; i < numChannels; i++) {
               const VolumeFile* vf = brainSet->getVolumeProbAtlasFile(i);
               sc.addSceneInfo(SceneFile::SceneInfo("vol-channelSelected",
                               vf->getDescriptiveLabel(),
                               channelSelected[i]));
            }

            int numNames = 0;
            BrainModelVolume* bmv = brainSet->getBrainModelVolume();
            if (bmv != NULL) {
               numNames = bmv->getNumberOfProbAtlasNames();
            }
            numNames = std::min(numNames, 
                                static_cast<int>(areaSelected.size()));
            for (int i = 0; i < numNames; i++) {
               sc.addSceneInfo(SceneFile::SceneInfo("vol-areaSelected",
                                                    bmv->getProbAtlasNameFromIndex(i),
                                                    areaSelected[i]));
            }
         }
         break;
   }
   
   scene.addSceneClass(sc);
}
                       

