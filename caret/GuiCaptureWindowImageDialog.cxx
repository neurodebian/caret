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

#include <limits>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTime>
#include <QToolTip>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelViewingWindow.h"
#include "GuiCaptureWindowImageDialog.h"
#include "GuiDisplayControlDialog.h"
#include "GuiDrawBorderDialog.h"
#include "GuiFilesModified.h"
#include "GuiIdentifyDialog.h"
#include "GuiImageResizeDialog.h"
#include "GuiMainWindow.h"
#include "GuiMapStereotaxicFocusDialog.h"
#include "GuiImageFormatComboBox.h"
#include "GuiRecordingDialog.h"
#include "GuiStudyMetaDataFileEditorDialog.h"
#include "GuiSurfaceRegionOfInterestDialog.h"
#include "GuiToolBar.h"
#include "ImageFile.h"
#include "WuQFileDialog.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCaptureWindowImageDialog::GuiCaptureWindowImageDialog(QWidget* parent)
   : QDialog(parent)
{
   setWindowTitle("Capture Image of Window");
   
   //
   // Create capture group
   //
   QWidget* captureTypeGroupWidget = createCaptureGroup();

   //
   // Capture image options group
   //
   QWidget* captureOptionsGroupWidget = captureOptionsGroup();
   
   //
   // Create the image destination group
   //
   QWidget*imageDestinationGroupWidget = createImageDestinationGroup();
   
   //
   // Widget for capture and dest groups
   //
   captureAndDestinationWidget = new QWidget;
   QVBoxLayout* captureAndDestinationLayout = new QVBoxLayout(captureAndDestinationWidget);
   captureAndDestinationLayout->addWidget(captureTypeGroupWidget);
   captureAndDestinationLayout->addWidget(captureOptionsGroupWidget);
   captureAndDestinationLayout->addWidget(imageDestinationGroupWidget);
   
   //
   // Create standard views widget
   //
   standardViewGroupWidget = createStandardViewsGroup();
   
   //
   // Tab widget for image dest vs std view
   //
   tabWidget = new QTabWidget;
   tabWidget->addTab(captureAndDestinationWidget, "Normal");
   tabWidget->addTab(standardViewGroupWidget, "Standard Views");
   
   //
   // Capture button
   //
   capturePushButton = new QPushButton("Capture");
   capturePushButton->setAutoDefault(capturePushButton);
   QObject::connect(capturePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCaptureImage()));
                    
   //
   // Help button
   //
   QPushButton* helpPushButton = new QPushButton("Help");
   helpPushButton->setAutoDefault(helpPushButton);
   QObject::connect(helpPushButton, SIGNAL(clicked()),
                    this, SLOT(slotHelpPushButton()));
                    
   //
   // close button
   //
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(closeDialog()));
                    
   //
   // Make buttons the same size
   //   
   WuQWidgetGroup* buttonWidgetGroup = new WuQWidgetGroup(this);
   buttonWidgetGroup->addWidget(capturePushButton);
   buttonWidgetGroup->addWidget(helpPushButton);
   buttonWidgetGroup->addWidget(closePushButton);
   buttonWidgetGroup->resizeAllToLargestSizeHint();
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(capturePushButton);
   buttonsLayout->addWidget(helpPushButton);
   buttonsLayout->addWidget(closePushButton);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(tabWidget);
   dialogLayout->addLayout(buttonsLayout);
}

/**
 * destructor.
 */
GuiCaptureWindowImageDialog::~GuiCaptureWindowImageDialog()
{
}

/**
 * called when help button pressed.
 */
void 
GuiCaptureWindowImageDialog::slotHelpPushButton()
{
   theMainWindow->showHelpViewerDialog("dialogs/capture_image_of_window_dialog.html");
}
      
/**
 * load window selection combo box items.
 */
void 
GuiCaptureWindowImageDialog::loadWindowSelectionComboBoxItems()
{
   windowSelectionComboBox->clear();
   
   windowSelectionComboBox->addItem("Main Window Graphics Area",
                                    static_cast<int>(WINDOW_MAIN_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Select Part of Main Window Graphics Area With Mouse",
                                    static_cast<int>(WINDOW_MAIN_SELECT_PART_OF_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 2 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_2_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 3 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_3_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 4 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_4_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 5 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_5_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 6 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_6_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 7 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_7_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 8 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_8_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 9 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_9_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Viewing Window 10 Graphics Area",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_10_GRAPHICS_AREA));
   windowSelectionComboBox->addItem("Main Window and Open Viewing Window Graphics Areas",
                                    static_cast<int>(WINDOW_MAIN_AND_OPEN_VIEWING_WINDOW_GRAPHICS));
   windowSelectionComboBox->addItem("Clipboard",
                                    static_cast<int>(WINDOW_CLIPBOARD));
   windowSelectionComboBox->addItem("Desktop",
                                    static_cast<int>(WINDOW_DESKTOP));
   windowSelectionComboBox->addItem("Display Control Window",
                                    static_cast<int>(WINDOW_DISPLAY_CONTROL_DIALOG));
   windowSelectionComboBox->addItem("Draw Border Window",
                                    static_cast<int>(WINDOW_DRAW_BORDER_DIALOG));
   windowSelectionComboBox->addItem("Identify Window",
                                    static_cast<int>(WINDOW_IDENTIFY_DIALOG));
   windowSelectionComboBox->addItem("Image Capture Window",
                                    static_cast<int>(WINDOW_IMAGE_CAPTURE_DIALOG));
   windowSelectionComboBox->addItem("Main Window Including Toolbar",
                                    static_cast<int>(WINDOW_MAIN_WINDOW));
   windowSelectionComboBox->addItem("Main Window Toolbar",
                                    static_cast<int>(WINDOW_MAIN_WINDOW_TOOLBAR));
   windowSelectionComboBox->addItem("Map Stereotaxic Focus Window",
                                    static_cast<int>(WINDOW_MAP_STEREOTAXIC_FOCUS_DIALOG));
   windowSelectionComboBox->addItem("Recording Window",
                                    static_cast<int>(WINDOW_RECORDING_DIALOG));
   windowSelectionComboBox->addItem("Study Metadata Editor Window",
                                    static_cast<int>(WINDOW_STUDY_METADATA_EDITOR_DIALOG));
   windowSelectionComboBox->addItem("Surface Region of Interest Window",
                                    static_cast<int>(WINDOW_SURFACE_REGION_OF_INTEREST_DIALOG));
   windowSelectionComboBox->addItem("Viewing Window 2 Including Toolbar",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_2));
   windowSelectionComboBox->addItem("Viewing Window 2 Toolbar",
                                    static_cast<int>(WINDOW_VIEWING_WINDOW_2_TOOLBAR));
}
      
/**
 * create capture type group.
 */
QWidget* 
GuiCaptureWindowImageDialog::createCaptureGroup()
{
   //
   // window selection combo box
   //
   windowSelectionComboBox = new QComboBox;
   loadWindowSelectionComboBoxItems();
   windowSelectionComboBox->setToolTip(
                 "Use this control to select the source of the\n"
                 "captured image.\n"
                 "\n"
                 "When selecting part of the main window, move\n"
                 "the mouse to one corner of the desired selection,\n"
                 "hold down the left mouse button, drag the mouse\n"
                 "to the opposite corner of the desired selection,\n"
                 "and release the left mouse button.\n"
                 "\n"
                 "Images of most Caret dialogs and windows can be\n"
                 "placed onto the computer's clipboard by using\n"
                 "the context menu which is available by clicking\n"
                 "the right mouse button (CONTROL-Mouse-Click on\n"
                 "Mac OSX).\n"
                 "\n"
                 "NOTE: If capturing an image of a window, such as\n"
                 "the Display Control or the Main Window and its\n"
                 "Toolbar, make sure no other windows are obscuring\n"
                 "the window being captured."
                 );
   QObject::connect(windowSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotWindowSelectionComboBox(int)));
      
   //
   // Wrapping for main and viewing window composite
   //
   QLabel* wrappingLabel = new QLabel("   Images Per Row ");
   multiImageWrappingSpinBox = new QSpinBox;
   multiImageWrappingSpinBox->setMinimum(1);
   multiImageWrappingSpinBox->setMaximum(10);
   multiImageWrappingSpinBox->setSingleStep(1);
   multiImageWrappingSpinBox->setValue(2);
   QHBoxLayout* wrappingLayout = new QHBoxLayout;
   wrappingLayout->addWidget(wrappingLabel);
   wrappingLayout->addWidget(multiImageWrappingSpinBox);
   wrappingLayout->addStretch();
   multiImageWrappingWidgetGroup = new WuQWidgetGroup(this);
   multiImageWrappingWidgetGroup->addWidget(wrappingLabel);
   multiImageWrappingWidgetGroup->addWidget(multiImageWrappingSpinBox);
   
   //
   // Arrange image capture items
   //
   QGroupBox* capTypeGroupBox = new QGroupBox("Capture Image Of");
   QVBoxLayout* captureTypeLayout = new QVBoxLayout(capTypeGroupBox);
   captureTypeLayout->addWidget(windowSelectionComboBox);
   captureTypeLayout->addLayout(wrappingLayout);
   
   return capTypeGroupBox;
}

/**
 * create capture options group.
 */
QWidget* 
GuiCaptureWindowImageDialog::captureOptionsGroup()
{
   //
   // Adjust captured image size check box
   //
   adjustCapturedImageSizeCheckBox = new QCheckBox("Popup Dialog to Adjust Captured Image Size");
   adjustCapturedImageSizeCheckBox->setToolTip(
                       "Allow adjustment of the captured\n"
                       "image prior to sending the image\n"
                       "to its destination.  If automatic \n"
                       "cropping is selected, this operation\n"
                       "is applied after the cropping.");
   adjustCapturedImageSizeCheckBox->setChecked(false);
   
   //
   // Auto crop check box
   //
   autoCropImageCheckBox = new QCheckBox("Automatically Crop Image");
   autoCropImageCheckBox->setToolTip(
                    "Remove background from around\n"
                    "the sides of the image.  This\n"
                    "option is processed only if the\n"
                    "catpured image is of the main\n"
                    "or viewing window's graphics.");
                    
   //
   // Auto crop margin spin box
   //
   autoCropImageMarginSpinBox = new QSpinBox;
   autoCropImageMarginSpinBox->setMinimum(0);
   autoCropImageMarginSpinBox->setMaximum(std::numeric_limits<int>::max());
   autoCropImageMarginSpinBox->setSingleStep(1);
   autoCropImageMarginSpinBox->setValue(10);
   QHBoxLayout* autoCropLayout = new QHBoxLayout;
   QLabel* maximumMarginLabel = new QLabel("      Margin"); 
   autoCropLayout->addWidget(maximumMarginLabel);
   autoCropLayout->addWidget(autoCropImageMarginSpinBox);
   autoCropLayout->addStretch();
   autoCropImageMarginSpinBox->setToolTip(
      "When autocropping, this number of pixels\n"
      "will remain on all sides of the object.");
      
   autoCropWidgetGroup = new WuQWidgetGroup(this);
   autoCropWidgetGroup->addWidget(autoCropImageCheckBox);
   autoCropWidgetGroup->addWidget(autoCropImageMarginSpinBox);
   autoCropWidgetGroup->addWidget(maximumMarginLabel);
    
   //
   // Enable margin spin box only if check box is checked
   //
   QObject::connect(autoCropImageCheckBox, SIGNAL(toggled(bool)),
                    maximumMarginLabel, SLOT(setEnabled(bool)));
   QObject::connect(autoCropImageCheckBox, SIGNAL(toggled(bool)),
                    autoCropImageMarginSpinBox, SLOT(setEnabled(bool)));
   autoCropImageCheckBox->setChecked(false);
   
   //
   // Arrange image capture items
   //
   QGroupBox* captureOptionsGroupBox = new QGroupBox("Capture Image Options");
   QVBoxLayout* captureOptionsLayout = new QVBoxLayout(captureOptionsGroupBox);
   captureOptionsLayout->addWidget(autoCropImageCheckBox);
   captureOptionsLayout->addLayout(autoCropLayout);
   captureOptionsLayout->addWidget(adjustCapturedImageSizeCheckBox);
   
   return captureOptionsGroupBox;
}
      
/**
 * create image destination group.
 */
QWidget* 
GuiCaptureWindowImageDialog::createImageDestinationGroup()
{
   //
   // image destination items
   //
   copyToClipBoardCheckBox = new QCheckBox("Copy to Clipboard");
   addToSpecFileCheckBox = new QCheckBox("Add to Spec File");
   addToSpecFileCheckBox->setChecked(true);
   addToLoadedImagesCheckBox = new QCheckBox("Add to Loaded Images");
   printImageCheckBox = new QCheckBox("Print Image");
   saveToFileCheckBox = new QCheckBox("Save to File");
   saveFileNameLineEdit = new QLineEdit;
   saveFileNameLineEdit->setText("capture.jpg");
   QPushButton* saveToFilePushButton = new QPushButton("Name...");
   saveToFilePushButton->setAutoDefault(false);
   QObject::connect(saveToFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotImageFileNameDialog()));
   QLabel* imageFormatLabel = new QLabel("Format");
   imageFormatComboBox = new GuiImageFormatComboBox(GuiImageFormatComboBox::IMAGE_MODE_SAVE,
                                                    GuiImageFormatComboBox::DISPLAY_IMAGE_FORMAT_NAMES);
   
   //
   // tooltips for image destination items
   //
   copyToClipBoardCheckBox->setToolTip(
                 "Copy the image to the computer system's\n"
                 "clipboard so that the image can be pasted\n"
                 "into another program such as Photoshop or Word.");
   addToLoadedImagesCheckBox->setToolTip(
                 "Copy the image to Caret's loaded images which\n"
                 "can be viewed by selecting Image Viewing Window\n"
                 "from the Window Menu.");
   printImageCheckBox->setToolTip(
                 "Send the image to a printer.");
   saveToFileCheckBox->setToolTip(
                 "Save the image to an image file.");
   
   //
   // Arrange file saving options
   //
   QGroupBox* fileSaveGroupBox = new QGroupBox("File Save Options");
   QGridLayout* fileSaveGridLayout = new QGridLayout(fileSaveGroupBox);
   fileSaveGridLayout->addWidget(addToSpecFileCheckBox, 0, 1, 1, -1, Qt::AlignLeft);
   fileSaveGridLayout->addWidget(saveToFilePushButton, 1, 1, 1, 1);
   fileSaveGridLayout->addWidget(saveFileNameLineEdit, 1, 2, 1, 1);
   fileSaveGridLayout->addWidget(imageFormatLabel, 2, 1, 1, 1, Qt::AlignRight);
   fileSaveGridLayout->addWidget(imageFormatComboBox, 2, 2, 1, 1);
   QHBoxLayout* fileSaveLayout = new QHBoxLayout;
   fileSaveLayout->addWidget(new QLabel("  "));
   fileSaveLayout->addWidget(fileSaveGroupBox);
   fileSaveLayout->addStretch();
   
   //
   // Enabling of file save options
   //
   QObject::connect(saveToFileCheckBox, SIGNAL(toggled(bool)),
                    fileSaveGroupBox, SLOT(setEnabled(bool)));
   saveToFileCheckBox->setChecked(false);
   fileSaveGroupBox->setEnabled(saveToFileCheckBox->isChecked());
   
   //
   // Arrange image destination items
   //
   QGroupBox* imageDestGroupBox = new QGroupBox("Image Destination");
   QVBoxLayout* imageDestinationLayout = new QVBoxLayout(imageDestGroupBox);
   imageDestinationLayout->addWidget(addToLoadedImagesCheckBox);
   imageDestinationLayout->addWidget(copyToClipBoardCheckBox);
   imageDestinationLayout->addWidget(printImageCheckBox);
   imageDestinationLayout->addWidget(saveToFileCheckBox);
   imageDestinationLayout->addLayout(fileSaveLayout);
   
   return imageDestGroupBox;
}

/**
 * create standard views group.
 */
QWidget* 
GuiCaptureWindowImageDialog::createStandardViewsGroup()
{
   QLabel* medialLabel = new QLabel("Medial");
   medialViewLineEdit = new QLineEdit;
   medialViewLineEdit->setText("medial.jpg");
   
   QLabel* lateralLabel = new QLabel("Lateral");
   lateralViewLineEdit = new QLineEdit;
   lateralViewLineEdit->setText("lateral.jpg");
   
   QLabel* posteriorLabel = new QLabel("Posterior");
   posteriorViewLineEdit = new QLineEdit;
   posteriorViewLineEdit->setText("posterior.jpg");
   
   QLabel* anteriorLabel = new QLabel("Anterior");
   anteriorViewLineEdit = new QLineEdit;
   anteriorViewLineEdit->setText("anterior.jpg");
   
   QLabel* ventralLabel = new QLabel("Ventral");
   ventralViewLineEdit = new QLineEdit;
   ventralViewLineEdit->setText("ventral.jpg");
   
   QLabel* dorsalLabel = new QLabel("Dorsal");
   dorsalViewLineEdit = new QLineEdit;
   dorsalViewLineEdit->setText("dorsal.jpg");
   
   QGroupBox* stdViewGroupBox = new QGroupBox("Standard Views");
   QGridLayout* stdViewGridLayout = new QGridLayout(stdViewGroupBox);
   stdViewGridLayout->addWidget(medialLabel, 0, 0);
   stdViewGridLayout->addWidget(medialViewLineEdit, 0, 1);
   stdViewGridLayout->addWidget(lateralLabel, 1, 0);
   stdViewGridLayout->addWidget(lateralViewLineEdit, 1, 1);
   stdViewGridLayout->addWidget(posteriorLabel, 2, 0);
   stdViewGridLayout->addWidget(posteriorViewLineEdit, 2, 1);
   stdViewGridLayout->addWidget(anteriorLabel, 3, 0);
   stdViewGridLayout->addWidget(anteriorViewLineEdit, 3, 1);
   stdViewGridLayout->addWidget(ventralLabel, 4, 0);
   stdViewGridLayout->addWidget(ventralViewLineEdit, 4, 1);
   stdViewGridLayout->addWidget(dorsalLabel, 5, 0);
   stdViewGridLayout->addWidget(dorsalViewLineEdit, 5, 1);
   stdViewGridLayout->setRowStretch(6, 100);
   return stdViewGroupBox;
}

/**
 * capture an image of the main window.
 */
void 
GuiCaptureWindowImageDialog::slotCaptureImage()
{
   if (tabWidget->currentWidget() == captureAndDestinationWidget) {
      captureNormalImage();
   }
   else {
      captureStandardViewImages();
   }
}

/**
 * capture a normal image.
 */
void 
GuiCaptureWindowImageDialog::captureNormalImage()
{
   if ((copyToClipBoardCheckBox->isChecked() == false) &&
       (addToLoadedImagesCheckBox->isChecked() == false) &&
       (printImageCheckBox->isChecked() == false) &&
       (saveToFileCheckBox->isChecked() == false)) {
      QMessageBox::critical(this, "ERROR", "No Image Destination is selected.");
      return;
   }
   
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   
   //
   // Get name for image file
   //
   const int imageQuality = 100;
   const QString format = imageFormatComboBox->getSelectedImageFormatName();
   QString name = saveFileNameLineEdit->text();
   if (name.isEmpty()) {
       if (saveToFileCheckBox->isChecked()) {
          QMessageBox::critical(this, "ERROR", "Image name is empty.");
          return;
       }
   }
   
   QString ext(".");
   ext.append(FileUtilities::filenameExtension(name));
   if (ext != imageFormatComboBox->getSelectedImageFormatExtension()) {
       name.append(imageFormatComboBox->getSelectedImageFormatExtension());
   }
   
   //
   // Get the window for selection
   //
   const int windowIndex = windowSelectionComboBox->currentIndex();
   const WINDOW window = static_cast<WINDOW>(
      windowSelectionComboBox->itemData(windowIndex).toInt());

   //
   // Auto cropping is allowed for only some image capture windows
   //
   bool autoCroppingEnabledFlag = false;
   
   //
   // Capture the image
   //
   QImage image;
   switch (window) {
      case WINDOW_MAIN_GRAPHICS_AREA:
         openGL->captureImage(image);
         autoCroppingEnabledFlag = true;
         break;
      case WINDOW_MAIN_SELECT_PART_OF_GRAPHICS_AREA:
         if (openGL->getCaptureImageSubRegionValid() == false) {
            QMessageBox::critical(this, "ERROR", "No image region is selected.\n"
                                                  "To select, move the mouse to one corner\n"
                                                  "of the desired selection, hold down the left\n"
                                                  "mouse button, drag the mouse to the opposite\n"
                                                  "corner of the desired selection, and release\n"
                                                  "the left mouse button.");
            return;
         }
         openGL->captureImageSubRegion(image);
         break;
      case WINDOW_VIEWING_WINDOW_2_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_3_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_4_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_5_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_6_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_7_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_8_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_9_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_10_GRAPHICS_AREA:
         {
            const int windowOffset = 
               static_cast<int>(window)
               - static_cast<int>(WINDOW_VIEWING_WINDOW_2_GRAPHICS_AREA);
               
            const BrainModel::BRAIN_MODEL_VIEW_NUMBER windowNumber =
               static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(
                  BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2
                  + windowOffset);
            if (captureViewingWindowGraphicsArea(windowNumber,
                                                 image) == false) {
               QMessageBox::critical(this, 
                                     "ERROR", 
                                     "Selected viewing window is not displayed.");
               return;
            }
         }
         autoCroppingEnabledFlag = true;
         break;
      case WINDOW_MAIN_AND_OPEN_VIEWING_WINDOW_GRAPHICS:
         captureMainAndViewingWindowGraphics(image,
                                             autoCropImageCheckBox->isChecked(),
                                             autoCropImageMarginSpinBox->value());
         break;
      case WINDOW_CLIPBOARD:
         {
            QClipboard* clipboard = QApplication::clipboard();
            image = clipboard->image();
            if (image.isNull()) {
               QMessageBox::critical(this, 
                                     "ERROR", 
                                     "There is no image on the clipboard.");
               return;
            }
         }
         break;
      case WINDOW_DESKTOP:
         {
            //
            // Hide this dialog and wait
            //
            this->hide();
            QApplication::processEvents();

            QTime timer;
            timer.start();
            const int maxTime = 1 * 1000;  // milliseconds
            while (timer.elapsed() < maxTime);
            
            //
            // Redraw all caret windows
            //
            GuiBrainModelOpenGL::updateAllGL();
            QApplication::processEvents();
            
            //
            // Wait a little bit
            //
            QTime timer2;
            timer2.start();
            while (timer2.elapsed() < maxTime);
            QApplication::processEvents();

            //
            // Capture events
            //
            QString message;
            if (captureImageOfWidget(QApplication::desktop(),
                                     image,
                                     true) == false) {
               message = "Desktop window is not displayed.";
            }
            
            this->show();
            
            if (message.isEmpty() == false) {
               QMessageBox::critical(this, 
                                     "ERROR", 
                                     message);
            }
         }
         break;
      case WINDOW_DISPLAY_CONTROL_DIALOG:
         if (captureImageOfWidget(theMainWindow->displayControlDialog,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Display Control window is not displayed.");
            return;
         }
         break;
      case WINDOW_DRAW_BORDER_DIALOG:
         if (captureImageOfWidget(theMainWindow->drawBorderDialog,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Draw Border window is not displayed.");
            return;
         }
         break;
      case WINDOW_IDENTIFY_DIALOG:
         if (captureImageOfWidget(theMainWindow->identifyDialog,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Identify window is not displayed.");
            return;
         }
         break;
      case WINDOW_IMAGE_CAPTURE_DIALOG:
         if (captureImageOfWidget(this,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Capture Image window is not displayed.");
            return;
         }
         break;
      case WINDOW_MAIN_WINDOW:
         if (captureImageOfWidget(theMainWindow,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Main Window is not displayed.");
            return;
         }
         break;
      case WINDOW_MAIN_WINDOW_TOOLBAR:
         if (captureImageOfWidget(theMainWindow->getToolBar(),
                                  image,
                                  false) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Main Window Toolbar is not displayed.");
            return;
         }
         break;
      case WINDOW_RECORDING_DIALOG:
         if (captureImageOfWidget(theMainWindow->recordingDialog,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Recording window is not displayed.");
            return;
         }
         break;
      case WINDOW_MAP_STEREOTAXIC_FOCUS_DIALOG:
         if (captureImageOfWidget(theMainWindow->mapStereotaxicFocusDialog,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Map Focus window is not displayed.");
            return;
         }
         break;
      case WINDOW_STUDY_METADATA_EDITOR_DIALOG:
         if (captureImageOfWidget(theMainWindow->studyMetaDataFileEditorDialog,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Study Metadata window is not displayed.");
            return;
         }
         break;
      case WINDOW_SURFACE_REGION_OF_INTEREST_DIALOG:
         if (captureImageOfWidget(theMainWindow->surfaceRegionOfInterestDialog,
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Surface ROI window is not displayed.");
            return;
         }
         break;
      case WINDOW_VIEWING_WINDOW_2:
         if (captureImageOfWidget(
            theMainWindow->modelWindow[BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2],
                                  image,
                                  true) == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  "Viewing window 2 is not displayed.");
            return;
         }
         break;
      case WINDOW_VIEWING_WINDOW_2_TOOLBAR:
         {
            QWidget* w = NULL;
            if (theMainWindow->modelWindow[BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2] != NULL) {
               w = theMainWindow->modelWindow[BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2]->getToolBar();
            }
            if (captureImageOfWidget(w,
                                     image,
                                     false) == false) {
               QMessageBox::critical(this, 
                                     "ERROR", 
                                     "Viewing Window 2 is not displayed.");
               return;
            }
         }
         break;
   }
   
   bool doCMYK = false;
   if (doCMYK) {
      for (int j = 0; j < image.height(); j++) {
         for (int i = 0; i < image.width(); i++) {
            QColor pixel = image.pixel(i, j);
            int r, g, b;
            pixel.getRgb(&r, &g, &b);
            int c = 255 - r;
            int m = 255 - g;
            int y = 255 - b;
//            pixel.setRgb(c, m, y);

            int k = std::min(c, std::min(m, y));
            c = c - k;
            m = m - k;
            y = y - k;
            pixel.setRgb(c, m, y, k);

            image.setPixel(i, j, pixel.rgba());
         }
      }
   }
   
   //
   // if auto cropping is selected
   //
   if (autoCropImageCheckBox->isChecked()) {
      if (autoCroppingEnabledFlag) {
         cropImage(image,
                   autoCropImageMarginSpinBox->value());
      }
   }
   
   //
   // Make adjustments to image size
   //
   if (adjustCapturedImageSizeCheckBox->isChecked()) {
      GuiImageResizeDialog resizeDialog(this,
                                        image);
      resizeDialog.exec();
   }
   
   //
   // image file might be used
   //
   GuiFilesModified fm;
   ImageFile* img = NULL;
   
   //
   // should image be copied to the clipboard
   //
   if (copyToClipBoardCheckBox->isChecked()) {
      QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
   }
   
   //
   // Should image be added to loaded images
   //
   if (addToLoadedImagesCheckBox->isChecked()) {
      img = new ImageFile(image);
      if (saveToFileCheckBox->isChecked()) {
         img->setFileName(name);
      }
      theMainWindow->getBrainSet(openGL->getModelViewNumber())->addImageFile(img);
      fm.setImagesModified();
   }
   
   //
   // Should the image be printed
   //
   if (printImageCheckBox->isChecked()) {
#ifdef Q_OS_WIN32
      QTime timer;
      timer.start();
#endif // Q_OS_WIN32
   
      QPrinter printer;
      QPrintDialog dialog(&printer, this);
      if (dialog.exec() == QDialog::Accepted) {
         QPainter painter(&printer);
         painter.drawImage(0, 0, image);
      }
#ifdef Q_OS_WIN32
      if (timer.elapsed() < 2000) {
         QString msg = "If you did not see the Print Dialog, then printing\n"
                       "will not work on your computer.  This is a problem\n"
                       "seen on Windows versions of Caret5 and there is no\n"
                       "solution to this problem.";
         QMessageBox::critical(theMainWindow, "ERROR", msg);
      }
#endif // Q_OS_WIN32
   }
   
   //
   // Should image be saved to a file
   //
   if (saveToFileCheckBox->isChecked()) {
      //
      // Save the image
      //
      QTime timer;
      timer.start();
      if (image.save(name, format.toAscii().constData(), imageQuality) == false) {
         QString msg("Unable to save: ");
         msg.append(name);
         QMessageBox::critical(this, "ERROR", msg);
         return;
      }
      const float timeToWriteFileInSeconds = static_cast<float>(timer.elapsed()) / 1000.0;
      if (DebugControl::getDebugOn()) {
         std::cout << "Time to write " << FileUtilities::basename(name).toAscii().constData()
                   << " was "
                   << timeToWriteFileInSeconds
                   << " seconds." << std::endl;
      }

      //
      // Since image is saved to file image file is unmodified
      //      
      if (img != NULL) {
         img->clearModified();
      }
      
      //
      // Update spec file
      //
      if (addToSpecFileCheckBox->isChecked()) {
         theMainWindow->getBrainSet(openGL->getModelViewNumber())->addToSpecFile(SpecFile::imageFileTag, name);
      }
   }
   
   theMainWindow->fileModificationUpdate(fm);
   
   QMessageBox::information(capturePushButton,
                            "Image Capture",
                            "Image captured successfully.");
}

/**
 * crop the image.
 */
void 
GuiCaptureWindowImageDialog::cropImage(QImage& image,
                                       const int margin)
{
   //
   // Get the background color
   //
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   unsigned char r, g, b;
   pf->getSurfaceBackgroundColor(r, g, b);
   const int backgroundColor[3] = { r, g, b };
   
   ImageFile::cropImageRemoveBackground(image,
                                        margin,
                                        backgroundColor);
/*
   //
   // Get cropping bounds
   //
   int leftTopRightBottom[4];
   ImageFile::findImageObject(image,
                              backgroundColor,
                              leftTopRightBottom);
   if (DebugControl::getDebugOn()) {
      std::cout 
         << "cropping: "
         << leftTopRightBottom[0]
         << " "        
         << leftTopRightBottom[1]
         << " "        
         << leftTopRightBottom[2]
         << " "        
         << leftTopRightBottom[3]
         << std::endl;
   }
    
   //
   // If cropping is valid
   //
   const int width = leftTopRightBottom[2] - leftTopRightBottom[0] + 1;
   const int height = leftTopRightBottom[3] - leftTopRightBottom[1] + 1;
   if ((width > 0) &&
       (height > 0)) {
      image = image.copy(leftTopRightBottom[0],
                         leftTopRightBottom[1],
                         width,
                         height);
   
      //
      // Process margin
      //
      if (margin > 0) {
         ImageFile::addMargin(image,
                              margin,
                              backgroundColor);

         //
         // Add margin
         //
         const int newWidth = width + margin * 2.0;
         const int newHeight = height + margin * 2.0;
         QRgb backgroundColorRGB = qRgba(r, g, b, 0);
         
         //
         // Insert image
         //
         ImageFile imageFile;
         imageFile.setImage(QImage(newWidth, newHeight, image.format()));
         imageFile.getImage()->fill(backgroundColorRGB);
         try {
            imageFile.insertImage(image, margin, margin);
            image = (*imageFile.getImage());
         }
         catch (FileException&) {
         }
      }
   }
*/
}
 
/**
 * capture image of main and viewing window graphics.
 */
bool 
GuiCaptureWindowImageDialog::captureMainAndViewingWindowGraphics(QImage& imageOut,
                                                         const bool cropImagesFlag,
                                                         const int cropMargin)
{
   std::vector<QImage> images;
   
   //
   // Capture images of all windows
   //
   for (int i = BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW; 
        i < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS;
        i++) {
      const BrainModel::BRAIN_MODEL_VIEW_NUMBER windowNumber =
               static_cast<BrainModel::BRAIN_MODEL_VIEW_NUMBER>(i);
      
      if (i == BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW) {
         QImage image;
         theMainWindow->getBrainModelOpenGL()->captureImage(image);
         images.push_back(image);
      }
      else {
         GuiBrainModelViewingWindow* viewWindow = theMainWindow->modelWindow[windowNumber];
         if (viewWindow != NULL) {
            GuiBrainModelOpenGL* openGL = viewWindow->getBrainModelOpenGL();
            QImage image;
            openGL->captureImage(image);
            images.push_back(image);
         }
      }
   }
   
   //
   // crop, if needed
   //
   const int numImages = static_cast<int>(images.size());
   for (int i = 0; i < numImages; i++) {
      //
      // Crop
      //
      if (cropImagesFlag) {
         cropImage(images[i],
                   cropMargin);
      }
   }
   
   //
   // Get the background color
   //
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   unsigned char r, g, b;
   pf->getSurfaceBackgroundColor(r, g, b);
   const int backgroundColor[3] = { r, g, b };

   //
   // Combine all images into single image
   //
   ImageFile::combinePreservingAspectAndFillIfNeeded(images,
                                           multiImageWrappingSpinBox->value(),
                                           backgroundColor,
                                           imageOut);

   return true;
}      

/**
 * capture image of a widget.
 */
bool
GuiCaptureWindowImageDialog::captureImageOfWidget(QWidget* w,
                                                  QImage& image,
                                       const bool captureWidgetsWindowFlag)
{
   if (w != NULL) {
      if (captureWidgetsWindowFlag) {
         //
         // Try to pop up over all other windows since
         // QPixmap::grabWindow() does a screen capture.
         // QPixmap::grabWindow() is used because it will
         // get the entire window, including the operating
         // system unique frame.  QPixmap::grabWidget()
         // gets only the widget without the window frame.
         //
         w->show();
         w->activateWindow();
         QApplication::processEvents();
         image = QPixmap::grabWindow(w->winId()).toImage();
         return true;
         //widget = w->window();
      }
      image = QPixmap::grabWidget(w).toImage();

      return true;
   }
   
   return false;
}

/**
 * capture an image of a viewing window (true if image valid).
 */
bool 
GuiCaptureWindowImageDialog::captureViewingWindowGraphicsArea(
                        const BrainModel::BRAIN_MODEL_VIEW_NUMBER windowNumber,
                        QImage& image)
{
   GuiBrainModelViewingWindow* viewWindow = theMainWindow->modelWindow[windowNumber];
   if (viewWindow != NULL) {
      GuiBrainModelOpenGL* openGL = viewWindow->getBrainModelOpenGL();
      openGL->captureImage(image);
      return true;
   }
   
   return false;
}

/**
 * capture images of standard view.
 */
void 
GuiCaptureWindowImageDialog::captureStandardViewImages()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      QMessageBox::critical(this, "ERROR", "You must have a surface in the Main Window.");
      return;
   }
   const int imageQuality = 100;
   const QString format("jpg");
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   QImage image;
   QString msg;
   
   //
   // Save view
   //
   bms->setToStandardView(BrainModelSurface::BRAIN_MODEL_VIEW_MAIN_WINDOW, 
                          BrainModelSurface::VIEW_MEDIAL);
   openGL->captureImage(image);
   if (image.save(medialViewLineEdit->text(), format.toAscii().constData(), imageQuality) == false) {
      msg.append(medialViewLineEdit->text());
   }
      
   //
   // Save view
   //
   bms->setToStandardView(BrainModelSurface::BRAIN_MODEL_VIEW_MAIN_WINDOW, 
                          BrainModelSurface::VIEW_LATERAL);
   openGL->captureImage(image);
   if (image.save(lateralViewLineEdit->text(), format.toAscii().constData(), imageQuality) == false) {
      msg.append(lateralViewLineEdit->text());
   }
      
   //
   // Save view
   //
   bms->setToStandardView(BrainModelSurface::BRAIN_MODEL_VIEW_MAIN_WINDOW, 
                          BrainModelSurface::VIEW_POSTERIOR);
   openGL->captureImage(image);
   if (image.save(posteriorViewLineEdit->text(), format.toAscii().constData(), imageQuality) == false) {
      msg.append(posteriorViewLineEdit->text());
   }
      
   //
   // Save view
   //
   bms->setToStandardView(BrainModelSurface::BRAIN_MODEL_VIEW_MAIN_WINDOW, 
                          BrainModelSurface::VIEW_ANTERIOR);
   openGL->captureImage(image);
   if (image.save(anteriorViewLineEdit->text(), format.toAscii().constData(), imageQuality) == false) {
      msg.append(anteriorViewLineEdit->text());
   }
      
   //
   // Save view
   //
   bms->setToStandardView(BrainModelSurface::BRAIN_MODEL_VIEW_MAIN_WINDOW, 
                          BrainModelSurface::VIEW_VENTRAL);
   openGL->captureImage(image);
   if (image.save(ventralViewLineEdit->text(), format.toAscii().constData(), imageQuality) == false) {
      msg.append(ventralViewLineEdit->text());
   }
      
   //
   // Save view
   //
   bms->setToStandardView(BrainModelSurface::BRAIN_MODEL_VIEW_MAIN_WINDOW, 
                          BrainModelSurface::VIEW_DORSAL);
   openGL->captureImage(image);
   if (image.save(dorsalViewLineEdit->text(), format.toAscii().constData(), imageQuality) == false) {
      msg.append(dorsalViewLineEdit->text());
   }
      
   
   QApplication::restoreOverrideCursor();

   if (msg.isEmpty() == false) {
      QString msg2("Unable to save images: \n");
      msg2.append(msg);
      QMessageBox::critical(this, "ERROR", msg2);
      return;
   }
}

/**
 * image file name selection.
 */
void 
GuiCaptureWindowImageDialog::slotImageFileNameDialog()
{
   WuQFileDialog fd(this);
   fd.setWindowTitle("Image File Name");
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptSave);
   fd.setFilters(imageFormatComboBox->getAllFileFilters());
   fd.setDirectory(QDir::currentPath());
   fd.setFileMode(WuQFileDialog::AnyFile);
   fd.selectFilter(imageFormatComboBox->getSelectedImageFormatFilter());
   fd.selectFilter("(*.jpg)");
   fd.selectFile(saveFileNameLineEdit->text());
   if (fd.exec() == WuQFileDialog::Accepted) {
      QString fn(fd.selectedFiles().at(0));
      if (FileUtilities::dirname(fn) == QDir::currentPath()) {
         saveFileNameLineEdit->setText(FileUtilities::basename(fn));
      }
      else {
         saveFileNameLineEdit->setText(fn);
      }
   }
}

/**
 * close the dialog.
 */
void 
GuiCaptureWindowImageDialog::closeDialog()
{
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   
   if (openGL->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION) {
      openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }

   QDialog::close();
}

/**
 * show the dialog.
 */
void 
GuiCaptureWindowImageDialog::show()
{
   slotWindowSelectionComboBox(windowSelectionComboBox->currentIndex());

   QDialog::show();
}

/**
 * image capture type selected.
 */
void 
GuiCaptureWindowImageDialog::slotWindowSelectionComboBox(int indx)
{
   const WINDOW window = static_cast<WINDOW>(
      windowSelectionComboBox->itemData(indx).toInt());

   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   
   if (window == WINDOW_MAIN_SELECT_PART_OF_GRAPHICS_AREA) {
      openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION);
   }
   else {
      if (openGL->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION) {
         openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
      }
   }
   
   //
   // Enable options based upon window being captured
   //
   bool automaticCroppingEnabledFlag = false;
   bool multiImageWrappingEnabledFlag = false;
   switch (window) {
      case WINDOW_MAIN_GRAPHICS_AREA:
         automaticCroppingEnabledFlag = true;
         break;
      case WINDOW_MAIN_SELECT_PART_OF_GRAPHICS_AREA:
         break;
      case WINDOW_VIEWING_WINDOW_2_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_3_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_4_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_5_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_6_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_7_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_8_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_9_GRAPHICS_AREA:
      case WINDOW_VIEWING_WINDOW_10_GRAPHICS_AREA:
         automaticCroppingEnabledFlag = true;
         break;
      case WINDOW_MAIN_AND_OPEN_VIEWING_WINDOW_GRAPHICS:
         automaticCroppingEnabledFlag = true;
         multiImageWrappingEnabledFlag = true;
         break;
      case WINDOW_CLIPBOARD:
         break;
      case WINDOW_DESKTOP:
         break;
      case WINDOW_DISPLAY_CONTROL_DIALOG:
         break;
      case WINDOW_DRAW_BORDER_DIALOG:
         break;
      case WINDOW_IDENTIFY_DIALOG:
         break;
      case WINDOW_IMAGE_CAPTURE_DIALOG:
         break;
      case WINDOW_MAIN_WINDOW:
         break;
      case WINDOW_MAIN_WINDOW_TOOLBAR:
         break;
      case WINDOW_MAP_STEREOTAXIC_FOCUS_DIALOG:
         break;
      case WINDOW_RECORDING_DIALOG:
         break;
      case WINDOW_STUDY_METADATA_EDITOR_DIALOG:
         break;
      case WINDOW_SURFACE_REGION_OF_INTEREST_DIALOG:
         break;
      case WINDOW_VIEWING_WINDOW_2:
         break;
      case WINDOW_VIEWING_WINDOW_2_TOOLBAR:
         break;
   }
   multiImageWrappingWidgetGroup->setEnabled(multiImageWrappingEnabledFlag);
   autoCropWidgetGroup->setEnabled(automaticCroppingEnabledFlag);
   autoCropImageCheckBox->setChecked(autoCropImageCheckBox->isChecked());
   
   GuiBrainModelOpenGL::updateAllGL();
}      
