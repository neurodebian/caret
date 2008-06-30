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


#ifndef __GUI_INTERPOLATE_SURFACES_DIALOG_H__
#define __GUI_INTERPOLATE_SURFACES_DIALOG_H__

#include <vector>

#include "WuQDialog.h"

class BrainModelSurface;
class GuiBrainModelSelectionComboBox;
class GuiTopologyFileComboBox;
class QCheckBox;
class QGridLayout;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;

/// dialog for interpolating surfaces
class GuiInterpolateSurfacesDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiInterpolateSurfacesDialog(QWidget* parent);
      
      /// destructor
      ~GuiInterpolateSurfacesDialog();
      
      /// update the dialog
      void updateDialog();
      
   public slots:
      /// called when apply button pressed
      void slotApplyButton();
      
   private:
      /// update projected files
      void updateProjectedFiles(const BrainModelSurface* bms);
      
      /// surface selection combo boxes
      std::vector<GuiBrainModelSelectionComboBox*> surfaceComboBoxes;
      
      /// labels for combo boxes
      std::vector<QLabel*> surfaceLabels;
      
      /// topology file selection combo box
      GuiTopologyFileComboBox* topologyComboBox;
      
      /// interpolation steps spin box
      QSpinBox* interpolationStepsSpinBox;
      
      /// reproject borders each step
      QCheckBox* reprojectBordersCheckBox;
      
      /// rotate non-flat surfaces check box
      QCheckBox* rotateSurfacesCheckBox;
      
      /// rotation step float spin box
      QDoubleSpinBox* rotationStepDoubleSpinBox;
      
      /// layout for surfaces list
      QGridLayout* surfaceGridLayout;
};

#endif // __GUI_INTERPOLATE_SURFACES_DIALOG_H__

