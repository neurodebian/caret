
#ifndef __GUI_DISPLAY_CONTROL_SURFACE_OVERLAY_WIDGET_H__
#define __GUI_DISPLAY_CONTROL_SURFACE_OVERLAY_WIDGET_H__

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

#include <QGroupBox>

class BrainModelSurfaceOverlay;
class GuiDisplayControlDialog;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class WuQWidgetGroup;

/// widget for controlling a surface overlay
class GuiDisplayControlSurfaceOverlayWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      GuiDisplayControlSurfaceOverlayWidget(const int overlayNumberIn,
                                 GuiDisplayControlDialog* displayControlDialogIn);
      
      // destructor
      ~GuiDisplayControlSurfaceOverlayWidget();
      
      // update the widget
      void updateWidget();
      
   protected slots:
      // called when data type selected
      void slotDataTypeComboBox(int item);
      
      // called when display column selected
      void slotDisplayColumnComboBox(int item);
      
      // called when threshold column selected
      void slotThresholdColumnComboBox(int item);
      
      // called when lighting checkbox toggled
      void slotLightingCheckBox(bool toggled);
      
      // called when opacity spin box value changed
      void slotOpacityDoubleSpinBox(double value);
      
      // called when display column metadata button clicked
      void slotDisplayColumnMetaDataToolButton();
      
      // called when threshold column metadata button clicked
      void slotThresholdColumnMetaDataToolButton();
      
   protected:
      /// get the surface model index from display control dialog
      int getSurfaceModelIndexFromDisplayControlDialog() const;
      
      /// update Display Control and redraw the surfaces
      void updateDisplayControlAndRedrawSurfaces();
      
      /// edit column metadata 
      void editColumnMetaData(const int columnNumber);
                              
      /// the display control dialog this widget is part of
      GuiDisplayControlDialog* displayControlDialog;
      
      /// the overlay number
      int overlayNumber;
      
      /// lighting check box
      QCheckBox* lightingCheckBox;
      
      /// opacity double spin box
      QDoubleSpinBox* opacityDoubleSpinBox;
      
      /// data type combo box
      QComboBox* dataTypeComboBox;
      
      /// display column combo box
      QComboBox* displayColumnComboBox;
      
      /// threshold combo box
      QComboBox* thresholdColumnComboBox;
      
      /// data type widget group
      WuQWidgetGroup* dataTypeWidgetGroup;
      
      /// display column widget group
      WuQWidgetGroup* displayColumnWidgetGroup;
      
      /// threshold column widget group
      WuQWidgetGroup* thresholdColumnWidgetGroup;
      
};

#endif // __GUI_DISPLAY_CONTROL_SURFACE_OVERLAY_WIDGET_H__
