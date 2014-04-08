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

#include "GuiCategoryComboBox.h"

/**
 * constructor.
 */
GuiCategoryComboBox::GuiCategoryComboBox(QWidget* parent)
   : QComboBox(parent)
{
   std::vector<Category::TYPE> types;
   std::vector<QString> names;
   Category::getAllCategoryTypesAndNames(types, names);
   
   int defaultIndex = 0;
   for (unsigned int i = 0; i < names.size(); i++) {
      addItem(names[i], static_cast<int>(types[i]));
      if (types[i] == Category::TYPE_INDIVIDUAL) {
         defaultIndex = i;
      }
   }
   setCurrentIndex(defaultIndex);
}

/**
 * destructor.
 */
GuiCategoryComboBox::~GuiCategoryComboBox()
{
}

/**
 * get selected category.
 */
Category 
GuiCategoryComboBox::getSelectedCategory() const
{
   Category category;
   const int indx = currentIndex();
   if (indx >= 0) {
      category = static_cast<Category::TYPE>(itemData(indx).toInt());
   }
   return category;
}

/**
 * set the category.
 */
void 
GuiCategoryComboBox::setSelectedCategory(const Category& c)
{
   const int indx = findData(static_cast<int>(c.getType()));
   if (indx >= 0) {
      setCurrentIndex(indx);
   }
}

/**
 * set the current item made protected to prevent user from calling it.
 */
void 
GuiCategoryComboBox::setCurrentIndex(int indx)
{
   QComboBox::setCurrentIndex(indx);
}
      
