
#ifndef __GUI_STRUCTURE_COMBO_BOX_H__
#define __GUI_STRUCTURE_COMBO_BOX_H__

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

#include <vector>
#include <QComboBox>

#include "Structure.h"

/// class for structure selection combo box
class GuiStructureComboBox : public QComboBox {
   Q_OBJECT
   
   public:
      // constructor
      GuiStructureComboBox(QWidget* parent = 0,
                            const char* name = 0,
                            const bool showInvalid = true);
      
      // destructor
      ~GuiStructureComboBox();
      
      // get selected structure
      Structure::STRUCTURE_TYPE getSelectedStructure() const;
      
      // get selected structure as string
      QString getSelectedStructureAsString() const;
      
      // get selected structure as abbreviated string
      QString getSelectedStructureAsAbbreviatedString() const;
      
      // set the structure
      void setStructure(const Structure::STRUCTURE_TYPE st);
      
   protected:
      /// set the current item made protected to prevent user from calling it
      void setCurrentIndex(int indx);
      
      /// values of items in combo box
      std::vector<Structure::STRUCTURE_TYPE> comboItems;
};

#endif // __GUI_STRUCTURE_COMBO_BOX_H__

