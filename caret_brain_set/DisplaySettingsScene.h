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

#ifndef __DISPLAY_SETTINGS_SCENE_H__
#define __DISPLAY_SETTINGS_SCENE_H__

#include "DisplaySettings.h"

/// Display settings for scenes
class DisplaySettingsScene : public DisplaySettings {
   public:
      /// type for window position
      enum WINDOW_POSITIONS {
         /// use all window positions from scene
         WINDOW_POSITIONS_USE_ALL,
         /// ignore main window position, make all others relative to scene
         WINDOW_POSITIONS_IGNORE_MAIN_OTHERS_RELATIVE,
         /// ignore all window positions
         WINDOW_POSITIONS_IGNORE_ALL
      };
      
      /// Constructor
      DisplaySettingsScene(BrainSet* bsIn);
      
      /// Destructor
      ~DisplaySettingsScene();

      /// reinitialize all display settings
      void reset();
      
      /// update any selections due to changes with loaded data files
      void update();
                  
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage);
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);  
         
      /// get the window position preference
      WINDOW_POSITIONS getWindowPositionPreference() const { return windowPositions; }
      
      /// set the window position preference
      void setWindowPositionPreference(const WINDOW_POSITIONS wpp) { windowPositions = wpp; }
      
   private:
      /// window positions (DO NOT SAVE TO SCENE)
      WINDOW_POSITIONS windowPositions;
};

#endif // __DISPLAY_SETTINGS_IMAGES_H__

