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
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsRgbPaint.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiCurrentColoringToRgbPaintDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "QtUtilities.h"
#include "RgbPaintFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiCurrentColoringToRgbPaintDialog::GuiCurrentColoringToRgbPaintDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setModal(true);
   setWindowTitle("Copy Current Coloring to RGB Paint");
   
   //
   // Vertical box layout of all items
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(2);
   rows->setSpacing(3);
   
   //
   // Horizontal box for column selection and name
   //
   QHBoxLayout* columnLayout = new QHBoxLayout;
   rows->addLayout(columnLayout);
   columnLayout->setSpacing(5);
   columnLayout->addWidget(new QLabel("Column", this));
   rgbColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox( 
                                GUI_NODE_FILE_TYPE_RGB_PAINT, true, false, false);
   QObject::connect(rgbColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotRgbPaintColumnSelection(int)));
   columnLayout->addWidget(rgbColumnComboBox);
   columnNameLineEdit = new QLineEdit(this);
   columnNameLineEdit->setText("New Column Name");
   columnLayout->addWidget(columnNameLineEdit);
   
   //
   // Horizontal box for comment
   //
   QHBoxLayout* commentLayout = new QHBoxLayout;
   rows->addLayout(commentLayout); 
   commentLayout->setSpacing(5);
   commentLayout->addWidget(new QLabel("Comment", this));
   commentLineEdit = new QLineEdit(this);
   commentLayout->addWidget(commentLineEdit);
   
   //
   // initialize column selection
   //
   rgbColumnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   
   //
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   buttonsLayout->addWidget(okButton);
   
   //
   // Close button
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setAutoDefault(false);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   buttonsLayout->addWidget(cancelButton);
   
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
}

/**
 * Destructor.
 */
GuiCurrentColoringToRgbPaintDialog::~GuiCurrentColoringToRgbPaintDialog()
{
}

/**
 * Called when an RGB paint column is selected.
 */
void
GuiCurrentColoringToRgbPaintDialog::slotRgbPaintColumnSelection(int col)
{
   if (col >= 0) {
      RgbPaintFile* rpf = theMainWindow->getBrainSet()->getRgbPaintFile();
      columnNameLineEdit->setText(rpf->getColumnName(col));
      commentLineEdit->setText(rpf->getColumnComment(col));
   }
   else {
      columnNameLineEdit->setText("New Column Name");
      commentLineEdit->setText("");
   }
}

/**
 *  Called when dialog is being closed.
 */
void
GuiCurrentColoringToRgbPaintDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
      RgbPaintFile *rpf = theMainWindow->getBrainSet()->getRgbPaintFile();
      int columnNumber = rgbColumnComboBox->currentIndex();
      if (columnNumber < 0) {
         columnNumber = rpf->getNumberOfColumns();
         if (rpf->getNumberOfColumns() <= 0) {
            rpf->setNumberOfNodesAndColumns(numNodes, 1);
         }
         else {
            rpf->addColumns(1);
         }
      }
   
      rpf->setColumnName(columnNumber, columnNameLineEdit->text());
      rpf->setColumnComment(columnNumber, commentLineEdit->text());
      rpf->setScaleRed(columnNumber, 0, 255.0);
      rpf->setScaleGreen(columnNumber, 0, 255.0);
      rpf->setScaleBlue(columnNumber, 0, 255.0);
      
      BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
      const int modelIndex = theMainWindow->getBrainModelIndex();
      for (int i = 0; i < numNodes; i++) {
         const unsigned char* rgb = bsnc->getNodeColor(modelIndex, i);
         const float rgbf[3] = { rgb[0], rgb[1], rgb[2] };
         rpf->setRgb(i, columnNumber, rgbf[0], rgbf[1], rgbf[2]);
      }
   
      DisplaySettingsRgbPaint* dsrgb = theMainWindow->getBrainSet()->getDisplaySettingsRgbPaint();
      dsrgb->setSelectedDisplayColumn(-1, -1, columnNumber);
      
      GuiFilesModified fm;
      fm.setRgbPaintModified();
      theMainWindow->fileModificationUpdate(fm);
      bsnc->assignColors();
      GuiBrainModelOpenGL::updateAllGL(NULL);
      
      QApplication::restoreOverrideCursor();   
   }
   
   QDialog::done(r);
}

