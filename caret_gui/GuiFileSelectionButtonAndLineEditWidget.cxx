
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
#include <QLineEdit>

#include "GuiFileSelectionButton.h"
#include "GuiFileSelectionButtonAndLineEditWidget.h"

/**
 * constructor.
 */
GuiFileSelectionButtonAndLineEditWidget::GuiFileSelectionButtonAndLineEditWidget(
                                                       const QString& buttonText,
                                                       const QString& fileFilter,
                                                       const bool fileMustExistIn,
                                                       QWidget* parent)
   : QWidget(parent)
{
   //
   // Create the file selection button
   //
   fileSelectionButton = new GuiFileSelectionButton(0,
                                                    buttonText,
                                                    fileFilter,
                                                    fileMustExistIn);
   
   //
   // The file name line edit
   //
   fileNameLineEdit = new QLineEdit;
   fileNameLineEdit->setReadOnly(fileMustExistIn);
   
   //
   // Layout the widgets
   //
   QHBoxLayout* layout = new QHBoxLayout(this);
   layout->addWidget(fileSelectionButton);
   layout->addWidget(fileNameLineEdit);
   layout->setStretchFactor(fileSelectionButton, 0);
   layout->setStretchFactor(fileNameLineEdit, 100);
                      
   //
   // Connect signals
   //
   QObject::connect(fileSelectionButton, SIGNAL(fileSelected(const QString&)),
                    fileNameLineEdit, SLOT(setText(const QString&)));
   QObject::connect(fileSelectionButton, SIGNAL(fileSelected(const QString&)),
                    this, SIGNAL(fileSelected(const QString&)));
}
                                        
/**
 * destructor.
 */
GuiFileSelectionButtonAndLineEditWidget::~GuiFileSelectionButtonAndLineEditWidget()
{
}

/**
 * get the file name.
 */
QString 
GuiFileSelectionButtonAndLineEditWidget::getFileName() const
{
   return fileNameLineEdit->text();
}

/**
 * set the file name.
 */
void 
GuiFileSelectionButtonAndLineEditWidget::setFileName(const QString& name)
{
   fileNameLineEdit->setText(name);
}

/**
 * set the button label's text.
 */
void 
GuiFileSelectionButtonAndLineEditWidget::setButtonLabelText(const QString& text)
{
   fileSelectionButton->setText(text);
}

/**
 * called when return pressed in the line edit.
 */
void 
GuiFileSelectionButtonAndLineEditWidget::slotLineEditReturnPressed()
{
   emit fileSelected(fileNameLineEdit->text());
}

