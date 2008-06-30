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

#include <QButtonGroup>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>

#include "BrainModelSurfacePointLocator.h"
#include "BrainSet.h"
#include "CellProjectionFile.h"
#include "FileUtilities.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiCellAndFociAttributeAssignmentDialog.h"
#include "GuiMainWindow.h"
#include "MathUtilities.h"
#include "PaintFile.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCellAndFociAttributeAssignmentDialog::GuiCellAndFociAttributeAssignmentDialog(QWidget* parent, 
                                                       const bool fociFlagIn)
   : WuQDialog(parent)
{
   fociFlag = fociFlagIn;
   typeString = "Cell";
   if (fociFlag) {
      typeString = "Foci";
   }

   setWindowTitle(typeString + " Attribute Assignment");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   
   //
   // Create surface section
   //
   dialogLayout->addWidget(createSurfaceSection());
   
   //
   // Create assigment options section
   //
   dialogLayout->addWidget(createAssignmentSection());
   
   //
   // Create cell/foci section
   //
   dialogLayout->addWidget(createCellFociSection(typeString));
   
   //
   // Create the paint section
   //
   QWidget* paintWidget = createPaintSection();
   if (paintWidget != NULL) {
      dialogLayout->addWidget(paintWidget);
   }
   
   dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Apply 
                                          | QDialogButtonBox::Close);
   QObject::connect(dialogButtonBox, SIGNAL(clicked(QAbstractButton*)),
                    this, SLOT(slotPushButton(QAbstractButton*)));
   dialogLayout->addWidget(dialogButtonBox);
   
   updateDialog();
}
                           
/**
 * destructor.
 */
GuiCellAndFociAttributeAssignmentDialog::~GuiCellAndFociAttributeAssignmentDialog()
{
}

/**
 * update the dialog.
 */
void 
GuiCellAndFociAttributeAssignmentDialog::updateDialog()
{
   leftHemSelectionComboBox->updateComboBox();
   rightHemSelectionComboBox->updateComboBox();
   cerebellumSelectionComboBox->updateComboBox();
   updatePaintColumnSection();
}
   
/**
 * create the surface section.
 */
QWidget* 
GuiCellAndFociAttributeAssignmentDialog::createSurfaceSection()
{
   //
   // left hem selection combo box
   //
   leftHemSelectionCheckBox = new QCheckBox("Left");
   leftHemSelectionCheckBox->setChecked(true);
   leftHemSelectionComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "left-hem",
                                                                 false,
                                                                 true,
                                                                 false);
   QObject::connect(leftHemSelectionCheckBox, SIGNAL(toggled(bool)),
                    leftHemSelectionComboBox, SLOT(setEnabled(bool)));

   //
   // right hem selection combo box
   //
   rightHemSelectionCheckBox = new QCheckBox("Right");
   rightHemSelectionCheckBox->setChecked(true);
   rightHemSelectionComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "right-hem",
                                                                 false,
                                                                 true,
                                                                 false);
   QObject::connect(rightHemSelectionCheckBox, SIGNAL(toggled(bool)),
                    rightHemSelectionComboBox, SLOT(setEnabled(bool)));
                                                                 
   //
   // right hem selection combo box
   //
   cerebellumSelectionCheckBox = new QCheckBox("Cerebellum");
   cerebellumSelectionCheckBox->setChecked(true);
   cerebellumSelectionComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "right-hem",
                                                                 false,
                                                                 true,
                                                                 false);
   QObject::connect(cerebellumSelectionCheckBox, SIGNAL(toggled(bool)),
                    cerebellumSelectionComboBox, SLOT(setEnabled(bool)));
                                                                 
   BrainModelSurface* leftBMS  = NULL;
   BrainModelSurface* rightBMS = NULL;
   BrainModelSurface* cerebellumBMS = NULL;
   const int numModels = theMainWindow->getBrainSet()->getNumberOfBrainModels();
   for (int i = 0; i < numModels; i++) {
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
            if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
               leftBMS = bms;
            }
            if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
               rightBMS = bms;
            }
            if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CEREBELLUM) {
               cerebellumBMS = bms;
            }
         }
      }
   }
   if (leftBMS != NULL) {
      leftHemSelectionComboBox->setSelectedBrainModel(leftBMS);
   }
   if (rightBMS != NULL) {
      rightHemSelectionComboBox->setSelectedBrainModel(rightBMS);
   }
   if (cerebellumBMS != NULL) {
      cerebellumSelectionComboBox->setSelectedBrainModel(cerebellumBMS);
   }

   QLabel* maximumDistanceLabel = new QLabel("Maximum Distance of Focus from Surface");
   maximumDistanceDoubleSpinBox = new QDoubleSpinBox;
   maximumDistanceDoubleSpinBox->setMinimum(0.0);
   maximumDistanceDoubleSpinBox->setMaximum(10000000000.0);
   maximumDistanceDoubleSpinBox->setDecimals(3);
   maximumDistanceDoubleSpinBox->setSingleStep(1.0);
   maximumDistanceDoubleSpinBox->setValue(100.0);
   QHBoxLayout* distanceLayout = new QHBoxLayout;
   distanceLayout->addWidget(maximumDistanceLabel);
   distanceLayout->addWidget(maximumDistanceDoubleSpinBox);
   distanceLayout->addStretch();
   
   QGroupBox* gb = new QGroupBox("Surface Selection");
   QGridLayout* gridLayout = new QGridLayout(gb);
   gridLayout->addWidget(leftHemSelectionCheckBox, 0, 0);
   gridLayout->addWidget(leftHemSelectionComboBox, 0, 1);
   gridLayout->addWidget(rightHemSelectionCheckBox, 1, 0);
   gridLayout->addWidget(rightHemSelectionComboBox, 1, 1);
   gridLayout->addWidget(cerebellumSelectionCheckBox, 2, 0);
   gridLayout->addWidget(cerebellumSelectionComboBox, 2, 1);
   gridLayout->addLayout(distanceLayout, 3, 0, 1, 2, Qt::AlignLeft);

   return gb;
}

/**
 * create the assignement options section.
 */
QWidget* 
GuiCellAndFociAttributeAssignmentDialog::createAssignmentSection()
{
   //
   // Append to current values check box
   //
   appendToCurrentValuesCheckBox = new QCheckBox("Append to Current Values");
   
   //
   // Clear attributes without setting
   //
   clearAttributesWithoutSettingCheckBox = new QCheckBox("Clear Attribute and Do NOT Set the Attribute");
   
   //
   // Ignore "?" entries check box
   //
   ignoreQuestionEntriesCheckBox = new QCheckBox("Ignore \"?\" Values");
   
   //
   // Group box for cell/foci attributes
   //
   QGroupBox* assGroupBox = new QGroupBox("Assignment Options");
   QVBoxLayout* assGroupLayout = new QVBoxLayout(assGroupBox);
   assGroupLayout->addWidget(appendToCurrentValuesCheckBox);
   assGroupLayout->addWidget(clearAttributesWithoutSettingCheckBox);
   assGroupLayout->addWidget(ignoreQuestionEntriesCheckBox);
   
   return assGroupBox;
}
      
/**
 * create the cell/foci section.
 */
QWidget* 
GuiCellAndFociAttributeAssignmentDialog::createCellFociSection(const QString& typeString)
{
   //
   // Area radio button
   //
   areaRadioButton = new QRadioButton("Area");
   
   //
   // Geography radio button
   //
   geographyRadioButton = new QRadioButton("Geography");
   
   //
   // Region of Interest radio button
   //
   regionOfInterestRadioButton = new QRadioButton("Region of Interest");
   
   //
   // Button group for mutual exclusion
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(areaRadioButton);
   buttGroup->addButton(geographyRadioButton);
   buttGroup->addButton(regionOfInterestRadioButton);
   
   //
   // Group box for cell/foci attributes
   //
   QGroupBox* attrGroupBox = new QGroupBox(typeString + " Attribute for Assignment");
   QVBoxLayout* attrGroupLayout = new QVBoxLayout(attrGroupBox);
   attrGroupLayout->addWidget(areaRadioButton);
   attrGroupLayout->addWidget(geographyRadioButton);
   attrGroupLayout->addWidget(regionOfInterestRadioButton);
   
   //
   // If doing a cell report, hide items specific to foci
   //
   if (fociFlag == false) {
      areaRadioButton->hide();
      geographyRadioButton->hide();
      regionOfInterestRadioButton->hide();
   }
   
   return attrGroupBox;
}
                           
/**
 * create the paint section.
 */
QWidget* 
GuiCellAndFociAttributeAssignmentDialog::createPaintSection()
{
   //
   // Group box for paint attributes
   //
   QGroupBox* paintGroupBox = new QGroupBox("Paint Attributes");
   QVBoxLayout* paintGroupLayout = new QVBoxLayout(paintGroupBox);
   
   //
   // Scroll view
   //
   QScrollArea* paintScrollView = new QScrollArea;
   paintGroupLayout->addWidget(paintScrollView);
   QWidget* paintScrollWidget = new QWidget;
   paintNameCheckBoxesLayout = new QVBoxLayout(paintScrollWidget);
   paintScrollView->setWidget(paintScrollWidget);
   paintScrollView->setWidgetResizable(true);

   return paintGroupBox;
}      

/**
 * update the paint column section.
 */
void 
GuiCellAndFociAttributeAssignmentDialog::updatePaintColumnSection()
{
   //
   // Add check boxes for paint columns
   //
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int numCols = pf->getNumberOfColumns();   
   
   for (int i = 0; i < numCols; i++) {
      QCheckBox* pcb = NULL;
      if (i < static_cast<int>(paintNameCheckBoxes.size())) {
         pcb = paintNameCheckBoxes[i];
      }
      else {
         pcb = new QCheckBox(pf->getColumnName(i));
         paintNameCheckBoxesLayout->addWidget(pcb);
         pcb->setChecked(false);
         paintNameCheckBoxes.push_back(pcb);
      }
      pcb->setText(pf->getColumnName(i));
      paintNameCheckBoxes[i]->show();
   }
   
   for (int i = numCols; i < static_cast<int>(paintNameCheckBoxes.size()); i++) {
      paintNameCheckBoxes[i]->hide();
   }
}      

/**
 * called when apply button pressed.
 */
void 
GuiCellAndFociAttributeAssignmentDialog::slotPushButton(QAbstractButton* buttonPressed)
{
   if (buttonPressed == dialogButtonBox->button(QDialogButtonBox::Apply)) {
      if ((rightHemSelectionCheckBox->isChecked() == false) &&
          (leftHemSelectionCheckBox->isChecked() == false)  &&
          (cerebellumSelectionCheckBox->isChecked() == false)) {
         QMessageBox::critical(this, "ERROR", 
                                 "All surfaces are deselected.");
         return;
      }
      
      const BrainModelSurface* leftBMS = leftHemSelectionComboBox->getSelectedBrainModelSurface();
      if (leftBMS == NULL) {
         QMessageBox::critical(this, "ERROR", 
                                 "No left surface is selected.");
         return;
      }
      const BrainModelSurface* rightBMS = rightHemSelectionComboBox->getSelectedBrainModelSurface();
      if (rightBMS == NULL) {
         QMessageBox::critical(this, "ERROR", 
                                 "No right surface is selected.");
         return;
      }
      const BrainModelSurface* cerebellumBMS = cerebellumSelectionComboBox->getSelectedBrainModelSurface();
      if (cerebellumBMS == NULL) {
         QMessageBox::critical(this, "ERROR", 
                                 "No cerebellum surface is selected.");
         return;
      }

      if ((areaRadioButton->isChecked() == false) &&
          (geographyRadioButton->isChecked() == false) &&
          (regionOfInterestRadioButton->isChecked() == false)) {
         QMessageBox::critical(this, "ERROR", 
                                 "One of Area, Geography, or Region of Interest must be selected.");
         return;
      }
      
      //
      // Get the foci projection file
      //
      CellProjectionFile* cf = NULL;
      if (fociFlag) {
         cf = theMainWindow->getBrainSet()->getFociProjectionFile();
      } 
      else {
         cf = theMainWindow->getBrainSet()->getCellProjectionFile();
      }
      
      if (cf != NULL) {
         const int numCells = cf->getNumberOfCellProjections();
         if (numCells > 0) {            
            //
            // See if paints are selected
            //
            PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
            const int numPaintCols = pf->getNumberOfColumns();

            if (numPaintCols <= 0) {
               if (clearAttributesWithoutSettingCheckBox->isChecked() == false) {
                  QMessageBox::critical(this, "ERROR", 
                                          "There are no paint columns.");
                  return;
               }
            }

            //
            // Determine node nearest to each cell
            //
            std::vector<float> cellsNearestLeftNodeDistance(numCells, -1.0);
            std::vector<float> cellsNearestRightNodeDistance(numCells, -1.0);
            std::vector<float> cellsNearestCerebellumNodeDistance(numCells, -1.0);
            std::vector<int> cellsNearestLeftNode(numCells, -1);
            std::vector<int> cellsNearestRightNode(numCells, -1);
            std::vector<int> cellsNearestCerebellumNode(numCells, -1);
            BrainModelSurfacePointLocator leftPointLocator(leftBMS,
                                                            true);
            BrainModelSurfacePointLocator rightPointLocator(rightBMS,
                                                            true);
            BrainModelSurfacePointLocator cerebellumPointLocator(cerebellumBMS,
                                                            true);
            
            for (int i = 0; i < numCells; i++) {
               CellProjection* cp = cf->getCellProjection(i);
               float xyz[3];
               cp->getXYZ(xyz);
               
               switch (cp->getCellStructure()) {
                  case Structure::STRUCTURE_TYPE_CORTEX_LEFT:         
                  case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
                     if (cp->getProjectedPosition(leftBMS->getCoordinateFile(),
                                                  leftBMS->getTopologyFile(),
                                                  leftBMS->getIsFiducialSurface(),
                                                  leftBMS->getIsFlatSurface(),
                                                  false,
                                                  xyz)) {
                        cellsNearestLeftNode[i] = leftPointLocator.getNearestPoint(xyz);
                        cellsNearestLeftNodeDistance[i] = 
                           MathUtilities::distance3D(xyz, 
                               leftBMS->getCoordinateFile()->getCoordinate(cellsNearestLeftNode[i]));
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                  case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
                     if (cp->getProjectedPosition(rightBMS->getCoordinateFile(),
                                                  rightBMS->getTopologyFile(),
                                                  rightBMS->getIsFiducialSurface(),
                                                  rightBMS->getIsFlatSurface(),
                                                  false,
                                                  xyz)) {
                        cellsNearestRightNode[i] = rightPointLocator.getNearestPoint(xyz);
                        cellsNearestRightNodeDistance[i] = 
                           MathUtilities::distance3D(xyz, 
                               rightBMS->getCoordinateFile()->getCoordinate(cellsNearestRightNode[i]));
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
                  case Structure::STRUCTURE_TYPE_CEREBELLUM:
                  case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:       
                  case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
                     if (cp->getProjectedPosition(cerebellumBMS->getCoordinateFile(),
                                                  cerebellumBMS->getTopologyFile(),
                                                  cerebellumBMS->getIsFiducialSurface(),
                                                  cerebellumBMS->getIsFlatSurface(),
                                                  false,
                                                  xyz)) {
                        cellsNearestCerebellumNode[i] = cerebellumPointLocator.getNearestPoint(xyz);
                        cellsNearestCerebellumNodeDistance[i] = 
                           MathUtilities::distance3D(xyz, 
                               cerebellumBMS->getCoordinateFile()->getCoordinate(cellsNearestCerebellumNode[i]));
                     }
                     break;
                  case Structure::STRUCTURE_TYPE_INVALID:
                     break;
               }
            }
            
            //
            // Is right hem deselected
            //
            if (rightHemSelectionCheckBox->isChecked() == false) {
               std::fill(cellsNearestRightNode.begin(),
                         cellsNearestRightNode.end(),
                         -1);
            }
            if (leftHemSelectionCheckBox->isChecked() == false) {
               std::fill(cellsNearestLeftNode.begin(),
                         cellsNearestLeftNode.end(),
                         -1);
            }
            if (cerebellumSelectionCheckBox->isChecked() == false) {
               std::fill(cellsNearestCerebellumNode.begin(),
                         cellsNearestCerebellumNode.end(),
                         -1);
            }
            
            const float maximumDistance = maximumDistanceDoubleSpinBox->value();
            
            //
            // Process the cells
            //
            for (int i = 0; i < numCells; i++) {
               //
               // Load cell data into the table
               //
               CellProjection* cd = cf->getCellProjection(i);
               
               QString valueString;
               if (areaRadioButton->isChecked()) {
                  valueString = cd->getArea();
               }
               else if (geographyRadioButton->isChecked()) {
                  valueString = cd->getGeography();
               }
               else if (regionOfInterestRadioButton->isChecked()) {
                  valueString = cd->getRegionOfInterest();
               }
               
               if (appendToCurrentValuesCheckBox->isChecked() == false) {
                  valueString = "";
               }
               
               if (clearAttributesWithoutSettingCheckBox->isChecked()) {
                  valueString = "";
               }
               else {
                  //
                  // Load paint into the table
                  //
                  int node = -1;
                  float distance = 100000000.0;
                  if (cellsNearestLeftNode[i] >= 0) {
                     node = cellsNearestLeftNode[i];
                     distance = cellsNearestLeftNodeDistance[i];
                  }
                  else if (cellsNearestRightNode[i] >= 0) {
                     node = cellsNearestRightNode[i];
                     distance = cellsNearestRightNodeDistance[i];
                  }
                  else if (cellsNearestCerebellumNode[i] >= 0) {
                     node = cellsNearestCerebellumNode[i];
                     distance = cellsNearestCerebellumNodeDistance[i];
                  }
                  if ((node >= 0) &&
                      (distance <= maximumDistance)) {
                     for (int j = 0; j < numPaintCols; j++) {
                        if (paintNameCheckBoxes[j]->isChecked()) {
                           const int paintIndex = pf->getPaint(node, j);
                           QString paintName = pf->getPaintNameFromIndex(paintIndex);
                           if (valueString.isEmpty() == false) {
                              valueString += "; ";
                           }
                           
                           if (ignoreQuestionEntriesCheckBox->isChecked()) {
                              if (paintName.startsWith("?")) {
                                 paintName = " ";
                              }
                           }
                           
                           valueString += paintName;
                        }
                     }
                  }
               }

               if (areaRadioButton->isChecked()) {
                  cd->setArea(valueString);
               }
               else if (geographyRadioButton->isChecked()) {
                  cd->setGeography(valueString);
               }
               else if (regionOfInterestRadioButton->isChecked()) {
                  cd->setRegionOfInterest(valueString);
               }
            } // for (i = 0; i < numCells...
         }  // if (numCells > 0)
         else {
            QString msg("There are no ");
            msg.append(typeString);
            msg.append(".");
            QMessageBox::critical(this, "ERROR", msg);
            return;
         }
      }  // if (cf != NULL)
      else {
         QString msg("There are no ");
         msg.append(typeString);
         msg.append(".");
         QMessageBox::critical(this, "ERROR", msg);
         return;
      }
   }
   else if (buttonPressed == dialogButtonBox->button(QDialogButtonBox::Close)) {
      close();
   }
}

/**
 * determine if a check box is shown and checked.
 */
bool 
GuiCellAndFociAttributeAssignmentDialog::checked(const QCheckBox* cb) const
{
   return (cb->isVisible() && cb->isChecked());
}
      
