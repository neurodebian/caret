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
#include "DisplaySettingsSurfaceVectors.h"
#include "FileUtilities.h"
#include "SurfaceVectorFile.h"

/**
 * The constructor.
 */
DisplaySettingsSurfaceVectors::DisplaySettingsSurfaceVectors(BrainSet* bs)
   : DisplaySettings(bs)
{
   lengthMultiplier = 1.0;
   sparseDistance = 50;
   displayMode = DISPLAY_MODE_NONE;
   
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsSurfaceVectors::~DisplaySettingsSurfaceVectors()
{
}

/**
 * Reinitialize all display settings
 */
void
DisplaySettingsSurfaceVectors::reset()
{
   selectedColumn.clear();
   displayVectorForNode.clear();
   updateDisplayedVectors();
}

/**
 * Update any selections due to changes in loaded areal estimation file
 */
void
DisplaySettingsSurfaceVectors::update()
{
   updateSelectedColumnIndices(brainSet->getSurfaceVectorFile(), selectedColumn);      
   updateDisplayedVectors();
}

/**
 * set the length multiplier.
 */
void 
DisplaySettingsSurfaceVectors::setLengthMultiplier(const float len)
{
   lengthMultiplier = len;
   updateDisplayedVectors();
}
      
/**
 * Get the selected column.
 */
int 
DisplaySettingsSurfaceVectors::getSelectedColumn(const int modelIn) const 
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
DisplaySettingsSurfaceVectors::setSelectedColumn(const int model, const int col) 
{ 
   if (model < 0) {
      std::fill(selectedColumn.begin(), selectedColumn.end(), col);
   }
   else {
      selectedColumn[model] = col; 
   }
   updateDisplayedVectors();
}

/**
 * set the display mode.
 */ 
void 
DisplaySettingsSurfaceVectors::setDisplayMode(const DISPLAY_MODE dm)
{ 
   displayMode = dm; 
   updateDisplayedVectors();
}

// update the displayed vectors
void 
DisplaySettingsSurfaceVectors::updateDisplayedVectors()
{
   const int numNodes = brainSet->getNumberOfNodes();
   displayVectorForNode.resize(numNodes, false);
   switch (displayMode) {
      case DISPLAY_MODE_ALL:
         for (int i = 0; i < numNodes; i++) {
            displayVectorForNode[i] = true;
         }
         break;
      case DISPLAY_MODE_NONE:
         for (int i = 0; i < numNodes; i++) {
            displayVectorForNode[i] = false;
         }
         break;
      case DISPLAY_MODE_SPARSE:
         for (int i = 0; i < numNodes; i++) {
            displayVectorForNode[i] = false;
         }
         for (int i = 0; i < numNodes; i += sparseDistance) {
            displayVectorForNode[i] = true;
         }
         break;
   }
}

/**
 * set the sparse distance.
 */
void 
DisplaySettingsSurfaceVectors::setSparseDisplayDistance(const int dist) 
{ 
   sparseDistance = dist; 
   updateDisplayedVectors();
}

static const QString surfaceVectorID("surface-vector-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsSurfaceVectors::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   SurfaceVectorFile* svf = brainSet->getSurfaceVectorFile();
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsSurfaceVectors") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();    
            if (infoName == surfaceVectorID) {
               showSceneNodeAttribute(*sc,
                                      surfaceVectorID,
                                      svf,
                                      "Surface Vector File",
                                      selectedColumn,
                                      errorMessage);
            }
            else if (infoName == "displayMode") {
               displayMode = static_cast<DISPLAY_MODE>(si->getValueAsInt());
            }
            else if (infoName == "sparseDistance") {
               si->getValue(sparseDistance);
            }
         }
      }
   }
   
   updateDisplayedVectors();
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsSurfaceVectors::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   SurfaceVectorFile* svf = brainSet->getSurfaceVectorFile();
   if (onlyIfSelected) {
      if (svf->getNumberOfColumns() <= 0) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsSurfaceVectors");
   
   saveSceneNodeAttribute(sc,
                          surfaceVectorID,
                          svf,
                          selectedColumn);

   sc.addSceneInfo(SceneFile::SceneInfo("displayMode", displayMode));
   
   sc.addSceneInfo(SceneFile::SceneInfo("sparseDistance",
                                        sparseDistance));

   scene.addSceneClass(sc);
}

/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsSurfaceVectors::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (selectedColumn[bm1] == selectedColumn[bm2]);
}      

/**
 * get display vector for node.
 */
bool
DisplaySettingsSurfaceVectors::getDisplayVectorForNode(const int nodeNum) const
{
   if (nodeNum < static_cast<int>(displayVectorForNode.size())) {
      return displayVectorForNode[nodeNum];
   }
   return false;
}

/**
 * set display vector for node.
 */
void
DisplaySettingsSurfaceVectors::setDisplayVectorForNode(const int nodeNum,
                                                         const bool status)
{
   if (nodeNum < static_cast<int>(displayVectorForNode.size())) {
      displayVectorForNode[nodeNum] = status;
   }
}

