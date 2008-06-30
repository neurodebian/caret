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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsSurfaceShape.h"
#include "PaletteFile.h"
#include "SurfaceShapeFile.h"

/**
 * The constructor.
 */
DisplaySettingsSurfaceShape::DisplaySettingsSurfaceShape(BrainSet* bs)
   : DisplaySettingsNodeAttributeFile(bs,
                                      bs->getSurfaceShapeFile(),
                                      NULL,
                                      BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE,
                                      true,
                                      false)
{
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsSurfaceShape::~DisplaySettingsSurfaceShape()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsSurfaceShape::reset()
{
   DisplaySettingsNodeAttributeFile::reset();
   colorMap = SURFACE_SHAPE_COLOR_MAP_GRAY;
   displayColorBar = false;
   nodeUncertaintyColumn = 0;
   nodeUncertaintyEnabled = false;
   paletteIndex = 0;
   interpolatePaletteColors = false;
}

/**
 * Update any selections du to changes in loaded surface shape file.
 */
void
DisplaySettingsSurfaceShape::update()
{
   DisplaySettingsNodeAttributeFile::update();
   
   if (nodeUncertaintyColumn >= 0) {
      const SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
      if (nodeUncertaintyColumn >= ssf->getNumberOfColumns()) {
         nodeUncertaintyColumn = -3; // -3 is none in combo box on d/c
      }
   }
   
   if (paletteIndex >= brainSet->getPaletteFile()->getNumberOfPalettes()) {
      paletteIndex = 0;
   }
}

static const QString surfaceShapeViewID("surface-shape-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsSurfaceShape::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
         
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsSurfaceShape") {
         showSceneSelectedColumns(*sc,
                                  "Surface Shape File",
                                  surfaceShapeViewID,
                                  "",
                                  errorMessage);

         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "colorMap") {
               int val;
               si->getValue(val);
               colorMap = static_cast<SURFACE_SHAPE_COLOR_MAP>(val);
            }
            else if (infoName == "shapeDisplayColorBar") {
               si->getValue(displayColorBar);
            }
            else if (infoName == "nodeUncertaintyColumn") {
               const QString name = si->getValueAsString();
               for (int j = 0; j < ssf->getNumberOfColumns(); j++) {
                  if (name == ssf->getColumnName(j)) {
                     nodeUncertaintyColumn = j;
                  }
               }
            }
            else if (infoName == "nodeUncertaintyEnabled") {
               si->getValue(nodeUncertaintyEnabled);
            }
            else if (infoName == "interpolatePaletteColors") {
               si->getValue(interpolatePaletteColors);
            }
            else if (infoName == "shapePaletteIndex") {
               const QString paletteName = si->getValueAsString();
               PaletteFile* pf = brainSet->getPaletteFile();
               const int num = pf->getNumberOfPalettes();
               bool found = false;
               for (int j = 0; j < num; j++) {
                  const Palette* pal = pf->getPalette(j);
                  if (paletteName == pal->getName()) {
                     paletteIndex = j;
                     found = true;
                     break;
                  }
               }
               if (found == false) {
                  errorMessage.append("Unable to find palette named: ");
                  errorMessage.append(paletteName);
                  errorMessage.append("\n");
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
DisplaySettingsSurfaceShape::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
   
   if (onlyIfSelected) {
      if (ssf->getNumberOfColumns() <= 0) {
         return;
      }
      
      if (brainSet->isASurfaceOverlayForAnySurface(
                  BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE) == false) {
//         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsSurfaceShape");
   
   saveSceneSelectedColumns(sc);

   sc.addSceneInfo(SceneFile::SceneInfo("colorMap",
                                        colorMap));
   sc.addSceneInfo(SceneFile::SceneInfo("shapeDisplayColorBar",
                                        displayColorBar));
   if ((nodeUncertaintyColumn >= 0) && (nodeUncertaintyColumn < ssf->getNumberOfColumns())) {
      sc.addSceneInfo(SceneFile::SceneInfo("nodeUncertaintyColumn",
                                            ssf->getColumnName(nodeUncertaintyColumn)));
   }
   sc.addSceneInfo(SceneFile::SceneInfo("nodeUncertaintyEnabled",
                                        nodeUncertaintyEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("interpolatePaletteColors",
                                        interpolatePaletteColors));
   PaletteFile* pf = brainSet->getPaletteFile();
   if ((paletteIndex >= 0) && (paletteIndex < pf->getNumberOfPalettes())) {
      const Palette* pal = pf->getPalette(paletteIndex);
      sc.addSceneInfo(SceneFile::SceneInfo("shapePaletteIndex",
                                           pal->getName()));
   }
   
   scene.addSceneClass(sc);
}
 
/**
 * get the columns for palette and color mapping (negative if invalid).
 */
int 
DisplaySettingsSurfaceShape::getShapeColumnForPaletteAndColorMapping() const
{
   int displayColumnOut = -1;
   
   //
   // Get the brain model surface in the main window
   //
   const int mainWindowBrainModelIndex = 
      brainSet->getDisplayedModelIndexForWindow(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
   const BrainModelSurface* bms = brainSet->getBrainModelSurface(mainWindowBrainModelIndex);
   if (bms == NULL) {
      return displayColumnOut;
   }
   
   //
   // See if surface has shape as an overlay
   //
   for (int i = 0; i < brainSet->getNumberOfSurfaceOverlays(); i++) {
      const BrainModelSurfaceOverlay* bmsOverlay = brainSet->getSurfaceOverlay(i); 
      if (bmsOverlay->getOverlay(mainWindowBrainModelIndex) ==
          BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE) {
         displayColumnOut = bmsOverlay->getDisplayColumnSelected(mainWindowBrainModelIndex);
      }
   }
   
   return displayColumnOut;
}
                                                     
