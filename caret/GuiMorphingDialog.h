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

#ifndef __GUI_MORPHING_DIALOG_H__
#define __GUI_MORPHING_DIALOG_H__

#include "QtDialog.h"

#include "BrainModelSurfaceMorphing.h"

class GuiBrainModelSelectionComboBox;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;

/// Dialog for morphing
class GuiMorphingDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiMorphingDialog(QWidget* parent, 
                  const BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingSurfaceTypeIn);
                  
      /// Destructor
      ~GuiMorphingDialog();
      
      /// update the dialog
      void updateDialog();
      
   private slots:
      /// called when apply button pressed
      void slotApplyButton();
      
   private: 
      /// create the parameters section
      QWidget* createParametersSection();
      
      /// create the misc section
      QWidget* createMiscSection();
      
      /// create the surface section
      QWidget* createSurfaceSection();
      
      /// type of morphing
      BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingSurfaceType;
      
      /// reference surface combo box
      GuiBrainModelSelectionComboBox* referenceSurfaceComboBox;
      
      /// morphing surface combo box
      GuiBrainModelSelectionComboBox* morphingSurfaceComboBox;
      
      /// iterations spin box
      QSpinBox* iterationsSpinBox;
   
      /// linear force float spin box
      QDoubleSpinBox* linearForceDoubleSpinBox;
      
      /// angular force float spin box
      QDoubleSpinBox* angularForceDoubleSpinBox;
      
      /// step size float spin box
      QDoubleSpinBox* stepSizeDoubleSpinBox;
      
      /// landmark morphing check box
      QCheckBox* landmarkMorphingCheckBox;
      
      /// measurements each iteration check box
      QCheckBox* measureEachIterationCheckBox;
      
      /// number of threads spin box
      QSpinBox* threadsSpinBox;
};

#endif // __GUI_MULTIRESOLUTION_MORPHING_DIALOG_H__


