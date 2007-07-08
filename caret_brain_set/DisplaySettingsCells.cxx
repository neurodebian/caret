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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX  // needed for min/max in algorithm & numeric_limits
#endif

#include <limits>
#include <set>

#include "BrainSet.h"
#include "CellFile.h"
#include "CellColorFile.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsSection.h"
#include "DisplaySettingsStudyMetaData.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "SectionFile.h"
#include "StudyMetaDataFile.h"

/**
 * Constructor.
 */
DisplaySettingsCells::DisplaySettingsCells(BrainSet* bs)
   : DisplaySettings(bs)
{
   displayCells = false;
   displayVolumeCells = false;
   displayFlatCellsRaised = true;
   displayPasteCellsOnto3D = false;
   cellSize = 4;
   symbolOverride = ColorFile::ColorStorage::SYMBOL_NONE;
   displayCellsOnCorrectHemisphereOnly = true;
   distanceToSurfaceLimit = 1000.0;
   cellDisplayMode = CELL_DISPLAY_MODE_SHOW_ALL;
   cellColorMode = CellBase::CELL_COLOR_MODE_NAME;
   opacity = 1.0;
   displayCellsWithoutClassAssignments = true;
   displayCellsWithoutMatchingColor = true;
   displayCellsWithoutLinkToStudyWithKeywords = true;
   displayCellsWithoutLinkToStudyWithTableSubHeader = true;
   
   reset();
}

/**
 * Destructor.
 */
DisplaySettingsCells::~DisplaySettingsCells()
{
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsCells::reset()
{
}

/**
 * Update selections due to changes in loaded cells.
 */
void
DisplaySettingsCells::update()
{
   determineDisplayedCells(false);
}

/**
 * determine which volume cells should be displayed (set's cell display flags).
 */
void 
DisplaySettingsCells::determineDisplayedVolumeCells(const bool fociFlag)
{
   ColorFile* colorFile = NULL;
   if (fociFlag) {
      colorFile = brainSet->getFociColorFile();
   }
   else {
      colorFile = brainSet->getCellColorFile();
   }
   const int numColors = colorFile->getNumberOfColors();
   
   CellFile* cf = NULL;
   if (fociFlag) {
      cf = brainSet->getVolumeFociFile();
   }
   else {
      cf = brainSet->getVolumeCellFile();
   }

   const int numCells = cf->getNumberOfCells();
   
   for (int j = 0; j < numCells; j++) {
      CellData* cd = cf->getCell(j);
      cd->setDisplayFlag(false);
      
      //
      // Check hemisphere
      //
/*
      bool hemisphereDisplayFlag = false;
      switch(cd->getCellStructure()) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
            hemisphereDisplayFlag = displayLeftHemisphereCells;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
            hemisphereDisplayFlag = displayRightHemisphereCells;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_INVALID:
            hemisphereDisplayFlag = true;
            break;
      }
*/      
      //
      // Check color selected for display
      //
      bool colorDisplayFlag = true;
      const int colorIndex = cd->getColorIndex();
      if ((colorIndex >= 0) && (colorIndex < numColors)) {
         colorDisplayFlag = colorFile->getSelected(colorIndex);
      }
      
      //
      // Check class selected
      //
/*
      bool classDisplayFlag = true;
      if (fiducialDataFile != NULL) {
         const int classIndex = cd->getClassIndex();
         classDisplayFlag = fiducialDataFile->getCellClassSelectedByIndex(classIndex);
      }
*/      
      //
      // Set the cell's display flag.
      //
      cd->setDisplayFlag( //hemisphereDisplayFlag &&
                         colorDisplayFlag
                         //&& classDisplayFlag
                         );
   }
}
      
/**
 * Determine cells that are displayed.
 */
void
DisplaySettingsCells::determineDisplayedCells(const bool fociFlag)
{
   DisplaySettingsSection* dss = brainSet->getDisplaySettingsSection();
   
   determineDisplayedVolumeCells(fociFlag);
    
   ColorFile* colorFile = NULL;
   if (fociFlag) {
      colorFile = brainSet->getFociColorFile();
   }
   else {
      colorFile = brainSet->getCellColorFile();
   }
   const int numColors = colorFile->getNumberOfColors();
   
   //
   // Only use class information in the fiducial foci or cell file
   //
   CellProjectionFile* projFile = NULL;
   if (fociFlag) {
      projFile = brainSet->getFociProjectionFile();
   }
   else {
      projFile = brainSet->getCellProjectionFile();
   }
   
   //
   // Minimum and maximum displayed section
   //
   int minimumSection = std::numeric_limits<int>::min();
   int maximumSection = std::numeric_limits<int>::max();
   if (fociFlag == false) {
      SectionFile* sectionFile = brainSet->getSectionFile();
      if (sectionFile != NULL) {
         bool checkSections = false;
         const int col = dss->getSelectedColumn();
         if ((col >= 0) && (col < sectionFile->getNumberOfColumns())) {
            switch (dss->getSelectionType()) {
               case DisplaySettingsSection::SELECTION_TYPE_SINGLE:
                  checkSections = true;
                  break;
               case DisplaySettingsSection::SELECTION_TYPE_MULTIPLE:
                  checkSections = true;
                  break;
               case DisplaySettingsSection::SELECTION_TYPE_ALL:
                  //
                  // Ignore sections if all sections selected
                  //
                  break;
            }
         }
      
         if (checkSections) {
            if ((col >= 0) &&
                (col < sectionFile->getNumberOfColumns())) {
               minimumSection = dss->getMinimumSelectedSection();
               maximumSection = dss->getMaximumSelectedSection();
            }
         }
      }
   }
         
   const int numCells = projFile->getNumberOfCellProjections();
   
   //
   // For foci, determine if studies have selected keywords
   //
   const StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
   std::vector<DisplaySettingsStudyMetaData::KEYWORD_STATUS> studyKeywordStatus;
   if (fociFlag) {
      const DisplaySettingsStudyMetaData* dssmd = brainSet->getDisplaySettingsStudyMetaData();
      dssmd->getStudiesWithSelectedKeywords(studyKeywordStatus);
      dssmd->updateStudyMetaDataTableSubHeaderSelectionFlags();
   }
   
   for (int j = 0; j < numCells; j++) {
      CellProjection* cp = projFile->getCellProjection(j);
      cp->setDisplayFlag(false);
      
      //
      // Check hemisphere
      //
/*
      bool hemisphereDisplayFlag = false;
      switch(cp->getCellStructure()) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
            hemisphereDisplayFlag = displayLeftHemisphereCells;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
            hemisphereDisplayFlag = displayRightHemisphereCells;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_INVALID:
            hemisphereDisplayFlag = true;
            break;
      }
*/      
      //
      //  Check all/deep/superficial
      //
      bool allDeepSuperficialFlag = true;
      if (fociFlag == false) {
         switch(cellDisplayMode) {
            case DisplaySettingsCells::CELL_DISPLAY_MODE_SHOW_ALL:
               allDeepSuperficialFlag = true;
               break;
            case DisplaySettingsCells::CELL_DISPLAY_MODE_SHOW_DEEP_ONLY:
               if (cp->getSignedDistanceAboveSurface() > 0.0) {
                  allDeepSuperficialFlag = false;
               }
               break;
            case DisplaySettingsCells::CELL_DISPLAY_MODE_SHOW_SUPERFICIAL_ONLY:
               if (cp->getSignedDistanceAboveSurface() < 0.0) {
                  allDeepSuperficialFlag = false;
               }
               break;
         }
      }
      
      //
      // Check distance to surface
      //
      bool distanceToSurfaceFlag = false;
      if (fabs(cp->getSignedDistanceAboveSurface()) < distanceToSurfaceLimit) {
         distanceToSurfaceFlag = true;
      }
      
      //
      // Check color selected for display
      //
      bool colorDisplayFlag = true;
      if (fociFlag) {
         colorDisplayFlag = displayCellsWithoutMatchingColor;
      }
      const int colorIndex = cp->getColorIndex();
      if ((colorIndex >= 0) && (colorIndex < numColors)) {
         colorDisplayFlag = colorFile->getSelected(colorIndex);
      }
      
      //
      // Check class selected
      //
      bool classDisplayFlag = true;
      if (fociFlag) {
         const int classIndex = cp->getClassIndex();
         if ((classIndex >= 0) &&
             (cp->getClassName().isEmpty() == false)) {
            classDisplayFlag = projFile->getCellClassSelectedByIndex(classIndex);
         }
         else {
            classDisplayFlag = displayCellsWithoutClassAssignments;
         }
      }
      
      //
      // Check name selected
      //
      bool nameDisplayFlag = true;
      if (fociFlag) {
         const int uniqueNameIndex = cp->getUniqueNameIndex();
         if ((uniqueNameIndex >= 0) &&
             (cp->getName().isEmpty() == false)) {
            nameDisplayFlag = projFile->getCellUniqueNameSelectedByIndex(uniqueNameIndex);
         }
      }
      
      //
      // Check section for cells
      //
      bool sectionDisplayFlag = true;
      if ((cp->getSectionNumber() < minimumSection) ||
          (cp->getSectionNumber() > maximumSection)) {
         sectionDisplayFlag = false;
      }
      
      //
      // Check foci keywords
      //
      bool keywordDisplayFlag = true;
      if (fociFlag) {
         keywordDisplayFlag = displayCellsWithoutLinkToStudyWithKeywords;
         const StudyMetaDataLink smdl = cp->getStudyMetaDataLink();
         const int smdIndex = smdf->getStudyIndexFromLink(smdl);
         if ((smdIndex >= 0) &&
             (smdIndex < smdf->getNumberOfStudyMetaData())) {
            switch (studyKeywordStatus[smdIndex]) {
               case DisplaySettingsStudyMetaData::KEYWORD_STATUS_KEYWORD_SELECTED:
                  keywordDisplayFlag = true;
                  break;
               case DisplaySettingsStudyMetaData::KEYWORD_STATUS_KEYWORD_NOT_SELECTED:
                  keywordDisplayFlag = false;
                  break;
               case DisplaySettingsStudyMetaData::KEYWORD_STATUS_HAS_NO_KEYWORDS:
                  keywordDisplayFlag = true;
                  break;
            }
         }
      }
      
      //
      // Check foci table subheaders
      //
      bool subHeaderDisplayFlag = true;
      if (fociFlag) {
         subHeaderDisplayFlag = displayCellsWithoutLinkToStudyWithTableSubHeader;
         const StudyMetaDataLink smdl = cp->getStudyMetaDataLink();
         const int smdIndex = smdf->getStudyIndexFromLink(smdl);
         if ((smdIndex >= 0) &&
             (smdIndex < smdf->getNumberOfStudyMetaData())) {
            const StudyMetaData* smd = smdf->getStudyMetaData(smdIndex);
            const QString tableNumber = smdl.getTableNumber();
            const StudyMetaData::Table* table = smd->getTableByTableNumber(tableNumber);
            if (table != NULL) {
               const QString subHeaderNumber = smdl.getTableSubHeaderNumber();
               const StudyMetaData::SubHeader* subHeader = table->getSubHeaderBySubHeaderNumber(subHeaderNumber);
               if (subHeader != NULL) {
                  subHeaderDisplayFlag = subHeader->getSelected();
               }
            }
         }
      }
      
      //
      // Set the cell's display flag.
      //
      cp->setDisplayFlag(//hemisphereDisplayFlag &&
                         allDeepSuperficialFlag &&
                         distanceToSurfaceFlag &&
                         colorDisplayFlag &&
                         classDisplayFlag &&
                         nameDisplayFlag &&
                         sectionDisplayFlag &&
                         keywordDisplayFlag &&
                         subHeaderDisplayFlag);
   }
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsCells::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   const bool fociFlag = (dynamic_cast<DisplaySettingsFoci*>(this) != NULL);
   CellProjectionFile* fidCells = brainSet->getCellProjectionFile();    
   if (fociFlag) {
      fidCells = brainSet->getFociProjectionFile();
   }

   const int numCellProj = fidCells->getNumberOfCellProjections();
   for (int m = 0; m < numCellProj; m++) {
      CellProjection* cp = fidCells->getCellProjection(m);
      cp->setHighlightFlag(false);
   }

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (((sc->getName() == "DisplaySettingsCells") &&
           (fociFlag == false)) ||
          ((sc->getName() == "DisplaySettingsFoci") &&
           fociFlag)) {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "displayCells") {
               si->getValue(displayCells);
            }
            else if (infoName == "displayVolumeCells") {
               si->getValue(displayVolumeCells);
            }
            else if (infoName == "displayFlatCellsRaised") {
               si->getValue(displayFlatCellsRaised);
            }
            else if (infoName == "displayPasteCellsOnto3D") {
               si->getValue(displayPasteCellsOnto3D);
            }
            else if (infoName == "drawMode") { // obsolete
               const int drawMode = si->getValueAsInt();
               if (drawMode == 0) {
                  symbolOverride = ColorFile::ColorStorage::SYMBOL_NONE;
               }
               else if (drawMode == 1) {
                  symbolOverride = ColorFile::ColorStorage::SYMBOL_OPENGL_POINT;
               }
               else if (drawMode == 2) {
                  symbolOverride = ColorFile::ColorStorage::SYMBOL_SPHERE;
               }
            }
            else if (infoName == "cellColorMode") {
               cellColorMode = static_cast<CellBase::CELL_COLOR_MODE>(si->getValueAsInt());
            }
            else if (infoName == "cellSize") {
               si->getValue(cellSize);
            }
/*
            else if (infoName == "displayLeftHemisphereCells") {
               si->getValue(displayLeftHemisphereCells);
            }
            else if (infoName == "displayRightHemisphereCells") {
               si->getValue(displayRightHemisphereCells);
            }
*/
            else if (infoName == "displayCellsOnCorrectHemisphereOnly") {
               si->getValue(displayCellsOnCorrectHemisphereOnly);
            }
            else if (infoName == "distanceToSurfaceLimit") {
               si->getValue(distanceToSurfaceLimit);
            }
            else if (infoName == "cellDisplayMode") {
               cellDisplayMode = static_cast<CELL_DISPLAY_MODE>(si->getValueAsInt());
            }
            else if (infoName == "symbolOverride") {
               symbolOverride = ColorFile::ColorStorage::textToSymbol(si->getValueAsString());
            }
            else if (infoName == "displayCellsWithoutClassAssignments") {
               displayCellsWithoutClassAssignments = si->getValueAsBool();
            }
            else if (infoName == "displayCellsWithoutMatchingColor") {
               displayCellsWithoutMatchingColor = si->getValueAsBool();
            }
            else if (infoName == "displayCellsWithoutLinkToStudyWithKeywords") {
               displayCellsWithoutLinkToStudyWithKeywords = si->getValueAsBool();
            }
            else if (infoName == "displayCellsWithoutLinkToStudyWithTableSubHeader") {
               displayCellsWithoutLinkToStudyWithTableSubHeader = si->getValueAsBool();
            }
            else if (infoName == "color") {
               if (fociFlag) {
                  showSceneColorFile(*si, brainSet->getFociColorFile(),
                                     "Foci", errorMessage);
               }
               else {
                  showSceneColorFile(*si, brainSet->getCellColorFile(),
                                     "Cell", errorMessage);
               }
            }
            else if (infoName == "class") {
               if (fidCells == NULL) {
                  if (fociFlag) {
                     errorMessage.append("No foci are loaded.\n");
                  }
                  else {
                     errorMessage.append("No cells are loaded.\n");
                  }
               }
               else {
                  const QString name = si->getModelName();
                  const bool selFlag = si->getValueAsBool();
                  
                  const int indx = fidCells->getCellClassIndexByName(name);
                  if (indx >= 0) {
                     fidCells->setCellClassSelectedByIndex(indx, selFlag);
                  }
                  else if (name.isEmpty() == false) {
                     QString msg;
                     if (fociFlag) {
                        msg = "Foci ";
                     }
                     else {
                        msg = "Cell ";
                     }
                     msg.append("class \"");
                     msg.append(name);
                     msg.append("\" not found.\n");
                     errorMessage.append(msg);
                  }
               }
            }
            else if (infoName == "names") {
               if (fidCells == NULL) {
                  if (fociFlag) {
                     errorMessage.append("No foci are loaded.\n");
                  }
                  else {
                     errorMessage.append("No cells are loaded.\n");
                  }
               }
               else {
                  const QString name = si->getModelName();
                  const bool selFlag = si->getValueAsBool();
                  
                  const int indx = fidCells->getCellUniqueNameIndexByName(name);
                  if (indx >= 0) {
                     fidCells->setCellUniqueNameSelectedByIndex(indx, selFlag);
                  }
                  else if (name.isEmpty() == false) {
                     QString msg;
                     if (fociFlag) {
                        msg = "Foci ";
                     }
                     else {
                        msg = "Cell ";
                     }
                     msg.append("name \"");
                     msg.append(name);
                     msg.append("\" not found.\n");
                     errorMessage.append(msg);
                  }
               }
            }
            else if (infoName == "HighlightName") {
               for (int m = 0; m < numCellProj; m++) {
                  CellProjection* cp = fidCells->getCellProjection(m);
                  if (cp->getName() == si->getModelName()) {
                     cp->setHighlightFlag(true);
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
DisplaySettingsCells::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   const bool fociFlag = (dynamic_cast<DisplaySettingsFoci*>(this) != NULL);
   
   CellProjectionFile* fidCells = brainSet->getCellProjectionFile();    
   if (fociFlag) {
      fidCells = brainSet->getFociProjectionFile();
   }

   int numCells = fidCells->getNumberOfCellProjections();
   
   if (onlyIfSelected) {
      if (numCells <= 0) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsCells");
   if (fociFlag) {
      sc.setName("DisplaySettingsFoci");
   }
   sc.addSceneInfo(SceneFile::SceneInfo("displayCells", displayCells));     
   sc.addSceneInfo(SceneFile::SceneInfo("displayVolumeCells", displayVolumeCells));     
   sc.addSceneInfo(SceneFile::SceneInfo("displayFlatCellsRaised", displayFlatCellsRaised));   
   sc.addSceneInfo(SceneFile::SceneInfo("displayPasteCellsOnto3D", displayPasteCellsOnto3D));
   sc.addSceneInfo(SceneFile::SceneInfo("symbolOverride", ColorFile::ColorStorage::symbolToText(symbolOverride)));     
   sc.addSceneInfo(SceneFile::SceneInfo("cellSize", cellSize));  
   sc.addSceneInfo(SceneFile::SceneInfo("cellColorMode", static_cast<int>(cellColorMode)));   
   //sc.addSceneInfo(SceneFile::SceneInfo("displayLeftHemisphereCells", displayLeftHemisphereCells));     
   //sc.addSceneInfo(SceneFile::SceneInfo("displayRightHemisphereCells", displayRightHemisphereCells));     
   sc.addSceneInfo(SceneFile::SceneInfo("displayCellsOnCorrectHemisphereOnly", displayCellsOnCorrectHemisphereOnly));     
   sc.addSceneInfo(SceneFile::SceneInfo("distanceToSurfaceLimit", distanceToSurfaceLimit));     
   sc.addSceneInfo(SceneFile::SceneInfo("cellDisplayMode", cellDisplayMode));     
   sc.addSceneInfo(SceneFile::SceneInfo("displayCellsWithoutClassAssignments", displayCellsWithoutClassAssignments));     
   sc.addSceneInfo(SceneFile::SceneInfo("displayCellsWithoutMatchingColor", displayCellsWithoutMatchingColor));     
   sc.addSceneInfo(SceneFile::SceneInfo("displayCellsWithoutLinkToStudyWithKeywords", displayCellsWithoutLinkToStudyWithKeywords));     
   sc.addSceneInfo(SceneFile::SceneInfo("displayCellsWithoutLinkToStudyWithTableSubHeader", displayCellsWithoutLinkToStudyWithTableSubHeader));     

   if (fociFlag) {
      saveSceneColorFile(sc, "color", brainSet->getFociColorFile());
   }
   else {
      saveSceneColorFile(sc, "color", brainSet->getCellColorFile());
   }
   
   //
   // Classes
   //
   if (fidCells != NULL) {
      const int num = fidCells->getNumberOfCellClasses();
      for (int j = 0; j < num; j++) {
         sc.addSceneInfo(SceneFile::SceneInfo("class", fidCells->getCellClassNameByIndex(j), 
                                              fidCells->getCellClassSelectedByIndex(j)));
      }
   }
   
   //
   // unique names
   //
   if (fidCells != NULL) {
      const int num = fidCells->getNumberOfCellUniqueNames();
      for (int j = 0; j < num; j++) {
         sc.addSceneInfo(SceneFile::SceneInfo("names", fidCells->getCellUniqueNameByIndex(j),
                                              fidCells->getCellUniqueNameSelectedByIndex(j)));
      }
   }
   
   //
   // Cell highlighting
   //
   if (fidCells != NULL) {
      //
      // Get list of names of highlighted cells
      //
      std::set<QString> highlightedNames;
      const int num = fidCells->getNumberOfCellProjections();
      for (int i = 0; i < num; i++) {
         const CellProjection* cellProj = fidCells->getCellProjection(i);
         if (cellProj->getHighlightFlag()) {
            highlightedNames.insert(cellProj->getName());
         }
      }
      
      //
      // Add names to scene file
      //
      for (std::set<QString>::const_iterator iter = highlightedNames.begin();
           iter != highlightedNames.end();
           iter++) {
         sc.addSceneInfo(SceneFile::SceneInfo("HighlightName",
                                              *iter,
                                              *iter));
      }
   }
   
   scene.addSceneClass(sc);
}
                       
