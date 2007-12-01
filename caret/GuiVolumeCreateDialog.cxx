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
#include <sstream>

#include <QComboBox>
#include "WuQFileDialog.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QToolTip>

#include "BrainSet.h"
#include "FileFilters.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiVolumeCreateDialog.h"
#include <QDoubleSpinBox>
#include "QtRadioButtonSelectionDialog.h"
#include "QtUtilities.h"
#include "SpecFile.h"
#include "StereotaxicSpace.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiVolumeCreateDialog::GuiVolumeCreateDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Create Volume");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(3);
   dialogLayout->setSpacing(3);
   
   //
   // Volume type
   //
   QLabel* typeLabel = new QLabel("Type ");
   volumeTypeComboBox = new QComboBox;
   volumeTypeComboBox->addItem("Anatomy", VolumeFile::VOLUME_TYPE_ANATOMY);
   volumeTypeComboBox->addItem("Functional", VolumeFile::VOLUME_TYPE_FUNCTIONAL);
   volumeTypeComboBox->addItem("Paint", VolumeFile::VOLUME_TYPE_PAINT);
   volumeTypeComboBox->addItem("Probabilistic Atlas", VolumeFile::VOLUME_TYPE_PROB_ATLAS);
   volumeTypeComboBox->addItem("RGB Paint", VolumeFile::VOLUME_TYPE_RGB);
   volumeTypeComboBox->addItem("Segmentation", VolumeFile::VOLUME_TYPE_SEGMENTATION);
   
   //
   // Volume name
   //
   QLabel* nameLabel = new QLabel("Name ");
   nameLineEdit = new QLineEdit;
   static int nameCtr = 0;
   std::ostringstream str;
   str << "volume_"
       << nameCtr
       << "+orig"
       << SpecFile::getAfniVolumeFileExtension().toAscii().constData();
   nameLineEdit->setText(str.str().c_str());
   nameCtr++;
   
   //
   // Group box for name and type
   //
   QGroupBox* nameGroupBox = new QGroupBox("Voxel Attributes");
   dialogLayout->addWidget(nameGroupBox);
   QGridLayout* nameGroupLayout = new QGridLayout(nameGroupBox);
   nameGroupLayout->addWidget(typeLabel, 0, 0);
   nameGroupLayout->addWidget(volumeTypeComboBox, 0, 1);
   nameGroupLayout->addWidget(nameLabel, 1, 0);
   nameGroupLayout->addWidget(nameLineEdit, 1, 1);
   
   //
   // Dimensions
   //
   QLabel* dimensionsLabel = new QLabel("Dimensions ");
   xDimensionSpinBox = new QSpinBox;
   xDimensionSpinBox->setMinimum(1);
   xDimensionSpinBox->setMaximum(std::numeric_limits<int>::max());
   xDimensionSpinBox->setSingleStep(1);
   yDimensionSpinBox = new QSpinBox; 
   yDimensionSpinBox->setMinimum(1);
   yDimensionSpinBox->setMaximum(std::numeric_limits<int>::max());
   yDimensionSpinBox->setSingleStep(1);
   zDimensionSpinBox = new QSpinBox; 
   zDimensionSpinBox->setMinimum(1);
   zDimensionSpinBox->setMaximum(std::numeric_limits<int>::max());
   zDimensionSpinBox->setSingleStep(1);
    
   //
   // Voxel sizes
   //
   QLabel* voxelSizeLabel = new QLabel("Voxel Sizes ");
   xVoxelSizeDoubleSpinBox = new QDoubleSpinBox;
   xVoxelSizeDoubleSpinBox->setMinimum(0.0);
   xVoxelSizeDoubleSpinBox->setMaximum(50000.0);
   xVoxelSizeDoubleSpinBox->setSingleStep(1.0);
   xVoxelSizeDoubleSpinBox->setDecimals(3);
   xVoxelSizeDoubleSpinBox->setValue(1.0);
   yVoxelSizeDoubleSpinBox = new QDoubleSpinBox;
   yVoxelSizeDoubleSpinBox->setMinimum(0.0);
   yVoxelSizeDoubleSpinBox->setMaximum(50000.0);
   yVoxelSizeDoubleSpinBox->setSingleStep(1.0);
   yVoxelSizeDoubleSpinBox->setDecimals(3);
   yVoxelSizeDoubleSpinBox->setValue(1.0);
   zVoxelSizeDoubleSpinBox = new QDoubleSpinBox;
   zVoxelSizeDoubleSpinBox->setMinimum(0.0);
   zVoxelSizeDoubleSpinBox->setMaximum(50000.0);
   zVoxelSizeDoubleSpinBox->setSingleStep(1.0);
   zVoxelSizeDoubleSpinBox->setDecimals(3);
   zVoxelSizeDoubleSpinBox->setValue(1.0);
                         
   //
   // origin
   //
   QLabel* originLabel = new QLabel("Origin ");
   xOriginDoubleSpinBox = new QDoubleSpinBox;
   xOriginDoubleSpinBox->setMinimum(-50000.0);
   xOriginDoubleSpinBox->setMaximum(50000.0);
   xOriginDoubleSpinBox->setSingleStep(1.0);
   xOriginDoubleSpinBox->setDecimals(3);
   yOriginDoubleSpinBox = new QDoubleSpinBox;
   yOriginDoubleSpinBox->setMinimum(-50000.0);
   yOriginDoubleSpinBox->setMaximum(50000.0);
   yOriginDoubleSpinBox->setSingleStep(1.0);
   yOriginDoubleSpinBox->setDecimals(3);
   zOriginDoubleSpinBox = new QDoubleSpinBox;
   zOriginDoubleSpinBox->setMinimum(-50000.0);
   zOriginDoubleSpinBox->setMaximum(50000.0);
   zOriginDoubleSpinBox->setSingleStep(1.0);
   zOriginDoubleSpinBox->setDecimals(3);
       
   //
   // Set attributes using volume file
   //
   QPushButton* setAttsFromFilePushButton = new QPushButton("Set Attributes Using Volume File...");
   setAttsFromFilePushButton->setAutoDefault(false);
   QObject::connect(setAttsFromFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetAttsFromFilePushButton()));
   setAttsFromFilePushButton->setToolTip(
                 "This button displays a dialog for selecting a volume\n"
                 "file.  The volume's dimensions, voxel sizes, and origin\n"
                 "will be placed into the dialog.");
   
   //
   // set attributes using standard space
   //
   QPushButton* setAttsUsingSpacePushButton = new QPushButton("Set Attributes Using Standard Stereotaxic Space...");
   setAttsUsingSpacePushButton->setAutoDefault(false);
   QObject::connect(setAttsUsingSpacePushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetAttsUsingStandardSpace()));
   setAttsUsingSpacePushButton->setToolTip(
                 "This button displays a dialog for selecting a standard\n"
                 "space that will be used for setting the dimensions,\n"
                 "voxel sizes, and origin.");
   
   //
   // Grid for volume attributes
   //
   QGridLayout* volAttGridLayout = new QGridLayout;
   volAttGridLayout->addWidget(dimensionsLabel, 0, 0);
   volAttGridLayout->addWidget(xDimensionSpinBox, 0, 1);
   volAttGridLayout->addWidget(yDimensionSpinBox, 0, 2);
   volAttGridLayout->addWidget(zDimensionSpinBox, 0, 3);
   volAttGridLayout->addWidget(voxelSizeLabel, 1, 0);
   volAttGridLayout->addWidget(xVoxelSizeDoubleSpinBox, 1, 1);
   volAttGridLayout->addWidget(yVoxelSizeDoubleSpinBox, 1, 2);
   volAttGridLayout->addWidget(zVoxelSizeDoubleSpinBox, 1, 3);
   volAttGridLayout->addWidget(originLabel, 2, 0);
   volAttGridLayout->addWidget(xOriginDoubleSpinBox, 2, 1);
   volAttGridLayout->addWidget(yOriginDoubleSpinBox, 2, 2);
   volAttGridLayout->addWidget(zOriginDoubleSpinBox, 2, 3);
                           
   //
   // Group box for volume dimensions and stuff
   //
   QGroupBox* volAttGroupBox = new QGroupBox("Voxel Attributes");
   dialogLayout->addWidget(volAttGroupBox);
   QVBoxLayout* volAttLayout = new QVBoxLayout(volAttGroupBox);
   volAttLayout->addLayout(volAttGridLayout);
   volAttLayout->addWidget(setAttsFromFilePushButton);
   volAttLayout->addWidget(setAttsUsingSpacePushButton);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(3);
   dialogLayout->addLayout(buttonsLayout);
 
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   okButton->setAutoDefault(false);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   
   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));

   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * destructor.
 */
GuiVolumeCreateDialog::~GuiVolumeCreateDialog()
{
}
   
/**
 * called to set the volume attributes using a volume file.
 */
void 
GuiVolumeCreateDialog::slotSetAttsFromFilePushButton()
{
   //
   // Create a spec file dialog to select the spec file.
   //
   WuQFileDialog openVolumeFileDialog(this);
   openVolumeFileDialog.setModal(true);
   openVolumeFileDialog.setWindowTitle("Choose Volume File");
   openVolumeFileDialog.setFileMode(WuQFileDialog::ExistingFile);
   openVolumeFileDialog.setDirectory(QDir::currentPath());
   openVolumeFileDialog.setAcceptMode(WuQFileDialog::AcceptOpen);
   openVolumeFileDialog.setFilter(FileFilters::getVolumeGenericFileFilter());
   if (openVolumeFileDialog.exec() == QDialog::Accepted) {
      if (openVolumeFileDialog.selectedFiles().count() > 0) {
         const QString vname(openVolumeFileDialog.selectedFiles().at(0));
         if (vname.isEmpty() == false) {
            VolumeFile vf;
            try {
               vf.readFile(vname);
               float origin[3];
               float spacing[3];
               int dim[3];
               vf.getDimensions(dim);
               vf.getOrigin(origin);
               vf.getSpacing(spacing);
               
               xDimensionSpinBox->setValue(dim[0]);
               yDimensionSpinBox->setValue(dim[1]);
               zDimensionSpinBox->setValue(dim[2]);
               xOriginDoubleSpinBox->setValue(origin[0]);
               yOriginDoubleSpinBox->setValue(origin[1]);
               zOriginDoubleSpinBox->setValue(origin[2]);
               xVoxelSizeDoubleSpinBox->setValue(spacing[0]);
               yVoxelSizeDoubleSpinBox->setValue(spacing[1]);
               zVoxelSizeDoubleSpinBox->setValue(spacing[2]);
            }
            catch (FileException& e) {
               QMessageBox::critical(this, "Error Reading Volume", e.whatQString());
            }
         }
      }
   }
}

/**
 * called to set the volume attributes using a standard space.
 */
void 
GuiVolumeCreateDialog::slotSetAttsUsingStandardSpace()
{
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);
   std::vector<QString> spaceNames;
   
   for (unsigned int i = 0; i < spaces.size(); i++) {
      float spacing[3];
      spaces[i].getVoxelSize(spacing);
      std::ostringstream str;
      str << "Space: "
          << spaces[i].getName().toAscii().constData()
          << " voxel size: ("
          << spacing[0] << ", " 
          << spacing[1] << ", " 
          << spacing[2]
          << ")";
      spaceNames.push_back(str.str().c_str());
   }
    
   QtRadioButtonSelectionDialog rbsd(this,
                                     "Choose Standard Space",
                                     "Choose Standard Space",
                                     spaceNames,
                                     0);
   if (rbsd.exec() == QDialog::Accepted) {
      const int indx = rbsd.getSelectedItemIndex();
      if ((indx >= 0) && (indx < static_cast<int>(spaces.size()))) {
         QString name;
         int dim[3];
         float origin[3];
         float spacing[3];
         spaces[indx].getOrigin(origin);
         spaces[indx].getVoxelSize(spacing);
         spaces[indx].getDimensions(dim);
         xDimensionSpinBox->setValue(dim[0]);
         yDimensionSpinBox->setValue(dim[1]);
         zDimensionSpinBox->setValue(dim[2]);
         xVoxelSizeDoubleSpinBox->setValue(spacing[0]);
         yVoxelSizeDoubleSpinBox->setValue(spacing[1]);
         zVoxelSizeDoubleSpinBox->setValue(spacing[2]);
         xOriginDoubleSpinBox->setValue(origin[0]);
         yOriginDoubleSpinBox->setValue(origin[1]);
         zOriginDoubleSpinBox->setValue(origin[2]);
      }
   }
}

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiVolumeCreateDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QString errorMessage;
      
      //
      // Check file name
      //
      const QString name(nameLineEdit->text());
      if (name.isEmpty()) {
         errorMessage.append("Filename must not be empty.\n");
      }
      
      //
      // Get and verify dimensions
      //
      const int dim[3] = {
         xDimensionSpinBox->value(),
         yDimensionSpinBox->value(),
         zDimensionSpinBox->value()
      };
      for (int i = 0; i < 3; i++) {
         if (dim[i] <= 0) {
            errorMessage.append("Dimensions must be greater than zero.\n");
            break;
         }
      }
      
      //
      // Get and verify voxel sizes
      //
      const float voxSize[3] = {
         xVoxelSizeDoubleSpinBox->value(),
         yVoxelSizeDoubleSpinBox->value(),
         zVoxelSizeDoubleSpinBox->value()
      };
      for (int i = 0; i < 3; i++) {
         if (voxSize[i] <= 0) {
            errorMessage.append("Voxel sizes must be greater than zero.\n");
            break;
         }
      }
      
      //
      // Get the origin
      //
      const float origin[3] = {
         xOriginDoubleSpinBox->value(),
         yOriginDoubleSpinBox->value(),
         zOriginDoubleSpinBox->value()
      };
      
      //
      // See if there are error
      //
      if (errorMessage.isEmpty() == false) {
         QMessageBox::critical(this, "ERROR", errorMessage);
         return;
      }
      
      //
      // Create the volume file
      //
      VolumeFile* vf = new VolumeFile;
      const VolumeFile::ORIENTATION orient[3] = {
         VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
         VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
         VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR
      };
      vf->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT,
                     dim,
                     orient,
                     origin,
                     voxSize,
                     true,
                     true);
      vf->setVolumeType(static_cast<VolumeFile::VOLUME_TYPE>(volumeTypeComboBox->currentIndex()));
      if (vf->getVolumeType() == VolumeFile::VOLUME_TYPE_PAINT) {
         vf->addRegionName("???");
      }
      vf->setFileName(name);
      
      //
      // Add the volume to the brain set
      //
      theMainWindow->getBrainSet()->addVolumeFile(vf->getVolumeType(),
                           vf,
                           name,
                           true,
                           false);
                           
      //
      // Update GUI
      //
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
   
   QDialog::done(r);
}


