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



#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsCoCoMac.h"
#include "PaintFile.h"

/**
 * The constructor.
 */
DisplaySettingsCoCoMac::DisplaySettingsCoCoMac(BrainSet* bs)
   : DisplaySettings(bs)
{
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsCoCoMac::~DisplaySettingsCoCoMac()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsCoCoMac::reset()
{
   selectedPaintColumn = 0;
   connectionDisplayType = CONNECTION_DISPLAY_AFFERENT;
   selectedNode = -1;
}

/**
 * Update selections due to changes in loaded topography files.
 */
void
DisplaySettingsCoCoMac::update()
{
   PaintFile* pf = brainSet->getPaintFile();
   if (selectedPaintColumn >= pf->getNumberOfColumns()) {
      selectedPaintColumn = 0;
   }
   
}

static const QString cocomacPaintID("cocomac-paint-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsCoCoMac::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsCoCoMac") {
         PaintFile* pf = brainSet->getPaintFile();
         
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == cocomacPaintID) {
               showSceneNodeAttributeColumn(si,
                                            pf,
                                            "Paint File",
                                            selectedPaintColumn,
                                            errorMessage);
            }
            else if (infoName == "connectionDisplayType") {
               int val;
               si->getValue(val);
               connectionDisplayType = static_cast<CONNECTION_DISPLAY_TYPE>(val);
            }
            else if (infoName == "selectedNode") {
               si->getValue(selectedNode);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsCoCoMac::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   if (onlyIfSelected) {
      BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
      if (bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::UNDERLAY_COCOMAC,
                                    BrainModelSurfaceNodeColoring::OVERLAY_COCOMAC) == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsCoCoMac");
   
   PaintFile* pf = brainSet->getPaintFile();
   
   saveSceneNodeAttributeColumn(sc,
                                cocomacPaintID,
                                pf,
                                selectedPaintColumn);
   sc.addSceneInfo(SceneFile::SceneInfo("connectionDisplayType",
                                        connectionDisplayType));
   sc.addSceneInfo(SceneFile::SceneInfo("selectedNode",
                                        selectedNode));
   scene.addSceneClass(sc);
}
                       

