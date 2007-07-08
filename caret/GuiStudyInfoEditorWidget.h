
#ifndef __GUI_STUDY_INFO_EDITOR_WIDGET_H__
#define __GUI_STUDY_INFO_EDITOR_WIDGET_H__

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

#include <QWidget>

#include "CellStudyInfo.h"

class QComboBox;
class QLineEdit;
class QTextEdit;
class QtWidgetGroup;

/// widget for editing study info
class GuiStudyInfoEditorWidget : public QWidget {
   Q_OBJECT
   
   public:
      // constructor
      GuiStudyInfoEditorWidget(std::vector<CellStudyInfo>* studyInfoIn,
                               QWidget* parent = 0);
      
      // destructor
      ~GuiStudyInfoEditorWidget();
      
      // update the widget
      void updateWidget(std::vector<CellStudyInfo>* studyInfoIn);
      
      // hide stereotaxic space controls
      void hideStereotaxicSpaceControls(const bool hideThem);

      // hide partitioning scheme controls
      void hidePartitioningSchemeControls(const bool hideThem);
            
   public slots:
      // accept changes in editor
      void slotAcceptEditorContents();
      
   protected slots:
      // called when study selection combo box selected
      void slotStudySelectionComboBox(int item);
      
      // called when stereotaxic space push button pressed
      void slotStereotaxicSpacePushButton();
      
   protected:
      /// load the selection combo box
      void loadStudySelectionComboBox();
      
      /// the study info being edited
      std::vector<CellStudyInfo>* studyInfo;
      
      /// study selection combo box
      QComboBox* studySelectionComboBox;
      
      /// title line edit
      QLineEdit* titleLineEdit;
      
      /// authors line edit
      QLineEdit* authorsLineEdit;
      
      /// citation line edit
      QLineEdit* citationLineEdit;
      
      /// url line edit
      QLineEdit* urlLineEdit;
      
      /// keywords line edit
      QLineEdit* keywordsLineEdit;
      
      /// stereotaxic space line edit
      QLineEdit* stereotaxicSpaceLineEdit;
      
      /// partitioning scheme abbreviation line edit
      QLineEdit* partitioningSchemeAbbreviationLineEdit;
      
      /// partitioning scheme full name line edit
      QLineEdit* partitioningSchemeFullNameLineEdit;
      
      /// comment text edit
      QTextEdit* commentTextEdit;
      
      /// stereotaxic space widget group
      QtWidgetGroup* stereotaxicSpaceWidgetGroup;
      
      /// partitioning scheme widget group
      QtWidgetGroup* partitioningSchemeWidgetGroup;
};

#endif // __GUI_STUDY_INFO_EDITOR_WIDGET_H__
