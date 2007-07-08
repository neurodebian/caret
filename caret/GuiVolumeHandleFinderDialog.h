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


#ifndef __GUI_VOLUME_HANDLE_FINDER_DIALOG_H__
#define __GUI_VOLUME_HANDLE_FINDER_DIALOG_H__

#include <vector>

#include "QtDialog.h"

#include "BrainModelVolumeTopologicalError.h"

class QCheckBox;
class QListWidget;
class QPushButton;
class QRadioButton;
class QtWidgetGroup;

/// Dialog for handle finding
class GuiVolumeHandleFinderDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiVolumeHandleFinderDialog(QWidget* parent);
      
      /// destructor
      ~GuiVolumeHandleFinderDialog();
      
      /// called to display the dialog
      virtual void show();
      
      /// called to close the dialog
      virtual void close();
      
   private slots:
      /// called when find handles pushbutton is pressed
      void slotFindHandlesPushButton();
      
      /// called when fill selected handle push button is pressed
      void slotFillHandlePushButton();
      
      /// called when and items is selected in the handles list box
      void handleListBoxSelection(int item);
      
      /// called when search method radio button clicked
      void slotSearchMethodButton();
      
   private:
      /// clear any current handles
      void clearHandles();
      
      /// load handles into list box
      void loadHandlesIntoListBox();
      
      /// search axis X check box
      QCheckBox* searchAxisXCheckBox;
      
      /// search axis Y check box
      QCheckBox* searchAxisYCheckBox;
      
      /// search axis Z check box
      QCheckBox* searchAxisZCheckBox;
      
      /// search segmentation radio button
      QRadioButton* searchSegmentationRadioButton;
      
      /// crossovers search radio button
      QRadioButton* searchCrossoversRadioButton;
      
      /// handles list box
      QListWidget* handlesListWidget;
      
      /// handles from last search
      std::vector<BrainModelVolumeTopologicalError> handles;
      
      /// create a volume showing the handles check box
      QCheckBox* createVolumeShowingHandlesCheckBox;
      
      /// create an rgb paint file showing the handles on the surface
      QCheckBox* createRgbPaintSurfaceHandlesCheckBox;
      
      /// index of selected handle
      int selectedHandleIndex;
      
      /// search segmentation widget group
      QtWidgetGroup* searchSegmentationWidgetGroup;
      
      /// fill handle push button
      QPushButton* fillHandlePushButton;
};

#endif // __GUI_VOLUME_HANDLE_FINDER_DIALOG_H__

