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



#ifndef __VE_DISPLAY_SETTINGS_CELLS_H__
#define __VE_DISPLAY_SETTINGS_CELLS_H__

#include "CellBase.h"
#include "CellColorFile.h"
#include "DisplaySettings.h"

class BrainSet;
class CellProjection;

/// Display settings for cells and cell projections
class DisplaySettingsCells : public DisplaySettings {
   public:
      
      /// Display Mode
      enum CELL_DISPLAY_MODE {
         CELL_DISPLAY_MODE_SHOW_ALL,
         CELL_DISPLAY_MODE_SHOW_DEEP_ONLY,
         CELL_DISPLAY_MODE_SHOW_SUPERFICIAL_ONLY
      };
      
      /// Constructor
      DisplaySettingsCells(BrainSet* bs);
      
      /// Destructor
      virtual ~DisplaySettingsCells();
      
      /// Reinitialize all display settings
      virtual void reset();
      
      /// Update any selections due to changes in loaded cells
      virtual void update();
      
      /// get display cells
      bool getDisplayCells() const { return displayCells; }
      
      /// set display cells
      void setDisplayCells(const bool dfc) { displayCells = dfc; }
      
      /// get display volume cells
      bool getDisplayVolumeCells() const { return displayVolumeCells; }
      
      /// set display volume cells
      void setDisplayVolumeCells(const bool dfc) { displayVolumeCells = dfc; }
      
      /// get display cell raised on flat surface
      bool getDisplayFlatCellsRaised() const { return displayFlatCellsRaised; }
      
      /// set dislay cells raised on flat surface
      void setDisplayFlatCellsRaised(const bool fsr) { displayFlatCellsRaised = fsr; }
     
      /// get display cells pasted onto 3D surfaces
      bool getDisplayPasteCellsOnto3D() const { return displayPasteCellsOnto3D; }
      
      /// set display cells pasted onto 3D surface
      void setDisplayPasteCellsOnto3D(const bool d3d) { displayPasteCellsOnto3D = d3d; }
      
      /// get the symbol override
      ColorFile::ColorStorage::SYMBOL getSymbolOverride() const { return symbolOverride; }
      
      /// set the symbol override
      void setSymbolOverride(const ColorFile::ColorStorage::SYMBOL so) { symbolOverride = so; }
      
      /// get cell draw size
      float getDrawSize() const { return cellSize; }
      
      /// set cell draw size
      void setDrawSize(const float ds) { cellSize = ds; }
      
      /// set display cells only on the correct hemisphere
      void setDisplayCellsOnCorrectHemisphereOnly(const bool b)
              { displayCellsOnCorrectHemisphereOnly = b; }
      
      /// ge display cells only on the correct hemisphere
      bool getDisplayCellsOnCorrectHemisphereOnly() const 
              { return displayCellsOnCorrectHemisphereOnly; }
      
      /// get the distance to surface limit
      float getDistanceToSurfaceLimit() const { return distanceToSurfaceLimit; }
      
      /// set the distance to surface limit
      void setDistanceToSurfaceLimit(const float dist) { distanceToSurfaceLimit = dist; }
      
      /// get the border opacity
      float getOpacity() const { return opacity; }
      
      /// set the border opacity
      void setOpacity(const float o) { opacity = o; }
      
      /// get the display mode
      CELL_DISPLAY_MODE getDisplayMode() const { return cellDisplayMode; }
      
      /// set the display mode
      void setDisplayMode(const CELL_DISPLAY_MODE cdm) { cellDisplayMode = cdm; }
      
      /// determine which cells should be displayed (set's cell display flags)
      void determineDisplayedCells(const bool fociFlag = false);
      
      /// determine which volume cells should be displayed (set's cell display flags)
      void determineDisplayedVolumeCells(const bool fociFlag = false);
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                 
      /// get the coloring mode
      CellBase::CELL_COLOR_MODE getColorMode() const { return cellColorMode; }
      
      /// set the coloring mode
      void setColorMode(const CellBase::CELL_COLOR_MODE ccm) { cellColorMode = ccm; }
      
      /// get display cells without class assignments
      bool getDisplayCellsWithoutClassAssignments() const 
           { return displayCellsWithoutClassAssignments; }
      
      /// set display cells without class assignments
      void setDisplayCellsWithoutClassAssignments(const bool b)
           { displayCellsWithoutClassAssignments = b; }
      
      /// get display cells without matching color
      bool getDisplayCellsWithoutMatchingColor() const
           { return displayCellsWithoutMatchingColor; }
      
      /// set display cells without matching color
      void setDisplayCellsWithoutMatchingColor(const bool b)
           { displayCellsWithoutMatchingColor = b; }
      
      /// get display cells without a link to a study with keywords
      bool getDisplayCellsWithoutLinkToStudyWithKeywords() const 
           { return displayCellsWithoutLinkToStudyWithKeywords; }
      
      /// set display cells without a link to a study with keywords
      void setDisplayCellsWithoutLinkToStudyWithKeywords(const bool b)
           { displayCellsWithoutLinkToStudyWithKeywords = b; }
      
      /// get display cells without a link to a table subheader
      bool getDisplayCellsWithoutLinkToStudyWithTableSubHeader() const 
           { return displayCellsWithoutLinkToStudyWithTableSubHeader; }
      
      /// set display cells without a link to a table subheader
      void setDisplayCellsWithoutLinkToStudyWithTableSubHeader(const bool b)
           { displayCellsWithoutLinkToStudyWithTableSubHeader = b; }

   protected:
      /// display cells
      bool displayCells;
      
      /// display volume cells
      bool displayVolumeCells;
      
      /// show cells raised on flat surface
      bool displayFlatCellsRaised;
      
      /// paste cells onto 3D surfaces
      bool displayPasteCellsOnto3D;
      
      /// symbol override
      ColorFile::ColorStorage::SYMBOL symbolOverride;
      
      /// draw size
      float cellSize;
      
      /// display cells only on the correct hemisphere
      bool displayCellsOnCorrectHemisphereOnly;
      
      /// limit display to cells within distance to surface
      float distanceToSurfaceLimit;
      
      /// cell display mode
      CELL_DISPLAY_MODE cellDisplayMode;

      /// cell coloring mode
      CellBase::CELL_COLOR_MODE cellColorMode;
      
      /// opacity
      float opacity;
      
      /// display cells without class assignments
      bool displayCellsWithoutClassAssignments;
      
      /// display cells without matching color
      bool displayCellsWithoutMatchingColor;
      
      /// display cells without a link to a study with keywords
      bool displayCellsWithoutLinkToStudyWithKeywords;
      
      /// display cells without a link to a table subheader
      bool displayCellsWithoutLinkToStudyWithTableSubHeader;
};

#endif

