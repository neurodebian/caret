
#ifndef __DISPLAY_SETTINGS_NODE_ATTRIBUTE_FILE_H__
#define __DISPLAY_SETTINGS_NODE_ATTRIBUTE_FILE_H__

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

#include "BrainModelSurfaceOverlay.h"
#include "DisplaySettings.h"
#include "SceneFile.h"

class GiftiNodeDataFile;
class NodeAttributeFile;

/// class for node attribute file display settings
class DisplaySettingsNodeAttributeFile : public DisplaySettings {
   public:
      // constructor
      DisplaySettingsNodeAttributeFile(BrainSet* bs,
                     GiftiNodeDataFile* gafIn,
                     NodeAttributeFile* nafIn,
                     const BrainModelSurfaceOverlay::OVERLAY_SELECTIONS overlayTypeIn,
                     const bool allowSurfaceUniqueColumnSelectionFlagIn,
                     const bool thresholdColumnValidFlagIn);
      
      // destructor
      ~DisplaySettingsNodeAttributeFile();
      
      /// reinitialize all display settings (be sure to call this from child)
      virtual void reset();
      
      /// update any selections due to changes with loaded data files (be sure to call from child)
      virtual void update();
   

      /// for node attribute files - all column selections for each surface are the same
      virtual bool columnSelectionsAreTheSame(const int bm1, const int bm2) const; 

      /// get column selected for display
      int getSelectedDisplayColumn(const int modelNumber,
                                   const int overlayNumber) const;

      /// set column for display
      void setSelectedDisplayColumn(const int modelNumber,
                                    const int overlayNumber,
                                    const int columnNumber);

      /// get column selected for thresholding
      int getSelectedThresholdColumn(const int modelNumber,
                                     const int overlayNumber) const;

      /// set column for thresholding
      void setSelectedThresholdColumn(const int modelNumber,
                                      const int overlayNumber,
                                      const int columnNumber);

      /// get apply to left and right structures flag
      bool getApplySelectionToLeftAndRightStructuresFlag() const
             { return applySelectionToLeftAndRightStructuresFlag; }

      /// set apply to left and right structures flag
      void setApplySelectionToLeftAndRightStructuresFlag(const bool b)
             { applySelectionToLeftAndRightStructuresFlag = b; }

      /// for node attribute files - all column selections for each surface are the same
      //virtual bool displayColumnSelectionsAreTheSame(const int bm1, const int bm2) const;
      
      /// for node attribute files - all column selections for each surface are the same
      //virtual bool thresholdColumnSelectionsAreTheSame(const int bm1, const int bm2) const;
      
      // get flags to find out if any columns are selected as one of the overlays
      void getSelectedColumnFlags(const int brainModelIndex,
                                  std::vector<bool>& selectedColumnFlagsOut) const;
                                  
      // Get first selected column that is an overlay for the brain model (-1 if none)
      int getFirstSelectedColumnForBrainModel(const int brainModelIndex) const;
      
   protected:
      /// get the number of columns for the file
      int getFileNumberOfColumns() const;
      
      /// get the names for file columns
      void getFileColumnNames(std::vector<QString>& columnNames) const;
      
      /// save scene for selected columns
      void saveSceneSelectedColumns(SceneFile::SceneClass& sc);
      
      /// show scene for selected columns
      void showSceneSelectedColumns(const SceneFile::SceneClass& sc,
                                    const QString& fileTypeName,
                                    const QString& legacyDisplayElementName,
                                    const QString& legacyThresholdElmentName,
                                    QString& errorMessage);
      
      /// Set the default value for selected column of any new surfaces      
      void updateSelectedColumnIndices(std::vector<int>& selCol);
 
      /// get the index for column selection
      int getColumnSelectionIndex(const int modelIndex,
                                  const int overlayNumber) const;
                                  
   private:
      /// the selected display column
      std::vector<int> displayColumn;
      
      /// the threshold column
      std::vector<int> thresholdColumn;
      
      /// gifti node attribute file for which this is setting the settings
      GiftiNodeDataFile* gaf;
      
      /// node attribute file for which this is setting the settings
      NodeAttributeFile* naf;
      
      /// type of overlay for this data type
      BrainModelSurfaceOverlay::OVERLAY_SELECTIONS overlayType;
      
      /// threshold column is valid 
      bool thresholdColumnValidFlag;

      /// allow each surface to have its own column selection
      bool allowSurfaceUniqueColumnSelectionFlag;
      
      /// apply coloring with corresponding structures
      bool applySelectionToLeftAndRightStructuresFlag;
};

#endif // __DISPLAY_SETTINGS_NODE_ATTRIBUTE_FILE_H__
