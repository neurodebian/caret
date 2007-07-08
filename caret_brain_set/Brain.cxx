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

#include "Brain.h"
#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BrainStructure.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CocomacConnectivityFile.h"
#include "ContourCellColorFile.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsContours.h"
#include "DisplaySettingsCuts.h"
#include "DisplaySettingsCoCoMac.h"
#include "DisplaySettingsDeformationField.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsGeodesicDistance.h"
#include "DisplaySettingsImages.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsModels.h"
#include "DisplaySettingsSurface.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsScene.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsSurfaceVectors.h"
#include "DisplaySettingsTopography.h"
#include "DisplaySettingsVolume.h"
#include "DisplaySettingsWustlRegion.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "PaletteFile.h"
#include "ParamsFile.h"
#include "SceneFile.h"
#include "TransformationMatrixFile.h"
#include "WustlRegionFile.h"

/**
 * constructor.
 */
Brain::Brain()
{
   areaColorFile          = new AreaColorFile;
   borderColorFile        = new BorderColorFile;
   cellColorFile          = new CellColorFile;
   cocomacFile            = new CocomacConnectivityFile;
   contourCellColorFile   = new ContourCellColorFile;
   fociColorFile          = new FociColorFile;
   paletteFile            = new PaletteFile;
   paramsFile             = new ParamsFile;
   sceneFile              = new SceneFile;
   transformationMatrixFile = new TransformationMatrixFile;
   volumeCellFile         = new CellFile;
   volumeFociFile         = new FociFile;
   wustlRegionFile        = new WustlRegionFile;

   displaySettingsArealEstimation = new DisplaySettingsArealEstimation(this);
   displaySettingsBorders         = new DisplaySettingsBorders(this);
   displaySettingsContours	  = new DisplaySettingsContours(this);
   displaySettingsDeformationField = new DisplaySettingsDeformationField(this);
   displaySettingsGeodesicDistance = new DisplaySettingsGeodesicDistance(this);
   displaySettingsImages          = new DisplaySettingsImages(this);
   displaySettingsMetric          = new DisplaySettingsMetric(this);
   displaySettingsModels          = new DisplaySettingsModels(this);
   displaySettingsSurface         = new DisplaySettingsSurface(this);
   displaySettingsPaint           = new DisplaySettingsPaint(this);
   displaySettingsProbabilisticAtlasSurface = new DisplaySettingsProbabilisticAtlas(this,
                                 DisplaySettingsProbabilisticAtlas::PROBABILISTIC_TYPE_SURFACE);
   displaySettingsRgbPaint        = new DisplaySettingsRgbPaint(this);
   displaySettingsScene           = new DisplaySettingsScene(this);
   displaySettingsSurfaceShape    = new DisplaySettingsSurfaceShape(this);
   displaySettingsSurfaceVectors  = new DisplaySettingsSurfaceVectors(this);
   displaySettingsCells           = new DisplaySettingsCells(this);
   displaySettingsCoCoMac         = new DisplaySettingsCoCoMac(this);
   displaySettingsCuts            = new DisplaySettingsCuts(this);
   displaySettingsFoci            = new DisplaySettingsFoci(this);
   displaySettingsTopography      = new DisplaySettingsTopography(this);
   displaySettingsVolume          = new DisplaySettingsVolume(this);
   displaySettingsWustlRegion     = new DisplaySettingsWustlRegion(this);
   displaySettingsProbabilisticAtlasVolume = new DisplaySettingsProbabilisticAtlas(this,
                                 DisplaySettingsProbabilisticAtlas::PROBABILISTIC_TYPE_VOLUME);

   resetDataFiles();
}

/**
 * destructor.
 */
Brain::~Brain()
{
   for (unsigned int i = 0; i < brainStructures.size(); i++) {
      if (brainStructureDeleteFlag[i]) {
         delete brainStructures[i];
         brainStructures[i] = NULL;
      }
   }
   delete areaColorFile;
   delete borderColorFile;
   delete cellColorFile;
   delete cocomacFile;
   delete contourCellColorFile;
   delete fociColorFile;
   delete paletteFile;
   delete paramsFile;
   delete sceneFile;
   delete transformationMatrixFile;
   delete volumeCellFile;
   delete volumeFociFile;
   delete wustlRegionFile;

   delete displaySettingsArealEstimation;  
   delete displaySettingsBorders;
   delete displaySettingsDeformationField;
   delete displaySettingsGeodesicDistance;
   delete displaySettingsImages;
   delete displaySettingsMetric; 
   delete displaySettingsModels;
   delete displaySettingsSurface;
   delete displaySettingsPaint;
   delete displaySettingsProbabilisticAtlasSurface;
   delete displaySettingsRgbPaint;  
   delete displaySettingsScene;
   delete displaySettingsSurfaceShape;  
   delete displaySettingsSurfaceVectors;
   delete displaySettingsCells;
   delete displaySettingsCoCoMac;
   delete displaySettingsContours;
   delete displaySettingsCuts;
   delete displaySettingsFoci;
   delete displaySettingsTopography;
   delete displaySettingsVolume;
   delete displaySettingsProbabilisticAtlasVolume;
   delete displaySettingsWustlRegion;
}

/**
 * add a brain structure.
 */
void 
Brain::addBrainStructure(BrainStructure* bs, const bool deleteBrainStructureFlag)
{
   brainStructures.push_back(bs);
   brainStructureDeleteFlag.push_back(deleteBrainStructureFlag);
}

/**
 * Reset the display settings.
 */
void
BrainStructure::resetDisplaySettings(const bool keepSceneData)
{
   displaySettingsArealEstimation->reset();  
   displaySettingsBorders->reset();
   displaySettingsDeformationField->reset();
   displaySettingsGeodesicDistance->reset();
   displaySettingsImages->reset();
   displaySettingsMetric->reset(); 
   displaySettingsModels->reset();
   displaySettingsSurface->reset();
   displaySettingsPaint->reset();
   displaySettingsProbabilisticAtlasSurface->reset();
   displaySettingsRgbPaint->reset();  
   if (keepSceneData == false) {
      displaySettingsScene->reset();
   }
   displaySettingsSurfaceShape->reset();  
   displaySettingsSurfaceVectors->reset();
   displaySettingsCells->reset();
   displaySettingsCoCoMac->reset();
   displaySettingsContours->reset();
   displaySettingsCuts->reset();
   displaySettingsFoci->reset();
   displaySettingsTopography->reset();
   displaySettingsVolume->reset();
   displaySettingsProbabilisticAtlasVolume->reset();
   displaySettingsWustlRegion->reset();
}

/**
 * reset the files.
 */
void 
Brain::resetDataFiles(const bool keepSceneData)
{
   areaColorFile->clear();
   borderColorFile->clear();
   cellColorFile->clear();
   cocomacFile->clear();
   contourCellColorFile->clear();
   fociColorFile->clear();
   paletteFile->clear();
   paletteFile->addDefaultPalettes();
   paletteFile->clearModified();
   paramsFile->clear();
   if (keepSceneData == false) {
      sceneFile->clear();
   }
   transformationMatrixFile->clear();
   volumeCellFile->clear();
   volumeFociFile->clear();
   wustlRegionFile->clear();
}

/**
 * Setup scene for display settings.
 */
void 
Brain::displaySettingsShowScene(const SceneFile::Scene* ss,
                                std::string& errorMessage)
{
      displaySettingsArealEstimation->showScene(*ss, errorMessage);
      displaySettingsBorders->showScene(*ss, errorMessage);
      displaySettingsCells->showScene(*ss, errorMessage);
      displaySettingsCoCoMac->showScene(*ss, errorMessage);
      displaySettingsContours->showScene(*ss, errorMessage);
      displaySettingsCuts->showScene(*ss, errorMessage);
      displaySettingsFoci->showScene(*ss, errorMessage);
      displaySettingsGeodesicDistance->showScene(*ss, errorMessage);
      displaySettingsDeformationField->showScene(*ss, errorMessage);
      displaySettingsImages->showScene(*ss, errorMessage);
      displaySettingsMetric->showScene(*ss, errorMessage);
      displaySettingsModels->showScene(*ss, errorMessage);
      displaySettingsPaint->showScene(*ss, errorMessage);
      displaySettingsProbabilisticAtlasSurface->showScene(*ss, errorMessage);
      displaySettingsProbabilisticAtlasVolume->showScene(*ss, errorMessage);
      displaySettingsRgbPaint->showScene(*ss, errorMessage);
      displaySettingsScene->showScene(*ss, errorMessage);
      displaySettingsSurface->showScene(*ss, errorMessage);
      displaySettingsSurfaceShape->showScene(*ss, errorMessage);
      displaySettingsSurfaceVectors->showScene(*ss, errorMessage);
      displaySettingsTopography->showScene(*ss, errorMessage);
      displaySettingsVolume->showScene(*ss, errorMessage);
      displaySettingsWustlRegion->showScene(*ss, errorMessage);
      
      displaySettingsBorders->determineDisplayedBorders();
      displaySettingsCells->determineDisplayedCells();
      displaySettingsFoci->determineDisplayedCells(true);
}

/**
 * save scene for display settings.
 */
void 
Brain::displaySettingsSaveScene(SceneFile::Scene& scene,
                                const bool onlyIfSelectedFlag)
{
   displaySettingsArealEstimation->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsBorders->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsCells->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsCoCoMac->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsContours->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsCuts->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsFoci->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsGeodesicDistance->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsDeformationField->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsImages->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsMetric->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsModels->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsPaint->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsProbabilisticAtlasSurface->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsProbabilisticAtlasVolume->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsRgbPaint->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsScene->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsSurface->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsSurfaceShape->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsSurfaceVectors->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsTopography->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsVolume->saveScene(scene, onlyIfSelectedFlag);
   displaySettingsWustlRegion->saveScene(scene, onlyIfSelectedFlag);
}

/**
 * Update all display settings.
 */
void
Brain::updateAllDisplaySettings()
{
   displaySettingsArealEstimation->update();  
   displaySettingsBorders->update();
   displaySettingsCells->update();
   displaySettingsCoCoMac->update();
   displaySettingsContours->update();
   displaySettingsCuts->update();
   displaySettingsFoci->update();
   displaySettingsDeformationField->update();
   displaySettingsMetric->update(); 
   displaySettingsPaint->update();
   displaySettingsProbabilisticAtlasSurface->update();
   displaySettingsRgbPaint->update();  
   displaySettingsScene->update();
   displaySettingsSurfaceShape->update();
   displaySettingsSurfaceVectors->update();  
   displaySettingsTopography->update();
   displaySettingsVolume->update();
   displaySettingsProbabilisticAtlasVolume->update();
   displaySettingsWustlRegion->update();
}
