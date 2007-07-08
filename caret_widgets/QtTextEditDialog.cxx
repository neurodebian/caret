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


#include <QLayout>
#include <QPushButton>
#include <QTextEdit>

#include "QtTextEditDialog.h"
#include "QtUtilities.h"

/**
 * The constructor.
 */
QtTextEditDialog::QtTextEditDialog(QWidget* parent, const bool readOnly,
                       const bool modalFlag)
   : QtDialog(parent)
{
   setModal(modalFlag);
   if (modalFlag == false) {
      //
      // Destroy the dialog when it is closed
      //
      setAttribute(Qt::WA_DeleteOnClose);
   }
   
   resize(400, 200);

   setWindowTitle("Text Editor");
   
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setSpacing(5);
   
   textEditor = new QTextEdit;
   textEditor->setReadOnly(readOnly);
   rows->addWidget(textEditor);
   
   //
   // OK and Cancel Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   QPushButton* okButton = NULL;
   if (readOnly == false) {
      okButton = new QPushButton("OK");
      QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
      buttonsLayout->addWidget(okButton);
   }
   
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   if (readOnly) {
      cancelButton->setText("Close");
      cancelButton->setFixedSize(cancelButton->sizeHint());
   }
   else {
      QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   }
}

/**
 * The destructor.
 */
QtTextEditDialog::~QtTextEditDialog()
{
}

/**
 * Set the text in the text editor.
 */
void
QtTextEditDialog::setText(const QString& s)
{
   textEditor->setPlainText(s);
}

/**
 * Get the text in the editor
 */
QString
QtTextEditDialog::getText() const
{
   return textEditor->toPlainText();
}


