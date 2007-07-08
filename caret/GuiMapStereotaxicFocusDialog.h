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


#ifndef __VE_GUI_MAP_STEREOTAXIC_FOCUS_DIALOG_H__
#define __VE_GUI_MAP_STEREOTAXIC_FOCUS_DIALOG_H__

#include "QtDialog.h"

class FociFileToPalsProjector;
class GuiStudyInfoEditorWidget;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QTextEdit;
class QVBoxLayout;

/// Dialog for entering Foci.
class GuiMapStereotaxicFocusDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMapStereotaxicFocusDialog(QWidget* parent);
      
      /// Destructor
      ~GuiMapStereotaxicFocusDialog();
      
      /// edit a focus by number
      void editFocus(const int focusNumber);
      
      /// update the dialog (typically called when fiducial foci file changed)
      void updateDialog();
      
   private slots:
      /// called when apply button is pressed
      void applyButtonSlot();
      
      /// load a focus into this dialog
      void loadFocusSlot();
      
      /// delete a focus into this dialog
      void deleteFocusSlot();
      
      /// called when the focus number is changed
      void editFocusNumberChanged();
      
      /// called when focus name button pressed
      void nameButtonSlot();
      
      /// called when class name button pressed
      void classNameButtonSlot();
      
      /// called when a mode button is selected
      void slotModeButtonSelected();
      
      /// called when cortical area button selected.
      void slotCorticalAreaPushButton();

      /// called when Geography button selected.
      void slotGeographyPushButton();

      /// update the study number combo box.
      void slotUpdateStudyNumberComboBox();
      
      /// called when study meta data button pressed
      void slotFocusStudyMetaDataPushButton();
      
   private:
      /// create the enter foci tab page
      void createEnterFociTabPage(QVBoxLayout* layout);
      
      /// process the entry of study information
      void processFocusEntry();
      
      /// tab widget for foci and studies
      QTabWidget* tabWidget;
      
      /// name line edit
      QLineEdit* focusNameLineEdit;
      
      /// class name line edit
      QLineEdit* focusClassNameLineEdit;
      
      /// X coord line edit
      QDoubleSpinBox* focusXCoordDoubleSpinBox;
      
      /// Y coord line edit
      QDoubleSpinBox* focusYCoordDoubleSpinBox;
      
      /// Z coord line edit
      QDoubleSpinBox* focusZCoordDoubleSpinBox;
      
      /// study meta data line edit
      QLineEdit* focusStudyMetaDataLineEdit;
      
      /// focus area
      QLineEdit* focusAreaLineEdit;
      
      /// focus geography
      QLineEdit* focusGeographyLineEdit;
      
      /// focus size
      QDoubleSpinBox* focusSizeDoubleSpinBox;
      
      /// focus statistic
      QLineEdit* focusStatisticLineEdit;
      
      /// focus comment
      QTextEdit* focusCommentTextEdit;
      
      /// focus study combo box
      QComboBox* focusStudyComboBox;
      
      /// edit focus line edit
      QSpinBox* editFocusNumberSpinBox;
      
      /// new foci radio button
      QRadioButton* newRadioButton;
      
      /// edit foci number radio button
      QRadioButton* editRadioButton;
      
      /// load focus push button
      QPushButton* loadFocusPushButton;
      
      /// delete focus push button
      QPushButton* deleteFocusPushButton;
      
      /// the study info editor widget
      GuiStudyInfoEditorWidget* studyInfoEditorWidget;
      
      /// study space line edit
      QLineEdit* studySpaceLineEdit;
      
      /// enter foci page
      QWidget* fociPageVBox;
      
      /// project to active fiducial surface
      QRadioButton* projectionMainWindowFiducialRadioButton;
      
      /// project to PALS atlas surfaces
      QRadioButton* projectionPalsAtlasRadioButton;
      
      /// projector for PALS atlas
      FociFileToPalsProjector* palsProjector;
      
      /// automatic projection group box
      QGroupBox* automaticProjectionGroupBox;
};

#endif // __VE_GUI_MAP_STEREOTAXIC_FOCUS_DIALOG_H__

