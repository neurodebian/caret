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

#include <cmath>
#include <iostream>

#include <QApplication>
#include <QContextMenuEvent>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextStream>

#include "StringTable.h"
#include "QtListBoxSelectionDialog.h"
#include "QtRadioButtonSelectionDialog.h"
#include "QtTableDialog.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
QtTableDialog::QtTableDialog(QWidget* parent, 
                             const QString& title,
                             const StringTable& dataTable)
   : QtDialog(parent, false)  
{
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(title);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(5);
   
   //
   // Add the table and load it with data
   //
   const int nr = dataTable.getNumberOfRows();
   const int nc = dataTable.getNumberOfColumns();
   table = new QTableWidget(nr, nc);
   for (int i = 0; i < nr; i++) {
      for (int j = 0; j < nc; j++) {
         table->setItem(i, j, new QTableWidgetItem(dataTable.getElement(i, j)));
      }
   }
   dialogLayout->addWidget(table);
   
   //
   // Set the table's left margin
   //
   //const int numChars = static_cast<int>(std::log10(static_cast<double>(nr))) + 3;
   //table->setLeftMargin(table->fontMetrics().width(QString().fill('X', numChars)));
   
   //
   // Set the column titles for the table
   //
   columnNames.clear();
   for (int j = 0; j < nc; j++) {
      columnNames << dataTable.getColumnTitle(j);
   }
   table->setHorizontalHeaderLabels(columnNames);
   
   //
   // connect signals from QTable
   //
   //QObject::connect(table, SIGNAL(contextMenuEvent(QContextMenuEvent*)),
   //                 this, SLOT(slotContextMenu(QContextMenuEvent*)));
                    
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(5);
   
   //
   // Save as Text button
   //
   QPushButton* saveAsTextButton = new QPushButton("Save As Text...");
   buttonsLayout->addWidget(saveAsTextButton);
   saveAsTextButton->setAutoDefault(false);
   QObject::connect(saveAsTextButton, SIGNAL(clicked()),
                    this, SLOT(slotSaveAsTextButton()));
   //
   // Sort button
   //
   QPushButton* sortButton = new QPushButton("Sort...");
   buttonsLayout->addWidget(sortButton);
   sortButton->setAutoDefault(false);
   QObject::connect(sortButton, SIGNAL(clicked()),
                    this, SLOT(slotSortButton()));
                    
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   QtUtilities::makeButtonsSameSize(saveAsTextButton,
                                    closeButton, 
                                    sortButton);
}
              
/**
 * destructor.
 */
QtTableDialog::~QtTableDialog()
{
}

/**
 * called when save as text button is pressed.
 */
void 
QtTableDialog::slotSaveAsTextButton()
{
   //
   // Use file dialog to get name of file
   //
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setWindowTitle("Choose File for Export");
   fd.setFileMode(QFileDialog::AnyFile);
   fd.setDirectory(".");
   fd.setAcceptMode(QFileDialog::AcceptSave);
   const QString textFilter("Text Files (*.txt *.text)");
   QStringList filters;
   filters << textFilter << "All Files (*)";
   fd.selectFilter(textFilter);
   if (fd.exec() == QDialog::Accepted) {
      QString filename = fd.selectedFiles().at(0);
      if (filename.isEmpty() == false) {
         //
         // Add extension if needed
         //
         if (fd.selectedFilter() == textFilter) {
            if ((filename.endsWith(".txt", Qt::CaseInsensitive) == false) &&
                (filename.endsWith(".text", Qt::CaseInsensitive) == false)) {
               filename += ".txt";
            }
         }
         
         //
         // Get separator to place between elements in file
         //
         const QString colonString("colon");
         const QString commaString("comma");
         const QString semicolonString("semicolon");
         const QString spaceString("space");
         const QString tabString("tab");
         static QString selectedSeparator(semicolonString);
         std::vector<QString> separators;
         separators.push_back(colonString);
         separators.push_back(commaString);
         separators.push_back(semicolonString);
         separators.push_back(spaceString);
         separators.push_back(tabString);
         int defaultItem = 0;
         for (unsigned int i = 0; i < separators.size(); i++) {
            if (selectedSeparator == separators[i]) {
               defaultItem = i;
            }
         }
         
         //
         // Allow user to choose separator
         //
         QApplication::beep();
         QtRadioButtonSelectionDialog rbd(this,
                                          "Choose Separator",
                                          "Choose separator that is placed\n"
                                             "between each element in the file.",
                                          separators,
                                          defaultItem);
         if (rbd.exec() == QDialog::Accepted) {
            selectedSeparator = separators[rbd.getSelectedItemIndex()];
            QString separator(";");
            if (selectedSeparator == colonString) {
               separator = ":";
            }
            else if (selectedSeparator == commaString) {
               separator = ",";
            }
            else if (selectedSeparator == semicolonString) {
               separator = ";";
            }
            else if (selectedSeparator == spaceString) {
               separator = " ";
            }
            else if (selectedSeparator == tabString) {
               separator = "\t";
            }
            
            //
            // Write the data to the file
            //
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly)) {
               QTextStream stream(&file);
               
               for (int j = 0; j < table->columnCount(); j++) {
                  if (j > 0) {
                     stream << separator;
                  }
                  stream << table->horizontalHeaderItem(j)->text();
               }
               stream << "\n";
               
               for (int i = 0; i < table->rowCount(); i++) {
                  for (int j = 0; j < table->columnCount(); j++) {
                     if (j > 0) {
                        stream << separator;
                     }
                     stream << table->item(i, j)->text();
                  }
                  stream << "\n";
               }
            }
            else {
               QApplication::beep();
               QString msg("Unable to open " + filename + " for writing.");
               QMessageBox::critical(this, "ERROR", msg, "OK");
               return;
            }
         }
      }
   }                  
}
      
/**
 * called when sort button is pressed.
 */
void 
QtTableDialog::slotSortButton()
{
   QtListBoxSelectionDialog lbsd(this,
                                 "Choose Column for Sorting",
                                 "Select the column for sorting",
                                 columnNames,
                                 -1);
   if (lbsd.exec() == QDialog::Accepted) {
      const int column = lbsd.getSelectedItemIndex();
      if (column >= 0) {
         table->sortByColumn(column);
      }
   }
}
      
/**
 * called when context menu requested.
 *
void 
QtTableDialog::slotContextMenu(QContextMenuEvent* e)
{
   //std::cout << "Context Menu: " << row << ", " << col << std::endl;
}
  */    
