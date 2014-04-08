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

#include <set>

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>

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
#include "StudyMetaDataFile.h"
#include "QtTableDialog.h"
#include "QtUtilities.h"
#include "WuQDataEntryDialog.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCellAndFociReportDialog::GuiCellAndFociReportDialog(QWidget* parent, 
                                                       const bool fociFlagIn)
   : WuQDialog(parent)
{
   setModal(true);
   resultsTableDialog = NULL;
   
   fociFlag = fociFlagIn;
   typeString = "Cell";
   if (fociFlag) {
      typeString = "Foci";
   }

   setWindowTitle(typeString + " Report");
   
   //
   // Create surface section
   //
   QWidget* surfaceWidget = createSurfaceSection();
   
   //
   // Selection section
   //
   QWidget* selectionSection = createCellSelectionSection();
   
   //
   // Create cell/foci section
   //
   QWidget* attributeWidget = createCellFociSection();
   
   //
   // Create the paint section
   //
   QWidget* paintWidget = createPaintSection();
   
   //
   // Create the sums section
   //
   QWidget* sumsWidget = createSumsSection();
   
   //
   // Layout for dialog
   //
   QWidget* widget = new QWidget;
   QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
   widgetLayout->setSpacing(5);
   widgetLayout->setMargin(5);
   widgetLayout->addWidget(selectionSection);
   widgetLayout->addWidget(surfaceWidget);
   widgetLayout->addWidget(attributeWidget);
   if (paintWidget != NULL) {
      widgetLayout->addWidget(paintWidget);
   }
   if (sumsWidget != NULL) {
      widgetLayout->addWidget(sumsWidget);
   }
   
   //
   // Scroll Area for Dialog
   //
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidgetResizable(true);
   scrollArea->setWidget(widget);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
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

   //
   // Dialog Layout
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(scrollArea);
   dialogLayout->addLayout(buttonsLayout);
}
                           
/**
 * destructor.
 */
GuiCellAndFociReportDialog::~GuiCellAndFociReportDialog()
{
}

/**
 * creat cell selection section.
 */
QWidget* 
GuiCellAndFociReportDialog::createCellSelectionSection()
{
   //
   // Limit report to just displayed cells/foci
   //
   includeDisplayedCellsOnlyCheckBox = new QCheckBox("Show Only Displayed "
                                                     + typeString);
   
   //
   // GroupBox and Layout for section
   //
   QGroupBox* selectionGroupBox = new QGroupBox(typeString + " Selection");
   QVBoxLayout* selectionLayout = new QVBoxLayout(selectionGroupBox);
   selectionLayout->addWidget(includeDisplayedCellsOnlyCheckBox);
   
   return selectionGroupBox;
}
      
/**
 * create the surface section.
 */
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
                                                                 
   //
   // right hem selection combo box
   //
   QLabel* cerebellumLabel = new QLabel("Cerebellum");
   cerebellumSelectionComboBox = new GuiBrainModelSelectionComboBox(false,
                                                                 true,
                                                                 false,
                                                                 "",
                                                                 0,
                                                                 "cerebellum",
                                                                 false,
                                                                 true,
                                                                 false);
                                                                 
   BrainModelSurface* leftBMS  = NULL;
   BrainModelSurface* rightBMS = NULL;
   BrainModelSurface* cerebellumBMS = NULL;
   const int numModels = theMainWindow->getBrainSet()->getNumberOfBrainModels();
   for (int i = 0; i < numModels; i++) {
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
            if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
               if (leftBMS == NULL) {
                  leftBMS = bms;
               }
            }
            if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
               if (rightBMS == NULL) {
                  rightBMS = bms;
               }
            }
            if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CEREBELLUM) {
               if (cerebellumBMS == NULL) {
                  cerebellumBMS = bms;
               }
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
   if (cerebellumBMS != NULL) {
      cerebellumSelectionComboBox->setSelectedBrainModel(cerebellumBMS);
   }

   QGroupBox* gb = new QGroupBox("Surface Selection");
   QGridLayout* gridLayout = new QGridLayout(gb);
   gridLayout->addWidget(leftLabel, 0, 0);
   gridLayout->addWidget(leftHemSelectionComboBox, 0, 1);
   gridLayout->addWidget(rightLabel, 1, 0);
   gridLayout->addWidget(rightHemSelectionComboBox, 1, 1);
   gridLayout->addWidget(cerebellumLabel, 2, 0);
   gridLayout->addWidget(cerebellumSelectionComboBox, 2, 1);

   return gb;
}

/**
 * create the cell/foci section.
 */
QWidget* 
GuiCellAndFociReportDialog::createCellFociSection()
{
   //
   // All On button
   //
   allAttributesOnToolButton = new QToolButton;
   allAttributesOnToolButton->setText("All On");
   QObject::connect(allAttributesOnToolButton, SIGNAL(clicked()),
                    this, SLOT(slotAttributesAllOnToolButton()));
   
   //
   // All Off button
   //
   allAttributesOffToolButton = new QToolButton;
   allAttributesOffToolButton->setText("All Off");
   QObject::connect(allAttributesOffToolButton, SIGNAL(clicked()),
                    this, SLOT(slotAttributesAllOffToolButton()));
   
   //
   // Core On button
   //
   coreAttributesOnToolButton = new QToolButton;
   coreAttributesOnToolButton->setText("Core On");
   QObject::connect(coreAttributesOnToolButton, SIGNAL(clicked()),
                    this, SLOT(slotAttributesCoreOnToolButton()));
   
   //
   // Layout for tool buttons
   //
   QHBoxLayout* toolButtonLayout = new QHBoxLayout;
   toolButtonLayout->addWidget(allAttributesOnToolButton);
   toolButtonLayout->addWidget(allAttributesOffToolButton);
   toolButtonLayout->addWidget(coreAttributesOnToolButton);
   toolButtonLayout->addStretch();
   
   //
   // check box
   //
   numberCheckBox = new QCheckBox("Number");
   numberCheckBox->setChecked(true);
   
   //
   // name check box
   //
   nameCheckBox = new QCheckBox("Name");
   nameCheckBox->setChecked(true);
   
   //
   // position check box
   //
   positionCheckBox = new QCheckBox("Position");
   positionCheckBox->setChecked(true);
   
   //
   // class check box
   //
   classCheckBox = new QCheckBox("Class");
   classCheckBox->setChecked(true);
   
   //
   // area check box
   //
   areaCheckBox = new QCheckBox("Area");
   areaCheckBox->setChecked(true);
   
   //
   // geography check box
   //
   geographyCheckBox = new QCheckBox("Geography");
   geographyCheckBox->setChecked(true);
   
   //
   // region of interest check box
   //
   regionOfInterestCheckBox = new QCheckBox("Region Of Interest");
   regionOfInterestCheckBox->setChecked(true);
   
   //
   // size check box
   //
   sizeCheckBox = new QCheckBox("Size");
   sizeCheckBox->setChecked(true);
   
   //
   // statistic check box
   //
   statisticCheckBox = new QCheckBox("Statistic");
   statisticCheckBox->setChecked(true);
   
   //
   // hemisphere check box
   //
   structureCheckBox = new QCheckBox("Structure");
   structureCheckBox->setChecked(true);
   
   //
   // study name check box
   //
   studyNameCheckBox = new QCheckBox("Study Title");
   studyNameCheckBox->setChecked(true);
   
   //
   // study name check box
   //
   studyPMIDCheckBox = new QCheckBox("Study PMID");
   studyPMIDCheckBox->setChecked(true);
   
   //
   // study data format check box
   //
   studyDataFormatCheckBox = new QCheckBox("Study Data Format");
   studyDataFormatCheckBox->setChecked(true);
   
   //
   // study data type check box
   //
   studyDataTypeCheckBox = new QCheckBox("Study Data Type");
   studyDataTypeCheckBox->setChecked(true);
   
   //
   // study stereotaxic space check box
   //
   studyStereotaxicSpaceCheckBox = new QCheckBox("Study Stereotaxic Space");
   studyStereotaxicSpaceCheckBox->setChecked(true);
   
   //
   // study table number check box
   //
   studyTableNumberCheckBox = new QCheckBox("Study Table Number");
   studyTableNumberCheckBox->setChecked(true);
   
   //
   // study table subheader check box
   //
   studyTableSubHeaderCheckBox = new QCheckBox("Study Table Subheader");
   studyTableSubHeaderCheckBox->setChecked(true);
   
   //
   // study study figure number check box
   //
   studyFigureNumberCheckBox = new QCheckBox("Study Figure Number");
   studyFigureNumberCheckBox->setChecked(true);
   
   //
   // study study figure panel check box
   //
   studyFigurePanelCheckBox = new QCheckBox("Study Figure Panel");
   studyFigurePanelCheckBox->setChecked(true);
   
   //
   // study page reference number check box
   //
   studyPageReferenceNumberCheckBox = new QCheckBox("Study Page Number");
   studyPageReferenceNumberCheckBox->setChecked(true);
       
   //
   // study page reference subheader check box
   //
   studyPageReferenceSubHeaderCheckBox = new QCheckBox("Study Page Subheader");
   studyPageReferenceSubHeaderCheckBox->setChecked(true);
       
   //
   // study study page number check box
   //
   //studyPageNumberCheckBox = new QCheckBox("Study Page Number");
   //studyPageNumberCheckBox->setChecked(true);
   
   //
   // comment check box
   //
   commentCheckBox = new QCheckBox("Comment");
   commentCheckBox->setChecked(true);
   
   
   //
   // Group box for cell/foci attributes
   //
   QGroupBox* attrGroupBox = new QGroupBox(typeString + " Attributes");
   QVBoxLayout* attrGroupLayout = new QVBoxLayout(attrGroupBox);
   attrGroupLayout->addLayout(toolButtonLayout);
   attrGroupLayout->addWidget(numberCheckBox);
   attrGroupLayout->addWidget(nameCheckBox);
   attrGroupLayout->addWidget(positionCheckBox);
   attrGroupLayout->addWidget(classCheckBox);
   attrGroupLayout->addWidget(areaCheckBox);
   attrGroupLayout->addWidget(geographyCheckBox);
   attrGroupLayout->addWidget(regionOfInterestCheckBox);
   attrGroupLayout->addWidget(sizeCheckBox);
   attrGroupLayout->addWidget(statisticCheckBox);
   attrGroupLayout->addWidget(structureCheckBox);
   attrGroupLayout->addWidget(studyNameCheckBox);
   attrGroupLayout->addWidget(studyPMIDCheckBox);
   attrGroupLayout->addWidget(studyDataFormatCheckBox);
   attrGroupLayout->addWidget(studyDataTypeCheckBox);
   attrGroupLayout->addWidget(studyStereotaxicSpaceCheckBox);
   attrGroupLayout->addWidget(studyTableNumberCheckBox);
   attrGroupLayout->addWidget(studyTableSubHeaderCheckBox);
   attrGroupLayout->addWidget(studyFigureNumberCheckBox);
   attrGroupLayout->addWidget(studyFigurePanelCheckBox);
   //attrGroupLayout->addWidget(studyPageNumberCheckBox);
   attrGroupLayout->addWidget(studyPageReferenceNumberCheckBox);
   attrGroupLayout->addWidget(studyPageReferenceSubHeaderCheckBox);
   attrGroupLayout->addWidget(commentCheckBox);
   
   //
   // Widget group for all attributes
   // 
   allAttributesWidgetGroup = new WuQWidgetGroup(this);
   allAttributesWidgetGroup->addWidget(numberCheckBox);
   allAttributesWidgetGroup->addWidget(nameCheckBox);
   allAttributesWidgetGroup->addWidget(positionCheckBox);
   allAttributesWidgetGroup->addWidget(classCheckBox);
   allAttributesWidgetGroup->addWidget(areaCheckBox);
   allAttributesWidgetGroup->addWidget(geographyCheckBox);
   allAttributesWidgetGroup->addWidget(regionOfInterestCheckBox);
   allAttributesWidgetGroup->addWidget(sizeCheckBox);
   allAttributesWidgetGroup->addWidget(statisticCheckBox);
   allAttributesWidgetGroup->addWidget(structureCheckBox);
   allAttributesWidgetGroup->addWidget(studyNameCheckBox);
   allAttributesWidgetGroup->addWidget(studyPMIDCheckBox);
   allAttributesWidgetGroup->addWidget(studyDataFormatCheckBox);
   allAttributesWidgetGroup->addWidget(studyDataTypeCheckBox);
   allAttributesWidgetGroup->addWidget(studyStereotaxicSpaceCheckBox);
   allAttributesWidgetGroup->addWidget(studyTableNumberCheckBox);
   allAttributesWidgetGroup->addWidget(studyTableSubHeaderCheckBox);
   allAttributesWidgetGroup->addWidget(studyFigureNumberCheckBox);
   allAttributesWidgetGroup->addWidget(studyFigurePanelCheckBox);
   //allAttributesWidgetGroup->addWidget(studyPageNumberCheckBox);
   allAttributesWidgetGroup->addWidget(studyPageReferenceNumberCheckBox);
   allAttributesWidgetGroup->addWidget(studyPageReferenceSubHeaderCheckBox);
   allAttributesWidgetGroup->addWidget(commentCheckBox);
   
   //
   // Widget group for core items
   //
   coreAttributesWidgetGroup = new WuQWidgetGroup(this);
   coreAttributesWidgetGroup->addWidget(numberCheckBox);
   coreAttributesWidgetGroup->addWidget(nameCheckBox);
   coreAttributesWidgetGroup->addWidget(positionCheckBox);
   coreAttributesWidgetGroup->addWidget(classCheckBox);
   coreAttributesWidgetGroup->addWidget(areaCheckBox);
   coreAttributesWidgetGroup->addWidget(geographyCheckBox);
   coreAttributesWidgetGroup->addWidget(regionOfInterestCheckBox);
   coreAttributesWidgetGroup->addWidget(structureCheckBox);
   coreAttributesWidgetGroup->addWidget(studyPMIDCheckBox);
   
   //
   // If doing a cell report, hide items specific to foci
   //
   if (fociFlag == false) {
      regionOfInterestCheckBox->hide();
      geographyCheckBox->hide();
      areaCheckBox->hide();
      sizeCheckBox->hide();
      statisticCheckBox->hide();
      commentCheckBox->hide();
      studyNameCheckBox->hide();
      studyPMIDCheckBox->hide();
      studyDataFormatCheckBox->hide();
      studyDataTypeCheckBox->hide();
      studyStereotaxicSpaceCheckBox->hide();
      studyTableNumberCheckBox->hide();
      studyTableSubHeaderCheckBox->hide();
      studyFigureNumberCheckBox->hide();
      studyFigurePanelCheckBox->hide();
      //studyPageNumberCheckBox->hide();
      studyPageReferenceNumberCheckBox->hide();
      studyPageReferenceSubHeaderCheckBox->hide();
   }
   
   return attrGroupBox;
}
                           
/**
 * called when attributes all on button clicked.
 */\
void
GuiCellAndFociReportDialog::slotAttributesAllOnToolButton()
{
   allAttributesWidgetGroup->setAllCheckBoxesChecked(true);
}

/**
 * called when attributes all off button clicked.
 */
void
GuiCellAndFociReportDialog::slotAttributesAllOffToolButton()
{
   allAttributesWidgetGroup->setAllCheckBoxesChecked(false);
}

/**
 * called when attributes core on button clicked.
 */
void
GuiCellAndFociReportDialog::slotAttributesCoreOnToolButton()
{
   allAttributesWidgetGroup->setAllCheckBoxesChecked(false);
   coreAttributesWidgetGroup->setAllCheckBoxesChecked(true);
}

/**
 * create the sums section.
 */
QWidget* 
GuiCellAndFociReportDialog::createSumsSection()
{
   //
   // All On button
   //
   QToolButton* allSumsAttributesOnToolButton = new QToolButton;
   allSumsAttributesOnToolButton->setText("All On");
   QObject::connect(allSumsAttributesOnToolButton, SIGNAL(clicked()),
                    this, SLOT(slotSumsAttributesAllOnToolButton()));
   
   //
   // All Off button
   //
   QToolButton* allSumsAttributesOffToolButton = new QToolButton;
   allSumsAttributesOffToolButton->setText("All Off");
   QObject::connect(allSumsAttributesOffToolButton, SIGNAL(clicked()),
                    this, SLOT(slotSumsAttributesAllOffToolButton()));
   
   //
   // Layout for tool buttons
   //
   QHBoxLayout* toolButtonLayout = new QHBoxLayout;
   toolButtonLayout->addWidget(allSumsAttributesOnToolButton);
   toolButtonLayout->addWidget(allSumsAttributesOffToolButton);
   toolButtonLayout->addStretch();

   //
   // Group box for sums attributes
   //
   QGroupBox* sumsGroupBox = new QGroupBox("Sums Attributes");
   QVBoxLayout* sumsGroupLayout = new QVBoxLayout(sumsGroupBox);
   sumsGroupLayout->addLayout(toolButtonLayout);

   sumsIDNumberCheckBox = new QCheckBox("SuMS ID Number");
   sumsGroupLayout->addWidget(sumsIDNumberCheckBox);
   sumsIDNumberCheckBox->setChecked(false);
   sumsCheckBoxes.push_back(sumsIDNumberCheckBox);

   sumsRepeatNumberCheckBox = new QCheckBox("SuMS Repeat Number");
   sumsGroupLayout->addWidget(sumsRepeatNumberCheckBox);
   sumsRepeatNumberCheckBox->setChecked(false);
   sumsCheckBoxes.push_back(sumsRepeatNumberCheckBox);

   sumsParentCellBaseIDCheckBox = new QCheckBox("SuMS Parent Cell Base ID");
   sumsGroupLayout->addWidget(sumsParentCellBaseIDCheckBox);
   sumsParentCellBaseIDCheckBox->setChecked(false);
   sumsCheckBoxes.push_back(sumsParentCellBaseIDCheckBox);

   sumsVersionNumberCheckBox = new QCheckBox("SuMS Version Number");
   sumsGroupLayout->addWidget(sumsVersionNumberCheckBox);
   sumsVersionNumberCheckBox->setChecked(false);
   sumsCheckBoxes.push_back(sumsVersionNumberCheckBox);

   sumsMSLIDCheckBox = new QCheckBox("SuMS MSL ID");
   sumsGroupLayout->addWidget(sumsMSLIDCheckBox);
   sumsMSLIDCheckBox->setChecked(false);
   sumsCheckBoxes.push_back(sumsMSLIDCheckBox);

   sumsAttributeIDCheckBox = new QCheckBox("Attribute ID");
   sumsGroupLayout->addWidget(sumsAttributeIDCheckBox);
   sumsAttributeIDCheckBox->setChecked(false);
   sumsCheckBoxes.push_back(sumsAttributeIDCheckBox);


   return sumsGroupBox;
}

/**
 * called when sums attributes all on button clicked.
 */
void 
GuiCellAndFociReportDialog::slotSumsAttributesAllOnToolButton()
{
   const int num = static_cast<int>(sumsCheckBoxes.size());
   for (int i = 0; i < num; i++) {
      sumsCheckBoxes[i]->setChecked(true);
   }
}

/**
 * called when sums attributes all off button clicked.
 */
void 
GuiCellAndFociReportDialog::slotSumsAttributesAllOffToolButton()
{
   const int num = static_cast<int>(sumsCheckBoxes.size());
   for (int i = 0; i < num; i++) {
      sumsCheckBoxes[i]->setChecked(false);
   }
}      

/**
 * create the paint section.
 */
QWidget* 
GuiCellAndFociReportDialog::createPaintSection()
{
   //
   // All On button
   //
   QToolButton* allPaintAttributesOnToolButton = new QToolButton;
   allPaintAttributesOnToolButton->setText("All On");
   QObject::connect(allPaintAttributesOnToolButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintAttributesAllOnToolButton()));
   
   //
   // All Off button
   //
   QToolButton* allPaintAttributesOffToolButton = new QToolButton;
   allPaintAttributesOffToolButton->setText("All Off");
   QObject::connect(allPaintAttributesOffToolButton, SIGNAL(clicked()),
                    this, SLOT(slotPaintAttributesAllOffToolButton()));
   
   //
   // Layout for tool buttons
   //
   QHBoxLayout* toolButtonLayout = new QHBoxLayout;
   toolButtonLayout->addWidget(allPaintAttributesOnToolButton);
   toolButtonLayout->addWidget(allPaintAttributesOffToolButton);
   toolButtonLayout->addStretch();

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
   paintGroupLayout->addLayout(toolButtonLayout);
   
   //
   // Add check boxes for paint columns
   //
   for (int i = 0; i < numCols; i++) {
      QCheckBox* pcb = new QCheckBox(pf->getColumnName(i));
      paintGroupLayout->addWidget(pcb);
      pcb->setChecked(false);
      paintNameCheckBoxes.push_back(pcb);
   }
   
   return paintGroupBox;
}      

/**
 * called when paint attributes all on button clicked.
 */
void 
GuiCellAndFociReportDialog::slotPaintAttributesAllOnToolButton()
{
   const int num = static_cast<int>(paintNameCheckBoxes.size());
   for (int i = 0; i < num; i++) {
      paintNameCheckBoxes[i]->setChecked(true);
   }
}

/**
 * called when paint attributes all off button clicked.
 */
void 
GuiCellAndFociReportDialog::slotPaintAttributesAllOffToolButton()
{
   const int num = static_cast<int>(paintNameCheckBoxes.size());
   for (int i = 0; i < num; i++) {
      paintNameCheckBoxes[i]->setChecked(false);
   }
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
      const BrainModelSurface* cerebellumBMS = cerebellumSelectionComboBox->getSelectedBrainModelSurface();

      StudyMetaDataFile* studyMetaDataFile = 
         theMainWindow->getBrainSet()->getStudyMetaDataFile();
      
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
            if (checked(structureCheckBox)) {
               columnTitle.push_back("Structure");
               hemisphereColumn = numTableColumns++;
            }
            
            int regionOfInterestColumn = -1;
            if (checked(regionOfInterestCheckBox)) {
               columnTitle.push_back("ROI");
               regionOfInterestColumn = numTableColumns++;
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
            
            int commentColumn = -1;
            if (checked(commentCheckBox)) {
               columnTitle.push_back("Comment");
               commentColumn = numTableColumns++;
            }
            
            int studyNameColumn = -1;
            if (checked(studyNameCheckBox)) {
               columnTitle.push_back("Study Title");
               studyNameColumn = numTableColumns++;
            }
            
            int studyPMIDColumn = -1;
            if (checked(studyPMIDCheckBox)) {
               columnTitle.push_back("Study PMID");
               studyPMIDColumn = numTableColumns++;
            }
            
            int studyDataFormatColumn = -1;
            if (checked(studyDataFormatCheckBox)) {
               columnTitle.push_back("Study Data Format");
               studyDataFormatColumn = numTableColumns++;
            }
            
            int studyDataTypeColumn = -1;
            if (checked(studyDataTypeCheckBox)) {
               columnTitle.push_back("Study Data Type");
               studyDataTypeColumn = numTableColumns++;
            }
            
            int stereotaxicSpaceColumn = -1;
            if (checked(studyStereotaxicSpaceCheckBox)) {
               columnTitle.push_back("Study Stereotaxic Space");
               stereotaxicSpaceColumn = numTableColumns++;
            }
            
            int studyTableNumberColumn = -1;
            if (checked(studyTableNumberCheckBox)) {
               columnTitle.push_back("Study Table Number");
               studyTableNumberColumn = numTableColumns++;
            }
            
            int studyTableSubHeaderColumn = -1;
            if (checked(studyTableSubHeaderCheckBox)) {
               columnTitle.push_back("Study Table Subheader");
               studyTableSubHeaderColumn = numTableColumns++;
            }
            
            int studyFigureNumberColumn = -1;
            if (checked(studyFigureNumberCheckBox)) {
               columnTitle.push_back("Study Figure Number");
               studyFigureNumberColumn = numTableColumns++;
            }
            
            int studyFigurePanelColumn = -1;
            if (checked(studyFigurePanelCheckBox)) {
               columnTitle.push_back("Study Figure Panel");
               studyFigurePanelColumn = numTableColumns++;
            }
            
            //int studyPageNumberColumn = -1;
            //if (checked(studyPageNumberCheckBox)) {
            //   columnTitle.push_back("Study Page Number");
            //   studyPageNumberColumn = numTableColumns++;
            //}
            
            int studyPageReferenceNumberColumn = -1;
            if (checked(studyPageReferenceNumberCheckBox)) {
               columnTitle.push_back("Study Page Number");
               studyPageReferenceNumberColumn = numTableColumns++;
            }
            
            int studyPageReferenceSubheaderColumn = -1;
            if (checked(studyPageReferenceSubHeaderCheckBox)) {
               columnTitle.push_back("Study Page Subheader");
               studyPageReferenceSubheaderColumn = numTableColumns++;
            }
            
            int sumsIDNumberColumn = -1;
            if (checked(sumsIDNumberCheckBox)) {
               columnTitle.push_back("SuMS ID Number");
               sumsIDNumberColumn = numTableColumns++;
            }
            
            int sumsRepeatNumberColumn = -1;
            if (checked(sumsRepeatNumberCheckBox)) {
               columnTitle.push_back("SuMS Repeat Number");
               sumsRepeatNumberColumn = numTableColumns++;
            }
            
            int sumsParentCellBaseIDColumn = -1;
            if (checked(sumsParentCellBaseIDCheckBox)) {
               columnTitle.push_back("Sums Parent Cell Base ID");
               sumsParentCellBaseIDColumn = numTableColumns++;
            }
            
            int sumsVersionNumberColumn = -1;
            if (checked(sumsVersionNumberCheckBox)) {
               columnTitle.push_back("SuMS Version Number");
               sumsVersionNumberColumn = numTableColumns++;
            }
            
            int sumsMSLIDColumn = -1;
            if (checked(sumsMSLIDCheckBox)) {
               columnTitle.push_back("SuMS MSL ID");
               sumsMSLIDColumn = numTableColumns++;
            }
            
            int sumsAttributeIDColumn = -1;
            if (checked(sumsAttributeIDCheckBox)) {
               columnTitle.push_back("Attribute ID");
               sumsAttributeIDColumn = numTableColumns++;
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
            std::vector<int> cellsNearestCerebellumNode(numCells, -1);
            if (havePaints) {
               BrainModelSurfacePointLocator leftPointLocator(leftBMS,
                                                               true);
               BrainModelSurfacePointLocator rightPointLocator(rightBMS,
                                                               true);
               BrainModelSurfacePointLocator* cerebellumPointLocator = NULL;
               if (cerebellumBMS != NULL) {
                  cerebellumPointLocator = 
                     new BrainModelSurfacePointLocator(cerebellumBMS, true);
               }
               
               for (int i = 0; i < numCells; i++) {
                  const CellProjection* cp = cf->getCellProjection(i);
                  float xyz[3];
                  cp->getXYZ(xyz);
                  
                  bool leftFlag = false;
                  bool rightFlag = false;
                  bool cerebellumFlag = false;
                  switch (cp->getCellStructure()) {
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
                        leftFlag = true;
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
                        rightFlag = true;
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_BOTH:         
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM:
                        cerebellumFlag = true;
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
                        cerebellumFlag = true;
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
                        cerebellumFlag = true;
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
                        leftFlag = true;
                        break;
                     case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
                        rightFlag = true;
                        break;
                     case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
                     case Structure::STRUCTURE_TYPE_SUBCORTICAL:
                     case Structure::STRUCTURE_TYPE_ALL:
                     case Structure::STRUCTURE_TYPE_INVALID:
                        if (xyz[0] >= 0) {
                           rightFlag = true;
                        }
                        else {
                           leftFlag = true;
                        }
                        break;
                  }
                  
                  if (rightFlag) {
                     if (cp->getProjectedPosition(rightBMS->getCoordinateFile(),
                                                  rightBMS->getTopologyFile(),
                                                  rightBMS->getIsFiducialSurface(),
                                                  rightBMS->getIsFlatSurface(),
                                                  false,
                                                  xyz)) {
                        cellsNearestRightNode[i] = rightPointLocator.getNearestPoint(xyz);
                     }
                  }
                  else if (rightFlag) {
                     if (cp->getProjectedPosition(leftBMS->getCoordinateFile(),
                                                  leftBMS->getTopologyFile(),
                                                  leftBMS->getIsFiducialSurface(),
                                                  leftBMS->getIsFlatSurface(),
                                                  false,
                                                  xyz)) {
                        cellsNearestLeftNode[i] = leftPointLocator.getNearestPoint(xyz);
                     }
                  }
                  else if (cerebellumFlag) {
                     if (cerebellumBMS != NULL) {
                        if (cp->getProjectedPosition(cerebellumBMS->getCoordinateFile(),
                                                     cerebellumBMS->getTopologyFile(),
                                                     cerebellumBMS->getIsFiducialSurface(),
                                                     cerebellumBMS->getIsFlatSurface(),
                                                     false,
                                                     xyz)) {
                           cellsNearestCerebellumNode[i] = cerebellumPointLocator->getNearestPoint(xyz);
                        }
                     }
                  }
               }  // for
               
               if (cerebellumPointLocator != NULL) {
                  delete cerebellumPointLocator;
               }
            }
            
            //
            // Count number of cells for table
            //
            std::vector<int> cellsForReport;
            for (int i = 0; i < numCells; i++) {
               const CellProjection* cd = cf->getCellProjection(i);
               bool useFocusFlag = true;
               if (includeDisplayedCellsOnlyCheckBox->isChecked()) {
                  useFocusFlag = cd->getDisplayFlag();
               }
               
               if (useFocusFlag) {
                  cellsForReport.push_back(i);
               }
            }
            
            const int numCellsForTable = static_cast<int>(cellsForReport.size());
            if (numCellsForTable > 0) {
               //
               // Create the QString table
               //
               StringTable cellTable(numCellsForTable, numTableColumns);
               
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
                  cellTable.setColumnMaxInteger(numberColumn, numCellsForTable);
               }
               
               //
               // Counts for missing items
               //
               int countFociLackMatchingStudyName = 0;
               int countFociLackMatchingPubMedID  = 0;
               std::set<QString> fociLackMatchingStudyName;
               
               //
               // Process the cells
               //
               for (int ii = 0; ii < numCellsForTable; ii++) {
                  const int tableRowNumber = ii;
                  const int cellNumber = cellsForReport[ii];
                  const CellProjection* cd = cf->getCellProjection(cellNumber);
                  //
                  // Load cell data into the table
                  //
                  if (numberColumn >= 0) {
                     cellTable.setElement(tableRowNumber, numberColumn, cellNumber);
                  }
                  
                  if (nameColumn >= 0) {
                     cellTable.setElement(tableRowNumber, nameColumn, cd->getName());
                  }
                  
                  if (positionColumn >= 0) {
                     const float* xyz = cd->getXYZ();
                     cellTable.setElement(tableRowNumber, positionColumn, xyz[0]);
                     cellTable.setElement(tableRowNumber, positionColumn + 1, xyz[1]);
                     cellTable.setElement(tableRowNumber, positionColumn + 2, xyz[2]);
                  }
                  
                  if (classColumn >= 0) {
                     cellTable.setElement(tableRowNumber, classColumn, cd->getClassName());
                  }

                  if (areaColumn >= 0) {
                     cellTable.setElement(tableRowNumber, areaColumn, cd->getArea());
                  }
                  
                  if (geographyColumn >= 0) {
                     cellTable.setElement(tableRowNumber, geographyColumn, cd->getGeography());
                  }

                  if (regionOfInterestColumn >= 0) {
                     cellTable.setElement(tableRowNumber, regionOfInterestColumn, cd->getRegionOfInterest());
                  }
                  
                  if (sizeColumn >= 0) {
                     cellTable.setElement(tableRowNumber, sizeColumn, cd->getSize());
                  }

                  if (statisticColumn >= 0) {
                     cellTable.setElement(tableRowNumber, statisticColumn, cd->getStatistic());
                  }

                  if (hemisphereColumn >= 0) {
                     cellTable.setElement(tableRowNumber, hemisphereColumn, 
                        Structure::convertTypeToString(cd->getCellStructure()));
                  }

                  QString pubMedID;
                  StudyMetaData* linkedStudyMetaData = NULL;
                  const StudyMetaDataLinkSet smdls = cd->getStudyMetaDataLinkSet();
                  StudyMetaDataLink metaDataLink;
                  if (smdls.getNumberOfStudyMetaDataLinks() > 0) {
                     metaDataLink = smdls.getStudyMetaDataLink(0);
                     pubMedID = metaDataLink.getPubMedID();
                     const int indx = studyMetaDataFile->getStudyIndexFromLink(metaDataLink);
                     if (indx >= 0) {
                        linkedStudyMetaData = studyMetaDataFile->getStudyMetaData(indx);
                     }
                  }
                  
                  if (studyNameColumn >= 0) {
                     QString studyTitle;
                     if (linkedStudyMetaData != NULL) {
                        studyTitle = linkedStudyMetaData->getTitle();
                     }
                     cellTable.setElement(tableRowNumber, studyNameColumn, studyTitle);
                  }
                  
                  if (studyPMIDColumn >= 0) {
                     //QString studyPMID;
                     //if (linkedStudyMetaData != NULL) {
                     //   studyPMID = linkedStudyMetaData->getPubMedID();
                     //}
                     //cellTable.setElement(tableRowNumber, studyPMIDColumn, studyPMID);
                     cellTable.setElement(tableRowNumber, studyPMIDColumn, pubMedID);
                  }

                  if (cd->getName().isEmpty() == false) {
                     int nameIndx = studyMetaDataFile->getStudyIndexFromName(cd->getName());
                     if (nameIndx >= 0) {
                        int pmidIndex = -1;
                        if (pubMedID.isEmpty() == false) {
                           pmidIndex = studyMetaDataFile->getStudyIndexFromPubMedID(pubMedID);
                        }
                        if (nameIndx != pmidIndex) {
                           countFociLackMatchingPubMedID++;
                           fociLackMatchingStudyName.insert("Focus "
                                                            + QString::number(cellNumber)
                                                            + " "
                                                            + cd->getName()
                                                            + " Study Index="
                                                            + QString::number(pmidIndex+1)
                                                            + ", Study With Same Name Uses Study Index=" 
                                                            + QString::number(nameIndx+1));
                        }
                     }
                     else {
                        countFociLackMatchingStudyName++;
                        fociLackMatchingStudyName.insert("No Study Name Matches " 
                                                         + QString::number(cellNumber)
                                                         + " "
                                                         + cd->getName());
                     }
                  }
                  
                  if (studyDataFormatColumn >= 0) {
                     QString dataFormat;
                     if (linkedStudyMetaData != NULL) {
                        dataFormat = linkedStudyMetaData->getStudyDataFormat();
                     }
                     cellTable.setElement(tableRowNumber, studyDataFormatColumn, dataFormat);
                  }
                  
                  if (studyDataTypeColumn >= 0) {
                     QString dataType;
                     if (linkedStudyMetaData != NULL) {
                        dataType = linkedStudyMetaData->getStudyDataType();
                     }
                     cellTable.setElement(tableRowNumber, studyDataTypeColumn, dataType);
                  }
               
                  if (stereotaxicSpaceColumn >= 0) {
                     QString spaceName;
                     if (linkedStudyMetaData != NULL) {
                        spaceName = linkedStudyMetaData->getStereotaxicSpace();
                     }
                     cellTable.setElement(tableRowNumber, stereotaxicSpaceColumn, spaceName);
                  }
                  
                  if (studyTableNumberColumn >= 0) {
                     QString txt = metaDataLink.getTableNumber();
                     cellTable.setElement(tableRowNumber, studyTableNumberColumn, txt);
                  }
                  
                  if (studyTableSubHeaderColumn >= 0) {
                     QString txt = metaDataLink.getTableSubHeaderNumber();
                     cellTable.setElement(tableRowNumber, studyTableSubHeaderColumn, txt);
                  }
                  
                  if (studyFigureNumberColumn >= 0) {
                     QString txt = metaDataLink.getFigureNumber();
                     cellTable.setElement(tableRowNumber, studyFigureNumberColumn, txt);
                  }
                  
                  if (studyFigurePanelColumn >= 0) {
                     QString txt = metaDataLink.getFigurePanelNumberOrLetter();
                     cellTable.setElement(tableRowNumber, studyFigurePanelColumn, txt);
                  }
                  
                  //if (studyPageNumberColumn >= 0) {
                  //   QString txt = metaDataLink.getPageNumber();
                  //   cellTable.setElement(tableRowNumber, studyPageNumberColumn, txt);
                  //}
                  
                  if (studyPageReferenceNumberColumn >= 0) {
                     QString txt = metaDataLink.getPageReferencePageNumber();
                     cellTable.setElement(tableRowNumber, studyPageReferenceNumberColumn, txt);
                  }
                  
                  if (studyPageReferenceSubheaderColumn >= 0) {
                     QString txt = metaDataLink.getPageReferenceSubHeaderNumber();
                     cellTable.setElement(tableRowNumber, studyPageReferenceSubheaderColumn, txt);
                  }
                  
                  if (commentColumn >= 0) {
                     cellTable.setElement(tableRowNumber, commentColumn, cd->getComment());
                  }

                  if (sumsIDNumberColumn >= 0) {
                     cellTable.setElement(tableRowNumber, sumsIDNumberColumn, cd->getSumsIDNumber());
                  }
                  
                  if (sumsRepeatNumberColumn >= 0) {
                     cellTable.setElement(tableRowNumber, sumsRepeatNumberColumn, cd->getSumsRepeatNumber());
                  }
                  
                  if (sumsParentCellBaseIDColumn >= 0) {
                     cellTable.setElement(tableRowNumber, sumsParentCellBaseIDColumn, cd->getSumsParentCellBaseID());
                  }
                  
                  if (sumsVersionNumberColumn >= 0) {
                     cellTable.setElement(tableRowNumber, sumsVersionNumberColumn, cd->getSumsVersionNumber());
                  }
                  
                  if (sumsMSLIDColumn >= 0) {
                     cellTable.setElement(tableRowNumber, sumsMSLIDColumn, cd->getSumsMSLID());
                  }
                  
                  if (sumsAttributeIDColumn >= 0) {
                     cellTable.setElement(tableRowNumber, sumsAttributeIDColumn, cd->getAttributeID());
                  }
                  
                  //
                  // Load paint into the table
                  //
                  if (havePaints) {
                     int node = -1;
                     if (cellsNearestLeftNode[cellNumber] >= 0) {
                        node = cellsNearestLeftNode[cellNumber];
                     }
                     else if (cellsNearestRightNode[cellNumber] >= 0) {
                        node = cellsNearestRightNode[cellNumber];
                     }
                     else if (cellsNearestCerebellumNode[cellNumber] >= 0) {
                        node = cellsNearestCerebellumNode[cellNumber];
                     }
                     if (node >= 0) {
                        for (int j = 0; j < numPaintCols; j++) {
                           if (paintNameCheckBoxes[j]->isChecked()) {
                              const int paintIndex = pf->getPaint(node, j);
                              const QString paintName = pf->getPaintNameFromIndex(paintIndex);
                              cellTable.setElement(tableRowNumber, paintTableColumn[j], paintName);
                           }
                        }
                     }
                  }
                  
               } // for (ii = 0; ii < numCells...
            
               //
               // Create and display the table dialog
               //
               const QString titleString(typeString
                                         + " Report ");
               resultsTableDialog = new QtTableDialog(theMainWindow,
                                                      titleString,
                                                      cellTable,
                                                      true);
               resultsTableDialog->show();
               resultsTableDialog->activateWindow();
               QApplication::processEvents();
                                                      
               //
               // Missing name/pub med ID alerts
               //
               QString msg;
               if (countFociLackMatchingStudyName > 0) {
                  if (msg.isEmpty() == false) {
                     msg += "\n\n";
                  }
                  msg += (QString::number(countFociLackMatchingStudyName)
                          + " foci lack a matching study name.");
               }
               if (countFociLackMatchingPubMedID > 0) {
                  if (msg.isEmpty() == false) {
                     msg += "\n\n";
                  }
                  msg += (QString::number(countFociLackMatchingPubMedID)
                          + " foci have mismatched PMID in foci file and study file.");
               }
               if (msg.isEmpty() == false) {
                  msg += ("\n\n"
                          "Use 'Layers->Foci->Update Focus PubMed ID if Focus Name Matches Study Name'\n"
                          "to correct mismatches.");
                  WuQDataEntryDialog ded(resultsTableDialog);
                  ded.setWindowTitle("Mismatched Foci");
                  ded.setTextAtTop(msg, true);
                  QStringList namesStringList;
                  for (std::set<QString>::iterator iter = fociLackMatchingStudyName.begin();
                       iter != fociLackMatchingStudyName.end();
                       iter++) {
                     namesStringList += *iter;
                  }
                  ded.addListWidget("", namesStringList);
                  ded.exec();
               }
               
            } // if (numCellsForTable > 0)
            else {
               QString msg("There are no ");
               msg.append(typeString);
               msg.append(" meeting display criteria.");
               QMessageBox::critical(this, "ERROR", msg);
            }
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
      
