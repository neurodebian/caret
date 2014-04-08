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

#ifndef __GUI_CONTOUR_SET_SCALE_DIALOG_H__
#define __GUI_CONTOUR_SET_SCALE_DIALOG_H__

#include "WuQDialog.h"

class QLabel;
class QDoubleSpinBox;

/// Dialog for setting contour scaling
class GuiContourSetScaleDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiContourSetScaleDialog(QWidget* parent = 0, bool modalFlag = false, Qt::WindowFlags f = 0);
      
      /// Destructor
      ~GuiContourSetScaleDialog();
      
      /// set the scale start point
      void setScaleStartPoint(const float x, const float y);
      
      /// set the scale end point
      void setScaleEndPoint(const float x, const float y);
      
      /// update the dialog due to contour changes
      void updateDialog();
      
   private slots:
      /// Called when apply button is pressed
      void slotApplyButton();
      
      /// Called when reset button is pressed
      void slotResetButton();
      
   private:
      /// scale start point x label
      QLabel* scaleStartPointXLabel;
      
      /// scale start point y label
      QLabel* scaleStartPointYLabel;
      
      /// scale end point x label
      QLabel* scaleEndPointXLabel;
      
      /// scale end point y label
      QLabel* scaleEndPointYLabel;
      
      /// distance line edit
      QDoubleSpinBox* distanceDoubleSpinBox;
      
      /// scale start point
      float scaleStartPoint[2];
      
      /// scale end point
      float scaleEndPoint[2];
};

#endif // __GUI_CONTOUR_SET_SCALE_DIALOG_H__

