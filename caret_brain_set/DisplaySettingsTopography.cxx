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
#include "BrainSet.h"
#include "DisplaySettingsTopography.h"
#include "TopographyFile.h"

/**
 * The constructor.
 */
DisplaySettingsTopography::DisplaySettingsTopography(BrainSet* bs)
   : DisplaySettingsNodeAttributeFile(bs,
                                      NULL,
                                      bs->getTopographyFile(),
                                      BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY,
                                      true,
                                      false)
{
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsTopography::~DisplaySettingsTopography()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsTopography::reset()
{
   DisplaySettingsNodeAttributeFile::reset();
   displayType = TOPOGRAPHY_DISPLAY_ECCENTRICITY;
}

/**
 * Update selections due to changes in loaded topography files.
 */
void
DisplaySettingsTopography::update()
{
   DisplaySettingsNodeAttributeFile::update();
}

static const QString topographyID("topography-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsTopography::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsTopography") {
         showSceneSelectedColumns(*sc,
                                  "Topograrphy File",
                                  topographyID,
                                  "",
                                  errorMessage);
                                
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "topography-displayType") {
               int val;
               si->getValue(val);
               displayType = static_cast<TOPOGRAPHY_DISPLAY_TYPE>(val);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsTopography::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   TopographyFile* tf = brainSet->getTopographyFile();
   if (onlyIfSelected) {
      if (tf->getNumberOfColumns() <= 0) {
         return;
      }
      
      if (brainSet->isASurfaceOverlayForAnySurface(
                     BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY) == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsTopography");
   
   saveSceneSelectedColumns(sc);

   sc.addSceneInfo(SceneFile::SceneInfo("topography-displayType",
                                        displayType));
   
   scene.addSceneClass(sc);
}

