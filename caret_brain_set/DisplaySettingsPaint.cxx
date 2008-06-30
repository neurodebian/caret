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
#include "DisplaySettingsPaint.h"
#include "PaintFile.h"

/**
 * The constructor.
 */
DisplaySettingsPaint::DisplaySettingsPaint(BrainSet* bs)
   : DisplaySettingsNodeAttributeFile(bs,
                                      bs->getPaintFile(),
                                      NULL,
                                      BrainModelSurfaceOverlay::OVERLAY_PAINT,
                                      true,
                                      false)
{
   medialWallOverrideColumn = -1;
   medialWallOverrideEnabled = false;
   geographyBlending = 0.6;
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsPaint::~DisplaySettingsPaint()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsPaint::reset()
{
   DisplaySettingsNodeAttributeFile::reset();
}

/**
 * Update any selections du to changes in loaded paint file.
 */
void
DisplaySettingsPaint::update()
{
   DisplaySettingsNodeAttributeFile::update();
   updateSelectedColumnIndex(brainSet->getPaintFile(), medialWallOverrideColumn);
}

static const QString paintColumnID("paint-column");
static const QString paintMedWallColumnID("paint-med-wall-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsPaint::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   PaintFile* pf = brainSet->getPaintFile();
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsPaint") {
         showSceneSelectedColumns(*sc,
                                  "Paint File",
                                  paintColumnID,
                                  "",
                                  errorMessage);

         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "medialWallOverrideEnabled") {
               si->getValue(medialWallOverrideEnabled);
            }
            else if (infoName == "geographyBlending") {
               si->getValue(geographyBlending);
            }
            else if (infoName == paintMedWallColumnID) {
               const QString columnName = si->getValueAsString();
               for (int i = 0; i < pf->getNumberOfColumns(); i++) {
                  if (columnName == pf->getColumnName(i)) {
                     medialWallOverrideColumn = i;
                     break;
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
DisplaySettingsPaint::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   PaintFile* pf = brainSet->getPaintFile();
   
   if (onlyIfSelected) {
      if (pf->getNumberOfColumns() <= 0) {
         return;
      }
      
      if (brainSet->isASurfaceOverlayForAnySurface(
                           BrainModelSurfaceOverlay::OVERLAY_PAINT) == false) {
         return;
      }
   }
   SceneFile::SceneClass sc("DisplaySettingsPaint");
   
   saveSceneSelectedColumns(sc);
                          
   sc.addSceneInfo(SceneFile::SceneInfo("medialWallOverrideEnabled",
                                        medialWallOverrideEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("geographyBlending",
                                        geographyBlending));
                                        
   if ((medialWallOverrideColumn >= 0) && 
       (medialWallOverrideColumn < pf->getNumberOfColumns())) {
      sc.addSceneInfo(SceneFile::SceneInfo(paintMedWallColumnID,
                                           pf->getColumnName(medialWallOverrideColumn)));
   }
   
   scene.addSceneClass(sc);
}
                       
