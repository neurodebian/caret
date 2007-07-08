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


#ifndef __GUI_QT_LIST_BOX_SELECTION_DIALOG_H__
#define __GUI_QT_LIST_BOX_SELECTION_DIALOG_H__

#include "QtDialog.h"

#include <QStringList>
#include <vector>

class QListWidget;
class QListWidgetItem;

/// Dialog for selecting items in a list box
class QtListBoxSelectionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor that adds select all button
      QtListBoxSelectionDialog(QWidget* parent, 
                               const QString& title,
                               const QString& instructions,
                               const std::vector<QString>& items,
                               const QString& selectAllButtonLabel,
                               const int defaultItem);
                              
      /// Constructor
      QtListBoxSelectionDialog(QWidget* parent, 
                               const QString& title,
                               const QString& instructions,
                               const std::vector<QString>& items,
                               const int defaultItem = -1);
      
      /// Constructor
      QtListBoxSelectionDialog(QWidget* parent, 
                               const QString& title,
                               const QString& instructions,
                               const QStringList& items,
                               const int defaultItem = -1);
      
      /// Constructor
      QtListBoxSelectionDialog(QWidget* parent,
                               const QString& title);
                               
      /// Destructor
      ~QtListBoxSelectionDialog();
      
      /// allow multiple item selection
      void setAllowMultipleItemSelection(const bool allowIt);
      
      /// set the contents of the list box
      void setListBoxContents(const std::vector<QString>& contents,
                              const int defaultIndex = -1);
                              
      /// get the selected item
      int getSelectedItemIndex() const;
      
      /// get the selected item text
      QString getSelectedText() const;
      
      /// get multiple selected items
      void getSelectedItemsIndices(std::vector<int>& selectedItemIndices) const;
      
      /// get multiple selected items
      void getSelectedItems(std::vector<QString>& selectedItems) const;
      
   protected slots:
      /// called when select all pushbutton is pressed
      void slotSelectAllPushButton();
      
   protected:
      /// the list box
      QListWidget* listWidget;
      
      /// items in list widget
      std::vector<QListWidgetItem*> items;
      
      /// create the dialog
      void createDialog(const QString& title,
                        const QString& selectAllButtonLabel,
                        const QString& instructions);
                        
      /// items input by the user
      std::vector<QString> inputItems;
};

#endif // __GUI_QT_LIST_BOX_SELECTION_DIALOG_H__

