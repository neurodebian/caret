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
   : DisplaySettings(bs)
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
   selectedColumn.clear();
   displayMode    = RGB_DISPLAY_MODE_POSITIVE;
   redThreshold   = 0.0;
   greenThreshold = 0.0;
   blueThreshold  = 0.0;
   redEnabled     = true;
   greenEnabled   = true;
   blueEnabled    = true;
   applySelectionToLeftAndRightStructuresFlag = false;
}

/**
 * Update any selections due to changes in loaded rgb paint file.
 */
void
DisplaySettingsRgbPaint::update()
{
   updateSelectedColumnIndices(brainSet->getRgbPaintFile(), selectedColumn);
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

/**
 * Get the column selected for display.
 * Returns -1 if there are no paint columns.
 */
int
DisplaySettingsRgbPaint::getSelectedColumn(const int modelIn) const
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
DisplaySettingsRgbPaint::setSelectedColumn(const int model, const int sdc) 
{ 
   if (applySelectionToLeftAndRightStructuresFlag) {
      RgbPaintFile* rgb = brainSet->getRgbPaintFile();
      if ((sdc >= 0) && (sdc < rgb->getNumberOfColumns())) {
         int leftCol = -1;
         int rightCol = -1;
         QString name = rgb->getColumnName(sdc).toLower().trimmed();
         if (name.indexOf("left") >= 0) {
            leftCol = sdc;
            const QString rightName = name.replace("left", "right");
            for (int i = 0; i < rgb->getNumberOfColumns(); i++) {
               if (rgb->getColumnName(i).toLower().trimmed() == rightName) {
                  rightCol = i;
                  break;
               }
            }
         }
         else if (name.indexOf("right") >= 0) {
            rightCol = sdc;
            const QString leftName = name.replace("right", "left");
            for (int i = 0; i < rgb->getNumberOfColumns(); i++) {
               if (rgb->getColumnName(i).toLower().trimmed() == leftName) {
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

static const QString rgbPaintSurfaceID("surface-rgb-paint-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsRgbPaint::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsRgbPaint") {
         showSceneNodeAttribute(*sc,
                                rgbPaintSurfaceID,
                                brainSet->getRgbPaintFile(),
                                "Rgb Paint File",
                                selectedColumn,
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
            else if (infoName == "applySelectionToLeftAndRightStructuresFlag") {
               applySelectionToLeftAndRightStructuresFlag = si->getValueAsBool();
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsRgbPaint::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   RgbPaintFile* rpf = brainSet->getRgbPaintFile();
   
   if (onlyIfSelected) {
      if (rpf->getNumberOfColumns() <= 0) {
         return;
      }
      
      BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
      if (bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::OVERLAY_RGB_PAINT) == false) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsRgbPaint");
   
   saveSceneNodeAttribute(sc,
                          rgbPaintSurfaceID,
                          rpf,
                          selectedColumn);
                          
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
   sc.addSceneInfo(SceneFile::SceneInfo("applySelectionToLeftAndRightStructuresFlag",
                                        applySelectionToLeftAndRightStructuresFlag));
   
   scene.addSceneClass(sc);
}

/**
 * for node attribute files - all column selections for each surface are the same.
 */
/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsRgbPaint::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (selectedColumn[bm1] == selectedColumn[bm2]);
}      


