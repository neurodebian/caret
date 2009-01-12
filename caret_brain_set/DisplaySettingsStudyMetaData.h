
#ifndef __DISPLAY_SETTINGS_STUDY_METADATA_H__
#define __DISPLAY_SETTINGS_STUDY_METADATA_H__

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

#include "DisplaySettings.h"

class BrainSet;

      
/// class for display settings of study meta data
class DisplaySettingsStudyMetaData : public DisplaySettings {
   public:
      /// keyword status
      enum KEYWORD_STATUS {
         /// 
         KEYWORD_STATUS_KEYWORD_SELECTED,
         /// 
         KEYWORD_STATUS_KEYWORD_NOT_SELECTED,
         /// 
         KEYWORD_STATUS_HAS_NO_KEYWORDS
      };
      
      // Constructor
      DisplaySettingsStudyMetaData(BrainSet* bs);
      
      // Destructor
      virtual ~DisplaySettingsStudyMetaData();
      
      // Reinitialize all display settings
      virtual void reset();
      
      // Update any selections due to changes in loaded cells
      virtual void update();
   
      // get keywords and number of foci using the keywords
      void getKeywordsAndUsageByFoci(std::vector<QString>& keywordsOut,
                                     std::vector<int>& fociCountForKeywordOut) const;
                                     
      // get keyword indices sorted by name case insensitive
      void getKeywordIndicesSortedByName(std::vector<int>& indicesSortedByNameOut,
                                         const bool reverseOrderFlag,
                                         const bool limitToDisplayedFociFlag) const;
      
      // update the keywords
      void updateKeywords();
      
      /// get the number of keywords
      int getNumberOfKeywords() const { return keywords.size(); }
      
      // get index of a keyword
      int getKeywordIndexByName(const QString& name) const;
      
      // get a keyword from its index
      QString getKeywordNameByIndex(const int indx) const;
      
      // get a keyword's selection status
      bool getKeywordSelected(const int indx) const;
      
      // get a keyword's selection status
      bool getKeywordSelected(const QString& name) const;
      
      // set a keyword's selection status
      void setKeywordSelected(const int indx,
                              const bool selFlag);
      
      // get subheaders and number of foci using the subheaders
      void getSubheadersAndUsageByFoci(std::vector<QString>& subheadersOut,
                                       std::vector<int>& fociCountForSubheadersOut) const;
                                     
      // get subheader indices sorted by name case insensitive
      void getSubHeaderIndicesSortedByName(std::vector<int>& indicesSortedByNameOut,
                                           const bool reverseOrderFlag,
                                           const bool limitToDisplayedFociFlag) const;
      
      // update the subheaders names
      void updateSubHeaderNames();
      
      /// get the number of subheader names
      int getNumberOfSubHeaderNames() const { return subHeaderNames.size(); }
      
      // get index of a subheader name
      int getSubHeaderIndexByName(const QString& name) const;
      
      // get a subh header name by its index
      QString getSubHeaderNameByIndex(const int indx) const;
      
      // get a subheader name's selection status
      bool getSubHeaderNameSelected(const int indx) const;

      // get a sub header name's selection status
      bool getSubHeaderNameSelected(const QString& name) const;
      
      // set a sub header name's selection status
      void setSubHeaderNameSelected(const int indx,
                                    const bool selFlag);
       
      // update study metadata table subheader selection status
      void updateStudyMetaDataTableSubHeaderSelectionFlags() const;
      
      // apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      // create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
         
      // determine studies with selected keywords
      void getStudiesWithSelectedKeywords(std::vector<KEYWORD_STATUS>& studyKeywordStatus) const;

   protected:

      // the keywords
      std::vector<QString> keywords;
      
      // the keyword selection status
      std::vector<bool> keywordsSelected;
      
      // the sub header names
      std::vector<QString> subHeaderNames;
      
      // the sub header selection status
      std::vector<bool> subHeaderNamesSelected;
};

#endif // __DISPLAY_SETTINGS_STUDY_METADATA_H__
