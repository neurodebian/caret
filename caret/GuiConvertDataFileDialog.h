
#ifndef __GUI_CONVERT_DATA_FILE_DIALOG_H__
#define __GUI_CONVERT_DATA_FILE_DIALOG_H__

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

class QComboBox;
class QListWidget;

/// dialog for converting data files between various formats
class GuiConvertDataFileDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiConvertDataFileDialog(QWidget* parent);
      
      /// destructor
      ~GuiConvertDataFileDialog();
      
   protected slots:
      /// called when add files button is pressed
      void slotAddFilesButton();
      
      /// called when remove files button is pressed
      void slotRemoveFilesButton();
      
      /// called when apply button is pressed.
      void slotApplyButton();
      
      /// called when close button is pressed.
      void slotCloseButton();
      
   protected:
      /// load file names into list box
      void loadFileNamesIntoListBox();
      
      /// list box for data files
      QListWidget* dataFilesListBox;
      
      /// combo boxes for file format
      std::vector<QComboBox*> formatComboBoxes;

      /// names of data files
      std::vector<QString> dataFileNames;
};

#endif // __GUI_CONVERT_DATA_FILE_DIALOG_H__

