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


#include "DisplaySettingsFoci.h"
#include "BrainSet.h"

/**
 * Constructor.
 */
DisplaySettingsFoci::DisplaySettingsFoci(BrainSet* bs) :
   DisplaySettingsCells(bs)
{
   reset();
}

/**
 * Destructor.
 */
DisplaySettingsFoci::~DisplaySettingsFoci()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsFoci::reset()
{
   DisplaySettingsCells::reset();
}

/**
 * Update selections due to changes in loaded foci.
 */
void
DisplaySettingsFoci::update()
{
   determineDisplayedFoci();
}

/**
 * Determine the foci that are displayed.
 */
void
DisplaySettingsFoci::determineDisplayedFoci()
{
   DisplaySettingsCells::determineDisplayedCells(true);
}

/**
 * apply a scene (set display settings).
 */
/*
void 
DisplaySettingsFoci::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsFoci") {
      }
   }
}
*/

/**
 * create a scene (read display settings).
 */
/*
void 
DisplaySettingsFoci::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   SceneFile::SceneClass sc("DisplaySettingsFoci");
   
   scene.addSceneClass(sc);
}
*/

