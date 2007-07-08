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


#ifndef __GUI_NODE_ATTRIBUTE_COLUMN_SELECTION_COMBO_BOX_H__
#define __GUI_NODE_ATTRIBUTE_COLUMN_SELECTION_COMBO_BOX_H__

#include <QString>
#include <vector>

#include <QComboBox>

#include "GuiNodeFileType.h"

class GiftiNodeDataFile;
class NodeAttributeFile;

/// This class is used to select a column in a node attribute file
class GuiNodeAttributeColumnSelectionComboBox : public QComboBox {
   Q_OBJECT
   
   public:
      enum {
         CURRENT_ITEM_INVALID = -1, // -1 is current item when combo box is empty
         CURRENT_ITEM_NEW  = -2,
         CURRENT_ITEM_NONE = -3,
         CURRENT_ITEM_ALL  = -4
      };
      
      /// Constructor USE THIS FOR ANY FILE IN THE BRAIN SET
      GuiNodeAttributeColumnSelectionComboBox(const GUI_NODE_FILE_TYPE nodeFileTypeIn,
                                              const bool addNewSelection,
                                              const bool addNoneSelection,
                                              const bool addAllSelection,
                                              QWidget* parent = 0);

      /// Constructor USE ONLY WITH FILES THAT ARE NOT IN THE BRAIN SET
      GuiNodeAttributeColumnSelectionComboBox(NodeAttributeFile* nonBrainSetNodeAttributeFileIn,
                                              const bool addNewSelection,
                                              const bool addNoneSelection,
                                              const bool addAllSelection,
                                              QWidget* parent = 0);

      /// Constructor USE ONLY WITH FILES THAT ARE NOT IN THE BRAIN SET
      GuiNodeAttributeColumnSelectionComboBox(GiftiNodeDataFile* nonBrainSetGiftiNodeDataFileIn,
                                              const bool addNewSelection,
                                              const bool addNoneSelection,
                                              const bool addAllSelection,
                                              QWidget* parent = 0);
      /// Destructor
      ~GuiNodeAttributeColumnSelectionComboBox();
      
      /// get the current label
      QString getCurrentLabel() const;
      
      /// get the current item
      int currentIndex() const;
      
      /// set the current item
      void setCurrentIndex(int item);
      
      /// get the new column label
      QString getNewColumnLabel() const { return newSelectionLabel; }
      
      /// set the none selection entry string
      void setNoneSelectionLabel(const QString& s);
      
      /// update the control
      void updateComboBox();
      
      /// update the control
      void updateComboBox(NodeAttributeFile* naf);
      
      /// update the control
      void updateComboBox(GiftiNodeDataFile* ndf);
      
      /// enable/disable the ALL selection
      void enableDisableAllSelection(const bool enableIt);
      
   signals:
      /// Emitted when item is selected (catches activated(int) signal and
      /// passes item selected, or CURRENT_ITEM_NEW or CURRENT_ITEM_NONE) 
      void itemSelected(int);
      
      /// Emitted when item is selected emitting the name of the column
      void itemNameSelected(const QString&);
      
   private slots:
      /// called when an item is selected
      void slotActivated();
      
   private:
      /// initialize the dialog
      void initializeDialog(const GUI_NODE_FILE_TYPE nodeFileTypeIn,
                            NodeAttributeFile* nonBrainSetNodeAttributeFileIn,
                            GiftiNodeDataFile* nonBrainSetGiftiNodeDataFileIn,
                            const bool addNewSelection,
                            const bool addNoneSelection,
                            const bool addAllSelection);
      
      /// node attribute file for non-brain set files
      NodeAttributeFile* nonBrainSetNodeAttributeFile;
      
      /// gifti node file for non-brain set files
      GiftiNodeDataFile* nonBrainSetGiftiNodeDataFile;
      
      /// type of node data file
      GUI_NODE_FILE_TYPE nodeFileType;
      
      /// New Column label
      QString newSelectionLabel;
      
      /// all columns label
      QString allSelectionLabel;
      
      /// None selection label
      QString noneSelectionLabel;
      
      /// allow a new column selection
      bool allowNewColumnSelection;
      
      /// allow a none selection
      bool allowNoneColumnSelection;
      
      /// allow an all selection
      bool allowAllColumnSelection;
      
      /// values corresponding to combo box entries, column numbers, neg for special (none, new, all)
      std::vector<int> itemValues;
};

#endif // __GUI_NODE_ATTRIBUTE_COLUMN_SELECTION_COMBO_BOX_H__



