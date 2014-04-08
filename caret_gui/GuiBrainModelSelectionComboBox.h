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


#ifndef __GUI_BRAIN_MODEL_SELECTION_COMBO_BOX_H__
#define __GUI_BRAIN_MODEL_SELECTION_COMBO_BOX_H__

#include <vector>

#include <QComboBox>

#include "BrainModelSurface.h"
#include "Structure.h"

class BrainModel;
class BrainModelContours;
class BrainModelVolume;

/// Combo box for selecting brain models
class GuiBrainModelSelectionComboBox : public QComboBox {
   public:
      /// index for "addNewName" if it is not blank
      enum { ADD_NEW_INDEX = 1000000 };
      
      /// options for creating selection combo box
      enum OPTIONS {
         /// show contours
         OPTION_SHOW_CONTOURS = 1,
         /// show all surfaces
         OPTION_SHOW_SURFACES_ALL = 2,
         /// show flat surfaces only
         OPTION_SHOW_SURFACES_FLAT = 4,
         /// show fiducial surfaces only
         OPTION_SHOW_SURFACES_FIDUCIAL = 8,
         /// show hull surfaces only
         OPTION_SHOW_SURFACES_HULL = 16,
         /// show volumes
         OPTION_SHOW_VOLUMES = 32,
         /// show "Add New" 
         OPTION_SHOW_ADD_NEW = 64
      };
      
      /// Constructor
      GuiBrainModelSelectionComboBox(const int options,
                                     const QString& addNewNameIn = "",
                                     QWidget* parent = 0);
                                     
      /// Constructor
      GuiBrainModelSelectionComboBox(const bool showContoursIn, 
                                     const bool showSurfacesIn,
                                     const bool showVolumesIn,
                                     const QString& addNewNameIn,
                                     QWidget* parent = 0,
                                     const char* name = 0,
                                     const bool flatSurfacesOnlyIn = false,
                                     const bool fiducialSurfacesOnlyIn = false,
                                     const bool hullSurfacesOnlyIn = false);
                                     
      /// Destructor
      ~GuiBrainModelSelectionComboBox();
      
      /// limit surface to this structure
      void setSurfaceStructureRequirement(const Structure::STRUCTURE_TYPE limitToStructureIn);
      
      /// get the selected brain model
      BrainModel* getSelectedBrainModel() const;
      
      /// get the selected brain model contours
      BrainModelContours* getSelectedBrainModelContours() const;
      
      /// get the selected brain model surface
      BrainModelSurface* getSelectedBrainModelSurface() const;
      
      /// get the selected brain model volume
      BrainModelVolume* getSelectedBrainModelVolume() const;
      
      /// See if "add new" is the selected item
      bool getAddNewSelected() const;
      
      /// get the selected model index 
      int getSelectedBrainModelIndex() const;
      
      /// set the selected brain model index
      void setSelectedBrainModelIndex(const int bmi);
      
      /// set the selected brain model
      void setSelectedBrainModel(const BrainModel* bm);
      
      /// set the selected brain model to the last surface of the specified type
      void setSelectedBrainModelToLastSurfaceOfType(const BrainModelSurface::SURFACE_TYPES st);
      
      /// set the selected brain model to the first surface of type and structure
      /// if structure type is invalid it is ignored
      void setSelectedBrainModelToFirstSurfaceOfType(const BrainModelSurface::SURFACE_TYPES surfaceType,
                                                     const Structure::STRUCTURE_TYPE structureType
                                                                = Structure::STRUCTURE_TYPE_INVALID);
      
      /// update the items in the combo box
      void updateComboBox();
      
   private:
      /// converts combo box indices to brain model indices
      std::vector<int> brainModelIndices;
      
      /// points to brain models in combo box
      std::vector<BrainModel*> brainModelPointers;
      
      /// index of the add new name
      int addNewIndex;
      
      /// show only this structure
      Structure::STRUCTURE_TYPE limitToStructure;
      
      /// contains contours
      bool showContours;
      
      /// contains surfaces
      bool showSurfaces;
      
      /// contains volumes
      bool showVolumes;
      
      /// name for add new
      QString addNewName;
      
      /// limit surfaces to flat surfaces
      bool flatSurfacesOnly;
      
      /// limit surface to fiducial surfaces
      bool fiducialSurfacesOnly;
      
      /// limit surface to hull surfaces
      bool hullSurfacesOnly;
};

#endif // __GUI_BRAIN_MODEL_SELECTION_COMBO_BOX_H__

