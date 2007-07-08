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

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "BrainModelSurfaceCurvature.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiGenerateSurfaceCurvatureDialog.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "QtUtilities.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiGenerateSurfaceCurvatureDialog::GuiGenerateSurfaceCurvatureDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Generate Curvature");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // surface group box
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surface");
   dialogLayout->addWidget(surfaceGroupBox);
   QVBoxLayout* surfaceGroupLayout = new QVBoxLayout(surfaceGroupBox);
   
   //
   // Fiducial Surface comb box
   //
   surfaceComboBox = new GuiBrainModelSelectionComboBox(
                                          false,
                                          true,
                                          false,
                                          "",
                                          0);
   surfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainModelSurface());
   surfaceGroupLayout->addWidget(surfaceComboBox);
   
   //
   // Group Box for line edits and combo boxes
   //
   QGroupBox* groupBox = new QGroupBox("Surface Shape Columns");
   dialogLayout->addWidget(groupBox);
   QGridLayout* groupGridLayout = new QGridLayout(groupBox);
   
   //
   // Mean Curvature Name line edit
   //
   meanCurvatureNameLineEdit = new QLineEdit;
   meanCurvatureNameLineEdit->setText("Folding (Mean Curvature)");
   groupGridLayout->addWidget(meanCurvatureNameLineEdit, 0, 0);
   
   //
   // Mean curvature column combo box
   //
   meanCurvatureColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                         GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                         true,
                                         true,
                                         false);
   meanCurvatureColumnComboBox->setCurrentIndex(
      GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
   groupGridLayout->addWidget(meanCurvatureColumnComboBox, 0, 1);
   
   //
   // Gaussian Curvature Name line edit
   //
   gaussianCurvatureNameLineEdit = new QLineEdit(groupBox);
   gaussianCurvatureNameLineEdit->setText("Gaussian Curvature");
   groupGridLayout->addWidget(gaussianCurvatureNameLineEdit, 1, 0);
   
   //
   // Gaussian curvature column combo box
   //
   gaussianCurvatureColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                         GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
                                         true,
                                         true,
                                         false);
   gaussianCurvatureColumnComboBox->setCurrentIndex(
      GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
   groupGridLayout->addWidget(gaussianCurvatureColumnComboBox, 1, 1);
   
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * Destructor
 */
GuiGenerateSurfaceCurvatureDialog::~GuiGenerateSurfaceCurvatureDialog()
{
}

/**
 * Called when OK or Cancel button pressed.
 */
void
GuiGenerateSurfaceCurvatureDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      //
      // Get the surface for curvature calculation
      //
      BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
      if (bms == NULL) {
         GuiMessageBox::critical(this, "Error",
                               "A surface must be selected.", "OK");
         return;
      }
      
      //
      // Get mean curvature selections
      //
      int meanCurvatureColumn = meanCurvatureColumnComboBox->currentIndex();
      if (meanCurvatureColumn == 
          GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
         meanCurvatureColumn = BrainModelSurfaceCurvature::CURVATURE_COLUMN_CREATE_NEW;
      }
      else if (meanCurvatureColumn == 
               GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE) {
         meanCurvatureColumn = BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE;
      }
      const QString meanCurvatureName(meanCurvatureNameLineEdit->text());
      if (meanCurvatureColumn != BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE) {
         if (meanCurvatureName.isEmpty()) {
            GuiMessageBox::critical(this, "Error",
                                  "Name for mean curvature must be provided.", "OK");
            return;
         }
      }
      
      //
      // Get gaussian curvature selections
      //
      int gaussianCurvatureColumn = gaussianCurvatureColumnComboBox->currentIndex();
      if (gaussianCurvatureColumn == 
          GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
         gaussianCurvatureColumn = BrainModelSurfaceCurvature::CURVATURE_COLUMN_CREATE_NEW;
      }
      else if (gaussianCurvatureColumn == 
               GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE) {
         gaussianCurvatureColumn = BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE;
      }
      const QString gaussianCurvatureName(gaussianCurvatureNameLineEdit->text());
      if (gaussianCurvatureColumn != BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE) {
         if (gaussianCurvatureName.isEmpty()) {
            GuiMessageBox::critical(this, "Error",
                                  "Name for gaussian curvature must be provided.", "OK");
            return;
         }
      }
      
      //
      // Make sure at least one curvature is being generated
      //
      if ((meanCurvatureColumn == BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE) &&
          (gaussianCurvatureColumn == BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE)) {
         GuiMessageBox::critical(this, "Error", "No shape file columns selected", "OK");
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      try {
         //
         // Generate curvature
         //
         BrainModelSurfaceCurvature bmsc(theMainWindow->getBrainSet(),
                                         bms,
                                         theMainWindow->getBrainSet()->getSurfaceShapeFile(),
                                         meanCurvatureColumn,
                                         gaussianCurvatureColumn,
                                         meanCurvatureName,
                                         gaussianCurvatureName);
         bmsc.execute();
         
         //
         // Update surface shape related items and redraw
         //
         GuiFilesModified fm;
         fm.setSurfaceShapeModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
      }
      catch (BrainModelAlgorithmException& e) {
         GuiMessageBox::critical(this, "Error", e.whatQString(), "OK");
         return;
      }
      
      QApplication::restoreOverrideCursor();
   }
   
   QDialog::done(r);
}

