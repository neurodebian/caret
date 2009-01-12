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
#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextEdit>

#include "ArealEstimationFile.h"
#include "BorderFile.h"
#include "BorderUncertaintyToArealEstimationConverter.h"
#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "GuiFilesModified.h"
#include "GuiGenerateArealEstimationDialog.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiPaintColumnNamesListBoxSelectionDialog.h"
#include "PaintFile.h"
#include <QDoubleSpinBox>
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiGenerateArealEstimationDialog::GuiGenerateArealEstimationDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setModal(true);
   setWindowTitle("Create Areal Estimation File");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   //
   // Groupbox for areal estimation file
   //
   QGroupBox* arealEstGroupBox = new QGroupBox("Areal Estimation File Column");
   dialogLayout->addWidget(arealEstGroupBox);
   QHBoxLayout* arealEstGroupLayout = new QHBoxLayout(arealEstGroupBox);
   
   //
   // column selection, and column name line edit
   //
   arealEstColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                  GUI_NODE_FILE_TYPE_AREAL_ESTIMATION,
                                                                  true,
                                                                  false,
                                                                  false);
   QObject::connect(arealEstColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotArealEstComboBox(int)));
   arealEstColumnNameLineEdit = new QLineEdit(arealEstGroupBox);
   slotArealEstComboBox(arealEstColumnComboBox->currentIndex());
   arealEstGroupLayout->addWidget(arealEstColumnComboBox);
   arealEstGroupLayout->addWidget(arealEstColumnNameLineEdit);

   //
   // Groupbox for node selection
   //
   QGroupBox* nodeGroupBox = new QGroupBox("Node Selection");
   dialogLayout->addWidget(nodeGroupBox);
   QVBoxLayout* nodeGroupLayout = new QVBoxLayout(nodeGroupBox);
   
   //
   // Button group for node selection
   //
   QButtonGroup* nodesButtonGroup = new QButtonGroup(this);
   
   //
   // Apply to all nodes radio button
   //
   nodesAllRadioButton = new QRadioButton("Apply to All Nodes");
   nodesButtonGroup->addButton(nodesAllRadioButton);
   nodeGroupLayout->addWidget(nodesAllRadioButton);
   
   //
   // Restrict to nodes button group
   //
   nodesRestrictRadioButton = new QRadioButton("Restrict to Nodes With:");
   nodesButtonGroup->addButton(nodesRestrictRadioButton);
   nodeGroupLayout->addWidget(nodesRestrictRadioButton);
      
   //
   // Grid for paint selection
   //
   QGridLayout* paintGridLayout = new QGridLayout;
   nodeGroupLayout->addLayout(paintGridLayout);
   
   //
   // paint type label and combo box
   //
   paintGridLayout->addWidget(new QLabel("Paint Type"), 0, 0);
   paintTypeComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                   GUI_NODE_FILE_TYPE_PAINT,
                                                                   false,
                                                                   false,
                                                                   false);
   paintGridLayout->addWidget(paintTypeComboBox, 0, 1);
                                                      
   //
   // paint name label, line edit, and push button
   //
   paintGridLayout->addWidget(new QLabel("Paint Name"), 1, 0);
   paintNameLineEdit = new QLineEdit;
   paintGridLayout->addWidget(paintNameLineEdit, 1, 1);
   QPushButton* paintNamePushButton = new QPushButton("Select...");
   paintGridLayout->addWidget(paintNamePushButton);
   paintNamePushButton->setAutoDefault(false);
   QObject::connect(paintNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintNamePushButton()));
   
   //
   //  Groupbox for border uncertainty
   //
   QGroupBox* borderGroupBox = new QGroupBox("Border Uncertainty Source");
   dialogLayout->addWidget(borderGroupBox);
   QVBoxLayout* borderGroupLayout = new QVBoxLayout(borderGroupBox);
   
   //
   // Button group for uncertainty selection
   //
   QButtonGroup* undertaintyButtonGroup = new QButtonGroup(this);
   
   //
   // border file uncertainty label
   //
   borderFileUncertaintyRadioButton = new QRadioButton("Border File");
   undertaintyButtonGroup->addButton(borderFileUncertaintyRadioButton);
   borderGroupLayout->addWidget(borderFileUncertaintyRadioButton);
   
   //
   // border file override label and spin box
   //
   QHBoxLayout* overrideLayout = new QHBoxLayout;
   borderGroupLayout->addLayout(overrideLayout);
   borderOverrideUncertaintyRadioButton = new QRadioButton("Override ");
   overrideLayout->addWidget(borderOverrideUncertaintyRadioButton);
   undertaintyButtonGroup->addButton(borderOverrideUncertaintyRadioButton);
   borderOverrideDoubleSpinBox = new QDoubleSpinBox;
   borderOverrideDoubleSpinBox->setMinimum(0.001);
   borderOverrideDoubleSpinBox->setMaximum(10000.0);
   borderOverrideDoubleSpinBox->setSingleStep(1.0);
   borderOverrideDoubleSpinBox->setDecimals(2);
   overrideLayout->addWidget(borderOverrideDoubleSpinBox);
   borderOverrideDoubleSpinBox->setValue(4.0);
   
   //
   // Naming group box
   //
   QGroupBox* namingGroupBox = new QGroupBox("Naming");
   dialogLayout->addWidget(namingGroupBox);
   QGridLayout* namingGroupLayout = new QGridLayout(namingGroupBox);
   
   //
   // long name label and line edit
   //
   namingGroupLayout->addWidget(new QLabel("Long Name"), 0, 0);
   longNameLineEdit = new QLineEdit(namingGroupBox);
   namingGroupLayout->addWidget(longNameLineEdit, 0, 1);
   
   //
   // comment label and text box
   //
   namingGroupLayout->addWidget(new QLabel("Comment"), 1, 0);
   commentTextEdit = new QTextEdit(namingGroupBox);
   namingGroupLayout->addWidget(commentTextEdit, 1, 1);
   
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
GuiGenerateArealEstimationDialog::~GuiGenerateArealEstimationDialog()
{
}

/**
 *
 */
void
GuiGenerateArealEstimationDialog::slotArealEstComboBox(int item)
{
   ArealEstimationFile* aef = theMainWindow->getBrainSet()->getArealEstimationFile();
   QString name("New Column Name");
   if ((item >= 0) && (item < aef->getNumberOfColumns())) {
      name = aef->getColumnName(item);
   }
   arealEstColumnNameLineEdit->setText(name);
}

/**
 * called when paint name select button pressed.
 */
void 
GuiGenerateArealEstimationDialog::slotPaintNamePushButton()
{
   const int column = paintTypeComboBox->currentIndex();
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   if ((column >= 0) && (column < pf->getNumberOfColumns())) {
      GuiPaintColumnNamesListBoxSelectionDialog pnd(this, column);
      if (pnd.exec() == QDialog::Accepted) {
         paintNameLineEdit->setText(pnd.getSelectedText());
      }
   }
   else {
      QMessageBox::critical(this, "ERROR", "Invalid paint column");
   }
}      

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiGenerateArealEstimationDialog::done(int r)
{
   //
   // Was the OK button pressed ?
   //
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
      if (bms == NULL) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", "There is no surface in the main window.");
         return;
      }
      
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      BorderFile borderFile;
      bmbs->copyBordersToBorderFile(bms, borderFile);
      if (borderFile.getNumberOfBorders() <= 0) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", "There are no borders on the main window surface.");
         return;
      }
      
      if ((borderFileUncertaintyRadioButton->isChecked() == false) &&
          (borderOverrideUncertaintyRadioButton->isChecked() == false)) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", "You must select a border uncertainty source.");
         return;
      }
      
      if ((nodesAllRadioButton->isChecked() == false) &&
          (nodesRestrictRadioButton->isChecked() == false)) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", "You must make a node selection.");
         return;
      }
      
      BorderUncertaintyToArealEstimationConverter::MODE mode =
              BorderUncertaintyToArealEstimationConverter::MODE_ALL_NODES;
      if (nodesRestrictRadioButton->isChecked()) {
         mode = BorderUncertaintyToArealEstimationConverter::MODE_NODES_WITH_PAINT;
      }
      
      BorderUncertaintyToArealEstimationConverter aec(
         theMainWindow->getBrainSet(),
         bms,
         theMainWindow->getBrainSet()->getArealEstimationFile(),
         &borderFile,
         theMainWindow->getBrainSet()->getPaintFile(),
         mode,
         arealEstColumnComboBox->currentIndex(),
         arealEstColumnNameLineEdit->text(),
         longNameLineEdit->text(),
         commentTextEdit->toPlainText(), 
         paintTypeComboBox->currentIndex(),
         paintNameLineEdit->text(),
         borderOverrideUncertaintyRadioButton->isChecked(),
         borderOverrideDoubleSpinBox->value());
         
      try {
         aec.execute();
         GuiFilesModified fm;
         fm.setArealEstimationModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
         return;
      }
      QApplication::restoreOverrideCursor();
   }
   QDialog::done(r);
}
