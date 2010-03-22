
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

#include "BrainSet.h"
#include "ColorFile.h"
#include "DisplaySettings.h"
#include "FileUtilities.h"
#include "GiftiNodeDataFile.h"
#include "NodeAttributeFile.h"

/**
 * constructor.
 */
DisplaySettings::DisplaySettings(BrainSet* bsIn) 
{ 
   brainSet = bsIn; 
}

/**
 * destructor.
 */
DisplaySettings::~DisplaySettings()
{
}      

/**
 * Set the default value for selected column of any new surfaces
 */
void
DisplaySettings::updateSelectedColumnIndices(const NodeAttributeFile* naf,
                                             std::vector<int>& selCol)
{
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
   
   const int numCols = naf->getNumberOfColumns(); 

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

/**
 * Set the default value for selected column of any new surfaces
 */
void
DisplaySettings::updateSelectedColumnIndices(const GiftiNodeDataFile* naf,
                                             std::vector<int>& selCol)
{
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
   
   const int numCols = naf->getNumberOfColumns(); 

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

/**
 * Set the default value for for a column
 */
void
DisplaySettings::updateSelectedColumnIndex(const NodeAttributeFile* naf,
                                           int& selCol)
{
   //
   // Reset column indices for any surfaces that may have been deleted
   //
   const int numCols = naf->getNumberOfColumns(); 
   if (selCol >= numCols) {
      selCol = 0;
   }
   else if (numCols > 0) {
      if (selCol < 0) {
         selCol = 0;
      }
   }
   if (numCols == 0) {
      selCol = -1;
   }
}

/**
 * Set the default value for for a column
 */
void
DisplaySettings::updateSelectedColumnIndex(const GiftiNodeDataFile* naf,
                                           int& selCol)
{
   //
   // Reset column indices for any surfaces that may have been deleted
   //
   const int numCols = naf->getNumberOfColumns(); 
   if (selCol >= numCols) {
      selCol = 0;
   }
   else if (numCols > 0) {
      if (selCol < 0) {
         selCol = 0;
      }
   }
   if (numCols == 0) {
      selCol = -1;
   }
}

/**
 * for node attribute files - all column selections for each surface are the same.
 */
bool 
DisplaySettings::columnSelectionsAreTheSame(const int /*bm1*/, const int /*bm2*/) const
{
   return true;
}      

/**
 * apply a scene for node attribute display settings.
 */
void 
DisplaySettings::showSceneNodeAttribute(const SceneFile::SceneClass& sc,
                                        const QString& myInfoName,
                                        const NodeAttributeFile* naf,
                                        const QString& fileTypeName,
                                        std::vector<int>& selectedColumn,
                                        QString& errorMessage)
{
   const int num = sc.getNumberOfSceneInfo();
   for (int i = 0; i < num; i++) {
      const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
      const QString infoName = si->getName();     
      
      if (infoName == myInfoName) {
         //
         // Handle all surfaces or a specific surface
         //
         const QString surfaceName = si->getModelName();
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
         for (int m = 0; m < naf->getNumberOfColumns(); m++) {
            if (naf->getColumnName(m) == dataColumnName) {
               columnNum = m;
               break;
            }
         }
         
         if (columnNum >= 0) {
            //
            // Set the selected column
            //
            const int lastIndex = std::min(endSurface,
                                           static_cast<int>(selectedColumn.size()));
            for (int k = startSurface; k < lastIndex; k++) {
               selectedColumn[k] = columnNum;
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
                                  
/**
 * apply a scene for node attribute display settings.
 */
void 
DisplaySettings::showSceneNodeAttribute(const SceneFile::SceneClass& sc,
                                        const QString& myInfoName,
                                        const GiftiNodeDataFile* naf,
                                        const QString& fileTypeName,
                                        std::vector<int>& selectedColumn,
                                        QString& errorMessage)
{
   const int num = sc.getNumberOfSceneInfo();
   for (int i = 0; i < num; i++) {
      const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
      const QString infoName = si->getName();     
      
      if (infoName == myInfoName) {
         //
         // Handle all surfaces or a specific surface
         //
         const QString surfaceName = si->getModelName();
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
         for (int m = 0; m < naf->getNumberOfColumns(); m++) {
            if (naf->getColumnName(m) == dataColumnName) {
               columnNum = m;
               break;
            }
         }
         
         if (columnNum >= 0) {
            //
            // Set the selected column
            //
            const int lastIndex = std::min(endSurface,
                                           static_cast<int>(selectedColumn.size()));
            for (int k = startSurface; k < lastIndex; k++) {
               selectedColumn[k] = columnNum;
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
                                  
/**
 * create a scene for node attribute display settings.
 */
void 
DisplaySettings::saveSceneNodeAttribute(SceneFile::SceneClass& sc,
                                        const QString& infoName,
                                        const NodeAttributeFile* naf,
                                        const std::vector<int>& selectedColumn)
{
   //
   // Check each brain model
   //
   bool didDefaultFlag = false;
/*
   const int num = std::min(brainSet->getNumberOfBrainModels(),
                            std::min(naf->getNumberOfColumns(),
                                     static_cast<int>(selectedColumn.size())));
*/
   const int num = std::min(brainSet->getNumberOfBrainModels(),
                            static_cast<int>(selectedColumn.size()));
                            
   for (int n = 0; n < num; n++) {
      //
      // Is this a surface ?
      //
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(n);
      if (bms != NULL) {
         const QString dataColumnName(naf->getColumnName(selectedColumn[n]));
         if (dataColumnName.isEmpty() == false) {
            //
            // Do the Default first
            //
            if (didDefaultFlag == false) {
               SceneFile::SceneInfo si(infoName, 
                            SceneFile::SceneInfo::getDefaultSurfacesName(), dataColumnName);
               sc.addSceneInfo(si);
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
            SceneFile::SceneInfo si(infoName, surfaceName, dataColumnName);
            sc.addSceneInfo(si);
         }
      }
   }
}                                  

/**
 * create a scene for node attribute display settings.
 */
void 
DisplaySettings::saveSceneNodeAttribute(SceneFile::SceneClass& sc,
                                        const QString& infoName,
                                        const GiftiNodeDataFile* naf,
                                        const std::vector<int>& selectedColumn)
{
   //
   // Check each brain model
   //
   bool didDefaultFlag = false;
   const int num = std::min(brainSet->getNumberOfBrainModels(),
                            static_cast<int>(selectedColumn.size()));
/*
   const int num = std::min(brainSet->getNumberOfBrainModels(),
                            std::min(naf->getNumberOfColumns(),
                                     static_cast<int>(selectedColumn.size())));
*/                            
   for (int n = 0; n < num; n++) {
      //
      // Is this a surface ?
      //
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(n);
      if (bms != NULL) {
         const QString dataColumnName(naf->getColumnName(selectedColumn[n]));
         if (dataColumnName.isEmpty() == false) {
            //
            // Do the Default first
            //
            if (didDefaultFlag == false) {
               SceneFile::SceneInfo si(infoName, 
                            SceneFile::SceneInfo::getDefaultSurfacesName(), dataColumnName);
               sc.addSceneInfo(si);
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
            SceneFile::SceneInfo si(infoName, surfaceName, dataColumnName);
            sc.addSceneInfo(si);
         }
      }
   }
}                                  

/**
 * apply a scene for a node attribute column.
 */
void 
DisplaySettings::showSceneNodeAttributeColumn(const SceneFile::SceneInfo* si,
                                              const NodeAttributeFile* naf,
                                              const QString& fileTypeName,
                                              int& displayColumn,
                                              QString& errorMessage)
{
   const QString colName = si->getValueAsString();
   for (int j = 0; j < naf->getNumberOfColumns(); j++) {
      if (colName == naf->getColumnName(j)) {
         displayColumn = j;
         return;
      }
   }
   
   QString msg(fileTypeName);
   msg.append(" column named \"");
   msg.append(colName);
   msg.append("\" not found.\n");
   errorMessage.append(msg);
}
                                  
/**
 * apply a scene for a node attribute column.
 */
void 
DisplaySettings::showSceneNodeAttributeColumn(const SceneFile::SceneInfo* si,
                                              const GiftiNodeDataFile* naf,
                                              const QString& fileTypeName,
                                              int& displayColumn,
                                              QString& errorMessage)
{
   const QString colName = si->getValueAsString();
   for (int j = 0; j < naf->getNumberOfColumns(); j++) {
      if (colName == naf->getColumnName(j)) {
         displayColumn = j;
         return;
      }
   }
   
   QString msg(fileTypeName);
   msg.append(" column named \"");
   msg.append(colName);
   msg.append("\" not found.\n");
   errorMessage.append(msg);
}
                                  
/**
 * create a scene for node attribute display settings.
 */
void 
DisplaySettings::saveSceneNodeAttributeColumn(SceneFile::SceneClass& sc,
                                              const QString& infoName,
                                              const NodeAttributeFile* naf,
                                              const int displayColumn)
{
   if ((displayColumn >= 0) && (displayColumn < naf->getNumberOfColumns())) {
      sc.addSceneInfo(SceneFile::SceneInfo(infoName,
                                           naf->getColumnName(displayColumn)));
   }
}      

/**
 * create a scene for node attribute display settings.
 */
void 
DisplaySettings::saveSceneNodeAttributeColumn(SceneFile::SceneClass& sc,
                                              const QString& infoName,
                                              const GiftiNodeDataFile* naf,
                                              const int displayColumn)
{
   if ((displayColumn >= 0) && (displayColumn < naf->getNumberOfColumns())) {
      sc.addSceneInfo(SceneFile::SceneInfo(infoName,
                                           naf->getColumnName(displayColumn)));
   }
}      

/**
 * apply scene for a color file.
 */
void 
DisplaySettings::showSceneColorFile(const SceneFile::SceneInfo& si,
                                    ColorFile* cf,
                                    const QString& errorName,
                                    QString& errorMessage)
{
   const QString name = si.getModelName();
   const bool selected = si.getValueAsBool();
   const int numColors = cf->getNumberOfColors();
   bool colorFound = false;
   for (int j = 0; j < numColors; j++) {
      ColorFile::ColorStorage* cs = cf->getColor(j);
      if (cs->getName() == name) {
         cs->setSelected(selected);
         colorFound = true;
      }
   }
   if (colorFound == false) {
      QString msg(errorName);
      msg.append(" color \"");
      msg.append(name);
      msg.append("\" not found.\n");
      errorMessage.append(msg);
   }
}

/**
 * save color file settings.
 */
void 
DisplaySettings::saveSceneColorFile(SceneFile::SceneClass& sc,
                                    const QString& sceneInfoName, 
                                    const ColorFile* cf)
{
   const int numColors = cf->getNumberOfColors();
   for (int i = 0; i < numColors; i++) {
      const ColorFile::ColorStorage* cs = cf->getColor(i);
      sc.addSceneInfo(SceneFile::SceneInfo(sceneInfoName, cs->getName(), cs->getSelected()));
   }
}
