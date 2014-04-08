#ifndef __GUI_AVERAGE_BORDER_DIALOG_H__
#define __GUI_AVERAGE_BORDER_DIALOG_H__

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

#include <QString>
#include <vector>

#include "WuQDialog.h"

class QCheckBox;
class QLineEdit;
class QListWidget;
class QDoubleSpinBox;

/// class for creating average border files
class GuiAverageBorderDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiAverageBorderDialog(QWidget* parent);
      
      /// Destructor
      ~GuiAverageBorderDialog();
      
   protected slots:
      /// called when apply button is pressed
      void slotApplyButton();
      
      /// called when add button is pressed
      void slotAddButton();
      
      /// called when remove button is pressed
      void slotRemoveButton();
      
      /// called to select output border file name
      void slotOutputBorderButton();
      
   protected:
      /// Load the border file list box
      void loadBorderFileListBox();

      /// list box of input border files
      QListWidget* inputListWidget;
      
      /// resampling float spin box
      QDoubleSpinBox* resamplingDoubleSpinBox;
      
      /// the name of the border files
      std::vector<QString> borderFileNames;
      
      /// output border file name line edit
      QLineEdit* outputBorderFileLineEdit;
      
      /// sphere check box
      QCheckBox* sphereCheckBox;
   
};

#endif // __GUI_AVERAGE_BORDER_DIALOG_H__

