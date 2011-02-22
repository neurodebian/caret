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

#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolTip>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceMetricSmoothing.h"
#include "BrainSet.h"
#include "GaussianComputation.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMetricModificationDialog.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "MetricFile.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "WuQWidgetGroup.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMetricModificationDialog::GuiMetricModificationDialog(QWidget* parent, 
                                                         const FILE_TYPE_MODE fileTypeModeIn)
   : WuQDialog(parent)
{
   fileTypeMode = fileTypeModeIn;
   
   switch (fileTypeMode) {
      case FILE_TYPE_MODE_METRIC:
         setWindowTitle("Metric Modification");
         break;
      case FILE_TYPE_MODE_SURFACE_SHAPE:
         setWindowTitle("Surface Shape Modification");
         break;
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);

   //
   // Create the surface part of the dialog
   //   
   QWidget* surfaceWidget = createSurfacePartOfDialog();
   dialogLayout->addWidget(surfaceWidget);
   
   //
   // Create the metric column part of the dialog
   //   
   QWidget* metricColumnWidget = createMetricColumnPartOfDialog();
   dialogLayout->addWidget(metricColumnWidget);
   
   //
   // Create the tab widget for the different modifications
   //
   modificationTypeTab = new QTabWidget(this);
   dialogLayout->addWidget(modificationTypeTab);
   
   //
   // Create the clustering part of the dialog
   //
   modificationTypeTab->addTab(createClusteringPartOfDialog(),
                               "Clustering");
   
   //
   // Create the smoothing part of the dialog
   //   
   modificationTypeTab->addTab(createSmoothingPartOfDialog(),
                               "Smoothing");
   
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
                    
   //
   // Help button
   //
   QPushButton* helpButton = new QPushButton("Help");
   helpButton->setAutoDefault(false);
   buttonsLayout->addWidget(helpButton);
   QObject::connect(helpButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpButton()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, closeButton, helpButton);
   
   //
   // Default to smoothing
   //
   modificationTypeTab->setCurrentIndex(modificationTypeTab->indexOf(smoothingWidget));
   
   slotInputMetricColumnComboBox();
   updateDialog();
}

/**
 * destructor.
 */
GuiMetricModificationDialog::~GuiMetricModificationDialog()
{
}

/**
 * called when Apply button is pressed.
 */
void 
GuiMetricModificationDialog::slotApplyButton()
{
   MetricFile* mf = getNodeDataFile();
   if (mf == NULL) {
      QMessageBox::critical(this, "ERROR",
                            "PROGRAM ERROR: metric/shape file invalid (NULL).");
      return;
   }
   
   if (mf->getNumberOfColumns() <= 0) {
      QMessageBox::critical(this, "ERROR",
                            "The file is empty.");
      return;
   }
   
   BrainModelSurface* bms = surfaceSelectionComboBox->getSelectedBrainModelSurface();
   if (bms == NULL) {
      QMessageBox::critical(this, "ERROR",
                            "No surface is selected.");
      return;
   }
   
   const int inputColumn = inputMetricColumnComboBox->currentIndex();
   
   int outputColumn = outputMetricColumnComboBox->currentIndex();
   if (outputColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
      outputColumn = -1;
   }
   const QString columnName = outputMetricNameLineEdit->text();
   if (outputColumn < 0) {
      if (columnName.isEmpty()) {
         if (inputColumn != GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL) {
            QMessageBox::critical(this, "ERROR",
                                 "Enter an output column name.");
            return;
         }
      }
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   QString talkingMessage;
   
   //
   // Create a coordinate file that contains the node normals
   //
   const int numNodes = bms->getNumberOfNodes();
   CoordinateFile nodeNormals;
   nodeNormals.setNumberOfCoordinates(numNodes);
   for (int m = 0; m < numNodes; m++) {
      nodeNormals.setCoordinate(m, bms->getNormal(m));
   }
         
   if (modificationTypeTab->currentWidget() == smoothingWidget) {
      const BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM algorithm =
         static_cast<BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM>(
                                       smoothingAlgorithmComboBox->currentIndex());
      if (inputColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL) {
         //
         //
         // With all selection all columns are smoothed and replaced
         //
         const int numColumns = mf->getNumberOfColumns();
         for (int i = 0; i < numColumns; i++) {
            BrainModelSurfaceMetricSmoothing bmsms(
                  theMainWindow->getBrainSet(),
                  bms,
                  gaussSphericalSurfaceComboBox->getSelectedBrainModelSurface(),
                  mf,
                  algorithm,
                  i,
                  i,
                  mf->getColumnName(i),
                  strengthDoubleSpinBox->value(),
                  iterationsSpinBox->value(),
                  fullWidthHalfMaximumDoubleSpinBox->value(),
                  gaussSmoothNormBelowDoubleSpinBox->value(),
                  gaussSmoothNormAboveDoubleSpinBox->value(),
                  gaussSmoothSigmaNormDoubleSpinBox->value(),
                  gaussSmoothSigmaTangDoubleSpinBox->value(),
                  gaussSmoothTangentDoubleSpinBox->value()); 
            try {
               //QTime timer;
               //timer.start();
               bmsms.execute();
               //std::cout << "Metric smoothing time: "
               //          << (timer.elapsed() / 1000.0)
               //          << std::endl;
            }
            catch (BrainModelAlgorithmException& e) {
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "ERROR", e.whatQString());
               return;
            }   
         }
      }
      else {
         BrainModelSurfaceMetricSmoothing bmsms(
               theMainWindow->getBrainSet(),
               bms,
               gaussSphericalSurfaceComboBox->getSelectedBrainModelSurface(),
               mf,
               algorithm,
               inputColumn,
               outputColumn,
               columnName,
               strengthDoubleSpinBox->value(),
               iterationsSpinBox->value(),
               fullWidthHalfMaximumDoubleSpinBox->value(),
               gaussSmoothNormBelowDoubleSpinBox->value(),
               gaussSmoothNormAboveDoubleSpinBox->value(),
               gaussSmoothSigmaNormDoubleSpinBox->value(),
               gaussSmoothSigmaTangDoubleSpinBox->value(),
               gaussSmoothTangentDoubleSpinBox->value()); 
         try {
            
            QTime timer;
            timer.start();
            bmsms.execute();
               std::cout << "Metric smoothing time: "
                         << (timer.elapsed() / 1000.0)
                         << std::endl;
               
            switch (algorithm) {
               case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
                  break;
               case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_DILATE:
                  break;
               case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM:
                  QApplication::restoreOverrideCursor();
                  QMessageBox::information(this, 
                                             "INFO", 
                                             bmsms.getFullWidthHalfMaximumSmoothingResultsDescription());
                  break;
               case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
                  break;
               case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
                  break;
               case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_NONE:
                  break;
            }                  
         }
         catch (BrainModelAlgorithmException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "ERROR", e.whatQString());
            return;
         }   
      }
      talkingMessage = "Smoothing has completed.";
   }
   else {
      if (inputColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL) {
         QApplication::restoreOverrideCursor();
         QMessageBox::warning(this, "Error", 
            "Clustering on all metric columns not supported at this time.");
         return;
      }
      
      //
      // Get the selected algorithm
      //
      BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM algorithm =
         BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_NONE;
      if (clusterSizeAnyRadioButton->isChecked()) {
         algorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_ANY_SIZE;
      }
      else if (clusterSizeNumberOfNodesRadioButton->isChecked()) {
         algorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES;
      }
      else if (clusterSizeMinimumSurfaceAreaRadioButton->isChecked()) {
         algorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA;
      }
      else {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Clustering Error", 
                                 "A clustering minimum size algorithm must be selected.");
      }
      
      //
      // Get the other parameters
      //
      const int minNumNodes = clusterSizeNumberOfNodesLineEdit->text().toInt();
      const float minSurfaceArea = clusterSizeSurfaceAreaLineEdit->text().toFloat();
      const float clusterNegMinThresh = clusterThresholdNegMinLineEdit->text().toFloat();
      const float clusterNegMaxThresh = clusterThresholdNegMaxLineEdit->text().toFloat();
      const float clusterPosMinThresh = clusterThresholdPosMinLineEdit->text().toFloat();
      const float clusterPosMaxThresh = clusterThresholdPosMaxLineEdit->text().toFloat();
      
      if (inputColumn == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL) {
         //
         // With all selection all columns are smoothed and replaced
         //
         const int numColumns = mf->getNumberOfColumns();
         for (int i = 0; i < numColumns; i++) {
            //
            // Perform clustering
            //
            BrainModelSurfaceMetricClustering bmsmc(theMainWindow->getBrainSet(),
                                                   bms,
                                                   mf,
                                                   algorithm,
                                                   i,
                                                   i,
                                                   mf->getColumnName(i),
                                                   minNumNodes,
                                                   minSurfaceArea,
                                                   clusterNegMinThresh,
                                                   clusterNegMaxThresh,
                                                   clusterPosMinThresh,
                                                   clusterPosMaxThresh,
                                                   true);
            try {
               bmsmc.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "Clustering Error", e.whatQString());
               return;
            }
         }
      }
      else {
         //
         // Perform clustering
         //
         BrainModelSurfaceMetricClustering bmsmc(theMainWindow->getBrainSet(),
                                                bms,
                                                mf,
                                                algorithm,
                                                inputColumn,
                                                outputColumn,
                                                columnName,
                                                minNumNodes,
                                                minSurfaceArea,
                                                clusterNegMinThresh,
                                                clusterNegMaxThresh,
                                                clusterPosMinThresh,
                                                clusterPosMaxThresh,
                                                true);
         try {
            bmsmc.execute();
         }
         catch (BrainModelAlgorithmException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Clustering Error", e.whatQString());
            return;
         }
      }
      talkingMessage = "Clustering has completed.";
   }
   
   //
   // Metric or shape file probably has changed
   //
   switch (fileTypeMode) {
      case FILE_TYPE_MODE_METRIC:
         {
            GuiFilesModified fm;
            fm.setMetricModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         break;
      case FILE_TYPE_MODE_SURFACE_SHAPE:
         {
            GuiFilesModified fm;
            fm.setSurfaceShapeModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         break;
   }
   
   //
   // Update output column
   //
   if (outputColumn < 0) {
      outputMetricColumnComboBox->setCurrentIndex(mf->getNumberOfColumns() - 1);
      slotOutputMetricColumnComboBox();
   }
   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
   
   theMainWindow->speakText(talkingMessage, false);
}

/**
 * called when help button is pressed.
 */
void 
GuiMetricModificationDialog::slotHelpButton()
{
theMainWindow->showHelpViewerDialog("dialogs/metric_smoothing_dialog.html");}

/**
 * create the surface part of the dialog.
 */
QWidget* 
GuiMetricModificationDialog::createSurfacePartOfDialog()
{
   surfaceSelectionComboBox = new GuiBrainModelSelectionComboBox(
                                       false,
                                       true,
                                       false,
                                       "",
                                       0);
                                       
   QGroupBox* surfaceGroup = new QGroupBox("Surface");
   QVBoxLayout* surfaceLayout = new QVBoxLayout(surfaceGroup);
   surfaceLayout->addWidget(surfaceSelectionComboBox);   
   return surfaceGroup;
}

/**
 * create the metric column part of the dialog.
 */
QWidget* 
GuiMetricModificationDialog::createMetricColumnPartOfDialog()
{
   QString title;
   GUI_NODE_FILE_TYPE nodeFileType = GUI_NODE_FILE_TYPE_NONE;
   switch (fileTypeMode) {
      case FILE_TYPE_MODE_METRIC:
         title = "Metric Column";
         nodeFileType = GUI_NODE_FILE_TYPE_METRIC;
         break;
      case FILE_TYPE_MODE_SURFACE_SHAPE:
         title = "Surface Shape Column";
         nodeFileType = GUI_NODE_FILE_TYPE_SURFACE_SHAPE;
         break;
   }
   //
   // input metric column
   //
   QLabel* inputLabel = new QLabel("Input");
   inputMetricColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                        nodeFileType,
                                        false,
                                        false,
                                        true);
   QObject::connect(inputMetricColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotInputMetricColumnComboBox()));
   
   //
   // input metric column
   //
   QLabel* outputLabel = new QLabel("Output");
   outputMetricColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                        nodeFileType,
                                        true,
                                        false,
                                        false);
   QObject::connect(outputMetricColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotOutputMetricColumnComboBox()));
   outputMetricNameLineEdit = new QLineEdit;

   //
   // Group box and arrange the widgets
   //
   QGroupBox* columnGroup = new QGroupBox(title);
   QGridLayout* columnLayout = new QGridLayout(columnGroup);
   columnLayout->addWidget(inputLabel, 0, 0);
   columnLayout->addWidget(inputMetricColumnComboBox, 0, 1);
   columnLayout->addWidget(outputLabel, 1, 0);
   columnLayout->addWidget(outputMetricColumnComboBox, 1, 1);
   columnLayout->addWidget(outputMetricNameLineEdit, 1, 2);
   
   return columnGroup;
}

/**
 * Called when input metric column selected.
 */
void
GuiMetricModificationDialog::slotInputMetricColumnComboBox()
{
   const bool enableOutputs = (inputMetricColumnComboBox->currentIndex() != 
                               GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL);
   outputMetricColumnComboBox->setEnabled(enableOutputs);
   outputMetricNameLineEdit->setEnabled(enableOutputs);
   outputMetricColumnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
   slotOutputMetricColumnComboBox();
}

/**
 * Called when output metric column selected.
 */
void
GuiMetricModificationDialog::slotOutputMetricColumnComboBox()
{
   MetricFile* mf = NULL;
   switch (fileTypeMode) {
      case FILE_TYPE_MODE_METRIC:
         mf = theMainWindow->getBrainSet()->getMetricFile();
         break;
      case FILE_TYPE_MODE_SURFACE_SHAPE:
         mf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
         break;
   }
   const int metricColumn = outputMetricColumnComboBox->currentIndex();
   if ((metricColumn >= 0) && (metricColumn < mf->getNumberOfColumns())) {
      outputMetricNameLineEdit->setText(mf->getColumnName(metricColumn));
   }
   else {
      outputMetricNameLineEdit->setText("New Column Name");
   }
}

/**
 * create the smoothing part of the dialog.
 */
QWidget* 
GuiMetricModificationDialog::createClusteringPartOfDialog()
{
   //
   // Minimum size Any 
   //
   clusterSizeAnyRadioButton = new QRadioButton("Any Size");
     
   //
   // Minimum number of nodes 
   //
   clusterSizeNumberOfNodesRadioButton = new QRadioButton("Minimum Number of Nodes");
   clusterSizeNumberOfNodesLineEdit = new QLineEdit;
   clusterSizeNumberOfNodesLineEdit->setText("100");
     
   //
   // Minimum surface area 
   //
   clusterSizeMinimumSurfaceAreaRadioButton = new QRadioButton("Minimum Surface Area (mm)");
   clusterSizeSurfaceAreaLineEdit = new QLineEdit;
   clusterSizeSurfaceAreaLineEdit->setText("100");
     
   //
   // Box for cluster minimum size
   //
   QGroupBox* clusterSizeGroupBox = new QGroupBox("Cluster Minimum Size");
   QGridLayout* clusterSizeLayout = new QGridLayout(clusterSizeGroupBox);
   clusterSizeLayout->addWidget(clusterSizeAnyRadioButton, 0, 0);
   clusterSizeLayout->addWidget(clusterSizeNumberOfNodesRadioButton, 1, 0);
   clusterSizeLayout->addWidget(clusterSizeNumberOfNodesLineEdit, 1, 1);
   clusterSizeLayout->addWidget(clusterSizeMinimumSurfaceAreaRadioButton, 2, 0);
   clusterSizeLayout->addWidget(clusterSizeSurfaceAreaLineEdit, 2, 1);
   
   //
   // Button group to keep cluster size radio buttons mutually exclusive
   //
   QButtonGroup* clusterSizeButtonGroup = new QButtonGroup(this);
   clusterSizeButtonGroup->addButton(clusterSizeAnyRadioButton, 0);
   clusterSizeButtonGroup->addButton(clusterSizeNumberOfNodesRadioButton, 1);
   clusterSizeButtonGroup->addButton(clusterSizeMinimumSurfaceAreaRadioButton, 2);
   
   //
   // positive minimum and maximum values
   //
   QLabel* posMinLabel = new QLabel("Positive Minimum");
   clusterThresholdPosMinLineEdit = new QLineEdit;
   clusterThresholdPosMinLineEdit->setText("0.00001");
   QLabel* posMaxLabel = new QLabel("Positive Maximum");
   clusterThresholdPosMaxLineEdit = new QLineEdit;
   clusterThresholdPosMaxLineEdit->setText("1000000.0");

   //
   // negative minimum and maximum values
   //
   QLabel* negMinLabel = new QLabel("Negative Minimum");
   clusterThresholdNegMinLineEdit = new QLineEdit;
   clusterThresholdNegMinLineEdit->setText("-0.00001");
   QLabel* negMaxLabel = new QLabel("Negative Maximum");
   clusterThresholdNegMaxLineEdit = new QLineEdit;
   clusterThresholdNegMaxLineEdit->setText("-1000000.0");

   //
   // Box/Layout for cluster value threshold 
   //
   QGroupBox* clusterThresholdGroupBox = new QGroupBox("Cluster Value Threshold");
   QGridLayout* clusterThresholdLayout = new QGridLayout(clusterThresholdGroupBox);
   clusterThresholdLayout->addWidget(posMinLabel);
   clusterThresholdLayout->addWidget(clusterThresholdPosMinLineEdit);
   clusterThresholdLayout->addWidget(posMaxLabel);
   clusterThresholdLayout->addWidget(clusterThresholdPosMaxLineEdit);
   clusterThresholdLayout->addWidget(negMinLabel);
   clusterThresholdLayout->addWidget(clusterThresholdNegMinLineEdit);
   clusterThresholdLayout->addWidget(negMaxLabel);
   clusterThresholdLayout->addWidget(clusterThresholdNegMaxLineEdit);
   
   clusteringWidget = new QWidget;
   QVBoxLayout* clusteringLayout = new QVBoxLayout(clusteringWidget);
   clusteringLayout->addWidget(clusterSizeGroupBox);
   clusteringLayout->addWidget(clusterThresholdGroupBox);
   return clusteringWidget;
}

/**
 * Called when an algorithm is selected.
 */
void
GuiMetricModificationDialog::slotSmoothingAlgorithmComboBox(int item)
{
   bool enableFwhmParams = false;
   bool enableGaussParams = false;
   bool enableStrengthParams = true;
   
   switch (static_cast<BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM>(item)) {
      case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS:
         break;
      case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_DILATE:
         enableStrengthParams = false;
         break;
      case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM:
         enableFwhmParams = true;
         enableStrengthParams = false;
         break;
      case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN:
         enableGaussParams = true;
         break;
      case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS:
         break;
      case BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_NONE:
         break;
   }   

   fullWidthHalfMaximumGroupBox->setEnabled(enableFwhmParams);
   gaussSmoothParametersGroupBox->setEnabled(enableGaussParams);
   gaussSurfaceGroupBox->setEnabled(enableGaussParams);
   strengthWidgetGroup->setEnabled(enableStrengthParams);
}

/**
 * create the smoothing part of the dialog.
 */
QWidget* 
GuiMetricModificationDialog::createSmoothingPartOfDialog()
{
   //
   // Gaussian spherical surface combo box
   //
   gaussSphericalSurfaceComboBox = new GuiBrainModelSelectionComboBox(
                                       false,
                                       true,
                                       false,
                                       "",
                                       0,
                                       "surfaceSelectionComboBox");
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      const BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
            gaussSphericalSurfaceComboBox->setSelectedBrainModel(bms);
            break;
         }
      }
   }

   //
   // Gaussian spherical surface group box
   //
   gaussSurfaceGroupBox = new QGroupBox("Gaussian Spherical Surface");
   QHBoxLayout* gaussSurfaceLayout = new QHBoxLayout(gaussSurfaceGroupBox);   
   gaussSurfaceLayout->addWidget(gaussSphericalSurfaceComboBox);
   gaussSurfaceGroupBox->setFixedHeight(gaussSurfaceGroupBox->sizeHint().height());
              
   //
   // Algorithm
   //
   QLabel* algorithmLabel = new QLabel("Algorithm");
   smoothingAlgorithmComboBox = new QComboBox;
   smoothingAlgorithmComboBox->insertItem(BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS,
                                          "Average Neighbors");
   smoothingAlgorithmComboBox->insertItem(BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_DILATE,
                                          "Dilation");
   smoothingAlgorithmComboBox->insertItem(BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM,
                                          "Full Width Half Maximum");
   smoothingAlgorithmComboBox->insertItem(BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN,
                                          "Gaussian");
   smoothingAlgorithmComboBox->insertItem(BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS,
                                          "Weighted Average Neighbors");
   QObject::connect(smoothingAlgorithmComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSmoothingAlgorithmComboBox(int)));
                    
   //
   // Smoothing iterations
   //
   QLabel* iterationsLabel = new QLabel("Iterations");
   iterationsSpinBox = new QSpinBox;
   iterationsSpinBox->setMinimum(1);
   iterationsSpinBox->setMaximum(100000);
   iterationsSpinBox->setSingleStep(5);
   iterationsSpinBox->setValue(10);
   
   //
   // Smoothing strength
   //
   QLabel* strengthLabel = new QLabel("Strength");
   strengthDoubleSpinBox = new QDoubleSpinBox;
   strengthDoubleSpinBox->setMinimum(0.0);
   strengthDoubleSpinBox->setMaximum(1.0);
   strengthDoubleSpinBox->setSingleStep(0.1);
   strengthDoubleSpinBox->setDecimals(2);
   strengthDoubleSpinBox->setValue(1.0);
   
   //
   // Widget group for strength
   //
   strengthWidgetGroup = new WuQWidgetGroup(this);
   strengthWidgetGroup->addWidget(strengthLabel);
   strengthWidgetGroup->addWidget(strengthDoubleSpinBox);
   
   //
   // Widget/Layout for Smoothing Parameters
   //
   QGroupBox* smoothingGroupBox = new QGroupBox("Smoothing Parameters");
   QGridLayout* smoothingGroupLayout = new QGridLayout(smoothingGroupBox);
   smoothingGroupLayout->addWidget(algorithmLabel, 0, 0);
   smoothingGroupLayout->addWidget(smoothingAlgorithmComboBox, 0, 1);
   smoothingGroupLayout->addWidget(iterationsLabel, 1, 0);
   smoothingGroupLayout->addWidget(iterationsSpinBox, 1, 1);
   smoothingGroupLayout->addWidget(strengthLabel, 2, 0);
   smoothingGroupLayout->addWidget(strengthDoubleSpinBox, 2, 1);
   smoothingGroupBox->setFixedSize(smoothingGroupBox->sizeHint());
   
   //
   // Get the default values
   //
   float normAbove, normBelow, tang, sigmaNorm, sigmaTang;
   GaussianComputation::getDefaultParameters(normBelow,
                                             normAbove,
                                             sigmaNorm,
                                             sigmaTang,
                                             tang);
                                             
   //
   // label and float spin box for 
   //
   QLabel* sigmaNormalLabel = new QLabel("Sigma Normal");
   gaussSmoothSigmaNormDoubleSpinBox = new QDoubleSpinBox;
   gaussSmoothSigmaNormDoubleSpinBox->setMinimum(0.0);
   gaussSmoothSigmaNormDoubleSpinBox->setMaximum(10000.0);
   gaussSmoothSigmaNormDoubleSpinBox->setSingleStep(1.0);
   gaussSmoothSigmaNormDoubleSpinBox->setDecimals(2);
   gaussSmoothSigmaNormDoubleSpinBox->setValue(sigmaNorm);
   gaussSmoothSigmaNormDoubleSpinBox->setToolTip(
                 GaussianComputation::tooltipTextForSigmaNorm());
                 
   //
   // label and float spin box for 
   //
   QLabel* sigmaTangentLabel = new QLabel("Sigma Tangent");
   gaussSmoothSigmaTangDoubleSpinBox = new QDoubleSpinBox;
   gaussSmoothSigmaTangDoubleSpinBox->setMinimum(0.0);
   gaussSmoothSigmaTangDoubleSpinBox->setMaximum(10000.0);
   gaussSmoothSigmaTangDoubleSpinBox->setSingleStep(1.0);
   gaussSmoothSigmaTangDoubleSpinBox->setDecimals(2);
   gaussSmoothSigmaTangDoubleSpinBox->setValue(sigmaTang);
   gaussSmoothSigmaTangDoubleSpinBox->setToolTip(
                 GaussianComputation::tooltipTextForSigmaTang());
                 
   //
   // label and float spin box for 
   //
   QLabel* normBelowLabel = new QLabel("Normal Below Cutoff");
   gaussSmoothNormBelowDoubleSpinBox = new QDoubleSpinBox;
   gaussSmoothNormBelowDoubleSpinBox->setMinimum(0.0);
   gaussSmoothNormBelowDoubleSpinBox->setMaximum(10000.0);
   gaussSmoothNormBelowDoubleSpinBox->setSingleStep(1.0);
   gaussSmoothNormBelowDoubleSpinBox->setDecimals(2);
   gaussSmoothNormBelowDoubleSpinBox->setValue(normBelow);
   gaussSmoothNormBelowDoubleSpinBox->setToolTip(
                 GaussianComputation::tooltipTextForNormBelowCutoff());
                 
   //
   // label and float spin box for 
   //
   QLabel* normAboveLabel = new QLabel("Normal Above Cutoff");
   gaussSmoothNormAboveDoubleSpinBox = new QDoubleSpinBox;
   gaussSmoothNormAboveDoubleSpinBox->setMinimum(0.0);
   gaussSmoothNormAboveDoubleSpinBox->setMaximum(10000.0);
   gaussSmoothNormAboveDoubleSpinBox->setSingleStep(1.0);
   gaussSmoothNormAboveDoubleSpinBox->setDecimals(2);
   gaussSmoothNormAboveDoubleSpinBox->setValue(normAbove);
   gaussSmoothNormAboveDoubleSpinBox->setToolTip(
                 GaussianComputation::tooltipTextForNormAboveCutoff());
                 
   //
   // label and float spin box for 
   //
   QLabel* tangentCutoffLabel = new QLabel("Tangent Cutoff");
   gaussSmoothTangentDoubleSpinBox = new QDoubleSpinBox;
   gaussSmoothTangentDoubleSpinBox->setMinimum(0.0);
   gaussSmoothTangentDoubleSpinBox->setMaximum(10000.0);
   gaussSmoothTangentDoubleSpinBox->setSingleStep(1.0);
   gaussSmoothTangentDoubleSpinBox->setDecimals(2);
   gaussSmoothTangentDoubleSpinBox->setValue(tang);
   gaussSmoothTangentDoubleSpinBox->setToolTip(
                 GaussianComputation::tooltipTextForTangentCutoff());
   
   //
   // Gaussian parameters grid
   //
   gaussSmoothParametersGroupBox = new QGroupBox("Gaussian Parameters");
   QGridLayout* gaussSmoothGridLayout = new QGridLayout(gaussSmoothParametersGroupBox);
   gaussSmoothGridLayout->addWidget(sigmaNormalLabel, 0, 0);
   gaussSmoothGridLayout->addWidget(gaussSmoothSigmaNormDoubleSpinBox, 0, 1);
   gaussSmoothGridLayout->addWidget(sigmaTangentLabel, 1, 0);
   gaussSmoothGridLayout->addWidget(gaussSmoothSigmaTangDoubleSpinBox, 1, 1);
   gaussSmoothGridLayout->addWidget(normBelowLabel, 2, 0);
   gaussSmoothGridLayout->addWidget(gaussSmoothNormBelowDoubleSpinBox, 2, 1);
   gaussSmoothGridLayout->addWidget(normAboveLabel, 3, 0);
   gaussSmoothGridLayout->addWidget(gaussSmoothNormAboveDoubleSpinBox, 3, 1);
   gaussSmoothGridLayout->addWidget(tangentCutoffLabel, 4, 0);
   gaussSmoothGridLayout->addWidget(gaussSmoothTangentDoubleSpinBox, 4, 1);
   gaussSmoothParametersGroupBox->setFixedSize(gaussSmoothParametersGroupBox->sizeHint());
   
   //
   // Full Width Half Maximum Parameters and GroupBox
   //
   QLabel* fullWidthHalfMaximumLabel = new QLabel("Desired FWHM");
   fullWidthHalfMaximumDoubleSpinBox = new QDoubleSpinBox;
   fullWidthHalfMaximumDoubleSpinBox->setValue(8.0);
   fullWidthHalfMaximumGroupBox = new QGroupBox("Full Width Half Maximum Parameter");
   QGridLayout* fullWidthHalfMaximumGroupLayout = new QGridLayout(fullWidthHalfMaximumGroupBox);
   fullWidthHalfMaximumGroupLayout->addWidget(fullWidthHalfMaximumLabel, 0, 0);
   fullWidthHalfMaximumGroupLayout->addWidget(fullWidthHalfMaximumDoubleSpinBox, 0, 1);
   fullWidthHalfMaximumGroupBox->setFixedSize(fullWidthHalfMaximumGroupBox->sizeHint());
   
   //
   // Enable/disable gaussian parameters
   //
   slotSmoothingAlgorithmComboBox(smoothingAlgorithmComboBox->currentIndex());
   
   //
   // Holds parameters
   //
   QVBoxLayout* leftColumnLayout = new QVBoxLayout;
   leftColumnLayout->addWidget(smoothingGroupBox);
   leftColumnLayout->addWidget(fullWidthHalfMaximumGroupBox);
   QVBoxLayout* rightColumnLayout = new QVBoxLayout;
   rightColumnLayout->addWidget(gaussSmoothParametersGroupBox);
   QHBoxLayout* smoothParamsLayout = new QHBoxLayout;
   smoothParamsLayout->addLayout(leftColumnLayout);
   smoothParamsLayout->addLayout(rightColumnLayout);
   
   //
   // Widget/Layout for this section
   //
   smoothingWidget = new QWidget; // prevents background showing thru
   QVBoxLayout* smoothingLayout = new QVBoxLayout(smoothingWidget);
   smoothingLayout->addWidget(gaussSurfaceGroupBox);
   smoothingLayout->addLayout(smoothParamsLayout);   
   return smoothingWidget;
}

/**
 * update the dialog.
 */
void 
GuiMetricModificationDialog::updateDialog()
{
   surfaceSelectionComboBox->updateComboBox();
   gaussSphericalSurfaceComboBox->updateComboBox();
   inputMetricColumnComboBox->updateComboBox(); 
   outputMetricColumnComboBox->updateComboBox();
   slotOutputMetricColumnComboBox();
}
           
/**
 * get the data file since.
 */
MetricFile* 
GuiMetricModificationDialog::getNodeDataFile()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   MetricFile* mf = NULL;
   switch (fileTypeMode) {
      case FILE_TYPE_MODE_METRIC:
         mf = bs->getMetricFile();
         break;
      case FILE_TYPE_MODE_SURFACE_SHAPE:
         mf = bs->getSurfaceShapeFile();
         break;
   }
   return mf;
}
      
