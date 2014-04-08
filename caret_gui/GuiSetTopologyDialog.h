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


#ifndef __GUI_SET_TOPOLOGY_DIALOG_H__
#define __GUI_SET_TOPOLOGY_DIALOG_H__

#include <vector>

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class GuiTopologyFileComboBox;

/// dialog for setting the topology file for surfaces
class GuiSetTopologyDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiSetTopologyDialog(QWidget* parent);
      
      /// destructor
      ~GuiSetTopologyDialog();
      
      /// update the dialog
      void updateDialog();
      
   private slots:
      /// called when a surface is selected
      void slotSurfaceSelection();
      
      /// called when a topology is selected
      void slotTopologySelection();
      
   private:
      /// surface selection combo box
      GuiBrainModelSelectionComboBox* surfaceComboBox;
      
      /// topology selection combo box
      GuiTopologyFileComboBox* topologyComboBox;
};

#endif // __GUI_SET_TOPOLOGY_DIALOG_H__

