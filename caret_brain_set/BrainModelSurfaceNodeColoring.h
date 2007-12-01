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



#ifndef __VE_BRAIN_SURFACE_NODE_COLORING_H__
#define __VE_BRAIN_SURFACE_NODE_COLORING_H__

#include <set>
#include <QString>
#include <vector>

#include "SceneFile.h"

class BrainModelSurface;
class BrainSet;
class PaintFile;

#include "PaletteFile.h"

/// Class for coloring nodes in a "BrainModelSurface"
class BrainModelSurfaceNodeColoring {
   public:
      /// Primary Overlay Underlay Selections
      enum OVERLAY_SELECTIONS {
         OVERLAY_NONE,
         OVERLAY_AREAL_ESTIMATION,
         OVERLAY_COCOMAC,
         OVERLAY_METRIC,
         OVERLAY_PAINT,
         OVERLAY_PROBABILISTIC_ATLAS,
         OVERLAY_RGB_PAINT,
         OVERLAY_SECTIONS,
         OVERLAY_SHOW_CROSSOVERS,
         OVERLAY_SHOW_EDGES,
         OVERLAY_SURFACE_SHAPE,
         OVERLAY_TOPOGRAPHY,
         OVERLAY_GEOGRAPHY_BLENDING
      };

/*
      /// Primary Overlay Underlay Selections
      enum UNDERLAY_SELECTIONS {
         UNDERLAY_NONE,
         UNDERLAY_AREAL_ESTIMATION,
         UNDERLAY_COCOMAC,
         UNDERLAY_METRIC,
         UNDERLAY_PAINT,
         UNDERLAY_PROBABILISTIC_ATLAS,
         UNDERLAY_RGB_PAINT,
         UNDERLAY_SURFACE_SHAPE,
         UNDERLAY_TOPOGRAPHY,
         UNDERLAY_GEOGRAPHY_BLENDING
      };
*/

      /// Color source for node
      enum NODE_COLOR_SOURCE {
         NODE_COLOR_SOURCE_MEDIAL_WALL_OVERRIDE,
         NODE_COLOR_SOURCE_PRIMARY_OVERLAY,
         NODE_COLOR_SOURCE_SECONDARY_OVERLAY,
         NODE_COLOR_SOURCE_UNDERLAY
      };
      
      /// Constructor
      BrainModelSurfaceNodeColoring(BrainSet* bs);
      
      /// Destructor
      ~BrainModelSurfaceNodeColoring();
      
      /// Assign colors to the Brain Surfaces Nodes
      void assignColors();
      
      /// get the colors for a node
      const unsigned char* getNodeColor(const int model, const int index) const;
      
      /// set the colors for a node
      void setNodeColor(const int model, const int index, const unsigned char rgb[3]);
      
      /// get the color source for a node
      NODE_COLOR_SOURCE getNodeColorSource(const int model, const int index) const;
            
      /// get the geography blending
      float getGeographyBlending() const { return geographyBlending; }
      
      /// set the geography blending
      void setGeographyBlending(const float gb) { geographyBlending = gb; }
      
      /// get the opacity
      float getOpacity() const { return opacity; }
      
      /// set the opacity
      void setOpacity(const float op) { opacity = op; }
      
      /// get the primary overlay selection
      OVERLAY_SELECTIONS getPrimaryOverlay(const int model) const;
      
      /// Set the primary overlay selection.
      /// For the change to take place, updateNodeColors() must also be called.
      void setPrimaryOverlay(const int model, const OVERLAY_SELECTIONS os);
            
      /// get the secondary overlay selection
      OVERLAY_SELECTIONS getSecondaryOverlay(const int model) const;
      
      /// set the secondary overlay selection
      /// For the change to take place, updateNodeColors() must also be called.
      void setSecondaryOverlay(const int model, const OVERLAY_SELECTIONS os);
      
      /// get the underlay selection
      OVERLAY_SELECTIONS getUnderlay(const int model) const;
      
      /// set the underlay selection
      /// For the change to take place, updateNodeColors() must also be called.
      void setUnderlay(const int model, const OVERLAY_SELECTIONS us);
            
      /// get overall lighting on
      bool getLightingOn() const { return lightingOn; }
      
      /// set overall lighting on
      void setLightingOn(const bool l) { lightingOn = l; }
      
      /// get primary overlay lighting on
      bool getPrimaryOverlayLightingOn() const { return primaryOverlayLightingOn; }
      
      /// set primary overlay lighting on
      void setPrimaryOverlayLightingOn(const bool l) { primaryOverlayLightingOn = l; }
      
      /// get secondary overlay lighting on
      bool getSecondaryOverlayLightingOn() const { return secondaryOverlayLightingOn; }
      
      /// set secondary overlay lighting on
      void setSecondaryOverlayLightingOn(const bool l) { secondaryOverlayLightingOn = l; }
      
      /// get underlay lighting on
      bool getUnderlayLightingOn() const { return underlayLightingOn; }
      
      /// set underlay lighting on
      void setUnderlayLightingOn(const bool l) { underlayLightingOn = l; }
      
      /// match paint file names to node colors
      static void matchPaintNamesToNodeColorFile(BrainSet* bs, int paintsNodeColorIndex[],
                                          std::vector<QString>& paintNames);
     
      /// Get the shape look up table
      void getShapeLookupTable(unsigned char lutOut[256][3]) const;

      /// add prob atlas thresholding to paint file
      /// the paint column must exist.
      void addProbAtlasThresholdingToPaintFile(PaintFile* paintFileIn,
                                               const int paintColumn);
                                               
      /// apply a scene (set display settings)
      void showScene(const SceneFile::Scene& scene,
                             QString& errorMessage);
      
      /// create a scene (read display settings)
      void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
               
      /// see if an overlay or underlay is of a specific type
      bool isUnderlayOrOverlay(const OVERLAY_SELECTIONS  ol) const;
                               
   private:
      /// class for storing colors associated with a node
      class NodeColor {
         public:
            int r;
            int g;
            int b;
            
            NodeColor()  { reset(); }
            void reset() { r = g = b = -1; }
      };

      /// coloring applied to node by primary overlay
      NodeColor* overlayPrimaryNodeColors;
      
      /// coloring applied to nodes by secondary overlay
      NodeColor* overlaySecondaryNodeColors;
      
      /// coloring applied to nodes by underlay
      NodeColor* underlayNodeColors;
      
      /// used to help apply colors to nodes
      NodeColor* nodeColors;


      /// brain set being colored (DO NOT "delete" IT !)
      BrainSet* brainSet;
      
      /// default color name
      QString defaultColorName;
      
      /// default color
      unsigned char defaultColor[3];
      
      /// geography blending
      float geographyBlending;
      
      /// node colors
      std::vector<unsigned char> nodeColoring;
      
      /// node color source
      std::vector<NODE_COLOR_SOURCE> nodeColorSource;
      
      /// opacity
      float opacity;
      
      /// primary overlay
      std::vector<OVERLAY_SELECTIONS> primaryOverlay;
      
      /// secondary overlay
      std::vector<OVERLAY_SELECTIONS> secondaryOverlay;
      
      /// underlay
      std::vector<OVERLAY_SELECTIONS> underlay;
      
      /// paint file for saving prob atlas paint assignment
      PaintFile* probAtlasThreshPaintFile;
      
      /// paint file column for saving prob atlas paint assignment
      int probAtlasThreshPaintColumn;
      
      /// overlay lighting on
      bool lightingOn;
      
      /// lighting for primary overlay
      bool primaryOverlayLightingOn;
      
      /// lighting for secondary overlay
      bool secondaryOverlayLightingOn;
      
      /// lighting for underlay
      bool underlayLightingOn;
      
      /// color palette for topography polar angle
      PaletteFile polarAngleTopographyPaletteFile;
      
      /// color palette for topography eccentricity
      PaletteFile eccentricityTopographyPaletteFile;
      
      /// question ??? color index
      int questionColorIndex;
      
      /// number of brain models last time
      int numBrainModelsLastTime;
      
      /// number of nodes last iteration to avoid frequent resizing
      int numNodesLastTime;
      
      /// brain model number for which coloring is being set
      int modelNumber;
      
      /// paint indices with no area color
      std::set<int> paintIndicesWithNoAreaColor;
      
      /// assign medial wall override coloring
      void assignMedialWallOverrideColoring(const int colorOffset,
                                            const int sourceOffset);
      
      /// Assign areal estimation coloring
      void assignArealEstimationColoring();
      
      /// Assign cocomac coloring
      void assignCocomacColoring();
      
      /// Assign crossover coloring
      void assignCrossoverColoring();
      
      /// Assign blend geography coloring
      void assignBlendGeographyColoring(const int offset, const int sourceOffset);
      
      /// Assign edges coloring
      void assignEdgesColoring();
      
      /// Assign metric coloring
      void assignMetricColoring();
      
      /// Assign none coloring
      void assignNoneColoring();
      
      /// assign paint coloring
      void assignPaintColoring();
      
      /// Assign probabilistic coloring to a node.
      void assignProbabilisticColorToNode(const int n, const int paintsAreaColorIndex[]);
                           
      /// Assign probabilistic coloring
      void assignProbabilisticNormalColoring();
      
      /// Assign Threshold Probabilistic coloring
      void assignProbabilisticThresholdColoring();
      
      /// Assign Probabilistic coloring
      void assignProbabilisticColoring(const BrainModelSurface* bms);

      /// Assign RGB Paint coloring
      void assignRgbPaintColoring(const bool underlayFlag);
      
      /// Assign section coloring
      void assignSectionColoring();
      
      /// Surface shape coloring
      void assignSurfaceShapeColoring();
      
      /// Assign topography coloring
      void assignTopographyColoring();
      
      /// Assign topography polar angle coloring palette
      void assignTopographyPolarAnglePalette();
      
      /// Assign topography eccentricity coloring palette
      void assignTopographyEccentricityPalette();
      
      /// clamp a float to int [0, 255]
      int clamp0255(const float v) const;
      
      /// get the LUT index for a value
      unsigned char getLutIndex(const float value, const float dmin, const float dmax) const;
      
      /// set the default color
      void setDefaultColor();
};

#endif


