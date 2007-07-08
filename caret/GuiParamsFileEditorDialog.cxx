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
#include <QScrollArea>
#include <QBoxLayout>

#include "BrainSet.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiParamsFileEditorDialog.h"
#include "ParamsFile.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiParamsFileEditorDialog::GuiParamsFileEditorDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Parameters File Editor");

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Grid for name/value line edits
   //
   paramsWidget = new QWidget;
   paramsGridLayout = new QGridLayout(paramsWidget);
   paramsGridLayout->setSpacing(3);
   
   //
   // Scrollview for entering the parameters
   //
   QScrollArea* paramsEntryScrollView = new QScrollArea(this);
   dialogLayout->addWidget(paramsEntryScrollView);
   paramsEntryScrollView->setWidget(paramsWidget);
   
   //
   // Column heading labels
   // 
   paramsGridLayout->addWidget(new QLabel("Name"), 0, 0);
   paramsGridLayout->addWidget(new QLabel("Value"), 0, 1);

   //
   // Button for new parameter
   //
   QPushButton* addNewParameterButton = new QPushButton("Add New Parameter",
                                                        this);
   addNewParameterButton->setFixedSize(addNewParameterButton->sizeHint());
   addNewParameterButton->setAutoDefault(false);
   QObject::connect(addNewParameterButton, SIGNAL(clicked()),
                    this, SLOT(slotAddNewParameterButton()));
   dialogLayout->addWidget(addNewParameterButton);
                    
   //
   // Horizontal layout apply/close buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(3);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply", this);
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                  this, SLOT(slotApplyButton()));
                  
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close", this);
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                  this, SLOT(slotCloseButton()));
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   updateDialog();   
}

/**
 * Destructor.
 */
GuiParamsFileEditorDialog::~GuiParamsFileEditorDialog()
{
}

/**
 * called when add new parameter button is pressed.
 */
void 
GuiParamsFileEditorDialog::slotAddNewParameterButton()
{
   //
   // Number of parameters in dialog
   //
   const int numInDialog = static_cast<int>(keyLineEdits.size());
   
   //
   // Find first hidden parameter
   //
   int availableIndex = -1;
   for (int i = 0; i < numInDialog; i++) {
      if (keyLineEdits[i]->isHidden()) {
         availableIndex = i;
         break;
      }
   }
   
   //  
   // Can we just show a hidden item
   //
   if (availableIndex >= 0) {
      keyLineEdits[availableIndex]->show();
      keyLineEdits[availableIndex]->clear();
      valueLineEdits[availableIndex]->show();
      valueLineEdits[availableIndex]->clear();
   }
   else {
      QLineEdit* keyLE = new QLineEdit;
      keyLineEdits.push_back(keyLE);
      paramsGridLayout->addWidget(keyLE, numInDialog + 1, 0);
      QLineEdit* valueLE = new QLineEdit;
      valueLineEdits.push_back(valueLE);
      paramsGridLayout->addWidget(valueLE, numInDialog + 1, 1);
      keyLineEdits[numInDialog]->show();
      valueLineEdits[numInDialog]->show();
   }
/*   
   //
   // Scroll to bottom
   //
   QScrollBar* vertScrollBar = paramsEntryScrollView->verticalScrollBar();
   vertScrollBar->setValue(vertScrollBar->maxValue());
*/
   paramsWidget->adjustSize();
}

/**
 * update the dialog.
 */
void 
GuiParamsFileEditorDialog::updateDialog()
{
   //
   // Get the parameters
   //
   std::vector<QString> keys, values;
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   pf->getAllParameters(keys, values);
   const int numParams = static_cast<int>(keys.size());
   
   //
   // Number of parameters in dialog
   //
   const int numInDialog = static_cast<int>(keyLineEdits.size());
   
   //
   // Add new parameters as needed
   //
   for (int i = numInDialog; i < numParams; i++) {
      QLineEdit* keyLE = new QLineEdit;
      paramsGridLayout->addWidget(keyLE, i + 1, 0);
      keyLineEdits.push_back(keyLE);
      QLineEdit* valueLE = new QLineEdit;
      paramsGridLayout->addWidget(valueLE, i + 1, 1);
      valueLineEdits.push_back(valueLE);
   }
   
   //
   // Hide unused items
   //
   for (int i = numParams; i < numInDialog; i++) {
      keyLineEdits[i]->hide();
      keyLineEdits[i]->clear();
      valueLineEdits[i]->hide();
      valueLineEdits[i]->clear();
   }

   //
   // Update params keys and values 
   //
   for (int i = 0; i < numParams; i++) {
      keyLineEdits[i]->setText(keys[i]);
      keyLineEdits[i]->show();
      valueLineEdits[i]->setText(values[i]);
      valueLineEdits[i]->show();
   }
   
   paramsWidget->adjustSize();
}

/**
 * called when apply button is pressed.
 */
void 
GuiParamsFileEditorDialog::slotApplyButton()
{
   //
   // Get all items from the dialog and put in a new parameters file
   //
   ParamsFile pfTemp;
   const int newNum = static_cast<int>(keyLineEdits.size());
   for (int i = 0; i < newNum; i++) {
      if (keyLineEdits[i]->isHidden() == false) {
         const QString key(keyLineEdits[i]->text());
         const QString value(valueLineEdits[i]->text());
         if (key.isEmpty() == false) {
            pfTemp.setParameter(key, value);
         }
      }
   }

   //
   // Get the current parameters file
   //
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   
   //
   // Have the parameters changed ?
   //
   if (pf->getAllParameters() != pfTemp.getAllParameters()) {
      //
      // Update the parameters
      //
      pf->setAllParameters(pfTemp.getAllParameters());
      
      //
      // Note theMainWindow->fileModificationUpdate(fm); will call GuiPaletteEditor::updateDialog()
      //
      GuiFilesModified fm;
      fm.setPaletteModified();
      theMainWindow->fileModificationUpdate(fm);
   }
}

/**
 * called when close button is pressed.
 */
void 
GuiParamsFileEditorDialog::slotCloseButton()
{
   slotApplyButton();
   close();
}
