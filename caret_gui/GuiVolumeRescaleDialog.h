
#ifndef __GUI_VOLUME_RESCALE_DIALOG_H__
#define __GUI_VOLUME_RESCALE_DIALOG_H__

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

#include "WuQDialog.h"

class VolumeFile;
class QLineEdit;

/// dialog for rescaling a volume's voxel values
class GuiVolumeRescaleDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // Constructor
      GuiVolumeRescaleDialog(QWidget* parent,
                             const QString& message,
                             VolumeFile* volumeFileIn,
                             const float* outputMinMaxValues = NULL);
      
      // Destructor
      ~GuiVolumeRescaleDialog();
      
   protected slots:
      // called when set input values button pressed
      void slotSetInputValuesPushButton();
      
      // called when set output values button pressed
      void slotSetOutputValuesPushButton();
      
   protected:
      // loads the input min/max line edits
      void loadInputMinMaxLineEdits(const float minValue, const float maxValue);
      
      // loads the output min/max line edits
      void loadOutputMinMaxLineEdits(const float minValue, const float maxValue);
      
      // called when ok/cancel pressed
      void done(int r);
      
      // choose values selection dialog
      void chooseValuesDialog(const bool inputVolumeFlag);
      
      /// the volume file
      VolumeFile* volumeFile;
      
      /// input min line edit
      QLineEdit* inputMinimumLineEdit;

      /// input max line edit
      QLineEdit* inputMaximumLineEdit;

      /// output min line edit
      QLineEdit* outputMinimumLineEdit;

      /// output max line edit
      QLineEdit* outputMaximumLineEdit;

};

#endif // __GUI_VOLUME_RESCALE_DIALOG_H__

