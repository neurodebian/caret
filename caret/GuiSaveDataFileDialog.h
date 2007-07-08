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


#ifndef __VE_GUI_SAVE_DATA_FILE_DIALOG_H__
#define __VE_GUI_SAVE_DATA_FILE_DIALOG_H__

#include <vector>

#include <q3filedialog.h>

#include "GuiDataFileDialog.h"
#include "VolumeFile.h"

class BrainModelSurface;
class CoordinateFile;
class GuiBrainModelSelectionComboBox;
class GuiStructureComboBox;
class GuiSurfaceTypeComboBox;
class GuiTopologyFileComboBox;
class GuiTopologyTypeComboBox;
class GuiVolumeSelectionControl;
class TopologyFile;
class VtkModelFile;

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QRadioButton;
class QTextEdit;

///  This dialog is used for saving caret data files
class GuiSaveDataFileDialog : public Q3FileDialog, GuiDataFileDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiSaveDataFileDialog(QWidget* parent,
                            const QString& useThisFileFilter = "");

      /// Destructor
      ~GuiSaveDataFileDialog();
      
      /// show the dialog
      virtual void show();
      
      /// update the dialog
      void updateDialog();
      
   private slots:
      /// called when file filter is selected in dialog
      void filterSelectedSlot(const QString& filterName);
      
      /// overrides QFileDialog's done() method.  Intercept to get the file selected
      /// and see if it already exists and leave dialog open in some cases.
      void done(int r);
      
      /// update the save parameters
      void updateSaveParameters();
      
      /// called when a volume type is selected
      void volumeTypeSelectedSlot();
      
      /// called when a previous directory is selected
      void slotPreviousDirectory(int item);
      
      /// called when foci save option changed
      void slotFociSaveUpdate();
      
   private:
      /// Update the file write type combo box.
      void updateFileWriteTypeComboBox(AbstractFile* af);
      
      /// update volume saving items
      void updateVolumeSavingItems();
      
      /// create the caret unique items
      QWidget* createCaretUniqueSection();
      
      /// the current file filter
      QString filterName;
      
      /// structure combo box
      GuiStructureComboBox* structureComboBox;
      
      /// the PubMed ID Widget
      QWidget* pubMedIDWidget;
      
      /// PubMed ID line edit
      QLineEdit* pubMedIDLineEdit;
      
      /// widgetcontaining comment information
      QWidget* commentWidget;

      /// text editor for comments
      QTextEdit* commentTextEditor;
      
      /// coord frame combo box
      QComboBox* coordFrameComboBox;
      
      /// combo box for previous directories
      QComboBox* previousDirectoryComboBox;
      
      /// border type combo box;
      GuiSurfaceTypeComboBox* borderTypeComboBox;
      
      /// coordinate type combo box;
      GuiSurfaceTypeComboBox* coordTypeComboBox;
      
      /// x orientation combo box
      QComboBox* xOrientationComboBox;
      
      /// y orientation combo box
      QComboBox* yOrientationComboBox;
      
      /// z orientation combo box
      QComboBox* zOrientationComboBox;
      
      /// horizontal box containing border file and border proj file selection
      QWidget* borderSurfaceWidget;
      
      /// remove duplicate border or border projection check box
      QCheckBox* removeDuplicateBorderCheckBox;
      
      /// horizontal box containing border selections
      QGroupBox* bordersGroupBox;
      
      /// horizontal box for coordinate file selection
      QGroupBox* coordFileGroupBox;
      
      /// horizontal box for topology file selection
      QGroupBox* topologyGroupBox;
      
      /// combo box for border file selection
      QComboBox* borderFileComboBox;
      
      /// indices for surface associated with borders
      std::vector<int> borderFileSurfaceIndex;
      
      /// combo box for coordinate file selection
      QComboBox* coordFileComboBox;
      
      /// combo box for topology file selection
      GuiTopologyFileComboBox* topologyFileComboBox;
      
      /// combo box for topology type
      GuiTopologyTypeComboBox* topologyTypeComboBox;
      
      /// group box for vtk model file selection
      QGroupBox* vtkModelGroupBox;
      
      /// combo box for vtk model file selection
      QComboBox* vtkModelFileComboBox;
      
      /// the VTK model files
      std::vector<VtkModelFile*> vtkModelFiles;
      
      /// volume group box
      QGroupBox* volumeGroupBox;
      
      /// volume selection control
      GuiVolumeSelectionControl* volumeSelectionControl;
      
      /// checkbox for write volume file gzipped
      QCheckBox* volumeFileGzipCheckBox;
      
      /// combo box for voxel data type
      QComboBox* volumeVoxelDataTypeComboBox;
      
      /// voxel data type for combo entries
      std::vector<VolumeFile::VOXEL_DATA_TYPE> volumeVoxelDataTypeComboBoxValues;
      
      /// brain models surfaces for saving
      std::vector<BrainModelSurface*> brainSurfaces;
      
      /// coordinate files for saving
      std::vector<CoordinateFile*> coordinateFiles;
      
      /// save as type (ascii, binary, xml)
      QComboBox* fileEncodingComboBox;
      
      /// values corresponding to labels in fileEncodingComboBox
      std::vector<AbstractFile::FILE_FORMAT> fileEncodingComboBoxValues;
      
      /// foci surface selections
      QWidget* fociSurfaceWidget;
      
      /// foci file selections
      QGroupBox* fociGroupBox;
      
      /// foci save original coordinates
      QRadioButton* fociSaveOriginalCoordsRadioButton;
      
      /// foci save projected to surfaces
      QRadioButton* fociSaveProjectedToSurfacesRadioButton;
      
      /// cell file selections
      QGroupBox* cellGroupBox;
      
      /// foci file left surface combo box
      GuiBrainModelSelectionComboBox* fociLeftSurfaceComboBox;
      
      /// foci file right surface combo box
      GuiBrainModelSelectionComboBox* fociRightSurfaceComboBox;
      
      /// cell file type combo box
      GuiBrainModelSelectionComboBox* cellTypeComboBox;
      
      /// automatically add file name extension check box
      QCheckBox* autoAppendExtensionCheckBox;
      
      /// previous directories
      std::vector<QString> previousDirectories;
      
      /// save data encoding type widget
      QWidget* saveDataEncodingTypeWidget;
      
      /// saved position and size
      QRect savedPositionAndSize;
      
      /// saved position and size valid
      bool savedPositionAndSizeValid;
};

#ifdef _GUI_SAVE_DATA_FILE_DIALOG_MAIN_

#endif // _GUI_SAVE_DATA_FILE_DIALOG_MAIN_
      
#endif
