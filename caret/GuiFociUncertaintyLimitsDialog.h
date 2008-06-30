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


#ifndef __VE_GUI_FOCI_UNCERTAINTY_LIMITS_DIALOG_H__
#define __VE_GUI_FOCI_UNCERTAINTY_LIMITS_DIALOG_H__

#include <QDialog>

class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLineEdit;

/// This dialog creates an RGB Paint file from foci proximities to surface.
class GuiFociUncertaintyLimitsDialog : public QDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiFociUncertaintyLimitsDialog(QWidget* parent);
       
      /// Destructor
      ~GuiFociUncertaintyLimitsDialog();
       
   private:
      /// called when OK or Cancel button pressed
      void done(int r);
      
      /// create the left surface widgets
      QGroupBox* createLeftSurfaceWidgets();
      
      /// create the right surface widgets
      QGroupBox* createRightSurfaceWidgets();
      
      /// create the limits widgets
      QWidget* createLimitsWidgets();
      
      /// group box for right surface
      QGroupBox* rightSurfaceGroupBox;
      
      /// group box for left surface
      QGroupBox* leftSurfaceGroupBox;
      
      /// left surface combo box
      GuiBrainModelSelectionComboBox* leftSurfaceComboBox;
      
      /// right surface combo box
      GuiBrainModelSelectionComboBox* rightSurfaceComboBox;
      
      /// lower uncertainty limit
      QDoubleSpinBox* lowerLimitDoubleSpinBox;
      
      /// middle uncertainty limit
      QDoubleSpinBox* middleLimitDoubleSpinBox;
      
      /// upper uncertainty limit
      QDoubleSpinBox* upperLimitDoubleSpinBox;
      
      /// rgb paint left column new name
      QLineEdit* rgbPaintLeftColumnNewNameLineEdit;
      
      /// rgb paint right column new name
      QLineEdit* rgbPaintRightColumnNewNameLineEdit;
      
      /// rgb paint column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* rgbPaintLeftSelectionComboBox;
      
      /// rgb paint column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* rgbPaintRightSelectionComboBox;
      
      /// lower limit saved between dialogs
      static float lowerLimit;

      /// middle limit saved between dialogs
      static float middleLimit;

      /// upper limit saved between dialogs
      static float upperLimit;

};

#ifdef __GUI_FOCI_UNCERTAINTY_DIALOG_MAIN__
float GuiFociUncertaintyLimitsDialog::lowerLimit =   0.0;
float GuiFociUncertaintyLimitsDialog::middleLimit =  0.0;
float GuiFociUncertaintyLimitsDialog::upperLimit =  10.0;
#endif // __GUI_FOCI_UNCERTAINTY_DIALOG_MAIN__

#endif // __VE_GUI_FOCI_UNCERTAINTY_LIMITS_DIALOG_H__

