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

#include "GuiChooseNodeAttributeColumnDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GiftiNodeDataFile.h"
#include "NodeAttributeFile.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
GuiChooseNodeAttributeColumnDialog::GuiChooseNodeAttributeColumnDialog(
                                         QWidget* parent,
                                         GUI_NODE_FILE_TYPE nft,
                                         const QString& noneColumnLabel,
                                         const bool addNewSelection,
                                         const bool addNoneSelection)
   : WuQDialog(parent)
{
   setModal(true);
   
   setWindowTitle("Choose Column");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // column to assign labe, combo box, and line edit
   //
   QHBoxLayout* columnHBoxLayout = new QHBoxLayout;
   dialogLayout->addLayout(columnHBoxLayout);
   columnHBoxLayout->addWidget(new QLabel("Column"));
   nodeAttributeComboBox = new GuiNodeAttributeColumnSelectionComboBox(
                                          nft,
                                          addNewSelection,
                                          addNoneSelection,
                                          false);
   columnHBoxLayout->addWidget(nodeAttributeComboBox);
   nodeAttributeComboBox->setNoneSelectionLabel(noneColumnLabel);
   QObject::connect(nodeAttributeComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotNodeAttributeComboBox()));
   columnNameLineEdit = new QLineEdit;
   columnHBoxLayout->addWidget(columnNameLineEdit);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK pushbutton
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   
   //
   // Cancel button.
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   //
   // Force buttons to be same size
   //
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   slotNodeAttributeComboBox();
}

/**
 * destructor.
 */
GuiChooseNodeAttributeColumnDialog::~GuiChooseNodeAttributeColumnDialog()
{
}

/**
 * called when a column is selected.
 */
void 
GuiChooseNodeAttributeColumnDialog::slotNodeAttributeComboBox()
{
   columnNameLineEdit->setText(nodeAttributeComboBox->getCurrentLabel());
}
      
/**
 * set the selected column number.
 */
void 
GuiChooseNodeAttributeColumnDialog::setSelectedColumnNumber(const int col) const
{
   if ((col >= 0) && (col < nodeAttributeComboBox->count())) {
      nodeAttributeComboBox->setCurrentIndex(col);
   }
}
      
/**
 * get the selected column number.
 * Returns GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW  for new
 * Returns GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE  for none
 */
int 
GuiChooseNodeAttributeColumnDialog::getSelectedColumnNumber() const
{
   return nodeAttributeComboBox->currentIndex();
}

/**
 * get the name for the column.
 */
QString 
GuiChooseNodeAttributeColumnDialog::getColumnName() const
{
   return columnNameLineEdit->text();
}

