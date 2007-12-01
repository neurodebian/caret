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
   : DisplaySettings(bs)
{
   medialWallOverrideColumn = -1;
   medialWallOverrideEnabled = false;
   applySelectionToLeftAndRightStructuresFlag = false;
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
   selectedColumn.clear();
}

/**
 * Update any selections du to changes in loaded paint file.
 */
void
DisplaySettingsPaint::update()
{
   updateSelectedColumnIndices(brainSet->getPaintFile(), selectedColumn);
   updateSelectedColumnIndex(brainSet->getPaintFile(), medialWallOverrideColumn);
}

/**
 * Get the column selected for display.
 * Returns -1 if there are no paint columns.
 */
int
DisplaySettingsPaint::getSelectedColumn(const int modelIn) const
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
 * set column for display.
 */
void 
DisplaySettingsPaint::setSelectedColumn(const int model, const int sdc) 
{ 
   if (applySelectionToLeftAndRightStructuresFlag) {
      PaintFile* pf = brainSet->getPaintFile();
      if ((sdc >= 0) && (sdc < pf->getNumberOfColumns())) {
         int leftCol = -1;
         int rightCol = -1;
         QString name = pf->getColumnName(sdc).toLower().trimmed();
         if (name.indexOf("left") >= 0) {
            leftCol = sdc;
            const QString rightName = name.replace("left", "right");
            for (int i = 0; i < pf->getNumberOfColumns(); i++) {
               if (pf->getColumnName(i).toLower().trimmed() == rightName) {
                  rightCol = i;
                  break;
               }
            }
         }
         else if (name.indexOf("right") >= 0) {
            rightCol = sdc;
            const QString leftName = name.replace("right", "left");
            for (int i = 0; i < pf->getNumberOfColumns(); i++) {
               if (pf->getColumnName(i).toLower().trimmed() == leftName) {
                  leftCol = i;
                  break;
               }
            }
         }
         
         for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
            const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
            if (bms != NULL) {
               switch (bms->getStructure().getType()) {
                  case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                     if (leftCol >= 0) {
                        selectedColumn[i] = leftCol;
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                     if (rightCol >= 0) {
                        selectedColumn[i] = rightCol;
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                     break;
                  case Structure::STRUCTURE_TYPE_CEREBELLUM:
                     break;
                  case Structure::STRUCTURE_TYPE_INVALID:
                     break;
               }
            }
         }
      }
   }
   else {
      if (model < 0) {
         std::fill(selectedColumn.begin(), selectedColumn.end(), sdc);
      }
      else {
         selectedColumn[model] = sdc; 
      }
   }
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
         showSceneNodeAttribute(*sc,
                                paintColumnID,
                                pf,
                                "Paint File",
                                selectedColumn,
                                errorMessage);

         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "medialWallOverrideEnabled") {
               si->getValue(medialWallOverrideEnabled);
            }
            else if (infoName == "applySelectionToLeftAndRightStructuresFlag") {
               applySelectionToLeftAndRightStructuresFlag = si->getValueAsBool();
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
DisplaySettingsPaint::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   PaintFile* pf = brainSet->getPaintFile();
   
   if (onlyIfSelected) {
      if (pf->getNumberOfColumns() <= 0) {
         return;
      }
      
      BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
      if (bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::OVERLAY_PAINT) == false) {
         return;
      }
   }
   SceneFile::SceneClass sc("DisplaySettingsPaint");
   
   saveSceneNodeAttribute(sc,
                          paintColumnID,
                          pf,
                          selectedColumn);
                          
   sc.addSceneInfo(SceneFile::SceneInfo("medialWallOverrideEnabled",
                                        medialWallOverrideEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("applySelectionToLeftAndRightStructuresFlag",
                                        applySelectionToLeftAndRightStructuresFlag));
                                        
   if ((medialWallOverrideColumn >= 0) && 
       (medialWallOverrideColumn < pf->getNumberOfColumns())) {
      sc.addSceneInfo(SceneFile::SceneInfo(paintMedWallColumnID,
                                           pf->getColumnName(medialWallOverrideColumn)));
   }
   
   scene.addSceneClass(sc);
}
                       
/**
 * for node attribute files - all column selections for each surface are the same.
 */
/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsPaint::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (selectedColumn[bm1] == selectedColumn[bm2]);
}      


