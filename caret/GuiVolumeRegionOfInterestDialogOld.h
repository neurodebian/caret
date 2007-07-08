
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


#ifndef __GUI_VOLUME_REGION_OF_INTEREST_DIALOG_OLD_H__
#define __GUI_VOLUME_REGION_OF_INTEREST_DIALOG_OLD_H__

#include <vector>
#include "QtDialog.h"

class GuiVolumeSelectionControl;
class QComboBox;
class QTabWidget;
class QTextEdit;
class QStackedWidget;
class QDoubleSpinBox;
class VolumeFile;

/// This class creates a dialog for performing region of interest operations on a volume
class GuiVolumeRegionOfInterestDialogOld : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiVolumeRegionOfInterestDialogOld(QWidget* parent);
      
      /// Destructor
      ~GuiVolumeRegionOfInterestDialogOld();
      
      /// update the dialog (typically due to file changes)
      void updateDialog();
      
   protected slots:
      /// called when dialog closed
      void close();
      
      /// slot called to set the selection mode
      void slotSelectionMode(int item);
      
      /// slot called to set the operation mode
      void slotOperationMode(int item);
      
      /// slot called to create probabilistic paint report
      void slotOperationProbabilisticPaint();
      
      /// slot called to report page
      void slotReportClearPushButton();
      
      /// slot called to report page
      void slotReportSavePushButton();
      
      /// slot called to create the segmentation COG report
      void slotOperationSegmentationCOG();
      
      /// slot called to create the paint COG report
      void slotOperationPaintCOG();
      
      /// slot called to create the paint percentage report
      void slotOperationPaintPercentage();
      
   protected:
      /// selection mode (how to select voxels)
      enum SELECTION_MODE {
         SELECTION_MODE_ALL_VOXELS,
         SELECTION_MODE_VOXELS_WITHIN_SEGMENTATION_VOLUME,
         SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_ANATOMY_VOLUME,
         SELECTION_MODE_VOXELS_WITHIN_THRESHOLDED_FUNCTIONAL_VOLUME
      };
      
      /// operations mode
      enum OPERATIONS_MODE {
         OPERATIONS_MODE_PAINT_CENTER_OF_GRAVITY,
         OPERATIONS_MODE_PAINT_PERCENTAGE_REPORT,
         OPERATIONS_MODE_PROBABILISTIC_PAINT,
         OPERATIONS_MODE_SEGMENTATION_CENTER_OF_GRAVITY
      };
      
      /// create the query page
      void createQuerySelectionPage();
      
      /// create the voxel selection ALL section
      void createSelectionAllVoxels(QStackedWidget* parent);
      
      /// create the voxel selection within segmentation section
      void createSelectionWithinSegmentation(QStackedWidget* parent);
      
      /// create the voxel selection thresholded anatomy section
      void createSelectionWithinAnatomyThreshold(QStackedWidget* parent);
      
      /// create the voxel selection thresholded functional section
      void createSelectionWithinFunctionalThreshold(QStackedWidget* parent);
      
      /// create the operations probabilistic paint
      void createOperationsProbabilisticPaint(QStackedWidget* parent);
      
      /// create the operations segmentation center of gravity
      void createOperationsSegmentationCenterOfGravity(QStackedWidget* parent);
      
      /// create the operations paint center of gravity
      void createOperationsPaintCenterOfGravity(QStackedWidget* parent);
      
      /// create the operations paint percentage section
      void createOperationsPaintPercentage(QStackedWidget* parent);
      
      /// create the report page
      void createReportPage();
      
      /// get the valid voxels for a volume (returns number of voxels in ROI, zero => error)
      int getVoxelsInROI(const VolumeFile* vf,
                          std::vector<int>& voxelInROI) ;
                          
      /// create the report header
      void createReportHeader(const VolumeFile* vf,
                              const int totalNumVoxels,
                              const int numVoxelsInROI,
                              const QString& description);
                              
      /// selection section
      QWidget* selectAllVoxelsQVBox;
      
      /// selection section
      QWidget* selectSegmentationVoxelsQVBox;
      
      /// selection section
      QWidget* selectAnatomyVoxelsQVBox;
      
      /// selection section
      QWidget* selectFunctionalVoxelsQVBox;
      
      /// the query widget
      QWidget* queryWidget;
      
      /// the report page
      QWidget* reportPage;
      
      /// voxel selection combo box
      QComboBox* selectionModeComboBox;
      
      /// widget stack for voxel selection
      QStackedWidget* queryControlWidgetStack;
      
      /// widget stack for operations
      QStackedWidget* operationsWidgetStack;
      
      /// operations mode combo box
      QComboBox* operationsModeComboBox;
      
      /// the current selection mode
      SELECTION_MODE selectionMode;
      
      /// the current operations mode
      OPERATIONS_MODE operationsMode;
      
      /// operations mode prob paint
      QWidget* operationsProbabilisticPaintQVBox;
      
      /// operations mode segmentation center of gravity
      QWidget* operationsSegmentationCenterOfGravityQVBox;
      
      /// operations mode paint center of gravity
      QWidget* operationsPaintCenterOfGravityQVBox;
      
      /// operations mode paint percentage
      QWidget* operationsPaintPercentageQVBox;
      
      /// the report text edit
      QTextEdit* reportTextEdit;
      
      /// selection of segmentation volume ROI
      GuiVolumeSelectionControl* selectRoiVolumeSegmentationControl;
      
      /// selection of anatomy volume ROI
      GuiVolumeSelectionControl* selectRoiVolumeAnatomyControl;
      
      /// selection of anatomy volume ROI
      GuiVolumeSelectionControl* selectRoiVolumeFunctionalControl;
      
      /// anatomical volume threshold float spin box
      QDoubleSpinBox* anatomicalVolumeThresholdDoubleSpinBox;
      
      /// functional volume positive threshold float spin box
      QDoubleSpinBox* functionalVolumePositiveThresholdDoubleSpinBox;
      
      /// functional volume negative threshold float spin box
      QDoubleSpinBox* functionalVolumeNegativeThresholdDoubleSpinBox;
      
      /// control for segmentation COG volume
      GuiVolumeSelectionControl* segmentationVolumeCogControl;
      
      /// control for paint COG volume
      GuiVolumeSelectionControl* paintVolumeCogControl;
      
      /// control for paint percentage
      GuiVolumeSelectionControl* paintVolumePercentControl;
      
      /// the tab widget
      QTabWidget* tabWidget;
};

#endif // __GUI_VOLUME_REGION_OF_INTEREST_DIALOG_OLD_H__
