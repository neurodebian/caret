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

#include <cstdlib>
#include <iostream>

#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QVBoxLayout>

#include "QtFileDialog.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
QtFileDialog::QtFileDialog(QWidget* parent, Qt::WFlags flags)
   : QFileDialog(parent, flags)
{
}

/**
 * destructor.
 */
QtFileDialog::~QtFileDialog()
{
}

/**
 * add an additional widget.
 */
void 
QtFileDialog::addWidget(QWidget* w)
{
   //
   // Get the dialog's layout and add a widget to it
   //
   QLayout* l = layout();
   if (dynamic_cast<QVBoxLayout*>(l) != 0) {
      QVBoxLayout* v = dynamic_cast<QVBoxLayout*>(l);
      v->addWidget(w);
   }
   if (dynamic_cast<QHBoxLayout*>(l) != 0) {
      std::cout << "QFileDialog layout is a QHBoxLayout, must be QVBoxLayout or QGridLayout" << std::endl;
      abort();
   }
   if (dynamic_cast<QGridLayout*>(l) != 0) {
      QGridLayout* grid = dynamic_cast<QGridLayout*>(l);
      const int numRows = grid->rowCount();
      const int numCols = grid->columnCount();
      grid->addWidget(w, numRows, 0, 1, numCols);
   }
}

/**
 * called by parent when a key is pressed.
 */
void 
QtFileDialog::keyPressEvent(QKeyEvent *ke)
{
   if (QtUtilities::getImageCaptureKeySelected(ke)) {
      QtUtilities::saveWidgetAsImage(this);
      ke->accept(); 
   }
   else {
      ke->ignore(); 
   }
}
