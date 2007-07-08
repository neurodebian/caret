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

#include <qapplication.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>

#include "GuiMapFmriAlgorithmDialog.h"
#include "map_fmri_global_variables.h"

/** 
 * Constructor
 */
GuiMapFmriAlgorithmDialog::GuiMapFmriAlgorithmDialog(QWidget* parent)
   : QDialog(parent, "GuIMapFmriAlgorithmDialog", true)
{
   setCaption("fMRI Mapping Parameters");

   QVBoxLayout* layout = new QVBoxLayout(this, 5, 5, "layout");
   
   //
   // Vertical Groupbox for algorithm selection
   //
   QVGroupBox* algorithmGroupBox = new QVGroupBox("Mapping Algorithm", this, "atlasGroupBox");
   layout->addWidget(algorithmGroupBox);
   
   //
   // Combo box for algorithms
   //
   algorithmSelectionComboBox = new QComboBox(algorithmGroupBox, "atlasSelectionComboBox");
   QObject::connect(algorithmSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotAlgorithmSelection(int)));
   algorithmSelectionComboBox->insertItem("Average Nodes", 
                                          FMRIDataMapper::ALGORITHM_AVERAGE_NODES);
   algorithmSelectionComboBox->insertItem("Average Voxel", 
                                          FMRIDataMapper::ALGORITHM_AVERAGE_VOXEL);
   algorithmSelectionComboBox->insertItem("Gaussian", 
                                          FMRIDataMapper::ALGORITHM_GAUSSIAN);
   algorithmSelectionComboBox->insertItem("Maximum Voxel", 
                                          FMRIDataMapper::ALGORITHM_MAX_VOXEL);
   algorithmSelectionComboBox->insertItem("MCW Brainfish", 
                                          FMRIDataMapper::ALGORITHM_MCW_BRAINFISH);
   algorithmSelectionComboBox->setCurrentItem(static_cast<int>(fmriMapper.getMappingAlgorithm()));
   
   //
   // Vertical Groupbox for algorithm parameters section
   //
   QVGroupBox* parametersGroupBox = new QVGroupBox("Algorithm Parameters", this, 
                                                   "parametersGroupBox");
   layout->addWidget(parametersGroupBox);
   
   //
   // Grid layout for parameters
   //
   QWidget* parametersWidget = new QWidget(parametersGroupBox, "parametersWidget");
   QGridLayout* parametersLayout = new QGridLayout(parametersWidget, 8, 2, 3, 3, "parametersLayout");
   
   const int lineEditWidth = 60;
   //
   // Neighbors 
   //
   neighborsLabel = new QLabel("Neighbors", parametersWidget);
   parametersLayout->addWidget(neighborsLabel, 0, 0);
   neighborsLineEdit = new QLineEdit(parametersWidget);
   neighborsLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(neighborsLineEdit, 0, 1);
   
   //
   // Sigma Norm
   //
   sigmaNormLabel = new QLabel("Sigma Norm", parametersWidget);
   parametersLayout->addWidget(sigmaNormLabel, 1, 0);
   sigmaNormLineEdit = new QLineEdit(parametersWidget);
   sigmaNormLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(sigmaNormLineEdit, 1, 1);
   
   //
   // Sigma Tang
   //
   sigmaTangLabel = new QLabel("Sigma Tang", parametersWidget);
   parametersLayout->addWidget(sigmaTangLabel, 2, 0);
   sigmaTangLineEdit = new QLineEdit(parametersWidget);
   sigmaTangLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(sigmaTangLineEdit, 2, 1);
   
   //
   // Norm Below Cutoff
   //
   normBelowCutoffLabel = new QLabel("Norm Below Cutoff", parametersWidget);
   parametersLayout->addWidget(normBelowCutoffLabel, 3, 0);
   normBelowCutoffLineEdit = new QLineEdit(parametersWidget);
   normBelowCutoffLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(normBelowCutoffLineEdit, 3, 1);
   
   //
   // Norm Above Cutoff
   //
   normAboveCutoffLabel = new QLabel("Norm Above Cutoff", parametersWidget);
   parametersLayout->addWidget(normAboveCutoffLabel, 4, 0);
   normAboveCutoffLineEdit = new QLineEdit(parametersWidget);
   normAboveCutoffLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(normAboveCutoffLineEdit, 4, 1);
   
   //
   // Tang Cutoff
   //
   tangCutoffLabel = new QLabel("Tang Cutoff", parametersWidget);
   parametersLayout->addWidget(tangCutoffLabel, 5, 0);
   tangCutoffLineEdit = new QLineEdit(parametersWidget);
   tangCutoffLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(tangCutoffLineEdit, 5, 1);
   
   //
   // Max Distance
   //
   maxDistanceLabel = new QLabel("Max Distance", parametersWidget);
   parametersLayout->addWidget(maxDistanceLabel, 6, 0);
   maxDistanceLineEdit = new QLineEdit(parametersWidget);
   maxDistanceLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(maxDistanceLineEdit, 6, 1);
   
   //
   // Splat factor
   //
   splatFactorLabel = new QLabel("Splat Factor", parametersWidget);
   parametersLayout->addWidget(splatFactorLabel, 7, 0);
   splatFactorLineEdit = new QLineEdit(parametersWidget);
   splatFactorLineEdit->setFixedWidth(lineEditWidth);
   parametersLayout->addWidget(splatFactorLineEdit, 7, 1);
   
   //
   // Dialog button(s)
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout(layout, 5, "buttonsLayout");
   QPushButton* okButton = new QPushButton("OK", this);
   okButton->setFixedSize(okButton->sizeHint());
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   
   loadAlgorithmParameters();
   enableParameters();
}

/** 
 * Destructor
 */
GuiMapFmriAlgorithmDialog::~GuiMapFmriAlgorithmDialog()
{
}

/**
 * Called when users closes dialog.
 */
void
GuiMapFmriAlgorithmDialog::done(int r)
{
   if (r == Accepted) {
      readAlgorithmParameters();
      
      if (fmriMapper.getMappingAlgorithm() == FMRIDataMapper::ALGORITHM_GAUSSIAN) {
         if (fmriMapper.getNeighborLayers() < 5) {
            QApplication::beep();
            if (QMessageBox::warning(this, "Neighbors Warning",
                                     "With the Gaussian Algorithm, the neighbors\n"
                                     "parameter should probably be at least 5.",
                                     "Accept Parameters", "Change Parameters") != 0) {
               return;
            }
         }
      }
   }
   QDialog::done(r);
}

/**
 * Called to load the algorithm parameters.
 */
void
GuiMapFmriAlgorithmDialog::loadAlgorithmParameters()
{
   neighborsLineEdit->setText(QString::number(fmriMapper.getNeighborLayers()));
   
   float sigmaNorm, sigmaTang, normBelowCutoff, normAboveCutoff, tangCutoff;
   fmriMapper.getGaussianParameters(sigmaNorm,
                                    sigmaTang,
                                    normBelowCutoff,
                                    normAboveCutoff,
                                    tangCutoff);
   sigmaNormLineEdit->setText(QString::number(sigmaNorm, 'f', 3));
   sigmaTangLineEdit->setText(QString::number(sigmaTang, 'f', 3));
   normBelowCutoffLineEdit->setText(QString::number(normBelowCutoff, 'f', 3));
   normAboveCutoffLineEdit->setText(QString::number(normAboveCutoff, 'f', 3));
   tangCutoffLineEdit->setText(QString::number(tangCutoff, 'f', 3));
   
   float maxDistance;
   int splatFactor;
   fmriMapper.getBrainfishParameters(maxDistance, splatFactor);
   maxDistanceLineEdit->setText(QString::number(maxDistance, 'f', 3));
   splatFactorLineEdit->setText(QString::number(splatFactor));
}

/**
 * Enable the parametes based upon the algorithm.
 */
void
GuiMapFmriAlgorithmDialog::enableParameters()
{
   neighborsLabel->setEnabled(false);
   sigmaNormLabel->setEnabled(false);
   sigmaTangLabel->setEnabled(false);
   normBelowCutoffLabel->setEnabled(false);
   normAboveCutoffLabel->setEnabled(false);
   tangCutoffLabel->setEnabled(false);
   maxDistanceLabel->setEnabled(false);
   splatFactorLabel->setEnabled(false);
   neighborsLineEdit->setEnabled(false);
   sigmaNormLineEdit->setEnabled(false);
   sigmaTangLineEdit->setEnabled(false);
   normBelowCutoffLineEdit->setEnabled(false);
   normAboveCutoffLineEdit->setEnabled(false);
   tangCutoffLineEdit->setEnabled(false);
   maxDistanceLineEdit->setEnabled(false);
   splatFactorLineEdit->setEnabled(false);
   
   switch(fmriMapper.getMappingAlgorithm()) {
      case FMRIDataMapper::ALGORITHM_AVERAGE_NODES:
         break;
      case FMRIDataMapper::ALGORITHM_AVERAGE_VOXEL:
         neighborsLineEdit->setEnabled(true);
         neighborsLabel->setEnabled(true);
         break;
      case FMRIDataMapper::ALGORITHM_GAUSSIAN:
         neighborsLineEdit->setEnabled(true);
         sigmaNormLineEdit->setEnabled(true);
         sigmaTangLineEdit->setEnabled(true);
         normBelowCutoffLineEdit->setEnabled(true);
         normAboveCutoffLineEdit->setEnabled(true);
         tangCutoffLineEdit->setEnabled(true);
         neighborsLabel->setEnabled(true);
         sigmaNormLabel->setEnabled(true);
         sigmaTangLabel->setEnabled(true);
         normBelowCutoffLabel->setEnabled(true);
         normAboveCutoffLabel->setEnabled(true);
         tangCutoffLabel->setEnabled(true);
         break;
      case FMRIDataMapper::ALGORITHM_MAX_VOXEL:
         neighborsLineEdit->setEnabled(true);
         neighborsLabel->setEnabled(true);
         break;
      case FMRIDataMapper::ALGORITHM_MCW_BRAINFISH:
         maxDistanceLabel->setEnabled(true);
         splatFactorLabel->setEnabled(true);
         maxDistanceLineEdit->setEnabled(true);
         splatFactorLineEdit->setEnabled(true);
         break;
   }
   
}

/**
 * Called to read the algorithm parameters.
 */
void
GuiMapFmriAlgorithmDialog::readAlgorithmParameters()
{
   fmriMapper.setNeighborLayers(neighborsLineEdit->text().toInt());
   fmriMapper.setGaussianParameters(sigmaNormLineEdit->text().toFloat(),
                                    sigmaTangLineEdit->text().toFloat(),
                                    normBelowCutoffLineEdit->text().toFloat(),
                                    normAboveCutoffLineEdit->text().toFloat(),
                                    tangCutoffLineEdit->text().toFloat());
   fmriMapper.setBrainfishParameters(maxDistanceLineEdit->text().toFloat(),
                                     splatFactorLineEdit->text().toInt());
}

/**
 * Called when an algorithm is selected.
 */
void
GuiMapFmriAlgorithmDialog::slotAlgorithmSelection(int item)
{
   fmriMapper.setMappingAlgorithm(static_cast<FMRIDataMapper::ALGORITHM_TYPE>(item));
   enableParameters();
}

