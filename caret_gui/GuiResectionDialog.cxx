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
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceResection.h"
#include "BrainSet.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiResectionDialog.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "SectionFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiResectionDialog::GuiResectionDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setModal(true);
   setWindowTitle("Resectioning");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Vertical group box for surface
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surface");
   dialogLayout->addWidget(surfaceGroupBox);
   QVBoxLayout* surfaceGroupLayout = new QVBoxLayout(surfaceGroupBox);
   
   //
   // surface combo box
   //
   surfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                        true,
                                                        false,
                                                        "",
                                                        0);
   surfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainModelSurface());
   surfaceGroupLayout->addWidget(surfaceComboBox);
   
   //
   // Horizontal Group box for section file
   //
   QGroupBox* sectionFileGroupBox = new QGroupBox("Section File");
   dialogLayout->addWidget(sectionFileGroupBox);
   QHBoxLayout* sectionFileGroupLayout = new QHBoxLayout(sectionFileGroupBox);
   
   //
   // Section file column and column name
   //
   columnComboBox = new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_SECTION,
                                                                true,
                                                                false,
                                                                false);
   QObject::connect(columnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotColumnComboBox(int)));
   columnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
   sectionFileGroupLayout->addWidget(columnComboBox);
   columnNameLineEdit = new QLineEdit;
   sectionFileGroupLayout->addWidget(columnNameLineEdit);
   
   //
   // Squish the section file group box
   //
   sectionFileGroupBox->setFixedSize(sectionFileGroupBox->sizeHint());
   
   //
   // Vertical group box for axis
   //
   QGroupBox* axisGroupBox = new QGroupBox("Axis");
   dialogLayout->addWidget(axisGroupBox);
   QVBoxLayout* axisGroupBoxLayout = new QVBoxLayout(axisGroupBox);
   
   //
   // axis combo box
   //
   axisComboBox = new QComboBox;
   axisGroupBoxLayout->addWidget(axisComboBox);
   axisComboBox->insertItem(BrainModelSurfaceResection::SECTION_AXIS_X,
                            "X Axis (Saggittal)");
   axisComboBox->insertItem(BrainModelSurfaceResection::SECTION_AXIS_Y,
                            "Y Axis (Coronal)");
   axisComboBox->insertItem(BrainModelSurfaceResection::SECTION_AXIS_Z,
                            "Z Axis (Horizontal)");
   axisComboBox->insertItem(BrainModelSurfaceResection::SECTION_AXIS_Z_WITH_ROTATION_MATRIX,
                            "Screen Z Axis");
   
   //
   // Squish the axis group box
   //
   axisGroupBox->setFixedSize(axisGroupBox->sizeHint());
   
   //
   // Group box for how to section
   //
   QGroupBox* howToGroupBox = new QGroupBox("How To Section");
   dialogLayout->addWidget(howToGroupBox);
   QGridLayout* howToGroupGridLayout = new QGridLayout(howToGroupBox);
   
   //
   // Radio button group for how to radio buttons
   //
   QButtonGroup* howToButtonGroup = new QButtonGroup(this);
   QObject::connect(howToButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotHowToRadioButton()));
                    
   //
   // Thickness radio button and spin box
   //
   numberOfSectionsRadioButton = new QRadioButton("Number of Sections");
   howToGroupGridLayout->addWidget(numberOfSectionsRadioButton, 0, 0);
   howToButtonGroup->addButton(numberOfSectionsRadioButton, 0);
   
   numberOfSectionsSpinBox = new QSpinBox;
   numberOfSectionsSpinBox->setMinimum(1);
   numberOfSectionsSpinBox->setMaximum(1000000);
   numberOfSectionsSpinBox->setSingleStep(1);
   numberOfSectionsSpinBox->setValue(50);
   howToGroupGridLayout->addWidget(numberOfSectionsSpinBox, 0, 1);
                                          
   //
   // Thickness radio button and spin box
   //
   thicknessRadioButton = new QRadioButton("Thickness");
   howToGroupGridLayout->addWidget(thicknessRadioButton, 1, 0);
   howToButtonGroup->addButton(thicknessRadioButton, 1);

   thicknessDoubleSpinBox = new QDoubleSpinBox;
   thicknessDoubleSpinBox->setMinimum(0.001);
   thicknessDoubleSpinBox->setMaximum(1000.0);
   thicknessDoubleSpinBox->setSingleStep(0.5);
   thicknessDoubleSpinBox->setDecimals(3);
   thicknessDoubleSpinBox->setValue(1.0);
   howToGroupGridLayout->addWidget(thicknessDoubleSpinBox, 1, 1);
      
   //
   // Squish the how to group box
   //
   howToGroupBox->setFixedSize(howToGroupBox->sizeHint());
   
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
   
   //
   // Initialize some stuff
   //
   slotColumnComboBox(columnComboBox->currentIndex());
   slotHowToRadioButton();
}

/**
 * Destructor
 */
GuiResectionDialog::~GuiResectionDialog()
{
}

/**
 * Called when a how to radio button is selected.
 */
void
GuiResectionDialog::slotHowToRadioButton()
{
   numberOfSectionsSpinBox->setEnabled(false);
   thicknessDoubleSpinBox->setEnabled(false);
   if (numberOfSectionsRadioButton->isChecked()) {
      numberOfSectionsSpinBox->setEnabled(true);
   }
   else if (thicknessRadioButton->isChecked()) {
      thicknessDoubleSpinBox->setEnabled(true);
   }
}

/**
 * Called when a section file column is selected.
 */
void
GuiResectionDialog::slotColumnComboBox(int item)
{
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if ((item >= 0) && (item < sf->getNumberOfColumns())) {
      columnNameLineEdit->setText(sf->getColumnName(item));
   }
   else {
      columnNameLineEdit->setText("new column name");
   }
}

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiResectionDialog::done(int r)
{
   //
   // Was the OK button pressed ?
   //
   if (r == QDialog::Accepted) {
      //
      // Get name of section file column
      //
      const QString columnName(columnNameLineEdit->text());
      if (columnName.isEmpty()) {
         QMessageBox::critical(this, "ERROR", "Column Name is blank.");
         return;
      }
      
      //
      // Get the column number
      //
      const int columnNumber = columnComboBox->currentIndex();
      
      //
      // Get the surface
      //
      BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
      if (bms == NULL) {
         QMessageBox::critical(this, "ERROR", "There is no surface.");
         return;
      }
      
      //
      // Get the section type
      //
      BrainModelSurfaceResection::SECTION_TYPE sectionType;
      if (numberOfSectionsRadioButton->isChecked()) {
         sectionType = BrainModelSurfaceResection::SECTION_TYPE_NUM_SECTIONS;
      }
      else if (thicknessRadioButton->isChecked()) {
         sectionType = BrainModelSurfaceResection::SECTION_TYPE_THICKNESS;
      }
      else {
            QMessageBox::critical(this, "ERROR", "Select How to Section.");
            return;
      }
      
      //
      // Get the axis
      //
      const BrainModelSurfaceResection::SECTION_AXIS axis = 
         static_cast<BrainModelSurfaceResection::SECTION_AXIS>(axisComboBox->currentIndex());
         
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      BrainModelSurfaceResection bmsr(theMainWindow->getBrainSet(),
                                      bms,
                                      bms->getRotationTransformMatrix(0),
                                      axis,
                                      sectionType,
                                      theMainWindow->getBrainSet()->getSectionFile(),
                                      columnNumber,
                                      columnName,
                                      thicknessDoubleSpinBox->value(),
                                      numberOfSectionsSpinBox->value());
      try {
         bmsr.execute();
         GuiFilesModified fm;
         fm.setSectionModified();
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
