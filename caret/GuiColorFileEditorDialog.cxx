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


#include <algorithm>

#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QSlider>
#include <QSpinBox>
#include <QToolTip>

#include "BrainSet.h"
#include "ColorFile.h"
#include <QDoubleSpinBox>

#include "GuiBrainModelOpenGL.h"
#include "GuiHtmlColorChooserDialog.h"
#include "GuiMainWindow.h"

#define __COLOR_FILE_EDITOR_MAIN__
#include "GuiColorFileEditorDialog.h"
#undef __COLOR_FILE_EDITOR_MAIN__

#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 * For non modal dialog, leave "modalFlag" and "f" at their defaults.  Call "show()" to display.
 * For modal dialog, set "modalFlag" to true and "f" to zero.  Call "exec()" to display.
 */
GuiColorFileEditorDialog::GuiColorFileEditorDialog(QWidget* parent, 
                                                   ColorFile* cf,
                                                   const bool showAlpha,
                                                   const bool showPointSize,
                                                   const bool showLineSize,
                                                   const bool showSymbol,
                                                   const bool modalFlag,
                                                   const bool deleteWhenClosed) :
   QtDialog(parent, modalFlag)

{
   if (deleteWhenClosed) {
      setAttribute(Qt::WA_DeleteOnClose);
   }
   
   //
   // color file being edited
   //
   colorFile = cf;
   
   currentColorFileIndex = -1;
   setWindowTitle("Color File Editor");
   
   //
   // layout for dialog
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(5);
   
   //
   // Group box for color naming
   //
   QGroupBox* colorNameGroupBox = new QGroupBox("Color Name");
   rows->addWidget(colorNameGroupBox);
   QVBoxLayout* colorNameGroupBoxLayout = new QVBoxLayout(colorNameGroupBox);
   
   //
   // Name selection list box
   //
   nameSelectionListBox = new QListWidget;
   nameSelectionListBox->setSelectionMode(QListWidget::SingleSelection);
   colorNameGroupBoxLayout->addWidget(nameSelectionListBox);
   QObject::connect(nameSelectionListBox, SIGNAL(currentRowChanged(int)),
                    this, SLOT(nameListSelectionSlot(int)));
   //QObject::connect(nameSelectionListBox, SIGNAL(selected(int)),
   //                 this, SLOT(nameListSelectionSlot(int)));
   
   //
   // Horizontal box for new color and delete buttons
   //
   QHBoxLayout* newDeleteButtonBoxLayout = new QHBoxLayout;
   colorNameGroupBoxLayout->addLayout(newDeleteButtonBoxLayout);
   
   //
   // Create new color button
   //
   QPushButton* newColorButton = new QPushButton("New Color...");
   newDeleteButtonBoxLayout->addWidget(newColorButton);
   newColorButton->setAutoDefault(false);
   QObject::connect(newColorButton, SIGNAL(clicked()),
                    this, SLOT(newColorButtonSlot()));
   
   //
   // Delete color button
   //
   QPushButton* deleteColorButton = new QPushButton("Delete Selected Color");
   newDeleteButtonBoxLayout->addWidget(deleteColorButton);
   deleteColorButton->setAutoDefault(false);
   QObject::connect(deleteColorButton, SIGNAL(clicked()),
                    this, SLOT(deleteColorButtonSlot()));
   
   //
   // Group box for color parameters
   //
   QGroupBox* colorParamGroupBox = new QGroupBox("Color Parameters");
   rows->addWidget(colorParamGroupBox);
   QVBoxLayout* colorParamGroupBoxLayout = new QVBoxLayout(colorParamGroupBox);
   
   //
   // "Color swatch" widget
   //
   colorSwatch = new QWidget;
   colorParamGroupBoxLayout->addWidget(colorSwatch);
   colorSwatch->setMinimumSize(QSize(50, 50));
   colorSwatch->setMaximumHeight(50);
   //colorSwatch->setFixedSize(QSize(200, 50));
   
   //
   // Grid Layout for Colors Sliders
   //
   QGridLayout* sliderGridLayout = new QGridLayout;
   colorParamGroupBoxLayout->addLayout(sliderGridLayout);
   sliderGridLayout->setMargin(5);
   sliderGridLayout->setSpacing(5);
   
   //
   // Red control
   //
   sliderGridLayout->addWidget(new QLabel("Red"), 0, 0, Qt::AlignLeft);
   redValueSpinBox = new QSpinBox;
   redValueSpinBox->setMinimum(0);
   redValueSpinBox->setMaximum(255);
   redValueSpinBox->setSingleStep(1);
   sliderGridLayout->addWidget(redValueSpinBox, 0, 1, Qt::AlignRight);
   redSlider = new QSlider(Qt::Horizontal);
   redSlider->setMinimum(0);
   redSlider->setMaximum(255);
   redSlider->setPageStep(5);
   redSlider->setValue(0);
   redSlider->setMinimumWidth(300);
   QObject::connect(redSlider, SIGNAL(valueChanged(int)),
                    redValueSpinBox, SLOT(setValue(int)));
   QObject::connect(redSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(redSliderMovedSlot(int)));
   QObject::connect(redValueSpinBox, SIGNAL(valueChanged(int)),
                    redSlider, SLOT(setValue(int)));
   QObject::connect(redValueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(redSliderMovedSlot(int)));
   sliderGridLayout->addWidget(redSlider, 0, 2, Qt::AlignLeft);

   //
   // Green control
   //
   sliderGridLayout->addWidget(new QLabel("Green"), 1, 0, Qt::AlignLeft);
   greenValueSpinBox = new QSpinBox;
   greenValueSpinBox->setMinimum(0);
   greenValueSpinBox->setMaximum(255);
   greenValueSpinBox->setSingleStep(1);
   sliderGridLayout->addWidget(greenValueSpinBox, 1, 1, Qt::AlignRight);
   greenSlider = new QSlider(Qt::Horizontal);
   greenSlider->setMinimum(0);
   greenSlider->setMaximum(255);
   greenSlider->setPageStep(5);
   greenSlider->setValue(0);
   greenSlider->setMinimumWidth(300);
   QObject::connect(greenSlider, SIGNAL(valueChanged(int)),
                    greenValueSpinBox, SLOT(setValue(int)));
   QObject::connect(greenSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(greenSliderMovedSlot(int)));
   QObject::connect(greenValueSpinBox, SIGNAL(valueChanged(int)),
                    greenSlider, SLOT(setValue(int)));
   QObject::connect(greenValueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(greenSliderMovedSlot(int)));
   sliderGridLayout->addWidget(greenSlider, 1, 2, Qt::AlignLeft);

   //
   // Blue control
   //
   sliderGridLayout->addWidget(new QLabel("Blue"), 2, 0, Qt::AlignLeft);
   blueValueSpinBox = new QSpinBox;
   blueValueSpinBox->setMinimum(0);
   blueValueSpinBox->setMaximum(255);
   blueValueSpinBox->setSingleStep(1);
   sliderGridLayout->addWidget(blueValueSpinBox, 2, 1, Qt::AlignRight);
   blueSlider = new QSlider(Qt::Horizontal);
   blueSlider->setMinimum(0);
   blueSlider->setMaximum(255);
   blueSlider->setPageStep(5);
   blueSlider->setValue(0);
   blueSlider->setMinimumWidth(300);
   QObject::connect(blueSlider, SIGNAL(valueChanged(int)),
                    blueValueSpinBox, SLOT(setValue(int)));
   QObject::connect(blueSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(blueSliderMovedSlot(int)));
   QObject::connect(blueValueSpinBox, SIGNAL(valueChanged(int)),
                    blueSlider, SLOT(setValue(int)));
   QObject::connect(blueValueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(blueSliderMovedSlot(int)));
   sliderGridLayout->addWidget(blueSlider, 2, 2, Qt::AlignLeft);

   //
   // Alpha control
   //
   QLabel* alphaLabel = new QLabel("Alpha");
   sliderGridLayout->addWidget(alphaLabel, 3, 0, Qt::AlignLeft);
   alphaValueSpinBox = new QSpinBox;
   alphaValueSpinBox->setMinimum(0);
   alphaValueSpinBox->setMaximum(255);
   alphaValueSpinBox->setSingleStep(1);
   sliderGridLayout->addWidget(alphaValueSpinBox, 3, 1, Qt::AlignRight);
   alphaSlider = new QSlider(Qt::Horizontal);
   alphaSlider->setMinimumWidth(300);
   alphaSlider->setMinimum(0);
   alphaSlider->setMaximum(255);
   alphaSlider->setPageStep(5);
   alphaSlider->setValue(0);
   QObject::connect(alphaSlider, SIGNAL(valueChanged(int)),
                    alphaValueSpinBox, SLOT(setValue(int)));
   QObject::connect(alphaSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(alphaSliderMovedSlot(int)));
   QObject::connect(alphaValueSpinBox, SIGNAL(valueChanged(int)),
                    alphaSlider, SLOT(setValue(int)));
   QObject::connect(alphaValueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(alphaSliderMovedSlot(int)));
   sliderGridLayout->addWidget(alphaSlider, 3, 2, Qt::AlignLeft);

   if (showAlpha == false) {
      alphaLabel->hide();
      alphaValueSpinBox->hide();
      alphaSlider->hide();
   }
   
   //
   // Pushbutton for color chooser
   //
   QPushButton* colorChooserButton = new QPushButton("Advanced Color Selection...");
   colorChooserButton->setAutoDefault(false);
   colorChooserButton->setToolTip(
                 "Press this button to pop up a dialog\n"
                 "with more powerful color selection.");
   colorChooserButton->setFixedSize(colorChooserButton->sizeHint());
   QObject::connect(colorChooserButton, SIGNAL(clicked()),
                    this, SLOT(slotColorChooserButton()));
   
   //
   // Pushbutton for web (html) colors
   //
   QPushButton* webHtmlColorPushButton = new QPushButton("List of Common Colors...");
   webHtmlColorPushButton->setAutoDefault(false);
   webHtmlColorPushButton->setToolTip(
                "Press this button to select colors\n"
                "defined by the World Wide Web Consortium");
   webHtmlColorPushButton->setFixedSize(webHtmlColorPushButton->sizeHint());
   QObject::connect(webHtmlColorPushButton, SIGNAL(clicked()),
                    this, SLOT(slotWebHtmlColorNamesPushButton()));
   
   //
   // Layout for color buttons
   //
   QHBoxLayout* colorPushButtonsLayout = new QHBoxLayout;
   colorPushButtonsLayout->addWidget(colorChooserButton);
   colorPushButtonsLayout->addWidget(webHtmlColorPushButton);
   colorPushButtonsLayout->addStretch();
   colorParamGroupBoxLayout->addLayout(colorPushButtonsLayout);
   
   //
   // QGrid for Line and Point Size and symbol
   //
   QGridLayout* attsGridLayout = new QGridLayout;
   colorParamGroupBoxLayout->addLayout(attsGridLayout);
   
   float minLineSize, maxLineSize;
   GuiBrainModelOpenGL::getLineWidthRange(minLineSize, maxLineSize);
   QLabel* lineSizeLabel = new QLabel("Line Size");
   attsGridLayout->addWidget(lineSizeLabel, 0, 0);
   lineSizeSpinBox = new QDoubleSpinBox;
   lineSizeSpinBox->setMinimum(minLineSize);
   lineSizeSpinBox->setMaximum(maxLineSize);
   lineSizeSpinBox->setSingleStep(1.0);
   lineSizeSpinBox->setDecimals(1);
   attsGridLayout->addWidget(lineSizeSpinBox, 0, 1);
   lineSizeSpinBox->setValue(1.0);
   QObject::connect(lineSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(lineSizeChangedSlot(double)));
   
   if (showLineSize == false) {
      lineSizeLabel->hide();
      lineSizeSpinBox->hide();
   }
   
   float minPointSize, maxPointSize;
   GuiBrainModelOpenGL::getPointSizeRange(minPointSize, maxPointSize);
   maxPointSize = 100000.0;
   QLabel* pointSizeLabel = new QLabel("Point Size");
   attsGridLayout->addWidget(pointSizeLabel, 1, 0);
   pointSizeSpinBox = new QDoubleSpinBox;
   pointSizeSpinBox->setMinimum(minPointSize);
   pointSizeSpinBox->setMaximum(maxPointSize);
   pointSizeSpinBox->setSingleStep(1.0);
   pointSizeSpinBox->setDecimals(1);
   attsGridLayout->addWidget(pointSizeSpinBox, 1, 1);
   pointSizeSpinBox->setValue(2.0);
   QObject::connect(pointSizeSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(pointSizeChangedSlot(double)));

   if (showPointSize == false) {
      pointSizeLabel->hide();
      pointSizeSpinBox->hide();
   }

   //
   // Symbol
   //
   QLabel* symbolLabel = new QLabel("Symbol");
   attsGridLayout->addWidget(symbolLabel, 2, 0);
   symbolComboBox = new QComboBox;
   attsGridLayout->addWidget(symbolComboBox, 2, 1);
   std::vector<QString> symbolLabels;
   ColorFile::ColorStorage::getAllSymbolTypesAsStrings(symbolLabels);
   for (unsigned int i = 0; i < symbolLabels.size(); i++) {
      symbolComboBox->addItem(symbolLabels[i]);
   }
   QObject::connect(symbolComboBox, SIGNAL(activated(int)),
                    this, SLOT(symbolComboBoxSlot(int)));
   if (showSymbol == false) {
      symbolLabel->hide();
      symbolComboBox->hide();
   }
   
   //
   // Squeeze Grid
   //
   //attsGridLayout->setFixedSize(attsGridLayout->sizeHint());
   
   //
   // Load and initialize the color list box
   //
   loadColorsIntoListBox();
   nameListSelectionSlot(0);
   
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = NULL;
   if (isModal() == false) {
      applyButton = new QPushButton("Apply");
      applyButton->setAutoDefault(false);
      QObject::connect(applyButton, SIGNAL(clicked()),
                       this, SLOT(applySlot()));
      QObject::connect(applyButton, SIGNAL(clicked()),
                       this, SIGNAL(redrawRequested()));
      buttonsLayout->addWidget(applyButton);
   }
   
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   if (isModal()) {
      closeButton->setText("OK");
   }
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SIGNAL(redrawRequested()));
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   buttonsLayout->addWidget(closeButton);
   
   //
   // Make buttons the same size
   //
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
}

/**
 * Destructor
 */
GuiColorFileEditorDialog::~GuiColorFileEditorDialog()
{
}

/**
 * called when color chooser button is pressed.
 */
void
GuiColorFileEditorDialog::slotColorChooserButton()
{
   //
   // Popup QT's color chooser dialog
   //
   QColor inputColor(redValueSpinBox->value(), greenValueSpinBox->value(), blueValueSpinBox->value());
   QColor outputColor = QColorDialog::getColor(inputColor, this);
   if (outputColor.isValid()) {
      if ((currentColorFileIndex >= 0) && 
          (currentColorFileIndex < static_cast<int>(colors.size()))) {
         colorFile->setColorByIndex(currentColorFileIndex, 
                                    outputColor.red(),
                                    outputColor.green(),
                                    outputColor.blue());
         redValueSpinBox->setValue(outputColor.red());
         greenValueSpinBox->setValue(outputColor.green());
         blueValueSpinBox->setValue(outputColor.blue());
         updateColorSwatch();
      }
   }
}

/**
 * Called when new color button is pressed
 */
void
GuiColorFileEditorDialog::newColorButtonSlot()
{
   //
   // Initialize color to foreground color
   //
   unsigned char red, green, blue;
   unsigned char alpha = 255;
   PreferencesFile* pref = theMainWindow->getBrainSet()->getPreferencesFile();
   pref->getSurfaceForegroundColor(red, green, blue);
   float pointSize = 2.0, lineSize = 1.0;
      
   //
   // Default to currently selected color
   //
   QString defaultName = "";
   if (currentColorFileIndex >= 0) {
      colorFile->getColorByIndex(currentColorFileIndex, red, green, blue, alpha);
      colorFile->getPointLineSizeByIndex(currentColorFileIndex, pointSize, lineSize);
      defaultName = colorFile->getColorNameByIndex(currentColorFileIndex);
   }
   
   //
   //  Get the new color name using a qt input dialog for text
   //
   bool ok = false;
   const QString nameInput = QInputDialog::getText(this, "New Color Name", "Color Name",
                                                   QLineEdit::Normal, defaultName,
                                                   &ok);
   //
   // Name entered and ok button was pressed
   //
   if (ok && (nameInput.isEmpty() == false)) {
      const QString name(nameInput);
      
      //
      // See if color with name exists, if so switch to it and popup message
      //
      bool match = false;
      int colorIndex = colorFile->getColorIndexByName(name, match);
      if ((colorIndex >= 0) && match) {
         for (int i = 0; i < static_cast<int>(colors.size()); i++) {
            if (colors[i].colorFileIndex == colorIndex) {
               nameSelectionListBox->setCurrentRow(i);
               nameListSelectionSlot(i);
               break;
            }
         }
         QString msg("Color ");
         msg.append(name);
         msg.append(" already exists.");
         QMessageBox::warning(this, "Color Exists", msg);
         return;
      }
      
      //
      // Create a new color
      //
      colorFile->addColor(name, red, green, blue, alpha, pointSize, lineSize);
      loadColorsIntoListBox();
      
      //
      // Select the new color
      //
      nameSelectionListBox->setCurrentRow(0);
      nameListSelectionSlot(0);
      colorIndex = colorFile->getColorIndexByName(name, match);
      if ((colorIndex >= 0) && match) {
         for (int i = 0; i < static_cast<int>(colors.size()); i++) {
            if (colors[i].colorFileIndex == colorIndex) {
               nameSelectionListBox->setCurrentRow(i);
               nameListSelectionSlot(i);
               break;
            }
         }
      }
   }
   
   //
   // Redraw since color file has changed
   //
   emit redrawRequested();
}

/**
 * Called when delete color button is pressed
 */
void
GuiColorFileEditorDialog::deleteColorButtonSlot()
{
   int listBoxIndex = nameSelectionListBox->currentRow();
   const int colorIndex = colors[listBoxIndex].colorFileIndex;
   
   //
   //  Remove the color
   //
   colorFile->removeColorByIndex(colorIndex);
   loadColorsIntoListBox();
   
   //
   // Reset items in list box
   //
   if (listBoxIndex >= static_cast<int>(colors.size())) {
      listBoxIndex = colors.size() - 1;
   }
   if (listBoxIndex >= 0) {
      nameSelectionListBox->setCurrentRow(listBoxIndex);
      nameListSelectionSlot(listBoxIndex);
   }
   
   //
   // Redraw since color file has changed
   //
   emit redrawRequested();
}

/**
 * Called when apply button is pressed.
 */
void 
GuiColorFileEditorDialog::applySlot()
{
}

/**
 * called when OK/Canel pressed if dialog is modal.
 */
void 
GuiColorFileEditorDialog::done(int r)
{
   applySlot();
   QDialog::done(r);
}
      
/**
 * Initialize the controls 
 */
void
GuiColorFileEditorDialog::loadColorsIntoListBox()
{
   //
   // Load and Sort the colors
   //
   colors.clear();
   int num = colorFile->getNumberOfColors();
   for (int i = 0; i < num; i++) {
      colors.push_back(ColorSorter(colorFile->getColorNameByIndex(i), i));
   }
   std::sort(colors.begin(), colors.end());
   
   //
   // Load the color names into the list selection box
   //
   nameSelectionListBox->clear();
   num = colors.size();
   for (int i = 0; i < num; i++) {
      nameSelectionListBox->addItem(colors[i].colorName);
   }
   if (num > 0) {
      currentColorFileIndex = colors[0].colorFileIndex;
      nameSelectionListBox->setCurrentRow(0);
   }
   else {
      currentColorFileIndex = -1;
   }   
}
 
/**
 * Update the color swatch
 */
void 
GuiColorFileEditorDialog::updateColorSwatch()
{
   QColor color;
   
   if (currentColorFileIndex >= 0) {
      unsigned char r, g, b;
      colorFile->getColorByIndex(currentColorFileIndex, r, g, b);
      color.setRgb(r, g, b);
   }
   
   QPalette pal;
   pal.setColor(QPalette::Window, color);
   colorSwatch->setAutoFillBackground(true);
   colorSwatch->setBackgroundRole(QPalette::Window);
   colorSwatch->setPalette(pal);
   //colorSwatch->setPaletteBackgroundColor(color);
   //colorSwatch->setPaletteForegroundColor(color);
}

/**
 * Slot called when a name is selected in the name list box.
 */
void
GuiColorFileEditorDialog::nameListSelectionSlot(int indx)
{
   if ((indx >= 0) && (indx < static_cast<int>(colors.size()))) {
      currentColorFileIndex = colors[indx].colorFileIndex;
   }
   else {
      currentColorFileIndex = -1;
   }
   
   if (currentColorFileIndex >= 0) {
      unsigned char r, g, b, a;
      colorFile->getColorByIndex(currentColorFileIndex, r, g, b, a);
      redSlider->setValue(r);
      redValueSpinBox->setValue(r);
      greenSlider->setValue(g);
      greenValueSpinBox->setValue(g);
      blueSlider->setValue(b);
      blueValueSpinBox->setValue(b);
      alphaSlider->setValue(a);
      alphaValueSpinBox->setValue(a);
      
      float pointSize, lineSize;
      colorFile->getPointLineSizeByIndex(currentColorFileIndex, pointSize, lineSize);
      if (lineSizeSpinBox != NULL) {
         lineSizeSpinBox->setValue(lineSize);
      }
      if (pointSizeSpinBox != NULL) {
         pointSizeSpinBox->setValue(pointSize);
      }
      
      symbolComboBox->setCurrentIndex(colorFile->getSymbolByIndex(currentColorFileIndex));
   }
   updateColorSwatch();
}

/**
 * called when symbol combo box is selected.
 */
void 
GuiColorFileEditorDialog::symbolComboBoxSlot(int value)
{
   if (currentColorFileIndex >= 0) {
      ColorFile::ColorStorage::SYMBOL s = static_cast<ColorFile::ColorStorage::SYMBOL>(value);
      if (s != colorFile->getSymbolByIndex(currentColorFileIndex)) {
         colorFile->setSymbolByIndex(currentColorFileIndex, s);
      }
   }
}
      
/**
 * Slot called when the red slider is moved.
 */
void
GuiColorFileEditorDialog::redSliderMovedSlot(int value)
{
   if (currentColorFileIndex >= 0) {
      unsigned char r, g, b, a;
      colorFile->getColorByIndex(currentColorFileIndex, r, g, b, a);
      const unsigned char red = static_cast<unsigned char>(value);
      if (red != r) {
         colorFile->setColorByIndex(currentColorFileIndex, red, g, b, a);
      }
   }
   updateColorSwatch();
}

/**
 * Slot called when the green slider is moved.
 */
void
GuiColorFileEditorDialog::greenSliderMovedSlot(int value)
{
   if (currentColorFileIndex >= 0) {
      unsigned char r, g, b, a;
      colorFile->getColorByIndex(currentColorFileIndex, r, g, b, a);
      const unsigned  char green = static_cast<unsigned char>(value);
      if (green != g) {
         colorFile->setColorByIndex(currentColorFileIndex, r, green, b, a);
      }
   }
   updateColorSwatch();
}

/**
 * Slot called when the blue slider is moved.
 */
void
GuiColorFileEditorDialog::blueSliderMovedSlot(int value)
{
   if (currentColorFileIndex >= 0) {
      unsigned char r, g, b, a;
      colorFile->getColorByIndex(currentColorFileIndex, r, g, b, a);
      const unsigned char blue = static_cast<unsigned char>(value);
      if (blue != b) {
         colorFile->setColorByIndex(currentColorFileIndex, r, g, blue, a);
      }
   }
   updateColorSwatch();
}

/**
 * Slot called when the alpha slider is moved.
 */
void
GuiColorFileEditorDialog::alphaSliderMovedSlot(int value)
{
   if (currentColorFileIndex >= 0) {
      unsigned char r, g, b, a;
      colorFile->getColorByIndex(currentColorFileIndex, r, g, b, a);
      const unsigned char alpha = static_cast<unsigned char>(value);
      if (alpha != a) {
         colorFile->setColorByIndex(currentColorFileIndex, r, g, b, alpha);
      }
   }
   updateColorSwatch();
}

/**
 * Called when line size is changed.
 */
void
GuiColorFileEditorDialog::lineSizeChangedSlot(double value)
{
   if (currentColorFileIndex >= 0) {
       float pointSize, lineSize;
       colorFile->getPointLineSizeByIndex(currentColorFileIndex, pointSize, lineSize);
       if (value != lineSize) {
          colorFile->setPointLineSizeByIndex(currentColorFileIndex, pointSize, value);
       }
   }
}

/**
 * Called when point size is changed.
 */
void
GuiColorFileEditorDialog::pointSizeChangedSlot(double value)
{
   if (currentColorFileIndex >= 0) {
       float pointSize, lineSize;
       colorFile->getPointLineSizeByIndex(currentColorFileIndex, pointSize, lineSize);
       if (value != pointSize) {
          colorFile->setPointLineSizeByIndex(currentColorFileIndex, value, lineSize);
       }
   }
}

/**
 * called when web name push button pressed.
 */
void 
GuiColorFileEditorDialog::slotWebHtmlColorNamesPushButton()
{
   GuiHtmlColorChooserDialog ghccd(this);
   if (ghccd.exec() == GuiHtmlColorChooserDialog::Accepted) {
      QString name;
      unsigned char r, g, b;
      ghccd.getSelectedColor(name, r, g, b);
      redValueSpinBox->setValue(r);
      greenValueSpinBox->setValue(g);
      blueValueSpinBox->setValue(b);
      alphaValueSpinBox->setValue(255);
      updateColorSwatch();
   }
}
      

