
#ifndef __DISPLAY_SETTINGS_GEODESIC_DISTANCE_H__
#define __DISPLAY_SETTINGS_GEODESIC_DISTANCE_H__

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

/// class for display control of geodesic distance
class DisplaySettingsGeodesicDistance : public DisplaySettings {
   public:
      /// Constructor
      DisplaySettingsGeodesicDistance(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsGeodesicDistance();
      
      /// get the selected display column
      int getDisplayColumn() const { return displayColumn; }
      
      /// set the selected display column
      void setDisplayColumn(const int col) { displayColumn = col; }
      
      /// get geodesic path display enabled
      bool getPathDisplayEnabled() const { return pathDisplayEnabled; }
      
      /// set geodesic path display enabled
      void setPathDisplayEnabled(const bool de) { pathDisplayEnabled = de; }
      
      /// get the path display node number
      int getPathDisplayNodeNumber() const { return pathDisplayNodeNumber; }
  
      /// set the path display node number
      void setPathDisplayNodeNumber(const int dnn) { pathDisplayNodeNumber = dnn; }

      /// show the root node
      bool getShowRootNode() const { return showRootNode; }
      
      /// set show the root node
      void setShowRootNode(const bool b) { showRootNode = b; }
      
      /// get the path line width
      int getPathLineWidth() const { return lineWidth; }
      
      /// set the path line width
      void setPathLineWidth(const int w) { lineWidth = w; }
      
      /// reinitialize all display settings
      virtual void reset();
      
      /// update any selections due to changes with loaded data files
      virtual void update();
   
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                       
   protected:
      /// selected display column
      int displayColumn;
      
      /// path display node number
      int pathDisplayNodeNumber;

      /// path display enabled
      bool pathDisplayEnabled;
      
      /// show the root node
      bool showRootNode;
      
      /// line width
      int lineWidth;
};

#endif // __DISPLAY_SETTINGS_GEODESIC_DISTANCE_H__

