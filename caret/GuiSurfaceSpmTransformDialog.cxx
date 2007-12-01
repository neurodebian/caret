
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

#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include "WuQFileDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiMainWindow.h"
#define __GUI_SURFACE_SPM_TRANSFORM_DIALOG_MAIN_H__
#include "GuiSurfaceSpmTransformDialog.h"
#undef __GUI_SURFACE_SPM_TRANSFORM_DIALOG_MAIN_H__
#include "MathUtilities.h"
#include "QtUtilities.h"
#include "VolumeFile.h"
#include "global_variables.h"

/*
 * Getting inverse files from SPM:
Using SPM99

Input volume: colin_7112B_111.hdr/img

Normalized volume produced by SPM  ncolin_7112B_111.hdr/img

Goal: Determine how to map coordinates in "input volume space" into coordinates in "normalized volume space".


After normalization:

Used toolbox: deformations: deformations from sn3d.mat

Specified the 

colin_7112B_111_sn3d.mat file
bounding box = Default
voxel sizes = 1, 1, 1

Toolbar deformations: Invert Deformations

Number of subjects = 1

Deformation field 1: selecte y1_colin_7112B_111.img y2_colin_7112B_111.img, y3_colin_7112B_111.img.

Image to Base Inverse on: colin_7112B_111.img


output is iy1_colin_7112B_111.hdr, iy2_colin_7112B_111.hdr, and iy3_colin_7112B_111.hdr


Use Display: Selected iy1_colin_7112B_111.img.
Changed World Space to Voxel Space
Entered 122  153  86   resulting intensity = 35.318


Use Display: Selected iy2_colin_7112B_111.img.
Changed World Space to Voxel Space
Entered 122  153  86   resulting intensity = 36.140


Use Display: Selected iy3_colin_7112B_111.img.
Changed World Space to Voxel Space
Entered 122  153  86   resulting intensity = 8.14


I assume (35.318, 36.140, 8.14) are relative to the SPM origin (79  113  51) obtained by running "mayo_analyze  ncolin_7112B_111.hdr".


Adding the spm origin to the gives  (114.318  149.1  59.14).

 */
 
/**
 * Constructor
 */
GuiSurfaceSpmTransformDialog::GuiSurfaceSpmTransformDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Apply SPM Inverse Transform");

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Layout for all dialog items
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->setSpacing(3);
   dialogLayout->addLayout(gridLayout);
   
   int rowNumber = 0;
   
   //
   // Coordinate file
   //
   gridLayout->addWidget(new QLabel("Coordinate File"), rowNumber, 0, Qt::AlignLeft);
   coordFileComboBox = new GuiBrainModelSelectionComboBox(false,
                                                          true,
                                                          false,
                                                          "",
                                                          0);
   coordFileComboBox->setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
   gridLayout->addWidget(coordFileComboBox, rowNumber, 1, Qt::AlignLeft);
   rowNumber++;
   
   const QString blanks("                            ");
   
   //
   // Normalized volume label and selection button
   //
   QPushButton* normalizedVolumePushButton = new QPushButton("Normalized Volume...");
   gridLayout->addWidget(normalizedVolumePushButton, rowNumber, 0);
   normalizedVolumePushButton->setAutoDefault(false);
   QObject::connect(normalizedVolumePushButton, SIGNAL(clicked()),
                    this, SLOT(slotNormalizedVolumeButton()));
   normalizedVolumeLabel = new QLabel(blanks);
   gridLayout->addWidget(normalizedVolumeLabel, rowNumber, 1, Qt::AlignLeft);
   rowNumber++;
   
   //
   // inverse X volume label and selection button
   //
   QPushButton* inverseXVolumePushButton = new QPushButton("Inverse X Volume...");
   gridLayout->addWidget(inverseXVolumePushButton, rowNumber, 0);
   inverseXVolumePushButton->setAutoDefault(false);
   QObject::connect(inverseXVolumePushButton, SIGNAL(clicked()),
                    this, SLOT(slotInverseXVolumeButton()));
   inverseXVolumeLabel = new QLabel(blanks);
   gridLayout->addWidget(inverseXVolumeLabel, rowNumber, 1, Qt::AlignLeft);
   rowNumber++;
   
   //
   // inverse Y volume label and selection button
   //
   QPushButton* inverseYVolumePushButton = new QPushButton("Inverse Y Volume...");
   gridLayout->addWidget(inverseYVolumePushButton, rowNumber, 0);
   inverseYVolumePushButton->setAutoDefault(false);
   QObject::connect(inverseYVolumePushButton, SIGNAL(clicked()),
                    this, SLOT(slotInverseYVolumeButton()));
   inverseYVolumeLabel = new QLabel(blanks);
   gridLayout->addWidget(inverseYVolumeLabel, rowNumber, 1, Qt::AlignLeft);
   rowNumber++;
   
   //
   // inverse Z volume label and selection button
   //
   QPushButton* inverseZVolumePushButton = new QPushButton("Inverse Z Volume...");
   gridLayout->addWidget(inverseZVolumePushButton, rowNumber, 0);
   inverseZVolumePushButton->setAutoDefault(false);
   QObject::connect(inverseZVolumePushButton, SIGNAL(clicked()),
                    this, SLOT(slotInverseZVolumeButton()));
   inverseZVolumeLabel = new QLabel(blanks);
   gridLayout->addWidget(inverseZVolumeLabel, rowNumber, 1, Qt::AlignLeft);
   rowNumber++;
   
   //
   // Interpolate voxels check box
   //
   interpolateVoxelsCheckBox = new QCheckBox("Interpolate Voxels");
   interpolateVoxelsCheckBox->setChecked(interpolateVoxelsFlag);
   dialogLayout->addWidget(interpolateVoxelsCheckBox);
   
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // ok button connects to QDialogs accept() slot.
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
        
   //
   // Cancel button connects to QDialogs close() slot.
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   displayFileNames();
}

/**
 * Destructor
 */
GuiSurfaceSpmTransformDialog::~GuiSurfaceSpmTransformDialog()
{
}

/**
 * Called when OK button pressed.
 */
void
GuiSurfaceSpmTransformDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QString msg;
      
      BrainModelSurface* bms = coordFileComboBox->getSelectedBrainModelSurface();
      if (bms == NULL) {
         msg.append("You must select a surface.\n");
      }
      if (normalizedVolumeFileName.isEmpty()) {
         msg.append("You must select a normalized volume.\n");
      }
      if (inverseXVolumeFileName.isEmpty()) {
         msg.append("You must select an inverse X volume.\n");
      }
      if (inverseYVolumeFileName.isEmpty()) {
         msg.append("You must select an inverse Y volume.\n");
      }
      if (inverseZVolumeFileName.isEmpty()) {
         msg.append("You must select an inverse Z volume.\n");
      }
      
      if (msg.isEmpty() == false) {
         QMessageBox::critical(this, "Error", msg);
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      interpolateVoxelsFlag = interpolateVoxelsCheckBox->isChecked();
      
      try {
         //
         // Read the header of the normalized volume file
         //
         VolumeFile normalizedVolumeFile;
         normalizedVolumeFile.readFile(normalizedVolumeFileName,
                                       VolumeFile::VOLUME_READ_HEADER_ONLY);
         float ac[3];
         normalizedVolumeFile.getSpmAcPosition(ac);
                                  
         //
         // Read the volume containing the inverse X deformations
         //
         VolumeFile inverseXVolumeFile;
         inverseXVolumeFile.readFile(inverseXVolumeFileName,
                                     0);
         
         //
         // Read the volume containing the inverse Y deformations
         //
         VolumeFile inverseYVolumeFile;
         inverseYVolumeFile.readFile(inverseYVolumeFileName,
                                     0);

         //
         // Read the volume containing the inverse Z deformations
         //
         VolumeFile inverseZVolumeFile;
         inverseZVolumeFile.readFile(inverseZVolumeFileName,
                                     0);

         //
         // Dimensions of one of the inverse volumes (should all be the same dimensions)
         //
         int dims[3];
         inverseZVolumeFile.getDimensions(dims);
         
         //
         // 1/2 size of voxels
         //
         float halfVoxelSize[3];
         inverseZVolumeFile.getSpacing(halfVoxelSize);
         halfVoxelSize[0] *= 0.5;
         halfVoxelSize[1] *= 0.5;
         halfVoxelSize[2] *= 0.5;
         
         //
         // Transform the coordinates
         //
         CoordinateFile* cf = bms->getCoordinateFile();
         const int numCoords = cf->getNumberOfCoordinates();
         for (int i = 0; i < numCoords; i++) {
            float xyz[3];
            cf->getCoordinate(i, xyz);
            
            //
            // Get the voxel coordinates
            //
            int ijk[3];
            float pcoords[3];
            const int insideVolume = inverseXVolumeFile.convertCoordinatesToVoxelIJK(xyz, ijk, pcoords);
            if (insideVolume) {
            
               bool edgeVoxel = false;
               if ((ijk[0] == 0) || (ijk[0] == (dims[0] - 1)) ||
                   (ijk[1] == 0) || (ijk[1] == (dims[1] - 1)) ||
                   (ijk[2] == 0) || (ijk[2] == (dims[2] - 1))) {
                  edgeVoxel = true;
               }
                     
               if (interpolateVoxelsFlag && (edgeVoxel == false)) {
                  //
                  // Offset position by half of the voxel size
                  //
                  const float vxyz[3] = { xyz[0] - halfVoxelSize[0], 
                                          xyz[1] - halfVoxelSize[1], 
                                          xyz[2] - halfVoxelSize[2] };
                  inverseXVolumeFile.convertCoordinatesToVoxelIJK((float*)vxyz, ijk, pcoords);
                  const float r = pcoords[0];
                  const float s = pcoords[1];
                  const float t = pcoords[2];
                  
                  xyz[0] = 0.0;
                  xyz[1] = 0.0;
                  xyz[2] = 0.0;
                  
                  //
                  // Weighting from book Visualization Toolkit, 2nd Ed, page 316
                  // 
                  for (int j = 0; j < 8; j++) {
                     int dijk[3] = { 0, 0, 0 };
                     float weight = 0.0;
                     switch(j) {
                        case 0:
                           weight = (1.0 - r) * (1.0 - s) * (1.0 - t);
                           break;
                        case 1:
                           weight = r * (1.0 - s) * (1.0 - t);
                           dijk[0] = 1;
                           break;
                        case 2:
                           weight = (1.0 - r) * s * (1.0 - t);
                           dijk[1] = 1;
                           break;
                        case 3:
                           weight = r * s * (1.0 - t);
                           dijk[0] = 1;
                           dijk[1] = 1;
                           break;
                        case 4:
                           weight = (1.0 - r) * (1.0 - s) * t;
                           dijk[2] = 1;
                           break;
                        case 5:
                           weight = r * (1.0 - s) * t;
                           dijk[0] = 1;
                           dijk[2] = 1;
                           break;
                        case 6:
                           weight = (1.0 - r) * s * t;
                           dijk[1] = 1;
                           dijk[2] = 1;
                           break;
                        case 7:
                           weight = r * s * t;
                           dijk[0] = 1;
                           dijk[1] = 1;
                           dijk[2] = 1;
                           break;
                     }
                     
                     //
                     // adjust the voxel indices
                     //
                     int vijk[3] = { ijk[0] + dijk[0], ijk[1] + dijk[1], ijk[2] + dijk[2] };
                     
                     xyz[0] += (inverseXVolumeFile.getVoxel(vijk, 0) + ac[0]) * weight;
                     xyz[1] += (inverseYVolumeFile.getVoxel(vijk, 0) + ac[1]) * weight;
                     xyz[2] += (inverseZVolumeFile.getVoxel(vijk, 0) + ac[2]) * weight;
                     
                     //
                     // For some voxels, the SPM Inverse has NaN (Not a Number)
                     //
                     if (MathUtilities::isNaN(xyz[0]) ||
                         MathUtilities::isNaN(xyz[1]) ||
                         MathUtilities::isNaN(xyz[2])) {
                        xyz[0] = 0.0;
                        xyz[1] = 0.0;
                        xyz[2] = 0.0;
                        break;
                     }
                  }  // for (i = 0; i < 8)
               }
               else {
                  xyz[0] = inverseXVolumeFile.getVoxel(ijk, 0) + ac[0];
                  xyz[1] = inverseYVolumeFile.getVoxel(ijk, 0) + ac[1];
                  xyz[2] = inverseZVolumeFile.getVoxel(ijk, 0) + ac[2];
                  
                  //
                  // For some voxels, the SPM Inverse has NaN (Not a Number)
                  //
                  if (MathUtilities::isNaN(xyz[0]) ||
                     MathUtilities::isNaN(xyz[1]) ||
                     MathUtilities::isNaN(xyz[2])) {
                     xyz[0] = 0.0;
                     xyz[1] = 0.0;
                     xyz[2] = 0.0;
                  }
               }
               
               cf->setCoordinate(i, xyz);
            }
            else {
               std::cout << "Info: " << xyz[0] << " " << xyz[1] << " " << xyz[2]
                         << " is outside volume." << std::endl;
            }
         }
         
      }
      catch(FileException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Error", e.whatQString());
         return;
      }
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
   QDialog::done(r);
}

/**
 * called when normalized volume select push button pressed
 */
void 
GuiSurfaceSpmTransformDialog::slotNormalizedVolumeButton()
{
   selectVolumeFile(FILE_SELECTION_VOLUME_NORMALIZED);
}
      
/**
 * called when inverse X volume select push button pressed
 */
void 
GuiSurfaceSpmTransformDialog::slotInverseXVolumeButton()
{
   selectVolumeFile(FILE_SELECTION_VOLUME_INVERSE_X);
}
      
/**
 * called when inverse Y volume select push button pressed
 */
void 
GuiSurfaceSpmTransformDialog::slotInverseYVolumeButton()
{
   selectVolumeFile(FILE_SELECTION_VOLUME_INVERSE_Y);
}
      
/**
 * called when inverse Z volume select push button pressed
 */
void 
GuiSurfaceSpmTransformDialog::slotInverseZVolumeButton()
{
   selectVolumeFile(FILE_SELECTION_VOLUME_INVERSE_Z);
}

/**
 * Called to select a file
 */
void
GuiSurfaceSpmTransformDialog::selectVolumeFile(const FILE_SELECTION_TYPE fst)
{
   QString caption;
   switch (fst) {
      case FILE_SELECTION_VOLUME_NORMALIZED:
         caption = "Normalized Volume File";
         break;
      case FILE_SELECTION_VOLUME_INVERSE_X:
         caption = "Inverse X Deformation Volume File";
         break;
      case FILE_SELECTION_VOLUME_INVERSE_Y:
         caption = "Inverse Y Deformation Volume File";
         break;
      case FILE_SELECTION_VOLUME_INVERSE_Z:
         caption = "Inverse Z Deformation Volume File";
         break;
   }
   
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   fd.setFilter("SPM Volume Files (*.hdr)");
   fd.setWindowTitle(caption);
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         const QString fn = fd.selectedFiles().at(0);
         switch (fst) {
            case FILE_SELECTION_VOLUME_NORMALIZED:
               normalizedVolumeFileName = fn;
               break;
            case FILE_SELECTION_VOLUME_INVERSE_X:
               inverseXVolumeFileName = fn;
               break;
            case FILE_SELECTION_VOLUME_INVERSE_Y:
               inverseYVolumeFileName = fn;
               break;
            case FILE_SELECTION_VOLUME_INVERSE_Z:
               inverseZVolumeFileName = fn;
               break;
         }
      }
   }
   
   displayFileNames();
}

/**
 * Called to select a file
 */
void
GuiSurfaceSpmTransformDialog::displayFileNames()
{
   normalizedVolumeLabel->setText(FileUtilities::basename(normalizedVolumeFileName));
   inverseXVolumeLabel->setText(FileUtilities::basename(inverseXVolumeFileName));
   inverseYVolumeLabel->setText(FileUtilities::basename(inverseYVolumeFileName));
   inverseZVolumeLabel->setText(FileUtilities::basename(inverseZVolumeFileName));
}
