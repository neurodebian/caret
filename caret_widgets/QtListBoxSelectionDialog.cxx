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
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QPushButton>

#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"

/**
 * Constructor that adds select all button
 */
QtListBoxSelectionDialog::QtListBoxSelectionDialog(QWidget* parent, 
                                                   const QString& title,
                                                   const QString& instructions,
                                                   const std::vector<QString>& items,
                                                   const QString& selectAllButtonLabel,
                                                   const int defaultItem)
   : WuQDialog(parent)
{
   createDialog(title, selectAllButtonLabel, instructions);
   setListBoxContents(items, defaultItem);
}

/** 
 * Constructor
 */
QtListBoxSelectionDialog::QtListBoxSelectionDialog(QWidget* parent, 
                                                   const QString& title,
                                                   const QString& instructions,
                                                   const std::vector<QString>& items,
                                                   const int defaultItem)
   : WuQDialog(parent)
{
   createDialog(title, "", instructions);
   setListBoxContents(items, defaultItem);
}

/** 
 * Constructor
 */
QtListBoxSelectionDialog::QtListBoxSelectionDialog(QWidget* parent, 
                                                   const QString& title,
                                                   const QString& instructions,
                                                   const QStringList& itemList,
                                                   const int defaultItem)
   : WuQDialog(parent)
{
   
   std::vector<QString> items;
   for (int i = 0; i < itemList.count(); i++) {
      items.push_back(itemList[i]);
   }
   createDialog(title, "", instructions);
   setListBoxContents(items, defaultItem);
}


QtListBoxSelectionDialog::QtListBoxSelectionDialog(QWidget* parent, 
                                                   const QString& title)
   : WuQDialog(parent)
{
   createDialog(title, "", "");
}

/** 
 * Destructor
 */
QtListBoxSelectionDialog::~QtListBoxSelectionDialog()
{
}

/**
 *
 */
void
QtListBoxSelectionDialog::setAllowMultipleItemSelection(const bool allowIt)
{
   if (allowIt) {
      listWidget->setSelectionMode(QListWidget::ExtendedSelection);
   }
   else {
      listWidget->setSelectionMode(QListWidget::SingleSelection);
   }
}

/**
 * Create the dialog.
 */
void
QtListBoxSelectionDialog::createDialog(const QString& title,
                                       const QString& selectAllButtonLabel,
                                       const QString& instructions)
{
   setWindowTitle(title);

   QVBoxLayout* layout = new QVBoxLayout;
   layout->setMargin(3);
   layout->setSpacing(3);
   setLayout(layout);
   
   if (instructions.isEmpty() == false) {
      layout->addWidget(new QLabel(instructions, this));
   }
   
   listWidget = new QListWidget;
   layout->addWidget(listWidget);
   
   if (selectAllButtonLabel.isEmpty() == false) {
      listWidget->setSelectionMode(QListWidget::ExtendedSelection);
      QPushButton* selectAllPushButton = new QPushButton(selectAllButtonLabel);
      selectAllPushButton->setAutoDefault(false);
      selectAllPushButton->setFixedSize(selectAllPushButton->sizeHint());
      QObject::connect(selectAllPushButton, SIGNAL(clicked()),
                       this, SLOT(slotSelectAllPushButton()));
      layout->addWidget(selectAllPushButton);
   }
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok 
                                                      | QDialogButtonBox::Cancel);
   layout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(accepted()),
                    this, SLOT(accept()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(reject()));
}

/**
 * called when select all pushbutton is pressed.
 */
void 
QtListBoxSelectionDialog::slotSelectAllPushButton()
{
   for (unsigned int i = 0; i < items.size(); i++) {
      listWidget->setItemSelected(items[i], true);
   }
}

/**
 * Load the list box
 */
void
QtListBoxSelectionDialog::setListBoxContents(const std::vector<QString>& contents,
                                             const int defaultIndex)
{
   inputItems = contents;
   
   listWidget->clear();
   items.clear();
   
   for (unsigned int i = 0; i < contents.size(); i++) {
      QListWidgetItem* it = new QListWidgetItem(contents[i]);
      listWidget->addItem(it);
      items.push_back(it);
   }
   if (defaultIndex >= 0) {
      listWidget->setItemSelected(items[defaultIndex], true);
   }
}

/*
 * Get the selected item
 */
int 
QtListBoxSelectionDialog::getSelectedItemIndex() const 
{ 
   const int num = listWidget->count();
   for (int i = 0; i < num; i++) {
      if (listWidget->isItemSelected(items[i])) {
         return i;
      }
   }
   return -1;
}

/**
 *  Get the text of the selected item
 */
QString
QtListBoxSelectionDialog::getSelectedText() const
{
   QString txt;
   const int indx = getSelectedItemIndex();
   if (indx >= 0) {
      txt = items[indx]->text();
   }
   return txt;
}

/**
 * get multiple selected items.
 */
void 
QtListBoxSelectionDialog::getSelectedItems(std::vector<QString>& selectedItems) const
{
   selectedItems.clear();
   const int num = listWidget->count();
   for (int i = 0; i < num; i++) {
      if (listWidget->isItemSelected(items[i])) {
         selectedItems.push_back(inputItems[i]);
      }
   }
}

/**
 * get multiple selected items.  On return vector contain selected item indices.
 */
void 
QtListBoxSelectionDialog::getSelectedItemsIndices(std::vector<int>& selectedItemIndices) const
{
   selectedItemIndices.clear();
   const int num = listWidget->count();
   for (int i = 0; i < num; i++) {
      if (listWidget->isItemSelected(items[i])) {
         selectedItemIndices.push_back(i);
      }
   }
}
      

