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



#ifndef __VE_DISPLAY_SETTINGS_SURFACE_H__
#define __VE_DISPLAY_SETTINGS_SURFACE_H__

#include "DisplaySettings.h"

class BrainModelSurface;
class BrainSet;

/// This class maintains parameters for display of surface.
class DisplaySettingsSurface : public DisplaySettings {
   public:
      /// Surface drawing enums.
      enum DRAW_MODE {
          DRAW_MODE_NODES,
          DRAW_MODE_LINKS,
          DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL,
          DRAW_MODE_LINKS_EDGES_ONLY,
          DRAW_MODE_NODES_AND_LINKS,
          DRAW_MODE_TILES,
          DRAW_MODE_TILES_WITH_LIGHT,
          DRAW_MODE_TILES_WITH_LIGHT_NO_BACK,
          DRAW_MODE_TILES_LINKS_NODES,
          DRAW_MODE_NONE
      };
       
      /// the viewing projection
      enum VIEWING_PROJECTION {
         VIEWING_PROJECTION_ORTHOGRAPHIC,
         VIEWING_PROJECTION_PERSPECTIVE
      };
      
      /// coloring for identify node symbols
      enum IDENTIFY_NODE_COLOR {
         IDENTIFY_NODE_COLOR_BLACK,
         IDENTIFY_NODE_COLOR_BLUE,
         IDENTIFY_NODE_COLOR_GREEN,
         IDENTIFY_NODE_COLOR_RED,
         IDENTIFY_NODE_COLOR_WHITE
      };
      
      /// clipping plane axis
      enum CLIPPING_PLANE_AXIS {
         CLIPPING_PLANE_AXIS_X_NEGATIVE = 0,
         CLIPPING_PLANE_AXIS_X_POSITIVE = 1,
         CLIPPING_PLANE_AXIS_Y_NEGATIVE = 2,
         CLIPPING_PLANE_AXIS_Y_POSITIVE = 3,
         CLIPPING_PLANE_AXIS_Z_NEGATIVE = 4,
         CLIPPING_PLANE_AXIS_Z_POSITIVE = 5,
         CLIPPING_PLANE_AXIS_NUMBER_OF = 6
      };

      /// clipping plane application
      enum CLIPPING_PLANE_APPLICATION {
         CLIPPING_PLANE_APPLICATION_MAIN_WINDOW_ONLY,
         CLIPPING_PLANE_APPLICATION_FIDUCIAL_SURFACES_ONLY,
         CLIPPING_PLANE_APPLICATION_ALL_SURFACES,
      };

      /// get the drawing mode
      DRAW_MODE getDrawMode() const { return drawMode; }
      
      /// set the drawing mode
      void setDrawMode(const DRAW_MODE dm);
      
      /// get the viewing projection
      VIEWING_PROJECTION getViewingProjection() const { return viewingProjection; }
      
      /// set the viewing projection
      void setViewingProjection(const VIEWING_PROJECTION vp) { viewingProjection = vp; }
      
      /// Constructor
      DisplaySettingsSurface(BrainSet* bs);
      
      /// Desstructor
      ~DisplaySettingsSurface();
      
      /// get node brightness
      float getNodeBrightness() const { return nodeBrightness; }
      
      /// set node brightness
      void setNodeBrightness(const float bright) { nodeBrightness = bright; }
      
      /// get node contrast
      float getNodeContrast() const { return nodeContrast; }
      
      /// set node contrast
      void setNodeContrast(const float contrast) { nodeContrast = contrast; }
      
      /// get node size
      float getNodeSize() const { return nodeSize; }
      
      /// set node size
      void setNodeSize(const float size) { nodeSize = size; }
      
      /// get link size
      float getLinkSize() const { return linkSize; }
      
      /// set link size
      void setLinkSize(const float size) { linkSize = size; }
      
      /// get show normals
      bool getShowNormals() const { return showNormals; }
      
      /// set show normals
      void setShowNormals(const bool show) { showNormals = show; }
      
      /// get show morphing total forces
      bool getShowMorphingTotalForces() const { return showMorphingTotalForces; }
      
      /// set show morphing total forces
      void setShowMorphingTotalForces(const bool show) { showMorphingTotalForces = show; }
      
      /// get show morphing angular forces
      bool getShowMorphingAngularForces() const { return showMorphingAngularForces; }
      
      /// set show morphing angular forces
      void setShowMorphingAngularForces(const bool show) { showMorphingAngularForces = show; }
      
      /// get show morphing linear forces
      bool getShowMorphingLinearForces() const { return showMorphingLinearForces; }
      
      /// set show morphing linear forces
      void setShowMorphingLinearForces(const bool show) { showMorphingLinearForces = show; }
      
      /// get display length of force vector
      float getForceVectorDisplayLength() const { return forceVectorDisplayLength; }
      
      /// set the display length of the force vector
      void setForceVectorDisplayLength(const float length) { forceVectorDisplayLength = length; }
      
      /// Get show surface axes info
      void getSurfaceAxesInfo(bool& showAxes,
                              bool& showLetters,
                              bool& showHashMarks,
                              float& axesLength,
                              float axesOffset[3]) const;
                          
      /// set show surface axes info
      void setSurfaceAxesInfo(const bool showAxes,
                              const bool showLetters,
                              const bool showHashMarks,
                              const float axesLength,
                              const float axesOffset[3]);
                          
      /// get the identify node color
      IDENTIFY_NODE_COLOR getIdentifyNodeColor() const { return identifyNodeColor; }
      
      /// set the identify node color
      void setIdentifyNodeColor(const IDENTIFY_NODE_COLOR idc) { identifyNodeColor = idc; }
      
      /// get the opacity
      float getOpacity() const { return opacity; }
      
      /// set the opacity
      void setOpacity(const float opacityIn) { opacity = opacityIn; };

      /// get a clipping plane coordinate
      float getClippingPlaneCoordinate(const CLIPPING_PLANE_AXIS planeAxis) const;

      /// set a clipping plane coordinate
      void setClippingPlaneCoordinate(const CLIPPING_PLANE_AXIS planeAxis,
                                      const float coordinateValue);

      /// get clipping plane enabled
      bool getClippingPlaneEnabled(const CLIPPING_PLANE_AXIS planeAxis) const;

      /// set a clipping plane coordinate
      void setClippingPlaneEnabled(const CLIPPING_PLANE_AXIS planeAxis,
                                   const bool enabled);

      /// get the clipping plane application
      CLIPPING_PLANE_APPLICATION getClippingPlaneApplication() const { return clippingPlaneApplication; }

      /// set the clipping plane application
      void setClippingPlaneApplication(const CLIPPING_PLANE_APPLICATION cpa) { clippingPlaneApplication = cpa; }

      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded surface shape file
      void update();
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       
   private:
      /// the drawing mode
      DRAW_MODE drawMode;
      
      /// node brightness
      float nodeBrightness;
      
      /// node contrast
      float nodeContrast;
      
      /// node size
      float nodeSize;
      
      /// link size
      float linkSize;
      
      /// display length of a force vector
      float forceVectorDisplayLength;
      
      /// show normals
      bool showNormals;
      
      /// show morphing total forces
      bool showMorphingTotalForces;
      
      /// show morphing angular forces
      bool showMorphingAngularForces;
      
      /// show morphing linear forces
      bool showMorphingLinearForces;
    
      /// the viewing projection
      VIEWING_PROJECTION viewingProjection;
      
      /// show surface axes
      bool showSurfaceAxes;
      
      /// show surface axes letters on surface axes
      bool showSurfaceAxesLetters;
      
      /// show surface axes hash marks
      bool showSurfaceAxesHashMarks;
      
      /// surface axes length
      float surfaceAxesLength;
      
      /// surface axis offset
      float surfaceAxesOffset[3];
      
      /// identify node color
      IDENTIFY_NODE_COLOR identifyNodeColor;
      
      /// surface opacity
      float opacity;

      /// clipping plane coordinate along each axis (negative and positive)
      float clippingPlaneCoordinate[CLIPPING_PLANE_AXIS_NUMBER_OF];

      /// clipping plane enabled for each axis (negative and positive)
      bool clippingPlaneEnabled[CLIPPING_PLANE_AXIS_NUMBER_OF];

      /// how the clipping planes are applied
      CLIPPING_PLANE_APPLICATION clippingPlaneApplication;
};

#endif // __VE_DISPLAY_SETTINGS_SURFACE_H__

