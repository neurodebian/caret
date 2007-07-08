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
#include <QClipboard>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTime>
#include <QToolTip>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiCaptureMainWindowImageDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiImageFormatComboBox.h"
#include "GuiMessageBox.h"
#include "ImageFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCaptureMainWindowImageDialog::GuiCaptureMainWindowImageDialog(QWidget* parent)
   : QtDialogNonModal(parent)
{
   setWindowTitle("Capture Main Window Image");
   
   //
   // Create capture group
   //
   QWidget* captureTypeGroupWidget = createCaptureGroup();

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
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = getDialogLayout();
   dialogLayout->addWidget(tabWidget);
   
   //
   // connect the buttons
   //
   QPushButton* captureButton = getApplyPushButton();
   captureButton->setText("Capture");
   QObject::connect(this, SIGNAL(signalApplyButtonPressed()),
                    this, SLOT(slotCaptureImage()));
   QObject::connect(this, SIGNAL(signalCloseButtonPressed()),
                    this, SLOT(closeDialog()));
   
}

/**
 * destructor.
 */
GuiCaptureMainWindowImageDialog::~GuiCaptureMainWindowImageDialog()
{
}

/**
 * create capture type group.
 */
QWidget* 
GuiCaptureMainWindowImageDialog::createCaptureGroup()
{
   //
   // image capture type full/selection
   //
   allCaptureRadioButton = new QRadioButton("All of Main Window Graphics Area");
   selectionImageRadioButton = new QRadioButton("Select Part of Graphics Area With Mouse");
   
   //
   // Tooltips for image capture type
   //
   allCaptureRadioButton->setToolTip(
                 "The captured image will contain all\n"
                 "of the main window graphics area.");
   selectionImageRadioButton->setToolTip(
                 "Capture a selection from the main window\n"
                 "graphics area.  Move the mouse to one corner\n"
                 "of the desired selection, hold down the left\n"
                 "mouse button, drag the mouse to the opposite\n"
                 "corner of the desired selection, and release\n"
                 "the left mouse button.");

   //
   // Button group so only one button selected at a time
   //
   QButtonGroup* captureButtonGroup = new QButtonGroup(this);
   QObject::connect(captureButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotImageCaptureTypeSelected()));
   captureButtonGroup->addButton(allCaptureRadioButton);
   captureButtonGroup->addButton(selectionImageRadioButton);
   
   //
   // Arrange image capture items
   //
   QGroupBox* capTypeGroupBox = new QGroupBox("Capture Type");
   QVBoxLayout* captureTypeLayout = new QVBoxLayout(capTypeGroupBox);
   captureTypeLayout->addWidget(allCaptureRadioButton);
   captureTypeLayout->addWidget(selectionImageRadioButton);
   
   return capTypeGroupBox;
}

/**
 * create image destination group.
 */
QWidget* 
GuiCaptureMainWindowImageDialog::createImageDestinationGroup()
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
   // Enabling of file save options
   //
   QObject::connect(saveToFileCheckBox, SIGNAL(toggled(bool)),
                    addToSpecFileCheckBox, SLOT(setEnabled(bool)));
   QObject::connect(saveToFileCheckBox, SIGNAL(toggled(bool)),
                    saveToFilePushButton, SLOT(setEnabled(bool)));
   QObject::connect(saveToFileCheckBox, SIGNAL(toggled(bool)),
                    saveFileNameLineEdit, SLOT(setEnabled(bool)));
   QObject::connect(saveToFileCheckBox, SIGNAL(toggled(bool)),
                    imageFormatLabel, SLOT(setEnabled(bool)));
   QObject::connect(saveToFileCheckBox, SIGNAL(toggled(bool)),
                    imageFormatComboBox, SLOT(setEnabled(bool)));
   saveToFileCheckBox->setChecked(false);
   addToSpecFileCheckBox->setEnabled(saveToFileCheckBox->isChecked());
   saveToFilePushButton->setEnabled(saveToFileCheckBox->isChecked());
   saveFileNameLineEdit->setEnabled(saveToFileCheckBox->isChecked());
   imageFormatLabel->setEnabled(saveToFileCheckBox->isChecked());
   imageFormatComboBox->setEnabled(saveToFileCheckBox->isChecked());
   
   //
   // Arrange file saving options
   //
   QGridLayout* fileSaveGridLayout = new QGridLayout;
   fileSaveGridLayout->setColumnMinimumWidth(0, 25);
   fileSaveGridLayout->addWidget(addToSpecFileCheckBox, 0, 1, 1, -1, Qt::AlignLeft);
   fileSaveGridLayout->addWidget(saveToFilePushButton, 1, 1, 1, 1);
   fileSaveGridLayout->addWidget(saveFileNameLineEdit, 1, 2, 1, 1);
   fileSaveGridLayout->addWidget(imageFormatLabel, 2, 1, 1, 1, Qt::AlignRight);
   fileSaveGridLayout->addWidget(imageFormatComboBox, 2, 2, 1, 1);
   
   //
   // Arrange image destination items
   //
   QGroupBox* imageDestGroupBox = new QGroupBox("Image Destination");
   QVBoxLayout* imageDestinationLayout = new QVBoxLayout(imageDestGroupBox);
   imageDestinationLayout->addWidget(addToLoadedImagesCheckBox);
   imageDestinationLayout->addWidget(copyToClipBoardCheckBox);
   imageDestinationLayout->addWidget(printImageCheckBox);
   imageDestinationLayout->addWidget(saveToFileCheckBox);
   imageDestinationLayout->addLayout(fileSaveGridLayout);
   
   return imageDestGroupBox;
}

/**
 * create standard views group.
 */
QWidget* 
GuiCaptureMainWindowImageDialog::createStandardViewsGroup()
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
GuiCaptureMainWindowImageDialog::slotCaptureImage()
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
GuiCaptureMainWindowImageDialog::captureNormalImage()
{
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   
   if ((allCaptureRadioButton->isChecked() == false) &&
       (selectionImageRadioButton->isChecked() == false)) {
      GuiMessageBox::critical(this, "ERROR", "No Capture Type is selected.", "OK");
      return;
   }
   
   if (selectionImageRadioButton->isChecked()) {
      if (openGL->getCaptureImageSubRegionValid() == false) {
         GuiMessageBox::critical(this, "ERROR", "No image subregion has been selected.", "OK");
         return;
      }
   }
   
   if ((copyToClipBoardCheckBox->isChecked() == false) &&
       (addToLoadedImagesCheckBox->isChecked() == false) &&
       (printImageCheckBox->isChecked() == false) &&
       (saveToFileCheckBox->isChecked() == false)) {
      GuiMessageBox::critical(this, "ERROR", "No Image Destination is selected.", "OK");
      return;
   }
   
   if (selectionImageRadioButton->isChecked()) {
      if (openGL->getCaptureImageSubRegionValid() == false) {
         GuiMessageBox::critical(this, "ERROR", "No image region is selected.\n"
                                               "To select, move the mouse to one corner\n"
                                               "of the desired selection, hold down the left\n"
                                               "mouse button, drag the mouse to the opposite\n"
                                               "corner of the desired selection, and release\n"
                                               "the left mouse button.",
                                             "OK");
         return;
      }
   }
   
   //
   // Get name for image file
   //
   const int imageQuality = 100;
   const QString format = imageFormatComboBox->getSelectedImageFormatName();
   QString name = saveFileNameLineEdit->text();
   if (name.isEmpty()) {
       if (saveToFileCheckBox->isChecked()) {
          GuiMessageBox::critical(this, "ERROR", "Image name is empty.", "OK");
       }
   }
   
   QString ext(".");
   ext.append(FileUtilities::filenameExtension(name));
   if (ext != imageFormatComboBox->getSelectedImageFormatExtension()) {
       name.append(imageFormatComboBox->getSelectedImageFormatExtension());
   }
   
   //
   // Capture the image
   //
   QImage image;
   if (allCaptureRadioButton->isChecked()) {
      openGL->captureImage(image);
   }
   else {
      openGL->captureImageSubRegion(image);
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
         GuiMessageBox::critical(theMainWindow, "ERROR", msg, "OK");
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
      if (image.save(name, format.toAscii().constData(), imageQuality) == false) {
         QString msg("Unable to save: ");
         msg.append(name);
         GuiMessageBox::critical(this, "ERROR", msg, "OK");
         return;
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
}

/**
 * capture images of standard view.
 */
void 
GuiCaptureMainWindowImageDialog::captureStandardViewImages()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      GuiMessageBox::critical(this, "ERROR", "You must have a surface in the Main Window.", "OK");
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
      GuiMessageBox::critical(this, "ERROR", msg2, "OK");
      return;
   }
}

/**
 * image file name selection.
 */
void 
GuiCaptureMainWindowImageDialog::slotImageFileNameDialog()
{
   QFileDialog fd(this);
   fd.setWindowTitle("Image File Name");
   fd.setModal(true);
   fd.setAcceptMode(QFileDialog::AcceptSave);
   fd.setFilters(imageFormatComboBox->getAllFileFilters());
   fd.setDirectory(QDir::currentPath());
   fd.setFileMode(QFileDialog::AnyFile);
   fd.selectFilter(imageFormatComboBox->getSelectedImageFormatFilter());
   fd.selectFilter("(*.jpg)");
   if (fd.exec() == QFileDialog::Accepted) {
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
GuiCaptureMainWindowImageDialog::closeDialog()
{
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   
   if (openGL->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION) {
      openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }

   QtDialogNonModal::close();
}

/**
 * show the dialog.
 */
void 
GuiCaptureMainWindowImageDialog::show()
{
   slotImageCaptureTypeSelected();

   QtDialogNonModal::show();
}

/**
 * image capture type selected.
 */
void 
GuiCaptureMainWindowImageDialog::slotImageCaptureTypeSelected()
{
   GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
   
   if (allCaptureRadioButton->isChecked()) {
      if (openGL->getMouseMode() == GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION) {
         openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
      }
   }
   else {
      openGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_IMAGE_SUBREGION);
   }
   GuiBrainModelOpenGL::updateAllGL();
}      
