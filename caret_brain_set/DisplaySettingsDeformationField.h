

#ifndef __VE_DISPLAY_SETTINGS_DEFORMATION_FIELD_H__
#define __VE_DISPLAY_SETTINGS_DEFORMATION_FIELD_H__

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

#include "DisplaySettings.h"

class BrainSet;

/// DisplaySettingsDeformationField is a class that maintains parameters for controlling
/// the display of deformation field data files.
class DisplaySettingsDeformationField : public DisplaySettings {
   public:
      
      /// display mode types
      enum DISPLAY_MODE {
         DISPLAY_MODE_ALL,
         DISPLAY_MODE_NONE,
         DISPLAY_MODE_SPARSE
      };
      
      /// Constructor
      DisplaySettingsDeformationField(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsDeformationField();
      
      /// get the display mode
      DISPLAY_MODE getDisplayMode() const { return displayMode; }
      
      /// set the display mode
      void setDisplayMode(const DISPLAY_MODE dm);
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded metric file
      void update();
      
      /// get column selected for display
      int getSelectedDisplayColumn();
      
      /// set column for display
      void setSelectedDisplayColumn(const int sdc) { displayColumn = sdc; }
      
      /// get the sparse distance
      int getSparseDisplayDistance() const { return sparseDistance; }
      
      /// set the sparse distance
      void setSparseDisplayDistance(const int dist) { sparseDistance = dist; }
      
      /// get display vectors on identified nodes
      bool getDisplayIdentifiedNodes() const { return displayIdentifiedNodes; }
      
      /// set display vector on identified nodes
      void setDisplayIdentifiedNodes(const bool b) { displayIdentifiedNodes = b; }
      
      /// get display vector for node
      bool getDisplayVectorForNode(const int nodeNum) const;
      
      /// set display vector for node
      void setDisplayVectorForNode(const int nodeNum, const bool status);
      
      /// show vectors unstretched on flat surface
      void getShowUnstretchedOnFlat(float& factor, bool& showIt);

      /// set show vectors unstretched on flat surface
      void setShowUnstretchedOnFlat(const float factor, const bool showIt);

      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                       
   private:
      /// selected column for dislay
      int displayColumn;
      
      /// the display mode
      DISPLAY_MODE displayMode;
      
      /// sparse distance (mm)
      int sparseDistance;
      
      /// display vectors on identified nodes
      bool displayIdentifiedNodes;
      
      /// display a vector for a node
      std::vector<bool> displayVectorForNode;

      /// show unstretched scale factor
      float unstretchedFactor;

      /// show unstretched vectors on flat
      bool showUnstretchedOnFlat;
};

#endif  // __VE_DISPLAY_SETTINGS_DEFORMATION_FIELD_H__
