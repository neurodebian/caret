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

#include "DisplaySettingsNodeAttributeFile.h"

/// DisplaySettingsSurfaceShape is a class that maintains parameters for controlling
/// the display of surface shape data files.
class DisplaySettingsSurfaceShape : public DisplaySettingsNodeAttributeFile {
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
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
      
      // get the columns for palette and color mapping (negative if invalid)
      int getShapeColumnForPaletteAndColorMapping() const;
                                                     
   private:
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
};

#endif


