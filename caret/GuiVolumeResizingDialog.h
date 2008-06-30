
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

#ifndef __GUI_VOLUME_RESIZING_DIALOG_H__
#define __GUI_VOLUME_RESIZING_DIALOG_H__

#include "WuQDialog.h"

class QCheckBox;
class QSpinBox;

/// Dialog for resizing a volume
class GuiVolumeResizingDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiVolumeResizingDialog(QWidget* parent);
   
      /// Destructor
      ~GuiVolumeResizingDialog();

      /// shows the dialog (override of QDialog::show())
      virtual void show();
      
      /// update the dialog
      void updateDialog(const bool fileChangeUpdateFlag);
      
   public slots:
      /// Called when OK button pressed
      void slotOKButton();
      
      /// called when dialog is being closed
      virtual void close();
      
      /// reset the cropping slices
      void slotResetPushButton();
      
   private slots:
      /// called when x minimum spin box changed
      void slotXMinSpinBox(int value);
      
      /// called when x maximum spin box changed
      void slotXMaxSpinBox(int value);
      
      /// called when y minimum spin box changed
      void slotYMinSpinBox(int value);
      
      /// called when y maximum spin box changed
      void slotYMaxSpinBox(int value);
      
      /// called when z minimum spin box changed
      void slotZMinSpinBox(int value);
      
      /// called when z maximum spin box changed
      void slotZMaxSpinBox(int value);
      
   private:
      /// x minimum spin box
      QSpinBox* xMinSpinBox;
      
      /// x maximum spin box
      QSpinBox* xMaxSpinBox;
      
      /// x minimum spin box
      QSpinBox* yMinSpinBox;
      
      /// x maximum spin box
      QSpinBox* yMaxSpinBox;
      
      /// x minimum spin box
      QSpinBox* zMinSpinBox;
      
      /// x maximum spin box
      QSpinBox* zMaxSpinBox;
      
      /// dialog is being updated
      bool updatingDialog;
      
      /// use X/Y/Z Min for ParametersFile X/Y/Z min
      QCheckBox* useXyzMinForParametersCheckBox;
};

#endif // __GUI_VOLUME_RESIZING_DIALOG_H__

