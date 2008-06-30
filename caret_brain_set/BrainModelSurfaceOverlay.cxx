
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

#include <algorithm>

#include "ArealEstimationFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "CocomacConnectivityFile.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsSection.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsTopography.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "RgbPaintFile.h"
#include "SectionFile.h"
#include "SurfaceShapeFile.h"
#include "TopographyFile.h"

//
// For scenes
//
static const QString overlayNoneName("none");
static const QString overlayArealEstimationName("areal-estimation");
static const QString overlayCocomacName("cocomac");
static const QString overlayMetricName("metric");
static const QString overlayPaintName("paint");
static const QString overlayProbabilisticAtlasName("probabilistic-atlas");
static const QString overlayRgbPaintName("rgb-paint");
static const QString overlaySectionsName("sections");
static const QString overlayShowCrossoversName("show-crossovers");
static const QString overlayShowEdgesName("show-edges");
static const QString overlaySurfaceShapeName("surface-shape");
static const QString overlayTopographyName("topography");
static const QString overlayGeographyBlendingName("geography-blending");

/**
 * constructor.
 */
BrainModelSurfaceOverlay::BrainModelSurfaceOverlay(BrainSet* brainSetIn,
                                                   const int overlayNumberIn)
{
   brainSet = brainSetIn;
   overlayNumber = overlayNumberIn;
   const int numberOfOverlays = brainSet->getNumberOfSurfaceOverlays();
   
   if (overlayNumber == 0) {
      name = "Underlay";
   }
   else if (overlayNumber == (numberOfOverlays - 1)) {
      name = "Primary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 2)) {
      name = "Secondary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 3)) {
      name = "Tertiary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 4)) {
      name = "Quaternary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 5)) {
      name = "Quinary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 6)) {
      name = "Senary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 7)) {
      name = "Septenary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 8)) {
      name = "Octonary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 9)) {
      name = "Nonary Overlay";
   }
   else if (overlayNumber == (numberOfOverlays - 10)) {
      name = "Denary Overlay";
   }
   else {
      name = QString::number(overlayNumber + 1) + " Overlay";
   }

   reset();
}

/**
 * destructor.
 */
BrainModelSurfaceOverlay::~BrainModelSurfaceOverlay()
{
}

/**
 * reset the overlay.
 */
void 
BrainModelSurfaceOverlay::reset()
{
   opacity = 1.0;
   lightingEnabled = true;
   update();
}

/**
 * update the overlay due to brain model being loaded/unloaded or files changing.
 */
void 
BrainModelSurfaceOverlay::update() const
{
   //
   // Get default selection
   //
   OVERLAY_SELECTIONS defaultSelection = OVERLAY_NONE;
   if (overlay.empty() == false) {
      defaultSelection = overlay[0];
   }
   
   //
   // Size up for number of brain models
   //
   const int numBrainModels = brainSet->getNumberOfBrainModels();
   overlay.resize(numBrainModels, 
                  defaultSelection);

   //
   // get valid types and names
   //
   std::vector<OVERLAY_SELECTIONS> dataTypes;
   std::vector<QString> dataTypeNames;
   getDataTypesAndNames(dataTypes, dataTypeNames);

   //
   // reset any overlays that are invalid
   //
   for (int i = 0; i < numBrainModels; i++) {
      if (std::find(dataTypes.begin(),
                    dataTypes.end(),
                    overlay[i]) == dataTypes.end()) {
         overlay[i] = OVERLAY_NONE;
      }
   }
}
      
/**
 * get the overlay.
 */
BrainModelSurfaceOverlay::OVERLAY_SELECTIONS 
BrainModelSurfaceOverlay::getOverlay(const int modelNumberIn,
                                     const bool doUpdateFlag) const 
{ 
   int modelNumber = modelNumberIn;
   if (doUpdateFlag) {
      update();
   }
   if (modelNumber < 0) {
      modelNumber = 0;
   }
   else if (modelNumber >= static_cast<int>(overlay.size())) {
      modelNumber = 0;
   }
   
   if (overlay.empty()) {
      return OVERLAY_NONE;
   }
   
   return overlay[modelNumber]; 
}

/**
 * set the overlay.
 */
void 
BrainModelSurfaceOverlay::setOverlay(const int modelNumberIn,
                                    const OVERLAY_SELECTIONS os) 
{ 
   int modelNumber = modelNumberIn;
   update();
   if (modelNumber < 0) {
      std::fill(overlay.begin(), overlay.end(), os);
   }
   else if ((modelNumber >= 0) && (modelNumber < static_cast<int>(overlay.size()))) {
      overlay[modelNumber] = os; 
   }
}
      
      
/**
 * get data types and names for selection.
 */
void 
BrainModelSurfaceOverlay::getDataTypesAndNames(std::vector<OVERLAY_SELECTIONS>& typesOut,
                                               std::vector<QString>& namesOut) const
{
   typesOut.clear();
   namesOut.clear();
   const int numNodes = brainSet->getNumberOfNodes();
   
   bool haveCrossovers = false;
   bool haveEdges = false;
   for (int i = 0; i < numNodes; i++) {
      const BrainSetNodeAttribute* bna = brainSet->getNodeAttributes(i);
      if (bna != NULL) {
         if (bna->getCrossover() != BrainSetNodeAttribute::CROSSOVER_NO) {
            haveCrossovers = true;
         }
         if (bna->getClassification() == BrainSetNodeAttribute::CLASSIFICATION_TYPE_EDGE) {
            haveEdges = true;
         }
      }
   }


   typesOut.push_back(OVERLAY_NONE);    
      namesOut.push_back("None");

   if (brainSet->getArealEstimationFile()->empty() == false) {
      typesOut.push_back(OVERLAY_AREAL_ESTIMATION);    
         namesOut.push_back("Areal Estimation");
   }

   if (brainSet->getCocomacFile()->empty() == false) {
      typesOut.push_back(OVERLAY_COCOMAC);    
         namesOut.push_back("CoCoMac");
   }   

   if (haveCrossovers) {
      typesOut.push_back(OVERLAY_SHOW_CROSSOVERS);    
         namesOut.push_back("Crossovers");
   }

   if (haveEdges) {
      typesOut.push_back(OVERLAY_SHOW_EDGES);    
         namesOut.push_back("Edges");
   }

   if (brainSet->getPaintFile()->empty() == false) {
      if (brainSet->getPaintFile()->getGeographyColumnNumber() >= 0) {
         typesOut.push_back(OVERLAY_GEOGRAPHY_BLENDING);    
            namesOut.push_back("Geography Blending");
      }
   }

   if (brainSet->getMetricFile()->empty() == false) {
      typesOut.push_back(OVERLAY_METRIC);    
         namesOut.push_back("Metric");
   }

   if (brainSet->getPaintFile()->empty() == false) {
      typesOut.push_back(OVERLAY_PAINT);    
         namesOut.push_back("Paint");
   }

   if (brainSet->getProbabilisticAtlasSurfaceFile()->empty() == false) {
      typesOut.push_back(OVERLAY_PROBABILISTIC_ATLAS);    
         namesOut.push_back("Probabilistic Atlas");
   }

   if (brainSet->getRgbPaintFile()->empty() == false) {
      typesOut.push_back(OVERLAY_RGB_PAINT);    
         namesOut.push_back("RGB Paint");
   }

   if (brainSet->getSectionFile()->empty() == false) {
      typesOut.push_back(OVERLAY_SECTIONS);    
         namesOut.push_back("Sections");
   }

   if (brainSet->getSurfaceShapeFile()->empty() == false) {
      typesOut.push_back(OVERLAY_SURFACE_SHAPE);    
         namesOut.push_back("Shape");
   }

   if (brainSet->getTopographyFile()->empty() == false) {
      typesOut.push_back(OVERLAY_TOPOGRAPHY);    
         namesOut.push_back("Topography");
   }
}

/**
 * get the display column valid.
 */
bool 
BrainModelSurfaceOverlay::getDisplayColumnValid(const int modelNumber) const
{
   return (getDisplayColumnNames(modelNumber).isEmpty() == false);
}

/**
 * get the display column names.
 */
QStringList 
BrainModelSurfaceOverlay::getDisplayColumnNames(const int modelNumberIn) const
{
   int modelNumber = modelNumberIn;
   if (modelNumber < 0) {
      modelNumber = 0;
   }

   GiftiNodeDataFile* gnf = NULL;
   NodeAttributeFile* naf = NULL;
   
   switch (overlay[modelNumber]) {
      case OVERLAY_NONE:
         break;
      case OVERLAY_AREAL_ESTIMATION:
         naf = brainSet->getArealEstimationFile();
         break;
      case OVERLAY_COCOMAC:
         break;
      case OVERLAY_METRIC:
         gnf = brainSet->getMetricFile();
         break;
      case OVERLAY_PAINT:
         gnf = brainSet->getPaintFile();
         break;
      case OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case OVERLAY_RGB_PAINT:
         naf = brainSet->getRgbPaintFile();
         break;
      case OVERLAY_SECTIONS:
         break;
      case OVERLAY_SHOW_CROSSOVERS:
         break;
      case OVERLAY_SHOW_EDGES:
         break;
      case OVERLAY_SURFACE_SHAPE:
         gnf = brainSet->getSurfaceShapeFile();
         break;
      case OVERLAY_TOPOGRAPHY:
         naf = brainSet->getTopographyFile();
         break;
      case OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   QStringList names;
   
   if (gnf != NULL) {
      for (int i = 0; i < gnf->getNumberOfColumns(); i++) {
         names << gnf->getColumnName(i);
      }
   }
   if (naf != NULL) {
      for (int i = 0; i < naf->getNumberOfColumns(); i++) {
         names << naf->getColumnName(i);
      }
   }
   
   return names;
}

/**
 * get the threshold column names.
 */
QStringList 
BrainModelSurfaceOverlay::getThresholdColumnNames(const int modelNumberIn) const
{
   int modelNumber = modelNumberIn;
   if (modelNumber < 0) {
      modelNumber = 0;
   }

   bool thresholdValid = false;
   
   switch (overlay[modelNumber]) {
      case OVERLAY_NONE:
         break;
      case OVERLAY_AREAL_ESTIMATION:
         break;
      case OVERLAY_COCOMAC:
         break;
      case OVERLAY_METRIC:
         thresholdValid = true;
         break;
      case OVERLAY_PAINT:
         break;
      case OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case OVERLAY_RGB_PAINT:
         break;
      case OVERLAY_SECTIONS:
         break;
      case OVERLAY_SHOW_CROSSOVERS:
         break;
      case OVERLAY_SHOW_EDGES:
         break;
      case OVERLAY_SURFACE_SHAPE:
         break;
      case OVERLAY_TOPOGRAPHY:
         break;
      case OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   QStringList names;
   if (thresholdValid) {
      names = getDisplayColumnNames(modelNumber);
   }
   
   return names;
}

/**
 * get the threshold column valid.
 */
bool 
BrainModelSurfaceOverlay::getThresholdColumnValid(const int modelNumber) const
{
   return (getThresholdColumnNames(modelNumber).isEmpty() == false);
}

/**
 * get the selected display column.
 */
int 
BrainModelSurfaceOverlay::getDisplayColumnSelected(const int modelNumberIn) const
{
   int modelNumber = modelNumberIn;
   if (modelNumber < 0) {
      modelNumber = 0;
   }

   DisplaySettingsNodeAttributeFile* dsna = NULL;
   switch (overlay[modelNumber]) {
      case OVERLAY_NONE:
         break;
      case OVERLAY_AREAL_ESTIMATION:
         dsna = brainSet->getDisplaySettingsArealEstimation();
         break;
      case OVERLAY_COCOMAC:
         break;
      case OVERLAY_METRIC:
         dsna = brainSet->getDisplaySettingsMetric();
         break;
      case OVERLAY_PAINT:
         dsna = brainSet->getDisplaySettingsPaint();
         break;
      case OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case OVERLAY_RGB_PAINT:
         dsna = brainSet->getDisplaySettingsRgbPaint();
         break;
      case OVERLAY_SECTIONS:
         dsna = brainSet->getDisplaySettingsSection();
         break;
      case OVERLAY_SHOW_CROSSOVERS:
         break;
      case OVERLAY_SHOW_EDGES:
         break;
      case OVERLAY_SURFACE_SHAPE:
         dsna = brainSet->getDisplaySettingsSurfaceShape();
         break;
      case OVERLAY_TOPOGRAPHY:
         dsna = brainSet->getDisplaySettingsTopography();
         break;
      case OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   int columnNumber = -1;
   
   if (dsna != NULL) {
       columnNumber = dsna->getSelectedDisplayColumn(modelNumber, overlayNumber);      
   }
   
   return columnNumber;
}

/**
 * set the selected display column.
 */
void 
BrainModelSurfaceOverlay::setDisplayColumnSelected(const int modelNumberIn,
                                                   const int columnNumber)
{
   int modelNumber = modelNumberIn;
   if (modelNumber < 0) {
      modelNumber = 0;
   }

   DisplaySettingsNodeAttributeFile* dsna = NULL;
   switch (overlay[modelNumber]) {
      case OVERLAY_NONE:
         break;
      case OVERLAY_AREAL_ESTIMATION:
         dsna = brainSet->getDisplaySettingsArealEstimation();
         break;
      case OVERLAY_COCOMAC:
         break;
      case OVERLAY_METRIC:
         dsna = brainSet->getDisplaySettingsMetric();
         break;
      case OVERLAY_PAINT:
         dsna = brainSet->getDisplaySettingsPaint();
         break;
      case OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case OVERLAY_RGB_PAINT:
         dsna = brainSet->getDisplaySettingsRgbPaint();
         break;
      case OVERLAY_SECTIONS:
         dsna = brainSet->getDisplaySettingsSection();
         break;
      case OVERLAY_SHOW_CROSSOVERS:
         break;
      case OVERLAY_SHOW_EDGES:
         break;
      case OVERLAY_SURFACE_SHAPE:
         dsna = brainSet->getDisplaySettingsSurfaceShape();
         break;
      case OVERLAY_TOPOGRAPHY:
         dsna = brainSet->getDisplaySettingsTopography();
         break;
      case OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   if (dsna != NULL) {
       dsna->setSelectedDisplayColumn(modelNumberIn, overlayNumber, columnNumber);      
   }
}      
      
/**
 * get the selected threshold column.
 */
int 
BrainModelSurfaceOverlay::getThresholdColumnSelected(const int modelNumberIn) const
{
   int modelNumber = modelNumberIn;
   if (modelNumber < 0) {
      modelNumber = 0;
   }

   DisplaySettingsNodeAttributeFile* dsna = NULL;
   switch (overlay[modelNumber]) {
      case OVERLAY_NONE:
         break;
      case OVERLAY_AREAL_ESTIMATION:
         dsna = brainSet->getDisplaySettingsArealEstimation();
         break;
      case OVERLAY_COCOMAC:
         break;
      case OVERLAY_METRIC:
         dsna = brainSet->getDisplaySettingsMetric();
         break;
      case OVERLAY_PAINT:
         dsna = brainSet->getDisplaySettingsPaint();
         break;
      case OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case OVERLAY_RGB_PAINT:
         dsna = brainSet->getDisplaySettingsRgbPaint();
         break;
      case OVERLAY_SECTIONS:
         dsna = brainSet->getDisplaySettingsSection();
         break;
      case OVERLAY_SHOW_CROSSOVERS:
         break;
      case OVERLAY_SHOW_EDGES:
         break;
      case OVERLAY_SURFACE_SHAPE:
         dsna = brainSet->getDisplaySettingsSurfaceShape();
         break;
      case OVERLAY_TOPOGRAPHY:
         dsna = brainSet->getDisplaySettingsTopography();
         break;
      case OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   int columnNumber = -1;
   
   if (dsna != NULL) {
       columnNumber = dsna->getSelectedThresholdColumn(modelNumber, overlayNumber);      
   }
   
   return columnNumber;
}

/**
 * set the selected threshold column.
 */
void 
BrainModelSurfaceOverlay::setThresholdColumnSelected(const int modelNumberIn,
                                                     const int columnNumber)
{
   int modelNumber = modelNumberIn;
   if (modelNumber < 0) {
      modelNumber = 0;
   }

   DisplaySettingsNodeAttributeFile* dsna = NULL;
   switch (overlay[modelNumber]) {
      case OVERLAY_NONE:
         break;
      case OVERLAY_AREAL_ESTIMATION:
         dsna = brainSet->getDisplaySettingsArealEstimation();
         break;
      case OVERLAY_COCOMAC:
         break;
      case OVERLAY_METRIC:
         dsna = brainSet->getDisplaySettingsMetric();
         break;
      case OVERLAY_PAINT:
         dsna = brainSet->getDisplaySettingsPaint();
         break;
      case OVERLAY_PROBABILISTIC_ATLAS:
         break;
      case OVERLAY_RGB_PAINT:
         dsna = brainSet->getDisplaySettingsRgbPaint();
         break;
      case OVERLAY_SECTIONS:
         dsna = brainSet->getDisplaySettingsSection();
         break;
      case OVERLAY_SHOW_CROSSOVERS:
         break;
      case OVERLAY_SHOW_EDGES:
         break;
      case OVERLAY_SURFACE_SHAPE:
         dsna = brainSet->getDisplaySettingsSurfaceShape();
         break;
      case OVERLAY_TOPOGRAPHY:
         dsna = brainSet->getDisplaySettingsTopography();
         break;
      case OVERLAY_GEOGRAPHY_BLENDING:
         break;
   }
   
   if (dsna != NULL) {
       dsna->setSelectedThresholdColumn(modelNumberIn, overlayNumber, columnNumber);      
   }
}      

/**
 * apply a scene (set display settings).
 */
void 
BrainModelSurfaceOverlay::showScene(const SceneFile::Scene& scene,
                                    QString& errorMessage)
{   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName().startsWith("BrainModelSurfaceOverlay")) {
         const QStringList sl = sc->getName().split(':');
         if (sl.count() == 2) {
            const int num = sl.at(1).toInt();
            if (num != overlayNumber) {
               continue;
            }
         }
         else {
            continue;
         }
         
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            const QString value = si->getValueAsString();
            
            if (infoName == "opacity") {
               setOpacity(si->getValueAsFloat());
            }
            else if (infoName == "lightingEnabled") {
               setLightingEnabled(si->getValueAsBool());
            }
             
            //
            // Do overlay
            //
            if (infoName == "overlay") {
               OVERLAY_SELECTIONS  overlay  = OVERLAY_NONE;
               const QString surfaceName = si->getModelName();
               
               if (value == overlayArealEstimationName) {
                  overlay  = OVERLAY_AREAL_ESTIMATION;
                  if (brainSet->getArealEstimationFile()->getNumberOfColumns() <= 0) {
                     errorMessage.append("Areal Estimation File is overlay/underlay but no Areal Estimation File is loaded.\n");
                  }
               }
               else if (value == overlayCocomacName) {
                  overlay  = OVERLAY_COCOMAC;
                  const CocomacConnectivityFile* coco = brainSet->getCocomacFile();
                  if (coco->empty()) {
                     errorMessage.append("CoCoMac File is overlay/underlay but no CoCoMac File is loaded.\n");
                  }
               }
               else if (value == overlayMetricName) {
                  overlay  = OVERLAY_METRIC;
                  if (brainSet->getMetricFile()->empty()) {
                     errorMessage.append("Metric File is overlay/underlay but no Metric File is loaded.\n");
                  }
               }
               else if (value == overlayPaintName) {
                  overlay  = OVERLAY_PAINT;
                  if (brainSet->getPaintFile()->empty()) {
                     errorMessage.append("Paint File is overlay/underlay but no Paint File is loaded.\n");
                  }
               }
               else if (value == overlayProbabilisticAtlasName) {
                  overlay  = OVERLAY_PROBABILISTIC_ATLAS;
                  if (brainSet->getProbabilisticAtlasSurfaceFile()->empty()) {
                     errorMessage.append("Prob Atlas File is overlay/underlay but no Prob Atlas File is loaded.\n");
                  }
               }
               else if (value == overlayRgbPaintName) {
                  overlay  = OVERLAY_RGB_PAINT;
                  if (brainSet->getRgbPaintFile()->empty()) {
                     errorMessage.append("RGB Paint File is overlay/underlay but no RGB Paint File is loaded.\n");
                  }
               }
               else if (value == overlaySectionsName) {
                  overlay  = OVERLAY_SECTIONS;
               }
               else if (value == overlayShowCrossoversName) {
                  overlay  = OVERLAY_SHOW_CROSSOVERS;
               }
               else if (value == overlayShowEdgesName) {
                  overlay  = OVERLAY_SHOW_EDGES;
               }
               else if (value == overlaySurfaceShapeName) {
                  overlay  = OVERLAY_SURFACE_SHAPE;
                  if (brainSet->getSurfaceShapeFile()->empty()) {
                     errorMessage.append("Surface Shape File is overlay/underlay but no Surface Shape File is loaded.\n");
                  }
               }
               else if (value == overlayTopographyName) {
                  overlay  = OVERLAY_TOPOGRAPHY;
                  if (brainSet->getTopographyFile()->empty()) {
                     errorMessage.append("Topography File is overlay/underlay but no Topography File is loaded.\n");
                  }
               }
               else if (value == overlayGeographyBlendingName) {
                  overlay = OVERLAY_GEOGRAPHY_BLENDING;
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
                  const BrainModelSurface* bms = brainSet->getBrainModelSurfaceWithFileName(surfaceName);
                  if (bms != NULL) {
                     startSurface = brainSet->getBrainModelIndex(bms);
                     if (startSurface >= 0) {
                        endSurface = startSurface + 1;
                     }
                  }
               }
               
               // 
               // Set the overlay
               //
               for (int k = startSurface; k < endSurface; k++) {
                  setOverlay(k, overlay);
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
BrainModelSurfaceOverlay::saveScene(SceneFile::Scene& scene, 
                                    const bool onlyIfSelected)
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
   SceneFile::SceneClass sc("BrainModelSurfaceOverlay:" 
                            + QString::number(overlayNumber));
   sc.addSceneInfo(SceneFile::SceneInfo("overlayNumber", getOverlayNumber()));
   sc.addSceneInfo(SceneFile::SceneInfo("opacity", getOpacity()));
   sc.addSceneInfo(SceneFile::SceneInfo("lightingEnabled", getLightingEnabled()));

   bool didDefaultFlag = false;
   
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
         
         QString ouValue;
         switch(getOverlay(surfaceIndex)) {
            case OVERLAY_NONE:
               ouValue = overlayNoneName;
               break;
            case OVERLAY_AREAL_ESTIMATION:
               ouValue = overlayArealEstimationName;
               break;
            case OVERLAY_COCOMAC:
               ouValue = overlayCocomacName;
               break;
            case OVERLAY_METRIC:
               ouValue = overlayMetricName;
               break;
            case OVERLAY_PAINT:
               ouValue = overlayPaintName;
               break;
            case OVERLAY_PROBABILISTIC_ATLAS:
               ouValue = overlayProbabilisticAtlasName;
               break;
            case OVERLAY_RGB_PAINT:
               ouValue = overlayRgbPaintName;
               break;
            case OVERLAY_SECTIONS:
               ouValue = overlaySectionsName;
               break;
            case OVERLAY_SHOW_CROSSOVERS:
               ouValue = overlayShowCrossoversName;
               break;
            case OVERLAY_SHOW_EDGES:
               ouValue = overlayShowEdgesName;
               break;
            case OVERLAY_SURFACE_SHAPE:
               ouValue = overlaySurfaceShapeName;
               break;
            case OVERLAY_TOPOGRAPHY:
               ouValue = overlayTopographyName;
               break;
            case OVERLAY_GEOGRAPHY_BLENDING:
               ouValue = overlayGeographyBlendingName;
               break;
         }
         
         //
         // Do default first
         //
         if (didDefaultFlag == false) {
            SceneFile::SceneInfo si("overlay", 
                        SceneFile::SceneInfo::getDefaultSurfacesName(), ouValue);
            sc.addSceneInfo(si);
            didDefaultFlag = true;
         }
         
         //
         // Create the scene info for this overlay/underlay
         //
         SceneFile::SceneInfo si("overlay", surfaceName, ouValue);
         sc.addSceneInfo(si);
      }  // if (bms != NULL)   
   }  // for n
   
   scene.addSceneClass(sc);
}
               
/**
 * copy the overlay selections from specified surface to all other surfaces.
 */
void 
BrainModelSurfaceOverlay::copyOverlaysFromSurface(const int surfaceModelIndex)
{
   if ((surfaceModelIndex >= 0) && 
       (surfaceModelIndex < static_cast<int>(overlay.size()))) {
      //
      // Set the overlay
      //
      const OVERLAY_SELECTIONS selectedOverlay = overlay[surfaceModelIndex];
      std::fill(overlay.begin(), overlay.end(), selectedOverlay);
      
      copyOverlaysFromSurfaceHelper(brainSet->getDisplaySettingsArealEstimation(),
                                    surfaceModelIndex);
      copyOverlaysFromSurfaceHelper(brainSet->getDisplaySettingsMetric(),
                                    surfaceModelIndex);
      copyOverlaysFromSurfaceHelper(brainSet->getDisplaySettingsPaint(),
                                    surfaceModelIndex);
      copyOverlaysFromSurfaceHelper(brainSet->getDisplaySettingsRgbPaint(),
                                    surfaceModelIndex);
      copyOverlaysFromSurfaceHelper(brainSet->getDisplaySettingsSection(),
                                    surfaceModelIndex);
      copyOverlaysFromSurfaceHelper(brainSet->getDisplaySettingsSurfaceShape(),
                                    surfaceModelIndex);
      copyOverlaysFromSurfaceHelper(brainSet->getDisplaySettingsTopography(),
                                    surfaceModelIndex);
   }
}
      
/**
 * copy the overlay selections from specified surface to all other surfaces.
 */
void 
BrainModelSurfaceOverlay::copyOverlaysFromSurfaceHelper(DisplaySettingsNodeAttributeFile* dsna,
                                                        const int surfaceModelIndex)
{
   for (int m = 0; m < overlayNumber; m++) {
      dsna->setSelectedDisplayColumn(-1, 
                                     overlayNumber,
                                     dsna->getSelectedDisplayColumn(surfaceModelIndex,
                                                                    overlayNumber));
   }
}

/**
 * get the name of the data type.
 */
QString 
BrainModelSurfaceOverlay::getDataTypeName(const int modelNumber) const
{
   QString s;
   
   std::vector<OVERLAY_SELECTIONS> types;
   std::vector<QString> names;
   getDataTypesAndNames(types, names);
   
   const int num = static_cast<int>(types.size());
   for (int i = 0; i < num; i++) {
      if (getOverlay(modelNumber) == types[i]) {
         s = names[i];
         break;
      }
   }
   
   return s;
}

      
