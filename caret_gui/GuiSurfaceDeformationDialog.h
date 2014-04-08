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


#ifndef __GUI_SURFACE_DEFORMATION_DIALOG_H__
#define __GUI_SURFACE_DEFORMATION_DIALOG_H__

#include <sstream>

#include <QLineEdit>
#include <QPushButton>

#include "WuQDialog.h"

#include "DeformationMapFile.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QRadioButton;
class QSpinBox;
class QTabWidget;

class QDoubleSpinBox;
class SpecFile;
class WuQWidgetGroup;

/// class for keeping track of deformation data files
class DeformationDataFiles {
   public:
      /// data file types
      enum DATA_FILE_TYPES {
         DATA_FILE_BORDER_FLAT,
         DATA_FILE_BORDER_FLAT_LOBAR,
         DATA_FILE_BORDER_SPHERICAL,
         DATA_FILE_BORDER_PROJECTION,
         DATA_FILE_TOPO_CLOSED,
         DATA_FILE_TOPO_CUT,
         DATA_FILE_TOPO_CUT_LOBAR,
         DATA_FILE_COORD_FIDUCIAL,
         DATA_FILE_COORD_FLAT,
         DATA_FILE_COORD_FLAT_LOBAR,
         DATA_FILE_COORD_SPHERICAL
      };
      
     /// constructor
     DeformationDataFiles();
     
     /// destructor
     ~DeformationDataFiles();
     
     /// load data files from a spec file
     void loadSpecFile(const SpecFile& sf, const bool flatDeformFlag);
     
     /// border file names
     std::vector<QString> borderFileNames;
     
     /// border file types
     std::vector<DATA_FILE_TYPES> borderFileTypes;
     
     /// selected border file
     int borderFileSelected[DeformationMapFile::MAX_SPHERICAL_STAGES];
     
     /// file names
     std::vector<QString> closedTopoFileNames;
     
     /// file types
     std::vector<DATA_FILE_TYPES> closedTopoFileTypes;
     
     /// selected file
     int closedTopoFileSelected;
     
     /// file names
     std::vector<QString> cutTopoFileNames;
     
     /// file types
     std::vector<DATA_FILE_TYPES> cutTopoFileTypes;
     
     /// selected file
     int cutTopoFileSelected;
     
     /// file names
     std::vector<QString> fiducialCoordFileNames;
     
     /// file types
     std::vector<DATA_FILE_TYPES> fiducialCoordFileTypes;
     
     /// selected file
     int fiducialCoordFileSelected;
     
     /// file names
     std::vector<QString> sphericalCoordFileNames;
     
     /// file types
     std::vector<DATA_FILE_TYPES> sphericalCoordFileTypes;
     
     /// selected file
     int sphericalCoordFileSelected;
     
     /// file names
     std::vector<QString> flatCoordFileNames;
     
     /// file types
     std::vector<DATA_FILE_TYPES> flatCoordFileTypes;
     
     /// selected file
     int flatCoordFileSelected;
     
};

/// class for an entry line
class FileEntryLine {
   public:
      /// constructor
      FileEntryLine(QPushButton* pushButtonIn,
                    QLineEdit* lineEditIn,
                    QPushButton* extraPushButtonIn = NULL) {
         this->pushButton = pushButtonIn;
         this->lineEdit = lineEditIn;
         this->extraButton = extraPushButtonIn;
      }

      /// destructor
      ~FileEntryLine() { }

      /// set the widgets visible
      void setVisible(bool visible) {
         this->pushButton->setVisible(visible);
         this->lineEdit->setVisible(visible);
         if (this->extraButton != NULL) {
            this->extraButton->setVisible(visible);
         }
      }

      /// set the widgets enabled
      void setEnabled(bool enable) {
         this->pushButton->setEnabled(enable);
         this->lineEdit->setEnabled(enable);
         if (this->extraButton != NULL) {
            this->extraButton->setEnabled(enable);
         }
      }

      /// the push button
      QPushButton* pushButton;

      /// the line edit
      QLineEdit* lineEdit;

      /// the extra button
      QPushButton* extraButton;

};
/// Dialog for deforming one surface to another
class GuiSurfaceDeformationDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiSurfaceDeformationDialog(QWidget* parent,
                                  const DeformationMapFile::DEFORMATION_TYPE deformationTypeIn);
      
      /// destructor
      ~GuiSurfaceDeformationDialog();
      
   private slots:
      /// Called when an atlas file selection is made
      void atlasFileSelection(int itemNum);
      
      /// Called when an indiv file selection is made
      void indivFileSelection(int itemNum);
      
      /// called when load deformation map file push button is pressed
      void loadDeformationMapFilePushButton();
      
      /// called when save deformation map file push button is pressed
      void saveDeformationMapFilePushButton();

      /// called when number of spherical cycles is changed
      void slotSphereNumberOfCyclesSpinBox(int item);
      
      /// called when sphere edit cycle is changed
      void slotSphereEditCycleSpinBox(int value);
      
      /// called when number of spherical stages is changed
      void slotSphereNumberOfStagesSpinBox(int item);

      /// called when sphere edit stage is changed
      void slotSphereEditStageSpinBox(int value);

      /// called when Use Standard Parameters button pressed
      void slotStandardParametersPushButton();
      
      /// called to edit individual borders
      void slotBordersEditIndividual();
      
      /// called to edit atlas borders
      void slotBordersEditAtlas(int index);
      
      /// called to update deformation map file names
      void slotUpdateDeformationMapFileNames();

      /// called when a spherical algorithm is selected
      void slotSphericalAlgorithmSelection();

      /// called when apply button pressed
      void slotApplyButton();

      /// called to read the deformation parameters from the dialog
      void slotDeformationParameterChanged();

      /// called to read the flat parameters from the dialog
      void slotFlatParameterChanged();

      /// called when a smoothing parameter is altered
      void slotSmoothingParameterChanged();

      /// called when a morphing parameter is changed
      void slotMorphingParameterChanged();

      /// called when a landmark vector parameter is changed
      void slotLandmarkVectorParameterChanged();

      /// called if sphere resolution is changed
      void slotSphereResolutionChanged();

      /// called when a correct for spherical distortion changed
      void slotCorrectSphericalDistortionChanged();

   private:
      /// file selection types
      enum SELECTION_TYPE {
         SELECTION_TYPE_ATLAS,
         SELECTION_TYPE_INDIV
      };
      
      /// Unique ID's for file types
      enum FILE_TYPES {
         FILE_TYPE_SPEC,
         FILE_TYPE_TOPO_CLOSED,
         FILE_TYPE_TOPO_CUT,
         FILE_TYPE_COORD_FIDUCIAL,
         FILE_TYPE_COORD_FLAT,
         FILE_TYPE_COORD_SPHERICAL,
         FILE_TYPE_BORDER_1,
         FILE_TYPE_BORDER_2,
         FILE_TYPE_BORDER_3,
         FILE_TYPE_BORDER_4,
         FILE_TYPE_BORDER_5,
         FILE_TYPE_BORDER_6,
         FILE_TYPE_BORDER_7,
         FILE_TYPE_BORDER_8,
         FILE_TYPE_BORDER_9,
         FILE_TYPE_BORDER_10,
         FILE_TYPE_BORDER_11,
         FILE_TYPE_BORDER_12,
         FILE_TYPE_BORDER_13,
         FILE_TYPE_BORDER_14,
         FILE_TYPE_BORDER_15,
         FILE_TYPE_BORDER_16,
         FILE_TYPE_BORDER_17,
         FILE_TYPE_BORDER_18,
         FILE_TYPE_BORDER_19,
         FILE_TYPE_BORDER_20
      };
      
      /// Add to the command.
      void addToCommand(std::ostringstream& commandStr, const QString& cmd,
                        const QString& value, const QString& value2 = "");
             
      /// load the deformation map file
      void loadDeformationMapFile(std::vector<QString>& errorMessages);
      
      /// create the atlas widget
      void createAtlasWidget();
      
      /// create the individual widget
      void createIndividualWidget();
      
      /// create the parameters widget
      void createParametersWidget();
      
      /// create the deformation widget
      void createDeformationWidget();
      
      /// create the border resampling section
      QWidget* createBorderResampling();

      /// create the metric deformation section
      QWidget* createMetricDeformation();

      /// create the spherical parameters section
      QWidget* createSphericalParameters();

      /// create the spherical algorithm section
      QWidget* createSphericalAlgorithmSection();

      /// create the landmark vector options
      QWidget* createLandmarkVectorOptionsSection();

      /// create the flat parameters section
      QWidget* createFlatParameters();

      /// create the morphing parameters section
      QWidget* createMorphingParametersSection();

      /// create the smoothing parameters section
      QWidget* createSmoothingParametersSection();

      /// create the Spherical parameters section
      QWidget* createSphericalParametersSection();

      /// display the atlas data files
      void displayAtlasFiles();
      
      /// display the indiv data files
      void displayIndivFiles();
      
      /// display dialog for deformation data file selection
      void displayFileSelection(const QString& title,
                                const std::vector<QString>& labelsIn,
                                const std::vector<DeformationDataFiles::DATA_FILE_TYPES> fileTypes,
                                int& selectedItem);
                                
      /// read an atlas or indiv spec file
      void readSpecFile(const SELECTION_TYPE st);
      
      /// create the pushbutton and the line edit
      FileEntryLine* createFileEntryLine(const QString& buttonLabel,
                                     const FILE_TYPES fileType,
                                     QGridLayout* parentGridLayout,
                                     QButtonGroup* buttonGroup,
                                     QPushButton* extraButton = NULL);
      
      /// load the flat/spherical parameters from a deformation map file
      void loadParametersIntoDialog();

      /// edit a border file
      void editBorderFile(const QString& specFileName,
                          const QString& borderFileName,
                          const DeformationDataFiles::DATA_FILE_TYPES fileType);

      /// setup connections for automatic update of deformation map file names
      void setConnectionsForAutoDefMapFileNameUpdates();

      /// update atlas border selections
      void updateAtlasBorderSelections();

      /// update the smoothing parameters
      void updateSmoothingParameters();

      /// update the landmark vector parameters
      void updateLandmarkVectorParameters();

      /// update the morphing parameters
      void updateMorphingParameters();

      /// update the sphere edit cycle spin box
      void updateSphereEditCycleSpinBox();

      /// update the sphere number of cycles spin box
      void updateSphereNumberOfCyclesSpinBox();

      /// update the sphere edit stage spin box
      void updateSphereEditStageSpinBox();

      /// update the spherical number of stages spin box
      void updateSphereNumberOfStagesSpinBox();

      /// update the spherical resolution combo box
      void updateSphericalResolutionComboBox();

      /// update correct spherical distortion correction
      void updateCorrectSphericalDistortion();

      /// update the flat parameters */
      void updateFlatParameters();

      /// update the deformation parameters
      void updateDeformationParametersPage();

      /// update algorithm selection
      void updateAlgorithmSelection();

      /// atlas data files for deformation
      DeformationDataFiles atlasDeformationFiles;
      
      /// indiv data files for deformation
      DeformationDataFiles indivDeformationFiles;
      
      /// atlas spec file name
      QString atlasSpecFileName;
      
      /// indiv spec file name
      QString indivSpecFileName;
      
      /// the deformation map file
      DeformationMapFile dmf;
      
      /// the dialogs tab widget
      QTabWidget* dialogTabWidget;
      
      /// individual's files widget
      QWidget* individualWidgetTop;
      
      /// atlas' files widget
      QWidget* atlasWidgetTop;
      
      /// parameters widget
      QWidget* parametersWidget;
      
      /// deformation widget
      QWidget* deformationWidgetTop;
      
      /// atlas file line edit
      QLineEdit* atlasSpecLineEdit;
      
      /// atlas file line edit
      QLineEdit* atlasBorderLineEdit[DeformationMapFile::MAX_SPHERICAL_STAGES];

      FileEntryLine* atlasBorderFileEntryLine[DeformationMapFile::MAX_SPHERICAL_STAGES]
              ;
      /// atlas file line edit
      QLineEdit* atlasClosedTopoLineEdit;
      
      /// atlas file line edit
      QLineEdit* atlasCutTopoLineEdit;
      
      /// atlas file line edit
      QLineEdit* atlasFiducialCoordLineEdit;
      
      /// atlas file line edit
      QLineEdit* atlasSphericalCoordLineEdit;
      
      /// atlas file line edit
      QLineEdit* atlasFlatCoordLineEdit;
      
      /// atlas file line edit
      QLineEdit* indivSpecLineEdit;
      
      /// atlas file line edit
      QLineEdit* indivBorderLineEdit;
      
      /// atlas file line edit
      QLineEdit* indivClosedTopoLineEdit;
      
      /// atlas file line edit
      QLineEdit* indivCutTopoLineEdit;
      
      /// atlas file line edit
      QLineEdit* indivFiducialCoordLineEdit;
      
      /// atlas file line edit
      QLineEdit* indivSphericalCoordLineEdit;
      
      /// atlas file line edit
      QLineEdit* indivFlatCoordLineEdit;
      
      /// border resampling none radio button
      QRadioButton* borderResamplingNoneRadioButton;
      
      /// border resampling from file radio button
      QRadioButton* borderResamplingFromFileRadioButton;
      
      /// border resampling to value radio button
      QRadioButton* borderResamplingToValueRadioButton;
      
      /// border resampling value Spin Box
      QDoubleSpinBox* borderResamplingDoubleSpinBox;
      
      /// metric nearest node radio button
      QRadioButton* metricNearestNodeRadioButton;

      /// metric average tile nodes radio button
      QRadioButton* metricAverageTileNodesRadioButton;
      
      /// flat sub sampling tiles spin box
      QSpinBox* flatSubSamplingTilesSpinBox;
      
      /// flat beta double spin box
      QDoubleSpinBox* flatBetaDoubleSpinBox;
      
      /// flat variance multiplier double spin box
      QDoubleSpinBox* flatVarMultDoubleSpinBox;
      
      /// flat iterations spin box
      QSpinBox* flatIterationsSpinBox;
      
      /// smoothing strength float spin box
      QDoubleSpinBox* smoothingStrengthDoubleSpinBox;
      
      /// smoothing cycles spin box
      QSpinBox* smoothingCyclesSpinBox;
      
      /// smoothing iterations spin box
      QSpinBox* smoothingIterationsSpinBox;
      
      /// smoothing neighbors spin box
      QSpinBox* smoothingNeighborsSpinBox;
      
      /// smoothing final iterations spin box
      QSpinBox* smoothingFinalSpinBox;
      
      /// morphing cycles spin box
      QSpinBox* morphingCyclesSpinBox;
      
      /// morphing linear force spin box
      QDoubleSpinBox* morphingLinearForceDoubleSpinBox;
      
      /// morphing angular force spin box
      QDoubleSpinBox* morphingAngularForceDoubleSpinBox;
      
      /// morphing step size spin box
      QDoubleSpinBox* morphingStepSizeDoubleSpinBox;
      
      /// morphing landmark step size spin box
      QDoubleSpinBox* morphingLandmarkStepSizeDoubleSpinBox;
      
      /// morphing iterations spin box
      QSpinBox* morphingIterationsSpinBox;
      
      /// morphing smooth iterations spin box
      QSpinBox* morphingSmoothIterationsSpinBox;

      /// widget group for sphere stages
      WuQWidgetGroup* sphereStagesWidgetGroup;

      /// sphere number of stages spin box
      QSpinBox* sphereNumberOfStagesSpinBox;

      /// sphere edige stages spin box
      QSpinBox* sphereEditStageSpinBox;

      /// sphere number of cycles spin box
      QSpinBox* sphereNumberOfCyclesSpinBox;
      
      /// sphere edit cycles spin box
      QSpinBox* sphereEditCycleSpinBox;
      
      /// sphere resolution combo box
      QComboBox* sphereResolutionComboBox;
      
      /// deform both ways check box
      QCheckBox* deformBothWaysCheckBox;
      
      /// deformed file prefix line edit
      QLineEdit* deformedFilePrefixLineEdit;
      
      /// deformed column prefix line edit
      QLineEdit* deformedColumnPrefixLineEdit;
      
      /// delete intermediate files check box
      QCheckBox* deleteIntermediateFilesCheckBox;
      
      /// fiducial sphere distortion correction check box
      QCheckBox* sphereDistortionCorrectionCheckBox;
      
      /// fiducial sphere distortion correction value float spin box
      QDoubleSpinBox* sphereDistortionDoubleSpinBox;
      
      /// deform indiv coord files to atlas check box
      QCheckBox* deformIndivFiducialCoordCheckBox;
      
      /// deform indiv coord files to atlas check box
      QCheckBox* deformIndivInflatedCoordCheckBox;
      
      /// deform indiv coord files to atlas check box
      QCheckBox* deformIndivVeryInflatedCoordCheckBox;
      
      /// deform indiv coord files to atlas check box
      QCheckBox* deformIndivSphericalCoordCheckBox;
      
      /// deform indiv coord files to atlas check box
      QCheckBox* deformIndivFlatCoordCheckBox;
      
      /// deform indiv coord files to atlas check box
      QCheckBox* deformIndivFlatLobarCoordCheckBox;
      
      /// smooth deformed coord files
      QCheckBox* smoothCoordsOneIterationCheckBox;

      /// indiv-to-atlas deformation map file name line edit
      QLineEdit* indivToAtlasDeformMapFileNameLineEdit;

      /// atlas-to-indiv deformation map file name line edit
      QLineEdit* atlasToIndivDeformMapFileNameLineEdit;

      /// landmark-constrained spherical algorithm radio button
      QRadioButton* sphericalLandmarkConstrainedRadioButton;

      /// landmark-vector spherical algorithm radio button
      QRadioButton* sphericalLandmarkVectorRadioButton;

      /// landmark-vector single stage algorithm radio button
      QRadioButton* sphericalLandmarkVectorSingleStageRadioButton;

      /// landmark-vector smoothing iterations
      QSpinBox* vectorSmoothingIterationsSpinBox;

      /// landmark-vector displacement factor double spin box
      QDoubleSpinBox* vectorDisplacementFactorDoubleSpinBox;

      /// landmark-vector endpoint factor double spin box
      QDoubleSpinBox* vectorEndpointFactorDoubleSpinBox;

      /// pause for crossover confirmation check box
      QCheckBox* pauseForCrossoversConfirmationCheckBox;
      
      /// landmark vector options widget
      QWidget* landmarkVectorOptionsWidget;

      /// widget group for smoothing parameters
      WuQWidgetGroup* smoothingParametersWidgetGroup;

      /// widget group for landmark vector parameters
      WuQWidgetGroup* landmarkVectorParametersWidgetGroup;

      /// widget group for morphing parameters
      WuQWidgetGroup* morphingParametersWidgetGroup;
};

#endif // __GUI_SURFACE_DEFORMATION_DIALOG_H__

