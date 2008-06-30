
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

#ifndef __GUI_FOCI_PALS_PROJECTION_DIALOG_H__
#define __GUI_FOCI_PALS_PROJECTION_DIALOG_H__

#include "WuQDialog.h"

class QCheckBox;
class QGroupBox;
class QRadioButton;
class QDoubleSpinBox;

/// class for dialog used to project cells and foci
class GuiFociPalsProjectionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiFociPalsProjectionDialog(QWidget* parent);
      
      /// Destructor
      ~GuiFociPalsProjectionDialog();
      
      /// get cerebral cutoff distance
      static float getCerebralCutoffDistance() { return cerebralCutoffDistance; }
      
      /// get cerebellar cutoff distance
      static float getCerebellarCutoffDistance() { return cerebellarCutoffDistance; }

   protected slots:
      /// called when help button is pressed
      void slotHelpButton();
      
   protected:
      /// called when OK or Cancel button pressed
      virtual void done(int r);
      
      /// project onto surface check box
      QRadioButton* projectOntoSurfaceRadioButton;
      
      /// keep offset from surface
      QRadioButton* maintainOffsetFromSurfaceRadioButton;
      
      /// above surface distance
      QDoubleSpinBox* surfaceOffsetDoubleSpinBox;
      
      /// allow projection to cerebellum check box
      QGroupBox* cerebellarOptionsGroupBox;
      
      /// cerebral cutoff distance spin box
      QDoubleSpinBox* cerebralCutoffDistanceDoubleSpinBox;
      
      /// cerebellar cutoff distance spin box
      QDoubleSpinBox* cerebellarCutoffDistanceDoubleSpinBox;
      
      /// cerebral cutoff distance
      static float cerebralCutoffDistance;
      
      /// cerebellar cutoff distance
      static float cerebellarCutoffDistance;
};

#ifdef __GUI_FOCI_PALS_PROJECTION_DIALOG_MAIN__
float GuiFociPalsProjectionDialog::cerebralCutoffDistance = 2.0;
float GuiFociPalsProjectionDialog::cerebellarCutoffDistance = 4.0;
#endif // __GUI_FOCI_PALS_PROJECTION_DIALOG_MAIN__

#endif // __GUI_FOCI_PALS_PROJECTION_DIALOG_H__

