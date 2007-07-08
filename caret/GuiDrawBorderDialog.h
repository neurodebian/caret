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


#ifndef __VE_GUI_DRAW_BORDER_DIALOG_H__
#define __VE_GUI_DRAW_BORDER_DIALOG_H__

#include <QString>
#include "QtDialog.h"

#include "BorderFile.h"

class BrainModel;
class GuiNodeAttributeColumnSelectionComboBox;
class GuiVolumeSelectionControl;

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTabWidget;

/// class for dialog used to draw borders
class GuiDrawBorderDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiDrawBorderDialog(QWidget* parent);
      
      /// Destructor
      ~GuiDrawBorderDialog();
      
      /// get the name of the border
      QString getBorderName() const;
      
      /// get the closed border flag
      bool getClosedBorderFlag() const;
      
      /// get the resampling density
      float getResampling() const;
      
      /// get the 3D flag
      bool getThreeDimensional() const;
      
      /// new border just drawn - create it
      void createNewBorder(BrainModel* bm, Border& border);
      
      /// update the dialog
      void updateDialog();
      
      /// Called when dialog is shown
      virtual void show();

   private slots:
      /// called when apply button is pressed
      void slotApplyButton();
      
      /// called when close button is pressed
      void slotCloseButton();

      /// enable/disable items in the dialog based upon current selections
      void slotEnableDisableItems();
      
      /// called when a paint column assignment is selected
      void slotAssignPaintColumnSelection(int col);
      
      /// called when reassign node name pushbutton is pressed
      void slotReassignNodeNamePushButton();
      
      /// called when select name button is pressed
      void slotSelectNameButton();
      
   private:
      /// create the assign nodes widget
      void createAssignNodesWidget();
      
      /// create the assign voxels widget
      void createAssignVoxelsWidget();
      
      /// create the main page
      QWidget* createMainPage();
      
      /// border name line edit
      QLineEdit* nameLineEdit;
      
      /// resampling line edit
      QLineEdit* resamplingLineEdit;
      
      /// closed border radio button
      QRadioButton* closedBorderRadioButton;
      
      /// open border radio button
      QRadioButton* openBorderRadioButton;
      
      /// 2D radio button
      QRadioButton* twoDimensionalRadioButton;
      
      /// 3D radio button
      QRadioButton* threeDimensionalRadioButton;
      
      /// color index for the border
      int borderColorIndex;
      
      /// assign tab widget
      QTabWidget* assignTabWidget;
      
      /// Vertical group box for node assignment
      QGroupBox* assignVGroup;
      
      /// assign nodes check box
      QCheckBox* assignNodesCheckBox;
      
      /// Horizontal box for node paint assignment
      //QWidget* assignNodesPaintQHBox;
      
      /// assign paint column label
      QLabel* assignPaintColumnLabel;
      
      /// paint column assignment combo box
      GuiNodeAttributeColumnSelectionComboBox* assignPaintColumnComboBox;
      
      /// paint column name assign line edit
      QLineEdit* assignPaintColumnNameLineEdit;
      
      /// reassign name push button
      QPushButton* reassignNamePushButton;
      
      /// qvbox containing reassignment items
      //QWidget* reassignNodesQVBox;
      
      /// reassign nodes paint column row
      //QWidget* reassignColumnQHBox;
      
      /// reassign nodes check box
      QCheckBox* reassignNodesCheckBox;
      
      /// reassign nodes name qhbox
      //QWidget* reassignNameQHBox;
      
      /// label for the reassignment name
      QLabel* reassignNameLabel;
      
      /// paint column for node reassignment
      GuiNodeAttributeColumnSelectionComboBox* reassignNodesPaintColumnComboBox;
      
      /// assign voxels vbox
      QWidget* assignVoxelsVBox;
      
      /// assign voxels within border check box
      QCheckBox* assignVoxelsWithinBorderCheckBox;
      
      /// assign voxels to volume control
      GuiVolumeSelectionControl* assignVoxelsVolumeSelectionControl;
      
      /// assign node widget
      QWidget* assignNodesVBox;
      
      /// assign voxels thickness label
      QLabel* assignVoxelsThicknessLabel;
      
      /// assign voxels slice thickness combo box
      QComboBox* assignVoxelsSliceThicknessComboBox;
};

#endif  // __VE_GUI_DRAW_BORDER_DIALOG_H__


