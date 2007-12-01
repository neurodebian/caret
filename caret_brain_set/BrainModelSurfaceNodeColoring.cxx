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

#include <QDateTime>

#include <iostream>
#include <limits>
#include <map>
#include <set>

#include "AreaColorFile.h"
#include "ArealEstimationFile.h"
#include "BrainModelVolume.h"
#include "CocomacConnectivityFile.h"
#include "ColorFile.h"
#include "DebugControl.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsCoCoMac.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsSurface.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsSection.h"
#include "DisplaySettingsSurface.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsTopography.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "RgbPaintFile.h"
#include "SectionFile.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopographyFile.h"
#include "VolumeFile.h"

#include "vtkPiecewiseFunction.h"

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"

//---------------------------------------------------------------------------

static unsigned char lutOrangeYellow[256][3] = {
   { 0, 0, 0 },
   { 2, 0, 0 },
   { 4, 0, 0 },
   { 6, 0, 0 },
   { 8, 0, 0 },
   { 10, 0, 0 },
   { 12, 0, 0 },
   { 14, 0, 0 },
   { 16, 0, 0 },
   { 18, 0, 0 },
   { 20, 0, 0 },
   { 22, 0, 0 },
   { 24, 0, 0 },
   { 26, 0, 0 },
   { 28, 0, 0 },
   { 30, 0, 0 },
   { 32, 0, 0 },
   { 34, 0, 0 },
   { 36, 0, 0 },
   { 38, 0, 0 },
   { 40, 0, 0 },
   { 42, 0, 0 },
   { 44, 0, 0 },
   { 46, 0, 0 },
   { 48, 0, 0 },
   { 50, 0, 0 },
   { 52, 0, 0 },
   { 54, 0, 0 },
   { 56, 0, 0 },
   { 58, 0, 0 },
   { 60, 0, 0 },
   { 62, 0, 0 },
   { 64, 0, 0 },
   { 66, 0, 0 },
   { 68, 0, 0 },
   { 70, 0, 0 },
   { 72, 0, 0 },
   { 74, 0, 0 },
   { 76, 0, 0 },
   { 78, 0, 0 },
   { 80, 0, 0 },
   { 82, 0, 0 },
   { 84, 0, 0 },
   { 86, 0, 0 },
   { 88, 0, 0 },
   { 90, 0, 0 },
   { 92, 0, 0 },
   { 94, 0, 0 },
   { 96, 0, 0 },
   { 98, 0, 0 },
   { 100, 0, 0 },
   { 102, 0, 0 },
   { 104, 0, 0 },
   { 106, 0, 0 },
   { 108, 0, 0 },
   { 110, 0, 0 },
   { 112, 0, 0 },
   { 114, 0, 0 },
   { 116, 0, 0 },
   { 118, 0, 0 },
   { 120, 0, 0 },
   { 122, 0, 0 },
   { 124, 0, 0 },
   { 126, 0, 0 },
   { 128, 0, 0 },
   { 130, 2, 0 },
   { 132, 4, 0 },
   { 134, 6, 0 },
   { 136, 8, 0 },
   { 138, 10, 0 },
   { 140, 12, 0 },
   { 142, 14, 0 },
   { 144, 16, 0 },
   { 146, 18, 0 },
   { 148, 20, 0 },
   { 150, 22, 0 },
   { 152, 24, 0 },
   { 154, 26, 0 },
   { 156, 28, 0 },
   { 158, 30, 0 },
   { 160, 32, 0 },
   { 162, 34, 0 },
   { 164, 36, 0 },
   { 166, 38, 0 },
   { 168, 40, 0 },
   { 170, 42, 0 },
   { 172, 44, 0 },
   { 174, 46, 0 },
   { 176, 48, 0 },
   { 178, 50, 0 },
   { 180, 52, 0 },
   { 182, 54, 0 },
   { 184, 56, 0 },
   { 186, 58, 0 },
   { 188, 60, 0 },
   { 190, 62, 0 },
   { 192, 64, 0 },
   { 194, 66, 0 },
   { 196, 68, 0 },
   { 198, 70, 0 },
   { 200, 72, 0 },
   { 202, 74, 0 },
   { 204, 76, 0 },
   { 206, 78, 0 },
   { 208, 80, 0 },
   { 210, 82, 0 },
   { 212, 84, 0 },
   { 214, 86, 0 },
   { 216, 88, 0 },
   { 218, 90, 0 },
   { 220, 92, 0 },
   { 222, 94, 0 },
   { 224, 96, 0 },
   { 226, 98, 0 },
   { 228, 100, 0 },
   { 230, 102, 0 },
   { 232, 104, 0 },
   { 234, 106, 0 },
   { 236, 108, 0 },
   { 238, 110, 0 },
   { 240, 112, 0 },
   { 242, 114, 0 },
   { 244, 116, 0 },
   { 246, 118, 0 },
   { 248, 120, 0 },
   { 250, 122, 0 },
   { 252, 124, 0 },
   { 254, 126, 0 },
   { 254, 128, 0 },
   { 254, 130, 2 },
   { 254, 132, 4 },
   { 254, 134, 6 },
   { 254, 136, 8 },
   { 254, 138, 10 },
   { 254, 140, 12 },
   { 254, 142, 14 },
   { 254, 144, 16 },
   { 254, 146, 18 },
   { 254, 148, 20 },
   { 254, 150, 22 },
   { 254, 152, 24 },
   { 254, 154, 26 },
   { 254, 156, 28 },
   { 254, 158, 30 },
   { 254, 160, 32 },
   { 254, 162, 34 },
   { 254, 164, 36 },
   { 254, 166, 38 },
   { 254, 168, 40 },
   { 254, 170, 42 },
   { 254, 172, 44 },
   { 254, 174, 46 },
   { 254, 176, 48 },
   { 254, 178, 50 },
   { 254, 180, 52 },
   { 254, 182, 54 },
   { 254, 184, 56 },
   { 254, 186, 58 },
   { 254, 188, 60 },
   { 254, 190, 62 },
   { 254, 192, 64 },
   { 254, 194, 66 },
   { 254, 196, 68 },
   { 254, 198, 70 },
   { 254, 200, 72 },
   { 254, 202, 74 },
   { 254, 204, 76 },
   { 254, 206, 78 },
   { 254, 208, 80 },
   { 254, 210, 82 },
   { 254, 212, 84 },
   { 254, 214, 86 },
   { 254, 216, 88 },
   { 254, 218, 90 },
   { 254, 220, 92 },
   { 254, 222, 94 },
   { 254, 224, 96 },
   { 254, 226, 98 },
   { 254, 228, 100 },
   { 254, 230, 102 },
   { 254, 232, 104 },
   { 254, 234, 106 },
   { 254, 236, 108 },
   { 254, 238, 110 },
   { 254, 240, 112 },
   { 254, 242, 114 },
   { 254, 244, 116 },
   { 254, 246, 118 },
   { 254, 248, 120 },
   { 254, 250, 122 },
   { 254, 252, 124 },
   { 254, 254, 126 },
   { 254, 254, 128 },
   { 254, 254, 130 },
   { 254, 254, 132 },
   { 254, 254, 134 },
   { 254, 254, 136 },
   { 254, 254, 138 },
   { 254, 254, 140 },
   { 254, 254, 142 },
   { 254, 254, 144 },
   { 254, 254, 146 },
   { 254, 254, 148 },
   { 254, 254, 150 },
   { 254, 254, 152 },
   { 254, 254, 154 },
   { 254, 254, 156 },
   { 254, 254, 158 },
   { 254, 254, 160 },
   { 254, 254, 162 },
   { 254, 254, 164 },
   { 254, 254, 166 },
   { 254, 254, 168 },
   { 254, 254, 170 },
   { 254, 254, 172 },
   { 254, 254, 174 },
   { 254, 254, 176 },
   { 254, 254, 178 },
   { 254, 254, 180 },
   { 254, 254, 182 },
   { 254, 254, 184 },
   { 254, 254, 186 },
   { 254, 254, 188 },
   { 254, 254, 190 },
   { 254, 254, 192 },
   { 254, 254, 194 },
   { 254, 254, 196 },
   { 254, 254, 198 },
   { 254, 254, 200 },
   { 254, 254, 202 },
   { 254, 254, 204 },
   { 254, 254, 206 },
   { 254, 254, 208 },
   { 254, 254, 210 },
   { 254, 254, 212 },
   { 254, 254, 214 },
   { 254, 254, 216 },
   { 254, 254, 218 },
   { 254, 254, 220 },
   { 254, 254, 222 },
   { 254, 254, 224 },
   { 254, 254, 226 },
   { 254, 254, 228 },
   { 254, 254, 230 },
   { 254, 254, 232 },
   { 254, 254, 234 },
   { 254, 254, 236 },
   { 254, 254, 238 },
   { 254, 254, 240 },
   { 254, 254, 242 },
   { 254, 254, 244 },
   { 254, 254, 246 },
   { 254, 254, 248 },
   { 254, 254, 250 },
   { 254, 254, 252 },
   { 254, 254, 254 }
};

/**
 * The constructor
 */
BrainModelSurfaceNodeColoring::BrainModelSurfaceNodeColoring(BrainSet* bs)
{
   overlayPrimaryNodeColors  = NULL;
   overlaySecondaryNodeColors  = NULL;
   underlayNodeColors = NULL;
   nodeColors = NULL;
   
   brainSet = bs;
   primaryOverlay.clear();
   secondaryOverlay.clear();
   underlay.clear();
   defaultColor[0] = 100;
   defaultColor[1] = 100;
   defaultColor[2] = 100;
   defaultColorName = "???";
   geographyBlending = 0.6;
   opacity = 1.0;
   lightingOn = true;
   primaryOverlayLightingOn = true;
   secondaryOverlayLightingOn = true;
   underlayLightingOn = true;
   lightingOn = true;
   numNodesLastTime = -1;
   numBrainModelsLastTime = -1;
   
   probAtlasThreshPaintFile = NULL;
   
   assignTopographyEccentricityPalette();
   assignTopographyPolarAnglePalette();
}

/**
 * The destructor.
 */
BrainModelSurfaceNodeColoring::~BrainModelSurfaceNodeColoring()
{
}
 
/**
 * Get the shape look up table
 */
void
BrainModelSurfaceNodeColoring::getShapeLookupTable(unsigned char lutOut[256][3]) const
{
   for (int i = 0; i < 256; i++) {
      for (int j = 0; j < 3; j++) {
         lutOut[i][j] = lutOrangeYellow[i][j];
      }
   }
}

/**
 * find color for ??? 
 */
void
BrainModelSurfaceNodeColoring::setDefaultColor()
{
   const AreaColorFile* cf = brainSet->getAreaColorFile();
   
   //
   // avoid search if default color index has not changed
   //
   bool match;
   unsigned char r, g, b;
   questionColorIndex = cf->getColorByName(defaultColorName, match, r, g, b);
   if (questionColorIndex >= 0) {
      defaultColor[0] = r;
      defaultColor[1] = g;
      defaultColor[2] = b;
   }
   else {
      defaultColor[0] = 100;
      defaultColor[1] = 100;
      defaultColor[2] = 100;
   }
}

/**
 * get the colors for a node.
 */
const unsigned char* 
BrainModelSurfaceNodeColoring::getNodeColor(const int model, const int indexIn) const 
{
   const int index = (model * numNodesLastTime * 3) + (indexIn * 3);
   return &nodeColoring[index];
}
      
/**
 * get the color source for a node.
 */
BrainModelSurfaceNodeColoring::NODE_COLOR_SOURCE 
BrainModelSurfaceNodeColoring::getNodeColorSource(const int model, const int indexIn) const 
{
   const int index = (model * numNodesLastTime) + indexIn;
   return nodeColorSource[index];
}

/**
 * Set the colors for a node.
 */
void
BrainModelSurfaceNodeColoring::setNodeColor(const int model,
                                            const int index, const unsigned char rgb[3])
{
   const int numNodes = brainSet->getNumberOfNodes();
   nodeColoring[model * numNodes * 3 + index * 3]     = rgb[0];
   nodeColoring[model * numNodes * 3 + index * 3 + 1] = rgb[1];
   nodeColoring[model * numNodes * 3 + index * 3 + 2] = rgb[2];
}

/**
 * Get the lookup table index for a value between (dmin, dmax)
 */
unsigned char     
BrainModelSurfaceNodeColoring::getLutIndex(const float value, const float dmin, const float dmax) const
{
   if (value < dmin) return 0;
   if (value > dmax) return 255;
   
   const float lutScale = 255.0/(dmax-dmin);
   int colorIndex = (int)(lutScale * (value - dmin));
   if (colorIndex < 0) colorIndex = 0;
   if (colorIndex > 255) colorIndex = 255;
   return (unsigned char)colorIndex;
}

/**
 * Assign surface painting for NONE
 */
void
BrainModelSurfaceNodeColoring::assignNoneColoring()
{
   const int numNodes = brainSet->getNumberOfNodes();
   for (register int i = 0; i < numNodes; i++) {
      nodeColors[i].r = defaultColor[0];
      nodeColors[i].g = defaultColor[1];
      nodeColors[i].b = defaultColor[2];
   }
}

/**
 * Assign RGB Paint Coloring.
 */
void
BrainModelSurfaceNodeColoring::assignRgbPaintColoring(const bool underlayFlag)
{
   DisplaySettingsRgbPaint* dsrp = brainSet->getDisplaySettingsRgbPaint();
   RgbPaintFile* rgbPaintFile = brainSet->getRgbPaintFile();

   if (rgbPaintFile->getNumberOfColumns() == 0) {
      return;
   }   
   
   const int numNodes = rgbPaintFile->getNumberOfNodes();
   if (numNodes == 0) {
      return;
   }
   
   if (numNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "RGB Paint has different number of nodes than surface." << std::endl;
      return;
   }
   const int column = dsrp->getSelectedColumn(modelNumber);
   if ((column < 0) || (column >= rgbPaintFile->getNumberOfColumns())) {
      std::cout << "Invalid RGB Paint column selected." << std::endl;
      return;
   }
   
   //
   // Note: Threshold values are always in the range 0 to 255.
   //
   float redThresh, greenThresh, blueThresh;
   dsrp->getThresholds(redThresh, greenThresh, blueThresh);
   
   const bool positiveDisplay =
       (dsrp->getDisplayMode() == DisplaySettingsRgbPaint::RGB_DISPLAY_MODE_POSITIVE);
   
   float redMaxValue, redMinValue;
   rgbPaintFile->getScaleRed(column, redMinValue, redMaxValue);
   vtkPiecewiseFunction* redPF = vtkPiecewiseFunction::New();
   if (positiveDisplay) {
      redPF->AddSegment(0.0, 0, redMaxValue, 255);
   }
   else {
      redPF->AddSegment(0.0, 0, redMinValue, 255);
   }
   redPF->ClampingOn();
   
   float greenMaxValue, greenMinValue;
   rgbPaintFile->getScaleGreen(column, greenMinValue, greenMaxValue);
   vtkPiecewiseFunction* greenPF = vtkPiecewiseFunction::New();
   if (positiveDisplay) {
      greenPF->AddSegment(0.0, 0, greenMaxValue, 255);
   }
   else {
      greenPF->AddSegment(0.0, 0, greenMinValue, 255);
   }
   greenPF->ClampingOn();
   
   float blueMaxValue, blueMinValue;
   rgbPaintFile->getScaleBlue(column, blueMinValue, blueMaxValue);
   vtkPiecewiseFunction* bluePF = vtkPiecewiseFunction::New();
   if (positiveDisplay) {
      bluePF->AddSegment(0.0, 0, blueMaxValue, 255);
   }
   else {
      bluePF->AddSegment(0.0, 0, blueMinValue, 255);
   }
   bluePF->ClampingOn();
   
   for (register int i = 0; i < numNodes; i++) {
      float r, g, b;
      rgbPaintFile->getRgb(i, column, r, g, b);
      
      if (positiveDisplay) {
         if (underlayFlag) {
            redThresh   = -1.0;
            greenThresh = -1.0;
            blueThresh  = -1.0;
         }
         if (dsrp->getRedEnabled()) {
            if (r >= 0.0) {
               if (r > redThresh) {
                  nodeColors[i].r = (unsigned char)redPF->GetValue(r);
               }
            }
         }
         if (dsrp->getGreenEnabled()) {
            if (g >= 0.0) {
               if (g > greenThresh) {
                  nodeColors[i].g = (unsigned char)greenPF->GetValue(g);
               }
            }
         }
         if (dsrp->getBlueEnabled()) {
            if (b >= 0.0) {
               if (b > blueThresh) {
                  nodeColors[i].b = (unsigned char)bluePF->GetValue(b);
               }
            }
         }
      }
      else {
         if (underlayFlag) {
            redThresh   = -1.0;
            greenThresh = -1.0;
            blueThresh  = -1.0;
         }
         if (dsrp->getRedEnabled()) {
            if (r <= 0.0) {
               r = -r;
               if (r > redThresh) {
                  nodeColors[i].r = (unsigned char)redPF->GetValue(-r);
               }
            }
         }
         if (dsrp->getGreenEnabled()) {
            if (g <= 0.0) {
               g = -g;
               if (g > greenThresh) {
                  nodeColors[i].g = (unsigned char)greenPF->GetValue(-g);
               }
            }
         }
         if (dsrp->getBlueEnabled()) {
            if (b <= 0.0) {
               b = -b;
               if (b > blueThresh) {
                  nodeColors[i].b = (unsigned char)bluePF->GetValue(-b);
               }
            }
         }
      }
      
      //
      // If any component is set, make sure any unset components are set 
      // to zero (all components are -1 upon entry to this function).
      //
      if ((nodeColors[i].r >= 0) ||
          (nodeColors[i].g >= 0) ||
          (nodeColors[i].b >= 0)) {
         if (nodeColors[i].r < 0) {
            nodeColors[i].r = 0;
         }
         if (nodeColors[i].g < 0) {
            nodeColors[i].g = 0;
         }
         if (nodeColors[i].b < 0) {
            nodeColors[i].b = 0;
         }
      }
   }
}

/**
 * Assign areal estimation coloring.
 */
void
BrainModelSurfaceNodeColoring::assignArealEstimationColoring()
{
   DisplaySettingsArealEstimation* dsae = brainSet->getDisplaySettingsArealEstimation();
   const int columnNumber = dsae->getSelectedColumn(modelNumber);
   
   ArealEstimationFile* aef = brainSet->getArealEstimationFile();
   const int numNodes = brainSet->getNumberOfNodes();

   if ((aef->getNumberOfNodes() == 0) || (aef->getNumberOfColumns() == 0)) {
      return;
   }
   
   if (aef->getNumberOfNodes() != numNodes) {
      std::cerr << "Number of node in areal estimation files does not match surface" << std::endl;
      return;
   }
    
   //
   // For each name in the ArealEstimationFile assign it an index into the
   // Area Colors
   //
   int* areaColorIndex = new int[aef->getNumberOfAreaNames()];
   AreaColorFile* cf = brainSet->getAreaColorFile();
   for (int j = 0; j < aef->getNumberOfAreaNames(); j++) {
      bool m;
      areaColorIndex[j] = cf->getColorIndexByName(aef->getAreaName(j), m);
   }
     
   for (register int i = 0; i < numNodes; i++) {
      float r = 0, g = 0, b = 0;
      
      int nameIndx[4];
      float prob[4];
      aef->getNodeData(i, columnNumber, nameIndx, prob);
      
      for (int j = 0; j < 4; j++) {
         const int indx = areaColorIndex[nameIndx[j]];
         unsigned char red, green, blue;
         if (indx >= 0) {
            cf->getColorByIndex(indx, red, green, blue);
         }
         else {
            paintIndicesWithNoAreaColor.insert(nameIndx[j]);
            red   = defaultColor[0];
            green = defaultColor[1];
            blue  = defaultColor[2];
         }
         r += prob[j] * red;
         g += prob[j] * green;
         b += prob[j] * blue;
      }
      if (r > 255.0) r = 255.0;
      if (r < 0.0)   r = 0.0;
      if (g > 255.0) g = 255.0;
      if (g < 0.0)   g = 0.0;
      if (b > 255.0) b = 255.0;
      if (b < 0.0)   b = 0.0;
      
      nodeColors[i].r = static_cast<unsigned char>(r);
      nodeColors[i].g = static_cast<unsigned char>(g);
      nodeColors[i].b = static_cast<unsigned char>(b);
   }
   
   delete[] areaColorIndex;
}

/**
 * Match paint names to names in area color file which produces an index for
 * each paint name into the area color file.  It finds the exact match or
 * the "longest" where the area color name is a prefix of the paint name.
 */
void
BrainModelSurfaceNodeColoring::matchPaintNamesToNodeColorFile(BrainSet* bs,
                                int paintIndexToColorFile[],
                               std::vector<QString>& paintNames)
{
   AreaColorFile* cf = bs->getAreaColorFile();
   const int numNames = static_cast<int>(paintNames.size());
   
   for (int j = 0; j < numNames; j++) {
      bool exactMatch = false;
      unsigned char r = 0, g = 0, b = 0;
      paintIndexToColorFile[j] = 
            cf->getColorByName(paintNames[j], exactMatch, r, g, b);
   }
}

/**
 * Assign paint coloring to nodes.
 */ 
void
BrainModelSurfaceNodeColoring::assignPaintColoring()
{
   PaintFile* pf = brainSet->getPaintFile();
   
   const int numNodes = pf->getNumberOfNodes();
   if (numNodes == 0) {
      return;
   }
   if (numNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "Number of nodes in Paint File does not match surface." << std::endl;
      return;
   }
   DisplaySettingsPaint* dsp = brainSet->getDisplaySettingsPaint();
   const int column = dsp->getSelectedColumn(modelNumber);
   if (column < 0) {
      //std::cerr << "No column of paint file selected." << std::endl;
      return;
   }
   
   //
   // Assign colors to the paint names
   //
   AreaColorFile* cf = brainSet->getAreaColorFile();
   pf->assignColors(*cf);
   
   //
   // The label table inside the paint file
   //
   GiftiLabelTable* labelTable = pf->getLabelTable();
   
   //
   // Assign the colors to the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      const int p = pf->getPaint(i, column);
      if (pf->getPaintNameEnabled(p)) {
         const int colorFileIndex = labelTable->getColorFileIndex(p);
         if (colorFileIndex >= 0) {
            if (colorFileIndex != questionColorIndex) {
               unsigned char r = 0, g = 0, b = 0;
               cf->getColorByIndex(colorFileIndex,
                                    r, g, b);
               nodeColors[i].r = r;
               nodeColors[i].g = g;
               nodeColors[i].b = b;
            }
         }
         else {
            paintIndicesWithNoAreaColor.insert(p);
         }
      }
   }
/*
   PaintFile* pf = brainSet->getPaintFile();
   
   const int numNodes = pf->getNumberOfNodes();
   if (numNodes == 0) {
      return;
   }
   if (numNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "Number of nodes in Paint File does not match surface." << std::endl;
      return;
   }
   DisplaySettingsPaint* dsp = brainSet->getDisplaySettingsPaint();
   const int column = dsp->getSelectedColumn(modelNumber);
   if (column < 0) {
      //std::cerr << "No column of paint file selected." << std::endl;
   }
   
   const int numPaintNames = pf->getNumberOfPaintNames();
   std::vector<QString> paintNames;
   for (int j = 0; j < numPaintNames; j++) {
      paintNames.push_back(pf->getPaintNameFromIndex(j));
   }
   int* paintIndexToColorFile = new int[numPaintNames];
        
   matchPaintNamesToNodeColorFile(brainSet, paintIndexToColorFile, paintNames);
   AreaColorFile* cf = brainSet->getAreaColorFile();
   
   //
   // Assign the colors to the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      const int p = pf->getPaint(i, column);
      const int colorFileIndex = paintIndexToColorFile[p];
      if (colorFileIndex >= 0) {
         if (colorFileIndex != questionColorIndex) {
            unsigned char r = 0, g = 0, b = 0;
            cf->getColorByIndex(colorFileIndex,
                                 r, g, b);
            nodeColors[i].r = r;
            nodeColors[i].g = g;
            nodeColors[i].b = b;
         }
      }
      else {
         paintIndicesWithNoAreaColor.insert(p);
      }
   }
   delete[] paintIndexToColorFile;
*/
}

/**
 * Assign probabilistic coloring to a node.
 */
void
BrainModelSurfaceNodeColoring::assignProbabilisticColorToNode(
                           const int n, const int paintsAreaColorIndex[])
{
   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
   ProbabilisticAtlasFile* paf = brainSet->getProbabilisticAtlasSurfaceFile();
   AreaColorFile* cf = brainSet->getAreaColorFile();
   
   const int numberOfColumns = paf->getNumberOfColumns();
   const int numSelectedChannels = dspa->getNumberOfChannelsSelected();

   if (numSelectedChannels > 0) {
      int* areaColorIndices = new int[numberOfColumns];
      int* paintIndices = new int[numberOfColumns];
      bool haveColors = false;
      for (int c = 0; c < numberOfColumns; c++) {
         if (dspa->getChannelSelectedForStructure(c)) {
            const int paintIndex = paf->getPaint(n, c);
            areaColorIndices[c] = paintsAreaColorIndex[paintIndex];
            paintIndices[c]     = paintIndex;
            // check > 0 since ??? is always first and is not a valid atlas index
            if (paintIndex > 0) { 
               if (dspa->getAreaSelected(paintIndex)) {            
                  haveColors = true;
               }
            }
         }
      }
      
      if (haveColors) {
         // clear colors since we have probabilistic data for this node
         nodeColors[n].r = 0; 
         nodeColors[n].g = 0; 
         nodeColors[n].b = 0; 
         for (int color = 0; color < numberOfColumns; color++) {
            if (dspa->getChannelSelectedForStructure(color)) {
               const int areaColorIndex = areaColorIndices[color];
               const int paintIndex = paintIndices[color];
               if (areaColorIndex >= 0) {
                  if (dspa->getAreaSelected(paintIndex)) {
                     //nodeWasColored = 1;
                     unsigned char r, g, b;
                     cf->getColorByIndex(areaColorIndex, r, g, b);
                     nodeColors[n].r += (unsigned char)(
                                                    (r / (float)(numSelectedChannels)));
                     nodeColors[n].g += (unsigned char)(
                                                    (g / (float)(numSelectedChannels)));
                     nodeColors[n].b += (unsigned char)(
                                                    (b / (float)(numSelectedChannels)));
                  }
               }
               else if (areaColorIndex < 0) {
                  paintIndicesWithNoAreaColor.insert(areaColorIndex);
               }
            }
         }
      }
      
      delete[] areaColorIndices;
      delete[] paintIndices;
   }
}

/**
 * Normal Probabilistic coloring.
 */
void
BrainModelSurfaceNodeColoring::assignProbabilisticNormalColoring()
{
   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
   PaintFile* paf = brainSet->getProbabilisticAtlasSurfaceFile();
   
   const int numberOfNodes = paf->getNumberOfNodes();
   const int numberOfColumns = paf->getNumberOfColumns();
   
   if (numberOfColumns <= 0) {
      return;
   }

   if (numberOfNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "Probabilistic Atlas file has different number of nodes "
                << numberOfNodes << " than surface " 
                << brainSet->getNumberOfNodes() << "." << std::endl;
      return;
   }
   const int numPaintNames = paf->getNumberOfPaintNames();
   if (numPaintNames <= 0) {
      std::cerr << "Probabilistic Atlas file contains no paint names." << std::endl;
      return;
   }
   
   const QString question3 = "???";
   
   std::vector<QString> paintNames;
   for (int j = 0; j < numPaintNames; j++) {
      QString name(paf->getPaintNameFromIndex(j));
      if (dspa->getTreatQuestColorAsUnassigned()) {
         if (name == question3) {
            name = "Unassigned";
         }
      }
      paintNames.push_back(name);
   }
   
   int* paintsAreaColorIndex = new int[numPaintNames];
        
   matchPaintNamesToNodeColorFile(brainSet, paintsAreaColorIndex, paintNames);

   int* probIndices = new int[numberOfColumns];
   
   for (int j = 0; j < numberOfNodes; j++){
      paf->getPaints(j, probIndices);
      bool haveNonQuestion = false;
      for (int k = 0; k < numberOfColumns; k++) {
         if (probIndices[k] < numPaintNames) {
            if (paintNames[probIndices[k]].left(3) != question3) {
               haveNonQuestion = true;
               break;
            }
         }
      }
      
      nodeColors[j].r = defaultColor[0]; 
      nodeColors[j].g = defaultColor[1]; 
      nodeColors[j].b = defaultColor[2]; 
      if (haveNonQuestion) {
         assignProbabilisticColorToNode(j, paintsAreaColorIndex);
      }
   }
   
   delete[] probIndices;
   delete[] paintsAreaColorIndex;
}

/**
 * Threshold probabilistic coloring.
 */
void
BrainModelSurfaceNodeColoring::assignProbabilisticThresholdColoring()
{
   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
   ProbabilisticAtlasFile* paf = brainSet->getProbabilisticAtlasSurfaceFile();
   
   const int numberOfNodes = paf->getNumberOfNodes();
   const int numberOfColumns = paf->getNumberOfColumns();
   
   if (numberOfNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "Probabilistic Atlas file has different number of nodes than surface." << std::endl;
      return;
   }
   if (numberOfColumns <= 0) {
      return;
   }
   const int numPaintNames = paf->getNumberOfPaintNames();
   if (numPaintNames <= 0) {
      std::cerr << "Probabilistic Atlas file contains no paint names." << std::endl;
      return;
   }

   int questionIndex = -1;
   int gyralIndex = -1;
   int gyrusIndex = -1;
   
   std::vector<QString> paintNames;
   for (int j = 0; j < numPaintNames; j++) {
      QString name(paf->getPaintNameFromIndex(j));
      if (dspa->getTreatQuestColorAsUnassigned()) {
         if (name == "???") {
            name = "Unassigned";
         }
      }
      
      if (name == "???") {
         questionIndex = static_cast<int>(paintNames.size());
      }
      if (name == "GYRUS") {
         gyrusIndex = static_cast<int>(paintNames.size());
      }
      if (name == "GYRAL") {
         gyralIndex = static_cast<int>(paintNames.size());
      }
      paintNames.push_back(name);
   }
   
   int* paintsAreaColorIndex = new int[numPaintNames];
        
   matchPaintNamesToNodeColorFile(brainSet, paintsAreaColorIndex, paintNames);
   
   const int numSelectedChannels = dspa->getNumberOfChannelsSelected();

   // see if there is a color for "ANYAREA" which is used when at least one paint column is
   // non-zero but there is not a majority of the columns the same.
   AreaColorFile* cf = brainSet->getAreaColorFile();
   unsigned char anyAreaColor[3] = { defaultColor[0], defaultColor[1], defaultColor[2] };
   bool anyAreaColorValid = false;
   cf->getColorByName("ANYAREA", anyAreaColorValid, anyAreaColor[0], anyAreaColor[1], anyAreaColor[2]);

   //
   // Number of columns that must match
   //
   const float ratio = dspa->getThresholdDisplayTypeRatio();
                             
   for (int j = 0; j < numberOfNodes; j++){
      nodeColors[j].r = defaultColor[0];
      nodeColors[j].g = defaultColor[1];
      nodeColors[j].b = defaultColor[2];

      std::map<int,int> indexCounterMap;
      
      bool atLeastOneNonZero = false;
      for (int k = 0; k < numberOfColumns; k++) {
         int cntIndex = 0;
         if (dspa->getChannelSelectedForStructure(k)) {
            cntIndex = paf->getPaint(j, k);
         }
         if ((cntIndex > 0) && (cntIndex < numPaintNames)) {
            if (dspa->getAreaSelected(cntIndex) == false) {
               cntIndex = -1;
            }
         }
         if (cntIndex > 0) {
            //
            // Skip non-sulci
            //
            bool useIt = true;
            if ((cntIndex == questionIndex) ||
                (cntIndex == gyralIndex) ||
                (cntIndex == gyrusIndex)) {
               useIt = false;
            }
            
            if (useIt) {
               atLeastOneNonZero = true;

               std::map<int,int>::iterator iter = indexCounterMap.find(cntIndex);
               if (iter != indexCounterMap.end()) {
                  iter->second++;
               }
               else {
                  indexCounterMap[cntIndex] = 1;
               }
            }
         }
      }

      int paintColIndex = -1;
      if (indexCounterMap.empty() == false) {
         int maxIndex = -1;
         int maxCount = -1;
         for (std::map<int,int>::iterator iter = indexCounterMap.begin();
              iter != indexCounterMap.end(); iter++) {
            if (iter->second > maxCount) {
               maxIndex = iter->first;
               maxCount = iter->second;
            }
         }
         
         if (maxCount >= 0) {
            const float percentSelected = static_cast<float>(maxCount)
                                        / static_cast<float>(numSelectedChannels);
            if (percentSelected >= ratio) {
               paintColIndex = maxIndex;
            }
         }
      }

      if (probAtlasThreshPaintFile != NULL) {
         probAtlasThreshPaintFile->setPaint(j, probAtlasThreshPaintColumn, 0);
      }
      
      if (paintColIndex >= 0) {
         const int areaColorIndex = paintsAreaColorIndex[paintColIndex];

         if (areaColorIndex >= 0) {
            unsigned char r, g, b;
            cf->getColorByIndex(areaColorIndex, r, g, b);
            nodeColors[j].r = r;
            nodeColors[j].g = g;
            nodeColors[j].b = b;
            if (probAtlasThreshPaintFile != NULL) {
               const int indx = probAtlasThreshPaintFile->addPaintName(
                                            paf->getPaintNameFromIndex(paintColIndex));
               probAtlasThreshPaintFile->setPaint(j, probAtlasThreshPaintColumn, indx);
            }
         }
         else {
             nodeColors[j].r = anyAreaColor[0];
             nodeColors[j].g = anyAreaColor[1];
             nodeColors[j].b = anyAreaColor[2];
             paintIndicesWithNoAreaColor.insert(areaColorIndex);
         }
      }
      else if (atLeastOneNonZero && anyAreaColorValid) {
          nodeColors[j].r = anyAreaColor[0];
          nodeColors[j].g = anyAreaColor[1];
          nodeColors[j].b = anyAreaColor[2];
      }
   }
   
   if (paintsAreaColorIndex != NULL) delete[] paintsAreaColorIndex;
}

/*
**
 * Threshold probabilistic coloring.
 *
void
BrainModelSurfaceNodeColoring::assignProbabilisticThresholdColoring()
{
   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlas();
   ProbabilisticAtlasFile* paf = brainSet->getProbabilisticAtlasFile();
   
   const int numberOfNodes = paf->getNumberOfNodes();
   const int numberOfColumns = paf->getNumberOfColumns();
   
   if (numberOfNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "Probabilistic Atlas file has different number of nodes than surface." << std::endl;
      return;
   }
   if (numberOfColumns <= 0) {
      return;
   }
   const int numPaintNames = paf->getNumberOfPaintNames();
   if (numPaintNames <= 0) {
      std::cerr << "Probabilistic Atlas file contains no paint names." << std::endl;
      return;
   }

   const QString question3 = "???";
   
   std::vector<QString> paintNames;
   for (int j = 0; j < numPaintNames; j++) {
      QString name(paf->getPaintNameFromIndex(j));
      if (dspa->getTreatQuestColorAsUnassigned()) {
         if (name == question3) {
            name = "Unassigned");
         }
      }
      paintNames.push_back(name);
   }
   
   int* paintsAreaColorIndex = new int[numPaintNames];
        
   matchPaintNamesToNodeColorFile(brainSet, paintsAreaColorIndex, paintNames);
   
   const int numSelectedChannels = dspa->getNumberOfChannelsSelected();

   // see if there is a color for "ANYAREA" which is used when at least one paint column is
   // non-zero but there is not a majority of the columns the same.
   AreaColorFile* cf = brainSet->getAreaColorFile();
   unsigned char anyAreaColor[3] = { defaultColor[0], defaultColor[1], defaultColor[2] };
   bool anyAreaColorValid = false;
   cf->getColorByName("ANYAREA", anyAreaColorValid, anyAreaColor[0], anyAreaColor[1], anyAreaColor[2]);

   //
   // Number of columns that must match
   //
   const float ratio = dspa->getThresholdDisplayTypeRatio();
                             
   for (int j = 0; j < numberOfNodes; j++){
      nodeColors[j].r = defaultColor[0];
      nodeColors[j].g = defaultColor[1];
      nodeColors[j].b = defaultColor[2];

      int paintColCount = 0;
      int paintColIndex = 0;
      bool atLeastOneNonZero = false;
      for (int k = 0; k < (numberOfColumns - 1); k++) {
         int cnt = 0;  
         int cntIndex = 0;
         if (dspa->getChannelSelectedForStructure(k)) {
            cntIndex = paf->getPaint(j, k);
         }
         if ((cntIndex > 0) && (cntIndex < numPaintNames)) {
            if (dspa->getAreaSelected(cntIndex) == false) {
               cntIndex = -1;
            }
         }
         if ((cntIndex > 0) && (cntIndex < numPaintNames)) {
            cnt = 1;   // count self so start at 1
            atLeastOneNonZero = true;
            // skip over ???
            if (paintNames[cntIndex] == "???") {
               continue;
            }

            for (int l = k + 1; l < numberOfColumns; l++) {
               if (dspa->getChannelSelectedForStructure(l)) {
                  if (cntIndex == paf->getPaint(j, l)) {
                      cnt++;
                  }
               }
            }

            const float percentSelected = static_cast<float>(cnt)
                                        / static_cast<float>(numSelectedChannels);
            if (percentSelected >= ratio) {
               if (cnt > paintColCount) {
                  paintColCount = cnt;
                  paintColIndex = cntIndex;
               }
               break;
            }
            else {
               cnt = 0;
            }
         }
      }

      if (probAtlasThreshPaintFile != NULL) {
         probAtlasThreshPaintFile->setPaint(j, probAtlasThreshPaintColumn, 0);
      }
      
      if (paintColCount > 0) {
         const int paintIndex = paintColIndex;
         const int areaColorIndex = paintsAreaColorIndex[paintIndex];

         if ((areaColorIndex >= 0) && 
             (dspa->getAreaSelected(paintIndex))) {
            unsigned char r, g, b;
            cf->getColorByIndex(areaColorIndex, r, g, b);
            nodeColors[j].r = r;
            nodeColors[j].g = g;
            nodeColors[j].b = b;
            if (probAtlasThreshPaintFile != NULL) {
               const int indx = probAtlasThreshPaintFile->addPaintName(
                                            paf->getPaintNameFromIndex(paintIndex));
               probAtlasThreshPaintFile->setPaint(j, probAtlasThreshPaintColumn, indx);
            }
         }
         else {
             nodeColors[j].r = anyAreaColor[0];
             nodeColors[j].g = anyAreaColor[1];
             nodeColors[j].b = anyAreaColor[2];
         }
      }
      else if (atLeastOneNonZero && anyAreaColorValid) {
          nodeColors[j].r = anyAreaColor[0];
          nodeColors[j].g = anyAreaColor[1];
          nodeColors[j].b = anyAreaColor[2];
      }
   }
   
   if (paintsAreaColorIndex != NULL) delete[] paintsAreaColorIndex;
}
*/

/**
 * add prob atlas thresholding to paint file
 * the paint column must exist.
 */
void 
BrainModelSurfaceNodeColoring::addProbAtlasThresholdingToPaintFile(PaintFile* paintFileIn,
                                                                   const int paintColumn)
{
   probAtlasThreshPaintFile = paintFileIn;
   probAtlasThreshPaintColumn = paintColumn;
   assignColors();
   probAtlasThreshPaintFile = NULL;
}                                               

void
BrainModelSurfaceNodeColoring::assignProbabilisticColoring(const BrainModelSurface* bms)
{
   DisplaySettingsProbabilisticAtlas* dspa = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
   if (bms != NULL) {
      dspa->updateSelectedChannelsForCurrentStructure(bms->getStructure());
   }
   
   if (dspa->getDisplayType() == DisplaySettingsProbabilisticAtlas::PROBABILISTIC_DISPLAY_TYPE_NORMAL) {
      assignProbabilisticNormalColoring();
   }
   else {
      assignProbabilisticThresholdColoring();
   }
}

//---------------------------------------------------------------------------
void
BrainModelSurfaceNodeColoring::assignBlendGeographyColoring(const int offset,
                                                            const int sourceOffset)
{
   PaintFile* pf = brainSet->getPaintFile();
   
   const int geoColumn = pf->getGeographyColumnNumber();
   if (geoColumn < 0) {
      return;
   }
   
   // find all paints that begin with "SUL"
   const int numPaints = pf->getNumberOfPaintNames();
   if (numPaints <= 0) {
      return;
   }
   int* geoPaint = new int[numPaints];
   for (int i = 0; i < numPaints; i++) {
      const QString name = pf->getPaintNameFromIndex(i);
      if (name.left(3) == "SUL") {
         geoPaint[i] = 1;
      }
      else {
         geoPaint[i] = 0;
      }
   }
   
   const int numNodes = brainSet->getNumberOfNodes();
   
   for (int j = 0; j < numNodes; j++) {
      const int paintIndex = pf->getPaint(j, geoColumn);
      if (overlayPrimaryNodeColors[j].r >= 0) {
         if (geoPaint[paintIndex] != 0) {
             nodeColoring[offset + j * 3] = 
                      (unsigned char)(overlayPrimaryNodeColors[j].r * geographyBlending);
             nodeColoring[offset + j * 3 + 1] = 
                      (unsigned char)(overlayPrimaryNodeColors[j].g * geographyBlending);
             nodeColoring[offset + j * 3 + 2] = 
                      (unsigned char)(overlayPrimaryNodeColors[j].b * geographyBlending);
         }
         else {
             nodeColoring[offset + j * 3]     = overlayPrimaryNodeColors[j].r;
             nodeColoring[offset + j * 3 + 1] = overlayPrimaryNodeColors[j].g;
             nodeColoring[offset + j * 3 + 2] = overlayPrimaryNodeColors[j].b;
         }
         nodeColorSource[sourceOffset + j] = NODE_COLOR_SOURCE_PRIMARY_OVERLAY;
      }
      else if (overlaySecondaryNodeColors[j].r >= 0) {
         if (geoPaint[paintIndex] != 0) {
             nodeColoring[offset + j * 3] = 
                      (unsigned char)(overlaySecondaryNodeColors[j].r * geographyBlending);
             nodeColoring[offset + j * 3 + 1] = 
                      (unsigned char)(overlaySecondaryNodeColors[j].g * geographyBlending);
             nodeColoring[offset + j * 3 + 2] = 
                      (unsigned char)(overlaySecondaryNodeColors[j].b * geographyBlending);
         }
         else {
             nodeColoring[offset + j * 3]     = overlaySecondaryNodeColors[j].r;
             nodeColoring[offset + j * 3 + 1] = overlaySecondaryNodeColors[j].g;
             nodeColoring[offset + j * 3 + 2] = overlaySecondaryNodeColors[j].b;
         }
         nodeColorSource[sourceOffset + j] = NODE_COLOR_SOURCE_SECONDARY_OVERLAY;
      }
      else if (geoPaint[paintIndex] != 0) {
         nodeColoring[offset + j * 3] = 
               (unsigned char)(underlayNodeColors[j].r * geographyBlending);
         nodeColoring[offset + j * 3 + 1] = 
               (unsigned char)(underlayNodeColors[j].g * geographyBlending);
         nodeColoring[offset + j * 3 + 2] = 
               (unsigned char)(underlayNodeColors[j].b * geographyBlending);
      }
      else {
          nodeColoring[offset + j * 3]     = underlayNodeColors[j].r;
          nodeColoring[offset + j * 3 + 1] = underlayNodeColors[j].g;
          nodeColoring[offset + j * 3 + 2] = underlayNodeColors[j].b;
          nodeColorSource[sourceOffset + j] = NODE_COLOR_SOURCE_UNDERLAY;
      }
   }
   
   delete geoPaint;
}

/** 
 *  Crossover coloring.
 */
void
BrainModelSurfaceNodeColoring::assignCrossoverColoring()
{
   const int numNodes = brainSet->getNumberOfNodes();   
   for (register  int i = 0; i < numNodes; i++) {
      BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(i);
      if (bna->getCrossover() != BrainSetNodeAttribute::CROSSOVER_NO) {
         nodeColors[i].r = 255;
         nodeColors[i].g = 0;
         nodeColors[i].b = 0;
      }
   }
}

/**
 * Sections coloring
 */
void
BrainModelSurfaceNodeColoring::assignSectionColoring()
{
   SectionFile* sf = brainSet->getSectionFile();
   const int numNodes = sf->getNumberOfNodes();
   if (numNodes != brainSet->getNumberOfNodes()) {
      std::cout << "ERROR: Section file has different number of nodes than surfaces." << std::endl;
      return;
   }
   
   const DisplaySettingsSurface* dss = brainSet->getDisplaySettingsSurface();
   int selectedSection = -100000;
   bool sectionEveryX  = false;
   dss->getSectionHighlighting(selectedSection, sectionEveryX);
   
   DisplaySettingsSection* ds = brainSet->getDisplaySettingsSection();
   
   const int sectionSetNumber = ds->getSelectedColumn();
   if ((sectionSetNumber >= 0) && (sectionSetNumber < sf->getNumberOfColumns())) {
      for (int i = 0; i < numNodes; i++) {
         bool colorIt = false;
         const int section = sf->getSection(i, sectionSetNumber);
         if (sectionEveryX) {
            if (selectedSection == 0) {
               colorIt = true;
            }
            else if ((section % selectedSection) == 0) {
               colorIt = true;
            }
         }
         else {
            if (section == selectedSection) {
               colorIt = true;
            }
         }
         if (colorIt) {
            nodeColors[i].r = 0;
            nodeColors[i].g = 0;
            nodeColors[i].b = 255;
         }
      }
   }
}

/**
 * Edges coloring
 */
void
BrainModelSurfaceNodeColoring::assignEdgesColoring()
{
   const int numNodes = brainSet->getNumberOfNodes();   
   for (register  int i = 0; i < numNodes; i++) {
      BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(i);
      if (bna->getClassification() == BrainSetNodeAttribute::CLASSIFICATION_TYPE_EDGE) {
         nodeColors[i].r = 0;
         nodeColors[i].g = 0;
         nodeColors[i].b = 255;
      }
   }
}

/**
 * Surface Shape coloring.
 */
void
BrainModelSurfaceNodeColoring::assignSurfaceShapeColoring()
{
   const SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
   const DisplaySettingsSurfaceShape* dsss = brainSet->getDisplaySettingsSurfaceShape();
   
   const int column = dsss->getSelectedDisplayColumn(modelNumber);
   if (column < 0) {
      return;
   }
   const int numNodes = ssf->getNumberOfNodes();
   if (numNodes <= 0) {
      return;
   }
   
   if (numNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "Surface shape file has different number of nodes than surface." << std::endl;
      return;
   }
   
   float minValue, maxValue;
   ssf->getColumnColorMappingMinMax(column, minValue, maxValue);
   float diffMinMax = maxValue - minValue;
   if (diffMinMax == 0.0) {
      diffMinMax = 1.0;
   }

   //
   // For palette coloring
   //
   PaletteFile* pf = brainSet->getPaletteFile();
   const Palette* palette = pf->getPalette(dsss->getSelectedPaletteIndex());

   //
   // Always interpolate if the palette has only two colors
   //
   bool interpolatePaletteColor = dsss->getInterpolatePaletteColors();
   if (palette->getNumberOfPaletteEntries() == 2) {
      interpolatePaletteColor = true;
   }
   
   for (int j = 0; j < numNodes; j++) {
      const float shape = ssf->getValue(j, column);
      const int gray = getLutIndex(shape, minValue, maxValue);
      switch(dsss->getColorMap()) {
         case DisplaySettingsSurfaceShape::SURFACE_SHAPE_COLOR_MAP_GRAY:
            nodeColors[j].r = gray;
            nodeColors[j].g = gray;
            nodeColors[j].b = gray;
            break;
         case DisplaySettingsSurfaceShape::SURFACE_SHAPE_COLOR_MAP_ORANGE_YELLOW:
            nodeColors[j].r = lutOrangeYellow[gray][0];
            nodeColors[j].g = lutOrangeYellow[gray][1];
            nodeColors[j].b = lutOrangeYellow[gray][2];
            break;
         case DisplaySettingsSurfaceShape::SURFACE_SHAPE_COLOR_MAP_PALETTE:
            {
               float normalized = 0.0;
                     
               if ((palette->getNumberOfPaletteEntries() == 2) &&
                    interpolatePaletteColor) {
                  //
                  // Normalize between [0, 1.0] when two color palette interpolate
                  //
                  normalized = (1.0 / diffMinMax) * (shape - minValue);
               }
               else {
                  if (shape >= 0.0) {
                     if (maxValue != 0.0) {
                        normalized = shape / maxValue;
                     }
                  }
                  else {
                     if (minValue != 0.0) {
                        normalized = -(shape / minValue);
                     }
                  }
               }

               bool isNoneColor = false;
               unsigned char colors[3];
               palette->getColor(normalized, interpolatePaletteColor,
                                 isNoneColor, colors);

               if (isNoneColor == false) {
                  nodeColors[j].r = colors[0];
                  nodeColors[j].g = colors[1];
                  nodeColors[j].b = colors[2];
               }
            }
            break;
      }
   }
}   

/**
 * Assign CoCoMac coloring.
 */
void
BrainModelSurfaceNodeColoring::assignCocomacColoring()
{
   DisplaySettingsCoCoMac* dsc = brainSet->getDisplaySettingsCoCoMac();
   CocomacConnectivityFile* ccf = brainSet->getCocomacFile();
   
   const int numProj = ccf->getNumberOfCocomacProjections();
   const int node = dsc->getSelectedNode();
   
   QString idInfo;
   
   if ((numProj >= 0) && (node >= 0) && (node < brainSet->getNumberOfNodes())) {
      //
      // Ensure valid paint column selected for cocomac
      //
      const int paintColumn = dsc->getSelectedPaintColumn();
      PaintFile* pf = brainSet->getPaintFile();
      if ((paintColumn >= 0) && (paintColumn < pf->getNumberOfColumns())) {
         //
         // Get the name of the paint
         //
         const int selectedPaintIndex = pf->getPaint(node, paintColumn);
         if ((selectedPaintIndex >= 0) && (selectedPaintIndex < pf->getNumberOfPaintNames())) {
            const QString paintName(pf->getPaintNameFromIndex(selectedPaintIndex));
            
            std::set<int> paintsOfInterest;
            
            for (int i = 0; i < numProj; i++) {
               CocomacProjection* cp = ccf->getCocomacProjection(i);
               switch(dsc->getConnectionDisplayType()) {
                  case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT:
                  case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT_OR_EFFERENT:
                     if (paintName == cp->getTargetSite()) {
                        const int sourcePaintIndex = pf->getPaintIndexFromName(cp->getSourceSite());
                        QString s("   ");
                        s.append(cp->getTargetSite());
                        s.append(" has afferent connection from ");
                        s.append(cp->getSourceSite());
                        s.append(" density ");
                        s.append(cp->getDensity());
                        s.append("\n");
                        idInfo.append(s);
                        if (sourcePaintIndex >= 0) {
                           paintsOfInterest.insert(sourcePaintIndex);
                        }
                     }
                     if (dsc->getConnectionDisplayType() == 
                         DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT) {
                         break;
                     }
                     //
                     // no "break;" here.  Fall through to show efferent connections if "OR".
                     //
                  case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_EFFERENT:
                     if (paintName == cp->getSourceSite()) {
                        const int targetPaintIndex = pf->getPaintIndexFromName(cp->getTargetSite());
                        QString s("   ");
                        s.append(cp->getSourceSite());
                        s.append(" has efferent connection to ");
                        s.append(cp->getTargetSite());
                        s.append(" density ");
                        s.append(cp->getDensity());
                        s.append("\n");
                        idInfo.append(s);
                        if (targetPaintIndex) {
                           paintsOfInterest.insert(targetPaintIndex);
                        }
                     }
                     break;
                  case DisplaySettingsCoCoMac::CONNECTION_DISPLAY_AFFERENT_AND_EFFERENT:
                     if (paintName == cp->getSourceSite()) {
                        const QString targetName(cp->getTargetSite());
                        QString afferentDensity;
                        bool connectsBothWays = false;
                        for (int j = 0; j < numProj; j++) {
                           CocomacProjection* ocp = ccf->getCocomacProjection(j);
                           if ((ocp->getSourceSite() == targetName) &&
                               (ocp->getTargetSite() == paintName)) {
                              connectsBothWays = true;
                              afferentDensity = ocp->getDensity();
                              break;
                           }
                        }
                        
                        if (connectsBothWays) {
                           const int targetPaintIndex = pf->getPaintIndexFromName(targetName);
                           QString s("   ");
                           s.append(cp->getSourceSite());
                           s.append(" has afferent and efferent connection with ");
                           s.append(cp->getTargetSite());
                           s.append(" afferent density ");
                           s.append(afferentDensity);
                           s.append(" efferent density ");
                           s.append(cp->getDensity());
                           s.append("\n");
                           idInfo.append(s);
                           if (targetPaintIndex >= 0) {
                              paintsOfInterest.insert(targetPaintIndex);
                           }
                        }
                     }
                     break;
               }
            }
            
            const int numNodes = brainSet->getNumberOfNodes();
            for (int i = 0; i < numNodes; i++) {
               int rgb[3] = { 0, 0, 0 };
               bool applyColorsToNode = false;
               
               const int nodePaintIndex = pf->getPaint(i, paintColumn);
               if (nodePaintIndex == selectedPaintIndex) {
                  rgb[0] = 255;
                  rgb[1] = 0;
                  rgb[2] = 0;
                  applyColorsToNode = true;
               }
               else {
                  if (paintsOfInterest.find(nodePaintIndex) != paintsOfInterest.end()) {
                     rgb[0] = 0;
                     rgb[1] = 255;
                     rgb[2] = 0;
                     applyColorsToNode = true;
                  }
               }
               
               if (applyColorsToNode) {
                  nodeColors[i].r = rgb[0];
                  nodeColors[i].g = rgb[1];
                  nodeColors[i].b = rgb[2];
               }
            }
         }
      }
   }
   
   dsc->setIDInfo(idInfo);
   
/*
      if (selectedNodePaintIndex >= 0) {
         const string paintName(s.paintID[selectedNodePaintIndex]);
         if (getCocomacShowConnectionInfo()) {
            char str[1024];
            sprintf(str, 
                    "\t-----------------------------------------------------"
                    "--------\n");
            StatusPrint(str);
            sprintf(str, "\tSelected node's area is %s.\n", paintName);
            StatusPrint(str);
         }
         
         set<int> tgtPaintIndices;
         
         for (int i = 0; i < numProj; i++) {
            CocomacProjection* cp = ccf->getCocomacProjection(i);
            
            switch(getCocomacViewConnection()) {
               case COCOMAC_VIEW_AFFERENT_OR_EFFERENT:
               case COCOMAC_VIEW_AFFERENT:
                  if (paintName == cp->getTargetSite()) {
                     const string srcName(cp->getSourceSite());
                     const int srcPaintIndex = s.PaintIndex(srcName);
                     if (getCocomacShowConnectionInfo()) {
                        char str[1024];
                        sprintf(str,
                           "\t%s has afferent connection from %s, density %s\n",
                           paintName, cp->getSourceSite(), 
                           cp->getDensity());
                        StatusPrint(str);
                     }
                     if (srcPaintIndex >= 0) {
                        tgtPaintIndices.insert(srcPaintIndex);   
                     }
                  }
                  if (getCocomacViewConnection() == COCOMAC_VIEW_AFFERENT) {
                     break;
                  }
                  else {
                     // for COCOMAC_VIEW_AFFERENT_OR_EFFERENT, fall through
                     // to also show efferent connections
                  }
               case COCOMAC_VIEW_EFFERENT:
                  if (paintName == cp->getSourceSite()) {
                     const string tgtName(cp->getTargetSite());
                     const int tgtPaintIndex = s.PaintIndex(tgtName);
                     if (getCocomacShowConnectionInfo()) {
                        char str[1024];
                        sprintf(str,
                             "\t%s has efferent connection to %s, density %s\n",
                              paintName, cp->getTargetSite(),
                              cp->getDensity());
                        StatusPrint(str);
                     }
                     if (tgtPaintIndex >= 0) {
                        tgtPaintIndices.insert(tgtPaintIndex);   
                     }
                  }
                  break;
               case COCOMAC_VIEW_AFFERENT_AND_EFFERENT:
                  if (paintName == cp->getSourceSite()) {
                     const string tgtName(cp->getTargetSite());
                     bool connectsBothWays = false;
                     string afferentDensity;
                     for (int i = 0; i < numProj; i++) {
                        CocomacProjection* ocp = ccf->getCocomacProjection(i);
                        if ((ocp->getSourceSite() == tgtName) &&
                            (ocp->getTargetSite() == paintName)) {
                           connectsBothWays = true;
                           afferentDensity = ocp->getDensity();
                           break;
                        }
                     }
                     if (connectsBothWays) {
                        const int tgtPaintIndex = s.PaintIndex(tgtName);
                        char str[1024];
                        if (getCocomacShowConnectionInfo()) {
                           sprintf(str,
                              "\t%s has afferent and efferent connections "
                              "with %s, afferent density %s, "
                              "efferent density %s\n",
                              paintName, cp->getTargetSite(),
                              afferentDensity,cp->getDensity());
                           StatusPrint(str);
                        }
                        if (tgtPaintIndex >= 0) {
                           tgtPaintIndices.insert(tgtPaintIndex);   
                        }
                     }
                  }
                  break;
            }
         }
         
         for (int j = 0; j < s.num_points; j++) {
            int rgb[3] = { 0, 0, 0 };
            bool applyColorsToNode = false;
            if (s.points[j].paint[paintColumn] == selectedNodePaintIndex) {
               rgb[0] = 255;
               rgb[1] =   0;
               rgb[2] =   0;
               applyColorsToNode = true;
            }
            else {
               for (set<int>::iterator is = tgtPaintIndices.begin();
                     is != tgtPaintIndices.end(); is++) {
                  if (s.points[j].paint[paintColumn] == *is) {
                     rgb[0] =   0;
                     rgb[1] = 255;
                     rgb[2] =   0;
                     applyColorsToNode = true;
                     break;
                  }
               }
            }
            if (applyColorsToNode == true) {
               nodeColors[j].r = rgb[0];
               nodeColors[j].g = rgb[1];
               nodeColors[j].b = rgb[2];
            }
         }
      }
   }
*/
}

/**
 * Assign metric coloring.
 */
void
BrainModelSurfaceNodeColoring::assignMetricColoring()
{
   QTime timer;
   timer.start();
   
   MetricFile* mf = brainSet->getMetricFile();
   DisplaySettingsMetric* dsm = brainSet->getDisplaySettingsMetric();
   
   const int viewIndex = dsm->getSelectedDisplayColumn(modelNumber);
   if (viewIndex < 0) {
      return;
   }
   
   const int numNodes = mf->getNumberOfNodes();
   if (numNodes != brainSet->getNumberOfNodes()) {
      std::cerr << "Metric file has different number of nodes than the surface." << std::endl;
      return;
   }
   
   const PaletteFile* pf = brainSet->getPaletteFile();
   if (pf->getNumberOfPalettes() == 0) {
      std::cerr << "There are no palette files loaded, cannot color metrics." << std::endl;
      return;
   }
   const Palette* palette = pf->getPalette(dsm->getSelectedPaletteIndex());
   const bool positiveOnlyPalette = palette->getPositiveOnly();
   
   VolumeFile* funcVolume = NULL;
   BrainModelVolume* bmv = brainSet->getBrainModelVolume();
   if (bmv != NULL) {
      funcVolume = bmv->getSelectedVolumeFunctionalViewFile();
   }
   float posMinMetric = 0.0, posMaxMetric = 0.0, negMinMetric = 0.0, negMaxMetric = 0.0;
   DisplaySettingsMetric::METRIC_OVERLAY_SCALE overlayScale =
                                            dsm->getSelectedOverlayScale();
   if (overlayScale == DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME) {
      if (funcVolume == NULL) {
         overlayScale = DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_METRIC;
      }
   }
   
   bool userScaleFlag = false;
   switch (overlayScale) {
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_METRIC:
         mf->getDataColumnMinMax(viewIndex, negMaxMetric, posMaxMetric);
         break;
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_FUNC_VOLUME:
         funcVolume->getMinMaxVoxelValues(negMaxMetric, posMaxMetric);
         break;
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_USER:
         userScaleFlag = true;
         dsm->getUserScaleMinMax(posMinMetric, posMaxMetric, negMinMetric, negMaxMetric);
         break;
   }
   
   //setColorbarMinMaxZero(minMetric, maxMetric, 0.0);
   
   const int thresholdIndex = dsm->getSelectedThresholdColumn(modelNumber);
   float thresholdNegativeValue = 0.0, thresholdPositiveValue = 0.0;
   dsm->getUserThresholdingValues(thresholdNegativeValue,
                                  thresholdPositiveValue);
   switch (dsm->getMetricThresholdingType()) {
      case DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_FILE_COLUMN:
         if ((thresholdIndex >= 0) && (thresholdIndex < mf->getNumberOfColumns())) {
            mf->getColumnThresholding(thresholdIndex,
                             thresholdNegativeValue,
                             thresholdPositiveValue);
         }
         break;
      case DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_FILE_COLUMN_AVERAGE:
         if ((thresholdIndex >= 0) && (thresholdIndex < mf->getNumberOfColumns())) {
            mf->getColumnAverageThresholding(thresholdIndex,
                                thresholdNegativeValue,
                                thresholdPositiveValue);
         }
         break;
      case DisplaySettingsMetric::METRIC_THRESHOLDING_TYPE_USER_VALUES:
         dsm->getUserThresholdingValues(thresholdNegativeValue,
                                        thresholdPositiveValue);
         break;
   }
   
   //
   // Always interpolate if the palette has only two colors
   //
   bool interpolateColor = dsm->getInterpolateColors();
   if (palette->getNumberOfPaletteEntries() == 2) {
      interpolateColor = true;
   }
   
   unsigned char negThreshColor[3], posThreshColor[3];
   dsm->getSpecialColorsForThresholdedNodes(negThreshColor,
                                            posThreshColor);
   const bool showThreshNodes = dsm->getShowSpecialColorForThresholdedNodes();
   
   enum DISPLAY_NODE {
      DISPLAY_NODE_NORMAL,
      DISPLAY_NODE_POS_THRESH_COLOR,
      DISPLAY_NODE_NEG_THRESH_COLOR,
      DISPLAY_NODE_DO_NOT
   };
   
   for (int j = 0; j < numNodes; j++) {
   
      //
      // when activation assignment dialog is active only display
      // those metrics that exceed the threshold value
      //
      DISPLAY_NODE displayNode = DISPLAY_NODE_NORMAL;
      const float thresh = mf->getValue(j, thresholdIndex);
      if (thresh >= 0.0) {
         if (thresh < thresholdPositiveValue) {
            displayNode = DISPLAY_NODE_DO_NOT;
            if (showThreshNodes) {
               if (thresh != 0.0) {
                  displayNode = DISPLAY_NODE_POS_THRESH_COLOR;
               }
            }
         }
      }
      if (thresh <= 0.0) {
         if (thresh > thresholdNegativeValue) {
            displayNode = DISPLAY_NODE_DO_NOT;
            if (showThreshNodes) {
               if (thresh != 0.0) { 
                  displayNode = DISPLAY_NODE_NEG_THRESH_COLOR;
               }
            }
         }
      }
      
      const float metric = mf->getValue(j, viewIndex);
      switch(dsm->getDisplayMode()) {
         case DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_AND_NEGATIVE:
            break;
         case DisplaySettingsMetric::METRIC_DISPLAY_MODE_NEGATIVE_ONLY:
            if (metric >= 0.0) {
               displayNode = DISPLAY_NODE_DO_NOT;
            }
            break;
         case DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_ONLY:
            if (metric <= 0.0) {
               displayNode = DISPLAY_NODE_DO_NOT;
            }
            break;
      }
      
      if (positiveOnlyPalette) {
         if (dsm->getDisplayMode() == 
             DisplaySettingsMetric::METRIC_DISPLAY_MODE_POSITIVE_AND_NEGATIVE) {         
            if (metric <= 0.0) {
               displayNode = DISPLAY_NODE_DO_NOT;
            }
         }
      }
      
      //
      // Do not color nodes that are not within user scale
      //
      if (userScaleFlag) {
         if ((metric > negMinMetric) &&
             (metric < posMinMetric)) {
            displayNode = DISPLAY_NODE_DO_NOT;
         }
      }
      
      switch (displayNode) {
         case DISPLAY_NODE_NORMAL:
            {
               float normalized = 0.0;
               
               if ((palette->getNumberOfPaletteEntries() == 2) &&
                    interpolateColor) {
                  //
                  // Normalize between [0, 1.0] when two color palette interpolate
                  //
                  float diffMetric = posMaxMetric - negMaxMetric;
                  if (diffMetric == 0.0) {
                     diffMetric = 1.0;
                  }
                  normalized = (metric - negMaxMetric) / diffMetric;
               }
               else {
                  if (metric >= posMinMetric) {
                     const float numerator = metric - posMinMetric;
                     float denominator = posMaxMetric - posMinMetric;
                     if (denominator == 0.0) {
                        denominator = 1.0;
                     }
                     normalized = numerator / denominator; 
                  }
                  else if (metric <= negMinMetric) {
                     const float numerator = metric - negMinMetric;
                     float denominator = negMaxMetric - negMinMetric;
                     if (denominator == 0.0) {
                        denominator = 1.0;
                     }
                     else if (denominator < 0.0) {
                        denominator = -denominator;
                     }
                     normalized = numerator / denominator; 
                     
                     //
                     // allow a "Postive Only" palette with "Negative Only" displayed
                     //
                     if (positiveOnlyPalette &&
                         (dsm->getDisplayMode() == 
                               DisplaySettingsMetric::METRIC_DISPLAY_MODE_NEGATIVE_ONLY)) {
                        normalized = -normalized;
                     }
                  }  
               }
                   
               bool isNoneColor = false;
               unsigned char colors[3];
               palette->getColor(normalized, interpolateColor,
                                 isNoneColor, colors);
               
               if (isNoneColor == false) { 
                  nodeColors[j].r = colors[0];
                  nodeColors[j].g = colors[1]; 
                  nodeColors[j].b = colors[2]; 
               }
            }
            break;
         case DISPLAY_NODE_POS_THRESH_COLOR:
            nodeColors[j].r = posThreshColor[0];
            nodeColors[j].g = posThreshColor[1];
            nodeColors[j].b = posThreshColor[2];
            break;
         case DISPLAY_NODE_NEG_THRESH_COLOR:
            nodeColors[j].r = negThreshColor[0];
            nodeColors[j].g = negThreshColor[1];
            nodeColors[j].b = negThreshColor[2];
            break;
         case DISPLAY_NODE_DO_NOT:
            break;
      }
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to assign metric colors: "
                << (static_cast<float>(timer.elapsed()) / 1000.0) << std::endl;
   }
}   

//---------------------------------------------------------------------------
// ***COLORS
//   dk-blue = #0000ff
//   blue = #0044ff
//   lt-blue1 = #0069ff
//   lt-blue2 = #0099ff
//   blue-cyan = #00ccff
//   cyan = #00ffff
//   green = #00ff00
//   limegreen = #10b010
//   yellow = #ffff00
//   orange = #ff6900
//   oran-red = #ff4400
//   red = #ff0000
//   purple = #66004c
// ***PALETTES  [13]
//   1.000000 -> dk-blue
//   0.640000 -> blue
//   0.460000 -> lt-blue1
//   0.320000 -> lt-blue2
//   0.230000 -> blue-cyan
//   0.160000 -> cyan
//   0.112000 -> green
//   0.080000 -> limegreen
//   0.056000 -> yellow
//   0.040000 -> orange
//   0.028000 -> oran-red
//   0.020000 -> red
//   0.000000 -> purple
void
BrainModelSurfaceNodeColoring::assignTopographyEccentricityPalette()
{
   unsigned char rgb[3];

   eccentricityTopographyPaletteFile.clear();
   Palette palette(&eccentricityTopographyPaletteFile);
   palette.setPositiveOnly(false);
   palette.setName("Eccentricity");
     
   rgb[0] = 0x00;
   rgb[1] = 0x00;
   rgb[2] = 0xff;
   PaletteColor dkBlue("dk-blue", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(dkBlue);
   palette.addPaletteEntry(1.0, dkBlue.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0x44;
   rgb[2] = 0xff;
   PaletteColor blue("blue", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(blue);
   palette.addPaletteEntry(0.64, blue.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0x69;
   rgb[2] = 0xff;
   PaletteColor ltBlue1("lt-blue1", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(ltBlue1);
   palette.addPaletteEntry(0.46, ltBlue1.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0x99;
   rgb[2] = 0xff;
   PaletteColor ltBlue2("lt-blue2", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(ltBlue2);
   palette.addPaletteEntry(0.32, ltBlue2.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0xcc;
   rgb[2] = 0xff;
   PaletteColor blueCyan("blue-cyan", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(blueCyan);
   palette.addPaletteEntry(0.23, blueCyan.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0xff;
   rgb[2] = 0xff;
   PaletteColor cyan("cyan", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(cyan);
   palette.addPaletteEntry(0.16, cyan.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0xff;
   rgb[2] = 0x00;
   PaletteColor green("green", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(green);
   palette.addPaletteEntry(0.112, green.getName());
   
   rgb[0] = 0x10;
   rgb[1] = 0xb0;
   rgb[2] = 0x10;
   PaletteColor limeGreen("limegreen", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(limeGreen);
   palette.addPaletteEntry(0.08, limeGreen.getName());
   
   rgb[0] = 0xff;
   rgb[1] = 0xff;
   rgb[2] = 0x00;
   PaletteColor yellow("yellow", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(yellow);
   palette.addPaletteEntry(0.056, yellow.getName());
   
   rgb[0] = 0xff;
   rgb[1] = 0x69;
   rgb[2] = 0x00;
   PaletteColor orange("orange", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(orange);
   palette.addPaletteEntry(0.04, orange.getName());
   
   rgb[0] = 0xff;
   rgb[1] = 0x44;
   rgb[2] = 0x00;
   PaletteColor oranRed("oran-red", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(oranRed);
   palette.addPaletteEntry(0.028, oranRed.getName());
   
   rgb[0] = 0xff;
   rgb[1] = 0x00;
   rgb[2] = 0x00;
   PaletteColor red("red", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(red);
   palette.addPaletteEntry(0.02, red.getName());
   
   rgb[0] = 0x66;
   rgb[1] = 0x00;
   rgb[2] = 0x4c;
   PaletteColor purple("purple", rgb);
   eccentricityTopographyPaletteFile.addPaletteColor(purple);
   palette.addPaletteEntry(0.00, purple.getName());  
   
   eccentricityTopographyPaletteFile.addPalette(palette);
}
//---------------------------------------------------------------------------
// ***COLORS
//   red = #ff0000
//   yellow = #ffff00
//   green = #00ff00
//   cyan = #00ffff
//   blue = #0044ff
// ***PALETTES  [5]
//   1.000000 -> red
//   0.50000 -> yellow
//   0.00000 -> green
//   -.50000 -> cyan
//  -1.00 -> blue
/**
 * Assign topography polar angle coloring
 */
void
BrainModelSurfaceNodeColoring::assignTopographyPolarAnglePalette()
{
   unsigned char rgb[3];

   polarAngleTopographyPaletteFile.clear();
   Palette palette(&polarAngleTopographyPaletteFile);
   palette.setPositiveOnly(false);
   palette.setName("Polar Angle");  

   rgb[0] = 0xff;
   rgb[1] = 0x00;
   rgb[2] = 0x00;
   PaletteColor red("red", rgb);
   polarAngleTopographyPaletteFile.addPaletteColor(red);
   palette.addPaletteEntry(1.0, red.getName());
     
   rgb[0] = 0xff;
   rgb[1] = 0xff;
   rgb[2] = 0x00;
   PaletteColor yellow("yellow", rgb);
   polarAngleTopographyPaletteFile.addPaletteColor(yellow);
   palette.addPaletteEntry(0.5, yellow.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0xff;
   rgb[2] = 0x00;
   PaletteColor green("green", rgb);
   polarAngleTopographyPaletteFile.addPaletteColor(green);
   palette.addPaletteEntry(0.0, green.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0xff;
   rgb[2] = 0xff;
   PaletteColor cyan("cyan", rgb);
   polarAngleTopographyPaletteFile.addPaletteColor(cyan);
   palette.addPaletteEntry(-0.5, cyan.getName());
   
   rgb[0] = 0x00;
   rgb[1] = 0x44;
   rgb[2] = 0xff;
   PaletteColor blue("blue", rgb);
   polarAngleTopographyPaletteFile.addPaletteColor(blue);
   palette.addPaletteEntry(-1.0, blue.getName());
   
   polarAngleTopographyPaletteFile.addPalette(palette);   
}

/**
 * Assign from topography
 */
void
BrainModelSurfaceNodeColoring::assignTopographyColoring()
{
   TopographyFile* tf = brainSet->getTopographyFile();
   if ((tf->getNumberOfNodes() == 0) || (tf->getNumberOfColumns() == 0)) {
      return;
   }
   
   DisplaySettingsTopography* dst = brainSet->getDisplaySettingsTopography();
   const int column = dst->getSelectedColumn(modelNumber);
   
   const bool showEccentricity = 
      (dst->getDisplayType() == DisplaySettingsTopography::TOPOGRAPHY_DISPLAY_ECCENTRICITY);
      
   const int numNodes = brainSet->getNumberOfNodes();
   if (numNodes != tf->getNumberOfNodes()) {
      std::cerr << "Topography has different number of nodes than surface." << std::endl;
      return;
   }
   
#ifdef Q_OS_WIN32
   float minScalar =  10000000000.0;
   float maxScalar = -minScalar;
#else
   float minScalar =  std::numeric_limits<float>::max();
   float maxScalar = -std::numeric_limits<float>::max();
#endif
   
   bool* topographyValid = new bool[numNodes];
   float* topographyScalar = new float[numNodes];

   for (int h = 0; h < numNodes; h++) {
      topographyValid[h] = false;
   }
   
   for (int i = 0; i < numNodes; i++) {
      const NodeTopography nt = tf->getNodeTopography(i, column);
      float eMean, eLow, eHigh, pMean, pLow, pHigh;
      QString areaName;
      nt.getData(eMean, eLow, eHigh, pMean, pLow, pHigh, areaName);
      if (areaName.isEmpty() == false) {

         topographyValid[i] = true;
         if (showEccentricity) {
            topographyScalar[i] = eMean;
         }
         else {
            topographyScalar[i] = pMean;
         }
         if (topographyScalar[i] > maxScalar) {
            maxScalar = topographyScalar[i];
         }
         if (topographyScalar[i] < minScalar) {
            minScalar = topographyScalar[i];
         }
      }
   }
   
   for (int j = 0; j < numNodes; j++) {
      if (topographyValid[j]) {
         float normalized = 0.0;
         if (topographyScalar[j] >= 0.0) {
            normalized = topographyScalar[j] / maxScalar;
         }
         else {
            normalized = -(topographyScalar[j] / minScalar);
         }      
         
         bool noneColorFlag = false;
         unsigned char colors[3];
         if (showEccentricity) {
            const Palette* pal = eccentricityTopographyPaletteFile.getPalette(0);
            pal->getColor(normalized, false, 
                          noneColorFlag,
                          colors);
         }
         else {
            const Palette* pal = polarAngleTopographyPaletteFile.getPalette(0);
            pal->getColor(normalized, false, 
                          noneColorFlag,
                          colors);
         }
         nodeColors[j].r = colors[0];
         nodeColors[j].g = colors[1];
         nodeColors[j].b = colors[2];
      }
   }
   
   delete[] topographyValid;
   delete[] topographyScalar;
}

/**
 * Clamp a float value to an int in [0, 255]
 */
int
BrainModelSurfaceNodeColoring::clamp0255(const float v) const
{
   int value = static_cast<int>(v + 0.5);
   if (value > 255) value = 255;
   if (value < 0) value = 0;
   return value;
}

/** 
 * Assign surface coloring to the brain surface's nodes.
 */
void
BrainModelSurfaceNodeColoring::assignColors()
{
   QTime timer;
   timer.start();
   
   paintIndicesWithNoAreaColor.clear();
   
   brainSet->clearAllDisplayLists();
   
   const int numNodes = brainSet->getNumberOfNodes();
   if (numNodes < 0) {
      return;
   }
   
   const int numBrainModels = brainSet->getNumberOfBrainModels();
   if (numBrainModels < 0) {
      return;
   }
   
   if ((numNodesLastTime < 0) ||
       (numNodesLastTime != numNodes) ||
       (numBrainModelsLastTime < 0) ||
       (numBrainModelsLastTime != numBrainModels)) {
      if (overlayPrimaryNodeColors != NULL) {
         delete[] overlayPrimaryNodeColors;
      }
      if (overlaySecondaryNodeColors != NULL) {
         delete[] overlaySecondaryNodeColors;
      }
      if (underlayNodeColors != NULL) {
         delete[] underlayNodeColors;
      }
      overlayPrimaryNodeColors = new NodeColor[numNodes];
      overlaySecondaryNodeColors = new NodeColor[numNodes];
      underlayNodeColors = new NodeColor[numNodes];
      nodeColoring.resize(numNodes * 3 * numBrainModels);
      nodeColorSource.resize(numNodes * numBrainModels);
      numNodesLastTime = numNodes;
      const int oldNumBrainModels = numBrainModelsLastTime;
      numBrainModelsLastTime = numBrainModels;
      
      OVERLAY_SELECTIONS primOver = OVERLAY_NONE;
      OVERLAY_SELECTIONS secOver  = OVERLAY_NONE;
      OVERLAY_SELECTIONS under   = OVERLAY_NONE;
      const int firstModelIndex = brainSet->getFirstBrainModelSurfaceIndex();
      if (primaryOverlay.empty() == false) {
         primOver = primaryOverlay[0];
         if ((firstModelIndex >= 0) && 
             (firstModelIndex < static_cast<int>(primaryOverlay.size()))) {
            primOver = primaryOverlay[firstModelIndex];
         }
      }
      if (secondaryOverlay.empty() == false) {
         secOver = secondaryOverlay[0];
         if ((firstModelIndex >= 0) && 
             (firstModelIndex < static_cast<int>(secondaryOverlay.size()))) {
            secOver = secondaryOverlay[firstModelIndex];
         }
      }
      if (underlay.empty() == false) {
         under = underlay[0];
         if ((firstModelIndex >= 0) && 
             (firstModelIndex < static_cast<int>(underlay.size()))) {
            under = underlay[firstModelIndex];
         }
      }
      primaryOverlay.resize(numBrainModels,   primOver);
      secondaryOverlay.resize(numBrainModels, secOver);
      underlay.resize(numBrainModels,         under);
      
      if (oldNumBrainModels > 0) {
         int firstSurfaceIndex = -1;
         for (int i = 0; i < oldNumBrainModels; i++) {
            if (i < numBrainModels) {
               if (brainSet->getBrainModelSurface(i) != NULL) {
                  firstSurfaceIndex = i;
                  break;
               }
            }
         }
         if (firstSurfaceIndex >= 0) {
            for (int i = oldNumBrainModels; i < numBrainModels; i++) {
               primaryOverlay[i]   = primaryOverlay[firstSurfaceIndex];
               secondaryOverlay[i] = secondaryOverlay[firstSurfaceIndex];
               underlay[i]         = underlay[firstSurfaceIndex];
            }
         }
      }      
   }
   else {
/*
      for (int i = 0; i < numNodesLastTime; i++) {
         overlayPrimaryNodeColors[i].reset();
         overlaySecondaryNodeColors[i].reset();
         underlayNodeColors[i].reset();
      }
*/
   }
   
   setDefaultColor();
   
   for (modelNumber = 0; modelNumber < numBrainModels; modelNumber++) {
      //
      // If this NOT a surface
      //
/*
      if ((brainSet->getBrainModelSurface(modelNumber) == NULL) &&
          (brainSet->getBrainModelSurfaceAndVolume(modelNumber) == NULL)) {
         //
         // Skip it
         //
         continue;
      }
*/      
      //
      // offsets into nodeColoring and nodeColorSource
      //
      const int nodeColorSourceOffset = modelNumber * numNodes;
      const int nodeColoringOffset = nodeColorSourceOffset * 3;
      
      //
      // If this is not the first model number
      //
      if (modelNumber > 0) {
         //
         // If this model has the same coloring as the first model
         //
         if ((getUnderlay(modelNumber) == getUnderlay(0)) &&
             (getSecondaryOverlay(modelNumber) == getSecondaryOverlay(0)) &&
             (getPrimaryOverlay(modelNumber) == getPrimaryOverlay(0))) {
            //
            // See if underlay and overlays are same for the surfaces
            //
            DisplaySettings* dsu = NULL;
            switch(getUnderlay(modelNumber)) {
               case OVERLAY_NONE:
                  break;
               case OVERLAY_AREAL_ESTIMATION:
                  dsu = brainSet->getDisplaySettingsArealEstimation();
                  break;
               case OVERLAY_COCOMAC:
                  dsu = brainSet->getDisplaySettingsCoCoMac();
                  break;
               case OVERLAY_METRIC:
                  dsu = brainSet->getDisplaySettingsMetric();
                  break;
               case OVERLAY_PAINT:
                  dsu = brainSet->getDisplaySettingsPaint();
                  break;
               case OVERLAY_PROBABILISTIC_ATLAS:
                  dsu = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
                  break;
               case OVERLAY_RGB_PAINT:
                  dsu = brainSet->getDisplaySettingsRgbPaint();
                  break;
               case OVERLAY_SECTIONS:
                  break;
               case OVERLAY_SHOW_CROSSOVERS:
                  break;
               case OVERLAY_SHOW_EDGES:
                  break;
               case OVERLAY_SURFACE_SHAPE:
                  dsu = brainSet->getDisplaySettingsSurfaceShape();
                  break;
               case OVERLAY_TOPOGRAPHY:
                  dsu = brainSet->getDisplaySettingsTopography();
                  break;
               case OVERLAY_GEOGRAPHY_BLENDING:
                  break;
            }
            
            DisplaySettings* dso2 = NULL;
            switch(getSecondaryOverlay(modelNumber)) {
               case OVERLAY_NONE:
                  break;
               case OVERLAY_AREAL_ESTIMATION:
                  dso2 = brainSet->getDisplaySettingsArealEstimation();
                  break;
               case OVERLAY_COCOMAC:
                  dso2 = brainSet->getDisplaySettingsCoCoMac();
                  break;
               case OVERLAY_METRIC:
                  dso2 = brainSet->getDisplaySettingsMetric();
                  break;
               case OVERLAY_PAINT:
                  dso2 = brainSet->getDisplaySettingsPaint();
                  break;
               case OVERLAY_PROBABILISTIC_ATLAS:
                  dso2 = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
                  break;
               case OVERLAY_RGB_PAINT:
                  dso2 = brainSet->getDisplaySettingsRgbPaint();
                  break;
               case OVERLAY_SECTIONS:
                  break;
               case OVERLAY_SHOW_CROSSOVERS:
                  break;
               case OVERLAY_SHOW_EDGES:
                  break;
               case OVERLAY_SURFACE_SHAPE:
                  dso2 = brainSet->getDisplaySettingsSurfaceShape();
                  break;
               case OVERLAY_TOPOGRAPHY:
                  dso2 = brainSet->getDisplaySettingsTopography();
                  break;
               case OVERLAY_GEOGRAPHY_BLENDING:
                  break;
            }

            DisplaySettings* dso1 = NULL;
            switch(getPrimaryOverlay(modelNumber)) {
               case OVERLAY_NONE:
                  break;
               case OVERLAY_AREAL_ESTIMATION:
                  dso1 = brainSet->getDisplaySettingsArealEstimation();
                  break;
               case OVERLAY_COCOMAC:
                  dso1 = brainSet->getDisplaySettingsCoCoMac();
                  break;
               case OVERLAY_METRIC:
                  dso1 = brainSet->getDisplaySettingsMetric();
                  break;
               case OVERLAY_PAINT:
                  dso1 = brainSet->getDisplaySettingsPaint();
                  break;
               case OVERLAY_PROBABILISTIC_ATLAS:
                  dso1 = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
                  break;
               case OVERLAY_RGB_PAINT:
                  dso1 = brainSet->getDisplaySettingsRgbPaint();
                  break;
               case OVERLAY_SHOW_CROSSOVERS:
                  break;
               case OVERLAY_SHOW_EDGES:
                  break;
               case OVERLAY_SECTIONS:
                  break;
               case OVERLAY_SURFACE_SHAPE:
                  dso1 = brainSet->getDisplaySettingsSurfaceShape();
                  break;
               case OVERLAY_TOPOGRAPHY:
                  dso1 = brainSet->getDisplaySettingsTopography();
                  break;
               case OVERLAY_GEOGRAPHY_BLENDING:
                  break;
            }
            
            bool theSame = true;
            if (dsu != NULL) {
               if (dsu->columnSelectionsAreTheSame(modelNumber, 0) == false) {
                  theSame = false;
               }
            }
            if (dso2 != NULL) {
               if (dso2->columnSelectionsAreTheSame(modelNumber, 0) == false) {
                  theSame = false;
               }
            }
            if (dso1 != NULL) {
               if (dso1->columnSelectionsAreTheSame(modelNumber, 0) == false) {
                  theSame = false;
               }
            }

            if (theSame) {
               //
               // Copy coloring
               //
               for (int i = 0; i < numNodes; i++) {
                  nodeColoring[nodeColoringOffset + i*3]   = nodeColoring[i*3];
                  nodeColoring[nodeColoringOffset + i*3+1] = nodeColoring[i*3+1];
                  nodeColoring[nodeColoringOffset + i*3+2] = nodeColoring[i*3+2];
                  nodeColorSource[nodeColorSourceOffset + i]  = nodeColorSource[i];
               }
               
               //
               // Go to the next model
               //
               continue;
            }
         }
      }

      //
      // Reset the colors
      //
      for (int i = 0; i < numNodesLastTime; i++) {
         overlayPrimaryNodeColors[i].reset();
         overlaySecondaryNodeColors[i].reset();
         underlayNodeColors[i].reset();
      }

      //
      // First, set the nodes' underlay colors
      //
      nodeColors = underlayNodeColors;
      
      //
      // initialize nodes to default color
      // 
      assignNoneColoring();
      
      switch(getUnderlay(modelNumber)) {
         case OVERLAY_NONE:
            break;
         case OVERLAY_AREAL_ESTIMATION:
            assignArealEstimationColoring();
            break;
         case OVERLAY_COCOMAC:
            assignCocomacColoring();
            break;
         case OVERLAY_METRIC:
            assignMetricColoring();
            break;
         case OVERLAY_PAINT:
            assignPaintColoring();
            break;
         case OVERLAY_PROBABILISTIC_ATLAS:
            assignProbabilisticColoring(brainSet->getBrainModelSurface(modelNumber));
            break;
         case OVERLAY_RGB_PAINT:
            assignRgbPaintColoring(true);
            break;
         case OVERLAY_SECTIONS:
            assignSectionColoring();
            break;
         case OVERLAY_SHOW_CROSSOVERS:
            assignCrossoverColoring();
            break;
         case OVERLAY_SHOW_EDGES:
            assignEdgesColoring();
            break;
         case OVERLAY_SURFACE_SHAPE:
            assignSurfaceShapeColoring();
            break;
         case OVERLAY_TOPOGRAPHY:
            assignTopographyColoring();
            break;
         case OVERLAY_GEOGRAPHY_BLENDING:
            // handled later in this method
            break;
      }
      
      //
      // Now set the nodes' secondary overlay colors
      //
      nodeColors = overlaySecondaryNodeColors;
      
      switch(getSecondaryOverlay(modelNumber)) {
         case OVERLAY_NONE:
            break;
         case OVERLAY_AREAL_ESTIMATION:
            assignArealEstimationColoring();
            break;
         case OVERLAY_COCOMAC:
            assignCocomacColoring();
            break;
         case OVERLAY_METRIC:
            assignMetricColoring();
            break;
         case OVERLAY_PAINT:
            assignPaintColoring();
            break;
         case OVERLAY_PROBABILISTIC_ATLAS:
            assignProbabilisticColoring(brainSet->getBrainModelSurface(modelNumber));
            break;
         case OVERLAY_RGB_PAINT:
            assignRgbPaintColoring(false);
            break;
         case OVERLAY_SECTIONS:
            assignSectionColoring();
            break;
         case OVERLAY_SHOW_CROSSOVERS:
            assignCrossoverColoring();
            break;
         case OVERLAY_SHOW_EDGES:
            assignEdgesColoring();
            break;
         case OVERLAY_SURFACE_SHAPE:
            assignSurfaceShapeColoring();
            break;
         case OVERLAY_TOPOGRAPHY:
            assignTopographyColoring();
            break;
         case OVERLAY_GEOGRAPHY_BLENDING:
            std::cout << "WARNING: Geography blending ignore for all but "
                      << "the bottom-most overlay."
                      << std::endl;
            break;
      }
      
      //
      // Now set the nodes' primary overlay colors
      //
      nodeColors = overlayPrimaryNodeColors;
      
      switch(getPrimaryOverlay(modelNumber)) {
         case OVERLAY_NONE:
            break;
         case OVERLAY_AREAL_ESTIMATION:
            assignArealEstimationColoring();
            break;
         case OVERLAY_COCOMAC:
            assignCocomacColoring();
            break;
         case OVERLAY_METRIC:
            assignMetricColoring();
            break;
         case OVERLAY_PAINT:
            assignPaintColoring();
            break;
         case OVERLAY_PROBABILISTIC_ATLAS:
            assignProbabilisticColoring(brainSet->getBrainModelSurface(modelNumber));
            break;
         case OVERLAY_RGB_PAINT:
            assignRgbPaintColoring(false);
            break;
         case OVERLAY_SHOW_CROSSOVERS:
            assignCrossoverColoring();
            break;
         case OVERLAY_SHOW_EDGES:
            assignEdgesColoring();
            break;
         case OVERLAY_SECTIONS:
            assignSectionColoring();
            break;
         case OVERLAY_SURFACE_SHAPE:
            assignSurfaceShapeColoring();
            break;
         case OVERLAY_TOPOGRAPHY:
            assignTopographyColoring();
            break;
         case OVERLAY_GEOGRAPHY_BLENDING:
            std::cout << "WARNING: Geography blending ignore for all but "
                      << "the bottom-most overlay."
                      << std::endl;
            break;
      }
      
      //
      // Ignore opacity if underlay is geography blending
      //
      if (getUnderlay(modelNumber) == OVERLAY_GEOGRAPHY_BLENDING){
         assignBlendGeographyColoring(nodeColoringOffset, nodeColorSourceOffset);
      }
      else {
         const float oneMinusOpacity = 1.0 - opacity;
         
         for (int i = 0; i < numNodes; i++) {
            unsigned char colors[3];
            NODE_COLOR_SOURCE colorSource;
            colors[0] = underlayNodeColors[i].r;
            colors[1] = underlayNodeColors[i].g;
            colors[2] = underlayNodeColors[i].b;
            colorSource = NODE_COLOR_SOURCE_UNDERLAY;

            if (overlayPrimaryNodeColors[i].r >= 0) {
               float r = overlayPrimaryNodeColors[i].r  * opacity
                       + underlayNodeColors[i].r * oneMinusOpacity;
               float g = overlayPrimaryNodeColors[i].g  * opacity
                       + underlayNodeColors[i].g * oneMinusOpacity;
               float b = overlayPrimaryNodeColors[i].b  * opacity
                       + underlayNodeColors[i].b * oneMinusOpacity;
               colors[0] = clamp0255(r);
               colors[1] = clamp0255(g);
               colors[2] = clamp0255(b);
               colorSource = NODE_COLOR_SOURCE_PRIMARY_OVERLAY;
            }
            else if (overlaySecondaryNodeColors[i].r >= 0) {
               float r = overlaySecondaryNodeColors[i].r  * opacity
                       + underlayNodeColors[i].r * oneMinusOpacity;
               float g = overlaySecondaryNodeColors[i].g  * opacity
                       + underlayNodeColors[i].g * oneMinusOpacity;
               float b = overlaySecondaryNodeColors[i].b  * opacity
                       + underlayNodeColors[i].b * oneMinusOpacity;
               colors[0] = clamp0255(r);
               colors[1] = clamp0255(g);
               colors[2] = clamp0255(b);
               colorSource = NODE_COLOR_SOURCE_SECONDARY_OVERLAY;
            }
            nodeColoring[nodeColoringOffset + i*3]   = colors[0];
            nodeColoring[nodeColoringOffset + i*3+1] = colors[1];
            nodeColoring[nodeColoringOffset + i*3+2] = colors[2];
            nodeColorSource[nodeColorSourceOffset + i]  = colorSource;
            
         }
      }
      
      //
      // Apply contrast and brightness
      //
      DisplaySettingsSurface* dsn = brainSet->getDisplaySettingsSurface();
      const float brightness = dsn->getNodeBrightness();
      const float contrast   = dsn->getNodeContrast();
      
      if ((brightness != 0.0) || (contrast != 1.0)) {
         for (int i = 0; i < numNodes; i++) {
            float r = nodeColoring[nodeColoringOffset + i*3];
            float g = nodeColoring[nodeColoringOffset + i*3+1];
            float b = nodeColoring[nodeColoringOffset + i*3+2];
            
            //
            //  Brightness is added
            //
            r += brightness;
            g += brightness;
            b += brightness;
            
            //
            // Contrast is multiplied
            //
            r *= contrast;
            g *= contrast;
            b *= contrast;
            
            //
            // color components may become greater than 255 in which case all of the components should
            // be scaled to the range [0 255] to maintain the proper hue.
            //
            float maxValue = r;
            if (g > maxValue) maxValue = g;
            if (b > maxValue) maxValue = b;
            float scale = 1.0;
            if (maxValue > 255.0) {
               scale = 255.0 / maxValue;
            }
            
            nodeColoring[nodeColoringOffset + i*3]   = clamp0255(r * scale);
            nodeColoring[nodeColoringOffset + i*3+1] = clamp0255(g * scale);
            nodeColoring[nodeColoringOffset + i*3+2] = clamp0255(b * scale);
         }
      }
   
      assignMedialWallOverrideColoring(nodeColoringOffset,
                                       nodeColorSourceOffset);
   }
   
   if (paintIndicesWithNoAreaColor.empty() == false) {
      PaintFile* pf = brainSet->getPaintFile();
      std::cout << "WARNING: Paint names with no corresponding area colors:" << std::endl;
      for (std::set<int>::iterator iter = paintIndicesWithNoAreaColor.begin();
           iter != paintIndicesWithNoAreaColor.end(); iter++) {
         std::cout << "   " << pf->getPaintNameFromIndex(*iter).toAscii().constData() << std::endl;
      }
      std::cout << std::endl;
      paintIndicesWithNoAreaColor.clear();
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to assign colors to surface nodes was "
                << (static_cast<float>(timer.elapsed()) / 1000.0) << std::endl;
   }
}

/**
 * assign medial wall override coloring.
 */
void 
BrainModelSurfaceNodeColoring::assignMedialWallOverrideColoring(const int colorOffset,
                                                                const int sourceOffset)
{
   PaintFile* pf = brainSet->getPaintFile();
   DisplaySettingsPaint* dsp = brainSet->getDisplaySettingsPaint();
   
   if (dsp->getMedialWallOverrideColumnEnabled()) {
      const int paintColumn = dsp->getMedialWallOverrideColumn();
      if ((paintColumn >= 0) && (paintColumn < pf->getNumberOfColumns())) {
         const int medWallIndex = pf->getPaintIndexFromName("MEDIAL.WALL");
         if (medWallIndex > 0) {
            const int numNodes = pf->getNumberOfNodes();
            if (numNodes == 0) {
               return;
            }
            if (numNodes != brainSet->getNumberOfNodes()) {
               std::cerr << "Number of nodes in Paint File does not match surface." << std::endl;
               return;
            }
            
            AreaColorFile* cf = brainSet->getAreaColorFile();
            bool match = false;
            const int colorIndex = cf->getColorIndexByName("MEDIAL.WALL", match);
            if ((colorIndex >= 0) && match) {
               unsigned char r, g, b;
               cf->getColorByIndex(colorIndex, r, g, b);
               for (int i = 0; i < numNodes; i++) {
                  if (pf->getPaint(i, paintColumn) == medWallIndex) {
                     nodeColoring[colorOffset + i*3]   = r;
                     nodeColoring[colorOffset + i*3+1] = g;
                     nodeColoring[colorOffset + i*3+2] = b;
                     nodeColorSource[sourceOffset + i]  = NODE_COLOR_SOURCE_MEDIAL_WALL_OVERRIDE;   
                  }
               }
            }
         }
      }
   }
}

/**
 * get the primary overlay selection.
 */
BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS 
BrainModelSurfaceNodeColoring::getPrimaryOverlay(const int modelIn) const 
{ 
   if (primaryOverlay.empty()) {
      return OVERLAY_NONE;
   }
    
   int model = modelIn;
   if (model < 0) {
      model = 0;
   }
   
   return primaryOverlay[model]; 
}
      
/** 
 * Set the primary overlay selection.
 * For the change to take place, updateNodeColors() must also be called.
 */
void 
BrainModelSurfaceNodeColoring::setPrimaryOverlay(const int model, const OVERLAY_SELECTIONS os) 
{
   if (model < 0) {
      std::fill(primaryOverlay.begin(), primaryOverlay.end(), os);
   }
   else {
      primaryOverlay[model] = os;
   }
   if (os == OVERLAY_SHOW_EDGES) {
      brainSet->classifyNodes(NULL, true);
   }
}

/**
 * get the secondary overlay selection.
 */
BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS 
BrainModelSurfaceNodeColoring::getSecondaryOverlay(const int modelIn) const 
{ 
   if (secondaryOverlay.empty()) {
      return OVERLAY_NONE;
   }
    
   int model = modelIn;
   if (model < 0) {
      model = 0;
   }
   
   return secondaryOverlay[model]; 
}

/** 
 * Set the secondary overlay selection
 * For the change to take place, updateNodeColors() must also be called.
 */
void 
BrainModelSurfaceNodeColoring::setSecondaryOverlay(const int model, const OVERLAY_SELECTIONS os) 
{
   if (model < 0) {
      std::fill(secondaryOverlay.begin(), secondaryOverlay.end(), os);
   }
   else {
      secondaryOverlay[model] = os;
   }
   if (os == OVERLAY_SHOW_EDGES) {
      brainSet->classifyNodes(NULL, true);
   }
}

/**
 * get the underlay selection.
 */
BrainModelSurfaceNodeColoring::OVERLAY_SELECTIONS 
BrainModelSurfaceNodeColoring::getUnderlay(const int modelIn) const 
{ 
   if (underlay.empty()) {
      return OVERLAY_NONE;
   }
    
   int model = modelIn;
   if (model < 0) {
      model = 0;
   }
   
   return underlay[model]; 
}
      
/**
 * set the underlay selection
 * For the change to take place, updateNodeColors() must also be called.
 */
void 
BrainModelSurfaceNodeColoring::setUnderlay(const int model, const OVERLAY_SELECTIONS us) 
{
   if (model < 0) {
      std::fill(underlay.begin(), underlay.end(), us);
   }
   else {
       underlay[model] = us;
   }
}

//
// Strings for showing and saving scenes
//
static const QString underlayNameID("Surface-Underlay-Name");
static const QString secondaryOverlayNameID("Surface-Secondary-Overlay-Name");
static const QString primaryOverlayNameID("Surface-Primary-Overlay-Name");

static const QString ouNoneName("none");
static const QString ouArealEstimationName("areal-estimation");
static const QString ouCocomacName("cocomac");
static const QString ouMetricName("metric");
static const QString ouPaintName("paint");
static const QString ouProbabilisticAtlasName("probabilistic-atlas");
static const QString ouRgbPaintName("rgb-paint");
static const QString ouSectionsName("sections");
static const QString ouShowCrossoversName("show-crossovers");
static const QString ouShowEdgesName("show-edges");
static const QString ouSurfaceShapeName("surface-shape");
static const QString ouTopographyName("topography");
static const QString ouGeographyBlendingName("geography-blending");

static const QString ouGeographyBlending("geographyBlending");
static const QString ouOpacity("opacity");
static const QString ouLighting("lightingOn");
static const QString ouPrimaryLighting("primaryOverlayLightingOn");
static const QString ouSecondaryLighting("secondaryOverlayLightingOn");
static const QString ouUnderlayLighting("underlayLightingOn");

/**
 * apply a scene (set display settings).
 */
void 
BrainModelSurfaceNodeColoring::showScene(const SceneFile::Scene& scene,
                                         QString& errorMessage) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "BrainModelSurfaceNodeColoring") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            const QString value = si->getValueAsString();
            
            if (infoName == ouGeographyBlending) {
               setGeographyBlending(si->getValueAsFloat());
            }
            else if (infoName == ouOpacity) {
               setOpacity(si->getValueAsFloat());
            }
            else if (infoName == ouLighting) {
               setLightingOn(si->getValueAsBool());
            }
            else if (infoName == ouPrimaryLighting) {
               setPrimaryOverlayLightingOn(si->getValueAsBool());
            }
            else if (infoName == ouSecondaryLighting) {
               setSecondaryOverlayLightingOn(si->getValueAsBool());
            }
            else if (infoName == ouUnderlayLighting) {
               setUnderlayLightingOn(si->getValueAsBool());
            }
            
            //
            // Is this for underlay or overlay ?
            //
            if ((infoName == underlayNameID) ||
                (infoName == secondaryOverlayNameID) ||
                (infoName == primaryOverlayNameID)) {
               OVERLAY_SELECTIONS  overlay  = OVERLAY_NONE;
               const QString surfaceName = si->getModelName();
               
               if (value == ouArealEstimationName) {
                  overlay  = OVERLAY_AREAL_ESTIMATION;
                  if (brainSet->getArealEstimationFile()->getNumberOfColumns() <= 0) {
                     errorMessage.append("No Areal Estimation File is loaded.\n");
                  }
               }
               else if (value == ouCocomacName) {
                  overlay  = OVERLAY_COCOMAC;
                  const CocomacConnectivityFile* coco = brainSet->getCocomacFile();
                  if (coco->empty()) {
                     errorMessage.append("No CoCoMac Connectivity File is loaded\n");
                  }
               }
               else if (value == ouMetricName) {
                  overlay  = OVERLAY_METRIC;
                  if (brainSet->getMetricFile()->empty()) {
                     errorMessage.append("No Metric File is loaded.\n");
                  }
               }
               else if (value == ouPaintName) {
                  overlay  = OVERLAY_PAINT;
                  if (brainSet->getPaintFile()->empty()) {
                     errorMessage.append("No Paint File is loaded.\n");
                  }
               }
               else if (value == ouProbabilisticAtlasName) {
                  overlay  = OVERLAY_PROBABILISTIC_ATLAS;
                  if (brainSet->getProbabilisticAtlasSurfaceFile()->empty()) {
                     errorMessage.append("No Probabilistic Atlas File is loaded.\n");
                  }
               }
               else if (value == ouRgbPaintName) {
                  overlay  = OVERLAY_RGB_PAINT;
                  if (brainSet->getRgbPaintFile()->empty()) {
                     errorMessage.append("No RGB Paint File is loaded.\n");
                  }
               }
               else if (value == ouSectionsName) {
                  overlay  = OVERLAY_SECTIONS;
               }
               else if (value == ouShowCrossoversName) {
                  overlay  = OVERLAY_SHOW_CROSSOVERS;
               }
               else if (value == ouShowEdgesName) {
                  overlay  = OVERLAY_SHOW_EDGES;
               }
               else if (value == ouSurfaceShapeName) {
                  overlay  = OVERLAY_SURFACE_SHAPE;
                  if (brainSet->getSurfaceShapeFile()->empty()) {
                     errorMessage.append("No Surface Shape File is loaded.\n");
                  }
               }
               else if (value == ouTopographyName) {
                  overlay  = OVERLAY_TOPOGRAPHY;
                  if (brainSet->getTopographyFile()->empty()) {
                     errorMessage.append("No Topography File is loaded.\n");
                  }
               }
               else if (value == ouGeographyBlendingName) {
                  overlay = OVERLAY_GEOGRAPHY_BLENDING;
                  PaintFile* pf = brainSet->getPaintFile();
                  if (pf->empty()) {
                     errorMessage.append("No Paint File is loaded.\n");
                  }
                  else if (pf->getGeographyColumnNumber() < 0) {
                     errorMessage.append("No Geography Column in Paint File.\n");
                  }
               }
               
               //
               // Handle all surfaces or a specific surface
               //
               int startSurface = 0;
               int endSurface   = brainSet->getNumberOfBrainModels();
               if (surfaceName != SceneFile::SceneInfo::getDefaultSurfacesName()) {
                  endSurface = 0;
                  const BrainModelSurface* bms = brainSet->getBrainModelSurfaceWithFileName(surfaceName);
                  if (bms != NULL) {
                     startSurface = brainSet->getBrainModelIndex(bms);
                     if (startSurface >= 0) {
                        endSurface = startSurface + 1;
                     }
                  }
               }
               
               // 
               // Set the overlay or underlay
               //
               for (int k = startSurface; k < endSurface; k++) {
                  if (infoName == underlayNameID) {
                     setUnderlay(k, overlay);
                  }
                  else if (infoName == secondaryOverlayNameID) {
                     setSecondaryOverlay(k, overlay);
                  }
                  else if (infoName == primaryOverlayNameID) {
                     setPrimaryOverlay(k, overlay);
                  }
               }
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
BrainModelSurfaceNodeColoring::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   if (onlyIfSelected) {
      const int num = brainSet->getNumberOfBrainModels();
      bool haveSurfacesFlag = false;
      for (int i = 0; i < num; i++) {
         if (brainSet->getBrainModelSurface(i) != NULL) {
            haveSurfacesFlag = true;
            break;
         }
      }
      if (haveSurfacesFlag == false) {
         return;
      }
   }
   SceneFile::SceneClass sc("BrainModelSurfaceNodeColoring");
   sc.addSceneInfo(SceneFile::SceneInfo(ouGeographyBlending, getGeographyBlending()));
   sc.addSceneInfo(SceneFile::SceneInfo(ouOpacity, getOpacity()));
   sc.addSceneInfo(SceneFile::SceneInfo(ouLighting, getLightingOn()));
   sc.addSceneInfo(SceneFile::SceneInfo(ouPrimaryLighting, getPrimaryOverlayLightingOn()));
   sc.addSceneInfo(SceneFile::SceneInfo(ouSecondaryLighting, getSecondaryOverlayLightingOn()));
   sc.addSceneInfo(SceneFile::SceneInfo(ouUnderlayLighting, getUnderlayLightingOn()));

   bool didDefaultFlag[3] = { false, false, false };
   
   //
   // Check each brain model
   //
   const int num = brainSet->getNumberOfBrainModels();
   for (int n = 0; n < num; n++) {
      //
      // Is this a surface ?
      //
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(n);
      if (bms != NULL) {
         //
         // Surface index but use zero index if doing all
         //
         int surfaceIndex = n;
         
         //
         // Get name of coordinate file
         //
         const CoordinateFile* cf = bms->getCoordinateFile();
         QString surfaceName = FileUtilities::basename(cf->getFileName());
         
         //
         // Do underlay and both overlays
         //
         for (int i = 0; i < 3; i++) {
            QString ouName;
            QString ouValue;
            if (i == 0) {
               ouName = underlayNameID;
               switch(getUnderlay(surfaceIndex)) {
                  case OVERLAY_NONE:
                     ouValue = ouNoneName;
                     break;
                  case OVERLAY_AREAL_ESTIMATION:
                     ouValue = ouArealEstimationName;
                     break;
                  case OVERLAY_COCOMAC:
                     ouValue = ouCocomacName;
                     break;
                  case OVERLAY_METRIC:
                     ouValue = ouMetricName;
                     break;
                  case OVERLAY_PAINT:
                     ouValue = ouPaintName;
                     break;
                  case OVERLAY_PROBABILISTIC_ATLAS:
                     ouValue = ouProbabilisticAtlasName;
                     break;
                  case OVERLAY_RGB_PAINT:
                     ouValue = ouRgbPaintName;
                     break;
                  case OVERLAY_SECTIONS:
                     ouValue = ouSectionsName;
                     break;
                  case OVERLAY_SHOW_CROSSOVERS:
                     ouValue = ouShowCrossoversName;
                     break;
                  case OVERLAY_SHOW_EDGES:
                     ouValue = ouShowEdgesName;
                     break;
                  case OVERLAY_SURFACE_SHAPE:
                     ouValue = ouSurfaceShapeName;
                     break;
                  case OVERLAY_TOPOGRAPHY:
                     ouValue = ouTopographyName;
                     break;
                  case OVERLAY_GEOGRAPHY_BLENDING:
                     ouValue = ouGeographyBlendingName;
                     break;
               }
            }
            else {
               OVERLAY_SELECTIONS overlay = OVERLAY_NONE;
               if (i == 1) {
                  ouName = primaryOverlayNameID;
                  overlay = getPrimaryOverlay(surfaceIndex);
               }
               else if (i == 2) {
                  ouName = secondaryOverlayNameID;
                  overlay = getSecondaryOverlay(surfaceIndex);
               }
               
               switch (overlay) {
                  case OVERLAY_NONE:
                     ouValue = ouNoneName;
                     break;
                  case OVERLAY_AREAL_ESTIMATION:
                     ouValue = ouArealEstimationName;
                     break;
                  case OVERLAY_COCOMAC:
                     ouValue = ouCocomacName;
                     break;
                  case OVERLAY_METRIC:
                     ouValue = ouMetricName;
                     break;
                  case OVERLAY_PAINT:
                     ouValue = ouPaintName;
                     break;
                  case OVERLAY_PROBABILISTIC_ATLAS:
                     ouValue = ouProbabilisticAtlasName;
                     break;
                  case OVERLAY_RGB_PAINT:
                     ouValue = ouRgbPaintName;
                     break;
                  case OVERLAY_SECTIONS:
                     ouValue = ouSectionsName;
                     break;
                  case OVERLAY_SHOW_CROSSOVERS:
                     ouValue = ouShowCrossoversName;
                     break;
                  case OVERLAY_SHOW_EDGES:
                     ouValue = ouShowEdgesName;
                     break;
                  case OVERLAY_SURFACE_SHAPE:
                     ouValue = ouSurfaceShapeName;
                     break;
                  case OVERLAY_TOPOGRAPHY:
                     ouValue = ouTopographyName;
                     break;
                  case OVERLAY_GEOGRAPHY_BLENDING:
                     ouValue = ouGeographyBlendingName;
                     break;
               }
            }
         
            //
            // Do default first
            //
            if (didDefaultFlag[i] == false) {
               SceneFile::SceneInfo si(ouName, 
                           SceneFile::SceneInfo::getDefaultSurfacesName(), ouValue);
               sc.addSceneInfo(si);
               didDefaultFlag[i] = true;
            }
            
            //
            // Create the scene info for this overlay/underlay
            //
            SceneFile::SceneInfo si(ouName, surfaceName, ouValue);
            sc.addSceneInfo(si);
         } // for (int i
      }  // if (bms != NULL)   
   }  // for n
   
   scene.addSceneClass(sc);
}

/**
 * see if an overlay is of a specific type.
 */
bool 
BrainModelSurfaceNodeColoring::isUnderlayOrOverlay(const OVERLAY_SELECTIONS  ol) const
{
   for (int i = 0; i < static_cast<int>(underlay.size()); i++) {
      if (ol != OVERLAY_NONE) {
         if (ol == underlay[i]) return true;
         if (ol == secondaryOverlay[i]) return true;
         if (ol == primaryOverlay[i]) return true;
      }
   }
   return false;
}                               

