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


#ifndef __GUI_INFLATE_AND_SMOOTH_FINGERS_DIALOG_H__
#define __GUI_INFLATE_AND_SMOOTH_FINGERS_DIALOG_H__

#include <vector>

#include "QtDialog.h"

class QCheckBox;
class QLabel;
class QRadioButton;
class QSpinBox;
class QDoubleSpinBox;

/// Dialog for surface smoothing.
class GuiInflateAndSmoothFingersDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiInflateAndSmoothFingersDialog(QWidget* parent, 
                                       const bool modal);
      
      /// Destructor
      ~GuiInflateAndSmoothFingersDialog();
      
   private slots:
      /// callde when apply button pressed
      void slotApply();
      
      /// called during modal operation
      virtual void done(int r);
      
   private:
      /// called to execute the smoothing
      bool doSmoothing();
      
      /// smoothing cycles spin box
      QSpinBox* smoothingCyclesSpinBox;
      
      /// regular smoothing strength float spin box
      QDoubleSpinBox* regularSmoothStrengthDoubleSpinBox;
      
      /// regular smoothing iterations spin box
      QSpinBox* regularSmoothIterationsSpinBox;
      
      /// inflation factor float spin box
      QDoubleSpinBox* inflationFactorDoubleSpinBox;
      
      /// finger smoothing threshold float spin box
      QDoubleSpinBox* fingerSmoothingThresholdDoubleSpinBox;
      
      /// finger smooothing strength float spin box
      QDoubleSpinBox* fingerSmoothingStrengthDoubleSpinBox;
      
      /// finger smoothing iterations spin box
      QSpinBox* fingerSmoothingIterationsSpinBox;
      
      /// add measurements to metrics check box
      QCheckBox* addToMetricsCheckBox;
};

#endif // __GUI_INFLATE_AND_SMOOTH_FINGERS_DIALOG_H__
