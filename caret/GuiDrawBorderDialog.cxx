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
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>
#include <QTabWidget>
#include <QToolTip>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "ColorFile.h"
#include "DisplaySettingsBorders.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiDrawBorderDialog.h"
#include "GuiFilesModified.h"
#include "GuiNameSelectionDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiMainWindow.h"
#include "GuiVolumeSelectionControl.h"
#include "QtUtilities.h"
#include "PaintFile.h"
#include "StringUtilities.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiDrawBorderDialog::GuiDrawBorderDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Draw Borders");
   
   //
   // Vertical box layout of all items
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(2);
   rows->setSpacing(3);
   
   //
   // Create the tabbed pages
   //
   rows->addWidget(createMainPage());

   //
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);

   //
   // Apply button
   // 
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   buttonsLayout->addWidget(applyButton);
  
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseButton()));
   buttonsLayout->addWidget(closeButton);
  
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);

   updateDialog();
   slotEnableDisableItems();

   assignPaintColumnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);   
   slotAssignPaintColumnSelection(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
}

/**
 * Destructor
 */
GuiDrawBorderDialog::~GuiDrawBorderDialog()
{
}

/**
 * Called when dialog is shown
 */
void
GuiDrawBorderDialog::show()
{
   if (theMainWindow->getBrainModelSurface() != NULL) {
      assignTabWidget->setCurrentIndex(assignTabWidget->indexOf(assignNodesVBox));
   }
   else if (theMainWindow->getBrainModelVolume() != NULL) {
      assignTabWidget->setCurrentIndex(assignTabWidget->indexOf(assignVoxelsVBox));
   }
   QtDialog::show();
}

/**
 * Create the main border drawing page.
 */
QWidget*
GuiDrawBorderDialog::createMainPage()
{
   QWidget* mainPageWidget = new QWidget;
   QVBoxLayout* mainPageLayout = new QVBoxLayout(mainPageWidget);
   
   //
   // Grid Layout for Attributes
   //
   QGroupBox* attributesGroup = new QGroupBox("Attributes");
   mainPageLayout->addWidget(attributesGroup);
   QGridLayout* attributesGrid = new QGridLayout(attributesGroup);
   attributesGrid->setSpacing(3);
   
   //
   // Label and text box for border name
   //
   attributesGrid->addWidget(new QLabel("Name"), 0, 0, Qt::AlignLeft);
   nameLineEdit = new QLineEdit;
   nameLineEdit->setMinimumWidth(200);
   attributesGrid->addWidget(nameLineEdit, 0, 1, Qt::AlignLeft);
   
   //
   // Pushbutton for border name selection
   //
   QPushButton* nameButton = new QPushButton("Select...");
   nameButton->setAutoDefault(false);
   nameButton->setFixedSize(nameButton->sizeHint());
   attributesGrid->addWidget(nameButton, 0, 2, Qt::AlignLeft);
   QObject::connect(nameButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectNameButton()));
   
   //
   // Label and text box for border resampling
   //
   attributesGrid->addWidget(new QLabel("Resampling (mm)"), 1, 0, Qt::AlignLeft);
   resamplingLineEdit = new QLineEdit;
   resamplingLineEdit->setFixedWidth(100);
   resamplingLineEdit->setText("2.0");
   attributesGrid->addWidget(resamplingLineEdit, 1, 1, Qt::AlignLeft);
   
   //-------------------------------------------------------------------------------------
   //
   // Horizontal box for type and dimensions
   //
   QHBoxLayout* typeDimHBoxLayout = new QHBoxLayout;
   mainPageLayout->addLayout(typeDimHBoxLayout);
   
   //--------------------------------------------------------------------------------------
   //
   // Button Group for open and closed buttons
   //
   QGroupBox* typeGroupBox = new QGroupBox("Type");
   QVBoxLayout* typeGroupLayout = new QVBoxLayout(typeGroupBox);
   QButtonGroup* typeButtonGroup = new QButtonGroup(this);
   typeDimHBoxLayout->addWidget(typeGroupBox);
   QObject::connect(typeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisableItems()));
   
   //
   // Closed Border Radio Button
   //
   closedBorderRadioButton = new QRadioButton("Closed (Boundary)");
   typeGroupLayout->addWidget(closedBorderRadioButton);
   typeButtonGroup->addButton(closedBorderRadioButton);

   //
   // Open Border Radio Button
   //
   openBorderRadioButton = new QRadioButton("Open");
   typeGroupLayout->addWidget(openBorderRadioButton);
   typeButtonGroup->addButton(openBorderRadioButton);

   //--------------------------------------------------------------------------------------
   //
   // Button Group for 2D and 3D buttons
   //
   QGroupBox* dimensionGroupBox = new QGroupBox("Dimensions");
   QVBoxLayout* dimensionLayout = new QVBoxLayout(dimensionGroupBox);
   QButtonGroup* dimensionButtonGroup = new QButtonGroup(this);
   typeDimHBoxLayout->addWidget(dimensionGroupBox);
   QObject::connect(dimensionButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisableItems()));
   
   //
   // 2D radio button
   //
   twoDimensionalRadioButton = new QRadioButton("2D");
   dimensionLayout->addWidget(twoDimensionalRadioButton);
   dimensionButtonGroup->addButton(twoDimensionalRadioButton);
   
   //
   // 3D radio button
   //
   threeDimensionalRadioButton = new QRadioButton("3D");
   dimensionLayout->addWidget(threeDimensionalRadioButton);
   dimensionButtonGroup->addButton(threeDimensionalRadioButton);
   
   //
   // Default to 2D
   //
   twoDimensionalRadioButton->setChecked(true);
   
   //--------------------------------------------------------------------------------------
   //
   // Auto Project Yes/No radio buttons
   //
   autoProjectYesRadioButton = new QRadioButton("Yes");
   autoProjectNoRadioButton = new QRadioButton("No");
    
   //
   // Button Group for Auto Project Yes/No
   //
   QButtonGroup* autoProjectButtonGroup = new QButtonGroup(this);
   autoProjectButtonGroup->addButton(autoProjectYesRadioButton);
   autoProjectButtonGroup->addButton(autoProjectNoRadioButton);
   
   //
   // Group Box and Layout for Auto Project
   //
   QGroupBox* autoProjectGroupBox = new QGroupBox("Auto Project");
   QVBoxLayout* autoProjectLayout = new QVBoxLayout(autoProjectGroupBox);
   autoProjectLayout->addWidget(autoProjectYesRadioButton);
   autoProjectLayout->addWidget(autoProjectNoRadioButton);
   typeDimHBoxLayout->addWidget(autoProjectGroupBox);
   
   //
   // Default proj off
   //
   autoProjectNoRadioButton->setChecked(true);
   
   //
   // Add stretch on right of type/dim/auto proj
   //
   typeDimHBoxLayout->addStretch();
   
   //--------------------------------------------------------------------------------------
   //
   // Assign nodes within closed border section
   //
   assignVGroup = new QGroupBox("Closed Border Assignment");
   QVBoxLayout* assignLayout = new QVBoxLayout(assignVGroup);
   mainPageLayout->addWidget(assignVGroup);
   
   
   //
   // Assign tab widget
   //
   assignTabWidget = new QTabWidget;
   assignLayout->addWidget(assignTabWidget);
   
   //
   // add assign widgets
   //
   createAssignNodesWidget();
   createAssignVoxelsWidget();
   assignTabWidget->addTab(assignNodesVBox, "Surface Nodes");
   assignTabWidget->addTab(assignVoxelsVBox, "Voxels");
   
   assignTabWidget->setFixedSize(assignTabWidget->sizeHint());
   
   return mainPageWidget;
}

/**
 * create the assign nodes widget.
 */
void 
GuiDrawBorderDialog::createAssignNodesWidget()
{
   //QVBoxLayout* assignNodesLayout = new QVBoxLayout(assignNodesVBox);
   assignNodesCheckBox = new QCheckBox("Assign Paint Identifiers to Nodes Within Border");
   //assignNodesLayout->addWidget(assignNodesCheckBox);
   QObject::connect(assignNodesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotEnableDisableItems()));
   
   //
   // Paint column selection
   //
   //assignNodesPaintQHBox = new QWidget;
   //assignNodesLayout->addWidget(assignNodesPaintQHBox);
   assignPaintColumnLabel = new QLabel("Paint Column ");
   assignPaintColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                                                     GUI_NODE_FILE_TYPE_PAINT,
                                                                     true,
                                                                     false,
                                                                     false);
   assignPaintColumnComboBox->setMaximumWidth(250);
   QObject::connect(assignPaintColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotAssignPaintColumnSelection(int)));
   assignPaintColumnNameLineEdit = new QLineEdit;
   assignPaintColumnNameLineEdit->setMaximumWidth(250);
   
   //QHBoxLayout* assignNodesPaintLayout = new QHBoxLayout(assignNodesPaintQHBox);
   //assignNodesPaintLayout->addWidget(new QLabel("   Paint Column "));
   //assignNodesPaintLayout->addWidget(assignPaintColumnComboBox);
   //assignNodesPaintLayout->addWidget(assignPaintColumnNameLineEdit);
   
   //--------------------------------------------------------------------------------------
   //
   // Reassign nodes within closed border section
   //
   //reassignNodesQVBox = new QWidget;
   //assignNodesLayout->addWidget(reassignNodesQVBox);
   
   //reassignColumnQHBox = new QWidget;
   reassignNodesCheckBox = new QCheckBox("Reassign Nodes With ");
   QObject::connect(reassignNodesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotEnableDisableItems()));
                    
   reassignNodesPaintColumnComboBox = 
      new GuiNodeAttributeColumnSelectionComboBox(
                                                   GUI_NODE_FILE_TYPE_PAINT,
                                                   false,
                                                   false,
                                                   false);
   reassignNodesPaintColumnComboBox->setFixedWidth(250);
   
   //QHBoxLayout* reassignColumnLayout = new QHBoxLayout(reassignColumnQHBox);
   //reassignColumnLayout->addWidget(new QLabel("   "));
   //reassignColumnLayout->addWidget(reassignNodesCheckBox);
   //reassignColumnLayout->addWidget(reassignNodesPaintColumnComboBox);
   //reassignColumnLayout->setStretchFactor(reassignNodesPaintColumnComboBox, 100);
   //  //reassignColumnQHBox->setMaximumWidth(500);
   
   //reassignNameQHBox = new QWidget;
   reassignNamePushButton = new QPushButton("Named...");
   QObject::connect(reassignNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotReassignNodeNamePushButton()));
   //reassignNamePushButton->setFixedSize(reassignNamePushButton->sizeHint());
   reassignNamePushButton->setAutoDefault(false);
   reassignNameLabel = new QLabel(" ");
   //reassignNameLabel->setFixedWidth(300);
   //reassignNameQHBox->setFixedSize(reassignNameQHBox->sizeHint());
   //QHBoxLayout* reassignNameLayout = new QHBoxLayout(reassignNameQHBox);
   //reassignNameLayout->addWidget(new QLabel("   "));
   //reassignNameLayout->addWidget(reassignNamePushButton);
   //reassignNameLayout->addWidget(reassignNameLabel);
   //reassignNameLayout->setStretchFactor(reassignNameLabel, 100);
   
   //QVBoxLayout* reassignNodesLayout = new QVBoxLayout(reassignNodesQVBox);
   //reassignNodesLayout->addWidget(reassignColumnQHBox);
   //reassignNodesLayout->addWidget(reassignNameQHBox);
   
   assignNodesVBox = new QWidget;
   QVBoxLayout* assignNodesLayout = new QVBoxLayout(assignNodesVBox);
   assignNodesLayout->addWidget(assignNodesCheckBox);
   
   QGridLayout* assignGrid = new QGridLayout;
   assignNodesLayout->addLayout(assignGrid);
   assignGrid->addWidget(assignPaintColumnLabel, 0, 0);
   assignGrid->addWidget(assignPaintColumnComboBox, 0, 1);
   assignGrid->addWidget(assignPaintColumnNameLineEdit, 0, 2);
   assignGrid->addWidget(reassignNodesCheckBox, 1, 0);
   assignGrid->addWidget(reassignNodesPaintColumnComboBox, 1, 1, 1, 2);
   assignGrid->addWidget(reassignNamePushButton, 2, 0);
   assignGrid->addWidget(reassignNameLabel, 2, 1, 1, 2);
   //assignGrid->setMaximumColumnWidth(1, 200);
   //assignGrid->setMaximumColumnWidth(2, 200);
}

/**
 * create the assign voxels widget.
 */
void 
GuiDrawBorderDialog::createAssignVoxelsWidget()
{
   assignVoxelsWithinBorderCheckBox = new QCheckBox("Assign Voxels Within Closed Border");
   QObject::connect(assignVoxelsWithinBorderCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotEnableDisableItems()));
                  
   assignVoxelsVolumeSelectionControl = 
            new GuiVolumeSelectionControl(0,
                                          false,
                                          false,
                                          true,
                                          false,
                                          false,
                                          false,
                                          false,
                                          GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                          "assignVoxelsVolumeSelectionControl",
                                          false,
                                          false,
                                          false);   
   assignVoxelsVolumeSelectionControl->setMaximumWidth(450);
   
   //                                       
   // slice thickness
   //
   QWidget* thickHBox = new QWidget;
   assignVoxelsSliceThicknessComboBox = new QComboBox;
   assignVoxelsSliceThicknessComboBox->addItem("Current Slice");
   assignVoxelsSliceThicknessComboBox->addItem("+/- 1 Slice");
   assignVoxelsSliceThicknessComboBox->addItem("+/- 2 Slices");
   assignVoxelsSliceThicknessComboBox->addItem("+/- 3 Slices");
   assignVoxelsSliceThicknessComboBox->addItem("+/- 4 Slices");
   assignVoxelsSliceThicknessComboBox->addItem("+/- 5 Slices");
   QHBoxLayout* thickHBoxLayout = new QHBoxLayout(thickHBox);
   assignVoxelsThicknessLabel = new QLabel("Thickness ");
   thickHBoxLayout->addWidget(assignVoxelsThicknessLabel);
   thickHBoxLayout->addWidget(assignVoxelsSliceThicknessComboBox);
   thickHBox->setFixedSize(thickHBox->sizeHint());
   
   assignVoxelsVBox = new QWidget;
   QVBoxLayout* assignVoxelsLayout = new QVBoxLayout(assignVoxelsVBox);
   assignVoxelsLayout->addWidget(assignVoxelsWithinBorderCheckBox);
   assignVoxelsLayout->addWidget(assignVoxelsVolumeSelectionControl);
   assignVoxelsLayout->addWidget(thickHBox);
   assignVoxelsVBox->setMaximumHeight(assignVoxelsVBox->sizeHint().height());
}
      
/**
 * Called when Reassign nodes name pushbutton is pressed.
 */
void
GuiDrawBorderDialog::slotReassignNodeNamePushButton()
{
   GuiNameSelectionDialog nsd(this, GuiNameSelectionDialog::LIST_PAINT_NAMES_ALPHA);
   if (nsd.exec() == QDialog::Accepted) {
      QString name(nsd.getName());
      if (name.isEmpty() == false) {
         reassignNameLabel->setText(name);
      }
   }
}

/**
 * Enable and disable items in dialog based upon current selections
 */
void
GuiDrawBorderDialog::slotEnableDisableItems()
{
   assignVGroup->setEnabled(false);
   assignNodesCheckBox->setEnabled(false);
   assignPaintColumnLabel->setEnabled(false);
   assignPaintColumnComboBox->setEnabled(false);
   assignPaintColumnNameLineEdit->setEnabled(false);
   reassignNodesCheckBox->setEnabled(false);
   reassignNodesPaintColumnComboBox->setEnabled(false);
   reassignNamePushButton->setEnabled(false);
   reassignNameLabel->setEnabled(false);
   
   assignVoxelsVBox->setEnabled(false);
   assignVoxelsWithinBorderCheckBox->setEnabled(false);
   assignVoxelsVolumeSelectionControl->setEnabled(false);
   assignVoxelsThicknessLabel->setEnabled(false);
   assignVoxelsSliceThicknessComboBox->setEnabled(false);
   
   if (closedBorderRadioButton->isChecked()) {
      assignNodesCheckBox->setEnabled(true);
      assignVGroup->setEnabled(true);
      if (assignNodesCheckBox->isChecked()) {
         assignPaintColumnLabel->setEnabled(true);
         assignPaintColumnComboBox->setEnabled(true);
         assignPaintColumnNameLineEdit->setEnabled(true);
         
         PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
         const bool havePaints = (pf->getNumberOfColumns() > 0);
         if (havePaints) {
            reassignNodesCheckBox->setEnabled(true);
            if (reassignNodesCheckBox->isChecked()) {
               reassignNodesPaintColumnComboBox->setEnabled(true);
               reassignNamePushButton->setEnabled(true);
               reassignNameLabel->setEnabled(true);
            }
         }
      }
         
      assignVoxelsVBox->setEnabled(true);
      assignVoxelsWithinBorderCheckBox->setEnabled(true);
      if (assignVoxelsWithinBorderCheckBox->isChecked()) {
         assignVoxelsVolumeSelectionControl->setEnabled(true);
         assignVoxelsThicknessLabel->setEnabled(true);
         assignVoxelsSliceThicknessComboBox->setEnabled(true);
      }
   }
}

/**
 * Called when assignment paint column is selected.
 */
void
GuiDrawBorderDialog::slotAssignPaintColumnSelection(int col)
{
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   if (col == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
      assignPaintColumnNameLineEdit->setText("New Column Name");
   }
   else if ((col >= 0) && (col < pf->getNumberOfColumns())) {
      assignPaintColumnNameLineEdit->setText(pf->getColumnName(col));
   }
}
 
/**
 * Called when select name button is pressed.
 */
void
GuiDrawBorderDialog::slotSelectNameButton()
{
   static GuiNameSelectionDialog::LIST_ITEMS_TYPE itemForDisplay = 
                   GuiNameSelectionDialog::LIST_BORDER_COLORS_ALPHA;
           
   GuiNameSelectionDialog nsd(this, 
                              GuiNameSelectionDialog::LIST_ALL,
                              itemForDisplay);
   if (nsd.exec() == QDialog::Accepted) {
      itemForDisplay = nsd.getSelectedItemType();
      QString name(nsd.getName());
      if (name.isEmpty() == false) {
         nameLineEdit->setText(name);
         if (StringUtilities::makeUpperCase(name) == "MEDIAL.WALL") {
            closedBorderRadioButton->setChecked(true);
         }
         else {
            const int len = name.length();
            if (len > 3) {
               const QString last3(StringUtilities::makeLowerCase(name.mid(len - 3, 3)));
               if (last3 == "cut") {
                  openBorderRadioButton->setChecked(true);
               }
            }
         
            if (name.indexOf("LANDMARK") != -1) {
               openBorderRadioButton->setChecked(true);
            }
         }
      }
   }
}

/**
 * Get the resampling density.
 */
float
GuiDrawBorderDialog::getResampling() const
{
   float resamplingDensity = resamplingLineEdit->text().toFloat();
   if (resamplingDensity <= 0.0) {
      resamplingDensity = 2.0;
      resamplingLineEdit->setText(QString::number(resamplingDensity, 'f', 2));
   }
   return resamplingDensity;
}

/**
 * Get the name of the border
 */
QString
GuiDrawBorderDialog::getBorderName() const
{
   return nameLineEdit->text();
}

/**
 * Get closed border flag
 */
bool
GuiDrawBorderDialog::getClosedBorderFlag() const
{
   return closedBorderRadioButton->isChecked();
}

/**
 * get the auto project border flag.
 */
bool 
GuiDrawBorderDialog::getAutoProjectBorder() const 
{ 
   return autoProjectYesRadioButton->isChecked(); 
}
      
/**
 * Get the 3D flag
 */
bool
GuiDrawBorderDialog::getThreeDimensional() const
{
   return threeDimensionalRadioButton->isChecked();
}

/**
 * Called when apply button is pressed
 */
void
GuiDrawBorderDialog::slotApplyButton()
{
   bool surfaceFlag = false;
   bool volumeFlag  = false;
   if (theMainWindow->getBrainModelSurfaceAndVolume() != NULL) {
      return;
   }
   else if (theMainWindow->getBrainModelSurface() != NULL) {
      assignTabWidget->setCurrentIndex(assignTabWidget->indexOf(assignNodesVBox));
      surfaceFlag = true;
   }
   else if (theMainWindow->getBrainModelVolume() != NULL) {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv->getMasterVolumeFile() == NULL) {
         QMessageBox::critical(this, "ERROR",
             "At least one volume must be displayed as\n"
             "an overlay or underlay to draw a border.");
         return;
      }
      assignTabWidget->setCurrentIndex(assignTabWidget->indexOf(assignVoxelsVBox));
      volumeFlag = true;
   }

   QString errorMessage;
   bool errorFlag = false;
   bool closedBorder = false;
   if ((closedBorderRadioButton->isChecked() == false) && 
       (openBorderRadioButton->isChecked() == false)) {
      errorFlag = true;
      if (errorMessage.isEmpty() == false) errorMessage.append("\n");
      errorMessage.append("You must select closed or open border !");
   }
   else {
      closedBorder = getClosedBorderFlag();
   }
   
   const QString borderName = getBorderName();
   if (borderName.isEmpty()) {
      errorFlag = true;
      if (errorMessage.isEmpty() == false) errorMessage.append("\n");
      errorMessage.append("You must enter the border name !");
   }
   
   const float resamplingDensity = getResampling();
   if (resamplingDensity <= 0.0) {
      errorFlag = true;
      if (errorMessage.isEmpty() == false) errorMessage.append("\n");
      errorMessage.append("Border Resampling must be greater than 0.0 !");
   }
   
   if (surfaceFlag) {
      if (getClosedBorderFlag() && assignNodesCheckBox->isChecked() &&
          (reassignNodesPaintColumnComboBox->count() > 0) &&
          reassignNodesCheckBox->isChecked()) {
         QString  name(StringUtilities::trimWhitespace(reassignNameLabel->text()));
         if (name.isEmpty()) {
            errorFlag = true;
            if (errorMessage.isEmpty() == false) errorMessage.append("\n");
            errorMessage.append("Name for reassignment is missing.!");
         }
      }
   }
   if (errorFlag) {
      QMessageBox::critical(this, "Error", errorMessage);
      return;
   }
   
   //
   // Find the matching color
   //
   bool borderColorMatch = false;
   BorderColorFile* borderColorFile = theMainWindow->getBrainSet()->getBorderColorFile();
   borderColorIndex = borderColorFile->getColorIndexByName(borderName, borderColorMatch);
   
   //
   // Border color may need to be created
   //
   bool createBorderColor = false;
   if ((borderColorIndex >= 0) && (borderColorMatch == true)) {
      createBorderColor = false;
   }
   else if ((borderColorIndex >= 0) && (borderColorMatch == false)) {
      QString msg("Use border color \"");
      msg.append(borderColorFile->getColorNameByIndex(borderColorIndex));
      msg.append("\" for border ");
      msg.append(borderName);
      msg.append(" ?");
      QString noButton("No, define color ");
      noButton.append(borderName);
      /*
      if (GuiMessageBox::information(this, "Use Partially Matching Color",
                                   msg, "Yes", noButton, QString::null, 0) != 0) {
         createBorderColor = true;
      }
      */
      QMessageBox msgBox(this);
      msgBox.setWindowTitle("Use Partially Matching Color");
      msgBox.setText(msg);
      QPushButton* yesPushButton = msgBox.addButton("Yes", 
                                                    QMessageBox::ActionRole);
      QPushButton* noPushButton = msgBox.addButton(noButton, 
                                                    QMessageBox::ActionRole);
      msgBox.exec();
      if (msgBox.clickedButton() == yesPushButton) {
         createBorderColor = false;
      }
      else if (msgBox.clickedButton() == noPushButton) {
         createBorderColor = true;
      }
   }
   else {
      createBorderColor = true;
   }
   
   if (createBorderColor) {
      QString title("Create Border Color: ");
      title.append(borderName);
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this, 
                                                                 title,
                                                                 false,
                                                                 false,
                                                                 false,
                                                                 false);
      csd->exec();
      
      //
      // Add new border color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      borderColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);
      borderColorIndex = borderColorFile->getNumberOfColors() - 1;
      
      //
      // Border Color File has changed
      //
      GuiFilesModified fm;
      fm.setBorderColorModified();
      theMainWindow->fileModificationUpdate(fm);
   }
   
   //
   // Assigning nodes within a closed border
   //
   if (getClosedBorderFlag() && 
       ((assignNodesCheckBox->isChecked() && surfaceFlag) ||
        (assignVoxelsWithinBorderCheckBox->isChecked() && volumeFlag))) {
      //
      // Find the matching color
      //
      bool areaColorMatch = false;
      AreaColorFile* areaColorFile = theMainWindow->getBrainSet()->getAreaColorFile();
      const int areaColorIndex = areaColorFile->getColorIndexByName(borderName, areaColorMatch);
      
      //
      // Area color may need to be created
      //
      if (areaColorMatch == false) {
         QString msg("Matching area color \"");
         msg.append(borderName);
         msg.append("\" not found");
         QString borderButton("Use Border Color");
         QString defineButton("Define Area Color ");
         int result = -1;
         if (areaColorIndex >= 0) {
            QString partialMatchButton("Use ");
            partialMatchButton.append(areaColorFile->getColorNameByIndex(areaColorIndex));
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Set Area Color");
            msgBox.setText(msg);
            QPushButton* useColorPushButton = msgBox.addButton(borderButton, 
                                                          QMessageBox::ActionRole);
            QPushButton* defineColorPushButton = msgBox.addButton(defineButton, 
                                                          QMessageBox::ActionRole);
            QPushButton* usePartialColorPushButton = msgBox.addButton(partialMatchButton, 
                                                          QMessageBox::ActionRole);
            msgBox.exec();
            if (msgBox.clickedButton() == useColorPushButton) {
               result = 0;
            }
            else if (msgBox.clickedButton() == defineColorPushButton) {
               result = 1;
            }
            else if (msgBox.clickedButton() == usePartialColorPushButton) {
               result = 2;
            }
            /*
            result = QMessageBox::information(this, "Set Area Color",
                                    msg, 
                                    borderButton,
                                    defineButton,
                                    partialMatchButton);
            */
         }
         else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Set Area Color");
            msgBox.setText(msg);
            QPushButton* useColorPushButton = msgBox.addButton(borderButton, 
                                                          QMessageBox::ActionRole);
            QPushButton* defineColorPushButton = msgBox.addButton(defineButton, 
                                                          QMessageBox::ActionRole);
            msgBox.exec();
            if (msgBox.clickedButton() == useColorPushButton) {
               result = 0;
            }
            else if (msgBox.clickedButton() == defineColorPushButton) {
               result = 1;
            }
            /*
            result = GuiMessageBox::information(this, "Set Area Color",
                                    msg, 
                                    borderButton,
                                    defineButton);
            */
         }
         
         if (result == 0) {
            //
            // Copy border color to area color
            //
            unsigned char r, g, b, a;
            borderColorFile->getColorByIndex(borderColorIndex, r, g, b, a);
            float pointSize, lineSize;
            borderColorFile->getPointLineSizeByIndex(borderColorIndex, pointSize, lineSize);
            areaColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize);
                        
            //
            // Area Color File has changed
            //
            GuiFilesModified fm;
            fm.setAreaColorModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         else if (result == 1) {
            //
            // define the area color
            //
            QString title("Create Area Color: ");
            title.append(borderName);
            GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this,
                                                                       title,
                                                                       false,
                                                                       false, 
                                                                       false,
                                                                       false);
            csd->exec();
            
            //
            // Add new area color
            //
            float pointSize = 2.0, lineSize = 1.0;
            unsigned char r, g, b, a;
            ColorFile::ColorStorage::SYMBOL symbol;
            csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
            areaColorFile->addColor(borderName, r, g, b, a, pointSize, lineSize, symbol);            
            //
            // Area Color File has changed
            //
            GuiFilesModified fm;
            fm.setAreaColorModified();
            theMainWindow->fileModificationUpdate(fm);
         }
         else if (result == 2) {
            //
            // do nothing so that partially matching color is used
            //
         }
      }
   }

   if (surfaceFlag) {
      //
      // Make sure a flat or compressed medial wall surface is not rotated
      //
      if (twoDimensionalRadioButton->isChecked()) {
         BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
         if (bms != NULL) {
            bool haveFlatSurface = false;
            switch (bms->getSurfaceType()) {
               case BrainModelSurface::SURFACE_TYPE_RAW:
               case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
               case BrainModelSurface::SURFACE_TYPE_INFLATED:
               case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
               case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
               case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
                  break;
               case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
               case BrainModelSurface::SURFACE_TYPE_FLAT:
               case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
                  haveFlatSurface = true;
                  break;
               case BrainModelSurface::SURFACE_TYPE_HULL:
               case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
               case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
                  break;
            }
         
            if (haveFlatSurface) {
               float matrix[16];
               bms->getRotationMatrix(0, matrix);
               
               if ((matrix[0] != 1.0) || (matrix[5] != 1.0) || (matrix[10] != 1.0)) {
                  if (QMessageBox::warning(this, "WARNING",
                          "The flat surface appears to be rotated which will\n"
                          "prevent the border from being drawn correctly.\n"
                          "Would you like to remove the rotation?",
                          (QMessageBox::Yes | QMessageBox::No),
                          QMessageBox::Yes)
                             == QMessageBox::Yes) {
                     //
                     // Setting the view to dorsal resets the rotation matrix without
                     // affecting the translate and scaling.
                     //
                     bms->setToStandardView(0, BrainModelSurface::VIEW_DORSAL);
                     GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
                  }
               }
            }
         }
      }
   }
   
   theMainWindow->getBrainModelOpenGL()->resetLinearObjectBeingDrawn();
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DRAW);
}

/**
 * Create a new border for the brain model.
 */
void
GuiDrawBorderDialog::createNewBorder(BrainModel* bm, Border& border)
{
   QString borderName = getBorderName();
   if (borderName.isEmpty()) {
      borderName = "No-Name";
   }
   border.setName(borderName);
   border.setBorderColorIndex(borderColorIndex);
   
   if (getClosedBorderFlag()) {
      border.addBorderLink(border.getLinkXYZ(0));
   }
   int dummy = 0;
   border.resampleBorderToDensity(getResampling(), 2, dummy);
   
   BrainModelVolume* bmv = dynamic_cast<BrainModelVolume*>(bm);
   BrainModelSurface* bms = dynamic_cast<BrainModelSurface*>(bm);
   
   if (bmv != NULL) {
      BorderFile* bf = theMainWindow->getBrainSet()->getVolumeBorderFile();
      bf->addBorder(border);
      
      //
      // Assigning voxels within closed border
      //
      if (getClosedBorderFlag() && assignVoxelsWithinBorderCheckBox->isChecked()) {
         //
         // Get the selected volume
         //
         VolumeFile* vf = assignVoxelsVolumeSelectionControl->getSelectedVolumeFile();
         if (vf != NULL) {
            vf->assignVoxelsWithinBorder(bmv->getSelectedAxis(0),
                                         borderName,
                                         &border,
                                         assignVoxelsSliceThicknessComboBox->currentIndex());
                                         
         }
      }
   }
   else if (bms != NULL) {
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      BrainModelBorder* b = new BrainModelBorder(theMainWindow->getBrainSet(), &border, bms->getSurfaceType());
      bmbs->addBorder(b);
      
      int newPaintColumnCreated = -1;
   
      //
      // drawing a closed border and assigning nodes within the border ?
      //
      if (getClosedBorderFlag() && assignNodesCheckBox->isChecked()) {
         PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
         int column = assignPaintColumnComboBox->currentIndex();
         const int paintIndex = pf->addPaintName(borderName);
         
         //
         // Does a new column need to be created ?
         //
         if (column < 0) {
            column = pf->getNumberOfColumns();
            if (pf->getNumberOfColumns() == 0) {
               pf->setNumberOfNodesAndColumns(theMainWindow->getBrainSet()->getNumberOfNodes(), 1);
            }
            else {
               pf->addColumns(1);
            }
            newPaintColumnCreated = pf->getNumberOfColumns() - 1;
         }
         
         //
         // Set the paint column name
         //
         pf->setColumnName(column, assignPaintColumnNameLineEdit->text());
         
         //
         // Assign the nodes within the closed border.
         //
         GuiBrainModelOpenGL* mainOpenGL = theMainWindow->getBrainModelOpenGL();      
         const BrainModelSurface* bms = mainOpenGL->getDisplayedBrainModelSurface();
         if (bms != NULL) {
            const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
            std::vector<bool> insideFlags(numNodes, false);
            const CoordinateFile* cf = bms->getCoordinateFile();
            const float* coords = cf->getCoordinate(0);

            //
            // Drawing a 3D border ?
            //
            if (getThreeDimensional()) {
               GuiBrainModelOpenGL::updateAllGL();
               BrainModelOpenGL* openGL = GuiBrainModelOpenGL::getOpenGLDrawing();
               border.pointsInsideBorder3D(openGL->getSelectionModelviewMatrix(0),
                                           openGL->getSelectionProjectionMatrix(0),
                                           openGL->getSelectionViewport(0),
                                           coords,
                                           numNodes,
                                           insideFlags);
            }
            else {
               border.pointsInsideBorder2D(coords, numNodes, insideFlags);
            }
            
            //
            // Are nodes being reassigned ?
            //
            if (reassignNodesCheckBox->isChecked()) {
               const int reassignColumn = reassignNodesPaintColumnComboBox->currentIndex();
               if ((reassignColumn >= 0) && (reassignColumn < pf->getNumberOfColumns())) {
                  const int reassignPaintIndex = pf->getPaintIndexFromName(  
                                                          reassignNameLabel->text());
                  for (int j = 0; j < numNodes; j++) {
                     if (insideFlags[j]) {
                        if (pf->getPaint(j, reassignColumn) == reassignPaintIndex) {
                           pf->setPaint(j, column, paintIndex);
                        }
                     }
                  }
               }
            }
            else {
               for (int j = 0; j < numNodes; j++) {
                  if (insideFlags[j]) {
                     pf->setPaint(j, column, paintIndex);
                  }
               }
            }
         }
      }
      if (newPaintColumnCreated >= 0) {
         //
         // Make paint column selection the new column
         //
         assignPaintColumnComboBox->updateComboBox(theMainWindow->getBrainSet()->getPaintFile());
         assignPaintColumnComboBox->setCurrentIndex(newPaintColumnCreated);   
         slotAssignPaintColumnSelection(newPaintColumnCreated);      
      }
      
      if (getAutoProjectBorder()) {
         const int borderNumber = bmbs->getNumberOfBorders() - 1;
         if (borderNumber >= 0) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            bmbs->projectBorders(bms,
                                 true,
                                 borderNumber,
                                 borderNumber);
            QApplication::restoreOverrideCursor();
         }
      }
   }
   
   //
   // Border File has changed
   //
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   dsb->setDisplayBorders(true);
   GuiFilesModified fm;
   fm.setBorderModified();
   fm.setPaintModified();
   theMainWindow->fileModificationUpdate(fm);   
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Called when close button is pressed
 */
void
GuiDrawBorderDialog::slotCloseButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   reject();
}

/**
 * Update the dialog.
 */
void
GuiDrawBorderDialog::updateDialog()
{
   assignPaintColumnComboBox->updateComboBox(theMainWindow->getBrainSet()->getPaintFile());
   reassignNodesPaintColumnComboBox->updateComboBox(theMainWindow->getBrainSet()->getPaintFile());
   slotAssignPaintColumnSelection(assignPaintColumnComboBox->currentIndex());
   assignVoxelsVolumeSelectionControl->updateControl();
   slotEnableDisableItems();
}
