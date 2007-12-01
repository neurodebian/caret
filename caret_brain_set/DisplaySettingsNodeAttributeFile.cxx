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
static const QString xmlDisplayElementName("ds-display-column");
static const QString xmlThresholdElementName("ds-threshold-column");

/**
 * constructor.
 */
DisplaySettingsNodeAttributeFile::DisplaySettingsNodeAttributeFile(
                                             BrainSet* bs,
                                             GiftiNodeDataFile* gafIn,
                                             NodeAttributeFile* nafIn,
                                             const bool allowSurfaceUniqueColumnSelectionFlagIn,
                                             const bool thresholdColumnValidFlagIn)
   : DisplaySettings(bs)
{
   gaf = gafIn;
   naf = nafIn;
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
      int defValue = 0;
    
      //
      // Determine default value for any new surfaces
      //
      const int numModels = brainSet->getNumberOfBrainModels();
      if (selCol.empty() == false) {
         defValue = selCol[0];
         const int modelNum = brainSet->getFirstBrainModelSurfaceIndex();
         if ((modelNum >= 0) && (modelNum < static_cast<int>(selCol.size()))) {
            defValue = selCol[modelNum];
         }
      }

      const int numCols = getFileNumberOfColumns();

      if (defValue >= numCols) {
         defValue = 0;
      }
      else if (defValue < 0) {
         if (numCols > 0) {
            defValue = 0;
         }
      }

      //
      // Resize to number of brain models
      //
      selCol.resize(numModels, defValue);

      //
      // Reset column indices for any surfaces that may have been deleted
      //
      for (int i = 0; i < numModels; i++) {
         if (selCol[i] >= numCols) {
            selCol[i] = defValue;
         }
         else if (selCol[i] < 0) {
            selCol[i] = defValue;
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
   if (displayColumn[bm1] != displayColumn[bm2]) {
      return false;
   }
   return (thresholdColumn[bm1] == thresholdColumn[bm2]);
}

/**
 * get column selected for display.
 */
int 
DisplaySettingsNodeAttributeFile::getSelectedDisplayColumn(const int modelNumber)
{
   if (displayColumn.empty()) {
      return -1;
   }
  
   int model = modelNumber;
   if (model < 0) {
      model = 0;
   }

   return displayColumn[model];
}

/**
 * set column for display.
 */
void 
DisplaySettingsNodeAttributeFile::setSelectedDisplayColumn(const int modelNumber,
                                                           const int columnNumber)
{
   if (allowSurfaceUniqueColumnSelectionFlag) { 
      const int numCols = getFileNumberOfColumns();
      std::vector<QString> columnNames;
      getFileColumnNames(columnNames);
      
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
            }
            for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
               const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
               if (bms != NULL) {
                  switch (bms->getStructure().getType()) {
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                        if (leftCol >= 0) {
                           displayColumn[i] = leftCol;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                        if (rightCol >= 0) {
                           displayColumn[i] = rightCol;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_INVALID:
                        break;

                  }
               }
            }
         }
      }
      else {
         if (modelNumber < 0) {
            std::fill(displayColumn.begin(), displayColumn.end(), columnNumber);
         }
         else {
            displayColumn[modelNumber] = columnNumber;
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
DisplaySettingsNodeAttributeFile::getSelectedThresholdColumn(const int modelNumber)
{
   if (allowSurfaceUniqueColumnSelectionFlag) {
      if (thresholdColumn.empty()) {
         return -1;
      }

      int model = modelNumber;
      if (model < 0) {
         model = 0;
      }

      return thresholdColumn[model];
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
                                                             const int columnNumber)
{
   if (allowSurfaceUniqueColumnSelectionFlag) {
      const int numCols = getFileNumberOfColumns();
      std::vector<QString> columnNames;
      getFileColumnNames(columnNames);
      
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
            }
            for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
               const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
               if (bms != NULL) {
                  switch (bms->getStructure().getType()) {
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                        if (leftCol >= 0) {
                           thresholdColumn[i] = leftCol;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                        if (rightCol >= 0) {
                           thresholdColumn[i] = rightCol;
                        }
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM:
                        break;
                     case Structure::STRUCTURE_TYPE_INVALID:
                        break;
                  }
               }
            }
         }
      }
      else {
         if (modelNumber < 0) {
            std::fill(thresholdColumn.begin(), thresholdColumn.end(), columnNumber);
         }
         else {
            thresholdColumn[modelNumber] = columnNumber;
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

   //
   // Is each surface allowed its own column selection
   //
   if (allowSurfaceUniqueColumnSelectionFlag) {
      //
      // Check each brain model
      //
      bool didDefaultFlag = false;
      const int num = std::min(brainSet->getNumberOfBrainModels(),
                               static_cast<int>(displayColumn.size()));
      
      for (int n = 0; n < num; n++) {
         //
         // Is this a surface ?
         //
         const BrainModelSurface* bms = brainSet->getBrainModelSurface(n);
         if (bms != NULL) {
            const QString displayColumnName(columnNames[displayColumn[n]]);
            const QString thresholdColumnName(columnNames[thresholdColumn[n]]);
            
            if (displayColumnName.isEmpty() == false) {
               //
               // Do the Default first
               //
               if (didDefaultFlag == false) {
                  SceneFile::SceneInfo si(xmlDisplayElementName, 
                               SceneFile::SceneInfo::getDefaultSurfacesName(), displayColumnName);
                  sc.addSceneInfo(si);
                  
                  if (thresholdColumnValidFlag) {
                     SceneFile::SceneInfo sit(xmlThresholdElementName, 
                                  SceneFile::SceneInfo::getDefaultSurfacesName(), thresholdColumnName);
                     sc.addSceneInfo(sit);
                  }
                  
                  didDefaultFlag = true;
               }
               
               //
               // Get name of coordinate file
               //
               const CoordinateFile* cf = bms->getCoordinateFile();
               QString surfaceName = FileUtilities::basename(cf->getFileName());
               
               //
               // Create the scene info for this model
               //
               SceneFile::SceneInfo si(xmlDisplayElementName, surfaceName, displayColumnName);
               sc.addSceneInfo(si);
               
               if (thresholdColumnValidFlag) {
                  //
                  // Create the scene info for this model
                  //
                  SceneFile::SceneInfo si(xmlThresholdElementName, surfaceName, thresholdColumnName);
                  sc.addSceneInfo(si);
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

   if (allowSurfaceUniqueColumnSelectionFlag) {
      const int num = sc.getNumberOfSceneInfo();
      for (int i = 0; i < num; i++) {
         const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
         const QString infoName = si->getName();     
         
         bool dispFlag = false;
         bool dispLegacyFlag = false;
         bool threshFlag = false;
         bool threshLegacyFlag = false;
         if (infoName == xmlDisplayElementName) {
            dispFlag = true;
         }
         else if (infoName == legacyDisplayElementName) {
            dispLegacyFlag = true;
         }
         else if (infoName == xmlThresholdElementName) {
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
                  const int lastIndex = std::min(endSurface,
                                                 static_cast<int>(displayColumn.size()));
                  for (int k = startSurface; k < lastIndex; k++) {
                     displayColumn[k] = columnNum;
                  }
               }
               else if (threshFlag || threshLegacyFlag) {
                  const int lastIndex = std::min(endSurface,
                                                 static_cast<int>(thresholdColumn.size()));
                  for (int k = startSurface; k < lastIndex; k++) {
                     thresholdColumn[k] = columnNum;
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
         if (infoName == xmlDisplayElementName) {
            dispFlag = true;
         }
         else if (infoName == legacyDisplayElementName) {
            dispLegacyFlag = true;
         }
         else if (infoName == xmlThresholdElementName) {
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
}      

/**
 * get the number of overlays.
 */
int 
DisplaySettingsNodeAttributeFile::getNumberOfOverlays()
{
   //
   // This value will eventually come from BrainModelSurfaceNodeColoring
   //
   return 1;
}
      
/**
 * get the index for column selection.
 */
int 
DisplaySettingsNodeAttributeFile::getColumnSelectionIndex(const int modelIndex,
                                                          const int overlayNumber)
{
   const int indx = (modelIndex * getNumberOfOverlays()) + overlayNumber;
   return indx;
}                                  
