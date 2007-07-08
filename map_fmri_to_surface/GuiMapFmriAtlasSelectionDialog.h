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



#ifndef __GUI_MAP_FMRI_ATLAS_SELECTION_DIALOG_H__
#define __GUI_MAP_FMRI_ATLAS_SELECTION_DIALOG_H__

#include <qdialog.h>

#include "AtlasSurfaceDirectoryFile.h"

class QCheckBox;
class QComboBox;

/// Dialog for selecting atlas and anatomical surface in a list box
class GuiMapFmriAtlasSelectionDialog : public QDialog {
   Q_OBJECT
   
   private:
      /// class for storing atlases
      class AvailableAtlases {
         public:
            /// name of the atlas
            std::string atlasName;
            
            /// index of right hemisphere 
            int cerebralRightIndex;
            
            ///  index of left hemisphere
            int cerebralLeftIndex;
            
            /// index of cerebellum
            int cerebellumIndex;
 
            /// constructor
            AvailableAtlases(const std::string& atlasNameIn) {
               atlasName.assign(atlasNameIn);
               cerebralRightIndex = -1;
               cerebralLeftIndex  = -1;
               cerebellumIndex    = -1;
            }
      };
      
   public:
      /// Constructor
      GuiMapFmriAtlasSelectionDialog(QWidget* parent, 
                                     AtlasSurfaceDirectoryFile* asdf);
      
      /// Destructor
      ~GuiMapFmriAtlasSelectionDialog();
      
      /// get the selected atlas index
      void getSelectedAtlas(std::vector<int>& atlasesSelected) const;
      
   private slots:
      /// called when an atlas is selected
      void slotAtlasSelection(int item);
      
      /// called when user closes dialog
      void done(int r);
      
   private:
      /// the atlas surface directory file
      AtlasSurfaceDirectoryFile* atlasDirectory;
      
      /// storage for atlases
      std::vector<AvailableAtlases> availableAtlases;
      
      /// atlas selection combo box
      QComboBox* atlasSelectionComboBox;
      
      /// left hemisphere check button
      QCheckBox* leftHemisphereCheckBox;
      
      /// right hemisphere check button
      QCheckBox* rightHemisphereCheckBox;
      
      /// cerebellum check button
      QCheckBox* cerebellumCheckBox;
      

};

#endif // __GUI_MAP_FMRI_ATLAS_SELECTION_DIALOG_H__

