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


#ifndef __VE_GUI_NAME_SELECTION_DIALOG_H__
#define __VE_GUI_NAME_SELECTION_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class ColorFile;
class QListWidget;
class QListWidgetItem;
class QComboBox;

/// This class displays a dialog that allows the user to choose from a variety
/// of data types' names.
class GuiNameSelectionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      enum LIST_ITEMS_TYPE {
         LIST_NONE                      =   0,
         LIST_AREA_COLORS_ALPHA         =   1,
         LIST_BORDER_NAMES_ALPHA        =   2,
         LIST_BORDER_COLORS_ALPHA       =   4,
         LIST_BORDER_COLORS_FILE        =   8,
         LIST_CELL_COLORS_ALPHA         =  16,
         LIST_CELL_COLORS_FILE          =  32,
         LIST_FOCI_NAMES_ALPHA          =  64,
         LIST_PAINT_NAMES_ALPHA         = 128,
         LIST_CONTOUR_CELL_COLORS_ALPHA = 256,
         LIST_CONTOUR_CELL_COLORS_FILE  = 512,
         LIST_VOCABULARY_ALPHA          = 1024,
         LIST_STEREOTAXIC_SPACES        = 2048,
         LIST_STATISTICS                = 4096,
         LIST_VOLUME_PAINT_NAMES_ALPHA  = 8192,
         LIST_ALL                       = 0xffffffff
      };
      
      /// Constructor
      GuiNameSelectionDialog(QWidget* parent, const unsigned int itemsToDisplay = LIST_ALL,
                             const LIST_ITEMS_TYPE defaultItem = LIST_NONE);

      /// Destructor
      ~GuiNameSelectionDialog();

      /// Get the name that was selected
      QString getName() const { return name; }
      
      /// Get the selected item type
      LIST_ITEMS_TYPE getSelectedItemType() const;
      
   private slots:
      /// called when a file type radio button is pressed
      void fileTypeSelectionSlot(int buttNum);
      
      /// called when an item is selected (double clicked) in the list box
      void listBoxSelectionSlot(QListWidgetItem*);
      
      /// called when an item is selected (single clicked) in the list box
      void listBoxHighlightSlot(QListWidgetItem*);
      
   private:
      /// Add a color file to this list box
      void addColorFileToListBox(ColorFile* cf, const bool sortThem);
       
      /// Add the names to the list box and possibly sort them
      void addNamesToListBox(std::vector<QString>& names, const bool sortThem);
      
      /// load the names for the file type and order
      void loadAreaColorsAlphaOrder();
      
      /// load the names for the file type and order
      void loadBorderNamesAlphaOrder();
      
      /// load the names for the file type and order
      void loadBorderColorsAlphaOrder();
      
      /// load the names for the file type and order
      void loadBorderColorsFileOrder();
      
      /// load the name for the file type and order
      void loadFociNamesAlphaOrder();
      
      /// load the names for the file type and order
      void loadCellColorsAlphaOrder();
      
      /// load the names for the file type and order
      void loadCellColorsFileOrder();
      
      /// load the names for the file type and order
      void loadPaintNamesAlphaOrder();
      
      /// load the names for the file type and order
      void loadContourCellColorsAlphaOrder();
      
      /// load the names for the file type and order
      void loadContourCellColorsFileOrder();
      
      /// load the names for the file type and order
      void loadVocabularyAlphaOrder();
      
      /// load volume paint name in alpha order
      void loadVolumePaintNamesAlpha();
      
      /// load the stereotaxic space names
      void loadStereotaxicSpaces();
      
      /// load the statistics
      void loadStatistics();
      
      /// selected name
      QString name;
      
      /// the list box containing names
      QListWidget* nameListBox;
      
      /// default selected item
      static int defaultSelection;
      
      /// item number for area colors alphabetical
      int areaColorsAlphaItemNumber;
      
      /// item number for border names alphabetical
      int borderNamesAlphaItemNumber;
      
      /// item number for border colors alphabetical
      int borderColorsAlphaItemNumber;
      
      /// item number for border colors file order
      int borderColorsFileItemNumber;
      
      /// item number for cell colors alphabetical
      int cellColorsAlphaItemNumber;
      
      /// item number for cell colors file order
      int cellColorsFileItemNumber;
      
      /// item number for foci names alphabetical
      int fociNamesAlphaItemNumber;
      
      /// item number for paint names alphabetical
      int paintNamesAlphaItemNumber;
      
      /// item number for contour cell colors alphabetical
      int contourCellColorsAlphaItemNumber;
      
      /// item number for contour cell colors file order
      int contourCellColorsFileItemNumber;

      /// item number for vocabulary alpha order
      int vocabularyAlphaItemNumber;
      
      /// item number for volume paint names alpha order
      int volumePaintNamesAlphaOrderItemNumber;
      
      /// item number for stereotaxic spaces
      int stereotaxicSpaceItemNumber;
      
      /// item number for statistics
      int statisticsItemNumber;
      
      /// combo box for file type selection
      QComboBox* fileTypeComboBox;      
};

#ifdef __NAME_DIALOG_MAIN__
   int GuiNameSelectionDialog::defaultSelection = 2;
#endif

#endif // __VE_GUI_NAME_SELECTION_DIALOG_H__


