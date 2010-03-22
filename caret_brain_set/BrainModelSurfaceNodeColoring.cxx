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
#include "BrainModelSurfaceOverlay.h"
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
   brainSet = bs;
   defaultColor[0] = 100;
   defaultColor[1] = 100;
   defaultColor[2] = 100;
   defaultColorName = "???";
   numNodesLastTime = -1;
   numBrainModelsLastTime = -1;
   
   probAtlasThreshPaintFile = NULL;
   
   coloringMode = COLORING_MODE_NORMAL;
   
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
BrainModelSurfaceNodeColoring::getNodeColor(const int modelIn, const int indexIn) const 
{
   //
   // Model might be "-1" which for display purposes means all models.
   // In this case, use the first model.
   //
   const int model = (modelIn < 0) ? 0 : modelIn;
   
   const int index = (model * numNodesLastTime * 4) + (indexIn * 4);
   return &nodeColoring[index];
}
      
/**
 * get the color source for a node.
 */
int 
BrainModelSurfaceNodeColoring::getNodeColorSource(const int modelIn, const int indexIn) const 
{
   //
   // Model might be "-1" which for display purposes means all models.
   // In this case, use the first model.
   //
   const int model = (modelIn < 0) ? 0 : modelIn;
   
   const int index = (model * numNodesLastTime) + indexIn;
   return nodeColorSource[index];
}

/**
 * Set the colors for a node.
 */
void
BrainModelSurfaceNodeColoring::setNodeColor(const int modelIn,
                                            const int index, const unsigned char rgb[3],
                                            const unsigned char alpha)
{
   //
   // Model might be "-1" which for display purposes means all models.
   // In this case, use the first model.
   //
   const int model = (modelIn < 0) ? 0 : modelIn;
   
   const int numNodes = brainSet->getNumberOfNodes();
   nodeColoring[model * numNodes * 4 + index * 4]     = rgb[0];
   nodeColoring[model * numNodes * 4 + index * 4 + 1] = rgb[1];
   nodeColoring[model * numNodes * 4 + index * 4 + 2] = rgb[2];
   nodeColoring[model * numNodes * 4 + index * 4 + 3] = alpha;
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
BrainModelSurfaceNodeColoring::assignNoneColoring(const int nodeColoringOffset,
                                                  const int nodeColorSourceOffset)
{
   const int numNodes = brainSet->getNumberOfNodes();
   for (register int i = 0; i < numNodes; i++) {
      nodeColoring[nodeColoringOffset + i*4]   = defaultColor[0];
      nodeColoring[nodeColoringOffset + i*4+1] = defaultColor[1];
      nodeColoring[nodeColoringOffset + i*4+2] = defaultColor[2];
      nodeColoring[nodeColoringOffset + i*4+3] = 255;
      nodeColorSource[nodeColorSourceOffset + i]  = -1;
   }
}

/**
 * Assign RGB Paint Coloring.
 */
void
BrainModelSurfaceNodeColoring::assignRgbPaintColoring(const int overlayNumber,
                                                      const bool underlayFlag)
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
   const int column = dsrp->getSelectedDisplayColumn(modelNumber, overlayNumber);
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
      
      if ((r == 0.0) &&
          (g == 0.0) &&
          (b == 0.0)) {
         // no RGB so do nothing
      }
      else if (positiveDisplay) {
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
               if (r < redThresh) {
                  nodeColors[i].r = (unsigned char)redPF->GetValue(r);
               }
            }
         }
         if (dsrp->getGreenEnabled()) {
            if (g <= 0.0) {
               if (g < greenThresh) {
                  nodeColors[i].g = (unsigned char)greenPF->GetValue(g);
               }
            }
         }
         if (dsrp->getBlueEnabled()) {
            if (b <= 0.0) {
               if (b < blueThresh) {
                  nodeColors[i].b = (unsigned char)bluePF->GetValue(b);
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
   
   redPF->Delete();
   greenPF->Delete();
   bluePF->Delete();
}

/**
 * Assign areal estimation coloring.
 */
void
BrainModelSurfaceNodeColoring::assignArealEstimationColoring(const int overlayNumber)
{
   DisplaySettingsArealEstimation* dsae = brainSet->getDisplaySettingsArealEstimation();
   const int columnNumber = dsae->getSelectedDisplayColumn(modelNumber, overlayNumber);
   
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
BrainModelSurfaceNodeColoring::assignPaintColoring(const int overlayNumber)
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
   const int column = dsp->getSelectedDisplayColumn(modelNumber, overlayNumber);
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
               unsigned char r = 0, g = 0, b = 0, a = 0;
               cf->getColorByIndex(colorFileIndex,
                                    r, g, b, a);
               if (a > 0) {
                  nodeColors[i].r = r;
                  nodeColors[i].g = g;
                  nodeColors[i].b = b;
               }
            }
         }
         else {
            paintIndicesWithNoAreaColor.insert(p);
         }
      }
   }
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
                                                            const int /*sourceOffset*/)
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
   
   DisplaySettingsPaint* dsp = brainSet->getDisplaySettingsPaint();
   const float geographyBlending = dsp->getGeographyBlending();
    
   const int numNodes = brainSet->getNumberOfNodes();
   
   for (int j = 0; j < numNodes; j++) {
      const int paintIndex = pf->getPaint(j, geoColumn);
      if (geoPaint[paintIndex]) {
          const int indx = offset + (j * 4);
          nodeColoring[indx] = 
             static_cast<unsigned char>(nodeColoring[indx]
                                        * geographyBlending);
          nodeColoring[indx + 1] = 
             static_cast<unsigned char>(nodeColoring[indx + 1]
                                        * geographyBlending);
          nodeColoring[indx + 2] = 
             static_cast<unsigned char>(nodeColoring[indx + 2]
                                        * geographyBlending);
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
BrainModelSurfaceNodeColoring::assignSectionColoring(const int overlayNumber)
{
   SectionFile* sf = brainSet->getSectionFile();
   const int numNodes = sf->getNumberOfNodes();
   if (numNodes != brainSet->getNumberOfNodes()) {
      std::cout << "ERROR: Section file has different number of nodes than surfaces." << std::endl;
      return;
   }
   
   const DisplaySettingsSection* dss = brainSet->getDisplaySettingsSection();
   int selectedSection = -100000;
   bool sectionEveryX  = false;
   dss->getSectionHighlighting(selectedSection, sectionEveryX);
   
   DisplaySettingsSection* ds = brainSet->getDisplaySettingsSection();
   
   const int sectionSetNumber = ds->getSelectedDisplayColumn(-1, overlayNumber);
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
BrainModelSurfaceNodeColoring::assignSurfaceShapeColoring(const int overlayNumber)
{
   const SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
   const DisplaySettingsSurfaceShape* dsss = brainSet->getDisplaySettingsSurfaceShape();
   
   const int column = dsss->getSelectedDisplayColumn(modelNumber, overlayNumber);
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
}

/**
 * Assign metric coloring.
 */
void
BrainModelSurfaceNodeColoring::assignMetricColoring(const int overlayNumber)
{
   QTime timer;
   timer.start();
   
   MetricFile* mf = brainSet->getMetricFile();
   DisplaySettingsMetric* dsm = brainSet->getDisplaySettingsMetric();
   
   const int viewIndex = dsm->getSelectedDisplayColumn(modelNumber, overlayNumber);
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
         overlayScale = DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO;
      }
   }
   
   bool userScaleFlag = false;
   switch (overlayScale) {
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO:
         mf->getDataColumnMinMax(dsm->getSelectedDisplayColumn(modelNumber, overlayNumber), //dsm->getFirstSelectedColumnForBrainModel(modelNumber),
                                 negMaxMetric, posMaxMetric);
         break;
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_PERCENTAGE:
         mf->getMinMaxValuesFromPercentages(dsm->getSelectedDisplayColumn(modelNumber, overlayNumber),
                                            dsm->getAutoScalePercentageNegativeMaximum(),
                                            dsm->getAutoScalePercentageNegativeMinimum(),
                                            dsm->getAutoScalePercentagePositiveMinimum(),
                                            dsm->getAutoScalePercentagePositiveMaximum(),
                                            negMaxMetric,
                                            negMinMetric,
                                            posMinMetric,
                                            posMaxMetric);
         break;
      case DisplaySettingsMetric::METRIC_OVERLAY_SCALE_AUTO_SPECIFIED_COLUMN:
         mf->getDataColumnMinMax(dsm->getOverlayScaleSpecifiedColumnNumber(),
                                 negMaxMetric, posMaxMetric);
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
   
   const int thresholdIndex = dsm->getSelectedThresholdColumn(modelNumber, overlayNumber);
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
BrainModelSurfaceNodeColoring::assignTopographyColoring(const int overlayNumber)
{
   TopographyFile* tf = brainSet->getTopographyFile();
   if ((tf->getNumberOfNodes() == 0) || (tf->getNumberOfColumns() == 0)) {
      return;
   }
   
   DisplaySettingsTopography* dst = brainSet->getDisplaySettingsTopography();
   const int column = dst->getSelectedDisplayColumn(modelNumber, overlayNumber);
   
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
   
   const int numberOfSurfaceOverlays = brainSet->getNumberOfSurfaceOverlays();
   
   if ((numNodesLastTime < 0) ||
       (numNodesLastTime != numNodes) ||
       (numBrainModelsLastTime < 0) ||
       (numBrainModelsLastTime != numBrainModels)) {
      nodeColors.resize(numNodes);
      nodeColoring.resize(numNodes * 4 * numBrainModels);
      nodeColorSource.resize(numNodes * numBrainModels);
      numNodesLastTime = numNodes;
      numBrainModelsLastTime = numBrainModels;
   }
   
   setDefaultColor();
   
   for (modelNumber = 0; modelNumber < numBrainModels; modelNumber++) {
      //
      // offsets into nodeColoring and nodeColorSource
      //
      const int nodeColorSourceOffset = modelNumber * numNodes;
      const int nodeColoringOffset = nodeColorSourceOffset * 4;
      
      //
      // If this is not the first model number
      //
      if (modelNumber > 0) {
         //
         // If this model has the same coloring as the first model
         //
         bool copySurfaceColorsFlag = true;
         for (int i = 0; i < numberOfSurfaceOverlays; i++) {
            //
            // Are the two overlays set the same ?
            //
            BrainModelSurfaceOverlay* bmsOverlay = brainSet->getSurfaceOverlay(i);
            if (bmsOverlay->getOverlay(modelNumber) == 
                bmsOverlay->getOverlay(0)) {
               DisplaySettingsNodeAttributeFile* dsnaf = NULL;
               switch(bmsOverlay->getOverlay(modelNumber)) {
                  case BrainModelSurfaceOverlay::OVERLAY_NONE:
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION:
                     dsnaf = brainSet->getDisplaySettingsArealEstimation();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_COCOMAC:
                     //dsnaf = brainSet->getDisplaySettingsCoCoMac();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_METRIC:
                     dsnaf = brainSet->getDisplaySettingsMetric();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_PAINT:
                     dsnaf = brainSet->getDisplaySettingsPaint();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_PROBABILISTIC_ATLAS:
                     //dsnaf = brainSet->getDisplaySettingsProbabilisticAtlasSurface();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT:
                     dsnaf = brainSet->getDisplaySettingsRgbPaint();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_SECTIONS:
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS:
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_SHOW_EDGES:
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE:
                     dsnaf = brainSet->getDisplaySettingsSurfaceShape();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY:
                     dsnaf = brainSet->getDisplaySettingsTopography();
                     break;
                  case BrainModelSurfaceOverlay::OVERLAY_GEOGRAPHY_BLENDING:
                     break;
               }
               if (dsnaf != NULL) {
                  if (dsnaf->columnSelectionsAreTheSame(modelNumber, 0) == false) {
                     copySurfaceColorsFlag = false;
                  }
               }
            }
            else {
               copySurfaceColorsFlag = false;
               break;
            }
         }

         if (copySurfaceColorsFlag) {
            //
            // Copy coloring
            //
            for (int i = 0; i < numNodes; i++) {
               nodeColoring[nodeColoringOffset + i*4]   = nodeColoring[i*4];
               nodeColoring[nodeColoringOffset + i*4+1] = nodeColoring[i*4+1];
               nodeColoring[nodeColoringOffset + i*4+2] = nodeColoring[i*4+2];
               nodeColoring[nodeColoringOffset + i*4+3] = nodeColoring[i*4+3];
               nodeColorSource[nodeColorSourceOffset + i]  = nodeColorSource[i];
            }
            
            //
            // Go to the next model
            //
            continue;
         }
      }

      //
      // initialize nodes to default color
      // 
      assignNoneColoring(nodeColoringOffset,
                         nodeColorSourceOffset);
      
      //
      // Use selected coloring mode
      //
      switch (coloringMode) {
         case COLORING_MODE_NORMAL:
            {
               //
               // Loop through the overlays and assign the colors
               //
               for (int iso = 0; iso < numberOfSurfaceOverlays; iso++) {
                  //
                  // Reset the colors
                  //
                  for (int i = 0; i < numNodes; i++) {
                     nodeColors[i].reset();
                  }

                  //
                  // Get the surface overlay
                  //
                  const BrainModelSurfaceOverlay* bmsOverlay = brainSet->getSurfaceOverlay(iso);
                  
                  //
                  // Color using the overlay
                  //
                  switch (bmsOverlay->getOverlay(modelNumber)) {
                     case BrainModelSurfaceOverlay::OVERLAY_NONE:
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION:
                        assignArealEstimationColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_COCOMAC:
                        assignCocomacColoring();
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_METRIC:
                        assignMetricColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_PAINT:
                        assignPaintColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_PROBABILISTIC_ATLAS:
                        assignProbabilisticColoring(brainSet->getBrainModelSurface(modelNumber));
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT:
                        assignRgbPaintColoring(iso, false);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SECTIONS:
                        assignSectionColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS:
                        assignCrossoverColoring();
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SHOW_EDGES:
                        assignEdgesColoring();
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE:
                        assignSurfaceShapeColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY:
                        assignTopographyColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_GEOGRAPHY_BLENDING:
                        // handled later in this method
                        break;
                  }

                  //
                  // Get opacity for blending overlays
                  //         
                  const float overlayOpacity = bmsOverlay->getOpacity();
                  const float oneMinusOverlayOpacity = 1.0 - overlayOpacity;
                  
                  //
                  // Apply coloring to nodes
                  //
                  for (int i = 0; i < numNodes; i++) {
                     //
                     // Was color applied to the node for this overlay
                     //
                     if ((nodeColors[i].r >= 0) ||
                         (nodeColors[i].g >= 0) ||
                         (nodeColors[i].b >= 0)) {
                        float r = nodeColors[i].r  * overlayOpacity
                                + nodeColoring[nodeColoringOffset + i*4] * oneMinusOverlayOpacity;
                        float g = nodeColors[i].g  * overlayOpacity
                                + nodeColoring[nodeColoringOffset + i*4+1] * oneMinusOverlayOpacity;
                        float b = nodeColors[i].b  * overlayOpacity
                                + nodeColoring[nodeColoringOffset + i*4+2] * oneMinusOverlayOpacity;
                        nodeColoring[nodeColoringOffset + i*4]   = clamp0255(r);
                        nodeColoring[nodeColoringOffset + i*4+1] = clamp0255(g);
                        nodeColoring[nodeColoringOffset + i*4+2] = clamp0255(b);
                        nodeColoring[nodeColoringOffset + i*4+3] = 255;
                        nodeColorSource[nodeColorSourceOffset + i]  = iso;
                     }
                  }
                  
                  //
                  // Special case for geography blending do if previous overlay was
                  // geography blending
                  //
                  if (iso > 0) {
                     if (brainSet->getSurfaceOverlay(iso - 1)->getOverlay(modelNumber) ==
                         BrainModelSurfaceOverlay::OVERLAY_GEOGRAPHY_BLENDING) {
                        assignBlendGeographyColoring(nodeColoringOffset,
                                                     nodeColorSourceOffset);
                     }
                  }         
               }
            }
            break;
         case COLORING_MODE_OVERLAY_BLENDING:
            {
               //
               // Node coloring for the three most primary overlays and the underlay
               //
               std::vector<NodeColor> primaryOverlayColors;
               std::vector<NodeColor> secondaryOverlayColors;
               std::vector<NodeColor> tertiaryOverlayColors;
               std::vector<NodeColor> underlayColors;
               
               //
               // Indices of overlays
               //
               const int primaryOverlayIndex = numberOfSurfaceOverlays - 1;
               const int secondaryOverlayIndex = numberOfSurfaceOverlays - 2;
               const int tertiaryOverlayIndex = numberOfSurfaceOverlays - 3;

               //
               // Loop through the overlays and assign the colors
               //
               for (int iso = 0; iso < numberOfSurfaceOverlays; iso++) {
                  //
                  // Reset the colors
                  //
                  for (int i = 0; i < numNodes; i++) {
                     nodeColors[i].reset();
                  }

                  //
                  // Get the surface overlay
                  //
                  const BrainModelSurfaceOverlay* bmsOverlay = brainSet->getSurfaceOverlay(iso);
                  
                  //
                  // Color using the overlay
                  //
                  switch (bmsOverlay->getOverlay(modelNumber)) {
                     case BrainModelSurfaceOverlay::OVERLAY_NONE:
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION:
                        assignArealEstimationColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_COCOMAC:
                        assignCocomacColoring();
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_METRIC:
                        assignMetricColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_PAINT:
                        assignPaintColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_PROBABILISTIC_ATLAS:
                        assignProbabilisticColoring(brainSet->getBrainModelSurface(modelNumber));
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT:
                        assignRgbPaintColoring(iso, false);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SECTIONS:
                        assignSectionColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS:
                        assignCrossoverColoring();
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SHOW_EDGES:
                        assignEdgesColoring();
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE:
                        assignSurfaceShapeColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY:
                        assignTopographyColoring(iso);
                        break;
                     case BrainModelSurfaceOverlay::OVERLAY_GEOGRAPHY_BLENDING:
                        // handled later in this method
                        break;
                  }
                  
                  //
                  // Assign the colors to the proper underlay/overlay
                  //
                  if (iso == 0) {
                     underlayColors = nodeColors;
                  }
                  else if (iso == primaryOverlayIndex) {
                     primaryOverlayColors = nodeColors;
                  }
                  else if (iso == secondaryOverlayIndex) {
                     secondaryOverlayColors = nodeColors;
                  }
                  else if (iso == tertiaryOverlayIndex) {
                     tertiaryOverlayColors = nodeColors;
                  }
               }
               
               //
               // Blend the colors
               //
               for (int i = 0; i < numNodes; i++) {
                  //
                  // Was color applied to the node for this overlay
                  //
                  if (primaryOverlayColors[i].isValid() ||
                      secondaryOverlayColors[i].isValid() ||
                      tertiaryOverlayColors[i].isValid()) {
                     float r = 0.0, g = 0.0, b = 0.0;
                     float sum = 0.0;
                     float maxBefore = 0.0;
                     int overlayColorSource = 0;
                      if (tertiaryOverlayColors[i].isValid()) {
                        r += tertiaryOverlayColors[i].r;
                        g += tertiaryOverlayColors[i].g;
                        b += tertiaryOverlayColors[i].b;
                        overlayColorSource = primaryOverlayIndex;
                        sum += 1.0;
                        
                        maxBefore = std::max(tertiaryOverlayColors[i].r,
                                             std::max(tertiaryOverlayColors[i].g,
                                                      tertiaryOverlayColors[i].b));
                     }
                    if (secondaryOverlayColors[i].isValid()) {
                        r += secondaryOverlayColors[i].r;
                        g += secondaryOverlayColors[i].g;
                        b += secondaryOverlayColors[i].b;
                        overlayColorSource = secondaryOverlayIndex;
                        sum += 1.0;
                        
                        maxBefore = std::max(secondaryOverlayColors[i].r,
                                             std::max(secondaryOverlayColors[i].g,
                                                      secondaryOverlayColors[i].b));
                     }
                     if (primaryOverlayColors[i].isValid()) {
                        r += primaryOverlayColors[i].r;
                        g += primaryOverlayColors[i].g;
                        b += primaryOverlayColors[i].b;
                        overlayColorSource = tertiaryOverlayIndex;
                        sum += 1.0;
                        
                        maxBefore = std::max(primaryOverlayColors[i].r,
                                             std::max(primaryOverlayColors[i].g,
                                                      primaryOverlayColors[i].b));
                     }
                     if (sum > 0.0) {
                        const float maxAfter = std::max(r, std::max(g, b));
                        const float scale = maxBefore / maxAfter;
                        
                        sum = 1.0;
                        nodeColoring[nodeColoringOffset + i*4]   = clamp0255(r / sum);
                        nodeColoring[nodeColoringOffset + i*4+1] = clamp0255(g / sum);
                        nodeColoring[nodeColoringOffset + i*4+2] = clamp0255(b / sum);
                        
                        nodeColoring[nodeColoringOffset + i*4]   = clamp0255(r * scale);
                        nodeColoring[nodeColoringOffset + i*4+1] = clamp0255(g * scale);
                        nodeColoring[nodeColoringOffset + i*4+2] = clamp0255(b * scale);
                        nodeColoring[nodeColoringOffset + i*4+3] = 255;
                        nodeColorSource[nodeColorSourceOffset + i]  = overlayColorSource;
                     }
                  }
                  else if (underlayColors[i].isValid()) {
                     nodeColoring[nodeColoringOffset + i*4]   = clamp0255(underlayColors[i].r);
                     nodeColoring[nodeColoringOffset + i*4+1] = clamp0255(underlayColors[i].g);
                     nodeColoring[nodeColoringOffset + i*4+2] = clamp0255(underlayColors[i].b);
                     nodeColoring[nodeColoringOffset + i*4+3] = 255;
                     nodeColorSource[nodeColorSourceOffset + i]  = 0;
                  }
               }
            }
            break;
      }
      
      //
      // Apply contrast and brightness
      //
      DisplaySettingsSurface* dsn = brainSet->getDisplaySettingsSurface();
      const float brightness = dsn->getNodeBrightness();
      const float contrast   = dsn->getNodeContrast();
      
      if ((brightness != 0.0) || (contrast != 1.0)) {
         for (int i = 0; i < numNodes; i++) {
            float r = nodeColoring[nodeColoringOffset + i*4];
            float g = nodeColoring[nodeColoringOffset + i*4+1];
            float b = nodeColoring[nodeColoringOffset + i*4+2];
            
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
            
            nodeColoring[nodeColoringOffset + i*4]   = clamp0255(r * scale);
            nodeColoring[nodeColoringOffset + i*4+1] = clamp0255(g * scale);
            nodeColoring[nodeColoringOffset + i*4+2] = clamp0255(b * scale);
         }
      }
      
      //
      // Apply opacity
      //
      const float surfaceOpacity = dsn->getOpacity();
      for (int i = 0; i < numNodes; i++) {
         nodeColoring[nodeColoringOffset + i*4+3] = clamp0255(surfaceOpacity * 255.0);
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
                     nodeColoring[colorOffset + i*4]   = r;
                     nodeColoring[colorOffset + i*4+1] = g;
                     nodeColoring[colorOffset + i*4+2] = b;
                     nodeColorSource[sourceOffset + i]  = -1;   
                  }
               }
            }
         }
      }
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
   coloringMode = COLORING_MODE_NORMAL;

   const int numOverlays = brainSet->getNumberOfSurfaceOverlays();
   const int primaryOverlayNumber = numOverlays - 1;
   const int secondaryOverlayNumber = numOverlays - 2;
   const int underlayNumber = 0;

   DisplaySettingsPaint* dsp = brainSet->getDisplaySettingsPaint();
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "BrainModelSurfaceNodeColoring") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            const QString value = si->getValueAsString();
            
            if (infoName == "coloringMode") {
               if (value == "NORMAL") {
                  coloringMode = COLORING_MODE_NORMAL;
                  break;
               }
               else if (value == "BLENDING") {
                  coloringMode = COLORING_MODE_OVERLAY_BLENDING;
                  break;
               }
            }
            
            //
            // NOTE: ALL OF THESE SCENE ITEMS SUPPORT SCENES MADE PRIOR TO
            // THE NEW OVERLAY SYSTEM
            //
            if (infoName == ouGeographyBlending) {
               dsp->setGeographyBlending(si->getValueAsFloat());
            }
            else if (infoName == ouOpacity) {
               for (int i = 0; i < numOverlays; i++) {
                  brainSet->getSurfaceOverlay(i)->setOpacity(si->getValueAsFloat());
               }
            }
            else if (infoName == ouLighting) {
               //setLightingOn(si->getValueAsBool());
            }
            else if (infoName == ouPrimaryLighting) {
               brainSet->getSurfaceOverlay(primaryOverlayNumber)->setLightingEnabled(si->getValueAsBool());
            }
            else if (infoName == ouSecondaryLighting) {
               brainSet->getSurfaceOverlay(secondaryOverlayNumber)->setLightingEnabled(si->getValueAsBool());
            }
            else if (infoName == ouUnderlayLighting) {
               brainSet->getSurfaceOverlay(underlayNumber)->setLightingEnabled(si->getValueAsBool());
            }
            
            //
            // Is this for underlay or overlay ?
            //
            if ((infoName == underlayNameID) ||
                (infoName == secondaryOverlayNameID) ||
                (infoName == primaryOverlayNameID)) {
               BrainModelSurfaceOverlay::OVERLAY_SELECTIONS  overlay  = BrainModelSurfaceOverlay::OVERLAY_NONE;
               const QString surfaceName = si->getModelName();
               
               if (value == ouArealEstimationName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_AREAL_ESTIMATION;
                  if (brainSet->getArealEstimationFile()->getNumberOfColumns() <= 0) {
                     errorMessage.append("Areal Estimation File is overlay/underlay but no Areal Estimation File is loaded.\n");
                  }
               }
               else if (value == ouCocomacName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_COCOMAC;
                  const CocomacConnectivityFile* coco = brainSet->getCocomacFile();
                  if (coco->empty()) {
                     errorMessage.append("CoCoMac File is overlay/underlay but no CoCoMac File is loaded.\n");
                  }
               }
               else if (value == ouMetricName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_METRIC;
                  if (brainSet->getMetricFile()->empty()) {
                     errorMessage.append("Metric File is overlay/underlay but no Metric File is loaded.\n");
                  }
               }
               else if (value == ouPaintName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_PAINT;
                  if (brainSet->getPaintFile()->empty()) {
                     errorMessage.append("Paint File is overlay/underlay but no Paint File is loaded.\n");
                  }
               }
               else if (value == ouProbabilisticAtlasName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_PROBABILISTIC_ATLAS;
                  if (brainSet->getProbabilisticAtlasSurfaceFile()->empty()) {
                     errorMessage.append("Prob Atlas File is overlay/underlay but no Prob Atlas File is loaded.\n");
                  }
               }
               else if (value == ouRgbPaintName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT;
                  if (brainSet->getRgbPaintFile()->empty()) {
                     errorMessage.append("RGB Paint File is overlay/underlay but no RGB Paint File is loaded.\n");
                  }
               }
               else if (value == ouSectionsName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_SECTIONS;
               }
               else if (value == ouShowCrossoversName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS;
               }
               else if (value == ouShowEdgesName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_SHOW_EDGES;
               }
               else if (value == ouSurfaceShapeName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE;
                  if (brainSet->getSurfaceShapeFile()->empty()) {
                     errorMessage.append("Surface Shape File is overlay/underlay but no Surface Shape File is loaded.\n");
                  }
               }
               else if (value == ouTopographyName) {
                  overlay  = BrainModelSurfaceOverlay::OVERLAY_TOPOGRAPHY;
                  if (brainSet->getTopographyFile()->empty()) {
                     errorMessage.append("Topography File is overlay/underlay but no Topography File is loaded.\n");
                  }
               }
               else if (value == ouGeographyBlendingName) {
                  overlay = BrainModelSurfaceOverlay::OVERLAY_GEOGRAPHY_BLENDING;
                  PaintFile* pf = brainSet->getPaintFile();
                  if (pf->empty()) {
                     errorMessage.append("Geography Blending is overlay/underlay but no Paint File is loaded.\n");
                  }
                  else if (pf->getGeographyColumnNumber() < 0) {
                     errorMessage.append("Geography Blending is overlay/underlay but no Geography Column in Paint File.\n");
                  }
               }
               
               //
               // Handle all surfaces or a specific surface
               //
               int startSurface = 0;
               int endSurface   = brainSet->getNumberOfBrainModels();
               if (surfaceName != SceneFile::SceneInfo::getDefaultSurfacesName()) {
                  endSurface = 0;
                  const BrainModelSurface* bms = brainSet->getBrainModelSurfaceWithCoordinateFileName(surfaceName);
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
                     brainSet->getSurfaceOverlay(underlayNumber)->setOverlay(k, overlay);
                  }
                  else if (infoName == secondaryOverlayNameID) {
                     brainSet->getSurfaceOverlay(secondaryOverlayNumber)->setOverlay(k, overlay);
                  }
                  else if (infoName == primaryOverlayNameID) {
                     brainSet->getSurfaceOverlay(primaryOverlayNumber)->setOverlay(k, overlay);
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
BrainModelSurfaceNodeColoring::saveScene(SceneFile::Scene& scene, 
                                          const bool /*onlyIfSelected*/)
{
   SceneFile::SceneClass sc("BrainModelSurfaceNodeColoring");
   switch (coloringMode) {
      case COLORING_MODE_NORMAL:
         sc.addSceneInfo(SceneFile::SceneInfo("coloringMode", 
                                              "NORMAL"));
         break;
      case COLORING_MODE_OVERLAY_BLENDING:
         sc.addSceneInfo(SceneFile::SceneInfo("coloringMode", 
                                              "BLENDING"));
         break;
   }
   scene.addSceneClass(sc);
}
