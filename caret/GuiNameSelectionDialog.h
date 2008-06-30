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

#include "WuQDialog.h"

class ColorFile;
class QListWidget;
class QListWidgetItem;
class QComboBox;

/// This class displays a dialog that allows the user to choose from a variety
/// of data types' names.
class GuiNameSelectionDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      enum LIST_ITEMS_TYPE {
         LIST_NONE                      =       0,  
         LIST_AREA_COLORS_ALPHA         =       1,  // 2^0
         LIST_BORDER_NAMES_ALPHA        =       2,  // 2^1
         LIST_BORDER_COLORS_ALPHA       =       4,  // 2^2
         LIST_BORDER_COLORS_FILE        =       8,  // 2^3
         LIST_CELL_COLORS_ALPHA         =      16,  // 2^4
         LIST_CELL_COLORS_FILE          =      32,  // 2^5
         LIST_FOCI_AREAS_ALPHA          =      64,  // 2^6
         LIST_FOCI_CLASSES_ALPHA        =     128,  // 2^7
         LIST_FOCI_COMMENT_ALPHA        =     256,  // 2^8
         LIST_FOCI_GEOGRAPHY_ALPHA      =     512,  // 2^9
         LIST_FOCI_ROI_ALPHA            =    1024,  // 2^10
         LIST_FOCI_NAMES_ALPHA          =    2048,  // 2^11
         LIST_PAINT_NAMES_ALPHA         =    4096,  // 2^12
         LIST_CONTOUR_CELL_COLORS_ALPHA =    8192,  // 2^13
         LIST_CONTOUR_CELL_COLORS_FILE  =   16384,  // 2^14
         LIST_VOCABULARY_ALPHA          =   32768,  // 2^15
         LIST_STEREOTAXIC_SPACES        =   65536,  // 2^16
         LIST_STATISTICS                =  131072,  // 2^17
         LIST_STRUCTURE                 =  262144,  // 2^18
         LIST_STUDY_CITATION            =  524288,  // 2^19
         LIST_STUDY_DATA_FORMAT         = 1048576,  // 2^20
         LIST_STUDY_DATA_TYPE           = 2097152,  // 2^21
         LIST_STUDY_KEYWORDS_ALPHA      = 4194304,  // 2^22
         LIST_STUDY_MESH_ALPHA          = 8388608,  // 2^23
         LIST_STUDY_TABLE_HEADERS       = 16777216,  // 2^24
         LIST_STUDY_TABLE_SUBHEADERS    = 33554432,  // 2^25
         LIST_VOLUME_PAINT_NAMES_ALPHA  = 67108864,  // 2^26
         LIST_ALL                       = 0xffffffff
      };
      
      /// Constructor
      GuiNameSelectionDialog(QWidget* parent, 
                             const unsigned int itemsToDisplay = LIST_ALL,
                             const LIST_ITEMS_TYPE defaultItem = LIST_NONE,
                             const bool allowMultipleSelectionsFlagIn = false);

      /// Destructor
      ~GuiNameSelectionDialog();

      /// Get the names that were selected (use if multiple selections)
      QStringList getNamesSelected() const;
      
      /// Get the name that was selected (use if single selections)
      QString getNameSelected() const;
      
      /// Get the selected item type
      LIST_ITEMS_TYPE getSelectedItemType() const;
      
   private slots:
      /// called when a file type radio button is pressed
      void fileTypeSelectionSlot(int buttNum);
      
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
      
      /// load the name for the file type and order
      void loadFociClassesAlphaOrder();
      
      /// load the name for the file type and order
      void loadFociAreasAlphaOrder();
      
      /// load the names for the file type and order
      void loadFociCommentAlphaOrder();
      
      /// load the name for the file type and order
      void loadFociGeographyAlphaOrder();
      
      /// load the names for the file type and order
      void loadFociROIAlphaOrder();
      
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
      
      /// load the structures
      void loadStructures();
      
      /// load the study citations
      void loadStudyCitation();
      
      /// load the study data format
      void loadStudyDataFormat();
      
      /// load the study data type
      void loadStudyDataType();
      
      /// load the name for the file type and order
      void loadStudyKeywords();
      
      /// load the name for the file type and order
      void loadStudyMedialSubjectHeadings();
      
      /// load study table headers
      void loadStudyTableHeaders();
      
      /// load study table subheaders
      void loadStudyTableSubHeaders();
      
      /// selected name
      QString name;
      
      /// the list box containing names
      QListWidget* nameListBox;
      
      /// allow multiple selections
      bool allowMultipleSelectionsFlag;
      
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
      
      /// item number for foci areas alphabetical
      int fociAreasAlphaItemNumber;
      
      /// item number for foci classes alphabetical
      int fociClassesAlphaItemNumber;
      
      /// item number for foci comment alphabetical
      int fociCommentAlphaItemNumber;
      
      /// item number for foci geography alphabetical
      int fociGeographyAlphaItemNumber;
      
      /// item number for foci roi alphabetical
      int fociRegionOfInterestAlphaItemNumber;
      
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
      
      /// item number for structures
      int structuresItemNumber;
      
      /// item number for study study citation
      int studyCitationItemNumber;
      
      /// item number for study data format
      int studyDataFormatItemNumber;
      
      /// item number for study data type
      int studyDataTypeItemNumber;
      
      /// item number for study keywords
      int studyKeywordsItemNumber;
      
      /// item number for study medical subject headings
      int studyMedicalSubjectHeadingsItemNumber;
      
      /// item number for study table headers
      int studyTableHeadersItemNumber;
      
      /// item number for study table subheaders
      int studyTableSubHeadersItemNumber;
      
      /// item number for statistics
      int statisticsItemNumber;
      
      /// combo box for file type selection
      QComboBox* fileTypeComboBox;      
};

#ifdef __NAME_DIALOG_MAIN__
   int GuiNameSelectionDialog::defaultSelection = 2;
#endif

#endif // __VE_GUI_NAME_SELECTION_DIALOG_H__


