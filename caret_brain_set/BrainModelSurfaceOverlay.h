
#ifndef __BRAIN_MODEL_SURFACE_OVERLAY_H__
#define __BRAIN_MODEL_SURFACE_OVERLAY_H__

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

#include <QString>
#include <QStringList>

#include "SceneFile.h"

class BrainSet;
class DisplaySettingsNodeAttributeFile;

/// class for an overlays settings
class BrainModelSurfaceOverlay {
   public:
      /// Primary Overlay Underlay Selections
      enum OVERLAY_SELECTIONS {
         OVERLAY_NONE,
         OVERLAY_AREAL_ESTIMATION,
         OVERLAY_COCOMAC,
         OVERLAY_METRIC,
         OVERLAY_PAINT,
         OVERLAY_PROBABILISTIC_ATLAS,
         OVERLAY_RGB_PAINT,
         OVERLAY_SECTIONS,
         OVERLAY_SHOW_CROSSOVERS,
         OVERLAY_SHOW_EDGES,
         OVERLAY_SURFACE_SHAPE,
         OVERLAY_TOPOGRAPHY,
         OVERLAY_GEOGRAPHY_BLENDING
      };

      // constructor
      BrainModelSurfaceOverlay(BrainSet* brainSetIn,
                               const int overlayNumberIn);
      
      // destructor
      ~BrainModelSurfaceOverlay();
      
      // reset the overlay
      void reset();
      
      /// get the overlay
      OVERLAY_SELECTIONS getOverlay(const int modelNumber,
                                    const bool doUpdateFlag = true) const;
      
      /// set the overlay
      void setOverlay(const int modelNumber,
                      const OVERLAY_SELECTIONS os);
      
      // get data types and names for selection
      void getDataTypesAndNames(std::vector<OVERLAY_SELECTIONS>& typesOut,
                                std::vector<QString>& namesOut) const;
                                
      // get the display column valid
      bool getDisplayColumnValid(const int modelNumber) const;
      
      // get the display column names
      QStringList getDisplayColumnNames(const int modelNumber) const;

      // get the selected display column
      int getDisplayColumnSelected(const int modelNumber) const;
      
      // set the selected display column
      void setDisplayColumnSelected(const int modelNumber,
                                    const int columnNumber);
      
      // get the threshold column names
      QStringList getThresholdColumnNames(const int modelNumber) const;
      
      // get the threshold column valid
      bool getThresholdColumnValid(const int modelNumber) const;
      
      // get the selected threshold column
      int getThresholdColumnSelected(const int modelNumber) const;
      
      // set the selected threshold column
      void setThresholdColumnSelected(const int modelNumber,
                                    const int columnNumber);
      
      /// get lighting enabled
      bool getLightingEnabled() const { return lightingEnabled; }
      
      /// set lighting enabled
      void setLightingEnabled(const bool le) { lightingEnabled = le; }
      
      /// get the opacity
      float getOpacity() const { return opacity; }
      
      /// set the opacity
      void setOpacity(const float op) { opacity = op; }
      
      /// get the overlay number
      int getOverlayNumber() const { return overlayNumber; }
      
      /// apply a scene (set display settings)
      void showScene(const SceneFile::Scene& scene,
                             QString& errorMessage);
      
      /// create a scene (read display settings)
      void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
               
      /// update the overlay due to brain model being loaded/unloaded or files changing
      void update() const;
      
      /// copy the overlay selections from specified surface to all other surfaces
      void copyOverlaysFromSurface(const int surfaceModelIndex);
      
      /// get name of the overlay
      QString getName() const { return name; }
      
      // get the name of the data type
      QString getDataTypeName(const int modelNumber) const;
      
   protected:
      /// copy the overlay selections from specified surface to all other surfaces
      void copyOverlaysFromSurfaceHelper(DisplaySettingsNodeAttributeFile* dsna,
                                         const int surfaceModelIndex);
      
      /// the overlay selected
      mutable std::vector<OVERLAY_SELECTIONS> overlay;
      
      /// the opacity
      float opacity;
      
      /// lighting
      bool lightingEnabled;
      
      /// brain set using this overlay
      BrainSet* brainSet;
      
      /// the overlay number
      int overlayNumber;
      
      /// the name of the overlay
      QString name;
};

#endif // __BRAIN_MODEL_SURFACE_OVERLAY_H__
