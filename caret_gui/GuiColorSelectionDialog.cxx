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


#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QToolTip>

#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiHtmlColorChooserDialog.h"
#include "GuiMainWindow.h"
#include <QDoubleSpinBox>
#include "global_variables.h"

/**
 * Constructor
 */
GuiColorSelectionDialog::GuiColorSelectionDialog(QWidget* parent, 
                                                 const QString& title,
                                                 const bool showAlpha,
                                                 const bool showPointSize,
                                                 const bool showLineSize,
                                                 const bool showSymbol) :
   WuQDialog(parent)

{
   setModal(true);
   setWindowTitle(title);
   
   //
   // Initialize color to foreground color
   //
   unsigned char red, green, blue;
   PreferencesFile* pref = theMainWindow->getBrainSet()->getPreferencesFile();
   pref->getSurfaceForegroundColor(red, green, blue);
   color.setRgb(red, green, blue);
   colorAlpha = 255;
   
   //
   // layout for dialog
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(5);
   
   //
   // "Color swatch" widget
   //
   QGroupBox* swatchGroup = new QGroupBox("Color Swatch");
   rows->addWidget(swatchGroup);
   QVBoxLayout* swatchGroupLayout = new QVBoxLayout(swatchGroup);
   colorSwatch = new QWidget;
   swatchGroupLayout->addWidget(colorSwatch);
   colorSwatch->setMinimumSize(QSize(50, 50));
   colorSwatch->setMaximumHeight(50);
   
   //
   // Grid Layout for Colors Sliders
   //
   QGridLayout* sliderGrid = new QGridLayout;
   rows->addLayout(sliderGrid);
   sliderGrid->setMargin(3);
   sliderGrid->setSpacing(5);
   
   //
   // Red control
   //
   sliderGrid->addWidget(new QLabel("Red"), 0, 0, Qt::AlignLeft);
   redSpinBox = new QSpinBox;
   redSpinBox->setMinimum(0);
   redSpinBox->setMaximum(255);
   redSpinBox->setSingleStep(1);
   sliderGrid->addWidget(redSpinBox, 0, 1, Qt::AlignRight);
   redSlider = new QSlider(Qt::Horizontal);
   redSlider->setMinimumWidth(300);
   redSlider->setMinimum(0);
   redSlider->setMaximum(255);
   redSlider->setPageStep(5);
   redSlider->setValue(red);
   QObject::connect(redSlider, SIGNAL(valueChanged(int)),
                    redSpinBox, SLOT(setValue(int)));
   QObject::connect(redSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(redSliderMovedSlot(int)));
   QObject::connect(redSpinBox, SIGNAL(valueChanged(int)),
                    redSlider, SLOT(setValue(int)));
   QObject::connect(redSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(redSliderMovedSlot(int)));
   sliderGrid->addWidget(redSlider, 0, 2, Qt::AlignLeft);

   //
   // Green control
   //
   sliderGrid->addWidget(new QLabel("Green"), 1, 0, Qt::AlignLeft);
   greenSpinBox = new QSpinBox;
   greenSpinBox->setMinimum(0);
   greenSpinBox->setMaximum(255);
   greenSpinBox->setSingleStep(1);
   sliderGrid->addWidget(greenSpinBox, 1, 1, Qt::AlignRight);
   greenSlider = new QSlider(Qt::Horizontal);
   greenSlider->setMinimum(0);
   greenSlider->setMaximum(255);
   greenSlider->setPageStep(5);
   greenSlider->setMinimumWidth(300);
   greenSlider->setValue(green);
   QObject::connect(greenSlider, SIGNAL(valueChanged(int)),
                    greenSpinBox, SLOT(setValue(int)));
   QObject::connect(greenSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(greenSliderMovedSlot(int)));
   QObject::connect(greenSpinBox, SIGNAL(valueChanged(int)),
                    greenSlider, SLOT(setValue(int)));
   QObject::connect(greenSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(greenSliderMovedSlot(int)));
   sliderGrid->addWidget(greenSlider, 1, 2, Qt::AlignLeft);

   //
   // Blue control
   //
   sliderGrid->addWidget(new QLabel("Blue"), 2, 0, Qt::AlignLeft);
   blueSpinBox = new QSpinBox;
   blueSpinBox->setMinimum(0);
   blueSpinBox->setMaximum(255);
   blueSpinBox->setSingleStep(1);
   sliderGrid->addWidget(blueSpinBox, 2, 1, Qt::AlignRight);
   blueSlider = new QSlider(Qt::Horizontal);
   blueSlider->setMinimum(0);
   blueSlider->setMaximum(255);
   blueSlider->setPageStep(5);
   blueSlider->setMinimumWidth(300);
   blueSlider->setValue(blue);
   QObject::connect(blueSlider, SIGNAL(valueChanged(int)),
                    blueSpinBox, SLOT(setValue(int)));
   QObject::connect(blueSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(blueSliderMovedSlot(int)));
   QObject::connect(blueSpinBox, SIGNAL(valueChanged(int)),
                    blueSlider, SLOT(setValue(int)));
   QObject::connect(blueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(blueSliderMovedSlot(int)));
   sliderGrid->addWidget(blueSlider, 2, 2, Qt::AlignLeft);

   //
   // Alpha control
   //
   QLabel* alphaLabel = new QLabel("Alpha");
   sliderGrid->addWidget(alphaLabel, 3, 0, Qt::AlignLeft);
   alphaSpinBox = new QSpinBox;
   alphaSpinBox->setMinimum(0);
   alphaSpinBox->setMaximum(255);
   alphaSpinBox->setSingleStep(1);
   sliderGrid->addWidget(alphaSpinBox, 3, 1, Qt::AlignRight);
   alphaSlider = new QSlider(Qt::Horizontal);
   alphaSlider->setMinimum(0);
   alphaSlider->setMaximum(255);
   alphaSlider->setPageStep(5);
   alphaSlider->setMinimumWidth(300);
   alphaSlider->setValue(colorAlpha);
   QObject::connect(alphaSlider, SIGNAL(valueChanged(int)),
                    alphaSpinBox, SLOT(setValue(int)));
   QObject::connect(alphaSlider, SIGNAL(valueChanged(int)),
                    this, SLOT(alphaSliderMovedSlot(int)));
   QObject::connect(alphaSpinBox, SIGNAL(valueChanged(int)),
                    alphaSlider, SLOT(setValue(int)));
   QObject::connect(alphaSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(alphaSliderMovedSlot(int)));
   sliderGrid->addWidget(alphaSlider, 3, 2, Qt::AlignLeft);
   if (showAlpha == false) {
      alphaLabel->hide();
      alphaSpinBox->hide();
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
   rows->addLayout(colorPushButtonsLayout);

   //
   // Grid Layout for Line and Point Size
   //
   QGridLayout* linePointSizeGrid = new QGridLayout;
   rows->addLayout(linePointSizeGrid);
   linePointSizeGrid->setMargin(2);
   linePointSizeGrid->setSpacing(5);
   
   float minLineSize, maxLineSize;
   GuiBrainModelOpenGL::getLineWidthRange(minLineSize, maxLineSize);
   QLabel* lineSizeLabel = new QLabel("Line Size");
   linePointSizeGrid->addWidget(lineSizeLabel, 0, 0, Qt::AlignLeft);
   lineSizeDoubleSpinBox = new QDoubleSpinBox;
   lineSizeDoubleSpinBox->setMinimum(minLineSize);
   lineSizeDoubleSpinBox->setMaximum(maxLineSize);
   lineSizeDoubleSpinBox->setSingleStep(1.0);
   lineSizeDoubleSpinBox->setDecimals(1);
   lineSizeDoubleSpinBox->setValue(1);
   linePointSizeGrid->addWidget(lineSizeDoubleSpinBox, 0, 1, Qt::AlignLeft);
   if (showLineSize == false) {
      lineSizeLabel->hide();
      lineSizeDoubleSpinBox->hide();
   }
   
   float minPointSize, maxPointSize;
   GuiBrainModelOpenGL::getPointSizeRange(minPointSize, maxPointSize);
   maxPointSize = 100000.0;
   QLabel* pointSizeLabel = new QLabel("Point Size");
   linePointSizeGrid->addWidget(pointSizeLabel, 1, 0, Qt::AlignLeft);
   pointSizeDoubleSpinBox = new QDoubleSpinBox;
   pointSizeDoubleSpinBox->setMinimum(minPointSize);
   pointSizeDoubleSpinBox->setMaximum(maxPointSize);
   pointSizeDoubleSpinBox->setSingleStep(1.0);
   pointSizeDoubleSpinBox->setDecimals(1);
   pointSizeDoubleSpinBox->setValue(2);
   linePointSizeGrid->addWidget(pointSizeDoubleSpinBox, 1, 1, Qt::AlignLeft);
   if (showPointSize == false) {
      pointSizeLabel->hide();
      pointSizeDoubleSpinBox->hide();
   }
   
   //
   // Symbol
   //
   QLabel* symbolLabel = new QLabel("Symbol");
   linePointSizeGrid->addWidget(symbolLabel, 2, 0, Qt::AlignLeft);
   symbolComboBox = new QComboBox;
   linePointSizeGrid->addWidget(symbolComboBox, 2, 1, Qt::AlignLeft);
   std::vector<QString> symbolLabels;
   ColorFile::ColorStorage::getAllSymbolTypesAsStrings(symbolLabels);
   for (unsigned int i = 0; i < symbolLabels.size(); i++) {
      symbolComboBox->addItem(symbolLabels[i]);
   }
   symbolComboBox->setCurrentIndex(ColorFile::ColorStorage::SYMBOL_OPENGL_POINT);
   if (showSymbol == false) {
      symbolLabel->hide();
      symbolComboBox->hide();
   }
   
   initializeControls();
   
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* ok = new QPushButton("OK", this);
   ok->setFixedSize(ok->sizeHint());
   QObject::connect(ok, SIGNAL(clicked()),
                  this, SLOT(accept()));
   ok->setAutoDefault(false);
   buttonsLayout->addWidget(ok);
}

/**
 * Destructor
 */
GuiColorSelectionDialog::~GuiColorSelectionDialog()
{
}

/**
 * set the color.
 */
void 
GuiColorSelectionDialog::setColor(const unsigned char r,
                                  const unsigned char g,
                                  const unsigned char b,
                                  const unsigned char a)
{
   color.setRgb(r, g, b, a);
   initializeControls();
}
                    
/**
 * called when color chooser button is pressed.
 */
void
GuiColorSelectionDialog::slotColorChooserButton()
{
   //
   // Popup QT's color chooser dialog
   //
   QColor outputColor = QColorDialog::getColor(color, this);
   if (outputColor.isValid()) {
      color = outputColor;
      initializeControls();
   }
}

/**
 * Initialize the controls 
 */
void
GuiColorSelectionDialog::initializeControls()
{
   redSlider->setValue(color.red());
   redSpinBox->setValue(color.red());
   greenSlider->setValue(color.green());
   greenSpinBox->setValue(color.green());
   blueSlider->setValue(color.blue());
   blueSpinBox->setValue(color.blue());
   alphaSlider->setValue(colorAlpha);
   alphaSpinBox->setValue(colorAlpha);
   updateColorSwatch();
}
 
/**
 * Update the color swatch
 */
void 
GuiColorSelectionDialog::updateColorSwatch()
{
   //colorSwatch->setPaletteBackgroundColor(color);
   QPalette pal;
   pal.setColor(QPalette::Window, color);
   colorSwatch->setAutoFillBackground(true);
   colorSwatch->setBackgroundRole(QPalette::Window);
   colorSwatch->setPalette(pal);
}

/**
 * Slot called when the red slider is moved.
 */
void
GuiColorSelectionDialog::redSliderMovedSlot(int value)
{
   color.setRgb(value, color.green(), color.blue());
   updateColorSwatch();
}

/**
 * Slot called when the green slider is moved.
 */
void
GuiColorSelectionDialog::greenSliderMovedSlot(int value)
{
   color.setRgb(color.red(), value, color.blue());
   updateColorSwatch();
}

/**
 * Slot called when the blue slider is moved.
 */
void
GuiColorSelectionDialog::blueSliderMovedSlot(int value)
{
   color.setRgb(color.red(), color.green(), value);
   updateColorSwatch();
}

/**
 * Slot called when the alpha slider is moved.
 */
void
GuiColorSelectionDialog::alphaSliderMovedSlot(int value)
{
   colorAlpha = value;
   updateColorSwatch();
}

/**
 * Called when dialog closed.
 */
void
GuiColorSelectionDialog::done(int r)
{
   color.setRgb(redSpinBox->value(), greenSpinBox->value(), blueSpinBox->value());
   colorAlpha = alphaSpinBox->value();
   
   if (pointSizeDoubleSpinBox != NULL) {
      pointSize = pointSizeDoubleSpinBox->value();
   }
   else {
      pointSize = 2.0;
   }
   if (lineSizeDoubleSpinBox != NULL) {
      lineSize  = lineSizeDoubleSpinBox->value();
   }
   else {
      lineSize = 1.0;
   }
   QDialog::done(r);
}

/**
 * Get the color information
 */
void
GuiColorSelectionDialog::getColorInformation(unsigned char& r, unsigned char& g, unsigned char& b,
                                             unsigned char& a,
                                             float& pointSizeOut, float& lineSizeOut,
                                             ColorFile::ColorStorage::SYMBOL& symbol)
{
   r = static_cast<unsigned char>(color.red());
   g = static_cast<unsigned char>(color.green());
   b = static_cast<unsigned char>(color.blue());
   a = static_cast<unsigned char>(colorAlpha);
   pointSizeOut = pointSize;
   lineSizeOut  = lineSize;
   symbol = static_cast<ColorFile::ColorStorage::SYMBOL>(symbolComboBox->currentIndex());
}

/**
 * called when web name push button pressed.
 */
void 
GuiColorSelectionDialog::slotWebHtmlColorNamesPushButton()
{
   GuiHtmlColorChooserDialog ghccd(this);
   if (ghccd.exec() == GuiHtmlColorChooserDialog::Accepted) {
      QString name;
      unsigned char r, g, b;
      ghccd.getSelectedColor(name, r, g, b);
      color.setRed(static_cast<int>(r));
      color.setGreen(static_cast<int>(g));
      color.setBlue(static_cast<int>(b));
      color.setAlpha(255);
      initializeControls();
   }
}
      
