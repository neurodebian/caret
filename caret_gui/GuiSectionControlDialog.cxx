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
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QSlider>

#include "BrainSet.h"
#include "DisplaySettingsSection.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiSectionControlDialog.h"
#include "SectionFile.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiSectionControlDialog::GuiSectionControlDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Section Control");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // column selection combo box
   //
   sectionFileColumnComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_SECTION,
                                                  false,
                                                  false,
                                                  false);
   QObject::connect(sectionFileColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(fileColumnComboBoxSlot(int)));
   dialogLayout->addWidget(sectionFileColumnComboBox);
      
   //
   // Single, Multiple, All Radio buttons
   //
   singleSectionRadioButton   = new QRadioButton("Single");
   multipleSectionRadioButton = new QRadioButton("Multiple");
   allSectionRadioButton      = new QRadioButton("All");

   //
   // Button Group for number of sections control
   //
   QGroupBox* sectionsGroupBox = new QGroupBox("Sections");
   QVBoxLayout* sectionsGroupLayout = new QVBoxLayout(sectionsGroupBox);
   sectionsGroupLayout->addWidget(singleSectionRadioButton);
   sectionsGroupLayout->addWidget(multipleSectionRadioButton);
   sectionsGroupLayout->addWidget(allSectionRadioButton);
   dialogLayout->addWidget(sectionsGroupBox);
   
   //
   // Button group to keep buttons mutually exclusive
   //
   QButtonGroup* sectionsButtonGroup = new QButtonGroup(this);
   sectionsButtonGroup->addButton(singleSectionRadioButton, DisplaySettingsSection::SELECTION_TYPE_SINGLE);
   sectionsButtonGroup->addButton(multipleSectionRadioButton, DisplaySettingsSection::SELECTION_TYPE_MULTIPLE);
   sectionsButtonGroup->addButton(allSectionRadioButton, DisplaySettingsSection::SELECTION_TYPE_ALL);
   QObject::connect(sectionsButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(sectionTypeSlot(int)));
   
   //
   // Horizontal box for labels and labels for minimum slider
   //
   QLabel* lowLabel = new QLabel("Low ");
   lowLabel->setFixedSize(lowLabel->sizeHint());
   minimumSpinBox = new QSpinBox;
   minimumSpinBox->setMinimum(1);
   minimumSpinBox->setMaximum(100);
   minimumSpinBox->setSingleStep(1);
   QObject::connect(minimumSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMinimumSpinBox(int)));
                       
   //
   //
   // Minimum slider
   //
   minimumSlider = new QSlider(Qt::Vertical);
   minimumSlider->setSingleStep(1);
   minimumSlider->setPageStep(10);
   QObject::connect(minimumSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(minimumSliderMovedSlot(int)));
   QObject::connect(minimumSlider, SIGNAL(sliderReleased()),
                    this, SLOT(sliderReleasedSlot()));
   
   //
   // Horizontal box for labels and labels for maximum slider
   //
   QLabel* highLabel = new QLabel("High ");
   highLabel->setFixedSize(highLabel->sizeHint());
   maximumSpinBox = new QSpinBox;
   maximumSpinBox->setMinimum(1);
   maximumSpinBox->setMaximum(100);
   maximumSpinBox->setSingleStep(1);
   QObject::connect(maximumSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotMaximumSpinBox(int)));
   
   //
   //
   // Maximum slider
   //
   maximumSlider = new QSlider(Qt::Vertical);
   maximumSlider->setSingleStep(1);
   maximumSlider->setPageStep(10);
   QObject::connect(maximumSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(maximumSliderMovedSlot(int)));
   QObject::connect(maximumSlider, SIGNAL(sliderReleased()),
                    this, SLOT(sliderReleasedSlot()));
   
   //
   // Group Box for sliders
   //
   QGroupBox* sliderGroup = new QGroupBox("Section Selection");
   dialogLayout->addWidget(sliderGroup);
   QGridLayout* sliderGridLayout = new QGridLayout(sliderGroup);
   sliderGridLayout->addWidget(lowLabel, 0, 0);
   sliderGridLayout->addWidget(minimumSpinBox, 1, 0);
   sliderGridLayout->addWidget(minimumSlider, 2, 0);
   sliderGridLayout->addWidget(highLabel, 0, 1);
   sliderGridLayout->addWidget(maximumSpinBox, 1, 1);
   sliderGridLayout->addWidget(maximumSlider, 2, 1);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
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
GuiSectionControlDialog::~GuiSectionControlDialog()
{
}

/**
 * Slot for column selection.
 */
void
GuiSectionControlDialog::fileColumnComboBoxSlot(int item)
{
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() == 0) {
      return;
   }
   dss->setSelectedDisplayColumn(-1, -1, item);
   updateDialog();
   //GuiFilesModified fm;
   //fm.setSectionModified();
   //theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Slot for release of minimum slider
 */
void
GuiSectionControlDialog::minimumSliderMovedSlot(int value)
{
   //
   // block signals from spin boxes to prevent display redraw while slider dragged
   //
   minimumSpinBox->blockSignals(true);
   maximumSpinBox->blockSignals(true);
   
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() <= 0) {
      return;
   }
   //const int column = dss->getSelectedColumn();
   if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_SINGLE) {
      maximumSlider->setValue(value);
      maximumSpinBox->setValue(value);
   }
   else if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_MULTIPLE) {
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
GuiSectionControlDialog::maximumSliderMovedSlot(int value)
{
   //
   // block signals from spin boxes to prevent display redraw while slider dragged
   //
   minimumSpinBox->blockSignals(true);
   maximumSpinBox->blockSignals(true);
   
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() <= 0) {
      return;
   }
   //const int column = dss->getSelectedColumn();
   if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_SINGLE) {
      minimumSlider->setValue(value);
      minimumSpinBox->setValue(value);
   }
   else if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_MULTIPLE) {
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
GuiSectionControlDialog::updateDialog()
{
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() <= 0) {
      return;
   }
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   const int column = dss->getSelectedDisplayColumn(-1, -1);
   
   //
   // Load the column selection combo box
   //
   sectionFileColumnComboBox->updateComboBox(sf);
   
   //
   // Set the selection type radio buttons
   //
   switch(dss->getSelectionType()) {
      case DisplaySettingsSection::SELECTION_TYPE_MULTIPLE:
         multipleSectionRadioButton->setChecked(true);
         break;
      case DisplaySettingsSection::SELECTION_TYPE_ALL:
         allSectionRadioButton->setChecked(true);
         dss->setMinimumSelectedSection(sf->getMinimumSection(column));
         dss->setMaximumSelectedSection(sf->getMaximumSection(column));
         break;
      case DisplaySettingsSection::SELECTION_TYPE_SINGLE:
      default:
         singleSectionRadioButton->setChecked(true);
         if (dss->getMinimumSelectedSection() != dss->getMaximumSelectedSection()) {
            dss->setMaximumSelectedSection(dss->getMinimumSelectedSection());
         }
         break;
   }
   
   //
   // Enable the sliders and spin boxes if all is NOT selected
   //
   minimumSlider->setEnabled( ! allSectionRadioButton->isChecked());
   maximumSlider->setEnabled( ! allSectionRadioButton->isChecked());
   minimumSpinBox->setEnabled( ! allSectionRadioButton->isChecked());
   maximumSpinBox->setEnabled( ! allSectionRadioButton->isChecked());

   //
   // Set the slider and spin box min/max range
   //
   minimumSlider->setMinimum(sf->getMinimumSection(column));
   minimumSlider->setMaximum(sf->getMaximumSection(column));
   maximumSlider->setMinimum(sf->getMinimumSection(column));
   maximumSlider->setMaximum(sf->getMaximumSection(column));
   minimumSpinBox->setRange(sf->getMinimumSection(column), sf->getMaximumSection(column));
   maximumSpinBox->setRange(sf->getMinimumSection(column), sf->getMaximumSection(column));

   //
   // Set the slider and spin box current value
   //
   minimumSlider->setValue(dss->getMinimumSelectedSection());
   maximumSlider->setValue(dss->getMaximumSelectedSection());
   minimumSpinBox->setValue(dss->getMinimumSelectedSection());
   maximumSpinBox->setValue(dss->getMaximumSelectedSection());
}

/**
 * Called when one of the selection type radio buttons is pressed.
 */
void
GuiSectionControlDialog::sectionTypeSlot(int item)
{
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() == 0) {
      return;
   }
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   const int column = dss->getSelectedDisplayColumn(-1, -1);
 
   DisplaySettingsSection::SELECTION_TYPE selType = static_cast<DisplaySettingsSection::SELECTION_TYPE>(item);
   dss->setSelectionType(selType);
   if (selType == DisplaySettingsSection::SELECTION_TYPE_ALL) {
      dss->setMinimumSelectedSection(sf->getMinimumSection(column));
      dss->setMaximumSelectedSection(sf->getMaximumSection(column));
   }
   else if (selType == DisplaySettingsSection::SELECTION_TYPE_SINGLE) {
      dss->setMinimumSelectedSection(minimumSlider->value());
      dss->setMaximumSelectedSection(minimumSlider->value());
   }
   updateDialog();
   //GuiFilesModified fm;
   //fm.setSectionModified();
   //theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Called when apply button is pressed.
 */
void
GuiSectionControlDialog::sliderReleasedSlot()
{
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() == 0) {
      return;
   }
   
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   //const int column = dss->getSelectedColumn();

/*   
   if (singleSectionRadioButton->isChecked()) {
      sf->setSelectionType(column, SectionFile::SELECTION_TYPE_SINGLE);
   }
   else if (multipleSectionRadioButton->isChecked()) {
      sf->setSelectionType(column, SectionFile::SELECTION_TYPE_MULTIPLE);
   }
   else if (allSectionRadioButton->isChecked()) {
      sf->setSelectionType(column, SectionFile::SELECTION_TYPE_ALL);
   }
*/
   
   dss->setMinimumSelectedSection(minimumSlider->value());
   dss->setMaximumSelectedSection(maximumSlider->value());
   
   //GuiFilesModified fm;
   //fm.setSectionModified();
   //theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Called when minimum spin box value is changed.
 */
void
GuiSectionControlDialog::slotMinimumSpinBox(int value)
{
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() == 0) {
      return;
   }
   
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   //const int column = dss->getSelectedColumn();
   dss->setMinimumSelectedSection(value);
   
   if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_SINGLE) {
      dss->setMaximumSelectedSection(value);
   }
   else if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_MULTIPLE) {
      if (dss->getMinimumSelectedSection() > dss->getMaximumSelectedSection()) {
         dss->setMaximumSelectedSection(value);
      }
   }
   
   updateDialog();
   //GuiFilesModified fm;
   //fm.setSectionModified();
   //theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Called when maximum spin box value is changed.
 */
void
GuiSectionControlDialog::slotMaximumSpinBox(int value)
{
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   if (sf->getNumberOfColumns() == 0) {
      return;
   }
   
   //const int column = dss->getSelectedColumn();
   dss->setMaximumSelectedSection(value);
   
   if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_SINGLE) {
      dss->setMinimumSelectedSection(value);
   }
   else if (dss->getSelectionType() == DisplaySettingsSection::SELECTION_TYPE_MULTIPLE) {
      if (dss->getMaximumSelectedSection() < dss->getMinimumSelectedSection()) {
         dss->setMinimumSelectedSection(value);
      }
   }
   
   updateDialog();
   //GuiFilesModified fm;
   //fm.setSectionModified();
   //theMainWindow->fileModificationUpdate(fm);
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

