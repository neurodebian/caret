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

#ifndef __VE_GUI_IMPORT_DATA_FILE_DIALOG_H__
#define __VE_GUI_IMPORT_DATA_FILE_DIALOG_H__

#include <vector>

#include <q3filedialog.h>

#include "GuiDataFileDialog.h"
#include "VolumeFile.h"

class GuiStructureComboBox;
class GuiSurfaceTypeComboBox;
class GuiTopologyTypeComboBox;
class GuiVolumeTypeComboBox;
class GuiVolumeVoxelDataTypeComboBox;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QSpinBox;

/// This dialog is used for selecting non-Caret data files for import.
class GuiImportDataFileDialog : public Q3FileDialog, GuiDataFileDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiImportDataFileDialog(QWidget* parent);
      
      /// Destructor
      ~GuiImportDataFileDialog();
      
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
      
      /// previous directories combo box
      QComboBox* previousDirectoryComboBox;
      
      /// widget for structure
      QWidget* structureWidget;
      
      /// import as structure combo box
      GuiStructureComboBox* structureComboBox;
      
      /// import topology check box
      QCheckBox* importTopologyCheckBox;
      
      /// import coordinates check box
      QCheckBox* importCoordinatesCheckBox;
      
      /// import colors as paint/area colors
      QCheckBox* importPaintAreaColorsCheckBox;
      
      /// import colors as RGB Paint
      QCheckBox* importRgbColorsCheckBox;
      
      /// group box containing volume selections
      QGroupBox* volumeOptionsGroupBox;
      
      /// combo box for volume type
      GuiVolumeTypeComboBox* volumeTypeComboBox;
      
      /// box for raw volume options
      QWidget* rawVolumeWidget;
      
      /// raw volume dimension X spin box
      QSpinBox* rawVolumeDimXSpinBox;
      
      /// raw volume dimension Y spin box
      QSpinBox* rawVolumeDimYSpinBox;
      
      /// raw volume dimension Z spin box
      QSpinBox* rawVolumeDimZSpinBox;
      
      // raw volume voxel data type combo box
      GuiVolumeVoxelDataTypeComboBox* rawVolumeVoxelDataTypeComboBox;
      
      /// raw volume byte swap combo box
      QComboBox* rawVolumeByteSwapComboBox;
      
      /// group box for contour Items
      QGroupBox* contourGroupBox;
      
      /// import contours check box
      QCheckBox* importContoursCheckBox;
      
      /// import contour cells check box
      QCheckBox* importContourCellsCheckBox;
      
      /// previous directories
      std::vector<QString> previousDirectories;

      /// surface options group box
      QGroupBox* surfaceGroupBox;
      
      /// surface type combo box
      GuiSurfaceTypeComboBox* surfaceTypeComboBox;
      
      /// surface type widget
      QWidget* surfaceTypeWidget;
      
      /// topology type combo box
      GuiTopologyTypeComboBox* topologyTypeComboBox;
      
      /// topology type widget
      QWidget* topologyTypeWidget;
};

#ifdef _GUI_IMPORT_DATA_FILE_DIALOG_MAIN_

#endif // _GUI_IMPORT_DATA_FILE_DIALOG_MAIN_

#endif  // __VE_GUI_IMPORT_DATA_FILE_DIALOG_H__


