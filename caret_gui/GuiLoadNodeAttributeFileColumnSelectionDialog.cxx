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
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>

#include "GuiLoadNodeAttributeFileColumnSelectionDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GiftiNodeDataFile.h"
#include "NodeAttributeFile.h"
#include "QtUtilities.h"

/**
 * Constructor.
 */
GuiLoadNodeAttributeFileColumnSelectionDialog::GuiLoadNodeAttributeFileColumnSelectionDialog(
                                          QWidget* parent,
                                          NodeAttributeFile* newFileIn,
                                          NodeAttributeFile* currentFileIn)
   : WuQDialog(parent)
{
   setModal(true);
   newFileNAF = newFileIn;
   currentFileNAF = currentFileIn;
   
   setWindowTitle("Choose Columns to Load");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Layout for appending comment
   //
   QHBoxLayout* commentLayout = new QHBoxLayout;
   commentLayout->setSpacing(3);
   dialogLayout->addLayout(commentLayout);
   commentLayout->addWidget(new QLabel("File Comment "));
   appendFileCommentComboBox = new QComboBox;
   appendFileCommentComboBox->insertItem(AbstractFile::FILE_COMMENT_MODE_APPEND, "Append");
   appendFileCommentComboBox->insertItem(AbstractFile::FILE_COMMENT_MODE_LEAVE_AS_IS, "Leave As Is");
   appendFileCommentComboBox->insertItem(AbstractFile::FILE_COMMENT_MODE_REPLACE, "Replace");
   commentLayout->addWidget(appendFileCommentComboBox);
   
   //
   // Top of dialog layout
   //
   QHBoxLayout* topLayout = new QHBoxLayout;
   topLayout->setSpacing(3);
   dialogLayout->addLayout(topLayout);
   
   //
   // Erase all existing columns checkbox
   //
   eraseAllExistingColumnsCheckBox = NULL;
   if (currentFileNAF->getNumberOfColumns() > 0) {
      eraseAllExistingColumnsCheckBox = new QCheckBox("Erase All Existing Columns");
      topLayout->addWidget(eraseAllExistingColumnsCheckBox);
      QObject::connect(eraseAllExistingColumnsCheckBox, SIGNAL(toggled(bool)),
                       this, SLOT(slotEraseAllExistingColumnsCheckBox(bool)));
   }
   
   //
   // all to do not load button
   //
   QPushButton* allToDoNotLoadPushButton = 
      new QPushButton("Set All Columns to Do Not Load");
   topLayout->addWidget(allToDoNotLoadPushButton);
   allToDoNotLoadPushButton->setFixedSize(allToDoNotLoadPushButton->sizeHint());
   allToDoNotLoadPushButton->setAutoDefault(false);
   QObject::connect(allToDoNotLoadPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAllToDoNotLoadPushButton()));
                    
   //
   // Scroll and grid for names widget since there could be many names
   //
   QWidget* nameWidget = new QWidget;
   QGridLayout* nameGridLayout = new QGridLayout(nameWidget);
   nameGridLayout->setSpacing(3);

   //
   // Column names
   //
   nameGridLayout->addWidget(new QLabel("Column"), 0, 0);
   nameGridLayout->addWidget(new QLabel("Load into Column"), 0, 1);
   
   const int numCols = newFileNAF->getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      //
      // line edit so new file column names can be changed
      //
      QLineEdit* le = new QLineEdit;
      nameGridLayout->addWidget(le, i + 1, 0);
      le->setFixedWidth(300);
      le->setText(newFileNAF->getColumnName(i));
      le->home(true);
      columnNameLineEdits.push_back(le);
      
      //
      // existing file column to overwrite, or NONE, or NEW
      //
      GuiNodeAttributeColumnSelectionComboBox* csc =
         new GuiNodeAttributeColumnSelectionComboBox(currentFileNAF,
                                                     true,
                                                     true,
                                                     false);
      csc->setMaximumWidth(400);
      csc->setNoneSelectionLabel("Do Not Load");
      csc->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
      columnComboBoxes.push_back(csc);
      nameGridLayout->addWidget(csc, i + 1, 1);
   }
   
   //
   // Scroll and for names widget since there could be many names
   //
   QScrollArea* sv = new QScrollArea;
   dialogLayout->addWidget(sv);
   sv->setWidget(nameWidget);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));

   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}
                                          
/**
 * Constructor.
 */
GuiLoadNodeAttributeFileColumnSelectionDialog::GuiLoadNodeAttributeFileColumnSelectionDialog(
                                          QWidget* parent,
                                          GiftiNodeDataFile* newFileIn,
                                          GiftiNodeDataFile* currentFileIn)
   : WuQDialog(parent)
{
   setModal(true);
   newFileNDF = newFileIn;
   currentFileNDF = currentFileIn;
   
   setWindowTitle("Choose Columns to Load");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Layout for appending comment
   //
   QHBoxLayout* commentLayout = new QHBoxLayout;
   commentLayout->setSpacing(3);
   dialogLayout->addLayout(commentLayout);
   commentLayout->addWidget(new QLabel("File Comment "));
   appendFileCommentComboBox = new QComboBox;
   appendFileCommentComboBox->insertItem(AbstractFile::FILE_COMMENT_MODE_APPEND, "Append");
   appendFileCommentComboBox->insertItem(AbstractFile::FILE_COMMENT_MODE_LEAVE_AS_IS, "Leave As Is");
   appendFileCommentComboBox->insertItem(AbstractFile::FILE_COMMENT_MODE_REPLACE, "Replace");
   commentLayout->addWidget(appendFileCommentComboBox);
   
   //
   // Top of dialog layout
   //
   QHBoxLayout* topLayout = new QHBoxLayout;
   topLayout->setSpacing(3);
   dialogLayout->addLayout(topLayout);
   
   //
   // Erase all existing columns checkbox
   //
   eraseAllExistingColumnsCheckBox = NULL;
   if (currentFileNDF->getNumberOfColumns() > 0) {
      eraseAllExistingColumnsCheckBox = new QCheckBox("Erase All Existing Columns");
      topLayout->addWidget(eraseAllExistingColumnsCheckBox);
      QObject::connect(eraseAllExistingColumnsCheckBox, SIGNAL(toggled(bool)),
                       this, SLOT(slotEraseAllExistingColumnsCheckBox(bool)));
   }
   
   //
   // all to do not load button
   //
   QPushButton* allToDoNotLoadPushButton = 
      new QPushButton("Set All Columns to Do Not Load");
   topLayout->addWidget(allToDoNotLoadPushButton);
   allToDoNotLoadPushButton->setFixedSize(allToDoNotLoadPushButton->sizeHint());
   allToDoNotLoadPushButton->setAutoDefault(false);
   QObject::connect(allToDoNotLoadPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAllToDoNotLoadPushButton()));
                    
   //
   // grid for names widget since there could be many names
   //
   QWidget* nameGrid = new QWidget;
   QGridLayout* nameGridLayout = new QGridLayout(nameGrid);
   nameGridLayout->setSpacing(3);

   //
   // Column names
   //
   nameGridLayout->addWidget(new QLabel("Column"), 0, 0);
   nameGridLayout->addWidget(new QLabel("Load into Column"), 0, 1);
   
   const int numCols = newFileNDF->getNumberOfColumns();
   for (int i = 0; i < numCols; i++) {
      //
      // line edit so new file column names can be changed
      //
      QLineEdit* le = new QLineEdit;
      le->setFixedWidth(300);
      le->setText(newFileNDF->getColumnName(i));
      le->home(true);
      columnNameLineEdits.push_back(le);
      nameGridLayout->addWidget(le, i + 1, 0);
      
      //
      // existing file column to overwrite, or NONE, or NEW
      //
      GuiNodeAttributeColumnSelectionComboBox* csc =
         new GuiNodeAttributeColumnSelectionComboBox(currentFileNDF,
                                                     true,
                                                     true,
                                                     false);
      csc->setMaximumWidth(400);
      csc->setNoneSelectionLabel("Do Not Load");
      csc->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW);
      columnComboBoxes.push_back(csc);
      nameGridLayout->addWidget(csc, i + 1, 1);
   }
   
   //
   // Scroll and grid for names widget since there could be many names
   //
   QScrollArea* sv = new QScrollArea;
   dialogLayout->addWidget(sv);
   sv->setWidget(nameGrid);

   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));

   //
   // Cancel button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
                    
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}
                                          
/**
 * Destructor.
 */
GuiLoadNodeAttributeFileColumnSelectionDialog::~GuiLoadNodeAttributeFileColumnSelectionDialog()
{
}

/**
 * called when erase all columns check box is selected.
 */
void 
GuiLoadNodeAttributeFileColumnSelectionDialog::slotEraseAllExistingColumnsCheckBox(bool b)
{
   if (b) {
      appendFileCommentComboBox->setCurrentIndex(AbstractFile::FILE_COMMENT_MODE_REPLACE);
   }
}
      
/**
 * called when set all to do not load button pressed.
 */
void 
GuiLoadNodeAttributeFileColumnSelectionDialog::slotAllToDoNotLoadPushButton()
{
   for (int i = 0; i < static_cast<int>(columnComboBoxes.size()); i++) {
      columnComboBoxes[i]->setCurrentIndex(
               GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   }
}

/**
 * Get the destination column in the existing file for a column from 
 * the new file that is being loaded.
 * Note that -1 implies the destination is a new column.
 * Note that -2 implies that the column should not be loaded.
 */
std::vector<int>
GuiLoadNodeAttributeFileColumnSelectionDialog::getDestinationColumns() const
{
   std::vector<int> selections;
   for (int i = 0; i < static_cast<int>(columnComboBoxes.size()); i++) {
      int value = columnComboBoxes[i]->currentIndex();
      if (value < 0) {
         if (value == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_INVALID) {
            value = NodeAttributeFile::APPEND_COLUMN_DO_NOT_LOAD;
         }
         else if (value == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NEW) {
            value = NodeAttributeFile::APPEND_COLUMN_NEW;
         }
         else if (value == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE) {
            value = NodeAttributeFile::APPEND_COLUMN_DO_NOT_LOAD;
         }
         else if (value == GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_ALL) {
            value = NodeAttributeFile::APPEND_COLUMN_DO_NOT_LOAD;
         }
         else {
            value = NodeAttributeFile::APPEND_COLUMN_DO_NOT_LOAD;
         }
      }
      selections.push_back(value);
   }
   return selections;
}

/**
 * Get the new file's updated column names.
 */
std::vector<QString>
GuiLoadNodeAttributeFileColumnSelectionDialog::getNewFileColumnNames() const
{
   std::vector<QString> names;
   for (int i = 0; i < static_cast<int>(columnNameLineEdits.size()); i++) {
      names.push_back(columnNameLineEdits[i]->text());
   }
   return names;
}

/**
 * Get the erase all existing columns flag.
 */
bool 
GuiLoadNodeAttributeFileColumnSelectionDialog::getEraseAllExistingColumns() const
{
   if (eraseAllExistingColumnsCheckBox != NULL) {
      return eraseAllExistingColumnsCheckBox->isChecked();
   }
   return false;
}

/**
 * called when OK or Cancel button pressed.
 */
void 
GuiLoadNodeAttributeFileColumnSelectionDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      if (getEraseAllExistingColumns()) {
         std::vector<int> selections = getDestinationColumns();
         for (unsigned int i = 0; i < selections.size(); i++) {
            if (selections[i] >= 0) {
               QMessageBox::critical(this, "ERROR",
                                     "Since \"Erase All Existing Columns\" is selected,\n"
                                     "columns from new file must be loaded into new\n"
                                     "columns or not loaded.");
               return;
            }
         }
      }
   }
   QDialog::done(r);
}

/**
 * get append file comment selection.
 */
AbstractFile::FILE_COMMENT_MODE 
GuiLoadNodeAttributeFileColumnSelectionDialog::getAppendFileCommentSelection() const
{
   AbstractFile::FILE_COMMENT_MODE fcm = 
      static_cast<AbstractFile::FILE_COMMENT_MODE>(appendFileCommentComboBox->currentIndex());
   return fcm;
}

