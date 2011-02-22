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

#ifndef __GUI_MULTIRESOLUTION_MORPHING_DIALOG_H__
#define __GUI_MULTIRESOLUTION_MORPHING_DIALOG_H__

#include "WuQDialog.h"

#include "BrainModelSurfaceMorphing.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"

class GuiBrainModelSelectionComboBox;
class QComboBox;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QTextEdit;

/// Dialog for multiresolution morphing
class GuiMultiresolutionMorphingDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiMultiresolutionMorphingDialog(QWidget* parent, 
                  BrainModelSurfaceMultiresolutionMorphing* morphObjectIn,
                  const bool parametersOnlyModeIn);
                  
      /// Destructor
      ~GuiMultiresolutionMorphingDialog();
      
   private slots:
      /// called when OK/Cancel buttons pressed
      void done(int r);
      
      /// called when Batch button is pressed.
      void slotBatchButton();
      
      /// called when  combo box item is selected
      void slotNumberOfCyclesSpinBox();
      
      /// called when  combo box item is selected
      void slotEditCycleSpinBox();
      
      /// called when edit cycle combo box item is highlighted before selection made
      void slotEditCycleSpinBoxOldValue();

   private: 
         
      /// read parameters from the dialog and place in morphing object
      void readParametersFromDialog();
         
      /// load parameters from the morphing object into the dialog
      void loadParametersIntoDialog();
         
      /// create the iterations section
      QWidget* createCyclesSection();
      
      /// create the levels section
      QWidget* createLevelsSection();
      
      /// create the iterations section
      QWidget* createIterationsSection();
      
      /// create the parameters section
      QWidget* createParametersSection();
      
      /// create the misc section
      QWidget* createMiscSection();
      
      /// create the surface section
      QWidget* createSurfaceSection();
      
      /// create the smoothing section
      QWidget* createSmoothingSection();
      
      /// reference surface combo box
      GuiBrainModelSelectionComboBox* referenceSurfaceComboBox;
      
      /// morphing surface combo box
      GuiBrainModelSelectionComboBox* morphingSurfaceComboBox;
      
      /// number of cycles spin box
      QComboBox* numberOfCyclesComboBox;
      
      /// edit cycle spin box
      QComboBox* editCycleComboBox;
      
      /// iterations spin boxes
      QSpinBox* iterationsSpinBoxes[BrainModelSurfaceMultiresolutionMorphing::MAXIMUM_NUMBER_OF_LEVELS];
   
      /// linear force float spin box
      QDoubleSpinBox* linearForceDoubleSpinBox;
      
      /// angular force float spin box
      QDoubleSpinBox* angularForceDoubleSpinBox;
      
      /// step size float spin box
      QDoubleSpinBox* stepSizeDoubleSpinBox;
      
      /// smoothing strength float spin box
      QDoubleSpinBox* smoothingStrengthDoubleSpinBox;
      
      /// smoothing edge iterations spin box
      QSpinBox* smoothingIterationsSpinBox;
      
      /// smoothing iterations spin box
      QSpinBox* smoothEdgesEverySpinBox;
      
      /// delete temporary files check box
      QCheckBox* deleteTempFilesCheckBox;
      
      /// smooth out overlap check box
      QCheckBox* smoothOutOverlapCheckBox;
      
      /// smooth out crossovers check box
      QCheckBox* smoothOutCrossoversCheckBox;
      
      /// point spherical tiles outward check box
      QCheckBox* pointSphericalTilesOutwardCheckBox;
      
      /// align to central sulcus check box
      QCheckBox* alignSurfaceCheckBox;
      
      /// name of central sulcus for alignment
      QLineEdit* cesLandmarkNameLineEdit;
      
      /// only display and process parameters with no morphing
      bool parametersOnlyMode;      
      
      /// the multiresolution morphing object
      BrainModelSurfaceMultiresolutionMorphing* morphObject;
      
      /// the type of morphing
      BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingSurfaceType;
      
};

#endif // __GUI_MULTIRESOLUTION_MORPHING_DIALOG_H__


