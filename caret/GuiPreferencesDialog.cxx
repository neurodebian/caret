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

#include <QGlobalStatic>

#include <limits>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolTip>

#include "BrainSet.h"
#include "DebugControl.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiMainWindow.h"
#include "GuiPreferencesDialog.h"
#include "QtUtilities.h"
#include "SystemUtilities.h"
#include "WuQWidgetGroup.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiPreferencesDialog::GuiPreferencesDialog(QWidget* parent) 
   : WuQDialog(parent)
{
   setWindowTitle("Caret Preferences");
   
   //
   // Widget group for all widgets that get updated
   //
   allWidgetsGroup = new WuQWidgetGroup(this);
   
   //
   // Create the tab pages
   //
   QTabWidget* tabWidget = new QTabWidget;
   tabWidget->addTab(createDebugSection(), "Debug");
   tabWidget->addTab(createFileSection(), "File");
   tabWidget->addTab(createMiscSection(), "Misc");
   tabWidget->addTab(createOpenglSection(), "OpenGL");
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);

   //
   // Apply pushbutton
   //
   QPushButton* applyButton = new QPushButton("Apply");
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   //
   // Force buttons to be same size
   //
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(tabWidget);
   dialogLayout->addLayout(buttonsLayout);
   
   loadPreferences();
}

/**
 * create the debug section.
 */
QWidget* 
GuiPreferencesDialog::createDebugSection()
{
   //
   // Debug enable
   //
   debugOnCheckBox = new QCheckBox("Debug Enabled");
   QObject::connect(debugOnCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Debug Node
   //
   QLabel* debugNodeLabel = new QLabel("DebugNode");
   debugNodeSpinBox = new QSpinBox;
   debugNodeSpinBox->setMinimum(-1);
   debugNodeSpinBox->setMaximum(std::numeric_limits<int>::max());
   debugNodeSpinBox->setSingleStep(1);
   QObject::connect(debugNodeSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Test Flag 1
   //
   testFlag1CheckBox = new QCheckBox("Test Flag 1");
   QObject::connect(testFlag1CheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Test Flag 2
   //
   testFlag2CheckBox = new QCheckBox("Test Flag 2");
   QObject::connect(testFlag2CheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Enable OpenGL debugging
   //
   openGLDebugCheckBox = new QCheckBox("OpenGL Debugging");
   QObject::connect(openGLDebugCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));

   //
   // updatable widget group
   //
   allWidgetsGroup->addWidget(debugOnCheckBox);
   allWidgetsGroup->addWidget(debugNodeSpinBox);
   allWidgetsGroup->addWidget(testFlag1CheckBox);
   allWidgetsGroup->addWidget(testFlag2CheckBox);
   allWidgetsGroup->addWidget(openGLDebugCheckBox);
   
   //
   // Widget and layout
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(debugOnCheckBox, 0, 0, 1, 2, Qt::AlignLeft);
   gridLayout->addWidget(debugNodeLabel, 1, 0);
   gridLayout->addWidget(debugNodeSpinBox, 1, 1);
   gridLayout->addWidget(testFlag1CheckBox, 2, 0, 1, 2, Qt::AlignLeft);
   gridLayout->addWidget(testFlag2CheckBox, 3, 0, 1, 2, Qt::AlignLeft);
   gridLayout->addWidget(openGLDebugCheckBox, 4, 0, 1, 2, Qt::AlignLeft);
   QHBoxLayout* layout = new QHBoxLayout;
   layout->addLayout(gridLayout);
   layout->addStretch();
   QWidget* w = new QWidget;
   QVBoxLayout* vertLayout = new QVBoxLayout(w);
   vertLayout->addLayout(layout);
   vertLayout->addStretch();
   return w;
}

/**
 * create the file section.
 */
QWidget* 
GuiPreferencesDialog::createFileSection()
{
   //
   // Number of spec file data file reading threads
   //
   QLabel* fileReadThreadsLabel = new QLabel("File Reading Threads");
   numberOfSpecFileReadThreadsSpinBox = new QSpinBox;
   numberOfSpecFileReadThreadsSpinBox->setMinimum(1);
   numberOfSpecFileReadThreadsSpinBox->setMaximum(4096);
   numberOfSpecFileReadThreadsSpinBox->setSingleStep(1);
   QObject::connect(numberOfSpecFileReadThreadsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
                    
   //
   // Floating point precision for text files
   //
   QLabel* floatPrecisionLabel = new QLabel("Text File Float Precision");
   floatPrecisionSpinBox = new QSpinBox;
   floatPrecisionSpinBox->setMinimum(1);
   floatPrecisionSpinBox->setMaximum(1024);
   floatPrecisionSpinBox->setSingleStep(1);
   QObject::connect(floatPrecisionSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Create the file format combo boxes
   //
   QGroupBox* fileFormatGroupBox = new QGroupBox("File Write Format");
   QGridLayout* formatGridLayout = new QGridLayout(fileFormatGroupBox);
   std::vector<AbstractFile::FILE_FORMAT> fileFormats;
   std::vector<QString> fileFormatNames;
   AbstractFile::getFileFormatTypesAndNames(fileFormats, fileFormatNames);
   const int numberOfFormats = static_cast<int>(fileFormats.size());
   for (int i = 0; i < numberOfFormats; i++) {
      QLabel* label = new QLabel("");
      if (i == 0) {
         label->setText("Highest Priority");
      }
      else if (i == (numberOfFormats - 1)) {
         label->setText("Lowest Priority");
      }
      QComboBox* comboBox = new QComboBox;
      QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)),
                       this, SLOT(applyButtonSlot()));
      formatGridLayout->addWidget(label, i, 0);
      formatGridLayout->addWidget(comboBox, i, 1);
      
      comboBox->blockSignals(true);
      for (int j = 0; j < numberOfFormats; j++) {
         comboBox->addItem(fileFormatNames[j],
                           QVariant(static_cast<int>(fileFormats[j])));
      }
      comboBox->setToolTip(
                    "Changing this will not have an effect\n"
                    "until the next time Caret is started.");
      comboBox->blockSignals(false);
                    
      fileWriteDataTypeComboBox.push_back(comboBox);
      allWidgetsGroup->addWidget(comboBox);
   }
   
   
   //
   // updatable widget group
   //
   allWidgetsGroup->addWidget(numberOfSpecFileReadThreadsSpinBox);
   allWidgetsGroup->addWidget(floatPrecisionSpinBox);
   
   //
   // widget and layout
   //
   QWidget* w = new QWidget;
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(fileReadThreadsLabel, 0, 0);
   gridLayout->addWidget(numberOfSpecFileReadThreadsSpinBox, 0, 1);
   gridLayout->addWidget(floatPrecisionLabel, 1, 0);
   gridLayout->addWidget(floatPrecisionSpinBox, 1, 1);
   QHBoxLayout* leftLayout = new QHBoxLayout;
   leftLayout->addLayout(gridLayout);
   leftLayout->addStretch();
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(leftLayout);
   layout->addWidget(fileFormatGroupBox, 0, Qt::AlignLeft);
   layout->addStretch();
   return w;
}

/**
 * create the misc section.
 */
QWidget* 
GuiPreferencesDialog::createMiscSection()
{
   //
   // Mouse Speed
   //
   QLabel* mouseSpeedLabel = new QLabel("Mouse Speed");
   mouseSpeedDoubleSpinBox = new QDoubleSpinBox;
   mouseSpeedDoubleSpinBox->setMinimum(0.0);
   mouseSpeedDoubleSpinBox->setMaximum(5000000.0);
   mouseSpeedDoubleSpinBox->setSingleStep(1);
   mouseSpeedDoubleSpinBox->setDecimals(3);
   QObject::connect(mouseSpeedDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Iterative update
   //
   QLabel* iterativeUpdateLabel = new QLabel("Iterative Update");
   iterUpdateSpinBox = new QSpinBox;
   iterUpdateSpinBox->setMinimum(0);
   iterUpdateSpinBox->setMaximum(5000000);
   iterUpdateSpinBox->setSingleStep(5);
   QObject::connect(iterUpdateSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
                    
   //
   // Maximum number of threads
   //
   const int numCPU = SystemUtilities::getNumberOfProcessors();
   const QString threadStr("Number of Threads ("
                           + QString::number(numCPU)
                           + " CPUs)");
   QLabel* numberOfThreadsLabel = new QLabel(threadStr);
   numberOfThreadsSpinBox = new QSpinBox;
   numberOfThreadsSpinBox->setMinimum(0);
   numberOfThreadsSpinBox->setMaximum(numCPU * 4);
   numberOfThreadsSpinBox->setSingleStep(1);
   QObject::connect(numberOfThreadsSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   if (DebugControl::getDebugOn()) {
      numberOfThreadsLabel->show();
      numberOfThreadsSpinBox->show();
   }
   else {
      numberOfThreadsLabel->hide();
      numberOfThreadsSpinBox->hide();
   }
    
   //
   // Random seed
   //
   QString randHelpText =
      "If the check box is checked, the value in the box to the right will\n"
      "be used to set the \"seed\" for the random number generator at the \n"
      "time Caret is started or when the \"Apply\" button on this dialog\n"
      "is pressed.  If not checked, Caret will set the seed to the number\n"
      "of seconds since 1970 at the time Caret IS started.  If you choose\n"
      "to use a specific seed, that will result in the random number generator\n"
      "generating the same sequence of numbers when the \"rand()\" function\n"
      "is called.";
   randomSeedCheckBox = new QCheckBox("Random Seed");
   QObject::connect(randomSeedCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));
   randomSeedCheckBox->setToolTip(randHelpText);
   randomSeedSpinBox = new QSpinBox;
   randomSeedSpinBox->setMinimum(1);
   randomSeedSpinBox->setMaximum(std::numeric_limits<int>::max());
   randomSeedSpinBox->setSingleStep(1);
   QObject::connect(randomSeedSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   randomSeedSpinBox->setToolTip(randHelpText);
                    
   //
   // Sums Hosts
   //
   QLabel* sumsHostLabel = new QLabel("SuMS Hosts");
   sumsHostsLineEdit = new QLineEdit;
   sumsHostsLineEdit->setToolTip(
                 "Separate multiple\n"
                 "hosts with commas.");
   QObject::connect(sumsHostsLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(applyButtonSlot()));
                    
   //
   // Web browser (do not show for Mac or Windows
   //
   QLabel* webBrowserLabel = new QLabel("Web Browser");
   webBrowserLineEdit = new QLineEdit;
   webBrowserLabel->hide();
   webBrowserLineEdit->hide();
#ifndef Q_OS_WIN32
#ifndef Q_OS_MACX
   webBrowserLabel->show();
   webBrowserLineEdit->show();
#endif // Q_OS_MACX
#endif // Q_OS_WIN32

   //
   // updatable widget group
   //
   allWidgetsGroup->addWidget(iterUpdateSpinBox);
   allWidgetsGroup->addWidget(mouseSpeedDoubleSpinBox);
   allWidgetsGroup->addWidget(numberOfThreadsSpinBox);
   allWidgetsGroup->addWidget(randomSeedCheckBox);
   allWidgetsGroup->addWidget(randomSeedSpinBox);
   allWidgetsGroup->addWidget(sumsHostsLineEdit);
   allWidgetsGroup->addWidget(webBrowserLineEdit);
   
   //
   // Widget and label
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(iterativeUpdateLabel, 0, 0);
   gridLayout->addWidget(iterUpdateSpinBox, 0, 1);
   gridLayout->addWidget(mouseSpeedLabel, 1, 0);
   gridLayout->addWidget(mouseSpeedDoubleSpinBox, 1, 1);
   gridLayout->addWidget(numberOfThreadsLabel, 2, 0);
   gridLayout->addWidget(numberOfThreadsSpinBox, 2, 1);
   gridLayout->addWidget(randomSeedCheckBox, 2, 0);
   gridLayout->addWidget(randomSeedSpinBox, 2, 1);
   gridLayout->addWidget(sumsHostLabel, 3, 0);
   gridLayout->addWidget(sumsHostsLineEdit, 3, 1);
   gridLayout->addWidget(webBrowserLabel, 4, 0);
   gridLayout->addWidget(webBrowserLineEdit, 4, 1);
   QHBoxLayout* layout = new QHBoxLayout;
   layout->addLayout(gridLayout);
   layout->addStretch();
   QWidget* w = new QWidget;
   QVBoxLayout* vertLayout = new QVBoxLayout(w);
   vertLayout->addLayout(layout);
   vertLayout->addStretch();
   return w;
}

/**
 * called to set background color.
 */
void 
GuiPreferencesDialog::slotSetBackgroundColor()
{
   GuiColorSelectionDialog csd(this,
                               "Background Color",
                               false,
                               false,
                               false,
                               false);
   csd.setColor(static_cast<unsigned char>(surfaceBackgroundRedSpinBox->value()),
                static_cast<unsigned char>(surfaceBackgroundGreenSpinBox->value()),
                static_cast<unsigned char>(surfaceBackgroundBlueSpinBox->value()),
                255);
   if (csd.exec() == GuiColorSelectionDialog::Accepted) {
      unsigned char r, g, b, a;
      float pointSize, lineSize;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd.getColorInformation(r, g, b, a,
                              pointSize, lineSize,
                              symbol);
      allWidgetsGroup->blockSignals(true);
      surfaceBackgroundRedSpinBox->setValue(r);
      surfaceBackgroundGreenSpinBox->setValue(g);
      surfaceBackgroundBlueSpinBox->setValue(b);
      allWidgetsGroup->blockSignals(true);
      applyButtonSlot();
   }
}

/**
 * called to set foreground color.
 */
void 
GuiPreferencesDialog::slotSetForegroundColor()
{
   GuiColorSelectionDialog csd(this,
                               "Background Color",
                               false,
                               false,
                               false,
                               false);
   csd.setColor(static_cast<unsigned char>(surfaceForegroundRedSpinBox->value()),
                static_cast<unsigned char>(surfaceForegroundGreenSpinBox->value()),
                static_cast<unsigned char>(surfaceForegroundBlueSpinBox->value()),
                255);
   if (csd.exec() == GuiColorSelectionDialog::Accepted) {
      unsigned char r, g, b, a;
      float pointSize, lineSize;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd.getColorInformation(r, g, b, a,
                              pointSize, lineSize,
                              symbol);
      allWidgetsGroup->blockSignals(true);
      surfaceForegroundRedSpinBox->setValue(r);
      surfaceForegroundGreenSpinBox->setValue(g);
      surfaceForegroundBlueSpinBox->setValue(b);
      allWidgetsGroup->blockSignals(true);
      applyButtonSlot();
   }
}
      
/**
 * create the opengl section.
 */
QWidget* 
GuiPreferencesDialog::createOpenglSection()
{
   //
   // Surface Background Color
   //   
   QPushButton* surfaceBackgroundPushButton = new QPushButton("Background Color...");
   surfaceBackgroundPushButton->setAutoDefault(false);
   surfaceBackgroundPushButton->setToolTip("Press this button to set\n"
                                           "the background color.");
   QObject::connect(surfaceBackgroundPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetBackgroundColor()));
   surfaceBackgroundRedSpinBox = new QSpinBox;
   surfaceBackgroundRedSpinBox->setMinimum(0);
   surfaceBackgroundRedSpinBox->setMaximum(255);
   surfaceBackgroundRedSpinBox->setSingleStep(1);
   QObject::connect(surfaceBackgroundRedSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   surfaceBackgroundGreenSpinBox = new QSpinBox;
   surfaceBackgroundGreenSpinBox->setMinimum(0);
   surfaceBackgroundGreenSpinBox->setMaximum(255);
   surfaceBackgroundGreenSpinBox->setSingleStep(1);
   QObject::connect(surfaceBackgroundGreenSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   surfaceBackgroundBlueSpinBox = new QSpinBox;
   surfaceBackgroundBlueSpinBox->setMinimum(0);
   surfaceBackgroundBlueSpinBox->setMaximum(255);
   surfaceBackgroundBlueSpinBox->setSingleStep(1);
   QObject::connect(surfaceBackgroundBlueSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
      
   //
   // Surface Foreground Color
   //   
   QPushButton* surfaceForegroundPushButton = new QPushButton("Foreground Color...");
   surfaceForegroundPushButton->setAutoDefault(false);
   surfaceForegroundPushButton->setToolTip("Press this button to set\n"
                                           "the foreground color.");
   QObject::connect(surfaceForegroundPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSetForegroundColor()));
   surfaceForegroundRedSpinBox = new QSpinBox;
   surfaceForegroundRedSpinBox->setMinimum(0);
   surfaceForegroundRedSpinBox->setMaximum(255);
   surfaceForegroundRedSpinBox->setSingleStep(1);
   surfaceForegroundGreenSpinBox = new QSpinBox;
   surfaceForegroundGreenSpinBox->setMinimum(0);
   surfaceForegroundGreenSpinBox->setMaximum(255);
   surfaceForegroundGreenSpinBox->setSingleStep(1);
   surfaceForegroundBlueSpinBox = new QSpinBox;
   surfaceForegroundBlueSpinBox->setMinimum(0);
   surfaceForegroundBlueSpinBox->setMaximum(255);
   surfaceForegroundBlueSpinBox->setSingleStep(1);
      
   //
   // Light Position
   //
   QLabel* lightPositionLabel = new QLabel("Light Position");
   lightPositionXDoubleSpinBox = new QDoubleSpinBox;
   lightPositionXDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   lightPositionXDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   lightPositionXDoubleSpinBox->setSingleStep(1.0);
   lightPositionXDoubleSpinBox->setDecimals(3);
   QObject::connect(lightPositionXDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(applyButtonSlot()));
   lightPositionYDoubleSpinBox = new QDoubleSpinBox;
   lightPositionYDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   lightPositionYDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   lightPositionYDoubleSpinBox->setSingleStep(1.0);
   lightPositionYDoubleSpinBox->setDecimals(3);
   QObject::connect(lightPositionYDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(applyButtonSlot()));
   lightPositionZDoubleSpinBox = new QDoubleSpinBox;
   lightPositionZDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   lightPositionZDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   lightPositionZDoubleSpinBox->setSingleStep(1.0);
   lightPositionZDoubleSpinBox->setDecimals(3);
   QObject::connect(lightPositionZDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Image capture
   //
   QLabel* imageCaptureLabel = new QLabel("Image Capture");
   imageCaptureTypeComboBox = new QComboBox;
   imageCaptureTypeComboBox->insertItem(PreferencesFile::IMAGE_CAPTURE_PIXMAP,
                                        "Use Pixmap");
   imageCaptureTypeComboBox->insertItem(PreferencesFile::IMAGE_CAPTURE_OPENGL_BUFFER,
                                        "Use OpenGL Buffer");

   //
   // Display lists
   //
   displayListsOnCheckBox = new QCheckBox("Display Lists Enabled");
   QObject::connect(displayListsOnCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));
   QPushButton* resetDisplayListsPushButton = new QPushButton("Reset Lists");                 
   resetDisplayListsPushButton->setAutoDefault(false);
   QObject::connect(resetDisplayListsPushButton, SIGNAL(clicked()),
                         this, SLOT(slotResetDisplayListsPushButton()));
                         
   //
   // updatable widget group
   //
   allWidgetsGroup->addWidget(surfaceBackgroundRedSpinBox);
   allWidgetsGroup->addWidget(surfaceBackgroundGreenSpinBox);
   allWidgetsGroup->addWidget(surfaceBackgroundBlueSpinBox);
   allWidgetsGroup->addWidget(surfaceForegroundRedSpinBox);
   allWidgetsGroup->addWidget(surfaceForegroundGreenSpinBox);
   allWidgetsGroup->addWidget(surfaceForegroundBlueSpinBox);
   allWidgetsGroup->addWidget(lightPositionXDoubleSpinBox);
   allWidgetsGroup->addWidget(lightPositionYDoubleSpinBox);
   allWidgetsGroup->addWidget(lightPositionZDoubleSpinBox);
   allWidgetsGroup->addWidget(imageCaptureTypeComboBox);
   allWidgetsGroup->addWidget(displayListsOnCheckBox);
   
   //
   // Widget and line edit
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(surfaceBackgroundPushButton, 0, 0);
   gridLayout->addWidget(surfaceBackgroundRedSpinBox, 0, 1);
   gridLayout->addWidget(surfaceBackgroundGreenSpinBox, 0, 2);
   gridLayout->addWidget(surfaceBackgroundBlueSpinBox, 0, 3);
   gridLayout->addWidget(surfaceForegroundPushButton, 1, 0);
   gridLayout->addWidget(surfaceForegroundRedSpinBox, 1, 1);
   gridLayout->addWidget(surfaceForegroundGreenSpinBox, 1, 2);
   gridLayout->addWidget(surfaceForegroundBlueSpinBox, 1, 3);
   gridLayout->addWidget(lightPositionLabel, 2, 0);
   gridLayout->addWidget(lightPositionXDoubleSpinBox, 2, 1);
   gridLayout->addWidget(lightPositionYDoubleSpinBox, 2, 2);
   gridLayout->addWidget(lightPositionZDoubleSpinBox, 2, 3);
   gridLayout->addWidget(imageCaptureLabel, 3, 0);
   gridLayout->addWidget(imageCaptureTypeComboBox, 3, 1, 1, 3, Qt::AlignLeft);
   gridLayout->addWidget(displayListsOnCheckBox, 4, 0);
   gridLayout->addWidget(resetDisplayListsPushButton, 4, 1, 1, 3, Qt::AlignLeft);
   QWidget* w = new QWidget;
   QVBoxLayout* vertLayout = new QVBoxLayout(w);
   vertLayout->addLayout(gridLayout);
   vertLayout->addStretch();
   return w;
}

/**
 * load preferences into dialog
 */
void
GuiPreferencesDialog::loadPreferences()
{
   allWidgetsGroup->blockSignals(true);
   
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   
   unsigned char r, g, b;
   pf->getSurfaceBackgroundColor(r, g, b);
   surfaceBackgroundRedSpinBox->setValue(static_cast<int>(r));
   surfaceBackgroundGreenSpinBox->setValue(static_cast<int>(g));
   surfaceBackgroundBlueSpinBox->setValue(static_cast<int>(b));

   pf->getSurfaceForegroundColor(r, g, b);
   surfaceForegroundRedSpinBox->setValue(static_cast<int>(r));
   surfaceForegroundGreenSpinBox->setValue(static_cast<int>(g));
   surfaceForegroundBlueSpinBox->setValue(static_cast<int>(b));
   
   float x, y, z;
   pf->getLightPosition(x, y, z);
   lightPositionXDoubleSpinBox->setValue(x);
   lightPositionYDoubleSpinBox->setValue(y);
   lightPositionZDoubleSpinBox->setValue(z);
   
   mouseSpeedDoubleSpinBox->setValue(pf->getMouseSpeed());
   
   iterUpdateSpinBox->setValue(pf->getIterativeUpdate());

   imageCaptureTypeComboBox->setCurrentIndex(static_cast<int>(pf->getImageCaptureType()));
   
   debugOnCheckBox->setChecked(pf->getDebugOn());
   
   debugNodeSpinBox->setValue(pf->getDebugNodeNumber());
   
   testFlag1CheckBox->setChecked(pf->getTestFlag1());
   testFlag2CheckBox->setChecked(pf->getTestFlag2());
   
   openGLDebugCheckBox->setChecked(pf->getOpenGLDebug());
   
   displayListsOnCheckBox->setChecked(pf->getDisplayListsEnabled());
   
   if (webBrowserLineEdit->isVisible()) {
      webBrowserLineEdit->setText(pf->getWebBrowser());
   }
   
   if (numberOfThreadsSpinBox->isVisible()) {
      numberOfThreadsSpinBox->setValue(pf->getMaximumNumberOfThreads());
   }
   
   numberOfSpecFileReadThreadsSpinBox->setValue(pf->getNumberOfFileReadingThreads());
   
   floatPrecisionSpinBox->setValue(pf->getTextFileDigitsRightOfDecimal());
   
   const std::vector<AbstractFile::FILE_FORMAT> fileFormats = 
                                        pf->getPreferredWriteDataType();
   for (unsigned int j = 0; j < fileWriteDataTypeComboBox.size(); j++) {
      QComboBox* comboBox = fileWriteDataTypeComboBox[j];
      for (int i = 0; i < comboBox->count(); i++) {
         if (j < fileFormats.size()) {
            if (static_cast<AbstractFile::FILE_FORMAT>(comboBox->itemData(i).toInt())
                      == fileFormats[j]) {
               comboBox->setCurrentIndex(i);
               break;
            }
         }
      }
   }
   
   sumsHostsLineEdit->setText(pf->getSumsDatabaseHosts());
   randomSeedCheckBox->setChecked(pf->getRandomSeedOverride());
   randomSeedSpinBox->setValue(pf->getRandomSeedOverrideValue());
   randomSeedSpinBox->setEnabled(randomSeedCheckBox->isChecked());
   
   allWidgetsGroup->blockSignals(false);
}
 
/**
 * Destructor.
 */
GuiPreferencesDialog::~GuiPreferencesDialog()
{
}

/**
 * called by reset display lists push button.
 */
void 
GuiPreferencesDialog::slotResetDisplayListsPushButton()
{
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * Called when apply button is pressed.
 */
void
GuiPreferencesDialog::applyButtonSlot()
{
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   
   pf->setSurfaceBackgroundColor(static_cast<unsigned char>(surfaceBackgroundRedSpinBox->value()),
                                 static_cast<unsigned char>(surfaceBackgroundGreenSpinBox->value()),
                                 static_cast<unsigned char>(surfaceBackgroundBlueSpinBox->value()));
   
   pf->setSurfaceForegroundColor(static_cast<unsigned char>(surfaceForegroundRedSpinBox->value()),
                                 static_cast<unsigned char>(surfaceForegroundGreenSpinBox->value()),
                                 static_cast<unsigned char>(surfaceForegroundBlueSpinBox->value()));
   
   const bool colorsTheSame =
      (surfaceBackgroundRedSpinBox->value() == surfaceForegroundRedSpinBox->value()) &&
      (surfaceBackgroundGreenSpinBox->value() == surfaceForegroundGreenSpinBox->value()) &&
      (surfaceBackgroundBlueSpinBox->value() == surfaceForegroundBlueSpinBox->value());
      
   if (colorsTheSame) {
      QMessageBox::warning(this, "Color Warning",
                           "The foreground and background colors are the same.\n"
                           "This will prevent some items in the main window, such\n"
                           "as color palette numbers, from being seen.  You should\n"
                           "change the foreground and background colors to\n"
                           "contrasting colors.");
   }
   
   pf->setLightPosition(lightPositionXDoubleSpinBox->value(),
                        lightPositionYDoubleSpinBox->value(),
                        lightPositionZDoubleSpinBox->value());
                        
   pf->setMouseSpeed(mouseSpeedDoubleSpinBox->value());
   
   pf->setIterativeUpdate(iterUpdateSpinBox->value());
   
   pf->setImageCaptureType(
      static_cast<PreferencesFile::IMAGE_CAPTURE_TYPE>(
                          imageCaptureTypeComboBox->currentIndex()));
                          
   pf->setDebugOn(debugOnCheckBox->isChecked());
   pf->setDebugNodeNumber(debugNodeSpinBox->value());
   
   pf->setTestFlag1(testFlag1CheckBox->isChecked());
   pf->setTestFlag2(testFlag2CheckBox->isChecked());
   
   pf->setOpenGLDebug(openGLDebugCheckBox->isChecked());
   
   pf->setDisplayListsEnabled(displayListsOnCheckBox->isChecked());
   if (pf->getDisplayListsEnabled() == false) {
      theMainWindow->getBrainSet()->clearAllDisplayLists();
   }
   
   if (webBrowserLineEdit->isVisible()) {
      pf->setWebBrowser(webBrowserLineEdit->text());
   }
   
   if (numberOfThreadsSpinBox->isVisible()) {
      pf->setMaximumNumberOfThreads(numberOfThreadsSpinBox->value());
   }
   
   pf->setNumberOfFileReadingThreads(numberOfSpecFileReadThreadsSpinBox->value());
   
   pf->setTextFileDigitsRightOfDecimal(floatPrecisionSpinBox->value());
   AbstractFile::setTextFileDigitsRightOfDecimal(pf->getTextFileDigitsRightOfDecimal());
   
   std::vector<AbstractFile::FILE_FORMAT> fileFormats;
   for (unsigned int i = 0; i < fileWriteDataTypeComboBox.size(); i++) {
      const int indx = fileWriteDataTypeComboBox[i]->currentIndex();
      fileFormats.push_back(
         static_cast<AbstractFile::FILE_FORMAT>(fileWriteDataTypeComboBox[i]->itemData(indx).toInt()));
   }
   pf->setPreferredWriteDataType(fileFormats);
   AbstractFile::setPreferredWriteType(fileFormats); 
        
   pf->setSumsDatabaseHosts(sumsHostsLineEdit->text());
   
   pf->setRandomSeedOverride(randomSeedCheckBox->isChecked());
   pf->setRandomSeedOverrideValue(randomSeedSpinBox->value());
   if (randomSeedCheckBox->isChecked()) {
      BrainSet::setRandomSeed(randomSeedSpinBox->value());
   }
   randomSeedSpinBox->setEnabled(randomSeedCheckBox->isChecked());

   try {
      pf->writeFile(pf->getFileName());
   }
   catch (FileException& e) {
      QString msg("Unable to write preferences file because ");
      msg.append(e.whatQString());
      QMessageBox::warning(this, "Error Writing Preferences", msg);
   }
   
   GuiBrainModelOpenGL::updateAllGL(NULL);
}


