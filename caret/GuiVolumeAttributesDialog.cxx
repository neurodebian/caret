
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

#include <algorithm>
#include <limits>
#include <sstream>

#include <QApplication>
#include <QComboBox>
#include <QDateTime>
#include "WuQFileDialog.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolTip>

#include "vtkTransform.h"

#include "BrainModelVolume.h"
#include "BrainModelVolumeBiasCorrection.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiHistogramDisplayDialog.h"
#include "GuiMainWindow.h"
#include "GuiToolBar.h"
#include "GuiVolumeAttributesDialog.h"
#include "GuiVolumeFileOrientationComboBox.h"
#include "GuiVolumeRescaleDialog.h"
#include "ParamsFile.h"
#include <QDoubleSpinBox>
#include "QtMultipleInputDialog.h"
#include "QtRadioButtonSelectionDialog.h"
#include "QtUtilities.h"
#include "StereotaxicSpace.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumeAttributesDialog::GuiVolumeAttributesDialog(QWidget* parent, 
                                                     const bool modalFlag, Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   setModal(modalFlag);
   setWindowTitle("Volume Attributes Editor");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Label and combo box for volume type
   //
   QLabel* volumeTypeLabel = new QLabel("Volume Type");
   volumeTypeComboBox = new QComboBox;
   volumeTypeComboBox->insertItem(VolumeFile::VOLUME_TYPE_ANATOMY, "Anatomy");
   volumeTypeComboBox->insertItem(VolumeFile::VOLUME_TYPE_FUNCTIONAL, "Functional");
   volumeTypeComboBox->insertItem(VolumeFile::VOLUME_TYPE_PAINT, "Paint");
   volumeTypeComboBox->insertItem(VolumeFile::VOLUME_TYPE_PROB_ATLAS, "Prob Atlas");
   volumeTypeComboBox->insertItem(VolumeFile::VOLUME_TYPE_RGB, "RGB");
   volumeTypeComboBox->insertItem(VolumeFile::VOLUME_TYPE_SEGMENTATION, "Segmentation");
   volumeTypeComboBox->insertItem(VolumeFile::VOLUME_TYPE_VECTOR, "Vector");
   QObject::connect(volumeTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotVolumeTypeSelected()));
   volumeTypeComboBox->setToolTip(
                 "Use this control to select the\n"
                 "type of volume for editing.");
   
   //
   // Label and combo box for volume file selection
   //
   QLabel* volumeFileLabel = new QLabel("Volume File");
   volumeFileComboBox = new QComboBox;
   QObject::connect(volumeFileComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotVolumeFileSelected()));
   volumeFileComboBox->setToolTip(
                 "Use this control to select a volume file.");
   
   //
   // Group box and layout for volume selection
   //
   QGroupBox* volumeGroupBox = new QGroupBox("Volume Selection");
   dialogLayout->addWidget(volumeGroupBox);
   QGridLayout* volumeGroupLayout = new QGridLayout(volumeGroupBox);
   volumeGroupLayout->addWidget(volumeTypeLabel, 0, 0);
   volumeGroupLayout->addWidget(volumeTypeComboBox, 0, 1);
   volumeGroupLayout->addWidget(volumeFileLabel, 1, 0);
   volumeGroupLayout->addWidget(volumeFileComboBox, 1, 1);
   
   //
   // Create the tab widget
   //
   tabWidget = new QTabWidget;
   
   //
   // create the alignment page
   //
   createAlignmentPage();
                                 
   //
   // create the coordinates page
   //
   createCoordinatesPage();
                                 
   //
   // create the orientation page
   //
   createOrientationPage();
   
   //
   // create the resampling page
   //
   createResamplingPage();
   
   //
   // create the data page
   //
   createDataPage();
   
   //
   // Group box and layout for tab widget
   //
   QGroupBox* tabGroupBox = new QGroupBox("Volume Attributes");
   QVBoxLayout* tabGroupLayout = new QVBoxLayout(tabGroupBox);
   tabGroupLayout->addWidget(tabWidget);
   dialogLayout->addWidget(tabGroupBox);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   tabWidget->setCurrentWidget(coordinatesPage);
   
   updateDialog();
}

/**
 * Destructor.
 */
GuiVolumeAttributesDialog::~GuiVolumeAttributesDialog()
{
}

/**
 * create the alignment page.
 */
void 
GuiVolumeAttributesDialog::createAlignmentPage()
{
   //
   // colum labels
   //
   QLabel* structureLabel = new QLabel("Structure");
   QLabel* xLabel = new QLabel("X");
   QLabel* yLabel = new QLabel("Y");
   QLabel* zLabel = new QLabel("Z");
   
   //
   // AC spin boxes
   //
   QLabel* acLabel = new QLabel("Anterior Commissure");
   alignAcXSpinBox = new QSpinBox;
   alignAcXSpinBox->setMinimum(0);
   alignAcXSpinBox->setMaximum(1000000);
   alignAcXSpinBox->setSingleStep(1);
   alignAcYSpinBox = new QSpinBox;
   alignAcYSpinBox->setMinimum(0);
   alignAcYSpinBox->setMaximum(1000000);
   alignAcYSpinBox->setSingleStep(1);
   alignAcZSpinBox = new QSpinBox;
   alignAcZSpinBox->setMinimum(0);
   alignAcZSpinBox->setMaximum(1000000);
   alignAcZSpinBox->setSingleStep(1);
   QPushButton* acPushButton = new QPushButton("Use Crosshairs");
   acPushButton->setAutoDefault(false);
   QObject::connect(acPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCrosshairsAlignAC()));
                    
   //
   // PC spin boxes
   //
   QLabel* pcLabel = new QLabel("Posterior Commissure");
   alignPcXSpinBox = new QSpinBox;
   alignPcXSpinBox->setMinimum(0);
   alignPcXSpinBox->setMaximum(1000000);
   alignPcXSpinBox->setSingleStep(1);
   alignPcYSpinBox = new QSpinBox;
   alignPcYSpinBox->setMinimum(0);
   alignPcYSpinBox->setMaximum(1000000);
   alignPcYSpinBox->setSingleStep(1);
   alignPcZSpinBox = new QSpinBox;
   alignPcZSpinBox->setMinimum(0);
   alignPcZSpinBox->setMaximum(1000000);
   alignPcZSpinBox->setSingleStep(1);
   QPushButton* pcPushButton = new QPushButton("Use Crosshairs");
   pcPushButton->setAutoDefault(false);
   QObject::connect(pcPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCrosshairsAlignPC()));
   
   //
   // CC spin boxes
   //
   QLabel* lfLabel = new QLabel("Longitudinal Fissure");
   alignLfXSpinBox = new QSpinBox;
   alignLfXSpinBox->setMinimum(0);
   alignLfXSpinBox->setMaximum(1000000);
   alignLfXSpinBox->setSingleStep(1);
   alignLfYSpinBox = new QSpinBox;
   alignLfYSpinBox->setMinimum(0);
   alignLfYSpinBox->setMaximum(1000000);
   alignLfYSpinBox->setSingleStep(1);
   alignLfZSpinBox = new QSpinBox;
   alignLfZSpinBox->setMinimum(0);
   alignLfZSpinBox->setMaximum(1000000);
   alignLfZSpinBox->setSingleStep(1);
   QPushButton* lfPushButton = new QPushButton("Use Crosshairs");
   lfPushButton->setAutoDefault(false);
   QObject::connect(lfPushButton, SIGNAL(clicked()),
                    this, SLOT(slotCrosshairsAlignLF()));
   
   //
   // Instructions
   //
   const QString instructions = 
      "Place the crosshairs over the Anterior Commissure and press\n"
      "the \"Use Crosshairs\" button.\n"
      "\n"
      "Place the crosshairs over the Posterior Commissure and press \n"
      "the \"Use Crosshairs\" button.\n"
      "\n"
      "Place the crosshairs over a voxel in the superior region of the \n"
      "longitudinal fissure and press the \"Use Crosshairs\" button.\n"
      "\n"
      "Press the \"Apply\" button to AC-PC align the volume.";
   QLabel* instructionsLabel = new QLabel(instructions);
   
   //
   // Arrange the grid widgets 
   //
   //
   // Child of page
   //
   QWidget* alignWidget = new QWidget;
   alignWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   QGridLayout* alignGrid = new QGridLayout(alignWidget);
   alignGrid->setMargin(3);
   alignGrid->setSpacing(3);
   alignGrid->addWidget(structureLabel, 0, 0, Qt::AlignHCenter); 
   alignGrid->addWidget(xLabel, 0, 1, Qt::AlignHCenter); 
   alignGrid->addWidget(yLabel, 0, 2, Qt::AlignHCenter); 
   alignGrid->addWidget(zLabel, 0, 3, Qt::AlignHCenter); 
   alignGrid->addWidget(acLabel, 1, 0); 
   alignGrid->addWidget(alignAcXSpinBox, 1, 1); 
   alignGrid->addWidget(alignAcYSpinBox, 1, 2); 
   alignGrid->addWidget(alignAcZSpinBox, 1, 3); 
   alignGrid->addWidget(acPushButton, 1, 4); 
   alignGrid->addWidget(pcLabel, 2, 0); 
   alignGrid->addWidget(alignPcXSpinBox, 2, 1); 
   alignGrid->addWidget(alignPcYSpinBox, 2, 2); 
   alignGrid->addWidget(alignPcZSpinBox, 2, 3); 
   alignGrid->addWidget(pcPushButton, 2, 4); 
   alignGrid->addWidget(lfLabel, 3, 0); 
   alignGrid->addWidget(alignLfXSpinBox, 3, 1); 
   alignGrid->addWidget(alignLfYSpinBox, 3, 2); 
   alignGrid->addWidget(alignLfZSpinBox, 3, 3); 
   alignGrid->addWidget(lfPushButton, 3, 4); 
   alignGrid->addWidget(instructionsLabel, 4, 0, 1, 5);
   
   //
   // widget and layout for page
   //
   //
   // Create the alignment page
   //
   alignmentPage = new QWidget;
   tabWidget->addTab(alignmentPage, "AC-PC Align");
   QVBoxLayout* pageLayout = new QVBoxLayout(alignmentPage);
   pageLayout->setSpacing(3);
   pageLayout->addWidget(alignWidget);
   
}
      
/**
 * called when align set AC X-Hairs button pressed.
 */
void 
GuiVolumeAttributesDialog::slotCrosshairsAlignAC()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.");
      return;
   }
   
   int slices[3];
   bmv->getSelectedOrthogonalSlices(0, slices);
   
   alignAcXSpinBox->setValue(slices[0]);
   alignAcYSpinBox->setValue(slices[1]);
   alignAcZSpinBox->setValue(slices[2]);
}

/**
 * called when align set PC X-Hairs button pressed.
 */
void 
GuiVolumeAttributesDialog::slotCrosshairsAlignPC()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.");
      return;
   }
   
   int slices[3];
   bmv->getSelectedOrthogonalSlices(0, slices);
   
   alignPcXSpinBox->setValue(slices[0]);
   alignPcYSpinBox->setValue(slices[1]);
   alignPcZSpinBox->setValue(slices[2]);
}

/**
 * called when align set LF X-Hairs button pressed.
 */
void 
GuiVolumeAttributesDialog::slotCrosshairsAlignLF()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.");
      return;
   }
   
   int slices[3];
   bmv->getSelectedOrthogonalSlices(0, slices);
   
   alignLfXSpinBox->setValue(slices[0]);
   alignLfYSpinBox->setValue(slices[1]);
   alignLfZSpinBox->setValue(slices[2]);
}

/**
 * Create the data page.
 */
void
GuiVolumeAttributesDialog::createDataPage()
{
   //
   // Histogram button
   //
   QPushButton* histogramButton = new QPushButton("Show Histogram...");
   histogramButton->setFixedSize(histogramButton->sizeHint());
   histogramButton->setAutoDefault(false);
   QObject::connect(histogramButton, SIGNAL(clicked()),
                    this, SLOT(slotHistogramButton()));
   histogramButton->setToolTip(
                 "Pressing this button will display a\n"
                 "histogram for the selected volume.");
                 
   //
   // Rescale Voxels button
   //
   QPushButton* rescaleButton = new QPushButton("Rescale Voxels Values...");
   rescaleButton->setFixedSize(rescaleButton->sizeHint());
   rescaleButton->setAutoDefault(false);
   QObject::connect(rescaleButton, SIGNAL(clicked()),
                    this, SLOT(slotRescaleVoxelsButton()));
   rescaleButton->setToolTip(
                 "Pressing this button will display a\n"
                 "dialog for rescaling the selected \n"
                 "volume's voxels.");
   
   QPushButton* uniformityButton = NULL;
#ifdef HAVE_ITK
   //
   // uniformity correction button
   //
   uniformityButton = new QPushButton("Non-Uniformity Correction...");
   uniformityButton->setFixedSize(uniformityButton->sizeHint());
   uniformityButton->setAutoDefault(false);
   QObject::connect(uniformityButton, SIGNAL(clicked()),
                    this, SLOT(slotUniformityCorrection()));
   uniformityButton->setToolTip(
                 "Pressing this button will perform\n"
                 "corrections for non-uniformity of \n"
                 "the volume's voxels.");
#endif // HAVE_ITK
   //
   // Create the data page
   //
   dataPage = new QWidget;
   QVBoxLayout* dataPageLayout = new QVBoxLayout(dataPage);
   dataPageLayout->addWidget(histogramButton);
   dataPageLayout->addWidget(rescaleButton);
   if (uniformityButton != NULL) {
      dataPageLayout->addWidget(uniformityButton);
   }
   dataPageLayout->addStretch();
   tabWidget->addTab(dataPage, "Data");   
}   

/**
 * called when uniformity correction button is pressed.
 */
void 
GuiVolumeAttributesDialog::slotUniformityCorrection()
{
   VolumeFile* vf = getSelectedVolume(false);
   if (vf != NULL) {
      try {
         
         //vf->uniformityCorrection();

         std::vector<QString> labels, values;
         labels.push_back("Gray Min ");         values.push_back("80");
         labels.push_back("Gray Max ");         values.push_back("125");
         labels.push_back("White Min ");        values.push_back("140");
         labels.push_back("White Max ");        values.push_back("190");
         labels.push_back("Threshold - Low ");  values.push_back("30");
         labels.push_back("Threshold - High "); values.push_back("255");
         labels.push_back("Iterations - X ");   values.push_back("25");
         labels.push_back("Iterations - Y ");   values.push_back("25");
         labels.push_back("Iterations - Z ");   values.push_back("25");
         
         QtMultipleInputDialog mid(this,
                                   "Enter Parameters",
                                   "Enter min/max gray and white matter values.\n"
                                   "Enter lower and upper thresholds.\n"
                                   "Enter Iterations for each slice axis.",
                                   labels,
                                   values,
                                   true,
                                   true);
         if (mid.exec() == QDialog::Accepted) {
            QTime timer;
            timer.start();
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            
            std::vector<int> floatValues;
            mid.getValues(floatValues);
            std::vector<int> intValues;
            mid.getValues(intValues);
            const float grayWhite[4] = {
               floatValues[0],
               floatValues[1],
               floatValues[2],
               floatValues[3]
            };
            const float thresholds[2] = {
               floatValues[4],
               floatValues[5]
            };
            const int iterations[3] = {
               intValues[6],
               intValues[7],
               intValues[8]
            };
            
            BrainModelVolumeBiasCorrection biasCorrector(vf, 
                                                         grayWhite,
                                                         thresholds,
                                                         iterations);
            biasCorrector.execute();
            
            const float elapsedTime = (static_cast<float>(timer.elapsed()) * 0.001) / 60.0;
            std::ostringstream str;
            str << "Uniformity Correction Time: "
                << elapsedTime
                << " minutes.";
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, "INFO", str.str().c_str());
            beep();
            GuiBrainModelOpenGL::updateAllGL();
         }
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
      }
      catch (FileException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }
}
      
/**
 * called when rescale voxels button is pressed.
 */
void 
GuiVolumeAttributesDialog::slotRescaleVoxelsButton()
{
   VolumeFile* vf = getSelectedVolume(true);
   if (vf != NULL) {
      GuiVolumeRescaleDialog vrd(this, 
                                 "",
                                 vf);
      vrd.exec();
   }
}
      
/**
 * Called when histogram button is pressed.
 */
void
GuiVolumeAttributesDialog::slotHistogramButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   VolumeFile* vf = getSelectedVolume(true);
   if (vf != NULL) {
      const int numVoxels = vf->getTotalNumberOfVoxels();
      std::vector<float> values(numVoxels);
      float minValue =  std::numeric_limits<float>::max();
      float maxValue = -std::numeric_limits<float>::max();
      for (int i = 0; i < numVoxels; i++) {
         values[i] = vf->getVoxelWithFlatIndex(i, 0);
         minValue = std::min(minValue, values[i]);
         maxValue = std::max(maxValue, values[i]);
      } 
      
      bool showPeaksFlag = (static_cast<VolumeFile::VOLUME_TYPE>(volumeTypeComboBox->currentIndex())
                            == VolumeFile::VOLUME_TYPE_ANATOMY);
                            
      GuiHistogramDisplayDialog* ghd = new GuiHistogramDisplayDialog(this, 
                                             FileUtilities::basename(vf->getFileName()),
                                             values,
                                             showPeaksFlag,
                                             static_cast<Qt::WindowFlags>(Qt::WA_DeleteOnClose));
      ghd->show();
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * Create the orientation page.
 */
void
GuiVolumeAttributesDialog::createResamplingPage()
{
   //
   // Resampling method
   //
   QLabel* resamplingMethodLabel = new QLabel("Interpolation Method");
   resamplingMethodComboBox = new QComboBox;
   resamplingMethodComboBox->addItem("Cubic (Best Quality)",
                               static_cast<int>(VolumeFile::INTERPOLATION_TYPE_CUBIC));
   resamplingMethodComboBox->addItem("Linear",
                               static_cast<int>(VolumeFile::INTERPOLATION_TYPE_LINEAR));
   resamplingMethodComboBox->addItem("Nearest Neighbor (use for Paint and Atlas)",
                               static_cast<int>(VolumeFile::INTERPOLATION_TYPE_NEAREST_NEIGHBOR));
   QHBoxLayout* resamplingLayout = new QHBoxLayout;
   resamplingLayout->addWidget(resamplingMethodLabel);
   resamplingLayout->addWidget(resamplingMethodComboBox);
   resamplingLayout->addStretch();
   
   //
   // spin boxes for resampling
   //
   QLabel* voxelSizeLabel = new QLabel("Voxel Size ");
   resamplingXDoubleSpinBox = new QDoubleSpinBox;
   resamplingXDoubleSpinBox->setMinimum(-10000.0);
   resamplingXDoubleSpinBox->setMaximum(10000.0);
   resamplingXDoubleSpinBox->setSingleStep(1.0);
   resamplingXDoubleSpinBox->setDecimals(3);
   resamplingYDoubleSpinBox = new QDoubleSpinBox;
   resamplingYDoubleSpinBox->setMinimum(-10000.0);
   resamplingYDoubleSpinBox->setMaximum(10000.0);
   resamplingYDoubleSpinBox->setSingleStep(1.0);
   resamplingYDoubleSpinBox->setDecimals(3);
   resamplingZDoubleSpinBox = new QDoubleSpinBox;
   resamplingZDoubleSpinBox->setMinimum(-10000.0);
   resamplingZDoubleSpinBox->setMaximum(10000.0);
   resamplingZDoubleSpinBox->setSingleStep(1.0);
   resamplingZDoubleSpinBox->setDecimals(3);
   QHBoxLayout* resampleLayout = new QHBoxLayout;
   resampleLayout->addWidget(voxelSizeLabel);
   resampleLayout->addWidget(resamplingXDoubleSpinBox);
   resampleLayout->addWidget(resamplingYDoubleSpinBox);
   resampleLayout->addWidget(resamplingZDoubleSpinBox);
   resampleLayout->addStretch();
   
   //
   // Create the resampling page
   //
   resamplingPage = new QWidget;
   QVBoxLayout* resamplePageLayout = new QVBoxLayout(resamplingPage);
   resamplePageLayout->addLayout(resampleLayout);
   resamplePageLayout->addLayout(resamplingLayout);
   resamplePageLayout->addStretch();
   tabWidget->addTab(resamplingPage, "Resample");
   
}   

/**
 * load the resampling parameters.
 */
void 
GuiVolumeAttributesDialog::loadVolumeResampling()
{
   VolumeFile* vf = getSelectedVolume(false);
   if (vf != NULL) {
      float spacing[3];
      vf->getSpacing(spacing);
      resamplingXDoubleSpinBox->setValue(spacing[0]);
      resamplingYDoubleSpinBox->setValue(spacing[1]);
      resamplingZDoubleSpinBox->setValue(spacing[2]);
   }
}

/**
 * Called to resample the volume.
 */
void
GuiVolumeAttributesDialog::resampleVolume()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   VolumeFile* vf = getSelectedVolume(false);
   if (vf != NULL) {
      float volSpace[3];
      vf->getSpacing(volSpace);
      if ((volSpace[0] <= 0.0) ||
          (volSpace[1] <= 0.0) ||
          (volSpace[2] <= 0.0)) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", 
                                 "The volume's X, Y, and Z voxel sizes (set using the Coordinates tab)\n"
                                 "must all be positive for resampling to function correctly.");
         return;
      }
      const float spacing[3] = {
         resamplingXDoubleSpinBox->value(),
         resamplingYDoubleSpinBox->value(),
         resamplingZDoubleSpinBox->value()
      };
      if ((spacing[0] <= 0.0) ||
          (spacing[1] <= 0.0) ||
          (spacing[2] <= 0.0)) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", 
                                 "The volume's X, Y, and Z resampling voxel sizes must\n"
                                 "all be positive for resampling to function correctly.");
         return;
      }
      
      const int methodIndex = resamplingMethodComboBox->currentIndex();
      const VolumeFile::INTERPOLATION_TYPE interpolationType =
         static_cast<VolumeFile::INTERPOLATION_TYPE>(
            resamplingMethodComboBox->itemData(methodIndex).toInt());
            
      vf->resampleToSpacing(spacing, interpolationType);
      loadVolumeParameters();
      loadVolumeResampling();
   }
   GuiBrainModelOpenGL::updateAllGL();
   GuiToolBar::updateAllToolBars(false); 
   
   QApplication::restoreOverrideCursor();
}

/**
 * Create the orientation page.
 */
void
GuiVolumeAttributesDialog::createOrientationPage()
{
   //
   // Orientation combo boxes
   //
   xOrientationComboBox = new GuiVolumeFileOrientationComboBox(0, 
                                                            VolumeFile::VOLUME_AXIS_X);
   QObject::connect(xOrientationComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotApplyButton()));
   xOrientationComboBox->setToolTip(
                 "Use this control to set the orientation of the\n"
                 "volume in its X-Axis.  Note that Caret always\n"
                 "draws volume assuming they are in LPI orientation.");
   yOrientationComboBox = new GuiVolumeFileOrientationComboBox(0, 
                                                            VolumeFile::VOLUME_AXIS_Y);
   QObject::connect(yOrientationComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotApplyButton()));
   yOrientationComboBox->setToolTip(
                 "Use this control to set the orientation of the\n"
                 "volume in its Y-Axis.  Note that Caret always\n"
                 "draws volume assuming they are in LPI orientation.");
   zOrientationComboBox = new GuiVolumeFileOrientationComboBox(0, 
                                                            VolumeFile::VOLUME_AXIS_Z);
   QObject::connect(zOrientationComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotApplyButton()));
   zOrientationComboBox->setToolTip(
                 "Use this control to set the orientation of the\n"
                 "volume in its Z-Axis.  Note that Caret always\n"
                 "draws volume assuming they are in LPI orientation.");
   QHBoxLayout* orientationLayout = new QHBoxLayout;
   orientationLayout->setSpacing(5);
   orientationLayout->addWidget(xOrientationComboBox);
   orientationLayout->addWidget(yOrientationComboBox);
   orientationLayout->addWidget(zOrientationComboBox);
   
   //
   // convert to LPI button
   //
   QPushButton* lpiPushButton = new QPushButton("Convert to LPI Orientation");
   lpiPushButton->setAutoDefault(false);
   lpiPushButton->setFixedSize(lpiPushButton->sizeHint());
   QObject::connect(lpiPushButton, SIGNAL(clicked()),
                    this, SLOT(slotLpiPushButton()));
   lpiPushButton->setToolTip(
                 "Pressing this button will perform operations to place\n"
                 "the volume into LPI orientation using the current orientation\n"
                 "as show in the combo boxes above.");
                    
   //
   // Flip X, Y, Z push buttons
   //
   QPushButton* flipXPushButton = new QPushButton("Flip About X Screen Axis");
   flipXPushButton->setAutoDefault(false);
   QObject::connect(flipXPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFlipXPushButton()));
   flipXPushButton->setToolTip(
                 "Flips the volume about the SCREEN X-AXIS so that\n"
                 "the top and bottom parts of the volume are flipped.");
   QPushButton* flipYPushButton = new QPushButton("Flip About Y Screen Axis");
   flipYPushButton->setAutoDefault(false);
   QObject::connect(flipYPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFlipYPushButton()));
   flipYPushButton->setToolTip(
                 "Flips the volume about the SCREEN Y-AXIS so that\n"
                 "the right and left parts of the volume are flipped.");
   QHBoxLayout* flipLayout = new QHBoxLayout;
   flipLayout->setSpacing(5);
   flipLayout->addWidget(flipXPushButton);
   flipLayout->addWidget(flipYPushButton);
   flipLayout->addStretch();
   
   //
   // Rotate volume button
   //
   QPushButton* rotatePushButton = new QPushButton("Rotate Clockwise");
   rotatePushButton->setAutoDefault(false);
   rotatePushButton->setFixedSize(rotatePushButton->sizeHint());
   QObject::connect(rotatePushButton, SIGNAL(clicked()),
                    this, SLOT(slotRotatePushButton()));
   rotatePushButton->setToolTip(
                 "Rotates the volume clockwise so that the\n"
                 "right side of the volume is shown at the\n"
                 "bottom of the screen.");

   const QString lpiMessage = 
      "NOTE: If the origin and voxel sizes are not properly set (on the \n"
      "Coordinates tab) the \"Convert to LPI Orientation\" operation will \n"
      "not operate correctly.";
   QLabel* lpiLabel = new QLabel(lpiMessage);
   
   //
   // Create the orientation page
   //
   orientationPage = new QWidget;
   QVBoxLayout* orientationPageLayout = new QVBoxLayout(orientationPage);
   orientationPageLayout->addLayout(orientationLayout);
   orientationPageLayout->addWidget(lpiPushButton);
   orientationPageLayout->addLayout(flipLayout);
   orientationPageLayout->addWidget(rotatePushButton);
   orientationPageLayout->addWidget(lpiLabel);
   orientationPageLayout->addStretch();
   tabWidget->addTab(orientationPage, "Orientation");
   
}

/**
 * Create the coordinates page.
 */
void
GuiVolumeAttributesDialog::createCoordinatesPage()
{
   //
   // Column titles
   //
   QLabel* xLabel = new QLabel("X");
   QLabel* yLabel = new QLabel("Y");
   QLabel* zLabel = new QLabel("Z");
   
   //
   // Dimensions
   //
   QLabel* dimensionsLabel = new QLabel("Dimensions");
   dimensionXLabel = new QLabel("        ");
   dimensionYLabel = new QLabel("        ");
   dimensionZLabel = new QLabel("        ");
   
   //
   // Voxel size
   //
   QLabel* voxelSizeLabel = new QLabel("Voxel Size");
   voxelSizeXDoubleSpinBox = new QDoubleSpinBox;
   voxelSizeXDoubleSpinBox->setMinimum(-5000.0);
   voxelSizeXDoubleSpinBox->setMaximum(5000.0);
   voxelSizeXDoubleSpinBox->setSingleStep(0.5);
   voxelSizeXDoubleSpinBox->setDecimals(3);
   voxelSizeYDoubleSpinBox = new QDoubleSpinBox;
   voxelSizeYDoubleSpinBox->setMinimum(-5000.0);
   voxelSizeYDoubleSpinBox->setMaximum(5000.0);
   voxelSizeYDoubleSpinBox->setSingleStep(0.5);
   voxelSizeYDoubleSpinBox->setDecimals(3);
   voxelSizeZDoubleSpinBox = new QDoubleSpinBox;
   voxelSizeZDoubleSpinBox->setMinimum(-5000.0);
   voxelSizeZDoubleSpinBox->setMaximum(5000.0);
   voxelSizeZDoubleSpinBox->setSingleStep(0.5);
   voxelSizeZDoubleSpinBox->setDecimals(3);

   //
   // origin
   //
   QLabel* originLabel = new QLabel("Origin at Center\nof First Voxel");
   originXDoubleSpinBox = new QDoubleSpinBox;
   originXDoubleSpinBox->setMinimum(-5000.0);
   originXDoubleSpinBox->setMaximum(5000.0);
   originXDoubleSpinBox->setSingleStep(0.5);
   originXDoubleSpinBox->setDecimals(3);
   originYDoubleSpinBox = new QDoubleSpinBox;
   originYDoubleSpinBox->setMinimum(-5000.0);
   originYDoubleSpinBox->setMaximum(5000.0);
   originYDoubleSpinBox->setSingleStep(0.5);
   originYDoubleSpinBox->setDecimals(3);
   originZDoubleSpinBox = new QDoubleSpinBox;
   originZDoubleSpinBox->setMinimum(-5000.0);
   originZDoubleSpinBox->setMaximum(5000.0);
   originZDoubleSpinBox->setSingleStep(0.5);
   originZDoubleSpinBox->setDecimals(3);
   originXDoubleSpinBox->setValue(0.0);
   originYDoubleSpinBox->setValue(0.0);
   originZDoubleSpinBox->setValue(0.0);
   
   //
   // Layout for parameters
   //
   QGridLayout* parametersLayout = new QGridLayout;
   parametersLayout->addWidget(xLabel, 0, 1);
   parametersLayout->addWidget(yLabel, 0, 2);
   parametersLayout->addWidget(zLabel, 0, 3);
   parametersLayout->addWidget(dimensionsLabel, 1, 0);
   parametersLayout->addWidget(dimensionXLabel, 1, 1);
   parametersLayout->addWidget(dimensionYLabel, 1, 2);
   parametersLayout->addWidget(dimensionZLabel, 1, 3);
   parametersLayout->addWidget(voxelSizeLabel, 2, 0);
   parametersLayout->addWidget(voxelSizeXDoubleSpinBox, 2, 1);
   parametersLayout->addWidget(voxelSizeYDoubleSpinBox, 2, 2);
   parametersLayout->addWidget(voxelSizeZDoubleSpinBox, 2, 3);
   parametersLayout->addWidget(originLabel, 3, 0);
   parametersLayout->addWidget(originXDoubleSpinBox, 3, 1);
   parametersLayout->addWidget(originYDoubleSpinBox, 3, 2);
   parametersLayout->addWidget(originZDoubleSpinBox, 3, 3);
   
   //
   // Use main window crosshairs as AC
   //
   QPushButton* mwXHairPushButton = new QPushButton(
                  "Use Main Window Crosshairs as AC Location");
   mwXHairPushButton->setAutoDefault(false);
   QObject::connect(mwXHairPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMainWindowXHairPushButton()));
   mwXHairPushButton->setToolTip("This button will use the main window\n"
                                    "crosshairs as the Anterior Commissure\n"
                                    "location.  The Anterior Commissure\n"
                                    "position and the voxel sizes will be\n"
                                    "used to determine the volume origin.");
                                    
   //
   // Enter AC button
   //
   QPushButton* acPushButton = new QPushButton("Enter AC Voxel...");
   acPushButton->setAutoDefault(false);
   QObject::connect(acPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAcPushButton()));
   acPushButton->setToolTip("This button displays a dialog for entry of the \n"
                               "Anterior Commissure voxel indices.  These voxel\n"
                               "indices will be used with the voxel size in the\n"
                               "dialog to determine the volume origin.");
   
   //
   // Get AC from params file
   //
   QPushButton* paramsAcPushButton = new QPushButton("Use AC From Params File");
   paramsAcPushButton->setAutoDefault(false);
   QObject::connect(paramsAcPushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamsAcPushButton()));
   paramsAcPushButton->setToolTip("Press this button will use the Anterior\n"
                                     "Commissure position stored in the params\n"
                                     "file to determine the volume's origin.");
                                     
   //
   // Get AC from params file
   //
   QPushButton* paramsWholeAcPushButton = new QPushButton("Use Whole Volume AC From Params File");
   paramsWholeAcPushButton->setAutoDefault(false);
   QObject::connect(paramsWholeAcPushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamsWholeAcPushButton()));
   paramsWholeAcPushButton->setToolTip("Press this button will use the Whole \n"
                                     "Anterior Commissure position stored in the \n"
                                     "params file to determine the volume's origin.");
                                     
   //
   // Get parameters from a volume file
   //
   QPushButton* paramtersFromVolumePushButton = 
            new QPushButton("Get Parameters From Volume File...");
   paramtersFromVolumePushButton->setAutoDefault(false);
   QObject::connect(paramtersFromVolumePushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamtersFromVolumePushButton()));
   paramtersFromVolumePushButton->setToolTip(
                 "This button displays a dialog for selecting a volume\n"
                 "file.  The volume's spacing and origin will be\n"
                 "placed into the dialog.");
                 
   //
   // Get parameters from a standard space
   //
   QPushButton* standardSpacePushButton = 
            new QPushButton("Use Standard Stereotaxic Space...");
   standardSpacePushButton->setAutoDefault(false);
   QObject::connect(standardSpacePushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamsStandardSpacePushButton()));
   standardSpacePushButton->setToolTip(
                 "This button displays a dialog for selecting a standard\n"
                 "space that will be used for setting the origin and spacing.\n"
                 "Only standard spaces having dimensions matching those\n"
                 "shown in the dialog will be listed for selection.");
                 
                 
   std::vector<QPushButton*> pbs;
   pbs.push_back(mwXHairPushButton);
   pbs.push_back(acPushButton);
   pbs.push_back(paramsAcPushButton);
   pbs.push_back(paramsWholeAcPushButton);
   pbs.push_back(paramtersFromVolumePushButton);
   pbs.push_back(standardSpacePushButton);
   QtUtilities::makeButtonsSameSize(pbs);

   //
   // Group box and layoutfor setting origin
   //
   QGroupBox* setOriginGroupBox = new QGroupBox("Set Origin");
   QVBoxLayout* setOriginGroupLayout = new QVBoxLayout(setOriginGroupBox);
   setOriginGroupLayout->addWidget(mwXHairPushButton);
   setOriginGroupLayout->addWidget(acPushButton);
   setOriginGroupLayout->addWidget(paramsAcPushButton);
   setOriginGroupLayout->addWidget(paramsWholeAcPushButton);
   setOriginGroupLayout->addWidget(paramtersFromVolumePushButton);
   setOriginGroupLayout->addWidget(standardSpacePushButton);
   
   //
   // Use main window crosshairs as AC
   //
   QPushButton* paramsACPushButton = new QPushButton("Update AC");
   paramsACPushButton->setAutoDefault(false);
   QObject::connect(paramsACPushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamsUpdateAC()));
   paramsACPushButton->setToolTip("This button will use the current dimensions,\n"
                                     "origin, and voxel sizes to set the parameter\n"
                                     "files's ACx, ACy, and ACz parameters.");

   //
   // Use main window crosshairs as AC
   //
   QPushButton* paramsWholeACPushButton = new QPushButton("Update Whole Volume AC");
   paramsWholeACPushButton->setAutoDefault(false);
   QObject::connect(paramsWholeACPushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamsUpdateWholeVolumeAC()));
   paramsWholeACPushButton->setToolTip("This button will use the current dimensions,\n"
                                          "origin, and voxel sizes to set the parameter\n"
                                          "files's ACx_WholeVolume, ACy_WholeVolume, and\n" 
                                          "ACz_WholeVolume parameters.");
                                          
   QtUtilities::makeButtonsSameSize(paramsACPushButton,
                                    paramsWholeACPushButton);
   
   //
   // Group box and layout for updating params file
   //
   QGroupBox* paramsGroupBox = new QGroupBox("Update Parameters File");
   QVBoxLayout* paramsGroupLayout = new QVBoxLayout(paramsGroupBox);
   paramsGroupLayout->addWidget(paramsACPushButton);
   paramsGroupLayout->addWidget(paramsWholeACPushButton);
   paramsGroupLayout->addStretch();
   
   //
   // Horizontal layout for settings origin and update params file
   //
   QHBoxLayout* originParamsHBoxLayout = new QHBoxLayout;
   originParamsHBoxLayout->addWidget(setOriginGroupBox);
   originParamsHBoxLayout->addWidget(paramsGroupBox);

   //
   // Create the coordinates page
   //
   coordinatesPage = new QWidget;
   QVBoxLayout* coordinatesLayout = new QVBoxLayout(coordinatesPage);
   coordinatesLayout->addLayout(parametersLayout);
   coordinatesLayout->addLayout(originParamsHBoxLayout);
   coordinatesLayout->addStretch();
   tabWidget->addTab(coordinatesPage, "Coordinates");
}

/**
 * called when Update Params File AC pushbutton is pressed.
 */
void 
GuiVolumeAttributesDialog::slotParamsUpdateAC()
{
   const float origin[3] = {
      originXDoubleSpinBox->value(),
      originYDoubleSpinBox->value(),
      originZDoubleSpinBox->value(),
   };
   const float voxelSize[3] = {
      voxelSizeXDoubleSpinBox->value(),
      voxelSizeYDoubleSpinBox->value(),
      voxelSizeZDoubleSpinBox->value(),
   };
   
   int ac[3];
   for (int i = 0; i < 3; i++) {
      if (voxelSize[i] == 0.0) {
         QMessageBox::critical(this, "ERROR", "All voxel sizes must be non-zero.");
         return;
      }
      ac[i] = -static_cast<int>(origin[i] / voxelSize[i]);
   }
   
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   pf->setParameter(ParamsFile::keyACx, ac[0]);
   pf->setParameter(ParamsFile::keyACy, ac[1]);
   pf->setParameter(ParamsFile::keyACz, ac[2]);
   try {
      theMainWindow->getBrainSet()->writeParamsFile(pf->getFileName());
   }
   catch (FileException& e) {
      QString msg("Unable to write parameters file:  ");
      msg.append(e.whatQString());
      QMessageBox::critical(this, "ERROR", msg);
   }
   
   GuiFilesModified fm;
   fm.setParameterModified();
   theMainWindow->fileModificationUpdate(fm);
}

/**
 * called when Update Params File Whole Volume AC pushbutton is pressed.
 */
void 
GuiVolumeAttributesDialog::slotParamsUpdateWholeVolumeAC()
{
   const float origin[3] = {
      originXDoubleSpinBox->value(),
      originYDoubleSpinBox->value(),
      originZDoubleSpinBox->value(),
   };
   const float voxelSize[3] = {
      voxelSizeXDoubleSpinBox->value(),
      voxelSizeYDoubleSpinBox->value(),
      voxelSizeZDoubleSpinBox->value(),
   };
   
   int ac[3];
   for (int i = 0; i < 3; i++) {
      if (voxelSize[i] == 0.0) {
         QMessageBox::critical(this, "ERROR", "All voxel sizes must be non-zero.");
         return;
      }
      ac[i] = -static_cast<int>(origin[i] / voxelSize[i]);
   }
   
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   pf->setParameter(ParamsFile::keyWholeVolumeACx, ac[0]);
   pf->setParameter(ParamsFile::keyWholeVolumeACy, ac[1]);
   pf->setParameter(ParamsFile::keyWholeVolumeACz, ac[2]);
   try {
      theMainWindow->getBrainSet()->writeParamsFile(pf->getFileName());
   }
   catch (FileException& e) {
      QString msg("Unable to write parameters file:  ");
      msg.append(e.whatQString());
      QMessageBox::critical(this, "ERROR", msg);
   }
   
   GuiFilesModified fm;
   fm.setParameterModified();
   theMainWindow->fileModificationUpdate(fm);
}
      
/**
 * called when use Standard Space pushbutton is pressed.
 */
void 
GuiVolumeAttributesDialog::slotParamsStandardSpacePushButton()
{
   const int volDim[3] = {
      dimensionXLabel->text().toInt(),
      dimensionYLabel->text().toInt(),
      dimensionZLabel->text().toInt(),
   };
   
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);

   std::vector<StereotaxicSpace> matchingSpaces;
   std::vector<QString> matchingSpaceNames;
   
   for (unsigned int i = 0; i < allSpaces.size(); i++) {
      int dim[3];
      allSpaces[i].getDimensions(dim);
                                             
      if ((volDim[0] == dim[0]) &&
          (volDim[1] == dim[1]) &&
          (volDim[2] == dim[2])) {
         matchingSpaces.push_back(allSpaces[i]);
         
         float spacing[3];
         allSpaces[i].getVoxelSize(spacing);
         std::ostringstream str;
         str << "Space: "
             << allSpaces[i].getName().toAscii().constData()
             << " voxel size: ("
             << spacing[0] << ", " 
             << spacing[1] << ", " 
             << spacing[2]
             << ")";
         matchingSpaceNames.push_back(str.str().c_str());
      }
   }
   
   if (matchingSpaces.empty()) {
      QMessageBox::critical(this, "ERROR", 
                              "No standard spaces have matching volume dimensions.");
      return;
   }
   
   QtRadioButtonSelectionDialog rbsd(this,
                                     "Choose Standard Space",
                                     "Choose Standard Space",
                                     matchingSpaceNames,
                                     0);
   if (rbsd.exec() == QDialog::Accepted) {
      const int indx = rbsd.getSelectedItemIndex();
      if ((indx >= 0) && (indx < static_cast<int>(matchingSpaces.size()))) {
         float origin[3];
         float spacing[3];
         matchingSpaces[indx].getOrigin(origin);
         matchingSpaces[indx].getVoxelSize(spacing);
         
         voxelSizeXDoubleSpinBox->setValue(spacing[0]);
         voxelSizeYDoubleSpinBox->setValue(spacing[1]);
         voxelSizeZDoubleSpinBox->setValue(spacing[2]);
         originXDoubleSpinBox->setValue(origin[0]);
         originYDoubleSpinBox->setValue(origin[1]);
         originZDoubleSpinBox->setValue(origin[2]);
      }
   }
}
      
/**
 * called when main window crosshairs as AC pushbutton is pressed.
 */
void 
GuiVolumeAttributesDialog::slotMainWindowXHairPushButton()
{
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::critical(this, "ERROR", "There is no volume in the Main Window.");
      return;
   }
   
   int acSlices[3];
   bmv->getSelectedOrthogonalSlices(0, acSlices);
   
   const float voxelSize[3] = {
      voxelSizeXDoubleSpinBox->value(),
      voxelSizeYDoubleSpinBox->value(),
      voxelSizeZDoubleSpinBox->value(),
   };
   
   float origin[3] = {
      -(acSlices[0] * voxelSize[0]),
      -(acSlices[1] * voxelSize[1]),
      -(acSlices[2] * voxelSize[2])
   };
   
   originXDoubleSpinBox->setValue(origin[0]);
   originYDoubleSpinBox->setValue(origin[1]);
   originZDoubleSpinBox->setValue(origin[2]);
}      

/**
 * Called when Use AC from Params File pushbutton pressed.
 */
void
GuiVolumeAttributesDialog::slotParamsAcPushButton()
{
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   float x, y, z;
   if (pf->empty()) {
      QMessageBox::critical(this, "ERROR", "No parameters are loaded.");
   }
   else if (pf->getParameter(ParamsFile::keyACx, x) &&
            pf->getParameter(ParamsFile::keyACy, y) &&
            pf->getParameter(ParamsFile::keyACz, z)) {
      const float voxelSize[3] = {
         voxelSizeXDoubleSpinBox->value(),
         voxelSizeYDoubleSpinBox->value(),
         voxelSizeZDoubleSpinBox->value(),
      };
      
      float origin[3] = {
         -(x * voxelSize[0]),
         -(y * voxelSize[1]),
         -(z * voxelSize[2])
      };
      
      originXDoubleSpinBox->setValue(origin[0]);
      originYDoubleSpinBox->setValue(origin[1]);
      originZDoubleSpinBox->setValue(origin[2]);
   }
   else {
      QMessageBox::critical(this, "ERROR", "AC not found in params file", "OK");
   }
}

/**
 * Called when Use AC from Params File pushbutton pressed.
 */
void
GuiVolumeAttributesDialog::slotParamsWholeAcPushButton()
{
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   float x, y, z;
   if (pf->empty()) {
      QMessageBox::critical(this, "ERROR", "No parameters are loaded.");
   }
   else if (pf->getParameter(ParamsFile::keyWholeVolumeACx, x) &&
            pf->getParameter(ParamsFile::keyWholeVolumeACy, y) &&
            pf->getParameter(ParamsFile::keyWholeVolumeACz, z)) {
      const float voxelSize[3] = {
         voxelSizeXDoubleSpinBox->value(),
         voxelSizeYDoubleSpinBox->value(),
         voxelSizeZDoubleSpinBox->value(),
      };
      
      float origin[3] = {
         -(x * voxelSize[0]),
         -(y * voxelSize[1]),
         -(z * voxelSize[2])
      };
      

      originXDoubleSpinBox->setValue(origin[0]);
      originYDoubleSpinBox->setValue(origin[1]);
      originZDoubleSpinBox->setValue(origin[2]);
   }
   else {
      QMessageBox::critical(this, "ERROR", "AC not found in params file");
   }
}

/**
 * Called when convert to LPI pushbutton is pressed.
 */
void
GuiVolumeAttributesDialog::slotLpiPushButton()
{
   VolumeFile* vf = getSelectedVolume(true);
   if (vf != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      VolumeFile::ORIENTATION lpiOrientation[3] = {
                                    VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                    VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                    VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR
                                                  };
      try {
         vf->permuteToOrientation(lpiOrientation);
      }
      catch (FileException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
         return;
      }
      GuiBrainModelOpenGL::updateAllGL();
      loadVolumeParameters();
      loadVolumeResampling();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * called when rotate push button is pressed.
 */
void
GuiVolumeAttributesDialog::slotRotatePushButton()
{
   bool volumeInMainWindow = false;
   VolumeFile* vf = getSelectedVolume(true);
   if (vf != NULL) {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         volumeInMainWindow = true;
         switch (bmv->getSelectedAxis(0)) {
            case VolumeFile::VOLUME_AXIS_X:
               vf->rotate(VolumeFile::VOLUME_AXIS_X);
               break;
            case VolumeFile::VOLUME_AXIS_Y:
               vf->rotate(VolumeFile::VOLUME_AXIS_Y);
               break;
            case VolumeFile::VOLUME_AXIS_Z:
               vf->rotate(VolumeFile::VOLUME_AXIS_Z);
               break;
            case VolumeFile::VOLUME_AXIS_ALL:
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         GuiBrainModelOpenGL::updateAllGL();
         loadVolumeParameters();
         loadVolumeResampling();
         QApplication::restoreOverrideCursor();
         
         const QString msg = 
            "After rotating a volume, an axis may appear upside down in one or more\n"
            "views.  This is a result of an origin having a component with a positive\n"
            "value and its corresponding voxel size being negative after rotation. \n"
            "Flipping the signs of these two items will correct the problem.\n"
            "Note that Caret is designed to display and operate on volumes in an LPI\n"
            "(Left=neg-X,  Posterior=neg-Y,  Inferior=neg-Z) orientation.";
         QApplication::restoreOverrideCursor();
         QMessageBox::information(this, "INFO", msg);
      }
   }
}

/**
 * called enter ac pushbutton is pressed.
 */
void 
GuiVolumeAttributesDialog::slotAcPushButton()
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
      
      const float voxelSize[3] = {
         voxelSizeXDoubleSpinBox->value(),
         voxelSizeYDoubleSpinBox->value(),
         voxelSizeZDoubleSpinBox->value(),
      };
      
      float origin[3] = {
         -(acxyz[0] * voxelSize[0]),
         -(acxyz[1] * voxelSize[1]),
         -(acxyz[2] * voxelSize[2])
      };
      
      originXDoubleSpinBox->setValue(origin[0]);
      originYDoubleSpinBox->setValue(origin[1]);
      originZDoubleSpinBox->setValue(origin[2]);
   }
}

/**
 * called when get parameters from volume file is pressed.
 */
void 
GuiVolumeAttributesDialog::slotParamtersFromVolumePushButton()
{
   //
   // Create a spec file dialog to select the spec file.
   //
   WuQFileDialog openVolumeFileDialog(this);
   openVolumeFileDialog.setModal(true);
   openVolumeFileDialog.setAcceptMode(WuQFileDialog::AcceptOpen);
   openVolumeFileDialog.setDirectory(QDir::currentPath());
   openVolumeFileDialog.setWindowTitle("Choose Volume File");
   openVolumeFileDialog.setFileMode(WuQFileDialog::ExistingFile);
   openVolumeFileDialog.setFilter(FileFilters::getVolumeGenericFileFilter());
   if (openVolumeFileDialog.exec() == QDialog::Accepted) {
      const QString vname(openVolumeFileDialog.selectedFiles().at(0));
      if (vname.isEmpty() == false) {
         VolumeFile vf;
         try {
            vf.readFile(vname);
   /*
            const VolumeFile::ORIENTATION lpiOrientation[3] = {
                                    VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
                                    VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                    VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR };
            vf.permuteToOrientation(lpiOrientation);
    */        
            float origin[3];
            float spacing[3];
            vf.getOrigin(origin);
            vf.getSpacing(spacing);
            
            originXDoubleSpinBox->setValue(origin[0]);
            originYDoubleSpinBox->setValue(origin[1]);
            originZDoubleSpinBox->setValue(origin[2]);
            voxelSizeXDoubleSpinBox->setValue(spacing[0]);
            voxelSizeYDoubleSpinBox->setValue(spacing[1]);
            voxelSizeZDoubleSpinBox->setValue(spacing[2]);
         }
         catch (FileException& e) {
            QMessageBox::critical(this, "Error Reading Volume", e.whatQString());
         }
      }
   }
}

/**
 * Called when flip X push button is pressed.
 */
void
GuiVolumeAttributesDialog::slotFlipXPushButton()
{
   bool volumeInMainWindow = false;
   VolumeFile* vf = getSelectedVolume(true);
   if (vf != NULL) {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         switch (bmv->getSelectedAxis(0)) {
            case VolumeFile::VOLUME_AXIS_X:
               vf->flip(VolumeFile::VOLUME_AXIS_Z, false);
               break;
            case VolumeFile::VOLUME_AXIS_Y:
               vf->flip(VolumeFile::VOLUME_AXIS_Z, false);
               break;
            case VolumeFile::VOLUME_AXIS_Z:
               vf->flip(VolumeFile::VOLUME_AXIS_Y, false);
               break;
            case VolumeFile::VOLUME_AXIS_ALL:
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         GuiBrainModelOpenGL::updateAllGL();
         loadVolumeParameters();
         loadVolumeResampling();
         QApplication::restoreOverrideCursor();
         volumeInMainWindow = true;
      }
   }
}

/**
 * Called when flip Y push button is pressed.
 */
void
GuiVolumeAttributesDialog::slotFlipYPushButton()
{
   bool volumeInMainWindow = false;
   VolumeFile* vf = getSelectedVolume(true);
   if (vf != NULL) {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         switch (bmv->getSelectedAxis(0)) {
            case VolumeFile::VOLUME_AXIS_X:
               vf->flip(VolumeFile::VOLUME_AXIS_Y, false);
               break;
            case VolumeFile::VOLUME_AXIS_Y:
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               break;
            case VolumeFile::VOLUME_AXIS_Z:
               vf->flip(VolumeFile::VOLUME_AXIS_X, false);
               break;
            case VolumeFile::VOLUME_AXIS_ALL:
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         GuiBrainModelOpenGL::updateAllGL();
         loadVolumeParameters();
         loadVolumeResampling();
         QApplication::restoreOverrideCursor();
         volumeInMainWindow = true;
      }
   }
   if (volumeInMainWindow = false) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR",
                            "The selected volume must be displayed in\n"
                            "the main window to use this operation.");
   }
}

/**
 * Called when apply button is pressed.
 */
void
GuiVolumeAttributesDialog::slotApplyButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   VolumeFile* vf = getSelectedVolume(true);
   if (vf != NULL) {
      if (tabWidget->currentWidget() == alignmentPage) {
         const int ac[3] = {
            alignAcXSpinBox->value(),
            alignAcYSpinBox->value(),
            alignAcZSpinBox->value()
         };
         const int pc[3] = {
            alignPcXSpinBox->value(),
            alignPcYSpinBox->value(),
            alignPcZSpinBox->value()
         };
         const int lf[3] = {
            alignLfXSpinBox->value(),
            alignLfYSpinBox->value(),
            alignLfZSpinBox->value()
         };
         
         vf->acPcAlign(ac, pc, lf);
         
         GuiToolBar* toolbar = theMainWindow->getToolBar();
         if (toolbar != NULL) {
            toolbar->setViewSelection(BrainModel::VIEW_RESET);
         }
         GuiBrainModelOpenGL::updateAllGL();
      }
      else if (tabWidget->currentWidget() == coordinatesPage) {
         float origin[3] = {
            originXDoubleSpinBox->value(),
            originYDoubleSpinBox->value(),
            originZDoubleSpinBox->value()
         };
         float spacing[3] = {
            voxelSizeXDoubleSpinBox->value(),
            voxelSizeYDoubleSpinBox->value(),
            voxelSizeZDoubleSpinBox->value()
         };
         vf->setOrigin(origin);
         vf->setSpacing(spacing);
         GuiBrainModelOpenGL::updateAllGL();
      }
      else if (tabWidget->currentWidget() == orientationPage) {
         VolumeFile::ORIENTATION orientation[3];
         vf->getOrientation(orientation);
         bool changeMade = false;
         if (orientation[0] != xOrientationComboBox->getOrientation()) {
            orientation[0] = xOrientationComboBox->getOrientation();
            changeMade = true;
         }
         if (orientation[1] != yOrientationComboBox->getOrientation()) {
            orientation[1] = yOrientationComboBox->getOrientation();
            changeMade = true;
         }
         if (orientation[2] != zOrientationComboBox->getOrientation()) {
            orientation[2] = zOrientationComboBox->getOrientation();
            changeMade = true;
         }
         if (changeMade) {
            vf->setOrientation(orientation);
         }
      }
      else if (tabWidget->currentWidget() == resamplingPage) {
         resampleVolume();
      }
      else if (tabWidget->currentWidget() == dataPage) {
        /* to be done later */
      }
      
      loadVolumeParameters();
      loadVolumeResampling();
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * Called when a volume file is selected
 */
 void
 GuiVolumeAttributesDialog::slotVolumeFileSelected()
 {
    loadVolumeParameters();
    loadVolumeResampling();
 }
 
/**
 * Called when a volume type is selected
 */
 void
 GuiVolumeAttributesDialog::slotVolumeTypeSelected()
 {
    loadVolumeFileComboBox();
    loadVolumeParameters();
    loadVolumeResampling();
 }
 
/**
 * Get the selected volume.
 */
VolumeFile*
GuiVolumeAttributesDialog::getSelectedVolume(const bool requireVolumeInMainWindow)
{
   VolumeFile* vf = NULL;
   const int fileIndex = volumeFileComboBox->currentIndex();
   switch(static_cast<VolumeFile::VOLUME_TYPE>(volumeTypeComboBox->currentIndex())) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         vf = theMainWindow->getBrainSet()->getVolumeAnatomyFile(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         vf = theMainWindow->getBrainSet()->getVolumeFunctionalFile(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_PAINT:
         vf = theMainWindow->getBrainSet()->getVolumePaintFile(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         vf = theMainWindow->getBrainSet()->getVolumeProbAtlasFile(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_RGB:
         vf = theMainWindow->getBrainSet()->getVolumeRgbFile(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_ROI:
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION:
         vf = theMainWindow->getBrainSet()->getVolumeSegmentationFile(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_VECTOR:
         vf = theMainWindow->getBrainSet()->getVolumeVectorFile(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;
   }
   
   if (requireVolumeInMainWindow && (vf != NULL)) {
      bool inMainWindowFlag = false;
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         if ((vf == bmv->getUnderlayVolumeFile()) ||
             (vf == bmv->getOverlayPrimaryVolumeFile()) ||
             (vf == bmv->getOverlaySecondaryVolumeFile())) {
            inMainWindowFlag = true;
         }
      }
      
      if (inMainWindowFlag == false) {
         if (QMessageBox::question(this, "ERROR",
                "The selected volume is not displayed in the main window.\n"
                "Do you want to continue applying your changes?", 
                (QMessageBox::Yes | QMessageBox::No),
                QMessageBox::No)
                   == QMessageBox::No) {
            vf = NULL;
         }
      }
   }
   
   return vf;
}

/**
 * Load the volume parameters.
 */
void
GuiVolumeAttributesDialog::loadVolumeParameters()
{
   VolumeFile* vf = getSelectedVolume(false);
   if (vf != NULL) {
      int dim[3];
      vf->getDimensions(dim);
      dimensionXLabel->setText(QString::number(dim[0]));
      dimensionYLabel->setText(QString::number(dim[1]));
      dimensionZLabel->setText(QString::number(dim[2]));
      
      float spacing[3];
      vf->getSpacing(spacing);
      voxelSizeXDoubleSpinBox->setValue(spacing[0]);
      voxelSizeYDoubleSpinBox->setValue(spacing[1]);
      voxelSizeZDoubleSpinBox->setValue(spacing[2]);
      
      float origin[3];
      vf->getOrigin(origin);
      originXDoubleSpinBox->setValue(origin[0]);
      originYDoubleSpinBox->setValue(origin[1]);
      originZDoubleSpinBox->setValue(origin[2]);
      
      VolumeFile::ORIENTATION orientation[3];
      vf->getOrientation(orientation);
      xOrientationComboBox->setOrientation(orientation[0]);
      yOrientationComboBox->setOrientation(orientation[1]);
      zOrientationComboBox->setOrientation(orientation[2]);
   }
}

/**
 * load the volume file combo box
 */
void
GuiVolumeAttributesDialog::loadVolumeFileComboBox()
{
   const int currentItem = volumeFileComboBox->currentIndex();
   
   volumeFileComboBox->clear();
   
   switch(static_cast<VolumeFile::VOLUME_TYPE>(volumeTypeComboBox->currentIndex())) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles(); i++) {
            volumeFileComboBox->addItem(
                   theMainWindow->getBrainSet()->getVolumeAnatomyFile(i)->getDescriptiveLabel());
         }
         break;
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeFunctionalFiles(); i++) {
            volumeFileComboBox->addItem(
                   theMainWindow->getBrainSet()->getVolumeFunctionalFile(i)->getDescriptiveLabel());
         }
         break;
      case VolumeFile::VOLUME_TYPE_PAINT:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumePaintFiles(); i++) {
            volumeFileComboBox->addItem(
                   theMainWindow->getBrainSet()->getVolumePaintFile(i)->getDescriptiveLabel());
         }
         break;
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeProbAtlasFiles(); i++) {
            volumeFileComboBox->addItem(
                   theMainWindow->getBrainSet()->getVolumeProbAtlasFile(i)->getDescriptiveLabel());
         }
         break;
      case VolumeFile::VOLUME_TYPE_RGB:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeRgbFiles(); i++) {
            volumeFileComboBox->addItem(
                   theMainWindow->getBrainSet()->getVolumeRgbFile(i)->getDescriptiveLabel());
         }
         break;
      case VolumeFile::VOLUME_TYPE_ROI:
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles(); i++) {
            volumeFileComboBox->addItem(
                   theMainWindow->getBrainSet()->getVolumeSegmentationFile(i)->getDescriptiveLabel());
         }
         break;
      case VolumeFile::VOLUME_TYPE_VECTOR:
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfVolumeVectorFiles(); i++) {
            volumeFileComboBox->addItem(
                   theMainWindow->getBrainSet()->getVolumeVectorFile(i)->getDescriptiveLabel());
         }
         break;
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;
   }
   
   if ((currentItem >= 0) && (currentItem < volumeFileComboBox->count())) {
      volumeFileComboBox->setCurrentIndex(currentItem);
   }
}

/**
 * Called to update the dialog.
 */
void
GuiVolumeAttributesDialog::updateDialog()
{
   loadVolumeFileComboBox();
   loadVolumeParameters();
   loadVolumeResampling();
}


