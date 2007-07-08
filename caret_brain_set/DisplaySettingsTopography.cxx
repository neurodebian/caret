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
   : DisplaySettings(bs)
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
   selectedColumn.clear();
   displayType = TOPOGRAPHY_DISPLAY_ECCENTRICITY;
}

/**
 * Update selections due to changes in loaded topography files.
 */
void
DisplaySettingsTopography::update()
{
   updateSelectedColumnIndices(brainSet->getTopographyFile(), selectedColumn);
}

/**
 * Get the selected column.
 */
int 
DisplaySettingsTopography::getSelectedColumn(const int modelIn) const 
{ 
   if (selectedColumn.empty()) {
      return -1;
   }
   
   int model = modelIn;
   if (model < 0) {
      model = 0;
   }
   
   return selectedColumn[model]; 
}

/**
 * Set the selected file index.
 */
void 
DisplaySettingsTopography::setSelectedColumn(const int model, const int col) 
{ 
   if (model < 0) {
      std::fill(selectedColumn.begin(), selectedColumn.end(), col);
   }
   else {
      selectedColumn[model] = col; 
   }
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
         showSceneNodeAttribute(*sc,
                                topographyID,
                                brainSet->getTopographyFile(),
                                "Topography File",
                                selectedColumn,
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
DisplaySettingsTopography::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   TopographyFile* tf = brainSet->getTopographyFile();
   if (onlyIfSelected) {
      if (tf->getNumberOfColumns() <= 0) {
         return;
      }
      
      BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
      if (bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::UNDERLAY_TOPOGRAPHY,
                                    BrainModelSurfaceNodeColoring::OVERLAY_TOPOGRAPHY) == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsTopography");
   
   saveSceneNodeAttribute(sc,
                          topographyID,
                          tf,
                          selectedColumn);

   sc.addSceneInfo(SceneFile::SceneInfo("topography-displayType",
                                        displayType));
   
   scene.addSceneClass(sc);
}

/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsTopography::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (selectedColumn[bm1] == selectedColumn[bm2]);
}      

                       

