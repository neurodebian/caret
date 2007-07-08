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


#ifndef __VE_GUI_AUTOMATIC_ROTATION_DIALOG_H__
#define __VE_GUI_AUTOMATIC_ROTATION_DIALOG_H__

#include "QtDialog.h"

class QPushButton;
class QRadioButton;
class QDoubleSpinBox;

/// Dialog for automatic rotation
class GuiAutomaticRotationDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiAutomaticRotationDialog(QWidget* parent);
      
      /// Destructor
      ~GuiAutomaticRotationDialog();
      
   private slots:
      /// called when an start button is pressed
      void startButtonSlot();
      
      /// called when stop button is pressed
      void stopButtonSlot();
      
      /// called when close button is pressed
      void closeButtonSlot();
      
   private:
      /// X axis radio button
      QRadioButton* xAxisRadioButton;
      
      /// Y axis radio button
      QRadioButton* yAxisRadioButton;
      
      /// Z axis radio button
      QRadioButton* zAxisRadioButton;
      
      /// Total fixed rotation spin box
      QDoubleSpinBox* fixedRotationSpinBox;
      
      /// Rotation increment spin box
      QDoubleSpinBox* incrementRotationSpinBox;
      
      /// the start pushbutton
      QPushButton* startButton;
      
      /// the stop pushbutton
      QPushButton* stopButton;
      
      /// fixed total rotation radio button
      QRadioButton* fixedTotalRadioButton;
      
      /// continuous rotation radio button
      QRadioButton* continuousTotalRadioButton;
      
      /// continue rotating flag
      bool continueRotatingFlag;
      
};

#endif  // __VE_GUI_AUTOMATIC_ROTATION_DIALOG_H__

