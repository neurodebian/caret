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

#include <iostream>
#include <sstream>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QToolTip>

#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBatchCommandDialog.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMorphingMeasurementsDialog.h"
#include "GuiMultiresolutionMorphingDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor
 */
GuiMultiresolutionMorphingDialog::GuiMultiresolutionMorphingDialog(QWidget* parent, 
            BrainModelSurfaceMultiresolutionMorphing* morphObjectIn,
            const bool parametersOnlyModeIn)
   : WuQDialog(parent)
{
   setModal(true);
   morphObject = morphObjectIn;
   morphingSurfaceType = morphObjectIn->getMorphingSurfaceType();
   parametersOnlyMode  = parametersOnlyModeIn;
   
   switch (morphingSurfaceType) {
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
         setWindowTitle("Multiresolution Morphing - Flat");
         break;
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
         setWindowTitle("Multiresolution Morphing - Sphere");
         break;
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   if (parametersOnlyMode == false) {
      //
      // Create the surfaces section
      //
      dialogLayout->addWidget(createSurfaceSection());
   }
   
   //
   // Horizontal layout for other layouts
   //
   QHBoxLayout* horizLayout = new QHBoxLayout;
   horizLayout->setSpacing(2);
   dialogLayout->addLayout(horizLayout);
   
   //
   // Vertical layout for cycles and iterations
   //
   QVBoxLayout* cyclesAndIterationsLayout = new QVBoxLayout;
   horizLayout->addLayout(cyclesAndIterationsLayout);
   
   //
   // Create the cycles section
   //
   cyclesAndIterationsLayout->addWidget(createCyclesSection());
   
   //
   // Create the iterations section
   //
   cyclesAndIterationsLayout->addWidget(createIterationsSection());
   
   
   //
   // Vertical layout for parameters and smoothing
   //
   QVBoxLayout* parametersAndSmoothingLayout = new QVBoxLayout;
   parametersAndSmoothingLayout->setSpacing(2);
   horizLayout->addLayout(parametersAndSmoothingLayout);
  
   //
   // Create the morphing parameters section
   //
   parametersAndSmoothingLayout->addWidget(createParametersSection());

   //
   // Create the smoothing section
   //
   parametersAndSmoothingLayout->addWidget(createSmoothingSection());

   //
   // Create the misc section
   //   
   dialogLayout->addWidget(createMiscSection());
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // load the saved parameters
   //
   loadParametersIntoDialog();
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   if (parametersOnlyMode == false) {
      okButton->setText("Run");
      okButton->setToolTip("Run multi-resolution morphing.");
   }
   else {
      okButton->setToolTip("Use parameters for morphing.");
   }
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   
   QPushButton* batchButton = NULL;
/*
   if (parametersOnlyMode == false) {
      batchButton = new QPushButton("Batch...", this);
      batchButton->setToolTip("Create a command file for\n"
                                 "multi-resolution morphing.");
      batchButton->setAutoDefault(false);
      buttonsLayout->addWidget(batchButton);
      QObject::connect(batchButton, SIGNAL(clicked()),
                       this, SLOT(slotBatchButton()));
   }
*/
   
   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setToolTip("Close dialog and\n"
                               "do not morph.");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   
   if (batchButton != NULL) {
      QtUtilities::makeButtonsSameSize(okButton, batchButton, cancelButton);
   }
   else {
      QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   }
   
   loadParametersIntoDialog();
}
            
/**
 * Destructor
 */
GuiMultiresolutionMorphingDialog::~GuiMultiresolutionMorphingDialog()
{
}

/**
 * Create a command file for running in batch mode.
 */
void
GuiMultiresolutionMorphingDialog::slotBatchButton()
{
   //
   // save the  parameters
   //
   readParametersFromDialog();
   
   //
   // get the morphing surfaces
   //
   BrainModelSurface* referenceSurface = 
      referenceSurfaceComboBox->getSelectedBrainModelSurface();
   BrainModelSurface* morphingSurface  = 
      morphingSurfaceComboBox->getSelectedBrainModelSurface();
   
   //
   // Check reference coordinate file
   //
   QString errorMessage;
   QString warningMessage;
   CoordinateFile* referenceCoordFile = NULL;
   if (referenceSurface == NULL) {
      errorMessage.append("No reference surface is selected.\n");
   }
   else {
      referenceCoordFile = referenceSurface->getCoordinateFile();
      if (referenceCoordFile->getModified()) {
         warningMessage.append("Reference surface coordinate file is modified and should be saved.\n");
      }
      else if (referenceCoordFile->getFileName().isEmpty()) {
         errorMessage.append("Reference surface coordinate file does not have a name so it needs to be saved\n"); 
      }      
   }
   
   //
   // Check morphing coordinate file
   //
   CoordinateFile* morphCoordFile = NULL;
   if (morphingSurface == NULL) {
      errorMessage.append("No morphing surface is selected.\n");
   }
   else {
      morphCoordFile = morphingSurface->getCoordinateFile();
      if (morphCoordFile->getModified()) {
         warningMessage.append("Morphing surface coordinate file is modified and should be saved.\n");
      }
      else if (morphCoordFile->getFileName().isEmpty()) {
         errorMessage.append("Morphing surface coordinate file does not have a name so it needs to be saved\n"); 
      }      
   }
   
   //
   // Check the topology file
   //
   TopologyFile* topologyFile = NULL;
   if (morphingSurface != NULL) {
      topologyFile = morphingSurface->getTopologyFile();
      if (topologyFile == NULL) {
         errorMessage.append("Morphing surface has no topology file.");
      }
      else if (topologyFile->getModified()) {
         warningMessage.append("Morphing surface topology file is modified and should be saved.\n");
      }
      else if (topologyFile->getFileName().isEmpty()) {
         errorMessage.append("Morphing surface topology file does not have a name so it needs to be saved\n"); 
      }      
   }
   
   //
   // Check the spec file
   //
   const QString specFileName(theMainWindow->getBrainSet()->getSpecFileName());
   if (specFileName.isEmpty()) {
      errorMessage.append("There is no spec file.  A spec file can be created by saving\n"
                          "the reference coordinate file, the morphing coordinate file\n"
                          "and the morphing surface topology file.");
   }
   
   //
   // Are there any problems
   //
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this, "Error", errorMessage);
      return;
   }
   
   //
   // Check for flat/spherical surface
   //
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
      if ((morphingSurface->getSurfaceType() !=
           BrainModelSurface::SURFACE_TYPE_FLAT) &
          (morphingSurface->getSurfaceType() !=
           BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         warningMessage.append("Surface for morphing is not a flat surface.\n");
      }
   }
   else {
      if (morphingSurface->getSurfaceType() !=
           BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
         warningMessage.append("Surface for morphing is not a spherical surface.\n");
      }
   }
   
   //
   // Are there potential problems
   //
   if (warningMessage.isEmpty() == false) {
      if (QMessageBox::warning(this, "Warning", warningMessage, 
                               (QMessageBox::Ok | QMessageBox::Cancel),
                               QMessageBox::Cancel)
                                  == QMessageBox::Cancel) {
         return;
      }
   }
   
   //
   // Index to parameters
   //
   QString morphSwitch("   -flat");
   int indx = 0;
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
      indx = 1;
      morphSwitch = "   -sphere";
   } 
   
   //
   // Create the command
   //
   std::ostringstream commandStr;
   commandStr 
      << theMainWindow->getBrainSet()->getCaretHomeDirectory().toAscii().constData() << "/bin/caret_morph \\"<< "\n"
      << morphSwitch.toAscii().constData() << "" << " \\\n"
      << "   -cycles " << morphObject->getNumberOfCycles() << " \\\n";

   if (morphObject->getDeleteIntermediateFiles()) {
      commandStr << "   -delete_temp YES" << " \\\n";
   }
   else {
      commandStr << "   -delete_temp NO" << " \\\n";
   }
   
   for (int j = 0; j < morphObject->getNumberOfCycles(); j++) {
      const int cycleNum = j + 1;
      
      int iters[BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS];
      morphObject->getIterationsPerLevel(j, iters);
      commandStr << "   -iter "
                 << cycleNum << " ";
      for (int k = 0; k < BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS; k++) {
         commandStr << iters[k] << " ";
      }
      commandStr << " \\\n";
      
      float linearForce, angularForce, stepSize;
      morphObject->getMorphingParameters(j, linearForce, angularForce, stepSize);
      commandStr << "   -morph_force " 
                 << cycleNum << " " 
                 << linearForce << " "
                 << angularForce << " "
                 << stepSize << " \\\n";
               
      float strength;
      int smoothIterations, edgeSmoothIterations;
      morphObject->getSmoothingParameters(j, strength, smoothIterations, edgeSmoothIterations);
      commandStr << "   -smooth " 
                 << cycleNum << " "
                 << strength << " "
                 << smoothIterations << " "
                 << edgeSmoothIterations << " \\\n";
   }
              
   if (morphObject->getCrossoverSmoothAtEndOfEachCycle()) {
      commandStr << "   -smooth_cross YES" << " \\\n";
   }
   else {
      commandStr << "   -smooth_cross NO" << " \\\n";
   }
   
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
      if (morphObject->getEnableFlatSurfaceOverlapSmoothing()) {
         commandStr << "   -smooth_over YES" << " \\\n";
      }
      else {
         commandStr << "   -smooth_over NO" << " \\\n";
      }
   }
   
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
      if (morphObject->getPointSphericalTilesOutward()) {
         commandStr << "   -sphere_tile YES" << " \\\n";
      }
      else {
         commandStr << "   -sphere_tile NO" << " \\\n";
      }
   }

   commandStr << "   " << FileUtilities::basename(theMainWindow->getBrainSet()->getSpecFileName()).toAscii().constData() << " \\\n";
   commandStr << "   " << FileUtilities::basename(topologyFile->getFileName()).toAscii().constData() << " \\\n";
   commandStr << "   " << FileUtilities::basename(referenceCoordFile->getFileName()).toAscii().constData() << " \\\n";
   commandStr << "   " << FileUtilities::basename(morphCoordFile->getFileName()).toAscii().constData() << "\n";
   
   QString commandFileName("caret_sphere_morph");
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
      commandFileName = "caret_flat_morph";
   }
   
   GuiBatchCommandDialog bcd(this,
                             QDir::currentPath(),
                             commandStr.str().c_str(),
                             commandFileName);
   if (bcd.exec() == QDialog::Rejected) {
      return;
   }
   
   //
   // Allow the dialog to close.
   //
   reject();
}

/**
 * Create the cycles section
 */
QWidget*
GuiMultiresolutionMorphingDialog::createCyclesSection()
{
   QLabel* totalLabel = new QLabel("Total ");
   numberOfCyclesComboBox = new QComboBox;
   for (int i = 0; i < 10; i++) {
      numberOfCyclesComboBox->addItem(QString::number(i + 1));
   }
   QObject::connect(numberOfCyclesComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotNumberOfCyclesSpinBox()));
                    
   QLabel* editLabel = new QLabel("Edit ");
   editCycleComboBox = new QComboBox;
   for (int i = 0; i < 10; i++) {
      editCycleComboBox->addItem(QString::number(i + 1));
   }
   QObject::connect(editCycleComboBox, SIGNAL(highlighted(int)),
                    this, SLOT(slotEditCycleSpinBoxOldValue()));
   QObject::connect(editCycleComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEditCycleSpinBox()));
                                             
   QGroupBox* cycleGroupBox = new QGroupBox("Cycles");
   QHBoxLayout* cycleLayout = new QHBoxLayout(cycleGroupBox);
   cycleLayout->addWidget(totalLabel);
   cycleLayout->addWidget(numberOfCyclesComboBox);
   cycleLayout->addWidget(editLabel);
   cycleLayout->addWidget(editCycleComboBox);
   return cycleGroupBox;
}

/**
 * called when number of cycles combo box item is selected.
 */
void 
GuiMultiresolutionMorphingDialog::slotNumberOfCyclesSpinBox()
{
   readParametersFromDialog();
   if (numberOfCyclesComboBox->currentIndex() > editCycleComboBox->currentIndex()) {
      editCycleComboBox->setCurrentIndex(numberOfCyclesComboBox->currentIndex());
   }
   loadParametersIntoDialog();
}

/**
 * called when edit cycle combo box item is selected.
 */
void 
GuiMultiresolutionMorphingDialog::slotEditCycleSpinBox()
{
   loadParametersIntoDialog();
}

/**
 * called when edit cycle combo box item is highlighted before selection made.
 */
void 
GuiMultiresolutionMorphingDialog::slotEditCycleSpinBoxOldValue()
{
   readParametersFromDialog();
}

/**
 * Create the surface section
 */
QWidget*
GuiMultiresolutionMorphingDialog::createSurfaceSection()
{   
   //
   // reference surface
   //
   QLabel* refLabel = new QLabel("Reference");
   referenceSurfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "referenceSurfaceComboBox");
   referenceSurfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
   
   //
   // morphing surface
   //
   QLabel* morphLabel = new QLabel("Morphing");
   morphingSurfaceComboBox = new GuiBrainModelSelectionComboBox( false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "morphingSurfaceComboBox");
   const BrainModelSurface* mainBMS = theMainWindow->getBrainModelSurface();
   morphingSurfaceComboBox->setSelectedBrainModel(mainBMS);
   switch (morphingSurfaceType) {
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
         if (mainBMS->getIsFlatSurface() == false) {
            morphingSurfaceComboBox->setSelectedBrainModelToFirstSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
         }
         break;
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
         if (mainBMS->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
            morphingSurfaceComboBox->setSelectedBrainModelToFirstSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
         }
         break;
   }

   //
   // Group box and layout
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surfaces");
   QGridLayout* surfaceLayout = new QGridLayout(surfaceGroupBox);
   surfaceLayout->addWidget(refLabel, 0, 0);
   surfaceLayout->addWidget(referenceSurfaceComboBox, 0, 1);
   surfaceLayout->addWidget(morphLabel, 1, 0);
   surfaceLayout->addWidget(morphingSurfaceComboBox, 1, 1);
   return surfaceGroupBox;
}


/**
 * Create the iterations section
 */
QWidget*
GuiMultiresolutionMorphingDialog::createIterationsSection()
{
   QGroupBox* itersGroupBox = new QGroupBox("Iterations");
   QGridLayout* itersLayout = new QGridLayout(itersGroupBox);
   
   for (int i = 0; i < BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS; i++) {
      QString labelValue("");
      if (i == 0) {
         labelValue = "Fine";
      }
      else if (i == (BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS - 1)) {
         labelValue = "Coarse";
      }
      else if (i == 1) {
         labelValue = "Level 2";
      }
      else if (i == 2) {
         labelValue = "Level 3";
      }
      else if (i == 3) {
         labelValue = "Level 4";
      }
      else if (i == 4) {
         labelValue = "Level 5";
      }
      else if (i == 5) {
         labelValue = "Level 6";
      }
      itersLayout->addWidget(new QLabel(labelValue), i, 0);
      
      iterationsSpinBoxes[i] = new QSpinBox;
      iterationsSpinBoxes[i]->setMinimum(0);
      iterationsSpinBoxes[i]->setMaximum(10000);
      iterationsSpinBoxes[i]->setSingleStep(1);
      itersLayout->addWidget(iterationsSpinBoxes[i], i, 1);
   }
   
   return itersGroupBox;
}

/**
 * Create the parameters section
 */
QWidget*
GuiMultiresolutionMorphingDialog::createParametersSection()
{
   //
   // Linear Force Float spin box
   //
   QLabel* linearLabel = new QLabel("Linear Force");
   linearForceDoubleSpinBox = new QDoubleSpinBox;
   linearForceDoubleSpinBox->setMinimum(0.0);
   linearForceDoubleSpinBox->setMaximum(1.0);
   linearForceDoubleSpinBox->setSingleStep(0.1);
   linearForceDoubleSpinBox->setDecimals(1);
                                                
   //
   // Angular Float spin box
   //
   QLabel* angularLabel = new QLabel("Angular Force");
   angularForceDoubleSpinBox = new QDoubleSpinBox;
   angularForceDoubleSpinBox->setMinimum(0.0);
   angularForceDoubleSpinBox->setMaximum(1.0);
   angularForceDoubleSpinBox->setSingleStep(0.10);
   angularForceDoubleSpinBox->setDecimals(1);
                                                
   //
   // Step Size Float spin box
   //
   QLabel* stepLabel = new QLabel("Step Size");
   stepSizeDoubleSpinBox = new QDoubleSpinBox;
   stepSizeDoubleSpinBox->setMinimum(0.0);
   stepSizeDoubleSpinBox->setMaximum(1.0);
   stepSizeDoubleSpinBox->setSingleStep(0.10);
   stepSizeDoubleSpinBox->setDecimals(1);
                                                
      
   QGroupBox* paramsGroupBox = new QGroupBox("Morphing Forces");
   QGridLayout* paramsLayout = new QGridLayout(paramsGroupBox);
   paramsLayout->addWidget(linearLabel, 0, 0);
   paramsLayout->addWidget(linearForceDoubleSpinBox, 0, 1);
   paramsLayout->addWidget(angularLabel, 1, 0);
   paramsLayout->addWidget(angularForceDoubleSpinBox, 1, 1);
   paramsLayout->addWidget(stepLabel, 2, 0);
   paramsLayout->addWidget(stepSizeDoubleSpinBox, 2, 1);
   return paramsGroupBox;
}

/**
 * Create the smoothing section
 */
QWidget*
GuiMultiresolutionMorphingDialog::createSmoothingSection()
{
   //
   // Smoothing Strength Float spin box
   //
   QLabel* strengthLabel = new QLabel("Strength");
   smoothingStrengthDoubleSpinBox = new QDoubleSpinBox;
   smoothingStrengthDoubleSpinBox->setMinimum(0.0);
   smoothingStrengthDoubleSpinBox->setMaximum(1.0);
   smoothingStrengthDoubleSpinBox->setSingleStep(0.10);
   smoothingStrengthDoubleSpinBox->setDecimals(1);
                                                
   //
   // Smoothing iterations spin box
   //
   QLabel* itersLabel = new QLabel("Iterations");
   smoothingIterationsSpinBox = new QSpinBox;
   smoothingIterationsSpinBox->setMinimum(0);
   smoothingIterationsSpinBox->setMaximum(10000);
   smoothingIterationsSpinBox->setSingleStep(10);
                                                
   //
   // Smoothing Edge iterations spin box
   //
   QLabel* smoothLabel = new QLabel("Smooth Edges Every");
   smoothEdgesEverySpinBox = new QSpinBox;
   smoothEdgesEverySpinBox->setMinimum(0);
   smoothEdgesEverySpinBox->setMaximum(100);
   smoothEdgesEverySpinBox->setSingleStep(10);
                                                
      
   QGroupBox* smoothingGroupBox = new QGroupBox("Smoothing");
   QGridLayout* smoothingLayout = new QGridLayout(smoothingGroupBox);
   smoothingLayout->addWidget(strengthLabel, 0, 0);
   smoothingLayout->addWidget(smoothingStrengthDoubleSpinBox, 0, 1);
   smoothingLayout->addWidget(itersLabel, 1, 0);
   smoothingLayout->addWidget(smoothingIterationsSpinBox, 1, 1);
   smoothingLayout->addWidget(smoothLabel, 2, 0);
   smoothingLayout->addWidget(smoothEdgesEverySpinBox, 2, 1);
   return smoothingGroupBox;
}

/**
 * Create the misc section
 */
QWidget*
GuiMultiresolutionMorphingDialog::createMiscSection()
{
   //
   // Central Sulcus Landmark
   //
   alignSurfaceCheckBox = new QCheckBox("Align to CeS Landmark");
   alignSurfaceCheckBox->setChecked(true);
   cesLandmarkNameLineEdit = new QLineEdit;
   cesLandmarkNameLineEdit->setText("LANDMARK.CentralSulcus");
   cesLandmarkNameLineEdit->setToolTip("If there is a border with this name,\n"
                                       "it will be used to automatically \n"
                                       "align the surface at the conclusion\n"
                                       "of multi-resolution morphing.");
   QHBoxLayout* cesLayout = new QHBoxLayout;
   cesLayout->addWidget(alignSurfaceCheckBox);
   cesLayout->addWidget(cesLandmarkNameLineEdit);
   cesLayout->setStretchFactor(alignSurfaceCheckBox, 0);
   cesLayout->setStretchFactor(cesLandmarkNameLineEdit, 100);
   
   //
   // delete temporary files check box
   //
   deleteTempFilesCheckBox = new QCheckBox("Delete Temporary Files");
   
   //
   // Smooth out crossovers check box
   //
   smoothOutCrossoversCheckBox = new QCheckBox("Smooth Out Crossovers Each Cycle");
                                               
   //
   // Flat surface smooth out overlap check box
   //
   smoothOutOverlapCheckBox = NULL;
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
      smoothOutOverlapCheckBox = new QCheckBox("Smooth Out Surface Overlap");
   }
   
   //
   // point spherical tiles outward check box
   //
   pointSphericalTilesOutwardCheckBox = NULL;
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
      pointSphericalTilesOutwardCheckBox = new QCheckBox("Point Spherical Tiles Outward");
   }
   
   QGroupBox* miscGroupBox = new QGroupBox("Misc");
   QVBoxLayout* miscLayout = new QVBoxLayout(miscGroupBox);
   miscLayout->addWidget(deleteTempFilesCheckBox);
   miscLayout->addWidget(smoothOutCrossoversCheckBox);
   if (smoothOutOverlapCheckBox != NULL) {
      miscLayout->addWidget(smoothOutOverlapCheckBox);
   }
   if (pointSphericalTilesOutwardCheckBox != NULL) {
      miscLayout->addWidget(pointSphericalTilesOutwardCheckBox);
   }
   miscLayout->addLayout(cesLayout);
   
   return miscGroupBox;
}

/**
 * called when OK/Cancel buttons pressed
 */
void 
GuiMultiresolutionMorphingDialog::done(int r)
{
   if (r != QDialog::Accepted) {
      QDialog::done(r);
      return;
   }
   
   
   //
   // read the  parameters
   //
   readParametersFromDialog();
   
   if (parametersOnlyMode) {
      QDialog::done(r);
      return;
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   //
   // get the morphing surfaces
   //
   BrainModelSurface* referenceSurface = 
      referenceSurfaceComboBox->getSelectedBrainModelSurface();
   BrainModelSurface* morphingSurface  = 
      morphingSurfaceComboBox->getSelectedBrainModelSurface();
   
   //
   // Check for flat/spherical surface
   //
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
      if ((morphingSurface->getSurfaceType() !=
           BrainModelSurface::SURFACE_TYPE_FLAT) &
          (morphingSurface->getSurfaceType() !=
           BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         QApplication::restoreOverrideCursor();
         if (QMessageBox::warning(this, "Warning",
                                  "Surface for morphing is not a flat surface.", 
                               (QMessageBox::Ok | QMessageBox::Cancel),
                               QMessageBox::Cancel)
                                  == QMessageBox::Cancel) {
            return;
         }
      }
   }
   else {
      if (morphingSurface->getSurfaceType() !=
           BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
         QApplication::restoreOverrideCursor();
         if (QMessageBox::warning(this, "Warning",
                                  "Surface for morphing is not a spherical surface.", 
                               (QMessageBox::Ok | QMessageBox::Cancel),
                               QMessageBox::Cancel)
                                  == QMessageBox::Cancel) {
            return;
         }
      }
   }
   
   //
   // Get the border projection file and look for the central sulcus border
   //
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   BorderProjectionFile bpf;
   BorderProjection* centralSulcusBorderProjection = NULL;
   if (alignSurfaceCheckBox->isChecked()) {
      const QString centralSulcusBorderProjectionName = cesLandmarkNameLineEdit->text().trimmed();
      bmbs->copyBordersToBorderProjectionFile(bpf);
      centralSulcusBorderProjection = 
         bpf.getFirstBorderProjectionByName(centralSulcusBorderProjectionName);
   }
   
   //
   // Create the morphing object
   //
   BrainModelSurfaceMultiresolutionMorphing bsmm(theMainWindow->getBrainSet(),
                                                referenceSurface,
                                                morphingSurface,
                                                morphingSurfaceType,
                                                centralSulcusBorderProjection);
   
   //
   // Copy the saved parameters to the morphing object
   //
   bsmm.copyParameters(*morphObject);
   
   //
   // Allow the dialog to close
   //
   QDialog::done(r);
   
   //
   // Execute the morphing
   //
   QTime timer;
   timer.start();
   try {
      bsmm.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(theMainWindow, "Error", e.whatQString());
      return;
   }
   const float elapsedTime = timer.elapsed() * 0.001;
   
   //
   // Set the overlay to crossovers (if there are any)
   //
   bool haveCrossovers = false;
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      if (theMainWindow->getBrainSet()->getNodeAttributes(i)->getCrossover()) {
         haveCrossovers = true;
         break;
      }
   }
   if (haveCrossovers) {
      BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
      theMainWindow->getBrainSet()->getPrimarySurfaceOverlay()->setOverlay(
                                            -1, BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS);
      bsnc->assignColors();
   }
   
   //
   // Display the morphing information
   //
   std::vector<MorphingMeasurements> mm;
   bsmm.getMorphingMeasurements(mm);
   GuiMorphingMeasurementsDialog* mmd = new GuiMorphingMeasurementsDialog(mm, elapsedTime,
                                                      morphingSurfaceType, true, theMainWindow);
   mmd->show();
   
   //
   // Clear modification status of borders
   //                                     
   bmbs->setAllBordersModifiedStatus(false);
   bmbs->setProjectionsModified(false);
                          
   //
   // Notify about modified files and redraw all displays
   //
   GuiFilesModified fm;
   fm.setCoordinateModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
   QApplication::beep();
   
   theMainWindow->speakText("Multi-resolution morphing has completed", false);
}

/**
 * load saved parameters into dialog.
 */
void
GuiMultiresolutionMorphingDialog::loadParametersIntoDialog()
{
   //
   // load number of cycles
   //
   numberOfCyclesComboBox->setCurrentIndex(morphObject->getNumberOfCycles() - 1);
   
   //
   // Get current cycle being edited
   //
   const int editCycle = editCycleComboBox->currentIndex();
   
   //
   // load iterations
   //
   int iters[BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS];
   morphObject->getIterationsPerLevel(editCycle, iters);
   for (int i = 0; i < BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS; i++) {
      iterationsSpinBoxes[i]->setValue(iters[i]);
   }
   
   //
   // load morphing forces
   //
   float linearForce, angularForce, stepSize;
   morphObject->getMorphingParameters(editCycle, linearForce, angularForce, stepSize);
   linearForceDoubleSpinBox->setValue(linearForce);
   angularForceDoubleSpinBox->setValue(angularForce);
   stepSizeDoubleSpinBox->setValue(stepSize);
   
   //
   // load smoothing parameters
   //
   float strength;
   int smoothIterations, smoothEdgeIterations;
   morphObject->getSmoothingParameters(editCycle, strength, smoothIterations, smoothEdgeIterations);
   smoothingStrengthDoubleSpinBox->setValue(strength);
   smoothingIterationsSpinBox->setValue(smoothIterations);
   smoothEdgesEverySpinBox->setValue(smoothEdgeIterations);
   
   //
   // load delete intermediate files
   //
   deleteTempFilesCheckBox->setChecked(morphObject->getDeleteIntermediateFiles());
   
   //
   // Load smooth out crossovers check box
   //
   smoothOutCrossoversCheckBox->setChecked(morphObject->getCrossoverSmoothAtEndOfEachCycle());
   
   //
   // Load smooth out flat surface overlap chekc box
   //
   if (smoothOutOverlapCheckBox != NULL) {
      smoothOutOverlapCheckBox->setChecked(morphObject->getEnableFlatSurfaceOverlapSmoothing());
   }
   
   //
   // Load point spherical tiles out check box
   //
   if (pointSphericalTilesOutwardCheckBox != NULL) {
      pointSphericalTilesOutwardCheckBox->setChecked(morphObject->getPointSphericalTilesOutward());
   }
}

/**
 * Save the dialog parameters.
 */
void
GuiMultiresolutionMorphingDialog::readParametersFromDialog()
{
   int indx = 0;
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
      indx = 1;
   } 
   
   //
   // save number of cycles
   //
   morphObject->setNumberOfCycles(numberOfCyclesComboBox->currentIndex() + 1);
   
   //
   // Get the cycle being edited
   //
   const int editCycle = editCycleComboBox->currentIndex();
   
   //
   // save iterations
   //
   int iters[BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS];
   for (int i = 0; i < BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS; i++) {
      iters[i] = iterationsSpinBoxes[i]->value();
   }
   morphObject->setIterationsPerLevel(editCycle, iters);
   
   //
   // save morphing forces
   //
   morphObject->setMorphingParameters(editCycle,
                                      linearForceDoubleSpinBox->value(),
                                      angularForceDoubleSpinBox->value(),
                                      stepSizeDoubleSpinBox->value());
   
   //
   // save smoothing parameters
   //
   morphObject->setSmoothingParameters(editCycle,
                                       smoothingStrengthDoubleSpinBox->value(),
                                       smoothingIterationsSpinBox->value(),
                                       smoothEdgesEverySpinBox->value());
   
   //
   // save delete intermediate files
   //
   morphObject->setDeleteIntermediateFiles(deleteTempFilesCheckBox->isChecked());
   
   //
   // Save smooth out crossovers
   //
   morphObject->setCrossoverSmoothAtEndOfEachCycle(smoothOutCrossoversCheckBox->isChecked());
   
   //
   // save smooth out overlap 
   //
   if (smoothOutOverlapCheckBox != NULL) {
      morphObject->setEnableFlatSurfaceOverlapSmoothing(smoothOutOverlapCheckBox->isChecked());
   }
   
   //
   // save point spherical tiles out
   //
   if (pointSphericalTilesOutwardCheckBox != NULL) {
      morphObject->setPointSphericalTilesOutward(pointSphericalTilesOutwardCheckBox->isChecked());
   }
}

