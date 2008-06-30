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

#include <limits>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "WuQFileDialog.h">
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QToolTip>

#define __GUI_SURFACE_TO_VOLUME_DIALOG_MAIN__
#include "GuiSurfaceToVolumeDialog.h"
#undef __GUI_SURFACE_TO_VOLUME_DIALOG_MAIN__

#include "BrainSet.h"
#include "DisplaySettingsMetric.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsSurfaceShape.h"
#include "FileFilters.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiMainWindow.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ParamsFile.h"
#include "QtMultipleInputDialog.h"
#include "QtUtilities.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiSurfaceToVolumeDialog::GuiSurfaceToVolumeDialog(QWidget* parent,
                                                   const DIALOG_MODE modeIn,
                                                   const QString& dialogTitle,
                                    const bool showSurfaceSelectionOptionsFlag)
   : WuQDialog(parent)
{
   mode = modeIn;
   
   static bool firstTime = true;
   
   setWindowTitle(dialogTitle);
   
   //
   // Find surface and volume from last time
   //
   int surfaceModelIndex = -1;
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      if (savedSurface == theMainWindow->getBrainSet()->getBrainModel(i)) {
         surfaceModelIndex = i;
      }
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // vertical box for surface selections
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surface Selection", this);
   dialogLayout->addWidget(surfaceGroupBox);
   QVBoxLayout* surfaceGroupLayout = new QVBoxLayout(surfaceGroupBox);
   //
   // Surface File
   //
   surfaceSelectionBox = new GuiBrainModelSelectionComboBox(false, true, false,
                                                      "", 0);
   surfaceGroupLayout->addWidget(surfaceSelectionBox);
   if (surfaceModelIndex >= 0) {
      surfaceSelectionBox->setSelectedBrainModelIndex(surfaceModelIndex);
   }

   if (showSurfaceSelectionOptionsFlag == false) {
      surfaceSelectionBox->setHidden(true);
   }
   
   const int spinWidth = 120;
   const double floatMin = -std::numeric_limits<float>::max();
   const double floatMax =  std::numeric_limits<float>::max();
   const int intMax = std::numeric_limits<int>::max();
   
   //
   // Surface thickness and step grid
   //
   QGridLayout* surfaceGridLayout = new QGridLayout;
   surfaceGridLayout->setSpacing(3);
   surfaceGroupLayout->addLayout(surfaceGridLayout);
   int rowNum = 0;
   
   surfaceInnerBoundaryDoubleSpinBox = NULL;
   surfaceOuterBoundaryDoubleSpinBox = NULL;
   surfaceThicknessStepDoubleSpinBox = NULL;
   if (mode != DIALOG_MODE_SEGMENT_VOLUME) {
      //
      // Inner boundary label and text box
      //
      QLabel* innerLabel = new QLabel("Inner Boundary (mm)");
      surfaceInnerBoundaryDoubleSpinBox = new QDoubleSpinBox;
      surfaceInnerBoundaryDoubleSpinBox->setFixedWidth(spinWidth);
      surfaceInnerBoundaryDoubleSpinBox->setSingleStep(0.5);
      surfaceInnerBoundaryDoubleSpinBox->setDecimals(3);
      surfaceInnerBoundaryDoubleSpinBox->setMinimum(floatMin);
      surfaceInnerBoundaryDoubleSpinBox->setMaximum(floatMax);
      surfaceInnerBoundaryDoubleSpinBox->setValue(savedInnerBoundary);
      surfaceGridLayout->addWidget(innerLabel, rowNum, 0);
      surfaceGridLayout->addWidget(surfaceInnerBoundaryDoubleSpinBox, rowNum, 1);
      rowNum++;
      
      //
      // Outer boundary label and text box
      //
      QLabel* outerLabel = new QLabel("Outer Boundary (mm)");
      surfaceOuterBoundaryDoubleSpinBox = new QDoubleSpinBox;
      surfaceOuterBoundaryDoubleSpinBox->setFixedWidth(spinWidth);
      surfaceOuterBoundaryDoubleSpinBox->setSingleStep(0.5);
      surfaceOuterBoundaryDoubleSpinBox->setDecimals(3);
      surfaceOuterBoundaryDoubleSpinBox->setMinimum(floatMin);
      surfaceOuterBoundaryDoubleSpinBox->setMaximum(floatMax);
      surfaceOuterBoundaryDoubleSpinBox->setValue(savedOuterBoundary);
      surfaceGridLayout->addWidget(outerLabel, rowNum, 0);
      surfaceGridLayout->addWidget(surfaceOuterBoundaryDoubleSpinBox, rowNum, 1);
      rowNum++;
      
      //
      // Thickness step
      //
      QLabel* thickLabel = new QLabel("Intersection Step (mm)");
      surfaceThicknessStepDoubleSpinBox = new QDoubleSpinBox;
      surfaceThicknessStepDoubleSpinBox->setFixedWidth(spinWidth);
      surfaceThicknessStepDoubleSpinBox->setSingleStep(0.5);
      surfaceThicknessStepDoubleSpinBox->setDecimals(3);
      surfaceThicknessStepDoubleSpinBox->setMinimum(0.0);
      surfaceThicknessStepDoubleSpinBox->setMinimum(floatMin);
      surfaceThicknessStepDoubleSpinBox->setMaximum(floatMax);
      surfaceThicknessStepDoubleSpinBox->setValue(savedThicknessStep);
      surfaceGridLayout->addWidget(thickLabel, rowNum, 0);
      surfaceGridLayout->addWidget(surfaceThicknessStepDoubleSpinBox, rowNum, 1);
      rowNum++;
   }
   
   //
   // Offset 
   //   
   QLabel* transLabel = new QLabel("Translation (mm)");
   surfaceOffsetXDoubleSpinBox = new QDoubleSpinBox;
   surfaceOffsetXDoubleSpinBox->setFixedWidth(spinWidth);
   surfaceOffsetXDoubleSpinBox->setSingleStep(1.0);
   surfaceOffsetXDoubleSpinBox->setDecimals(3);
   surfaceOffsetXDoubleSpinBox->setMinimum(floatMin);
   surfaceOffsetXDoubleSpinBox->setMaximum(floatMax);
   surfaceOffsetYDoubleSpinBox = new QDoubleSpinBox;
   surfaceOffsetYDoubleSpinBox->setFixedWidth(spinWidth);
   surfaceOffsetYDoubleSpinBox->setSingleStep(1.0);
   surfaceOffsetYDoubleSpinBox->setDecimals(3);
   surfaceOffsetYDoubleSpinBox->setMinimum(floatMin);
   surfaceOffsetYDoubleSpinBox->setMaximum(floatMax);
   surfaceOffsetZDoubleSpinBox = new QDoubleSpinBox;
   surfaceOffsetZDoubleSpinBox->setFixedWidth(spinWidth);
   surfaceOffsetZDoubleSpinBox->setSingleStep(1.0);
   surfaceOffsetZDoubleSpinBox->setDecimals(3);
   surfaceOffsetZDoubleSpinBox->setMinimum(floatMin);
   surfaceOffsetZDoubleSpinBox->setMaximum(floatMax);
   surfaceGridLayout->addWidget(transLabel, rowNum, 0);
   surfaceGridLayout->addWidget(surfaceOffsetXDoubleSpinBox, rowNum, 1);
   surfaceGridLayout->addWidget(surfaceOffsetYDoubleSpinBox, rowNum, 2);
   surfaceGridLayout->addWidget(surfaceOffsetZDoubleSpinBox, rowNum, 3);
   rowNum++;
   
   if (showSurfaceSelectionOptionsFlag == false) {
       transLabel->setHidden(true);
       surfaceOffsetXDoubleSpinBox->setHidden(true);
       surfaceOffsetYDoubleSpinBox->setHidden(true);
       surfaceOffsetZDoubleSpinBox->setHidden(true);
   }
   
   const ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   if (showSurfaceSelectionOptionsFlag) {
      //
      // Parameters file contains information for offset
      // xmin, ymin, zmin
      //
      QString xmin, ymin, zmin;
      if (pf->getParameter(ParamsFile::keyXmin, xmin) &&
          pf->getParameter(ParamsFile::keyYmin, ymin) &&
          pf->getParameter(ParamsFile::keyZmin, zmin)) {
         QLabel* xyzLabel = new QLabel("XYZ min");
         QLabel* xLabel = new QLabel(xmin);
         QLabel* yLabel = new QLabel(ymin);
         QLabel* zLabel = new QLabel(zmin);
         surfaceGridLayout->addWidget(xyzLabel, rowNum, 0);
         surfaceGridLayout->addWidget(xLabel, rowNum, 1);
         surfaceGridLayout->addWidget(yLabel, rowNum, 2);
         surfaceGridLayout->addWidget(zLabel, rowNum, 3);
         rowNum++;
      }
      
      //
      // AC position
      //
      QString acx, acy, acz;
      if (pf->getParameter(ParamsFile::keyACx, acx) &&
          pf->getParameter(ParamsFile::keyACy, acy) &&
          pf->getParameter(ParamsFile::keyACz, acz)) {
         QLabel* xyzLabel = new QLabel("AC xyz");
         QLabel* xLabel = new QLabel(acx);
         QLabel* yLabel = new QLabel(acy);
         QLabel* zLabel = new QLabel(acz);
         surfaceGridLayout->addWidget(xyzLabel, rowNum, 0);
         surfaceGridLayout->addWidget(xLabel, rowNum, 1);
         surfaceGridLayout->addWidget(yLabel, rowNum, 2);
         surfaceGridLayout->addWidget(zLabel, rowNum, 3);
         rowNum++;
      }
      
      //
      // Whole volume ac
      //
      QString wholeAcx, wholeAcy, wholeAcz;
      if (pf->getParameter(ParamsFile::keyWholeVolumeACx, wholeAcx) &&
          pf->getParameter(ParamsFile::keyWholeVolumeACy, wholeAcy) &&
          pf->getParameter(ParamsFile::keyWholeVolumeACz, wholeAcz)) {
         QLabel* xyzLabel = new QLabel("Whole Vol AC xyz");
         QLabel* xLabel = new QLabel(wholeAcx);
         QLabel* yLabel = new QLabel(wholeAcy);
         QLabel* zLabel = new QLabel(wholeAcz);
         surfaceGridLayout->addWidget(xyzLabel, rowNum, 0);
         surfaceGridLayout->addWidget(xLabel, rowNum, 1);
         surfaceGridLayout->addWidget(yLabel, rowNum, 2);
         surfaceGridLayout->addWidget(zLabel, rowNum, 3);
         rowNum++;
      }
   }
   
   //
   // Shrink the grid
   //
   //surfaceGrid->setFixedSize(surfaceGrid->sizeHint());
   
   //
   // vertical box for volume selections
   //
   QGroupBox* volumeGroupBox = new QGroupBox("Volume Selection");
   dialogLayout->addWidget(volumeGroupBox);
   QVBoxLayout* volumeGroupLayout = new QVBoxLayout(volumeGroupBox);
   
   //
   // Grid for volume parameters
   //
   QGridLayout* volumeGridLayout = new QGridLayout;
   volumeGridLayout->setSpacing(3);
   volumeGroupLayout->addLayout(volumeGridLayout);
   int rowNumber = 0;
   
   //
   // Volume Space
   //
   int spaceIndex = -1;
   volumeGridLayout->addWidget(new QLabel("Space"), rowNumber, 0, Qt::AlignLeft);
   volumeSpaceComboBox = new QComboBox;
   volumeGridLayout->addWidget(volumeSpaceComboBox, rowNumber, 1, 1, 3, Qt::AlignLeft);
   StereotaxicSpace::getAllStereotaxicSpaces(volumeSpaceComboBoxSpaces);
   for (unsigned int i = 0; i < volumeSpaceComboBoxSpaces.size(); i++) {
      volumeSpaceComboBox->addItem(volumeSpaceComboBoxSpaces[i].getName());
      if (savedVolumeSpace == volumeSpaceComboBoxSpaces[i]) {
         spaceIndex = i;
      }
   }
   QObject::connect(volumeSpaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotVolumeSpaceSelection(int)));
   rowNumber++;
   
   //
   // Volume Dimensions
   //
   volumeGridLayout->addWidget(new QLabel("Dimension"), rowNumber, 0, Qt::AlignLeft);
   volumeDimensionXSpinBox = new QSpinBox;
   volumeDimensionXSpinBox->setMinimum(1);
   volumeDimensionXSpinBox->setMaximum(intMax);
   volumeDimensionXSpinBox->setSingleStep(1);
   volumeDimensionXSpinBox->setFixedWidth(spinWidth);
   volumeGridLayout->addWidget(volumeDimensionXSpinBox, rowNumber, 1, Qt::AlignLeft);
   volumeDimensionYSpinBox = new QSpinBox;
   volumeDimensionYSpinBox->setMinimum(1);
   volumeDimensionYSpinBox->setMaximum(intMax);
   volumeDimensionYSpinBox->setSingleStep(1);
   volumeDimensionYSpinBox->setFixedWidth(spinWidth);
   volumeGridLayout->addWidget(volumeDimensionYSpinBox, rowNumber, 2, Qt::AlignLeft);
   volumeDimensionZSpinBox = new QSpinBox;
   volumeDimensionZSpinBox->setMinimum(1);
   volumeDimensionZSpinBox->setMaximum(intMax);
   volumeDimensionZSpinBox->setSingleStep(1);
   volumeDimensionZSpinBox->setFixedWidth(spinWidth);
   volumeGridLayout->addWidget(volumeDimensionZSpinBox, rowNumber, 3, Qt::AlignLeft);
   rowNumber++;
   
   //
   // Volume voxel sizes
   //
   volumeGridLayout->addWidget(new QLabel("Voxel Size"), rowNumber, 0, Qt::AlignLeft);
   volumeVoxelSizeXDoubleSpinBox = new QDoubleSpinBox;
   volumeVoxelSizeXDoubleSpinBox->setFixedWidth(spinWidth);
   volumeVoxelSizeXDoubleSpinBox->setSingleStep(0.5);
   volumeVoxelSizeXDoubleSpinBox->setDecimals(3);
   volumeVoxelSizeXDoubleSpinBox->setMinimum(0.0);
   volumeVoxelSizeXDoubleSpinBox->setMaximum(floatMax);
   volumeGridLayout->addWidget(volumeVoxelSizeXDoubleSpinBox, rowNumber, 1, Qt::AlignLeft);
   volumeVoxelSizeYDoubleSpinBox = new QDoubleSpinBox;
   volumeVoxelSizeYDoubleSpinBox->setFixedWidth(spinWidth);
   volumeVoxelSizeYDoubleSpinBox->setSingleStep(0.5);
   volumeVoxelSizeYDoubleSpinBox->setDecimals(3);
   volumeVoxelSizeYDoubleSpinBox->setMinimum(0.0);
   volumeVoxelSizeYDoubleSpinBox->setMaximum(floatMax);
   volumeGridLayout->addWidget(volumeVoxelSizeYDoubleSpinBox, rowNumber, 2, Qt::AlignLeft);
   volumeVoxelSizeZDoubleSpinBox = new QDoubleSpinBox;
   volumeVoxelSizeZDoubleSpinBox->setFixedWidth(spinWidth);
   volumeVoxelSizeZDoubleSpinBox->setSingleStep(0.5);
   volumeVoxelSizeZDoubleSpinBox->setDecimals(3);
   volumeVoxelSizeZDoubleSpinBox->setMinimum(0.0);
   volumeVoxelSizeZDoubleSpinBox->setMaximum(floatMax);
   volumeGridLayout->addWidget(volumeVoxelSizeZDoubleSpinBox, rowNumber, 3, Qt::AlignLeft);
   rowNumber++;
   
   //
   // Volume origin
   //
   volumeGridLayout->addWidget(new QLabel("Volume Origin"), rowNumber, 0, Qt::AlignLeft);
   volumeOriginXDoubleSpinBox = new QDoubleSpinBox;
   volumeOriginXDoubleSpinBox->setFixedWidth(spinWidth);
   volumeOriginXDoubleSpinBox->setSingleStep(0.5);
   volumeOriginXDoubleSpinBox->setDecimals(3);
   volumeOriginXDoubleSpinBox->setMinimum(floatMin);
   volumeOriginXDoubleSpinBox->setMaximum(floatMax);
   volumeGridLayout->addWidget(volumeOriginXDoubleSpinBox, rowNumber, 1, Qt::AlignLeft);
   volumeOriginYDoubleSpinBox = new QDoubleSpinBox;
   volumeOriginYDoubleSpinBox->setFixedWidth(spinWidth);
   volumeOriginYDoubleSpinBox->setSingleStep(0.5);
   volumeOriginYDoubleSpinBox->setDecimals(3);
   volumeOriginYDoubleSpinBox->setMinimum(floatMin);
   volumeOriginYDoubleSpinBox->setMaximum(floatMax);
   volumeGridLayout->addWidget(volumeOriginYDoubleSpinBox, rowNumber, 2, Qt::AlignLeft);
   volumeOriginZDoubleSpinBox = new QDoubleSpinBox;
   volumeOriginZDoubleSpinBox->setFixedWidth(spinWidth);
   volumeOriginZDoubleSpinBox->setSingleStep(0.5);
   volumeOriginZDoubleSpinBox->setDecimals(3);
   volumeOriginZDoubleSpinBox->setMinimum(floatMin);
   volumeOriginZDoubleSpinBox->setMaximum(floatMax);
   volumeGridLayout->addWidget(volumeOriginZDoubleSpinBox, rowNumber, 3, Qt::AlignLeft);
   rowNumber++;

   //
   // Volume dimensions
   //
   QString xdim, ydim, zdim;
   if (pf->getParameter(ParamsFile::keyXdim, xdim) &&
       pf->getParameter(ParamsFile::keyYdim, ydim) &&
       pf->getParameter(ParamsFile::keyZdim, zdim)) {
      volumeGridLayout->addWidget(new QLabel("XYZ dim"), rowNumber, 0, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(xdim), 
                             rowNumber, 1, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(ydim), 
                             rowNumber, 2, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(zdim), 
                             rowNumber, 3, Qt::AlignLeft);
      rowNumber++;
      
      if (firstTime) {
         savedDimensions[0] = xdim.toInt();
         savedDimensions[1] = ydim.toInt();
         savedDimensions[2] = zdim.toInt();
      }
   }

   //
   // Whole volume dimensions
   // 
   QString wholeXdim, wholeYdim, wholeZdim;
   if (pf->getParameter(ParamsFile::keyXdim, wholeXdim) &&
       pf->getParameter(ParamsFile::keyYdim, wholeYdim) &&
       pf->getParameter(ParamsFile::keyZdim, wholeZdim)) {
      volumeGridLayout->addWidget(new QLabel("Whole XYZ dim"), rowNumber, 0, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(wholeXdim), 
                             rowNumber, 1, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(wholeYdim), 
                             rowNumber, 2, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(wholeZdim), 
                             rowNumber, 3, Qt::AlignLeft);
      rowNumber++;
      
      if (firstTime) {
         savedDimensions[0] = wholeXdim.toInt();
         savedDimensions[1] = wholeYdim.toInt();
         savedDimensions[2] = wholeZdim.toInt();
      }
   }

   QString resolution;
   if (pf->getParameter(ParamsFile::keyResolution, resolution)) {
      volumeGridLayout->addWidget(new QLabel("Resolution"), rowNumber, 0, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(resolution),
                            rowNumber, 1, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel("  "),
                            rowNumber, 2, Qt::AlignLeft);
      volumeGridLayout->addWidget(new QLabel(" "),
                            rowNumber, 3, Qt::AlignLeft);
      rowNumber++;
   }
   
   //
   // Enter AC button
   //
   QPushButton* acPushButton = new QPushButton("Enter AC to Determine Origin...");
   acPushButton->setAutoDefault(false);
   acPushButton->setFixedSize(acPushButton->sizeHint());
   QObject::connect(acPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAcPushButton()));
   acPushButton->setToolTip("This button displays a dialog for entry of the \n"
                               "Anterior Commissure voxel indices.  These voxel\n"
                               "indices will be used with the voxel size in the\n"
                               "dialog to determine the volume origin.");
   volumeGroupLayout->addWidget(acPushButton);
                               
   //
   // Get parameters from a volume file
   //
   QPushButton* paramtersFromVolumePushButton = 
            new QPushButton("Get Parameters Using a Volume File...");
   paramtersFromVolumePushButton->setAutoDefault(false);
   paramtersFromVolumePushButton->setFixedSize(paramtersFromVolumePushButton->sizeHint());
   QObject::connect(paramtersFromVolumePushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamtersFromVolumePushButton()));
   paramtersFromVolumePushButton->setToolTip(
                 "This button displays a dialog for selecting a volume\n"
                 "file.  The volume's dimensions, voxel size, and origin\n"
                 "will be placed into the dialog.");
   volumeGroupLayout->addWidget(paramtersFromVolumePushButton);
                 
   //
   // Initialize volume dimensions
   //
   volumeDimensionXSpinBox->setValue(savedDimensions[0]);
   volumeDimensionYSpinBox->setValue(savedDimensions[1]);
   volumeDimensionZSpinBox->setValue(savedDimensions[2]);
   
   //
   // Initialize volume voxel dimensions
   //
   volumeVoxelSizeXDoubleSpinBox->setValue(savedVoxelSize[0]);
   volumeVoxelSizeYDoubleSpinBox->setValue(savedVoxelSize[1]);
   volumeVoxelSizeZDoubleSpinBox->setValue(savedVoxelSize[2]);
   
   //
   // Initialize volume origin
   //
   volumeOriginXDoubleSpinBox->setValue(savedOrigin[0]);
   volumeOriginYDoubleSpinBox->setValue(savedOrigin[1]);
   volumeOriginZDoubleSpinBox->setValue(savedOrigin[2]);
   
   //
   // Initialize surface offset
   //
   surfaceOffsetXDoubleSpinBox->setValue(savedOffset[0]);
   surfaceOffsetYDoubleSpinBox->setValue(savedOffset[1]);
   surfaceOffsetZDoubleSpinBox->setValue(savedOffset[2]);
   
   //
   // Initialize space
   //
   if (spaceIndex >= 0) {
      slotVolumeSpaceSelection(spaceIndex);
      volumeSpaceComboBox->setCurrentIndex(spaceIndex);
   }
   
   int nodeAttributeColumn = 0;
   QString nodeAttributeLabel;
   
   GUI_NODE_FILE_TYPE nodeFileType = GUI_NODE_FILE_TYPE_NONE;
   switch(mode) {
      case DIALOG_MODE_NORMAL:
         break;
      case DIALOG_MODE_PAINT:
         {
            //
            // set up for paint
            //
            DisplaySettingsPaint* dsp = theMainWindow->getBrainSet()->getDisplaySettingsPaint();
            PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
            nodeAttributeLabel = "Paint";
            if (pf->getNumberOfColumns() > 0) {
               nodeAttributeColumn = dsp->getFirstSelectedColumnForBrainModel(surfaceModelIndex);
            }
            nodeFileType = GUI_NODE_FILE_TYPE_PAINT;
         }
         break;
      case DIALOG_MODE_METRIC:
         {
            //
            // set up for metric
            //
            MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
            nodeAttributeLabel = "Metric";
            if (mf->getNumberOfColumns() > 0) {
               DisplaySettingsMetric* dsm = theMainWindow->getBrainSet()->getDisplaySettingsMetric();
               nodeAttributeColumn = dsm->getFirstSelectedColumnForBrainModel(surfaceModelIndex);
            }
            nodeFileType = GUI_NODE_FILE_TYPE_METRIC;
         }
         break;
      case DIALOG_MODE_SURFACE_SHAPE:
         {
            //
            // set up for surface shape
            //
            SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
            nodeAttributeLabel = "Surface Shape";
            if (ssf->getNumberOfColumns() > 0) {
               DisplaySettingsSurfaceShape* dsss = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceShape();
               nodeAttributeColumn = dsss->getFirstSelectedColumnForBrainModel(surfaceModelIndex);
            }
            nodeFileType = GUI_NODE_FILE_TYPE_SURFACE_SHAPE;
         }
         break;
      case DIALOG_MODE_SEGMENT_VOLUME:
         break;
   }

   //
   // Create the column selection for node attribute (metric/paint/shape) conversions
   //
   metricAlgorithmComboBox = NULL;
   nodeAttributeColumnComboBox = NULL;
   if (nodeFileType != GUI_NODE_FILE_TYPE_NONE) {
      //
      // vertical box for node attribute column
      //
      QGroupBox* nodeAttributeGroupBox = new QGroupBox(nodeAttributeLabel);
      dialogLayout->addWidget(nodeAttributeGroupBox);
      QVBoxLayout* nodeAttributeLayout = new QVBoxLayout(nodeAttributeGroupBox);
      
      //
      // Node attribute column selection
      //
      nodeAttributeColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                          nodeFileType,
                                          false,
                                          false,
                                          false);
      if (nodeAttributeColumnComboBox->count() > 0) {
         nodeAttributeColumnComboBox->setCurrentIndex(nodeAttributeColumn);
      }
      nodeAttributeLayout->addWidget(nodeAttributeColumnComboBox);
      
      if (mode == DIALOG_MODE_METRIC) {
         QLabel* metricLabel = new QLabel("Metric Algorithm  ");
         metricAlgorithmComboBox = new QComboBox;
         metricAlgorithmComboBox->addItem("Interpolate");
         metricAlgorithmComboBox->addItem("Largest Value");
         metricAlgorithmComboBox->setToolTip(
                       "Interpolate - The output voxel is the average\n"
                       "of all surface pieces that intersect the voxel.\n"
                       "\n"
                       "Largest Value - The output voxel is the largest\n"
                       "value of all surface pieces that intersect the voxel.");
         QHBoxLayout* metricLayout = new QHBoxLayout;
         metricLayout->addWidget(metricLabel);
         metricLayout->addWidget(metricAlgorithmComboBox);
         nodeAttributeLayout->addLayout(metricLayout);
      }
   }
   
   
   //
   // vertical box for misc selections
   //
   QGroupBox* miscGroupBox = new QGroupBox("Miscellaneous");
   QVBoxLayout* miscGroupLayout = new QVBoxLayout(miscGroupBox);
   dialogLayout->addWidget(miscGroupBox);
   
   //
   // Create node to voxel mapping
   //
   nodeToVoxelCheckBox = new QCheckBox("Create Node to Voxel Mapping");
   miscGroupLayout->addWidget(nodeToVoxelCheckBox);
   
   //
   // Node to voxel mapping file name
   //
   QLabel* nodeToVoxelLabel = new QLabel("File Name ");
   nodeToVoxelFileNameLineEdit = new QLineEdit;
   nodeToVoxelFileNameLineEdit->setText(savedNodeToVoxelFileName);
   QHBoxLayout* nodeToVoxelLayout = new QHBoxLayout;
   nodeToVoxelLayout->addWidget(nodeToVoxelLabel);
   nodeToVoxelLayout->addWidget(nodeToVoxelFileNameLineEdit);
   miscGroupLayout->addLayout(nodeToVoxelLayout);
   
   //
   // Enable node to voxel mapping filename when checkbox checked
   //
   QObject::connect(nodeToVoxelCheckBox, SIGNAL(toggled(bool)),
                    nodeToVoxelFileNameLineEdit, SLOT(setEnabled(bool)));
   nodeToVoxelCheckBox->setChecked(savedNodeToVoxelMapping);
   nodeToVoxelFileNameLineEdit->setEnabled(savedNodeToVoxelMapping);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   firstTime = false;
}

/**
 * Destructor.
 */
GuiSurfaceToVolumeDialog::~GuiSurfaceToVolumeDialog()
{
}

/**
 * called enter ac pushbutton is pressed.
 */
void 
GuiSurfaceToVolumeDialog::slotAcPushButton()
{
   std::vector<QString> labels;
   labels.push_back("AC X Voxel Index ");
   labels.push_back("AC Y Voxel Index ");
   labels.push_back("AC Z Voxel Index ");
   std::vector<QString> values;
   values.push_back("0");
   values.push_back("0");
   values.push_back("0");
   
   //
   // Popup a dialog to get the AC position
   //
   QtMultipleInputDialog mid(this,
                             "Enter Anterior Commissure Voxel Indices",
                             "",
                             labels,
                             values,
                             true,
                             true);
   if (mid.exec() == QDialog::Accepted) {
      std::vector<float> acxyz(3);
      mid.getValues(acxyz);
      
      float voxelSize[3];
      getVolumeVoxelSizes(voxelSize);
      
      float origin[3] = {
         -(acxyz[0] * voxelSize[0]),
         -(acxyz[1] * voxelSize[1]),
         -(acxyz[2] * voxelSize[2])
      };
      setVolumeOrigin(origin);
   }
}

/**
 * called when get parameters from volume file is pressed.
 */
void 
GuiSurfaceToVolumeDialog::slotParamtersFromVolumePushButton()
{
   //
   // Create a spec file dialog to select the spec file.
   //
   WuQFileDialog openVolumeFileDialog(this);
   openVolumeFileDialog.setDirectory(QDir::currentPath());
   openVolumeFileDialog.setModal(true);
   openVolumeFileDialog.setAcceptMode(WuQFileDialog::AcceptOpen);
   openVolumeFileDialog.setWindowTitle("Choose Volume File");
   openVolumeFileDialog.setFileMode(WuQFileDialog::ExistingFile);
   openVolumeFileDialog.setFilter(FileFilters::getVolumeGenericFileFilter());
   if (openVolumeFileDialog.exec() == QDialog::Accepted) {
      if (openVolumeFileDialog.selectedFiles().count() > 0) {
         const QString vname(openVolumeFileDialog.selectedFiles().at(0));
         if (vname.isEmpty() == false) {
            VolumeFile vf;
            try {
               vf.readFile(vname, VolumeFile::VOLUME_READ_HEADER_ONLY);
               const VolumeFile::ORIENTATION lpiOrientation[3] = {
                                       VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                       VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                       VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR };
               vf.permuteToOrientation(lpiOrientation);

               float origin[3];
               float spacing[3];
               int dim[3];
               vf.getDimensions(dim);
               vf.getOrigin(origin);
               vf.getSpacing(spacing);
               
               setVolumeDimensions(dim);
               setVolumeVoxelSizes(spacing);
               setVolumeOrigin(origin);
            }
            catch (FileException& e) {
               QMessageBox::critical(this, "Error Reading Volume", e.whatQString());
            }
         }
      }
   }
}

/**
 * Called when a volume space is selected.
 */
void
GuiSurfaceToVolumeDialog::slotVolumeSpaceSelection(int item)
{
   if (volumeSpaceComboBox->count() == 0) {
      return;
   }
   
   savedVolumeSpace = volumeSpaceComboBoxSpaces[item];
   
   int dim[3]    = { 0, 0, 0 };
   float size[3] = { 0.0, 0.0, 0.0 };
   float origin[3] = { 0.0, 0.0, 0.0 };
   
   savedVolumeSpace.getDimensions(dim);
   savedVolumeSpace.getVoxelSize(size);
   savedVolumeSpace.getOrigin(origin);
   
   const bool standardSpaceFlag = (dim[0] > 0);
   if (standardSpaceFlag) {
      volumeDimensionXSpinBox->setValue(dim[0]);
      volumeDimensionYSpinBox->setValue(dim[1]);
      volumeDimensionZSpinBox->setValue(dim[2]);
      volumeVoxelSizeXDoubleSpinBox->setValue(size[0]);
      volumeVoxelSizeYDoubleSpinBox->setValue(size[1]);
      volumeVoxelSizeZDoubleSpinBox->setValue(size[2]);
      volumeOriginXDoubleSpinBox->setValue(origin[0]);
      volumeOriginYDoubleSpinBox->setValue(origin[1]);
      volumeOriginZDoubleSpinBox->setValue(origin[2]);
   }
   
   volumeDimensionXSpinBox->setEnabled(! standardSpaceFlag);
   volumeDimensionYSpinBox->setEnabled(! standardSpaceFlag);
   volumeDimensionZSpinBox->setEnabled(! standardSpaceFlag);
   volumeVoxelSizeXDoubleSpinBox->setEnabled(! standardSpaceFlag);
   volumeVoxelSizeYDoubleSpinBox->setEnabled(! standardSpaceFlag);
   volumeVoxelSizeZDoubleSpinBox->setEnabled(! standardSpaceFlag);
   volumeOriginXDoubleSpinBox->setEnabled(! standardSpaceFlag);
   volumeOriginYDoubleSpinBox->setEnabled(! standardSpaceFlag);
   volumeOriginZDoubleSpinBox->setEnabled(! standardSpaceFlag);
}

/**
 * get the selected surface
 */
BrainModelSurface* 
GuiSurfaceToVolumeDialog::getSelectedSurface() const
{
   const int modelIndex = surfaceSelectionBox->getSelectedBrainModelIndex();
   if (modelIndex < theMainWindow->getBrainSet()->getNumberOfBrainModels()) {
      return dynamic_cast<BrainModelSurface*>(theMainWindow->getBrainSet()->getBrainModel(modelIndex));
   }
   return NULL;
}

/**
 * get the selected paint column.
 */
int
GuiSurfaceToVolumeDialog::getSelectedNodeAttributeColumn() const
{
   if (nodeAttributeColumnComboBox != NULL) {
      return nodeAttributeColumnComboBox->currentIndex();
   }
   return -1;
}

/**
 * get the offset
 */
void 
GuiSurfaceToVolumeDialog::getSurfaceOffset(float offset[3]) const
{
   offset[0] = surfaceOffsetXDoubleSpinBox->text().toFloat();
   offset[1] = surfaceOffsetYDoubleSpinBox->text().toFloat();
   offset[2] = surfaceOffsetZDoubleSpinBox->text().toFloat();
}

/**
 * get the desired surface inner boundary.
 */
float 
GuiSurfaceToVolumeDialog::getSurfaceInnerBoundary() const
{
   if (surfaceThicknessStepDoubleSpinBox != NULL) {
      return surfaceInnerBoundaryDoubleSpinBox->text().toFloat();
   }
   return 0;
}

/**
 * get the desired surface outer boundary.
 */
float 
GuiSurfaceToVolumeDialog::getSurfaceOuterBoundary() const
{
   if (surfaceOuterBoundaryDoubleSpinBox != NULL) {
      return surfaceOuterBoundaryDoubleSpinBox->text().toFloat();
   }
   return 0;
}

/**
 * get the desired surface thickness step.
 */
float 
GuiSurfaceToVolumeDialog::getSurfaceThicknessStep() const
{
   if (surfaceThicknessStepDoubleSpinBox != NULL) {
      return surfaceThicknessStepDoubleSpinBox->text().toFloat();
   }
   return 0;
}

/**
 * get the volume dimensions
 */
void 
GuiSurfaceToVolumeDialog::getVolumeDimensions(int dim[3]) const
{
   dim[0] = volumeDimensionXSpinBox->value();
   dim[1] = volumeDimensionYSpinBox->value();
   dim[2] = volumeDimensionZSpinBox->value();
}
      
/**
 * set the volume dimensions
 */
void 
GuiSurfaceToVolumeDialog::setVolumeDimensions(const int dim[3]) 
{
   volumeDimensionXSpinBox->setValue(dim[0]);
   volumeDimensionYSpinBox->setValue(dim[1]);
   volumeDimensionZSpinBox->setValue(dim[2]);
}
      
/**
 * Get the volume voxel sizes.
 */
void
GuiSurfaceToVolumeDialog::getVolumeVoxelSizes(float voxelSize[3]) const
{
   voxelSize[0] = volumeVoxelSizeXDoubleSpinBox->text().toFloat();
   voxelSize[1] = volumeVoxelSizeYDoubleSpinBox->text().toFloat();
   voxelSize[2] = volumeVoxelSizeZDoubleSpinBox->text().toFloat();
}

/**
 * Set the volume voxel sizes.
 */
void
GuiSurfaceToVolumeDialog::setVolumeVoxelSizes(const float voxelSize[3]) 
{
   volumeVoxelSizeXDoubleSpinBox->setValue(voxelSize[0]);
   volumeVoxelSizeYDoubleSpinBox->setValue(voxelSize[1]);
   volumeVoxelSizeZDoubleSpinBox->setValue(voxelSize[2]);
}
/**
 * Get the volume origin.
 */
void
GuiSurfaceToVolumeDialog::getVolumeOrigin(float origin[3]) const
{
   origin[0] = volumeOriginXDoubleSpinBox->text().toFloat();
   origin[1] = volumeOriginYDoubleSpinBox->text().toFloat();
   origin[2] = volumeOriginZDoubleSpinBox->text().toFloat();
}

/**
 * Set the volume origin.
 */
void
GuiSurfaceToVolumeDialog::setVolumeOrigin(const float origin[3]) 
{
   volumeOriginXDoubleSpinBox->setValue(origin[0]);
   volumeOriginYDoubleSpinBox->setValue(origin[1]);
   volumeOriginZDoubleSpinBox->setValue(origin[2]);
}

/**
 * get the standard volume space.
 */
StereotaxicSpace 
GuiSurfaceToVolumeDialog::getStandardVolumeSpace() const
{
   StereotaxicSpace volumeSpace = StereotaxicSpace(StereotaxicSpace::SPACE_UNKNOWN);
   
   if (volumeSpaceComboBox->count() > 0) {
      volumeSpace = volumeSpaceComboBoxSpaces[volumeSpaceComboBox->currentIndex()];
   }
   
   return volumeSpace;
}

/**
 * Get node to voxel mapping enabled.
 */
bool 
GuiSurfaceToVolumeDialog::getNodeToVoxelMappingEnabled() const 
{ 
   return nodeToVoxelCheckBox->isChecked(); 
}

/**
 * Get node to voxel file name.
 */
QString 
GuiSurfaceToVolumeDialog::getNodeToVoxelMappingFileName() const 
{ 
   return nodeToVoxelFileNameLineEdit->text();
}

/**
 * Called when OK or Cancel buttons pressed.
 */
void
GuiSurfaceToVolumeDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      getSurfaceOffset(savedOffset);
      getVolumeDimensions(savedDimensions);
      getVolumeVoxelSizes(savedVoxelSize);
      getVolumeOrigin(savedOrigin);
      savedInnerBoundary = getSurfaceInnerBoundary();
      savedOuterBoundary = getSurfaceOuterBoundary();
      savedThicknessStep = getSurfaceThicknessStep();
      savedSurface = getSelectedSurface();
      savedVolumeSpace = volumeSpaceComboBoxSpaces[volumeSpaceComboBox->currentIndex()];
      savedNodeToVoxelMapping = getNodeToVoxelMappingEnabled();
      savedNodeToVoxelFileName = getNodeToVoxelMappingFileName();
   }
   
   QDialog::done(r);
}

/**
 * get intersection mode.
 */
BrainModelSurfaceToVolumeConverter::INTERSECTION_MODE 
GuiSurfaceToVolumeDialog::getIntersectionMode() const
{
   return BrainModelSurfaceToVolumeConverter::INTERSECTION_MODE_INTERSECT_TILES_AND_VOXELS;
}
      
/**
 * get the metric conversion mode.
 */
BrainModelSurfaceToVolumeConverter::CONVERSION_MODE 
GuiSurfaceToVolumeDialog::getMetricConversionMode() const
{
   BrainModelSurfaceToVolumeConverter::CONVERSION_MODE conversionMode = 
            BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE;
   if (metricAlgorithmComboBox != NULL) {
      if (metricAlgorithmComboBox->currentIndex() != 0) {
         conversionMode = BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_METRIC_NO_INTERPOLATE;
      }
   }
   return conversionMode;
}

