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


#ifndef __GUI_SURFACE_INFORMATION_DIALOG_H__
#define __GUI_SURFACE_INFORMATION_DIALOG_H__

#include <vector>

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class GuiStructureComboBox;
class QGridLayout;
class QLabel;
class QPushButton;

/// dialog for setting and viewing surface information
class GuiSurfaceInformationDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiSurfaceInformationDialog(QWidget* parent);
      
      /// destructor
      ~GuiSurfaceInformationDialog();
      
   private slots:
      /// called when a surface is selected
      void slotSurfaceSelection();
      
      /// called when a hemisphere is selected
      void slotHemisphereComboBox();
      
   private:
      /// surface selection combo box
      GuiBrainModelSelectionComboBox* surfaceComboBox;
      
      /// label for names
      std::vector<QLabel*> namesLabels;
      
      /// label for values
      std::vector<QLabel*> valuesLabels;
      
      /// hemisphere combo box
      GuiStructureComboBox* hemisphereComboBox;
      
      /// layout for names and values
      QGridLayout* namesAndValuesGridLayout;
};

#endif // __GUI_SURFACE_INFORMATION_DIALOG_H__

