
#ifndef __DISPLAY_SETTINGS_WUSTL_REGION_H__
#define __DISPLAY_SETTINGS_WUSTL_REGION_H__

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

#include <QString>

#include "DisplaySettings.h"

/// class for controlling display of a wustl region file
class DisplaySettingsWustlRegion : public DisplaySettings {
   public:
      /// graph mode 
      enum GRAPH_MODE {
         GRAPH_MODE_AUTO_SCALE,
         GRAPH_MODE_USER_SCALE
      };
      
      /// time course selection
      enum TIME_COURSE_SELECTION {
         TIME_COURSE_SELECTION_ALL,
         TIME_COURSE_SELECTION_SINGLE
      };
      
      /// constructor
      DisplaySettingsWustlRegion(BrainSet* bsIn);
      
      /// destructor
      ~DisplaySettingsWustlRegion();
      
      /// reinitialize all display settings
      virtual void reset();
      
      /// update any selections due to changes with loaded data files
      virtual void update();
   
      /// get popup graph enabled
      bool getPopupGraphEnabled() const { return popupGraphEnabled; }
      
      /// set popup graph enabled
      void setPopupGraphEnabled(const bool enabled)  { popupGraphEnabled = enabled; }
      
      /// get the selected time course number
      int getSelectedTimeCourse() const { return selectedTimeCourse; }
      
      /// set the selected time course number
      void setSelectedTimeCourse(const int tc);
      
      /// get the selected case name
      QString getSelectedCaseName() const { return selectedCaseName; }
      
      /// set the selected case name
      void setSelectedCaseName(const QString& caseName) { selectedCaseName = caseName; }
      
      /// get the selected paint volume
      int getSelectedPaintVolume() const { return selectedPaintVolume; }
      
      /// set the selected paint volume
      void setSelectedPaintVolume(const int spv) { selectedPaintVolume = spv; }
      
      /// get the graph mode
      GRAPH_MODE getGraphMode() const { return graphMode; }
      
      /// set the graph mode
      void setGraphMode(const GRAPH_MODE gm) { graphMode = gm; }
      
      /// get the user scale
      void getUserScale(float& minScaleOut, float& maxScaleOut) const;
      
      /// set the user scale
      void setUserScale(const float minScaleIn, const float maxScaleIn);
      
      /// get the time course selection
      TIME_COURSE_SELECTION getTimeCourseSelection() const { return timeCourseSelection; }
      
      /// set the time course selection
      void setTimeCourseSelection(const TIME_COURSE_SELECTION tcs) { timeCourseSelection = tcs; }
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                       
   protected:
      /// time course selection
      TIME_COURSE_SELECTION timeCourseSelection;
      
      /// popup graph enabled
      bool popupGraphEnabled;
      
      /// the selected time course
      int selectedTimeCourse;
      
      /// the selected case name
      QString selectedCaseName;
      
      /// the selected paint volume
      int selectedPaintVolume;
      
      /// the graph mode
      GRAPH_MODE graphMode;
      
      /// the user scale
      float userScale[2];
};

#endif // __DISPLAY_SETTINGS_WUSTL_REGION_H__

