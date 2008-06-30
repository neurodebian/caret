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



#ifndef __DISPLAY_SETTINGS_SURFACE_VECTORS_H__
#define __DISPLAY_SETTINGS_SURFACE_VECTORS_H__

#include <vector>

#include "DisplaySettings.h"

/// DisplaySettingsSurfaceVectors is a class that maintains parameters for
/// controlling the display of surface vector data.
class DisplaySettingsSurfaceVectors : public DisplaySettings {
   public:
      /// display mode types
      enum DISPLAY_MODE {
         /// show all vectors
         DISPLAY_MODE_ALL,
         /// show no vectors
         DISPLAY_MODE_NONE,
         /// show sparse (some) vectors
         DISPLAY_MODE_SPARSE 
      };

      /// Constructor
      DisplaySettingsSurfaceVectors(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsSurfaceVectors();
      
      /// get the display mode
      DISPLAY_MODE getDisplayMode() const { return displayMode; }

      /// set the display mode
      void setDisplayMode(const DISPLAY_MODE dm);
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded areal estimation file
      void update();
      
      /// Get the selected column
      int getSelectedColumn(const int model) const;
      
      /// Set the selected file index
      void setSelectedColumn(const int model, const int col);
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       
      /// for node attribute files - all column selections for each surface are the same
      virtual bool columnSelectionsAreTheSame(const int bm1, const int bm2) const;
      
      /// get the sparse distance
      int getSparseDisplayDistance() const { return sparseDistance; }

      /// set the sparse distance
      void setSparseDisplayDistance(const int dist);

      /// get display vector for node
      bool getDisplayVectorForNode(const int nodeNum) const;
      
      /// set display vector for node
      void setDisplayVectorForNode(const int nodeNum, const bool status);

      /// get the length multiplier
      float getLengthMultiplier() const { return lengthMultiplier; }
      
      // set the length multiplier
      void setLengthMultiplier(const float len);
      
   private:
      // update the displayed vectors
      void updateDisplayedVectors();
      
      /// selected column
      std::vector<int> selectedColumn;
      
      /// the display mode
      DISPLAY_MODE displayMode;

      /// sparse distance (mm)
      int sparseDistance;

      /// display a vector for a node
      std::vector<bool> displayVectorForNode;

      /// vector length multiplier
      float lengthMultiplier;
};

#endif // __DISPLAY_SETTINGS_SURFACE_VECTORS_H__

