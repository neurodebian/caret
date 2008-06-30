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

#include <set>

#include "BrainModelSurfaceFociSearch.h"
#include "FociProjectionFile.h"
#include "MathUtilities.h"
#include "Structure.h"
#include "StudyMetaDataFile.h"

/**
 * constructor.
 */
BrainModelSurfaceFociSearch::BrainModelSurfaceFociSearch(BrainSet* bsIn,
                                        const StudyMetaDataFile* studyMetaDataFileIn,
                                        FociProjectionFile* fociProjectionFileIn,
                                        const FociSearchSet* fociSearchSetIn,
                                        const SEARCH_MODE searchModeIn,
                                        const bool selectAllFociInMatchingStudiesFlagIn)
   : BrainModelAlgorithm(bsIn),
     studyMetaDataFile(studyMetaDataFileIn),
     fociProjectionFile(fociProjectionFileIn),
     fociSearchSet(fociSearchSetIn),
     searchMode(searchModeIn),
     selectAllFociInMatchingStudiesFlag(selectAllFociInMatchingStudiesFlagIn)
{
   numberOfFociInSearch = 0;
   numberOfFociFromMatchingStudies = 0;
}
                            
/**
 * destructor.
 */
BrainModelSurfaceFociSearch::~BrainModelSurfaceFociSearch()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceFociSearch::execute() throw (BrainModelAlgorithmException)
{
   numberOfFociInSearch = 0;
   numberOfFociFromMatchingStudies = 0;
   
   //
   // Make sure there are search parameters
   //
   const int numSearchParameters = fociSearchSet->getNumberOfFociSearches();
   if (numSearchParameters <= 0) {
      throw BrainModelAlgorithmException("The search contains no search parameters.");
   }
   
   //
   // Spatial search coordinates and range
   //
   std::vector<float> spatialSearchXYZRange(numSearchParameters * 4, 0);
   
   //
   // Check search parameters
   //
   QString msg;
   for (int i = 0; i < numSearchParameters; i++) {
      const FociSearch* fs = fociSearchSet->getFociSearch(i);
      if (fs->getSearchText().isEmpty()) {
         msg += ("Search parameter "
                 + QString::number(i + 1)
                 + " text is empty\n");
      }
      
      //
      // Check spatial searches
      //
      if (fs->getAttribute() == FociSearch::ATTRIBUTE_FOCUS_SPATIAL) {
         const QString regExpText("(\\s|,|;)+");
         const QStringList sl = fs->getSearchText().split(QRegExp(regExpText), 
                                                 QString::SkipEmptyParts);
         if (sl.size() == 4) {
            const int i4 = i * 4;
            spatialSearchXYZRange[i4] = sl.at(0).toDouble();
            spatialSearchXYZRange[i4+1] = sl.at(1).toDouble();
            spatialSearchXYZRange[i4+2] = sl.at(2).toDouble();
            spatialSearchXYZRange[i4+3] = sl.at(3).toDouble();
         }
         else {
            msg += ("ERROR: " 
                    + fs->getSearchText()
                    + " must be 4 floating point numbers (x, y, z, range).\n");
         }
      }
   }
   if (msg.isEmpty() == false) {
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Check foci file
   //
   const int numFoci = fociProjectionFile->getNumberOfCellProjections();
   if (numFoci <= 0) {
      throw BrainModelAlgorithmException("The Foci Projection File contains no foci.");
   }
   
   //
   // PubMed IDs of studies from the matching foci
   //
   std::set<QString> matchingStudiesPubMedIDs;
   
   //
   // Checking only displayed foci
   //
   bool displayedFociOnlyFlag = false;
   switch (searchMode) {
      case SEARCH_MODE_ALL_FOCI:
         displayedFociOnlyFlag = false;
         break;
      case SEARCH_MODE_DISPLAYED_FOCI:
         displayedFociOnlyFlag = true;
         break;
   }
   
   //
   // Loop through the foci
   //
   for (int i = 0; i < numFoci; i++) {
      CellProjection* focus = fociProjectionFile->getCellProjection(i);
      
      // 
      // Should this foci be checked
      //
      bool useIt = false;
      if (displayedFociOnlyFlag) {
         if (focus->getDisplayFlag()) {
            useIt = true;
         }
      }
      else {
         useIt = true;
      }
      
      if (useIt) {
         //
         // Loop through the search parameters
         //
         std::vector<bool> searchResult(numSearchParameters);
         for (int j = 0; j < numSearchParameters; j++) {
            //
            // Get the search parameters
            //
            const FociSearch* search = fociSearchSet->getFociSearch(j);
            searchResult[j] = applySearchToFocus(search, 
                                                 focus,
                                                 &spatialSearchXYZRange[j*4]);
         }
         
         //
         // Perform boolean logic to determine final result
         // Initialize using the current in search status
         //
         bool result = focus->getInSearchFlag();
         for (int j = 0; j < numSearchParameters; j++) {
            const FociSearch* search = fociSearchSet->getFociSearch(j);
            switch (search->getLogic()) {
               case FociSearch::LOGIC_UNION:
                  result = result || searchResult[j];
                  break;
               case FociSearch::LOGIC_INTERSECTION:
                  result = result && searchResult[j];
                  break;
            }
         }
         
         //
         // Set focus' in search flag
         //
         focus->setInSearchFlag(result);
         
         //
         // Update number matching
         //
         if (result) {
            numberOfFociInSearch++;
            
            //
            // Keep track studies from foci in search
            //
            const StudyMetaDataLinkSet smdls = focus->getStudyMetaDataLinkSet();
            const int numLinks = smdls.getNumberOfStudyMetaDataLinks();
            for (int i = 0; i < numLinks; i++) {
               const StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(i);
               const int studyIndex = studyMetaDataFile->getStudyIndexFromLink(smdl);
               if (studyIndex >= 0) {
                  const StudyMetaData* smd = studyMetaDataFile->getStudyMetaData(studyIndex);
                  matchingStudiesPubMedIDs.insert(smd->getPubMedID());
               }
            }
         }
      }
   }
   
   //
   // Should foci from matching studies be added to the search results?
   //
   if (selectAllFociInMatchingStudiesFlag) {
      includeFociInMatchingStudiesIntoSearch(matchingStudiesPubMedIDs);
   }
}

/**
 * include foci in matching studies into search.
 */
void 
BrainModelSurfaceFociSearch::includeFociInMatchingStudiesIntoSearch(const std::set<QString>& matchingStudiesPubMedIDs)
{
   //
   // Loop through the foci
   //
   const int numFoci = fociProjectionFile->getNumberOfCellProjections();
   for (int i = 0; i < numFoci; i++) {
      CellProjection* focus = fociProjectionFile->getCellProjection(i);
      
      //
      // If focus is NOT already in search
      //
      if (focus->getInSearchFlag() == false) {
         //
         // Loop through studies used by focus
         //      
         const StudyMetaDataLinkSet smdls = focus->getStudyMetaDataLinkSet();
         const int numLinks = smdls.getNumberOfStudyMetaDataLinks();
         for (int i = 0; i < numLinks; i++) {
            const StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(i);
            const int studyIndex = studyMetaDataFile->getStudyIndexFromLink(smdl);
            if (studyIndex >= 0) {
               const StudyMetaData* smd = studyMetaDataFile->getStudyMetaData(studyIndex);
               const QString focusPubMedID = smd->getPubMedID();
               
               //
               // Loop through studies that should be displayed
               //
               for (std::set<QString>::const_iterator iter = matchingStudiesPubMedIDs.begin();
                    iter != matchingStudiesPubMedIDs.end();
                    iter++) {
                  //
                  // Do PubMedIDs match??
                  //
                  if (focusPubMedID == *iter) {
                     focus->setInSearchFlag(true);
                     numberOfFociFromMatchingStudies++;
                  }
               }
            }
         }
      }
   }
}
            
/**
 * apply the search to a focus.
 */
bool 
BrainModelSurfaceFociSearch::applySearchToFocus(const FociSearch* fociSearch,
                                                const CellProjection* focus,
                                                const float* spatialXYZRange)
{ 
   //
   // Get the studies associated with the focus
   //
   std::vector<const StudyMetaData*> studies;
   const StudyMetaDataLinkSet smdls = focus->getStudyMetaDataLinkSet();
   const int numLinks = smdls.getNumberOfStudyMetaDataLinks();
   for (int i = 0; i < numLinks; i++) {
      const StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(i);
      const int studyIndex = studyMetaDataFile->getStudyIndexFromLink(smdl);
      if (studyIndex >= 0) {
         const StudyMetaData* smd = studyMetaDataFile->getStudyMetaData(studyIndex);
         studies.push_back(smd);
      }
   }

   //
   // Split on whitespace, comma, or semi-colon
   //
   const QString regExpText("(\\s|,|;)+");
   const QString itemSeparator(";");

   //   
   // Get the text of the attribute
   //
   QString attributeText;
   switch (fociSearch->getAttribute()) {
      case FociSearch::ATTRIBUTE_ALL:
         for (int i = FociSearch::ATTRIBUTE_FOCUS_AREA;
              i <= FociSearch::ATTRIBUTE_STUDY_TITLE;
              i++) {
            const FociSearch::ATTRIBUTE attributeValue =
                         static_cast<FociSearch::ATTRIBUTE>(i);
            const QString s = getAttributeText(attributeValue,
                                               focus,
                                               studies);
            if (s.isEmpty() == false) {
               if (attributeText.isEmpty() == false) {
                  attributeText += itemSeparator;
               }
               attributeText += s;
            }
         }
         break;
      case FociSearch::ATTRIBUTE_FOCUS_SPATIAL:
         {
            float focusSearchXYZ[3];
            focus->getSearchXYZ(focusSearchXYZ);
            if ((focusSearchXYZ[0] != 0.0) ||
                (focusSearchXYZ[1] != 0.0) ||
                (focusSearchXYZ[2] != 0.0)) {
               //
               // Note: elements in spatialXYZRange[] are X, Y, Z, and MAX RANGE
               //
               const float maxRange = spatialXYZRange[3];
               
               const float distSQ = MathUtilities::distanceSquared3D(spatialXYZRange,
                                                                     focusSearchXYZ);
               if (distSQ < (maxRange * maxRange)) {
                  return true;
               }

               return false;
            }
         }
         break;
      case FociSearch::ATTRIBUTE_FOCUS_AREA:
      case FociSearch::ATTRIBUTE_FOCUS_CLASS:
      case FociSearch::ATTRIBUTE_FOCUS_COMMENT:
      case FociSearch::ATTRIBUTE_FOCUS_GEOGRAPHY:
      case FociSearch::ATTRIBUTE_FOCUS_ROI:
      case FociSearch::ATTRIBUTE_FOCUS_STRUCTURE:
      case FociSearch::ATTRIBUTE_STUDY_AUTHORS:
      case FociSearch::ATTRIBUTE_STUDY_CITATION:
      case FociSearch::ATTRIBUTE_STUDY_COMMENT:
      case FociSearch::ATTRIBUTE_STUDY_DATA_FORMAT:
      case FociSearch::ATTRIBUTE_STUDY_DATA_TYPE:
      case FociSearch::ATTRIBUTE_STUDY_KEYWORDS:
      case FociSearch::ATTRIBUTE_STUDY_MESH_TERMS:
      case FociSearch::ATTRIBUTE_STUDY_NAME:
      case FociSearch::ATTRIBUTE_STUDY_STEREOTAXIC_SPACE:
      case FociSearch::ATTRIBUTE_STUDY_TABLE_HEADER:
      case FociSearch::ATTRIBUTE_STUDY_TABLE_SUBHEADER:
      case FociSearch::ATTRIBUTE_STUDY_TITLE:
         attributeText = getAttributeText(fociSearch->getAttribute(),
                                          focus,
                                          studies);
         break;
   }

   //
   // If attribute is empty, search failed
   //
   attributeText = attributeText.trimmed();
   if (attributeText.isEmpty()) {
      return false;
   }
   
   //
   // Process matching
   //
   switch (fociSearch->getMatching()) {
      case FociSearch::MATCHING_ANY_OF:
         {
            //
            // split search terms into a list
            //
            const QStringList searchList = 
               fociSearch->getSearchText().split(itemSeparator, QString::SkipEmptyParts);
            const int numInList = searchList.count();
            
            //
            // See if any search term is in the attribute's text
            //
            for (int i = 0; i < numInList; i++) {
               if (attributeText.contains(searchList.at(i).trimmed(),
                                          Qt::CaseInsensitive)) {
                  return true;
               }
            }
         }
         break;
      case FociSearch::MATCHING_ALL_OF:
         {
            //
            // split search terms into a list
            //
            const QStringList searchList = 
               fociSearch->getSearchText().split(itemSeparator, QString::SkipEmptyParts);
            const int numInList = searchList.count();
            
            //
            // See if any search term is NOT in the attribute's text
            //
            for (int i = 0; i < numInList; i++) {
               if (attributeText.contains(searchList.at(i).trimmed(),
                                          Qt::CaseInsensitive) == false) {
                  return false;
               }
            }
            
            //
            // If we are here, all must have matched
            //
            return true;
         }
         break;
      case FociSearch::MATCHING_NONE_OF:
         {
            //
            // split search terms into a list
            //
            const QStringList searchList = 
               fociSearch->getSearchText().split(itemSeparator, QString::SkipEmptyParts);
            const int numInList = searchList.count();
            
            //
            // See if any search term is in the attribute's text
            //
            for (int i = 0; i < numInList; i++) {
               if (attributeText.contains(searchList.at(i).trimmed(),
                                          Qt::CaseInsensitive)) {
                  return false;
               }
            }
            
            //
            // If we are here, all must NOT have matched
            //
            return true;
         }
         break;
      case FociSearch::MATCHING_EXACT_PHRASE:
         if (attributeText.contains(fociSearch->getSearchText(), Qt::CaseInsensitive)) {
            return true;
         }
         break;
   }
   
   return false;
}

/**
 * get the text for the attribute.
 */
QString 
BrainModelSurfaceFociSearch::getAttributeText(const FociSearch::ATTRIBUTE attribute,
                                  const CellProjection* focus,
                                  std::vector<const StudyMetaData*> studies) const
{
   const QString itemSeparator(";");
   const int numStudies = static_cast<int>(studies.size());

   QString attributeText;
   switch (attribute) {
      case FociSearch::ATTRIBUTE_ALL:
         break;
      case FociSearch::ATTRIBUTE_FOCUS_AREA:
         attributeText = focus->getArea();
         break;
      case FociSearch::ATTRIBUTE_FOCUS_CLASS:
         attributeText = focus->getClassName();
         break;
      case FociSearch::ATTRIBUTE_FOCUS_COMMENT:
         attributeText = focus->getComment();
         break;
      case FociSearch::ATTRIBUTE_FOCUS_GEOGRAPHY:
         attributeText = focus->getGeography();
         break;
      case FociSearch::ATTRIBUTE_FOCUS_ROI:
         attributeText = focus->getRegionOfInterest();
         break;
      case FociSearch::ATTRIBUTE_FOCUS_SPATIAL:
         break;
      case FociSearch::ATTRIBUTE_FOCUS_STRUCTURE:
         attributeText = Structure::convertTypeToString(focus->getCellStructure());
         break;
      case FociSearch::ATTRIBUTE_STUDY_AUTHORS:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getAuthors();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_CITATION:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getCitation();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_COMMENT:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getComment();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_DATA_FORMAT:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getStudyDataFormat();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_DATA_TYPE:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getStudyDataType();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_KEYWORDS:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getKeywords();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_MESH_TERMS:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getMedicalSubjectHeadings();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_NAME:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getName();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_STEREOTAXIC_SPACE:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getStereotaxicSpace();
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_TABLE_HEADER:
         for (int i = 0; i < numStudies; i++) {
            const int numTables = studies[i]->getNumberOfTables();
            for (int j = 0; j < numTables; j++) {
               if (attributeText.isEmpty() == false) {
                  attributeText += itemSeparator;
               }
               const StudyMetaData::Table* table = studies[i]->getTable(j);
               attributeText += table->getHeader();
            }
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_TABLE_SUBHEADER:
         for (int i = 0; i < numStudies; i++) {
            const int numTables = studies[i]->getNumberOfTables();
            for (int j = 0; j < numTables; j++) {
               const StudyMetaData::Table* table = studies[i]->getTable(j);
               const int numSubHeaders = table->getNumberOfSubHeaders();
               for (int k = 0; k < numSubHeaders; k++) {
                  if (attributeText.isEmpty() == false) {
                     attributeText += itemSeparator;
                  }
                  const StudyMetaData::SubHeader* sh = table->getSubHeader(k);
                  attributeText += sh->getShortName();
               }
            }
         }
         break;
      case FociSearch::ATTRIBUTE_STUDY_TITLE:
         for (int i = 0; i < numStudies; i++) {
            if (attributeText.isEmpty() == false) {
               attributeText += itemSeparator;
            }
            attributeText += studies[i]->getTitle();
         }
         break;
   }
   
   return attributeText;
}
                       
      
