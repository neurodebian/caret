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



#ifndef __VE_DISPLAY_SETTINGS_BORDERS_H__
#define __VE_DISPLAY_SETTINGS_BORDERS_H__

class BrainSet;

#include "ColorFile.h"
#include "DisplaySettings.h"

/// Display settings for all of the border types
class DisplaySettingsBorders : public DisplaySettings {
   public:
      /// how to display the borders
      enum BORDER_DRAW_MODE {
         BORDER_DRAW_AS_SYMBOLS,
         BORDER_DRAW_AS_LINES,
         BORDER_DRAW_AS_SYMBOLS_AND_LINES,
         BORDER_DRAW_AS_UNSTRETCHED_LINES,
         BORDER_DRAW_AS_VARIABILITY,
         BORDER_DRAW_AS_VARIABILITY_AND_LINES
      };
      
      /// Constructor
      DisplaySettingsBorders(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsBorders();
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded borders
      void update();
      
      /// get the draw mode
      BORDER_DRAW_MODE getDrawMode() const { return drawMode; }
      
      /// set the draw mode
      void setDrawMode(const BORDER_DRAW_MODE bdm) { drawMode = bdm; }
      
      /// get the border opacity
      float getOpacity() const { return opacity; }
      
      /// set the border opacity
      void setOpacity(const float o) { opacity = o; }
      
      /// get the symbol type
      ColorFile::ColorStorage::SYMBOL getSymbolType() const { return symbolType; }
      
      /// set the symbol type
      void setSymbolType(const ColorFile::ColorStorage::SYMBOL st) { symbolType = st; }
      
      /// get display borders
      bool getDisplayBorders() const { return displayBorders; }
      
      /// set display borders
      void setDisplayBorders(const bool db) { displayBorders = db; }
      
      /// get display raised flat borders
      bool getDisplayFlatBordersRaised() const { return displayFlatBordersRaised; }
      
      /// set display raised flat borders
      void setDisplayFlatBordersRaised(const bool dfr) { displayFlatBordersRaised = dfr; }
      
      /// get display first link in red
      bool getDisplayFirstLinkRed() const { return displayFirstLinkRed; }
      
      /// set display first link in red
      void setDisplayFirstLinkRed(const bool flr) { displayFirstLinkRed = flr; }
      
      /// get display uncertainty vectors (flat only)
      bool getDisplayUncertaintyVectors() const { return displayFlatUncertaintyVectors; }
      
      /// set display uncertainty vectors (flat only)
      void setDisplayUncertaintyVector(const bool duv) { displayFlatUncertaintyVectors = duv; }
      
      /// get the draw borders as stretch lines stretch factor
      float getDrawAsStretchedLinesStretchFactor() const { return stretchFactor; }
      
      /// set the draw borders as stretch lines stretch factor
      void setDrawAsStretchedLinesStretchFactor(const float sf) { stretchFactor = sf; }
      
      /// get border draw size
      float getDrawSize() const { return borderSize; }
      
      /// set border draw size
      void setDrawSize(const float ds) { borderSize = ds; }
      
      /// set border display flag
      void determineDisplayedBorders();
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       
      /// get override border colors with area colors
      bool getOverrideBorderColorsWithAreaColors() const { return overrideBorderColorsWithAreaColors; }
      
      /// set override border colors with area colors
      void setOverrideBorderColorsWithAreaColors(const bool b) { overrideBorderColorsWithAreaColors = b; }
      
   private:
      /// border draw mode
      BORDER_DRAW_MODE drawMode;
      
      /// display borders
      bool displayBorders;
            
      /// show flat borders raised
      bool displayFlatBordersRaised;
      
      /// show first link in red
      bool displayFirstLinkRed;
      
      /// show flat uncertainty vectors
      bool displayFlatUncertaintyVectors;
      
      /// border draw size
      float borderSize;
      
      /// draw borders as stretch lines stretch factor
      float stretchFactor;
      
      /// override border colors with area colors
      bool overrideBorderColorsWithAreaColors;
      
      /// symbol type
      ColorFile::ColorStorage::SYMBOL symbolType;
      
      /// opacity
      float opacity;
      
};

#endif

