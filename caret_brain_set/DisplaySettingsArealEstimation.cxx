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

#include "ArealEstimationFile.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsArealEstimation.h"

#include "FileUtilities.h"

/**
 * The constructor.
 */
DisplaySettingsArealEstimation::DisplaySettingsArealEstimation(BrainSet* bs)
   : DisplaySettingsNodeAttributeFile(bs,
                                      NULL,
                                      bs->getArealEstimationFile(),
                                      BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION,
                                      true,
                                      false)
{
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsArealEstimation::~DisplaySettingsArealEstimation()
{
}

/**
 * Reinitialize all display settings
 */
void
DisplaySettingsArealEstimation::reset()
{
   DisplaySettingsNodeAttributeFile::reset();
}

/**
 * Update any selections due to changes in loaded areal estimation file
 */
void
DisplaySettingsArealEstimation::update()
{
   DisplaySettingsNodeAttributeFile::update();
}


static const QString arealEstimationID("areal-estimation-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsArealEstimation::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsArealEstimation") {
         showSceneSelectedColumns(*sc,
                                  "Areal Estimation File",
                                  arealEstimationID,
                                  "",
                                  errorMessage);
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsArealEstimation::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   ArealEstimationFile* aef = brainSet->getArealEstimationFile();
   if (onlyIfSelected) {
      if (aef->getNumberOfColumns() <= 0) {
         return;
      }
      
      if (brainSet->isASurfaceOverlayForAnySurface(
                BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION) == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsArealEstimation");
   
   saveSceneSelectedColumns(sc);

   scene.addSceneClass(sc);
}
