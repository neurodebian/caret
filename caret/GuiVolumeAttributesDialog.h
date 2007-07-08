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

#ifndef __GUI_VOLUME_ATTRIBUTES_DIALOG_H__
#define __GUI_VOLUME_ATTRIBUTES_DIALOG_H__

#include "QtDialog.h"

class GuiVolumeFileOrientationComboBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QTabWidget;
class QDoubleSpinBox;
class VolumeFile;

/// Dialog for viewing and editing volume attributes
class GuiVolumeAttributesDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiVolumeAttributesDialog(QWidget* parent, const bool modal = false, Qt::WFlags f = 0);
      
      /// Destructor
      ~GuiVolumeAttributesDialog();
      
      /// update the dialog
      void updateDialog();
      
   private slots:
      /// called when apply button pressed
      void slotApplyButton();
      
      /// called when flip X pushbutton is selected
      void slotFlipXPushButton();
      
      /// called when flip Y pushbutton is selected
      void slotFlipYPushButton();
      
      /// called when a volume type is selected
      void slotVolumeTypeSelected();
      
      /// called when a volume file is selected
      void slotVolumeFileSelected();
      
      /// called enter ac pushbutton is pressed
      void slotAcPushButton();
      
      /// called when main window crosshairs as AC pushbutton is pressed
      void slotMainWindowXHairPushButton();
      
      /// called when get parameters from volume file is pressed
      void slotParamtersFromVolumePushButton();
      
      /// called when rotate push button is pressed
      void slotRotatePushButton();
      
      /// called when histogram button is pressed
      void slotHistogramButton();
      
      /// called when rescale voxels button is pressed
      void slotRescaleVoxelsButton();
      
      /// called when uniformity correction button is pressed
      void slotUniformityCorrection();
      
      /// called when convert to LPI pushbutton is pressed
      void slotLpiPushButton();
      
      /// called when use AC from Params File pushbutton is pressed
      void slotParamsAcPushButton();
      
      /// called when use Whole Volume AC from Params File pushbutton is pressed
      void slotParamsWholeAcPushButton();
      
      /// called when use Standard Space pushbutton is pressed
      void slotParamsStandardSpacePushButton();
      
      /// called when Update Params File AC pushbutton is pressed
      void slotParamsUpdateAC();
      
      /// called when Update Params File Whole Volume AC pushbutton is pressed
      void slotParamsUpdateWholeVolumeAC();
      
      // called when align set AC X-Hairs button pressed
      void slotCrosshairsAlignAC();
      
      // called when align set PC X-Hairs button pressed
      void slotCrosshairsAlignPC();
      
      // called when align set LF X-Hairs button pressed
      void slotCrosshairsAlignLF();
      
   private:
      /// get the selected volume
      VolumeFile* getSelectedVolume(const bool requireVolumeInMainWindow);
      
      /// create the alignment page
      void createAlignmentPage();
      
      /// create the coordinates page
      void createCoordinatesPage();
      
      /// create the orientation page
      void createOrientationPage();
      
      /// create the resampling page
      void createResamplingPage();
      
      /// create the data page
      void createDataPage();
      
      /// loads the volume file combo box
      void loadVolumeFileComboBox();
      
      /// loads the volume parameters
      void loadVolumeParameters();
      
      /// load the resampling parameters
      void loadVolumeResampling();

      /// Called to resample the volume.
      void resampleVolume();

      /// the tab widget
      QTabWidget* tabWidget;
      
      /// the alignment page
      QWidget* alignmentPage;
      
      /// the coordinates widget
      QWidget* coordinatesPage;
      
      /// the orientation widget
      QWidget* orientationPage;
      
      /// the data widget
      QWidget* dataPage;
      
      /// dimension X label
      QLabel* dimensionXLabel;
      
      /// dimension Y line edit
      QLabel* dimensionYLabel;
      
      /// dimension Z line edit
      QLabel* dimensionZLabel;
      
      /// volume type combo box
      QComboBox* volumeTypeComboBox;
      
      /// volume file combo box
      QComboBox* volumeFileComboBox;
      
      /// voxel size X float spin box
      QDoubleSpinBox* voxelSizeXDoubleSpinBox;
      
      /// voxel size Y float spin box
      QDoubleSpinBox* voxelSizeYDoubleSpinBox;
      
      /// voxel size Z float spin box
      QDoubleSpinBox* voxelSizeZDoubleSpinBox;
      
      /// Origin X float spin box
      QDoubleSpinBox* originXDoubleSpinBox;
      
      /// Origin Y float spin box
      QDoubleSpinBox* originYDoubleSpinBox;
      
      /// Origin Z float spin box
      QDoubleSpinBox* originZDoubleSpinBox;
      
      /// X orientation combo box
      GuiVolumeFileOrientationComboBox* xOrientationComboBox;
      
      /// Y orientation combo box
      GuiVolumeFileOrientationComboBox* yOrientationComboBox;
      
      /// Z orientation combo box
      GuiVolumeFileOrientationComboBox* zOrientationComboBox;
      
      /// the resampling page
      QWidget* resamplingPage;
      
      /// resampling  float spin box
      QDoubleSpinBox* resamplingXDoubleSpinBox;
      
      /// resampling  float spin box
      QDoubleSpinBox* resamplingYDoubleSpinBox;
      
      /// resampling  float spin box
      QDoubleSpinBox* resamplingZDoubleSpinBox;
      
      /// aligment AC X
      QSpinBox* alignAcXSpinBox;
      
      /// aligment AC Y
      QSpinBox* alignAcYSpinBox;
      
      /// aligment AC Z
      QSpinBox* alignAcZSpinBox;
      
      /// aligment PC X
      QSpinBox* alignPcXSpinBox;
      
      /// aligment PC Y
      QSpinBox* alignPcYSpinBox;
      
      /// aligment PC Z
      QSpinBox* alignPcZSpinBox;
      
      /// aligment LF X
      QSpinBox* alignLfXSpinBox;
      
      /// aligment LF Y
      QSpinBox* alignLfYSpinBox;
      
      /// aligment LF Z
      QSpinBox* alignLfZSpinBox;
};

#endif //  __GUI_VOLUME_ATTRIBUTES_DIALOG_H__

