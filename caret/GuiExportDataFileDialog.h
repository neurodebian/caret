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

#ifndef __VE_GUI_EXPORT_DATA_FILE_DIALOG_H__
#define __VE_GUI_EXPORT_DATA_FILE_DIALOG_H__

#include <vector>

#include <q3filedialog.h>

#include "GuiDataFileDialog.h"
class GuiBrainModelSelectionComboBox;
class GuiVolumeSelectionControl;
class QComboBox;
class QGroupBox;

/// This dialog is used for selecting non-Caret data files for export.
class GuiExportDataFileDialog : public Q3FileDialog, GuiDataFileDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiExportDataFileDialog(QWidget* parent);
      
      /// Destructor
      ~GuiExportDataFileDialog();
            
      /// show the dialog
      virtual void show();
      
      /// update the dialog
      void updateDialog();
      
   private slots:
      /// overrides QFileDialog's done() method.  Intercept to get the file selected
      /// and see if it already exists and leave dialog open in some cases.
      void done(int r);
      
      /// Called when a file filter is selected
      void filterSelectedSlot(const QString& filterNameIn);

      /// called when a previous directory is selected
      void slotPreviousDirectory(int item);
      
   private:
      /// create the caret unique section.
      QWidget* createCaretUniqueSection();
      
      /// group box for surface items
      QGroupBox* surfaceGroupBox;
      
      /// combo box for selecting surface for export
      GuiBrainModelSelectionComboBox* surfaceSelectionComboBox;
      
      /// group box for volume items
      QGroupBox* volumeGroupBox;
      
      /// volume selection control
      GuiVolumeSelectionControl* volumeSelectionControl;
      
      /// previous directories combo box
      QComboBox* previousDirectoryComboBox;
      
      /// previous directories
      std::vector<QString> previousDirectories;
};

#ifdef _GUI_EXPORT_DATA_FILE_DIALOG_MAIN_


#endif // _GUI_EXPORT_DATA_FILE_DIALOG_MAIN_

#endif  // __VE_GUI_EXPORT_DATA_FILE_DIALOG_H__


