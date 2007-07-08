
#ifndef __GUI_PALETTE_COLOR_SELECTION_DIALOG_H__
#define __GUI_PALETTE_COLOR_SELECTION_DIALOG_H__

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

#include "QtDialog.h"

class QListWidget;
class PaletteFile;

/// dialog for selecting a palette color
class GuiPaletteColorSelectionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiPaletteColorSelectionDialog(QWidget* parent,
                                     PaletteFile* pf,
                                     const int initialColorIndex);
                                     
      /// Destructor
      ~GuiPaletteColorSelectionDialog();
      
      /// get the index of the selected color
      int getSelectedColorIndex() const;
      
   protected slots:
      
   protected:
      /// called when OK/Cancel button pressed
      void done(int r);
      
      /// the palette file
      PaletteFile* paletteFile;
      
      /// color selection list box
      QListWidget* colorSelectionListBox;
};

#endif // __GUI_PALETTE_COLOR_SELECTION_DIALOG_H__

