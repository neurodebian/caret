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



#ifndef __VE_DISPLAY_SETTINGS_RGB_PAINT_H__
#define __VE_DISPLAY_SETTINGS_RGB_PAINT_H__

#include <vector>

#include "DisplaySettingsNodeAttributeFile.h"

/// DisplaySettingsRgbPaint is a class that maintains parameters for controlling
/// the display of RGB Paint files.
class DisplaySettingsRgbPaint : public DisplaySettingsNodeAttributeFile {
   public:
      /// Display Mode
      enum RGB_DISPLAY_MODE {
         RGB_DISPLAY_MODE_POSITIVE,
         RGB_DISPLAY_MODE_NEGATIVE
      };
      
      /// Constructor
      DisplaySettingsRgbPaint(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsRgbPaint();
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded rgb paint file
      void update();
      
      /// Get the thresholds
      void getThresholds(float& redThresh, float& greenThresh, float& blueThresh) const;
      
      /// Set the thresholds
      void setThresholds(const float redThresh, const float greenThresh, const float blueThresh);
      
      /// get the display mode
      RGB_DISPLAY_MODE getDisplayMode() const { return displayMode; }
      
      /// set the display mode
      void setDisplayMode(const RGB_DISPLAY_MODE dm) { displayMode = dm; }
      
      /// red enabled for display
      bool getRedEnabled() const { return redEnabled; }
      
      /// set red enabled for display
      void setRedEnabled(const int re) { redEnabled = re; };

      /// green enabled for display
      bool getGreenEnabled() const { return greenEnabled; }
      
      /// set red enabled for display
      void setGreenEnabled(const int ge) { greenEnabled = ge; };

      /// blue enabled for display
      bool getBlueEnabled() const { return blueEnabled; }

      /// set blue enabled for display
      void setBlueEnabled(const int be) { blueEnabled = be; };

      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
       
   private:
      /// red threshold
      float redThreshold;
      
      /// green threshold
      float greenThreshold;
      
      /// blue threshold
      float blueThreshold;
      
      /// display mode
      RGB_DISPLAY_MODE displayMode;
      
      /// red enabled for display
      bool redEnabled; 
      
      /// green enabled for display
      bool greenEnabled;
      
      /// blue enabled for display
      bool blueEnabled;
};

#endif
