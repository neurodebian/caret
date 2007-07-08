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



#ifndef __DISPLAY_SETTINGS_SECTION_H__
#define __DISPLAY_SETTINGS_SECTION_H__

#include <vector>

#include "DisplaySettings.h"

/// DisplaySettingsSection is a class that maintains parameters for
/// controlling the display of section data.
class DisplaySettingsSection : public DisplaySettings {
   public:
      enum SELECTION_TYPE {
         SELECTION_TYPE_SINGLE,
         SELECTION_TYPE_MULTIPLE,
         SELECTION_TYPE_ALL
      };
      
      /// Constructor
      DisplaySettingsSection(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsSection();
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded areal estimation file
      void update();
      
      /// get the section type
      SELECTION_TYPE getSelectionType() const;
      
      /// set the selection type
      void setSelectionType(SELECTION_TYPE type);
      
      /// Get the selected column
      int getSelectedColumn() const;
      
      /// Set the selected file index
      void setSelectedColumn(const int col);
      
      /// get the minimum selected section
      int getMinimumSelectedSection() const;
      
      /// get the maximum selected section
      int getMaximumSelectedSection() const;
      
      /// set the minimum selected section
      void setMinimumSelectedSection(const int sect);
      
      /// set the maximum selected section
      void setMaximumSelectedSection(const int sect);
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                       
   private:
      /// update section selections
      void updateSectionSelections() const;
      
      /// type of section selection
      SELECTION_TYPE selectionType;
      
      /// selected column
      mutable int selectedColumn;
      
      /// minimum selected section for each column
      mutable int minimumSelectedSection;
      
      /// maximum selected section for each column
      mutable int maximumSelectedSection;
      
};

#endif // __DISPLAY_SETTINGS_SECTION_H__



