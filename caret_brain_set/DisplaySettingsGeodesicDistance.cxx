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

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsGeodesicDistance.h"
#include "GeodesicDistanceFile.h"

/**
 * Constructor.
 */
DisplaySettingsGeodesicDistance::DisplaySettingsGeodesicDistance(BrainSet* bs)
   : DisplaySettings(bs)
{
   displayColumn = -1;
   pathDisplayEnabled = false;
   showRootNode = false;
   lineWidth = 3;
   reset();
}

/**
 * Destructor.
 */
DisplaySettingsGeodesicDistance::~DisplaySettingsGeodesicDistance()
{
}

/**
 * reinitialize all display settings.
 */
void 
DisplaySettingsGeodesicDistance::reset()
{
   pathDisplayNodeNumber = -1;
}

/**
 * update any selections due to changes with loaded data files.
 */
void 
DisplaySettingsGeodesicDistance::update()
{
   GeodesicDistanceFile* gdf = brainSet->getGeodesicDistanceFile();
   const int numCols = gdf->getNumberOfColumns();
   if (displayColumn >= numCols) {
      if (numCols > 0) {
         displayColumn = 0;
      }
      else {
         displayColumn = -1;
      }
   }
   else if ((numCols > 0) && (displayColumn < 0)) {
      displayColumn = 0;
   }

   if (pathDisplayNodeNumber >= brainSet->getNumberOfNodes()) {
      pathDisplayNodeNumber = -1;
   }
}   

static const QString geodesicID("geodesic-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsGeodesicDistance::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   GeodesicDistanceFile* gdf = brainSet->getGeodesicDistanceFile();
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsGeodesicDistance") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == geodesicID) {
               showSceneNodeAttributeColumn(si,
                                            gdf,
                                            "Geodesic Distance File",
                                            displayColumn,
                                            errorMessage);
            }
            else if (infoName == "pathDisplayNodeNumber") {
               si->getValue(pathDisplayNodeNumber);
            }
            else if (infoName == "pathDisplayEnabled") {
               si->getValue(pathDisplayEnabled);
            }
            else if (infoName == "showRootNode") {
               si->getValue(showRootNode);
            }
            else if (infoName == "geolineWidth") {
               si->getValue(lineWidth);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsGeodesicDistance::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   GeodesicDistanceFile* gdf = brainSet->getGeodesicDistanceFile();

   if (onlyIfSelected) {
      if (gdf->getNumberOfColumns() <= 0) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsGeodesicDistance");
   
   saveSceneNodeAttributeColumn(sc,
                                geodesicID,
                                gdf,
                                displayColumn);
   sc.addSceneInfo(SceneFile::SceneInfo("pathDisplayNodeNumber",
                                        pathDisplayNodeNumber));
   sc.addSceneInfo(SceneFile::SceneInfo("pathDisplayEnabled",
                                        pathDisplayEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("showRootNode",
                                        showRootNode));
   sc.addSceneInfo(SceneFile::SceneInfo("geolineWidth",
                                        lineWidth));
   
   scene.addSceneClass(sc);
}
                       
