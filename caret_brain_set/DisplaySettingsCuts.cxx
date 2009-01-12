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



#include "DisplaySettingsCuts.h"
#include "BrainSet.h"

/**
 * Constructor.
 */
DisplaySettingsCuts::DisplaySettingsCuts(BrainSet* bs)
   : DisplaySettings(bs)
{
   reset();
}

/**
 * Destructor.
 */
DisplaySettingsCuts::~DisplaySettingsCuts()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsCuts::reset()
{
   displayCuts = false;
}

/**
 * Update selections due to changes in loaded cuts.
 */
void
DisplaySettingsCuts::update()
{
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsCuts::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsCuts") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "displayCuts") {
               si->getValue(displayCuts);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsCuts::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   if (onlyIfSelected) {
      if (displayCuts == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsCuts");
   
   sc.addSceneInfo(SceneFile::SceneInfo("displayCuts",
                                        displayCuts));
   
   scene.addSceneClass(sc);
}
                       
