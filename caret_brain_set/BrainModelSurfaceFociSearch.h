
#ifndef __BRAIN_MODEL_SURFACE_FOCI_SEARCH_H__
#define __BRAIN_MODEL_SURFACE_FOCI_SEARCH_H__

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

#include "BrainModelAlgorithm.h"
#include "FociSearchFile.h"

class CellProjection;
class FociProjectionFile;
class StudyMetaData;
class StudyMetaDataFile;

/// class for searching foci
class BrainModelSurfaceFociSearch : public BrainModelAlgorithm {
   public:
      /// search mode
      enum SEARCH_MODE {
         /// search all foci
         SEARCH_MODE_ALL_FOCI,
         /// search only those marked as searched
         SEARCH_MODE_DISPLAYED_FOCI
      };
      
      // constructor
      BrainModelSurfaceFociSearch(BrainSet* bsIn,
                                  const StudyMetaDataFile* studyMetaDataFileIn,
                                  FociProjectionFile* fociProjectionFileIn,
                                  const FociSearchSet* fociSearchSetIn,
                                  const SEARCH_MODE searchModeIn,
                                  const bool selectAllFociInMatchingStudiesFlagIn);
                                  
      // destructor
      ~BrainModelSurfaceFociSearch();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
                  
      /// get the number of foci meeting search parameters
      int getNumberOfMatchingFoci() const { return numberOfFociInSearch; }
      
      /// get the number of foci from matching studies
      int getNumberOfFociFromMatchingStudies() const { return numberOfFociFromMatchingStudies; }
      
   protected:
      // apply the search to a focus
      bool applySearchToFocus(const FociSearch* fociSearch,
                              const CellProjection* focus,
                              const float* spatialXYZRange);
      
      // include foci in matching studies into search
      void includeFociInMatchingStudiesIntoSearch(const std::set<QString>& matchingStudiesPubMedIDs);
      
      // get the text for the attribute
      QString getAttributeText(const FociSearch::ATTRIBUTE attribute,
                               const CellProjection* focus,
                               std::vector<const StudyMetaData*> studies) const;
                       
      /// the study meta data file
      const StudyMetaDataFile* studyMetaDataFile;
      
      /// the foci projection file
      FociProjectionFile* fociProjectionFile;
      
      /// the foci search set
      const FociSearchSet* fociSearchSet;
      
      /// the search mode
      const SEARCH_MODE searchMode;
      
      /// also select foci in matching studies
      const bool selectAllFociInMatchingStudiesFlag;

      /// number of foci meeting search parameters
      int numberOfFociInSearch;
      
      /// number of foci from matching studies
      int numberOfFociFromMatchingStudies;      
};

#endif // __BRAIN_MODEL_SURFACE_FOCI_SEARCH_H__

