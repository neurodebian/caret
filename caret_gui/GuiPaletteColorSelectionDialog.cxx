
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

#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>

#include "GuiPaletteColorSelectionDialog.h"
#include "PaletteFile.h"
#include "QtUtilities.h"

/**
 * Constructor.
 */
GuiPaletteColorSelectionDialog::GuiPaletteColorSelectionDialog(QWidget* parent,
                                                               PaletteFile* pf,
                                                               const int initialColorIndex)
 : WuQDialog(parent)
{
   setModal(true);
   paletteFile = pf;
   
   setWindowTitle("Palette Color Selection");

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Create a list box for selecting the colors
   //
   colorSelectionListBox = new QListWidget;
   dialogLayout->addWidget(colorSelectionListBox);
   
   //
   // Load the list box
   //
   const int numColors = paletteFile->getNumberOfPaletteColors();
   for (int i = 0; i < numColors; i++) {
      //
      // Get the color information
      //
      const PaletteColor* pc = paletteFile->getPaletteColor(i);
      const QString name(pc->getName());
      unsigned char rgb[3];
      pc->getRGB(rgb);
      
      //
      // Create the pixmap
      //
      QPixmap pix(QSize(15, 15));
      pix.fill(QColor(rgb[0], rgb[1], rgb[2]));
      
      //
      // Stick the pixmap into the list box
      //
      colorSelectionListBox->addItem(new QListWidgetItem(QIcon(pix), name));
   }
   
   //
   // Use the initial color
   //
   if ((initialColorIndex >= 0) && 
       (initialColorIndex < static_cast<int>(colorSelectionListBox->count()))) {
      colorSelectionListBox->setCurrentRow(initialColorIndex);
   }
                       
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
   // Cancel button connects to QDialogs close() slot.
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
GuiPaletteColorSelectionDialog::~GuiPaletteColorSelectionDialog()
{
}

/**
 * get the index of the selected color.
 */
int 
GuiPaletteColorSelectionDialog::getSelectedColorIndex() const
{
   return colorSelectionListBox->currentRow();
}

/**
 * called when OK/Cancel button pressed.
 */
void 
GuiPaletteColorSelectionDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      if (colorSelectionListBox->count() > 0) {
         if (getSelectedColorIndex() < 0) {
            QMessageBox::critical(this, "ERROR", "No color selected.");
            return;
         }
      }
   }
   
   QDialog::done(r);
}

