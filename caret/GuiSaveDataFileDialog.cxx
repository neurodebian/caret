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

#include <QGlobalStatic>

#include <iostream>
#include <sstream>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <q3filedialog.h>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QRegExp>
#include <QTextEdit>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWindowsStyle>

#define _GUI_SAVE_DATA_FILE_DIALOG_MAIN_
#include "GuiSaveDataFileDialog.h"
#undef _GUI_SAVE_DATA_FILE_DIALOG_MAIN_

#include "AreaColorFile.h"
#include "ArealEstimationFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "CocomacConnectivityFile.h"
#include "ColorFile.h"
#include "ContourCellColorFile.h"
#include "ContourCellFile.h"
#include "CutsFile.h"
#include "DeformationFieldFile.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GeodesicDistanceFile.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiStructureComboBox.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiSpecFileCreationDialog.h"
#include "GuiToolBar.h"
#include "GuiTopologyFileComboBox.h"
#include "GuiTopologyTypeComboBox.h"
#include "GuiVolumeSelectionControl.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "PaletteFile.h"
#include "ParamsFile.h"
#include "ProbabilisticAtlasFile.h"
#include "PreferencesFile.h"
#include "RgbPaintFile.h"
#include "SceneFile.h"
#include "SectionFile.h"
#include "StereotaxicSpace.h"
#include "StudyMetaDataFile.h"
#include "SurfaceShapeFile.h"
#include "GuiSurfaceTypeComboBox.h"
#include "SurfaceVectorFile.h"
#include "TopographyFile.h"
#include "TopologyFile.h"
#include "VocabularyFile.h"
#include "VtkModelFile.h"
#include "WustlRegionFile.h"

#include "global_variables.h"
#include "FileUtilities.h"
#include "StringUtilities.h"
#include "QtUtilities.h"

/**
 * Constructor
 */
GuiSaveDataFileDialog::GuiSaveDataFileDialog(QWidget* parent,
                                             const QString& useThisFileFilter)
           : Q3FileDialog(parent, "Save Data File", false)
{   
   savedPositionAndSizeValid = false;
   
   QObject::connect(this, SIGNAL(filterSelected(const QString&)),
                    this, SLOT(filterSelectedSlot(const QString&)));
                    
   setWindowTitle("Save Data File");
   setMode(Q3FileDialog::AnyFile);
   setDir(QDir::currentPath());
   
   //
   // Add the additional widgets to the dialog
   //
   QWidget* caretWidget = createCaretUniqueSection();
   addWidgets(NULL, caretWidget, NULL);
   
   //
   // Set up the file filters for the dialog
   //
   setFilter(areaColorFileFilter);
   addFilter(arealEstimationFileFilter);
   addFilter(borderGenericFileFilter);
   addFilter(borderVolumeFileFilter);
   addFilter(borderColorFileFilter);
   addFilter(borderProjectionFileFilter);
   addFilter(cellFileFilter);
   addFilter(cellColorFileFilter);
   addFilter(cellProjectionFileFilter);
   addFilter(cellVolumeFileFilter);
   addFilter(cocomacFileFilter);
   addFilter(contourFileFilter);
   addFilter(contourCellFileFilter);
   addFilter(contourCellColorFileFilter);
   addFilter(coordinateGenericFileFilter);
   addFilter(deformationFieldFileFilter);
   addFilter(cutsFileFilter);
   addFilter(fociFileFilter);
   addFilter(fociColorFileFilter);
   addFilter(fociProjectionFileFilter);
   addFilter(fociVolumeFileFilter);
   addFilter(geodesicDistanceFileFilter);
   addFilter(latitudeLongitudeFileFilter);
   addFilter(metricFileFilter);
   addFilter(paintFileFilter);
   addFilter(paletteFileFilter);
   addFilter(paramsFileFilter);
   addFilter(probAtlasFileFilter);
   addFilter(rgbPaintFileFilter);
   addFilter(sceneFileFilter);
   addFilter(sectionFileFilter);
   addFilter(studyMetaDataFileFilter);
   addFilter(surfaceShapeFileFilter);
   addFilter(surfaceVectorFileFilter);
   addFilter(topographyFileFilter);
   addFilter(topologyGenericFileFilter);
   addFilter(transformationMatrixFileFilter);
   addFilter(vocabularyFileFilter);
   addFilter(volumeMultiFileAfniFilter);
   addFilter(volumeMultiFileNiftiFilter);
   addFilter(volumeFileAfniFilter);
   addFilter(volumeFileNiftiWriteFilter);
   addFilter(volumeFileSpmMedxFilter);
   addFilter(volumeFileWuNilFilter);
   addFilter(vtkModelFileFilter);
   addFilter(wustlRegionFileFilter);
   
   if (useThisFileFilter.isEmpty() == false) {
      setSelectedFilter(useThisFileFilter);
   }
   else {
      setSelectedFilter(0);
   }
   filterSelectedSlot(selectedFilter());
   
   QtUtilities::setMaximumHeightToNinetyPercentOfScreenHeight(this);

   updateDialog();   
}

/**
 * create the caret unique items.
 */
QWidget* 
GuiSaveDataFileDialog::createCaretUniqueSection()
{
   //---------------------------------------------------------------------
   //
   // Previous directories combo box
   //
   previousDirectoryComboBox = new QComboBox;
   QObject::connect(previousDirectoryComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPreviousDirectory(int)));
                       
   //
   // Previous directories group box
   //
   QGroupBox* prevDirGroupBox = new QGroupBox("Previous Directories");
   QVBoxLayout* prevDirGroupLayout = new QVBoxLayout(prevDirGroupBox);
   prevDirGroupLayout->addWidget(previousDirectoryComboBox);
   
   //---------------------------------------------------------------------
   //
   // PubMed ID widget
   //
   pubMedIDLineEdit = new QLineEdit;
   QLabel* pubMedIDLabel = new QLabel("PubMed ID");
   pubMedIDWidget = new QWidget;
   QHBoxLayout* pubMedIDLayout = new QHBoxLayout(pubMedIDWidget);
   pubMedIDLayout->addWidget(pubMedIDLabel);
   pubMedIDLayout->addWidget(pubMedIDLineEdit);
   pubMedIDLayout->addStretch();
   
   //---------------------------------------------------------------------
   //
   // Comment widget
   //
   commentWidget = new QWidget;
   QHBoxLayout* commentLayout = new QHBoxLayout(commentWidget);
   QLabel* commentLabel = new QLabel("C\nO\nM\nM\nE\nN\nT");
   commentLayout->addWidget(commentLabel);
   commentTextEditor = new QTextEdit;
   commentLayout->addWidget(commentTextEditor);
   commentTextEditor->setReadOnly(false);
   commentTextEditor->setLineWrapMode(QTextEdit::WidgetWidth);
   commentTextEditor->setPlainText("Place comment here.");
   commentTextEditor->setFixedHeight(commentLabel->sizeHint().height());

   //---------------------------------------------------------------------
   //
   // Auto append extension
   //
   autoAppendExtensionCheckBox = new QCheckBox("Add File Name extension() (If Needed)");
   autoAppendExtensionCheckBox->setChecked(true);
   
   //---------------------------------------------------------------------
   //
   // data file encoding
   //
   QLabel* encodingLabel = new QLabel("Data File Encoding ");
   fileEncodingComboBox = new QComboBox;
   fileEncodingComboBox->setMinimumWidth(350);
   saveDataEncodingTypeWidget = new QWidget;
   QHBoxLayout* saveDataEncodingLayout = new QHBoxLayout(saveDataEncodingTypeWidget);
   saveDataEncodingLayout->addWidget(encodingLabel);
   saveDataEncodingLayout->addWidget(fileEncodingComboBox);
   saveDataEncodingLayout->addStretch();
   
   //---------------------------------------------------------------------
   //
   // Remove duplicates check box
   //
   removeDuplicateBorderCheckBox = new QCheckBox("Remove Duplicates");
   removeDuplicateBorderCheckBox->setChecked(true);
   
   //
   // Label and combo box for border file selection
   //
   QLabel* borderSurfaceLabel = new QLabel("Associated Surface");
   borderFileComboBox = new QComboBox;
   borderFileComboBox->setMinimumWidth(350);
   QObject::connect(borderFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateSaveParameters()));
   QHBoxLayout* borderSurfaceLayout = new QHBoxLayout;
   borderSurfaceLayout->addWidget(borderSurfaceLabel);
   borderSurfaceLayout->addWidget(borderFileComboBox);
   borderSurfaceLayout->addStretch();
   
   borderFileComboBox->setToolTip("This control chooses the surface\n"
                                     "whose borders are to be saved.");
                                     
   //
   // save as type combo box
   //
   QLabel* borderSaveAsTypeLabel = new QLabel("Save As Type ");
   borderTypeComboBox = new GuiSurfaceTypeComboBox;
   borderTypeComboBox->setToolTip(
                 "This selects the type for the\n"
                 "border file that is to be saved.");
   QHBoxLayout* borderSaveAsLayout = new QHBoxLayout;
   borderSaveAsLayout->addWidget(borderSaveAsTypeLabel);
   borderSaveAsLayout->addWidget(borderTypeComboBox);
   borderSaveAsLayout->addStretch();
   
   //
   // Border file surface and save as type
   //
   borderSurfaceWidget = new QWidget;
   QVBoxLayout* bordSurfLayout = new QVBoxLayout(borderSurfaceWidget);
   bordSurfLayout->addLayout(borderSurfaceLayout);
   bordSurfLayout->addLayout(borderSaveAsLayout);
   
   //
   // The borders group box
   //
   bordersGroupBox = new QGroupBox("Border Selections");
   QVBoxLayout* borderGroupLayout = new QVBoxLayout(bordersGroupBox);
   borderGroupLayout->addWidget(removeDuplicateBorderCheckBox);
   borderGroupLayout->addWidget(borderSurfaceWidget);
   
   //---------------------------------------------------------------------
   //
   // Volume selection control
   //
   volumeSelectionControl = new GuiVolumeSelectionControl(0,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL,
                                                          "",
                                                          false,
                                                          false,
                                                          true);
   QObject::connect(volumeSelectionControl, SIGNAL(signalSelectionChanged()),
                    this, SLOT(volumeTypeSelectedSlot()));
                    
   //
   // Volume voxel data type 
   //
   QLabel* voxelDataTypeLabel = new QLabel("Voxel Data Type");
   volumeVoxelDataTypeComboBox = new QComboBox;
   volumeVoxelDataTypeComboBox->addItem("Choose Voxel Data Type");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_UNKNOWN);
   volumeVoxelDataTypeComboBox->addItem("Byte - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_CHAR);
   volumeVoxelDataTypeComboBox->addItem("Byte - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_CHAR_UNSIGNED);
   volumeVoxelDataTypeComboBox->addItem("Short - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_SHORT);
   volumeVoxelDataTypeComboBox->addItem("Short - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_SHORT_UNSIGNED);
   volumeVoxelDataTypeComboBox->addItem("Int - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_INT);
   volumeVoxelDataTypeComboBox->addItem("Int - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_INT_UNSIGNED);
   volumeVoxelDataTypeComboBox->addItem("Long - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_LONG);
   volumeVoxelDataTypeComboBox->addItem("Long - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_LONG_UNSIGNED);
   volumeVoxelDataTypeComboBox->addItem("Float");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_FLOAT);
   volumeVoxelDataTypeComboBox->addItem("Double");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_DOUBLE);
   volumeVoxelDataTypeComboBox->addItem("RGB - Voxel Interleaved");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED);
   volumeVoxelDataTypeComboBox->addItem("RGB - Slice Interleaved");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_RGB_SLICE_INTERLEAVED);
   volumeVoxelDataTypeComboBox->setToolTip(
                 "Available data types depend upon the volume file type."
                 );
   QHBoxLayout* voxelDataTypeLayout = new QHBoxLayout;
   voxelDataTypeLayout->addWidget(voxelDataTypeLabel);
   voxelDataTypeLayout->addWidget(volumeVoxelDataTypeComboBox);
   voxelDataTypeLayout->addStretch();
   
   //
   // Write volume gzipped check box
   //
   volumeFileGzipCheckBox = new QCheckBox("Write Volume Data GZipped");

   //
   // Volume Group Box
   //
   volumeGroupBox = new QGroupBox("Volume Selections");
   QVBoxLayout* volumeGroupLayout = new QVBoxLayout(volumeGroupBox);
   volumeGroupLayout->addWidget(volumeSelectionControl);
   volumeGroupLayout->addLayout(voxelDataTypeLayout);
   volumeGroupLayout->addWidget(volumeFileGzipCheckBox);
      
   //---------------------------------------------------------------------
   //
   // Label and combo box for coordinate file selection
   //
   QLabel* coordFileLabel = new QLabel("Coordinate File");
   coordFileComboBox = new QComboBox;
   QObject::connect(coordFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateSaveParameters()));
   coordFileComboBox->setToolTip(
                 "This chooses which of the loaded\n"
                 "Coordinate Files is to be saved.");
   QHBoxLayout* coordFileLayout = new QHBoxLayout;
   coordFileLayout->addWidget(coordFileLabel);
   coordFileLayout->addWidget(coordFileComboBox);
   coordFileLayout->addStretch();
   
   //
   // save as type combo box
   //
   QLabel* coordSaveAsTypeLabel = new QLabel("Save As Type ");
   coordTypeComboBox = new GuiSurfaceTypeComboBox;
   coordTypeComboBox->setToolTip(
                 "This selects the type for the\n"
                 "coordinate file that is to be saved.");
   QHBoxLayout* coordSaveAsLayout = new QHBoxLayout;
   coordSaveAsLayout->addWidget(coordSaveAsTypeLabel);
   coordSaveAsLayout->addWidget(coordTypeComboBox);
   coordSaveAsLayout->addStretch();
   
   //
   // Label and combo box for structure
   //
   QLabel* structureLabel = new QLabel("Structure ");
   structureComboBox = new GuiStructureComboBox(0);
   QHBoxLayout* structureLayout = new QHBoxLayout;
   structureLayout->addWidget(structureLabel);
   structureLayout->addWidget(structureComboBox);
   structureLayout->addStretch();
   
   //
   // Label and combo box for coord frame 
   //
   QLabel* coordFrameLabel = new QLabel("Coord Frame (Stereotaxic Space)");
   coordFrameComboBox = new QComboBox;
   //coordFrameComboBox->setStyle(new QWindowsStyle);
   coordFrameComboBox->addItem("AC-PC");
   coordFrameComboBox->addItem("Anterior Commissure");
   coordFrameComboBox->addItem("Cartesian Standard");
   coordFrameComboBox->addItem("Cartesian Non-Standard");
   coordFrameComboBox->addItem("Center of Gravity");
   coordFrameComboBox->addItem("Native");
   coordFrameComboBox->addItem("Spherical Standard");
   coordFrameComboBox->addItem("Talairach");
   coordFrameComboBox->addItem("Unspecified");
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
   for (unsigned int j = 0; j < allSpaces.size(); j++) {
       coordFrameComboBox->addItem(allSpaces[j].getName());
   }
   coordFrameComboBox->setToolTip(
                 "This selects the the coordinate frame\n"
                 "for the border or coordinate file that\n"
                 "is being saved.");
   QHBoxLayout* coordFrameLayout = new QHBoxLayout;
   coordFrameLayout->addWidget(coordFrameLabel);
   coordFrameLayout->addWidget(coordFrameComboBox);
   coordFrameLayout->addStretch();

   //
   // Label and orientation combo boxes
   //
   QLabel* orientationLabel = new QLabel("Orientation");
   xOrientationComboBox = new QComboBox;
   xOrientationComboBox->addItem("Unknown");
   xOrientationComboBox->addItem("Left");
   xOrientationComboBox->addItem("Right");
   xOrientationComboBox->addItem("Anterior");
   xOrientationComboBox->addItem("Posterior");
   xOrientationComboBox->addItem("Inferior");
   xOrientationComboBox->addItem("Superior");
   xOrientationComboBox->setFixedSize(xOrientationComboBox->sizeHint());
   xOrientationComboBox->setToolTip( "This control selects the\n"
                                      "orientation of the negative\n"
                                      "X axis.");
   yOrientationComboBox = new QComboBox;
   yOrientationComboBox->addItem("Unknown");
   yOrientationComboBox->addItem("Left");
   yOrientationComboBox->addItem("Right");
   yOrientationComboBox->addItem("Anterior");
   yOrientationComboBox->addItem("Posterior");
   yOrientationComboBox->addItem("Inferior");
   yOrientationComboBox->addItem("Superior");
   yOrientationComboBox->setFixedSize(yOrientationComboBox->sizeHint());
   yOrientationComboBox->setToolTip( "This control selects the\n"
                                      "orientation of the negative\n"
                                      "Y axis.");
   zOrientationComboBox = new QComboBox;
   zOrientationComboBox->addItem("Unknown");
   zOrientationComboBox->addItem("Left");
   zOrientationComboBox->addItem("Right");
   zOrientationComboBox->addItem("Anterior");
   zOrientationComboBox->addItem("Posterior");
   zOrientationComboBox->addItem("Inferior");
   zOrientationComboBox->addItem("Superior");
   zOrientationComboBox->setFixedSize(zOrientationComboBox->sizeHint());
   zOrientationComboBox->setToolTip("This control selects the\n"
                                      "orientation of the negative\n"
                                      "Z axis.");
   QHBoxLayout* orientationLayout = new QHBoxLayout;
   orientationLayout->addWidget(orientationLabel);
   orientationLayout->addWidget(xOrientationComboBox);
   orientationLayout->addWidget(yOrientationComboBox);
   orientationLayout->addWidget(zOrientationComboBox);
   orientationLayout->addStretch();
   
   //
   // Coordinates group box
   //
   coordFileGroupBox = new QGroupBox("Coordinate Selections");
   QVBoxLayout* coordFileGroupLayout = new QVBoxLayout(coordFileGroupBox);
   coordFileGroupLayout->addLayout(coordFileLayout);
   coordFileGroupLayout->addLayout(coordSaveAsLayout);
   coordFileGroupLayout->addLayout(structureLayout);
   coordFileGroupLayout->addLayout(coordFrameLayout);
   coordFileGroupLayout->addLayout(orientationLayout);
   
   //---------------------------------------------------------------------
   //
   // Foci file selection
   //
   QLabel* leftFociTypeLabel = new QLabel("Left Surface ");
   QLabel* rightFociTypeLabel = new QLabel("Right Surface ");
   fociLeftSurfaceComboBox = new GuiBrainModelSelectionComboBox(false, true, false, "NO SURFACE");
   fociLeftSurfaceComboBox->setSurfaceStructureRequirement(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
   QObject::connect(fociLeftSurfaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateSaveParameters()));
   fociRightSurfaceComboBox = new GuiBrainModelSelectionComboBox(false, true, false, "NO SURFACE");
   fociRightSurfaceComboBox->setSurfaceStructureRequirement(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
   QObject::connect(fociRightSurfaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateSaveParameters()));
   
   fociSurfaceWidget = new QWidget;
   QGridLayout* fociSurfaceLayout = new QGridLayout(fociSurfaceWidget);
   fociSurfaceLayout->addWidget(new QLabel("  "), 0, 0);
   fociSurfaceLayout->addWidget(leftFociTypeLabel, 0, 1);
   fociSurfaceLayout->addWidget(fociLeftSurfaceComboBox, 0, 2);
   fociSurfaceLayout->addWidget(rightFociTypeLabel, 1, 1);
   fociSurfaceLayout->addWidget(fociRightSurfaceComboBox, 1, 2);
   
   fociSaveOriginalCoordsRadioButton = new QRadioButton("Save Original Coordinates");
   fociSaveProjectedToSurfacesRadioButton = new QRadioButton("Save Projected Coordinates");
   QButtonGroup* fociSaveButtonGroup = new QButtonGroup(this);
   QObject::connect(fociSaveButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotFociSaveUpdate()));
   fociSaveButtonGroup->addButton(fociSaveOriginalCoordsRadioButton);
   fociSaveButtonGroup->addButton(fociSaveProjectedToSurfacesRadioButton);
      
   fociGroupBox = new QGroupBox("Foci Selections");
   QVBoxLayout* fociGroupLayout = new QVBoxLayout(fociGroupBox);
   fociGroupLayout->addWidget(fociSaveOriginalCoordsRadioButton);
   fociGroupLayout->addWidget(fociSaveProjectedToSurfacesRadioButton);
   fociGroupLayout->addWidget(fociSurfaceWidget);
   fociGroupLayout->addStretch();
   
   fociSaveOriginalCoordsRadioButton->setChecked(true);
   slotFociSaveUpdate();
   
   //---------------------------------------------------------------------
   //
   // Cell file selection
   //
   QLabel* cellTypeLabel = new QLabel("Cells Associated With Surface Type    ");
   cellTypeComboBox = new GuiBrainModelSelectionComboBox(false, true, false, "");
   QObject::connect(cellTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateSaveParameters()));
   cellGroupBox = new QGroupBox("Cell Selections");
   QHBoxLayout* cellGroupLayout = new QHBoxLayout(cellGroupBox);
   cellGroupLayout->addWidget(cellTypeLabel);
   cellGroupLayout->addWidget(cellTypeComboBox);
   cellGroupLayout->addStretch();
   
   //---------------------------------------------------------------------
   //
   // Label and combo box for vtk model file selections
   //
   QLabel* vtkFileLabel = new QLabel("VTK Model Files: ");
   vtkModelFileComboBox = new QComboBox;
   QObject::connect(vtkModelFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateSaveParameters()));
   vtkModelGroupBox = new QGroupBox("VTK Model Selections");
   QHBoxLayout* vtkModelGroupLayout = new QHBoxLayout(vtkModelGroupBox);
   vtkModelGroupLayout->addWidget(vtkFileLabel);
   vtkModelGroupLayout->addWidget(vtkModelFileComboBox);
   vtkModelGroupLayout->addStretch();
   
   //---------------------------------------------------------------------
   //
   // Label and combo box for topology file selection
   //
   QLabel* topologyFileLabel = new QLabel("Topology File");
   topologyFileComboBox = new GuiTopologyFileComboBox(0);
   QObject::connect(topologyFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(updateSaveParameters()));
   topologyFileComboBox->setToolTip(
                 "This chooses which of the loaded\n"
                 "Topology Files is to be saved.");
   
   
   //
   // Label and combo box for topology type selection
   //
   QLabel* topologyTypeLabel = new QLabel("Save As Type");
   topologyTypeComboBox = new GuiTopologyTypeComboBox;
   topologyTypeComboBox->setToolTip(
                 "This choose the the type of\n"
                 "the topology file being saved.");
   
   //
   // group box for topology
   //
   topologyGroupBox = new QGroupBox("Topology Selections");
   QGridLayout* topologyGridLayout = new QGridLayout(topologyGroupBox);
   topologyGridLayout->addWidget(topologyFileLabel, 0, 0);
   topologyGridLayout->addWidget(topologyFileComboBox, 0, 1);
   topologyGridLayout->addWidget(topologyTypeLabel, 1, 0);
   topologyGridLayout->addWidget(topologyTypeComboBox, 1, 1);

   //---------------------------------------------------------------------
   //
   // Save parameters group box and layout
   //
   QGroupBox* saveParametersGroupBox = new QGroupBox("Save Parameters");
   QVBoxLayout* saveParametersGroupLayout = new QVBoxLayout(saveParametersGroupBox);
   saveParametersGroupLayout->addWidget(pubMedIDWidget);
   saveParametersGroupLayout->addWidget(commentWidget);
   saveParametersGroupLayout->addWidget(autoAppendExtensionCheckBox);
   saveParametersGroupLayout->addWidget(saveDataEncodingTypeWidget);
   
   //---------------------------------------------------------------------
   //
   // Create the widget containing the caret items
   //
   const int stretchNumber = 1;
   QWidget* w = new QWidget(this);
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(prevDirGroupBox);
   layout->addWidget(saveParametersGroupBox);
   layout->addWidget(bordersGroupBox, stretchNumber, Qt::AlignLeft);
   layout->addWidget(volumeGroupBox, stretchNumber, Qt::AlignLeft);
   layout->addWidget(coordFileGroupBox, stretchNumber, Qt::AlignLeft);
   layout->addWidget(fociGroupBox, stretchNumber, Qt::AlignLeft);
   layout->addWidget(cellGroupBox, stretchNumber, Qt::AlignLeft);
   layout->addWidget(vtkModelGroupBox, stretchNumber, Qt::AlignLeft);
   layout->addWidget(topologyGroupBox, stretchNumber, Qt::AlignLeft);
   /*
   */
   return w;
}      

/**
 * called when foci save option changed.
 */
void 
GuiSaveDataFileDialog::slotFociSaveUpdate()
{
   const bool enableSurfacesFlag = fociSaveProjectedToSurfacesRadioButton->isChecked();
   fociSurfaceWidget->setEnabled(enableSurfacesFlag);
}

/**
 * show the dialog.
 */
void 
GuiSaveDataFileDialog::show()
{
   setDir(QDir::currentPath());
   updateDialog();
   updateSaveParameters();

   Q3FileDialog::show();

   if (savedPositionAndSizeValid) {
      setGeometry(savedPositionAndSize);
      adjustSize();
   }   
}
      
/**
 * update the dialog.
 */
void 
GuiSaveDataFileDialog::updateDialog()
{
   //
   // Previous directories 
   //
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   pf->getRecentDataFileDirectories(previousDirectories);
   previousDirectoryComboBox->clear();
   if (previousDirectories.empty() == false) {
      for (int i = 0; i < static_cast<int>(previousDirectories.size()); i++) {
         QString dirName(previousDirectories[i]);
         const int dirNameLength = static_cast<int>(dirName.length());
         if (dirNameLength > 50) {
            QString s("...");
            s.append(dirName.mid(dirNameLength - 50));
            dirName = s;
         }
         previousDirectoryComboBox->addItem(dirName);
      }
   }
   
   //
   // Data file encoding
   //
   updateFileWriteTypeComboBox(NULL);

   //
   // Borders
   //
   int defaultBorderFileIndex = -1;
   int borderCounter = 0;
   borderFileComboBox->clear();
   borderFileSurfaceIndex.clear();
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      const BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms == theMainWindow->getBrainModelSurface()) {
            defaultBorderFileIndex = borderCounter;
         }
         QString name(bms->getDescriptiveName());
         borderFileComboBox->addItem(name);
         borderFileSurfaceIndex.push_back(i);
         borderCounter++;
      }
   }
   if (defaultBorderFileIndex >= 0) {
      borderFileComboBox->setCurrentIndex(defaultBorderFileIndex);
   }
   
   //
   // cells/foci
   //
   cellTypeComboBox->updateComboBox();
   fociLeftSurfaceComboBox->updateComboBox();
   fociRightSurfaceComboBox->updateComboBox();
   
   //
   // Update coordinate files
   //
   int defaultCoordFileIndex = -1;
   int ctr = 0;
   coordFileComboBox->clear();
   brainSurfaces.clear();
   coordinateFiles.clear();
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
      if (bms != NULL) {
         brainSurfaces.push_back(bms);
         CoordinateFile* cf = bms->getCoordinateFile();
         coordinateFiles.push_back(cf);
         QString name(StringUtilities::makeUpperCase(bms->getSurfaceTypeName()));
         if (cf->getFileName().isEmpty()) {
            name.append("  Unnamed");
         }
         else {
            name.append("  ");
            name.append(FileUtilities::basename(cf->getFileName()));
         }
         coordFileComboBox->addItem(name);
         if (bms == theMainWindow->getBrainModelSurface()) {
            defaultCoordFileIndex = ctr;
         }
         ctr++;
      }
   }
   if (defaultCoordFileIndex >= 0) {
      coordFileComboBox->setCurrentIndex(defaultCoordFileIndex);
   }
   
   //
   // Update VTK models
   //
   vtkModelFileComboBox->clear();
   vtkModelFiles.clear();
   for (int j = 0; j < theMainWindow->getBrainSet()->getNumberOfVtkModelFiles(); j++) {
      VtkModelFile* vmf = theMainWindow->getBrainSet()->getVtkModelFile(j);
      vtkModelFiles.push_back(vmf);
      vtkModelFileComboBox->addItem(vmf->getFileName());
   }
   
   //
   // Update topology
   //
   topologyFileComboBox->updateComboBox();
   const BrainModelSurface* mainBMS = theMainWindow->getBrainModelSurface();
   if (mainBMS != NULL) {
      const TopologyFile* tf = mainBMS->getTopologyFile();
      if (tf != NULL) {
         topologyFileComboBox->setSelectedTopologyFile(tf);
      }
   }
   
   //
   // Update volume
   //
   volumeSelectionControl->updateControl();
}      

/**
 * Overrides QFileDialog::done(int).  This allows us to examine the file selected by the
 * user and to see if the file already exists in which case we ask the user if the file
 * should be overwritten.
 */
void
GuiSaveDataFileDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QString name(selectedFile());
      
      //
      // If file exists, see if user wants to overwrite it.
      //
      QFileInfo fileInfo(name);
      if (fileInfo.exists()) {
         if (GuiMessageBox::warning(this, "Replace", "File exists.  Do you want to replace it?",
                                  "Yes", "No") != 0) {
            return;
         }
//
// Samba (used to mount unix file systems on Windows) incorrectly reports file permissions
//
#ifndef Q_OS_WIN32
        if (fileInfo.isWritable() == false) {
           GuiMessageBox::critical(this, "File Error", "File is not writable!", "OK");
           return;
        }
#endif
      }
      
      //
      // Update previous directories
      //
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      pf->addToRecentDataFileDirectories(FileUtilities::dirname(name), true);
      
      //
      // Allow user to set the spec file name if not already set
      //
      if (theMainWindow->getBrainSet()->getSpecFileName().isEmpty()) {
         if (GuiMessageBox::information(this, "Spec File", "Would you like to create a Spec File ?",
                                      "Yes", "No") == 0) {
            //
            // Create spec file dialog will set the directory and create the spec file
            //
            GuiSpecFileCreationDialog sfcd(this);
            sfcd.exec();
            PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
            pf->addToRecentSpecFiles(sfcd.getCreatedSpecFileName(), true);
/*
            QFileDialog specFileNameDialog(this, "specFileNameDialog", true);
            specFileNameDialog.setWindowTitle("Set Spec File Name");
            specFileNameDialog.setFilter("Spec File (*.spec)");
            specFileNameDialog.setFileMode(QFileDialog::AnyFile);
            if (specFileNameDialog.exec() == QFileDialog::Accepted) {
               QString specName(specFileNameDialog.addAction(selectedFile());
               if (FileUtilities::filenameExtension(specName) != "spec") {
                  specName.append(".spec");
               }
               theMainWindow->getBrainSet()->setSpecFileName(specName);
               QDir::setCurrent(FileUtilities::dirname(specName));
               PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
               pf->addToRecentSpecFiles(specName, true);
            }
*/
         }
      }
      
      const QString filterName = selectedFilter();
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      AbstractFile* af = NULL;
      QString extension;
      
      ContourFile* contourFile = NULL;
      CoordinateFile* coordFile = NULL;
      BrainModelSurface* borderSurface = NULL;
      BrainModelSurface::SURFACE_TYPES borderFileType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
      BrainModelSurface::SURFACE_TYPES coordFileType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
      TopologyFile* topologyFile    = NULL;
      TopologyFile::TOPOLOGY_TYPES topologyFileType = TopologyFile::TOPOLOGY_TYPE_UNKNOWN;
      
      bool borderOrBorderProjFileFlag = false;
      
      bool doNotModifyHeaderFlag = false;

      VolumeFile::VOLUME_TYPE volumeType = VolumeFile::VOLUME_TYPE_UNKNOWN;
      VolumeFile* volumeFile = NULL;
      VtkModelFile* vtkModelFile = NULL;
      bool multiVolumeFileFlag = false;
      
      if (filterName == GuiDataFileDialog::areaColorFileFilter) {
         af = theMainWindow->getBrainSet()->getAreaColorFile();
         extension = SpecFile::getAreaColorFileExtension();
      }
      else if (filterName == GuiDataFileDialog::arealEstimationFileFilter) {
         af = theMainWindow->getBrainSet()->getArealEstimationFile();
         extension = SpecFile::getArealEstimationFileExtension();
      }
      else if ((filterName == GuiDataFileDialog::borderRawFileFilter)          ||
               (filterName == GuiDataFileDialog::borderFiducialFileFilter)     ||
               (filterName == GuiDataFileDialog::borderInflatedFileFilter)     ||
               (filterName == GuiDataFileDialog::borderVeryInflatedFileFilter) ||
               (filterName == GuiDataFileDialog::borderSphericalFileFilter)    ||
               (filterName == GuiDataFileDialog::borderEllipsoidalFileFilter)  ||
               (filterName == GuiDataFileDialog::borderCompressedFileFilter)   ||
               (filterName == GuiDataFileDialog::borderHullFileFilter)         ||
               (filterName == GuiDataFileDialog::borderFlatFileFilter)         ||
               (filterName == GuiDataFileDialog::borderFlatLobarFileFilter)    ||
               (filterName == GuiDataFileDialog::borderUnknownFileFilter)      ||
               (filterName == GuiDataFileDialog::borderGenericFileFilter)) {
               
         borderFileType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
         borderOrBorderProjFileFlag = true;
         
         extension = SpecFile::getBorderFileExtension();
         if (borderFileComboBox->count() > 0) {
            const int borderIndex = borderFileComboBox->currentIndex();
            if (borderIndex >= 0) {
               const int bordedrSurfaceIndex = borderFileSurfaceIndex[borderIndex];
               borderSurface = theMainWindow->getBrainSet()->getBrainModelSurface(bordedrSurfaceIndex);
               //BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
               af = NULL;
               
               if (borderTypeComboBox->count() > 0) {
                  borderFileType = borderTypeComboBox->getSurfaceType();
/*
                  const int borderTypeIndex = coordBorderTypeComboBox->currentIndex();
                  if (borderTypeIndex >= 0) {
                     borderOrBorderProjFileFlag = true;
                     switch(borderTypeIndex) {
                        case 0:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "RAW");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_RAW;
                           break;
                        case 1:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "FIDUCIAL");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_FIDUCIAL;
                           break;
                        case 2:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "INFLATED");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_INFLATED;
                           break;
                        case 3:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "VERY_INFLATED");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_VERY_INFLATED;
                           break;
                        case 4:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "SPHERICAL");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
                           break;
                        case 5:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "ELLIPSOID");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL;
                           break;
                        case 6:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "CMW");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL;
                           break;
                        case 7:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "FLAT");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_FLAT;
                           break;
                        case 8:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "FLAT_LOBAR");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR;
                           break;
                        case 9:
                           //af->setHeaderTag(AbstractFile::headerTagConfigurationID, "UNKNOWN");
                           borderFileType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
                           break;
                     }
                  }
*/
               }
            }
         }
      }
      else if (filterName == GuiDataFileDialog::borderGenericFileFilter) {
      }
      else if (filterName == GuiDataFileDialog::borderVolumeFileFilter) {
         af = theMainWindow->getBrainSet()->getVolumeBorderFile();
         extension = SpecFile::getBorderFileExtension();
      }
      else if (filterName == GuiDataFileDialog::borderColorFileFilter) {
         af = theMainWindow->getBrainSet()->getBorderColorFile();
         extension = SpecFile::getBorderColorFileExtension();
      }
      else if (filterName == GuiDataFileDialog::borderProjectionFileFilter) {
         //af = theMainWindow->getBrainSet()->getBorderProjectionFile();
         extension = SpecFile::getBorderProjectionFileExtension();
         borderOrBorderProjFileFlag = true;
      }
      else if (filterName == GuiDataFileDialog::cellFileFilter) {
         af = theMainWindow->getBrainSet()->getCellProjectionFile();
         extension = SpecFile::getCellFileExtension();
         doNotModifyHeaderFlag = true;
      }
      else if (filterName == GuiDataFileDialog::cellColorFileFilter) {
         af = theMainWindow->getBrainSet()->getCellColorFile();
         extension = SpecFile::getCellColorFileExtension();
      }
      else if (filterName == GuiDataFileDialog::cellProjectionFileFilter) {
         af = theMainWindow->getBrainSet()->getCellProjectionFile();
         extension = SpecFile::getCellProjectionFileExtension();
      }
      else if (filterName == GuiDataFileDialog::cellVolumeFileFilter) {
         af = theMainWindow->getBrainSet()->getVolumeCellFile();
         extension = SpecFile::getCellFileExtension();
      }
      else if (filterName == GuiDataFileDialog::cocomacFileFilter) {
         af = theMainWindow->getBrainSet()->getCocomacFile();
         extension = SpecFile::getCocomacConnectivityFileExtension();
      }
      else if (filterName == GuiDataFileDialog::contourFileFilter) {
         BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours();
         if (bmc != NULL) {
            contourFile = bmc->getContourFile();
            af = contourFile;
            extension = SpecFile::getContourFileExtension();
         }
      }
      else if (filterName == GuiDataFileDialog::contourCellFileFilter) {
         af = theMainWindow->getBrainSet()->getContourCellFile();
         extension = SpecFile::getContourCellFileExtension();
      }
      else if (filterName == GuiDataFileDialog::contourCellColorFileFilter) {
         af = theMainWindow->getBrainSet()->getContourCellColorFile();
         extension = SpecFile::getContourCellColorFileExtension();
      }
      else if ((filterName == GuiDataFileDialog::coordinateRawFileFilter)          ||
               (filterName == GuiDataFileDialog::coordinateFiducialFileFilter)     ||
               (filterName == GuiDataFileDialog::coordinateInflatedFileFilter)     ||
               (filterName == GuiDataFileDialog::coordinateVeryInflatedFileFilter) ||
               (filterName == GuiDataFileDialog::coordinateSphericalFileFilter)    ||
               (filterName == GuiDataFileDialog::coordinateEllipsoidalFileFilter)  ||
               (filterName == GuiDataFileDialog::coordinateCompressedFileFilter)   ||
               (filterName == GuiDataFileDialog::coordinateFlatFileFilter)         ||
               (filterName == GuiDataFileDialog::coordinateFlatLobarFileFilter)    ||
               (filterName == GuiDataFileDialog::coordinateHullFileFilter)         ||
               (filterName == GuiDataFileDialog::coordinateUnknownFileFilter)      ||
               (filterName == GuiDataFileDialog::coordinateGenericFileFilter)) {
               
         coordFileType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
         
         extension = SpecFile::getCoordinateFileExtension();
         if (coordFileComboBox->count() > 0) {
            coordFileType = coordTypeComboBox->getSurfaceType();

            const int coordIndex = coordFileComboBox->currentIndex();
            if (coordIndex >= 0) {
               coordFile = coordinateFiles[coordIndex];
               af = coordFile;
               af->setHeaderTag(AbstractFile::headerTagConfigurationID,
                                BrainModelSurface::getSurfaceConfigurationIDFromType(coordFileType));
/*               
               if (coordTypeComboBox->count() > 0) {
                  const int coordIndex = coordBorderTypeComboBox->currentIndex();
                  if (coordIndex >= 0) {
                     switch(coordIndex) {
                        case 0:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "RAW");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_RAW;
                           break;
                        case 1:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "FIDUCIAL");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_FIDUCIAL;
                           break;
                        case 2:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "INFLATED");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_INFLATED;
                           break;
                        case 3:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "VERY_INFLATED");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_VERY_INFLATED;
                           break;
                        case 4:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "SPHERICAL");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
                           break;
                        case 5:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "ELLIPSOID");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL;
                           break;
                        case 6:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "CMW");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL;
                           break;
                        case 7:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "FLAT");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_FLAT;
                           break;
                        case 8:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "FLAT_LOBAR");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR;
                           break;
                        case 9:
                           af->setHeaderTag(AbstractFile::headerTagConfigurationID, "UNKNOWN");
                           coordFileType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
                           break;
                     }
                  }
                  //brainSurfaces[coordIndex]->setSurfaceType(coordFileType);
               }
*/
         
               for (int is = 0; is < theMainWindow->getBrainSet()->getNumberOfBrainModels(); is++) {
                  BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(is);
                  if (bms != NULL) {
                     if (bms->getCoordinateFile() == coordFile) {
                        bms->setStructure(structureComboBox->getSelectedStructure());
                     }
                  }
               }
                                         
               QString frame;
               switch(coordFrameComboBox->currentIndex()) {
                  case 0:
                     frame = "ACPC";
                     break;
                  case 1:
                     frame = "AC";
                     break;
                  case 2:
                     frame = "CS";
                     break;
                  case 3:
                     frame = "CN";
                     break;
                  case 4:
                     frame = "COG";
                     break;
                  case 5:
                     frame = "NA";
                     break;
                  case 6:
                     frame = "SS";
                     break;
                  case 7:
                     frame = "TAL";
                     break;
                  case 8:
                     frame = "UNK";
                     break;
                  default:
                     frame = coordFrameComboBox->currentText();
                     break;
               }
               af->setHeaderTag(AbstractFile::headerTagCoordFrameID, frame);
               
               char orient[4] = "UUU";
               
               switch(xOrientationComboBox->currentIndex()) {
                  case 1:
                     orient[0] = 'L';
                     break;  
                  case 2:
                     orient[0] = 'R';
                     break;  
                  case 3:
                     orient[0] = 'A';
                     break;  
                  case 4:
                     orient[0] = 'P';
                     break;  
                  case 5:
                     orient[0] = 'I';
                     break;  
                  case 6:
                     orient[0] = 'S';
                     break;  
                  default:
                     orient[0] = 'U';
                     break;  
               }
               
               switch(yOrientationComboBox->currentIndex()) {
                  case 1:
                     orient[1] = 'L';
                     break;  
                  case 2:
                     orient[1] = 'R';
                     break;  
                  case 3:
                     orient[1] = 'A';
                     break;  
                  case 4:
                     orient[1] = 'P';
                     break;  
                  case 5:
                     orient[1] = 'I';
                     break;  
                  case 6:
                     orient[1] = 'S';
                     break;  
                  default:
                     orient[1] = 'U';
                     break;  
               }
               switch(zOrientationComboBox->currentIndex()) {
                  case 1:
                     orient[2] = 'L';
                     break;  
                  case 2:
                     orient[2] = 'R';
                     break;  
                  case 3:
                     orient[2] = 'A';
                     break;  
                  case 4:
                     orient[2] = 'P';
                     break;  
                  case 5:
                     orient[2] = 'I';
                     break;  
                  case 6:
                     orient[2] = 'S';
                     break;  
                  default:
                     orient[2] = 'U';
                     break;  
               }

               af->setHeaderTag(AbstractFile::headerTagOrientation, orient);
            }
         }
      }
      else if (filterName == GuiDataFileDialog::cutsFileFilter) {
         af = theMainWindow->getBrainSet()->getCutsFile();
         extension = SpecFile::getCutsFileExtension();
      }
      else if (filterName == GuiDataFileDialog::deformationFieldFileFilter) {
         af = theMainWindow->getBrainSet()->getDeformationFieldFile();
         extension = SpecFile::getDeformationFieldFileExtension();
      }
      else if (filterName == GuiDataFileDialog::fociFileFilter) {
         af = theMainWindow->getBrainSet()->getFociProjectionFile();
         af = NULL;
         extension = SpecFile::getFociFileExtension();
         doNotModifyHeaderFlag = true;
      }
      else if (filterName == GuiDataFileDialog::fociColorFileFilter) {
         af = theMainWindow->getBrainSet()->getFociColorFile();
         extension = SpecFile::getFociColorFileExtension();
      }
      else if (filterName == GuiDataFileDialog::fociProjectionFileFilter) {
         af = theMainWindow->getBrainSet()->getFociProjectionFile();
         extension = SpecFile::getFociProjectionFileExtension();
      }
      else if (filterName == GuiDataFileDialog::fociVolumeFileFilter) {
         af = theMainWindow->getBrainSet()->getVolumeFociFile();
         extension = SpecFile::getFociFileExtension();
      }
      else if (filterName == GuiDataFileDialog::geodesicDistanceFileFilter) {
         af = theMainWindow->getBrainSet()->getGeodesicDistanceFile();
         extension = SpecFile::getGeodesicDistanceFileExtension();
      }
      else if (filterName == GuiDataFileDialog::latitudeLongitudeFileFilter) {
         af = theMainWindow->getBrainSet()->getLatLonFile();
         extension = SpecFile::getLatLonFileExtension();
      }
      else if (filterName == GuiDataFileDialog::metricFileFilter) {
         af = theMainWindow->getBrainSet()->getMetricFile();
         extension = SpecFile::getMetricFileExtension();
      }
      else if (filterName == GuiDataFileDialog::paintFileFilter) {
         af = theMainWindow->getBrainSet()->getPaintFile();
         extension = SpecFile::getPaintFileExtension();
      }
      else if (filterName == GuiDataFileDialog::paletteFileFilter) {
         af = theMainWindow->getBrainSet()->getPaletteFile();
         extension = SpecFile::getPaletteFileExtension();
      }
      else if (filterName == GuiDataFileDialog::paramsFileFilter) {
         af = theMainWindow->getBrainSet()->getParamsFile();
         extension = SpecFile::getParamsFileExtension();
      }
      else if (filterName == GuiDataFileDialog::probAtlasFileFilter) {
         af = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
         extension = SpecFile::getProbabilisticAtlasFileExtension();
      }
      else if (filterName == GuiDataFileDialog::rgbPaintFileFilter) {
         af = theMainWindow->getBrainSet()->getRgbPaintFile();
         extension = SpecFile::getRgbPaintFileExtension();
      }
      else if (filterName == GuiDataFileDialog::sectionFileFilter) {
         af = theMainWindow->getBrainSet()->getSectionFile();
         extension = SpecFile::getSectionFileExtension();
      }
      else if (filterName == GuiDataFileDialog::sceneFileFilter) {
         af = theMainWindow->getBrainSet()->getSceneFile();
         extension = SpecFile::getSceneFileExtension();
      }
      else if (filterName == GuiDataFileDialog::studyMetaDataFileFilter) {
         af = theMainWindow->getBrainSet()->getStudyMetaDataFile();
         extension = SpecFile::getStudyMetaDataFileExtension();
      }
      else if (filterName == GuiDataFileDialog::surfaceShapeFileFilter) {
         af = theMainWindow->getBrainSet()->getSurfaceShapeFile();
         extension = SpecFile::getSurfaceShapeFileExtension();
      }
      else if (filterName == GuiDataFileDialog::surfaceVectorFileFilter) {
         af = theMainWindow->getBrainSet()->getSurfaceVectorFile();
         extension = SpecFile::getSurfaceVectorFileExtension();
      }
      else if (filterName == GuiDataFileDialog::topographyFileFilter) {
         af = theMainWindow->getBrainSet()->getTopographyFile();
         extension = SpecFile::getTopographyFileExtension();
      }
      else if (filterName == GuiDataFileDialog::transformationMatrixFileFilter) {
         af = theMainWindow->getBrainSet()->getTransformationMatrixFile();
         extension = SpecFile::getTransformationMatrixFileExtension();
      }
      else if (filterName == GuiDataFileDialog::volumeMultiFileAfniFilter) {
         af = NULL;
         extension = ".HEAD";
         multiVolumeFileFlag = true;
      }
      else if (filterName == GuiDataFileDialog::volumeMultiFileNiftiFilter) {
         af = NULL;
         multiVolumeFileFlag = true;
         bool zipItFlag = volumeFileGzipCheckBox->isChecked();
         if (name.right(3) == ".gz") {
            zipItFlag = true;
            name = name.left(name.length() - 3);
         }
         if (name.right(SpecFile::getNiftiVolumeFileExtension().length())
             == SpecFile::getNiftiVolumeFileExtension()) {
            name = name.left(name.length() - SpecFile::getNiftiVolumeFileExtension().length());
         }
         extension = SpecFile::getNiftiVolumeFileExtension();
         if (zipItFlag) {
            extension += ".gz";
         }
      }
      else if ((filterName == GuiDataFileDialog::volumeFileAfniFilter)    ||
               (filterName == GuiDataFileDialog::volumeFileSpmMedxFilter) ||
               (filterName == GuiDataFileDialog::volumeFileNiftiWriteFilter)   ||
               (filterName == GuiDataFileDialog::volumeFileWuNilFilter)) {
         if (filterName == GuiDataFileDialog::volumeFileAfniFilter) {
            extension = SpecFile::getAfniVolumeFileExtension();
         }
         else if (filterName == GuiDataFileDialog::volumeFileSpmMedxFilter) {
            extension = SpecFile::getAnalyzeVolumeFileExtension();
         }
         else if (filterName == GuiDataFileDialog::volumeFileNiftiWriteFilter) {
            bool zipItFlag = volumeFileGzipCheckBox->isChecked();
            if (name.right(3) == ".gz") {
               zipItFlag = true;
               name = name.left(name.length() - 3);
            }
            if (name.right(3) == ".gz") {
               name = name.left(name.length() - 3);
            }
            if (name.right(SpecFile::getNiftiVolumeFileExtension().length())
                == SpecFile::getNiftiVolumeFileExtension()) {
               name = name.left(name.length() - SpecFile::getNiftiVolumeFileExtension().length());
            }
            extension = SpecFile::getNiftiVolumeFileExtension();
            if (zipItFlag) {
               extension += ".gz";
            }
         }
         else if (filterName == GuiDataFileDialog::volumeFileWuNilFilter) {
            extension = ".4dfp.ifh";
         }
         else {
            return;
         }
         
         volumeFile = volumeSelectionControl->getSelectedVolumeFile();

         if (volumeFile == NULL) {
            GuiMessageBox::critical(this, "Error", "No volume is selected for saving.  Check\n"
                                                 "the \"Save Parameters\" to see if the correct\n"
                                                 "Volume Type and Volume File are selected.", "OK"); 
            return;
         }
      }
      else if ((filterName == GuiDataFileDialog::topologyClosedFileFilter) ||
               (filterName == GuiDataFileDialog::topologyOpenFileFilter)  ||
               (filterName == GuiDataFileDialog::topologyCutFileFilter)   ||
               (filterName == GuiDataFileDialog::topologyCutLobarFileFilter) ||
               (filterName == GuiDataFileDialog::topologyUnknownFileFilter) ||
               (filterName == GuiDataFileDialog::topologyGenericFileFilter)) {
         extension = SpecFile::getTopoFileExtension();
         if (topologyFileComboBox->count() > 0) {
            topologyFile = topologyFileComboBox->getSelectedTopologyFile();
            if (topologyFile != NULL) {
               af = topologyFile;
               topologyFileType = topologyTypeComboBox->getTopologyType();
            }
         }
      }
      else if (filterName == vtkModelFileFilter) {
         extension = ".vtk";
         if (vtkModelFileComboBox->count() > 0) {
            const int vtkIndex = vtkModelFileComboBox->currentIndex();
            if (vtkIndex >= 0) {
               vtkModelFile = vtkModelFiles[vtkIndex];
               af = vtkModelFile;
            }
            extension = FileUtilities::filenameExtension(vtkModelFile->getFileName());
            if (extension == "vtk") {
               extension = ".vtk";
            }
            else if (extension == "vtp") {
               extension = ".vtp";
            }
            else {
               extension = ".vtk";
            }
            if (af != NULL) {
               const int index = fileEncodingComboBox->currentIndex();
               if ((index >= 0) && (index < static_cast<int>(fileEncodingComboBoxValues.size()))) {
                  switch (fileEncodingComboBoxValues[index]) {
                     case AbstractFile::FILE_FORMAT_ASCII:
                        extension = ".vtk";
                        break;
                     case AbstractFile::FILE_FORMAT_BINARY:
                        extension = ".vtk";
                        break;
                     case AbstractFile::FILE_FORMAT_XML:
                        extension = ".vtp";
                        break;
                     case AbstractFile::FILE_FORMAT_XML_BASE64:
                        extension = ".vtp";
                        break;
                     case AbstractFile::FILE_FORMAT_XML_GZIP_BASE64:
                        extension = ".vtp";
                        break;
                     case AbstractFile::FILE_FORMAT_OTHER:
                        extension = ".vtk";
                        break;
                     case AbstractFile::FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
                        extension = ".vtk";
                        break;
                  }
               }
            }
         }
      }
      else if (filterName == vocabularyFileFilter) {
         af = theMainWindow->getBrainSet()->getVocabularyFile();
         extension = SpecFile::getVocabularyFileExtension();
      }
      else if (filterName == wustlRegionFileFilter) {
         af = theMainWindow->getBrainSet()->getWustlRegionFile();
         extension = SpecFile::getWustlRegionFileExtension();
      }
      else {
         std::cerr << "PROGRAM ERROR: unhandled file type " << filterName.toAscii().constData() << " at " << __LINE__
                  << " in " << __FILE__ << std::endl;
      }
      
      //
      // Add file extension if necessary
      //
      if (autoAppendExtensionCheckBox->isChecked()) {
         if (StringUtilities::endsWith(name, extension) == false) {
            name.append(extension);
         }
      }
      //
      // update stuff in header
      //
      const QString commentText = commentTextEditor->toPlainText();
      const QString pubMedIDText = pubMedIDLineEdit->text();
      if (af != NULL) {
         if (doNotModifyHeaderFlag == false)  {
            af->setFileComment(commentText);
            af->setFilePubMedID(pubMedIDText);
         }
      }
      else if (volumeFile != NULL) {
         volumeFile->setFileComment(commentText);
         volumeFile->setFilePubMedID(pubMedIDText);
         const VolumeFile::VOXEL_DATA_TYPE vdt =
            static_cast<VolumeFile::VOXEL_DATA_TYPE>(
               volumeVoxelDataTypeComboBoxValues[volumeVoxelDataTypeComboBox->currentIndex()]);
         if (vdt == VolumeFile::VOXEL_DATA_TYPE_UNKNOWN) {
            GuiMessageBox::critical(this, "ERROR", "Choose voxel data type.", "OK");
            return;
         }
         double typeMin, typeMax;
         float volMin, volMax;
         VolumeFile::getDataTypeMinMaxValues(vdt, typeMin, typeMax);
         volumeFile->getMinMaxVoxelValues(volMin, volMax);
         if ((volMin < typeMin) || (volMax > typeMax)) {
            std::ostringstream str;
            str << "The voxel values exceed the range of the selected voxel data type.\n"
                << "The voxel values range from " << volMin << " to " << volMax << ".\n"
                << "The allowable values for the data type range from " 
                << typeMin << " to " << typeMax << ".\n"
                << "\n"
                << "You may choose to continue but the volume saved will be invalid.\n"
                << "It is best to press the \"Cancel\" button and either choose a \n"
                << "different voxel data type or to rescale the voxels using the\n"
                << "Data Page on the Edit Volume Attributes Dialog available from\n"
                << "the Volume Menu.";
            if (GuiMessageBox::critical(this, "WARNING", str.str().c_str(), "Continue", "Cancel") != 0) {
               return;
            }
         }
      }
      else if (borderOrBorderProjFileFlag) {
         // nothing
      }
      else if (multiVolumeFileFlag) {
         // nothing
      }
      else if (doNotModifyHeaderFlag) {
         // nothing
      }
      else if (volumeFile == NULL) {
         std::cerr << "PROGRAM ERROR Unknown file " << filterName.toAscii().constData() << " at "
                   <<  __LINE__ << " in " << __FILE__ << std::endl;
      }
      
      //
      // Set the files write type
      //
      if (af != NULL) {
         const int index = fileEncodingComboBox->currentIndex();
         if ((index >= 0) && (index < static_cast<int>(fileEncodingComboBoxValues.size()))) {
            if (doNotModifyHeaderFlag == false) {
               af->setFileWriteType(fileEncodingComboBoxValues[index]);
            }
         }
      }
      
      //
      // Write the file
      // 
      try {
         if (filterName == GuiDataFileDialog::areaColorFileFilter) {
            theMainWindow->getBrainSet()->writeAreaColorFile(name);
         }
         else if (filterName == GuiDataFileDialog::arealEstimationFileFilter) {
            theMainWindow->getBrainSet()->writeArealEstimationFile(name);
         }
         else if ((filterName == GuiDataFileDialog::borderRawFileFilter)  ||
                  (filterName == GuiDataFileDialog::borderFiducialFileFilter) ||
                  (filterName == GuiDataFileDialog::borderInflatedFileFilter) ||
                  (filterName == GuiDataFileDialog::borderVeryInflatedFileFilter) ||
                  (filterName == GuiDataFileDialog::borderSphericalFileFilter) ||
                  (filterName == GuiDataFileDialog::borderEllipsoidalFileFilter) ||
                  (filterName == GuiDataFileDialog::borderCompressedFileFilter) ||
                  (filterName == GuiDataFileDialog::borderHullFileFilter)         ||
                  (filterName == GuiDataFileDialog::borderFlatFileFilter) ||
                  (filterName == GuiDataFileDialog::borderFlatLobarFileFilter) ||
                  (filterName == GuiDataFileDialog::borderUnknownFileFilter) ||
                  (filterName == GuiDataFileDialog::borderGenericFileFilter)) {
            theMainWindow->getBrainSet()->writeBorderFile(name, borderSurface, borderFileType, commentText,
                                   pubMedIDText, removeDuplicateBorderCheckBox->isChecked());
         }
         else if (filterName == GuiDataFileDialog::borderVolumeFileFilter) {
            theMainWindow->getBrainSet()->writeVolumeBorderFile(name);
         }
         else if (filterName == GuiDataFileDialog::borderColorFileFilter) {
            theMainWindow->getBrainSet()->writeBorderColorFile(name);
         }
         else if (filterName == GuiDataFileDialog::borderProjectionFileFilter) {
            theMainWindow->getBrainSet()->writeBorderProjectionFile(name, commentText, pubMedIDText,
                                             removeDuplicateBorderCheckBox->isChecked());
         }
         else if (filterName == GuiDataFileDialog::cellFileFilter) {
            const BrainModelSurface* bms = cellTypeComboBox->getSelectedBrainModelSurface();
            if (bms == NULL) {
               QString msg("There is no surface for which cells should be saved.");
               GuiMessageBox::critical(this, "ERROR", msg, "OK");
               return;
            }

            const int index = fileEncodingComboBox->currentIndex();
            AbstractFile::FILE_FORMAT fileFormat = AbstractFile::FILE_FORMAT_XML;
            if ((index >= 0) && (index < static_cast<int>(fileEncodingComboBoxValues.size()))) {
               fileFormat = fileEncodingComboBoxValues[index];
            }

            theMainWindow->getBrainSet()->writeCellFile(name, bms, fileFormat, commentText);
         }
         else if (filterName == GuiDataFileDialog::cellColorFileFilter) {
            theMainWindow->getBrainSet()->writeCellColorFile(name);
         }
         else if (filterName == GuiDataFileDialog::cellProjectionFileFilter) {
            theMainWindow->getBrainSet()->writeCellProjectionFile(name);
         }
         else if (filterName == GuiDataFileDialog::cellVolumeFileFilter) {
            theMainWindow->getBrainSet()->writeVolumeCellFile(name);
         }
         else if (filterName == GuiDataFileDialog::cocomacFileFilter) {
            theMainWindow->getBrainSet()->writeCocomacConnectivityFile(name);
         }
         else if (filterName == GuiDataFileDialog::contourFileFilter) {
            theMainWindow->getBrainSet()->writeContourFile(name, contourFile);
         }
         else if (filterName == GuiDataFileDialog::contourCellFileFilter) {
            theMainWindow->getBrainSet()->writeContourCellFile(name);
         }
         else if (filterName == GuiDataFileDialog::contourCellColorFileFilter) {
            theMainWindow->getBrainSet()->writeContourCellColorFile(name);
         }
         else if ((filterName == GuiDataFileDialog::coordinateRawFileFilter)          ||
               (filterName == GuiDataFileDialog::coordinateFiducialFileFilter)     ||
               (filterName == GuiDataFileDialog::coordinateInflatedFileFilter)     ||
               (filterName == GuiDataFileDialog::coordinateVeryInflatedFileFilter) ||
               (filterName == GuiDataFileDialog::coordinateSphericalFileFilter)    ||
               (filterName == GuiDataFileDialog::coordinateEllipsoidalFileFilter)  ||
               (filterName == GuiDataFileDialog::coordinateCompressedFileFilter)   ||
               (filterName == GuiDataFileDialog::coordinateFlatFileFilter)         ||
               (filterName == GuiDataFileDialog::coordinateFlatLobarFileFilter)    ||
               (filterName == GuiDataFileDialog::coordinateHullFileFilter)         ||
               (filterName == GuiDataFileDialog::coordinateUnknownFileFilter)      ||
               (filterName == GuiDataFileDialog::coordinateGenericFileFilter)) {
            theMainWindow->getBrainSet()->writeCoordinateFile(name, coordFileType, coordFile);
            GuiFilesModified fm;
            fm.setCoordinateModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         else if (filterName == GuiDataFileDialog::cutsFileFilter) {
            theMainWindow->getBrainSet()->writeCutsFile(name);
         }
         else if (filterName == GuiDataFileDialog::deformationFieldFileFilter) {
            theMainWindow->getBrainSet()->writeDeformationFieldFile(name);
         }
         else if (filterName == GuiDataFileDialog::fociFileFilter) {
            const int encodeIndex = fileEncodingComboBox->currentIndex();
            AbstractFile::FILE_FORMAT fileFormat = AbstractFile::FILE_FORMAT_XML;
            if ((encodeIndex >= 0) && (encodeIndex < static_cast<int>(fileEncodingComboBoxValues.size()))) {
               fileFormat = fileEncodingComboBoxValues[encodeIndex];
            }
               
            if (fociSaveOriginalCoordsRadioButton->isChecked()) {
               theMainWindow->getBrainSet()->writeFociFileOriginalCoordinates(name, 
                                                                              fileFormat, 
                                                                              commentText);
            }
            else if (fociSaveProjectedToSurfacesRadioButton->isChecked()) {
               const BrainModelSurface* leftBms = fociLeftSurfaceComboBox->getSelectedBrainModelSurface();
               const BrainModelSurface* rightBms = fociRightSurfaceComboBox->getSelectedBrainModelSurface();
               if ((leftBms == NULL) && (rightBms == NULL)) {
                  QString msg("There is no surface selected for which foci should be saved.");
                  GuiMessageBox::critical(this, "ERROR", msg, "OK");
                  return;
               }
               
               theMainWindow->getBrainSet()->writeFociFile(name, leftBms, rightBms, fileFormat, commentText);
            }
            else {
               std::cout << "PROGRAM ERROR at line " << __LINE__
                         << " in file " << __FILE__ ": no foci save selected has been made."
                         << std::endl;
            }
         }
         else if (filterName == GuiDataFileDialog::fociColorFileFilter) {
            theMainWindow->getBrainSet()->writeFociColorFile(name);
         }
         else if (filterName == GuiDataFileDialog::fociProjectionFileFilter) {
            theMainWindow->getBrainSet()->writeFociProjectionFile(name);
         }
         else if (filterName == GuiDataFileDialog::fociVolumeFileFilter) {
            theMainWindow->getBrainSet()->writeVolumeFociFile(name);
         }
         else if (filterName == GuiDataFileDialog::geodesicDistanceFileFilter) {
            theMainWindow->getBrainSet()->writeGeodesicDistanceFile(name);
         }
         else if (filterName == GuiDataFileDialog::latitudeLongitudeFileFilter) {
            theMainWindow->getBrainSet()->writeLatLonFile(name);
         }
         else if (filterName == GuiDataFileDialog::metricFileFilter) {
            theMainWindow->getBrainSet()->writeMetricFile(name);
         }
         else if (filterName == GuiDataFileDialog::paintFileFilter) {
            theMainWindow->getBrainSet()->writePaintFile(name);
         }
         else if (filterName == GuiDataFileDialog::paletteFileFilter) {
            theMainWindow->getBrainSet()->writePaletteFile(name);
         }
         else if (filterName == GuiDataFileDialog::paramsFileFilter) {
            theMainWindow->getBrainSet()->writeParamsFile(name);
         }
         else if (filterName == GuiDataFileDialog::probAtlasFileFilter) {
            theMainWindow->getBrainSet()->writeProbabilisticAtlasFile(name);
         }
         else if (filterName == GuiDataFileDialog::rgbPaintFileFilter) {
            theMainWindow->getBrainSet()->writeRgbPaintFile(name);
         }
         else if (filterName == GuiDataFileDialog::sceneFileFilter) {
            theMainWindow->getBrainSet()->writeSceneFile(name);
         }
         else if (filterName == GuiDataFileDialog::sectionFileFilter) {
            theMainWindow->getBrainSet()->writeSectionFile(name);
         }
         else if (filterName == GuiDataFileDialog::studyMetaDataFileFilter) {
            theMainWindow->getBrainSet()->writeStudyMetaDataFile(name);
         }
         else if (filterName == GuiDataFileDialog::surfaceShapeFileFilter) {
            theMainWindow->getBrainSet()->writeSurfaceShapeFile(name);
         }
         else if (filterName == GuiDataFileDialog::surfaceVectorFileFilter) {
            theMainWindow->getBrainSet()->writeSurfaceVectorFile(name);
         }
         else if (filterName == GuiDataFileDialog::topographyFileFilter) {
            theMainWindow->getBrainSet()->writeTopographyFile(name);
         }
         else if ((filterName == volumeMultiFileAfniFilter) ||
                  (filterName == volumeMultiFileNiftiFilter)) {
            
            if (filterName == volumeMultiFileAfniFilter) {
               //
               // Regular expression for end of valid AFNI volume name
               //
               const QRegExp regExp(".\\+[\\w]+\\.HEAD$");
               if (regExp.indexIn(name) < 0) {
                  QString msg(FileUtilities::basename(name));
                  msg.append("\n");
                  msg.append("For AFNI to read this volume file, it's name must end with\n"
                             "\"+orig.HEAD\", \"+acpc.HEAD\", or \"+tlrc.HEAD\".  If you \n"
                             "are not sure which to use, add \"+orig\".  Using \"+acpc\" \n"
                             "or \"+tlrc\" may cause an error in AFNI if there is not a\n"
                             "corresponding \"+orig\" file or if your file does not contain\n"
                             "warping data.\n"
                             "\n"
                             "Do you want to continue saving with the name you have \n"
                             "entered or change the name of the file ?");
                  if (GuiMessageBox::warning(this, "AFNI", msg, "Continue Saving",
                                             "Change Name of File") != 0) {
                     return;
                  }
                  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               }
            }
            
            volumeType = volumeSelectionControl->getSelectedVolumeType();
            std::vector<VolumeFile*> volumes;
            switch(volumeType) {
               case VolumeFile::VOLUME_TYPE_ANATOMY:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeAnatomyFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeFunctionalFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_PAINT:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumePaintFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_RGB:   
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeRgbFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_ROI:
                  GuiMessageBox::critical(this, "Error", "Select the type of volume and the\n"
                                                       "specific volume to be saved.", "OK"); 
                  return;
                  break;    
               case VolumeFile::VOLUME_TYPE_SEGMENTATION:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeSegmentationFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_VECTOR:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeVectorFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_UNKNOWN:
                  GuiMessageBox::critical(this, "Error", "Select the type of volume and the\n"
                                                       "specific volume to be saved.", "OK"); 
                  return;
                  break;    
            }
            
            //
            // Set voxel data type
            //
            volumes[0]->setVoxelDataType(volumeVoxelDataTypeComboBoxValues[volumeVoxelDataTypeComboBox->currentIndex()]);

            if (volumes.empty()) {
               GuiMessageBox::critical(this, "ERROR", "There are no volumes of the selected type.", "OK");
               return;
            }
            theMainWindow->getBrainSet()->writeMultiVolumeFile(name,
                                        volumeType, commentText, volumes,
                                        volumes[0]->getVoxelDataType(),
                                        volumeFileGzipCheckBox->isChecked());
                                        
            GuiFilesModified fm;
            fm.setVolumeModified();
            theMainWindow->fileModificationUpdate(fm);
         }
/*
         else if (filterName == volumeMultiFileNiftiFilter) {
            volumeType = volumeSelectionControl->getSelectedVolumeType();
            std::vector<VolumeFile*> volumes;
            switch(volumeType) {
               case VolumeFile::VOLUME_TYPE_ANATOMY:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeAnatomyFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeFunctionalFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_PAINT:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumePaintFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_RGB:   
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeRgbFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_SEGMENTATION:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeSegmentationFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_VECTOR:
                  for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
                     volumes.push_back(theMainWindow->getBrainSet()->getVolumeVectorFile(i));
                  }
                  break;
               case VolumeFile::VOLUME_TYPE_UNKNOWN:
                  GuiMessageBox::critical(this, "Error", "Select the type of volume and the\n"
                                                       "specific volume to be saved.", "OK"); 
               return;
                  break;    
            }
            
            //
            // Set voxel data type
            //
            volumes[0]->setVoxelDataType(volumeVoxelDataTypeComboBoxValues[volumeVoxelDataTypeComboBox->currentIndex()]);

            if (volumes.empty()) {
               GuiMessageBox::critical(this, "ERROR", "There are no volumes of the selected type.", "OK");
               return;
            }
            theMainWindow->getBrainSet()->writeMultiVolumeFile(name
                                        volumeType, commentText, volumes,
                                        volumes[0]->getVoxelDataType(),
                                        false);
                                        
            GuiFilesModified fm;
            fm.setVolumeModified();
            theMainWindow->fileModificationUpdate(fm);
         }        
*/
         else if (filterName == GuiDataFileDialog::volumeFileAfniFilter) {
            //
            // Regular expression for end of valid AFNI volume name
            //
            const QRegExp regExp(".\\+[\\w]+\\.HEAD$");
            if (regExp.indexIn(name) < 0) {
               QString msg(FileUtilities::basename(name));
               msg.append("\n");
               msg.append("For AFNI to read this volume file, it's name must end with\n"
                          "\"+orig.HEAD\", \"+acpc.HEAD\", or \"+tlrc.HEAD\".  If you \n"
                          "are not sure which to use, add \"+orig\".  Using \"+acpc\" \n"
                          "or \"+tlrc\" may cause an error in AFNI if there is not a\n"
                          "corresponding \"+orig\" file or if your file does not contain\n"
                          "warping data.\n"
                          "\n"
                          "Do you want to continue saving with the name you have \n"
                          "entered or change the name of the file ?");
               if (GuiMessageBox::warning(this, "AFNI", msg, "Continue Saving",
                                          "Change Name of File") != 0) {
                  return;
               }
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            }
            if (volumeFile != NULL) {
               volumeFile->setVoxelDataType(volumeVoxelDataTypeComboBoxValues[volumeVoxelDataTypeComboBox->currentIndex()]);
               volumeFile->setDescriptiveLabel(volumeSelectionControl->getSelectedVolumeDescriptiveLabel());
               theMainWindow->getBrainSet()->writeVolumeFile(name, VolumeFile::FILE_READ_WRITE_TYPE_AFNI, 
                                      volumeSelectionControl->getSelectedVolumeType(), volumeFile,
                                      volumeFile->getVoxelDataType(),
                                      volumeFileGzipCheckBox->isChecked());
               GuiFilesModified fm;
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
         }
         else if (filterName == GuiDataFileDialog::volumeFileSpmMedxFilter) {
            if (volumeFile != NULL) {
               volumeFile->setVoxelDataType(volumeVoxelDataTypeComboBoxValues[volumeVoxelDataTypeComboBox->currentIndex()]);
               volumeFile->setDescriptiveLabel(volumeSelectionControl->getSelectedVolumeDescriptiveLabel());
               theMainWindow->getBrainSet()->writeVolumeFile(name, VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX, 
                                      volumeSelectionControl->getSelectedVolumeType(), volumeFile,
                                      volumeFile->getVoxelDataType(),
                                      false);
               GuiFilesModified fm;
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
         }
         else if (filterName == GuiDataFileDialog::volumeFileNiftiWriteFilter) {
            if (volumeFile != NULL) {
               volumeFile->setVoxelDataType(volumeVoxelDataTypeComboBoxValues[volumeVoxelDataTypeComboBox->currentIndex()]);
               volumeFile->setDescriptiveLabel(volumeSelectionControl->getSelectedVolumeDescriptiveLabel());
               theMainWindow->getBrainSet()->writeVolumeFile(name, VolumeFile::FILE_READ_WRITE_TYPE_NIFTI, 
                                      volumeSelectionControl->getSelectedVolumeType(), volumeFile,
                                      volumeFile->getVoxelDataType(),
                                      false);
               GuiFilesModified fm;
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
         }
         else if (filterName == GuiDataFileDialog::volumeFileWuNilFilter) {
            if (volumeFile != NULL) {
               volumeFile->setVoxelDataType(VolumeFile::VOXEL_DATA_TYPE_FLOAT);
               volumeFile->setDescriptiveLabel(volumeSelectionControl->getSelectedVolumeDescriptiveLabel());
               theMainWindow->getBrainSet()->writeVolumeFile(name, VolumeFile::FILE_READ_WRITE_TYPE_WUNIL, 
                                      volumeSelectionControl->getSelectedVolumeType(), volumeFile,
                                      volumeFile->getVoxelDataType(),
                                      false);
               GuiFilesModified fm;
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
         }
         else if (filterName == GuiDataFileDialog::transformationMatrixFileFilter) {
            theMainWindow->getBrainSet()->writeTransformationMatrixFile(name);
         }
         else if ((filterName == GuiDataFileDialog::topologyClosedFileFilter) ||
                  (filterName == GuiDataFileDialog::topologyOpenFileFilter)  ||
                  (filterName == GuiDataFileDialog::topologyCutFileFilter)   ||
                  (filterName == GuiDataFileDialog::topologyCutLobarFileFilter) ||
                  (filterName == GuiDataFileDialog::topologyUnknownFileFilter) ||
                  (filterName == GuiDataFileDialog::topologyGenericFileFilter)) {
            theMainWindow->getBrainSet()->writeTopologyFile(name, topologyFileType, topologyFile);
         }
         else if (filterName == GuiDataFileDialog::vtkModelFileFilter) {
            theMainWindow->getBrainSet()->writeVtkModelFile(name, vtkModelFile);
         }
         else if (filterName == GuiDataFileDialog::vocabularyFileFilter) {
            theMainWindow->getBrainSet()->writeVocabularyFile(name);
         }
         else if (filterName == GuiDataFileDialog::wustlRegionFileFilter) {
            theMainWindow->getBrainSet()->writeWustlRegionFile(name);
         }
         else {
            std::cerr << "PROGRAM ERROR: unhandled file type " << filterName.toAscii().constData() << " at " << __LINE__
                     << " in " << __FILE__ << std::endl;
         }
      }
      catch (FileException& e) {
         GuiMessageBox::critical(this, "File Write Error", e.whatQString(), "OK");
         return;
      }
      
      QApplication::restoreOverrideCursor(); 
      
   }
   
   savedPositionAndSize = geometry();
   savedPositionAndSizeValid = true;
   
   Q3FileDialog::done(r);
}

/**
 * Destructor
 */
GuiSaveDataFileDialog::~GuiSaveDataFileDialog()
{
}

/**
 * Slot called when a file filter is selected
 */
void
GuiSaveDataFileDialog::filterSelectedSlot(const QString& filterNameIn)
{
   filterName = filterNameIn;
   updateDialog();
   updateSaveParameters();
}

/**
 * Update the file write type combo box.
 */
void
GuiSaveDataFileDialog::updateFileWriteTypeComboBox(AbstractFile* af)
{
   fileEncodingComboBox->clear();
   fileEncodingComboBoxValues.clear();
   
   AbstractFile::FILE_FORMAT writeType = AbstractFile::FILE_FORMAT_OTHER;
   
   if (af != NULL) {
      if (af->getCanWrite(AbstractFile::FILE_FORMAT_ASCII)) {
         fileEncodingComboBox->addItem("Ascii (Text)");
         fileEncodingComboBoxValues.push_back(AbstractFile::FILE_FORMAT_ASCII);
      }
      
      if (af->getCanWrite(AbstractFile::FILE_FORMAT_BINARY)) {
         fileEncodingComboBox->addItem("Binary");
         fileEncodingComboBoxValues.push_back(AbstractFile::FILE_FORMAT_BINARY);
      }
      
      if (af->getCanWrite(AbstractFile::FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE)) {
         fileEncodingComboBox->addItem("Comma Separated Value File");
         fileEncodingComboBoxValues.push_back(AbstractFile::FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE);
      }
      
      if (af->getCanWrite(AbstractFile::FILE_FORMAT_XML)) {
         fileEncodingComboBox->addItem("XML");
         fileEncodingComboBoxValues.push_back(AbstractFile::FILE_FORMAT_XML);
      }
      
      if (af->getCanWrite(AbstractFile::FILE_FORMAT_XML_BASE64)) {
         fileEncodingComboBox->addItem("XML Base64");
         fileEncodingComboBoxValues.push_back(AbstractFile::FILE_FORMAT_XML_BASE64);
      }
      
      if (af->getCanWrite(AbstractFile::FILE_FORMAT_XML_GZIP_BASE64)) {
         fileEncodingComboBox->addItem("XML GZip Base64");
         fileEncodingComboBoxValues.push_back(AbstractFile::FILE_FORMAT_XML_GZIP_BASE64);
      }
      
      writeType = af->getFileWriteType();
   }
   else {
      fileEncodingComboBox->addItem("Other");
      fileEncodingComboBoxValues.push_back(AbstractFile::FILE_FORMAT_OTHER);
   }  
    
   int defaultIndex = 0;   
   for (int i = 0; i < fileEncodingComboBox->count(); i++) {
      if (fileEncodingComboBoxValues[i] == writeType) {
         defaultIndex = i;
         break;
      }
   }
      
   fileEncodingComboBox->setCurrentIndex(defaultIndex);
}

/**
 * Update the save parameters
 */
void
GuiSaveDataFileDialog::updateSaveParameters()
{
   AbstractFile* af = NULL;
   VolumeFile::VOLUME_TYPE volumeType = VolumeFile::VOLUME_TYPE_UNKNOWN;
   VolumeFile*   volumeFile = NULL;
   
   bool borderFileFlag   = false;
   bool borderProjFileFlag = false;
   bool cellFileFlag     = false;
   bool coordFileFlag    = false;
   bool fociFileFlag     = false;
   bool topologyFileFlag = false;
   bool volumeFileFlag   = false;
   bool volumeMultiFileFlag = false;
   bool vtkModelFileFlag = false;
   
   CellFile dummyCellFile;
   FociFile dummyFociFile;
   
   volumeSelectionControl->showOnlyVolumeType(false);
   
   if (filterName == GuiDataFileDialog::areaColorFileFilter) {
      af = theMainWindow->getBrainSet()->getAreaColorFile();
   }
   else if (filterName == GuiDataFileDialog::arealEstimationFileFilter) {
      af = theMainWindow->getBrainSet()->getArealEstimationFile();
   }
   else if ((filterName == GuiDataFileDialog::borderRawFileFilter)          ||
            (filterName == GuiDataFileDialog::borderFiducialFileFilter)     ||
            (filterName == GuiDataFileDialog::borderInflatedFileFilter)     ||
            (filterName == GuiDataFileDialog::borderVeryInflatedFileFilter) ||
            (filterName == GuiDataFileDialog::borderSphericalFileFilter)    ||
            (filterName == GuiDataFileDialog::borderEllipsoidalFileFilter)  ||
            (filterName == GuiDataFileDialog::borderCompressedFileFilter)   ||
            (filterName == GuiDataFileDialog::borderFlatFileFilter)         ||
            (filterName == GuiDataFileDialog::borderFlatLobarFileFilter)    ||
            (filterName == GuiDataFileDialog::borderHullFileFilter)         ||
            (filterName == GuiDataFileDialog::borderUnknownFileFilter)      ||
            (filterName == GuiDataFileDialog::borderGenericFileFilter)) {
      borderFileFlag = true;
      //if (borderFileComboBox->count() > 0) {
      //   const int borderIndex = borderFileComboBox->currentIndex();
         //if (borderIndex < theMainWindow->getBrainSet()->getNumberOfBorderFiles()) {
            //af = theMainWindow->getBrainSet()->getBorderFile(borderIndex);
         //}
      //}
   }
   else if (filterName == GuiDataFileDialog::borderVolumeFileFilter) {
      af = theMainWindow->getBrainSet()->getVolumeBorderFile();
   }
   else if (filterName == GuiDataFileDialog::borderColorFileFilter) {
      af = theMainWindow->getBrainSet()->getBorderColorFile();
   }
   else if (filterName == GuiDataFileDialog::borderProjectionFileFilter) {
      //af = theMainWindow->getBrainSet()->getBorderProjectionFile();
      borderProjFileFlag = true;
   }
   else if (filterName == GuiDataFileDialog::cellFileFilter) {
      dummyCellFile.setFileComment(theMainWindow->getBrainSet()->getCellProjectionFile()->getFileComment());
      af = &dummyCellFile;
      cellFileFlag = true;
   }
   else if (filterName == GuiDataFileDialog::cellColorFileFilter) {
      af = theMainWindow->getBrainSet()->getCellColorFile();
   }
   else if (filterName == GuiDataFileDialog::cellProjectionFileFilter) {
      af = theMainWindow->getBrainSet()->getCellProjectionFile();
   }
   else if (filterName == GuiDataFileDialog::cellVolumeFileFilter) {
      af = theMainWindow->getBrainSet()->getVolumeCellFile();
   }
   else if (filterName == GuiDataFileDialog::cocomacFileFilter) {
      af = theMainWindow->getBrainSet()->getCocomacFile();
   }
   else if (filterName == GuiDataFileDialog::contourFileFilter) {
      BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours();
      if (bmc != NULL) {
         af = bmc->getContourFile();
      }
   }
   else if (filterName == GuiDataFileDialog::contourCellFileFilter) {
      af = theMainWindow->getBrainSet()->getContourCellFile();
   }
   else if (filterName == GuiDataFileDialog::contourCellColorFileFilter) {
      af = theMainWindow->getBrainSet()->getContourCellColorFile();
   }
   else if ((filterName == GuiDataFileDialog::coordinateRawFileFilter)          ||
            (filterName == GuiDataFileDialog::coordinateFiducialFileFilter)     ||
            (filterName == GuiDataFileDialog::coordinateInflatedFileFilter)     ||
            (filterName == GuiDataFileDialog::coordinateVeryInflatedFileFilter) ||
            (filterName == GuiDataFileDialog::coordinateSphericalFileFilter)    ||
            (filterName == GuiDataFileDialog::coordinateEllipsoidalFileFilter)  ||
            (filterName == GuiDataFileDialog::coordinateCompressedFileFilter)   ||
            (filterName == GuiDataFileDialog::coordinateFlatFileFilter)         ||
            (filterName == GuiDataFileDialog::coordinateFlatLobarFileFilter)    ||
            (filterName == GuiDataFileDialog::coordinateHullFileFilter)         ||
            (filterName == GuiDataFileDialog::coordinateUnknownFileFilter)      ||
            (filterName == GuiDataFileDialog::coordinateGenericFileFilter)) {
      coordFileFlag = true;
      if (coordFileComboBox->count() > 0) {
         const int coordIndex = coordFileComboBox->currentIndex();
         if (coordIndex >= 0) {
            af = coordinateFiles[coordIndex];
         }
      }
   }
   else if (filterName == GuiDataFileDialog::cutsFileFilter) {
      af = theMainWindow->getBrainSet()->getCutsFile();
   }
   else if (filterName == GuiDataFileDialog::deformationFieldFileFilter) {
      af = theMainWindow->getBrainSet()->getDeformationFieldFile();
   }
   else if (filterName == GuiDataFileDialog::fociFileFilter) {
      dummyFociFile.setFileComment(theMainWindow->getBrainSet()->getFociProjectionFile()->getFileComment());
      af = &dummyFociFile;
      fociFileFlag = true;
   }
   else if (filterName == GuiDataFileDialog::fociColorFileFilter) {
      af = theMainWindow->getBrainSet()->getFociColorFile();
   }
   else if (filterName == GuiDataFileDialog::fociProjectionFileFilter) {
      af = theMainWindow->getBrainSet()->getFociProjectionFile();
   }
   else if (filterName == GuiDataFileDialog::fociVolumeFileFilter) {
      af = theMainWindow->getBrainSet()->getVolumeFociFile();
   }
   else if (filterName == GuiDataFileDialog::geodesicDistanceFileFilter) {
      af = theMainWindow->getBrainSet()->getGeodesicDistanceFile();
   }
   else if (filterName == GuiDataFileDialog::latitudeLongitudeFileFilter) {
      af = theMainWindow->getBrainSet()->getLatLonFile();
   }
   else if (filterName == GuiDataFileDialog::metricFileFilter) {
      af = theMainWindow->getBrainSet()->getMetricFile();
   }
   else if (filterName == GuiDataFileDialog::paintFileFilter) {
      af = theMainWindow->getBrainSet()->getPaintFile();
   }
   else if (filterName == GuiDataFileDialog::paletteFileFilter) {
      af = theMainWindow->getBrainSet()->getPaletteFile();
   }
   else if (filterName == GuiDataFileDialog::paramsFileFilter) {
      af = theMainWindow->getBrainSet()->getParamsFile();
   }
   else if (filterName == GuiDataFileDialog::probAtlasFileFilter) {
      af = theMainWindow->getBrainSet()->getProbabilisticAtlasSurfaceFile();
   }
   else if (filterName == GuiDataFileDialog::rgbPaintFileFilter) {
      af = theMainWindow->getBrainSet()->getRgbPaintFile();
   }
   else if (filterName == GuiDataFileDialog::sceneFileFilter) {
      af = theMainWindow->getBrainSet()->getSceneFile();
   }
   else if (filterName == GuiDataFileDialog::sectionFileFilter) {
      af = theMainWindow->getBrainSet()->getSectionFile();
   }
   else if (filterName == GuiDataFileDialog::studyMetaDataFileFilter) {
      af = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   }
   else if (filterName == GuiDataFileDialog::surfaceShapeFileFilter) {
      af = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   }
   else if (filterName == GuiDataFileDialog::surfaceVectorFileFilter) {
      af = theMainWindow->getBrainSet()->getSurfaceVectorFile();
   }
   else if (filterName == GuiDataFileDialog::topographyFileFilter) {
      af = theMainWindow->getBrainSet()->getTopographyFile();
   }
   else if (filterName == GuiDataFileDialog::transformationMatrixFileFilter) {
      af = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   }
   else if ((filterName == GuiDataFileDialog::topologyClosedFileFilter) ||
            (filterName == GuiDataFileDialog::topologyOpenFileFilter)  ||
            (filterName == GuiDataFileDialog::topologyCutFileFilter)   ||
            (filterName == GuiDataFileDialog::topologyCutLobarFileFilter) ||
            (filterName == GuiDataFileDialog::topologyUnknownFileFilter) ||
            (filterName == GuiDataFileDialog::topologyGenericFileFilter)) {
      topologyFileFlag = true;
      af = topologyFileComboBox->getSelectedTopologyFile();
   }
   else if (filterName == GuiDataFileDialog::vtkModelFileFilter) {
      vtkModelFileFlag = true;
      if (vtkModelFileComboBox->count() > 0) {
         const int modelIndex = vtkModelFileComboBox->currentIndex();
         if (modelIndex >= 0) {
            af = vtkModelFiles[modelIndex];
         }
      }
   }
   else if (filterName == GuiDataFileDialog::vocabularyFileFilter) {
      af = theMainWindow->getBrainSet()->getVocabularyFile();
   }
   else if (filterName == GuiDataFileDialog::wustlRegionFileFilter) {
      af = theMainWindow->getBrainSet()->getWustlRegionFile();
   }
   else if (filterName == volumeMultiFileAfniFilter) {
      volumeFileFlag = true;
      volumeMultiFileFlag = true;
      volumeType = volumeSelectionControl->getSelectedVolumeType();
      volumeSelectionControl->showOnlyVolumeType(true);
      updateVolumeSavingItems();
   }
   else if (filterName == volumeMultiFileNiftiFilter) {
      volumeFileFlag = true;
      volumeMultiFileFlag = true;
      volumeType = volumeSelectionControl->getSelectedVolumeType();
      volumeSelectionControl->showOnlyVolumeType(true);
      updateVolumeSavingItems();
   }
   else if ((filterName == GuiDataFileDialog::volumeFileAfniFilter) ||
            (filterName == GuiDataFileDialog::volumeFileSpmMedxFilter) ||
            (filterName == GuiDataFileDialog::volumeFileNiftiWriteFilter) ||
            (filterName == GuiDataFileDialog::volumeFileWuNilFilter)) {
      volumeFileFlag = true;
      volumeType = volumeSelectionControl->getSelectedVolumeType();
      volumeFile = volumeSelectionControl->getSelectedVolumeFile();
      updateVolumeSavingItems();
   }
   else {
      std::cerr << "PROGRAM ERROR: unhandled file type " << filterName.toAscii().constData() << " at " << __LINE__
               << " in " << __FILE__ << std::endl;
   }
   
   commentWidget->setHidden(false);
   pubMedIDWidget->setHidden(false);
   pubMedIDLineEdit->setText("");
   
   updateFileWriteTypeComboBox(af);
   
   setSelection("");
   if (af != NULL) {
      //SpecFile* sf = theMainWindow->getBrainSet()->getSpecFile();
      //if (sf->getFileNamePath().empty()  == false) {
      //   sf->setCurrentDirectoryToSpecFileDirectory();
      //}
      commentTextEditor->setPlainText(af->getFileComment());
      pubMedIDLineEdit->setText(af->getFilePubMedID());
      setSelection(FileUtilities::basename(af->getFileName()));
   }
   else if (filterName == cellFileFilter) {
      commentTextEditor->setPlainText(theMainWindow->getBrainSet()->getCellProjectionFile()->getFileComment());
      pubMedIDLineEdit->setText(theMainWindow->getBrainSet()->getCellProjectionFile()->getFilePubMedID());
   }
   else if (filterName == fociFileFilter) {
      commentTextEditor->setPlainText(theMainWindow->getBrainSet()->getFociProjectionFile()->getFileComment());
      pubMedIDLineEdit->setText(theMainWindow->getBrainSet()->getFociProjectionFile()->getFilePubMedID());
   }
   else if (filterName == volumeMultiFileAfniFilter) {
      volumeType = volumeSelectionControl->getSelectedVolumeType();
      QString text;
      switch(volumeType) {
         case VolumeFile::VOLUME_TYPE_ANATOMY:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeAnatomyFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeFunctionalFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_PAINT:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumePaintFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_RGB:      
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeRgbFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_ROI:
            break;
         case VolumeFile::VOLUME_TYPE_SEGMENTATION:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeSegmentationFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_VECTOR:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeVectorFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_UNKNOWN:
            break;     
      }
      commentTextEditor->setPlainText(text);
   }
   else if (filterName == volumeMultiFileNiftiFilter) {
      volumeType = volumeSelectionControl->getSelectedVolumeType();
      QString text;
      switch(volumeType) {
         case VolumeFile::VOLUME_TYPE_ANATOMY:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeAnatomyFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeFunctionalFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_PAINT:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumePaintFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_RGB:      
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeRgbFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_ROI:
            break;
         case VolumeFile::VOLUME_TYPE_SEGMENTATION:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeSegmentationFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_VECTOR:
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
               text.append(theMainWindow->getBrainSet()->getVolumeVectorFile(i)->getFileComment());
               text.append("\n");
            }
            break;
         case VolumeFile::VOLUME_TYPE_UNKNOWN:
            break;     
      }
      commentTextEditor->setPlainText(text);
   }
   else if (volumeFile != NULL) {
      commentTextEditor->setPlainText(volumeFile->getFileComment());
      pubMedIDLineEdit->setText(volumeFile->getFilePubMedID());
      volumeFileGzipCheckBox->setChecked(volumeFile->getDataFileWasZipped());
      volumeVoxelDataTypeComboBox->setCurrentIndex(0);
      int floatIndex = -1;
      for (unsigned int i = 0; i < volumeVoxelDataTypeComboBoxValues.size(); i++) {
         if (volumeVoxelDataTypeComboBoxValues[i] == volumeFile->getVoxelDataType()) {
            volumeVoxelDataTypeComboBox->setCurrentIndex(i);
            floatIndex = -1;
            break;
         }
         else if (volumeVoxelDataTypeComboBoxValues[i] == VolumeFile::VOXEL_DATA_TYPE_FLOAT) {
            floatIndex = i;
         }
      }
      if (floatIndex >= 0) {
         volumeVoxelDataTypeComboBox->setCurrentIndex(floatIndex);
      }
   }
   else if (borderFileFlag) {
      if (borderFileComboBox->count() > 0) {
         const int brainModelIndex = borderFileComboBox->currentIndex();
         if ((brainModelIndex >= 0) && (brainModelIndex < theMainWindow->getBrainSet()->getNumberOfBrainModels())) {
            const BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(brainModelIndex);
            BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
            BrainModelBorderFileInfo* bmi = bmbs->getBorderFileInfo(bms->getSurfaceType());
            commentTextEditor->setPlainText(bmi->getFileComment());
            pubMedIDLineEdit->setText(bmi->getPubMedID());
            setSelection(FileUtilities::basename(bmi->getFileName()));
            borderTypeComboBox->setSurfaceType(bms->getSurfaceType());
         }
      }
   }
   else if (borderProjFileFlag) {
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      BrainModelBorderFileInfo* bmi = bmbs->getBorderProjectionFileInfo();
      commentTextEditor->setPlainText(bmi->getFileComment());
      pubMedIDLineEdit->setText(bmi->getPubMedID());
      setSelection(FileUtilities::basename(bmi->getFileName()));
   }
   
   fociGroupBox->setHidden(true);
   cellGroupBox->setHidden(true);
   if (cellFileFlag) {
      cellGroupBox->setHidden(false);
   }
   if (fociFileFlag) {
      fociGroupBox->setHidden(false);
   }
   
   if (vtkModelFileFlag) {
      vtkModelGroupBox->setHidden(false);
   }
   else {
      vtkModelGroupBox->setHidden(true);
   }
   
   if (topologyFileFlag) {
      TopologyFile* tf = topologyFileComboBox->getSelectedTopologyFile();
      if (tf != NULL) {
         topologyTypeComboBox->setCurrentIndex(tf->getTopologyType());
      }
      topologyGroupBox->setHidden(false);
   }
   else {
      topologyGroupBox->setHidden(true);
   }
   
   if (volumeFileFlag) {
      saveDataEncodingTypeWidget->setHidden(true);
      //volumeFileHBox->setHidden(volumeMultiFileFlag);
      if (volumeFileFlag && (volumeFile != NULL)) {
         setSelection(FileUtilities::basename(volumeFile->getFileName()));
      }
      volumeGroupBox->setHidden(false);
      if (filterName == GuiDataFileDialog::volumeFileWuNilFilter) {
         commentWidget->setHidden(true);
         pubMedIDWidget->setHidden(true);
      }
   }
   else {
      volumeGroupBox->setHidden(true);
      saveDataEncodingTypeWidget->setHidden(false);
   }
   
   bordersGroupBox->setHidden(true);
   if (borderFileFlag || borderProjFileFlag) {
      bordersGroupBox->setHidden(false);
   }
   
   borderSurfaceWidget->setHidden(true);
   if (borderFileFlag) {
      borderSurfaceWidget->setHidden(false);
      
      const int num = borderFileComboBox->currentIndex();
      if ((num >= 0) && (num < static_cast<int>(borderFileSurfaceIndex.size()))) {
         const int surfaceIndex = borderFileSurfaceIndex[num];
         const BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(surfaceIndex);
         borderTypeComboBox->setSurfaceType(bms->getSurfaceType());
      }
   }
   
   if (coordFileFlag) {
      const int num = coordFileComboBox->currentIndex();
      if ((num >= 0) && (num < static_cast<int>(coordinateFiles.size()))) {
         BrainModelSurface* bms = brainSurfaces[num];
         if (bms != NULL) {
/*
            int newIndex = 0;
            switch(bms->getSurfaceType()) {
               case BrainModelSurface::SURFACE_TYPE_RAW:
                  newIndex = 0;
                  break;
               case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
                  newIndex = 1;
                  break;
               case BrainModelSurface::SURFACE_TYPE_INFLATED:
                  newIndex = 2;
                  break;
               case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
                  newIndex = 3;
                  break;
               case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
                  newIndex = 4;
                  break;
               case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
                  newIndex = 5;
                  break;
               case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
                  newIndex = 6;
                  break;
               case BrainModelSurface::SURFACE_TYPE_FLAT:
                  newIndex = 7;
                  break;
               case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
                  newIndex = 8;
                  break;
               case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
               case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
                  newIndex = 9;
                  break;
            }
*/
            structureComboBox->setStructure(bms->getStructure().getType());
            coordTypeComboBox->setSurfaceType(bms->getSurfaceType());
         }
      }
      
      coordFileGroupBox->setHidden(false);
      
      QString coordFrame;
      QString coordScale;
      QString coordOrient;
      if (af != NULL) {
         coordFrame  = af->getHeaderTag(AbstractFile::headerTagCoordFrameID);
         coordScale  = af->getHeaderTag(AbstractFile::headerTagScale);
         coordOrient = af->getHeaderTag(AbstractFile::headerTagOrientation);
      }
           
      bool unknownCoordFrameFlag = false;
      if (coordFrame == "ACPC") {
         coordFrameComboBox->setCurrentIndex(0);
      }
      else if (coordFrame == "AC") {
         coordFrameComboBox->setCurrentIndex(1);
      }
      else if (coordFrame == "CS") {
         coordFrameComboBox->setCurrentIndex(2);
      }
      else if (coordFrame == "CN") {
         coordFrameComboBox->setCurrentIndex(3);
      }
      else if (coordFrame == "COG") {
         coordFrameComboBox->setCurrentIndex(4);
      }
      else if (coordFrame == "NA") {
         coordFrameComboBox->setCurrentIndex(5);
      }
      else if (coordFrame == "SS") {
         coordFrameComboBox->setCurrentIndex(6);
      }
      else if (coordFrame == "TAL") {
         coordFrameComboBox->setCurrentIndex(7);
      }
      else if (coordFrame == "UNK") {
         coordFrameComboBox->setCurrentIndex(8);
         unknownCoordFrameFlag = true;
      }
      else {
         coordFrameComboBox->setCurrentIndex(8);
         unknownCoordFrameFlag = true;
      }
      
      //
      // This will check for stereotaxic spaces in coord frame id
      //
      if (unknownCoordFrameFlag) {
         for (int i = 0; i < coordFrameComboBox->count(); i++) {
            if (coordFrame == coordFrameComboBox->itemText(i)) {
               coordFrameComboBox->setCurrentIndex(i);
               break;
            }
         }
      }
      
      char xo = 'U';
      char yo = 'U';
      char zo = 'U';
      if (coordOrient.length() > 0) {
         xo = coordOrient[0].toLatin1();
         if (coordOrient.length() > 1) {
            yo = coordOrient[1].toLatin1();
            if (coordOrient.length() > 2) {
               zo = coordOrient[2].toLatin1();
            }
         }
      }
      
      switch(xo) {
         case 'L':
            xOrientationComboBox->setCurrentIndex(1);
            break;
         case 'R':
            xOrientationComboBox->setCurrentIndex(2);
            break;
         case 'A':
            xOrientationComboBox->setCurrentIndex(3);
            break;
         case 'P':
            xOrientationComboBox->setCurrentIndex(4);
            break;
         case 'I':
            xOrientationComboBox->setCurrentIndex(5);
            break;
         case 'S':
            xOrientationComboBox->setCurrentIndex(6);
            break;
         case 'U':
         default:
            xOrientationComboBox->setCurrentIndex(0);
            break;
      }

      switch(yo) {
         case 'L':
            yOrientationComboBox->setCurrentIndex(1);
            break;
         case 'R':
            yOrientationComboBox->setCurrentIndex(2);
            break;
         case 'A':
            yOrientationComboBox->setCurrentIndex(3);
            break;
         case 'P':
            yOrientationComboBox->setCurrentIndex(4);
            break;
         case 'I':
            yOrientationComboBox->setCurrentIndex(5);
            break;
         case 'S':
            yOrientationComboBox->setCurrentIndex(6);
            break;
         case 'U':
         default:
            yOrientationComboBox->setCurrentIndex(0);
            break;
      }
      switch(zo) {
         case 'L':
            zOrientationComboBox->setCurrentIndex(1);
            break;
         case 'R':
            zOrientationComboBox->setCurrentIndex(2);
            break;
         case 'A':
            zOrientationComboBox->setCurrentIndex(3);
            break;
         case 'P':
            zOrientationComboBox->setCurrentIndex(4);
            break;
         case 'I':
            zOrientationComboBox->setCurrentIndex(5);
            break;
         case 'S':
            zOrientationComboBox->setCurrentIndex(6);
            break;
         case 'U':
         default:
            zOrientationComboBox->setCurrentIndex(0);
            break;
      }
   }
   else {
      coordFileGroupBox->setHidden(true);
   }
}

/**
 * Called when a volume file type selection is made
 */
void
GuiSaveDataFileDialog::volumeTypeSelectedSlot()
{
   updateVolumeSavingItems();
   updateSaveParameters();
}

/**
 * update volume saving items.
 */
void 
GuiSaveDataFileDialog::updateVolumeSavingItems()
{
   const bool afni  = ((filterName == volumeMultiFileAfniFilter) ||
                       (filterName == volumeFileAfniFilter));
   const bool spm   = (filterName == volumeFileSpmMedxFilter);
   const bool nifti = ((filterName == volumeFileNiftiWriteFilter) ||
                       (filterName == volumeMultiFileNiftiFilter));
   const bool wustl = (filterName == volumeFileWuNilFilter);
      
   volumeVoxelDataTypeComboBox->clear();
   volumeVoxelDataTypeComboBoxValues.clear();
   
   volumeVoxelDataTypeComboBox->addItem("Choose Voxel Data Type");   
   volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_UNKNOWN);
   if (nifti) {
      volumeVoxelDataTypeComboBox->addItem("Byte - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_CHAR);
   }
   if (afni || spm || nifti) {
      volumeVoxelDataTypeComboBox->addItem("Byte - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_CHAR_UNSIGNED);
   }
   if (afni || spm || nifti) {
      volumeVoxelDataTypeComboBox->addItem("Short - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_SHORT);
   }
   if (nifti) {
      volumeVoxelDataTypeComboBox->addItem("Short - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_SHORT_UNSIGNED);
   }
   if (afni || spm || nifti) {
      volumeVoxelDataTypeComboBox->addItem("Int - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_INT);
   }
   if (nifti) {
      volumeVoxelDataTypeComboBox->addItem("Int - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_INT_UNSIGNED);
   }
   if (nifti) {
      volumeVoxelDataTypeComboBox->addItem("Long - Signed");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_LONG);
   }
   if (nifti) {
      volumeVoxelDataTypeComboBox->addItem("Long - Unsigned");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_LONG_UNSIGNED);
   }
   if (afni || spm || nifti || wustl) {
      volumeVoxelDataTypeComboBox->addItem("Float");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_FLOAT);
   }
   if (afni || spm || nifti) {
      volumeVoxelDataTypeComboBox->addItem("Double");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_DOUBLE);
   }
   if (afni) {
      volumeVoxelDataTypeComboBox->addItem("RGB - Voxel Interleaved");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED);
   }
   if (spm) {
      volumeVoxelDataTypeComboBox->addItem("RGB - Slice Interleaved");   
      volumeVoxelDataTypeComboBoxValues.push_back(VolumeFile::VOXEL_DATA_TYPE_RGB_SLICE_INTERLEAVED);
   }
}      

/**
 * called when a previous directory is selected.
 */
void 
GuiSaveDataFileDialog::slotPreviousDirectory(int item)
{
   if ((item >= 0) && (item < static_cast<int>(previousDirectories.size()))) {
      setDir(previousDirectories[item]);
      setFilter(selectedFilter());
   }
}      


/**********************************************************************
   if (filter == GuiDataFileDialog::areaColorFileFilter) {
   }
   else if (filter == GuiDataFileDialog::arealEstimationFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderRawFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderFiducialFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderInflatedFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderVeryInflatedFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderSphericalFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderEllipsoidalFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderCompressedFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderFlatFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderFlatLobarFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderHullFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderUnknownFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderGenericFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderColorFileFilter) {
   }
   else if (filter == GuiDataFileDialog::borderProjectionFileFilter) {
   }
   else if (filter == GuiDataFileDialog::cellFileFilter) {
   }
   else if (filter == GuiDataFileDialog::cellColorFileFilter) {
   }
   else if (filter == GuiDataFileDialog::cellProjectionFileFilter) {
   }
   else if (filter == GuiDataFileDialog::cocomacFileFilter) {
   }
   else if (filter == GuiDataFileDialog::contourFileFilter) {
   }
   else if (filter == GuiDataFileDialog::contourCellFileFilter) {
   }
   else if (filter == GuiDataFileDialog::contourCellColorFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateRawFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateFiducialFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateInflatedFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateVeryInflatedFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateSphericalFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateEllipsoidalFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateCompressedFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateFlatFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateHullFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateFlatLobarFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateUnknownFileFilter) {
   }
   else if (filter == GuiDataFileDialog::coordinateGenericFileFilter) {
   }
   else if (filter == GuiDataFileDialog::fociFileFilter) {
   }
   else if (filter == GuiDataFileDialog::fociColorFileFilter) {
   }
   else if (filter == GuiDataFileDialog::fociProjectionFileFilter) {
   }
   else if (filter == GuiDataFileDialog::latitudeLongitudeFileFilter) {
   }
   else if (filter == GuiDataFileDialog::metricFileFilter) {
   }
   else if (filter == GuiDataFileDialog::paintFileFilter) {
   }
   else if (filter == GuiDataFileDialog::paletteFileFilter) {
   }
   else if (filter == GuiDataFileDialog::paramsFileFilter) {
   }
   else if (filter == GuiDataFileDialog::probAtlasFileFilter) {
   }
   else if (filter == GuiDataFileDialog::rgbPaintFileFilter) {
   }
   else if (filter == GuiDataFileDialog::sectionFileFilter) {
   }
   else if (filter == GuiDataFileDialog::surfaceShapeFileFilter) {
   }
   else if (filter == GuiDataFileDialog::topographyFileFilter) {
   }
   else if (filter == GuiDataFileDialog::topologyClosedFileFilter) {
   }
   else if (filter == GuiDataFileDialog::topologyOpenFileFilter) {
   }
   else if (filter == GuiDataFileDialog::topologyCutFileFilter) {
   }
   else if (filter == GuiDataFileDialog::topologyCutLobarFileFilter) {
   }
   else if (filter == GuiDataFileDialog::topologyUnknownFileFilter) {
   }
   else if (filter == GuiDataFileDialog::topologyGenericFileFilter) {
   }
   else if (filter == GuiDataFileDialog::volumeCommTransformFileFilter) {
   }
   else if (filter == GuiDataFileDialog::volumeStructuralFileFilter) {
   }
   else {
      std::cerr << "PROGRAM ERROR: unhandled file type " << filterName << " at " << __LINE__
               << " in " << __FILE__ << std::endl;
   }
***********************************************************************/
