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

#include "WuQDialog.h"

#include "DeformationMapFile.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QTabWidget;

class QDoubleSpinBox;
class SpecFile;

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
     int borderFileSelected;
     
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
      /// Called when batch button is pressed
      void slotBatchButton();
      
      /// Called when user press OK or Cancel buttons
      void done(int r);

      /// Called when an atlas file selection is made
      void atlasFileSelection(int itemNum);
      
      /// Called when an indiv file selection is made
      void indivFileSelection(int itemNum);
      
      /// called when load deformation map file push button is pressed
      void loadDeformationMapFilePushButton();
      
      /// called when number of spherical cycles is changed
      void slotSphereNumberOfCyclesComboBox(int item);
      
      /// called before sphere edit cycle is changed
      void slotSphereEditCycleComboBoxOldValue();
      
      /// called when sphere edit cycle is changed
      void slotSphereEditCycleComboBoxNewValue();
      
      /// called when Use Standard Parameters button pressed
      void slotStandardParametersPushButton();
      
      /// called to edit individual borders
      void slotBordersEditIndividual();
      
      /// called to edit atlas borders
      void slotBordersEditAtlas();
      
   private:
      /// file selection types
      enum SELECTION_TYPE {
         SELECTION_TYPE_ATLAS,
         SELECTION_TYPE_INDIV
      };
      
      /// Unique ID's for file types
      enum FILE_TYPES {
         FILE_TYPE_SPEC,
         FILE_TYPE_BORDER,
         FILE_TYPE_TOPO_CLOSED,
         FILE_TYPE_TOPO_CUT,
         FILE_TYPE_COORD_FIDUCIAL,
         FILE_TYPE_COORD_FLAT,
         FILE_TYPE_COORD_SPHERICAL
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

      /// create the flat parameters section
      QWidget* createFlatParameters();

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
      QLineEdit* createFileEntryLine(const QString& buttonLabel,
                                     const FILE_TYPES fileType,
                                     QGridLayout* parentGridLayout,
                                     QButtonGroup* buttonGroup,
                                     QPushButton* extraButton = NULL);
      
      /// load the flat/spherical parameters from a deformation map file
      void loadParametersIntoDialog();

      /// read the flat/spherical parameters from the dialog
      void readParametersFromDialog();
      
      /// edit a border file
      void editBorderFile(const QString& specFileName,
                          const QString& borderFileName,
                          const DeformationDataFiles::DATA_FILE_TYPES fileType);
                          
      /// type of deformation
      DeformationMapFile::DEFORMATION_TYPE deformationType;
      
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
      QLineEdit* atlasBorderLineEdit;
      
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
      
      /// flat beta line edit
      QLineEdit* flatBetaLineEdit;
      
      /// flat variance multiplier line edit
      QLineEdit* flatVarMultDoubleSpinBox;
      
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
      
      /// sphere number of cycles combo box
      QComboBox* sphereNumberOfCyclesComboBox;
      
      /// sphere edit cycles combo box
      QComboBox* sphereEditCycleComboBox;
      
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
};

#endif // __GUI_SURFACE_DEFORMATION_DIALOG_H__

