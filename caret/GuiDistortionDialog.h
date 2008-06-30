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

#ifndef __GUI_DISTORTION_DIALOG_H__
#define __GUI_DISTORTION_DIALOG_H__

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class BrainModelSurface;
class GuiNodeAttributeColumnSelectionComboBox;
class QLineEdit;

/// Dialog for generating surface distortion measurements
class GuiDistortionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiDistortionDialog(QWidget* parent, BrainModelSurface* bmsIn);
      
      /// destructor
      ~GuiDistortionDialog();
   
   private slots:
      /// called when OK/Cancel buttons pressed
      void done(int r);
      
      /// called when linear distortion column selected
      void slotLinearDistortion(int item);
      
      /// called when areal distortion column selected
      void slotArealDistortion(int item);
      
   private:
      /// brain model surface for which distortion is being calculated 
      BrainModelSurface* bms;
      
      /// reference surface selection combo box
      GuiBrainModelSelectionComboBox* referenceSurfaceComboBox;
      
      /// areal distortion column name
      QLineEdit* arealDistortionNameLineEdit;
      
      /// areal distortion surface shape column
      GuiNodeAttributeColumnSelectionComboBox* arealDistortionColumnComboBox;
      
      /// linear distortion column name
      QLineEdit* linearDistortionNameLineEdit;
      
      /// linear distortion surface shape column
      GuiNodeAttributeColumnSelectionComboBox* linearDistortionColumnComboBox;
};

#endif // __GUI_DISTORTION_DIALOG_H__

