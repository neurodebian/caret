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
//#include <QDesktopWidget>
#include <QKeyEvent>

#include "QtDialog.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
QtDialog::QtDialog(QWidget* parent,
                   const bool modalFlag,
                   Qt::WFlags f)
   : QDialog(parent, f)
{
   setModal(modalFlag);
}
              
/**
 * destructor.
 */
QtDialog::~QtDialog()
{
}
      
/**
 * show the dialog.
 */
void 
QtDialog::show()
{
   setMaximumHeight(QtUtilities::getMaximumWindowHeight());
   QDialog::show();
}
      
/**
 * called by parent when a key is pressed.
 */
void 
QtDialog::keyPressEvent(QKeyEvent *ke)
{
   if (QtUtilities::getImageCaptureKeySelected(ke)) {
      QtUtilities::saveWidgetAsImage(this);
      ke->accept();
   }
   else {
      ke->ignore();
   }
}

/**
 * ring the bell.
 */
void 
QtDialog::beep()
{
   QApplication::beep();
}

/**
 * show the watch cursor.
 */
void 
QtDialog::showWaitCursor()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

/**
 * normal cursor.
 */
void 
QtDialog::showNormalCursor()
{
   QApplication::restoreOverrideCursor();
}
