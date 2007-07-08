
#ifndef __GUI_VOLUME_CREATE_DIALOG_H__
#define __GUI_VOLUME_CREATE_DIALOG_H__

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

#include "QtDialog.h"

class QComboBox;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;

/// dialog for creating a volume
class GuiVolumeCreateDialog : public QtDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiVolumeCreateDialog(QWidget* parent);
      
      // destructor
      ~GuiVolumeCreateDialog();
   
   protected slots:
      // called to set the volume attributes using a volume file
      void slotSetAttsFromFilePushButton();
      
      // called to set the volume attributes using a standard space
      void slotSetAttsUsingStandardSpace();
      
   protected:
      // called when OK or Cancel button pressed
      void done(int r);
      
      /// X dimension spin box
      QSpinBox* xDimensionSpinBox;
      
      /// Y dimension spin box
      QSpinBox* yDimensionSpinBox;
      
      /// Z dimension spin box
      QSpinBox* zDimensionSpinBox;
      
      /// X voxel size float spin box
      QDoubleSpinBox* xVoxelSizeDoubleSpinBox;
      
      /// Y voxel size float spin box
      QDoubleSpinBox* yVoxelSizeDoubleSpinBox;
      
      /// Z voxel size float spin box
      QDoubleSpinBox* zVoxelSizeDoubleSpinBox;
      
      /// X origin float spin box
      QDoubleSpinBox* xOriginDoubleSpinBox;
      
      /// Y origin float spin box
      QDoubleSpinBox* yOriginDoubleSpinBox;
      
      /// Z origin float spin box
      QDoubleSpinBox* zOriginDoubleSpinBox;
      
      /// volume type combo box
      QComboBox* volumeTypeComboBox;
      
      /// name line edit
      QLineEdit* nameLineEdit;
};

#endif // __GUI_VOLUME_CREATE_DIALOG_H__

