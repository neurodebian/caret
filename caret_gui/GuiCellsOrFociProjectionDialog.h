
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

#ifndef __GUI_PROJECT_CELLS_OR_FOCI_DIALOG_H__
#define __GUI_PROJECT_CELLS_OR_FOCI_DIALOG_H__

#include "WuQDialog.h"

class QRadioButton;
class QDoubleSpinBox;

/// class for dialog used to project cells and foci
class GuiCellsOrFociProjectionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// type of file to project
      enum FILE_TYPE {
         FILE_TYPE_CELL,
         FILE_TYPE_FOCI
      };
      
      /// Constructor
      GuiCellsOrFociProjectionDialog(QWidget* parent, FILE_TYPE fileTypeIn);
      
      /// Destructor
      ~GuiCellsOrFociProjectionDialog();
      
   private slots:
      /// called when help button is pressed
      void slotHelpButton();
      
   private:
      /// called when OK or Cancel button pressed
      virtual void done(int r);
      
      /// type of file being projected
      FILE_TYPE fileType;
      
      /// project all radio button
      QRadioButton* projectAllRadioButton;
      
      /// project hemisphere only radio button
      QRadioButton* projectHemisphereOnlyRadioButton;
      
      /// project flip to match radio button
      QRadioButton* projectFlipToMatchRadioButton;
      
      /// project onto surface check box
      QRadioButton* projectOntoSurfaceRadioButton;
      
      /// keep offset from surface
      QRadioButton* maintainOffsetFromSurfaceRadioButton;
      
      /// above surface distance
      QDoubleSpinBox* surfaceOffsetDoubleSpinBox;
};

#endif // __GUI_PROJECT_CELLS_OR_FOCI_DIALOG_H__

