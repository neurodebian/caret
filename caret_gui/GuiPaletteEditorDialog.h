
#ifndef __GUI_PALETTE_EDITOR_DIALOG_H__
#define __GUI_PALETTE_EDITOR_DIALOG_H__

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

#include <vector>

#include "WuQDialog.h"

class Palette;

class QComboBox;
class QGridLayout;
class QPushButton;
class QSignalMapper;

class QDoubleSpinBox;

/// class for dialog used to edit palette files
class GuiPaletteEditorDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiPaletteEditorDialog(QWidget* parent);
      
      /// destructor
      ~GuiPaletteEditorDialog();
      
      /// update the dialog
      void updateDialog();
   
   protected slots:
      /// called when apply button is pressed
      void slotApplyButton();
      
      /// called when a palette is selected
      void slotPaletteSelectionComboBox(int item);
      
      /// called when new palette button pressed
      void slotCreateNewPaletteButton();
      
      /// called when delete palette button pressed
      void slotDeletePaletteButton();
      
      /// called when rename palette button pressed
      void slotRenamePaletteButton();
      
      /// called when an add palette entry button is pressed
      void slotAddPaletteEntry(int item);
      
      /// called when a remove palette entry button is pressed
      void slotRemovePaletteEntry(int item);
      
      /// called when palette entry color button is pressed
      void slotColorPaletteEntry(int item);
      
      /// called when edit colors button is pressed
      void slotEditColorsPushButton();
      
   protected:
      /// update coloring
      void updateColoring();
      
      /// signal mapper for add palette entry buttons
      QSignalMapper* addPaletteEntrySignalMapper;
      
      /// signal mapper for remove palette entry buttons
      QSignalMapper* removePaletteEntrySignalMapper;
      
      /// signal mapper for color buttons
      QSignalMapper* colorPaletteEntrySignalMapper;
      
      /// the palette being editied
      Palette* paletteBeingEdited;
      
      /// palette selection combo box
      QComboBox* paletteSelectionComboBox;
      
      /// widget for palette editing
      QWidget* paletteEntryWidget;
      
      /// grid layout for palette editing controls
      QGridLayout* paletteEntryGridLayout;
      
      /// palette numeric entry spin boxes
      std::vector<QDoubleSpinBox*> paletteEntryScalarSpinBoxes;
      
      /// palette entry color push buttons
      std::vector<QPushButton*> paletteEntryColorPushButtons;
      
      /// widgets containing color swatches
      std::vector<QWidget*> paletteEntryColorSwatch;
      
      /// palette insert new entry push buttons
      std::vector<QPushButton*> paletteEntryInsertPushButtons;

      /// palette delete new entry push buttons
      std::vector<QPushButton*> paletteEntryDeletePushButtons;
      
      /// prevent overwriting palette while it is being loaded into dialog
      bool loadingPalette;
};

#endif // __GUI_PALETTE_EDITOR_DIALOG_H__

