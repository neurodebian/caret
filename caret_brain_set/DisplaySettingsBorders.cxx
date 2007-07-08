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

#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BrainSet.h"
#include "BrainModelBorderSet.h"
#include "DisplaySettingsBorders.h"

/**
 * Constructor
 */
DisplaySettingsBorders::DisplaySettingsBorders(BrainSet* bs)
   : DisplaySettings(bs)
{
   drawMode = BORDER_DRAW_AS_SYMBOLS;
   displayBorders = false;
   overrideBorderColorsWithAreaColors = false;
   displayFlatBordersRaised = true;
   displayFirstLinkRed = false;
   displayFlatUncertaintyVectors = false;
   borderSize = 2;
   stretchFactor = 10.0;
   symbolType = ColorFile::ColorStorage::SYMBOL_OPENGL_POINT;
   opacity = 1.0;
   reset();
}

/**
 * Destructor
 */
DisplaySettingsBorders::~DisplaySettingsBorders()
{
}

/**
 * Reset to default settings
 */
void
DisplaySettingsBorders::reset()
{
}

/**
 * Updates when files are loaded
 */
void
DisplaySettingsBorders::update()
{
   determineDisplayedBorders();
}

/**
 * Determine which borders should be displayed in all border files.
 */
void
DisplaySettingsBorders::determineDisplayedBorders()
{
   const BorderColorFile* cf = brainSet->getBorderColorFile();
   const int numColors = cf->getNumberOfColors();
   
   BrainModelBorderSet* bmbs = brainSet->getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   
   for (int j = 0; j < numBorders; j++) {
      BrainModelBorder* b = bmbs->getBorder(j);
      
      bool colorDisplayed = true;
      
      const int colorIndex = b->getBorderColorFileIndex();
      if ((colorIndex >= 0) && (colorIndex < numColors)) {
         colorDisplayed = cf->getSelected(colorIndex);
      }
      
      b->setDisplayFlag(displayBorders &&
                        colorDisplayed &&
                        b->getNameDisplayFlag());
   }

   BorderFile* volumeBorders = bmbs->getVolumeBorders();
   const int numVolumeBorders = volumeBorders->getNumberOfBorders();
   for (int j = 0; j < numVolumeBorders; j++) {
      Border* b = volumeBorders->getBorder(j);

      bool colorDisplayed = true;
      const int colorIndex = b->getBorderColorIndex();
      if ((colorIndex >= 0) && (colorIndex < numColors)) {
         colorDisplayed = cf->getSelected(colorIndex);
      }

      b->setDisplayFlag(displayBorders &&
                        colorDisplayed &&
                        b->getNameDisplayFlag());
   }
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsBorders::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsBorders") {
         //BorderColorFile* cf = brainSet->getBorderColorFile();
         //const int numColors = cf->getNumberOfColors();
         
         BrainModelBorderSet* bmbs = brainSet->getBorderSet();
         const int numBorders = bmbs->getNumberOfBorders();

         BorderFile* volumeBorders = bmbs->getVolumeBorders();
         const int numVolumeBorders = volumeBorders->getNumberOfBorders();
         
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "drawMode") {
               drawMode = static_cast<BORDER_DRAW_MODE>(si->getValueAsInt());
            }
            else if (infoName == "displayBorders") {
               si->getValue(displayBorders);
            }
            else if (infoName == "displayFlatBordersRaised") {
               si->getValue(displayFlatBordersRaised);
            }
            else if (infoName == "displayFirstLinkRed") {
               si->getValue(displayFirstLinkRed);
            }
            else if (infoName == "displayFlatUncertaintyVectors") {
               si->getValue(displayFlatUncertaintyVectors);
            }
            else if (infoName == "overrideBorderColorsWithAreaColors") {
               si->getValue(overrideBorderColorsWithAreaColors);
            }
            else if (infoName == "borderSize") {
               si->getValue(borderSize);
            }
            else if (infoName == "stretchFactor") {
               si->getValue(stretchFactor);
            }
            else if (infoName == "symbolType") {
               symbolType = ColorFile::ColorStorage::textToSymbol(si->getValueAsString());
            }
            else if (infoName == "color") {
               showSceneColorFile(*si, brainSet->getBorderColorFile(), 
                                  "Border", errorMessage);
/*
               const QString name = si->getModelName();
               const bool selected = si->getValueAsBool();
               bool colorFound = false;
               for (int j = 0; j < numColors; j++) {
                  ColorFile::ColorStorage* cs = cf->getColor(j);
                  if (cs->getName() == name) {
                     cs->setSelected(selected);
                     colorFound = true;
                  }
               }
               if (colorFound == false) {
                  QString msg("Border color \"");
                  msg.append(name);
                  msg.append("\" not found.\n");
                  errorMessage.append(msg);
               }
*/
            }
            else if (infoName == "border-surf") {
               const QString name = si->getModelName();
               const bool selected = si->getValueAsBool();
               bool borderFound = false;
               for (int j = 0; j < numBorders; j++) {
                  BrainModelBorder* b = bmbs->getBorder(j);
                  if (b->getName() == name) {
                     b->setNameDisplayFlag(selected);
                     borderFound = true;
                  }
               }
               if (borderFound == false) {
                  QString msg("Surface Border named \"");
                  msg.append(name);
                  msg.append("\" not found.\n");
                  errorMessage.append(msg);
               }
            }
            else if (infoName == "border-vol") {
               const QString name = si->getModelName();
               const bool selected = si->getValueAsBool();
               bool borderFound = false;
               for (int j = 0; j < numVolumeBorders; j++) {
                  Border* b = volumeBorders->getBorder(j);
                  if (b->getName() == name) {
                     b->setNameDisplayFlag(selected);
                     borderFound = true;
                  }
               }
               if (borderFound == false) {
                  QString msg("Volume Border named \"");
                  msg.append(name);
                  msg.append("\" not found.\n");
                  errorMessage.append(msg);
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
DisplaySettingsBorders::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   if (onlyIfSelected) {
      if (displayBorders == false) {
         return;
      }
      BrainModelBorderSet* bmbs = brainSet->getBorderSet();
      if (bmbs->getNumberOfBorders() <= 0) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsBorders");
   
   sc.addSceneInfo(SceneFile::SceneInfo("drawMode", drawMode));      
   sc.addSceneInfo(SceneFile::SceneInfo("displayBorders", displayBorders));  
   sc.addSceneInfo(SceneFile::SceneInfo("displayFlatBordersRaised", displayFlatBordersRaised));      
   sc.addSceneInfo(SceneFile::SceneInfo("displayFirstLinkRed", displayFirstLinkRed));      
   sc.addSceneInfo(SceneFile::SceneInfo("displayFlatUncertaintyVectors", displayFlatUncertaintyVectors));      
   sc.addSceneInfo(SceneFile::SceneInfo("borderSize", borderSize));      
   sc.addSceneInfo(SceneFile::SceneInfo("stretchFactor", stretchFactor)); 
   sc.addSceneInfo(SceneFile::SceneInfo("overrideBorderColorsWithAreaColors", overrideBorderColorsWithAreaColors));
   sc.addSceneInfo(SceneFile::SceneInfo("symbolType", ColorFile::ColorStorage::symbolToText(symbolType)));     
  
/*
   const BorderColorFile* cf = brainSet->getBorderColorFile();
   const int numColors = cf->getNumberOfColors();
   for (int i = 0; i < numColors; i++) {
      const ColorFile::ColorStorage* cs = cf->getColor(i);
      sc.addSceneInfo(SceneFile::SceneInfo("color", cs->getName(), cs->getSelected()));
   }
*/
   saveSceneColorFile(sc, "color", brainSet->getBorderColorFile());
   
   BrainModelBorderSet* bmbs = brainSet->getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   
   for (int j = 0; j < numBorders; j++) {
      BrainModelBorder* b = bmbs->getBorder(j);
      SceneFile::SceneInfo si("border-surf", b->getName(), b->getNameDisplayFlag());
      sc.addSceneInfo(si);
   }
   
   BorderFile* volumeBorders = bmbs->getVolumeBorders();
   const int numVolumeBorders = volumeBorders->getNumberOfBorders();
   for (int j = 0; j < numVolumeBorders; j++) {
      Border* b = volumeBorders->getBorder(j);
      sc.addSceneInfo(SceneFile::SceneInfo("border-vol", b->getName(), b->getNameDisplayFlag()));
   }
   
   scene.addSceneClass(sc);
}
                       

