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
#include "PaintFile.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCellAndFociAttributeAssignmentDialog::GuiCellAndFociAttributeAssignmentDialog(QWidget* parent, 
                                                       const bool fociFlagIn)
   : QtDialogModal(parent)
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
   QVBoxLayout* dialogLayout = getDialogLayout();
   
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
   
   setOkButtonEnabled(true);
}
                           
/**
 * destructor.
 */
GuiCellAndFociAttributeAssignmentDialog::~GuiCellAndFociAttributeAssignmentDialog()
{
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
                                                                 
   BrainModelSurface* leftBMS  = NULL;
   BrainModelSurface* rightBMS = NULL;
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
         }
      }
   }
   if (leftBMS != NULL) {
      leftHemSelectionComboBox->setSelectedBrainModel(leftBMS);
   }
   if (rightBMS != NULL) {
      rightHemSelectionComboBox->setSelectedBrainModel(rightBMS);
   }

   QGroupBox* gb = new QGroupBox("Surface Selection");
   QGridLayout* gridLayout = new QGridLayout(gb);
   gridLayout->addWidget(leftHemSelectionCheckBox, 0, 0);
   gridLayout->addWidget(leftHemSelectionComboBox, 0, 1);
   gridLayout->addWidget(rightHemSelectionCheckBox, 1, 0);
   gridLayout->addWidget(rightHemSelectionComboBox, 1, 1);

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
   // Ignore "?" entries check box
   //
   ignoreQuestionEntriesCheckBox = new QCheckBox("Ignore \"?\" Values");
   
   //
   // Group box for cell/foci attributes
   //
   QGroupBox* assGroupBox = new QGroupBox("Assignment Options");
   QVBoxLayout* assGroupLayout = new QVBoxLayout(assGroupBox);
   assGroupLayout->addWidget(appendToCurrentValuesCheckBox);
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
   // Button group for mutual exclusion
   //
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(areaRadioButton);
   buttGroup->addButton(geographyRadioButton);
   
   //
   // Group box for cell/foci attributes
   //
   QGroupBox* attrGroupBox = new QGroupBox(typeString + " Attribute for Assignment");
   QVBoxLayout* attrGroupLayout = new QVBoxLayout(attrGroupBox);
   attrGroupLayout->addWidget(areaRadioButton);
   attrGroupLayout->addWidget(geographyRadioButton);
   
   //
   // If doing a cell report, hide items specific to foci
   //
   if (fociFlag == false) {
      areaRadioButton->hide();
      geographyRadioButton->hide();
   }
   
   return attrGroupBox;
}
                           
/**
 * create the paint section.
 */
QWidget* 
GuiCellAndFociAttributeAssignmentDialog::createPaintSection()
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int numCols = pf->getNumberOfColumns();
   if (numCols <= 0) {
      return NULL;
   }
   
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
   QVBoxLayout* paintScrollLayout = new QVBoxLayout(paintScrollWidget);
   paintScrollView->setWidget(paintScrollWidget);
   paintScrollView->setWidgetResizable(true);

   //
   // Add check boxes for paint columns
   //
   for (int i = 0; i < numCols; i++) {
      QCheckBox* pcb = new QCheckBox(pf->getColumnName(i));
      paintScrollLayout->addWidget(pcb);
      pcb->setChecked(false);
      paintNameCheckBoxes.push_back(pcb);
   }
   
   return paintGroupBox;
}      

/**
 * called when apply button pressed.
 */
void 
GuiCellAndFociAttributeAssignmentDialog::done(int r)
{
   if (r == QtDialogModal::Accepted) {
      if ((rightHemSelectionCheckBox->isChecked() == false) &&
          (leftHemSelectionCheckBox->isChecked() == false)) {
         QMessageBox::critical(this, "ERROR", 
                                 "Both hemispheres are deselected.");
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

      if ((areaRadioButton->isChecked() == false) &&
          (geographyRadioButton->isChecked() == false)) {
         QMessageBox::critical(this, "ERROR", 
                                 "One of Area or Geography must be selected.");
         return;
      }
      
      //
      // Get the fiducial foci file
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
               QMessageBox::critical(this, "ERROR", 
                                       "There are no paint columns.");
               return;
            }

            //
            // Determine node nearest to each cell
            //
            std::vector<int> cellsNearestLeftNode(numCells, -1);
            std::vector<int> cellsNearestRightNode(numCells, -1);
            BrainModelSurfacePointLocator leftPointLocator(leftBMS,
                                                            true);
            BrainModelSurfacePointLocator rightPointLocator(rightBMS,
                                                            true);
            
            for (int i = 0; i < numCells; i++) {
               CellProjection* cp = cf->getCellProjection(i);
               float xyz[3];
               cp->getXYZ(xyz);
               if (xyz[0] >= 0) {
                  if (cp->getProjectedPosition(rightBMS->getCoordinateFile(),
                                               rightBMS->getTopologyFile(),
                                               rightBMS->getIsFiducialSurface(),
                                               rightBMS->getIsFlatSurface(),
                                               false,
                                               xyz)) {
                     cellsNearestRightNode[i] = rightPointLocator.getNearestPoint(xyz);
                  }
               }
               else {
                  if (cp->getProjectedPosition(leftBMS->getCoordinateFile(),
                                               leftBMS->getTopologyFile(),
                                               leftBMS->getIsFiducialSurface(),
                                               leftBMS->getIsFlatSurface(),
                                               false,
                                               xyz)) {
                     cellsNearestLeftNode[i] = leftPointLocator.getNearestPoint(xyz);
                  }
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
               
               if (appendToCurrentValuesCheckBox->isChecked() == false) {
                  valueString = "";
               }
               
               //
               // Load paint into the table
               //
               int node = -1;
               if (cellsNearestLeftNode[i] >= 0) {
                  node = cellsNearestLeftNode[i];
               }
               else if (cellsNearestRightNode[i] >= 0) {
                  node = cellsNearestRightNode[i];
               }
               if (node >= 0) {
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

               if (areaRadioButton->isChecked()) {
                  cd->setArea(valueString);
               }
               else if (geographyRadioButton->isChecked()) {
                  cd->setGeography(valueString);
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
   
   QtDialogModal::done(r);
}

/**
 * determine if a check box is shown and checked.
 */
bool 
GuiCellAndFociAttributeAssignmentDialog::checked(const QCheckBox* cb) const
{
   return (cb->isVisible() && cb->isChecked());
}
      
