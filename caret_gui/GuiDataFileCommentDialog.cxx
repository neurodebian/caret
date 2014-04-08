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
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QTabWidget>
#include <QTextEdit>

#include "AreaColorFile.h"
#include "BorderFile.h"
#include "BorderColorFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "CellColorFile.h"
#include "ContourCellColorFile.h"
#include "FileUtilities.h"
#include "FociColorFile.h"
#include "GuiDataFileCommentDialog.h"
#include "GuiFilesModified.h"
#include "GuiHyperLinkTextBrowser.h"
#include "GuiMainWindow.h"
#include "NodeAttributeFile.h"
#include "PaintFile.h"
#include "ParamsFile.h"
#include "RgbPaintFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * The constructor for an AbstractFile subclass in memory.
 */
GuiDataFileCommentDialog::GuiDataFileCommentDialog(QWidget* parent, 
                                                   AbstractFile* af)
   : WuQDialog(parent)
{
   setAttribute(Qt::WA_DeleteOnClose);
   
   initialize();
   
   std::set<QString> names;
   PaintFile* paintFile = dynamic_cast<PaintFile*>(af);
   ParamsFile* paramsFile = dynamic_cast<ParamsFile*>(af);
   volumeFileInMemory = dynamic_cast<VolumeFile*>(af);
   if (paintFile != NULL) {
      const int num = paintFile->getNumberOfPaintNames();
      for (int i = 0; i < num; i++) {
         names.insert(paintFile->getPaintNameFromIndex(i));
      }
   }
   else if (paramsFile != NULL) {
      std::vector<QString> keys, values;
      paramsFile->getAllParameters(keys, values);
      const int num = static_cast<int>(keys.size());
      for (int i = 0; i < num; i++) {
         QString s(keys[i]);
         s.append("=");
         s.append(values[i]);
         names.insert(s);
      }
   }
   else if (volumeFileInMemory != NULL) {
      const int num = volumeFileInMemory->getNumberOfRegionNames();
      for (int i = 0; i < num; i++) {
         names.insert(volumeFileInMemory->getRegionNameFromIndex(i));
      }
   }
   

   //
   // Create the dialog
   //
   std::vector<QString> namesVector(names.begin(), names.end());
   if (volumeFileInMemory != NULL) {
      createDialog(DIALOG_MODE_VOLUME_FILE_IN_MEMORY, af->getFileName(), namesVector, false);
   }
   else {
      createDialog(DIALOG_MODE_ABSTRACT_FILE_IN_MEMORY, af->getFileName(), namesVector, false);
   }
   
   //
   // Add volume file label
   //
   if (volumeFileInMemory != NULL) {
      volumeFileLabelLineEdit->setText(volumeFileInMemory->getDescriptiveLabel());
   }
   
   //
   // Add comment to the text browser
   //
   dataFile = af;
   if (dataFile != NULL) {
      textBrowser->setText(dataFile->getFileComment());
      textEditor->setPlainText(dataFile->getFileComment());
      textEditor->document()->setModified(false);
      
      AbstractFile::AbstractFileHeaderContainer afhc = dataFile->getHeader();
      loadHeaderTagGrid(afhc);

   }
   else {
      textBrowser->setText("ERROR: Invalid file.");
      textEditor->setPlainText("ERROR: Invalid file.");
   }
   
}

/**
 * Constructor for an RGB Paint File Column Color in memory.
 */
GuiDataFileCommentDialog::GuiDataFileCommentDialog(QWidget* parent, 
                                                   RgbPaintFile* rgbPaintFileIn,
                                                   const int rgbPaintColumnIn,
                                                   const int rgbColorComponentIn)
   : WuQDialog(parent)
{
   setAttribute(Qt::WA_DeleteOnClose);
   
   initialize();
   
   rgbPaintFile = rgbPaintFileIn;
   rgbPaintColumn = rgbPaintColumnIn;
   rgbColorComponent = rgbColorComponentIn;
   
   dataFile = rgbPaintFile;
   
   //
   // Create the dialog
   //
   std::vector<QString> namesVector;
   createDialog(DIALOG_MODE_RGB_PAINT_FILE_IN_MEMORY, rgbPaintFile->getFileName(), namesVector, false);
   
   //
   // Add comment to the text browser
   //
   QString commentText;
   switch (rgbColorComponent) {
      case 0:
         commentText = rgbPaintFile->getCommentRed(rgbPaintColumn);
         break;
      case 1:
         commentText = rgbPaintFile->getCommentGreen(rgbPaintColumn);
         break;
      case 2:
         commentText = rgbPaintFile->getCommentBlue(rgbPaintColumn);
         break;
   }
   textBrowser->setText(commentText);
   textEditor->setPlainText(commentText);
   textEditor->document()->setModified(false);
}

/**
 * Constructor for a BrainModelBorderFileInfo class in memory.
 */
GuiDataFileCommentDialog::GuiDataFileCommentDialog(QWidget* parent,
                                                   BrainModelBorderFileInfo* bfi)
   : WuQDialog(parent)
{
   setAttribute(Qt::WA_DeleteOnClose);
   initialize();
   
   //
   // list the border names
   //
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   QString numBorderString("Number of Borders: ");
   numBorderString.append(StringUtilities::fromNumber(bmbs->getNumberOfBorders()));
   std::vector<QString> nameSet;
   bmbs->getAllBorderNames(nameSet, false);
   nameSet.insert(nameSet.begin(), numBorderString);
   
   //
   // Create the dialog
   //
   createDialog(DIALOG_MODE_BORDER_FILE_INFO_IN_MEMORY, bfi->getFileName(), nameSet, false);
   
   //
   // Add comment to the text browser
   //
   borderFileInfo = bfi;
   if (borderFileInfo != NULL) {
      textBrowser->setText(borderFileInfo->getFileComment());
      textEditor->setPlainText(borderFileInfo->getFileComment());
      textEditor->document()->setModified(false);
      loadHeaderTagGrid(borderFileInfo->getFileHeader());
   }
   else {
      borderFileInfo = NULL;
      textBrowser->setText("ERROR: Invalid border file.");
      textEditor->setPlainText("ERROR: Invalid border file.");
   }
}                               

/**
 * Constructor for a Node Attribute File Column subclass in memory.
 */
GuiDataFileCommentDialog::GuiDataFileCommentDialog(QWidget* parent, 
                                                   NodeAttributeFile* naf,
                                                   int nodeAttributeFileColumnIn)
   : WuQDialog(parent)
{
   setAttribute(Qt::WA_DeleteOnClose);
   initialize();

   //
   // Create the dialog
   //
   std::vector<QString> dummy;
   createDialog(DIALOG_MODE_NODE_ATTRIBUTE_FILE_COLUMN_IN_MEMORY,
                naf->getFileName(), dummy, false);
   
   //
   // Add comment to the text browser
   //
   nodeAttributeFile = naf;
   nodeAttributeFileColumn = nodeAttributeFileColumnIn;
   if (nodeAttributeFile != NULL) {
      if ((nodeAttributeFileColumn >= 0) &&
          (nodeAttributeFileColumn < naf->getNumberOfColumns())) {
         textBrowser->setText(nodeAttributeFile->getColumnComment(nodeAttributeFileColumn));
         textEditor->setPlainText(nodeAttributeFile->getColumnComment(nodeAttributeFileColumn));
         textEditor->document()->setModified(false);
         nodeAttributeColumnNameLineEdit->setText(
                   nodeAttributeFile->getColumnName(nodeAttributeFileColumn));
      }
      else {
         nodeAttributeFile = NULL;
         nodeAttributeFileColumn = -1;
         textBrowser->setText("ERROR: Invalid column.");
         textEditor->setPlainText("ERROR: Invalid column.");
      }
   }
   else {
      textBrowser->setText("ERROR: Invalid file.");
      textEditor->setPlainText("ERROR: Invalid file.");
   }
   
}

/**
 * Constructor for a Node Attribute File Column subclass in memory.
 */
GuiDataFileCommentDialog::GuiDataFileCommentDialog(QWidget* parent, 
                                                   GiftiNodeDataFile* naf,
                                                   int nodeAttributeFileColumnIn)
   : WuQDialog(parent)
{
   initialize();

   //
   // Create the dialog
   //
   std::vector<QString> dummy;
   createDialog(DIALOG_MODE_NODE_ATTRIBUTE_FILE_COLUMN_IN_MEMORY,
                naf->getFileName(), dummy, false);
   
   //
   // Add comment to the text browser
   //
   giftiNodeDataFile = naf;
   nodeAttributeFileColumn = nodeAttributeFileColumnIn;
   if (giftiNodeDataFile != NULL) {
      if ((nodeAttributeFileColumn >= 0) &&
          (nodeAttributeFileColumn < naf->getNumberOfColumns())) {
         textBrowser->setText(giftiNodeDataFile->getColumnComment(nodeAttributeFileColumn));
         textEditor->setPlainText(giftiNodeDataFile->getColumnComment(nodeAttributeFileColumn));
         textEditor->document()->setModified(false);
         nodeAttributeColumnNameLineEdit->setText(
                   giftiNodeDataFile->getColumnName(nodeAttributeFileColumn));
      }
      else {
         giftiNodeDataFile = NULL;
         nodeAttributeFileColumn = -1;
         textBrowser->setText("ERROR: Invalid column.");
         textEditor->setPlainText("ERROR: Invalid column.");
      }
   }
   else {
      textBrowser->setText("ERROR: Invalid file.");
      textEditor->setPlainText("ERROR: Invalid file.");
   }
   
}

/**
 * The constructor for an AbstractFile subclass in a file.
 */
GuiDataFileCommentDialog::GuiDataFileCommentDialog(QWidget* parent, 
                                                   const QString& fileName,
                                                   const bool volumeFileFlag)
   : WuQDialog(parent)
{
   initialize();

   //
   // Get the file's extension to see if it is a file that should have its data names listed
   //
   int numBorders = -1;
   std::set<QString> dataNames;
   QString ext(".");
   ext.append(FileUtilities::filenameExtension(fileName));
   if (ext.length() > 1) {
      if (ext == SpecFile::getAreaColorFileExtension()) {
         AreaColorFile cf;
         try {
            cf.readFile(fileName);
            const int num = cf.getNumberOfColors();
            for (int i = 0; i < num; i++) {
               const ColorFile::ColorStorage* cs = cf.getColor(i);
               dataNames.insert(cs->getName());
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getBorderColorFileExtension()) {
         BorderColorFile cf;
         try {
            cf.readFile(fileName);
            const int num = cf.getNumberOfColors();
            for (int i = 0; i < num; i++) {
               const ColorFile::ColorStorage* cs = cf.getColor(i);
               dataNames.insert(cs->getName());
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getCellColorFileExtension()) {
         CellColorFile cf;
         try {
            cf.readFile(fileName);
            const int num = cf.getNumberOfColors();
            for (int i = 0; i < num; i++) {
               const ColorFile::ColorStorage* cs = cf.getColor(i);
               dataNames.insert(cs->getName());
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getContourCellColorFileExtension()) {
         ContourCellColorFile cf;
         try {
            cf.readFile(fileName);
            const int num = cf.getNumberOfColors();
            for (int i = 0; i < num; i++) {
               const ColorFile::ColorStorage* cs = cf.getColor(i);
               dataNames.insert(cs->getName());
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getFociColorFileExtension()) {
         FociColorFile cf;
         try {
            cf.readFile(fileName);
            const int num = cf.getNumberOfColors();
            for (int i = 0; i < num; i++) {
               const ColorFile::ColorStorage* cs = cf.getColor(i);
               dataNames.insert(cs->getName());
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getBorderFileExtension()) {
         BorderFile bf;
         try {
            bf.readFile(fileName);
            numBorders = bf.getNumberOfBorders();
            for (int i = 0; i < numBorders; i++) {
               const Border* b = bf.getBorder(i);
               dataNames.insert(b->getName());
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getBorderProjectionFileExtension()) {
         BorderProjectionFile bpf;
         try {
            bpf.readFile(fileName);
            numBorders = bpf.getNumberOfBorderProjections();
            for (int i = 0; i < numBorders; i++) {
               const BorderProjection* bp = bpf.getBorderProjection(i);
               dataNames.insert(bp->getName());
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getPaintFileExtension()) {
         PaintFile pf;
         try {
            pf.readFile(fileName);
            const int num = pf.getNumberOfPaintNames();
            for (int i = 0; i < num; i++) {
               dataNames.insert(pf.getPaintNameFromIndex(i));
            }
         }
         catch (FileException& /*e*/) {
         }
      }
      else if (ext == SpecFile::getParamsFileExtension()) {
         ParamsFile pf;
         try {
            pf.readFile(fileName);
            std::vector<QString> keys, values;
            pf.getAllParameters(keys, values);
            const int num = static_cast<int>(keys.size());
            for (int i = 0; i < num; i++) {
               QString s(keys[i]);
               s.append("=");
               s.append(values[i]);
               dataNames.insert(s);
            }
         }
         catch (FileException& e) {
         }
      }
   }
   
   //
   // Create the dialog
   //
   std::vector<QString> dataVector(dataNames.begin(), dataNames.end());
   if (numBorders >= 0) {
      QString numString("Number of Borders: " );
      numString.append(StringUtilities::fromNumber(numBorders));
      dataVector.insert(dataVector.begin(), numString);
   }
   if (volumeFileFlag) {
      createDialog(DIALOG_MODE_VOLUME_FILE_NAME, fileName, dataVector, false);
   }
   else {
      createDialog(DIALOG_MODE_ABSTRACT_FILE_NAME, fileName, dataVector, false);
   }
   
   QFileInfo fileInfo(fileName);
   
   bool invalidFile = false;
   QString invalidFileMessage;
   
   //
   // Make sure the file exists
   //
   if (fileInfo.exists() == false) {
      invalidFile = true;
      invalidFileMessage = fileName;
      invalidFileMessage.append(" not found!");
   }
   //
   // Make sure the file is readable
   //
   else if (fileInfo.isReadable() == false) {
      invalidFile = true;
      invalidFileMessage = fileName;
      invalidFileMessage.append(" is not readable.\nCheck file permissions!");
   }
   else if (volumeFileFlag == false) {
      if (fileInfo.isWritable() == false) {
         invalidFile = true;
         invalidFileMessage = fileName;
         invalidFileMessage.append(" is not writable.");
      }
   }
   
   //
   // Add comment to the text browser
   //
   if (invalidFile == false) {
      if (volumeFileFlag) {
         try {
            volumeFileOnDisk = new VolumeFile;
            volumeFileOnDisk->readFile(fileName, VolumeFile::VOLUME_READ_HEADER_ONLY);
            textBrowser->setText(volumeFileOnDisk->getFileComment());
            textEditor->setPlainText(volumeFileOnDisk->getFileComment());
            textEditor->document()->setModified(false);
         }
         catch (FileException& e) {
            textBrowser->setText(e.whatQString());
            textEditor->setPlainText(e.whatQString());
            delete volumeFileOnDisk;
            volumeFileOnDisk = NULL;
         }
      }
      else {

/*         
         fileHeaderUpdater = new AbstractFileHeaderUpdater();
         try {
            fileHeaderUpdater->readFile(fileName);
            textBrowser->setText(fileHeaderUpdater->getFileComment());
            textEditor->setPlainText(fileHeaderUpdater->getFileComment());
            textEditor->document()->setModified(false);
            AbstractFile::AbstractFileHeaderContainer afhc = fileHeaderUpdater->getHeader();
            loadHeaderTagGrid(afhc);
         }
         catch (FileException& e) {
            delete fileHeaderUpdater;
            fileHeaderUpdater = NULL;
            textBrowser->setText(e.whatQString());
            textEditor->setPlainText(e.whatQString());
         }
*/
         QString msg;
         fileHeaderUpdater = AbstractFile::readAnySubClassDataFile(fileName, true, msg);
         if (fileHeaderUpdater != NULL) {
            textBrowser->setText(fileHeaderUpdater->getFileComment());
            textEditor->setPlainText(fileHeaderUpdater->getFileComment());
            textEditor->document()->setModified(false);
            AbstractFile::AbstractFileHeaderContainer afhc = fileHeaderUpdater->getHeader();
            loadHeaderTagGrid(afhc);
         }
      }
   }
   else {
      textBrowser->setText(invalidFileMessage);
      textEditor->setPlainText(invalidFileMessage);
   }
   
}

/**
 * initialize the dialog.
 */
void 
GuiDataFileCommentDialog::initialize()
{
   dataFile = NULL;
   fileHeaderUpdater = NULL;
   nodeAttributeFile = NULL;
   nodeAttributeFileColumn = -1;
   volumeFileOnDisk = NULL;
   volumeFileInMemory = NULL;
   volumeFileLabelLineEdit = NULL;
}      

/**
 * Create the dialog.
 */
void
GuiDataFileCommentDialog::createDialog(const DIALOG_MODE modeIn, const QString& fileName,
                                       const std::vector<QString>& namesList,
                                       const bool viewCommentOnly)
{
   dialogMode = modeIn;

   resize(400, 200);

   QString caption("Comment Editor - ");
   caption.append(FileUtilities::basename(fileName));
   setWindowTitle(caption);
   
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(5);
   
   nodeAttributeColumnNameLineEdit = NULL;
   volumeFileLabelLineEdit = NULL;
   switch (dialogMode) {
      case DIALOG_MODE_ABSTRACT_FILE_IN_MEMORY:
         break;
      case DIALOG_MODE_ABSTRACT_FILE_NAME:
         break;
      case DIALOG_MODE_VOLUME_FILE_NAME:
         break;
      case DIALOG_MODE_VOLUME_FILE_IN_MEMORY:
         {
            QHBoxLayout* labelLayout = new QHBoxLayout;
            labelLayout->setSpacing(3);
            rows->addLayout(labelLayout);
            labelLayout->addWidget(new QLabel("Label: "), 0);
            volumeFileLabelLineEdit = new QLineEdit;
            labelLayout->addWidget(volumeFileLabelLineEdit, 100);
         }
         break;
      case DIALOG_MODE_NODE_ATTRIBUTE_FILE_COLUMN_IN_MEMORY:
         {
            QHBoxLayout* columnNameLayout = new QHBoxLayout;
            columnNameLayout->setSpacing(3);
            rows->addLayout(columnNameLayout);
            columnNameLayout->addWidget(new QLabel("Column Name: "), 0);
            nodeAttributeColumnNameLineEdit = new QLineEdit;
            columnNameLayout->addWidget(nodeAttributeColumnNameLineEdit, 100);
         }
         break;
      case DIALOG_MODE_BORDER_FILE_INFO_IN_MEMORY:
         break;
      case DIALOG_MODE_RGB_PAINT_FILE_IN_MEMORY:
         break;
   }
   
   //
   // Tab widget to hold viewer and editor
   //
   tabWidget = new QTabWidget;
   rows->addWidget(tabWidget);
   QObject::connect(tabWidget, SIGNAL(currentChanged(int)),
                    this, SLOT(slotTabWidgetPageChanged(int)));
   
   //
   // Create the text browser that allows hyperlinks (read only)
   //
   textBrowser = new GuiHyperLinkTextBrowser(0);
   tabWidget->blockSignals(true);
   tabWidget->addTab(textBrowser, "View Comment");
   tabWidget->blockSignals(false);
   textBrowser->setReadOnly(true);
   QObject::connect(textBrowser, SIGNAL(keyPressed()),
                    this, SLOT(slotTextBrowserKeyPress()));
   
   //
   // Create the text editor for editing the comment
   //
   textEditor = new QTextEdit;
   tabWidget->blockSignals(true);
   tabWidget->addTab(textEditor, "Edit Comment");   
   tabWidget->blockSignals(false);
   if (viewCommentOnly) {
      tabWidget->setTabEnabled(tabWidget->indexOf(textEditor), false);
   }
   
   //
   // Create the grid for viewing header tags
   //
   headerTagGridLayout = NULL;
   switch (dialogMode) {
      case DIALOG_MODE_ABSTRACT_FILE_IN_MEMORY:
      case DIALOG_MODE_ABSTRACT_FILE_NAME:
      case DIALOG_MODE_VOLUME_FILE_NAME:
      case DIALOG_MODE_VOLUME_FILE_IN_MEMORY:
      case DIALOG_MODE_BORDER_FILE_INFO_IN_MEMORY:
         {
            QWidget* vbox = new QWidget;
            headerTagGridLayout = new QGridLayout(vbox);
            tabWidget->blockSignals(true);
            tabWidget->addTab(vbox, "Header");
            tabWidget->blockSignals(false);
            if (viewCommentOnly) {
               tabWidget->setTabEnabled(tabWidget->indexOf(vbox), false);
            }
         }
         break;
      case DIALOG_MODE_NODE_ATTRIBUTE_FILE_COLUMN_IN_MEMORY:
         break;
      case DIALOG_MODE_RGB_PAINT_FILE_IN_MEMORY:
         break;
   }
   
   if (namesList.empty() == false) {
      QListWidget* lb = new QListWidget;
      tabWidget->blockSignals(true);
      tabWidget->addTab(lb, "Names");
      tabWidget->blockSignals(false);
      for (std::vector<QString>::const_iterator iter = namesList.begin();
           iter != namesList.end(); iter++) {
         lb->addItem(*iter);
      }
   }
   
   //
   // Close Button
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setFixedSize(closeButton->sizeHint());
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseDialog()));
}

/**
 * The destructor.
 */
GuiDataFileCommentDialog::~GuiDataFileCommentDialog()
{
   if (fileHeaderUpdater != NULL) {
      delete fileHeaderUpdater;
   }
   if (volumeFileOnDisk != NULL) {
      delete volumeFileOnDisk;
      volumeFileOnDisk = NULL;
   }
}

/**
 * Load the header tag grid
 */
void
GuiDataFileCommentDialog::loadHeaderTagGrid(const AbstractFile::AbstractFileHeaderContainer& hc)
{
   if (headerTagGridLayout != NULL) {
      for (AbstractFile::AbstractFileHeaderContainer::const_iterator iter = hc.begin();
           iter != hc.end(); iter++) {
         if (iter->first != AbstractFile::getHeaderTagComment()) {
            const int rowNum = headerTagGridLayout->rowCount();
            headerTagGridLayout->addWidget(new QLabel(iter->first), rowNum, 0);
            headerTagGridLayout->addWidget(new QLabel(iter->second), rowNum, 1);
         }
      }
      //headerTagGridLayout->setFixedSize(headerTagGridLayout->sizeHint());
   }
}

/**
 * Called when edit button pressed.
 */
void
GuiDataFileCommentDialog::slotCloseDialog()
{
   switch (dialogMode) {
      case DIALOG_MODE_ABSTRACT_FILE_IN_MEMORY:
         if (dataFile != NULL) {
            if (textEditor->document()->isModified()) {
               dataFile->setFileComment(textEditor->toPlainText());
            }
         }
         break;
      case DIALOG_MODE_ABSTRACT_FILE_NAME:
         if (textEditor->document()->isModified()) {
            if (fileHeaderUpdater != NULL) {
               const QString name = fileHeaderUpdater->getFileName();
               QString msg;
               AbstractFile* file = AbstractFile::readAnySubClassDataFile(name, false, msg);
               if (file != NULL) {
                  file->setFileComment(textEditor->toPlainText());
                  file->setFileWriteType(file->getFileReadType());
                  try {
                     file->writeFile(name);
                  }
                  catch (FileException& e) {
                     throw FileException(name, "Unable to write");
                  }
               }
            }
         }
         break;
      case DIALOG_MODE_VOLUME_FILE_IN_MEMORY:
         if (volumeFileInMemory != NULL) {
            bool modified = false;
            if (textEditor->document()->isModified()) {
               volumeFileInMemory->setFileComment(textEditor->toPlainText());
               modified = true;
            }
            if (volumeFileLabelLineEdit->isModified()) {
               volumeFileInMemory->setDescriptiveLabel(volumeFileLabelLineEdit->text());
               modified = true;
            }
            if (modified) {
               GuiFilesModified fm;
               fm.setVolumeModified();
               theMainWindow->fileModificationUpdate(fm);
            }
         }
         break;
      case DIALOG_MODE_VOLUME_FILE_NAME:
         if (volumeFileOnDisk != NULL) {
            if (textEditor->document()->isModified()) {
               std::vector<VolumeFile*> volumes;
               try {
                  //volumeFileOnDisk->setFileComment(textEditor->toPlainText());
                  //volumeFileOnDisk->writeVolumeHeader(volumeFileOnDisk->getFileName());
                  
                  //VolumeFile vf;
                  //vf.readFile(volumeFileOnDisk->getFileName(),
                  //            VolumeFile::VOLUME_READ_SELECTION_ALL);
                  //vf.setFileComment(textEditor->toPlainText());
                  //vf.writeFile(volumeFileOnDisk->getFileName());
                  
                  //
                  // Read as multi-brick since it may be a multi-brick volume
                  //
                  VolumeFile::readFile(volumeFileOnDisk->getFileName(),
                                       VolumeFile::VOLUME_READ_SELECTION_ALL,
                                       volumes);
                  if (volumes.size() > 0) {
                     volumes[0]->setFileComment(textEditor->toPlainText());
                     VolumeFile::writeFile(volumeFileOnDisk->getFileName(),
                                           volumeFileOnDisk->getVolumeType(),
                                           volumeFileOnDisk->getVoxelDataType(),
                                           volumes,
                                           volumeFileOnDisk->getDataFileWasZipped());
                  }
                  else {
                     QString msg("No volume data read from ");
                     msg.append(FileUtilities::basename(volumeFileOnDisk->getFileName()));
                     QMessageBox::critical(this, "ERROR", msg);
                  }
               }
               catch (FileException& e) {
                  QString msg("Unable to save ");
                  msg.append(FileUtilities::basename(volumeFileOnDisk->getFileName()));
                  msg.append("\n");
                  msg.append(e.whatQString());
                  QMessageBox::critical(this, "ERROR", msg);
               }
               for (unsigned int i = 0; i < volumes.size(); i++) {
                  delete volumes[i];
               }
            }
         }
         break;
      case DIALOG_MODE_NODE_ATTRIBUTE_FILE_COLUMN_IN_MEMORY:
         if (nodeAttributeFile != NULL) {
            if (nodeAttributeColumnNameLineEdit->isModified()) {
               nodeAttributeFile->setColumnName(nodeAttributeFileColumn,
                                             nodeAttributeColumnNameLineEdit->text());
               GuiFilesModified fm;
               fm.setArealEstimationModified();
               fm.setLatLonModified();
               fm.setMetricModified();
               fm.setProbabilisticAtlasModified();
               fm.setRgbPaintModified();
               fm.setSectionModified();
               fm.setSurfaceShapeModified();
               fm.setTopographyModified();
               theMainWindow->fileModificationUpdate(fm);
            }
            if (textEditor->document()->isModified()) {
               nodeAttributeFile->setColumnComment(nodeAttributeFileColumn,
                                                textEditor->toPlainText());
            }
         }
         else if (giftiNodeDataFile != NULL) {
            if (nodeAttributeColumnNameLineEdit->isModified()) {
               giftiNodeDataFile->setColumnName(nodeAttributeFileColumn,
                                             nodeAttributeColumnNameLineEdit->text());
               GuiFilesModified fm;
               fm.setArealEstimationModified();
               fm.setLatLonModified();
               fm.setMetricModified();
               fm.setProbabilisticAtlasModified();
               fm.setRgbPaintModified();
               fm.setSectionModified();
               fm.setSurfaceShapeModified();
               fm.setTopographyModified();
               theMainWindow->fileModificationUpdate(fm);
            }
            if (textEditor->document()->isModified()) {
               giftiNodeDataFile->setColumnComment(nodeAttributeFileColumn,
                                                textEditor->toPlainText());
            }
         }
         break;
      case DIALOG_MODE_BORDER_FILE_INFO_IN_MEMORY:
         if (borderFileInfo != NULL) {
            if (textEditor->document()->isModified()) {
               borderFileInfo->setFileComment(textEditor->toPlainText());
            }
         }
         break;
      case DIALOG_MODE_RGB_PAINT_FILE_IN_MEMORY:
         if (rgbPaintFile != NULL) {
            if (rgbPaintColumn >= 0) {
               switch (rgbColorComponent) {
                  case 0:
                     rgbPaintFile->setCommentRed(rgbPaintColumn, textEditor->toPlainText());
                     break;
                  case 1:
                     rgbPaintFile->setCommentGreen(rgbPaintColumn, textEditor->toPlainText());
                     break;
                  case 2:
                     rgbPaintFile->setCommentBlue(rgbPaintColumn, textEditor->toPlainText());
                     break;
               }
            }
         }
         break;
   }
   QDialog::close();
}

/**
 * Called when edit button pressed.
 */
void
GuiDataFileCommentDialog::slotTabWidgetPageChanged(int indx)
{
   QWidget* page = tabWidget->widget(indx);
   if (page == textBrowser) {
      if (textEditor->document()->isModified()) {
         textBrowser->clear();
         textBrowser->setText(textEditor->toPlainText());
      }
   }
}

/**
 * Called if a key is pressed.
 */
void
GuiDataFileCommentDialog::slotTextBrowserKeyPress()
{
   QMessageBox::information(this, "Information", 
                            "Press the \"Edit Comment\" tab to edit the comment.");
}

