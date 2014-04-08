
#ifndef __GUI_BORDER_DRAW_UPDATE_DIALOG_H__
#define __GUI_BORDER_DRAW_UPDATE_DIALOG_H__

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

#include "BrainModelBorderSet.h"
#include "WuQDialog.h"

class QAbstractButton;
class QCheckBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QRadioButton;

/// dialog for drawing border updates
class GuiBorderDrawUpdateDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiBorderDrawUpdateDialog(QWidget* parent = 0);
      
      // destructor
      ~GuiBorderDrawUpdateDialog();
      
      // get the resampling density
      float getResampling() const;
      
      // get the 3D flag
      bool getThreeDimensional() const;
      
      // get the auto project border flag
      bool getAutoProjectBorder() const;
      
      // get the update mode
      BrainModelBorderSet::UPDATE_BORDER_MODE getBorderUpdateMode() const;
      
   protected slots:
      // called when standard button pressed
      void slotStandardButtonPressed(QAbstractButton*);
      
   protected:
      /// dialog button box
      QDialogButtonBox* dialogButtonBox;
      
      /// resampling spin box
      QDoubleSpinBox* resamplingDoubleSpinBox;
      
      /// dimension two radio button
      QRadioButton* dimensionTwoRadioButton;
      
      /// dimension three radio button
      QRadioButton* dimensionThreeRadioButton;
      
      /// auto project YES check box
      QRadioButton* autoProjectYesRadioButton;
      
      /// auto project NO check box
      QRadioButton* autoProjectNoRadioButton;
      
      ///  radio button
      QRadioButton* replaceSegmentInBorderModeRadioButton;
      
      ///  radio button
      QRadioButton* extendBorderModeRadioButton;
      
      /// radio button
      QRadioButton* eraseBorderModeRadioButton;
      
};

#endif // __GUI_BORDER_DRAW_UPDATE_DIALOG_H__
