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

#ifndef __DISPLAY_SETTINGS_CONTOURS_H__
#define __DISPLAY_SETTINGS_CONTOURS_H__

#include "DisplaySettings.h"

/// Display settings for contours
class DisplaySettingsContours : public DisplaySettings {
   public:
      /// drawing mode for contours
      enum DRAW_MODE {
         /// draw as points
         DRAW_MODE_POINTS,
         /// draw as lines
         DRAW_MODE_LINES,
         /// draw as points and lines
         DRAW_MODE_POINTS_AND_LINES
      };
      
      /// Constructor
      DisplaySettingsContours(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsContours();

      /// Reinitialize all display settings
      virtual void reset();
      
      /// Update any selections due to changes in loaded cells
      virtual void update();
      
      /// get draw mode
      DRAW_MODE getDrawMode() const { return drawMode; }
      
      /// set draw mode
      void setDrawMode(const DRAW_MODE dm) { drawMode = dm; }
      
      /// get show end points
      bool getShowEndPoints() const { return showEndPoints; }
      
      /// set show end points
      void setShowEndPoints(const bool sep) { showEndPoints = sep; }
      
      /// get line thickness
      float getLineThickness() const { return drawLineThickness; }

      /// set line thickness
      void setLineThickness(const float ds) { drawLineThickness = ds; }

      /// get point size
      float getPointSize() const { return drawPointSize; }

      /// set  point size
      void setPointSize(const float ds) { drawPointSize = ds; }

      /// get show contour cells
      bool getDisplayContourCells() const { return showContourCells; }
      
      /// set show contour cells
      void setDisplayContourCells(const bool scc) { showContourCells = scc; }
      
      /// determine which contour cells should be displayed
      void determineDisplayedContourCells();
      
      /// get contour cell size
      float getContourCellSize() const { return contourCellSize; }
   
      /// set contour cell size
      void setContourCellSize(const float cs) { contourCellSize = cs; }

      /// get alignment contour valid
      bool getAlignmentContourValid() const { return alignmentContourValid; }

      /// set alignment contour valid
      void setAlignmentContourValid(const bool b) { alignmentContourValid = b; }
      
      /// get alignment contour number
      int getAlignmentContourNumber() const { return alignmentContourNumber; }
      
      /// set alignment contour number
      void setAlignmentContourNumber(const int num) {alignmentContourNumber = num; }
      
      /// get display cross at origin
      bool getDisplayCrossAtOrigin() const { return displayCrossAtOrigin; }
      
      /// set display cross at origin
      void setDisplayCrossAtOrigin(const bool b) { displayCrossAtOrigin = b; }
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                       
   private:
      /// draw as lines
      DRAW_MODE drawMode;
      
      /// show end points
      bool showEndPoints;

      /// drawing line thickness
      float drawLineThickness;
      
      /// drawing point size
      float drawPointSize;
      
      /// show contour cells
      bool showContourCells;

      /// contour cell size
      float contourCellSize;
      
      /// alignment contour valid
      bool alignmentContourValid;
      
      /// alignment contour number
      int alignmentContourNumber;
      
      /// display a cross at the origin
      bool displayCrossAtOrigin;
};

#endif // __DISPLAY_SETTINGS_CONTOURS_H__
