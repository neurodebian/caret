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
#include "DisplaySettingsRgbPaint.h"
#include "RgbPaintFile.h"

/**
 * The constructor.
 */
DisplaySettingsRgbPaint::DisplaySettingsRgbPaint(BrainSet* bs)
   : DisplaySettingsNodeAttributeFile(bs,
                                      NULL,
                                      bs->getRgbPaintFile(),
                                      BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT,
                                      true,
                                      false)
{
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsRgbPaint::~DisplaySettingsRgbPaint()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsRgbPaint::reset()
{
   DisplaySettingsNodeAttributeFile::reset();
   displayMode    = RGB_DISPLAY_MODE_POSITIVE;
   redThreshold   = 0.0;
   greenThreshold = 0.0;
   blueThreshold  = 0.0;
   redEnabled     = true;
   greenEnabled   = true;
   blueEnabled    = true;
}

/**
 * Update any selections due to changes in loaded rgb paint file.
 */
void
DisplaySettingsRgbPaint::update()
{
   DisplaySettingsNodeAttributeFile::update();
}

/**
 * Get the red, green, and blue thresholds
 */
void
DisplaySettingsRgbPaint::getThresholds(float& redThresh, float& greenThresh, 
                                       float& blueThresh) const
{
   redThresh   = redThreshold;
   greenThresh = greenThreshold;
   blueThresh  = blueThreshold;
}

/**
 * Get the red, green, and blue thresholds
 */
void
DisplaySettingsRgbPaint::setThresholds(const float redThresh, 
                                       const float greenThresh, 
                                       const float blueThresh)
{
   redThreshold   = redThresh;
   greenThreshold = greenThresh;
   blueThreshold  = blueThresh;
}

static const QString rgbPaintSurfaceID("surface-rgb-paint-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsRgbPaint::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   DisplaySettingsNodeAttributeFile::showScene(scene, errorMessage);

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsRgbPaint") {
         showSceneSelectedColumns(*sc,
                                  "RGB Paint File",
                                  rgbPaintSurfaceID,
                                  "",
                                  errorMessage);
                                
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "rgb-displayMode") {
               int val;
               si->getValue(val);
               displayMode = static_cast<RGB_DISPLAY_MODE>(val);
            }
            else if (infoName == "redThreshold") {
               si->getValue(redThreshold);
            }
            else if (infoName == "greenThreshold") {
               si->getValue(greenThreshold);
            }
            else if (infoName == "blueThreshold") {
               si->getValue(blueThreshold);
            }
            else if (infoName == "redEnabled") {
               si->getValue(redEnabled);
            }
            else if (infoName == "greenEnabled") {
               si->getValue(greenEnabled);
            }
            else if (infoName == "blueEnabled") {
               si->getValue(blueEnabled);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsRgbPaint::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage)
{
   DisplaySettingsNodeAttributeFile::saveScene(scene, onlyIfSelected, errorMessage);

   RgbPaintFile* rpf = brainSet->getRgbPaintFile();
   
   if (onlyIfSelected) {
      if (rpf->getNumberOfColumns() <= 0) {
         return;
      }
      
      if (brainSet->isASurfaceOverlayForAnySurface(
                        BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT) == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsRgbPaint");
   
   saveSceneSelectedColumns(sc);
                          
   sc.addSceneInfo(SceneFile::SceneInfo("rgb-displayMode",
                                        displayMode));
   sc.addSceneInfo(SceneFile::SceneInfo("redThreshold",
                                        redThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("greenThreshold",
                                        greenThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("blueThreshold",
                                        blueThreshold));
   sc.addSceneInfo(SceneFile::SceneInfo("redEnabled",
                                        redEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("greenEnabled",
                                        greenEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("blueEnabled",
                                        blueEnabled));
   
   scene.addSceneClass(sc);
}
