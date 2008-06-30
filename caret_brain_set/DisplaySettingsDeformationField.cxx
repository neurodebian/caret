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



#include "BrainSet.h"
#include "DisplaySettingsDeformationField.h"
#include "DeformationFieldFile.h"

/**
 * The constructor.
 */
DisplaySettingsDeformationField::DisplaySettingsDeformationField(BrainSet* bs)
   : DisplaySettings(bs)
{
   displayMode = DISPLAY_MODE_NONE;
   sparseDistance = 50;
   displayIdentifiedNodes = false;
   showUnstretchedOnFlat = true;
   unstretchedFactor = 2.5;
   reset();
}

/**
 * The destructor.
 */
DisplaySettingsDeformationField::~DisplaySettingsDeformationField()
{
}

/**
 * show vectors unstretched on flat surface
 */
void 
DisplaySettingsDeformationField::getShowUnstretchedOnFlat(float& factor, bool& showIt)
{
   factor = unstretchedFactor;
   showIt = showUnstretchedOnFlat;
}

/**
 * set show vectors unstretched on flat surface
 */
void 
DisplaySettingsDeformationField::setShowUnstretchedOnFlat(const float factor, const bool showIt)
{
   unstretchedFactor = factor;
   showUnstretchedOnFlat = showIt;
}

/**
 * Reinitialize all display settings.
 */
void
DisplaySettingsDeformationField::reset()
{
   displayColumn = 0;
   displayVectorForNode.clear();
}

/**
 * Update any selections due to changes in loaded metric file.
 */
void
DisplaySettingsDeformationField::update()
{
   const int numCol = brainSet->getDeformationFieldFile()->getNumberOfColumns();
   
   if (displayColumn >= numCol) {
      displayColumn = 0;
   }
   
   bool defaultValue = false;
   switch (displayMode) {
      case DISPLAY_MODE_ALL:
         defaultValue = true;
         break;
      case DISPLAY_MODE_NONE:
         defaultValue = false;
         break;
      case DISPLAY_MODE_SPARSE:
         defaultValue = false;
         break;
   }
   const int numNodes = brainSet->getNumberOfNodes();
   displayVectorForNode.resize(numNodes, defaultValue);
   
}

/**
 * set the display mode.
 */
void 
DisplaySettingsDeformationField::setDisplayMode(const DISPLAY_MODE dm) 
{ 
   displayMode = dm; 
   const int numNodes = static_cast<int>(displayVectorForNode.size());

   switch (displayMode) {
      case DISPLAY_MODE_ALL:
         for (int i = 0; i < numNodes; i++) {
            displayVectorForNode[i] = true;
         }
         break;
      case DISPLAY_MODE_NONE:
         for (int i = 0; i < numNodes; i++) {
            displayVectorForNode[i] = false;
         }
         break;
      case DISPLAY_MODE_SPARSE:
         for (int i = 0; i < numNodes; i++) {
            displayVectorForNode[i] = false;
         }
         for (int i = 0; i < numNodes; i += sparseDistance) {
            displayVectorForNode[i] = true;
         }
         break;
   }
}

/**
 * Get the column selected for display.
 * Returns -1 if there are no metric columns available.
 */
int
DisplaySettingsDeformationField::getSelectedDisplayColumn()
{
   if (displayColumn >= brainSet->getDeformationFieldFile()->getNumberOfColumns()) {
      displayColumn = -1;
   }
   else if ((displayColumn < 0) && 
            (brainSet->getDeformationFieldFile()->getNumberOfColumns() > 0)) {
      displayColumn = 0;
   }
   return displayColumn;
}

/**
 * get display vector for node.
 */
bool 
DisplaySettingsDeformationField::getDisplayVectorForNode(const int nodeNum) const
{
   if (nodeNum < static_cast<int>(displayVectorForNode.size())) {
      return displayVectorForNode[nodeNum];
   }
   return false;
}

/**
 * set display vector for node.
 */
void 
DisplaySettingsDeformationField::setDisplayVectorForNode(const int nodeNum,
                                                         const bool status)
{
   if (nodeNum < static_cast<int>(displayVectorForNode.size())) {
      displayVectorForNode[nodeNum] = status;
   }
}

static const QString deformationFieldID("deformation-field-column");

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsDeformationField::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   DeformationFieldFile* dff = brainSet->getDeformationFieldFile();
   setDisplayMode(DISPLAY_MODE_NONE);
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsDeformationField") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == deformationFieldID) {
               showSceneNodeAttributeColumn(si,
                                            dff,
                                            "Deformation Field File",
                                            displayColumn,
                                            errorMessage);
            }
            else if (infoName == "deformation-field-displayMode") {
               setDisplayMode(static_cast<DISPLAY_MODE>(si->getValueAsInt()));
            }
            else if (infoName == "sparseDistance") {
               si->getValue(sparseDistance);
            }
            else if (infoName == "displayIdentifiedNodes") {
               si->getValue(displayIdentifiedNodes);
            }
            else if (infoName == "unstretchedFactor") {
               si->getValue(unstretchedFactor);
            }
            else if (infoName == "showUnstretchedOnFlat") {
               si->getValue(showUnstretchedOnFlat);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsDeformationField::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& /*errorMessage*/)
{
   DeformationFieldFile* dff = brainSet->getDeformationFieldFile();
   
   if (onlyIfSelected) {
      if (dff->getNumberOfColumns() <= 0) {
         return;
      }
      if (displayMode == DISPLAY_MODE_NONE) {
         return;
      }
   }

   SceneFile::SceneClass sc("DisplaySettingsDeformationField");
   
   saveSceneNodeAttributeColumn(sc,
                                deformationFieldID,
                                dff,
                                displayColumn);
                                
   sc.addSceneInfo(SceneFile::SceneInfo("deformation-field-displayMode",
                                        displayMode));
   sc.addSceneInfo(SceneFile::SceneInfo("sparseDistance",
                                        sparseDistance));
   sc.addSceneInfo(SceneFile::SceneInfo("displayIdentifiedNodes",
                                        displayIdentifiedNodes));
   sc.addSceneInfo(SceneFile::SceneInfo("unstretchedFactor",
                                        unstretchedFactor));
   sc.addSceneInfo(SceneFile::SceneInfo("showUnstretchedOnFlat",
                                        showUnstretchedOnFlat));
   scene.addSceneClass(sc);
}
                       

