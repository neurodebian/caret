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
#include "GuiFileSelectionButton.h"

/**
 * constructor
 */
GuiFileSelectionButton::GuiFileSelectionButton(QWidget* parent,
                                               const QString& buttonText,
                                               const QString& fileFilterIn,
                                               const bool fileMustExistIn)
   : QPushButton(buttonText, parent)
{
   setAutoDefault(false);
   fileFilter = fileFilterIn;
   fileMustExist = fileMustExistIn;
   
   QObject::connect(this, SIGNAL(clicked()),
                    this, SLOT(slotFileSelected()));
}

/**
 * destructor*/
GuiFileSelectionButton::~GuiFileSelectionButton()
{
}

/**
 * called when file selected
 */
void 
GuiFileSelectionButton::slotFileSelected()
{
   WuQFileDialog fd(this);
   fd.setDirectory(QDir::currentPath());
   fd.setModal(true);
   fd.setWindowTitle("Choose File");
   if (fileMustExist) {
      fd.setFileMode(WuQFileDialog::ExistingFile);
      fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   }
   else {
      fd.setFileMode(WuQFileDialog::AnyFile);
      fd.setAcceptMode(WuQFileDialog::AcceptSave);
   }
   
   fd.setFilters(QStringList(fileFilter));
   fd.selectFilter(fileFilter);
   if (fd.exec() == QDialog::Accepted) {
      QString name(fd.selectedFiles().at(0));
      if (name.isEmpty() == false) {
         emit fileSelected(name);
      }
   }   
}
