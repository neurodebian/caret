
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

#include <QAction>
#include <QApplication>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSet>
#include <QSignalMapper>
#include <QToolButton>

#include "AreaColorFile.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiPaintNameEditorDialog.h"
#include "NameIndexSort.h"
#include "PaintFile.h"
#include "PreferencesFile.h"
#include "WuQDataEntryDialog.h"
#include "WuQWidgetGroup.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiPaintNameEditorDialog::GuiPaintNameEditorDialog(QWidget* parent)
   : WuQDialog(parent)
{
   addColumnTitlesToPaintNameLayoutFlag = true;
   
   setWindowTitle("Paint Name and Attributes Editor");
   
   //
   // Colunn selection control
   //
   columnSelectionComboBox =
      new GuiNodeAttributeColumnSelectionComboBox(theMainWindow->getBrainSet()->getPaintFile(),
                                                  false,
                                                  false,
                                                  true);
   QObject::connect(columnSelectionComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotColumnSelectionComboBox(int)));     
   QGroupBox* columnGroupBox = new QGroupBox("Column Selection");
   QHBoxLayout* columnSelectionLayout = new QHBoxLayout(columnGroupBox);
   columnSelectionLayout->addWidget(columnSelectionComboBox);
       
   //
   // widget and grid layout for paint names
   // Use nested layouts to keep widgets to the left and top
   //
   paintNameGridLayout = new QGridLayout;
   QWidget* paintNameWidget = new QWidget;
   QVBoxLayout* paintNameLayout = new QVBoxLayout(paintNameWidget);
   paintNameLayout->addLayout(paintNameGridLayout);
   paintNameLayout->setAlignment(paintNameGridLayout, Qt::AlignLeft | Qt::AlignTop);
   
   //
   // Scroll area for paint names
   //
   QScrollArea* nameScrollArea = new QScrollArea;
   nameScrollArea->setWidget(paintNameWidget);
   nameScrollArea->setWidgetResizable(true);
   
   //
   // Groupbox for paint name scroll area
   //
   QGroupBox* paintNameGroupBox = new QGroupBox("Paint Names");
   QVBoxLayout* paintNameGroupBoxLayout = new QVBoxLayout(paintNameGroupBox);
   paintNameGroupBoxLayout->addWidget(nameScrollArea);
   
   //
   // Add new Paint Name button
   //
   QPushButton* addPaintNameButton = new QPushButton("Add New Paint Name...");
   addPaintNameButton->setAutoDefault(false);
   addPaintNameButton->setFixedSize(addPaintNameButton->sizeHint());
   QObject::connect(addPaintNameButton, SIGNAL(clicked()),
                    this, SLOT(slotAddPaintName()));
   //
   // Remove prefix from names
   //
   QPushButton* removeNamePrefixesButton = new QPushButton("Remove Name Prefixes");
   removeNamePrefixesButton->setAutoDefault(false);
   removeNamePrefixesButton->setFixedSize(removeNamePrefixesButton->sizeHint());
   QObject::connect(removeNamePrefixesButton, SIGNAL(clicked()),
                    this, SLOT(slotRemovePrefixesFromPaintNames()));

   //
   // Remove suffix from names
   //
   QPushButton* removeNameSuffixesButton = new QPushButton("Remove Name Suffixes");
   removeNameSuffixesButton->setAutoDefault(false);
   removeNameSuffixesButton->setFixedSize(removeNameSuffixesButton->sizeHint());
   QObject::connect(removeNameSuffixesButton, SIGNAL(clicked()),
                    this, SLOT(slotRemoveSuffixesFromPaintNames()));

   //
   // Layout for name buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(addPaintNameButton);
   buttonsLayout->addWidget(removeNamePrefixesButton);
   buttonsLayout->addWidget(removeNameSuffixesButton);
   
   //
   // Standard Dialog Buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close,
                                                      Qt::Horizontal);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
                    
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(columnGroupBox);
   dialogLayout->addWidget(paintNameGroupBox);
   dialogLayout->addLayout(buttonsLayout);
   dialogLayout->addWidget(buttonBox);
   
   //
   // Create signal mappers
   //
   nameButtonsSignalMapper = new QSignalMapper(this);
   QObject::connect(nameButtonsSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotNameButtonClicked(int)));
   deleteButtonsSignalMapper = new QSignalMapper(this);
   QObject::connect(deleteButtonsSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotDeleteButtonClicked(int)));
   deassignButtonsSignalMapper = new QSignalMapper(this);
   QObject::connect(deassignButtonsSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotDeassignButtonClicked(int)));
   reassignButtonsSignalMapper = new QSignalMapper(this);
   QObject::connect(reassignButtonsSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotReassignButtonClicked(int)));
   editColorButtonsSignalMapper = new QSignalMapper(this);
   QObject::connect(editColorButtonsSignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotEditColorButtonClicked(int)));

   //
   // load names into dialog
   //    
   loadPaintNames();
}

/**
 * destructor.
 */
GuiPaintNameEditorDialog::~GuiPaintNameEditorDialog()
{
    PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
    pf->removePrefixesAndSuffixesFromNames(true, false);
    this->loadPaintNames();
}
      
/**
 * called to remove prefixes from names.
 */
void
GuiPaintNameEditorDialog::slotRemovePrefixesFromPaintNames()
{
    PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
    pf->removePrefixesAndSuffixesFromNames(true, false);
    this->loadPaintNames();
    this->updateGUI();
}

/**
 * called to remove suffixes from names.
 */
void
GuiPaintNameEditorDialog::slotRemoveSuffixesFromPaintNames()
{
    PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
    pf->removePrefixesAndSuffixesFromNames(false, true);
    this->loadPaintNames();
    this->updateGUI();
}

/**
 * update the dialog.
 */
void 
GuiPaintNameEditorDialog::updateDialog()
{
   columnSelectionComboBox->updateComboBox();
   loadPaintNames();
}
      
/**
 * called to add a new paint name.
 */
void 
GuiPaintNameEditorDialog::slotAddPaintName()
{
   WuQDataEntryDialog ded(this);
   QLineEdit* le = ded.addLineEditWidget("New Paint Name");
   if (ded.exec() == WuQDataEntryDialog::Accepted) {
      QString name = le->text().trimmed();
      if (name.isEmpty() == false) {
         PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
         if (pf->getPaintIndexFromName(name) >= 0) {
            QMessageBox::critical(this,
                                  "ERROR",
                                  "Name \""
                                  + name
                                  + "\" is already in paint file.");
            return;
         }

         pf->addPaintName(name);         
         updateGUI();
      }
   }
}

/**
 * called when a column is selected.
 */
void 
GuiPaintNameEditorDialog::slotColumnSelectionComboBox(int /*col*/)
{
   loadPaintNames();
}

/**
 * load the paint names.
 */
void 
GuiPaintNameEditorDialog::loadPaintNames()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   NameIndexSort nameSort;
   PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
   const int numPaints = pf->getNumberOfPaintNames();
   
   //
   // Get the paint names
   //
   if (columnSelectionComboBox->getAllSelected()) {
      for (int i = 0; i < numPaints; i++) {
         nameSort.add(i, pf->getPaintNameFromIndex(i));
      }
   }
   else {
      const int col = columnSelectionComboBox->currentIndex();
      if ((col >= 0) && (col < pf->getNumberOfColumns())) {
         std::vector<int> indices;
         pf->getPaintNamesForColumn(col, indices);
         
         for (unsigned int i = 0; i < indices.size(); i++) {
            nameSort.add(indices[i], pf->getPaintNameFromIndex(indices[i]));
         }
      }
   }
   
   //
   // Sort the names alphabetically
   //
   nameSort.sortByNameCaseSensitive();
   const int numberOfPaintNamesForDisplay = nameSort.getNumberOfItems();
   
   //
   // Add to grid layout for names
   //
   int GRID_COLUMN_COUNT = 0;
   const int indexCol = GRID_COLUMN_COUNT++;
   const int nameCol = GRID_COLUMN_COUNT++;
   const int countCol = GRID_COLUMN_COUNT++;
   const int colorCol = GRID_COLUMN_COUNT++;
   const int editColorCol = GRID_COLUMN_COUNT++;
   const int deassignCol = GRID_COLUMN_COUNT++;
   const int reassignCol = GRID_COLUMN_COUNT++;
   const int deleteCol = GRID_COLUMN_COUNT++;
   
   if (addColumnTitlesToPaintNameLayoutFlag) {
      addColumnTitlesToPaintNameLayoutFlag = false;
      
      //
      // Add column titles
      //
      paintNameGridLayout->addWidget(new QLabel("Node"), 0, indexCol);
      paintNameGridLayout->addWidget(new QLabel("Value"), 1, indexCol);
      paintNameGridLayout->addWidget(new QLabel("Name"), 1, nameCol);
      paintNameGridLayout->addWidget(new QLabel("Node"), 0, countCol);
      paintNameGridLayout->addWidget(new QLabel("Count"), 1, countCol);
      paintNameGridLayout->addWidget(new QLabel("Color"), 1, colorCol);
   }
   const int numberOfPaintsInDialog = static_cast<int>(paintFileIndices.size());
   
   //
   // Create new rows
   //
   for (int i = numberOfPaintsInDialog; i < numberOfPaintNamesForDisplay; i++) {
      const int rowNumber = i + 2;  // 1st row is column titles
      
      //
      // Create the widgets for the row
      //
      QLabel* countLabel =new QLabel("");
      countLabel->setToolTip("This count represents the number \n"
                             "of nodes using the paint name.");
      QLabel* indexLabel = new QLabel("");
      indexLabel->setToolTip("This number is the value assigned\n"
                             "to nodes using this paint name.");
      QLabel* colorWidget = new QLabel("");
      colorWidget->setFixedWidth(50);
      
      //
      // Name button
      //
      QAction* nameButtonAction = new QAction("", this);
      QToolButton* nameToolButton = new QToolButton;
      nameToolButton->setDefaultAction(nameButtonAction);
      nameToolButton->setToolTip("Change the name\n"
                                 "of the paint.");
      QObject::connect(nameButtonAction, SIGNAL(triggered(bool)),
                       nameButtonsSignalMapper, SLOT(map()));
      nameButtonsSignalMapper->setMapping(nameButtonAction, i);
      
      //
      // Delete button
      //
      QAction* deleteButtonAction = new QAction("X", this);
      QToolButton* deleteToolButton = new QToolButton;
      deleteToolButton->setDefaultAction(deleteButtonAction);
      deleteToolButton->setToolTip("Delete the paint.  All nodes that\n"
                                   "were using the paint name are \n"
                                   "deassigned for all paint columns.");
      QObject::connect(deleteButtonAction, SIGNAL(triggered(bool)),
                       deleteButtonsSignalMapper, SLOT(map()));
      deleteButtonsSignalMapper->setMapping(deleteButtonAction, i);
      
      //
      // Deassign button
      //
      QAction* deassignButtonAction = new QAction("D", this);
      QToolButton* deassignToolButton = new QToolButton;
      deassignToolButton->setDefaultAction(deassignButtonAction);
      deassignToolButton->setToolTip("Deassign all nodes using the paint \n"
                                     "name in  the paint column(s) selected\n"
                                     "at the top of this dialog.");
      QObject::connect(deassignButtonAction, SIGNAL(triggered(bool)),
                       deassignButtonsSignalMapper, SLOT(map()));
      deassignButtonsSignalMapper->setMapping(deassignButtonAction, i);
      
      //
      // Reassign button
      //
      QAction* reassignButtonAction = new QAction("R", this);
      QToolButton* reassignToolButton = new QToolButton;
      reassignToolButton->setDefaultAction(reassignButtonAction);
      reassignToolButton->setToolTip("Reassign all nodes using the paint \n"
                                     "name in the paint column(s) selected\n"
                                     "at the top of the dialog.");
      QObject::connect(reassignButtonAction, SIGNAL(triggered(bool)),
                       reassignButtonsSignalMapper, SLOT(map()));
      reassignButtonsSignalMapper->setMapping(reassignButtonAction, i);
      
      //
      // Edit color button
      //
      QAction* editColorButtonAction = new QAction("Edit Color", this);
      QToolButton* editColorToolButton = new QToolButton;
      editColorToolButton->setDefaultAction(editColorButtonAction);
      editColorToolButton->setToolTip("Edit the best matching\n"
                                      "color for this paint name.");
      QObject::connect(editColorButtonAction, SIGNAL(triggered(bool)),
                       editColorButtonsSignalMapper, SLOT(map()));
      editColorButtonsSignalMapper->setMapping(editColorButtonAction, i);
      
      //
      // keep pointers to the row's widgets
      //
      paintFileIndices.push_back(i);
      paintNameActions.push_back(nameButtonAction);
      paintIndexLabels.push_back(indexLabel);
      paintCountLabels.push_back(countLabel);
      paintColorLabels.push_back(colorWidget);
      
      //
      // Add the widgets to the layout
      //
      paintNameGridLayout->addWidget(indexLabel, rowNumber, indexCol);
      paintNameGridLayout->addWidget(nameToolButton, rowNumber, nameCol);
      paintNameGridLayout->addWidget(countLabel, rowNumber, countCol);
      paintNameGridLayout->addWidget(colorWidget, rowNumber, colorCol);
      paintNameGridLayout->addWidget(editColorToolButton, rowNumber, editColorCol);
      paintNameGridLayout->addWidget(deassignToolButton, rowNumber, deassignCol);
      paintNameGridLayout->addWidget(reassignToolButton, rowNumber, reassignCol);
      paintNameGridLayout->addWidget(deleteToolButton, rowNumber, deleteCol);
      
      //
      // Widget group for all items in row
      //
      WuQWidgetGroup* wg = new WuQWidgetGroup(this);
      wg->addWidget(indexLabel);
      wg->addWidget(nameToolButton);
      wg->addWidget(countLabel);
      wg->addWidget(colorWidget);
      wg->addWidget(editColorToolButton);
      wg->addWidget(deassignToolButton);
      wg->addWidget(reassignToolButton);
      wg->addWidget(deleteToolButton);
      rowWidgetGroups.push_back(wg);
   }
   
   const AreaColorFile* colorFile = theMainWindow->getBrainSet()->getAreaColorFile();
   
   //
   // Paint counts
   //
   std::vector<int> paintCounts;
   pf->getAllPaintCounts(paintCounts);
   const int numPaintCounts = paintCounts.size();
   
   //
   // Update existing rows
   //
   for (int i = 0; i < numberOfPaintNamesForDisplay; i++) {
      int indx;
      QString name;
      nameSort.getSortedNameAndIndex(i, indx, name);
      
      paintFileIndices[i] = indx;
      paintIndexLabels[i]->setText(QString::number(indx));
      paintNameActions[i]->setText(name);
      paintNameActions[i]->setToolTip("Change the name\n"
                                      "of this paint paint.");
      
      int paintNameCount = 0;
      if ((indx >= 0) &&
          (indx < numPaintCounts)) {
         paintNameCount = paintCounts[indx];
      }
      paintCountLabels[i]->setText(QString::number(paintNameCount));


      QColor labelColor(170, 170, 170);
      bool autoFill = false;
      QPalette::ColorRole backgroundRole = QPalette::NoRole;
            
      bool match = false;
      unsigned char r, g, b;
      const int colorIndex = colorFile->getColorByName(name, match, r, g, b);
      if (colorIndex >= 0) {
         labelColor.setRgb(r, g, b, 255);
         backgroundRole = QPalette::Window;
         autoFill = true;
         paintColorLabels[i]->setText("    ");
      }
      else {
         paintColorLabels[i]->setText("None");
      }
      
      QPalette pal;
      pal.setColor(QPalette::Window, labelColor);
      paintColorLabels[i]->setAutoFillBackground(autoFill);
      paintColorLabels[i]->setBackgroundRole(backgroundRole);
      paintColorLabels[i]->setPalette(pal);
      paintColorLabels[i]->repaint();
      
      rowWidgetGroups[i]->setVisible(true);
   }
   
   //
   // Remove rows not needed
   //
   for (int i = numberOfPaintNamesForDisplay; i < numberOfPaintsInDialog; i++) {
      rowWidgetGroups[i]->setVisible(false);
   }
   
   QApplication::restoreOverrideCursor();
}
      
/** 
 * called when a name button is clicked.
 */
void 
GuiPaintNameEditorDialog::slotNameButtonClicked(int indx)
{
   if ((indx >= 0) &&
       (indx < static_cast<int>(paintFileIndices.size()))) {
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      const QString oldName = pf->getPaintNameFromIndex(paintFileIndices[indx]);
      
      WuQDataEntryDialog ded(this);
      QLineEdit* le = ded.addLineEditWidget("Edit Paint Name", oldName);
      if (ded.exec() == WuQDataEntryDialog::Accepted) {
         const QString newName = le->text().trimmed();
         if (newName.isEmpty() == false) {
            if (newName != oldName) {
               PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
               if (pf->getPaintIndexFromName(newName) >= 0) {
                  QMessageBox::critical(this,
                                        "ERROR",
                                        "Name \""
                                        + newName
                                        + "\" is already in paint file.\n"
                                          "Use the reassign \"R\" button to change the name.");
                  return;
               }

               pf->setPaintName(paintFileIndices[indx], newName);  
               
               /*
                * If needed, copy old color to a new color.
                */
               AreaColorFile* acf = theMainWindow->getBrainSet()->getAreaColorFile();
               bool match = false;
               acf->getColorIndexByName(newName, match);
               if (match == false) {
                  unsigned char r, g, b, a;
                  acf->getColorByName(oldName, match,
                                                          r, g, b, a);
                  acf->addColor(newName, r, g, b, a);
               }
               updateGUI();
            }
         }
      }
   }
}
            
/**
 * called when delete button is clicked.
 */
void 
GuiPaintNameEditorDialog::slotDeleteButtonClicked(int indx)
{
   if ((indx >= 0) &&
       (indx < static_cast<int>(paintFileIndices.size()))) {
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      pf->deletePaintName(paintFileIndices[indx]);
      updateGUI();
   }
}

/**
 * called when deassign button is clicked.
 */
void 
GuiPaintNameEditorDialog::slotDeassignButtonClicked(int indx)
{
   if ((indx >= 0) &&
       (indx < static_cast<int>(paintFileIndices.size()))) {
      const int columnNumber = columnSelectionComboBox->currentIndex();
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      pf->deassignPaintName(columnNumber,
                            paintFileIndices[indx]);
                            
      updateGUI();
   }
}

/**
 * called when reassign button is clicked.
 */
void 
GuiPaintNameEditorDialog::slotReassignButtonClicked(int indx)
{
   if ((indx >= 0) &&
       (indx < static_cast<int>(paintFileIndices.size()))) {
      //
      // Get a list of unique paint names
      //
      const int columnNumber = columnSelectionComboBox->currentIndex();
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      std::vector<QString> paintNames;
      std::vector<int> paintNameIndices;
      pf->getAllPaintNamesAndIndices(paintNames, paintNameIndices);
      const int numNames = static_cast<int>(paintNames.size());
      QStringList names;
      for (int i = 0; i < numNames; i++) {
         names << (paintNames[i] 
                   + "  [" 
                   + QString::number(paintNameIndices[i])
                   + "]");
      }
     
      //
      // Let user choose the new paint name for reassignment
      //
      WuQDataEntryDialog ed(this);
      ed.setWindowTitle("Choose Reassignment Name");
      ed.setTextAtTop("Choose New Name", false);
      QListWidget* lw = ed.addListWidget("", names);
      if (ed.exec() == WuQDataEntryDialog::Accepted) {
         const int itemIndex = lw->currentRow();
         if ((itemIndex >= 0) &&
             (itemIndex < lw->count())) {            
            const int newIndex = paintNameIndices[itemIndex];
            if (newIndex >= 0) {
               pf->reassignPaintName(columnNumber,
                                     paintFileIndices[indx],
                                     newIndex);
                                     
               updateGUI();
            }
         }
      }
   }
}

/**
 * called when edit color button is clicked.
 */
void 
GuiPaintNameEditorDialog::slotEditColorButtonClicked(int indx)
{
   if ((indx >= 0) &&
       (indx < static_cast<int>(paintFileIndices.size()))) {
      // 
      // Get index for color
      //
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      const QString name = pf->getPaintNameFromIndex(paintFileIndices[indx]);
      AreaColorFile* acf = theMainWindow->getBrainSet()->getAreaColorFile();
      bool match = false;
      unsigned char r, g, b, a;
      int colorIndex = acf->getColorByName(name, match, r, g, b, a);
      
      //
      // Was the color or partially matching color found
      //
      QString colorName;
      bool createNewColorFlag = false;
      if (colorIndex >= 0) {
         if (match) {
            //
            // Exact match found
            //
            colorName = name;
         }
         else {
            //
            // Partial match found, allow user to choose to edit the partially
            // matching color or create a new color that is an exact match
            //
            const QString partialMatchColor(acf->getColorNameByIndex(colorIndex));
            const QString msg =
               ("The partially matching color named "
                + partialMatchColor
                + "\n"
                   "is being used for "
                + name
                + ".");
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Color Name");
            msgBox.setText(msg);
            QPushButton* modifyButton = 
               msgBox.addButton("Modify Color " + partialMatchColor,
                                QMessageBox::AcceptRole);
            QPushButton* defineButton = 
               msgBox.addButton("Define New Color " + name,
                                QMessageBox::AcceptRole);
            msgBox.addButton("Cancel", QMessageBox::RejectRole);
            msgBox.exec();
            if (msgBox.clickedButton() == modifyButton) {
               colorName = partialMatchColor;
            }
            else if (msgBox.clickedButton() == defineButton) {
               createNewColorFlag = true;
            }
            else {
               return;
            }
         }
      }
      else {
         //
         // No exact or partially matching color
         //
         createNewColorFlag = true;
      }
      
      if (createNewColorFlag) {
         colorName = name;
         PreferencesFile* pref = theMainWindow->getBrainSet()->getPreferencesFile();
         pref->getSurfaceForegroundColor(r, g, b);
         colorIndex = acf->addColor(name, r, g, b);
      }

      if (colorName.isEmpty() == false) {
         GuiColorSelectionDialog csd(this,
                                     "Edit Color " + colorName,
                                     false,
                                     false,
                                     false,
                                     false);
         csd.setColor(r, g, b, 255);
         if (csd.exec() == GuiColorSelectionDialog::Accepted) {
            float pointSize, lineSize;
            ColorFile::ColorStorage::SYMBOL symbol;
            csd.getColorInformation(r, g, b, a,
                                    pointSize,
                                    lineSize,
                                    symbol);
            acf->setColorByIndex(colorIndex, r, g, b);
         }

         updateGUI();
      }
   }
}
      
/**
 * update the GUI.
 */
void 
GuiPaintNameEditorDialog::updateGUI()
{
      theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
      GuiFilesModified fm;
      fm.setPaintModified();   
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
}

