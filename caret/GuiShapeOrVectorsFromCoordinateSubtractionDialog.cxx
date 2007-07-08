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

#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "DisplaySettingsSurfaceVectors.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiShapeOrVectorsFromCoordinateSubtractionDialog.h"
#include "SurfaceShapeFile.h"
#include "SurfaceVectorFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiShapeOrVectorsFromCoordinateSubtractionDialog::GuiShapeOrVectorsFromCoordinateSubtractionDialog(
                                                                 QWidget* parent,
                                                                 const MODE modeIn)
   : QtDialogModal(parent)
{
   mode = modeIn;
   
   QString labelString;
   switch (mode) {
      case MODE_SHAPE:
         labelString = "Surface Shape File Column ";
         setWindowTitle("Create Surface Shape From Surface Difference");
         break;
      case MODE_VECTOR:
         labelString = "Vector File Column ";
         setWindowTitle("Create Vectors From Surface Subtraction");
         break;
   }
   
   //
   //  Layout for dialog
   //
   QVBoxLayout* dialogLayout = getDialogLayout();
   
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
   // vector column combo box
   //
   QLabel* columnComboBoxLabel = new QLabel(labelString);
   switch (mode) {
      case MODE_SHAPE:
         columnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                                                true,
                                                                false,
                                                                false);
         break;
      case MODE_VECTOR:
         columnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                GUI_NODE_FILE_TYPE_SURFACE_VECTOR,
                                                                true,
                                                                false,
                                                                false);
         break;
   }
   
   //
   // Column name
   //
   QLabel* columnLabel = new QLabel("Column Name ");
   columnNameLineEdit = new QLineEdit;
   QObject::connect(columnComboBox, SIGNAL(itemNameSelected(const QString&)),
                    columnNameLineEdit, SLOT(setText(const QString&)));

   //
   // Column comment
   //
   QLabel* columnCommentLabel = new QLabel("Column Comment ");
   columnCommentLineEdit = new QLineEdit;
   QObject::connect(columnComboBox, SIGNAL(itemNameSelected(const QString&)),
                    this, SLOT(slotColumnComment()));

   //
   // Grid for items
   //
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   gridLayout->addWidget(surfaceALabel, 0, 0);   
   gridLayout->addWidget(surfaceAComboBox, 0, 1);   
   gridLayout->addWidget(surfaceBLabel, 1, 0);   
   gridLayout->addWidget(surfaceBComboBox, 1, 1);   
   gridLayout->addWidget(columnComboBoxLabel, 2, 0);   
   gridLayout->addWidget(columnComboBox, 2, 1);   
   gridLayout->addWidget(columnLabel, 3, 0);   
   gridLayout->addWidget(columnNameLineEdit, 3, 1);   
   gridLayout->addWidget(columnCommentLabel, 4, 0);   
   gridLayout->addWidget(columnCommentLineEdit, 4, 1);   

   columnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
   columnNameLineEdit->setText(columnComboBox->currentText());
}

/**
 * destructor.
 */
GuiShapeOrVectorsFromCoordinateSubtractionDialog::~GuiShapeOrVectorsFromCoordinateSubtractionDialog()
{
}

/**
 * update the column comment.
 */
void 
GuiShapeOrVectorsFromCoordinateSubtractionDialog::slotColumnComment()
{
   SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
   SurfaceVectorFile* svf = theMainWindow->getBrainSet()->getSurfaceVectorFile();
   const int col = columnComboBox->currentIndex();
   switch (mode) {
      case MODE_SHAPE:
         if ((col >= 0) && (col < ssf->getNumberOfColumns())) {
            columnCommentLineEdit->setText(ssf->getColumnComment(col));
         }
         else {
            columnCommentLineEdit->setText("");
         }
         break;
      case MODE_VECTOR:
         if ((col >= 0) && (col < svf->getNumberOfColumns())) {
            columnCommentLineEdit->setText(svf->getColumnComment(col));
         }
         else {
            columnCommentLineEdit->setText("");
         }
   }
}
      
/**
 * called when OK or Cancel buttons pressed.
 */
void 
GuiShapeOrVectorsFromCoordinateSubtractionDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      showWaitCursor();
      
      const BrainModelSurface* bmsA = surfaceAComboBox->getSelectedBrainModelSurface();
      if (bmsA == NULL) {
         GuiMessageBox::critical(this, "ERROR", "Surface A is invalid.", "OK");
         return;
      }
      
      const BrainModelSurface* bmsB = surfaceBComboBox->getSelectedBrainModelSurface();
      if (bmsB == NULL) {
         GuiMessageBox::critical(this, "ERROR", "Surface B is invalid.", "OK");
         return;
      }
      
      if (bmsA->getNumberOfNodes() <= 0) {
         GuiMessageBox::critical(this, "ERROR", "Surface A has no nodes.", "OK");
         return;
      }
      if (bmsA->getNumberOfNodes() != bmsB->getNumberOfNodes()) {
         GuiMessageBox::critical(this, "ERROR", "Surface A and B have a different number of nodes.", "OK");
         return;
      }
      if (bmsA == bmsB) {
         GuiMessageBox::critical(this, "ERROR", "Surface A and B are the same.", "OK");
         return;
      }
      
      const int column = columnComboBox->currentIndex();
      const QString columnName = columnNameLineEdit->text();
      const QString columnComment = columnCommentLineEdit->text();
      
      switch (mode) {
         case MODE_SHAPE:
            {
               SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
               try {
                  ssf->addColumnOfCoordinateDifference(bmsA->getCoordinateFile(),
                                       bmsB->getCoordinateFile(),
                                       bmsA->getTopologyFile(),
                                       column,
                                       columnName,
                                       columnComment);
               }
               catch (FileException& e) {
                  GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
                  return;
               }
                              
               GuiFilesModified fm;
               fm.setSurfaceShapeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
            break;
         case MODE_VECTOR:
            {
               SurfaceVectorFile* svf = theMainWindow->getBrainSet()->getSurfaceVectorFile();
               try {
                  svf->addUpdateColumn(bmsA->getCoordinateFile(),
                                       bmsB->getCoordinateFile(),
                                       bmsA->getTopologyFile(),
                                       column,
                                       columnName,
                                       columnComment);
               }
               catch (FileException& e) {
                  GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
                  return;
               }
                              
               DisplaySettingsSurfaceVectors* dssv = theMainWindow->getBrainSet()->getDisplaySettingsSurfaceVectors();
               if (dssv->getDisplayMode() == DisplaySettingsSurfaceVectors::DISPLAY_MODE_NONE) {
                  dssv->setDisplayMode(DisplaySettingsSurfaceVectors::DISPLAY_MODE_SPARSE);
               }
               dssv->update();

               GuiFilesModified fm;
               fm.setSurfaceVectorModified();
               theMainWindow->fileModificationUpdate(fm);
            }
      }

      GuiBrainModelOpenGL::updateAllGL();
      
      showNormalCursor();
      beep();
   }
   
   QtDialogModal::done(r);
}
