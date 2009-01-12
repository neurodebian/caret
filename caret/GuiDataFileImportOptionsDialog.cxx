
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

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>

#include "FileFilters.h"
#include "GuiDataFileImportOptionsDialog.h"
#include "GuiStructureComboBox.h"
#include "GuiSurfaceTypeComboBox.h"
#include "GuiTopologyTypeComboBox.h"
#include "GuiVolumeTypeComboBox.h"
#include "GuiVolumeVoxelDataTypeComboBox.h"

/**
 * constructor.
 */
GuiDataFileImportOptionsDialog::GuiDataFileImportOptionsDialog(QWidget* parent,
                                                               const QString& filterName)
   : WuQDialog(parent)
{
   QVBoxLayout* layout = new QVBoxLayout(this);
   
   if (filterName == FileFilters::getAnalyzeVolumeFileFilter()) {
      layout->addWidget(createVolumeOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getBrainVoyagerFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getByuSurfaceFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getFreeSurferAsciiSurfaceFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getFreeSurferBinarySurfaceFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getMdPlotFileFilter()) {
      layout->addWidget(createContourOptionsWidget(filterName));   
   }
#ifdef HAVE_MINC
   else if (filterName == FileFilters::getMincVolumeFileFilter()) {
      layout->addWidget(createVolumeOptionsWidget(filterName));   
   }
#endif // HAVE_MINC
   else if (filterName == FileFilters::getMniObjSurfaceFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getNeurolucidaFileFilter()) {
      layout->addWidget(createContourOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getRawVolumeFileFilter()) {
      layout->addWidget(createVolumeOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getStlSurfaceFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getSumaRgbFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getVtkSurfaceFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getVtkXmlSurfaceFileFilter()) {
      layout->addWidget(createSurfaceOptionsWidget(filterName));   
   }
   else if (filterName == FileFilters::getVtkVolumeFileFilter()) {
      layout->addWidget(createVolumeOptionsWidget(filterName));   
   }
   else {
      std::cout << "PROGRAM ERROR: invalid filter ("
                << filterName.toAscii().constData()
                << ") passed to GuiDataFileImportOptionsDialog()."
                << std::endl;
   }
   
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok 
                                                      | QDialogButtonBox::Cancel);
   layout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(accepted()),
                    this, SLOT(accept()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(reject()));
}

/**
 * destructor.
 */
GuiDataFileImportOptionsDialog::~GuiDataFileImportOptionsDialog()
{
}

/**
 * create the contours options widget.
 */
QWidget* 
GuiDataFileImportOptionsDialog::createContourOptionsWidget(const QString& filterName)
{
   QString contourLabelName("Import Contours");
   QString contourCellLabelName("Import Contour Cells");
   if (filterName == FileFilters::getMdPlotFileFilter()) {
      contourLabelName = "Import MD Plot Lines as Contours";
      contourCellLabelName = "Import MD Plot Points as Contour Cells";
   }
   if (filterName == FileFilters::getNeurolucidaFileFilter()) {
      contourLabelName = "Import Contours";
      contourCellLabelName = "Import Markers as Contour Cells";
   }
   
   //
   // Contour selections
   //
   importContoursCheckBox = new QCheckBox(contourLabelName);
   importContoursCheckBox->setChecked(true);
   importContourCellsCheckBox = new QCheckBox(contourCellLabelName);
   importContourCellsCheckBox->setChecked(true);
   
   //
   // Layout items
   //
   QWidget* contourOptionsWidget = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(contourOptionsWidget);
   layout->addWidget(importContoursCheckBox);
   layout->addWidget(importContourCellsCheckBox);
   
   return contourOptionsWidget;
}

/**
 * get the contour options.
 */
void 
GuiDataFileImportOptionsDialog::getContourOptions(bool& importContours,
                                                  bool& importCells) const
{
   importContours = importContoursCheckBox->isChecked();
   importCells    = importContourCellsCheckBox->isChecked();
}
                             
/**
 * create the surface options widget.
 */
QWidget*
GuiDataFileImportOptionsDialog::createSurfaceOptionsWidget(const QString& filterName)
{
   //
   // Structure selection
   //
   QLabel* structureLabel = new QLabel("Structure ");
   structureComboBox = new GuiStructureComboBox;
   QWidget* structureWidget = new QWidget;
   QHBoxLayout* structureLayout = new QHBoxLayout(structureWidget);
   structureLayout->addWidget(structureLabel);
   structureLayout->addWidget(structureComboBox);
   structureLayout->addStretch();
   
   //
   // Import type selections
   //
   importTopologyCheckBox = new QCheckBox("Import Topology");
   importTopologyCheckBox->setChecked(true);
   topologyTypeComboBox = new GuiTopologyTypeComboBox;
   QWidget* topologyTypeWidget = new QWidget;
   QHBoxLayout* topologyTypeLayout = new QHBoxLayout(topologyTypeWidget);
   topologyTypeLayout->addWidget(importTopologyCheckBox);
   topologyTypeLayout->addWidget(topologyTypeComboBox);
   topologyTypeLayout->addStretch();
   
   importCoordinatesCheckBox = new QCheckBox("Import Coordinates");
   importCoordinatesCheckBox->setChecked(true);
   surfaceTypeComboBox = new GuiSurfaceTypeComboBox;
   QWidget* surfaceTypeWidget = new QWidget;
   QHBoxLayout* surfaceTypeLayout = new QHBoxLayout(surfaceTypeWidget);
   surfaceTypeLayout->addWidget(importCoordinatesCheckBox);
   surfaceTypeLayout->addWidget(surfaceTypeComboBox);
   surfaceTypeLayout->addStretch();
   
   importPaintAreaColorsCheckBox = new QCheckBox("Import Colors as Paint and Area Colors");
   importPaintAreaColorsCheckBox->setChecked(true);
   
   importRgbColorsCheckBox = new QCheckBox("Import Colors as RGB Paint");
   importRgbColorsCheckBox->setChecked(true);

   QWidget* surfaceOptionsWidget = new QWidget;   
   QVBoxLayout* surfaceGroupLayout = new QVBoxLayout(surfaceOptionsWidget);
   surfaceGroupLayout->addWidget(structureWidget);
   surfaceGroupLayout->addWidget(topologyTypeWidget);
   surfaceGroupLayout->addWidget(surfaceTypeWidget);
   surfaceGroupLayout->addWidget(importPaintAreaColorsCheckBox);
   surfaceGroupLayout->addWidget(importRgbColorsCheckBox);
   
   bool showStructure = false;
   bool showTopologyType  = false;
   bool showSurfaceType = false;
   bool showPaint = false;
   bool showRgb = false;
   
   if (filterName == FileFilters::getBrainVoyagerFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
      showPaint = true;
   }
   else if (filterName == FileFilters::getByuSurfaceFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
   }
   else if (filterName == FileFilters::getFreeSurferAsciiSurfaceFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
   }
   else if (filterName == FileFilters::getFreeSurferBinarySurfaceFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
   }
   else if (filterName == FileFilters::getMniObjSurfaceFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
      showRgb = true;
   }
   else if (filterName == FileFilters::getStlSurfaceFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
   }
   else if (filterName == FileFilters::getSumaRgbFileFilter()) {
      showRgb = true;
   }
   else if (filterName == FileFilters::getVtkSurfaceFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
      showRgb = true;
   }
   else if (filterName == FileFilters::getVtkXmlSurfaceFileFilter()) {
      showStructure = true;
      showTopologyType  = true;
      showSurfaceType = true;
      showRgb = true;
   }
   
   structureWidget->setVisible(showStructure);
   topologyTypeWidget->setVisible(showTopologyType);
   surfaceTypeWidget->setVisible(showSurfaceType);
   importPaintAreaColorsCheckBox->setVisible(showPaint);
   importRgbColorsCheckBox->setVisible(showRgb);
   
   return surfaceOptionsWidget;
}

/**
 * get the surface options.
 */
void 
GuiDataFileImportOptionsDialog::getSurfaceOptions(Structure::STRUCTURE_TYPE& structureType,
                                                  bool& importTopology,
                                                  TopologyFile::TOPOLOGY_TYPES& topologyType,
                                                  bool& importCoordinates,
                                                  BrainModelSurface::SURFACE_TYPES& coordinateType,
                                                  bool& importColorsAsPaint,
                                                  bool& importColorsAsRgbPaint)
{
   structureType = structureComboBox->getSelectedStructure();
   importTopology = importTopologyCheckBox->isChecked();
   topologyType  = topologyTypeComboBox->getTopologyType();
   importCoordinates = importCoordinatesCheckBox->isChecked();
   coordinateType   = surfaceTypeComboBox->getSurfaceType();
   importColorsAsPaint = importPaintAreaColorsCheckBox->isChecked();
   importColorsAsRgbPaint = importRgbColorsCheckBox->isChecked();
}

/**
 * create the volume options widget.
 */
QWidget* 
GuiDataFileImportOptionsDialog::createVolumeOptionsWidget(const QString& filterName)
{
   //
   // Volume selections
   //
   QLabel* volumeTypeLabel = new QLabel("Volume Type");
   volumeTypeComboBox = new GuiVolumeTypeComboBox;
   QHBoxLayout* volumeTypeLayout = new QHBoxLayout;
   volumeTypeLayout->addWidget(volumeTypeLabel);
   volumeTypeLayout->addWidget(volumeTypeComboBox);
   volumeTypeLayout->addStretch();
   
   //
   // Raw volume dimensions
   //
   const int rawDimSize = 100;
   QLabel* volumeDimensionsLabel = new QLabel("Dimensions ");
   volumeDimXSpinBox = new QSpinBox;
   volumeDimXSpinBox->setMinimum(0);
   volumeDimXSpinBox->setMaximum(100000);
   volumeDimXSpinBox->setSingleStep(1);
   volumeDimXSpinBox->setFixedWidth(rawDimSize);
   volumeDimYSpinBox = new QSpinBox;
   volumeDimYSpinBox->setMinimum(0);
   volumeDimYSpinBox->setMaximum(100000);
   volumeDimYSpinBox->setSingleStep(1);
   volumeDimYSpinBox->setFixedWidth(rawDimSize);
   volumeDimZSpinBox = new QSpinBox;
   volumeDimZSpinBox->setFixedWidth(rawDimSize);
   volumeDimZSpinBox->setMinimum(0);
   volumeDimZSpinBox->setMaximum(100000);
   volumeDimZSpinBox->setSingleStep(1);
   QWidget* volumeDimensionsWidget = new QWidget;
   QHBoxLayout* volumeDimensionsLayout = new QHBoxLayout(volumeDimensionsWidget);
   volumeDimensionsLayout->setSpacing(5);
   volumeDimensionsLayout->addWidget(volumeDimensionsLabel);
   volumeDimensionsLayout->addWidget(volumeDimXSpinBox);
   volumeDimensionsLayout->addWidget(volumeDimYSpinBox);
   volumeDimensionsLayout->addWidget(volumeDimZSpinBox);
   volumeDimensionsLayout->addStretch();
   
   //
   // Raw volume data type
   //
   QLabel* volumeVoxelDataTypeLabel = new QLabel("Voxel Data Type");
   volumeVoxelDataTypeComboBox = new GuiVolumeVoxelDataTypeComboBox;
   QWidget* volumeVoxelDataTypeWidget = new QWidget;
   QHBoxLayout* volumeVoxelDataTypeLayout = new QHBoxLayout(volumeVoxelDataTypeWidget);
   volumeVoxelDataTypeLayout->addWidget(volumeVoxelDataTypeLabel);
   volumeVoxelDataTypeLayout->addWidget(volumeVoxelDataTypeComboBox);
   volumeVoxelDataTypeLayout->addStretch();
   
   //
   // Raw volume byte swap
   //
   QLabel* volumeByteSwapLabel = new QLabel("Byte Swap ");
   volumeByteSwapComboBox = new QComboBox;
   volumeByteSwapComboBox->addItem("No");
   volumeByteSwapComboBox->addItem("Yes");
   QWidget* volumeByteSwapWidget = new QWidget;
   QHBoxLayout* volumeByteSwapLayout = new QHBoxLayout(volumeByteSwapWidget);
   volumeByteSwapLayout->addWidget(volumeByteSwapLabel);
   volumeByteSwapLayout->addWidget(volumeByteSwapComboBox);
   volumeByteSwapLayout->addStretch();
   
   //
   // Widget and layout for raw volume options
   //
   QWidget* volumeOptionsWidget = new QWidget;
   QVBoxLayout* volumeLayout = new QVBoxLayout(volumeOptionsWidget);
   volumeLayout->addLayout(volumeTypeLayout);
   volumeLayout->addWidget(volumeDimensionsWidget);
   volumeLayout->addWidget(volumeVoxelDataTypeWidget);
   volumeLayout->addWidget(volumeByteSwapWidget);
   
   const bool showDimensionsDataTypeByteSwap = 
      (filterName == FileFilters::getRawVolumeFileFilter());
   volumeDimensionsWidget->setVisible(showDimensionsDataTypeByteSwap);
   volumeVoxelDataTypeWidget->setVisible(showDimensionsDataTypeByteSwap);
   volumeByteSwapWidget->setVisible(showDimensionsDataTypeByteSwap);
   
   return volumeOptionsWidget;
}

/**
 * get the volume options.
 */
void 
GuiDataFileImportOptionsDialog::getVolumeOptions(VolumeFile::VOLUME_TYPE& volumeType,
                                                 int dimensions[3],
                                                 VolumeFile::VOXEL_DATA_TYPE& voxelDataType,
                                                 bool& byteSwap)
{
   volumeType = volumeTypeComboBox->getVolumeType();
   dimensions[0] = volumeDimXSpinBox->value();
   dimensions[1] = volumeDimYSpinBox->value();
   dimensions[2] = volumeDimZSpinBox->value();
   voxelDataType = volumeVoxelDataTypeComboBox->getVolumeVoxelDataType();
   byteSwap = (volumeByteSwapComboBox->currentIndex() != 0);
}
                            
