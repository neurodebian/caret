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

#include "ArealEstimationFile.h"
#include "BrainModelContours.h"
#include "BrainModelIdentification.h"
#include "BrainModelOpenGL.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "ContourCellFile.h"
#include "ContourFile.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsCoCoMac.h"
#include "DisplaySettingsGeodesicDistance.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsRgbPaint.h"
#include "DisplaySettingsSurfaceShape.h"
#include "DisplaySettingsTopography.h"
#include "FileUtilities.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GeodesicDistanceFile.h"
#include "LatLonFile.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "PaletteFile.h"
#include "PreferencesFile.h"
#include "ProbabilisticAtlasFile.h"
#include "RgbPaintFile.h"
#include "SectionFile.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "StudyMetaDataFile.h"
#include "SurfaceShapeFile.h"
#include "TopographyFile.h"
#include "TransformationMatrixFile.h"
#include "VolumeFile.h"
#include "VocabularyFile.h"
#include "VtkModelFile.h"

/**
 * constructor.
 */
BrainModelIdentification::BrainModelIdentification(BrainSet* brainSetIn,
                                                   QObject* parent)
   : QObject(parent)
{
   brainSetParent = brainSetIn;
   displayIDSymbol = true;

   significantDigits = 2;   
   const PreferencesFile* pf = BrainSet::getPreferencesFile();
   if (pf != NULL) {
      significantDigits = pf->getSignificantDigitsDisplay();
   }
}

/**
 * destructor.
 */
BrainModelIdentification::~BrainModelIdentification()
{ 
}

/**
 * get the identification text.
 */
QString 
BrainModelIdentification::getIdentificationTextForCell(BrainModelOpenGL* openGL,
                                                       const bool enableHtml,
                                                       const bool enableVocabularyLinksIn)
{
   htmlFlag = enableHtml;
   
   //
   // Save current id filter
   //
   IdFilter savedFilter = idFilter;
   
   //
   // Turn on all but node
   //
   idFilter.allOff(false);
   setDisplayCellInformation(true);
   
   //
   // Do ID
   //
   QString idString = getIdentificationText(openGL,
                                            enableHtml,
                                            enableVocabularyLinksIn);
                         
   //
   // Restore ID filter
   //
   idFilter = savedFilter;
   
   return idString;
}                                    

/**
 * get the identification text.
 */
QString 
BrainModelIdentification::getIdentificationTextForFocus(BrainModelOpenGL* openGL,
                                                       const bool enableHtml,
                                                       const bool enableVocabularyLinksIn)
{
   htmlFlag = enableHtml;

   //
   // Save current id filter
   //
   IdFilter savedFilter = idFilter;
   
   //
   // Turn on all but node
   //
   idFilter.allOff(false);
   setDisplayFociInformation(true);
   
   //
   // Do ID
   //
   QString idString = getIdentificationText(openGL,
                                            enableHtml,
                                            enableVocabularyLinksIn);
                         
   //
   // Restore ID filter
   //
   idFilter = savedFilter;
   
   return idString;
}                                    

/**
 * get the identification text.
 */
QString 
BrainModelIdentification::getIdentificationTextForBorder(BrainModelOpenGL* openGL,
                                                       const bool enableHtml,
                                                       const bool enableVocabularyLinksIn)
{
   htmlFlag = enableHtml;

   //
   // Save current id filter
   //
   IdFilter savedFilter = idFilter;
   
   //
   // Turn on all but node
   //
   idFilter.allOff(false);
   setDisplayBorderInformation(true);
   
   //
   // Do ID
   //
   QString idString = getIdentificationText(openGL,
                                            enableHtml,
                                            enableVocabularyLinksIn);
                         
   //
   // Restore ID filter
   //
   idFilter = savedFilter;
   
   return idString;
}                                    

/**
 * get the identification text.
 */
QString 
BrainModelIdentification::getIdentificationTextForNode(BrainModelOpenGL* openGL,
                                                       const bool enableHtml,
                                                       const bool enableVocabularyLinksIn)
{
   htmlFlag = enableHtml;

   //
   // Save current id filter
   //
   IdFilter savedFilter = idFilter;
   
   //
   // Turn on all but node
   //
   idFilter.allOn();
   setDisplayBorderInformation(false);
   setDisplayCellInformation(false);
   setDisplayFociInformation(false);
   setDisplayVoxelInformation(false);
   
   //
   // Do ID
   //
   QString idString = getIdentificationText(openGL,
                                            enableHtml,
                                            enableVocabularyLinksIn);
                         
   //
   // Restore ID filter
   //
   idFilter = savedFilter;
   
   return idString;
}                                    

/**
 * get the identification text for node by its number.
 */
QString 
BrainModelIdentification::getIdentificationTextForNode(BrainSet* brainSet,
                                                       const int nodeNumber,
                                                       const bool enableHtml,
                                                       const bool enableVocabularyLinksIn)
{
   htmlFlag = enableHtml;
   enableVocabularyLinks = (enableVocabularyLinksIn && enableHtml);
   
   setupHtmlOrTextTags(enableHtml);
      
   QString idString;
   
   idString += getIdentificationTextForNode(nodeNumber,
                                            -1,   // window number
                                            brainSet,
                                            brainSet->getActiveFiducialSurface());
                                            
   return idString;
}

/**
 * setup the tags for html or text.
 */
void 
BrainModelIdentification::setupHtmlOrTextTags(const bool doHTML)
{
   tagBoldStart = "";
   tagBoldEnd   = "";
   tagIndentation = "    ";
   tagNewLine   = "\n";
   if (doHTML) {
      tagBoldStart = "<B>";
      tagBoldEnd   = "</B>";
      tagIndentation = "&nbsp;&nbsp;&nbsp;&nbsp;";
      tagNewLine   = "<BR>";
   }
}      

/**
 * get the identification text.
 */
QString 
BrainModelIdentification::getIdentificationText(BrainModelOpenGL* openGLIn,
                                                const bool enableHtml,
                                                const bool enableVocabularyLinksIn)
{
   openGL = openGLIn;
   htmlFlag = enableHtml;
   enableVocabularyLinks = (enableVocabularyLinksIn && enableHtml);
   
   setupHtmlOrTextTags(enableHtml);
   
   QString idString;
   
   if (getDisplayFociInformation()) {
      idString += getIdentificationTextForFoci();
      idString += getIdentificationTextForTransformFoci();
   }

   if (getDisplayCellInformation()) {
      idString += getIdentificationTextForCellProjection();
      idString += getIdentificationTextForTransformCell();
   }   
   
   if (getDisplayNodeInformation()) {
      idString += getIdentificationTextForNode();
   }
   
   if (getDisplayBorderInformation()) {
      idString += getIdentificationTextForSurfaceBorder();
   }
   
   idString += getIdentificationTextForVtkModel();
   
   if (getDisplayVoxelInformation()) {
      idString += getIdentificationTextForVoxel();
      idString += getIdentificationTextForVolumeBorder();
      idString += getIdentificationTextForVolumeCell();
      idString += getIdentificationTextForVolumeFoci();
      idString += getIdentificationTextForVoxelCloudFunctional();
   }

   idString += getIdentificationTextForPalette(true);   
   idString += getIdentificationTextForPalette(false);   
   
   if (getDisplayContourInformation()) {
      idString += getIdentificationTextForContour();
      idString += getIdentificationTextForContourCell();
   }
   
   idString += getIdentificationTextForTransformContourCell();
   
/*
   if (idString.isEmpty() == false) {
      if (enableHtml) {
         idString.insert(0, "<hr><br>");
      }
      else {
         idString.insert(0, QString(60, '-') + "\n");
      }
   }
*/

   return idString;
}

/**
 * get identification text for node.
 */
QString 
BrainModelIdentification::getIdentificationTextForNode()
{
   BrainModelOpenGLSelectedItem nodeID = openGL->getSelectedNode();
   return getIdentificationTextForNode(nodeID.getItemIndex1(),
                                       nodeID.getViewingWindowNumber(),
                                       nodeID.getBrainSet(),
                                       nodeID.getBrainModelSurface());
}
 
/**
 * Get the identification text for a node.
 */
QString
BrainModelIdentification::getIdentificationTextForNode(const int nodeNumber,
                                                       const int windowNumber,
                                                       BrainSet* brainSet,
                                                       BrainModelSurface* bms)
{
   //
   // Keep track of last identified node per window
   //
   static bool lastValid[BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
   static float lastXYZ[BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS][3];
   static bool firstTime = true;
   if (firstTime) {
      firstTime = false;
      for (int i = 0; i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
         lastValid[i] = false;
      }
   }
   
   
   //
   // Verify that a node was selected
   //
   if (nodeNumber < 0) {
      return "";
   }   
   
   //
   // Verify that at least one node display filter is enabled
   //
   if (idFilter.anyNodeDataOn() == false) {
      return "";
   }
   
   QString idString;
   
   //
   // Add node number
   //
   idString += (tagBoldStart
                + "Node "
                + QString::number(nodeNumber)
                + tagBoldEnd
                + tagNewLine);
                
   
   //
   // Set if BrainSet and Surface are valid
   //
   if ((brainSet != NULL) &&
       (bms != NULL)) {
      //
      // Index of brain model in BrainSet
      //
      const int brainModelIndex = bms->getBrainModelIndex();
      
      //
      // If there is not a surface in the window then it must be a volume
      // and the surface passed in is the active fiducial surface
      //
      bool mustBeVolumeIdFlag = false;
      if (windowNumber >= 0) {
         BrainModel* bm = openGL->getBrainModelInWindowNumberForIdentification(windowNumber);
         if (bm != NULL) {
            //
            // NOT a surface
            //
            if (dynamic_cast<BrainModelSurface*>(bm) == NULL) {
               mustBeVolumeIdFlag = true;
            }
         }
      }
      else {
         mustBeVolumeIdFlag = true;
      }
      
      //
      // IDing node corresponding to volume voxel?
      //
      if (mustBeVolumeIdFlag) {
         //
         // Coordinate of node
         //
         const CoordinateFile* cf = bms->getCoordinateFile();
         float xyz[3];
         cf->getCoordinate(nodeNumber, xyz);
         
         //
         // XYZ postion
         //
         idString += (tagIndentation
                      + bms->getSurfaceTypeName()
                      + " XYZ: "
                      + QString::number(xyz[0], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[1], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[2], 'f', significantDigits)
                      + tagNewLine);
      }
      else {
         //
         // Identifying node in a window, show xyz for all surfaces in windows
         // Get displayed brain models in each window
         //
         for (int j = 0; j < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; j++) {
            BrainModel* bm = openGL->getBrainModelInWindowNumberForIdentification(j);
            if (bm != NULL) {
               BrainModelSurface* idSurface = dynamic_cast<BrainModelSurface*>(bm);
               if (idSurface != NULL) {
                  
                  //
                  // Coordinate of node
                  //
                  const CoordinateFile* cf = idSurface->getCoordinateFile();
                  float xyz[3];
                  cf->getCoordinate(nodeNumber, xyz);
                  
                  //
                  // Distance from last identified node
                  //
                  QString distanceString;
                  if (windowNumber == j) {
                     if (lastValid[windowNumber]) {
                        const double dist = MathUtilities::distance3D(xyz, lastXYZ[windowNumber]);
                        distanceString = (" Distance: "
                                          + QString::number(dist, 'f', significantDigits));
                     }
                     lastValid[windowNumber] = true;
                     lastXYZ[windowNumber][0] = xyz[0];
                     lastXYZ[windowNumber][1] = xyz[1];
                     lastXYZ[windowNumber][2] = xyz[2];
                  }
                  
                  //
                  // XYZ postion
                  //
                  idString += (tagIndentation
                               + getWindowName(j)
                               + " "
                               + idSurface->getSurfaceTypeName()
                               + " XYZ: "
                               + QString::number(xyz[0], 'f', significantDigits)
                               + ", "
                               + QString::number(xyz[1], 'f', significantDigits)
                               + ", "
                               + QString::number(xyz[2], 'f', significantDigits)
                               + distanceString
                               + tagNewLine);
               }
            }
         }
      }
      
      //
      // Areal Estimation Information
      //
      if (getDisplayNodeArealEstInformation()) {
         ArealEstimationFile* aef = brainSet->getArealEstimationFile();
         const int numCols = aef->getNumberOfColumns();
         if (numCols > 0) {
            DisplaySettingsArealEstimation* dsae = brainSet->getDisplaySettingsArealEstimation();
            //const int selectedColumn = dsae->getSelectedDisplayColumn(brainModelIndex);
            std::vector<bool> selectedColumnFlags;
            dsae->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
            
            for (int i = 0; i < numCols; i++) {
               idString += (tagIndentation
                            + "Areal Estimation: ");
               if (selectedColumnFlags[i]) {
                  idString += tagBoldStart;
               }
               idString += (aef->getColumnName(i)
                            + " ");
               QString areas[4];
               float prob[4];
               aef->getNodeData(nodeNumber, i, areas, prob);               
               for (int j = 0; j < 4; j++) {
                  idString += (linkToVocabulary(brainSet, areas[j])
                               + " "
                               + QString::number(prob[j], 'f', significantDigits)
                               + "  ");
               }
               if (selectedColumnFlags[i]) {
                  idString += tagBoldEnd;
               }
               idString += tagNewLine;
            }

            for (int i = 0; i < numCols; i++) {
               if (selectedColumnFlags[i]) {
                  //
                  // Study Meta Data from Study Metadata File
                  //
                  const StudyMetaDataLinkSet smdls = aef->getColumnStudyMetaDataLinkSet(i);
                  StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
                  idString += getIdentificationTextForStudies(smdf,
                                                              smdls,
                                                              true);
               }
            }
         }
      }

      //
      // CoCoMac Information
      //
      if (brainSet->isASurfaceOverlay(brainModelIndex,
                        BrainModelSurfaceOverlay::OVERLAY_COCOMAC)) {
         DisplaySettingsCoCoMac* dsc = brainSet->getDisplaySettingsCoCoMac();
         dsc->setSelectedNode(nodeNumber);
         idString += htmlTranslate(dsc->getIDInfo());
      }
    
      //
      // Geodesic Distance
      //
      DisplaySettingsGeodesicDistance* dsgd = brainSet->getDisplaySettingsGeodesicDistance();
      GeodesicDistanceFile* gdf = brainSet->getGeodesicDistanceFile();
      if (dsgd->getPathDisplayEnabled()) {
         const int column = dsgd->getDisplayColumn();
         if ((column >= 0) && (column < gdf->getNumberOfColumns())) {
            if ((nodeNumber >= 0) && (nodeNumber < gdf->getNumberOfNodes())) {
               dsgd->setPathDisplayNodeNumber(nodeNumber);
               const float dist = gdf->getNodeParentDistance(nodeNumber, column);
               idString += (tagIndentation
                            + "Geodesic Distance: "
                            + QString::number(dist, 'f', significantDigits)
                            + tagNewLine);
            }
         }
      }

      //
      // Show lat lon of node
      //
      if (getDisplayNodeLatLonInformation()) {
         LatLonFile* llf = brainSet->getLatLonFile();
         int numCols = llf->getNumberOfColumns();
         for (int j = 0; j < numCols; j++) {
            float lat, lon;
            llf->getLatLon(nodeNumber, j, lat, lon);
            const QString name(llf->getColumnName(j));
            
            idString += (tagIndentation
                         + "Lat/Lon "
                         + htmlTranslate(name)
                         + ": "
                         + QString::number(lat, 'f', significantDigits)
                         + " "
                         + QString::number(lon, 'f', significantDigits)
                         + tagNewLine);
            
            if (llf->getDeformedLatLonValid(j)) {
               llf->getDeformedLatLon(nodeNumber, j, lat, lon);
               idString += (tagIndentation
                            + "Lat/Lon "
                            + htmlTranslate(name)
                            + ": "
                            + QString::number(lat, 'f', significantDigits)
                            + " "
                            + QString::number(lon, 'f', significantDigits)
                            + tagNewLine);
            }
         }
      }
      
      if (getDisplayNodeMetricInformation()) {
         //
         // Show metric
         //
         MetricFile* mf = brainSet->getMetricFile();
         const int numCols = mf->getNumberOfColumns();
         if (numCols > 0) {
            DisplaySettingsMetric* dsm = brainSet->getDisplaySettingsMetric();
            //const int selectedColumn = dsm->getSelectedDisplayColumn(brainModelIndex);
            std::vector<bool> selectedColumnFlags;
            dsm->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
            
            idString += (tagIndentation
                         + "Metric: ");
            for (int i = 0; i < mf->getNumberOfColumns(); i++) {
               if (selectedColumnFlags[i]) {
                  idString += tagBoldStart;
               }
               idString += (QString::number(mf->getValue(nodeNumber, i), 'f', significantDigits)
                            + " ");
               if (selectedColumnFlags[i]) {
                  idString += tagBoldEnd;
               }
            }
            idString += tagNewLine;
            
            for (int i = 0; i < numCols; i++) {
               if ((selectedColumnFlags[i]) &&
                   (mf->getValue(nodeNumber, i) != 0.0)) {
                  //
                  // Study Meta Data from Study Metadata File
                  //
                  const StudyMetaDataLinkSet smdls = mf->getColumnStudyMetaDataLinkSet(i);
                  StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
                  idString += getIdentificationTextForStudies(smdf,
                                                              smdls,
                                                              true);
               }
            }
         }
      }
      
      //
      // Show Paint Information
      //
      if (getDisplayNodePaintInformation()) {
         PaintFile* pf = brainSet->getPaintFile();
         const int numCols = pf->getNumberOfColumns();
         if (numCols > 0) {
            DisplaySettingsPaint* dsp = brainSet->getDisplaySettingsPaint();
            //const int selectedPaintColumn = dsp->getSelectedDisplayColumn(brainModelIndex);
            std::vector<bool> selectedColumnFlags;
            dsp->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
            
            idString += (tagIndentation
                         + "Paint: ");
            for (int i = 0; i < numCols; i++) {
               const int paintIndex = pf->getPaint(nodeNumber, i);
               if (selectedColumnFlags[i]) {
                  idString += tagBoldStart;
               }
               idString += linkToVocabulary(brainSet, pf->getPaintNameFromIndex(paintIndex));
               if (selectedColumnFlags[i]) {
                  idString += tagBoldEnd;
               }
               idString += " ";
            }
            idString += tagNewLine;
            
            for (int i = 0; i < numCols; i++) {
               if (selectedColumnFlags[i]) {
                  //
                  // Is this an "unassigned" paint
                  //
                  bool showMetaDataFlag = true;
                  const int paintIndex = pf->getPaint(nodeNumber, i);
                  if ((paintIndex >= 0) &&
                      (paintIndex < pf->getNumberOfPaintNames())) {
                     const QString paintName = pf->getPaintNameFromIndex(paintIndex);
                     if (paintName.startsWith("?")) {
                        showMetaDataFlag = false;
                     }
                  }
                  
                  //
                  // Study Meta Data from Study Metadata File
                  //
                  if (showMetaDataFlag) {
                     const StudyMetaDataLinkSet smdls = pf->getColumnStudyMetaDataLinkSet(i);
                     StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
                     idString += getIdentificationTextForStudies(smdf,
                                                                 smdls,
                                                                 true);
                  }
               }
            }
         }
      }


      //
      // Show probabilistic atlas information
      //
      if (getDisplayNodeProbAtlasInformation()) {
         ProbabilisticAtlasFile* pf = brainSet->getProbabilisticAtlasSurfaceFile();
         const int numCols = pf->getNumberOfColumns();
         if (numCols > 0) {
            idString += (tagIndentation
                         + "Prob Atlas: ");
            for (int i = 0; i < pf->getNumberOfColumns(); i++) {
               const int paintIndex = pf->getPaint(nodeNumber, i);
               idString += (linkToVocabulary(brainSet, pf->getPaintNameFromIndex(paintIndex))
                            + " ");
            }
            idString += tagNewLine;
            
            //
            // Study Meta Data from Study Metadata File
            //
            const StudyMetaDataLinkSet smdls = pf->getStudyMetaDataLinkSet();
            StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
            idString += getIdentificationTextForStudies(smdf,
                                                        smdls,
                                                        true);
         }
      }

      //
      // Show RGB Paint Information
      //
      if (getDisplayNodeRgbPaintInformation()) {      
         RgbPaintFile* rpf = brainSet->getRgbPaintFile();
         const int numCols = rpf->getNumberOfColumns();
         if (numCols > 0) {
            DisplaySettingsRgbPaint* dsrp = brainSet->getDisplaySettingsRgbPaint();
            //const int selectedColumn = dsrp->getSelectedDisplayColumn(brainModelIndex);
            std::vector<bool> selectedColumnFlags;
            dsrp->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
            
            for (int i = 0; i < numCols; i++) {
               idString += (tagIndentation
                            + "RGB Paint: ");
               if (selectedColumnFlags[i]) {
                  idString + tagBoldStart;
               }
               float r, g, b;
               rpf->getRgb(nodeNumber, i, r, g, b);
               idString += (htmlTranslate(rpf->getColumnName(i))
                            + " "
                            + QString::number(r, 'f', significantDigits)
                            + " "
                            + QString::number(g, 'f', significantDigits)
                            + " "
                            + QString::number(b, 'f', significantDigits));
               if (selectedColumnFlags[i]) {
                  idString += tagBoldEnd;
               }
               idString += tagNewLine;
            }      
         }
      }

      //
      // Section information
      //
      if (getDisplayNodeSectionInformation()) {
         SectionFile* sf = brainSet->getSectionFile();
         int numCols = sf->getNumberOfColumns();
         if (numCols > 0) {
            idString += (tagIndentation
                         + "Section: ");
            for (int j = 0; j < numCols; j++) {
               idString += (QString::number(sf->getSection(nodeNumber, j))
                            + " ");
            }
            idString += tagNewLine;
         }
      }
            
      //
      // Surface shape information
      //
      if (getDisplayNodeShapeInformation()) {
         SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
         const int numCols = ssf->getNumberOfColumns();
         if (numCols > 0) {
            DisplaySettingsSurfaceShape* dss = brainSet->getDisplaySettingsSurfaceShape();
            //const int selectedColumn = dss->getSelectedDisplayColumn(brainModelIndex);
            std::vector<bool> selectedColumnFlags;
            dss->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
            
            idString += (tagIndentation
                         + "Shape: ");
            for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
               if (selectedColumnFlags[i]) {
                  idString += tagBoldStart;
               }
               idString += (QString::number(ssf->getValue(nodeNumber, i), 'f', significantDigits)
                            + " ");
               if (selectedColumnFlags[i]) {
                  idString += tagBoldEnd;
               }
            }
            idString += tagNewLine;
            
            for (int i = 0; i < ssf->getNumberOfColumns(); i++) {
               if (selectedColumnFlags[i]) {
                  //
                  // Study Meta Data from Study Metadata File
                  //
                  const StudyMetaDataLinkSet smdls = ssf->getColumnStudyMetaDataLinkSet(i);
                  StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
                  idString += getIdentificationTextForStudies(smdf,
                                                              smdls,
                                                              true);
               }
            }
         }
      }

      //
      // Topography information
      //
      if (getDisplayNodeTopographyInformation()) {
         TopographyFile* tf = brainSet->getTopographyFile();
         const int numCols = tf->getNumberOfColumns();
         if (numCols > 0) {
            DisplaySettingsTopography* dst = brainSet->getDisplaySettingsTopography();
            //const int selectedColumn = dst->getSelectedDisplayColumn(brainModelIndex);
            std::vector<bool> selectedColumnFlags;
            dst->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);

            for (int i = 0; i < numCols; i++) {
               const NodeTopography nt = tf->getNodeTopography(nodeNumber, i);       
               float eMean, eLow, eHigh, pMean, pLow, pHigh;
               QString areaName;
               nt.getData(eMean, eLow, eHigh, pMean, pLow,
                              pHigh, areaName);
               if (areaName.isEmpty() == false) {
                  if (selectedColumnFlags[i]) {
                     idString += tagBoldStart;
                  }                  
                  idString += (tagIndentation
                               + "Topography Eccentricity: "
                               + "mean: "
                               + QString::number(eMean, 'f', significantDigits)
                               + " range: "
                               + QString::number(eLow, 'f', significantDigits)
                               + " to "
                               + QString::number(eHigh, 'f', significantDigits));
                  if (selectedColumnFlags[i]) {
                     idString += tagBoldEnd;
                  }                  


                  if (selectedColumnFlags[i]) {
                     idString += tagBoldStart;
                  }                  
                  idString += (tagIndentation
                               + "Topography Polar Angle: "
                               + "mean: "
                               + QString::number(pMean, 'f', significantDigits)
                               + " range: "
                               + QString::number(pLow, 'f', significantDigits)
                               + " to "
                               + QString::number(pHigh, 'f', significantDigits));
                  if (selectedColumnFlags[i]) {
                     idString += tagBoldEnd;
                  }   
                  idString += tagNewLine;               
               }
            }
         }
      }


   }

   return idString;
}
                                    
/**
 * get the name of a window.
 */
QString 
BrainModelIdentification::getWindowName(const int windowNumber) const
{
   QString windowName;
   switch (static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(windowNumber)) {
      case BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW:
         windowName = "Main Window   ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2:
         windowName = "View Window 2 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_3:
         windowName = "View Window 3 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_4:
         windowName = "View Window 4 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_5:
         windowName = "View Window 5 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_6:
         windowName = "View Window 6 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_7:
         windowName = "View Window 7 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_8:
         windowName = "View Window 8 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_9:
         windowName = "View Window 9 ";
         break;
      case BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_10:
         windowName = "View Window 10 ";
         break;
      case BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS:
         break;
   }
   return windowName;
}
      
/**
 * get identification text for volume border.
 */
QString 
BrainModelIdentification::getIdentificationTextForVolumeBorder()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem volumeBorderItem = openGL->getSelectedVolumeBorder();

   BrainSet* brainSet     = volumeBorderItem.getBrainSet();
   const int borderNumber = volumeBorderItem.getItemIndex1();
   const int linkNumber   = volumeBorderItem.getItemIndex2();
   
   if ((brainSet != NULL) &&
       (borderNumber >= 0) &&
       (linkNumber >= 0)) {
      const BorderFile* bf = brainSet->getVolumeBorderFile();
      const Border* b = bf->getBorder(borderNumber);
      const float* xyz = b->getLinkXYZ(linkNumber);
      
      idString += (tagBoldStart
                   + "Volume Border"
                   + tagBoldEnd
                   + ": "
                   + QString::number(borderNumber)
                   + " Link: "
                   + QString::number(linkNumber)
                   + ", Name: "
                   + linkToVocabulary(brainSet, htmlTranslate(b->getName()))
                   + "   ("
                   + QString::number(xyz[0], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[1], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[2], 'f', significantDigits)
                   + ")"
                   + tagNewLine);
   }
   
   return idString;
}

/**
 * get identification text for surface border.
 */
QString 
BrainModelIdentification::getIdentificationTextForSurfaceBorder()
{
   QString idString;
   
   //
   // two borders may be identified
   //
   for (int i = 0; i < 2; i++) {
      //
      // Get the selected border information
      //
      BrainModelOpenGLSelectedItem borderID = openGL->getSelectedBorder1();
      if (i == 1) {
         borderID = openGL->getSelectedBorder2();
      }
      
      //
      // Make sure valid border identified
      //
      BrainSet* brainSet = borderID.getBrainSet();
      const int brainModelIndex = borderID.getItemIndex1();
      const int borderNumber = borderID.getItemIndex2();
      const int linkNumber = borderID.getItemIndex3();
      if ((brainSet != NULL) &&
          (brainModelIndex >= 0) &&
          (borderNumber >= 0) &&
          (linkNumber >= 0)) {
         
         BrainModelBorderSet* bmbs = brainSet->getBorderSet();
         if (bmbs->getNumberOfBorders() > borderNumber) {
            BrainModelBorder* b = bmbs->getBorder(borderNumber);
            BrainModelBorderLink* link = b->getBorderLink(linkNumber);
            float xyz[3];
            link->getLinkPosition(brainModelIndex, xyz);
            idString += (tagBoldStart
                         + "Surface Border"
                         + tagBoldEnd
                         + ": "
                         + QString::number(borderNumber)
                         + " Link: "
                         + QString::number(linkNumber)
                         + " Name: "
                         + linkToVocabulary(brainSet, htmlTranslate(b->getName()))
                         + "   ("
                         + QString::number(xyz[0], 'f', significantDigits)
                         + ", "
                         + QString::number(xyz[1], 'f', significantDigits)
                         + ", "
                         + QString::number(xyz[2], 'f', significantDigits)
                         + ")"
                         + tagNewLine);
         }
      }
   }
   
   return idString;
}

/**
 * get identification text for cell.
 */
QString 
BrainModelIdentification::getIdentificationTextForCellProjection()
{
   QString idString;

   BrainModelOpenGLSelectedItem cellID = openGL->getSelectedCellProjection();
   BrainSet* brainSet = cellID.getBrainSet();
   const int cellNumber = cellID.getItemIndex1();
   
   if ((brainSet != NULL) &&
       (cellNumber >= 0)) {
      CellProjectionFile* cf = brainSet->getCellProjectionFile();
      if (cf == NULL) {
         return "";
      }

      CellProjection* cell = cf->getCellProjection(cellNumber);
      const float* xyz = cell->getXYZ();
      idString += (tagBoldStart
                   + "Cell"
                   + tagBoldEnd
                   + " "
                   + QString::number(cellNumber)
                   + ": "
                   + linkToVocabulary(brainSet, htmlTranslate(cell->getName()))
                   + "  Class: "
                   + cell->getClassName()
                   + "   Position: ("
                   + QString::number(xyz[0], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[1], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[2], 'f', significantDigits)
                   + ")"
                   + tagNewLine);
   }

   return idString;
}

/**
 * get identification text for foci.
 */
QString 
BrainModelIdentification::getIdentificationTextForFoci()
{
   if (idFilter.anyFociDataOn() == false) {
      return "";
   }

   QString idString;
   
   BrainModelOpenGLSelectedItem focusID = openGL->getSelectedFocusProjection();
   const int focusNumber = focusID.getItemIndex1();
   BrainSet* brainSet = focusID.getBrainSet();

   if ((brainSet != NULL) &&
       (focusNumber >= 0)) {   
      FociProjectionFile* ff = brainSet->getFociProjectionFile();
      if (ff == NULL) {
         return "";
      }
      
      CellProjection* focus = ff->getCellProjection(focusNumber);
      idString += getIdentificationTextForSingleFocus(focusID,
                                                      focus,
                                                      ff,
                                                      false);
   }
   return idString;
}

/**
 * get identification text for a focus.
 */
QString 
BrainModelIdentification::getIdentificationTextForSingleFocus(
                                            BrainModelOpenGLSelectedItem focusID,
                                            CellProjection* focus,
                                            FociProjectionFile* fociProjectionFile,
                                            const bool volumeFlag)                                            
{
   QString idString;
   
   const int focusNumber = focusID.getItemIndex1();
   BrainSet* brainSet = focusID.getBrainSet();

   if (focus != NULL) {
      idString += (tagBoldStart
                  + "Focus "
                  + tagBoldEnd
                  + QString::number(focusNumber)
                  + tagNewLine);
      
      if (getDisplayFociAreaInformation()) {
         if (focus->getArea().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Area: "
                         + tagBoldEnd
                         + linkStringToVocabulary(brainSet, focus->getArea())
                         + tagNewLine);
         }
      }
      
      if (getDisplayFociClassInformation()) {
         if (focus->getClassName().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Class: "
                         + tagBoldEnd
                         + htmlTranslate(focus->getClassName())
                         + tagNewLine);
         }
      }
                   
      if (getDisplayFociCommentInformation()) {      
         if (focus->getComment().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Comment: "
                         + tagBoldEnd
                         + StringUtilities::convertURLsToHyperlinks(htmlTranslate(focus->getComment()))
                         + tagNewLine);
         }
      }
      
      if (getDisplayFociGeographyInformation()) {
         if (focus->getGeography().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Geography: "
                         + tagBoldEnd
                         + linkStringToVocabulary(brainSet, focus->getGeography())
                         + tagNewLine);
         }
      }
      
      if (getDisplayFociNameInformation()) {
         idString += (tagIndentation
                      + tagBoldStart
                      + "Name: "
                      + tagBoldEnd
                      + linkToVocabulary(brainSet, htmlTranslate(focus->getName()))
                      + tagNewLine);
      }
      
      float xyz[3];
      focus->getXYZ(xyz);
      QString spaceName;
      const int studyNumber = focus->getStudyNumber();
      const CellStudyInfo* csi = fociProjectionFile->getStudyInfo(studyNumber);
      if (csi != NULL) {
         spaceName = csi->getStereotaxicSpace();
         if (spaceName.isEmpty() == false) {
            spaceName.insert(0, "(");
            spaceName.append(")");
         }
      }
      
      if (getDisplayFociSizeInformation()) {
         if (focus->getSize() != 0.0) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Extent: "
                         + tagBoldEnd
                         + QString::number(focus->getSize(), 'f', significantDigits)
                         + tagNewLine);
         }
      }
      
      if (getDisplayFociRegionOfInterestInformation()) {
         if (focus->getRegionOfInterest().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "RegionOfInterest: "
                         + tagBoldEnd
                         + linkStringToVocabulary(brainSet, focus->getRegionOfInterest())
                         + tagNewLine);
         }
      }
      Structure focusStructure;
      if (getDisplayFociOriginalStereotaxicPositionInformation()) {
         float volumeXYZ[3];
         focus->getVolumeXYZ(volumeXYZ);
         if ((volumeFlag) ||
             (volumeXYZ[0] != 0.0)) {
            idString += (tagIndentation 
                         + tagBoldStart
                         + "Volume Position"
                         + tagBoldEnd
                         + spaceName
                         + ": ("
                         + QString::number(volumeXYZ[0], 'f', significantDigits)
                         + ", "
                         + QString::number(volumeXYZ[1], 'f', significantDigits)
                         + ", "
                         + QString::number(volumeXYZ[2], 'f', significantDigits)
                         + ")"
                         + tagNewLine);
            if (volumeXYZ[0] > 0) {
               focusStructure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
            }
            else if (volumeXYZ[1] < 0) {
               focusStructure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
            }
         }
      }
      
      if (getDisplayFociOriginalStereotaxicPositionInformation()) {
         idString += (tagIndentation 
                      + tagBoldStart
                      + "Stereotaxic Position (Original)"
                      + tagBoldEnd
                      + spaceName
                      + ": ("
                      + QString::number(xyz[0], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[1], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[2], 'f', significantDigits)
                      + ")"
                      + tagNewLine);
      }
      
      if (getDisplayFociStereotaxicPositionInformation()) {
         const BrainModelSurface* bms = focusID.getBrainModelSurface();
         if (bms != NULL) {
            focusStructure = bms->getStructure();
         }
         for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
            const BrainModelSurface* surface = brainSet->getBrainModelSurface(i);
            if (surface != NULL) {
               if (surface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
                  if (surface->getStructure() == focusStructure) {
                     const CoordinateFile* cf = surface->getCoordinateFile();
                     QString surfaceSpaceName = cf->getHeaderTag(AbstractFile::headerTagCoordFrameID);
                     
                     bool doIt = false;
                     if (StereotaxicSpace::validStereotaxicSpaceName(surfaceSpaceName)) {
                        doIt = true;
                     }
                     else if (bms == surface) {
                        doIt = true;
                        surfaceSpaceName = "Unknown Space";
                     }
                     if (doIt) {
                        float projPos[3];
                        CellProjection* cp = dynamic_cast<CellProjection*>(focus);
                        if (cp != NULL) {
                           if (cp->getProjectedPosition(cf, 
                                                       surface->getTopologyFile(),
                                                       false,
                                                       false,
                                                       false,
                                                       projPos)) {
                                                          
                              idString += (tagIndentation
                                           + tagBoldStart
                                           + "Stereotaxic Position"
                                           + tagBoldEnd
                                           + " ("
                                           + surfaceSpaceName
                                           + "): ("
                                           + QString::number(projPos[0], 'f', significantDigits)
                                           + ", "
                                           + QString::number(projPos[1], 'f', significantDigits)
                                           + ", "
                                           + QString::number(projPos[2], 'f', significantDigits)
                                           + ")"
                                           + tagNewLine);
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      
      if (getDisplayFociStatisticInformation()) {
         if (focus->getStatistic().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Statistic: "
                         + tagBoldEnd
                         + focus->getStatistic()
                         + tagNewLine);
         }
      }
      
      if (getDisplayFociStructureInformation()) {
         if (Structure::convertTypeToString(focus->getCellStructure()).isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Structure: "
                         + tagBoldEnd
                         + Structure::convertTypeToString(focus->getCellStructure())
                         + tagNewLine);
         }
      }
      
      //
      // Study Meta Data from Study Metadata File
      //
      const StudyMetaDataLinkSet smdls = focus->getStudyMetaDataLinkSet();
      StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
      idString += getIdentificationTextForStudies(smdf,
                                                  smdls,
                                                  true);
/*
      const int smdIndex = smdf->getStudyIndexFromLink(smdl);
      if ((smdIndex >= 0) &&
          (smdIndex < smdf->getNumberOfStudyMetaData())) {
         const StudyMetaData* smd = smdf->getStudyMetaData(smdIndex);
         if (smd != NULL) {
            idString += getIdentificationTextForStudy(smd, smdIndex, &smdl);
            idString += getIdentificationTextForMetaAnalysisStudies(smd);
         }
      }
*/
      
      //
      // Old Study Info
      //
      if ((studyNumber >= 0) && 
          (studyNumber < fociProjectionFile->getNumberOfStudyInfo())) {
         csi = fociProjectionFile->getStudyInfo(studyNumber);
      }
      
      if (csi != NULL) {
         if (csi->getTitle().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Study Title: "
                         + tagBoldEnd
                         + csi->getTitle()
                         + tagNewLine);
         }
         if (csi->getAuthors().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Study Authors: "
                         + tagBoldEnd
                         + csi->getAuthors()
                         + tagNewLine);
         }
         if (csi->getCitation().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Study Citation: "
                         + tagBoldEnd
                         + csi->getCitation()
                         + tagNewLine);
         }
         if (csi->getURL().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Study URL: "
                         + tagBoldEnd
                         + StringUtilities::convertURLsToHyperlinks(csi->getURL())
                         + tagNewLine);
         }
         if (csi->getKeywords().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Study Keywords:  "
                         + tagBoldEnd
                         + csi->getKeywords()
                         + tagNewLine);
         }
         if (csi->getStereotaxicSpace().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Study Stereotaxic Space: "
                         + tagBoldEnd
                         + csi->getStereotaxicSpace()
                         + tagNewLine);
         }
         if (csi->getComment().isEmpty() == false) {
            idString += (tagIndentation
                         + tagBoldStart
                         + "Study Comment: "
                         + tagBoldEnd
                         + StringUtilities::convertURLsToHyperlinks(htmlTranslate(csi->getComment()))
                         + tagNewLine);
         }
      }
   }

   return idString;
}

/**
 * get identification text for voxel.
 */
QString 
BrainModelIdentification::getIdentificationTextForVoxel()
{
   QString idString;
   
   for (int indx = 0; indx < 3; indx++) {
      BrainModelOpenGLSelectedItem voxelID;
      switch (indx) {
         case 0: 
            voxelID = openGL->getSelectedVoxelUnderlay();
            break;
         case 1: 
            voxelID = openGL->getSelectedVoxelOverlaySecondary();
            break;
         case 2: 
            voxelID = openGL->getSelectedVoxelOverlayPrimary();
            break;
      }
      
      BrainSet* brainSet = voxelID.getBrainSet();
      const int i = voxelID.getItemIndex1();
      const int j = voxelID.getItemIndex2();
      const int k = voxelID.getItemIndex3();
      
      if ((brainSet != NULL) &&
          (i >= 0) &&
          (j >= 0) &&
          (k >= 0)) {
         BrainModelVolume* bmv = brainSet->getBrainModelVolume();
         if (bmv != NULL) {
            VolumeFile* selectionVolume = NULL;
            switch (indx) {
               case 0:
                  selectionVolume = bmv->getUnderlayVolumeFile();
                  break;
               case 1:
                  selectionVolume = bmv->getOverlaySecondaryVolumeFile();
                  break;
               case 2:
                  selectionVolume = bmv->getOverlayPrimaryVolumeFile();
                  break;
            } // switch
            
            if (selectionVolume != NULL) {
               idString += getVolumeFileIdentificationText(brainSet,
                                                           bmv,
                                                           selectionVolume,
                                                           i,
                                                           j,
                                                           k);
               //
               // Only show info for the first volume since it will cause
               // values for all volumes at the identified location
               //
               break;
            }
         } // if (bmv != NULL...
      } // if (brainSet != NULL...
   } // for (indx...
   
   return idString;
}   

      /*
         BrainModelVolume* bmv = theMainWindow->getBrainSet(windowNumber)->getBrainModelVolume(-1);
         if (bmv != NULL) {
            const BrainSet* selectedBrainSet = theMainWindow->getBrainSet(windowNumber);
            
            //
            // Place all volume viewer at selected voxel
            //
            int ijk[3] = { i, j, k };
            for (int k = 0; k < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; k++) {
               //
               // Skip this window if the brain sets do no match
               //
               if (theMainWindow->getBrainSet(static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(k)) != 
                   selectedBrainSet) {
                  continue;
               }
               
               bool adjustSlicesFlag = true;
               if (windowNumber == k) {
                  if (tm != NULL) {
                     adjustSlicesFlag = false;
                  }
               }
               
               switch (bmv->getSelectedAxis(windowNumber)) {
                  case VolumeFile::VOLUME_AXIS_X:
                  case VolumeFile::VOLUME_AXIS_Y:
                  case VolumeFile::VOLUME_AXIS_Z:
                  case VolumeFile::VOLUME_AXIS_ALL:
                     break;
                  case VolumeFile::VOLUME_AXIS_OBLIQUE:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_X: 
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                  case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                     adjustSlicesFlag = false;
                     break;
                  case VolumeFile::VOLUME_AXIS_UNKNOWN:
                     break;
               }
               
               if (adjustSlicesFlag) {
                  bmv->setSelectedOrthogonalSlices(k, ijk);
                  GuiToolBar::updateAllToolBars(false);
               }
            }
         }
      */

/**
 * get identification text for a volume file.
 */
QString 
BrainModelIdentification::getVolumeFileIdentificationText(BrainSet* brainSet,
                                                          BrainModelVolume* bmv,
                                                          VolumeFile* selectionVolume,
                                                          const int vi,
                                                          const int vj,
                                                          const int vk)
{
   QString idString;
   
   //
   // Coordinate of voxel
   //
   float xyz[3];
   selectionVolume->getVoxelCoordinate(vi, vj, vk, true, xyz);
   //if (tm != NULL) {
   //   tm->multiplyPoint(xyz);
   //}

   const QString indent("\t");
   
   //
   // Display the voxel index and coordinate
   //
   idString += (tagBoldStart 
                + "VOXEL ("
                + selectionVolume->getVolumeTypeDescription()
                + ")"
                + tagBoldEnd
                + " IJK ("
                + QString::number(vi)
                + ", "
                + QString::number(vj)
                + ", "
                + QString::number(vk)
                + ")  XYZ ("
                + QString::number(xyz[0], 'f', significantDigits)
                + ", "
                + QString::number(xyz[1], 'f', significantDigits)
                + ", "
                + QString::number(xyz[2], 'f', significantDigits)
                + ")"
                + tagNewLine);
                
   //
   // Loop through the file types
   //
   for (int m = 0; m < 6; m++) {
      //
      // Get name of file type and the files of that type
      //
      QString volumeName;
      std::vector<VolumeFile*> files;
      VolumeFile::VOLUME_TYPE volumeType = VolumeFile::VOLUME_TYPE_UNKNOWN;
      switch (static_cast<VolumeFile::VOLUME_TYPE>(m)) {
         case VolumeFile::VOLUME_TYPE_ANATOMY:
            volumeName = "Anatomy: ";
            brainSet->getVolumeAnatomyFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_ANATOMY;
            break;
         case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
            volumeName = "Functional: ";
            brainSet->getVolumeFunctionalFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_FUNCTIONAL;
            break;
         case VolumeFile::VOLUME_TYPE_PAINT:
            volumeName = "Paint: ";
            brainSet->getVolumePaintFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_PAINT;
            break;
         case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
            volumeName = "Prob Atlas: ";
            brainSet->getVolumeProbAtlasFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_PROB_ATLAS;
            break;
         case VolumeFile::VOLUME_TYPE_RGB:
            volumeName = "RGB: ";
            brainSet->getVolumeRgbFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_RGB;
            break;
         case VolumeFile::VOLUME_TYPE_ROI:
            volumeName = "ROI: ";
            //brainSet->getVolumeRgbFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_ROI;
            break;
         case VolumeFile::VOLUME_TYPE_SEGMENTATION:
            volumeName = "Segmentation: ";
            brainSet->getVolumeSegmentationFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_SEGMENTATION;
            break;
         case VolumeFile::VOLUME_TYPE_VECTOR:
            volumeName = "Vector: ";
            brainSet->getVolumeVectorFiles(files);
            volumeType = VolumeFile::VOLUME_TYPE_VECTOR;
            break;
         case VolumeFile::VOLUME_TYPE_UNKNOWN:
            break;
      }
      
      //
      // If there are data files
      //
      if (files.empty() == false) {
         //
         // add name of file type
         //
         idString += (tagIndentation
                      + volumeName);
                      
         //
         // Print info about each file
         //
         for (unsigned int n = 0; n < files.size(); n++) {
            int ijk[3];
            if (files[n]->convertCoordinatesToVoxelIJK(xyz, ijk)) {
               switch (volumeType) {
                  case VolumeFile::VOLUME_TYPE_ANATOMY:
                     if (bmv->getSelectedVolumeAnatomyFile() == files[n]) {
                        idString += tagBoldStart;
                     }
                     idString += QString::number(files[n]->getVoxel(ijk), 'f', significantDigits);
                     if (bmv->getSelectedVolumeAnatomyFile() == files[n]) {
                        idString += tagBoldEnd;
                     }
                     break;
                  case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
                     if (bmv->getSelectedVolumeFunctionalViewFile() == files[n]) {
                        idString += tagBoldStart;
                     }
                     idString += QString::number(files[n]->getVoxel(ijk), 'f', significantDigits);
                     if (bmv->getSelectedVolumeFunctionalViewFile() == files[n]) {
                        idString += tagBoldEnd;
                     }
                     break;
                  case VolumeFile::VOLUME_TYPE_PAINT:
                     {
                        if (bmv->getSelectedVolumePaintFile() == files[n]) {
                           idString += tagBoldStart;
                        }
                        const int voxel = static_cast<int>(files[n]->getVoxel(ijk));
                        if ((voxel >= 0) && (voxel < files[n]->getNumberOfRegionNames())) {
                           idString += linkToVocabulary(brainSet, files[n]->getRegionNameFromIndex(voxel));
                        }
                        else {
                           idString += "bad-index ";
                        }
                        if (bmv->getSelectedVolumePaintFile() == files[n]) {
                           idString += tagBoldEnd;
                        }
                     }
                     break;
                  case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
                     {
                        const int voxel = static_cast<int>(files[n]->getVoxel(ijk));
                        if ((voxel >= 0) && (voxel < files[n]->getNumberOfRegionNames())) {
                           idString += linkToVocabulary(brainSet, files[n]->getRegionNameFromIndex(voxel));
                        }
                        else {
                           idString += "bad-index";
                        }
                     }
                     break;
                  case VolumeFile::VOLUME_TYPE_ROI:
                     break;
                  case VolumeFile::VOLUME_TYPE_RGB:
                     {
                        if (bmv->getSelectedVolumeRgbFile() == files[n]) {
                           idString += tagBoldStart;
                        }
                        float rgb[3];
                        files[n]->getVoxel(vi, vj, vk);
                        idString += ("("
                                     + QString::number(rgb[0], 'f', significantDigits)
                                     + ", "
                                     + QString::number(rgb[1], 'f', significantDigits)
                                     + ", "
                                     + QString::number(rgb[2], 'f', significantDigits)
                                     + ")");
                        if (bmv->getSelectedVolumeRgbFile() == files[n]) {
                           idString += tagBoldEnd;
                        }
                     }
                     break;
                  case VolumeFile::VOLUME_TYPE_SEGMENTATION:
                     if (bmv->getSelectedVolumeSegmentationFile() == files[n]) {
                        idString += tagBoldStart;
                     }
                     idString += QString::number(files[n]->getVoxel(ijk), 'f', significantDigits);
                     if (bmv->getSelectedVolumeSegmentationFile() == files[n]) {
                        idString += tagBoldEnd;
                     }
                     break;
                  case VolumeFile::VOLUME_TYPE_VECTOR:
                     if (bmv->getSelectedVolumeVectorFile() == files[n]) {
                        idString += tagBoldStart;
                     }
                     idString += QString::number(files[n]->getVoxel(ijk), 'f', significantDigits);
                     if (bmv->getSelectedVolumeVectorFile() == files[n]) {
                        idString += tagBoldEnd;
                     }
                     break;
                  case VolumeFile::VOLUME_TYPE_UNKNOWN:
                     break;
               } // switch
            } // if (files[n]->convertCoordinatesToVoxelIJK(xyz, ijk)) {
            idString += " ";
         }  // for
         
         idString += tagNewLine;
         
         for (unsigned int n = 0; n < files.size(); n++) {
            const StudyMetaDataLinkSet smdls = files[n]->getStudyMetaDataLinkSet();
            const StudyMetaDataFile* smdf = brainSetParent->getStudyMetaDataFile();
            idString += getIdentificationTextForStudies(smdf,
                                                        smdls,
                                                        true);
                                                        
            //
            // Only show 1st prob atlas
            // 
            if (volumeType == VolumeFile::VOLUME_TYPE_PROB_ATLAS) {
               break;
            }
         }
         
      } // if (files.empty()
   } // for (m...

   idString += tagNewLine;
   
   //
   // Add linked study meta data
   //
   //const StudyMetaDataLinkSet smdls = selectionVolume->getStudyMetaDataLinkSet();
   //const StudyMetaDataFile* smdf = brainSetParent->getStudyMetaDataFile();
   //idString += getIdentificationTextForStudies(smdf,
   //                                            smdls,
   //                                            true);
/*
   const int studyNumber = smdf->getStudyIndexFromLink(smdl);
   if ((studyNumber >= 0) && (studyNumber < smdf->getNumberOfStudyMetaData())) {
      const StudyMetaData* smd = smdf->getStudyMetaData(studyNumber);
      idString += getIdentificationTextForStudy(smd, studyNumber, &smdl);
      idString += getIdentificationTextForMetaAnalysisStudies(smd);
   }
*/
   return idString;
}
      
/**
 * get identification text for volume cell.
 */
QString 
BrainModelIdentification::getIdentificationTextForVolumeCell()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem volumeCellID = openGL->getSelectedVolumeCell();
   BrainSet* brainSet = volumeCellID.getBrainSet();
   const int cellNumber = volumeCellID.getItemIndex1();
   if ((brainSet != NULL) &&
       (cellNumber >= 0)) {
      CellFile* cf = brainSet->getVolumeCellFile();
      if (cf == NULL) {
         return "";
      }
      
      CellData* cell = cf->getCell(cellNumber);
      idString += (tagBoldStart
                   + "Volume Cell"
                   + tagBoldEnd
                   + " "
                   + QString::number(cellNumber)
                   + ": "
                   + cell->getName()
                   + tagNewLine);
      
      idString += (tagIndentation
                   + tagBoldStart
                   + "Class: "
                   + tagBoldEnd
                   + cell->getClassName()
                   + tagNewLine);
                   
      float xyz[3];
      cell->getXYZ(xyz);
      idString += (tagIndentation
                   + "Position: ("
                   + QString::number(xyz[0], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[1], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[2], 'f', significantDigits)
                   + ")"
                   + tagNewLine);
   }
   
   return idString;
}

/**
 * get identification text for volume foci.
 */
QString 
BrainModelIdentification::getIdentificationTextForVolumeFoci()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem volumeFociID = openGL->getSelectedVolumeFoci();
   BrainSet* brainSet = volumeFociID.getBrainSet();
   const int focusNumber = volumeFociID.getItemIndex1();
   if ((brainSet != NULL) &&
       (focusNumber >= 0)) {
      FociProjectionFile* fpf = brainSet->getFociProjectionFile();
      if (fpf == NULL) {
         return "";
      }
      
      CellProjection* focus = fpf->getCellProjection(focusNumber);
      
      idString += getIdentificationTextForSingleFocus(volumeFociID,
                                                      focus,
                                                      fpf,
                                                      true);
/*
      idString += (tagBoldStart
                   + "Volume Focus"
                   + tagBoldEnd
                   + " "
                   + QString::number(focusNumber)
                   + ": "
                   + focus->getName()
                   + "  "
                   + "Class: "
                   + focus->getClassName()
                   + tagNewLine);
      
      float xyz[3];
      focus->getXYZ(xyz);
      idString += (tagIndentation
                   + "Position: ("
                   + QString::number(xyz[0], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[1], 'f', significantDigits)
                   + ", "
                   + QString::number(xyz[2], 'f', significantDigits)
                   + ")"
                   + tagNewLine);
*/
   }
   
   return idString;
}

/**
 * get identification text for transform cell.
 */
QString 
BrainModelIdentification::getIdentificationTextForTransformCell()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem cellID = openGL->getSelectedTransformationCell();
   BrainSet* brainSet = cellID.getBrainSet();
   const int fileNumber = cellID.getItemIndex1();
   const int cellNumber = cellID.getItemIndex2();
   if ((brainSet != NULL) &&
       (fileNumber >= 0) && 
       (cellNumber >= 0)) {
      //
      // Note that a foci file is derived from a cell file
      //
      FociFile* ff = dynamic_cast<FociFile*>(brainSet->getTransformationDataFile(fileNumber));
      CellFile* cf = dynamic_cast<CellFile*>(brainSet->getTransformationDataFile(fileNumber));
      if ((ff == NULL) & (cf != NULL)) {
         CellData* cell = cf->getCell(cellNumber);
         idString += (tagBoldStart
                      + "Transform Cell"
                      + tagBoldEnd
                      + " "
                      + QString::number(cellNumber)
                      + ": "
                      + cell->getName()
                      + tagIndentation
                      + "Class: "
                      + cell->getClassName()
                      + tagNewLine);
         
         float xyz[3];
         cell->getXYZ(xyz);
         idString += (tagIndentation
                      + "Position: ("
                      + QString::number(xyz[0], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[1], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[2], 'f', significantDigits)
                      + ")"
                      + tagNewLine);
      }
   }
   
   return idString;
}

/**
 * get identification text for transform contour cell.
 */
QString 
BrainModelIdentification::getIdentificationTextForTransformContourCell()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem cellID = openGL->getSelectedTransformationContourCell();
   BrainSet* brainSet = cellID.getBrainSet();
   const int fileNumber = cellID.getItemIndex1();
   const int cellNumber = cellID.getItemIndex2();
   if ((brainSet != NULL) &&
       (fileNumber >= 0) && 
       (cellNumber >= 0)) {
      //
      // Note that a foci file is derived from a cell file
      //
      FociFile* ff = dynamic_cast<FociFile*>(brainSet->getTransformationDataFile(fileNumber));
      CellFile* cf = dynamic_cast<CellFile*>(brainSet->getTransformationDataFile(fileNumber));
      if ((ff == NULL) & (cf != NULL)) {
         CellData* cell = cf->getCell(cellNumber);
         idString += (tagBoldStart
                      + "Transform Contour Cell"
                      + tagBoldEnd
                      + " "
                      + QString::number(cellNumber)
                      + ": "
                      + cell->getName()
                      + tagIndentation
                      + "Class: "
                      + cell->getClassName()
                      + tagNewLine);
         
         float xyz[3];
         cell->getXYZ(xyz);
         idString += (tagIndentation
                      + "Position: ("
                      + QString::number(xyz[0], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[1], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[2], 'f', significantDigits)
                      + ")"
                      + tagNewLine);
      }
   }
   
   return idString;
}

/**
 * get identification text for transform foci.
 */
QString 
BrainModelIdentification::getIdentificationTextForTransformFoci()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem focusID = openGL->getSelectedTransformationFoci();
   BrainSet* brainSet = focusID.getBrainSet();
   const int fileNumber = focusID.getItemIndex1();
   const int focusNumber = focusID.getItemIndex2();
   if ((brainSet != NULL) &&
       (fileNumber >= 0) && 
       (focusNumber >= 0)) {
      //
      // Note that a foci file is derived from a cell file
      //
      FociFile* ff = dynamic_cast<FociFile*>(brainSet->getTransformationDataFile(fileNumber));
      if (ff != NULL) {
         CellData* focus = ff->getCell(focusNumber);
         idString += (tagBoldStart
                      + "Transform Focus"
                      + tagBoldEnd
                      + " "
                      + QString::number(focusNumber)
                      + ": "
                      + focus->getName()
                      + tagIndentation
                      + "Class: "
                      + focus->getClassName()
                      + tagNewLine);
         
         float xyz[3];
         focus->getXYZ(xyz);
         idString += (tagIndentation
                      + "Position: ("
                      + QString::number(xyz[0], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[1], 'f', significantDigits)
                      + ", "
                      + QString::number(xyz[2], 'f', significantDigits)
                      + ")"
                      + tagNewLine);
      }
   }
   
   return idString;
}

/**
 * get identification text for palette.
 */
QString 
BrainModelIdentification::getIdentificationTextForPalette(const bool metricFlag)
{
   QString idString;
   
   BrainModelOpenGLSelectedItem paletteID = openGL->getSelectedPaletteShape();
   QString paletteName("Shape Palette");
   if (metricFlag) {
      paletteID = openGL->getSelectedPaletteMetric();
      paletteName = "Metric Palette";
   }
   
   BrainSet* brainSet = paletteID.getBrainSet();
   const int paletteNumber = paletteID.getItemIndex1();
   const int entryIndex = paletteID.getItemIndex2();
   //const int windowNumber = paletteID.getViewingWindowNumber();
   
   PaletteFile* pf = brainSet->getPaletteFile();
   if ((paletteNumber < 0) || 
       (paletteNumber >= pf->getNumberOfPalettes())) {
      return "";
   }
   DisplaySettingsMetric* dsm = brainSet->getDisplaySettingsMetric();
   //MetricFile* mf = brainSet->getMetricFile();
   
   const Palette* palette = pf->getPalette(paletteNumber);
   if (entryIndex >= palette->getNumberOfPaletteEntries()) {
      return "";
   }
   
   PaletteEntry* pe = (PaletteEntry*)palette->getPaletteEntry(entryIndex);
   float value = pe->getValue();
   const int colorIndex = pe->getColorIndex();
   const PaletteColor* pc = pf->getPaletteColor(colorIndex);
   QString name = pc->getName();
   
   const int nextPaletteEntryIndex = entryIndex + 1;
   float valueNext = 0.0;
   if (nextPaletteEntryIndex >= palette->getNumberOfPaletteEntries()) {
      if (palette->getPositiveOnly()) {
            valueNext = 0.0;
         }
         else {
            valueNext = -1.0;
      }
   }
   else {
      PaletteEntry* pe = (PaletteEntry*)palette->getPaletteEntry(nextPaletteEntryIndex);
      valueNext = pe->getValue();
   }

   BrainModelOpenGLSelectedItem nodeID = openGL->getSelectedNode();
   
   float posMinMetric = 0.0, posMaxMetric = 0.0, negMinMetric = 0.0, negMaxMetric = 0.0;
   if (metricFlag) {
      int displayColumn, thresholdColumn;
      dsm->getMetricsForColoringAndPalette(displayColumn,
                                          thresholdColumn,
                                          negMaxMetric,
                                          negMinMetric,
                                          posMinMetric,
                                          posMaxMetric);                                          
   }
   else {
      DisplaySettingsSurfaceShape* dsss = brainSet->getDisplaySettingsSurfaceShape();
      const int col = dsss->getShapeColumnForPaletteAndColorMapping();
      SurfaceShapeFile* ssf = brainSet->getSurfaceShapeFile();
      if (col >= 0) {
         ssf->getColumnColorMappingMinMax(col, negMaxMetric, posMaxMetric);
      }
      else {
         return "";
      }
     //const int col = dsss->getSelectedDisplayColumn(windowNumber);
      //ssf->getColumnColorMappingMinMax(col, negMaxMetric, posMaxMetric);
   }
      
   if (value >= 0.0) {
      value = value * (posMaxMetric - posMinMetric) + posMinMetric;
   }
   else {
      float num = (negMaxMetric - negMinMetric);
      if (num < 0.0) {
         num = -num;
      }
      value = value *  num + negMinMetric;
   }
   
   if (valueNext >= 0.0) {
      valueNext = valueNext * (posMaxMetric - posMinMetric) + posMinMetric;
   }
   else {
      float num = (negMaxMetric - negMinMetric);
      if (num < 0.0) {
         num = -num;
      }
      valueNext = valueNext * num + negMinMetric;
   }
      
   idString += (paletteName
                + ": "
                + name
                + "  ("
                + QString::number(valueNext, 'f', significantDigits)
                + ", "
                + QString::number(value, 'f', significantDigits)
                + ")"
                + tagNewLine);

   return idString;
}

/**
 * get identification text for voxel cloud functional.
 */
QString 
BrainModelIdentification::getIdentificationTextForVoxelCloudFunctional()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem cloudID = openGL->getSelectedVoxelFunctionalCloud();
   BrainSet* brainSet = cloudID.getBrainSet();
   const int i = cloudID.getItemIndex1();
   const int j = cloudID.getItemIndex2();
   const int k = cloudID.getItemIndex3();
   if ((brainSet != NULL) &&
       (i >= 0) && (j >= 0) && (k >= 0)) {
      BrainModelVolume* bmv = brainSet->getBrainModelVolume();
      if (bmv != NULL) {
         VolumeFile* vf = bmv->getSelectedVolumeFunctionalViewFile();
         if (vf != NULL) {
            idString += getVolumeFileIdentificationText(brainSet,
                                                        bmv,
                                                        vf,
                                                        i,
                                                        j,
                                                        k);
         }
      }
   }
   
   return idString;
}

/**
 * get identification text for VTK model.
 */
QString 
BrainModelIdentification::getIdentificationTextForVtkModel()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem vtkID = openGL->getSelectedVtkModel();
   BrainSet* brainSet = vtkID.getBrainSet();
   const int modelNum = vtkID.getItemIndex1();
   const int openGLType = vtkID.getItemIndex2();
   const int itemNum    = vtkID.getItemIndex3();
   if ((brainSet != NULL) &&
       (modelNum >= 0) &&
       (itemNum >= 0)) {
      if ((modelNum >= 0) && (modelNum < brainSet->getNumberOfVtkModelFiles())) {
         const VtkModelFile* vmf = brainSet->getVtkModelFile(modelNum);
         if (openGLType == GL_TRIANGLES) {
            float xyz[3];
            vmf->getTriangleCoordinate(itemNum, xyz);
            
            const TransformationMatrixFile* tmf = brainSet->getTransformationMatrixFile();
            const TransformationMatrix* tm = vmf->getAssociatedTransformationMatrix();
            if (tmf->getMatrixValid(tm)) {
               tm->multiplyPoint(xyz);
            }
            
            const int* tv = vmf->getTriangle(itemNum);
            idString += ("VTK Model "
                         + FileUtilities::basename(vmf->getFileName())
                         + ", Triangle "
                         + QString::number(itemNum)
                         + " ("
                         + QString::number(tv[0])
                         + ", "
                         + QString::number(tv[1])
                         + ", "
                         + QString::number(tv[2])
                         + ") "
                         + ": ("
                         + QString::number(xyz[0], 'f', significantDigits)
                         + ", "
                         + QString::number(xyz[1], 'f', significantDigits)
                         + ", "
                         + QString::number(xyz[2], 'f', significantDigits)
                         + ")"
                         + tagNewLine);
         }
         else if (openGLType == GL_POINTS) {
            const int pointNumber = *vmf->getVertex(itemNum);
            const float* xyz = vmf->getCoordinateFile()->getCoordinate(pointNumber);
            const unsigned char* rgba = vmf->getPointColor(pointNumber);
            idString += ("VTK Model "
                         + FileUtilities::basename(vmf->getFileName())
                         + ", Vertex "
                         + QString::number(itemNum)
                         + ": ("
                         + QString::number(xyz[0], 'f', significantDigits)
                         + ", "
                         + QString::number(xyz[1], 'f', significantDigits)
                         + ", "
                         + QString::number(xyz[2], 'f', significantDigits)
                         + ")  Color ("
                         + QString::number(rgba[0])
                         + ", "
                         + QString::number(rgba[1])
                         + ", "
                         + QString::number(rgba[2])
                         + ", "
                         + QString::number(rgba[3])
                         + ")"
                         + tagNewLine);
         }
      }
   }
   return idString;
}

/**
 * get identification text for Contour.
 */
QString
BrainModelIdentification::getIdentificationTextForContour()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem contourID = openGL->getSelectedContour();
   BrainSet* brainSet = contourID.getBrainSet();
   const int contourNumber = contourID.getItemIndex1();
   
   if ((brainSet != NULL) &&
       (contourNumber >= 0)) {
      BrainModelContours* bmc = brainSet->getBrainModelContours();
      if (bmc != NULL) {
         if (contourNumber >= 0) {
            const ContourFile* cf = bmc->getContourFile();
            if (cf != NULL) {
               const CaretContour* contour = cf->getContour(contourNumber);
               if (contour != NULL) {
                  const int pointNumber = contourID.getItemIndex2();
                  
                  idString += (tagBoldStart
                               + "Contour"
                               + tagBoldEnd
                               + ": "
                               + QString::number(contourNumber)
                               + ", Point: "
                               + QString::number(pointNumber)
                               + tagNewLine);
                               
                  idString += (tagIndentation
                               + "Section: "
                               + QString::number(contour->getSectionNumber())
                               + tagNewLine);
                  
                  float x, y, z;
                  contour->getPointXYZ(pointNumber, x, y, z);
                  idString += (tagIndentation
                               + "Position: ("
                               + QString::number(x, 'f', significantDigits)
                               + ", "
                               + QString::number(y, 'f', significantDigits)
                               + ", "
                               + QString::number(z, 'f', significantDigits)
                               + ")"
                               + tagNewLine);
               }
            }
         }
      }
   }

   return idString;
}

/**
 * get identification text for contour cell.
 */
QString 
BrainModelIdentification::getIdentificationTextForContourCell()
{
   QString idString;
   
   BrainModelOpenGLSelectedItem cellID = openGL->getSelectedContourCell();
   BrainSet* brainSet = cellID.getBrainSet();
   const int cellNumber = cellID.getItemIndex1();
   if ((brainSet != NULL) &&
       (cellNumber >= 0)) {
      ContourCellFile* cf = brainSet->getContourCellFile();
      if (cf != NULL) {
         CellData* cell = cf->getCell(cellNumber);
         
         idString += (tagBoldStart
                      + "Contour Cell"
                      + tagBoldEnd
                      + " "
                      + QString::number(cellNumber)
                      + ": "
                      + cell->getName()
                      + tagIndentation
                      + "Class: "
                      + cell->getClassName()
                      + tagNewLine);
         
         float xyz[3];
         cell->getXYZ(xyz);
         idString += (tagIndentation
                      + "Position: ("
                      + QString::number(xyz[0], 'f', significantDigits) 
                      + ", "
                      + QString::number(xyz[1], 'f', significantDigits) 
                      + ", "
                      + QString::number(xyz[2], 'f', significantDigits) 
                      + ")"
                      + tagNewLine);
      }
   }

   return idString;
}
      
/**
 * get the identification text for vocabulary.
 */
QString 
BrainModelIdentification::getIdentificationTextForVocabulary(const bool enableHtml,
                                                             const QString& vocabularyName)
{
   setupHtmlOrTextTags(enableHtml);

   const VocabularyFile* vf = brainSetParent->getVocabularyFile();
   const bool caseSensitiveFlag = false;
   const VocabularyFile::VocabularyEntry* ve = vf->getBestMatchingVocabularyEntry(vocabularyName,
                                                                                  caseSensitiveFlag);
   if (ve == NULL) {
      return "";
   }
   
   QString idString;
   
   idString += tagBoldStart;
   idString += "Abbreviation";
   idString += tagBoldEnd;
   idString += ": ";
   idString += ve->getAbbreviation();
   idString += tagNewLine;
   
   if (ve->getFullName().isEmpty() == false) {
      idString += tagBoldStart;
      idString += "Full Name";
      idString += tagBoldEnd;
      idString += ": ";
      idString += ve->getFullName();
      idString += tagNewLine;
   } 
   
   if (ve->getClassName().isEmpty() == false) {
      idString += tagBoldStart;
      idString += "Class Name";
      idString += tagBoldEnd;
      idString += ": ";
      idString += ve->getClassName();
      idString += tagNewLine;
   } 
   
   if (ve->getVocabularyID().isEmpty() == false) {
      idString += tagBoldStart;
      idString += "Vocabulary ID";
      idString += tagBoldEnd;
      idString += ": ";
      idString += ve->getVocabularyID();
      idString += tagNewLine;
   } 
   
   if (ve->getOntologySource().isEmpty() == false) {
      idString += tagBoldStart;
      idString += "Ontology Source";
      idString += tagBoldEnd;
      idString += ": ";
      idString += ve->getOntologySource();
      idString += tagNewLine;
   } 
   
   if (ve->getTermID().isEmpty() == false) {
      idString += tagBoldStart;
      idString += "Term ID";
      idString += tagBoldEnd;
      idString += ": ";
      idString += ve->getTermID();
      idString += tagNewLine;
   } 
   
   if (ve->getDescription().isEmpty() == false) {
      idString += tagBoldStart;
      idString += "Description";
      idString += tagBoldEnd;
      idString += ": ";
      idString += ve->getDescription();
      idString += tagNewLine;
   } 
   
   const StudyMetaDataFile* smdf = brainSetParent->getStudyMetaDataFile();
   idString += getIdentificationTextForStudies(enableHtml,
                                               smdf,
                                               ve->getStudyMetaDataLinkSet());
/*
   const int studyIndex = smdf->getStudyIndexFromLink(ve->getStudyMetaDataLink());
   if (studyIndex >= 0) {
      const StudyMetaDataLink smdl = ve->getStudyMetaDataLink();
      idString += getIdentificationTextForStudy(enableHtml,
                                                smdf->getStudyMetaData(studyIndex),
                                                studyIndex,
                                                &smdl);
   }
*/

   if (vf != NULL) {
      const int studyNumber = ve->getStudyNumber();
      if ((studyNumber >= 0) && (studyNumber < vf->getNumberOfStudyInfo())) {
         const CellStudyInfo* csi = vf->getStudyInfo(studyNumber);
         const QString s2 = csi->getFullDescriptionForDisplayToUser(true);
         if (s2.isEmpty() == false) {
            idString += s2;
         }
      }
   }
   
   return idString;
}
                                                 
/**
 * get the identification text for studies.
 */
QString 
BrainModelIdentification::getIdentificationTextForStudies(const bool enableHtml,
                                                          const StudyMetaDataFile* smdf,
                                                          const StudyMetaDataLinkSet& smdls)
{
   setupHtmlOrTextTags(enableHtml);
   
   return getIdentificationTextForStudies(smdf,
                                          smdls,
                                          false);
}
      
/**
 * get the identification text for a study.
 */
QString 
BrainModelIdentification::getIdentificationTextForStudy(const bool enableHtml,
                                                        const StudyMetaData* smd,
                                                        const int studyIndex,
                                                        const StudyMetaDataLink* smdl)
{
   setupHtmlOrTextTags(enableHtml);
   
   return getIdentificationTextForStudy(smd,
                                        studyIndex,
                                        smdl);
}

/**
 * get the identification text for a meta-analysis study.
 */
QString 
BrainModelIdentification::getIdentificationTextForMetaAnalysisStudy(const QString& pubMedID)
{
   const StudyMetaDataFile* smdf = brainSetParent->getStudyMetaDataFile();
   const int indx = smdf->getStudyIndexFromPubMedID(pubMedID);
   QString idString;

   if (indx >= 0) {   
      const StudyMetaData* smd = smdf->getStudyMetaData(indx);
      
      idString += tagIndentation;
      idString += tagBoldStart;
      idString += "Meta-Analysis,  Study Number ";
      idString += QString::number(indx);
      idString += tagBoldEnd;
      idString += ": ";
      idString += tagNewLine;
      
      if (smd->getName().isEmpty() == false) {
         if (getDisplayStudyMetaAnalysisNameInformation()) {
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Meta-Analysis Name";
            idString += tagBoldEnd;
            idString += ": ";
            idString += htmlTranslate(smd->getName());
            idString += tagNewLine;
         }
      } 
      
      if (smd->getTitle().isEmpty() == false) {
         if (getDisplayStudyMetaAnalysisTitleInformation()) {
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Meta-Analysis Title";
            idString += tagBoldEnd;
            idString += ": ";
            idString += htmlTranslate(smd->getTitle());
            idString += tagNewLine;
         }
      }

      if (smd->getAuthors().isEmpty() == false) {
         if (getDisplayStudyMetaAnalysisAuthorsInformation()) {
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Meta-Analysis Authors";
            idString += tagBoldEnd;
            idString += ": ";
            idString += htmlTranslate(smd->getAuthors());
            idString += tagNewLine;
         }
      } 
      
      if (smd->getCitation().isEmpty() == false) {
         if (getDisplayStudyMetaAnalysisCitationInformation()) {
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Meta-Analysis Citation";
            idString += tagBoldEnd;
            idString += ": ";
            idString += htmlTranslate(smd->getCitation());
            idString += tagNewLine;
         }
      } 
      
      if (smd->getDocumentObjectIdentifier().isEmpty() == false) {
         if (getDisplayStudyMetaAnalysisDoiUrlInformation()) {
            idString += tagIndentation;
            QString theURL;
            if (smd->getDocumentObjectIdentifier().startsWith("http:")) {
               theURL = smd->getDocumentObjectIdentifier();
            }
            else {
               theURL = "http://dx.doi.org/" + smd->getDocumentObjectIdentifier();
            }
            idString += tagBoldStart;
            idString += "Meta-Analysis DOI/URL";
            idString += tagBoldEnd;
            idString += ": ";
            idString += StringUtilities::convertURLsToHyperlinks(theURL);
            idString += tagNewLine;
         }
      } 
   }
   return idString;
}
      
/**
 * get the identification text for a meta-analysis studies attached to the input study.
 */
QString 
BrainModelIdentification::getIdentificationTextForMetaAnalysisStudies(const StudyMetaData* smd)
{
   if (getDisplayStudyMetaAnalysisInformation() == false) {
      return "";
   }
   if (idFilter.anyStudyMetaAnalysisDataOn() == false) {
      return "";
   }

   QString idString;
   
   std::vector<QString> otherStudies;
   const StudyMetaDataFile* smdf = smd->getParent();
   if (smdf != NULL) {
      smdf->getMetaAnalysisStudiesForStudy(smd, otherStudies);
      if (otherStudies.empty() == false) {
         for (unsigned int mm = 0; mm < otherStudies.size(); mm++) {
            idString += getIdentificationTextForMetaAnalysisStudy(otherStudies[mm]);
         }
      }
   }
   
   return idString;
}   

/**
 * get the identification text for studies.
 */
QString 
BrainModelIdentification::getIdentificationTextForStudies(const StudyMetaDataFile* smdf,
                                                          const StudyMetaDataLinkSet& smdls,
                                                          const bool showMetaAnalysisFlag)
{
   QString idString;
   
   const int num = smdls.getNumberOfStudyMetaDataLinks();
   for (int i = 0; i < num; i++) {
      const StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(i);
      const int smdIndex = smdf->getStudyIndexFromLink(smdl);
      if ((smdIndex >= 0) &&
          (smdIndex < smdf->getNumberOfStudyMetaData())) {
         const StudyMetaData* smd = smdf->getStudyMetaData(smdIndex);
         if (smd != NULL) {
            idString += getIdentificationTextForStudy(smd, smdIndex, &smdl);
            if (showMetaAnalysisFlag) {
               idString += getIdentificationTextForMetaAnalysisStudies(smd);
            }
         }
      }
   }
   
   return idString;
}
                                              
/**
 * get the identification text for a study.
 */
QString 
BrainModelIdentification::getIdentificationTextForStudy(const StudyMetaData* smd,
                                                        const int studyIndex,
                                                        const StudyMetaDataLink* smdl)
{
   if (getDisplayStudyInformation() == false) {
      return "";
   }
   if (idFilter.anyStudyDataOn() == false) {
      return "";
   }

   QString idString;
   
   //
   // Study Number
   //
   idString += (tagBoldStart
                + "Study Number "
                + tagBoldEnd
                + QString::number(studyIndex + 1)
                + tagNewLine);

   //
   // Do Name first
   //
   if (smd->getName().isEmpty() == false) {
      if (getDisplayStudyNameInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Name";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getName());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getTitle().isEmpty() == false) {
      if (getDisplayStudyTitleInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Title";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getTitle());
         idString += tagNewLine;
      }
   }

   if (smd->getAuthors().isEmpty() == false) {
      if (getDisplayStudyAuthorsInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Authors";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getAuthors());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getCitation().isEmpty() == false) {
      if (getDisplayStudyCitationInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Citation";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getCitation());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getComment().isEmpty() == false) {
      if (getDisplayStudyCommentInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Comment";
         idString += tagBoldEnd;
         idString += ": ";
         idString += StringUtilities::convertURLsToHyperlinks(htmlTranslate(smd->getComment()));
         idString += tagNewLine;
      }
   } 
   
   if (smd->getStudyDataFormat().isEmpty() == false) {
      if (getDisplayStudyDataFormatInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Data Format";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getStudyDataFormat());
         idString += tagNewLine;
      }
   }
   
   if (smd->getStudyDataType().isEmpty() == false) {
      if (getDisplayStudyDataTypeInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Data Type";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getStudyDataType());
         idString += tagNewLine;
      }
   }
   
   if (smd->getDocumentObjectIdentifier().isEmpty() == false) {
      if (getDisplayStudyDOIInformation()) {
         idString += tagIndentation;
         QString theURL;
         if (smd->getDocumentObjectIdentifier().startsWith("http:")) {
            theURL = smd->getDocumentObjectIdentifier();
         }
         else {
            theURL = "http://dx.doi.org/" + smd->getDocumentObjectIdentifier();
         }
         idString += tagBoldStart;
         idString += "Study DOI/URL";
         idString += tagBoldEnd;
         idString += ": ";
         idString += StringUtilities::convertURLsToHyperlinks(theURL);
         idString += tagNewLine;
      }
   } 
   
   if (smd->getKeywords().isEmpty() == false) {
      if (getDisplayStudyKeywordsInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Keywords";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getKeywords());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getMedicalSubjectHeadings().isEmpty() == false) {
      if (getDisplayStudyMedicalSubjectHeadingsInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Medical Subject Headings";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getMedicalSubjectHeadings());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getPartitioningSchemeAbbreviation().isEmpty() == false) {
      if (getDisplayStudyPartSchemeAbbrevInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Partitioning Scheme Abbreviation";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getPartitioningSchemeAbbreviation());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getPartitioningSchemeFullName().isEmpty() == false) {
      if (getDisplayStudyPartSchemeFullInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Partitioning Scheme Full Name";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getPartitioningSchemeFullName());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getPubMedID().isEmpty() == false) {
      if (getDisplayStudyPubMedIDInformation()) {
         if (smd->getPubMedID().startsWith(StudyMetaData::getProjectIDInPubMedIDPrefix())) {
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Study PubMed ID";
            idString += tagBoldEnd;
            idString += ": ";
            idString += smd->getPubMedID();
            idString += tagNewLine;
         }
         else {
            const QString pubURL = "http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=retrieve&db=pubmed&list_uids="
                                 + smd->getPubMedID().trimmed();
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Study PubMed URL";
            idString += tagBoldEnd;
            idString += ": ";
            idString += ("<a href=\"" + pubURL + "\">"
                         + smd->getPubMedID().trimmed()
                         + "</a>");
            //idString += StringUtilities::convertURLsToHyperlinks(pubURL);
            idString += tagNewLine;
         }
      }
   } 
   
   if (smd->getProjectID().isEmpty() == false) {
      if (getDisplayStudyProjectIDInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Project ID";
         idString += tagBoldEnd;
         idString += ": ";
         idString += smd->getProjectID();
         idString += tagNewLine;
      }
   } 
   
   if (smd->getStereotaxicSpace().isEmpty() == false) {
      if (getDisplayStudyStereotaxicSpaceInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Stereotaxic Space";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getStereotaxicSpace());
         idString += tagNewLine;
      }
   } 
   
   if (smd->getStereotaxicSpaceDetails().isEmpty() == false) {
      if (getDisplayStudyStereotaxicSpaceDetailsInformation()) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += "Study Stereotaxic Space Details";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(smd->getStereotaxicSpaceDetails());
         idString += tagNewLine;
      }
   } 
   
   //
   // Is link valid
   //
   if (smdl != NULL) {
      //
      // Display table information?
      //
      if (getDisplayStudyTableInformation() && idFilter.anyStudyTableDataOn()) {
         const QString tableNumber = smdl->getTableNumber();
         const StudyMetaData::Table* table = smd->getTableByTableNumber(tableNumber);
         if (table != NULL) {
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Table Number";
            idString += tagBoldEnd;
            idString += ": ";
            idString += table->getNumber();
            idString += tagNewLine;

            if (table->getFooter().isEmpty() == false) {
               if (getDisplayStudyTableFooterInformation()) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Table Footer";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(table->getFooter());
                  idString += tagNewLine;
               }
            } 
            
            if (table->getHeader().isEmpty() == false) {
               if (getDisplayStudyTableHeaderInformation()) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Table Header";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(table->getHeader());
                  idString += tagNewLine;
               }
            } 
            
            if (table->getSizeUnits().isEmpty() == false) {
               if (getDisplayStudyTableSizeUnitsInformation()) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Table Size Units";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(table->getSizeUnits());
                  idString += tagNewLine;
               }
            } 
            
            if (table->getStatisticType().isEmpty() == false) {
               if (getDisplayStudyTableStatisticInformation()) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Table Statistic Type";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(table->getStatisticType());
                  idString += tagNewLine;
               }
            } 
            
            if (table->getStatisticDescription().isEmpty() == false) {
               if (getDisplayStudyTableStatisticDescriptionInformation()) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Table Statistic Description";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(table->getStatisticDescription());
                  idString += tagNewLine;
               }
            } 
            
            if (table->getVoxelDimensions().isEmpty() == false) {
               if (getDisplayStudyTableVoxelSizeInformation()) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Table Voxel Dimensions";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(table->getVoxelDimensions());
                  idString += tagNewLine;
               }
            } 
            
            //
            // Is link valid?
            //
            if (smdl != NULL) {
               const StudyMetaData::SubHeader* sh = table->getSubHeaderBySubHeaderNumber(smdl->getTableSubHeaderNumber());
               if (sh != NULL) {
                  idString += getStudyMetaDataSubHeaderIdentificationText(sh, "Table");
               }
            }
         }
      }

      //
      // Display figure information?
      //
      if (getDisplayStudyFigureInformation() && idFilter.anyStudyFigureDataOn()) {
         const QString figureNumber = smdl->getFigureNumber();
         const StudyMetaData::Figure* figure = smd->getFigureByFigureNumber(figureNumber);
      
            if (figure != NULL) {
            //
            // Do number first
            //
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Figure Number";
            idString += tagBoldEnd;
            idString += ": ";
            idString += figure->getNumber();
            idString += tagNewLine;

            if (figure->getLegend().isEmpty() == false) {
               if (getDisplayStudyFigureInformation()) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Figure Legend";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(figure->getLegend());
                  idString += tagNewLine;
               }
            } 
            
            if (smdl != NULL) {
               const StudyMetaData::Figure::Panel* panel = figure->getPanelByPanelNumberOrLetter(smdl->getFigurePanelNumberOrLetter());
               if (panel != NULL) {
                  if (idFilter.anyStudyFigurePanelDataOn()) {
                     //
                     // Do number first
                     //
                     idString += tagIndentation;
                     idString += tagBoldStart;
                     idString += "Figure Panel Number/Letter";
                     idString += tagBoldEnd;
                     idString += ": ";
                     idString += QString(panel->getPanelNumberOrLetter());
                     idString += tagNewLine;

                     if (getDisplayStudyFigurePanelDescriptionInformation()) {
                        if (panel->getDescription().isEmpty() == false) {
                           idString += tagIndentation;
                           idString += tagBoldStart;
                           idString += "Figure Panel Description";
                           idString += tagBoldEnd;
                           idString += ": ";
                           idString += htmlTranslate(panel->getDescription());
                           idString += tagNewLine;
                        } 
                     }
                     if (getDisplayStudyFigurePanelTaskDescriptionInformation()) {
                        if (panel->getTaskDescription().isEmpty() == false) {
                           idString += tagIndentation;
                           idString += tagBoldStart;
                           idString += "Figure Panel Task Description";
                           idString += tagBoldEnd;
                           idString += ": ";
                           idString += htmlTranslate(panel->getTaskDescription());
                           idString += tagNewLine;
                        } 
                     }
                     if (getDisplayStudyFigurePanelTaskBaselineInformation()) {
                        if (panel->getTaskBaseline().isEmpty() == false) {
                           idString += tagIndentation;
                           idString += tagBoldStart;
                           idString += "Figure Panel Task Baseline";
                           idString += tagBoldEnd;
                           idString += ": ";
                           idString += htmlTranslate(panel->getTaskBaseline());
                           idString += tagNewLine;
                        } 
                     }
                     if (getDisplayStudyFigurePanelTestAttributesInformation()) {
                        if (panel->getTestAttributes().isEmpty() == false) {
                           idString += tagIndentation;
                           idString += tagBoldStart;
                           idString += "Figure Panel Test Attributes";
                           idString += tagBoldEnd;
                           idString += ": ";
                           idString += htmlTranslate(panel->getTestAttributes());
                           idString += tagNewLine;
                        } 
                     }
                  }
               }
            }
         }
      }
      
      //
      // Display Page Reference information?
      //
      if (idFilter.anyPageReferenceDataOn()) {
         const QString pageRefPageNumber = smdl->getPageReferencePageNumber();
         const StudyMetaData::PageReference* pageRef = smd->getPageReferenceByPageNumber(pageRefPageNumber);
         if (pageRef != NULL) {
            //
            // Do number first
            //
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Page Reference";
            idString += tagBoldEnd;
            idString += ": ";
            idString += pageRef->getPageNumber();
            idString += tagNewLine;
            
            if (getDisplayStudyPageReferenceCommentInformation()) {
               if (pageRef->getComment().isEmpty() == false) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Page Reference Comment";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(pageRef->getComment());
                  idString += tagNewLine;
               } 
            }
            if (getDisplayStudyPageReferenceHeaderInformation()) {
               if (pageRef->getHeader().isEmpty() == false) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Page Reference Header";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(pageRef->getHeader());
                  idString += tagNewLine;
               } 
            }
            if (getDisplayStudyPageReferenceSizeUnitsInformation()) {
               if (pageRef->getSizeUnits().isEmpty() == false) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Page Reference Size Units";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(pageRef->getSizeUnits());
                  idString += tagNewLine;
               } 
            }
            if (getDisplayStudyPageReferenceStatisticInformation()) {
               if (pageRef->getStatisticType().isEmpty() == false) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "PageReference Statistic Type";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(pageRef->getStatisticType());
                  idString += tagNewLine;
               } 
            }
            if (getDisplayStudyPageReferenceStatisticDescriptionInformation()) {
               if (pageRef->getStatisticDescription().isEmpty() == false) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Page Reference Statistic Description";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(pageRef->getStatisticDescription());
                  idString += tagNewLine;
               } 
            }
            if (getDisplayStudyPageReferenceVoxelSizeInformation()) {
               if (pageRef->getVoxelDimensions().isEmpty() == false) {
                  idString += tagIndentation;
                  idString += tagBoldStart;
                  idString += "Page Reference Voxel Dimensions";
                  idString += tagBoldEnd;
                  idString += ": ";
                  idString += htmlTranslate(pageRef->getVoxelDimensions());
                  idString += tagNewLine;
               }  
            }
                  
            //
            // Is link valid?
            //
            if (smdl != NULL) {
               const StudyMetaData::SubHeader* sh = pageRef->getSubHeaderBySubHeaderNumber(smdl->getPageReferenceSubHeaderNumber());
               if (sh != NULL) {
                  idString += getStudyMetaDataSubHeaderIdentificationText(sh, "Page Reference");
               }
            }
         }
      }

      if (smdl->getPageNumber().isEmpty() == false) {
         if (getDisplayStudyPageNumberInformation()) {
            idString += tagIndentation;
            idString += tagBoldStart;
            idString += "Page Number";
            idString += tagBoldEnd;
            idString += ": ";
            idString += smdl->getPageNumber();
            idString += tagNewLine;
         }
      }
   }
   
   return idString;
}
      
/**
 * Get ID string for study meta data sub header
 */
QString
BrainModelIdentification::getStudyMetaDataSubHeaderIdentificationText(
                                     const StudyMetaData::SubHeader* sh,
                                     const QString& namePrefix) const
{
   if (idFilter.anySubHeaderDataOn() == false) {
      return "";
   }
   
   QString idString;
   
   idString += tagIndentation;
   idString += tagBoldStart;
   idString += namePrefix;
   idString += " Sub Header Number";
   idString += tagBoldEnd;
   idString += ": ";
   idString += sh->getNumber();
   idString += tagNewLine;

   if (getDisplayStudySubHeaderNameInformation()) {
      if (sh->getName().isEmpty() == false) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += namePrefix;
         idString += " Sub Header Name";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(sh->getName());
         idString += tagNewLine;
      } 
   }
   
   if (getDisplayStudySubHeaderShortNameInformation()) {
      if (sh->getShortName().isEmpty() == false) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += namePrefix;
         idString += " Sub Header Short Name";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(sh->getShortName());
         idString += tagNewLine;
      } 
   }
   
   if (getDisplayStudySubHeaderTaskDescriptionInformation()) {
      if (sh->getTaskDescription().isEmpty() == false) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += namePrefix;
         idString += " Sub Header Task Description";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(sh->getTaskDescription());
         idString += tagNewLine;
      } 
   }
   
   if (getDisplayStudySubHeaderTaskBaselineInformation()) {
      if (sh->getTaskBaseline().isEmpty() == false) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += namePrefix;
         idString += " Sub Header Task Baseline";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(sh->getTaskBaseline());
         idString += tagNewLine;
      } 
   }
   
   if (getDisplayStudySubHeaderTestAttributesInformation()) {
      if (sh->getTestAttributes().isEmpty() == false) {
         idString += tagIndentation;
         idString += tagBoldStart;
         idString += namePrefix;
         idString += " Sub Header Test Attributes";
         idString += tagBoldEnd;
         idString += ": ";
         idString += htmlTranslate(sh->getTestAttributes());
         idString += tagNewLine;
      }
   }
   
   return idString;
}

/**
 * apply a scene (update dialog).
 */
void 
BrainModelIdentification::showScene(const SceneFile::Scene& scene,
                                    QString& /*errorMessage*/)
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "BrainModelIdentification") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName(si->getName());
            if (infoName == "displayBorderInformation") {
               setDisplayBorderInformation(si->getValueAsBool());
            }
            else if (infoName == "displayCellInformation") {
               setDisplayCellInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociInformation") {
               setDisplayFociInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociNameInformation") {
               setDisplayFociNameInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociClassInformation") {
               setDisplayFociClassInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociOriginalStereotaxicPositionInformation") {
               setDisplayFociOriginalStereotaxicPositionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociStereotaxicPositionInformation") {
               setDisplayFociStereotaxicPositionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociAreaInformation") {
               setDisplayFociAreaInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociGeographyInformation") {
               setDisplayFociGeographyInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociRegionOfInterestInformation") {
               setDisplayFociRegionOfInterestInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociSizeInformation") {
               setDisplayFociSizeInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociStructureInformation") {
               setDisplayFociStructureInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociStatisticInformation") {
               setDisplayFociStatisticInformation(si->getValueAsBool());
            }
            else if (infoName == "displayFociCommentInformation") {
               setDisplayFociCommentInformation(si->getValueAsBool());
            }
            else if (infoName == "displayVoxelInformation") {
               setDisplayVoxelInformation(si->getValueAsBool());
            }
            else if (infoName == "displayContourInformation") {
               setDisplayContourInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeInformation") {
               setDisplayNodeInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeCoordInformation") {
               setDisplayNodeCoordInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeLatLonInformation") {
               setDisplayNodeLatLonInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodePaintInformation") {
               setDisplayNodePaintInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeProbAtlasInformation") {
               setDisplayNodeProbAtlasInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeRgbPaintInformation") {
               setDisplayNodeRgbPaintInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeMetricInformation") {
               setDisplayNodeMetricInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeShapeInformation") {
               setDisplayNodeShapeInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeSectionInformation") {
               setDisplayNodeSectionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeArealEstInformation") {
               setDisplayNodeArealEstInformation(si->getValueAsBool());
            }
            else if (infoName == "displayNodeTopographyInformation") {
               setDisplayNodeTopographyInformation(si->getValueAsBool());
            }
            else if (infoName == "displayIDSymbol") {
               setDisplayIDSymbol(si->getValueAsBool());
            }
            else if (infoName == "displayStudyInformation") {
               setDisplayStudyInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTitleInformation") {
               setDisplayStudyTitleInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyAuthorsInformation") {
               setDisplayStudyAuthorsInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyCitationInformation") {
               setDisplayStudyCitationInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyCommentInformation") {
               setDisplayStudyCommentInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyDataFormatInformation") {
               setDisplayStudyDataFormatInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyDataTypeInformation") {
               setDisplayStudyDataTypeInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyDOIInformation") {
               setDisplayStudyDOIInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyKeywordsInformation") {
               setDisplayStudyKeywordsInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyMedicalSubjectHeadingsInformation") {
               setDisplayStudyMedicalSubjectHeadingsInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyMetaAnalysisInformation") {
               setDisplayStudyMetaAnalysisInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyMetaAnalysisNameInformation") {
               setDisplayStudyMetaAnalysisNameInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyMetaAnalysisTitleInformation") {
               setDisplayStudyMetaAnalysisTitleInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyMetaAnalysisAuthorsInformation") {
               setDisplayStudyMetaAnalysisAuthorsInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyMetaAnalysisCitationInformation") {
               setDisplayStudyMetaAnalysisCitationInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyMetaAnalysisDoiUrlInformation") {
               setDisplayStudyMetaAnalysisDoiUrlInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyNameInformation") {
               setDisplayStudyNameInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPartSchemeAbbrevInformation") {
               setDisplayStudyPartSchemeAbbrevInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPartSchemeFullInformation") {
               setDisplayStudyPartSchemeFullInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPubMedIDInformation") {
               setDisplayStudyPubMedIDInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyProjectIDInformation") {
               setDisplayStudyProjectIDInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyStereotaxicSpaceInformation") {
               setDisplayStudyStereotaxicSpaceInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyStereotaxicSpaceDetailsInformation") {
               setDisplayStudyStereotaxicSpaceDetailsInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyURLInformation") {
               setDisplayStudyURLInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTableInformation") {
               setDisplayStudyTableInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTableHeaderInformation") {
               setDisplayStudyTableHeaderInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTableFooterInformation") {
               setDisplayStudyTableFooterInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTableSizeUnitsInformation") {
               setDisplayStudyTableSizeUnitsInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTableVoxelSizeInformation") {
               setDisplayStudyTableVoxelSizeInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTableStatisticInformation") {
               setDisplayStudyTableStatisticInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyTableStatisticDescriptionInformation") {
               setDisplayStudyTableStatisticDescriptionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyFigureInformation") {
               setDisplayStudyFigureInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyFigureLegendInformation") {
               setDisplayStudyFigureLegendInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyFigurePanelInformation") {
               setDisplayStudyFigurePanelInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyFigurePanelDescriptionInformation") {
               setDisplayStudyFigurePanelDescriptionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyFigurePanelTaskDescriptionInformation") {
               setDisplayStudyFigurePanelTaskDescriptionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyFigurePanelTaskBaselineInformation") {
               setDisplayStudyFigurePanelTaskBaselineInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyFigurePanelTestAttributesInformation") {
               setDisplayStudyFigurePanelTestAttributesInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudySubHeaderInformation") {
               setDisplayStudySubHeaderInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudySubHeaderNameInformation") {
               setDisplayStudySubHeaderNameInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudySubHeaderShortNameInformation") {
               setDisplayStudySubHeaderShortNameInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudySubHeaderTaskDescriptionInformation") {
               setDisplayStudySubHeaderTaskDescriptionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudySubHeaderTaskBaselineInformation") {
               setDisplayStudySubHeaderTaskBaselineInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudySubHeaderTestAttributesInformation") {
               setDisplayStudySubHeaderTestAttributesInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageReferenceInformation") {
               setDisplayStudyPageReferenceInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageReferenceHeaderInformation") {
               setDisplayStudyPageReferenceHeaderInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageReferenceCommentInformation") {
               setDisplayStudyPageReferenceCommentInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageReferenceSizeUnitsInformation") {
               setDisplayStudyPageReferenceSizeUnitsInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageReferenceVoxelSizeInformation") {
               setDisplayStudyPageReferenceVoxelSizeInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageReferenceStatisticInformation") {
               setDisplayStudyPageReferenceStatisticInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageReferenceStatisticDescriptionInformation") {
               setDisplayStudyPageReferenceStatisticDescriptionInformation(si->getValueAsBool());
            }
            else if (infoName == "displayStudyPageNumberInformation") {
               setDisplayStudyPageNumberInformation(si->getValueAsBool());
            }
         }
      }      
      else if (sc->getName() == "GuiIdentifyMainWindow") { // used to be in Identify Dialog
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();
            if (infoName == "borderButton") {
               setDisplayBorderInformation(si->getValueAsBool());
            }
            else if (infoName == "cellButton") {
               setDisplayCellInformation(si->getValueAsBool());
            }
            else if (infoName == "fociButton") {
               setDisplayFociInformation(si->getValueAsBool());
            }
            else if (infoName == "voxelButton") {
               setDisplayVoxelInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeCoordButton") {
               setDisplayNodeCoordInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeLatLonButton") {
               setDisplayNodeLatLonInformation(si->getValueAsBool());
            }
            else if (infoName == "nodePaintButton") {
               setDisplayNodePaintInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeProbAtlasButton") {
               setDisplayNodeProbAtlasInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeRgbPaintButton") {
               setDisplayNodeRgbPaintInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeMetricButton") {
               setDisplayNodeMetricInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeShapeButton") {
               setDisplayNodeShapeInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeSectionButton") {
               setDisplayNodeSectionInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeArealEstButton") {
               setDisplayNodeArealEstInformation(si->getValueAsBool());
            }
            else if (infoName == "nodeTopographyButton") {
               setDisplayNodeTopographyInformation(si->getValueAsBool());
            }
            else if (infoName == "showIDButton") {
               setDisplayIDSymbol(si->getValueAsBool());
            }
            else if (infoName == "significantDigitsSpinBox") {
               setSignificantDigits(si->getValueAsInt());
            }
         }
      }
   }
}

/**
 * get significant digits for floating point numbers.
 */
void 
BrainModelIdentification::setSignificantDigits(const int num) 
{ 
   significantDigits = num;
   PreferencesFile* pf = brainSetParent->getPreferencesFile();
   if (significantDigits != pf->getSignificantDigitsDisplay()) {
      pf->setSignificantDigitsDisplay(significantDigits);
      try {
         pf->writeFile(pf->getFileName());
      }
      catch (FileException& e) {
      }
   }
}

/**
 * create a scene (save dialog settings)
 */
void 
BrainModelIdentification::saveScene(SceneFile::Scene& scene)
{
   SceneFile::SceneClass sc("BrainModelIdentification");
   sc.addSceneInfo(SceneFile::SceneInfo("displayBorderInformation", getDisplayBorderInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayCellInformation", getDisplayCellInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociInformation", getDisplayFociInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociNameInformation", getDisplayFociNameInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociClassInformation", getDisplayFociClassInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociOriginalStereotaxicPositionInformation", getDisplayFociOriginalStereotaxicPositionInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociStereotaxicPositionInformation", getDisplayFociStereotaxicPositionInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociAreaInformation", getDisplayFociAreaInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociRegionOfInterestInformation", getDisplayFociRegionOfInterestInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociGeographyInformation", getDisplayFociGeographyInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociSizeInformation", getDisplayFociSizeInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociStructureInformation", getDisplayFociStructureInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociStatisticInformation", getDisplayFociStatisticInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayFociCommentInformation", getDisplayFociCommentInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayVoxelInformation", getDisplayVoxelInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayContourInformation", getDisplayContourInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeInformation", getDisplayNodeInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeCoordInformation", getDisplayNodeCoordInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeLatLonInformation", getDisplayNodeLatLonInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodePaintInformation", getDisplayNodePaintInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeProbAtlasInformation", getDisplayNodeProbAtlasInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeRgbPaintInformation", getDisplayNodeRgbPaintInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeMetricInformation", getDisplayNodeMetricInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeShapeInformation", getDisplayNodeShapeInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeSectionInformation", getDisplayNodeSectionInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeArealEstInformation", getDisplayNodeArealEstInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayNodeTopographyInformation", getDisplayNodeTopographyInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayIDSymbol", displayIDSymbol));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyInformation", getDisplayStudyInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTitleInformation", getDisplayStudyTitleInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyAuthorsInformation", getDisplayStudyAuthorsInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyCitationInformation", getDisplayStudyCitationInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyCommentInformation", getDisplayStudyCommentInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyDataFormatInformation", getDisplayStudyDataFormatInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyDataTypeInformation", getDisplayStudyDataTypeInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyDOIInformation", getDisplayStudyDOIInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyKeywordsInformation", getDisplayStudyKeywordsInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyMedicalSubjectHeadingsInformation", getDisplayStudyMedicalSubjectHeadingsInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyMetaAnalysisInformation", getDisplayStudyMetaAnalysisInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyMetaAnalysisNameInformation", getDisplayStudyMetaAnalysisNameInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyMetaAnalysisTitleInformation", getDisplayStudyMetaAnalysisTitleInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyMetaAnalysisAuthorsInformation", getDisplayStudyMetaAnalysisAuthorsInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyMetaAnalysisCitationInformation", getDisplayStudyMetaAnalysisCitationInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyMetaAnalysisDoiUrlInformation", getDisplayStudyMetaAnalysisDoiUrlInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyNameInformation", getDisplayStudyNameInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPartSchemeAbbrevInformation", getDisplayStudyPartSchemeAbbrevInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPartSchemeFullInformation", getDisplayStudyPartSchemeFullInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPubMedIDInformation", getDisplayStudyPubMedIDInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyProjectIDInformation", getDisplayStudyProjectIDInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyStereotaxicSpaceInformation", getDisplayStudyStereotaxicSpaceInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyStereotaxicSpaceDetailsInformation", getDisplayStudyStereotaxicSpaceDetailsInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyURLInformation", getDisplayStudyURLInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTableInformation", getDisplayStudyTableInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTableHeaderInformation", getDisplayStudyTableHeaderInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTableFooterInformation", getDisplayStudyTableFooterInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTableSizeUnitsInformation", getDisplayStudyTableSizeUnitsInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTableVoxelSizeInformation", getDisplayStudyTableVoxelSizeInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTableStatisticInformation", getDisplayStudyTableStatisticInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyTableStatisticDescriptionInformation", getDisplayStudyTableStatisticDescriptionInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyFigureInformation", getDisplayStudyFigureInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyFigureLegendInformation", getDisplayStudyFigureLegendInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyFigurePanelInformation", getDisplayStudyFigurePanelInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyFigurePanelDescriptionInformation", getDisplayStudyFigurePanelDescriptionInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyFigurePanelTaskDescriptionInformation", getDisplayStudyFigurePanelTaskDescriptionInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyFigurePanelTaskBaselineInformation", getDisplayStudyFigurePanelTaskBaselineInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyFigurePanelTestAttributesInformation", getDisplayStudyFigurePanelTestAttributesInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageReferenceInformation", getDisplayStudyPageReferenceInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageReferenceHeaderInformation", getDisplayStudyPageReferenceHeaderInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageReferenceCommentInformation", getDisplayStudyPageReferenceCommentInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageReferenceSizeUnitsInformation", getDisplayStudyPageReferenceSizeUnitsInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageReferenceVoxelSizeInformation", getDisplayStudyPageReferenceVoxelSizeInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageReferenceStatisticInformation", getDisplayStudyPageReferenceStatisticInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageReferenceStatisticDescriptionInformation", getDisplayStudyPageReferenceStatisticDescriptionInformation()));
   sc.addSceneInfo(SceneFile::SceneInfo("displayStudyPageNumberInformation", getDisplayStudyPageNumberInformation()));
   scene.addSceneClass(sc);
}

/**
 * all identification off (if any on, all turned off, otherwise all off) .
 */
void 
BrainModelIdentification::toggleAllIdentificationOnOff()
{
   idFilter.toggleAllOnOff();
}
      
/**
 * all identification on.
 */
void 
BrainModelIdentification::setAllIdentificationOn()
{
   idFilter.allOn();
}

/**
 * all identification off.
 */
void 
BrainModelIdentification::setAllIdentificationOff()
{
   idFilter.allOff();
}

/** 
 * make name a link to vocabulary file if it matches a vocabulary file entry.
 */
QString 
BrainModelIdentification::linkToVocabulary(BrainSet* brainSet,
                                           const QString& name)
{
   if (enableVocabularyLinks) {
      VocabularyFile* vf = brainSet->getVocabularyFile();
      VocabularyFile::VocabularyEntry* ve = vf->getBestMatchingVocabularyEntry(name);
      if (ve != NULL) {
         QString link = ("<A HREF=\"vocabulary://"
                         + name
                         + "\">"
                         + name
                         + "</A>");
         return link;
      }
   }
   
   return name;
}

/**
 * make a list of names separated by semicolon possible links to vocabulary.
 */
QString 
BrainModelIdentification::linkStringToVocabulary(BrainSet* brainSet,
                                                 const QString& s)
{
   QString stringOut;
   
   const QStringList sl = s.split(';', QString::SkipEmptyParts);
   for (int i = 0; i < sl.count(); i++) {
      if (i > 0) {
         stringOut += "; ";
      }
      stringOut += linkToVocabulary(brainSet, sl.at(i).trimmed());
   }
   
   return stringOut;
}                                     

//==================================================================================

/**
 * constructor.
 */
BrainModelIdentification::IdFilter::IdFilter()
{
   allOn();
}

/**
 * destructor.
 */
BrainModelIdentification::IdFilter::~IdFilter()
{
}

/**
 * turn all off.
 */
void 
BrainModelIdentification::IdFilter::allOff(const bool turnSubFlagsOff)
{
   displayBorderInformation = false;
   displayCellInformation = false;
   displayFociInformation = false;
   if (turnSubFlagsOff) {
      displayFociNameInformation = false;
      displayFociClassInformation = false;
      displayFociOriginalStereotaxicPositionInformation = false;
      displayFociStereotaxicPositionInformation = false;
      displayFociAreaInformation = false;
      displayFociGeographyInformation = false;
      displayFociRegionOfInterestInformation = false;
      displayFociSizeInformation = false;
      displayFociStructureInformation = false;
      displayFociStatisticInformation = false;
      displayFociCommentInformation = false;
   }
   displayVoxelInformation = false;
   displayContourInformation = false;
   displayNodeInformation = false;
   if (turnSubFlagsOff) {
      displayNodeCoordInformation = false;
      displayNodeLatLonInformation = false;
      displayNodePaintInformation = false;
      displayNodeProbAtlasInformation = false;
      displayNodeRgbPaintInformation = false;
      displayNodeMetricInformation = false;
      displayNodeShapeInformation = false;
      displayNodeSectionInformation = false;
      displayNodeArealEstInformation = false;
      displayNodeTopographyInformation = false;
   }
   if (turnSubFlagsOff) {
      displayStudyInformation = false;
      displayStudyTitleInformation = false;
      displayStudyAuthorsInformation = false;
      displayStudyCitationInformation = false;
      displayStudyCommentInformation = false;
      displayStudyDataFormatInformation = false;
      displayStudyDataTypeInformation = false;
      displayStudyDOIInformation = false;
      displayStudyKeywordsInformation = false;
      displayStudyMedicalSubjectHeadingsInformation = false;
      displayStudyMetaAnalysisInformation = false;
      displayStudyMetaAnalysisNameInformation = false;
      displayStudyMetaAnalysisTitleInformation = false;
      displayStudyMetaAnalysisAuthorsInformation = false;
      displayStudyMetaAnalysisCitationInformation = false;
      displayStudyMetaAnalysisDoiUrlInformation = false;
      displayStudyNameInformation = false;
      displayStudyPartSchemeAbbrevInformation = false;
      displayStudyPartSchemeFullInformation = false;
      displayStudyPubMedIDInformation = false;
      displayStudyProjectIDInformation = false;
      displayStudyStereotaxicSpaceInformation = false;
      displayStudyStereotaxicSpaceDetailsInformation = false;
      displayStudyURLInformation = false;
      displayStudyTableInformation = false;
      displayStudyTableHeaderInformation = false;
      displayStudyTableFooterInformation = false;
      displayStudyTableSizeUnitsInformation = false;
      displayStudyTableVoxelSizeInformation = false;
      displayStudyTableStatisticInformation = false;
      displayStudyTableStatisticDescriptionInformation = false;
      displayStudyFigureInformation = false;
      displayStudyFigureLegendInformation = false;
      displayStudyFigurePanelInformation = false;
      displayStudyFigurePanelDescriptionInformation = false;
      displayStudyFigurePanelTaskDescriptionInformation = false;
      displayStudyFigurePanelTaskBaselineInformation = false;
      displayStudyFigurePanelTestAttributesInformation = false;
      displayStudySubHeaderInformation = false;
      displayStudySubHeaderNameInformation = false;
      displayStudySubHeaderShortNameInformation = false;
      displayStudySubHeaderTaskDescriptionInformation = false;
      displayStudySubHeaderTaskBaselineInformation = false;
      displayStudySubHeaderTestAttributesInformation = false;
      displayStudyPageReferenceInformation = false;
      displayStudyPageReferenceHeaderInformation = false;
      displayStudyPageReferenceCommentInformation = false;
      displayStudyPageReferenceSizeUnitsInformation = false;
      displayStudyPageReferenceVoxelSizeInformation = false;
      displayStudyPageReferenceStatisticInformation = false;
      displayStudyPageReferenceStatisticDescriptionInformation = false;
      displayStudyPageNumberInformation = false;
   }
}

/**
 * turn all on.
 */
void 
BrainModelIdentification::IdFilter::allOn()
{
   displayBorderInformation = true;
   displayCellInformation = true;
   displayFociInformation = true;
   displayFociNameInformation = true;
   displayFociClassInformation = true;
   displayFociOriginalStereotaxicPositionInformation = true;
   displayFociStereotaxicPositionInformation = true;
   displayFociAreaInformation = true;
   displayFociGeographyInformation = true;
   displayFociRegionOfInterestInformation = true;
   displayFociSizeInformation = true;
   displayFociStructureInformation = true;
   displayFociStatisticInformation = true;
   displayFociCommentInformation = true;
   displayVoxelInformation = true;
   displayContourInformation = true;
   displayNodeInformation = true;
   displayNodeCoordInformation = true;
   displayNodeLatLonInformation = true;
   displayNodePaintInformation = true;
   displayNodeProbAtlasInformation = true;
   displayNodeRgbPaintInformation = true;
   displayNodeMetricInformation = true;
   displayNodeShapeInformation = true;
   displayNodeSectionInformation = true;
   displayNodeArealEstInformation = true;
   displayNodeTopographyInformation = true;
   displayStudyInformation = true;
   displayStudyTitleInformation = true;
   displayStudyAuthorsInformation = true;
   displayStudyCitationInformation = true;
   displayStudyCommentInformation = true;
   displayStudyDataFormatInformation = true;
   displayStudyDataTypeInformation = true;
   displayStudyDOIInformation = true;
   displayStudyKeywordsInformation = true;
   displayStudyMedicalSubjectHeadingsInformation = true;
   displayStudyMetaAnalysisInformation = true;
   displayStudyMetaAnalysisNameInformation = true;
   displayStudyMetaAnalysisTitleInformation = true;
   displayStudyMetaAnalysisAuthorsInformation = true;
   displayStudyMetaAnalysisCitationInformation = true;
   displayStudyMetaAnalysisDoiUrlInformation = true;
   displayStudyNameInformation = true;
   displayStudyPartSchemeAbbrevInformation = true;
   displayStudyPartSchemeFullInformation = true;
   displayStudyPubMedIDInformation = true;
   displayStudyProjectIDInformation = true;
   displayStudyStereotaxicSpaceInformation = true;
   displayStudyStereotaxicSpaceDetailsInformation = true;
   displayStudyURLInformation = true;
   displayStudyTableInformation = true;
   displayStudyTableHeaderInformation = true;
   displayStudyTableFooterInformation = true;
   displayStudyTableSizeUnitsInformation = true;
   displayStudyTableVoxelSizeInformation = true;
   displayStudyTableStatisticInformation = true;
   displayStudyTableStatisticDescriptionInformation = true;
   displayStudyFigureInformation = true;
   displayStudyFigureLegendInformation = true;
   displayStudyFigurePanelInformation = true;
   displayStudyFigurePanelDescriptionInformation = true;
   displayStudyFigurePanelTaskDescriptionInformation = true;
   displayStudyFigurePanelTaskBaselineInformation = true;
   displayStudyFigurePanelTestAttributesInformation = true;
   displayStudySubHeaderInformation = true;
   displayStudySubHeaderNameInformation = true;
   displayStudySubHeaderShortNameInformation = true;
   displayStudySubHeaderTaskDescriptionInformation = true;
   displayStudySubHeaderTaskBaselineInformation = true;
   displayStudySubHeaderTestAttributesInformation = true;
   displayStudyPageReferenceInformation = true;
   displayStudyPageReferenceHeaderInformation = true;
   displayStudyPageReferenceCommentInformation = true;
   displayStudyPageReferenceSizeUnitsInformation = true;
   displayStudyPageReferenceVoxelSizeInformation = true;
   displayStudyPageReferenceStatisticInformation = true;
   displayStudyPageReferenceStatisticDescriptionInformation = true;
   displayStudyPageNumberInformation = true;
}

/**
 * toggle all on/off.
 */
void 
BrainModelIdentification::IdFilter::toggleAllOnOff()
{
   const bool anyOn = 
      displayBorderInformation ||
      displayCellInformation ||
      displayFociInformation ||
      anyFociDataOn() ||
/*
      displayFociNameInformation ||
      displayFociClassInformation ||
      displayFociOriginalStereotaxicPositionInformation ||
      displayFociStereotaxicPositionInformation ||
      displayFociAreaInformation ||
      displayFociGeographyInformation ||
      displayFociRegionOfInterestInformation ||
      displayFociSizeInformation ||
      displayFociStructureInformation ||
      displayFociStatisticInformation ||
      displayFociCommentInformation ||
*/
      displayVoxelInformation ||
      displayContourInformation ||
      displayNodeInformation ||
      anyNodeDataOn() ||
/*
      displayNodeCoordInformation ||
      displayNodeLatLonInformation ||
      displayNodePaintInformation ||
      displayNodeProbAtlasInformation ||
      displayNodeRgbPaintInformation ||
      displayNodeMetricInformation ||
      displayNodeShapeInformation ||
      displayNodeSectionInformation ||
      displayNodeArealEstInformation ||
      displayNodeTopographyInformation ||
*/
      displayStudyInformation ||
      anyStudyDataOn();

   if (anyOn) {
      allOff();
   }
   else {
      allOn();
   }
}

/**
 * report if any node information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anyNodeDataOn() const
{
   const bool anyOn =
       displayNodeCoordInformation ||
       displayNodeLatLonInformation ||
       displayNodePaintInformation ||
       displayNodeProbAtlasInformation ||
       displayNodeRgbPaintInformation ||
       displayNodeMetricInformation ||
       displayNodeShapeInformation ||
       displayNodeSectionInformation ||
       displayNodeArealEstInformation ||
       displayNodeTopographyInformation;
   return anyOn;
}

/**
 * report if any foci information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anyFociDataOn() const
{
   const bool anyOn =
       displayFociNameInformation ||
       displayFociClassInformation ||
       displayFociOriginalStereotaxicPositionInformation ||
       displayFociStereotaxicPositionInformation ||
       displayFociAreaInformation ||
       displayFociGeographyInformation ||
       displayFociRegionOfInterestInformation ||
       displayFociSizeInformation ||
       displayFociStatisticInformation ||
       displayFociStructureInformation ||
       displayFociCommentInformation;
   return anyOn;
}

/**
 * report if any study meta-analysis information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anyStudyMetaAnalysisDataOn() const
{
   const bool anyOn = 
      displayStudyMetaAnalysisNameInformation ||
      displayStudyMetaAnalysisTitleInformation ||
      displayStudyMetaAnalysisAuthorsInformation ||
      displayStudyMetaAnalysisCitationInformation ||
      displayStudyMetaAnalysisDoiUrlInformation;
   return anyOn;
}
            
/**
 * report if any study information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anyStudyDataOn() const
{
   const bool anyOn = 
      displayStudyTitleInformation ||
      displayStudyAuthorsInformation ||
      displayStudyCitationInformation ||
      displayStudyCommentInformation ||
      displayStudyDataFormatInformation ||
      displayStudyDataTypeInformation ||
      displayStudyDOIInformation ||
      displayStudyKeywordsInformation ||
      displayStudyMedicalSubjectHeadingsInformation ||
      displayStudyNameInformation ||
      displayStudyPartSchemeAbbrevInformation ||
      displayStudyPartSchemeFullInformation ||
      displayStudyPubMedIDInformation ||
      displayStudyProjectIDInformation ||
      displayStudyStereotaxicSpaceInformation ||
      displayStudyStereotaxicSpaceDetailsInformation ||
      displayStudyURLInformation ||
      anyStudyTableDataOn() ||
      anyStudyFigureDataOn() ||
      anySubHeaderDataOn() ||
      anyPageReferenceDataOn() ||
      displayStudyPageNumberInformation;
      
   return anyOn;
}            

/**
 * report if any study table information should be displayed
 */
bool
BrainModelIdentification::IdFilter::anyStudyTableDataOn() const
{
   const bool anyOn = 
      (displayStudyTableInformation &&
         (displayStudyTableHeaderInformation ||
          displayStudyTableFooterInformation ||
          displayStudyTableSizeUnitsInformation ||
          displayStudyTableVoxelSizeInformation ||
          displayStudyTableStatisticInformation ||
          displayStudyTableStatisticDescriptionInformation));
      
   return anyOn;
}

/**
 * report if any study figure information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anyStudyFigureDataOn() const
{
   const bool anyOn = 
      (displayStudyFigureInformation &&
         (displayStudyFigureLegendInformation) &&
         anyStudyFigurePanelDataOn());
   return anyOn;
}
            
/**
 * report if any study figure panel information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anyStudyFigurePanelDataOn() const
{
   const bool anyOn = 
      (displayStudyFigurePanelInformation &&
          (displayStudyFigurePanelDescriptionInformation ||
           displayStudyFigurePanelTaskDescriptionInformation ||
           displayStudyFigurePanelTaskBaselineInformation ||
           displayStudyFigurePanelTestAttributesInformation));
   return anyOn;
}
            
/**
 * report if any subh header information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anySubHeaderDataOn() const
{
   const bool anyOn =
      (displayStudySubHeaderInformation &&
         (displayStudySubHeaderNameInformation ||
          displayStudySubHeaderShortNameInformation ||
          displayStudySubHeaderTaskDescriptionInformation ||
          displayStudySubHeaderTaskBaselineInformation ||
          displayStudySubHeaderTestAttributesInformation));
   return anyOn;
}
            
/**
 * report if any page reference information should be displayed.
 */
bool 
BrainModelIdentification::IdFilter::anyPageReferenceDataOn() const
{
   const bool anyOn = 
      (displayStudyPageReferenceInformation &&
         (displayStudyPageReferenceHeaderInformation ||
          displayStudyPageReferenceCommentInformation ||
          displayStudyPageReferenceSizeUnitsInformation ||
          displayStudyPageReferenceVoxelSizeInformation ||
          displayStudyPageReferenceStatisticInformation));
   return anyOn;
}
            
/**
 * translate special HTML characters to HTML special characters.
 */
QString 
BrainModelIdentification::htmlTranslate(const QString& ss) const
{
   QString s = ss;
   if (htmlFlag) {
      s = s.replace("&", "&amp;");
      s = s.replace("<", "&lt;");
      s = s.replace(">", "&gt;");   
   }
   return s;
}      

