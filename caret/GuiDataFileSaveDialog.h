#ifndef __GUI_DATA_FILE_SAVE_DIALOG_H__
#define __GUI_DATA_FILE_SAVE_DIALOG_H__

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


#include "AbstractFile.h"
#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class GuiStructureComboBox;
class GuiSurfaceTypeComboBox;
class GuiTopologyFileComboBox;
class GuiTopologyTypeComboBox;
class GuiVectorFileComboBox;
class GuiVolumeFileSelectionComboBox;
class GuiVolumeSelectionControl;
class GuiVolumeVoxelDataTypeComboBox;
class ImageFile;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTextEdit;
class WuQWidgetGroup;

/// dialog for saving Caret data files.
class GuiDataFileSaveDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiDataFileSaveDialog(QWidget* parent = 0);
                        
      // destructor
      ~GuiDataFileSaveDialog();
      
      // select the file type for saving
      void selectFileType(const QString& fileFilterName);
      
      // select the image file for saving
      void selectImageFile(const ImageFile* imageFile);
      
      // select a file
      void selectFile(AbstractFile* af);
      
      // get the file filter
      QString getFileTypeFilter() const;
      
      // get the selected file name
      QString getSelectedFileName() const;
      
   protected slots:
      // called when file name line edit changed
      void slotFileNameLineEditChanged(const QString&);
      
      // called when a file type combo box selection is made
      void slotFileTypeComboBox(const QString&);
      
      // called when file name push button pressed
      void slotFileNamePushButton();
      
      // load parameters for the selected file type
      void slotLoadParametersForFileType();
      
   protected:
      // called when accept/reject pressed
      void done(int r);
      
      // create the file information section
      QWidget* createFileInformationSection();
      
      // create the metadata section
      QWidget* createMetadataSection();
      
      // create the surface border options section
      QGroupBox* createBorderSurfaceOptionsSection();
      
      // create the border projection options section
      QGroupBox* createBorderProjectionOptionsSection();
      
      // create the cell options section
      QGroupBox* createCellOptionsSection();
      
      // create the coordinate options section
      QGroupBox* createCoordinateOptionsSection();
      
      // create the foci options section
      QGroupBox* createFociOptionsSection();
      
      // create the image options section
      QGroupBox* createImageOptionsSection();
      
      // create the topology options section
      QGroupBox* createTopologyOptionsSection();

      // create the vector options section
      QGroupBox* createVectorOptionsSection();

      // create the volume anatomy options section
      QGroupBox* createVolumeAnatomyOptionsSection();
      
      // create the volume functional options section
      QGroupBox* createVolumeFunctionalOptionsSection();
      
      // create the volume paint options section
      QGroupBox* createVolumePaintOptionsSection();
      
      // create the volume prob atlas options section
      QGroupBox* createVolumeProbAtlasOptionsSection();
      
      // create the volume rgb options section
      QGroupBox* createVolumeRgbOptionsSection();
      
      // create the volume segmentation options section
      QGroupBox* createVolumeSegmentationOptionsSection();
      
      // create the volume vector options section
      QGroupBox* createVolumeVectorOptionsSection();
      
      // create the vtk model options section
      QGroupBox* createVtkModelOptionsSection();
      
      // create the export volume section
      QGroupBox* createExportVolumeOptionsSection();
      
      // create the export surface section
      QGroupBox* createExportSurfaceOptionsSection();
      
      // create paint export options section
      QGroupBox* createPaintExportOptionsSection();
      
      // create shape export options section
      QGroupBox* createShapeExportOptionsSection();
      
      // create metric export options section
      QGroupBox* createMetricExportOptionsSection();
      
      // create gifti options section
      QGroupBox* createGiftiOptionsSection();
      
      // get supported encodings from a file
      void getSupportedEncodings(const AbstractFile& af,
                                 std::vector<AbstractFile::FILE_FORMAT>& encodingsOut,
                                 AbstractFile::FILE_FORMAT& preferredEncodingOut);
      
      // get supported XML encodings from a file
      void getSupportedXmlEncodings(const AbstractFile& af,
                                 std::vector<AbstractFile::FILE_FORMAT>& encodingsOut,
                                 AbstractFile::FILE_FORMAT& preferredEncodingOut);
      
      // update file extension
      void updateFileNamesFileExtension(QString& fileName,
                                        const QString& fileExtension);
                                 
      // update metadata and name.
      void updateMetadataAndName(AbstractFile* af,
                                 QString& fileName,
                                 const QString& fileExtension);
                                             
      /// file name line edit
      QLineEdit* fileNameLineEdit;
      
      /// file type combo box
      QComboBox* fileTypeComboBox;
      
      /// save push button
      QPushButton* savePushButton;
      
      /// metadata PubMed ID line edit
      QLineEdit* metadataPubMedIDLineEdit;
      
      /// metadat comment text edit
      QTextEdit* metadataCommentTextEdit;
      
      /// file encoding combo box
      QComboBox* fileEncodingComboBox;
      
      /// add file extension check box
      QCheckBox* addFileExtensionCheckBox;

      /// surface border options group box
      QGroupBox* borderSurfaceOptionsGroupBox;
      
      /// border projection options group box
      QGroupBox* borderProjectionOptionsGroupBox;
      
      /// cell options group box
      QGroupBox* cellOptionsGroupBox;
      
      /// coordinate options group box
      QGroupBox* coordinateOptionsGroupBox;
      
      /// foci options group box
      QGroupBox* fociOptionsGroupBox;
      
      /// image options group box
      QGroupBox* imageOptionsGroupBox;
      
      /// topology options group box
      QGroupBox* topologyOptionsGroupBox;

      /// vector options group box
      QGroupBox* vectorOptionsGroupBox;

      /// vector file selection combo box
      GuiVectorFileComboBox* vectorFileSelectionComboBox;

      /// volume anatomy options group box
      QGroupBox* volumeAnatomyOptionsGroupBox;
      
      /// volume functional options group box
      QGroupBox* volumeFunctionalOptionsGroupBox;
      
      /// volume paint options group box
      QGroupBox* volumePaintOptionsGroupBox;
      
      /// volume prob atlas options group box
      QGroupBox* volumeProbAtlasOptionsGroupBox;
      
      /// volume rgb options group box
      QGroupBox* volumeRgbOptionsGroupBox;
      
      /// volume segmentation options group box
      QGroupBox* volumeSegmentationOptionsGroupBox;
      
      /// volume vector options group box
      QGroupBox* volumeVectorOptionsGroupBox;
      
      /// vtk model options group box
      QGroupBox* vtkModelOptionsGroupBox;
      
      /// export surface options group box
      QGroupBox* exportSurfaceOptionsGroupBox;
      
      /// export volume options group box
      QGroupBox* exportVolumeOptionsGroupBox;
      
      /// export metric options group box
      QGroupBox* exportMetricOptionsGroupBox;
      
      /// export paint options group box
      QGroupBox* exportPaintOptionsGroupBox;
      
      /// export shape options group box
      QGroupBox* exportShapeOptionsGroupBox;
      
      /// gifti surface selection control
      QGroupBox* giftiSurfaceOptionsGroupBox;
      
      /// widget group for file options
      WuQWidgetGroup* fileOptionsWidgetGroup;
      
      /// surface border associated surface combo box
      GuiBrainModelSelectionComboBox* borderSurfaceSelectionComboBox;
      
      /// surface border type combo box
      GuiSurfaceTypeComboBox* borderSurfaceTypeComboBox;
      
      /// surface border remove duplicates check box
      QCheckBox* borderSurfaceRemoveDuplicatesCheckBox;

      /// cell surface combo box
      GuiBrainModelSelectionComboBox* cellSurfaceSelectionComboBox;
      
      /// border projections remove duplicates check box
      QCheckBox* borderProjectionsRemoveDuplicatesCheckBox;
      
      /// coordinate surface selection combo box
      GuiBrainModelSelectionComboBox* coordinateSurfaceSelectionComboBox;
      
      /// coordinate surface type combo combo box
      GuiSurfaceTypeComboBox* coordinateTypeComboBox;
      
      /// coordinate structure combo box
      GuiStructureComboBox* coordinateStructureComboBox;

      /// coordinate stereotaxic space combo box
      QComboBox* coordinateStereotaxicSpaceComboBox;
      
      /// foci save original coordinate radio button
      QRadioButton* fociSaveOriginalCoordinatesRadioButton;
      
      /// foci save projected coordinate radio button
      QRadioButton* fociSaveProjectedCoordinatesRadioButton;
      
      /// foci left surface selection combo box
      GuiBrainModelSelectionComboBox* fociLeftSurfaceSelectionComboBox;
      
      /// foci right surface selection combo box
      GuiBrainModelSelectionComboBox* fociRightSurfaceSelectionComboBox;
      
      /// foci cerebellum surface selection combo box
      GuiBrainModelSelectionComboBox* fociCerebellumSurfaceSelectionComboBox;
      
      /// image selection combo box
      QComboBox* imageSelectionComboBox;
      
      /// topology selection combo box
      GuiTopologyFileComboBox* topologySelectionComboBox;
      
      /// topology type selection combo box
      GuiTopologyTypeComboBox* topologyTypeComboBox;
      
      /// vtk model selection combo box
      QComboBox* vtkModelSelectionComboBox;
      
      /// volume anatomy file selection
      GuiVolumeFileSelectionComboBox* volumeAnatomyFileSelectionComboBox;
      
      /// volume anatomy file label
      QLineEdit* volumeAnatomyLabelLineEdit;
      
      /// volume anatomy voxel data type
      GuiVolumeVoxelDataTypeComboBox* volumeAnatomyVolumeDataTypeComboBox;
      
      /// volume anatomy write compressed
      QCheckBox* volumeAnatomyWriteCompressedCheckBox;

      /// volume functional file selection
      GuiVolumeFileSelectionComboBox* volumeFunctionalFileSelectionComboBox;
      
      /// volume functional file label
      QLineEdit* volumeFunctionalLabelLineEdit;
      
      /// volume functional voxel data type
      GuiVolumeVoxelDataTypeComboBox* volumeFunctionalVolumeDataTypeComboBox;
      
      /// volume functional write compressed
      QCheckBox* volumeFunctionalWriteCompressedCheckBox;

      /// volume paint file selection
      GuiVolumeFileSelectionComboBox* volumePaintFileSelectionComboBox;
      
      /// volume paint file label
      QLineEdit* volumePaintLabelLineEdit;
      
      /// volume paint voxel data type
      GuiVolumeVoxelDataTypeComboBox* volumePaintVolumeDataTypeComboBox;
      
      /// volume paint write compressed
      QCheckBox* volumePaintWriteCompressedCheckBox;

      /// volume prob atlas file selection
      GuiVolumeFileSelectionComboBox* volumeProbAtlasFileSelectionComboBox;
      
      /// volume prob atlas file label
      QLineEdit* volumeProbAtlasLabelLineEdit;
      
      /// volume prob atlas voxel data type
      GuiVolumeVoxelDataTypeComboBox* volumeProbAtlasVolumeDataTypeComboBox;
      
      /// volume prob atlas write compressed
      QCheckBox* volumeProbAtlasWriteCompressedCheckBox;

      /// volume rgb file selection
      GuiVolumeFileSelectionComboBox* volumeRgbFileSelectionComboBox;
      
      /// volume rgb file label
      QLineEdit* volumeRgbLabelLineEdit;
      
      /// volume rgb voxel data type
      GuiVolumeVoxelDataTypeComboBox* volumeRgbVolumeDataTypeComboBox;
      
      /// volume rgb write compressed
      QCheckBox* volumeRgbWriteCompressedCheckBox;

      /// volume segmentation file selection
      GuiVolumeFileSelectionComboBox* volumeSegmentationFileSelectionComboBox;
      
      /// volume segmentation file label
      QLineEdit* volumeSegmentationLabelLineEdit;
      
      /// volume segmentation voxel data type
      GuiVolumeVoxelDataTypeComboBox* volumeSegmentationVolumeDataTypeComboBox;
      
      /// volume segmentation write compressed
      QCheckBox* volumeSegmentationWriteCompressedCheckBox;

      /// volume vector file selection
      GuiVolumeFileSelectionComboBox* volumeVectorFileSelectionComboBox;
      
      /// volume vector file label
      QLineEdit* volumeVectorLabelLineEdit;
      
      /// volume vector voxel data type
      GuiVolumeVoxelDataTypeComboBox* volumeVectorVolumeDataTypeComboBox;
      
      /// volume vector write compressed
      QCheckBox* volumeVectorWriteCompressedCheckBox;
      
      /// export volume selection control
      GuiVolumeSelectionControl* exportVolumeSelectionControl;
      
      /// export surface selection control
      GuiBrainModelSelectionComboBox* exportSurfaceSelectionControl;
      
      /// export paint surface selection control
      GuiBrainModelSelectionComboBox* exportPaintSurfaceSelectionControl;
      
      /// export paint column selection control
      GuiNodeAttributeColumnSelectionComboBox* exportPaintColumnSelectionControl;
      
      /// export shape surface selection control
      GuiBrainModelSelectionComboBox* exportShapeSurfaceSelectionControl;
      
      /// export shape column selection control
      GuiNodeAttributeColumnSelectionComboBox* exportShapeColumnSelectionControl;
      
      /// export metric surface selection control
      GuiBrainModelSelectionComboBox* exportMetricSurfaceSelectionControl;
      
      /// export metric column selection control
      GuiNodeAttributeColumnSelectionComboBox* exportMetricColumnSelectionControl;

      /// GIFTI surface selection control
      GuiBrainModelSelectionComboBox* giftiSurfaceSelectionControl;
      
};

#endif // __GUI_DATA_FILE_SAVE_DIALOG_H__

