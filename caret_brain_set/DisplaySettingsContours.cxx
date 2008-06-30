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

#include "BrainModelContours.h"
#include "BrainSet.h"
#include "ContourCellFile.h"
#include "ContourCellColorFile.h"
#include "DisplaySettingsContours.h"

/**
 * Constructor.
 */
DisplaySettingsContours::DisplaySettingsContours(BrainSet* bs)
   : DisplaySettings(bs)
{
   reset();
}

/**
 * Destructor.
 */
DisplaySettingsContours::~DisplaySettingsContours()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsContours::reset()
{
   drawMode   = DRAW_MODE_POINTS_AND_LINES;
   showEndPoints = false;
   showContourCells = true;
   drawLineThickness   = 1.0;
   drawPointSize = 2.0;
   contourCellSize = 4.0;
   alignmentContourValid = false;
   alignmentContourNumber = 0;
   displayCrossAtOrigin = true;
}

/**
 * Update selections due to changes in loaded cells.
 */
void
DisplaySettingsContours::update()
{
   determineDisplayedContourCells();
}

/**
 * Determine which contour cells are displayed.
 */
void
DisplaySettingsContours::determineDisplayedContourCells()
{
   BrainModelContours* bmc = brainSet->getBrainModelContours(-1);
   if (bmc == NULL) {
      return;
   }
   //ContourFile* contourFile = bmc->getContourFile();
   ContourCellColorFile* colorFile = brainSet->getContourCellColorFile();
   const int numColors = colorFile->getNumberOfColors();
   
   //
   // Only use class information in the fiducial foci or cell file
   //
   ContourCellFile* cellFile = brainSet->getContourCellFile();
   
   //
   // Minimum and maximum displayed section
   //
   //const int minimumSection = contourFile->getMinimumSelectedSection();
   //const int maximumSection = contourFile->getMaximumSelectedSection();
      
   const int numCells = cellFile->getNumberOfCells();      
   for (int j = 0; j < numCells; j++) {
      CellData* cd = cellFile->getCell(j);
      cd->setDisplayFlag(false);
      
      //
      // Check color selected for display
      //
      bool colorDisplayFlag = true;
      const int colorIndex = cd->getColorIndex();
      if ((colorIndex >= 0) && (colorIndex < numColors)) {
         colorDisplayFlag = colorFile->getSelected(colorIndex);
      }
      
      //
      // Check class selected
      //
      bool classDisplayFlag = true;
      if (cd->getClassIndex() >= 0) {
         classDisplayFlag = cellFile->getCellClassSelectedByIndex(cd->getClassIndex());
      }   
      //
      // Check section for cells
      //
      bool sectionDisplayFlag = true;
      //if ((cd->getSectionNumber() < minimumSection) ||
      //    (cd->getSectionNumber() > maximumSection)) {
      //   sectionDisplayFlag = false;
      //}
      
      //
      // Set the cell's display flag.
      //
      cd->setDisplayFlag(colorDisplayFlag &&
                         classDisplayFlag &&
                         sectionDisplayFlag);
   }
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsContours::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsContours") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "drawMode") {
               drawMode = static_cast<DRAW_MODE>(si->getValueAsInt());
            }
            else if (infoName == "contour-drawAsLines") {
               bool b;
               si->getValue(b);
               if (b) {
                  drawMode = DRAW_MODE_LINES;
               }
               else {
                  drawMode = DRAW_MODE_POINTS_AND_LINES;
               }
            }
            else if (infoName == "contour-showEndPoints") {
               si->getValue(showEndPoints);
            }
            else if (infoName == "contour-drawingSize") {
               si->getValue(drawPointSize);
            }
            else if (infoName == "drawLineThickness") {
               si->getValue(drawLineThickness);
            }
            else if (infoName == "drawPointSize") {
               si->getValue(drawPointSize);
            }
            else if (infoName == "showContourCells") {
               si->getValue(showContourCells);
            }
            else if (infoName == "contourCellSize") {
               si->getValue(contourCellSize);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsContours::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   if (onlyIfSelected) {
      BrainModelContours* bmc = brainSet->getBrainModelContours();
      if (bmc == NULL) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsContours");
   
   sc.addSceneInfo(SceneFile::SceneInfo("drawMode",
                                        static_cast<int>(drawMode)));
   sc.addSceneInfo(SceneFile::SceneInfo("contour-showEndPoints",
                                        showEndPoints));
   sc.addSceneInfo(SceneFile::SceneInfo("drawPointSize",
                                        drawPointSize));
   sc.addSceneInfo(SceneFile::SceneInfo("drawLineThickness",
                                        drawLineThickness));
   sc.addSceneInfo(SceneFile::SceneInfo("showContourCells",
                                        showContourCells));
   sc.addSceneInfo(SceneFile::SceneInfo("contourCellSize",
                                        contourCellSize));
   
   scene.addSceneClass(sc);
}
                       
