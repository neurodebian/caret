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
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QSlider>

#include "BrainModelContours.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiContourSectionControlDialog.h"
#include "WuQWidgetGroup.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiContourSectionControlDialog::GuiContourSectionControlDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Contour Section Control");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Button Group for number of sections control
   //
   QGroupBox* sectionButtonBox = new QGroupBox("Sections");
   dialogLayout->addWidget(sectionButtonBox);
   QButtonGroup* sectionButtonGroup= new QButtonGroup(this);
   QVBoxLayout* sectionButtonsLayout = new QVBoxLayout(sectionButtonBox);
   QObject::connect(sectionButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(sectionTypeSlot(int)));
   
   //
   // Single, Multiple, All Radio buttons
   //
   singleSectionRadioButton   = new QRadioButton("Single");
   multipleSectionRadioButton = new QRadioButton("Multiple");
   allSectionRadioButton      = new QRadioButton("Show All");
   hideSectionRadioButton     = new QRadioButton("Hide All");
   sectionButtonsLayout->addWidget(singleSectionRadioButton);
   sectionButtonsLayout->addWidget(multipleSectionRadioButton);
   sectionButtonsLayout->addWidget(allSectionRadioButton);
   sectionButtonsLayout->addWidget(hideSectionRadioButton);
   sectionButtonGroup->addButton(singleSectionRadioButton, 0);
   sectionButtonGroup->addButton(multipleSectionRadioButton, 1);
   sectionButtonGroup->addButton(allSectionRadioButton, 2); 
   sectionButtonGroup->addButton(hideSectionRadioButton, 3);  
   
   //
   // Group Box for sliders
   //
   QGroupBox* sliderGroup = new QGroupBox("Section Selection");
   QHBoxLayout* sliderLayout = new QHBoxLayout(sliderGroup);
   dialogLayout->addWidget(sliderGroup);
   
   //
   //
   // Vertical Box for minimum slider and labels
   //
   QVBoxLayout* minimumQVBoxLayout = new QVBoxLayout;
   sliderLayout->addLayout(minimumQVBoxLayout);
   
   //
   // Horizontal box for labels and labels for minimum slider
   //
   QLabel* lowLabel = new QLabel("Low ");
   minimumQVBoxLayout->addWidget(lowLabel);
   lowLabel->setFixedSize(lowLabel->sizeHint());
   minimumSpinBox = new QSpinBox;
   minimumSpinBox->setMinimum(2);
   minimumSpinBox->setMaximum(100);
   minimumSpinBox->setSingleStep(1);
   QObject::connect(minimumSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMinimumSpinBox(int)));
   minimumQVBoxLayout->addWidget(minimumSpinBox);
                       
   //
   //
   // Minimum slider
   //
   minimumSlider = new QSlider(Qt::Vertical);
   minimumSlider->setMinimum(1);
   minimumSlider->setMaximum(100);
   minimumSlider->setSingleStep(1);
   QObject::connect(minimumSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(minimumSliderMovedSlot(int)));
   QObject::connect(minimumSlider, SIGNAL(sliderReleased()),
                    this, SLOT(sliderReleasedSlot()));
   minimumQVBoxLayout->addWidget(minimumSlider);
   
   //
   // Vertical Box for maximum slider and labels
   //
   QVBoxLayout* maximumQVBoxLayout = new QVBoxLayout;
   sliderLayout->addLayout(maximumQVBoxLayout);
   
   //
   // Horizontal box for labels and labels for maximum slider
   //
   QLabel* highLabel = new QLabel("High ");
   maximumQVBoxLayout->addWidget(highLabel);
   highLabel->setFixedSize(highLabel->sizeHint());
   maximumSpinBox = new QSpinBox;
   maximumSpinBox->setMinimum(1);
   maximumSpinBox->setMaximum(100);
   maximumSpinBox->setSingleStep(1);
   QObject::connect(maximumSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMaximumSpinBox(int)));
   maximumQVBoxLayout->addWidget(maximumSpinBox);
   
   //
   //
   // Maximum slider
   //
   maximumSlider = new QSlider(Qt::Vertical);
   maximumSlider->setMinimum(1);
   maximumSlider->setMaximum(100);
   maximumSlider->setSingleStep(1);
   QObject::connect(maximumSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(maximumSliderMovedSlot(int)));
   QObject::connect(maximumSlider, SIGNAL(sliderReleased()),
                    this, SLOT(sliderReleasedSlot()));
   maximumQVBoxLayout->addWidget(maximumSlider);
   
   //
   // Keep track of the section adjustment widgets
   //
   adjustSectionsWidgetGroup = new WuQWidgetGroup(this);
   adjustSectionsWidgetGroup->addWidget(minimumSlider);
   adjustSectionsWidgetGroup->addWidget(maximumSlider);
   adjustSectionsWidgetGroup->addWidget(minimumSpinBox);
   adjustSectionsWidgetGroup->addWidget(maximumSpinBox);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setFixedSize(closeButton->sizeHint());
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   updateDialog();
}

/**
 * Destructor.
 */
GuiContourSectionControlDialog::~GuiContourSectionControlDialog()
{
}

/**
 * Slot for release of minimum slider
 */
void
GuiContourSectionControlDialog::minimumSliderMovedSlot(int value)
{
   //
   // block signals from spin boxes to prevent display redraw while slider dragged
   //
   minimumSpinBox->blockSignals(true);
   maximumSpinBox->blockSignals(true);
   
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();
   
   if (cf->getSectionType() == ContourFile::SECTION_TYPE_SINGLE) {
      maximumSlider->setValue(value);
      maximumSpinBox->setValue(value);
   }
   else if (cf->getSectionType() == ContourFile::SECTION_TYPE_MULTIPLE) {
      if (value > maximumSlider->value()) {
         maximumSlider->setValue(value);
         maximumSpinBox->setValue(value);
      }
   }
   minimumSpinBox->setValue(value);

   //
   // enable signals from spin boxes now that the displayed value has been changed
   //
   minimumSpinBox->blockSignals(false);
   maximumSpinBox->blockSignals(false);
}

/**
 * Slot for release of maximum slider
 */
void
GuiContourSectionControlDialog::maximumSliderMovedSlot(int value)
{
   //
   // block signals from spin boxes to prevent display redraw while slider dragged
   //
   minimumSpinBox->blockSignals(true);
   maximumSpinBox->blockSignals(true);
   
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();

   if (cf->getSectionType() == ContourFile::SECTION_TYPE_SINGLE) {
      minimumSlider->setValue(value);
      minimumSpinBox->setValue(value);
   }
   else if (cf->getSectionType() == ContourFile::SECTION_TYPE_MULTIPLE) {
      if (value < minimumSlider->value()) {
         minimumSlider->setValue(value);
         minimumSpinBox->setValue(value);
      }
   }
   maximumSpinBox->setValue(value);
   
   //
   // enable signals from spin boxes now that the displayed value has been changed
   //
   minimumSpinBox->blockSignals(false);
   maximumSpinBox->blockSignals(false);   
}

/**
 * Update due to change in sections.
 */
void
GuiContourSectionControlDialog::updateDialog()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();
   
   //
   // Set the selection type radio buttons
   //
   bool slidersEnabled = true;
   switch(cf->getSectionType()) {
      case ContourFile::SECTION_TYPE_MULTIPLE:
         multipleSectionRadioButton->setChecked(true);
         break;
      case ContourFile::SECTION_TYPE_ALL:
         allSectionRadioButton->setChecked(true);
         slidersEnabled = false;
         break;
      case ContourFile::SECTION_TYPE_HIDE:
         hideSectionRadioButton->setChecked(true);
         slidersEnabled = false;
         break;
      case ContourFile::SECTION_TYPE_SINGLE:
      default:
         singleSectionRadioButton->setChecked(true);
         if (cf->getMinimumSelectedSection() != cf->getMaximumSelectedSection()) {
            cf->setMaximumSelectedSection(cf->getMinimumSelectedSection());
         }
         break;
   }
   
   //
   // block signals to adjustment widgets
   //
   adjustSectionsWidgetGroup->blockSignals(true);
   
   //
   // Enable the sliders and spin boxes if all is NOT selected
   //
   minimumSlider->setEnabled(slidersEnabled);
   maximumSlider->setEnabled(slidersEnabled);
   minimumSpinBox->setEnabled(slidersEnabled);
   maximumSpinBox->setEnabled(slidersEnabled);

   //
   // Set the slider and spin box min/max range
   //
   minimumSlider->setMinimum(cf->getMinimumSection());
   minimumSlider->setMaximum(cf->getMaximumSection());
   maximumSlider->setMinimum(cf->getMinimumSection());
   maximumSlider->setMaximum(cf->getMaximumSection());
   minimumSpinBox->setMinimum(cf->getMinimumSection());
   minimumSpinBox->setMaximum(cf->getMaximumSection());
   maximumSpinBox->setMinimum(cf->getMinimumSection());
   maximumSpinBox->setMaximum(cf->getMaximumSection());

   //
   // Set the slider and spin box current value
   //
   minimumSlider->setValue(cf->getMinimumSelectedSection());
   maximumSlider->setValue(cf->getMaximumSelectedSection());
   minimumSpinBox->setValue(cf->getMinimumSelectedSection());
   maximumSpinBox->setValue(cf->getMaximumSelectedSection());

   //
   // unblock signals to adjustment widgets
   //
   adjustSectionsWidgetGroup->blockSignals(false);
}

/**
 * Called when one of the selection type radio buttons is pressed.
 */
void
GuiContourSectionControlDialog::sectionTypeSlot(int item)
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();
 
   ContourFile::SECTION_TYPE selType = static_cast<ContourFile::SECTION_TYPE>(item);
   cf->setSectionType(selType);
   if (selType == ContourFile::SECTION_TYPE_ALL) {
      cf->setMinimumSelectedSection(cf->getMinimumSection());
      cf->setMaximumSelectedSection(cf->getMaximumSection());
   }
   else if (selType == ContourFile::SECTION_TYPE_SINGLE) {
      cf->setMinimumSelectedSection(minimumSlider->value());
      cf->setMaximumSelectedSection(minimumSlider->value());
   }
   updateDialog();
   GuiFilesModified fm;
   fm.setContourModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Called when apply button is pressed.
 */
void
GuiContourSectionControlDialog::sliderReleasedSlot()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();

   cf->setMinimumSelectedSection(minimumSlider->value());
   cf->setMaximumSelectedSection(maximumSlider->value());
   
   GuiFilesModified fm;
   fm.setContourModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Called when minimum spin box value is changed.
 */
void
GuiContourSectionControlDialog::slotMinimumSpinBox(int value)
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();
   
   cf->setMinimumSelectedSection(value);
   
   if (cf->getSectionType() == ContourFile::SECTION_TYPE_SINGLE) {
      cf->setMaximumSelectedSection(value);
   }
   else if (cf->getSectionType() == ContourFile::SECTION_TYPE_MULTIPLE) {
      if (cf->getMinimumSelectedSection() > cf->getMaximumSelectedSection()) {
         cf->setMaximumSelectedSection(value);
      }
   }
   
   updateDialog();
   GuiFilesModified fm;
   fm.setContourModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Called when maximum spin box value is changed.
 */
void
GuiContourSectionControlDialog::slotMaximumSpinBox(int value)
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();

   cf->setMaximumSelectedSection(value);
   
   if (cf->getSectionType() == ContourFile::SECTION_TYPE_SINGLE) {
      cf->setMinimumSelectedSection(value);
   }
   else if (cf->getSectionType() == ContourFile::SECTION_TYPE_MULTIPLE) {
      if (cf->getMaximumSelectedSection() < cf->getMinimumSelectedSection()) {
         cf->setMinimumSelectedSection(value);
      }
   }
   
   updateDialog();
   GuiFilesModified fm;
   fm.setContourModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

