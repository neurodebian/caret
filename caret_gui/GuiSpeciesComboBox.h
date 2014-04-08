
#ifndef __GUI_SPECIES_COMBO_BOX_H__
#define __GUI_SPECIES_COMBO_BOX_H__

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

#include "Species.h"

/// class for species selection combo box
class GuiSpeciesComboBox : public QComboBox {
   Q_OBJECT
   
   public:
      // constructor
      GuiSpeciesComboBox(QWidget* parent = 0);
      
      // destructor
      ~GuiSpeciesComboBox();
      
      // get selected species
      Species getSelectedSpecies() const;
      
      // set the species
      void setSelectedSpecies(const Species& s);
      
   protected:
      /// set the current item made protected to prevent user from calling it
      void setCurrentIndex(int indx);
};

#endif // __GUI_SPECIES_COMBO_BOX_H__

