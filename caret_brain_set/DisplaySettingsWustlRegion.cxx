
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

#include "BrainSet.h"
#include "DisplaySettingsWustlRegion.h"
#include "WustlRegionFile.h"

/**
 * constructor.
 */
DisplaySettingsWustlRegion::DisplaySettingsWustlRegion(BrainSet* bsIn)
   : DisplaySettings(bsIn)
{
   popupGraphEnabled   = false;
   userScale[0] = -10.0;
   userScale[1] =  10.0;
   graphMode = GRAPH_MODE_AUTO_SCALE;
   timeCourseSelection = TIME_COURSE_SELECTION_SINGLE;
   reset();
}

/**
 * destructor.
 */
DisplaySettingsWustlRegion::~DisplaySettingsWustlRegion()
{
}

/**
 * get the user scale.
 */
void 
DisplaySettingsWustlRegion::getUserScale(float& minScaleOut, float& maxScaleOut) const
{
   minScaleOut = userScale[0];
   maxScaleOut = userScale[1];
}

/**
 * set the user scale.
 */
void 
DisplaySettingsWustlRegion::setUserScale(const float minScaleIn, const float maxScaleIn)
{
   userScale[0] = minScaleIn;
   userScale[1] = maxScaleIn;
}
      
/**
 * set the selected time course number.
 */
void 
DisplaySettingsWustlRegion::setSelectedTimeCourse(const int tc)
{
   selectedTimeCourse = tc;
   update();
}
      
/**
 * reinitialize all display settings.
 */
void 
DisplaySettingsWustlRegion::reset()
{
   selectedTimeCourse  = -1;
   selectedCaseName    = "";
   selectedPaintVolume = -1;
}

/**
 * update any selections due to changes with loaded data files.
 */
void 
DisplaySettingsWustlRegion::update()
{
   //
   // Update selected paint volume
   //
   const int numPaintVolumes = brainSet->getNumberOfVolumePaintFiles();
   if (numPaintVolumes > 0) {
      if ((selectedPaintVolume < 0) ||
          (selectedPaintVolume >= numPaintVolumes)) {
         selectedPaintVolume = 0;
      }
   }
   else {
      selectedPaintVolume = -1;
   }
   
   //
   // Update selected time course
   //
   const WustlRegionFile* wrf = brainSet->getWustlRegionFile();
   const int numTimeCourses = wrf->getNumberOfTimeCourses();
   if (numTimeCourses > 0) {
      if ((selectedTimeCourse < 0) ||
          (selectedTimeCourse >= numTimeCourses)) {
         selectedTimeCourse = 0;
      }
   }
   else {
      selectedTimeCourse = -1;
   }
   
   //
   // Update for selected case name
   //
   if (selectedTimeCourse >= 0) {
      if (selectedCaseName.isEmpty()) {
         selectedCaseName = "average";
      }
      const WustlRegionFile::TimeCourse* tc = wrf->getTimeCourse(selectedTimeCourse);
      std::vector<QString> caseNames;
      tc->getAllRegionCaseNames(caseNames);
      
      //
      // If current case name is not found, try to set it to first available case name
      //
      if (std::find(caseNames.begin(), caseNames.end(), selectedCaseName)
          == caseNames.end()) {
         selectedCaseName = "";
         if (caseNames.empty() == false) {
            selectedCaseName = caseNames[0];
         }
      }
   }
   else {
      selectedCaseName = "";
   }
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsWustlRegion::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsWustlRegion") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "popupGraphEnabled") {
               si->getValue(popupGraphEnabled);
            }
            else if (infoName == "userScale0") {
               si->getValue(userScale[0]);
            }
            else if (infoName == "userScale1") {
               si->getValue(userScale[1]);
            }
            else if (infoName == "timeCourseSelection") {
               timeCourseSelection = static_cast<TIME_COURSE_SELECTION>(si->getValueAsInt());
            }
            else if (infoName == "graphMode") {
               graphMode = static_cast<GRAPH_MODE>(si->getValueAsInt());
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsWustlRegion::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   const WustlRegionFile* wrf = brainSet->getWustlRegionFile();
   
   if (onlyIfSelected) {
      if (popupGraphEnabled == false) {
         return;
      }
      if (wrf->empty()) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsWustlRegion");
   
   sc.addSceneInfo(SceneFile::SceneInfo("timeCourseSelection", timeCourseSelection));
   sc.addSceneInfo(SceneFile::SceneInfo("popupGraphEnabled", popupGraphEnabled));
   sc.addSceneInfo(SceneFile::SceneInfo("graphMode", graphMode));
   sc.addSceneInfo(SceneFile::SceneInfo("userScale0", userScale[0]));
   sc.addSceneInfo(SceneFile::SceneInfo("userScale1", userScale[1]));
        
   scene.addSceneClass(sc);
}
                       
