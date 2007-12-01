
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

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "FileFilters.h"
#include "QtInputDialog.h"
#include "WuQFileDialog.h"

/**
 * constructor.
 */
QtInputDialog::QtInputDialog(QWidget* parent,
                             const QString& messageText,
                             const bool showFileSelectionPushButtonFlag,
                             Qt::WFlags f)
   : QtDialogModal(parent, f)
{
   QLabel* label = new QLabel(messageText);
   
   lineEdit = new QLineEdit;
   
   QPushButton* filePushButton = NULL;
   if (showFileSelectionPushButtonFlag) {
      filePushButton = new QPushButton("Select File...");
      filePushButton->setAutoDefault(false);
      filePushButton->setFixedSize(filePushButton->sizeHint());
      QObject::connect(filePushButton, SIGNAL(clicked()),
                       this, SLOT(slotFilePushButton()));
   }
   
   QVBoxLayout* layout = getDialogLayout();
   layout->addWidget(label);
   layout->addWidget(lineEdit);
   if (filePushButton != NULL) {
      layout->addWidget(filePushButton);
   }
}

/**
 * constructor.
 */
QtInputDialog::~QtInputDialog()
{
}

/**
 * called when file push button is selected.
 */
void 
QtInputDialog::slotFilePushButton()
{
   QStringList allFileFilters;
   FileFilters::getAllFileFilters(allFileFilters);
   
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(".");
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilters(allFileFilters);
   fd.selectFilter(FileFilters::getAnyFileFilter());
   if (fd.exec() == WuQFileDialog::Accepted) {
      QStringList selectedFiles = fd.selectedFiles();
      if (selectedFiles.count() > 0) {
         lineEdit->setText(selectedFiles.at(0));
      }
   }
}

/**
 * get text entered by user.
 */
QString 
QtInputDialog::getInputText() const
{
   return lineEdit->text().trimmed();
}
