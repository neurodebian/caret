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



#ifndef __VE_DISPLAY_SETTINGS_CUTS_H__
#define __VE_DISPLAY_SETTINGS_CUTS_H__

#include "DisplaySettings.h"

/// Display settings for cuts
class DisplaySettingsCuts : public DisplaySettings {
   public:
      
      /// Constructor
      DisplaySettingsCuts(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsCuts();
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded cuts
      void update();
      
      /// get display cuts
      bool getDisplayCuts() const { return displayCuts; }
      
      /// set display cuts
      void setDisplayCuts(const bool dfc) { displayCuts = dfc; }
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                       
   protected:
      /// display cuts
      bool displayCuts;      
};

#endif // __VE_DISPLAY_SETTINGS_CUTS_H__

