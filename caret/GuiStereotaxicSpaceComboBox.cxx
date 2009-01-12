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

#include "GuiStereotaxicSpaceComboBox.h"

/**
 * constructor.
 */
GuiStereotaxicSpaceComboBox::GuiStereotaxicSpaceComboBox(QWidget* parent)
   : QComboBox(parent)
{
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);
   
   for (unsigned int i = 0; i < spaces.size(); i++) {
      addItem(spaces[i].getName(), static_cast<int>(spaces[i].getSpace()));
   }
}

/**
 * destructor.
 */
GuiStereotaxicSpaceComboBox::~GuiStereotaxicSpaceComboBox()
{
}

/**
 * get selected stereotaxic space.
 */
StereotaxicSpace 
GuiStereotaxicSpaceComboBox::getSelectedStereotaxicSpace() const
{
   StereotaxicSpace space;
   const int indx = currentIndex();
   if (indx >= 0) {
      space = StereotaxicSpace(static_cast<StereotaxicSpace::SPACE>(itemData(indx).toInt()));
   }
   return space;
}

/**
 * set the stereotaxic space.
 */
void 
GuiStereotaxicSpaceComboBox::setSelectedStereotaxicSpace(const StereotaxicSpace& ss)
{
   const int indx = findData(static_cast<int>(ss.getSpace()));
   if (indx >= 0) {
      setCurrentIndex(indx);
   }
}

/**
 * set the current item made protected to prevent user from calling it.
 */
void 
GuiStereotaxicSpaceComboBox::setCurrentIndex(int indx)
{
   QComboBox::setCurrentIndex(indx);
}
      
