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

#include <algorithm>
#include <functional>
#include <iostream>
#include <set>

#include <QAction>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QToolBar>
#include <QToolButton>

#include "CellFile.h"
#include "CellProjectionFile.h"
#include "FileUtilities.h"
#include "GuiCommaSeparatedValueFileEditor.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "QtTextEditDialog.h"
#include "StringTable.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiCommaSeparatedValueFileEditor::GuiCommaSeparatedValueFileEditor(QWidget* parent,
                                                     AbstractFile* dataFileIn,
                                                     const GuiFilesModified& filesModified)
   : WuQDialog(parent)
{
   //
   // Request that this dialog be deleted when closed
   //
   setAttribute(Qt::WA_DeleteOnClose);
   
   setWindowTitle("Edit File " + FileUtilities::basename(dataFileIn->getFileName()));
   
   //
   // The editor main window
   //
   editorMainWindow = new GuiCommaSeparatedValueFileEditorMainWindow(dataFileIn, filesModified);
      
   //
   // Get the layout for the dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);

   //
   // Layout dialog items
   //
   dialogLayout->addWidget(editorMainWindow);
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
   buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(false);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(slotCloseButtonPressed()));
}

/**
 * destructor.
 */
GuiCommaSeparatedValueFileEditor::~GuiCommaSeparatedValueFileEditor()
{
}

/**
 * enable/disable row addition and deletion.
 */
void 
GuiCommaSeparatedValueFileEditor::setRowAdditionDeletionEnabled(const bool enabled)
{
   editorMainWindow->setRowAdditionDeletionEnabled(enabled);
}

/**
 * called when close button pressed.
 */
void 
GuiCommaSeparatedValueFileEditor::slotCloseButtonPressed()
{
   editorMainWindow->checkForFileChanges();
   WuQDialog::close();
}      
      
//--------------------------------------------------------------------------------
/**
 * constructor.
 */
GuiCommaSeparatedValueFileEditorMainWindow::GuiCommaSeparatedValueFileEditorMainWindow(
                                                    AbstractFile* dataFileIn,
                                                    const GuiFilesModified& filesModifiedIn,
                                                    QWidget* parent)
   : QMainWindow(parent)
{
   filesModified = filesModifiedIn;
   dataFile = dataFileIn;
   dataFile->writeDataIntoCommaSeparatedValueFile(csvFile);

   //
   // the toolbar
   //
   QToolBar* toolBar = new QToolBar;
   addToolBar(toolBar);
   
   //
   // Add Row(s) toolbar button
   //
   QAction* addRowAction = new QAction("Add\nRow", this);
   addRowAction->setToolTip("Add a row after the\n"
                            "last selected row.");
   QObject::connect(addRowAction, SIGNAL(triggered()),
                    this, SLOT(addRow()));
   addRowToolButton = new QToolButton;
   addRowToolButton->setDefaultAction(addRowAction);
   toolBar->addWidget(addRowToolButton);
   
   //
   // Delete Row(s) toolbar button
   //
   QAction* deleteRowAction = new QAction("Delete\nRow(s)", this);
   deleteRowAction->setToolTip("Delete the\n"
                               "selected row(s)");
   QObject::connect(deleteRowAction, SIGNAL(triggered()),
                    this, SLOT(deleteRows()));
   deleteRowToolButton = new QToolButton;
   deleteRowToolButton->setDefaultAction(deleteRowAction);
   toolBar->addWidget(deleteRowToolButton);
   
   //
   // Widget stack for data sections
   //
   dataSectionsStackedWidget = new QStackedWidget;
   
   //
   // data section selections
   //
   QLabel* dataSectionLabel = new QLabel("Data Section");
   dataSectionComboBox = new QComboBox;
   QObject::connect(dataSectionComboBox, SIGNAL(activated(int)),
                    dataSectionsStackedWidget, SLOT(setCurrentIndex(int)));
   QHBoxLayout* dataSectionLayout = new QHBoxLayout;
   dataSectionLayout->addWidget(dataSectionLabel);
   dataSectionLayout->addWidget(dataSectionComboBox);
   dataSectionLayout->setStretchFactor(dataSectionLabel, 0);
   dataSectionLayout->setStretchFactor(dataSectionComboBox, 100);
   
   //
   // Add data sections to dialog
   //
   for (int i = 0; i < csvFile.getNumberOfDataSections(); i++) {
      addDataSection(csvFile.getDataSection(i));
   }
   
   //
   // Layout for main window
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(dataSectionLayout);
   layout->addWidget(dataSectionsStackedWidget);
   
   //
   // Set widget for the main window
   //
   setCentralWidget(w);
   
   //
   // See if data file can be read from comma separated file
   //
   bool readFromCSV, writeToCSV;
   dataFile->getCommaSeparatedFileSupport(readFromCSV, writeToCSV);
   
   //
   // enable/disable editing
   //
   fileEditingAllowed = false;
   if (readFromCSV) {
      //if ((dynamic_cast<CellFile*>(dataFile) != NULL) ||
      //    (dynamic_cast<CellProjectionFile*>(dataFile) != NULL)) {
         fileEditingAllowed = true;
      //}
   }
   setRowAdditionDeletionEnabled(fileEditingAllowed);
}

/**
 * destructor.
 */
GuiCommaSeparatedValueFileEditorMainWindow::~GuiCommaSeparatedValueFileEditorMainWindow()
{
}

/**
 * check for data file changes.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::checkForFileChanges()
{
   if (fileEditingAllowed) {
      const int num = static_cast<int>(tableWidgets.size());
      
      bool modifiedFlag = false;
      for (int i = 0; i < num; i++) {
         if (tableHasBeenModified[i]) {
            modifiedFlag = true;
         }
      }
      
      if (modifiedFlag) {
         if (QMessageBox::question(this, 
                                   "Apply Changes", 
                                   "Data has been modified.  Apply Changes?", 
                                   (QMessageBox::Yes | QMessageBox::No),
                                   QMessageBox::Yes)
                                      == QMessageBox::Yes) {
            for (int i = 0; i < num; i++) {
               if (tableHasBeenModified[i]) {
                  QTableWidget* tw = tableWidgets[i];
                  const int rows = tw->rowCount();
                  const int cols = tw->columnCount();
                  
                  StringTable* st = csvFile.getDataSection(i);
                  QVector<QString> columnTitles;
                  for (int j = 0; j < cols; j++) {
                     columnTitles.push_back(st->getColumnTitle(j));
                  }
                  st->setNumberOfRowsAndColumns(rows, cols, st->getTableTitle());
                  for (int j = 0; j < cols; j++) {
                     st->setColumnTitle(j, columnTitles.at(j));
                  }
                  for (int k = 0; k < rows; k++) {
                     for (int j = 0; j < cols; j++) {
                        st->setElement(k, j, tw->item(k, j)->text());
                     }
                  }
               }
            }
            
            dataFile->readDataFromCommaSeparatedValuesTable(csvFile);

            theMainWindow->fileModificationUpdate(filesModified);
            GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}
      
/**
 * enable/disable row addition and deletion.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::setRowAdditionDeletionEnabled(const bool enabled)
{
   addRowToolButton->setEnabled(enabled);
   deleteRowToolButton->setEnabled(enabled);
}
      
/**
 * called to add a row.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::addRow()
{
   QTableWidget* tw = dynamic_cast<QTableWidget*>(dataSectionsStackedWidget->currentWidget());
   if (tw != NULL) {
      int lastRow = -1;
      QList<QTableWidgetSelectionRange> sel = tw->selectedRanges();
      for (int i = 0; i < sel.size(); i++) {
         QTableWidgetSelectionRange twsr = sel.at(i);
         lastRow = std::max(lastRow, twsr.bottomRow());
      }
      
      if (lastRow >= 0) {
         tw->insertRow(lastRow+1);
         slotCellChanged(lastRow+1, 0);
      }
      else {
         QMessageBox::information(this, "INFO",
                  "A row must be selected before pressing the Add Row button. \n"
                  "Select a row by clicking on the row number in the left column.");
      }
   }
}

/**
 * called to delete a row.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::deleteRows()
{
   QTableWidget* tw = dynamic_cast<QTableWidget*>(dataSectionsStackedWidget->currentWidget());
   if (tw != NULL) {
      std::set<int, std::greater<int> > rowsToDelete;  // DESCENDING ORDER
      QList<QTableWidgetSelectionRange> sel = tw->selectedRanges();
      for (int i = 0; i < sel.size(); i++) {
         QTableWidgetSelectionRange twsr = sel.at(i);
         if (twsr.columnCount() == tw->columnCount()) {
            for (int j = twsr.topRow(); j <= twsr.bottomRow(); j++) {
               rowsToDelete.insert(j);
            }
         }
      }
      
      if (rowsToDelete.empty() == false) {
         for (std::set<int>::iterator iter = rowsToDelete.begin();
              iter != rowsToDelete.end();
              iter++) {
            //std::cout << "Deleting row: " << *iter << std::endl;
            slotCellChanged(*iter, 0);
            tw->removeRow(*iter);
         }
      }
      else {
         QMessageBox::information(this, "INFO", 
             "One or more rows must be selected before pressing the Delete Row(s)\n"  
             "button.  Select a row by clicking on the row number in the left column.");
      }
   }
}

/**
 * called if a cell's contents are changed.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::slotCellChanged(int /*row*/, int /*col*/)
{
   const int indx = dataSectionsStackedWidget->currentIndex();
   if ((indx >= 0) && (indx < tableHasBeenModified.size())) {
      tableHasBeenModified[indx] = true;
   }
   //std::cout << "Table modified" << std::endl;
}

/**
 * called if a cell is double clicked.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::slotTableCellDoubleClicked(int row,int col)
{
   QTableWidget* tw = dynamic_cast<QTableWidget*>(dataSectionsStackedWidget->currentWidget());
   if (tw != NULL) {
      //
      // Get the table widget item that was clicked
      //
      QTableWidgetItem* twi = tw->item(row, col);
      
      //
      // Load the text from the table's cell into a text editor
      //
      QtTextEditDialog te(this,
                          (fileEditingAllowed == false), // NOT read-only
                          true); // modal
      const QString cellText(twi->text());
      te.setText(cellText);
      if (te.exec() == QtTextEditDialog::Accepted) {
         const QString newText(te.getText());
         if (cellText != newText) {
            twi->setText(newText);
         }
      }
   }
}

/**
 * header selected.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::rowSelected(int /*row*/)
{
   //std::cout << "Selected row: " << row << std::endl;
/*
   QTableWidget* tw = dynamic_cast<QTableWidget*>(dataSectionsStackedWidget->currentWidget());
   if (tw != NULL) {
      QHeaderView* vertHeader = tw->verticalHeader();
      for (int i = 0; i < vertHeader->count(); i++) {
         const QTableWidgetItem* twi = tw->verticalHeaderItem(i);
         if (tw->isItemSelected(twi)) {
            std::cout << "Row " << i << " is selected." << std::endl;
         }
         else {
            std::cout << "Row " << i << " is NOT selected." << std::endl;
         }
      }
      std::cout << "Current Row: " << tw->currentRow() << std::endl;
      QList<QTableWidgetSelectionRange> sel = tw->selectedRanges();
      for (int i = 0; i < sel.size(); i++) {
         QTableWidgetSelectionRange twsr = sel.at(i);
         std::cout << "Selection range: " << std::endl;
         std::cout << "bottom row:      " << twsr.bottomRow() << std::endl;
         std::cout << "column count:    " << twsr.columnCount() << std::endl;
         std::cout << "left column:     " << twsr.leftColumn() << std::endl;
         std::cout << "right column:    " << twsr.rightColumn() << std::endl;
         std::cout << "row count:       " << twsr.rowCount() << std::endl;
         std::cout << "top row:         " << twsr.topRow() << std::endl;
      }
   }
*/
}
      
/**
 * add a data section to the dialog.
 */
void 
GuiCommaSeparatedValueFileEditorMainWindow::addDataSection(const StringTable* dataSection)
{
   //
   // Verify data in table
   //
   const int numRows = dataSection->getNumberOfRows();
   const int numCols = dataSection->getNumberOfColumns();
   if ((numRows <= 0) || (numCols <= 0)) {
      return;
   }
   
   //
   // Create a QTableWidget for the data
   //
   QTableWidget* tableWidget = new QTableWidget(numRows, numCols);
   tableWidgets.push_back(tableWidget);
   tableHasBeenModified.push_back(false);
   
   //
   // Display editor for double clicks
   //
   QObject::connect(tableWidget, SIGNAL(cellDoubleClicked(int,int)),
                    this, SLOT(slotTableCellDoubleClicked(int,int)));
                    
   //
   // Track changes to table widgets
   //
   QObject::connect(tableWidget, SIGNAL(cellChanged(int,int)),
                    this, SLOT(slotCellChanged(int,int)));
   
   //
   // row selected
   //
   QObject::connect(tableWidget->verticalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(rowSelected(int)));
   //
   // Set the column titles
   //
   QStringList columnLabels;
   for (int i = 0; i < numCols; i++) {
      columnLabels << dataSection->getColumnTitle(i);
   }
   tableWidget->setHorizontalHeaderLabels(columnLabels);
   
   //
   // Load the table
   //
   tableWidget->blockSignals(true);
   for (int i = 0; i < numRows; i++) {
      for (int j = 0; j < numCols; j++) {
         tableWidget->setItem(i, j, new QTableWidgetItem(dataSection->getElement(i, j)));
      }
   }
   tableWidget->blockSignals(false);
   
   //
   // Add to widget stack
   //
   dataSectionsStackedWidget->addWidget(tableWidget);
   
   //
   // add to combo box
   //
   QString name(dataSection->getTableTitle());
   if (name.isEmpty()) {
      name = "Data Item " + QString::number(dataSectionComboBox->count());
   }
   dataSectionComboBox->addItem(name);
}
      
