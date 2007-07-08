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

#ifndef __GUI_CONTOUR_RECONSTRUCTION_DIALOG_H__
#define __GUI_CONTOUR_RECONSTRUCTION_DIALOG_H__

#include "QtDialog.h"

class GuiStructureComboBox;
class QCheckBox;
class QSpinBox;

/// Dialog for constructing a surface from contours
class GuiContourReconstructionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      
      /// Constructor
      GuiContourReconstructionDialog(QWidget* parent, bool modal = true, Qt::WFlags f = 0);
      
      /// Destructor
      ~GuiContourReconstructionDialog();
      
   private slots:
      /// Called when OK or Cancel button is pressed
      void done(int r);
      
   private:
      /// load saved values
      void loadSavedValues();
      
      /// structure selection combo box
      GuiStructureComboBox* structureComboBox;
      
      /// convert cells hemisphere check box
      QCheckBox* convertContourCellsCheckBox;
      
      /// voxel dimension spin box
      QSpinBox* voxelDimensionSpinBox;
      
      /// polygon limit spin box
      QSpinBox* polygonLimitSpinBox;
      
      /// saved convert cells
      static bool savedConvertCells;
      
      /// saved voxel dimensions
      static int savedVoxelDimensions;
      
      /// saved polygon limit
      static int savedPolygonLimit;
};      

#ifdef __GUI_CONTOUR_RECONSTRUCTION_DIALOG_MAIN__
bool GuiContourReconstructionDialog::savedConvertCells    = true;
int GuiContourReconstructionDialog::savedVoxelDimensions  = 128;
int GuiContourReconstructionDialog::savedPolygonLimit     = 100000;
#endif // __GUI_CONTOUR_RECONSTRUCTION_DIALOG_MAIN__

#endif // __GUI_CONTOUR_RECONSTRUCTION_DIALOG_H__

