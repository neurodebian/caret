
#ifndef __GUI_VOLUME_REGION_OF_INTEREST_DIALOG_H__
#define __GUI_VOLUME_REGION_OF_INTEREST_DIALOG_H__

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

#include "QtDialogWizard.h"

class GuiVolumeSelectionControl;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTextEdit;

/// dialog for performing region of interest operations
class GuiVolumeRegionOfInterestDialog : public QtDialogWizard {
   Q_OBJECT
   
   public:
      // constructor
      GuiVolumeRegionOfInterestDialog(QWidget* parent);
      
      // destructor
      ~GuiVolumeRegionOfInterestDialog();
      
      // update the dialog
      virtual void updateDialog();
      
   protected slots:
      // called to select a paint id for voxel selection
      void slotSelectionPaintVoxelIdPushButton();
       
      // slot called to save report page to a file.
      void slotReportSavePushButton();

      // slot called to clear report page.
      void slotReportClearPushButton();

      // called when select voxels push button pressed
      void slotSelectVoxelsPushButton();
      
      // called when show selected voxels check box
      void slotShowSelectedVoxelsCheckBox(bool val);
      
      // called to clear the selected voxels
      void slotClearSelectedVoxels();
      
      // called when the finished button is pressed
      void slotFinishPushButtonPressed();
      
      // called to choose paint assignment name
      void slotPaintAssignmentNamePushButton();
      
   protected:
      /// selection mode (how to select voxels)
      enum SELECTION_MODE {
         SELECTION_MODE_NONE,
         SELECTION_MODE_ALL_UNDERLAY_VOXELS,
         SELECTION_MODE_ANATOMY_ALL_VOXELS_DISPLAYED,
         SELECTION_MODE_FUNCTIONAL_ALL_VOXELS_DISPLAYED,
         SELECTION_MODE_FUNCTIONAL_VOXEL_RANGE,
         SELECTION_MODE_PAINT_VOXELS_WITH_PAINT_ID,
         SELECTION_MODE_SEGMENTATION_ALL_VOXELS_DISPLAYED
      };
      
      /// operations mode
      enum OPERATIONS_MODE {
         OPERATIONS_MODE_NONE,
         OPERATIONS_MODE_FUNCTIONAL_ASSIGNMENT,
         OPERATIONS_MODE_PAINT_ID_ASSIGNMENT,
         OPERATIONS_MODE_PAINT_REGION_CENTER_OF_GRAVITY,
         OPERATIONS_MODE_PAINT_PERCENTAGE_REPORT,
         OPERATIONS_MODE_PROBABILISTIC_PAINT,
         OPERATIONS_MODE_SEGMENTATION_CENTER_OF_GRAVITY
      };
      
      // called to set the number of selected voxels label
      void setNumberOfSelectedVoxelsLabel();
      
      // called when a page is about to show user should override this
      void pageAboutToShow(QWidget* pageWidget);
      
      // set the pages that are valid and buttons that are valid (all invalid when this called)
      void setValidPagesAndButtons(QWidget* currentWidget);
      
      // create the selection page
      QWidget* createSelectionPage();
      
      // create the operation page
      QWidget* createOperationPage();
      
      // create the operation paint ID assignment page
      QWidget* createOperationPaintIdAssignmentPage();
      
      // create the operation functional value assignment page
      QWidget* createOperationFunctionalValueAssignmentPage();
      
      // create the operation paint region COG page
      QWidget* createOperationPaintRegionCOGPage();
      
      // create the operation paint percent report page
      QWidget* createOperationPaintPercentReportPage();
      
      // create the operation  page
      QWidget* createOperationPageProbPaintOverlap();
      
      // create the operation  page
      QWidget* createOperationPageSegmentationCOG();
      
      // create the output report page
      QWidget* createOutputReportPage();
      
      /// the selection mode
      SELECTION_MODE selectionMode;
      
      /// the operations mode
      OPERATIONS_MODE operationsMode;
      
      /// selection all underlay voxels radio button
      QRadioButton* selectionAllUnderlayVoxelsRadioButton;
      
      /// selection anatomy voxels displayed radio button
      QRadioButton* selectionAnatomyVoxelsDisplayedRadioButton;
      
      /// selection segmentation voxels displayed radio button
      QRadioButton* selectionSegmentationVoxelsDisplayedRadioButton;
      
      /// selection functional voxels displayed radio button
      QRadioButton* selectionFunctionalVoxelsDisplayedRadioButton;
      
      /// selection functional voxel range radio button
      QRadioButton* selectionFunctionalVoxelsRangeRadioButton;
      
      /// selection paint voxel ID radio button
      QRadioButton* selectionPaintVoxelIdRadioButton;
      
      /// the selection page
      QWidget* selectionPage;
      
      /// the operations page
      QWidget* operationsPage;
      
      /// the operation paint ID assignment page
      QWidget* operationPaintIdAssignmentPage;
      
      /// the operation functional value assignment page
      QWidget* operationFunctionalVolumeAssignmentPage;
      
      /// the operation Paint Region COG page
      QWidget* operationPaintRegionCOGPage;
      
      /// the operation Paint Percentage Report page
      QWidget* operationPaintPercentReportPage;
      
      /// the operation Prob Paint Overlap page
      QWidget* operationProbPaintOverlapPage;
      
      /// the operation segmentation cog page
      QWidget* operationSegmentationCOGPage;
      
      /// the output report page
      QWidget* outputReportPage;
      
      /// the operation paint ID assignment radio button
      QRadioButton* operationPaintIdAssignmentRadioButton;
      
      /// the operation functional value assignment radio button
      QRadioButton* operationFunctionalVolumeAssignmentRadioButton;
      
      /// the operation Paint Region COG radio button
      QRadioButton* operationPaintRegionCOGRadioButton;
      
      /// the operation Paint Percentage Report radio button
      QRadioButton* operationPaintPercentReportRadioButton;
      
      /// the operation Prob Paint Overlap radio button
      QRadioButton* operationProbPaintOverlapRadioButton;
      
      /// the operation segmentation cog radio button
      QRadioButton* operationSegmentationCOGRadioButton;
      
      /// functional range minimum double spin box
      QDoubleSpinBox* functionalRangeMinimumDoubleSpinBox;
      
      /// functional range maximum double spin box
      QDoubleSpinBox* functionalRangeMaximumDoubleSpinBox;
      
      /// paint voxel selection push button
      QPushButton* selectionPaintVoxelIdPushButton;
      
      /// paint voxel selection label
      QLabel* selectionPaintNameLabel;
      
      /// control for segmention COG volume selection
      GuiVolumeSelectionControl* segmentationVolumeCogControl;
      
      /// control for paint percentage volume selection
      GuiVolumeSelectionControl* paintVolumePercentControl;
      
      /// control for paint COG volume selection
      GuiVolumeSelectionControl* paintVolumeCogControl;
      
      /// control for paint assignment volume selection
      GuiVolumeSelectionControl* paintVolumeAssignmentControl;
      
      /// line edit for paint volume assignment paint name
      QLineEdit* paintVolumeAssignmentLineEdit;
      
      /// control for functional assignment volume selection
      GuiVolumeSelectionControl* functionalVolumeAssignmentControl;
      
      /// double spin box for functional volume assignment value
      QDoubleSpinBox* functionalVolumeAssignmentValueDoubleSpinBox;
 
      /// text edit for report
      QTextEdit* reportTextEdit;
      
      /// voxels selected text
      QString voxelsSelectedText;
      
      /// number of selected voxels label
      QLabel* numberOfSelectedVoxelsLabel;
};

#endif // __GUI_VOLUME_REGION_OF_INTEREST_DIALOG_H__

