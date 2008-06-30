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

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "DisplaySettingsNodeAttributeFile.h"
#include "FileUtilities.h"
#include "GiftiNodeDataFile.h"
#include "NodeAttributeFile.h"

//
// Name of scene element
//
static const QString xmlOldDisplayElementName("ds-display-column");
static const QString xmlOldThresholdElementName("ds-display-column");
static const QString xmlDisplayElementName("DisplaySettingsDisplayColumn");
static const QString xmlThresholdElementName("DisplaySettingsThresholdColumn");
static const QString xmlNumberOfOverlaysName("DisplaySettingsNumberOfOverlays");

/**
 * constructor.
 */
DisplaySettingsNodeAttributeFile::DisplaySettingsNodeAttributeFile(
                     BrainSet* bs,
                     GiftiNodeDataFile* gafIn,
                     NodeAttributeFile* nafIn,
                     const BrainModelSurfaceOverlay::OVERLAY_SELECTIONS overlayTypeIn,
                     const bool allowSurfaceUniqueColumnSelectionFlagIn,
                     const bool thresholdColumnValidFlagIn)
   : DisplaySettings(bs)
{
   gaf = gafIn;
   naf = nafIn;
   overlayType = overlayTypeIn;
   allowSurfaceUniqueColumnSelectionFlag = allowSurfaceUniqueColumnSelectionFlagIn;
   thresholdColumnValidFlag = thresholdColumnValidFlagIn;
   applySelectionToLeftAndRightStructuresFlag = false;
}

/**
 * destructor.
 */
DisplaySettingsNodeAttributeFile::~DisplaySettingsNodeAttributeFile()
{
}

/**
 * reinitialize all display settings (be sure to call this from child).
 */
void 
DisplaySettingsNodeAttributeFile::reset()
{
   displayColumn.clear();
   thresholdColumn.clear();
}

/**
 * update any selections due to changes with loaded data files (be sure to call from child).
 */
void 
DisplaySettingsNodeAttributeFile::update()
{
   updateSelectedColumnIndices(displayColumn);
   updateSelectedColumnIndices(thresholdColumn);
}

/**
 * Set the default value for selected column of any new surfaces.
 */
void 
DisplaySettingsNodeAttributeFile::updateSelectedColumnIndices(std::vector<int>& selCol)
{
   if (allowSurfaceUniqueColumnSelectionFlag) { 
      const int numCols = getFileNumberOfColumns();
      const int numOverlays = brainSet->getNumberOfSurfaceOverlays();
      std::vector<int> defValue(numOverlays, 0);
    
      //
      // Determine default value for any new surfaces
      //
      const int numModels = brainSet->getNumberOfBrainModels();
      if (selCol.empty() == false) {
         //defValue = selCol[0];
         const int modelNum = brainSet->getFirstBrainModelSurfaceIndex();
         if ((modelNum >= 0) && (modelNum < static_cast<int>(selCol.size()))) {
            for (int i = 0; i < numOverlays; i++) {
               defValue[i] = selCol[getColumnSelectionIndex(modelNum, i)];
            }
         }
      }

      //
      // Limit default columns to valid columns
      //
      for (int i = 0; i < numOverlays; i++) {
         if (defValue[i] >= numCols) {
            defValue[i] = 0;
         }
         else if (defValue[i] < 0) {
            if (numCols > 0) {
               defValue[i] = 0;
            }
         }
      }

      //
      // Resize to number of brain models and overlays
      //
      const int oldNumModels = selCol.size() / numOverlays;
      selCol.resize(numModels * numOverlays, 0);
      for (int i = oldNumModels; i < numModels; i++) {
         for (int j = 0; j < numOverlays; j++) {
            selCol[getColumnSelectionIndex(i, j)] = defValue[j];
         }
      }

      //
      // Reset column indices for any surfaces that may have been deleted
      //
      for (int i = 0; i < numModels; i++) {
         for (int j = 0; j < numOverlays; j++) {
            if (selCol[getColumnSelectionIndex(i, j)] >= numCols) {
               selCol[getColumnSelectionIndex(i, j)] = defValue[j];
            }
            else if (selCol[getColumnSelectionIndex(i, j)] < 0) {
               selCol[getColumnSelectionIndex(i, j)] = defValue[j];
            }
         }
      }
   }
   else {
      if (selCol.empty()) {
         selCol.resize(1, 0);
      }
   }
}

/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettingsNodeAttributeFile::columnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   const int numOverlays = brainSet->getNumberOfSurfaceOverlays();
   
   for (int j = 0; j < numOverlays; j++) {
      if (displayColumn[getColumnSelectionIndex(bm1, j)] 
          != displayColumn[getColumnSelectionIndex(bm2, j)]) {
         return false;
      }
      if (thresholdColumn[getColumnSelectionIndex(bm1, j)] 
          != thresholdColumn[getColumnSelectionIndex(bm2, j)]) {
         return false;
      }
   }
   
   return true;
}

/**
 * get column selected for display.
 */
int 
DisplaySettingsNodeAttributeFile::getSelectedDisplayColumn(const int modelNumber,
                                                           const int overlayNumber) const
{
   if (displayColumn.empty()) {
      return -1;
   }
  
   if (allowSurfaceUniqueColumnSelectionFlag) { 
      int model = modelNumber;
      if (model < 0) {
         model = 0;
      }

      return displayColumn[getColumnSelectionIndex(model, overlayNumber)];
   }
   else {
      return displayColumn[0];
   }
}

/**
 * set column for display.
 */
void 
DisplaySettingsNodeAttributeFile::setSelectedDisplayColumn(const int modelNumber,
                                                           const int overlayNumber,
                                                           const int columnNumber)
{
   const int numOverlays = brainSet->getNumberOfSurfaceOverlays();
   
   if (allowSurfaceUniqueColumnSelectionFlag) { 
      const int numCols = getFileNumberOfColumns();
      std::vector<QString> columnNames;
      getFileColumnNames(columnNames);
      
      bool madeSelectionsLeftRightFlag = false;
      
      if (applySelectionToLeftAndRightStructuresFlag) {
         if ((columnNumber >= 0) && (columnNumber < numCols)) {
            int leftCol = -1;
            int rightCol = -1;
            QString name = columnNames[columnNumber].toLower().trimmed();
            if (name.indexOf("left") >= 0) {
               leftCol = columnNumber;
               const QString rightName = name.replace("left", "right");
               for (int i = 0; i < numCols; i++) {
                  if (columnNames[i].toLower().trimmed() == rightName) {
                     rightCol = i;
                     break;
                  }
               }
               madeSelectionsLeftRightFlag = true;
            }
            else if (name.indexOf("right") >= 0) {
               rightCol = columnNumber;
               const QString leftName = name.replace("right", "left");
               for (int i = 0; i < numCols; i++) {
                  if (columnNames[i].toLower().trimmed() == leftName) {
                     leftCol = i;
                     break;
                  }
               }
               madeSelectionsLeftRightFlag = true;
            }

            for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
               const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
               if (bms != NULL) {
                  switch (bms->getStructure().getType()) {
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                        if (leftCol >= 0) {
                           if (overlayNumber < 0) {
                              for (int nn = 0; nn < numOverlays; nn++) {
                                 displayColumn[getColumnSelectionIndex(i, nn)]
                                    = leftCol;
                              }
                           }
                           else {
                              displayColumn[getColumnSelectionIndex(i, overlayNumber)]
                                 = leftCol;
                           }
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                        if (rightCol >= 0) {
                           if (overlayNumber < 0) {
                              for (int nn = 0; nn < numOverlays; nn++) {
                                 displayColumn[getColumnSelectionIndex(i, nn)]
                                    = rightCol;
                              }
                           }
                           else {
                              displayColumn[getColumnSelectionIndex(i, overlayNumber)]
                                 = rightCol;
                           }
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_INVALID:
                        break;
                  }
               }
            }
         }
      }
      
      if (madeSelectionsLeftRightFlag == false) {
         if (modelNumber < 0) {
            for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
               if (overlayNumber < 0) {
                  for (int nn = 0; nn < numOverlays; nn++) {
                     displayColumn[getColumnSelectionIndex(i, nn)] = columnNumber;
                  }
               }
               else {
                  displayColumn[getColumnSelectionIndex(i, overlayNumber)] = columnNumber;
               }
            }
         }
         else {
            if (overlayNumber < 0) {
               for (int nn = 0; nn < numOverlays; nn++) {
                  displayColumn[getColumnSelectionIndex(modelNumber, nn)] = columnNumber;
               }
            }
            else {
               displayColumn[getColumnSelectionIndex(modelNumber, overlayNumber)] = columnNumber;
            }
         }
      }
   }
   else {
      displayColumn[0] = columnNumber;
   }
}

/**
 * get column selected for thresholding.
 */
int 
DisplaySettingsNodeAttributeFile::getSelectedThresholdColumn(const int modelNumber,
                                                             const int overlayNumber) const
{
   if (allowSurfaceUniqueColumnSelectionFlag) {
      if (thresholdColumn.empty()) {
         return -1;
      }

      int model = modelNumber;
      if (model < 0) {
         model = 0;
      }

      return thresholdColumn[getColumnSelectionIndex(model, overlayNumber)];
   }
   else {
      return thresholdColumn[0];
   }
}

/**
 * set column for thresholding.
 */
void 
DisplaySettingsNodeAttributeFile::setSelectedThresholdColumn(const int modelNumber,
                                                             const int overlayNumber,
                                                             const int columnNumber)
{
   const int numOverlays = brainSet->getNumberOfSurfaceOverlays();

   if (allowSurfaceUniqueColumnSelectionFlag) {
      const int numCols = getFileNumberOfColumns();
      std::vector<QString> columnNames;
      getFileColumnNames(columnNames);
      
      bool madeSelectionsLeftRightFlag = false;
      if (applySelectionToLeftAndRightStructuresFlag) {
         if ((columnNumber >= 0) && (columnNumber < numCols)) {
            int leftCol = -1;
            int rightCol = -1;
            QString name = columnNames[columnNumber].toLower().trimmed();
            if (name.indexOf("left") >= 0) {
               leftCol = columnNumber;
               const QString rightName = name.replace("left", "right");
               for (int i = 0; i < numCols; i++) {
                  if (columnNames[i].toLower().trimmed() == rightName) {
                     rightCol = i;
                     break;
                  }
               }
               madeSelectionsLeftRightFlag = true;
            }
            else if (name.indexOf("right") >= 0) {
               rightCol = columnNumber;
               const QString leftName = name.replace("right", "left");
               for (int i = 0; i < numCols; i++) {
                  if (columnNames[i].toLower().trimmed() == leftName) {
                     leftCol = i;
                     break;
                  }
               }
               madeSelectionsLeftRightFlag = true;
            }
            for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
               const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
               if (bms != NULL) {
                  switch (bms->getStructure().getType()) {
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                        if (leftCol >= 0) {
                           if (overlayNumber < 0) {
                              for (int nn = 0; nn < numOverlays; nn++) {
                                 thresholdColumn[getColumnSelectionIndex(i, nn)]
                                    = leftCol;
                              }
                           }
                           else {
                              thresholdColumn[getColumnSelectionIndex(i, overlayNumber)]
                                 = leftCol;
                           }
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                        if (rightCol >= 0) {
                          if (overlayNumber < 0) {
                              for (int nn = 0; nn < numOverlays; nn++) {
                                 thresholdColumn[getColumnSelectionIndex(i, nn)]
                                    = rightCol;
                              }
                           }
                           else {
                              thresholdColumn[getColumnSelectionIndex(i, overlayNumber)]
                                 = rightCol;
                           }
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_INVALID:
                        break;
                  }
               }
            }
         }
      }
      
      if (madeSelectionsLeftRightFlag == false) {
         if (modelNumber < 0) {
            for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
              if (overlayNumber < 0) {
                  for (int nn = 0; nn < numOverlays; nn++) {
                     thresholdColumn[getColumnSelectionIndex(i, nn)] = columnNumber;
                  }
               }
               else {
                  thresholdColumn[getColumnSelectionIndex(i, overlayNumber)] = columnNumber;
               }
            }
         }
         else {
           if (overlayNumber < 0) {
               for (int nn = 0; nn < numOverlays; nn++) {
                  thresholdColumn[getColumnSelectionIndex(modelNumber, nn)] = columnNumber;
               }
            }
            else {
               thresholdColumn[getColumnSelectionIndex(modelNumber, overlayNumber)] = columnNumber;
            }
         }
      }
   }
   else {
      thresholdColumn[0] = columnNumber;
   }
}

/**
 * get the number of columns for the file.
 */
int 
DisplaySettingsNodeAttributeFile::getFileNumberOfColumns() const
{
   int numCols = 0;
   
   if (gaf != NULL) {
      numCols = gaf->getNumberOfColumns();
   }
   if (naf != NULL) {
      numCols = naf->getNumberOfColumns();
   }
   
   return numCols;
}

/**
 * get the names for file columns.
 */
void 
DisplaySettingsNodeAttributeFile::getFileColumnNames(std::vector<QString>& columnNames) const
{
   columnNames.clear();
   
   if (gaf != NULL) {
      const int numCols = gaf->getNumberOfColumns();
      for (int i = 0; i < numCols; i++) {
         columnNames.push_back(gaf->getColumnName(i));
      }
   }
   else if (naf != NULL) {
      const int numCols = naf->getNumberOfColumns();
      for (int i = 0; i < numCols; i++) {
         columnNames.push_back(naf->getColumnName(i));
      }
   }
}
      
/**
 * save scene for selected columns.
 */
void 
DisplaySettingsNodeAttributeFile::saveSceneSelectedColumns(SceneFile::SceneClass& sc)
{
      
   //
   // Names of file columns
   //
   std::vector<QString> columnNames;
   getFileColumnNames(columnNames);

   if (columnNames.empty() == false) {
      //
      // Is each surface allowed its own column selection
      //
      if (allowSurfaceUniqueColumnSelectionFlag) {
         //
         // Check each brain model
         //
         const int numBrainModels = brainSet->getNumberOfBrainModels();
         const int numOverlays = brainSet->getNumberOfSurfaceOverlays();
         
         //
         // Add the number of overlays
         //
         SceneFile::SceneInfo sin(xmlNumberOfOverlaysName,
                                  numOverlays);
         sc.addSceneInfo(sin);
         
         //
         // Process each brain model
         //
         bool didDefaultFlag = false;
         for (int n = 0; n < numBrainModels; n++) {            
            //
            // Is this a surface ?
            //
            const BrainModelSurface* bms = brainSet->getBrainModelSurface(n);
            if (bms != NULL) {
               //
               // Setup the default values
               // 
               for (int j = 0; j < numOverlays; j++) {
                  const int overlayNumber = j;
                  const QString displayColumnName(
                     columnNames[displayColumn[getColumnSelectionIndex(n, j)]]);
                  const QString thresholdColumnName(
                     columnNames[thresholdColumn[getColumnSelectionIndex(n, j)]]);
                  
                  if (displayColumnName.isEmpty() == false) {
                     //
                     // Do the Default first
                     //
                     if (didDefaultFlag == false) {
                        SceneFile::SceneInfo si(xmlDisplayElementName, 
                                     SceneFile::SceneInfo::getDefaultSurfacesName(),
                                     overlayNumber,
                                     displayColumnName);
                        sc.addSceneInfo(si);
                        
                        if (thresholdColumnValidFlag) {
                           SceneFile::SceneInfo sit(xmlThresholdElementName, 
                                        SceneFile::SceneInfo::getDefaultSurfacesName(),
                                        overlayNumber,
                                        thresholdColumnName);
                           sc.addSceneInfo(sit);
                        }
                     }
                  }
               }
               
               didDefaultFlag = true;            
            }
            
            //
            //
            // Process each overlay for the brain model
            //
            for (int j = 0; j < numOverlays; j++) {
               const int overlayNumber = j;
               
               //
               // Is this a surface ?
               //
               const BrainModelSurface* bms = brainSet->getBrainModelSurface(n);
               if (bms != NULL) {
                  const QString displayColumnName(
                     columnNames[displayColumn[getColumnSelectionIndex(n, j)]]);
                  const QString thresholdColumnName(
                     columnNames[thresholdColumn[getColumnSelectionIndex(n, j)]]);
                  
                  if (displayColumnName.isEmpty() == false) {
                     //
                     // Get name of coordinate file
                     //
                     const CoordinateFile* cf = bms->getCoordinateFile();
                     QString surfaceName = FileUtilities::basename(cf->getFileName());
                     
                     //
                     // Create the scene info for this model
                     //
                     SceneFile::SceneInfo si(xmlDisplayElementName, 
                                             surfaceName,  
                                             overlayNumber, 
                                             displayColumnName);
                     sc.addSceneInfo(si);
                     
                     if (thresholdColumnValidFlag) {
                        //
                        // Create the scene info for this model
                        //
                        SceneFile::SceneInfo si(xmlThresholdElementName, 
                                                surfaceName, 
                                                overlayNumber, 
                                                thresholdColumnName);
                        sc.addSceneInfo(si);
                     }
                  }
               }
            }
         }
      }
      else {
         if ((displayColumn[0] >= 0) && (displayColumn[0] < getFileNumberOfColumns())) {
            sc.addSceneInfo(SceneFile::SceneInfo(xmlDisplayElementName,
                                                 columnNames[displayColumn[0]]));
         }
         
         if (thresholdColumnValidFlag) {
            if ((thresholdColumn[0] >= 0) && (thresholdColumn[0] < getFileNumberOfColumns())) {
               sc.addSceneInfo(SceneFile::SceneInfo(xmlThresholdElementName,
                                                    columnNames[thresholdColumn[0]]));
            }
         }
      }
   }

   sc.addSceneInfo(SceneFile::SceneInfo("applySelectionToLeftAndRightStructuresFlag",
                                        applySelectionToLeftAndRightStructuresFlag));
}

/**
 * show scene for selected columns.
 */
void 
DisplaySettingsNodeAttributeFile::showSceneSelectedColumns(const SceneFile::SceneClass& sc,
                                                           const QString& fileTypeName,
                                                           const QString& legacyDisplayElementName,
                                                           const QString& legacyThresholdElmentName,
                                                           QString& errorMessage)
{
   //
   // Names of file columns
   //
   std::vector<QString> columnNames;
   getFileColumnNames(columnNames);
   const int numColumns = static_cast<int>(columnNames.size());

   const int totalNumberOfOverlays = brainSet->getNumberOfSurfaceOverlays();
   int numberOfOverlaysInScene = 1;
   
   if (allowSurfaceUniqueColumnSelectionFlag) {
      const int num = sc.getNumberOfSceneInfo();
      for (int i = 0; i < num; i++) {
         const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
         const QString infoName = si->getName();     
         
         bool dispFlag = false;
         bool dispLegacyFlag = false;
         bool threshFlag = false;
         bool threshLegacyFlag = false;
         
         if (infoName == xmlNumberOfOverlaysName) {
            numberOfOverlaysInScene = si->getValueAsInt();
            if (numberOfOverlaysInScene > totalNumberOfOverlays) {
               errorMessage = "The scene contains more overlays than Caret supports.  "
                              "Some data may not be displayed properly.";
            }
         }
         else if ((infoName == xmlDisplayElementName) ||
                  (infoName == xmlOldDisplayElementName)) {
            dispFlag = true;
         }
         else if (infoName == legacyDisplayElementName) {
            dispLegacyFlag = true;
         }
         else if ((infoName == xmlThresholdElementName) ||
                  (infoName == xmlOldThresholdElementName)) {
            threshFlag = true;
         }
         else if (infoName == legacyThresholdElmentName) {
            threshLegacyFlag = true;
         }
         
         if (dispFlag || dispLegacyFlag || threshFlag || threshLegacyFlag) {
            //
            // Handle all surfaces or a specific surface
            //
            const QString surfaceName = si->getModelName();
            int startSurface = 0;
            int endSurface   = brainSet->getNumberOfBrainModels();
            
            //
            // Old scenes (before separate overlays) should set all overlays
            //
            int overlayStartNumber = 0;
            int overlayEndNumber   = totalNumberOfOverlays;
            const int overlayNumberInSceneInfo = si->getOverlayNumber();
            if (overlayNumberInSceneInfo >= 0) {
               overlayStartNumber = std::min(overlayNumberInSceneInfo,
                                             totalNumberOfOverlays);
               overlayEndNumber = std::min(overlayNumberInSceneInfo + 1,
                                           totalNumberOfOverlays);
            }
            
            //
            // Is this NOT the default surface
            //
            if (surfaceName != SceneFile::SceneInfo::getDefaultSurfacesName()) {
               endSurface = 0;
               const BrainModelSurface* bms = brainSet->getBrainModelSurfaceWithFileName(surfaceName);
               if (bms != NULL) {
                  startSurface = brainSet->getBrainModelIndex(bms);
                  if (startSurface >= 0) {
                     endSurface = startSurface + 1;
                  }
               }
               else {
                  QString msg("Surface named \"");
                  msg.append(surfaceName);
                  msg.append("\" not found.\n");
                  errorMessage.append(msg);
               }
            }
            
            //
            // Get the index of the data column
            //
            int columnNum = -1;
            const QString dataColumnName = si->getValueAsString();
            for (int m = 0; m < numColumns; m++) {
               if (columnNames[m] == dataColumnName) {
                  columnNum = m;
                  break;
               }
            }
            
            if (columnNum >= 0) {
               //
               // Set the selected column
               //
               if (dispFlag || dispLegacyFlag) {
                  //const int lastIndex = std::min(endSurface,
                  //                               static_cast<int>(displayColumn.size()));
                  for (int k = startSurface; k < endSurface; k++) {
                     for (int m = overlayStartNumber; m < overlayEndNumber; m++) {
                        setSelectedDisplayColumn(k, m, columnNum);
                        //displayColumn[k] = columnNum;
                     }
                  }
               }
               else if (threshFlag || threshLegacyFlag) {
                  //const int lastIndex = std::min(endSurface,
                  //                               static_cast<int>(thresholdColumn.size()));
                  for (int k = startSurface; k < endSurface; k++) {
                     for (int m = overlayStartNumber; m < overlayEndNumber; m++) {
                        setSelectedThresholdColumn(k, m, columnNum);
                        //thresholdColumn[k] = columnNum;
                     }
                  }
               }
            }
            else {
               QString msg(fileTypeName);
               msg.append(" column named \"");
               msg.append(dataColumnName);
               msg.append("\" not found.\n");
               errorMessage.append(msg);
            }
         }
      }
   }
   else {
      const int num = sc.getNumberOfSceneInfo();
      for (int i = 0; i < num; i++) {
         const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
         const QString infoName = si->getName();     
         
         bool dispFlag = false;
         bool dispLegacyFlag = false;
         bool threshFlag = false;
         bool threshLegacyFlag = false;
         if ((infoName == xmlDisplayElementName) ||
             (infoName == xmlOldDisplayElementName)) {
            dispFlag = true;
         }
         else if (infoName == legacyDisplayElementName) {
            dispLegacyFlag = true;
         }
         else if ((infoName == xmlThresholdElementName) ||
                  (infoName == xmlOldThresholdElementName)) {
            threshFlag = true;
         }
         else if (infoName == legacyThresholdElmentName) {
            threshLegacyFlag = true;
         }
         
         if (dispFlag || dispLegacyFlag || threshFlag || threshLegacyFlag) {
            const QString colName = si->getValueAsString();
            for (int j = 0; j < numColumns; j++) {
               if (colName == columnNames[j]) {
                  if (dispFlag || dispLegacyFlag) {
                     displayColumn[0] = j;
                  }
                  else if (threshFlag || threshLegacyFlag) {
                     thresholdColumn[0] = j;
                  }
                  return;
               }
            }
            
            QString msg(fileTypeName);
            msg.append(" column named \"");
            msg.append(colName);
            msg.append("\" not found.\n");
            errorMessage.append(msg);
         }
      }
   }

   const int num = sc.getNumberOfSceneInfo();
   for (int i = 0; i < num; i++) {
      const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
      const QString infoName = si->getName();  
      if (infoName == "applySelectionToLeftAndRightStructuresFlag") {
         applySelectionToLeftAndRightStructuresFlag = si->getValueAsBool();
      }
   }
}      

/**
 * get the index for column selection.
 */
int 
DisplaySettingsNodeAttributeFile::getColumnSelectionIndex(const int modelIndex,
                                                          const int overlayNumber) const
{
   const int indx = (modelIndex * brainSet->getNumberOfSurfaceOverlays())
                    + overlayNumber;
   return indx;
}                                  

/**
 * for node attribute files - all column selections for each surface are the same.
 */
/*
bool 
DisplaySettingsNodeAttributeFile::displayColumnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (displayColumn[bm1] == displayColumn[bm2]);
}      
*/

/**
 * for node attribute files - all column selections for each surface are the same.
 */
/*
bool 
DisplaySettingsNodeAttributeFile::thresholdColumnSelectionsAreTheSame(const int bm1, const int bm2) const
{
   return (thresholdColumn[bm1] == thresholdColumn[bm2]);
} 
*/     

/**
 * get flags to find out if any columns are selected as one of the overlays.
 */
void
DisplaySettingsNodeAttributeFile::getSelectedColumnFlags(const int brainModelIndex,
                                          std::vector<bool>& selectedColumnFlagsOut) const
{
   //
   // Clear the selected column flags
   //
   selectedColumnFlagsOut.resize(getFileNumberOfColumns());
   std::fill(selectedColumnFlagsOut.begin(), selectedColumnFlagsOut.end(), false);
   
   for (int i = 0; i < brainSet->getNumberOfSurfaceOverlays(); i++) {
      const BrainModelSurfaceOverlay* bmsOverlay = brainSet->getSurfaceOverlay(i);
      if (bmsOverlay->getOverlay(brainModelIndex) == overlayType) {
         selectedColumnFlagsOut[getSelectedDisplayColumn(brainModelIndex, i)] = true;
      }
   }
}

/**
 * Get the selected column for the first brain model that is an overla of this type.
 */
/*
int
DisplaySettingsNodeAttributeFile::getPrioritySelectedColumn() const
{
   for (int i = 0; i < brainSet->getNumberOfSurfaceOverlays(); i++) {
      const BrainModelSurfaceOverlay* bmsOverlay = brainSet->getSurfaceOverlay(i);
      if (bmsOverlay->getOverlay() == overlayType) {
         for (int j = 0; j < brainSet->getNumberOfBrainModels(); j++) {
           const int col = getSelectedDisplayColumn(j, i);
           if (col >= 0) {
              return col;
           }
         }
      }
   }
   
   return -1;
}
*/

/**
 * Get first selected column that is an overlay for the brain model (-1 if none)
 */
int 
DisplaySettingsNodeAttributeFile::getFirstSelectedColumnForBrainModel(const int brainModelIndex) const
{
   std::vector<bool> selectedColumnFlags;
   getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
   
   for (unsigned int i = 0; i < selectedColumnFlags.size(); i++) {
      if (selectedColumnFlags[i]) {
         return i;
      }
   }
   
   return -1;
}

