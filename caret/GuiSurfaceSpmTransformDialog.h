
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


#ifndef __GUI_SURFACE_SPM_TRANSFORM_DIALOG_H__
#define __GUI_SURFACE_SPM_TRANSFORM_DIALOG_H__

#include <QString>

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class QCheckBox;
class QLabel;

/// Dialog for transforming a surface using SPM inverse transforms
class GuiSurfaceSpmTransformDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiSurfaceSpmTransformDialog(QWidget* parent);
      
      /// Destructor
      ~GuiSurfaceSpmTransformDialog();
      
   private slots:
      /// called when apply button pressed
      void done(int r);
      
      /// called when normalized volume select push button pressed
      void slotNormalizedVolumeButton();
      
      /// called when inverse X volume select push button pressed
      void slotInverseXVolumeButton();
      
      /// called when inverse Y volume select push button pressed
      void slotInverseYVolumeButton();
      
      /// called when inverse Z volume select push button pressed
      void slotInverseZVolumeButton();
      
   private:
      enum FILE_SELECTION_TYPE {
         FILE_SELECTION_VOLUME_NORMALIZED,
         FILE_SELECTION_VOLUME_INVERSE_X,
         FILE_SELECTION_VOLUME_INVERSE_Y,
         FILE_SELECTION_VOLUME_INVERSE_Z
      };
         
      /// called to select a volume file
      void selectVolumeFile(const FILE_SELECTION_TYPE fst);
      
      /// display the selected files
      void displayFileNames();
      
      /// combo box for selecting coord file
      GuiBrainModelSelectionComboBox* coordFileComboBox;
      
      /// label normalized volume
      QLabel* normalizedVolumeLabel;
      
      /// label for inverse X volume
      QLabel* inverseXVolumeLabel;
      
      /// label for inverse Y volume
      QLabel* inverseYVolumeLabel;
      
      /// label for inverse Z volume
      QLabel* inverseZVolumeLabel;
      
      /// interpolate voxels checkbox
      QCheckBox* interpolateVoxelsCheckBox;
      
      /// name of volume file
      static QString normalizedVolumeFileName;
      
      /// name of inverse X volume file
      static QString inverseXVolumeFileName;
      
      /// name of inverse Y volume file
      static QString inverseYVolumeFileName;
      
      /// name of inverse Z volume file
      static QString inverseZVolumeFileName;
      
      /// interpolate among voxels
      static bool interpolateVoxelsFlag;
};

#ifdef __GUI_SURFACE_SPM_TRANSFORM_DIALOG_MAIN_H__
   QString GuiSurfaceSpmTransformDialog::normalizedVolumeFileName;
   QString GuiSurfaceSpmTransformDialog::inverseXVolumeFileName;
   QString GuiSurfaceSpmTransformDialog::inverseYVolumeFileName;
   QString GuiSurfaceSpmTransformDialog::inverseZVolumeFileName;
   bool        GuiSurfaceSpmTransformDialog::interpolateVoxelsFlag = true;
#endif // __GUI_SURFACE_SPM_TRANSFORM_DIALOG_MAIN_H__

#endif // __GUI_SURFACE_SPM_TRANSFORM_DIALOG_H__

