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
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QToolTip>

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMorphingDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "PaintFile.h"
#include "global_variables.h"

/**
 * constructor
 */
GuiMorphingDialog::GuiMorphingDialog(QWidget* parent, 
            const BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingSurfaceTypeIn)
   : WuQDialog(parent)
{
   morphingSurfaceType = morphingSurfaceTypeIn;
   
   switch (morphingSurfaceType) {
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
         setWindowTitle("Morphing - Flat");
         break;
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
         setWindowTitle("Morphing - Sphere");
         break;
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // Create the surfaces section
   //
   dialogLayout->addWidget(createSurfaceSection());
     
   //
   // Create the morphing parameters section
   //
   dialogLayout->addWidget(createParametersSection());

   //
   // Create the misc section
   //   
   dialogLayout->addWidget(createMiscSection());
   
   threadsSpinBox = NULL;
   if (DebugControl::getDebugOn()) {
      //
      // Group box for threading
      //
      QGroupBox* threadsGroupBox = new QGroupBox("Execution Threads");
      dialogLayout->addWidget(threadsGroupBox);
      QHBoxLayout* threadsLayout = new QHBoxLayout(threadsGroupBox);
      
      //
      // Threads strength
      //
      threadsLayout->addWidget(new QLabel("Number to Run"));
      threadsSpinBox = new QSpinBox;
      threadsSpinBox->setMinimum(0);
      threadsSpinBox->setMaximum(1024);
      threadsSpinBox->setSingleStep(1);
      threadsLayout->addWidget(threadsSpinBox);
      threadsSpinBox->setToolTip(
                  "Morphing will be split into this\n"
                  "number of simultaneous processes.");
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      threadsSpinBox->setValue(pf->getMaximumNumberOfThreads());
   }
   
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
   applyButton->setToolTip( "Run Morphing.");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setToolTip( "Close dialog.");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   //
   // Load the default parameters
   //
   int iter;
   float ang, lin, step;
   BrainModelSurfaceMorphing junk(NULL, NULL, NULL, morphingSurfaceType);
   junk.getMorphingParameters(iter, lin, ang, step);
   iterationsSpinBox->setValue(iter);
   linearForceDoubleSpinBox->setValue(lin);
   angularForceDoubleSpinBox->setValue(ang);
   stepSizeDoubleSpinBox->setValue(step);
}
            
/**
 * Destructor
 */
GuiMorphingDialog::~GuiMorphingDialog()
{
}

/**
 * Update the dialog
 */
void
GuiMorphingDialog::updateDialog()
{
   referenceSurfaceComboBox->updateComboBox();
   morphingSurfaceComboBox->updateComboBox();
}

/**
 * Create the surface section
 */
QWidget*
GuiMorphingDialog::createSurfaceSection()
{
   //
   // reference surface
   //
   QLabel* refLabel = new QLabel("Reference");
   referenceSurfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0);
   referenceSurfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
   //
   // morphing surface
   //
   QLabel* morphLabel = new QLabel("Morphing");
   morphingSurfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0);
   morphingSurfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainModelSurface());

   QGroupBox* surfaceGroupBox = new QGroupBox("Surfaces");
   QGridLayout* surfaceLayout = new QGridLayout(surfaceGroupBox);
   surfaceLayout->addWidget(refLabel, 0, 0);
   surfaceLayout->addWidget(referenceSurfaceComboBox, 0, 1);
   surfaceLayout->addWidget(morphLabel, 1, 0);
   surfaceLayout->addWidget(morphingSurfaceComboBox, 1, 1);
   return surfaceGroupBox;
}

/**
 * Create the parameters section
 */
QWidget*
GuiMorphingDialog::createParametersSection()
{
   //
   // Linear Force Float spin box
   //
   QLabel* linearLabel = new QLabel("Linear Force");
   linearForceDoubleSpinBox = new QDoubleSpinBox;
   linearForceDoubleSpinBox->setMinimum(0.0);
   linearForceDoubleSpinBox->setMaximum(1.0);
   linearForceDoubleSpinBox->setSingleStep(0.10);
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
   stepSizeDoubleSpinBox->setSingleStep(0.1);
   stepSizeDoubleSpinBox->setDecimals(1);
                                                
      
   //
   // Iterations spin box
   //
   QLabel* itersLabel = new QLabel("Iterations");
   iterationsSpinBox = new QSpinBox;
   iterationsSpinBox->setMinimum(0);
   iterationsSpinBox->setMaximum(100000);
   iterationsSpinBox->setSingleStep(10);
                        
   //
   // Group box and layout
   //
   QGroupBox* paramsGroupBox = new QGroupBox("Morphing Forces");
   QGridLayout* paramsLayout = new QGridLayout(paramsGroupBox);
   paramsLayout->addWidget(linearLabel, 0, 0);
   paramsLayout->addWidget(linearForceDoubleSpinBox, 0, 1);
   paramsLayout->addWidget(angularLabel, 1, 0);
   paramsLayout->addWidget(angularForceDoubleSpinBox, 1, 1);
   paramsLayout->addWidget(stepLabel, 2, 0);
   paramsLayout->addWidget(stepSizeDoubleSpinBox, 2, 1);
   paramsLayout->addWidget(itersLabel, 3, 0);
   paramsLayout->addWidget(iterationsSpinBox, 3, 1);
   return paramsGroupBox;
}

/**
 * Create the misc section
 */
QWidget*
GuiMorphingDialog::createMiscSection()
{                                          
   //
   // delete temporary files check box
   //
   landmarkMorphingCheckBox = new QCheckBox("Landmark Morphing");
   landmarkMorphingCheckBox->setToolTip(
                 "If checked and there is a paint file column named\n"
                 "\"Landmarks\", any node with a paint value of \"Landmark\"\n"
                 "will not be morphed and special forces are applied to\n"
                 "their neighbors.");
                 
   //
   // make measurements each iteration check box
   //
   measureEachIterationCheckBox = new QCheckBox("Measure Each Iteration");
   measureEachIterationCheckBox->setToolTip(
                 "If checked, distortion and crossovers are measured each\n"
                 "pass and written to the file \"morph_stats.txt\".  This\n"
                 "information may be used help choose the best morphing \n"
                 "parameters for use during multi-resolution morphing.");
                 
   QGroupBox* miscGroupBox = new QGroupBox("Misc");
   QVBoxLayout* miscLayout = new QVBoxLayout(miscGroupBox);
   miscLayout->addWidget(landmarkMorphingCheckBox);
   miscLayout->addWidget(measureEachIterationCheckBox);
   return miscGroupBox;
}

/**
 * called when Apply button pressed
 */
void 
GuiMorphingDialog::slotApplyButton()
{
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
         if (QMessageBox::warning(this, "Warning",
                                  "Surface for morphing is not a spherical surface.", 
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Cancel)
                                     == QMessageBox::Cancel) {
            return;
         }
      }
   }
   
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   int numberOfThreads = pf->getMaximumNumberOfThreads();
   if (threadsSpinBox != NULL) {
      numberOfThreads = threadsSpinBox->value();
   }

   //
   // Create the morphing object
   //
   BrainModelSurfaceMorphing bsm(theMainWindow->getBrainSet(),
                                  referenceSurface,
                                  morphingSurface,
                                  morphingSurfaceType,
                                  numberOfThreads);
                                 
   //
   // Copy the saved parameters to the morphing object
   //
   bsm.setMorphingParameters(iterationsSpinBox->value(),
                              linearForceDoubleSpinBox->value(),
                              angularForceDoubleSpinBox->value(),
                              stepSizeDoubleSpinBox->value());
   
   
   //
   // if landmark contrained morphing
   //
   if (landmarkMorphingCheckBox->isChecked()) {
      const int numNodes = morphingSurface->getNumberOfNodes();
      if (numNodes > 0) {
         PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
         const int column = pf->getColumnWithName("Landmarks");
         if (column >= 0) {
            const int paintIndex = pf->getPaintIndexFromName("Landmark");
            std::vector<bool> morphNodes(numNodes, true);
            int cnt = 0;
            for (int i = 0; i < numNodes; i++) {
               if (pf->getPaint(i, column) == paintIndex) {
                  morphNodes[i] = false;
                  cnt++;
               }
            }
            if (DebugControl::getDebugOn()) {
               std::cout << "There are " << cnt << " landmark nodes." << std::endl;
            }
            bsm.setNodesThatShouldBeMorphed(morphNodes, 0.5);
         }
      }
   }
   
   //
   // Do statistics each pass
   //
   bsm.setDoStatisticsEachPass(measureEachIterationCheckBox->isChecked());
   
   //
   // Execute the morphing
   //
   QTime timer;
   timer.start();
   try {
      bsm.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(theMainWindow, "Error", e.whatQString(), "OK");
      return;
   }
   const float elapsedTime = timer.elapsed() * 0.001;
   if (DebugControl::getDebugOn()) {
      std::cout << "Morphing Time: " << elapsedTime << " seconds." << std::endl;
   }
   
   //
   // Notify about modified files and redraw all displays
   //
   GuiFilesModified fm;
   fm.setCoordinateModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
   QApplication::beep();
   
   //theMainWindow->speakText("Morphing has completed", false);
}

