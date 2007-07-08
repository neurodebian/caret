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
   : DisplaySettings(bs)
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
   selectedColumn.clear();
}

/**
 * Update any selections due to changes in loaded areal estimation file
 */
void
DisplaySettingsArealEstimation::update()
{
   updateSelectedColumnIndices(brainSet->getArealEstimationFile(), selectedColumn);   
}

/**
 * Get the selected column.
 */
int 
DisplaySettingsArealEstimation::getSelectedColumn(const int modelIn) const 
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
DisplaySettingsArealEstimation::setSelectedColumn(const int model, const int col) 
{ 
   if (model < 0) {
      std::fill(selectedColumn.begin(), selectedColumn.end(), col);
   }
   else {
      selectedColumn[model] = col; 
   }
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
         showSceneNodeAttribute(*sc,
                                arealEstimationID,
                                brainSet->getArealEstimationFile(),
                                "Areal Estimation File",
                                selectedColumn,
                                errorMessage);
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsArealEstimation::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   ArealEstimationFile* aef = brainSet->getArealEstimationFile();
   if (onlyIfSelected) {
      if (aef->getNumberOfColumns() <= 0) {
         return;
      }
      
      BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
      if (bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::UNDERLAY_AREAL_ESTIMATION,
                                    BrainModelSurfaceNodeColoring::OVERLAY_AREAL_ESTIMATION) == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsArealEstimation");
   
   saveSceneNodeAttribute(sc,
                          arealEstimationID,
                          aef,
                          selectedColumn);

   scene.addSceneClass(sc);
}

/**
 * for node attribute files - all column selections for each surface are the same.
 */
/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsArealEstimation::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (selectedColumn[bm1] == selectedColumn[bm2]);
}      

