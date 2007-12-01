
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

#include <QDir>
#include "WuQFileDialog.h"
#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>

#include "GuiFileSelectionListWidget.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
GuiFileSelectionListWidget::GuiFileSelectionListWidget(const QString& groupBoxTitle,
                                                       QWidget* parent)
   : QGroupBox(groupBoxTitle, parent)
{   
   //
   // List Widget for files
   //
   listWidget = new QListWidget;
   listWidget->setSelectionMode(QListWidget::ExtendedSelection);
   
   //
   // Add files push button
   //
   QPushButton* addFilesPushButton = new QPushButton("Add Files...");
   addFilesPushButton->setAutoDefault(false);
   QObject::connect(addFilesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddFilesPushButton()));
                    
   //
   // Remove files push button
   //
   QPushButton* removeFilesPushButton = new QPushButton("Remove Selected Files");
   removeFilesPushButton->setAutoDefault(false);
   QObject::connect(removeFilesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRemoveFilesPushButton()));
   
   //
   // Make the buttons the same size
   //
   QtUtilities::makeButtonsSameSize(addFilesPushButton,
                                    removeFilesPushButton);
                                    
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(addFilesPushButton);
   buttonsLayout->addWidget(removeFilesPushButton);
   
   //
   // Layout for this widget
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(listWidget);
   layout->addLayout(buttonsLayout);
}
            
/**
 * destructor.
 */
GuiFileSelectionListWidget::~GuiFileSelectionListWidget()
{
}

/**
 * set the file selection filters.
 */
void 
GuiFileSelectionListWidget::setFileSelectionFilters(const QStringList& fileSelectionFiltersIn)
{
   fileSelectionFilters = fileSelectionFiltersIn;
}
      
/**
 * slot for adding files.
 */
void 
GuiFileSelectionListWidget::slotAddFilesPushButton()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setFileMode(WuQFileDialog::ExistingFiles);
   fd.setDirectory(QDir::currentPath());
   if (fileSelectionFilters.count() <= 0) {
      fd.setFilters(QStringList("All Files (*)"));
   }
   else {
      fd.setFilters(fileSelectionFilters);
   }
   if (fd.exec() == WuQFileDialog::Accepted) {
      const QStringList& files = fd.selectedFiles();
      for (int i = 0; i < files.size(); i++) {
         listWidget->addItem(files.at(i));
      }
   }
   emit signalFilesChanged();
}

/**
 * slot for removing files.
 */
void 
GuiFileSelectionListWidget::slotRemoveFilesPushButton()
{
   QStringList itemsToKeep;
   
   for (int i = 0; i < listWidget->count(); i++) {
      QListWidgetItem* lwi = listWidget->item(i);
      if (listWidget->isItemSelected(lwi) == false) {
         itemsToKeep << lwi->text();
      }
   }
   
   listWidget->clear();
   for (int i = 0; i < itemsToKeep.size(); i++) {
      listWidget->addItem(itemsToKeep.at(i));
   }
   emit signalFilesChanged();
}

/**
 * add files.
 */
void 
GuiFileSelectionListWidget::addFiles(const QStringList& sl)
{
   for (int i = 0; i < sl.count(); i++) {
      listWidget->addItem(sl.at(i));
   }
   emit signalFilesChanged();
}
      
/**
 * see if there are files in the list widget.
 */
bool 
GuiFileSelectionListWidget::containsFiles() const
{
   return (listWidget->count() > 0);
}
      
/**
 * remove all files.
 */
void 
GuiFileSelectionListWidget::removeAllFiles()
{
   listWidget->clear();
   emit signalFilesChanged();
}

/**
 * get the files from the list widget.
 */
void 
GuiFileSelectionListWidget::getFileNames(QStringList& fileNames) const
{
   fileNames.clear();
   for (int i = 0; i < listWidget->count(); i++) {
      fileNames << listWidget->item(i)->text();
   }
}

/**
 * get the files from the list widget.
 */
void 
GuiFileSelectionListWidget::getFileNames(std::vector<QString>& fileNames) const
{
   fileNames.clear();
   for (int i = 0; i < listWidget->count(); i++) {
      fileNames.push_back(listWidget->item(i)->text());
   }
}
