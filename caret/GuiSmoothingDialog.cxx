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
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolTip>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceSmoothing.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiSmoothingDialog.h"
#include "PaintFile.h"
#include "PreferencesFile.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiSmoothingDialog::GuiSmoothingDialog(QWidget* parent, 
                                       const bool modal,
                                       const bool showOtherSmoothingOptions,
                                       std::vector<bool>* smoothOnlyTheseNodesIn)
   : QtDialog(parent, modal)
{
   setWindowTitle("Smoothing");
   
   smoothOnlyTheseNodes = smoothOnlyTheseNodesIn;
   
   smoothingMode = MODE_NOT_SELECTED;

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Areal smoothing radio button
   //
   arealSmoothingRadioButton = new QRadioButton("Normal Smoothing");
                                                
   //
   // Smooth Out Crossovers
   //
   crossoverSmoothingRadioButton = NULL;
   if (showOtherSmoothingOptions) {
      crossoverSmoothingRadioButton = new QRadioButton("Crossover Smoothing");
   }
   
   //
   // curvature smoothing
   //
   curvatureSmoothingRadioButton = NULL;
   if (showOtherSmoothingOptions) {
      curvatureSmoothingRadioButton = new QRadioButton("Curvature Smoothing");
   }
   
   //
   // Smooth flat surface overlap
   //
   flatOverlapSmoothingRadioButton = NULL;
   if (showOtherSmoothingOptions) {
      flatOverlapSmoothingRadioButton = new QRadioButton("Flat Surface Overlap Smoothing");
   }
   
   //
   // landmark constrained smoothing radio button
   //
   landmarkSmoothingRadioButton = NULL;
   if (showOtherSmoothingOptions) {
      landmarkSmoothingRadioButton = new QRadioButton("Landmark Constrained Smoothing");
   }
    
   //
   // landmark neighbor constrained smoothing radio button
   //
   landmarkNeighborSmoothingRadioButton = NULL;
   if (showOtherSmoothingOptions) {
      landmarkNeighborSmoothingRadioButton = new QRadioButton("Landmark Neighbor Constrained Smoothing");
   }
    
   //
   // linear smoothing radio button
   //
   linearSmoothingRadioButton = NULL;
//   linearSmoothingRadioButton = new QRadioButton("Linear Smoothing");
    
   //
   // Group Box and layout for radio buttons
   //
   QGroupBox* typeGroupBox = new QGroupBox("Smoothing Type");
   QVBoxLayout* typeLayout = new QVBoxLayout(typeGroupBox);
   typeLayout->addWidget(arealSmoothingRadioButton);
   if (crossoverSmoothingRadioButton != NULL) {
      typeLayout->addWidget(crossoverSmoothingRadioButton);
   }
   if (curvatureSmoothingRadioButton != NULL) {
      typeLayout->addWidget(curvatureSmoothingRadioButton);
   }
   if (flatOverlapSmoothingRadioButton != NULL) {
      typeLayout->addWidget(flatOverlapSmoothingRadioButton);
   }
   if (landmarkSmoothingRadioButton != NULL) {
      typeLayout->addWidget(landmarkSmoothingRadioButton);
   }
   if (landmarkNeighborSmoothingRadioButton != NULL) {
      typeLayout->addWidget(landmarkNeighborSmoothingRadioButton);
   }
   if (linearSmoothingRadioButton != NULL) {
      typeLayout->addWidget(linearSmoothingRadioButton);
   }
   dialogLayout->addWidget(typeGroupBox);

   //
   // Button group to keep radio buttons mutually exclusive
   //   
   QButtonGroup* typeButtonGroup = new QButtonGroup(this);
   typeButtonGroup->addButton(arealSmoothingRadioButton, 0);
   if (crossoverSmoothingRadioButton != NULL) {
      typeButtonGroup->addButton(crossoverSmoothingRadioButton, 1);
   }
   if (curvatureSmoothingRadioButton != NULL) {
      typeButtonGroup->addButton(curvatureSmoothingRadioButton, 2);
   }
   if (flatOverlapSmoothingRadioButton != NULL) {
      typeButtonGroup->addButton(flatOverlapSmoothingRadioButton, 3);
   }
   if (landmarkSmoothingRadioButton != NULL) {
      typeButtonGroup->addButton(landmarkSmoothingRadioButton, 4);
   }
   if (landmarkNeighborSmoothingRadioButton != NULL) {
      typeButtonGroup->addButton(landmarkNeighborSmoothingRadioButton, 5);
   }
   if (linearSmoothingRadioButton != NULL) {
      typeButtonGroup->addButton(linearSmoothingRadioButton, 6);
   }
   QObject::connect(typeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotSmoothingType()));
                    
    
    
   //
   // Smoothing strength
   //
   QLabel* strengthLabel = new QLabel("Strength");
   strengthSpinBox = new QDoubleSpinBox;
   strengthSpinBox->setMinimum(0.0);
   strengthSpinBox->setMaximum(1.0);
   strengthSpinBox->setSingleStep(0.01);
   strengthSpinBox->setDecimals(2);
   strengthSpinBox->setValue(1.0);
   
   //
   // Iterations spin box
   //
   QLabel* iterationsLabel = new QLabel("Iterations");
   iterationsSpinBox = new QSpinBox;
   iterationsSpinBox->setMinimum(1);
   iterationsSpinBox->setMaximum(10000);
   iterationsSpinBox->setSingleStep(10);
   iterationsSpinBox->setValue(50);
   
   //
   // Edge Iterations spin box
   //
   edgeIterationsLabel = new QLabel("Edge Iterations Every X");
   edgeIterationsSpinBox = new QSpinBox;
   edgeIterationsSpinBox->setMinimum(0);
   edgeIterationsSpinBox->setMaximum(10000);
   edgeIterationsSpinBox->setSingleStep(1);
   edgeIterationsSpinBox->setValue(10);
   
   //
   // Cycles spin box
   //
   numberOfCyclesLabel = new QLabel("Number of Cycles");
   numberOfCyclesSpinBox = new QSpinBox;
   numberOfCyclesSpinBox->setMinimum(1);
   numberOfCyclesSpinBox->setMaximum(1000);
   numberOfCyclesSpinBox->setSingleStep(1);
   numberOfCyclesSpinBox->setValue(5);
   
   //
   // Depth spin box
   //
   depthLabel = new QLabel("Node Depth");
   depthSpinBox = new QSpinBox;
   depthSpinBox->setMinimum(0);
   depthSpinBox->setMaximum(50);
   depthSpinBox->setSingleStep(1);
   depthSpinBox->setValue(5);
   
   //
   // Curvature maximum float spin box
   //
   curvatureMaximumLabel = NULL;
   curvatureMaximumDoubleSpinBox = NULL;
   if (showOtherSmoothingOptions) {
      curvatureMaximumLabel = new QLabel("Max Curvature");
      curvatureMaximumDoubleSpinBox = new QDoubleSpinBox;
      curvatureMaximumDoubleSpinBox->setMinimum(0.0);
      curvatureMaximumDoubleSpinBox->setMaximum(10000.0);
      curvatureMaximumDoubleSpinBox->setSingleStep(0.25);
      curvatureMaximumDoubleSpinBox->setDecimals(4);
      curvatureMaximumDoubleSpinBox->setValue(0.25);
   }
   
   //
   // Group box for parameters
   //
   QGroupBox* parametersGroupBox = new QGroupBox("Smoothing Parameters");
   dialogLayout->addWidget(parametersGroupBox);
   QGridLayout* paramsGridLayout = new QGridLayout(parametersGroupBox);
   int rowNum = 0;
   paramsGridLayout->addWidget(strengthLabel, rowNum, 0);
   paramsGridLayout->addWidget(strengthSpinBox, rowNum, 1);
   rowNum++;
   paramsGridLayout->addWidget(iterationsLabel, rowNum, 0);
   paramsGridLayout->addWidget(iterationsSpinBox, rowNum, 1);
   rowNum++;
   paramsGridLayout->addWidget(edgeIterationsLabel, rowNum, 0);
   paramsGridLayout->addWidget(edgeIterationsSpinBox, rowNum, 1);
   rowNum++;
   paramsGridLayout->addWidget(numberOfCyclesLabel, rowNum, 0);
   paramsGridLayout->addWidget(numberOfCyclesSpinBox, rowNum, 1);
   rowNum++;
   paramsGridLayout->addWidget(depthLabel, rowNum, 0);
   paramsGridLayout->addWidget(depthSpinBox, rowNum, 1);
   rowNum++;
   if (curvatureMaximumLabel != NULL) {
      paramsGridLayout->addWidget(curvatureMaximumLabel, rowNum, 0);
      paramsGridLayout->addWidget(curvatureMaximumDoubleSpinBox, rowNum, 1);
      rowNum++;
   }

   //
   // Sphere Options Group Box
   //
   projectToSphereIterationsCheckBox = new QCheckBox("Project To Sphere Every X Iterations");
   projectToSphereIterationsSpinBox = new QSpinBox;
   projectToSphereIterationsSpinBox->setMinimum(0);
   projectToSphereIterationsSpinBox->setMaximum(10000);
   projectToSphereIterationsSpinBox->setSingleStep(1);
   projectToSphereIterationsSpinBox->setEnabled(false);
   projectToSphereIterationsSpinBox->setValue(10);
   sphereOptionsGroupBox = new QGroupBox("Spherical Options");
   QHBoxLayout* sphereOptionsLayout = new QHBoxLayout(sphereOptionsGroupBox);
   sphereOptionsLayout->addWidget(projectToSphereIterationsCheckBox);
   sphereOptionsLayout->addWidget(projectToSphereIterationsSpinBox);
   dialogLayout->addWidget(sphereOptionsGroupBox);
   
   //
   // Enable project to sphere iterations spin box when check box is checked
   //
   QObject::connect(projectToSphereIterationsCheckBox, SIGNAL(toggled(bool)),
                    projectToSphereIterationsSpinBox, SLOT(setEnabled(bool)));
   
   threadsSpinBox = NULL;
   if (DebugControl::getDebugOn()) {      
      //
      // Threads strength
      //
      QLabel* threadsLabel = new QLabel("Number to Run");
      threadsSpinBox = new QSpinBox;
      threadsSpinBox->setMinimum(0);
      threadsSpinBox->setMaximum(1024);
      threadsSpinBox->setSingleStep(1);
      threadsSpinBox->setToolTip(
                  "Smoothing will be split into this\n"
                  "number of simultaneous processes.");
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      threadsSpinBox->setValue(pf->getMaximumNumberOfThreads());

      //
      // Group box for threading
      //
      QGroupBox* threadsGroupBox = new QGroupBox("Execution Threads");
      QHBoxLayout* threadsLayout = new QHBoxLayout(threadsGroupBox);
      threadsLayout->addWidget(threadsLabel);
      threadsLayout->addWidget(threadsSpinBox);
      dialogLayout->addWidget(threadsGroupBox);
   }
   
   //
   // Note about normals
   //
   updateNormalsCheckBox = new QCheckBox("Update Surface Normals After Smoothing");
   updateNormalsCheckBox->setChecked(true);
   QGroupBox* normalsGroup = new QGroupBox("Surface Normals");
   QVBoxLayout* normalsLayout = new QVBoxLayout(normalsGroup);
   normalsLayout->addWidget(updateNormalsCheckBox);
   dialogLayout->addWidget(normalsGroup);
              
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   if (isModal()) {
      //
      // OK button
      //
      QPushButton* okButton = new QPushButton("OK");
      okButton->setAutoDefault(false);
      buttonsLayout->addWidget(okButton);
      QObject::connect(okButton, SIGNAL(clicked()),
                     this, SLOT(accept()));
                     
      //
      // Cancel button
      //
      QPushButton* cancelButton = new QPushButton("Cancel");
      cancelButton->setAutoDefault(false);
      buttonsLayout->addWidget(cancelButton);
      QObject::connect(cancelButton, SIGNAL(clicked()),
                     this, SLOT(reject()));
      
      QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   }
   else {
      //
      // Apply button
      //
      QPushButton* applyButton = new QPushButton("Apply");
      applyButton->setAutoDefault(false);
      buttonsLayout->addWidget(applyButton);
      QObject::connect(applyButton, SIGNAL(clicked()),
                     this, SLOT(slotApply()));
                     
      //
      // Close button connects to QDialogs close() slot.
      //
      QPushButton* closeButton = new QPushButton("Close");
      closeButton->setAutoDefault(false);
      buttonsLayout->addWidget(closeButton);
      QObject::connect(closeButton, SIGNAL(clicked()),
                     this, SLOT(close()));
      
      QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   }

   //
   // Default to areal smoothing
   //
   arealSmoothingRadioButton->setChecked(true);
   slotSmoothingType();
}

/**
 * Destructor.
 */
GuiSmoothingDialog::~GuiSmoothingDialog()
{
}

/**
 * Slot for OK or Cancel button in modal mode.
 */
void
GuiSmoothingDialog::done(int r) 
{
   if (r == QDialog::Accepted) {
      if (doSmoothing()) {
         return;
      }
   }
   QDialog::done(r);
}
 
/**
 * smoothing type selection.
 */
void 
GuiSmoothingDialog::slotSmoothingType()
{
   smoothingMode = MODE_NOT_SELECTED;
   
   if (arealSmoothingRadioButton->isChecked()) {
      smoothingMode = MODE_AREAL_SMOOTHING;
   }
   
   if (landmarkSmoothingRadioButton != NULL) {
      if (landmarkSmoothingRadioButton->isChecked()) {
         smoothingMode = MODE_LANDMARK_SMOOTHING;
      }
   }
   
   if (linearSmoothingRadioButton != NULL) {
      if (linearSmoothingRadioButton->isChecked()) {
         smoothingMode = MODE_LINEAR_SMOOTHING;
      }
   }
   
   if (landmarkNeighborSmoothingRadioButton != NULL) {
      if (landmarkNeighborSmoothingRadioButton->isChecked()) {
         smoothingMode = MODE_LANDMARK_NEIGHBOR_SMOOTHING;
      }
   }
   
   if (crossoverSmoothingRadioButton != NULL) {
      if (crossoverSmoothingRadioButton->isChecked()) {
         smoothingMode = MODE_CROSSOVER_SMOOTHING;
      }
   }

   if (curvatureSmoothingRadioButton != NULL) {
      if (curvatureSmoothingRadioButton->isChecked()) {
         smoothingMode = MODE_CURVATURE_SMOOTHING;
      }
   }
   
   if (flatOverlapSmoothingRadioButton != NULL) {
      if (flatOverlapSmoothingRadioButton->isChecked()) {
         smoothingMode = MODE_FLAT_OVERLAP_SMOOTHING;
      }
   }
   
   bool depthCyclesEnabled = false;
   bool curvatureEnabled = false;
   bool sphereOptionsEnabled = true;
   bool edgeIterationsEnabled = true;
   edgeIterationsLabel->setText("Edges Every X Iterations");

   switch (smoothingMode) {
      case MODE_NOT_SELECTED:
         break;
      case MODE_AREAL_SMOOTHING:
         break;
      case MODE_CROSSOVER_SMOOTHING:
         depthCyclesEnabled = true;
         break;
      case MODE_CURVATURE_SMOOTHING:
         curvatureEnabled = true;
         edgeIterationsEnabled = false;
         sphereOptionsEnabled = false;
         break;
      case MODE_FLAT_OVERLAP_SMOOTHING:
         depthCyclesEnabled = true;
         break;
      case MODE_LANDMARK_SMOOTHING:
         edgeIterationsEnabled = false;
         break;
      case MODE_LANDMARK_NEIGHBOR_SMOOTHING:
         edgeIterationsLabel->setText("Neighbors Every X Iterations");
         break;
      case MODE_LINEAR_SMOOTHING:
         break;
   }
   
   sphereOptionsGroupBox->setEnabled(sphereOptionsEnabled);
   
   edgeIterationsLabel->setEnabled(edgeIterationsEnabled);
   edgeIterationsSpinBox->setEnabled(edgeIterationsEnabled);
   numberOfCyclesLabel->setEnabled(depthCyclesEnabled);
   numberOfCyclesSpinBox->setEnabled(depthCyclesEnabled);
   depthLabel->setEnabled(depthCyclesEnabled);
   depthSpinBox->setEnabled(depthCyclesEnabled);
   if (curvatureMaximumLabel != NULL) {
      curvatureMaximumLabel->setEnabled(curvatureEnabled);
   }
   if (curvatureMaximumDoubleSpinBox != NULL) {
      curvatureMaximumDoubleSpinBox->setEnabled(curvatureEnabled);
   }
}

/**
 * Slot for apply button in non-modal mode.
 */
void
GuiSmoothingDialog::slotApply()
{
   doSmoothing();
}

/**
 * Called to do the actual smoothing.  Returns true if an error occurs.
 */   
bool
GuiSmoothingDialog::doSmoothing()
{
   const float strength = strengthSpinBox->value();
   const int iterations = iterationsSpinBox->value();
   const int edgeIters  = edgeIterationsSpinBox->value();
   
   int projectToSphereEveryX = projectToSphereIterationsSpinBox->value();
   if (projectToSphereIterationsCheckBox->isChecked() == false) {
      projectToSphereEveryX = 0;
   }

   const int nodeDepth = depthSpinBox->value();
   const int numberOfCycles = numberOfCyclesSpinBox->value();
      
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   int numberOfThreads = pf->getMaximumNumberOfThreads();
   if (threadsSpinBox != NULL) {
      numberOfThreads = threadsSpinBox->value();
   }

   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
   
      //
      // Check spherical things
      //
      switch (bms->getSurfaceType()) {
         case BrainModelSurface::SURFACE_TYPE_RAW:
         case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
         case BrainModelSurface::SURFACE_TYPE_INFLATED:
         case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
         case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
         case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
         case BrainModelSurface::SURFACE_TYPE_FLAT:
         case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
         case BrainModelSurface::SURFACE_TYPE_HULL:
         case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
         case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
            if (projectToSphereIterationsCheckBox->isChecked()) {
               if (GuiMessageBox::question(this, "Question",
                     "The surface does not appear to be a sphere\n"
                     "but Project to Sphere is selected.  Continue?",
                     "Continue", "Change Selections") == 1) {
                  return true;
               }
            }
            break;
         case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
            if (projectToSphereIterationsCheckBox->isChecked() == false) {
               if (GuiMessageBox::question(this, "Question",
                     "The surface appears to be a sphere but\n"
                     "Project to Sphere is not selected.  Continue?",
                     "Continue", "Change Selections") == 1) {
                  return true;
               }
            }
            break;
      }
      
      //
      // Check flat things
      //
      switch (bms->getSurfaceType()) {
         case BrainModelSurface::SURFACE_TYPE_RAW:
         case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
         case BrainModelSurface::SURFACE_TYPE_INFLATED:
         case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
         case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
         case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
         case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
         case BrainModelSurface::SURFACE_TYPE_HULL:
         case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
         case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
            if (smoothingMode == MODE_FLAT_OVERLAP_SMOOTHING) {
               if (GuiMessageBox::question(this, "Question",
                     "The surface does not appear to be a flat but\n"
                     "Flat Surface Overlap Smoothing is selected.  Continue?",
                     "Continue", "Change Selections") == 1) {
                  return true;
               }
            }
            break;
         case BrainModelSurface::SURFACE_TYPE_FLAT:
         case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
            break;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      switch (smoothingMode) {
         case MODE_NOT_SELECTED:
            GuiMessageBox::critical(this, "Smoothing Error",
                                    "You must select a smoothing type.", "OK");
            return true;
            break;
         case MODE_AREAL_SMOOTHING:
            if (numberOfThreads <= 0) {
               bms->arealSmoothing(strength, iterations, edgeIters,
                                 smoothOnlyTheseNodes, projectToSphereEveryX);
            }
            else {
               BrainModelSurfaceSmoothing smooth(theMainWindow->getBrainSet(),
                                                bms,
                                                BrainModelSurfaceSmoothing::SMOOTHING_TYPE_AREAL,
                                                strength,
                                                iterations,
                                                edgeIters,
                                                0,
                                                smoothOnlyTheseNodes,
                                                NULL,
                                                projectToSphereEveryX,
                                                numberOfThreads);
               try {
                  smooth.execute();
               }
               catch (BrainModelAlgorithmException& e) {
                  GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
                  return true;
               }
            }
            break;
         case MODE_CROSSOVER_SMOOTHING:
            bms->smoothOutSurfaceCrossovers(strength, numberOfCycles, iterations,
                                            edgeIters, projectToSphereEveryX, nodeDepth,
                                            bms->getSurfaceType());
            break;
         case MODE_CURVATURE_SMOOTHING:
            bms->smoothSurfaceUsingCurvature(-strength, iterations, 
                                             curvatureMaximumDoubleSpinBox->value());
            break;
         case MODE_FLAT_OVERLAP_SMOOTHING:
            bms->smoothOutFlatSurfaceOverlap(strength, numberOfCycles, iterations,
                                             edgeIters, nodeDepth);
            break;
         case MODE_LANDMARK_SMOOTHING:
            {
               std::vector<bool> landmarkNodes;
               QString errorMessage;
               if (markLandmarkNodes(bms, landmarkNodes, errorMessage)) {   
                  //
                  // Do the landmark constrained smoothing
                  //
                  bms->landmarkConstrainedSmoothing(strength, iterations, landmarkNodes);
               }
               else {
                  GuiMessageBox::critical(this, "Error", errorMessage, "OK");
                  return true;
               }
            }
            break;
         case MODE_LANDMARK_NEIGHBOR_SMOOTHING:
            {
               std::vector<bool> landmarkNodes;
               QString errorMessage;
               if (markLandmarkNodes(bms, landmarkNodes, errorMessage)) {   
                  //
                  // Do the landmark constrained smoothing
                  //
                  bms->landmarkNeighborConstrainedSmoothing(strength, iterations, 
                                                            landmarkNodes, edgeIters,
                                                            projectToSphereEveryX);
               }
               else {
                  GuiMessageBox::critical(this, "Error", errorMessage, "OK");
                  return true;
               }
            }
            break;
         case MODE_LINEAR_SMOOTHING:
            if (numberOfThreads <= 0) {
               bms->linearSmoothing(strength, iterations, edgeIters,
                                    smoothOnlyTheseNodes, projectToSphereEveryX);
            }
            else {
               BrainModelSurfaceSmoothing smooth(theMainWindow->getBrainSet(),
                                                bms,
                                                BrainModelSurfaceSmoothing::SMOOTHING_TYPE_LINEAR,
                                                strength,
                                                iterations,
                                                edgeIters,
                                                0,
                                                smoothOnlyTheseNodes,
                                                NULL,
                                                projectToSphereEveryX,
                                                numberOfThreads);
               try {
                  smooth.execute();
               }
               catch (BrainModelAlgorithmException& e) {
                  GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
                  return true;
               }
            }
            break;
      }

      if (updateNormalsCheckBox->isChecked()) {
         bms->computeNormals();
      }
      GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
      QApplication::restoreOverrideCursor();
      QApplication::beep();
   }
   
   theMainWindow->speakText("Smoothing has completed.", false);
   
   return false;
}

/**
 * Set landmark nodes.  Returns false if no landmark nodes were found.
 */ 
bool
GuiSmoothingDialog::markLandmarkNodes(BrainModelSurface* bms,
                                      std::vector<bool>& landmarkNodes,
                                      QString& errorMessage)
{
   errorMessage = "";
   
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes > 0) {
      landmarkNodes.resize(numNodes);
      std::fill(landmarkNodes.begin(), landmarkNodes.end(), false);
      
      //
      // Make sure there is a paint column named "Landmarks"
      //
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      const int landmarksColumn = pf->getColumnWithName("Landmarks");
      if (landmarksColumn < 0) {
         errorMessage = "There is no paint column named \"Landmarks\"";
         return false;
      }
      
      //
      // Make sure there is a paint name "Landmark"
      //
      const int landmarkPaintIndex = pf->getPaintIndexFromName("Landmark");
      if (landmarkPaintIndex < 0) {
         errorMessage = "There is no paint name \"Landmark\"";
         return false;
      }
      
      //
      // Find nodes with name "Landmark"
      //
      bool nodeFound = false;
      for (int i = 0; i < numNodes; i++) {
         if (pf->getPaint(i, landmarksColumn) == landmarkPaintIndex) {
            landmarkNodes[i] = true;
            nodeFound = true;
         }
      }
      
      //
      // Make sure landmark nodes exist
      //
      if (nodeFound == false) {
         errorMessage = "No nodes have the paint name \"Landmark\"";
         return false;
      }
      
      return true;
   }
   else {
      errorMessage = "There are no nodes.";
      return false;
   }
}

