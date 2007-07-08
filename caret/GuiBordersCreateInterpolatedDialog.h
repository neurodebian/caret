
#ifndef __GUI_BORDERS_CREATE_INTERPOLATED_DIALOG_H__
#define __GUI_BORDERS_CREATE_INTERPOLATED_DIALOG_H__

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

#include "QtDialogNonModal.h"

class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QSpinBox;

/// dialog for creating interpolated borders
class GuiBordersCreateInterpolatedDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // interpolate border selection
      enum INTERPOLATE_BORDER {
         // first border for interpolation
         INTERPOLATE_BORDER_1,
         // second border for interpolation
         INTERPOLATE_BORDER_2
      };
      
      // constructor
      GuiBordersCreateInterpolatedDialog(QWidget* parent);
      
      // destructor
      ~GuiBordersCreateInterpolatedDialog();
      
      // called to update a border selection
      void updateBorderSelection(const INTERPOLATE_BORDER interpolateBorderChoice,
                                 const int borderIndex);
                                 
   protected slots:
      /// called when Apply button pressed
      void slotApplyPushButton();
      
      /// set the mouse mode for picking borders
      void slotSetMouseModeForThisDialog();
      
   protected:
      /// sampling double spin box
      QDoubleSpinBox* samplingDoubleSpinBox;
      
      /// name prefix line edit
      QLineEdit* namePrefixLineEdit;
      
      /// number of interpolated borders spin box
      QSpinBox* numberOfInterplatedBordersSpinBox;
      
      /// label for first border number and name
      QLabel* firstBorderNumberAndNameLabel;
      
      /// label for second border number and name
      QLabel* secondBorderNumberAndNameLabel;
      
      /// index of first border
      int firstBorderIndex;
      
      /// index of second border
      int secondBorderIndex;
      
};

#endif // __GUI_BORDERS_CREATE_INTERPOLATED_DIALOG_H__

