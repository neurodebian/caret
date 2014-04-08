
#ifndef __GUI_FOCI_SEARCH_WIDGET_H__
#define __GUI_FOCI_SEARCH_WIDGET_H__

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

#include <vector>

#include <QWidget>

#include "SceneFile.h"

class FociProjectionFile;
class FociSearchFile;
class FociSearchSet;
class GuiBrainModelSelectionComboBox;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QScrollArea;
class QSignalMapper;
class QSpinBox;
class QToolButton;
class WuQWidgetGroup;

/// class for searching foci widget
class GuiFociSearchWidget : public QWidget {
   Q_OBJECT
   
   public:
      // constructor
      GuiFociSearchWidget(FociProjectionFile* fociProjectionFileIn,
                          FociSearchFile* fociSearchFileIn,
                          const bool useScrollAreaFlag,
                          QWidget* parent = 0);
      
      // destructor
      ~GuiFociSearchWidget();
      
      /// apply a scene (set display settings)
      void showScene(const SceneFile::Scene& scene,
                     QString& errorMessage);

      /// create a scene (read display settings)
      SceneFile::SceneClass saveScene();
                             
      // update the widget
      void updateWidget(FociProjectionFile* fpf,
                        FociSearchFile* fsf);
                        
   protected slots:
      // called when search selection spin box value is changed
      void slotSearchSelectionSpinBox(int);
      
      // called when search all button pressed
      void slotSearchAllPushButton();
      
      // called when search displayed button pressed
      void slotSearchDisplayedPushButton();
      
      // called when reset button pressed
      void slotResetPushButton();
      
      // called when insert search tool button pressed
      void slotInsertSearchToolButtonClicked(int);
      
      // called when delete search tool button pressed
      void slotDeleteSearchToolButtonClicked(int);
      
      // called when a search logic is changed
      void slotSearchLogicChanged(int);
      
      // called when a search attribute is changed
      void slotSearchAttributeChanged(int);
      
      // called when a search matching is changed
      void slotSearchMatchingChanged(int);
      
      // called when a search text is changed
      void slotSearchTextChanged(int);
      
      // called when an assistance tool button is clicked
      void slotAssistanceToolButtonClicked(int);
      
      // called when add search button pressed
      void slotAddSearchPushButton();
      
      // called when delete search button pressed
      void slotDeleteSearchPushButton();
      
      // called when copy search set button pressed
      void slotCopySearchSetPushButton();
      
      // called when search number button pressed
      void slotSearchNumberPushButton();
      
      // called when search name is changed
      void slotSearchNameLineEdit(const QString&);

   protected:
      // add a search set 
      void addSearchSetToFociSearchFile(FociSearchSet* fss);
      
      // create the search operations group
      QWidget* createSearchOperationsGroup();
      
      // create the search set group
      QWidget* createSearchSetGroup();
      
      // create the search parameters group
      QWidget* createSearchParametersGroup(const bool useScrollAreaFlag);
      
      // create the search surfaces group
      //QWidget* createSearchSurfacesGroup();
      
      // load the foci searches
      void loadFociSearches();
      
      // get the selected search set
      FociSearchSet* getSelectedSearchSet();
      
      // perform the search
      void performSearch(const bool searchDisplayedOnlyFlag);
      
      /// foci projection file that is to be searched
      FociProjectionFile* fociProjectionFile;
      
      /// foci search file
      FociSearchFile* fociSearchFile;
      
      /// search selection spin box
      QSpinBox* searchSelectionSpinBox;
      
      /// scroll area for search widgets
      QScrollArea* searchScrollArea;
      
      /// layout for searches
      QGridLayout* searchGridLayout;
      
      /// search name label
      QLabel* searchNameLabel;
      
      /// search name line edit
      QLineEdit* searchNameLineEdit;
      
      /// widget group for each search
      std::vector<WuQWidgetGroup*> searchWidgetGroups;
      
      /// toolbuttons for inserting new searches
      std::vector<QToolButton*> insertSearchToolButtons;
      
      /// toolbuttons for deleting searches
      std::vector<QToolButton*> deleteSearchToolButtons;
      
      /// combo boxes for logic selection
      std::vector<QComboBox*> logicComboBoxes;
      
      /// combo boxes for attribute selection
      std::vector<QComboBox*> attributeComboBoxes;
      
      /// combo boxes for matching selection
      std::vector<QComboBox*> matchingComboBoxes;
      
      /// line edits for search text
      std::vector<QLineEdit*> searchTextLineEdits;
      
      /// toolbuttons for assistance
      std::vector<QToolButton*> assistanceToolButtons;
      
      /// search results label
      QLabel* searchResultsLabel;
      
      /// show foci from matching studies check box
      QCheckBox* showFociFromMatchingStudiesCheckBox;
      
      /// signal mapper for insert search tool buttons
      QSignalMapper* insertSearchToolButtonSignalMapper;
      
      /// signal mapper for delete search tool buttons
      QSignalMapper* deleteSearchToolButtonSignalMapper;
      
      /// signal mapper for search logic combo boxes
      QSignalMapper* searchLogicComboBoxSignalMapper;
      
      /// signal mapper for search attribute combo boxes
      QSignalMapper* searchAttributeComboBoxSignalMapper;
      
      /// signal mapper for search matching combo boxes
      QSignalMapper* searchMatchingComboBoxSignalMapper;
      
      /// signal mapper for search line edit combo boxes
      QSignalMapper* searchTextLineEditSignalMapper;
      
      /// signal mapper for assistance buttons
      QSignalMapper* assistanceToolButtonSignalMapper;
      
      /// left surface selection combo box
      //GuiBrainModelSelectionComboBox* leftSurfaceSelectionComboBox;
      
      /// right surface selection combo box
      //GuiBrainModelSelectionComboBox* rightSurfaceSelectionComboBox;
      
      /// cerebellum surface selection combo box
      //GuiBrainModelSelectionComboBox* cerebellumSurfaceSelectionComboBox;
};

#endif // __GUI_FOCI_SEARCH_WIDGET_H__
