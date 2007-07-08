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



#ifndef __VE_BRAIN_MODEL_H__
#define __VE_BRAIN_MODEL_H__


#include <QString>

class BrainSet;
class vtkTransform;

/// Class for the different brain model types.  This class cannot be instantiated.  
/// Must create subclass.
class BrainModel {
   public:
      /// Number of brain model viewers (also update brain model open gl
      enum BRAIN_MODEL_VIEW_NUMBER {     
         BRAIN_MODEL_VIEW_MAIN_WINDOW       = 0,
         BRAIN_MODEL_VIEW_AUX_WINDOW_2      = 1,
         BRAIN_MODEL_VIEW_AUX_WINDOW_3      = 2,
         BRAIN_MODEL_VIEW_AUX_WINDOW_4      = 3,
         BRAIN_MODEL_VIEW_AUX_WINDOW_5      = 4,
         BRAIN_MODEL_VIEW_AUX_WINDOW_6      = 5,
         BRAIN_MODEL_VIEW_AUX_WINDOW_7      = 6,
         BRAIN_MODEL_VIEW_AUX_WINDOW_8      = 7,
         BRAIN_MODEL_VIEW_AUX_WINDOW_9      = 8,
         BRAIN_MODEL_VIEW_AUX_WINDOW_10     = 9,
         NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS = 10 
      };

      /// The brain model types available
      enum BRAIN_MODEL_TYPE {
         BRAIN_MODEL_CONTOURS,
         BRAIN_MODEL_SURFACE,
         BRAIN_MODEL_VOLUME,
         BRAIN_MODEL_SURFACE_AND_VOLUME
      };
      
      /// Standard views enumerated types
      enum STANDARD_VIEWS {   // if the order of these is changed then
         VIEW_NONE,           // the combo box that uses these must be
         VIEW_RESET,          // updated.
         VIEW_ANTERIOR,
         VIEW_DORSAL,
         VIEW_LATERAL,
         VIEW_MEDIAL,
         VIEW_POSTERIOR,
         VIEW_VENTRAL,
         VIEW_ROTATE_X_90,
         VIEW_ROTATE_Y_90,
         VIEW_ROTATE_Z_90
      };

      /// Destructor
      virtual ~BrainModel();
      
      /// Get a descriptive name of the model
      virtual QString getDescriptiveName() const = 0;
      
      /// Get the model type.
      BRAIN_MODEL_TYPE getModelType() const { return modelType; }
      
      /// Get the index of this model in the brain set
      int getBrainModelIndex() const;
      
      /// get the brain set
      BrainSet* getBrainSet() { return brainSet; }
      
      /// get the brain set const methods
      const BrainSet* getBrainSet() const { return brainSet; }
      
      /// set the brain set
      void setBrainSet(BrainSet* bs) { brainSet = bs; }
      
      /// Copy the sources transformation to me
      void copyTransformations(BrainModel* sourceModel,
                               const int sourceModelNumber,
                               const int targetModelNumber);
                               
      /// get the rotation matrix inverse
      void getRotationMatrixInverse(const int viewNumber, float matrix[16]) const;
      
      /// get the rotation matrix
      void getRotationMatrix(const int viewNumber, float matrix[16]) const;
      
      /// set the rotation matrix
      void setRotationMatrix(const int viewNumber, const float matrix[16]);
      
      /// get the rotation matrix
      vtkTransform* getRotationTransformMatrix(const int viewNumber) { 
         return rotationMatrix[viewNumber]; 
      }
      
      /// get the model's scaling
      void getScaling(const int viewNumber, float scaleOut[3]) const;
            
      /// set the model's scaling
      void setScaling(const int viewNumber, const float scaleIn[3]);
            
      /// set the model's scaling
      void setScaling(const int viewNumber, const float sx, const float sy, const float sz);
            
      /// get the model's translation
      void getTranslation(const int viewNumber, float translationOut[3]) const;
      
      /// set the model's translation
      void setTranslation(const int viewNumber, const float translationIn[3]);
      
      /// get the model's perspective zooming
      float getPerspectiveZooming(const int viewNumber) const;
      
      /// set the model's perspective translation
      void setPerspectiveZooming(const int viewNumber, const float zoomingIn);
      
      /// reset the viewing transformations
      void resetViewingTransformations();
      
      /// get the perspective field of view
      float getPerspectiveFieldOfView() const { return perspectiveFieldOfView; }
      
      /// get transformations as string (3 trans, 16 rot, 1 scale, 1 persp fov)
      QString getTransformationsAsString(const int viewNumber) const;
      
      /// set transformations from string (3 trans, 16 rot, 1 scale, 1 persp fov)
      void setTransformationsAsString(const int viewNumber, const QString s);
      
      /// set to a standard view
      virtual void setToStandardView(const int viewNumber, const STANDARD_VIEWS view);
          
   protected:
      /// Constructor
      BrainModel(BrainSet* bs, const BRAIN_MODEL_TYPE bmt);
      
      /// Copy Constructor
      BrainModel(const BrainModel& bm);
      
      /// brain set this model is associated with
      BrainSet* brainSet;

      /// the model type
      BRAIN_MODEL_TYPE modelType;
      
      /// Rotation of model
      vtkTransform* rotationMatrix[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
      
      /// Scaling of model
      float scaling[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS][3];
      
      /// Translation of model
      float translation[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS][3];
      
      /// Perspective (translate in Z)
      float perspectiveZooming[NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS];
      
      /// perspective field of view
      float perspectiveFieldOfView;
      
   private:
      /// initialize this object
      void initialize(BrainSet* bs, const BRAIN_MODEL_TYPE bmt);
};

#endif
