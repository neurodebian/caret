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


#ifndef __GUI_SECTION_SMOOTHING_DIALOG_H__
#define __GUI_SECTION_SMOOTHING_DIALOG_H__

#include <vector>
#include "WuQDialog.h"

class BrainModelSurface;
class QCheckBox;
class QGroupBox;
class QLabel;
class QRadioButton;
class QSpinBox;
class QDoubleSpinBox;

/// Dialog for surface smoothing.
class GuiSmoothingDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiSmoothingDialog(QWidget* parent, 
                         const bool modal,
                         const bool showLandmarkConstrainedSelection,
                         std::vector<bool>* smoothOnlyTheseNodesIn);
      
      /// Destructor
      ~GuiSmoothingDialog();
      
      // set the smoothing surface (if not set, does smooths main window surface)
      void setSmoothingSurface(BrainModelSurface* smoothingSurfaceIn);
      
   private slots:
      /// callde when apply button pressed
      void slotApply();
      
      /// called during modal operation
      virtual void done(int r);
      
      /// smoothing type selection
      void slotSmoothingType();
      
   private:
      /// types of smoothing mode
      enum SMOOTHING_MODE {
         MODE_NOT_SELECTED,
         MODE_AREAL_SMOOTHING,
         MODE_CROSSOVER_SMOOTHING,
         MODE_CURVATURE_SMOOTHING,
         MODE_FLAT_OVERLAP_SMOOTHING,
         MODE_LANDMARK_SMOOTHING,
         MODE_LANDMARK_NEIGHBOR_SMOOTHING,
         MODE_LINEAR_SMOOTHING
      };
      
      /// type of smoothing selected
      SMOOTHING_MODE smoothingMode;
      
      /// called to execute the smoothing
      bool doSmoothing();
      
      /// mark the landmark nodes (returns true if there are landmark nodes)
      bool markLandmarkNodes(BrainModelSurface* bms,
                             std::vector<bool>& landmarkNodes,
                             QString& errorMessage);
      
      /// areal smoothing radio button
      QRadioButton* arealSmoothingRadioButton;

      /// curvature smoothing radio button
      QRadioButton* curvatureSmoothingRadioButton;
      
      /// linear smoothing radio button
      QRadioButton* linearSmoothingRadioButton;
      
      /// landmark smoothing radio button
      QRadioButton* landmarkSmoothingRadioButton;
      
      /// landmark neighbor smoothing radio button
      QRadioButton* landmarkNeighborSmoothingRadioButton;
      
      /// smooth out crossovers
      QRadioButton* crossoverSmoothingRadioButton;
      
      /// smooth out flat surface overlap
      QRadioButton* flatOverlapSmoothingRadioButton;
      
      /// iterations spin box
      QSpinBox* iterationsSpinBox;
      
      /// edge iterations label
      QLabel* edgeIterationsLabel;
      
      /// edge iterations spin box
      QSpinBox* edgeIterationsSpinBox;
      
      /// strength spin box
      QDoubleSpinBox* strengthSpinBox;
      
      /// threads spin box
      QSpinBox* threadsSpinBox;
      
      /// project to sphere every X Iterations check box
      QCheckBox* projectToSphereIterationsCheckBox;
      
      /// project to sphere every X Iterations spin box
      QSpinBox* projectToSphereIterationsSpinBox;
      
      /// number of cycles label
      QLabel* numberOfCyclesLabel;
      
      /// number of cycles spin box
      QSpinBox* numberOfCyclesSpinBox;
      
      /// depth label
      QLabel* depthLabel;
      
      /// depth spin box
      QSpinBox* depthSpinBox;
      
      /// curvature maximum float spin box
      QDoubleSpinBox* curvatureMaximumDoubleSpinBox;
      
      /// curvature maximum label
      QLabel* curvatureMaximumLabel;
      
      /// limit smoothing to only the selected nodes
      std::vector<bool>* smoothOnlyTheseNodes;
      
      /// sphere options group box
      QGroupBox* sphereOptionsGroupBox;
      
      /// update normals check box
      QCheckBox* updateNormalsCheckBox;
      
      /// smoothing surface (if NULL, main window surface is smoothed)
      BrainModelSurface* smoothingSurface;
};

#endif
