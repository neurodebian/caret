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

#include "GuiSpeciesComboBox.h"

/**
 * constructor.
 */
GuiSpeciesComboBox::GuiSpeciesComboBox(QWidget* parent)
   : QComboBox(parent)
{
   std::vector<Species::TYPE> speciesTypes;
   std::vector<QString> speciesNames;
   Species::getAllSpeciesTypesAndNames(speciesTypes, speciesNames);
   
   for (unsigned int i = 0; i < speciesNames.size(); i++) {
      addItem(speciesNames[i], static_cast<int>(speciesTypes[i]));
   }
}

/**
 * destructor.
 */
GuiSpeciesComboBox::~GuiSpeciesComboBox()
{
}

/**
 * get selected species.
 */
Species 
GuiSpeciesComboBox::getSelectedSpecies() const
{
   Species species;
   const int indx = currentIndex();
   if (indx >= 0) {
      species = static_cast<Species::TYPE>(itemData(indx).toInt());
   }
   return species;
}

/**
 * set the species.
 */
void 
GuiSpeciesComboBox::setSelectedSpecies(const Species& s)
{
   const int indx = findData(static_cast<int>(s.getType()));
   if (indx >= 0) {
      setCurrentIndex(indx);
   }
}

/**
 * set the current item made protected to prevent user from calling it.
 */
void 
GuiSpeciesComboBox::setCurrentIndex(int indx)
{
   QComboBox::setCurrentIndex(indx);
}
      
