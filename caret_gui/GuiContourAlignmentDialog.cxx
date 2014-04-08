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

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QSpinBox>

#include "BrainModelContours.h"
#include "BrainSet.h"
#include "ContourCellFile.h"
#include "DisplaySettingsContours.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiContourAlignmentDialog.h"
#include "GuiMainWindow.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiContourAlignmentDialog::GuiContourAlignmentDialog(QWidget* parent, bool modalFlag, Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   setModal(modalFlag);
   setWindowTitle("Contour Alignment");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Section number spin box
   //
   QHBoxLayout* sectionNumberLayout = new QHBoxLayout;
   dialogLayout->addLayout(sectionNumberLayout);
   sectionNumberLayout->addWidget(new QLabel("Section " ));
   sectionNumberSpinBox = new QSpinBox;
   sectionNumberSpinBox->setMinimum(0);
   sectionNumberSpinBox->setMaximum(10);
   sectionNumberSpinBox->setSingleStep(1);
   QObject::connect(sectionNumberSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotSectionNumberSpinBox()));
   sectionNumberLayout->addWidget(sectionNumberSpinBox);
   
   //
   // Apply alignment button
   //
   QPushButton* applyAlignmentButton = new QPushButton("Apply Alignment");
   dialogLayout->addWidget(applyAlignmentButton);
   applyAlignmentButton->setAutoDefault(false);
   applyAlignmentButton->setFixedSize(applyAlignmentButton->sizeHint());
   QObject::connect(applyAlignmentButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyAlignmentButton()));
   
   //
   // Reset view button
   //
   QPushButton* resetViewButton = new QPushButton("Reset View");
   dialogLayout->addWidget(resetViewButton);
   resetViewButton->setAutoDefault(false);
   resetViewButton->setFixedSize(resetViewButton->sizeHint());
   QObject::connect(resetViewButton, SIGNAL(clicked()),
                    this, SLOT(slotResetViewButton()));
   
   //
   // Select region button
   //
   QPushButton* selectRegionButton = new QPushButton("Select Region");
   dialogLayout->addWidget(selectRegionButton);
   selectRegionButton->setAutoDefault(false);
   selectRegionButton->setFixedSize(selectRegionButton->sizeHint());
   QObject::connect(selectRegionButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectRegionButton()));
                    
   //
   // Clear region button
   //
   QPushButton* clearRegionButton = new QPushButton("Clear Region");
   dialogLayout->addWidget(clearRegionButton);
   clearRegionButton->setAutoDefault(false);
   clearRegionButton->setFixedSize(clearRegionButton->sizeHint());
   QObject::connect(clearRegionButton, SIGNAL(clicked()),
                    this, SLOT(slotClearRegionButton()));
   
   //
   // Horizontal layout for dialog buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseButton()));
   closeButton->setFixedSize(closeButton->sizeHint());
   
   updateDialog();
}

/**
 * show the dialog.
 */
void 
GuiContourAlignmentDialog::show()
{
   slotSectionNumberSpinBox();
   WuQDialog::show();
}

/**
 * Destructor
 */
GuiContourAlignmentDialog::~GuiContourAlignmentDialog()
{
}

/**
 * Called when select region button is pressed.
 */
void 
GuiContourAlignmentDialog::slotSelectRegionButton()
{
   static bool firstTime = true;
   if (firstTime) {
      const QString msg("Region alignment will not function\n"
                        "if the contours are rotated.");
      QMessageBox::information(this, "Information", msg);
      firstTime = false;
   }
   
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                                GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN_REGION);
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   bmc->setAligningRegionFlag(true);
   bmc->resetAlignmentRegionBox();
   ContourFile* contourFile = bmc->getContourFile();
   contourFile->clearSpecialFlags();
   ContourCellFile* contourCells = theMainWindow->getBrainSet()->getContourCellFile();
   contourCells->clearAllSpecialFlags();

   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Called when clear region button is pressed.
 */
void 
GuiContourAlignmentDialog::slotClearRegionButton()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   GuiBrainModelOpenGL* bmo = theMainWindow->getBrainModelOpenGL();
   if (bmo->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN_REGION) {
      bmo->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN);
   }
   bmc->setAligningRegionFlag(false);
   ContourFile* contourFile = bmc->getContourFile();
   contourFile->clearSpecialFlags();
   ContourCellFile* contourCells = theMainWindow->getBrainSet()->getContourCellFile();
   contourCells->clearAllSpecialFlags();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Get the alignment section number.
 */
int
GuiContourAlignmentDialog::getAlignmentSectionNumber() const
{
   return sectionNumberSpinBox->value();
}

/**
 * update the dialog due to contour changes.
 */
void 
GuiContourAlignmentDialog::updateDialog()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc != NULL) {
      ContourFile* cf = bmc->getContourFile();
      sectionNumberSpinBox->blockSignals(true);
      sectionNumberSpinBox->setMinimum(cf->getMinimumSection());
      sectionNumberSpinBox->setMaximum(cf->getMaximumSection());
      sectionNumberSpinBox->blockSignals(false);
   }
}      

/**
 * Called when apply alignment button is pressed.
 */
void
GuiContourAlignmentDialog::slotApplyAlignmentButton()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc != NULL) {
      //
      // Note: OpenGL transform is transposed compared to VTK
      //
      bmc->applyAlignmentToSection(sectionNumberSpinBox->value());
      bmc->resetAlignmentTransformations();
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * Called when close button is pressed.
 */
void
GuiContourAlignmentDialog::slotCloseButton()
{
   GuiBrainModelOpenGL* bmo = theMainWindow->getBrainModelOpenGL();
   if ((bmo->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN) ||
       (bmo->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN_REGION)) {
      bmo->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }
   
   DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();
   dsc->setAlignmentContourValid(false);
   GuiBrainModelOpenGL::updateAllGL();
   close();
}

/**
 * Called when reset view button is pressed.
 */
void
GuiContourAlignmentDialog::slotResetViewButton()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc != NULL) {
      bmc->resetAlignmentTransformations();
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
                                GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN);
      bmc->setAligningRegionFlag(false);
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * Called when value of section number spin box is changed.
 */
void GuiContourAlignmentDialog::slotSectionNumberSpinBox()
{
   DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();
   dsc->setAlignmentContourNumber(sectionNumberSpinBox->value());
   dsc->setAlignmentContourValid(true);
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                                GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN);
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   bmc->setAligningRegionFlag(false);
   ContourFile* contourFile = bmc->getContourFile();
   contourFile->clearSpecialFlags();
   ContourCellFile* contourCells = theMainWindow->getBrainSet()->getContourCellFile();
   contourCells->clearAllSpecialFlags();
   GuiBrainModelOpenGL::updateAllGL();
}
