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

#ifndef __GUI_SET_VIEW_DIALOG_H__
#define __GUI_SET_VIEW_DIALOG_H__

#include "WuQDialog.h"

class BrainModel;
class QDoubleSpinBox;

/// class for setting the view of a surface
class GuiSetViewDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiSetViewDialog(QWidget* parent, 
                       BrainModel* brainModelIn,
                       const int viewNumberIn);
      
      /// Destructor
      ~GuiSetViewDialog();
      
   private slots:
      /// called when the view is changed.
      void slotViewChanged();
      
   private:
      /// Called when OK or Cancel button is pressed
      void done(int r);
      
      /// load transformation into the dialog
      void loadTransformation();
      
      /// the brain model
      BrainModel* brainModel;
      
      /// the view number of the brain model
      int viewNumber;
      
      /// x translate float spin box
      QDoubleSpinBox* xTranslateDoubleSpinBox;
      
      /// y translate float spin box
      QDoubleSpinBox* yTranslateDoubleSpinBox;
      
      /// z translate float spin box
      QDoubleSpinBox* zTranslateDoubleSpinBox;
      
      /// x rotate float spin box
      QDoubleSpinBox* xRotateDoubleSpinBox;
      
      /// y rotate float spin box
      QDoubleSpinBox* yRotateDoubleSpinBox;
      
      /// z rotate float spin box
      QDoubleSpinBox* zRotateDoubleSpinBox;
      
      /// x scale float spin box
      QDoubleSpinBox* xScaleDoubleSpinBox;
      
      /// y scale float spin box
      QDoubleSpinBox* yScaleDoubleSpinBox;
      
      /// z scale float spin box
      QDoubleSpinBox* zScaleDoubleSpinBox;
      
      /// creating dialog flag
      bool creatingDialog;
};

#endif // __GUI_SET_VIEW_DIALOG_H__

