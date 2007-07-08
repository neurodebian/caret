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



#ifndef __VE_BRAIN_MODEL_VOLUME_H__
#define __VE_BRAIN_MODEL_VOLUME_H__

#include "BrainModel.h"
#include "VolumeFile.h"

/// BrainModelVolume stores a single volume
class BrainModelVolume : public BrainModel {
   public:
      
      /// Constructor
      BrainModelVolume(BrainSet* bs);
      
      /// Destructor
      ~BrainModelVolume();
      
      /// Initialize the selected slices for all views.
      void initializeSelectedSlicesAllViews(const bool initializeAxis);
      
      /// initialize the selected slices for a single view
      void initializeSelectedSlices(const int viewNumber, const bool initializeAxis);
      
      /// Get a descriptive name of the model
      QString getDescriptiveName() const;
      
      /// get the selected slices for orthogonal views
      void getSelectedOrthogonalSlices(const int volumeViewNumber, int slices[3]);
      
      /// set the selected slices for orthogonal views
      void setSelectedOrthogonalSlices(const int volumeViewNumber, const int slices[3]);

      /// get the selected slices for oblique view
      void getSelectedObliqueSlices(int slices[3]);
      
      /// set the selected slices for oblique view
      void setSelectedObliqueSlices(const int slices[3]);

      /// get the selected oblique slice offsets
      void getSelectedObliqueSliceOffsets(const int volumeViewNumber, int slices[3]);
      
      /// set the selected oblique slice offsets
      void setSelectedObliqueSliceOffsets(const int volumeViewNumber, const int slices[3]);

      /// get the selected axis
      VolumeFile::VOLUME_AXIS getSelectedAxis(const int volumeViewNumber) const { return selectedAxis[volumeViewNumber]; }
      
      /// set the selected axis
      void setSelectedAxis(const int volumeViewNumber, const VolumeFile::VOLUME_AXIS axis);
               
      /// reset the volume
      void reset();
      
      /// reset the viewing transform
      void resetViewingTransform(const int viewNumber);

      /// get the volume is an underlay or an overlay
      bool getVolumeIsAnUnderlayOrAnOverlay(const VolumeFile* vf) const;
      
      /// get the "bottom-most" selected volume file
      VolumeFile* getMasterVolumeFile();
      
      /// get the "bottom-most" selected volume file (const method)
      const VolumeFile* getMasterVolumeFile() const;
      
      /// get the underlay volume file
      VolumeFile* getUnderlayVolumeFile();
 
      /// get the underlay volume file  (const method)
      const VolumeFile* getUnderlayVolumeFile() const;
 
      /// get the secondary overlay volume file
      VolumeFile* getOverlaySecondaryVolumeFile();
 
      /// get the secondary overlay volume file (const method)
      const VolumeFile* getOverlaySecondaryVolumeFile() const;
 
      /// get the primary overlay volume file
      VolumeFile* getOverlayPrimaryVolumeFile();
 
      /// get the primary overlay volume file (const method)
      const VolumeFile* getOverlayPrimaryVolumeFile() const;
 
      /// get the selected anatomy volume file (const method)
      const VolumeFile* getSelectedVolumeAnatomyFile() const;
      
      /// get the selected anatomy volume file
      VolumeFile* getSelectedVolumeAnatomyFile();
      
      /// get the selected functional view volume file
      VolumeFile* getSelectedVolumeFunctionalViewFile();
      
      /// get the selected functional view volume file  (const method)
      const VolumeFile* getSelectedVolumeFunctionalViewFile() const;
      
      /// get the selected functional threshold volume file
      VolumeFile* getSelectedVolumeFunctionalThresholdFile();
      
      /// get the selected functional threshold volume file (const method)
      const VolumeFile* getSelectedVolumeFunctionalThresholdFile() const;
      
      /// get the selected paint volume file
      VolumeFile* getSelectedVolumePaintFile();
      
      /// get the selected paint volume file  (const method)
      const VolumeFile* getSelectedVolumePaintFile() const;
      
      /// get the selected rgb volume file
      VolumeFile* getSelectedVolumeRgbFile();
      
      /// get the selected rgb volume file  (const method)
      const VolumeFile* getSelectedVolumeRgbFile() const;
      
      /// get the selected segmentation volume file
      VolumeFile* getSelectedVolumeSegmentationFile();
      
      /// get the selected segmentation volume file  (const method)
      const VolumeFile* getSelectedVolumeSegmentationFile() const;
      
      /// get the selected vector volume file
      VolumeFile* getSelectedVolumeVectorFile();
      
      /// get the selected vector volume file  (const method)
      const VolumeFile* getSelectedVolumeVectorFile() const;
      
      /// get the display rotation
      float getDisplayRotation(const int volumeViewNumber) const;
      
      /// set the display rotation
      void setDisplayRotation(const int volumeViewNumber,
                              const float dr);
      
      /// add to the display rotation
      void addToDisplayRotation(const int volumeViewNumber,
                                const float delta);
      
      /// get the number of names in the prob atlas name table
      int getNumberOfProbAtlasNames() const { return probAtlasNameTable.size(); }
      
      /// get a name by its index from the prob atlas name table
      QString getProbAtlasNameFromIndex(const int index) const;
      
      /// get an index for a name in the prob atlas name table 
      int getProbAtlasIndexFromName(const QString& name) const;
      
      /// update prob atlas name table indices (call without arguments to update for all)
      void updateProbAtlasNameTableIndices(const int probAtlasVolumeIndex = -1);
      
      /// get the prob atlas name table index for a prob atlas volume voxel
      int getProbAtlasNameTableIndex(const int probAtlasVolumeIndex,
                                     const int voxelValue) const;
                                     
      /// get the oblique rotation matrix
      vtkTransform* getObliqueRotationMatrix() { 
         return obliqueRotationMatrix; 
      }
      
      /// get the oblique rotation matrix (const method)
      const vtkTransform* getObliqueRotationMatrix() const{ 
         return obliqueRotationMatrix; 
      }
      
      /// get the oblique rotation matrix (const method)
      void getObliqueRotationMatrix(float matrix[16]) const;
      
      /// set the oblique rotation matrix
      void setObliqueRotationMatrix(const float matrix[16]);
      
      /// get oblique transformations as string (16 rot, 1 scale)
      QString getObliqueTransformationsAsString(const int viewNumber) const;
      
      /// set oblique transformations from string (16 rot, 1 scale)
      void setObliqueTransformationsAsString(const int viewNumber, const QString s);
      
      /// set to a standard view
      virtual void setToStandardView(const int viewNumber, const STANDARD_VIEWS view);
      
      /// view stereotaxic coordinates flag
      bool getViewStereotaxicCoordinatesFlag(const int viewNumber) const;
       
      /// view stereotaxic coordinates flag
      void setViewStereotaxicCoordinatesFlag(const int viewNumber, const bool b);
      
      // get show underlay only in window
      bool getShowUnderlayOnlyInWindow(const int viewNumber) const;
      
      // set show underlay only in window
      void setShowUnderlayOnlyInWindow(const int viewNumber,
                                       const bool underlayOnlyFlag);
                                       
   protected:
      /// the selected ortogonal view slices
      int selectedOrthogonalSlices[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS][3];
      
      /// the selected oblique view slices
      int selectedObliqueSlices[3];
      
      /// the oblique selected slice offsets
      int selectedObliqueSliceOffsets[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS][3];
      
      /// the selected axis
      VolumeFile::VOLUME_AXIS selectedAxis[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
      
      /// the display rotation
      float displayRotation[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
      
      /// show underlay only
      float showUnderlayOnly[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
      
      /// add name to prob atlas name table (returns its index);
      int addNameToProbAtlasNameTable(const QString& name);
      
      /// prob atlas name table
      std::vector<QString> probAtlasNameTable;
      
      /// prob atlas name table indices
      std::vector< std::vector<int> > probAtlasNameTableIndices;
      
      /// oblique view rotation matrix
      vtkTransform* obliqueRotationMatrix;
      
      /// view stereotaxic coordinates flag
      bool viewStereotaxicCoordinatesFlag[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
};

#endif
