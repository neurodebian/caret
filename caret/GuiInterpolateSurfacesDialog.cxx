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

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "BrainModelBorderSet.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiInterpolateSurfacesDialog.h"
#include "GuiMainWindow.h"
#include "GuiTopologyFileComboBox.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"
#include "vtkTransform.h"

/**
 * Constructor.
 */
GuiInterpolateSurfacesDialog::GuiInterpolateSurfacesDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Interpolate Surfaces");
   
   //
   // Vertical box layout of all items
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(3);
   rows->setSpacing(3);
   
   //
   // Interpolation parameters group box
   //
   QGroupBox* parametersGroupBox = new QGroupBox("Interpolation Parameters");
   QVBoxLayout* parametersGroupLayout = new QVBoxLayout(parametersGroupBox);
   rows->addWidget(parametersGroupBox);
   
   //
   // Interpolate steps spin box
   //
   QHBoxLayout* stepsBoxLayout = new QHBoxLayout;
   parametersGroupLayout->addLayout(stepsBoxLayout);
   stepsBoxLayout->addWidget(new QLabel("Interplation Steps"));
   interpolationStepsSpinBox = new QSpinBox;
   interpolationStepsSpinBox->setMinimum(1);
   interpolationStepsSpinBox->setMaximum(100000);
   interpolationStepsSpinBox->setSingleStep(1);
   interpolationStepsSpinBox->setValue(20);
   stepsBoxLayout->addWidget(interpolationStepsSpinBox);
   stepsBoxLayout->addStretch(1000);
   
   //
   // Reproject borders, cells, and foci each step check boxes
   //
   reprojectBordersCheckBox = new QCheckBox("Reproject Borders Each Step");
   reprojectBordersCheckBox->setChecked(true);
   parametersGroupLayout->addWidget(reprojectBordersCheckBox);
   
   //
   // Topology group box
   //
   QGroupBox* topologyGroupBox = new QGroupBox("Topology File");
   rows->addWidget(topologyGroupBox);
   QVBoxLayout* topologyGroupLayout = new QVBoxLayout(topologyGroupBox);
   
   //
   // Topology selection combo box
   //
   topologyComboBox = new GuiTopologyFileComboBox(0);
   topologyGroupLayout->addWidget(topologyComboBox);
   
   //
   // group box and layout for the surfaces
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surfaces");
   rows->addWidget(surfaceGroupBox);
   surfaceGridLayout = new QGridLayout(surfaceGroupBox);
   
   //
   // rotate group box
   //
   QGroupBox* rotateGroupBox = new QGroupBox("Rotation");
   QVBoxLayout* rotateLayout = new QVBoxLayout(rotateGroupBox);
   rows->addWidget(rotateGroupBox);
   
   //
   // Rotate surfaces check box
   //
   rotateSurfacesCheckBox = new QCheckBox("Rotate Surfaces (execept flat)");
   rotateLayout->addWidget(rotateSurfacesCheckBox);
   
   //
   // rotation step hbox
   //
   QHBoxLayout* rotationStepLayout = new QHBoxLayout;
   rotateLayout->addLayout(rotationStepLayout);
   rotationStepLayout->addWidget(new QLabel("Degrees Per Step"));
   rotationStepDoubleSpinBox = new QDoubleSpinBox;
   rotationStepDoubleSpinBox->setMinimum(0.01);
   rotationStepDoubleSpinBox->setMaximum(360.0);
   rotationStepDoubleSpinBox->setSingleStep(1.0);
   rotationStepDoubleSpinBox->setDecimals(2);
   rotationStepDoubleSpinBox->setValue(10.0);
   rotationStepLayout->addWidget(rotationStepDoubleSpinBox);
   rotationStepLayout->addStretch(1000);
   
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   rows->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                  this, SLOT(slotApplyButton()));
   buttonsLayout->addWidget(applyButton);
   
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                  this, SLOT(reject()));
   buttonsLayout->addWidget(closeButton);
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   updateDialog();
}

/**
 * Destructor.
 */
GuiInterpolateSurfacesDialog::~GuiInterpolateSurfacesDialog()
{
}

/**
 * Called to update the dialog
 */
void
GuiInterpolateSurfacesDialog::updateDialog()
{
   static QDateTime prevSpecLoadTime = QDateTime::currentDateTime();
   QDateTime specLoadTime = theMainWindow->getBrainSet()->getSpecFileTimeOfLoading();
   
   topologyComboBox->updateComboBox();

   //
   // Count the number of surfaces (max of 10)
   //
   int numSurfaces = 0;
   std::vector<BrainModelSurface*> defaultSurfaces;
   for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
      if (theMainWindow->getBrainSet()->getBrainModelSurface(i) != NULL) {
         defaultSurfaces.push_back(theMainWindow->getBrainSet()->getBrainModelSurface(i));
         numSurfaces++;
      }
   }

   const int oldNumSurfaces = static_cast<int>(surfaceComboBoxes.size());
   
   //
   // add new combo boxes 
   //
   for (int i = oldNumSurfaces; i < numSurfaces; i++) {
      GuiBrainModelSelectionComboBox* modelComboBox =
                             new GuiBrainModelSelectionComboBox(
                                                false,
                                                true,
                                                false,
                                                "None",
                                                NULL);
      modelComboBox->setSelectedBrainModel(defaultSurfaces[i]);
      
      QLabel* label = new QLabel("");
      surfaceLabels.push_back(label);
      surfaceComboBoxes.push_back(modelComboBox); 
      
      surfaceGridLayout->addWidget(label, i, 0);
      surfaceGridLayout->addWidget(modelComboBox, i, 1);
   }
   
   //
   // Update/hide combo boxes
   //
   for (int i = 0; i < static_cast<int>(surfaceComboBoxes.size()); i++) {
      //
      // Update combo box
      //
      surfaceComboBoxes[i]->updateComboBox();
      
      if (i < numSurfaces) {
         //
         // show labels and combo box
         //
         surfaceLabels[i]->show();
         surfaceComboBoxes[i]->show();
         
         //
         // Update labels
         //
         if (i == 0) {
            surfaceLabels[i]->setText("First Surface");
         }
         else if (i == (numSurfaces - 1)) {
            surfaceLabels[i]->setText("Final Surface");
         }
         else {
            surfaceLabels[i]->setText(QString("Surface %1").arg(i + 1));
         }
         
         //
         // If brain set has changed
         //
         if (prevSpecLoadTime != specLoadTime) {
            if (i < static_cast<int>(defaultSurfaces.size())) {
               surfaceComboBoxes[i]->setSelectedBrainModel(defaultSurfaces[i]);
            }
         }
      }
      else {
         surfaceLabels[i]->hide();
         surfaceComboBoxes[i]->hide();
      }
   }

   //
   //
   // save for next time
   prevSpecLoadTime = specLoadTime;
}

/**
 * Called when apply button pressed.
 */
void
GuiInterpolateSurfacesDialog::slotApplyButton()
{
   //
   // Get the topology file to be used for interpolation
   //
   TopologyFile* topologyFile = topologyComboBox->getSelectedTopologyFile();
   if (topologyFile == NULL) {
      QMessageBox::critical(this, "Error", "You must have a topology file.");
      return;
   }
   
   //
   // Get the surfaces selected by the user
   //
   std::vector<BrainModelSurface*> inputSurfaces;  // DO NOT FREE AS POINT TO BRAIN SET
   std::vector<BrainModelSurface*> surfaces;       // FREE AT END OF METHOD
   for (unsigned int i = 0; i < surfaceComboBoxes.size(); i++) {
      BrainModelSurface* bms = surfaceComboBoxes[i]->getSelectedBrainModelSurface();
      if (bms != NULL) {
         inputSurfaces.push_back(bms);
         surfaces.push_back(new BrainModelSurface(*bms));
      }
   }
   
   //
   // Must have at least two surfaces
   //
   if (surfaces.size() < 2) {
      //
      // Free Memory
      //
      for (unsigned int k = 0; k < surfaces.size(); k++) {
         delete surfaces[k];
      }
      surfaces.clear();

      QMessageBox::critical(this, "Error", "You must select at least two surfaces.");
      
      return;
   }
   
   //
   // Apply scaling to the surfaces to that they fit the main window
   //
   for (unsigned int k = 0; k < surfaces.size(); k++) {
      //
      // Copy scaling since not copied as part of copy constructor
      //
      //float scale[3];
      //inputSurfaces[k]->getScaling(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
      //                             scale);
      //surfaces[k]->setScaling(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
      //                        scale);  
      //surfaces[k]->applyCurrentView(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
      //                              false,
      //                              false,
      //                              true);
      surfaces[k]->copyTransformations(inputSurfaces[k],
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
      surfaces[k]->applyCurrentView(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                    true,
                                    true,
                                    true);
   }
   
   //
   // Point to first surface and add it to the brain set temporarily
   //
   BrainModelSurface* surface = new BrainModelSurface(*(surfaces[0]));
   theMainWindow->getBrainSet()->addBrainModel(surface);
   const int numNodes = surface->getNumberOfNodes();
   CoordinateFile* surfaceCoords = surface->getCoordinateFile();

   //
   // Set the type of surface to the special type
   //
   surface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_UNSPECIFIED);

   //
   // Update any projected files
   //
   updateProjectedFiles(surface);
   
   //
   // Copy view from the first brain model
   //
   //surface->copyTransformations(surfaces[0],
   //                              BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
   //                              BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
   
   //
   // Display this surface in the main window
   //
   theMainWindow->displayBrainModelInMainWindow(surface);
   
   //
   // Add the topology file to the new surfaces
   //
   surface->setTopologyFile(topologyFile);
   
   //
   // Number of interpolation steps
   //
   const int numSteps = interpolationStepsSpinBox->value();
   const float stepSize = 1.0 / static_cast<float>(numSteps + 1);
   
   //
   // Interpolate between the surfaces
   //      
   for (int j = 0; j < static_cast<int>(surfaces.size()); j++) {  
      //
      // surface for start of this iteration
      //       
      const BrainModelSurface* iterationSurface = surfaces[j];
      const CoordinateFile* iterationCoords = iterationSurface->getCoordinateFile();
      
      //
      // Use transform of first surface for all except flat surfaces
      //
   /*
      if ((surfaces[j]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT) &&
          (surfaces[j]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         surface->copyTransformations(surfaces[0],
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
      }
      else {
         surface->copyTransformations(surfaces[j],
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
      }
   */
      
      //
      // load the coordinates of the "jTH" surface into the new surface
      //    
      for (int i = 0; i < numNodes; i++) {
         surfaceCoords->setCoordinate(i, iterationCoords->getCoordinate(i));
      }
      
      //
      // Update any projected files
      //
      updateProjectedFiles(surface);

      //
      // Get the type of surface
      //
      const BrainModelSurface::SURFACE_TYPES surfaceType = 
                                       iterationSurface->getSurfaceType();
      
      //
      // Does the user want the surface rotated ?
      //
      if (rotateSurfacesCheckBox->isChecked()) {
         //
         // Do not rotate a flat surface
         //
         if ((surfaceType != BrainModelSurface::SURFACE_TYPE_FLAT) &&
               (surfaceType != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
            //
            // Determine amount to rotate each increment so that a total
            // of 360 degrees of rotation is made
            //
            const int numRotationSteps = 
                  static_cast<int>(360.0 / rotationStepDoubleSpinBox->value());
            const float rotationDelta = 360.0 / numRotationSteps;
            
            //
            // Rotate the surface for the needed number of iterations
            //
            vtkTransform* matrix = surface->getRotationTransformMatrix(
                                    BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW);
            for (int j = 0; j < numRotationSteps; j++) {
               matrix->RotateY(rotationDelta);

               //
               // Display the surface
               //
               GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
            }
         }
      }
      else {
         //
         // Display the surface
         //
         GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
      }
      
      //
      // If not the last surface
      //
      if (j < static_cast<int>(surfaces.size() - 1)) {
         //
         // Compute the delta coordinates for interpolating
         //
         const BrainModelSurface* nextSurface = surfaces[j+1];
         const CoordinateFile* nextCoords = nextSurface->getCoordinateFile();
         CoordinateFile deltaCoords;
         deltaCoords.setNumberOfCoordinates(numNodes);
         for (int i = 0; i < numNodes; i++) {
            const float* p2 = nextCoords->getCoordinate(i);
            const float* p1 = surfaceCoords->getCoordinate(i);
            const float dxyz[3] = { 
                                    (p2[0] - p1[0]) * stepSize,
                                    (p2[1] - p1[1]) * stepSize,
                                    (p2[2] - p1[2]) * stepSize
                                  };
            deltaCoords.setCoordinate(i, dxyz);
         }
         
         //
         // Interpolate
         //
         for (int k = 0; k < numSteps; k++) {
            //
            // Update the surface normals
            //
            surface->computeNormals();
            
            //
            // Display the surface
            //
            GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
            
            //
            // Add deltas to the surface coordinates
            //
            for (int i = 0; i < numNodes; i++) {
               float xyz[3];
               surfaceCoords->getCoordinate(i, xyz);
               const float* dxyz = deltaCoords.getCoordinate(i);
               xyz[0] += dxyz[0];
               xyz[1] += dxyz[1];
               xyz[2] += dxyz[2];
               surfaceCoords->setCoordinate(i, xyz);
            }
            
            //
            // Update projected files
            //
            updateProjectedFiles(surface);
         }
      }
   }
   
   //
   // Set the current surface to the last surface
   //
   const int lastSurfaceIndex = static_cast<int>(surfaces.size() - 1);
//   if ((surfaces[lastSurfaceIndex]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT) &&
//       (surfaces[lastSurfaceIndex]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
//      surfaces[lastSurfaceIndex]->copyTransformations(surface, 0, 0);
//   }
   theMainWindow->displayBrainModelInMainWindow(inputSurfaces[lastSurfaceIndex]);
   
   //
   // Remove the temporary surface from the brain set and delete it
   //
   theMainWindow->getBrainSet()->deleteBrainModel(surface);
   GuiFilesModified fm;
   fm.setCoordinateModified();
   fm.setInhibitSurfaceDefaultScaling(); // do NOT scale surfaces for default scaling
   theMainWindow->fileModificationUpdate(fm);
   
   //
   // Update the display
   //
   GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());

   //
   // Free Memory
   //
   for (unsigned int k = 0; k < surfaces.size(); k++) {
      delete surfaces[k];
   }
   surfaces.clear();
}
/*
void
GuiInterpolateSurfacesDialog::slotApplyButton()
{
   //
   // Get the surfaces selected by the user
   //
   std::vector<BrainModelSurface*> surfaces;
   for (unsigned int i = 0; i < surfaceComboBoxes.size(); i++) {
      BrainModelSurface* bms = surfaceComboBoxes[i]->getSelectedBrainModelSurface();
      if (bms != NULL) {
         surfaces.push_back(bms);
      }
   }
   
   //
   // Must have at least two surfaces
   //
   if (surfaces.size() < 2) {
      GuiMessageBox::critical(this, "Error", "You must select at least two surfaces.", "OK");
      return;
   }
   
   //
   // Get the topology file to be used for interpolation
   //
   TopologyFile* topologyFile = topologyComboBox->getSelectedTopologyFile();
   if (topologyFile == NULL) {
      GuiMessageBox::critical(this, "Error", "You must have a topology file.", "OK");
      return;
   }
   
   //
   // Create a new brain model surface by copying the first surface
   //
   BrainModelSurface* surface = new BrainModelSurface(*(surfaces[0]));
   theMainWindow->getBrainSet()->addBrainModel(surface);
   const int numNodes = surface->getNumberOfNodes();
   CoordinateFile* surfaceCoords = surface->getCoordinateFile();

   //
   // Set the type of surface to the special type
   //
   surface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_UNSPECIFIED);

   //
   // Update any projected files
   //
   updateProjectedFiles(surface);
   
   //
   // Copy view from the first brain model
   //
   surface->copyTransformations(surfaces[0],
                                 BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                 BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
   
   //
   // Display this surface in the main window
   //
   theMainWindow->displayBrainModelInMainWindow(surface);
   
   //
   // Add the topology file to the new surfaces
   //
   surface->setTopologyFile(topologyFile);
   
   //
   // Number of interpolation steps
   //
   const int numSteps = interpolationStepsSpinBox->value();
   const float stepSize = 1.0 / static_cast<float>(numSteps + 1);
   
   //
   // Interpolate between the surfaces
   //      
   for (int j = 0; j < static_cast<int>(surfaces.size()); j++) {  
      //
      // surface for start of this iteration
      //       
      const BrainModelSurface* iterationSurface = surfaces[j];
      const CoordinateFile* iterationCoords = iterationSurface->getCoordinateFile();
      
      //
      // Use transform of first surface for all except flat surfaces
      //
      if ((surfaces[j]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT) &&
          (surfaces[j]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         surface->copyTransformations(surfaces[0],
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
      }
      else {
         surface->copyTransformations(surfaces[j],
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                       BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
      }
      
      //
      // load the coordinates of the "jTH" surface into the new surface
      //    
      for (int i = 0; i < numNodes; i++) {
         surfaceCoords->setCoordinate(i, iterationCoords->getCoordinate(i));
      }
      
      //
      // Update any projected files
      //
      updateProjectedFiles(surface);

      //
      // Get the type of surface
      //
      const BrainModelSurface::SURFACE_TYPES surfaceType = 
                                       iterationSurface->getSurfaceType();
      
      //
      // Does the user want the surface rotated ?
      //
      if (rotateSurfacesCheckBox->isChecked()) {
         //
         // Do not rotate a flat surface
         //
         if ((surfaceType != BrainModelSurface::SURFACE_TYPE_FLAT) &&
               (surfaceType != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
            //
            // Determine amount to rotate each increment so that a total
            // of 360 degrees of rotation is made
            //
            const int numRotationSteps = 
                  static_cast<int>(360.0 / rotationStepDoubleSpinBox->value());
            const float rotationDelta = 360.0 / numRotationSteps;
            
            //
            // Rotate the surface for the needed number of iterations
            //
            vtkTransform* matrix = surface->getRotationTransformMatrix(
                                    BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW);
            for (int j = 0; j < numRotationSteps; j++) {
               matrix->RotateY(rotationDelta);

               //
               // Display the surface
               //
               GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
            }
         }
      }
      else {
         //
         // Display the surface
         //
         GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
      }
      
      //
      // If not the last surface
      //
      if (j < static_cast<int>(surfaces.size() - 1)) {
         //
         // Compute the delta coordinates for interpolating
         //
         const BrainModelSurface* nextSurface = surfaces[j+1];
         const CoordinateFile* nextCoords = nextSurface->getCoordinateFile();
         CoordinateFile deltaCoords;
         deltaCoords.setNumberOfCoordinates(numNodes);
         for (int i = 0; i < numNodes; i++) {
            const float* p2 = nextCoords->getCoordinate(i);
            const float* p1 = surfaceCoords->getCoordinate(i);
            const float dxyz[3] = { 
                                    (p2[0] - p1[0]) * stepSize,
                                    (p2[1] - p1[1]) * stepSize,
                                    (p2[2] - p1[2]) * stepSize
                                  };
            deltaCoords.setCoordinate(i, dxyz);
         }
         
         //
         // Compute delta scaling for interpolating
         //
         float scaling[3];
         iterationSurface->getScaling(0, scaling);
         float nextScaling[3];
         nextSurface->getScaling(0, nextScaling);
         const float deltaScaling[3] = {
            (nextScaling[0] - scaling[0]) * stepSize,
            (nextScaling[1] - scaling[1]) * stepSize,
            (nextScaling[2] - scaling[2]) * stepSize,
         };
                  
         //
         // Interpolate
         //
         for (int k = 0; k < numSteps; k++) {
            //
            // Update the surface normals
            //
            surface->computeNormals();
            
            //
            // Display the surface
            //
            GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
            
            //
            // Add deltas to the surface coordinates
            //
            for (int i = 0; i < numNodes; i++) {
               float xyz[3];
               surfaceCoords->getCoordinate(i, xyz);
               const float* dxyz = deltaCoords.getCoordinate(i);
               xyz[0] += dxyz[0];
               xyz[1] += dxyz[1];
               xyz[2] += dxyz[2];
               surfaceCoords->setCoordinate(i, xyz);
            }
            
            //
            // Update default scaling
            //
            scaling[0] += deltaScaling[0];
            scaling[1] += deltaScaling[1];
            scaling[2] += deltaScaling[2];
            //surface->setScaling(0, scaling);

            //
            // Update projected files
            //
            updateProjectedFiles(surface);
         }
      }
   }
   
   //
   // Set the current surface to the last surface
   //
   const int lastSurfaceIndex = static_cast<int>(surfaces.size() - 1);
   if ((surfaces[lastSurfaceIndex]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT) &&
       (surfaces[lastSurfaceIndex]->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
      surfaces[lastSurfaceIndex]->copyTransformations(surface, 0, 0);
   }
   theMainWindow->displayBrainModelInMainWindow(surfaces[lastSurfaceIndex]);
   
   //
   // Remove the temporary surface from the brain set and delete it
   //
   theMainWindow->getBrainSet()->deleteBrainModel(surface);
   GuiFilesModified fm;
   fm.setCoordinateModified();
   theMainWindow->fileModificationUpdate(fm);
   
   //
   // Update the display
   //
   GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
}
*/

void
GuiInterpolateSurfacesDialog::updateProjectedFiles(const BrainModelSurface* bms)
{
   if (reprojectBordersCheckBox->isChecked()) {
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->unprojectBorders(bms);
   }
}
               

