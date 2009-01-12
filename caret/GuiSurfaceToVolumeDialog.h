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


#ifndef __GUI_SURFACE_TO_VOLUME_DIALOG_H__
#define __GUI_SURFACE_TO_VOLUME_DIALOG_H__

#include "WuQDialog.h"

#include "BrainModelSurfaceToVolumeConverter.h"
#include "StereotaxicSpace.h"
#include "VolumeFile.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class BrainModel;
class BrainModelSurface;
class BrainModelVolume;
class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;

/// Dialog for converting a surface to a volume.
class GuiSurfaceToVolumeDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// mode of dialog
      enum DIALOG_MODE {
         DIALOG_MODE_NORMAL,
         DIALOG_MODE_PAINT,
         DIALOG_MODE_METRIC,
         DIALOG_MODE_SEGMENT_VOLUME,
         DIALOG_MODE_SURFACE_SHAPE
      };
      
      /// Constructor
      GuiSurfaceToVolumeDialog(QWidget* parent, const DIALOG_MODE modeIn,
                               const QString& dialogTitle,
                               const bool showSurfaceSelectionOptionsFlag = true);
      
      /// Destructor
      ~GuiSurfaceToVolumeDialog();
      
      /// get the selected surface
      BrainModelSurface* getSelectedSurface() const;
      
      /// get the selected node attribute (metric/paint/shape) column
      int getSelectedNodeAttributeColumn() const;
      
      /// get the offset
      void getSurfaceOffset(float offset[3]) const;
      
      /// get the inner boundary
      float getSurfaceInnerBoundary() const;
      
      /// get the outer boundary
      float getSurfaceOuterBoundary() const;
      
      /// get the desired surface thickness step
      float getSurfaceThicknessStep() const;
      
      /// get the volume dimensions
      void getVolumeDimensions(int dim[3]) const;
      
      /// set the volume dimensions
      void setVolumeDimensions(const int dim[3]);
      
      /// get the volume voxel sizes
      void getVolumeVoxelSizes(float voxelSize[3]) const;
      
      /// set the volume voxel sizes
      void setVolumeVoxelSizes(const float voxelSize[3]);
      
      /// Get the volume origin.
      void getVolumeOrigin(float origin[3]) const;
      
      /// Set the volume origin.
      void setVolumeOrigin(const float origin[3]);
      
      /// get the standard volume space
      StereotaxicSpace getStandardVolumeSpace() const;
      
      /// get node to voxel mapping enabled
      bool getNodeToVoxelMappingEnabled() const;
      
      /// get node to voxel file name
      QString getNodeToVoxelMappingFileName() const;
      
      /// get the metric conversion mode
      BrainModelSurfaceToVolumeConverter::CONVERSION_MODE getMetricConversionMode() const;
      
   private slots:
      /// called when a volume space is selected
      void slotVolumeSpaceSelection(int item);
      
      /// called enter ac pushbutton is pressed
      void slotAcPushButton();
      
      /// called when get parameters from volume file is pressed
      void slotParamtersFromVolumePushButton();
      
      /// called when apply button pressed
      void done(int r);
      
   private:
      /// surface selection combo box
      GuiBrainModelSelectionComboBox* surfaceSelectionBox;
      
      /// surface offset X line edit
      QDoubleSpinBox* surfaceOffsetXDoubleSpinBox;
      
      /// surface offset Y line edit
      QDoubleSpinBox* surfaceOffsetYDoubleSpinBox;
      
      /// surface offset Z line edit
      QDoubleSpinBox* surfaceOffsetZDoubleSpinBox;
      
      /// surface inner boundary line edit
      QDoubleSpinBox* surfaceInnerBoundaryDoubleSpinBox;
      
      /// surface outer boundary line edit
      QDoubleSpinBox* surfaceOuterBoundaryDoubleSpinBox;
      
      /// surface thickness step line edit
      QDoubleSpinBox* surfaceThicknessStepDoubleSpinBox;
      
      /// volume X dimension line edit
      QSpinBox* volumeDimensionXSpinBox;
      
      /// volume Y dimension line edit
      QSpinBox* volumeDimensionYSpinBox;
      
      /// volume Z dimension line edit
      QSpinBox* volumeDimensionZSpinBox;
      
      /// volume voxel size x line edit
      QDoubleSpinBox* volumeVoxelSizeXDoubleSpinBox;
      
      /// volume voxel size y line edit
      QDoubleSpinBox* volumeVoxelSizeYDoubleSpinBox;
      
      /// volume voxel size z line edit
      QDoubleSpinBox* volumeVoxelSizeZDoubleSpinBox;
      
      /// volume origin x line edit
      QDoubleSpinBox* volumeOriginXDoubleSpinBox;
       
      /// volume origin y line edit
      QDoubleSpinBox* volumeOriginYDoubleSpinBox;
       
      /// volume origin z line edit
      QDoubleSpinBox* volumeOriginZDoubleSpinBox;
       
      /// mode of dialog
      DIALOG_MODE mode;
            
      /// combo box for space selection
      QComboBox* volumeSpaceComboBox;
      
      /// node attribute (metric/paint/shape) column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* nodeAttributeColumnComboBox;

      /// metric algorithm combo box
      QComboBox* metricAlgorithmComboBox;
      
      /// node to voxel mapping check box
      QCheckBox* nodeToVoxelCheckBox;
      
      /// line edit for node to voxel file name
      QLineEdit* nodeToVoxelFileNameLineEdit;
      
      /// offset from last time
      static float savedOffset[3];
      
      /// outer boundary from last time
      static float savedOuterBoundary;
      
      /// inner boundary from last time
      static float savedInnerBoundary;
      
      /// thickness step from last time
      static float savedThicknessStep;
      
      /// dimensions from last time
      static int savedDimensions[3];
      
      /// voxel size from last time
      static float savedVoxelSize[3];
      
      /// volume origin from last time
      static float savedOrigin[3];
      
      /// surface from last time
      static BrainModel* savedSurface;
      
      /// volume space from last time
      static StereotaxicSpace savedVolumeSpace;
      
      /// create node to voxel mapping last time
      static bool savedNodeToVoxelMapping;
      
      /// node to voxel file name from last time
      static QString savedNodeToVoxelFileName;
      
      /// the standard volume spaces
      std::vector<StereotaxicSpace> volumeSpaceComboBoxSpaces;
};

#ifdef __GUI_SURFACE_TO_VOLUME_DIALOG_MAIN__
   float GuiSurfaceToVolumeDialog::savedOffset[3] = { 0.0, 0.0, 0.0 };
   float GuiSurfaceToVolumeDialog::savedInnerBoundary = -1.5;
   float GuiSurfaceToVolumeDialog::savedOuterBoundary =  1.5;
   float GuiSurfaceToVolumeDialog::savedThicknessStep = 0.5;
   int   GuiSurfaceToVolumeDialog::savedDimensions[3] = { 0, 0, 0 };
   float GuiSurfaceToVolumeDialog::savedVoxelSize[3]  = { 1.0, 1.0, 1.0 };
   float GuiSurfaceToVolumeDialog::savedOrigin[3] = { 0.0, 0.0, 0.0 };
   BrainModel* GuiSurfaceToVolumeDialog::savedSurface = NULL;
   StereotaxicSpace GuiSurfaceToVolumeDialog::savedVolumeSpace;
   bool  GuiSurfaceToVolumeDialog::savedNodeToVoxelMapping;
   QString GuiSurfaceToVolumeDialog::savedNodeToVoxelFileName;
   
#endif // __GUI_SURFACE_TO_VOLUME_DIALOG_MAIN__

#endif // __GUI_SURFACE_TO_VOLUME_DIALOG_H__
