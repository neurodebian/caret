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

#include "WuQDialog.h"

class BrainSet;
class GuiVolumeFileOrientationComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QGroupBox;
class GuiGraphWidget;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QSplitter;
class QTextBrowser;
class QTextEdit;
class QVBoxLayout;
class QStackedWidget;
class VolumeFile;

/// dialog for segmentating multiple hemispheres
class GuiVolumeMultiHemSureFitSegmentationDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiVolumeMultiHemSureFitSegmentationDialog(QWidget* parent = 0);
      
      // destructor
      ~GuiVolumeMultiHemSureFitSegmentationDialog();
      
      // update the dialog
      void updateDialog();
      
   protected slots:
      // called when "Prev" pushbutton is pressed
      void slotPrevPushButton();
      
      // called when "Next" pushbutton is pressed
      void slotNextPushButton();
      
      // called when "Close" pushbutton is pressed
      void slotClosePushButton();
      
      // called when "Start" pushbutton is pressed
      void slotStartPushButton();

      // enable/disable buttons
      void slotEnableDisableButtons();
      
      // called when an anatomy volume is selected
      void slotAnatomyVolumeSelected(const QString& name);
      
      // select left hemisphere segmentation volume
      void slotLeftSegmentationVolumePushButton();
      
      // select right hemisphere segmentation volume
      void slotRightSegmentationVolumePushButton();
      
      // select cerebellum hemisphere segmentation volume
      void slotCerebellumSegmentationVolumePushButton();
      
      // called when anatomy species button pressed
      void slotAnatomySpeciesPushButton();
      
      // called when anatomy category button pressed
      void slotAnatomyCategoryPushButton();
      
      // called when an MRI type is selected
      void slotMriTypeSelected();
      
      // resample the anatomy voxels
      void slotAnatomyResampleVoxels();
      
      // save the anatomy volume file
      void slotSaveAnatomyVolumeFile();
      
      // flip anatomy volume about screen X-Axis
      void slotAnatomyVolumeFlipX();
      
      // flip anatomy volume about screen Y-Axis
      void slotAnatomyVolumeFlipY();
      
      // called when align set AC X-Hairs button pressed
      void slotAnatomyCrosshairsAlignAC();
      
      // called when align set PC X-Hairs button pressed
      void slotAnatomyCrosshairsAlignPC();
      
      // called when align set LF X-Hairs button pressed
      void slotAnatomyCrosshairsAlignLF();
      
      // called to AC-PC align a volume
      void slotAcPcAlignAnatomyVolume();
      
      // called to rotate anatomy volume
      void slotAnatomyVolumeRotate();
      
      // called to set origin
      void slotAnatomyVolumeSetOrigin();
      
      // called to remap voxel values
      void slotAnatomyVolumeRemapVoxelValues();
      
      // called to show a histogram for the anatomy volume
      void slotAnatomyVolumeShowHistogram();
      
      // called to perform non-uniformity correction on the anatomy volume
      void slotAnatomyVolumeNonUniformityCorrection();
      
      /// called when gray or white peak radio button selected
      void slotAnatomyPeakRadioButton();
      
   protected:
      /// base class for each of the pages
      class Page : public QWidget {
         public:
            // constructor
            Page(const QString& pageTitle);
            
            // destructor
            ~Page();
            
            /// get the layout
            QVBoxLayout* getLayout()  { return pageLayout; }
            
            /// set the help text
            void setHelp(const QString& s) { helpText = s; }
            
            /// get the help text
            QString getHelp() const { return helpText; }
            
         protected:
            /// the vertical box layout for the widgets added by the user
            QVBoxLayout* pageLayout;
            
            /// the help text
            QString helpText;
      };
      
      /// type of anatomical MRI
      enum ANATOMICAL_MRI_TYPE {
         /// unknown MRI type
         ANATOMICAL_MRI_TYPE_UNKNOWN,
         /// T1 MRI
         ANATOMICAL_MRI_TYPE_T1
      };
      
      // reset the dialog selections
      void resetDialogSelections();
      
      // show the page
      void showPage(const int pageIndex);
      
      // create the instructions page
      void createAllInstructionsPage();
      
      // create the unsaved files page
      void createAllUnsavedFilesPage();
      
      // create the task page
      void createAllTaskPage();
      
      // create the segmentation page
      void createSegmentationChooseFilesPage();
      
      // create the anatomy choose file page
      void createAnatomyChooseFilePage();
      
      // create the volume file type page
      void createAllVolumeFileTypePage();
      
      // create the anatomy choice page
      void createAnatomyMriTypePage();
      
      // create the anatomy standard space page
      void createAnatomyStandardSpacePage();
      
      // create the anatomy choose spec page
      void createAnatomyChooseSpecPage();
      
      // create the anatomy subject information page
      void createAnatomySubjectInformationPage();
      
      // create the anatomy create spec page
      void createAnatomyCreateSpecPage();
      
      // create the orientation page
      void createAnatomyOrientationPage();
      
      // create the voxel size page
      void createAnatomyVoxelSizePage();
      
      // create the origin page
      void createAnatomyOriginPage();
      
      // create the voxel values page
      void createAnatomyVoxelValuesPage();
      
      // create the uniformity page
      void createAnatomyUniformityPage();
      
      // create the crop mask page
      void createAnatomyCropMaskPage();
      
      // create the strip page
      void createAnatomyStripPage();
      
      // create the histogram page
      void createAnatomyHistogramPage();
      
      // create the segmentation operations page
      void createSegmentationOperationsPage();
      
      // create the surface operations page
      void createAllSurfaceOperationsPage();
      
      // create the run page
      void createAllRunPage();
      
      // create the progress page
      void createAllProgressPage();
      
      // update the unsaved files page
      void updateAllUnsavedFilesPage();
      
      // update the segmentation choose files page
      void updateSegmentationChooseFilesPage();
      
      // update the all volume file type page
      void updateAllVolumeFileTypePage();
      
      // read the all volume file type page
      void readAllVolumeFileTypePage();
      
      // choose segmentatinon volume from spec file
      void chooseSegmentationVolumeFromSpecFile(const int indx);
      
      // update the voxel size page
      void updateAnatomyVoxelSizePage();
      
      // update the anatomy orientation page
      void updateAnatomyOrientationPage();
      
      // update the voxel values page
      void updateAnatomyVoxelValuesPage();
      
      // upate the anatomy histogram page
      void updateAnatomyHistogramPage();
      
      // update the anatomy choose spec page
      void updateAnatomyChooseSpecPage();
      
      // create the save anatomy volume file push button
      QPushButton* createSaveAnatomyVolumeFilePushButton();
      
      // check for external programs
      void checkForExternalPrograms();
      
      /// type of MRI being processed
      ANATOMICAL_MRI_TYPE anatomicalMriType;
      
      /// the all instructions page
      Page* pageAllInstructions;
      
      /// the all unsaved files page
      Page* pageAllUnsavedFiles;
      
      /// the all task page
      Page* pageAllTask;
      
      /// the segmentation choose files page
      Page* pageSegmentationChooseFiles;
      
      /// the anatomy choose file page
      Page* pageAnatomyChooseFile;
      
      /// page anatomy subject information
      Page* pageAnatomySubjectInformation;
      
      /// the anatomy create spec page
      Page* pageAnatomyCreateSpec;
      
      /// the anatomy choose spec page
      Page* pageAnatomyChooseSpec;
      
      /// the anatomy volume file type page
      Page* pageAllVolumeFileType;
      
      /// the anatomy mri type page
      Page* pageAnatomyMriType;
      
      /// the anatomy standard space page
      Page* pageAnatomyStandardSpace;
      
      /// the anatomy orientation page
      Page* pageAnatomyOrientation;
      
      /// the anatomy voxel size page
      Page* pageAnatomyVoxelSize;
      
      /// the anatomy origin page
      Page* pageAnatomyOrigin;
      
      /// the anatomy voxel values page
      Page* pageAnatomyVoxelValues;
      
      /// the anatomy uniformity page
      Page* pageAnatomyUniformity;
      
      /// the anatomy crop mask page
      Page* pageAnatomyCropMask;
      
      /// the anatomy strip page
      Page* pageAnatomyStrip;
      
      /// the anatomy histogram page
      Page* pageAnatomyHistogram;
      
      /// the segmentatino operations page
      Page* pageSegmentationOperations;
      
      /// the surface operations page
      Page* pageAllSurfaceOperations;
      
      /// the all run page
      Page* pageAllRun;
      
      /// the all progress page
      Page* pageAllProgress;
      
      /// widget stack for the pages
      QStackedWidget* pageStackedWidget;
      
      /// splitter that splits page widget and help browser
      QSplitter* pageSplitter;
      
      /// the help section
      QTextBrowser* helpBrowser;
      
      /// the prev push button
      QPushButton* prevPushButton;
      
      /// the next push button
      QPushButton* nextPushButton;
      
      /// the close push button
      QPushButton* closePushButton;
      
      /// the start push button
      QPushButton* startPushButton;
      
      /// the unsaved pages text edit
      QTextEdit* unsavedFilesTextEdit;
      
      /// anatomical volume task radio button
      QRadioButton* taskAnatomicalVolumeRadioButton;
      
      /// segment volume task radio button
      QRadioButton* taskSegmentationVolumeRadioButton;
      
      /// anatomical volume task segment left hem check box
      QCheckBox* taskAnatomyProcessLeftCheckBox;
      
      /// anatomical volume task segment right hem check box
      QCheckBox* taskAnatomyProcessRightCheckBox;
      
      /// anatomical volume task segment cerebellum check box
      QCheckBox* taskAnatomyProcessCerebellumCheckBox;
      
      /// the anatomy volume being processed
      VolumeFile* anatomyVolume;
      
      /// brain set for left segmentation
      BrainSet* leftBrainSet;
      
      /// brain set for right segmentation
      BrainSet* rightBrainSet;
      
      /// brain set for cerebellum segmentation
      BrainSet* cerebellumBrainSet;
      
      /// left segmentation volume
      VolumeFile* leftSegmentationVolume;
      
      /// right segmentation volume
      VolumeFile* rightSegmentationVolume;
      
      /// cerebellum segmentation volume
      VolumeFile* cerebellumSegmentationVolume;
      
      /// preferred volume type 
      QRadioButton* volumeFileTypeAfniRadioButton;
      
      /// preferred volume type 
      QRadioButton* volumeFileTypeNiftiRadioButton;
      
      /// preferred volume type 
      QRadioButton* volumeFileTypeSpmRadioButton;
      
      /// preferred volume type 
      QRadioButton* volumeFileTypeWuNilRadioButton;
      
      /// preferred file compressed writing check box
      QCheckBox* volumeFileWriteCompressedCheckBox;
      
      /// line edit for name of anatomy volume file
      QLineEdit* anatomyFileNameLineEdit;
      
      /// segmentation page label
      QLineEdit* leftSegmentationLineEdit;
      
      /// segmentation page label
      QLineEdit* rightSegmentationLineEdit;
      
      /// segmentation page label
      QLineEdit* cerebellumSegmentationLineEdit;
      
      /// mri type T1 radio button
      QRadioButton* anatomyMriTypeT1RadioButton;
      
      /// mri type image label
      QLabel* mriTypeImageLabel;
      
      /// left hemisphere spec file line edit
      QLineEdit* leftAnatomySpecFileLineEdit;
      
      /// right hemisphere spec file line edit
      QLineEdit* rightAnatomySpecFileLineEdit;
      
      /// cerebellum spec file line edit
      QLineEdit* cerebellumAnatomySpecFileLineEdit;
      
      /// left hemisphere anatomy create spec file radio button
      QRadioButton* leftAnatomyCreateSpecFileRadioButton;
      
      /// left hemisphere anatomy use spec file radio button
      QRadioButton* leftAnatomyUseSpecFileRadioButton;
      
      /// right hemisphere anatomy create spec file radio button
      QRadioButton* rightAnatomyCreateSpecFileRadioButton;
      
      /// right hemisphere anatomy use spec file radio button
      QRadioButton* rightAnatomyUseSpecFileRadioButton;
      
      /// cerebellum hemisphere anatomy create spec file radio button
      QRadioButton* cerebellumAnatomyCreateSpecFileRadioButton;
      
      /// cerebellum hemisphere anatomy use spec file radio button
      QRadioButton* cerebellumAnatomyUseSpecFileRadioButton;
      
      /// anatomy species line edit
      QLineEdit* anatomySpeciesLineEdit;
      
      /// anatomy subject line edit
      QLineEdit* anatomySubjectLineEdit;
      
      /// anatomy category line edit
      QLineEdit* anatomyCategoryLineEdit;
      
      /// anatomy create/use left spec group box
      QGroupBox* anatomyLeftCreateUseSpecGroupBox;
      
      /// anatomy create/use right spec group box
      QGroupBox* anatomyRightCreateUseSpecGroupBox;
      
      /// anatomy create/use cerebellum spec group box
      QGroupBox* anatomyCerebellumCreateUseSpecGroupBox;
      
      /// anatomy create left hem spec file group box
      QGroupBox* anatomyCreateLeftSpecFileGroupBox;
      
      /// anatomy create right hem spec file group box
      QGroupBox* anatomyCreateRightSpecFileGroupBox;
      
      /// anatomy create cerebellum spec file group box
      QGroupBox* anatomyCreateCerebellumSpecFileGroupBox;
      
      /// anatomy create left hem spec directory line edit
      QLineEdit* anatomyCreateLeftSpecDirectoryLineEdit;
      
      /// anatomy create right hem spec directory line edit
      QLineEdit* anatomyCreateRightSpecDirectoryLineEdit;
      
      /// anatomy create cerebellum spec directory line edit
      QLineEdit* anatomyCreateCerebellumDirectoryFileLineEdit;
      
      /// anatomy create left hem spec file line edit
      QLineEdit* anatomyCreateLeftSpecFileLineEdit;
      
      /// anatomy create right hem spec file line edit
      QLineEdit* anatomyCreateRightSpecFileLineEdit;
      
      /// anatomy create cerebellum spec file line edit
      QLineEdit* anatomyCreateCerebellumSpecFileLineEdit;
      
      /// anatomy voxel size X spin box
      QDoubleSpinBox* anatomyVoxelSizeXSpinBox;
      
      /// anatomy voxel size Y spin box
      QDoubleSpinBox* anatomyVoxelSizeYSpinBox;
      
      /// anatomy voxel size Z spin box
      QDoubleSpinBox* anatomyVoxelSizeZSpinBox;
      
      /// anatomy voxel size text edit
      QTextEdit* anatomyVoxelSizeTextEdit;
      
      /// anatomy volume X orientation combo box
      GuiVolumeFileOrientationComboBox* anatomyVolumeXOrientationComboBox;
      
      /// anatomy volume Y orientation combo box
      GuiVolumeFileOrientationComboBox* anatomyVolumeYOrientationComboBox;
      
      /// anatomy volume Z orientation combo box
      GuiVolumeFileOrientationComboBox* anatomyVolumeZOrientationComboBox;

      /// aligment AC X
      QSpinBox* alignAcXSpinBox;
      
      /// aligment AC Y
      QSpinBox* alignAcYSpinBox;
      
      /// aligment AC Z
      QSpinBox* alignAcZSpinBox;
      
      /// aligment PC X
      QSpinBox* alignPcXSpinBox;
      
      /// aligment PC Y
      QSpinBox* alignPcYSpinBox;
      
      /// aligment PC Z
      QSpinBox* alignPcZSpinBox;
      
      /// aligment LF X
      QSpinBox* alignLfXSpinBox;
      
      /// aligment LF Y
      QSpinBox* alignLfYSpinBox;
      
      /// aligment LF Z
      QSpinBox* alignLfZSpinBox;
      
      /// line edit for anatomy volume minimum voxel value
      QLineEdit* anatomyVolumeVoxelMinValueLineEdit;
      
      /// line edit for anatomy volume maximum voxel value
      QLineEdit* anatomyVolumeVoxelMaxValueLineEdit;
      
      /// directory containing segmentation image files
      QString imageDirectory;
      
      /// anatomy volume non-uniformity gray min spin box
      QSpinBox* anatomyVolumeNonUniformITKGrayMinSpinBox;
      
      /// anatomy volume non-uniformity gray max spin box
      QSpinBox* anatomyVolumeNonUniformITKGrayMaxSpinBox;
      
      /// anatomy volume non-uniformity white min spin box
      QSpinBox* anatomyVolumeNonUniformITKWhiteMinSpinBox;
      
      /// anatomy volume non-uniformity white max spin box
      QSpinBox* anatomyVolumeNonUniformITKWhiteMaxSpinBox;
      
      /// anatomy volume non-uniformity threshold low spin box
      QSpinBox* anatomyVolumeNonUniformITKTheshLowSpinBox;
      
      /// anatomy volume non-uniformity threshold high spin box
      QSpinBox* anatomyVolumeNonUniformITKThreshHighSpinBox;
      
      /// anatomy volume non-uniformity X iterations spin box
      QSpinBox* anatomyVolumeNonUniformITKXIterSpinBox;
      
      /// anatomy volume non-uniformity Y iterations spin box
      QSpinBox* anatomyVolumeNonUniformITKYIterSpinBox;
      
      /// anatomy volume non-uniformity Z iterations spin box
      QSpinBox* anatomyVolumeNonUniformITKZIterSpinBox;
            
      /// anatomy non uniformity AFNI algorithm radio button
      QRadioButton* anatomyVolumeNonUniformAlgorithmAFNIRadioButton;
      
      /// anatomy non-uniformity ITK algorithm radio button
      QRadioButton* anatomyVolumeNonUniformAlgorithmITKRadioButton;
      
      /// widget stack for anatomy non-uniformity algorithm parameters
      QStackedWidget* anatomyVolumeNonUniformStackedWidget;
      
      /// widget for antatomy non-uniformity AFNI algorithm
      QWidget* anatomyVolumeNonUniformAFNIWidget;
      
      /// widget for antatomy non-uniformity ITK algorithm
      QWidget* anatomyVolumeNonUniformITKWidget;
      
      /// anatomy non-uniformity AFNI gray min spin box
      QSpinBox* anatomyVolumeNonUniformAFNIGrayMinSpinBox;
      
      /// anatomy non-uniformity AFNI white max spin box
      QSpinBox* anatomyVolumeNonUniformAFNIWhiteMaxSpinBox;
      
      /// anatomy non-uniformity AFNI iterations spin box
      QSpinBox* anatomyVolumeNonUniformAFNIIterationsSpinBox;
      
      /// spin box for setting gray matter peak
      QSpinBox* anatomyGrayPeakSpinBox;

      /// spin box for setting white matter peak
      QSpinBox* anatomyWhitePeakSpinBox;

      /// histogram widget for setting peaks
      GuiGraphWidget* anatomyPeakHistogramWidget;

      /// override peak average check box
      QCheckBox* anatomyOverrideThreshSetCheckBox;

      /// spin box for peak average override check box
      QSpinBox* anatomyOverridePeakSpinBox;

      /// gray peak radio button
      QRadioButton* anatomyGrayPeakRadioButton;

      /// white peak radio button
      QRadioButton* anatomyWhitePeakRadioButton;
      
      /// have AFNI 3duniformize program in path
      bool haveAfni3dUniformizeProgramFlag;
      
      
      /// initialize 

};

