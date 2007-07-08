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
#include "DisplaySettingsSection.h"
#include "SectionFile.h"

#include "FileUtilities.h"

/**
 * The constructor.
 */
DisplaySettingsSection::DisplaySettingsSection(BrainSet* bs)
   : DisplaySettings(bs)
{
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsSection::~DisplaySettingsSection()
{
}

/**
 * Reinitialize all display settings
 */
void
DisplaySettingsSection::reset()
{
   selectedColumn = 0;
   selectionType = SELECTION_TYPE_ALL;
   updateSectionSelections();
}

/**
 * Update any selections due to changes in loaded areal estimation file
 */
void
DisplaySettingsSection::update()
{
   updateSelectedColumnIndex(brainSet->getSectionFile(), selectedColumn);  
   updateSectionSelections();
}

/**
 * Get the selected column.
 */
int 
DisplaySettingsSection::getSelectedColumn() const 
{ 
   if (selectedColumn >= brainSet->getSectionFile()->getNumberOfColumns()) {
      selectedColumn = 0;
   }
   updateSectionSelections();
   return selectedColumn; 
}

/**
 * Set the selected file index.
 */
void 
DisplaySettingsSection::setSelectedColumn(const int col) 
{ 
   selectedColumn = col; 
   updateSectionSelections();
}

static const QString sectionID("section-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsSection::showScene(const SceneFile::Scene& scene, QString& /*errorMessage*/) 
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsSection") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();
            if (infoName == "selectionType") {
               selectionType = static_cast<SELECTION_TYPE>(si->getValueAsInt());
            }
            else if (infoName == "selectedColumn") {
               si->getValue(selectedColumn);
            }
            else if (infoName == "minimumSelectedSection") {
               si->getValue(minimumSelectedSection);
            }
            else if (infoName == "maximumSelectedSection") {
               si->getValue(maximumSelectedSection);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsSection::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   SectionFile* sf = brainSet->getSectionFile();
   if (onlyIfSelected) {
      if (sf->getNumberOfColumns() <= 0) {
         return;
      }
   }
   
   SceneFile::SceneClass sc("DisplaySettingsSection");
   
   sc.addSceneInfo(SceneFile::SceneInfo("selectedColumn",
                                        selectedColumn));

   sc.addSceneInfo(SceneFile::SceneInfo("selectionType",
                                        static_cast<int>(selectionType)));

   sc.addSceneInfo(SceneFile::SceneInfo("minimumSelectedSection",
                                        minimumSelectedSection));

   sc.addSceneInfo(SceneFile::SceneInfo("maximumSelectedSection",
                                        maximumSelectedSection));

   scene.addSceneClass(sc);
}

/**
 * get the minimum selected section
 */
int 
DisplaySettingsSection::getMinimumSelectedSection() const
{
   updateSectionSelections();
   return minimumSelectedSection;
}

/**
 * get the maximum selected section
 */
int 
DisplaySettingsSection::getMaximumSelectedSection() const
{
   updateSectionSelections();
   return maximumSelectedSection;
}

/**
 * update section selections.
 */
void 
DisplaySettingsSection::updateSectionSelections() const
{
   SectionFile* sf = brainSet->getSectionFile();
   if ((selectedColumn < 0) ||
       (selectedColumn >= sf->getNumberOfColumns())) {
      selectedColumn = 0;
   }
   
   if (selectedColumn < sf->getNumberOfColumns()) {
      maximumSelectedSection = std::min(maximumSelectedSection, sf->getMaximumSection(selectedColumn));
      maximumSelectedSection = std::max(maximumSelectedSection, sf->getMinimumSection(selectedColumn));
      
      minimumSelectedSection = std::max(minimumSelectedSection, sf->getMinimumSection(selectedColumn));
      minimumSelectedSection = std::min(minimumSelectedSection, sf->getMaximumSection(selectedColumn));
   }
}

/**
 * set the minimum selected section
 */
void 
DisplaySettingsSection::setMinimumSelectedSection(const int sect)
{
   minimumSelectedSection = sect;
}

/**
 * set the maximum selected section
 */
void 
DisplaySettingsSection::setMaximumSelectedSection(const int sect)
{
   maximumSelectedSection = sect;
}

/**
 * get the section type
 */
DisplaySettingsSection::SELECTION_TYPE 
DisplaySettingsSection::getSelectionType() const
{
   return selectionType;
}

/**
 * set the selection type
 */
void 
DisplaySettingsSection::setSelectionType(SELECTION_TYPE type)
{
   selectionType = type;
}     
 

