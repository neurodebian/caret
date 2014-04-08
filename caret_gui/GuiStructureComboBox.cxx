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

#include "GuiStructureComboBox.h"

/**
 * constructor.
 */
GuiStructureComboBox::GuiStructureComboBox(QWidget* parent,
                      const char* /*name */,
                      const bool showInvalid)
   : QComboBox(parent)
{
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes, structureNames, showInvalid);
   
   for (unsigned int i = 0; i < structureTypes.size(); i++) {
      addItem(structureNames[i]);
      comboItems.push_back(structureTypes[i]);
   }
}

/**
 * destructor.
 */
GuiStructureComboBox::~GuiStructureComboBox()
{
}

/**
 * get selected structure.
 */
Structure::STRUCTURE_TYPE 
GuiStructureComboBox::getSelectedStructure() const
{
   Structure::STRUCTURE_TYPE st = Structure::STRUCTURE_TYPE_INVALID;
   
   const int item = currentIndex();
   if ((item >= 0) && (item < static_cast<int>(count()))) {
      st = comboItems[item];
   }
   
   return st;
}

/**
 * get selected structure as string.
 */
QString 
GuiStructureComboBox::getSelectedStructureAsString() const
{
   return Structure::convertTypeToString(getSelectedStructure());
}

/**
 * get selected structure as abbreviated string.
 */
QString 
GuiStructureComboBox::getSelectedStructureAsAbbreviatedString() const
{
   return Structure::convertTypeToAbbreviatedString(getSelectedStructure());
}

/**
 * set the structure.
 */
void 
GuiStructureComboBox::setStructure(const Structure::STRUCTURE_TYPE ht)
{
   for (unsigned int i = 0; i < comboItems.size(); i++) {
      if (comboItems[i] == ht) {
         setCurrentIndex(i);
         break;
      }
   }
}

/**
 * set the current item made protected to prevent user from calling it.
 */
void 
GuiStructureComboBox::setCurrentIndex(int indx)
{
   QComboBox::setCurrentIndex(indx);
}
      
