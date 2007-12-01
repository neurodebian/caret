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
#include "DisplaySettingsSurfaceShape.h"
#include "PaletteFile.h"
#include "SurfaceShapeFile.h"

/**
 * The constructor.
 */
DisplaySettingsSurfaceShape::DisplaySettingsSurfaceShape(BrainSet* bs)
   : DisplaySettings(bs)
{
   applySelectionToLeftAndRightStructuresFlag = false;
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
   colorMap = SURFACE_SHAPE_COLOR_MAP_GRAY;
   displayColorBar = false;
   nodeUncertaintyColumn = 0;
   nodeUncertaintyEnabled = false;
   paletteIndex = 0;
   interpolatePaletteColors = false;
   displayColumn.clear();
}

/**
 * Update any selections du to changes in loaded surface shape file.
 */
void
DisplaySettingsSurfaceShape::update()
{
   updateSelectedColumnIndices(brainSet->getSurfaceShapeFile(), displayColumn);
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

/**
 * Get the column selected for display.
 * Returns -1 if there are no surface shape columns.
 */
int
DisplaySettingsSurfaceShape::getSelectedDisplayColumn(const int modelIn) const
{
   if (displayColumn.empty()) {
      return -1;
   }
   
   int model = modelIn;
   if (model < 0) {
      model = 0;
   }
   
   return displayColumn[model]; 
}

/**
 * set column for display.
 */
void 
DisplaySettingsSurfaceShape::setSelectedDisplayColumn(const int model,
                                                      const int sdc) 
{ 
   if (applySelectionToLeftAndRightStructuresFlag) {
      SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
      if ((sdc >= 0) && (sdc < ssf->getNumberOfColumns())) {
         int leftCol = -1;
         int rightCol = -1;
         QString name = ssf->getColumnName(sdc).toLower().trimmed();
         if (name.indexOf("left") >= 0) {
            leftCol = sdc;
            const QString rightName = name.replace("left", "right");
            for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
               if (ssf->getColumnName(i).toLower().trimmed() == rightName) {
                  rightCol = i;
                  break;
               }
            }
         }
         else if (name.indexOf("right") >= 0) {
            rightCol = sdc;
            const QString leftName = name.replace("right", "left");
            for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
               if (ssf->getColumnName(i).toLower().trimmed() == leftName) {
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
                        displayColumn[i] = leftCol;
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                     if (rightCol >= 0) {
                        displayColumn[i] = rightCol;
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
         std::fill(displayColumn.begin(), displayColumn.end(), sdc);
      }
      else {
         displayColumn[model] = sdc; 
      }
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
         showSceneNodeAttribute(*sc,
                                surfaceShapeViewID,
                                brainSet->getSurfaceShapeFile(),
                                "Surface Shape File",
                                displayColumn,
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
            else if (infoName == "applySelectionToLeftAndRightStructuresFlag") {
               si->getValue(applySelectionToLeftAndRightStructuresFlag);
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
DisplaySettingsSurfaceShape::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
   
   if (onlyIfSelected) {
      if (ssf->getNumberOfColumns() <= 0) {
         return;
      }
      
      BrainModelSurfaceNodeColoring* bsnc = brainSet->getNodeColoring();
      if (bsnc->isUnderlayOrOverlay(BrainModelSurfaceNodeColoring::OVERLAY_SURFACE_SHAPE) == false) {
//         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsSurfaceShape");
   
   saveSceneNodeAttribute(sc,
                          surfaceShapeViewID,
                          ssf,
                          displayColumn);

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
   sc.addSceneInfo(SceneFile::SceneInfo("applySelectionToLeftAndRightStructuresFlag",
                                        applySelectionToLeftAndRightStructuresFlag));
   PaletteFile* pf = brainSet->getPaletteFile();
   if ((paletteIndex >= 0) && (paletteIndex < pf->getNumberOfPalettes())) {
      const Palette* pal = pf->getPalette(paletteIndex);
      sc.addSceneInfo(SceneFile::SceneInfo("shapePaletteIndex",
                                           pal->getName()));
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
DisplaySettingsSurfaceShape::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (displayColumn[bm1] == displayColumn[bm2]);
}      


