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



#ifndef __VE_DISPLAY_SETTINGS_SURFACE_SHAPE_H__
#define __VE_DISPLAY_SETTINGS_SURFACE_SHAPE_H__

#include <vector>

#include "DisplaySettings.h"

/// DisplaySettingsSurfaceShape is a class that maintains parameters for controlling
/// the display of surface shape data files.
class DisplaySettingsSurfaceShape : public DisplaySettings {
   public:
      /// color map for display
      enum SURFACE_SHAPE_COLOR_MAP {
         SURFACE_SHAPE_COLOR_MAP_GRAY,
         SURFACE_SHAPE_COLOR_MAP_ORANGE_YELLOW,
         SURFACE_SHAPE_COLOR_MAP_PALETTE
      };
      
      /// Constructor
      DisplaySettingsSurfaceShape(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsSurfaceShape();
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded surface shape file
      void update();
      
      /// get column selected for display
      int getSelectedDisplayColumn(const int model) const;
      
      /// set column for display
      void setSelectedDisplayColumn(const int model,
                                    const int sdc);
     
      /// get the color map for display
      SURFACE_SHAPE_COLOR_MAP getColorMap() const { return colorMap; }
      
      /// set the color map for display
      void setColorMap(const SURFACE_SHAPE_COLOR_MAP cm) { colorMap = cm; }
      
      /// get display color bar      
      bool getDisplayColorBar() const { return displayColorBar; }

      /// set display color bar
      void setDisplayColorBar(const bool dcb) { displayColorBar = dcb; }

      /// get node uncertainty column
      int getNodeUncertaintyColumn() const { return nodeUncertaintyColumn; }
      
      /// set the node undertainty column
      void setNodeUncertaintyColumn(const int col) { nodeUncertaintyColumn = col; }
      
      /// get node uncertainty enabled
      bool getNodeUncertaintyEnabled() const { return nodeUncertaintyEnabled; }

      /// set node uncertainty enabled
      void setNodeUncertaintyEnabled(const bool ne) { nodeUncertaintyEnabled = ne; }

      /// get the selected palette index
      int getSelectedPaletteIndex() const { return paletteIndex; }

      /// set the selected palette index
      void setSelectedPaletteIndex(const int pi) { paletteIndex = pi; }

      /// get interpolate palette colors flag
      bool getInterpolatePaletteColors() const { return interpolatePaletteColors; }

      /// set interpolate palette colors flag
      void setInterpolatePaletteColors(const int ic) { interpolatePaletteColors = ic; }
      
      /// get apply to left and right structures flag
      bool getApplySelectionToLeftAndRightStructuresFlag() const 
             { return applySelectionToLeftAndRightStructuresFlag; }
      
      /// set apply to left and right structures flag
      void setApplySelectionToLeftAndRightStructuresFlag(const bool b) 
             { applySelectionToLeftAndRightStructuresFlag = b; }
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                       
      /// for node attribute files - all column selections for each surface are the same
      virtual bool columnSelectionsAreTheSame(const int bm1, const int bm2) const;
      
   private:
      /// selected column for display
      std::vector<int> displayColumn;
      
      /// color map for display
      SURFACE_SHAPE_COLOR_MAP colorMap;
   
      /// display metric color bar
      bool displayColorBar;
      
      /// node uncertainty column
      int nodeUncertaintyColumn;
 
      /// node uncertainty enabled
      bool nodeUncertaintyEnabled;
      
      /// interpolate palette colors
      bool interpolatePaletteColors;
      
      /// selected palette index
      int paletteIndex;
      
      /// apply coloring with corresponding structures
      bool applySelectionToLeftAndRightStructuresFlag;
};

#endif


