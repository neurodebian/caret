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

#include <QDir>
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>

#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiChooseSpecFileDialog.h"
#include "GuiDataFileCommentDialog.h"
#include "GuiPreviousSpecFileComboBox.h"
#include "SpecFile.h"
#include "PreferencesFile.h"
#include "QtTextEditDialog.h"
#include "QtUtilities.h"

/**
 * Constructor.
 */
GuiChooseSpecFileDialog::GuiChooseSpecFileDialog(QWidget* parent,
                                       const std::vector<QString>& previousSpecFilesIn,
                                       const bool modalIn,
                                       const bool allowMultipleSelectionsFlag)
   : WuQFileDialog(parent)
{
   setModal(modalIn);
   createDialog(previousSpecFilesIn,
                allowMultipleSelectionsFlag);
}

/**
 * Constructor.
 */
GuiChooseSpecFileDialog::GuiChooseSpecFileDialog(QWidget* parent,
                                       const PreferencesFile* pf,
                                       const bool modalIn,
                                       const bool allowMultipleSelectionsFlag)
   : WuQFileDialog(parent)
{
   setModal(modalIn);
   std::vector<QString> specFileNames;
   pf->getRecentSpecFiles(specFileNames);
   createDialog(specFileNames,
                allowMultipleSelectionsFlag);
}

/**
 * Create the dialog.
 */
void
GuiChooseSpecFileDialog::createDialog(const std::vector<QString>& specFileNames,
                                      const bool allowMultipleSpecFilesFlag)
{
   viewContentsDialog = NULL;
   
   setDirectory(QDir::currentPath());
   setWindowTitle("Choose Spec File");
   setAcceptMode(WuQFileDialog::AcceptOpen);
   if (allowMultipleSpecFilesFlag) {
      setFileMode(WuQFileDialog::ExistingFiles);
   }
   else {
      setFileMode(WuQFileDialog::ExistingFile);
   }
   setFilters(QStringList(FileFilters::getSpecFileFilter()));
   selectFilter(FileFilters::getSpecFileFilter());
   
   //
   // Combo box for selecting previous spec files
   //
   previousSpecFilesComboBox = new GuiPreviousSpecFileComboBox(0,
                                             specFileNames);
   QObject::connect(previousSpecFilesComboBox, SIGNAL(specFileSelected(const QString&)),
                    this, SLOT(slotPreviousSpecFileComboBox(const QString&)));
                    
   //
   // Create group box for previous spec files
   //
   previousSpecFilesGroupBox = new QGroupBox("Previous Spec Files");
   QVBoxLayout* prevSpecFileLayout = new QVBoxLayout(previousSpecFilesGroupBox);
   prevSpecFileLayout->addWidget(previousSpecFilesComboBox);
   
   //
   // View contents button
   //
   viewPushButton = new QPushButton("View/Edit Comment...", this);
   viewPushButton->setAutoDefault(false);
   QObject::connect(viewPushButton, SIGNAL(clicked()),
                    this, SLOT(slotViewPushButton()));
   viewPushButton->setEnabled(false);
   
   //
   // Called when a file is highlighted
   //
   QObject::connect(this, SIGNAL(filesSelected(const QStringList&)),
                    this, SLOT(slotFilesSelected(const QStringList&)));
                    
   addWidgets(previousSpecFilesGroupBox, previousSpecFilesGroupBox, viewPushButton);
   
   QtUtilities::setMaximumHeightToNinetyPercentOfScreenHeight(this);
}

/**
 * get the selected spec file.
 */
QString 
GuiChooseSpecFileDialog::getSelectedSpecFile() const
{
   QString name;
   
   const QStringList fileList = selectedFiles();
   if (fileList.count() > 0) {
      name = fileList.at(0);
   }
   
   return name;
}      

/**
 * get the selected spec files.
 */
std::vector<QString> 
GuiChooseSpecFileDialog::getSelectedSpecFiles() const
{
   std::vector<QString> names;
   
   const QStringList fileList = selectedFiles();
   int numFiles = fileList.count();
   if ((numFiles > 1) &&
       (fileMode() == WuQFileDialog::ExistingFile)) {
      numFiles = 1;
   }
   for (int i = 0; i < numFiles; i++) {
      names.push_back(fileList.at(i));
   }
   
   return names;
}

/**
 * Destructor.
 */
GuiChooseSpecFileDialog::~GuiChooseSpecFileDialog()
{
}

/**
 * Called when a file is highlighted
 */
void
GuiChooseSpecFileDialog::slotFilesSelected(const QStringList& names)
{
   highlightedFileName = "";
   viewPushButton->setEnabled(false);
   if (names.count() == 1) {
      highlightedFileName = names.at(0);
      viewPushButton->setEnabled(true);
   }
}

/**
 * called when view pushbutton is pressed.
 */
void 
GuiChooseSpecFileDialog::slotViewPushButton()
{
   GuiDataFileCommentDialog* dfcd = new GuiDataFileCommentDialog(this,
                                                                 highlightedFileName,
                                                                 false);
   dfcd->show();
}

/**
 * called when a previous spec file is selected.
 */
void 
GuiChooseSpecFileDialog::slotPreviousSpecFileComboBox(const QString& name)
{
   selectFile(name);
}
