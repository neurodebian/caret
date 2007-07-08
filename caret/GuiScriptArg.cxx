
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

#include <sstream>

#include <QComboBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QToolTip>
#include <QValidator>

#include "GuiScriptArg.h"
#include "FileUtilities.h"
#include "SpecFile.h"

/**
 * Constructor.
 */
GuiScriptArg::GuiScriptArg(QGridLayout* parentLayout,
                           const QString& labelString)
   : QObject(parentLayout->parentWidget())
{
   argLabel = labelString;
   
   QString lab(labelString);
   lab += " ";
   rowNumber = parentLayout->rowCount();
   parentLayout->addWidget(new QLabel(lab), rowNumber, 0);
}

/**
 * Destructor.
 */
GuiScriptArg::~GuiScriptArg()
{
}

/**
 * see if a value of an arg is valid.
 */
bool 
GuiScriptArg::getValueValid(QString& errorMessage) const
{
   const QString val = getValue();
   if (val.isEmpty()) {
      errorMessage = "Parameter \"";
      errorMessage.append(argLabel);
      errorMessage.append(" \"is missing.");
      return false;
   }
   
   return true;
}
      
//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptDirectoryArg::GuiScriptDirectoryArg(QGridLayout* parentLayout,
                                             const QString& labelString)
   : GuiScriptArg(parentLayout, labelString)
{
   QHBoxLayout* hbox = new QHBoxLayout;
   parentLayout->addLayout(hbox, rowNumber, 1);
   nameLineEdit = new QLineEdit;
   hbox->addWidget(nameLineEdit);
   QPushButton* fileSelectPushButton = new QPushButton("Select...");
   hbox->addWidget(fileSelectPushButton);
   fileSelectPushButton->setAutoDefault(false);
   fileSelectPushButton->setFixedSize(fileSelectPushButton->sizeHint());
   fileSelectPushButton->setToolTip( "Push to select name of new or existing directory.");
   QObject::connect(fileSelectPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDirectoryNamePushButton()));
}

/**
 * destructor.
 */
GuiScriptDirectoryArg::~GuiScriptDirectoryArg()
{
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptDirectoryArg::getValue() const
{
   QString s;
   getValue(s);
   return s;
}

/**
 * see if a value of an arg is valid.
 */
bool 
GuiScriptDirectoryArg::getValueValid(QString& errorMessage) const
{
   const QString val = getValue().trimmed();
   if (val.isEmpty()) {
      errorMessage = "Directory missing for " + argLabel;
      return false;
   }
   return true;
}

/**
 * get the argument's value.
 */
void 
GuiScriptDirectoryArg::getValue(QString& directoryNameOut) const
{
   directoryNameOut = nameLineEdit->text();
}

/**
 * set the value.
 */
void 
GuiScriptDirectoryArg::setValue(const QString& directoryNameIn)
{
   nameLineEdit->setText(directoryNameIn);
}

/**
 * called when directory name push button is selected.
 */
void 
GuiScriptDirectoryArg::slotDirectoryNamePushButton()
{
   QFileDialog fd(nameLineEdit);
   fd.setDirectory(QDir::currentPath());
   
   fd.setFilter("*");
   fd.setFileMode(QFileDialog::DirectoryOnly);
   
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         QString name(fd.selectedFiles().at(0));
         if (name.isEmpty() == false) {
            nameLineEdit->setText(name);
         }
      }
   }
}

//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptFileNameArg::GuiScriptFileNameArg(QGridLayout* parentLayout,
                                           const QString& labelString,
                                           const FILE_TYPE ft,
                                           const bool existingFilesOnlyFlagIn,
                                           const bool fileNameIsOptionalIn)
   : GuiScriptArg(parentLayout, labelString)
{
   fileType = ft;
   existingFilesOnlyFlag = existingFilesOnlyFlagIn;
   fileNameIsOptional = fileNameIsOptionalIn;
   
   QHBoxLayout* hbox = new QHBoxLayout;
   parentLayout->addLayout(hbox, rowNumber, 1);
   nameLineEdit = new QLineEdit;
   hbox->addWidget(nameLineEdit);
   if (existingFilesOnlyFlag) {
      nameLineEdit->setReadOnly(true);
   }
   QPushButton* fileSelectPushButton = new QPushButton("Select...");
   hbox->addWidget(fileSelectPushButton);
   fileSelectPushButton->setAutoDefault(false);
   fileSelectPushButton->setFixedSize(fileSelectPushButton->sizeHint());
   if (existingFilesOnlyFlag) {
      fileSelectPushButton->setToolTip("Push to select name of existing file.");
   }
   else {
      fileSelectPushButton->setToolTip("Push to select name of new or existing file.");
   }
   QObject::connect(fileSelectPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFileNamePushButton()));
                    
   labelLineEdit = NULL;
   switch (fileType) {
      case FILE_TYPE_GENERIC:
         break;
      case FILE_TYPE_LIMITS:
         break;
      case FILE_TYPE_SPEC:
         break;
      case FILE_TYPE_VECTOR:
         break;
      case FILE_TYPE_VOLUME_INPUT:
         break;
      case FILE_TYPE_VOLUME_OUTPUT:
         {
            rowNumber++;
            parentLayout->addWidget(new QLabel("Output Volume Label"), rowNumber, 0);
            labelLineEdit = new QLineEdit;
            parentLayout->addWidget(labelLineEdit, rowNumber, 1);
            //new QLabel("", parent);
         }
         break;
   }
}

/**
 * constructor.
 */
GuiScriptFileNameArg::GuiScriptFileNameArg(QGridLayout* parentLayout,
                           const QString& labelString,
                           const QString& fileFilterIn,
                           const bool existingFilesOnlyFlagIn,
                           const bool fileNameIsOptionalIn)
   : GuiScriptArg(parentLayout, labelString)
{
   fileType = FILE_TYPE_GENERIC;
   genericFileFilter = fileFilterIn;
   existingFilesOnlyFlag = existingFilesOnlyFlagIn;
   fileNameIsOptional = fileNameIsOptionalIn;
   
   QHBoxLayout* hbox = new QHBoxLayout;
   parentLayout->addLayout(hbox, rowNumber, 1);
   nameLineEdit = new QLineEdit;
   hbox->addWidget(nameLineEdit);
   if (existingFilesOnlyFlag) {
      nameLineEdit->setReadOnly(true);
   }
   QPushButton* fileSelectPushButton = new QPushButton("Select...");
   hbox->addWidget(fileSelectPushButton);
   fileSelectPushButton->setAutoDefault(false);
   fileSelectPushButton->setFixedSize(fileSelectPushButton->sizeHint());
   if (existingFilesOnlyFlag) {
      fileSelectPushButton->setToolTip("Push to select name of existing file.");
   }
   else {
      fileSelectPushButton->setToolTip("Push to select name of new or existing file.");
   }
   QObject::connect(fileSelectPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFileNamePushButton()));
                    
   labelLineEdit = NULL;
}

/**
 * destructor.
 */
GuiScriptFileNameArg::~GuiScriptFileNameArg()
{
}

/**
 * set the value.
 */
void 
GuiScriptFileNameArg::setValue(const QString& filenameIn, const QString& labelNameIn)
{
   nameLineEdit->setText(filenameIn);
   labelLineEdit->setText(labelNameIn);
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptFileNameArg::getValue() const
{
   QString s;
   getValue(s);
   return s;
}

/**
 * get the argument's value.
 */
void 
GuiScriptFileNameArg::getValue(QString& fileNameOut) const
{
   fileNameOut = nameLineEdit->text();
   if (fileNameOut.isEmpty() == false) {
      if (labelLineEdit != NULL) {
         const QString s(labelLineEdit->text().trimmed());
         if (s.isEmpty() == false) {
            fileNameOut = s;
            fileNameOut.append(":::");
            fileNameOut.append(nameLineEdit->text());
         }
      }
   }
   else if (fileNameIsOptional) {
      fileNameOut = "";
   }
   else {
      fileNameOut = "\"\"";  // two double quotes is empty file
   }
}

/**
 * see if a value of an arg is valid.
 */
bool 
GuiScriptFileNameArg::getValueValid(QString& errorMessage) const
{
   const QString val = getValue().trimmed();
   if (val.isEmpty() || (val == "\"\"")) {
      if (fileNameIsOptional) {
         return true;
      }
      errorMessage = "Parameter \"";
      errorMessage.append(argLabel);
      errorMessage.append(" \" is missing.");
      return false;
   }
   
   QString ext(".");
   ext.append(FileUtilities::filenameExtension(val));
   
   switch (fileType) {
      case FILE_TYPE_GENERIC:
         break;
      case FILE_TYPE_LIMITS:
         if (ext != SpecFile::getLimitsFileExtension()) {
            errorMessage = "Parameter \"";
            errorMessage.append(argLabel);
            errorMessage.append("\" must have file extension \"");
            errorMessage.append(SpecFile::getLimitsFileExtension());
            errorMessage.append("\".");
            return false;
         }
         break;
      case FILE_TYPE_SPEC:
         if (ext != SpecFile::getSpecFileExtension()) {
            errorMessage = "Parameter \"";
            errorMessage.append(argLabel);
            errorMessage.append("\" must have file extension \"");
            errorMessage.append(SpecFile::getSpecFileExtension());
            errorMessage.append("\".");
            return false;
         }
         break;
      case FILE_TYPE_VECTOR:
         if (ext != SpecFile::getVectorFileExtension()) {
            errorMessage = "Parameter \"";
            errorMessage.append(argLabel);
            errorMessage.append("\" must have file extension \"");
            errorMessage.append(SpecFile::getVectorFileExtension());
            errorMessage.append("\".");
            return false;
         }
         break;
      case FILE_TYPE_VOLUME_INPUT:
      case FILE_TYPE_VOLUME_OUTPUT:
         if ((ext != SpecFile::getAfniVolumeFileExtension()) &&
             (ext != SpecFile::getAnalyzeVolumeFileExtension()) &&
             (ext != SpecFile::getMincVolumeFileExtension()) &&
             (ext != SpecFile::getNiftiVolumeFileExtension()) &&
             (val.endsWith(SpecFile::getNiftiGzipVolumeFileExtension()) == false) &&
             (ext != SpecFile::getWustlVolumeFileExtension())) {
            errorMessage = "Parameter \"";
            errorMessage.append(argLabel);
            errorMessage.append("\" must have one of the file extension:\n");
            errorMessage.append("   ");
            errorMessage.append(SpecFile::getAfniVolumeFileExtension());
            errorMessage.append("\n");
            errorMessage.append("   ");
            errorMessage.append(SpecFile::getAnalyzeVolumeFileExtension());
            errorMessage.append("\n");
            errorMessage.append("   ");
            errorMessage.append(SpecFile::getMincVolumeFileExtension());
            errorMessage.append("\n");
            errorMessage.append("   ");
            errorMessage.append(SpecFile::getNiftiVolumeFileExtension());
            errorMessage.append("\n");
            errorMessage.append("   ");
            errorMessage.append(SpecFile::getNiftiGzipVolumeFileExtension());
            errorMessage.append("\n");
            errorMessage.append("   ");
            errorMessage.append(SpecFile::getWustlVolumeFileExtension());
            errorMessage.append("\n");
            return false;
         }
         break;
   }
   
   if (labelLineEdit != NULL) {
      const QString s(labelLineEdit->text().trimmed());
      if (s.indexOf(' ') != -1) {
         errorMessage = "The label \"";
         errorMessage.append(s);
         errorMessage.append("\" may not contain spaces.");
         return false;
      }
   }
   return true;
}
      
/**
 * called when file name push button is selected.
 */
void 
GuiScriptFileNameArg::slotFileNamePushButton()
{
   QFileDialog fd(nameLineEdit);
   fd.setDirectory(QDir::currentPath());
   std::ostringstream str;
   
   switch (fileType) {
      case FILE_TYPE_GENERIC:
         str << genericFileFilter.toAscii().constData();
         break;
      case FILE_TYPE_LIMITS:
         str << "Limits Files ("
             << "*" << SpecFile::getLimitsFileExtension().toAscii().constData() << ")";
         break;
      case FILE_TYPE_SPEC:
         str << "Spec Files ("
             << "*" << SpecFile::getSpecFileExtension().toAscii().constData() << ")";
         break;
      case FILE_TYPE_VECTOR:
         str << "Vector Files ("
             << "*" << SpecFile::getVectorFileExtension().toAscii().constData() << ")";
         break;
      case FILE_TYPE_VOLUME_INPUT:
      case FILE_TYPE_VOLUME_OUTPUT:
         str << "Volume Files ("
             << "*" << SpecFile::getAfniVolumeFileExtension().toAscii().constData() << " "
             << "*" << SpecFile::getAnalyzeVolumeFileExtension().toAscii().constData() << " "
             << "*" << SpecFile::getMincVolumeFileExtension().toAscii().constData() << " "
             << "*" << SpecFile::getNiftiVolumeFileExtension().toAscii().constData() << " "
             << "*" << SpecFile::getNiftiGzipVolumeFileExtension().toAscii().constData() << " "
             << "*" << SpecFile::getWustlVolumeFileExtension().toAscii().constData() << ")";
         break;
   }
   
   fd.setFilter(str.str().c_str());
   if (existingFilesOnlyFlag) {
      fd.setFileMode(QFileDialog::ExistingFile);
   }
   else {
      fd.setFileMode(QFileDialog::AnyFile);
   }
   
   if (fd.exec() == QDialog::Accepted) {
      QString name(fd.selectedFiles().at(0));
      if (name.isEmpty() == false) {
         const QString filePath = FileUtilities::dirname(name);
         if (filePath.isEmpty() == false) {
            if (filePath == QDir::currentPath()) {
               name = FileUtilities::basename(name);
            }
         }
         nameLineEdit->setText(name);
      }
   }
}
      
//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptFloatArg::GuiScriptFloatArg(QGridLayout* parentLayout,
                  const QString& labelString)
   : GuiScriptArg(parentLayout, labelString)
{
   valueLineEdit = new QLineEdit;
   valueLineEdit->setText("0.0");
   valueLineEdit->setValidator(new QDoubleValidator(valueLineEdit));
   valueLineEdit->setToolTip("Floating point value.");
   parentLayout->addWidget(valueLineEdit, rowNumber, 1);
}

/**
 * destructor.
 */
GuiScriptFloatArg::~GuiScriptFloatArg()
{
}

/**
 * set the value of the arg.
 */
void 
GuiScriptFloatArg::setValue(const float f)
{
   valueLineEdit->setText(QString::number(f));
}
      
/**
 * get the value of the arg as text.
 */
QString 
GuiScriptFloatArg::getValue() const
{
   const QString s = valueLineEdit->text();
   return s;
}

//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptIntArg::GuiScriptIntArg(QGridLayout* parentLayout,
                  const QString& labelString)
   : GuiScriptArg(parentLayout, labelString)
{
   valueLineEdit = new QLineEdit;
   valueLineEdit->setText("0");
   valueLineEdit->setValidator(new QIntValidator(valueLineEdit));
   valueLineEdit->setToolTip("Integer value.");
   parentLayout->addWidget(valueLineEdit, rowNumber, 1);
}

/**
 * destructor.
 */
GuiScriptIntArg::~GuiScriptIntArg()
{
}

/**
 * set the value.
 */
void 
GuiScriptIntArg::setValue(const int v)
{
   valueLineEdit->setText(QString::number(v));
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptIntArg::getValue() const
{
   const QString s = valueLineEdit->text();
   return s;
}

//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptBoolArg::GuiScriptBoolArg(QGridLayout* parentLayout,
                  const QString& labelString)
   : GuiScriptArg(parentLayout, labelString)
{
   valueComboBox = new QComboBox;
   valueComboBox->addItem("false");
   valueComboBox->addItem("true");
   parentLayout->addWidget(valueComboBox, rowNumber, 1);
}

/**
 * destructor.
 */
GuiScriptBoolArg::~GuiScriptBoolArg()
{
}

/**
 * set the value.
 */
void 
GuiScriptBoolArg::setValue(const bool b)
{
   if (b) valueComboBox->setCurrentIndex(1);
   else   valueComboBox->setCurrentIndex(0);
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptBoolArg::getValue() const
{
   QString s = "false";
   if (valueComboBox->currentIndex() != 0) {
      s = "true";
   }
   return s;
}

//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptSeedArg::GuiScriptSeedArg(QGridLayout* parentLayout,
                  const QString& labelString)
   : GuiScriptArg(parentLayout, labelString)
{
   QHBoxLayout* hbox = new QHBoxLayout;
   parentLayout->addLayout(hbox, rowNumber, 1);

   const int maxWidth = 80;
   
   valueLineEditX = new QLineEdit;
   hbox->addWidget(valueLineEditX);
   valueLineEditX->setText("0.0");
   valueLineEditX->setValidator(new QIntValidator(valueLineEditX));
   valueLineEditX->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditX, 0);
   valueLineEditX->setToolTip(
                 "Seed X");
   
   valueLineEditY = new QLineEdit;
   hbox->addWidget(valueLineEditY);
   valueLineEditY->setText("0.0");
   valueLineEditY->setValidator(new QIntValidator(valueLineEditY));
   valueLineEditY->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditY, 0);
   valueLineEditY->setToolTip(
                 "Seed Y");

   valueLineEditZ = new QLineEdit;
   hbox->addWidget(valueLineEditZ);
   valueLineEditZ->setText("0.0");
   valueLineEditZ->setValidator(new QIntValidator(valueLineEditZ));
   valueLineEditZ->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditZ, 0);
   valueLineEditZ->setToolTip(
                 "Seed Z");
   
   QLabel* dummy = new QLabel(" ");
   hbox->addWidget(dummy);
   hbox->setStretchFactor(dummy, 1000);
}

/**
 * destructor.
 */
GuiScriptSeedArg::~GuiScriptSeedArg()
{
}

/**
 * set the seed.
 */
void 
GuiScriptSeedArg::setValue(const int x, const int y, const int z)
{
   valueLineEditX->setText(QString::number(x));
   valueLineEditY->setText(QString::number(y));
   valueLineEditZ->setText(QString::number(z));
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptSeedArg::getValue() const
{
   std::ostringstream str;
   str << valueLineEditX->text().toAscii().constData() << " "
       << valueLineEditY->text().toAscii().constData() << " "
       << valueLineEditZ->text().toAscii().constData();
   const QString s = str.str().c_str();
   return s;
}
//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptExtentArg::GuiScriptExtentArg(QGridLayout* parentLayout,
                  const QString& labelString)
   : GuiScriptArg(parentLayout, labelString)
{
   QHBoxLayout* hbox = new QHBoxLayout;
   parentLayout->addLayout(hbox, rowNumber, 1);
   
   const int maxWidth = 80;
   
   valueLineEditMinX = new QLineEdit;
   hbox->addWidget(valueLineEditMinX);
   valueLineEditMinX->setText("0.0");
   valueLineEditMinX->setValidator(new QIntValidator(valueLineEditMinX));
   valueLineEditMinX->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditMinX, 0);
   valueLineEditMinX->setToolTip(
                 "Minimum X");
   
   valueLineEditMaxX = new QLineEdit;
   hbox->addWidget(valueLineEditMaxX);
   valueLineEditMaxX->setText("0.0");
   valueLineEditMaxX->setValidator(new QIntValidator(valueLineEditMaxX));
   valueLineEditMaxX->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditMaxX, 0);
   valueLineEditMaxX->setToolTip(
                 "Maximum X");
   
   valueLineEditMinY = new QLineEdit;
   hbox->addWidget(valueLineEditMinY);
   valueLineEditMinY->setText("0.0");
   valueLineEditMinY->setValidator(new QIntValidator(valueLineEditMinY));
   valueLineEditMinY->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditMinY, 0);
   valueLineEditMinY->setToolTip(
                 "Minimum Y");
   
   valueLineEditMaxY = new QLineEdit;
   hbox->addWidget(valueLineEditMaxY);
   valueLineEditMaxY->setText("0.0");
   valueLineEditMaxY->setValidator(new QIntValidator(valueLineEditMaxY));
   valueLineEditMaxY->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditMaxY, 0);
   valueLineEditMaxY->setToolTip(
                 "Maximum Y");
   
   valueLineEditMinZ = new QLineEdit;
   hbox->addWidget(valueLineEditMinZ);
   valueLineEditMinZ->setText("0.0");
   valueLineEditMinZ->setValidator(new QIntValidator(valueLineEditMinZ));
   valueLineEditMinZ->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditMinZ, 0);
   valueLineEditMinZ->setToolTip(
                 "Minimum Z");
   
   valueLineEditMaxZ = new QLineEdit;
   hbox->addWidget(valueLineEditMaxZ);
   valueLineEditMaxZ->setText("0.0");
   valueLineEditMaxZ->setValidator(new QIntValidator(valueLineEditMaxZ));   
   valueLineEditMaxZ->setMaximumWidth(maxWidth);
   hbox->setStretchFactor(valueLineEditMaxZ, 0);
   valueLineEditMaxZ->setToolTip(
                 "Maximum Z");

   QLabel* dummy = new QLabel(" ");   
   hbox->addWidget(dummy);
   hbox->setStretchFactor(dummy, 1000);
}

/**
 * destructor.
 */
GuiScriptExtentArg::~GuiScriptExtentArg()
{
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptExtentArg::getValue() const
{
   std::ostringstream str;
   str << valueLineEditMinX->text().toAscii().constData() << " "
       << valueLineEditMaxX->text().toAscii().constData() << " "
       << valueLineEditMinY->text().toAscii().constData() << " "
       << valueLineEditMaxY->text().toAscii().constData() << " "
       << valueLineEditMinZ->text().toAscii().constData() << " "
       << valueLineEditMaxZ->text().toAscii().constData();
   const QString s = str.str().c_str();
   return s;
}

/**
 * set the value.
 */
void 
GuiScriptExtentArg::setValue(const int x1, const int x2,
                             const int y1, const int y2,
                             const int z1, const int z2)
{
   valueLineEditMinX->setText(QString::number(x1));   
   valueLineEditMaxX->setText(QString::number(x2));   
   valueLineEditMinY->setText(QString::number(y1));   
   valueLineEditMaxY->setText(QString::number(y2));   
   valueLineEditMinZ->setText(QString::number(z1));   
   valueLineEditMaxZ->setText(QString::number(z2));   
}

//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptComboBoxArg::GuiScriptComboBoxArg(QGridLayout* parentLayout,
                  const QString& labelString,
                  const std::vector<QString>& comboBoxLabels,
                  const std::vector<QString>& comboBoxValuesIn,
                  const int defaultItem)
   : GuiScriptArg(parentLayout, labelString)
{
   valueComboBox = new QComboBox;
   parentLayout->addWidget(valueComboBox, rowNumber, 1);
   comboBoxValues = comboBoxValuesIn;
   for (unsigned int i = 0; i < comboBoxLabels.size(); i++) {
      valueComboBox->addItem(comboBoxLabels[i]);
   }
   valueComboBox->setCurrentIndex(defaultItem);
}

/**
 * destructor.
 */
GuiScriptComboBoxArg::~GuiScriptComboBoxArg()
{
}

/**
 * set the value.
 */
void 
GuiScriptComboBoxArg::setValue(const int indx)
{
   valueComboBox->setCurrentIndex(indx);
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptComboBoxArg::getValue() const
{
   QString s;
   
   const int num = valueComboBox->currentIndex(); 
   if ((num >= 0) && (num < static_cast<int>(comboBoxValues.size()))) {
      s = comboBoxValues[num];
   }
   
   return s;
}

//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptAxisArg::GuiScriptAxisArg(QGridLayout* parentLayout,
                  const QString& labelString)
   : GuiScriptArg(parentLayout, labelString)
{
   valueComboBox = new QComboBox;
   parentLayout->addWidget(valueComboBox, rowNumber, 1);
   valueComboBox->addItem("X Axis");
   valueComboBox->addItem("Y Axis");
   valueComboBox->addItem("Z Axis");
}

/**
 * destructor.
 */
GuiScriptAxisArg::~GuiScriptAxisArg()
{
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptAxisArg::getValue() const
{
   QString s = "false";
   switch (valueComboBox->currentIndex()) {
      case 0:
         s = "X";
         break;
      case 1:
         s = "Y";
         break;
      case 2:
         s = "Z";
         break;
   }
   return s;
}

/**
 * set the value.
 */
void 
GuiScriptAxisArg::setValue(const int indx)
{
   valueComboBox->setCurrentIndex(indx);
}

//--------------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiScriptStringArg::GuiScriptStringArg(QGridLayout* parentLayout,
                  const QString& labelString,
                  const QString& defaultValue)
   : GuiScriptArg(parentLayout, labelString)
{
   valueLineEdit = new QLineEdit;
   valueLineEdit->setText(defaultValue);
   parentLayout->addWidget(valueLineEdit, rowNumber, 1);
   valueLineEdit->setToolTip( "Text value.");
}

/**
 * destructor.
 */
GuiScriptStringArg::~GuiScriptStringArg()
{
}

/**
 * set the value of the arg.
 */
void 
GuiScriptStringArg::setValue(const QString& s)
{
   valueLineEdit->setText(s);
}

/**
 * get the value of the arg as text.
 */
QString 
GuiScriptStringArg::getValue() const
{
   const QString s = valueLineEdit->text();
   return s;
}
//--------------------------------------------------------------------------------------

