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

#ifndef __DISPLAY_SETTINGS_VOLUME_H__
#define __DISPLAY_SETTINGS_VOLUME_H__

#include "DisplaySettings.h"

class BrainModelSurface;
class TransformationMatrix;
class VolumeFile;

/// Display settings for volumes
class DisplaySettingsVolume : public DisplaySettings {
   public:
      /// anatomy coloring type
      enum ANATOMY_COLORING_TYPE {
         ANATOMY_COLORING_TYPE_0_255,
         ANATOMY_COLORING_TYPE_MIN_MAX,
         ANATOMY_COLORING_TYPE_2_98,
      };
      
      /// segmentation draw type
      enum SEGMENTATION_DRAW_TYPE {
         SEGMENTATION_DRAW_TYPE_BLEND,
         SEGMENTATION_DRAW_TYPE_SOLID,
         SEGMENTATION_DRAW_TYPE_BOX,
         SEGMENTATION_DRAW_TYPE_CROSS
      };
      
      /// coloring for surface outline
      enum SURFACE_OUTLINE_COLOR {
         SURFACE_OUTLINE_COLOR_BLACK,
         SURFACE_OUTLINE_COLOR_BLUE,
         SURFACE_OUTLINE_COLOR_GREEN,
         SURFACE_OUTLINE_COLOR_RED,
         SURFACE_OUTLINE_COLOR_WHITE
      };
      
      /// misc values
      enum {
         /// maximum number of overlay surfaces
         MAXIMUM_OVERLAY_SURFACES = 3
      };
      
      /// Constructor
      DisplaySettingsVolume(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsVolume();

      /// Reinitialize all display settings
      virtual void reset();
      
      /// Update any selections due to changes in loaded volumes
      virtual void update();

      /// get the segmentation draw type
      SEGMENTATION_DRAW_TYPE getSegmentationDrawType() const { return segmentationDrawType; }
      
      /// set the segmentation draw type
      void setSegmentationDrawType(const SEGMENTATION_DRAW_TYPE sdt) { segmentationDrawType = sdt; }
      
      /// get the selected functional volume file for viewing
      int getSelectedFunctionalVolumeView() const { return selectedFunctionalVolumeView; }
      
      /// set the selected functional volume file for viewing
      void setSelectedFunctionalVolumeView(const int index) {
         selectedFunctionalVolumeView = index;
      }
      
      /// set the selected functional volume file for viewing
      void setSelectedFunctionalVolumeView(const QString& name);
      
      /// get the selected functional volume file for thresholding
      int getSelectedFunctionalVolumeThreshold() const { return selectedFunctionalVolumeThreshold; }
      
      /// set the selected functional volume file for thresholding
      void setSelectedFunctionalVolumeThreshold(const int index) {
         selectedFunctionalVolumeThreshold = index;
      }
      
      /// set the selected functional volume file for thresholding
      void setSelectedFunctionalVolumeThreshold(const QString& name);
      
      /// get the selected paint volume file
      int getSelectedPaintVolume() const { return selectedPaintVolume; }
      
      /// set the selected paint volume file
      void setSelectedPaintVolume(const int index) {
         selectedPaintVolume = index;
      }
      
      /// set the selected paint volume file
      void setSelectedPaintVolume(const QString& name);
      
      /// get the selected rgb volume file
      int getSelectedRgbVolume() const { return selectedRgbVolume; }
      
      /// set the selected rgb volume file
      void setSelectedRgbVolume(const int index) {
         selectedRgbVolume = index;
      }
      
      /// set the selected rgb volume file
      void setSelectedRgbVolume(const QString& name);
      
      /// get the selected segmentation volume file
      int getSelectedSegmentationVolume() const { return selectedSegmentationVolume; }
      
      /// set the selected segmentation volume file
      void setSelectedSegmentationVolume(const int index) {
         selectedSegmentationVolume = index;
      }
      
      /// set the selected segmentation volume file
      void setSelectedSegmentationVolume(const QString& name);
      
      /// get the selected anatomy volume file
      int getSelectedAnatomyVolume() const { return selectedAnatomyVolume; }
      
      /// set the selected anatomy volume file
      void setSelectedAnatomyVolume(const int index) {
         selectedAnatomyVolume = index;
      }
      
      /// set the selected anatomy volume file
      void setSelectedAnatomyVolume(const QString& name);
      
      /// get the anatomy volume brightness
      int getAnatomyVolumeBrightness() const { return anatomyVolumeBrightness; }
      
      /// get the anatomy volume contrast
      int getAnatomyVolumeContrast() const { return anatomyVolumeContrast; }
      
      /// set the anatomy volume brightness
      void setAnatomyVolumeBrightness(const int brightness) { 
         anatomyVolumeBrightness = brightness; 
      }
      
      /// set the anatomy volume contrast
      void setAnatomyVolumeContrast(const int contrast)  { 
         anatomyVolumeContrast = contrast; 
      }
      
      /// get the selected vector volume file
      int getSelectedVectorVolume() const { return selectedVectorVolume; }
      
      /// set the selected vector volume file
      void setSelectedVectorVolume(const int index) {
         selectedVectorVolume = index;
      }
      
      /// set the selected vector volume file
      void setSelectedVectorVolume(const QString& name);
      
      /// get vector volume sparsity
      int getVectorVolumeSparsity() const { return vectorVolumeSparsity; }
      
      /// set vector volume sparsity
      void setVectorVolumeSparsity(const int vvs) { vectorVolumeSparsity = vvs; }
      
      /// get display the crosshair coordinates
      bool getDisplayCrosshairCoordinates() const { return displayCrosshairCoordinates; }
      
      /// set display the crosshair coordinates
      void setDisplayCrosshairCoordinates(const bool d) { displayCrosshairCoordinates = d; }

      /// get display the crosshairs
      bool getDisplayCrosshairs() const { return displayCrosshairs; }
      
      /// set display the crosshairss
      void setDisplayCrosshairs(const bool d) { displayCrosshairs = d; }

      /// get display the orientation labels
      bool getDisplayOrientationLabels() const { return displayOrientationLabels; }
      
      /// set display the orientation labels
      void setDisplayOrientationLabels(const bool d) { displayOrientationLabels = d; }
      
      /// get the anatomical volume coloring type
      ANATOMY_COLORING_TYPE getAnatomyVolumeColoringType() const { return anatomyColoringType; }
      
      /// set the anatomy volume coloring type
      void setAnatomyVolumeColoringType(const ANATOMY_COLORING_TYPE act)
         { anatomyColoringType = act; };
         
      /// set the cropping slices valid
      void setCroppingSlicesValid(const bool b) { croppingSlicesValid = b; }
      
      /// get the cropping slices valid
      bool getCroppingSlicesValid() const { return croppingSlicesValid; }
   
      /// set the cropping slices
      void setCroppingSlices(const int slices[6]);
      
      /// get the cropping slices
      void getCroppingSlices(int slices[6]) const;
      
      /// get the anatomy thresholding valid
      bool getAnatomyThresholdValid() const { return anatomyThresholdValid; }
      
      /// set the anatomy thresholding valid
      void setAnatomyThresholdValid(const bool b) { anatomyThresholdValid = b; }
      
      /// get the anatomy threshold value
      void getAnatomyThreshold(float& minThresh, float& maxThresh) const;
      
      /// set the anatomy threshold value
      void setAnatomyThreshold(const float minThresh,
                               const float maxThresh = 1.0e10);
      
      /// get the segmentation translucency
      float getSegmentationTranslucency() const { return segmentationTranslucency; }
      
      /// set the segmentation translucency
      void setSegmentationTranslucency(const float st) { segmentationTranslucency = st; }
      
      /// get montage view selected
      bool getMontageViewSelected() const { return montageViewSelected; }
      
      /// set montage view selected
      void setMontageViewSelected(const int b) { montageViewSelected = b; }
      
      /// get volume montage settings (returns true if montage on)
      void getMontageViewSettings(int& numRows, int& numSlices, int& sliceIncrement) const;
      
      /// set volume montage settings 
      void setMontageViewSettings(const int numRows, const int numCols, 
                                  const int sliceIncrement);
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected);
                     
      /// get the overlay opacity
      float getOverlayOpacity() const { return overlayOpacity; }
      
      /// set the overlay opacity
      void setOverlayOpacity(const float oo) { overlayOpacity = oo; }
      
      /// get show palette color bar
      bool getDisplayColorBar() const { return displayColorBar; }
      
      /// set show palette color bar
      void setDisplayColorBar(const bool showIt) { displayColorBar = showIt; }
      
      /// get the oblique slices transformation matrix
      TransformationMatrix* getObliqueSlicesTransformationMatrix()
          { return obliqueTransform; }
          
      /// set the oblique slices transformation matrix
      void setObliqueSlicesTransformationMatrix(TransformationMatrix* tm)
          { obliqueTransform = tm; }
          
      /// get the oblique slices sampling size
      float getObliqueSlicesSamplingSize() const { return obliqueSlicesSamplingSize; }
      
      /// set the oblique slices samping size
      void setObliqueSlicesSamplingSize(const float ss)
              { obliqueSlicesSamplingSize = ss; }
         
      ///  Get one of the "overlay" surfaces.
      BrainModelSurface* getOverlaySurface(const int overlaySurfaceNumber);

      /// Set one of the "overlay" surfaces.
      void setOverlaySurface(const int overlaySurfaceNumber,
                             BrainModelSurface* bms);
                            
      /// get display overlay surface over the volume
      bool getDisplayOverlaySurfaceOutline(const int overlaySurfaceNumber) const
                               { return displayOverlaySurfaceOutline[overlaySurfaceNumber]; }
      
      /// set display overlay surface over the volume
      void setDisplayOverlaySurfaceOutline(const int overlaySurfaceNumber,
                                           const bool dso)
                                 { displayOverlaySurfaceOutline[overlaySurfaceNumber] = dso; }
      
      /// get the overlay surface outline color
      SURFACE_OUTLINE_COLOR getOverlaySurfaceOutlineColor(const int overlaySurfaceNumber) const
                              { return overlaySurfaceOutlineColor[overlaySurfaceNumber]; } 

      /// set the overlay surface outline color
      void setOverlaySurfaceOutlineColor(const int overlaySurfaceNumber,
                             const SURFACE_OUTLINE_COLOR soc)
                              { overlaySurfaceOutlineColor[overlaySurfaceNumber] = soc; } 

      /// get the overlay surface outline thickness
      float getOverlaySurfaceOutlineThickness(const int overlaySurfaceNumber) const 
                                 { return overlaySurfaceOutlineThickness[overlaySurfaceNumber]; }
      
      /// set the overlay surface outline thickness
      void setOverlaySurfaceOutlineThickness(const int overlaySurfaceNumber,
                                             const float sot) 
                                    { overlaySurfaceOutlineThickness[overlaySurfaceNumber] = sot; }
      
   private:
      /// file selection helper
      int fileSelectionHelper(const std::vector<VolumeFile*>& files,
                              const QString& fileName);
                              
      /// help save volume file for scene
      void sceneSelectedVolumeHelper(SceneFile::SceneClass& sceneClass,
                                     const QString& infoName,
                                     const VolumeFile* vf);
      
      /// anatomy threshold valid
      bool anatomyThresholdValid;
      
      /// anatomy threshold
      float anatomyThreshold[2];
      
      /// cropping slices are valid
      bool croppingSlicesValid;
      
      /// the cropping slices
      int croppingSlices[6];
      
      /// update the selection index for a file type
      void updateFileType(const int numFiles, int& selectedIndex);
      
      /// selected functional volume file for viewing
      int selectedFunctionalVolumeView;
      
      /// selected functional volume file for thresholding
      int selectedFunctionalVolumeThreshold;
      
      /// selected paint volume file
      int selectedPaintVolume;
      
      /// selected rgb volume file
      int selectedRgbVolume;
      
      /// selected segmentation volume file
      int selectedSegmentationVolume;
      
      /// selected vector volume file
      int selectedVectorVolume;
      
      /// selected anatomy volume file
      int selectedAnatomyVolume;
      
      /// anatomy volume brightness
      int anatomyVolumeBrightness;
      
      /// anatomy volume contrast
      int anatomyVolumeContrast;
      
      /// display the crosshair coordinates
      bool displayCrosshairCoordinates;
      
      /// display the crosshairs
      bool displayCrosshairs;
      
      /// display the orientation labels
      bool displayOrientationLabels;
      
      /// anatomy coloring type
      ANATOMY_COLORING_TYPE anatomyColoringType;
      
      /// segmentation draw type
      SEGMENTATION_DRAW_TYPE segmentationDrawType;
      
      /// segmentation translucency
      float segmentationTranslucency;

      /// montage view selected
      bool montageViewSelected;
      
      /// montage view number of rows
      int montageViewNumberOfRows;
      
      /// montage view number of columns
      int montageViewNumberOfColumns;
      
      /// montage slice increment
      int montageSliceIncrement;
      
      /// overlay opacity
      float overlayOpacity;
      
      /// show color palette color bar
      bool displayColorBar;
      
      /// vector volume sparsity
      int vectorVolumeSparsity;
      
      /// oblique slices transform
      TransformationMatrix* obliqueTransform;
      
      /// oblique slices sampling size
      float obliqueSlicesSamplingSize;
      
      /// surface outlines overlayed on volume slices
      BrainModelSurface* overlaySurface[MAXIMUM_OVERLAY_SURFACES];
      
      /// display overlay surface outline over the volume
      bool displayOverlaySurfaceOutline[MAXIMUM_OVERLAY_SURFACES];
      
      /// surface overlay outline color
      SURFACE_OUTLINE_COLOR overlaySurfaceOutlineColor[MAXIMUM_OVERLAY_SURFACES];
      
      /// surface overlay outline thickness
      float overlaySurfaceOutlineThickness[MAXIMUM_OVERLAY_SURFACES];
      
};

#endif // __DISPLAY_SETTINGS_VOLUME_H__
