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

#ifndef __GUI_GENERATE_SURFACE_CURVATURE_DIALOG_H__
#define __GUI_GENERATE_SURFACE_CURVATURE_DIALOG_H__

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class QLineEdit;

/// Class for dialog that generates surface curvature
class GuiGenerateSurfaceCurvatureDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiGenerateSurfaceCurvatureDialog(QWidget* parent); 
                                 
      /// Destructor
      ~GuiGenerateSurfaceCurvatureDialog();
      
   private slots:
      /// called when Ok or Cancel button pressed
      void done(int r);
      
   private:
      /// surface combo box
      GuiBrainModelSelectionComboBox* surfaceComboBox;
      
      /// curvature column combo box
      GuiNodeAttributeColumnSelectionComboBox* meanCurvatureColumnComboBox;
      
      /// curvature name line edit
      QLineEdit* meanCurvatureNameLineEdit;
      
      /// curvature column combo box
      GuiNodeAttributeColumnSelectionComboBox* gaussianCurvatureColumnComboBox;
      
      /// curvature name line edit
      QLineEdit* gaussianCurvatureNameLineEdit;
      
};

#endif // __GUI_GENERATE_SURFACE_CURVATURE_DIALOG_H__

