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

#ifndef __DISPLAY_SETTINGS_MODELS_H__
#define __DISPLAY_SETTINGS_MODELS_H__

#include "DisplaySettings.h"

/// Display settings for models
class DisplaySettingsModels : public DisplaySettings {
   public:
      /// Constructor
      DisplaySettingsModels(BrainSet* bsIn);
      
      /// Destructor
      ~DisplaySettingsModels();

      /// reinitialize all display settings
      void reset();
      
      /// update any selections due to changes with loaded data files
      void update();
   
      /// get line width
      float getLineWidth() const { return lineWidth; }
      
      /// set line width
      void setLineWidth(const float lw) { lineWidth = lw; }
      
      /// get vertex size
      float getVertexSize() const { return vertexSize; }
      
      /// set vertex size
      void setVertexSize(const float vs) { vertexSize = vs; }
      
      /// get light enabled for vertices
      bool getLightVerticesEnabled() const { return lightVertices; }
      
      /// set light enabled for vertices
      void setLightVerticesEnabled(const bool le) { lightVertices = le; }
      
      /// get light enabled for lines
      bool getLightLinesEnabled() const { return lightLines; }
      
      /// set light enabled for lines
      void setLightLinesEnabled(const bool le) { lightLines = le; }
      
      /// get light enabled for polygons
      bool getLightPolygonsEnabled() const { return lightPolygons; }
      
      /// set light enabled for polygons
      void setLightPolygonsEnabled(const bool le) { lightPolygons = le; }
      
      /// get the opacity
      float getOpacity() const { return opacity; }
      
      /// set the opacity
      void setOpacity(const float op) { opacity = op; }
      
      /// get show polygons
      bool getShowPolygons() const { return showPolygons; }
      
      /// set show polygons
      void setShowPolygons(const bool b) { showPolygons = b; }
      
      /// get show triangles
      bool getShowTriangles() const { return showTriangles; }
      
      /// set show triangles
      void setShowTriangles(const bool b) { showTriangles = b; }
      
      /// get show lines
      bool getShowLines() const { return showLines; }
      
      /// set show lines
      void setShowLines(const bool b) { showLines = b; }
      
      /// get show vertices
      bool getShowVertices() const { return showVertices; }
      
      /// set show vertices
      void setShowVertices(const bool b) { showVertices = b; }
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       
   private:
      /// opacity
      float opacity;
      
      /// line width
      float lineWidth;
      
      /// vertex size
      float vertexSize;
      
      /// light enable for 
      bool lightVertices;
      
      /// light enable for 
      bool lightLines;
      
      /// light enable for 
      bool lightPolygons;
      
      /// show polygons
      bool showPolygons;
      
      /// show triangles
      bool showTriangles;
      
      /// show lines
      bool showLines;
      
      /// show vertices
      bool showVertices;
      
};

#endif // __DISPLAY_SETTINGS_MODELS_H__

