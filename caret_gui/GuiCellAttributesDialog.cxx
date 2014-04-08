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


#include <iostream>
#include <set>

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

#include "BrainSet.h"
#include "CellProjectionFile.h"
#include "GuiCellAttributesDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "QtUtilities.h"
#include "global_variables.h"

static int 
operator<(const GuiCellAttributesDialog::CellAttributes& b1,
          const GuiCellAttributesDialog::CellAttributes& b2)
{
   return (b1.name < b2.name);
}

/**
 * Constructor.
 */
GuiCellAttributesDialog::GuiCellAttributesDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setAttribute(Qt::WA_DeleteOnClose);
   
   setWindowTitle("Edit Cell Attributes");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setSpacing(5);
   dialogLayout->setMargin(5);
   
   //
   // Consolidate the cell attributes
   //
   setupCellAttributes();
   const int numAttributes = static_cast<int>(attributes.size());
   int numColumns = 0;
   const int RETAIN_COLUMN      = numColumns++;
   const int NAME_COLUMN        = numColumns++;
   int rowNumber = 0;
   
   //
   // widget and layout in the scrollview
   //
   QWidget* svWidget = new QWidget;
   QGridLayout* grid = new QGridLayout(svWidget);
   grid->setMargin(3);
   grid->setSpacing(3);
   
   //
   // column titles
   //
   grid->addWidget(new QLabel("Keep"), rowNumber, RETAIN_COLUMN, Qt::AlignLeft);
   grid->addWidget(new QLabel("Name"), rowNumber, NAME_COLUMN, Qt::AlignLeft);
   rowNumber++;
   
   //
   // Cell data
   //
   for (int i = 0; i < numAttributes; i++) {
      // 
      // Retain checkbox
      //
      QCheckBox* checkBox = new QCheckBox("");
      checkBox->setChecked(true);
      grid->addWidget(checkBox, rowNumber, RETAIN_COLUMN, Qt::AlignLeft);
      retainCheckBoxes.push_back(checkBox);
      
      //
      // Name line edit
      //
      QLineEdit* nameLE = new QLineEdit;
      nameLE->setMinimumWidth(300);
      nameLE->setText(attributes[i].name);
      grid->addWidget(nameLE, rowNumber, NAME_COLUMN, Qt::AlignLeft);
      nameLineEdits.push_back(nameLE);
      originalNames.push_back(attributes[i].name);      
      
      rowNumber++;
   }
   
   //
   // Scroll View for all selections
   //
   QScrollArea* sv = new QScrollArea(this);
   sv->setWidget(svWidget);
   dialogLayout->addWidget(sv);

   //
   // Set the minimum size for the scroll area
   //
   int minWidth = svWidget->sizeHint().width() + 10;
   if (minWidth > 600) minWidth = 600;
   sv->setMinimumWidth(minWidth);
   int minHeight = svWidget->sizeHint().height();
   if (minHeight > 200) minHeight = 200;
   sv->setMinimumHeight(minHeight);
   
   //
   // Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* applyButton = new QPushButton("Apply");
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   buttonsLayout->addWidget(applyButton);
   
   QPushButton* closeButton = new QPushButton("Close");
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   buttonsLayout->addWidget(closeButton);
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);   
}

/**
 * Destructor.
 */
GuiCellAttributesDialog::~GuiCellAttributesDialog()
{
}

/**
 * Setup the cell attributes.
 */
void
GuiCellAttributesDialog::setupCellAttributes()
{
   //
   // Set to keep cell names sorted
   //
   std::set<CellAttributes> sortedCellAttributes;
   
   CellProjectionFile* cpf = theMainWindow->getBrainSet()->getCellProjectionFile();
   const int numCells = cpf->getNumberOfCellProjections();
   for (int j = 0; j < numCells; j++) {
      CellProjection* c = cpf->getCellProjection(j);
      
      CellAttributes ba(c->getName());
      
      sortedCellAttributes.insert(ba);
   }
   
   attributes.insert(attributes.end(), 
                     sortedCellAttributes.begin(), sortedCellAttributes.end());
}

/**
 * Get the index into the dialog arrays for the cell's original name.
 */
int
GuiCellAttributesDialog::getIndexFromOriginalName(const QString& name) const
{
   const int num = static_cast<int>(originalNames.size());
   for (int i = 0; i < num; i++) {
      if (originalNames[i] == name) {
         return i;
      }
   }
   return -1;
}

/**
 * Apply button slot.
 */
void
GuiCellAttributesDialog::slotApplyButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   CellProjectionFile* cpf = theMainWindow->getBrainSet()->getCellProjectionFile();
   const int numCells = cpf->getNumberOfCellProjections();
   for (int j = numCells - 1; j >= 0; j--) {
      CellProjection* c = cpf->getCellProjection(j);

      //
      // Find index of this cell
      //
      const int index = getIndexFromOriginalName(c->getName());
      if (index >= 0) {
         if (retainCheckBoxes[index]->isChecked()) {
            //
            // Update the cell
            //
            c->setName(nameLineEdits[index]->text());
         }
         else {
            //
            // Delete the cell
            //
            cpf->deleteCellProjection(j);
         }
      }
      else {
         std::cerr << "Program Error: original name " << c->getName().toAscii().constData()
                   << " not found at " << __LINE__ << " in " << __FILE__ << std::endl;
      }
   }
   
   //
   // Since names may have changed change the original names
   //
   const int num = static_cast<int>(originalNames.size());
   for (int i = 0; i < num; i++) {
      originalNames[i] = nameLineEdits[i]->text();
   }
   
   GuiFilesModified fm;
   fm.setCellModified();
   theMainWindow->fileModificationUpdate(fm);
   
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
}

