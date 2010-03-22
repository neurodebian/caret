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

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsSection.h"
#include "DisplaySettingsSurface.h"
#include "StringUtilities.h"

/**
 * The constructor.
 */
DisplaySettingsSurface::DisplaySettingsSurface(BrainSet* bs)
   : DisplaySettings(bs)
{
   drawMode = DRAW_MODE_TILES_WITH_LIGHT;
   nodeBrightness = 0.0;
   nodeContrast   = 1.0;
   nodeSize       = 2;
   linkSize       = 2;
   forceVectorDisplayLength = 10.0;
   
   opacity = 1.0;
   
   identifyNodeColor = IDENTIFY_NODE_COLOR_GREEN;
   
   showNormals               = false;
   showMorphingTotalForces   = false;
   showMorphingAngularForces = false;
   showMorphingLinearForces  = false;

   viewingProjection = VIEWING_PROJECTION_ORTHOGRAPHIC;

   showSurfaceAxes = false;
   showSurfaceAxesLetters = true;
   showSurfaceAxesHashMarks = true;
   surfaceAxesLength = 110.0;
   
   surfaceAxesOffset[0] = 0.0;
   surfaceAxesOffset[1] = 0.0;
   surfaceAxesOffset[2] = 0.0;
   
   for (int i = 0; i < CLIPPING_PLANE_AXIS_NUMBER_OF; i++) {
      this->clippingPlaneEnabled[i] = false;
   }
   this->clippingPlaneCoordinate[0] = -100.0;
   this->clippingPlaneCoordinate[1] =  100.0;
   this->clippingPlaneCoordinate[2] = -100.0;
   this->clippingPlaneCoordinate[3] =  100.0;
   this->clippingPlaneCoordinate[4] = -100.0;
   this->clippingPlaneCoordinate[5] =  100.0;
   this->clippingPlaneApplication = CLIPPING_PLANE_APPLICATION_MAIN_WINDOW_ONLY;

   reset();
}

/**
 * The destructor.
 */
DisplaySettingsSurface::~DisplaySettingsSurface()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsSurface::reset()
{
}   

/**
 * Update any selections do to changes in loaded surface shape file.
 */
void
DisplaySettingsSurface::update()
{
}

/**
 * set the drawing mode.
 */
void 
DisplaySettingsSurface::setDrawMode(const DRAW_MODE dm) 
{ 
   drawMode = dm;
      
   bool lightOn = false;
   switch (drawMode) {
      case DRAW_MODE_NODES:
         break;
      case DRAW_MODE_LINKS:
         break;
      case DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL:
         break;
      case DRAW_MODE_LINKS_EDGES_ONLY:
         break;
      case DRAW_MODE_NODES_AND_LINKS:
         break;
      case DRAW_MODE_TILES:
         break;
      case DRAW_MODE_TILES_WITH_LIGHT:
         lightOn = true;
         break;
      case DRAW_MODE_TILES_WITH_LIGHT_NO_BACK:
         lightOn = true;
      case DRAW_MODE_TILES_LINKS_NODES:
         lightOn = true;
         break;
      case DRAW_MODE_NONE:
         break;
   }
}

/**
 * Get show surface axes info.
 */
void 
DisplaySettingsSurface::getSurfaceAxesInfo(bool& showAxes,
                                           bool& showLetters,
                                           bool& showHashMarks,
                                           float& axesLength,
                                           float axesOffset[3]) const
{
   showAxes = showSurfaceAxes;
   showLetters  = showSurfaceAxesLetters;
   showHashMarks = showSurfaceAxesHashMarks;
   axesLength = surfaceAxesLength;
   
   for (int i = 0; i < 3; i++) {
      axesOffset[i] = surfaceAxesOffset[i];
   }
}
                    
/**
 * set show surface axes info.
 */
void 
DisplaySettingsSurface::setSurfaceAxesInfo(const bool showAxes,
                                           const bool showLetters,
                                           const bool showHashMarks,
                                           const float axesLength,
                                           const float axesOffset[3])
{
   showSurfaceAxes = showAxes;
   showSurfaceAxesLetters = showLetters;
   showSurfaceAxesHashMarks = showHashMarks;
   surfaceAxesLength = axesLength;
   for (int i = 0; i < 3; i++) {
      surfaceAxesOffset[i] = axesOffset[i];
   }
}                          

static const QString DRAW_MODE_NODES_VALUE("DRAW_MODE_NODES");
static const QString DRAW_MODE_LINKS_VALUE("DRAW_MODE_LINKS");
static const QString DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL_VALUE("DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL");
static const QString DRAW_MODE_LINKS_EDGES_ONLY_VALUE("DRAW_MODE_LINKS_EDGES_ONLY");
static const QString DRAW_MODE_NODES_AND_LINKS_VALUE("DRAW_MODE_NODES_AND_LINKS");
static const QString DRAW_MODE_TILES_VALUE("DRAW_MODE_TILES");
static const QString DRAW_MODE_TILES_WITH_LIGHT_VALUE("DRAW_MODE_TILES_WITH_LIGHT");
static const QString DRAW_MODE_TILES_WITH_LIGHT_NO_BACK_VALUE("DRAW_MODE_TILES_WITH_LIGHT_NO_BACK");
static const QString DRAW_MODE_TILES_LINKS_NODES_VALUE("DRAW_MODE_TILES_LINKS_NODES");
static const QString DRAW_MODE_NONE_VALUE("DRAW_MODE_NONE");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsSurface::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsSurface") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "surfaceDrawMode") {
               const QString drawModeValue = si->getValueAsString();
               if (drawModeValue == DRAW_MODE_NODES_VALUE) {
                  drawMode = DRAW_MODE_NODES;
               }
               else if (drawModeValue == DRAW_MODE_LINKS_VALUE) {
                  drawMode = DRAW_MODE_LINKS;
               }
               else if (drawModeValue == DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL_VALUE) {
                  drawMode = DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL;
               }
               else if (drawModeValue == DRAW_MODE_LINKS_EDGES_ONLY_VALUE) {
                  drawMode = DRAW_MODE_LINKS_EDGES_ONLY;
               }
               else if (drawModeValue == DRAW_MODE_NODES_AND_LINKS_VALUE) {
                  drawMode = DRAW_MODE_NODES_AND_LINKS;
               }
               else if (drawModeValue == DRAW_MODE_TILES_VALUE) {
                  drawMode = DRAW_MODE_TILES;
               }
               else if (drawModeValue == DRAW_MODE_TILES_WITH_LIGHT_VALUE) {
                  drawMode = DRAW_MODE_TILES_WITH_LIGHT;
               }
               else if (drawModeValue == DRAW_MODE_TILES_WITH_LIGHT_NO_BACK_VALUE) {
                  drawMode = DRAW_MODE_TILES_WITH_LIGHT_NO_BACK;
               }
               else if (drawModeValue == DRAW_MODE_TILES_LINKS_NODES_VALUE) {
                  drawMode = DRAW_MODE_TILES_LINKS_NODES;
               }
               else if (drawModeValue == DRAW_MODE_NONE_VALUE) {
                  drawMode = DRAW_MODE_NONE;
               }
            }
            else if (infoName == "nodeBrightness") {
               si->getValue(nodeBrightness);
            }
            else if (infoName == "opacity") {
               si->getValue(opacity);
            }
            else if (infoName == "nodeContrast") {
               si->getValue(nodeContrast);
            }
            else if (infoName == "nodeSize") {
               si->getValue(nodeSize);
            }
            else if (infoName == "linkSize") {
               si->getValue(linkSize);
            }
            else if (infoName == "forceVectorDisplayLength") {
               si->getValue(forceVectorDisplayLength);
            }
            else if (infoName == "showNormals") {
               si->getValue(showNormals);
            }
            else if (infoName == "showMorphingTotalForces") {
               si->getValue(showMorphingTotalForces);
            }
            else if (infoName == "showMorphingAngularForces") {
               si->getValue(showMorphingAngularForces);
            }
            else if (infoName == "showMorphingLinearForces") {
               si->getValue(showMorphingLinearForces);
            }
            else if (infoName == "sectionToHighlight") {
               int sectionToHighlight;
               bool sectionHighlightEveryX;
               DisplaySettingsSection* dss = brainSet->getDisplaySettingsSection();
               dss->getSectionHighlighting(sectionToHighlight, sectionHighlightEveryX);
               si->getValue(sectionToHighlight);
               dss->getSectionHighlighting(sectionToHighlight, sectionHighlightEveryX);
             }
            else if (infoName == "sectionHighlightEveryX") {
               int sectionToHighlight;
               bool sectionHighlightEveryX;
               DisplaySettingsSection* dss = brainSet->getDisplaySettingsSection();
               dss->getSectionHighlighting(sectionToHighlight, sectionHighlightEveryX);
               si->getValue(sectionHighlightEveryX);
               dss->getSectionHighlighting(sectionToHighlight, sectionHighlightEveryX);
            }
            else if (infoName == "viewingProjection") {
               int val;
               si->getValue(val);
               viewingProjection = static_cast<VIEWING_PROJECTION>(val);
            }
            else if (infoName == "showSurfaceAxes") {
               si->getValue(showSurfaceAxes);
            }
            else if (infoName == "showSurfaceAxesLetters") {
               si->getValue(showSurfaceAxesLetters);
            }
            else if (infoName == "showSurfaceAxesHashMarks") {
               si->getValue(showSurfaceAxesHashMarks);
            }
            else if (infoName == "surfaceAxesLength") {
               si->getValue(surfaceAxesLength);
            }
            else if (infoName == "surfaceAxesOffset") {
               const QString val = si->getValueAsString();
               std::vector<QString> tokens;
               StringUtilities::token(val, " ", tokens);
               if (tokens.size() >= 3) {
                  surfaceAxesOffset[0] = tokens[0].toFloat();
                  surfaceAxesOffset[1] = tokens[1].toFloat();
                  surfaceAxesOffset[2] = tokens[2].toFloat();
               }
            }
            else if (infoName == "identifyNodeColor") {
               identifyNodeColor = static_cast<IDENTIFY_NODE_COLOR>(si->getValueAsInt());
            }
            else if (infoName == "clippingPlaneApplication") {
               clippingPlaneApplication = static_cast<CLIPPING_PLANE_APPLICATION>(si->getValueAsInt());
            }
            else if (infoName == "clippingPlaneEnabled") {
               const QString val = si->getValueAsString();
               std::vector<bool> tokens;
               StringUtilities::token(val, " ", tokens);
               if (tokens.size() >= CLIPPING_PLANE_AXIS_NUMBER_OF) {
                  for (int i = 0; i < CLIPPING_PLANE_AXIS_NUMBER_OF; i++) {
                     clippingPlaneEnabled[i] = tokens[i];
                  }
               }
            }
            else if (infoName == "clippingPlaneCoordinate") {
               const QString val = si->getValueAsString();
               std::vector<float> tokens;
               StringUtilities::token(val, " ", tokens);
               if (tokens.size() >= CLIPPING_PLANE_AXIS_NUMBER_OF) {
                  for (int i = 0; i < CLIPPING_PLANE_AXIS_NUMBER_OF; i++) {
                     clippingPlaneCoordinate[i] = tokens[i];
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
DisplaySettingsSurface::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
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
   
   SceneFile::SceneClass sc("DisplaySettingsSurface");
   
   QString drawModeValue(DRAW_MODE_NODES_VALUE);
   switch (drawMode) {
      case DRAW_MODE_NODES:
         drawModeValue = DRAW_MODE_NODES_VALUE;
         break;
      case DRAW_MODE_LINKS:
         drawModeValue = DRAW_MODE_LINKS_VALUE;
         break;
      case DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL:
         drawModeValue = DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL_VALUE;
         break;
      case DRAW_MODE_LINKS_EDGES_ONLY:
         drawModeValue = DRAW_MODE_LINKS_EDGES_ONLY_VALUE;
         break;
      case DRAW_MODE_NODES_AND_LINKS:
         drawModeValue = DRAW_MODE_NODES_AND_LINKS_VALUE;
         break;
      case DRAW_MODE_TILES:
         drawModeValue = DRAW_MODE_TILES_VALUE;
         break;
      case DRAW_MODE_TILES_WITH_LIGHT:
         drawModeValue = DRAW_MODE_TILES_WITH_LIGHT_VALUE;
         break;
      case DRAW_MODE_TILES_WITH_LIGHT_NO_BACK:
         drawModeValue = DRAW_MODE_TILES_WITH_LIGHT_NO_BACK_VALUE;
         break;
      case DRAW_MODE_TILES_LINKS_NODES:
         drawModeValue = DRAW_MODE_TILES_LINKS_NODES_VALUE;
         break;
      case DRAW_MODE_NONE:
         drawModeValue = DRAW_MODE_NONE_VALUE;
         break;
   }
   sc.addSceneInfo(SceneFile::SceneInfo("surfaceDrawMode",
                                        drawModeValue));
   sc.addSceneInfo(SceneFile::SceneInfo("nodeBrightness",
                                        nodeBrightness));
   sc.addSceneInfo(SceneFile::SceneInfo("nodeContrast",
                                        nodeContrast));
   sc.addSceneInfo(SceneFile::SceneInfo("opacity",
                                        opacity));
   sc.addSceneInfo(SceneFile::SceneInfo("nodeSize",
                                        nodeSize));
   sc.addSceneInfo(SceneFile::SceneInfo("linkSize",
                                        linkSize));
   sc.addSceneInfo(SceneFile::SceneInfo("forceVectorDisplayLength",
                                        forceVectorDisplayLength));
   sc.addSceneInfo(SceneFile::SceneInfo("showNormals",
                                        showNormals));
   sc.addSceneInfo(SceneFile::SceneInfo("showMorphingTotalForces",
                                        showMorphingTotalForces));
   sc.addSceneInfo(SceneFile::SceneInfo("showMorphingAngularForces",
                                        showMorphingAngularForces));
   sc.addSceneInfo(SceneFile::SceneInfo("showMorphingLinearForces",
                                        showMorphingLinearForces));
   sc.addSceneInfo(SceneFile::SceneInfo("viewingProjection",
                                        viewingProjection));
   sc.addSceneInfo(SceneFile::SceneInfo("showSurfaceAxes",
                                        showSurfaceAxes));
   sc.addSceneInfo(SceneFile::SceneInfo("showSurfaceAxesLetters",
                                        showSurfaceAxesLetters));
   sc.addSceneInfo(SceneFile::SceneInfo("showSurfaceAxesHashMarks",
                                        showSurfaceAxesHashMarks));
   sc.addSceneInfo(SceneFile::SceneInfo("surfaceAxesLength",
                                        surfaceAxesLength));
   sc.addSceneInfo(SceneFile::SceneInfo("identifyNodeColor",
                                        identifyNodeColor));

   std::vector<float> offsets;
   offsets.push_back(surfaceAxesOffset[0]);
   offsets.push_back(surfaceAxesOffset[1]);
   offsets.push_back(surfaceAxesOffset[2]);
   sc.addSceneInfo(SceneFile::SceneInfo("surfaceAxesOffset",
                                        StringUtilities::combine(offsets, " ")));

   std::vector<bool> planeEnabled;
   std::vector<float> planeCoord;
   for (int i = 0; i < CLIPPING_PLANE_AXIS_NUMBER_OF; i++) {
      planeEnabled.push_back(clippingPlaneEnabled[i]);
      planeCoord.push_back(clippingPlaneCoordinate[i]);
   }
   sc.addSceneInfo(SceneFile::SceneInfo("clippingPlaneEnabled",
                                        StringUtilities::combine(planeEnabled, " ")));
   sc.addSceneInfo(SceneFile::SceneInfo("clippingPlaneCoordinate",
                                        StringUtilities::combine(planeCoord, " ")));
   sc.addSceneInfo(SceneFile::SceneInfo("clippingPlaneApplication",
                                        static_cast<int>(clippingPlaneApplication)));

   scene.addSceneClass(sc);

}
                       
/**
 * get a clipping plane coordinate.
 */
float
DisplaySettingsSurface::getClippingPlaneCoordinate(const CLIPPING_PLANE_AXIS planeAxis) const
{
   return this->clippingPlaneCoordinate[planeAxis];
}

/**
 * set a clipping plane coordinate.
 */
void
DisplaySettingsSurface::setClippingPlaneCoordinate(const CLIPPING_PLANE_AXIS planeAxis,
                              const float coordinateValue)
{
   this->clippingPlaneCoordinate[planeAxis] = coordinateValue;
}

/**
 * get clipping plane enabled.
 */
bool
DisplaySettingsSurface::getClippingPlaneEnabled(const CLIPPING_PLANE_AXIS planeAxis) const
{
   return this->clippingPlaneEnabled[planeAxis];
}

/**
 * set a clipping plane coordinate.
 */
void
DisplaySettingsSurface::setClippingPlaneEnabled(const CLIPPING_PLANE_AXIS planeAxis,
                           const bool enabled)
{
   this->clippingPlaneEnabled[planeAxis] = enabled;
}
