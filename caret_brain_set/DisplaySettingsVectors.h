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



#ifndef __DISPLAY_SETTINGS_VECTORS_H__
#define __DISPLAY_SETTINGS_VECTORS_H__

#include <vector>

#include "DisplaySettings.h"

/// DisplaySettingsVectors is a class that maintains parameters for
/// controlling the display of vector data.
class DisplaySettingsVectors : public DisplaySettings {
   public:
      /// display mode types
      enum DISPLAY_MODE {
         /// show all vectors
         DISPLAY_MODE_ALL,
         /// show no vectors
         DISPLAY_MODE_NONE,
         /// show sparse (some) vectors
         DISPLAY_MODE_SPARSE 
      };

      /// type of vectors
      enum VECTOR_TYPE {
         /// bidirectional (two opposite) vectors
         VECTOR_TYPE_BIDIRECTIONAL,
         /// unidirectional (one direction) vectors
         VECTOR_TYPE_UNIDIRECTIONAL_ARROW,
         /// unidirectional (one direction) vectors
         VECTOR_TYPE_UNIDIRECTIONAL_CYLINDER
      };

      /// SURFACE_SYMBOL {
      enum SURFACE_SYMBOL {
         /// 3D Symbol
         SURFACE_SYMBOL_3D,
         /// 2D Line
         SURFACE_SYMBOL_2D_LINE
      };

      /// coloring mode
      enum COLOR_MODE {
         /// color using colors form file
         COLOR_MODE_VECTOR_COLORS,
         /// color using XYZ as RGB
         COLOR_MODE_XYZ_AS_RGB
      };

      /// vector display orientation
      enum DISPLAY_ORIENTATION {
         /// vectors oriented along any axis
         DISPLAY_ORIENTATION_ANY,
         /// vectors oriented along left/right axis
         DISPLAY_ORIENTATION_LEFT_RIGHT,
         /// vectors oriented along posterior/anterior axis
         DISPLAY_ORIENTATION_POSTERIOR_ANTERIOR,
         /// vectors oriented along inferior/superior axis
         DISPLAY_ORIENTATION_INFERIOR_SUPERIOR
      };

      /// Constructor
      DisplaySettingsVectors(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsVectors();
      
      /// get the vector type
      VECTOR_TYPE getVectorType() const { return vectorType; }

      /// set the vector type
      void setVectorType(const VECTOR_TYPE vt) { vectorType = vt; }

      /// get the surface symbol
      SURFACE_SYMBOL getSurfaceSymbol() const { return surfaceSymbol; }

      /// set the surface symbol
      void setSurfaceSymbol(const SURFACE_SYMBOL ss) { surfaceSymbol = ss; }

      /// get the display mode on surface
      DISPLAY_MODE getDisplayModeSurface() const { return displayModeSurface; }

      /// get the display mode on volume
      DISPLAY_MODE getDisplayModeVolume() const { return displayModeVolume; }

      /// set the display mode on surface
      void setDisplayModeSurface(const DISPLAY_MODE dm);
      
      /// set the display mode on volume
      void setDisplayModeVolume(const DISPLAY_MODE dm);

      // get the color mode
      COLOR_MODE getColorMode() const { return colorMode; }

      /// set the color mode
      void setColorMode(const COLOR_MODE colorModeIn);

      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded areal estimation file
      void update();
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       
      /// get the sparse distance
      int getSparseDisplayDistance() const { return sparseDistance; }

      /// set the sparse distance
      void setSparseDisplayDistance(const int dist);

      /// get the length multiplier
      float getLengthMultiplier() const { return lengthMultiplier; }
      
      // set the length multiplier
      void setLengthMultiplier(const float len);

      /// get draw with magnitude
      bool getDrawWithMagnitude() const { return drawWithMagnitude; }

      // set draw with magnitude
      void setDrawWithMagnitude(const bool dwm);

      /// volume slice distance above limit
      float getVolumeSliceDistanceAboveLimit() const { return volumeSliceDistanceAboveLimit; }

      /// volume slice distance below limit
      float getVolumeSliceDistanceBelowLimit() const { return volumeSliceDistanceBelowLimit; }

      // set the volume slice above limit
      void setVolumeSliceDistanceAboveLimit(const float limit);

      // set the volume slice below limit
      void setVolumeSliceDistanceBelowLimit(const float limit);

      /// get the surface vector line width
      float getSurfaceVectorLineWidth() const { return surfaceVectorLineWidth; }

      // set the surface vector line width
      void setSurfaceVectorLineWidth(const float lineWidth);

      /// get the magnitude threshold
      float getMagnitudeThreshold() const { return magnitudeThreshold; }

      // set the magnitude threshold
      void setMagnitudeThreshold(const float magThresh);

      /// get enable segmentation masking volume
      bool getSegmentationMaskingVolumeEnabled() { return segmentationMaskingVolumeEnabled; }

      /// set enable segmentation masking volume
      void setSegmentationMaskingVolumeEnabled(bool enableIt);

      /// get the segmentation volume used for masking
      VolumeFile* getSegmentationMaskingVolumeFile();

      // set the segmentation volume used for masking
      void setSegmentationMaskingVolumeFile(VolumeFile* vf);

      /// get enable functional masking volume
      bool getFunctionalMaskingVolumeEnabled() const { return functionalMaskingVolumeEnabled; }

      /// set enable functional masking volume
      void setFunctionalMaskingVolumeEnabled(bool enableIt);

      /// get the functional volume used for masking
      VolumeFile* getFunctionalMaskingVolumeFile();

      // set the functional volume used for masking
      void setFunctionalMaskingVolumeFile(VolumeFile* vf);

      /// get the functional masking volume negative threshold
      float getFunctionalMaskingVolumeNegativeThreshold() const { return functionalMaskingVolumeNegativeThreshold; }

      /// set the functional masking volume negative threshold
      void setFunctionalMaskingVolumeNegativeThreshold(const float value);

      /// get the functional masking volume positive theshold
      float getFunctionalMaskingVolumePositiveThreshold() const { return functionalMaskingVolumePositiveThreshold; }

      /// set the functional masking volume positive theshold
      void setFunctionalMaskingVolumePositiveThreshold(const float value);

      /// get display a vector file
      bool getDisplayVectorFile(const int indx) const;

      /// set display a vector file
      void setDisplayVectorFile(const int indx, const bool dispFlag);

      /// get the display orientation
      DISPLAY_ORIENTATION getDisplayOrientation() const { return displayOrientation; }

      /// set the display orientation
      void setDisplayOrientation(const DISPLAY_ORIENTATION orient) { displayOrientation = orient; }

      /// get the display orientation angle
      float getDisplayOrientationAngle() const { return displayOrientationAngle; }

      /// set the display orientation angle
      void setDisplayOrientationAngle(const float angle) { displayOrientationAngle = angle; }

   private:
      /// surface symbol
      SURFACE_SYMBOL surfaceSymbol;

      /// vector type
      VECTOR_TYPE vectorType;

      /// the display mode on surface
      DISPLAY_MODE displayModeSurface;

      /// the display mode on volume
      DISPLAY_MODE displayModeVolume;

      /// the color mode
      COLOR_MODE colorMode;

      /// sparse distance (mm)
      int sparseDistance;

      /// tracks which vector files should be displayed
      mutable std::vector<bool> displayVectorFileFlag;

      /// vector length multiplier
      float lengthMultiplier;

      /// draw the vectors using the magnitude
      bool drawWithMagnitude;

      /// volume slice distance above limit
      float volumeSliceDistanceAboveLimit;

      /// volume slice distance below limit
      float volumeSliceDistanceBelowLimit;

      /// width for surface vectors drawn as lines
      float surfaceVectorLineWidth;

      /// threshold for magnitude
      float magnitudeThreshold;

      /// enable segmentation masking volume
      bool segmentationMaskingVolumeEnabled;

      /// segmentation volume used for masking
      VolumeFile* segmentationMaskingVolumeFile;

      /// enable functional volume masking
      bool functionalMaskingVolumeEnabled;

      /// functional volume used for masking
      VolumeFile* functionalMaskingVolumeFile;

      /// functional volume masking negative threshold
      float functionalMaskingVolumeNegativeThreshold;

      /// functional volume masking positive threshold
      float functionalMaskingVolumePositiveThreshold;

      /// the display orientation
      DISPLAY_ORIENTATION displayOrientation;

      /// the display orientation angle
      float displayOrientationAngle;

};

#endif // __DISPLAY_SETTINGS_VECTORS_H__

