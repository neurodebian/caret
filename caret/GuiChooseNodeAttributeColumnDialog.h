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

#ifndef __GUI_CHOOSE_NODE_ATTRIBUTE_COLUMN_DIALOG_H__
#define __GUI_CHOOSE_NODE_ATTRIBUTE_COLUMN_DIALOG_H__

#include <QString>

#include "GuiNodeFileType.h"
#include "WuQDialog.h"

class GuiNodeAttributeColumnSelectionComboBox;
class GiftiNodeDataFile;
class NodeAttributeFile;
class QLineEdit;

/// class for dialog for choosing a node attribute file column and column name
class GuiChooseNodeAttributeColumnDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiChooseNodeAttributeColumnDialog(QWidget* parent,
                                         GUI_NODE_FILE_TYPE nft,
                                         const QString& noneColumnLabel,
                                         const bool addNewSelection = false,
                                         const bool addNoneSelection = false);
      
      /// destructor
      ~GuiChooseNodeAttributeColumnDialog();
      
      /// get the selected column number
      int getSelectedColumnNumber() const;
      
      /// set the selected column number
      void setSelectedColumnNumber(const int col) const;
      
      /// get the name for the column
      QString getColumnName() const;
      
   protected slots:
      /// called when a column is selected
      void slotNodeAttributeComboBox();
      
   protected:
      /// column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* nodeAttributeComboBox;
      
      /// column name line edit
      QLineEdit* columnNameLineEdit;
};

#endif // __GUI_CHOOSE_NODE_ATTRIBUTE_COLUMN_DIALOG_H__

