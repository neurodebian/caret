
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
#include "DisplaySettingsStudyMetaData.h"
#include "NameIndexSort.h"
#include "StudyMetaDataFile.h"

/**
 * Constructor.
 */
DisplaySettingsStudyMetaData::DisplaySettingsStudyMetaData(BrainSet* bs)
   : DisplaySettings(bs)
{
   reset();
}

/**
 * Destructor.
 */
DisplaySettingsStudyMetaData::~DisplaySettingsStudyMetaData()
{
}

/**
 * Reinitialize all display settings.
 */
void 
DisplaySettingsStudyMetaData::reset()
{
}

/**
 * Update any selections due to changes in loaded cells.
 */
void 
DisplaySettingsStudyMetaData::update()
{
   updateKeywords();
   updateSubHeaderNames();
}
   
/**
 * get keyword indices sorted by name case insensitive.
 */
void 
DisplaySettingsStudyMetaData::getKeywordIndicesSortedByName(
                                   std::vector<int>& indicesSortedByNameOut,
                                   const bool reverseOrderFlag,
                                   const bool limitToDisplayedFociFlag) const
{
   NameIndexSort nis;
   
   int numKeywords = 0;
   if (limitToDisplayedFociFlag) {
      std::vector<QString> subsetOfKeywords;
      brainSet->getStudyMetaDataFile()->getAllKeywordsUsedByDisplayedFoci(
                                             brainSet->getFociProjectionFile(),
                                             subsetOfKeywords);
      numKeywords = static_cast<int>(subsetOfKeywords.size());
      for (int i = 0; i < numKeywords; i++) {
         //
         // Need to use index of ALL keywords
         //
         nis.add(getKeywordIndexByName(subsetOfKeywords[i]), subsetOfKeywords[i]);
      }
   }
   else {
      numKeywords = getNumberOfKeywords();
      for (int i = 0; i < numKeywords; i++) {
         nis.add(i, getKeywordNameByIndex(i));
      }
   }
   
   nis.sortByNameCaseInsensitive();
   
   indicesSortedByNameOut.resize(numKeywords, 0);
   for (int i = 0; i < numKeywords; i++) {
      indicesSortedByNameOut[i] = nis.getSortedIndex(i);
   }
   
   if (reverseOrderFlag) {
      std::reverse(indicesSortedByNameOut.begin(), indicesSortedByNameOut.end());
   }
}

/**
 * update the keywords.
 */
void 
DisplaySettingsStudyMetaData::updateKeywords()
{
   //
   // Save current selections and clear current
   //
   const std::vector<QString> oldKeywords = keywords;
   const std::vector<bool> oldKeywordsSelected = keywordsSelected;
   const int numOldKeywords = static_cast<int>(oldKeywords.size());
   keywords.clear();
   keywordsSelected.clear();
   
   //
   // get the updated keywords
   //
   const StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
   smdf->getAllKeywords(keywords);
   
   //
   // Update current selection status with old selection status
   //
   const int numKeywords = getNumberOfKeywords();
   if (numKeywords > 0) {
      keywordsSelected.resize(numKeywords);
      std::fill(keywordsSelected.begin(), keywordsSelected.end(), true);
      
      for (int i = 0; i < numOldKeywords; i++) {
         const int indx = getKeywordIndexByName(oldKeywords[i]);
         if (indx >= 0) {
            keywordsSelected[indx] = oldKeywordsSelected[i];
         }
      }
   }
}
      
/**
 * get index of a keyword.
 */
int 
DisplaySettingsStudyMetaData::getKeywordIndexByName(const QString& name) const
{
   const int num = keywords.size();
   for (int i = 0; i < num; i++) {
      if (keywords[i] == name) {
         return i;
      }
   }
   
   return -1;
}
      
/**
 * get a keyword from its index
 */
QString 
DisplaySettingsStudyMetaData::getKeywordNameByIndex(const int indx) const
{
   if (indx < getNumberOfKeywords()) {
      return keywords[indx];
   }
   
   return "";
}

/**
 * get a keyword's selection status.
 */
bool 
DisplaySettingsStudyMetaData::getKeywordSelected(const int indx) const
{
   if (indx < getNumberOfKeywords()) {
      return keywordsSelected[indx];
   }
   return false;
}

/**
 * get a keyword's selection status.  Returns false if keyword does not exist.
 */
bool 
DisplaySettingsStudyMetaData::getKeywordSelected(const QString& name) const
{
   const int indx = getKeywordIndexByName(name);
   if (indx >= 0) {
      return getKeywordSelected(indx);
   }
   return false;
}
      
/**
 * set a keyword's selection status.
 */
void 
DisplaySettingsStudyMetaData::setKeywordSelected(const int indx,
                                        const bool selFlag)
{
   if (indx < getNumberOfKeywords()) {
      keywordsSelected[indx] = selFlag;
   }
}
                        
/**
 * determine studies with selected keywords.
 */
void 
DisplaySettingsStudyMetaData::getStudiesWithSelectedKeywords(
                         std::vector<KEYWORD_STATUS>& studyKeywordStatus) const
{
   studyKeywordStatus.clear();
   
   //
   // get the updated keywords
   //
   const StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
   const int num = smdf->getNumberOfStudyMetaData();
   if (num > 0) {
      studyKeywordStatus.resize(num);
      
      for (int i = 0; i < num; i++) {
         std::vector<QString> studyKeywords;
         const StudyMetaData* smd = smdf->getStudyMetaData(i);
         smd->getKeywords(studyKeywords);
         
         const int numKeywords = static_cast<int>(studyKeywords.size());
         studyKeywordStatus[i] = KEYWORD_STATUS_HAS_NO_KEYWORDS;
         if (numKeywords > 0) {
            studyKeywordStatus[i] = KEYWORD_STATUS_KEYWORD_NOT_SELECTED;
            
            //
            // See if at least one keyword is selected
            //
            for (int j = 0; j < numKeywords; j++) {
               if (getKeywordSelected(studyKeywords[j])) {
                  studyKeywordStatus[i] = KEYWORD_STATUS_KEYWORD_SELECTED;
                  break;
               }
            }
         }
      }
   }
}

/**
 * get subheader indices sorted by name case insensitive.
 */
void 
DisplaySettingsStudyMetaData::getSubHeaderIndicesSortedByName(std::vector<int>& indicesSortedByNameOut,
                                            const bool reverseOrderFlag,
                                            const bool limitToDisplayedFociFlag) const
{
   NameIndexSort nis;
   int numSubHeaderNames = 0;
   
   if (limitToDisplayedFociFlag) {
      std::vector<QString> subsetOfSubHeaderNames;
      brainSet->getStudyMetaDataFile()->getAllTableSubHeaderShortNamesUsedByDisplayedFoci(
                                             brainSet->getFociProjectionFile(),
                                             subsetOfSubHeaderNames);
      numSubHeaderNames = static_cast<int>(subsetOfSubHeaderNames.size());
      for (int i = 0; i < numSubHeaderNames; i++) {
         //
         // Need to use index of ALL subheaders
         //
         nis.add(getSubHeaderIndexByName(subsetOfSubHeaderNames[i]), subsetOfSubHeaderNames[i]);
      }
   }
   else {
      numSubHeaderNames = getNumberOfSubHeaderNames();
      for (int i = 0; i < numSubHeaderNames; i++) {
         nis.add(i, getSubHeaderNameByIndex(i));
      }
   }
   
   nis.sortByNameCaseInsensitive();
   
   indicesSortedByNameOut.resize(numSubHeaderNames, 0);
   for (int i = 0; i < numSubHeaderNames; i++) {
      indicesSortedByNameOut[i] = nis.getSortedIndex(i);
   }
   
   if (reverseOrderFlag) {
      std::reverse(indicesSortedByNameOut.begin(), indicesSortedByNameOut.end());
   }
}
       
/**
 * update the subheaders names.
 */
void 
DisplaySettingsStudyMetaData::updateSubHeaderNames()
{
   //
   // Save current selections and clear current
   //
   const std::vector<QString> oldSubHeaderNames = subHeaderNames;
   const std::vector<bool> oldSubHeaderNamesSelected = subHeaderNamesSelected;
   const int numOldSubHeaderNames = static_cast<int>(oldSubHeaderNames.size());
   subHeaderNames.clear();
   subHeaderNamesSelected.clear();
   
   //
   // Get the updated subheader names
   //
   const StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
   smdf->getAllTableSubHeaderShortNames(subHeaderNames);
   
   //
   // Update current selections status with old selection status
   //
   const int numSubHeaderNames = getNumberOfSubHeaderNames();
   if (numSubHeaderNames > 0) {
      subHeaderNamesSelected.resize(numSubHeaderNames);
      std::fill(subHeaderNamesSelected.begin(), subHeaderNamesSelected.end(), true);
      
      for (int i = 0; i < numOldSubHeaderNames; i++) {
         const int indx = getSubHeaderIndexByName(oldSubHeaderNames[i]);
         if (indx >= 0) {
            subHeaderNamesSelected[indx] = oldSubHeaderNamesSelected[i];
         }
      }
   }
}

/**
 * get index of a subheader name.
 */
int 
DisplaySettingsStudyMetaData::getSubHeaderIndexByName(const QString& name) const
{
   const int num = getNumberOfSubHeaderNames();
   for (int i = 0; i < num; i++) {
      if (subHeaderNames[i] == name) {
         return i;
      }
   }
   return -1;
}

/**
 * get a subh header name by its index.
 */
QString 
DisplaySettingsStudyMetaData::getSubHeaderNameByIndex(const int indx) const
{
   if (indx < getNumberOfSubHeaderNames()) {
      return subHeaderNames[indx];
   }
   return "";
}

/**
 * get a subheader name's selection status.
 */
bool 
DisplaySettingsStudyMetaData::getSubHeaderNameSelected(const int indx) const
{
   if (indx < getNumberOfSubHeaderNames()) {
      return subHeaderNamesSelected[indx];
   }
   return false;
}

/**
 * get a sub header name's selection status.
 */
bool 
DisplaySettingsStudyMetaData::getSubHeaderNameSelected(const QString& name) const
{
   const int indx = getSubHeaderIndexByName(name);
   if (indx >= 0) {
      return getSubHeaderNameSelected(indx);
   }
   return false;
}

/**
 * set a sub header name's selection status.
 */
void 
DisplaySettingsStudyMetaData::setSubHeaderNameSelected(const int indx,
                                                       const bool selFlag)
{
   if (indx < getNumberOfSubHeaderNames()) {
      subHeaderNamesSelected[indx] = selFlag;
   }
}
  
/**
 * update study metadata table subheader selection status.
 */
void 
DisplaySettingsStudyMetaData::updateStudyMetaDataTableSubHeaderSelectionFlags() const   
{
   StudyMetaDataFile* smdf = brainSet->getStudyMetaDataFile();
   const int numMetaData = smdf->getNumberOfStudyMetaData();
   for (int i = 0; i < numMetaData; i++) {
      StudyMetaData* smd = smdf->getStudyMetaData(i);
      const int numTables = smd->getNumberOfTables();
      for (int j = 0; j < numTables; j++) {
         StudyMetaData::Table* table = smd->getTable(j);
         const int numSubHeaders = table->getNumberOfSubHeaders();
         for (int k = 0; k < numSubHeaders; k++) {
            StudyMetaData::SubHeader* sh = table->getSubHeader(k);
            bool selFlag = true;
            const int indx = getSubHeaderIndexByName(sh->getShortName());
            if (indx >= 0) {
               selFlag = getSubHeaderNameSelected(indx);
            }
            sh->setSelected(selFlag);
         }
      }
   }
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsStudyMetaData::showScene(const SceneFile::Scene& scene, QString& errorMessage)
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsStudyMetaData") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if (infoName == "keywords") {
               const QString name = si->getModelName();
               const bool selFlag = si->getValueAsBool();
               
               const int indx = getKeywordIndexByName(name);
               if (indx >= 0) {
                  setKeywordSelected(indx, selFlag);
               }
               else {
                  QString msg("study keyword \"");
                  msg.append(name);
                  msg.append("\" not loaded in memory.\n");
                  errorMessage.append(msg);
               }
            }
            else if (infoName == "subheaders") {
               const QString name = si->getModelName();
               const bool selFlag = si->getValueAsBool();
               
               const int indx = getSubHeaderIndexByName(name);
               if (indx >= 0) {
                  setSubHeaderNameSelected(indx, selFlag);
               }
               else {
                  QString msg("study subheader \"");
                  msg.append(name);
                  msg.append("\" not loaded in memory.\n");
                  errorMessage.append(msg);
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
DisplaySettingsStudyMetaData::saveScene(SceneFile::Scene& scene, const bool /*onlyIfSelected*/,
                             QString& /*errorMessage*/)
{
   SceneFile::SceneClass sc("DisplaySettingsStudyMetaData");
   const int num = getNumberOfKeywords();
   for (int j = 0; j < num; j++) {
      sc.addSceneInfo(SceneFile::SceneInfo("keywords", 
                                           getKeywordNameByIndex(j), 
                                           getKeywordSelected(j)));
   }
   
   const int numSHN = getNumberOfSubHeaderNames();
   for (int j = 0; j < numSHN; j++) {
      sc.addSceneInfo(SceneFile::SceneInfo("subheaders",
                                           getSubHeaderNameByIndex(j),
                                           getSubHeaderNameSelected(j)));
   }
   
   scene.addSceneClass(sc);
}                       
