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

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "GuiGraphWidget.h"
#include "GuiGraphWidgetDialog.h"

/**
 * constructor.
 */
GuiGraphWidgetDialog::GuiGraphWidgetDialog(const QString& title,
                                           QWidget* parent,
                                           Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   
   graphWidget = new GuiGraphWidget(0, title);
   dialogLayout->addWidget(graphWidget);
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
   buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(false);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
}

/**
 * destructor.
 */
GuiGraphWidgetDialog::~GuiGraphWidgetDialog()
{
}
