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


#ifndef __VE_GUI_CELL_ATTRIBUTES_DIALOG_H__
#define __VE_GUI_CELL_ATTRIBUTES_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class QCheckBox;
class QLineEdit;


/// This class creates the cell attributes editor dialog
class GuiCellAttributesDialog : public QtDialog {
   Q_OBJECT
    
   public:
      /// storage for unique cell attributes
      class CellAttributes {
         public:
            /// name of cell
            QString name;
            
            /// retain this cell
            bool retain;
            
            /// constructor
            CellAttributes(const QString& nameIn) {
               name = nameIn;
               retain = true;
            }
            
            bool operator=(const CellAttributes& ba) const {
               return (name == ba.name);
            }
      };
      
      /// Constructor
      GuiCellAttributesDialog(QWidget* parent);
       
      /// Destructor
      ~GuiCellAttributesDialog();
   
   private slots:
      /// Apply button slot
      void slotApplyButton();
      
   private:      
      /// storage for the unique cells
      std::vector<CellAttributes> attributes;
      
      /// retain check boxes
      std::vector<QCheckBox*> retainCheckBoxes;
      
      /// names or cells
      std::vector<QString> originalNames;
      
      /// name line edits
      std::vector<QLineEdit*> nameLineEdits;
      
      /// get the index for this original name
      int getIndexFromOriginalName(const QString& name) const;
      
      /// setup the cell attributes
      void setupCellAttributes();
};

#endif // __VE_GUI_CELL_ATTRIBUTES_DIALOG_H__

