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

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "DisplaySettingsVectors.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiShapeOrVectorsFromCoordinateSubtractionDialog.h"
#include "MathUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VectorFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiShapeOrVectorsFromCoordinateSubtractionDialog::GuiShapeOrVectorsFromCoordinateSubtractionDialog(
                                                                 QWidget* parent,
                                                                 const MODE modeIn)
   : WuQDialog(parent)
{
   mode = modeIn;
   
   switch (mode) {
      case MODE_SHAPE:
         setWindowTitle("Create Surface Shape From Surface Difference");
         break;
      case MODE_VECTOR:
         setWindowTitle("Create Vectors From Surface Subtraction");
         break;
   }
   
   //
   //  Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   
   //
   // surface A
   //
   QLabel* surfaceALabel = new QLabel("Surface A ");
   surfaceAComboBox = new GuiBrainModelSelectionComboBox(false,
                                                         true,
                                                         false,
                                                         "",
                                                         0);
   
   //
   // surface B
   //
   QLabel* surfaceBLabel = new QLabel("Surface B ");
   surfaceBComboBox = new GuiBrainModelSelectionComboBox(false,
                                                         true,
                                                         false,
                                                         "",
                                                         0);
   
   //
   // Grid for items
   //
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   gridLayout->addWidget(surfaceALabel, 0, 0);
   gridLayout->addWidget(surfaceAComboBox, 0, 1);
   gridLayout->addWidget(surfaceBLabel, 1, 0);
   gridLayout->addWidget(surfaceBComboBox, 1, 1);

   //
   // vector column combo box
   //
   switch (mode) {
      case MODE_SHAPE:
          {
             QLabel* columnLabel = new QLabel("Column Name ");
             columnNameLineEdit = new QLineEdit;
             QLabel* columnCommentLabel = new QLabel("Column Comment ");
             columnCommentLineEdit = new QLineEdit;
             //
             // Shape difference mode
             //
             QLabel* shapeDiffModeLabel = new QLabel("XYZ Difference");
             this->shapeDiffModeComboBox = new QComboBox;
             this->shapeDiffModeComboBox->addItem("Absolute Value",
                        QVariant((int)MetricFile::COORDINATE_DIFFERENCE_MODE_ABSOLUTE));
             this->shapeDiffModeComboBox->addItem("Signed",
                        QVariant((int)MetricFile::COORDINATE_DIFFERENCE_MODE_SIGNED));
             gridLayout->addWidget(columnLabel, 3, 0);
             gridLayout->addWidget(columnNameLineEdit, 3, 1);
             gridLayout->addWidget(columnCommentLabel, 4, 0);
             gridLayout->addWidget(columnCommentLineEdit, 4, 1);
             gridLayout->addWidget(shapeDiffModeLabel, 5, 0);
             gridLayout->addWidget(shapeDiffModeComboBox, 5, 1);
          }
         break;
      case MODE_VECTOR:
         break;
   }
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok 
                                                      | QDialogButtonBox::Cancel);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(accepted()),
                    this, SLOT(accept()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(reject()));

   switch (mode) {
      case MODE_SHAPE:
         columnNameLineEdit->setText("Difference");
         columnCommentLineEdit->setText("");
         break;
      case MODE_VECTOR:
         break;
   }
}

/**
 * destructor.
 */
GuiShapeOrVectorsFromCoordinateSubtractionDialog::~GuiShapeOrVectorsFromCoordinateSubtractionDialog()
{
}

/**
 * called when OK or Cancel buttons pressed.
 */
void 
GuiShapeOrVectorsFromCoordinateSubtractionDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      const BrainModelSurface* bmsA = surfaceAComboBox->getSelectedBrainModelSurface();
      if (bmsA == NULL) {
         QMessageBox::critical(this, "ERROR", "Surface A is invalid.");
         return;
      }
      
      const BrainModelSurface* bmsB = surfaceBComboBox->getSelectedBrainModelSurface();
      if (bmsB == NULL) {
         QMessageBox::critical(this, "ERROR", "Surface B is invalid.");
         return;
      }
      
      if (bmsA->getNumberOfNodes() <= 0) {
         QMessageBox::critical(this, "ERROR", "Surface A has no nodes.");
         return;
      }
      if (bmsA->getNumberOfNodes() != bmsB->getNumberOfNodes()) {
         QMessageBox::critical(this, "ERROR", "Surface A and B have a different number of nodes.");
         return;
      }
      if (bmsA == bmsB) {
         QMessageBox::critical(this, "ERROR", "Surface A and B are the same.");
         return;
      }
      
      showWaitCursor();
      
      switch (mode) {
         case MODE_SHAPE:
            {
               const QString columnName = columnNameLineEdit->text();
               const QString columnComment = columnCommentLineEdit->text();

               int diffIndex = this->shapeDiffModeComboBox->currentIndex();
               const MetricFile::COORDINATE_DIFFERENCE_MODE diffMode =
                  (MetricFile::COORDINATE_DIFFERENCE_MODE)
                  this->shapeDiffModeComboBox->itemData(diffIndex).toInt();
               SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
               try {
                  int diffColNum = ssf->getNumberOfColumns();
                  ssf->addColumns(4);
                  ssf->addColumnOfCoordinateDifference(diffMode,
                                                       bmsA->getCoordinateFile(),
                                                       bmsB->getCoordinateFile(),
                                                       bmsA->getTopologyFile(),
                                                       diffColNum,
                                                       columnName,
                                                       columnComment,
                                                       diffColNum + 1,
                                                       diffColNum + 2,
                                                       diffColNum + 3);
               }
               catch (FileException& e) {
                  showNormalCursor();
                  QMessageBox::critical(this, "ERROR", e.whatQString());
                  return;
               }
                              
               GuiFilesModified fm;
               fm.setSurfaceShapeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
            break;
         case MODE_VECTOR:
            {
               VectorFile* vf = new VectorFile();
               int numNodes = bmsA->getNumberOfNodes();
               const TopologyHelper* th = bmsA->getTopologyFile()->getTopologyHelper(false, true, false);
               vf->setNumberOfVectors(numNodes);
               for (int i = 0; i < numNodes; i++) {
                  float xyz[3] = { 0.0, 0.0, 0.0 };
                  float vector[3] = { 0.0, 0.0, 0.0 };
                  float magnitude = 0.0;
                  if (th->getNodeHasNeighbors(i)) {
                     const float* xyzA = bmsA->getCoordinateFile()->getCoordinate(i);
                     const float* xyzB = bmsB->getCoordinateFile()->getCoordinate(i);
                     xyz[0] = xyzA[0];
                     xyz[1] = xyzA[1];
                     xyz[2] = xyzA[2];
                     vector[0] = xyzB[0] - xyzA[0];
                     vector[1] = xyzB[1] - xyzA[1];
                     vector[2] = xyzB[2] - xyzA[2];
                     magnitude = MathUtilities::vectorLength(vector);
                     MathUtilities::normalize(vector);
                  }
                  vf->setVectorData(i, xyz, vector, magnitude, i);
               }
               theMainWindow->getBrainSet()->addVectorFile(vf);
               DisplaySettingsVectors* dssv = theMainWindow->getBrainSet()->getDisplaySettingsVectors();
               if (dssv->getDisplayModeSurface() == DisplaySettingsVectors::DISPLAY_MODE_NONE) {
                  dssv->setDisplayModeSurface(DisplaySettingsVectors::DISPLAY_MODE_SPARSE);
               }
               dssv->update();

               GuiFilesModified fm;
               fm.setVectorModified();
               theMainWindow->fileModificationUpdate(fm);
            }
      }

      GuiBrainModelOpenGL::updateAllGL();
      
      showNormalCursor();
      beep();
   }
   
   WuQDialog::done(r);
}
