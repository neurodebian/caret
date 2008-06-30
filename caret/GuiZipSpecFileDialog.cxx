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

#include <sstream>

#include <QDialogButtonBox>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QToolTip>

#include "GuiChooseSpecFileDialog.h"
#include "GuiZipSpecFileDialog.h"
#include "SpecFileUtilities.h"
#include "StringUtilities.h"
#include "WuQFileDialog.h"

/**
 * constructor.
 */
GuiZipSpecFileDialog::GuiZipSpecFileDialog(QWidget* parent,
                                           PreferencesFile* pref)
   : WuQDialog(parent)
{
   preferencesFile = pref;
   
   setWindowTitle("Zip Spec File");
   
   //
   // Get the layout
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   
   //
   // Grid layout for dialog items
   //
   QGridLayout* grid = new QGridLayout;
   dialogLayout->addLayout(grid);
   grid->setMargin(5);
   
   const int lineEditWidth = 350;
   
   //
   // Spec File
   //
   QPushButton* specButton = new QPushButton("Zip This Spec File...");
   specButton->setAutoDefault(false);
   QObject::connect(specButton, SIGNAL(clicked()),
                    this, SLOT(slotSpecFileButton()));
   specFileLineEdit = new QLineEdit; 
   specFileLineEdit->setReadOnly(true); 
   specFileLineEdit->setMinimumWidth(lineEditWidth);
   specFileLineEdit->setToolTip(
                 "Spec File that will be zipped."); 
                    
   //
   // Zip File
   //
   QPushButton* zipButton = new QPushButton("Create This Zip File...");
   zipButton->setAutoDefault(false);
   QObject::connect(zipButton, SIGNAL(clicked()),
                    this, SLOT(slotZipFileButton()));
   zipFileLineEdit = new QLineEdit;   
   zipFileLineEdit->setMinimumWidth(lineEditWidth);
   zipFileLineEdit->setToolTip(
                 "Name of ZIP file that will be created."); 
           
   //
   // Unzip directory
   //
   QLabel* unzipLabel = new QLabel("Unzips to Subdirectory");
   unzipDirLineEdit = new QLineEdit;
   unzipDirLineEdit->setMinimumWidth(lineEditWidth);
   unzipDirLineEdit->setToolTip(
                 "When the user unzips the ZIP file,\n"
                 "the Spec File and its data files\n"
                 "will be placed in this subdirectory."); 
   
   //
   // Add widgets to grid
   //
   grid->addWidget(specButton, 0, 0, Qt::AlignLeft);
   grid->addWidget(specFileLineEdit, 0, 1);
   grid->addWidget(zipButton, 1, 0, Qt::AlignLeft);
   grid->addWidget(zipFileLineEdit, 1, 1);
   grid->addWidget(unzipLabel, 2, 0, Qt::AlignLeft);
   grid->addWidget(unzipDirLineEdit, 2, 1);
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply 
                                                      | QDialogButtonBox::Close);
   dialogLayout->addWidget(buttonBox);
   QPushButton* applyButton = buttonBox->button(QDialogButtonBox::Apply);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
}

/**
 * destructor.
 */
GuiZipSpecFileDialog::~GuiZipSpecFileDialog()
{
}

/**
 * called when apply button is pressed.
 */
void 
GuiZipSpecFileDialog::slotApplyButton()
{
   //
   // Check parameters
   //
   const QString specFileName(specFileLineEdit->text());
   if (specFileName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Spec File name is empty.");
      return;
   }
   QString zipFileName(zipFileLineEdit->text());
   if (zipFileName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Zip File name is empty.");
      return;
   }
   const QString unzipDirName(unzipDirLineEdit->text());
   if (unzipDirName.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Unzip directory name is empty.");
      return;
   }
   
   if (QFile::exists(specFileName) == false) {
      QMessageBox::critical(this, "ERROR", "Spec File does not exist.");
      return;
   }
   
   //
   // Add zip file extension (if needed)
   // 
   if (StringUtilities::endsWith(StringUtilities::makeLowerCase(zipFileName),
                                 ".zip") == false) {
      zipFileName.append(".zip");
      zipFileLineEdit->setText(zipFileName);
   }
   
   //
   // Warn user if zip file already exists
   //
   if (QFile::exists(zipFileName)) {
      std::ostringstream str;
      str << "The ZIP file "
          << zipFileName.toAscii().constData()
          << "\n"
          << " already exists.\n"
          << "Do you want to overwrite it?";
      if (QMessageBox::question(this, "Overwrite", str.str().c_str(), 
                                (QMessageBox::Yes | QMessageBox::No),
                                QMessageBox::No)
                                   == QMessageBox::No) {
         return;
      }  
      QFile::remove(zipFileName);       
   }
   
   showWaitCursor();
   
   //
   // Zip the spec file
   //
   QString errorMessage;
   const bool errorFlag = SpecFileUtilities::zipSpecFile(specFileName, 
                                                         zipFileName,
                                                         unzipDirName,
                                                         errorMessage);

   showNormalCursor();
   
   if (errorFlag) {
      QMessageBox::critical(this, "ERROR", errorMessage);
      return;
   }
   QMessageBox::information(this, "Success", "ZIP file has been created.");
}

/**
 * called when spec file button is pressed.
 */
void 
GuiZipSpecFileDialog::slotSpecFileButton()
{
   //
   // Choose the spec file to be copied
   //
   GuiChooseSpecFileDialog csfd(this,
                                preferencesFile,
                                true);
   if (csfd.exec() == GuiChooseSpecFileDialog::Accepted) {
      specFileLineEdit->setText(csfd.getSelectedSpecFile());
   }
}

/**
 * called when zip file button is pressed.
 */
void 
GuiZipSpecFileDialog::slotZipFileButton()
{
   //
   // Create a zip file dialog to select the zip file. 
   //
   WuQFileDialog zipFileDialog(this);
   zipFileDialog.setModal(true);
   zipFileDialog.setDirectory(QDir::currentPath());
   zipFileDialog.setAcceptMode(WuQFileDialog::AcceptSave);
   zipFileDialog.setWindowTitle("Choose Zip File");
   zipFileDialog.setFileMode(WuQFileDialog::AnyFile);
   zipFileDialog.setFilters(QStringList("Zip File (*.zip)"));
   if (zipFileDialog.exec() == WuQFileDialog::Accepted) {
      if (zipFileDialog.selectedFiles().count() > 0) {
         zipFileLineEdit->setText(zipFileDialog.selectedFiles().at(0));
      }
   }
}

      
