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


#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>

#include "GuiMapFmriIndivParametersDialog.h"
#include "QtUtilities.h"
#include "map_fmri_global_variables.h"

/** 
 * Constructor
 */
GuiMapFmriIndivParametersDialog::GuiMapFmriIndivParametersDialog(QWidget* parent)
   : QDialog(parent, "GuiMapFmriIndivParametersDialog", true)
{
   setCaption("Individual Parameters");

   QVBoxLayout* layout = new QVBoxLayout(this, 5, 5, "layout");
   
   //
   // Vertical Groupbox for cropping selection
   //
   QVGroupBox* croppingGroupBox = new QVGroupBox("Cropping", this, "croppingGroupBox");
   layout->addWidget(croppingGroupBox);
   
   const int lineEditWidth = 60;
   
   //
   // Enable cropping check button
   // 
   enableCroppingCheckBox = new QCheckBox("Enable Cropping", croppingGroupBox,
                                          "enableCroppingCheckBox");
   QObject::connect(enableCroppingCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotCroppingCheckBox()));
   
   //
   // Horizontal box for cropping origin label and line edits
   //
   croppingOriginHBox = new QHBox(croppingGroupBox, "croppingOrientHBox");
   
   //
   // cropping origin label and line edits
   //
   new QLabel("Origin", croppingOriginHBox);
   croppingOriginXLineEdit = new QLineEdit(croppingOriginHBox, "croppingOriginXLineEdit");
   croppingOriginXLineEdit->setFixedWidth(lineEditWidth);
   croppingOriginYLineEdit = new QLineEdit(croppingOriginHBox, "croppingOriginYLineEdit");
   croppingOriginYLineEdit->setFixedWidth(lineEditWidth);
   croppingOriginZLineEdit = new QLineEdit(croppingOriginHBox, "croppingOriginZLineEdit");
   croppingOriginZLineEdit->setFixedWidth(lineEditWidth);
   
   //
   // Vertical Groupbox for padding selection
   //
   QVGroupBox* paddingGroupBox = new QVGroupBox("Padding", this, "paddingGroupBox");
   layout->addWidget(paddingGroupBox);
   
   //
   // Enable padding check button
   // 
   enablePaddingCheckBox = new QCheckBox("Enable Padding", paddingGroupBox,
                                         "enablePaddingCheckBox");
   QObject::connect(enablePaddingCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotPaddingCheckBox()));
   
   //
   // Widget and grid layout for padding
   //
   paddingWidget = new QWidget(paddingGroupBox, "paddingWidget");
   QGridLayout* paddingGrid = new QGridLayout(paddingWidget, 2, 4, 3, 3, "paddingGrid");
   
   //
   // negative padding labels and line edits
   //
   paddingGrid->addWidget(new QLabel("Negative", paddingWidget), 0, 0);
   negativeXPaddingLineEdit = new QLineEdit(paddingWidget, "negativeXPaddingLineEdit");
   negativeXPaddingLineEdit->setFixedWidth(lineEditWidth);
   paddingGrid->addWidget(negativeXPaddingLineEdit, 0, 1);
   negativeYPaddingLineEdit = new QLineEdit(paddingWidget, "negativeYPaddingLineEdit");
   negativeYPaddingLineEdit->setFixedWidth(lineEditWidth);
   paddingGrid->addWidget(negativeYPaddingLineEdit, 0, 2);
   negativeZPaddingLineEdit = new QLineEdit(paddingWidget, "negativeZPaddingLineEdit");
   negativeZPaddingLineEdit->setFixedWidth(lineEditWidth);
   paddingGrid->addWidget(negativeZPaddingLineEdit, 0, 3);
   
   //
   // positive padding labels and line edits
   //
   paddingGrid->addWidget(new QLabel("Positive", paddingWidget), 1, 0);
   positiveXPaddingLineEdit = new QLineEdit(paddingWidget, "positiveXPaddingLineEdit");
   positiveXPaddingLineEdit->setFixedWidth(lineEditWidth);
   paddingGrid->addWidget(positiveXPaddingLineEdit, 1, 1);
   positiveYPaddingLineEdit = new QLineEdit(paddingWidget, "positiveYPaddingLineEdit");
   positiveYPaddingLineEdit->setFixedWidth(lineEditWidth);
   paddingGrid->addWidget(positiveYPaddingLineEdit, 1, 2);
   positiveZPaddingLineEdit = new QLineEdit(paddingWidget, "positiveZPaddingLineEdit");
   positiveZPaddingLineEdit->setFixedWidth(lineEditWidth);
   paddingGrid->addWidget(positiveZPaddingLineEdit, 1, 3);
   
   //
   // Vertical Groupbox for Orientation selection
   //
   QVGroupBox* orientationGroupBox = new QVGroupBox("Orientation", this, "orientationGroupBox");
   layout->addWidget(orientationGroupBox);
   
   //
   // Widget and grid layout for orientation
   //
   QWidget* orientWidget = new QWidget(orientationGroupBox, "orientWidget");
   QGridLayout* orientGrid = new QGridLayout(orientWidget, 3, 2, 3, 3, "orientGrid");
   
   //
   // X Orientation
   //
   orientGrid->addWidget(new QLabel("Negative X ", orientWidget), 0, 0);
   xOrientationComboBox = new QComboBox(orientWidget, "xOrientationComboBox");
   xOrientationComboBox->insertItem("Left (Anatomical)", 
                                    FMRIDataMapper::X_NEGATIVE_ORIENTATION_LEFT);
   xOrientationComboBox->insertItem("Right (Radiological)", 
                                    FMRIDataMapper::X_NEGATIVE_ORIENTATION_RIGHT);
   orientGrid->addWidget(xOrientationComboBox, 0, 1);
   
   //
   // Y Orientation
   //
   orientGrid->addWidget(new QLabel("Negative Y ", orientWidget), 1, 0);
   yOrientationComboBox = new QComboBox(orientWidget, "yOrientationComboBox");
   yOrientationComboBox->insertItem("Posterior", 
                                    FMRIDataMapper::Y_NEGATIVE_ORIENTATION_POSTERIOR);
   yOrientationComboBox->insertItem("Anterior", 
                                    FMRIDataMapper::Y_NEGATIVE_ORIENTATION_ANTERIOR);
   orientGrid->addWidget(yOrientationComboBox, 1, 1);
   
   //
   // Z Orientation
   //
   orientGrid->addWidget(new QLabel("Negative Z ", orientWidget), 2, 0);
   zOrientationComboBox = new QComboBox(orientWidget, "zOrientationComboBox");
   zOrientationComboBox->insertItem("Inferior", 
                                    FMRIDataMapper::Z_NEGATIVE_ORIENTATION_INFERIOR);
   zOrientationComboBox->insertItem("Superior", 
                                    FMRIDataMapper::Z_NEGATIVE_ORIENTATION_SUPERIOR);
   orientGrid->addWidget(zOrientationComboBox, 2, 1);
   
   //
   // Dialog button(s)
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout(layout, 5, "buttonsLayout");
   QPushButton* okButton = new QPushButton("OK", this);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   QPushButton* paramsPushButton = new QPushButton("Params...", this);
   buttonsLayout->addWidget(paramsPushButton);
   QObject::connect(paramsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotParamsButton()));
                    
   QtUtilities::makeButtonsSameSize(okButton, paramsPushButton);
   
   loadParameters();
}

/** 
 * Destructor
 */
GuiMapFmriIndivParametersDialog::~GuiMapFmriIndivParametersDialog()
{
}

/**
 * slot called when cropping check box activated
 */
void
GuiMapFmriIndivParametersDialog::slotCroppingCheckBox()
{
   fmriMapper.setIndivCroppingEnabled(enableCroppingCheckBox->isChecked());
   enableDialogItems();
}

/**
 * slot called when cropping check box activated
 */
void
GuiMapFmriIndivParametersDialog::slotPaddingCheckBox()
{
   fmriMapper.setIndivPaddingEnabled(enablePaddingCheckBox->isChecked());
   enableDialogItems();
}

/**
 * Enable items based upon selections
 */
void
GuiMapFmriIndivParametersDialog::enableDialogItems()
{
   const bool cropEnabled = fmriMapper.getIndivCroppingEnabled();
   croppingOriginHBox->setEnabled(cropEnabled);

   const bool padEnabled = fmriMapper.getIndivPaddingEnabled();
   paddingWidget->setEnabled(padEnabled);
}

/**
 * load paramters into the dialog.
 */
void
GuiMapFmriIndivParametersDialog::loadParameters()
{
   //
   // Load cropping
   //
   bool croppingEnabled = false;
   int cropX, cropY, cropZ;
   fmriMapper.getIndivCroppingOffset(croppingEnabled, cropX, cropY, cropZ);
   enableCroppingCheckBox->setChecked(croppingEnabled);
   croppingOriginXLineEdit->setText(QString::number(cropX));
   croppingOriginYLineEdit->setText(QString::number(cropY));
   croppingOriginZLineEdit->setText(QString::number(cropZ));
   
   //
   // Load surface padding 
   //
   bool paddingEnabled = false;
   int negX, negY, negZ, posX, posY, posZ;
   fmriMapper.getIndivPadding(paddingEnabled, negX, negY, negZ, posX, posY, posZ);
   enablePaddingCheckBox->setChecked(paddingEnabled);
   negativeXPaddingLineEdit->setText(QString::number(negX));
   negativeYPaddingLineEdit->setText(QString::number(negY));
   negativeZPaddingLineEdit->setText(QString::number(negZ));
   positiveXPaddingLineEdit->setText(QString::number(posX));
   positiveYPaddingLineEdit->setText(QString::number(posY));
   positiveZPaddingLineEdit->setText(QString::number(posZ));
   
   //
   // Load orientation
   //
   FMRIDataMapper::X_NEGATIVE_ORIENTATION_TYPE xOrient;
   FMRIDataMapper::Y_NEGATIVE_ORIENTATION_TYPE yOrient;
   FMRIDataMapper::Z_NEGATIVE_ORIENTATION_TYPE zOrient;
   fmriMapper.getIndivSurfaceOrientation(xOrient, yOrient, zOrient);
   xOrientationComboBox->setCurrentItem(
      static_cast<FMRIDataMapper::X_NEGATIVE_ORIENTATION_TYPE>(xOrient));
   yOrientationComboBox->setCurrentItem(
      static_cast<FMRIDataMapper::Y_NEGATIVE_ORIENTATION_TYPE>(yOrient));
   zOrientationComboBox->setCurrentItem(
      static_cast<FMRIDataMapper::Z_NEGATIVE_ORIENTATION_TYPE>(zOrient));
      
   enableDialogItems();
}

/**
 * read parameters from the dialog.
 */
void
GuiMapFmriIndivParametersDialog::readParameters()
{
   fmriMapper.setIndivCroppingOffset(enableCroppingCheckBox->isChecked(),
                                     croppingOriginXLineEdit->text().toInt(),
                                     croppingOriginYLineEdit->text().toInt(),
                                     croppingOriginZLineEdit->text().toInt());
   fmriMapper.setIndivPadding(enablePaddingCheckBox->isChecked(),
                              negativeXPaddingLineEdit->text().toInt(),
                              negativeYPaddingLineEdit->text().toInt(),
                              negativeZPaddingLineEdit->text().toInt(),
                              positiveXPaddingLineEdit->text().toInt(),
                              positiveYPaddingLineEdit->text().toInt(),
                              positiveZPaddingLineEdit->text().toInt());
   fmriMapper.setIndivSurfaceOrientation(
      static_cast<FMRIDataMapper::X_NEGATIVE_ORIENTATION_TYPE>(xOrientationComboBox->currentItem()),
      static_cast<FMRIDataMapper::Y_NEGATIVE_ORIENTATION_TYPE>(yOrientationComboBox->currentItem()),
      static_cast<FMRIDataMapper::Z_NEGATIVE_ORIENTATION_TYPE>(zOrientationComboBox->currentItem()));
}

/**
 * Called when params button pressed.
 */
void
GuiMapFmriIndivParametersDialog::slotParamsButton()
{
   QFileDialog fd(this, "params-file-dialog", true);
   fd.setCaption("Choose Params File");
   fd.setFilter("Params File (*.params)");
   fd.setMode(QFileDialog::ExistingFile);
   if (fd.exec() == QDialog::Accepted) {
      const std::string fileName(fd.selectedFile().latin1());
      if (fileName.empty() == false) {
         fmriMapper.loadParamsFile(fileName);
         loadParameters();
      }
   }
}

/**
 * Called when users closes dialog.
 */
void
GuiMapFmriIndivParametersDialog::done(int r)
{
   if (r == Accepted) {
      readParameters();
   }
   QDialog::done(r);
}

