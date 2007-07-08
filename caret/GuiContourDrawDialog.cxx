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
#include <QPushButton>
#include <QSpinBox>

#include "GuiBrainModelOpenGL.h"
#include "GuiContourDrawDialog.h"
#include "GuiMainWindow.h"
#include <QDoubleSpinBox>
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiContourDrawDialog::GuiContourDrawDialog(QWidget* parent, bool modal, Qt::WFlags f)
   : QtDialog(parent, modal, f)
{
   setWindowTitle("Contour Drawing");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Grid box
   //
   QGridLayout* grid = new QGridLayout;
   dialogLayout->addLayout(grid);
   
   //
   // Point spacing label and float spin box
   //
   grid->addWidget(new QLabel("Point Spacing (mm) "), 0, 0);
   pointSpacingDoubleSpinBox = new QDoubleSpinBox;
   pointSpacingDoubleSpinBox->setMinimum(0.0);
   pointSpacingDoubleSpinBox->setMaximum(100.0);
   pointSpacingDoubleSpinBox->setSingleStep(0.5);
   pointSpacingDoubleSpinBox->setDecimals(2);
   pointSpacingDoubleSpinBox->setValue(0.5);
   grid->addWidget(pointSpacingDoubleSpinBox, 0, 1);
   //
   // Section number spin box and label
   //
   grid->addWidget(new QLabel("Section Number "), 1, 0);
   sectionNumberSpinBox = new QSpinBox;
   sectionNumberSpinBox->setMinimum(-1000000);
   sectionNumberSpinBox->setMaximum( 1000000);
   sectionNumberSpinBox->setSingleStep(1);
   sectionNumberSpinBox->setValue(0);
   grid->addWidget(sectionNumberSpinBox, 1, 1);  
   QObject::connect(sectionNumberSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSectionNumberSpinBox()));
                                                    
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button 
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseButton()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
}

/**
 * Destructor
 */
GuiContourDrawDialog::~GuiContourDrawDialog()
{
}

/**
 * called when section changed.
 */
void 
GuiContourDrawDialog::slotSectionNumberSpinBox()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DRAW);
}
      
/**
 * get the point spacing
 */
float 
GuiContourDrawDialog::getPointSpacing() const
{
   return pointSpacingDoubleSpinBox->value();
}

/**
 * get the section number
 */
int 
GuiContourDrawDialog::getSectionNumber() const
{
   return sectionNumberSpinBox->value();
}
      
/**
 * Called when apply button is pressed.
 */
void
GuiContourDrawDialog::slotApplyButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                    GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DRAW);
}

/**
 * Called when apply button is pressed.
 */
void
GuiContourDrawDialog::slotCloseButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                    GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   close();
}

