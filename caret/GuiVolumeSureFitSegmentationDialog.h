

#ifndef __GUI_VOLUME_SUREFIT_SEGMENTATION_DIALOG_H__
#define __GUI_VOLUME_SUREFIT_SEGMENTATION_DIALOG_H__

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

#include "WuQDialog.h"
#include "VolumeFile.h"

class GuiGraphWidget;
class GuiStructureComboBox;
class GuiVolumeSelectionControl;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSlider;
class QSpinBox;
class QStackedWidget;
class QTextEdit;
class StatisticHistogram;

/// Dialog for segmenting an anatomy volume
class GuiVolumeSureFitSegmentationDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiVolumeSureFitSegmentationDialog(QWidget* parent);

      /// Destructor
      ~GuiVolumeSureFitSegmentationDialog();
   
      /// Update the dialog
      void updateDialog();
      
      /// called to show the dialog (overrides parent's method)
      void show();
      
   protected slots:
      /// called when close button is pressed
      void slotCloseButton();
      
      /// Enable/Disable pushbuttons based upon selections.
      void slotEnableDisablePushButtons();
 
      /// Called when next pushbutton pressed
      void slotNextPushButton();
 
      /// Called when back pushbutton pressed
      void slotBackPushButton();
 
      /// Perform the segmentation (returns true if an error occurred)
      bool performSegmentation();
         
      /// Called when species push button pressed
      void slotSpeciesPushButton();
      
      /// called to enable/disable selection check boxes
      void slotEnableDisableSelectionCheckBoxes();
      
      /// called when gray or white peak radio button selected
      void slotPeakRadioButton();
      
      /// called when directory push button is pressed
      void slotDirectoryPushButton();
      
      /// called when a spec file is selected on spec file page
      void slotSpecFileSelected(const QString& name);
      
      /// called when correct topological errors in fiducial surface checkbox toggled
      void slotGenerateTopologicallyCorrectFiducialSurfaceCheckBox(bool b);
      
      /// called when smooth button pressed
      void slotSmoothHistogramPushButton();
      
   protected:
      /// hind brain removal thresholds
      enum HIND_BRAIN_THRESHOLD {
         /// high threshold
         HIND_BRAIN_THRESHOLD_HIGH,
         /// low threshold
         HIND_BRAIN_THRESHOLD_LOW
      };
      
      /// add a page to the dialog
      void addPage(QWidget* w, const QString& legend);
      
      /// Prepare some pages when they are about to be shown
      void showPage(QWidget* page, const bool backPushButtonPressed = false);

      /// see if the current directory is set to the caret installation directory
      bool currentDirectoryIsCaretInstalltionDirectory();
      
      /// load histogram into graph
      void loadHistogramIntoGraph();
      
      /// create change directory page
      QWidget* createChangeDirectoryPage();
      
      /// Create the subject info page
      QWidget* createSubjectInfoPage();
      
      /// Create the spec file page
      QWidget* createSpecFilePage();
      
      /// Create the volume selection page
      QWidget* createVolumeSelectionPage();
      
      /// Create the volume file type page
      QWidget* createVolumeFileTypePage();
      
      /// Create the volume attributes page
      QWidget* createVolumeAttributesPage();
      
      /// Create the gray white peaks page
      QWidget* createGrayWhitePeaksPage();
      
      /// Create the segmentation selections page
      QWidget* createSegmentationSelectionsPage();
      
      /// Create the segmentation complete page
      QWidget* createSegmentationCompletePage();
      
      /// update the volume attributes page
      void updateVolumeAttributesPage();
      
      /// update the current directory page
      void updateCurrentDirectoryPage();
      
      /// the change directory page widget
      QWidget* changeDirectoryPage;
      
      /// the subject info page widget
      QWidget* subjectInfoPage;
      
      /// the spec file page
      QWidget* specFilePage;
      
      /// the volume selection page
      QWidget* volumeSelectionPage;
      
      /// the volume file type page
      QWidget* volumeFileTypePage;
      
      /// the volume attributes page widget
      QWidget* volumeAttributesPage;
      
      /// the gray white peaks page page widget
      QWidget* grayWhitePeaksPage;
      
      /// gray peak radio button
      QRadioButton* grayPeakRadioButton;
      
      /// white peak radio button
      QRadioButton* whitePeakRadioButton;
      
      /// the segmentation selections page widget
      QWidget* segmentationSelectionsPage;
      
      /// the segmentation complete page widget
      QWidget* segmentationCompletePage;
      
      /// spin box for setting gray matter peak
      QSpinBox* grayPeakSpinBox;
      
      /// spin box for setting white matter peak
      QSpinBox* whitePeakSpinBox;
      
      /// histogram widget for setting peaks
      GuiGraphWidget* peakHistogramWidget;
      
      /// override peak average check box
      QCheckBox* overrideThreshSetCheckBox;
      
      /// spin box for peak average override check box
      QSpinBox* overridePeakSpinBox;
      
      /// estimate peak label
      QLabel* csfPeakLabel;
      
      /// estimate peak label
      QLabel* grayPeakLabel;
      
      /// estimate peak label
      QLabel* whitePeakLabel;
      
      /// the species line edit
      QLineEdit* speciesLineEdit;
      
      /// the subject line edit
      QLineEdit* subjectLineEdit;
      
      /// the structure combo box
      GuiStructureComboBox* structureComboBox;
      
      /// disconnect eye check box
      QCheckBox* disconnectEyeCheckBox;
      
      /// disconnect hindbrain check box
      QCheckBox* disconnectHindbrainCheckBox;
      
      /// disconnect hindbrain hi/lo threshold combo box
      QComboBox* hindLoHiThreshComboBox;
      
      /// cut corpus callosum check box
      QCheckBox* cutCorpusCallosumCheckBox;
      
      /// generate segmentation check box
      QCheckBox* generateSegmentationCheckBox;
      
      /// fill ventricles check box
      QCheckBox* fillVentriclesCheckBox;
      
      /// generate raw and fiducial surface check box
      QCheckBox* generateRawAndFiducialSurfaceCheckBox;
      
      /// generate topologically correct fiducial surface check box
      QCheckBox* generateTopologicallyCorrectFiducialSurfaceCheckBox;
      
      /// limit polygons in generated surfaces check box
      QCheckBox* generateSurfaceLimitPolygonsCheckBox;
      
      /// generate inflated check box
      QCheckBox* generateInflatedSurfaceCheckBox;
      
      /// generate very inflated check box
      QCheckBox* generateVeryInflatedSurfaceCheckBox;
      
      /// generate ellipsoid check box
      QCheckBox* generateEllipsoidSurfaceCheckBox;
      
      /// generate hull check box
      QCheckBox* generateHullCheckBox;
      
      /// automatic error correction check box
      QCheckBox* automaticErrorCorrectionCheckBox;
      
      /// identify sulci check box
      QCheckBox* identifySulciCheckBox;
      
      /// auto save files check box
      QCheckBox* autoSaveFilesCheckBox;
      
      /// label for the volume attributes label
      QLabel* volumeAttributesLabel;
      
      /// volume selection control
      GuiVolumeSelectionControl* volumeSelectionControl;
      
      /// line edit for spec file name
      QLineEdit* specFileNameLineEdit;
      
      /// text edit for spec file comment
      QTextEdit* specFileCommentTextEdit;
      
      /// segmentation time label
      QLabel* segmentationCompleteTimeLabel;
      
      /// segmentation surface handle count
      QLabel* segmentationCompleteHandleLabel;
      
      /// time needed by segmentation algorithm
      float segmentationTime;     
      
      /// initialize graph scale flag
      bool initializeGraphScaleFlag;
      
      /// stacked widget for pages
      QStackedWidget* pagesStackedWidget;
      
      /// label for current page's legend
      QLabel* currentPageLegend;
      
      /// legends for pages
      std::vector<QString> pageLegends;
      
      /// the back push button
      QPushButton* backPushButton;
      
      /// the next push button
      QPushButton* nextPushButton;
      
      /// current directory line edit
      QLineEdit* currentDirectoryLineEdit;
      
      /// current directory label
      QLabel* currentDirectoryLabel;
      
      /// AFNI file type radio button
      QRadioButton* fileTypeAfniRadioButton;
      
      /// NIFTI file type radio button
      QRadioButton* fileTypeNiftiRadioButton;
      
      /// type of volume files to write
      VolumeFile::FILE_READ_WRITE_TYPE typeOfVolumesToWrite;
      
      /// histogram for gray/white peaks
      StatisticHistogram* peakHistogram;
};  

#endif // __GUI_VOLUME_SUREFIT_SEGMENTATION_DIALOG_H__
