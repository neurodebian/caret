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
      /// coloring mode
      enum COLORING_MODE {
         /// coloring mode normal
         COLORING_MODE_NORMAL,
         /// coloring mode blend overlays
         COLORING_MODE_OVERLAY_BLENDING
      };
      
      /// Constructor
      BrainModelSurfaceNodeColoring(BrainSet* bs);
      
      /// Destructor
      ~BrainModelSurfaceNodeColoring();
      
      /// Assign colors to the Brain Surfaces Nodes
      void assignColors();
      
      /// get the coloring mode
      COLORING_MODE getColoringMode() const { return coloringMode; }
      
      /// set the coloring mode
      void setColoringMode(const COLORING_MODE mode) { coloringMode = mode; }
      
      /// get the colors for a node
      const unsigned char* getNodeColor(const int model, const int index) const;
      
      /// set the colors for a node
      void setNodeColor(const int model, const int index, 
                        const unsigned char rgb[3],
                        const unsigned char alpha = 255);
      
      /// get the color source for a node
      int getNodeColorSource(const int model, const int index) const;
            
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
               
   private:
      /// class for storing colors associated with a node
      class NodeColor {
         public:
            int r;
            int g;
            int b;
            int a;
            
            NodeColor()  { reset(); }
            void reset() { r = g = b = -1; a = 1.0; }
            inline bool isValid() const { return ((r >= 0) || (g >= 0) || (b >= 0)); }
      };

      /// the coloring mode
      COLORING_MODE coloringMode;
      
      /// used to help apply colors to nodes
      std::vector<NodeColor> nodeColors;

      /// brain set being colored (DO NOT "delete" IT !)
      BrainSet* brainSet;
      
      /// default color name
      QString defaultColorName;
      
      /// default color
      unsigned char defaultColor[3];
      
      /// node colors
      std::vector<unsigned char> nodeColoring;
      
      /// node color source
      std::vector<int> nodeColorSource;
      
      /// paint file for saving prob atlas paint assignment
      PaintFile* probAtlasThreshPaintFile;
      
      /// paint file column for saving prob atlas paint assignment
      int probAtlasThreshPaintColumn;
      
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
      void assignArealEstimationColoring(const int overlayNumber);
      
      /// Assign cocomac coloring
      void assignCocomacColoring();
      
      /// Assign crossover coloring
      void assignCrossoverColoring();
      
      /// Assign blend geography coloring
      void assignBlendGeographyColoring(const int offset, const int sourceOffset);
      
      /// Assign edges coloring
      void assignEdgesColoring();
      
      /// Assign metric coloring
      void assignMetricColoring(const int overlayNumber);
      
      /// Assign none coloring
      void assignNoneColoring(const int nodeColoringOffset,
                              const int nodeColorSourceOffset);
      
      /// assign paint coloring
      void assignPaintColoring(const int overlayNumber);
      
      /// Assign probabilistic coloring to a node.
      void assignProbabilisticColorToNode(const int n, const int paintsAreaColorIndex[]);
                           
      /// Assign probabilistic coloring
      void assignProbabilisticNormalColoring();
      
      /// Assign Threshold Probabilistic coloring
      void assignProbabilisticThresholdColoring();
      
      /// Assign Probabilistic coloring
      void assignProbabilisticColoring(const BrainModelSurface* bms);

      /// Assign RGB Paint coloring
      void assignRgbPaintColoring(const int overlayNumber,
                                  const bool underlayFlag);
      
      /// Assign section coloring
      void assignSectionColoring(const int overlayNumber);
      
      /// Surface shape coloring
      void assignSurfaceShapeColoring(const int overlayNumber);
      
      /// Assign topography coloring
      void assignTopographyColoring(const int overlayNumber);
      
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


