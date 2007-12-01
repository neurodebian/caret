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

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>

#include "BrainModelSurfacePointLocator.h"
#include "BrainSet.h"
#include "CellProjectionFile.h"
#include "FileUtilities.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiCellAndFociReportDialog.h"
#include "GuiMainWindow.h"
#include "PaintFile.h"
#include "StringTable.h"
#include "QtTableDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCellAndFociReportDialog::GuiCellAndFociReportDialog(QWidget* parent, 
                                                       const bool fociFlagIn)
   : QtDialog(parent, true)
{
   resultsTableDialog = NULL;
   
   fociFlag = fociFlagIn;
   typeString = "Cell";
   if (fociFlag) {
      typeString = "Foci";
   }

   setWindowTitle(typeString + " Report");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setSpacing(5);
   dialogLayout->setMargin(5);
   
   //
   // Create surface section
   //
   dialogLayout->addWidget(createSurfaceSection());
   
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
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(5);
   
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
GuiCellAndFociReportDialog::~GuiCellAndFociReportDialog()
{
}

// create the surface section
QWidget* 
GuiCellAndFociReportDialog::createSurfaceSection()
{
   //
   // left hem selection combo box
   //
   QLabel* leftLabel = new QLabel("Left");
   leftHemSelectionComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "left-hem",
                                                                 false,
                                                                 true,
                                                                 false);
                                                                 
   //
   // right hem selection combo box
   //
   QLabel* rightLabel = new QLabel("Right");
   rightHemSelectionComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "left-hem",
                                                                 false,
                                                                 true,
                                                                 false);
                                                                 
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
   gridLayout->addWidget(leftLabel, 0, 0);
   gridLayout->addWidget(leftHemSelectionComboBox, 0, 1);
   gridLayout->addWidget(rightLabel, 1, 0);
   gridLayout->addWidget(rightHemSelectionComboBox, 1, 1);

   return gb;
}

/**
 * create the cell/foci section.
 */
QWidget* 
GuiCellAndFociReportDialog::createCellFociSection(const QString& typeString)
{
   //
   // Group box for cell/foci attributes
   //
   QGroupBox* attrGroupBox = new QGroupBox(typeString + " Attributes");
   QVBoxLayout* attrGroupLayout = new QVBoxLayout(attrGroupBox);
   
   //
   // check box
   //
   numberCheckBox = new QCheckBox("Number");
   numberCheckBox->setChecked(true);
   attrGroupLayout->addWidget(numberCheckBox);
   
   //
   // name check box
   //
   nameCheckBox = new QCheckBox("Name");
   nameCheckBox->setChecked(true);
   attrGroupLayout->addWidget(nameCheckBox);
   
   //
   // position check box
   //
   positionCheckBox = new QCheckBox("Position");
   positionCheckBox->setChecked(true);
   attrGroupLayout->addWidget(positionCheckBox);
   
   //
   // class check box
   //
   classCheckBox = new QCheckBox("Class");
   classCheckBox->setChecked(true);
   attrGroupLayout->addWidget(classCheckBox);
   
   //
   // area check box
   //
   areaCheckBox = new QCheckBox("Area");
   areaCheckBox->setChecked(true);
   attrGroupLayout->addWidget(areaCheckBox);
   
   //
   // geography check box
   //
   geographyCheckBox = new QCheckBox("Geography");
   geographyCheckBox->setChecked(true);
   attrGroupLayout->addWidget(geographyCheckBox);
   
   //
   // size check box
   //
   sizeCheckBox = new QCheckBox("Size");
   sizeCheckBox->setChecked(true);
   attrGroupLayout->addWidget(sizeCheckBox);
   
   //
   // statistic check box
   //
   statisticCheckBox = new QCheckBox("Statistic");
   statisticCheckBox->setChecked(true);
   attrGroupLayout->addWidget(statisticCheckBox);
   
   //
   // hemisphere check box
   //
   hemisphereCheckBox = new QCheckBox("Hemisphere");
   hemisphereCheckBox->setChecked(true);
   attrGroupLayout->addWidget(hemisphereCheckBox);
   
   //
   // study check box
   //
   studyCheckBox = new QCheckBox("Study");
   studyCheckBox->setChecked(true);
   attrGroupLayout->addWidget(studyCheckBox);
   
   //
   // comment check box
   //
   commentCheckBox = new QCheckBox("Comment");
   commentCheckBox->setChecked(true);
   attrGroupLayout->addWidget(commentCheckBox);
   
   
   //
   // If doing a cell report, hide items specific to foci
   //
   if (fociFlag == false) {
      geographyCheckBox->hide();
      areaCheckBox->hide();
      sizeCheckBox->hide();
      statisticCheckBox->hide();
      commentCheckBox->hide();
      studyCheckBox->hide();
   }
   
   return attrGroupBox;
}
                           
/**
 * create the paint section.
 */
QWidget* 
GuiCellAndFociReportDialog::createPaintSection()
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
 * called when ok/cancel button pressed.
 */
void 
GuiCellAndFociReportDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      const BrainModelSurface* leftBMS = leftHemSelectionComboBox->getSelectedBrainModelSurface();
      if (leftBMS == NULL) {
         QMessageBox::critical(this, "ERROR", 
                                 "No left surface is selected.");
      }
      const BrainModelSurface* rightBMS = rightHemSelectionComboBox->getSelectedBrainModelSurface();
      if (rightBMS == NULL) {
         QMessageBox::critical(this, "ERROR", 
                                 "No right surface is selected.");
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
            int numTableColumns = 0;
            std::vector<QString> columnTitle;
            
            int numberColumn = -1;
            if (checked(numberCheckBox)) {
               columnTitle.push_back("Number");
               numberColumn = numTableColumns++;
            }
            
            int nameColumn = -1;
            if (checked(nameCheckBox)) {
               columnTitle.push_back("Name");
               nameColumn = numTableColumns++;
            }
            
            int positionColumn = -1;
            if (checked(positionCheckBox)) {
               columnTitle.push_back("X");
               columnTitle.push_back("Y");
               columnTitle.push_back("Z");
               positionColumn = numTableColumns;
               numTableColumns += 3;
            }
            
            int classColumn = -1;
            if (checked(classCheckBox)) {
               columnTitle.push_back("Class");
               classColumn = numTableColumns++;
            }
            
            int areaColumn = -1;
            if (checked(areaCheckBox)) {
               columnTitle.push_back("Area");
               areaColumn = numTableColumns++;
            }
            
            int geographyColumn = -1;
            if (checked(geographyCheckBox)) {
               columnTitle.push_back("Geography");
               geographyColumn = numTableColumns++;
            }
            
            int hemisphereColumn = -1;
            if (checked(hemisphereCheckBox)) {
               columnTitle.push_back("Hemisphere");
               hemisphereColumn = numTableColumns++;
            }
            
            int sizeColumn = -1;
            if (checked(sizeCheckBox)) {
               columnTitle.push_back("Size");
               sizeColumn = numTableColumns++;
            }
            
            int statisticColumn = -1;
            if (checked(statisticCheckBox)) {
               columnTitle.push_back("Statistic");
               statisticColumn = numTableColumns++;
            }
            
            int studyColumn = -1;
            if (checked(studyCheckBox)) {
               columnTitle.push_back("Study");
               studyColumn = numTableColumns++;
            }
            
            int commentColumn = -1;
            if (checked(commentCheckBox)) {
               columnTitle.push_back("Comment");
               commentColumn = numTableColumns++;
            }
            
            //
            // See if paints are selected
            //
            PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
            const int numPaintCols = pf->getNumberOfColumns();
            bool havePaints = false;
            std::vector<int> paintTableColumn;
            for (int j = 0; j < numPaintCols; j++) {
               int column = -1;
               if (paintNameCheckBoxes[j]->isChecked()) {
                  columnTitle.push_back(pf->getColumnName(j));
                  column = numTableColumns++;
                  havePaints = true;
               }
               paintTableColumn.push_back(column);
            }
            
            //
            // Determine node nearest to each cell
            //
            std::vector<int> cellsNearestLeftNode(numCells, -1);
            std::vector<int> cellsNearestRightNode(numCells, -1);
            if (havePaints) {
               BrainModelSurfacePointLocator leftPointLocator(leftBMS,
                                                               true);
               BrainModelSurfacePointLocator rightPointLocator(rightBMS,
                                                               true);
               
               for (int i = 0; i < numCells; i++) {
                  const CellProjection* cp = cf->getCellProjection(i);
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
            }
            
            //
            // Create the QString table
            //
            StringTable cellTable(numCells, numTableColumns);
            
            //
            // Set the column titles
            //
            for (int j = 0; j < static_cast<int>(columnTitle.size()); j++) {
               cellTable.setColumnTitle(j, columnTitle[j]);
            }
            
            //
            // Handle columns that contain integer data so sorted correctly
            //
            if (numberColumn >= 0) {
               cellTable.setColumnMaxInteger(numberColumn, numCells);
            }
            
            //
            // Process the cells
            //
            for (int i = 0; i < numCells; i++) {
               //
               // Load cell data into the table
               //
               const CellProjection* cd = cf->getCellProjection(i);
               if (numberColumn >= 0) {
                  cellTable.setElement(i, numberColumn, i);
               }
               
               if (nameColumn >= 0) {
                  cellTable.setElement(i, nameColumn, cd->getName());
               }
               
               if (positionColumn >= 0) {
                  const float* xyz = cd->getXYZ();
                  cellTable.setElement(i, positionColumn, xyz[0]);
                  cellTable.setElement(i, positionColumn + 1, xyz[1]);
                  cellTable.setElement(i, positionColumn + 2, xyz[2]);
               }
               
               if (classColumn >= 0) {
                  cellTable.setElement(i, classColumn, cd->getClassName());
               }

               if (areaColumn >= 0) {
                  cellTable.setElement(i, areaColumn, cd->getArea());
               }
               
               if (geographyColumn >= 0) {
                  cellTable.setElement(i, geographyColumn, cd->getGeography());
               }

               if (sizeColumn >= 0) {
                  cellTable.setElement(i, sizeColumn, cd->getSize());
               }

               if (statisticColumn >= 0) {
                  cellTable.setElement(i, statisticColumn, cd->getStatistic());
               }

               if (hemisphereColumn >= 0) {
                  cellTable.setElement(i, hemisphereColumn, 
                     Structure::convertTypeToString(cd->getCellStructure()));
               }

               if (studyColumn >= 0) {
                  QString studyTitle;
                  if (cd->getStudyNumber() >= 0) {
                     const CellStudyInfo* cs = cf->getStudyInfo(cd->getStudyNumber());
                     studyTitle = cs->getTitle();
                  }
                  cellTable.setElement(i, studyColumn, studyTitle);
               }

               if (commentColumn >= 0) {
                  cellTable.setElement(i, commentColumn, cd->getComment());
               }

               //
               // Load paint into the table
               //
               if (havePaints) {
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
                           const QString paintName = pf->getPaintNameFromIndex(paintIndex);
                           cellTable.setElement(i, paintTableColumn[j], paintName);
                        }
                     }
                  }
               }
            } // for (i = 0; i < numCells...
            
            //
            // Create and display the table dialog
            //
            const QString titleString(typeString
                                      + " Report ");
            resultsTableDialog = new QtTableDialog(theMainWindow,
                                                   titleString,
                                                   cellTable,
                                                   true);
         }  // if (numCells > 0)
         else {
            QString msg("There are no ");
            msg.append(typeString);
            msg.append(".");
            QMessageBox::critical(this, "ERROR", msg);
         }
      }  // if (cf != NULL)
      else {
         QString msg("There are no ");
         msg.append(typeString);
         msg.append(".");
         QMessageBox::critical(this, "ERROR", msg);
      }
   }  // if (r == QDialog::done)
   
   QDialog::done(r);
}

/**
 * determine if a check box is shown and checked.
 */
bool 
GuiCellAndFociReportDialog::checked(const QCheckBox* cb) const
{
   return (cb->isVisible() && cb->isChecked());
}
      
