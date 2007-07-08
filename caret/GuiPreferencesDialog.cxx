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

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QToolTip>

#include "BrainSet.h"
#include "DebugControl.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiPreferencesDialog.h"
#include "QtUtilities.h"
#include "SystemUtilities.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiPreferencesDialog::GuiPreferencesDialog(QWidget* parent) 
   : QtDialog(parent, false)
{
   setWindowTitle("Caret Preferences");
   
   loadingPreferencesFile = false;
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   int numRows = 0;
   const int SURFACE_BACKGROUND_ROW = numRows++;
   const int SURFACE_FOREGROUND_ROW = numRows++;
   const int LIGHT_POSITION_ROW     = numRows++;
   const int MOUSE_SPEED_ROW        = numRows++;
   const int ITER_UPDATE_ROW        = numRows++;
   const int IMAGE_CAPTURE_ROW      = numRows++;
   const int DEBUG_ROW              = numRows++;
   const int DEBUG_NODE_ROW         = numRows++;
   const int DISPLAY_LIST_ROW       = numRows++;
#ifndef Q_OS_WIN32
#ifndef Q_OS_MACX
   const int WEB_BROWSER_ROW        = numRows++;
#endif
#endif
   const int THREADS_ROW            = numRows++;
#ifdef Q_OS_WIN32
   const int MOTIF_COMBO_ROW        = numRows++;
#endif
   const int SPEECH_ROW             = numRows++;
   const int FLOAT_ROW              = numRows++;
   const int FILE_WRITE_ROW         = numRows++;
   const int SUMS_ROW               = numRows++;
   const int RANDOM_SEED_ROW        = numRows++;
   
   //
   // Grid for preferences
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->setSpacing(5);
   dialogLayout->addLayout(gridLayout);
   
   //
   // Surface Background Color
   //   
   gridLayout->addWidget(new QLabel("Surface Background Color"), 
                         SURFACE_BACKGROUND_ROW, 0, Qt::AlignLeft);
   surfaceBackgroundRedSpinBox = new QSpinBox;
   surfaceBackgroundRedSpinBox->setMinimum(0);
   surfaceBackgroundRedSpinBox->setMaximum(255);
   surfaceBackgroundRedSpinBox->setSingleStep(1);
   gridLayout->addWidget(surfaceBackgroundRedSpinBox, SURFACE_BACKGROUND_ROW, 1, Qt::AlignLeft);
   surfaceBackgroundGreenSpinBox = new QSpinBox;
   surfaceBackgroundGreenSpinBox->setMinimum(0);
   surfaceBackgroundGreenSpinBox->setMaximum(255);
   surfaceBackgroundGreenSpinBox->setSingleStep(1);
   gridLayout->addWidget(surfaceBackgroundGreenSpinBox, SURFACE_BACKGROUND_ROW, 2, Qt::AlignLeft);
   surfaceBackgroundBlueSpinBox = new QSpinBox;
   surfaceBackgroundBlueSpinBox->setMinimum(0);
   surfaceBackgroundBlueSpinBox->setMaximum(255);
   surfaceBackgroundBlueSpinBox->setSingleStep(1);
   gridLayout->addWidget(surfaceBackgroundBlueSpinBox, SURFACE_BACKGROUND_ROW, 3, Qt::AlignLeft);
      
   //
   // Surface Foreground Color
   //   
   gridLayout->addWidget(new QLabel("Surface Foreground Color"), 
                         SURFACE_FOREGROUND_ROW, 0, Qt::AlignLeft);
   surfaceForegroundRedSpinBox = new QSpinBox;
   surfaceForegroundRedSpinBox->setMinimum(0);
   surfaceForegroundRedSpinBox->setMaximum(255);
   surfaceForegroundRedSpinBox->setSingleStep(1);
   gridLayout->addWidget(surfaceForegroundRedSpinBox, SURFACE_FOREGROUND_ROW, 1, Qt::AlignLeft);
   surfaceForegroundGreenSpinBox = new QSpinBox;
   surfaceForegroundGreenSpinBox->setMinimum(0);
   surfaceForegroundGreenSpinBox->setMaximum(255);
   surfaceForegroundGreenSpinBox->setSingleStep(1);
   gridLayout->addWidget(surfaceForegroundGreenSpinBox, SURFACE_FOREGROUND_ROW, 2, Qt::AlignLeft);
   surfaceForegroundBlueSpinBox = new QSpinBox;
   surfaceForegroundBlueSpinBox->setMinimum(0);
   surfaceForegroundBlueSpinBox->setMaximum(255);
   surfaceForegroundBlueSpinBox->setSingleStep(1);
   gridLayout->addWidget(surfaceForegroundBlueSpinBox, SURFACE_FOREGROUND_ROW, 3, Qt::AlignLeft);
      
   //
   // Light Position
   //
   gridLayout->addWidget(new QLabel("Light Position"),
                         LIGHT_POSITION_ROW, 0, Qt::AlignLeft);
   lightPositionLineEditX = new QLineEdit;
   gridLayout->addWidget(lightPositionLineEditX, LIGHT_POSITION_ROW, 1, Qt::AlignLeft);
   QObject::connect(lightPositionLineEditX, SIGNAL(returnPressed()),
                    this, SLOT(applyButtonSlot()));
   lightPositionLineEditY = new QLineEdit;
   gridLayout->addWidget(lightPositionLineEditY, LIGHT_POSITION_ROW, 2, Qt::AlignLeft);
   QObject::connect(lightPositionLineEditY, SIGNAL(returnPressed()),
                    this, SLOT(applyButtonSlot()));
   lightPositionLineEditZ = new QLineEdit;
   gridLayout->addWidget(lightPositionLineEditZ, LIGHT_POSITION_ROW, 3, Qt::AlignLeft);
   QObject::connect(lightPositionLineEditZ, SIGNAL(returnPressed()),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Mouse Speed
   //
   gridLayout->addWidget(new QLabel("Mouse Speed"),
                         MOUSE_SPEED_ROW, 0, Qt::AlignLeft);
   mouseSpeedLineEdit = new QLineEdit;
   gridLayout->addWidget(mouseSpeedLineEdit, MOUSE_SPEED_ROW, 1,
                                  1, 3, Qt::AlignLeft);
   QObject::connect(mouseSpeedLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Sums Hosts
   //
   gridLayout->addWidget(new QLabel("SuMS Hosts"),
                         SUMS_ROW, 0, Qt::AlignLeft);
   sumsHostsLineEdit = new QLineEdit;
   sumsHostsLineEdit->setToolTip(
                 "Separate multiple\n"
                 "hosts with commas.");
   gridLayout->addWidget(sumsHostsLineEdit, SUMS_ROW, 1,
                                  1, 3, Qt::AlignLeft);
   QObject::connect(sumsHostsLineEdit, SIGNAL(returnPressed()),
                    this, SLOT(applyButtonSlot()));
                    
   //
   // Iterative update
   //
   gridLayout->addWidget(new QLabel("Iterative Update"),
                         ITER_UPDATE_ROW, 0, Qt::AlignLeft);
   iterUpdateSpinBox = new QSpinBox;
   iterUpdateSpinBox->setMinimum(0);
   iterUpdateSpinBox->setMaximum(5000000);
   iterUpdateSpinBox->setSingleStep(5);
   gridLayout->addWidget(iterUpdateSpinBox, ITER_UPDATE_ROW, 1,
                                  1, 3, Qt::AlignLeft);
   QObject::connect(iterUpdateSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
                    
   //
   // Image capture
   //
   gridLayout->addWidget(new QLabel("Image Capture"),
                         IMAGE_CAPTURE_ROW, 0, Qt::AlignLeft);
   imageCaptureTypeComboBox = new QComboBox;
   imageCaptureTypeComboBox->insertItem(PreferencesFile::IMAGE_CAPTURE_PIXMAP,
                                        "Use Pixmap");
   imageCaptureTypeComboBox->insertItem(PreferencesFile::IMAGE_CAPTURE_OPENGL_BUFFER,
                                        "Use OpenGL Buffer");
   gridLayout->addWidget(imageCaptureTypeComboBox, IMAGE_CAPTURE_ROW, 1,
                                  1, 3, Qt::AlignLeft);
   //
   // Debug enable
   //
   gridLayout->addWidget(new QLabel("Debug Enabled"),
                         DEBUG_ROW, 0, Qt::AlignLeft);
   debugOnCheckBox = new QCheckBox("");
   gridLayout->addWidget(debugOnCheckBox, DEBUG_ROW, 1, Qt::AlignLeft);
   QObject::connect(debugOnCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Debug Node
   //
   gridLayout->addWidget(new QLabel("Debug Node"),
                         DEBUG_NODE_ROW, 0, Qt::AlignLeft);
   debugNodeSpinBox = new QSpinBox;
   debugNodeSpinBox->setMinimum(-1);
   debugNodeSpinBox->setMaximum(std::numeric_limits<int>::max());
   debugNodeSpinBox->setSingleStep(1);
   gridLayout->addWidget(debugNodeSpinBox, DEBUG_NODE_ROW, 1, 1, 3, Qt::AlignLeft);
   QObject::connect(debugNodeSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // Display lists
   //
   gridLayout->addWidget(new QLabel("Display Lists Enabled"),
                         DISPLAY_LIST_ROW, 0, Qt::AlignLeft);
   displayListsOnCheckBox = new QCheckBox("");
   gridLayout->addWidget(displayListsOnCheckBox, DISPLAY_LIST_ROW, 1, Qt::AlignLeft);
   QObject::connect(displayListsOnCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(applyButtonSlot()));
   QPushButton* resetDisplayListsPushButton = new QPushButton("Reset Lists");                 
   resetDisplayListsPushButton->setAutoDefault(false);
   QObject::connect(resetDisplayListsPushButton, SIGNAL(clicked()),
                         this, SLOT(slotResetDisplayListsPushButton()));
   gridLayout->addWidget(resetDisplayListsPushButton, DISPLAY_LIST_ROW, 2,
                         1, 2);

   //
   // Web browser (do not show for Mac or Windows
   //
   webBrowserLineEdit = NULL;
#ifndef Q_OS_WIN32
#ifndef Q_OS_MACX
   gridLayout->addWidget(new QLabel("Web Browser"),
                         WEB_BROWSER_ROW, 0, Qt::AlignLeft);
   webBrowserLineEdit = new QLineEdit;
   gridLayout->addWidget(webBrowserLineEdit, WEB_BROWSER_ROW, 1,
                                  1, 3, Qt::AlignLeft);
#endif // Q_OS_MACX
#endif // Q_OS_WIN32

   //
   // Maximum number of threads
   //
   numberOfThreadsSpinBox = NULL;
   if (DebugControl::getDebugOn()) {
      const int numCPU = SystemUtilities::getNumberOfProcessors();
      QString threadStr("Number of Threads (");
      threadStr += QString::number(numCPU);
      threadStr += " CPUs)";
      gridLayout->addWidget(new QLabel(threadStr),
                            THREADS_ROW, 0, Qt::AlignLeft);
      numberOfThreadsSpinBox = new QSpinBox;
      numberOfThreadsSpinBox->setMinimum(0);
      numberOfThreadsSpinBox->setMaximum(numCPU * 4);
      numberOfThreadsSpinBox->setSingleStep(1);
      gridLayout->addWidget(numberOfThreadsSpinBox, THREADS_ROW, 1,
                            1, 3, Qt::AlignLeft);
      QObject::connect(numberOfThreadsSpinBox, SIGNAL(valueChanged(int)),
                       this, SLOT(applyButtonSlot()));
   }
                    
   //
   // Floating point precision for text files
   //
   gridLayout->addWidget(new QLabel("Text File Float Precision"),
                         FLOAT_ROW, 0, Qt::AlignLeft);
   floatPrecisionSpinBox = new QSpinBox;
   floatPrecisionSpinBox->setMinimum(1);
   floatPrecisionSpinBox->setMaximum(1024);
   floatPrecisionSpinBox->setSingleStep(1);
   gridLayout->addWidget(floatPrecisionSpinBox, FLOAT_ROW, 1, 1, 3, Qt::AlignLeft);
   QObject::connect(floatPrecisionSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   
   //
   // File write data type preference
   //
   gridLayout->addWidget(new QLabel("File Writing Preference"),
                         FILE_WRITE_ROW, 0, Qt::AlignLeft);
   fileWriteDataTypeComboBox = new QComboBox;
   fileWriteDataTypeComboBox->insertItem(AbstractFile::FILE_FORMAT_ASCII, "Text (Ascii)");
   fileWriteDataTypeComboBox->insertItem(AbstractFile::FILE_FORMAT_BINARY, "Binary");
   fileWriteDataTypeComboBox->insertItem(AbstractFile::FILE_FORMAT_XML, "XML");
   fileWriteDataTypeComboBox->insertItem(AbstractFile::FILE_FORMAT_XML_BASE64, "XML Base64");
   fileWriteDataTypeComboBox->insertItem(AbstractFile::FILE_FORMAT_XML_GZIP_BASE64, "XML GZip Base64");
   fileWriteDataTypeComboBox->insertItem(AbstractFile::FILE_FORMAT_OTHER, "No Preference");
   gridLayout->addWidget(fileWriteDataTypeComboBox, FILE_WRITE_ROW, 1,
                                  1, 3, Qt::AlignLeft);
   QObject::connect(fileWriteDataTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(applyButtonSlot()));
   fileWriteDataTypeComboBox->setToolTip(
                 "Changing this will not have an effect\n"
                 "until the next time Caret is started.");

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
   gridLayout->addWidget(randomSeedCheckBox, RANDOM_SEED_ROW, 0, Qt::AlignLeft);
   randomSeedCheckBox->setToolTip(randHelpText);
   randomSeedSpinBox = new QSpinBox;
   randomSeedSpinBox->setMinimum(1);
   randomSeedSpinBox->setMaximum(std::numeric_limits<int>::max());
   randomSeedSpinBox->setSingleStep(1);
   gridLayout->addWidget(randomSeedSpinBox, RANDOM_SEED_ROW, 1,
                                  1, 3, Qt::AlignLeft);
   QObject::connect(randomSeedSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(applyButtonSlot()));
   randomSeedSpinBox->setToolTip(randHelpText);
                    
   //
   // Speech enabled
   //
   bool haveSpeech = false;
#ifdef Q_OS_MACX
   haveSpeech = true;
#endif
#ifdef HAVE_MS_SPEECH
   haveSpeech = true;
#endif
   speechComboBox = NULL;
   if (haveSpeech) {
      gridLayout->addWidget(new QLabel("Speech"),
                           SPEECH_ROW, 0, Qt::AlignLeft);
      speechComboBox = new QComboBox;
      speechComboBox->insertItem(PreferencesFile::SPEECH_TYPE_OFF, "Off");
      speechComboBox->insertItem(PreferencesFile::SPEECH_TYPE_NORMAL, "Normal");
      speechComboBox->insertItem(PreferencesFile::SPEECH_TYPE_VERBOSE, "Verbose");
      gridLayout->addWidget(speechComboBox, SPEECH_ROW, 1, 1, 3, Qt::AlignLeft);
      QObject::connect(speechComboBox, SIGNAL(activated(int)),
                     this, SLOT(applyButtonSlot()));
   }
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);

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
   
   loadPreferences();
}

/**
 * load preferences into dialog
 */
void
GuiPreferencesDialog::loadPreferences()
{
   loadingPreferencesFile = true;
   
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
   lightPositionLineEditX->setText(QString("%1").arg(x, 0, 'f', 1));
   lightPositionLineEditY->setText(QString("%1").arg(y, 0, 'f', 1));
   lightPositionLineEditZ->setText(QString("%1").arg(z, 0, 'f', 1));
   
   mouseSpeedLineEdit->setText(QString("%1").arg(pf->getMouseSpeed(), 0, 'f', 3));
   
   iterUpdateSpinBox->blockSignals(true);
   iterUpdateSpinBox->setValue(pf->getIterativeUpdate());
   iterUpdateSpinBox->blockSignals(false);

   imageCaptureTypeComboBox->blockSignals(true);
   imageCaptureTypeComboBox->setCurrentIndex(static_cast<int>(pf->getImageCaptureType()));
   imageCaptureTypeComboBox->blockSignals(false);
   
   debugOnCheckBox->blockSignals(true);
   debugOnCheckBox->setChecked(pf->getDebugOn());
   debugOnCheckBox->blockSignals(false);
   
   debugNodeSpinBox->blockSignals(true);
   debugNodeSpinBox->setValue(pf->getDebugNodeNumber());
   debugNodeSpinBox->blockSignals(false);
   
   displayListsOnCheckBox->setChecked(pf->getDisplayListsEnabled());
   
   if (webBrowserLineEdit != NULL) {
      webBrowserLineEdit->setText(pf->getWebBrowser());
   }
   
   if (numberOfThreadsSpinBox != NULL) {
      numberOfThreadsSpinBox->blockSignals(true);
      numberOfThreadsSpinBox->setValue(pf->getMaximumNumberOfThreads());
      numberOfThreadsSpinBox->blockSignals(false);
   }
   
   if (speechComboBox != NULL) {
      speechComboBox->blockSignals(true);
      speechComboBox->setCurrentIndex(pf->getSpeechType());
      speechComboBox->blockSignals(false);
   }
   
   floatPrecisionSpinBox->blockSignals(true);
   floatPrecisionSpinBox->setValue(pf->getTextFileDigitsRightOfDecimal());
   floatPrecisionSpinBox->blockSignals(false);
   
   fileWriteDataTypeComboBox->blockSignals(true);
   fileWriteDataTypeComboBox->setCurrentIndex(pf->getPreferredWriteDataType());
   fileWriteDataTypeComboBox->blockSignals(false);
   
   sumsHostsLineEdit->setText(pf->getSumsDatabaseHosts());
   randomSeedCheckBox->setChecked(pf->getRandomSeedOverride());
   randomSeedSpinBox->setValue(pf->getRandomSeedOverrideValue());
   randomSeedSpinBox->setEnabled(randomSeedCheckBox->isChecked());
   
   loadingPreferencesFile = false;
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
   if (loadingPreferencesFile) {
      return;
   }
   
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
      GuiMessageBox::warning(this, "Color Warning",
                           "The foreground and background colors are the same.\n"
                           "This will prevent some items in the main window, such\n"
                           "as color palette numbers, from being seen.  You should\n"
                           "change the foreground and background colors to\n"
                           "contrasting colors.", "OK");
   }
   
   pf->setLightPosition(lightPositionLineEditX->text().toFloat(),
                        lightPositionLineEditY->text().toFloat(),
                        lightPositionLineEditZ->text().toFloat());
                        
   pf->setMouseSpeed(mouseSpeedLineEdit->text().toFloat());
   
   pf->setIterativeUpdate(iterUpdateSpinBox->value());
   
   pf->setImageCaptureType(
      static_cast<PreferencesFile::IMAGE_CAPTURE_TYPE>(
                          imageCaptureTypeComboBox->currentIndex()));
                          
   pf->setDebugOn(debugOnCheckBox->isChecked());
   pf->setDebugNodeNumber(debugNodeSpinBox->value());
   
   pf->setDisplayListsEnabled(displayListsOnCheckBox->isChecked());
   if (pf->getDisplayListsEnabled() == false) {
      theMainWindow->getBrainSet()->clearAllDisplayLists();
   }
   
   if (webBrowserLineEdit != NULL) {
      pf->setWebBrowser(webBrowserLineEdit->text());
   }
   
   if (numberOfThreadsSpinBox != NULL) {
      pf->setMaximumNumberOfThreads(numberOfThreadsSpinBox->value());
   }
   
   if (speechComboBox != NULL) {
      pf->setSpeechType(static_cast<PreferencesFile::SPEECH_TYPE>(speechComboBox->currentIndex()));
   }
   
   pf->setTextFileDigitsRightOfDecimal(floatPrecisionSpinBox->value());
   AbstractFile::setTextFileDigitsRightOfDecimal(pf->getTextFileDigitsRightOfDecimal());
   
   pf->setPreferredWriteDataType(static_cast<AbstractFile::FILE_FORMAT>(
                                        fileWriteDataTypeComboBox->currentIndex()));
   AbstractFile::setPreferredWriteType(pf->getPreferredWriteDataType()); 
        
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
      GuiMessageBox::warning(this, "Error Writing Preferences", msg, "OK");
   }
   
   GuiBrainModelOpenGL::updateAllGL(NULL);
}


