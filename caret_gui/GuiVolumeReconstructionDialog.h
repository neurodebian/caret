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

#ifndef __GUI_VOLUME_RECONSTRUCTION_DIALOG_H__
#define __GUI_VOLUME_RECONSTRUCTION_DIALOG_H__

#include "WuQDialog.h"

class QCheckBox;
class QComboBox;
class QSpinBox;
class QGroupBox;
class VolumeFile;

/// Dialog for constructing a surface from a volume
class GuiVolumeReconstructionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      
      /// Constructor
      GuiVolumeReconstructionDialog(QWidget* parent, VolumeFile* segmentationVolumeFileIn,
                                    bool modalFlag = true, Qt::WindowFlags f = 0);
      
      /// Destructor
      ~GuiVolumeReconstructionDialog();
      
   private slots:
      /// Called when OK or Cancel button is pressed
      void done(int r);
      
      /// called when surface type selected
      void slotSurfaceTypeComboBox(int item);
      
   private:
      /// surface type
      enum SURFACE_TYPE {
         /// brain model surface
         SURFACE_TYPE_BRAIN_MODEL = 0,
         /// solid structure
         SURFACE_TYPE_SOLID_STRUCTURE,
         /// visualization toolkit model
         SURFACE_TYPE_VTK_MODEL
      };
      
      /// segmentation volume being reconstructed
      VolumeFile* segmentationVolumeFile;
      
      /// left hemisphere check box
      QCheckBox* leftHemisphereCheckBox;
      
      /// right hemisphere check box
      QCheckBox* rightHemisphereCheckBox;
      
      /// hemisphere group box
      QGroupBox* hemisphereGroup;
      
      /// surface type combo box
      QComboBox* surfaceTypeComboBox;
      
      /// misc group box
      QGroupBox* miscGroup;
      
      /// hypersmooth check box
      QCheckBox* hypersmoothCheckBox;
      
      /// maximumu polygons check box
      QCheckBox* maximumPolygonsCheckBox;
};      

#ifdef __GUI_VOLUME_RECONSTRUCTION_DIALOG_MAIN__
#endif // __GUI_VOLUME_RECONSTRUCTION_DIALOG_MAIN__

#endif // __GUI_VOLUME_RECONSTRUCTION_DIALOG_H__

