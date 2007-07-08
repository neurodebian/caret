
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

#include <QIcon>
#include <QLayout>
#include <QListWidget>
#include <QPixmap>

#include "GuiHtmlColorChooserDialog.h"
#include "HtmlColors.h"

/**
 * constructor.
 */
GuiHtmlColorChooserDialog::GuiHtmlColorChooserDialog(QWidget* parent)
   : QtDialogModal(parent)
{
   //
   // Get number of HTML colors
   //
   const int numColors = HtmlColors::getNumberOfColors();
   
   //
   // Create the color selection list widget
   //
   colorListWidget = new QListWidget;
   for (int i = 0; i < numColors; i++) {
      QString name;
      unsigned char red, green, blue;
      HtmlColors::getColorInformation(i, name, red, green, blue);
      QPixmap pix(24, 12);
      pix.fill(QColor(red, green, blue));
      QIcon icon(pix);
      QListWidgetItem* item = new QListWidgetItem(icon, name);
      colorListWidget->addItem(item);
   }
   
   //
   // Layout the dialog
   //
   QVBoxLayout* dialogLayout = getDialogLayout();
   dialogLayout->addWidget(colorListWidget);
}

/**
 * destructor.
 */
GuiHtmlColorChooserDialog::~GuiHtmlColorChooserDialog()
{
}

/**
 * get the selected color name and color components.
 */
void 
GuiHtmlColorChooserDialog::getSelectedColor(QString& name,
                                      unsigned char& red,
                                      unsigned char& green,
                                      unsigned char& blue) const
{
   const int colorIndex = colorListWidget->currentRow();
   HtmlColors::getColorInformation(colorIndex,
                                   name,
                                   red,
                                   green,
                                   blue);
}