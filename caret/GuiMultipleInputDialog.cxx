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


#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

#include "WuQDialog.h"
#include "QtUtilities.h"

#include "GuiMultipleInputDialog.h"

/**
 * Constructor
 */
GuiMultipleInputDialog::GuiMultipleInputDialog(QWidget* parent, 
                                               const QString& caption,
                                               const std::vector<QString>& labels)
   : WuQDialog(parent)
{
   setModal(true);
   setWindowTitle(caption);
   
   const int numRows = static_cast<int>(labels.size());
   
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(5);
   
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   
   //
   // Create the labels an line edits
   //
   for (int i = 0; i < numRows; i++) {
      gridLayout->addWidget(new QLabel(labels[i]), i, 0, Qt::AlignLeft);
      QLineEdit* le = new QLineEdit;
      gridLayout->addWidget(le, i, 1, Qt::AlignLeft);
      lineEdits.push_back(le);
   }
   
   //
   // Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* ok = new QPushButton("OK");
   QObject::connect(ok, SIGNAL(clicked()),
                    this, SLOT(accept()));
   buttonsLayout->addWidget(ok);
   
   QPushButton* close = new QPushButton("Cancel");
   QObject::connect(close, SIGNAL(clicked()),
                    this, SLOT(reject()));
   buttonsLayout->addWidget(close);
   
   QtUtilities::makeButtonsSameSize(ok, close);   
}

/**
 * Destructor
 */
GuiMultipleInputDialog::~GuiMultipleInputDialog()
{
}

/**
 * Set a line edit to an integer value.
 */
void
GuiMultipleInputDialog::setLineEdit(const int index, const int value)
{
   lineEdits[index]->setText(QString("%1").arg(value));
}

/**
 * Set a line edit to a float value.
 */
void
GuiMultipleInputDialog::setLineEdit(const int index, const float value, const int precision)
{
   lineEdits[index]->setText(QString("%1").arg(value, 0, 'f', precision));
}

/**
 * Set a line edit to a string value.
 */
void
GuiMultipleInputDialog::setLineEdit(const int index, const QString& value)
{
   lineEdits[index]->setText(value);
}

/**
 * Get the integer value from a line edit.
 */
void
GuiMultipleInputDialog::getLineEdit(const int index, int& value) const
{
   value = lineEdits[index]->text().toInt();
}

/**
 * Get a float value from a line edit.
 */
void
GuiMultipleInputDialog::getLineEdit(const int index, float& value) const
{
   value = lineEdits[index]->text().toFloat();
}

/**
 * Get a string value from a line edit.
 */
void
GuiMultipleInputDialog::getLineEdit(const int index, QString& value) const
{
   value = lineEdits[index]->text();
}

