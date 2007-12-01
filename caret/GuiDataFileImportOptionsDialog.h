
#ifndef __GUI_DATA_FILE_IMPORT_OPTIONS_DIALOG_H__
#define __GUI_DATA_FILE_IMPORT_OPTIONS_DIALOG_H__

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

#include "BrainModelSurface.h"
#include "QtDialogModal.h"
#include "Structure.h"
#include "TopologyFile.h"
#include "VolumeFile.h"

class GuiStructureComboBox;
class GuiSurfaceTypeComboBox;
class GuiTopologyTypeComboBox;
class GuiVolumeTypeComboBox;
class GuiVolumeVoxelDataTypeComboBox;
class QCheckBox;
class QComboBox;
class QSpinBox;

/// dialog for setting options for importing files
class GuiDataFileImportOptionsDialog : public QtDialogModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiDataFileImportOptionsDialog(QWidget* parent,
                                     const QString& filterName);
      
      // destructor
      ~GuiDataFileImportOptionsDialog();
      
      // get the contour options
      void getContourOptions(bool& importContours,
                             bool& importCells) const;
      
      // get the surface options
      void getSurfaceOptions(Structure::STRUCTURE_TYPE& structureType,
                             bool& importTopology,
                             TopologyFile::TOPOLOGY_TYPES& topologyType,
                             bool& importCoordinates,
                             BrainModelSurface::SURFACE_TYPES& coordinateType,
                             bool& importColorsAsPaint,
                             bool& importColorsAsRgbPaint);
                             
      // get the volume options
      void getVolumeOptions(VolumeFile::VOLUME_TYPE& volumeType,
                            int dimensions[3],
                            VolumeFile::VOXEL_DATA_TYPE& voxelDataType,
                            bool& byteSwap);
                            
   protected:
      // create the contours options widget
      QWidget* createContourOptionsWidget(const QString& filterName);
      
      // create the surface options widget
      QWidget* createSurfaceOptionsWidget(const QString& filterName);
      
      // create the volume options widget
      QWidget* createVolumeOptionsWidget(const QString& filterName);
      
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
      
      /// combo box for volume type
      GuiVolumeTypeComboBox* volumeTypeComboBox;
      
      /// raw volume dimension X spin box
      QSpinBox* volumeDimXSpinBox;
      
      /// raw volume dimension Y spin box
      QSpinBox* volumeDimYSpinBox;
      
      /// raw volume dimension Z spin box
      QSpinBox* volumeDimZSpinBox;
      
      // raw volume voxel data type combo box
      GuiVolumeVoxelDataTypeComboBox* volumeVoxelDataTypeComboBox;
      
      /// raw volume byte swap combo box
      QComboBox* volumeByteSwapComboBox;
      
      /// import contours check box
      QCheckBox* importContoursCheckBox;
      
      /// import contour cells check box
      QCheckBox* importContourCellsCheckBox;
      
      /// surface type combo box
      GuiSurfaceTypeComboBox* surfaceTypeComboBox;
      
      /// topology type combo box
      GuiTopologyTypeComboBox* topologyTypeComboBox;
};

#endif // __GUI_DATA_FILE_IMPORT_OPTIONS_DIALOG_H__

