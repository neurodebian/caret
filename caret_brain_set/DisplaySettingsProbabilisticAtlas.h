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



#ifndef __VE_DISPLAY_SETTINGS_PROB_ATLAS_H__
#define __VE_DISPLAY_SETTINGS_PROB_ATLAS_H__

#include <vector>
#include "DisplaySettings.h"

class Structure;

/// DisplaySettingsProbabilisticAtlas is a class that maintains parameters for controlling
/// the display of Probabilistic Atlas Files.
class DisplaySettingsProbabilisticAtlas : public DisplaySettings {
   public:
      /// Probabilistic type
      enum PROBABILISTIC_TYPE {
         PROBABILISTIC_TYPE_SURFACE,
         PROBABILISTIC_TYPE_VOLUME
      };
      
      /// Type of topography display
      enum PROBABILISTIC_DISPLAY_TYPE {
         PROBABILISTIC_DISPLAY_TYPE_NORMAL,
         PROBABILISTIC_DISPLAY_TYPE_THRESHOLD
      };
      
      /// Constructor
      DisplaySettingsProbabilisticAtlas(BrainSet* bs,
                                        const PROBABILISTIC_TYPE probTypeIn);
      
      /// Constructor
      ~DisplaySettingsProbabilisticAtlas();
      
      /// get the number of channels selected
      int getNumberOfChannelsSelected() const;
      
      /// get channel selected
      bool getChannelSelected(const int index) const;
      
      /// get channel selected for structure set with updateSelectedChannelsForCurrentStructure()
      bool getChannelSelectedForStructure(const int index) const;
      
      /// set channel selected
      void setChannelSelected(const int index, const bool sel) { channelSelected[index] = sel; }
      
      /// set selections status of all channels
      void setAllChannelsSelectedStatus(const bool status);

      /// get area selected
      bool getAreaSelected(const int index) const { return areaSelected[index]; }
    
      /// set selections status of all areas
      void setAllAreasSelectedStatus(const bool status);
      
      /// set area selected
      void setAreaSelected(const int index, const bool sel) { areaSelected[index] = sel; }
      
      /// get the topography display type
      PROBABILISTIC_DISPLAY_TYPE getDisplayType() const { return displayType; }
      
      /// set the topography display type
      void setDisplayType(const PROBABILISTIC_DISPLAY_TYPE pdt) { displayType = pdt; }
      
      /// get treat color ??? as if it was Unassigned
      bool getTreatQuestColorAsUnassigned() const { return treatQuestColorAsUnassigned; }
      
      /// set treat color ??? as if it was Unassigned
      void setTreatQuestColorAsUnassigned(const bool tcu) { treatQuestColorAsUnassigned = tcu; }
      
      /// get thresholding display type ratio (0 = at least one to 1 = all)
      float getThresholdDisplayTypeRatio() const { return thresholdDisplayTypeRatio; }
      
      /// set thresholding display type ratio (0 = at least one to 1 = all)
      void setThresholdDisplayTypeRatio(const float ratio) 
                                                { thresholdDisplayTypeRatio = ratio; }
      
      /// get apply to left and right structures flag
      bool getApplySelectionToLeftAndRightStructuresFlag() const 
             { return applySelectionToLeftAndRightStructuresFlag; }
      
      /// set apply to left and right structures flag
      void setApplySelectionToLeftAndRightStructuresFlag(const bool b) 
             { applySelectionToLeftAndRightStructuresFlag = b; }
      
      /// for node attribute files - all column selections for each surface are the same
      virtual bool columnSelectionsAreTheSame(const int bm1, const int bm2) const; 
       
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded metric file
      void update();
      
      /// update structure associated with each channel
      void updateSelectedChannelsForCurrentStructure(const Structure& structure);
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       

   private:
      /// type of data
      PROBABILISTIC_TYPE probType;
      
      /// display type
      PROBABILISTIC_DISPLAY_TYPE displayType;
      
      /// channel selections
      std::vector<bool> channelSelected;
      
      /// channel selections for specific structures (do not need to save to scene)
      std::vector<bool> channelSelectedForStructure;
      
      /// areas (colors) selected
      std::vector<bool> areaSelected;
      
      /// threshold display type ratio
      float thresholdDisplayTypeRatio;
      
      /// treat the color ??? as if was the color Unassigned
      bool treatQuestColorAsUnassigned;
      
      /// apply coloring with corresponding structures
      bool applySelectionToLeftAndRightStructuresFlag;
};

#endif


