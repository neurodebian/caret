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

#include <sstream>

#include <QTextStream>

#include "BrainModel.h"
#include "BrainSet.h"
#include "StringUtilities.h"
#include "vtkTransform.h"

/**
 * Constructor
 */
BrainModel::BrainModel(BrainSet* bs, const BRAIN_MODEL_TYPE bmt) 
{ 
   initialize(bs, bmt);
}

/**
 * Copy Constructor.
 */
BrainModel::BrainModel(const BrainModel& bm)
{
   initialize(bm.brainSet, bm.modelType);
   
   //
   // Do not copy these members
   //   rotationMatrix
   //   scaling
   //   translation
   //
}

/**
 * Destructor.
 */
BrainModel::~BrainModel()
{
   for (int i = 0; i < NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      rotationMatrix[i]->Delete();
   }
}

/**
 * Initialize this object
 */
void
BrainModel::initialize(BrainSet* bs, const BRAIN_MODEL_TYPE bmt)
{
   brainSet = bs;   
   modelType = bmt; 
   for (int i = 0; i < NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      rotationMatrix[i] = vtkTransform::New();
   }
   perspectiveFieldOfView = 30.0;
   resetViewingTransformations();
}

/**
 * Get the index of this model in the brain set.
 */
int 
BrainModel::getBrainModelIndex() const
{
   int modelNumber = -1;
   
   if (brainSet != NULL) {
      for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
         if (brainSet->getBrainModel(i) == this) {
            modelNumber = i;
            break;
         }
      }
   }
   
   return modelNumber;
}
      
/**
 * Reset the viewing transformations of the model
 */
void
BrainModel::resetViewingTransformations()
{
   for (int i = 0; i < NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; i++) {
      rotationMatrix[i]->Identity();
      scaling[i][0] = 1.0;
      scaling[i][1] = 1.0;
      scaling[i][2] = 1.0;
      translation[i][0] = 0.0;
      translation[i][1] = 0.0;
      translation[i][2] = 0.0;
      perspectiveZooming[i] = 200.0;
   }
}

/**
 * Get the rotation matrix as an array
 */
void
BrainModel::getRotationMatrix(const int viewNumber, float matrix[16]) const
{
   vtkMatrix4x4* m = vtkMatrix4x4::New();
   rotationMatrix[viewNumber]->GetMatrix(m);
   
   int cnt = 0;
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[cnt] = m->GetElement(i, j);
         cnt++;
      }
   }
   m->Delete();
}

/**
 * Get the rotation matrix inverse as an array
 */
void
BrainModel::getRotationMatrixInverse(const int viewNumber, float matrix[16]) const
{
   vtkMatrix4x4* m = vtkMatrix4x4::New();
   rotationMatrix[viewNumber]->GetMatrix(m);
   m->Invert();
   
   int cnt = 0;
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[cnt] = m->GetElement(i, j);
         cnt++;
      }
   }
   m->Delete();
}

/**
 * Set the rotation matrix as an array.
 */
void
BrainModel::setRotationMatrix(const int viewNumber, const float matrix[16])
{
   vtkMatrix4x4* m = vtkMatrix4x4::New();
   rotationMatrix[viewNumber]->GetMatrix(m);
   
   int cnt = 0;
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m->SetElement(i, j, matrix[cnt]);
         cnt++;
      }
   }
   rotationMatrix[viewNumber]->SetMatrix(m);
   m->Delete();
}

/**
 * Get the model's translation.
 */
void 
BrainModel::getTranslation(const int viewNumber, float translationOut[3]) const
{
   translationOut[0] = translation[viewNumber][0];
   translationOut[1] = translation[viewNumber][1];
   translationOut[2] = translation[viewNumber][2];
}

/**
 * Set the model's translation.
 */
void 
BrainModel::setTranslation(const int viewNumber, const float translationIn[3])
{
   translation[viewNumber][0] = translationIn[0];
   translation[viewNumber][1] = translationIn[1];
   translation[viewNumber][2] = translationIn[2];
}

/**
 * get the model's scaling.
 */
void 
BrainModel::getScaling(const int viewNumber, float scaleOut[3]) const 
{ 
   scaleOut[0] = scaling[viewNumber][0]; 
   scaleOut[1] = scaling[viewNumber][1]; 
   scaleOut[2] = scaling[viewNumber][2]; 
}

/**
 * set the model's scaling.
 */
void 
BrainModel::setScaling(const int viewNumber, const float scaleIn[3]) 
{ 
   scaling[viewNumber][0] = scaleIn[0]; 
   scaling[viewNumber][1] = scaleIn[1]; 
   scaling[viewNumber][2] = scaleIn[2]; 
}

/**
 * set the model's scaling.
 */
void 
BrainModel::setScaling(const int viewNumber, const float sx, const float sy, const float sz)
{
   scaling[viewNumber][0] = sx; 
   scaling[viewNumber][1] = sy; 
   scaling[viewNumber][2] = sz; 
}
            
/**
 * Get the model's perspective zooming.
 */
float 
BrainModel::getPerspectiveZooming(const int viewNumber) const
{
   return perspectiveZooming[viewNumber];
}

/**
 * Set the model's perspective zooming.
 */
void 
BrainModel::setPerspectiveZooming(const int viewNumber, const float zoomingIn)
{
   perspectiveZooming[viewNumber] = zoomingIn;
}

/**
 * Copy the transformations from the specified model and model viewer.  Used when 
 * views are yoked.
 */
void
BrainModel::copyTransformations(BrainModel* sourceModel,
                                       const int sourceModelNumber,
                                       const int targetModelNumber)
{
   //
   // Copy scaling.
   //
   float scale[3];
   sourceModel->getScaling(sourceModelNumber, scale);
   setScaling(targetModelNumber, scale);
   
   //
   // Copy translation.
   //
   float trans[3];
   sourceModel->getTranslation(sourceModelNumber, trans);
   setTranslation(targetModelNumber, trans);
   
   //
   // Copy perspective zooming.
   //
   setPerspectiveZooming(targetModelNumber, sourceModel->getPerspectiveZooming(sourceModelNumber));
   
   //
   // Copy rotation matrix
   //
   float mat[16];
   sourceModel->getRotationMatrix(sourceModelNumber, mat);
   setRotationMatrix(targetModelNumber, mat);
}

/**
 * get transformations as string (3 trans, 16 rot, 3 scale, 1 persp fov).
 */
QString 
BrainModel::getTransformationsAsString(const int viewNumber) const
{
   
   std::ostringstream str;
   str.precision(2);
   str.setf(std::ios::fixed);
   
   float trans[3];
   getTranslation(viewNumber, trans);
   for (int i = 0; i < 3; i++) {
      str << trans[i]
          << " ";
   }

   float mat[16];
   getRotationMatrix(viewNumber, mat);
   for (int i = 0; i < 16; i++) {
      str << mat[i]
          << " ";
   }
   
   float scale[3];
   getScaling(viewNumber, scale);
   str << scale[0]
       << " "
       << scale[1]
       << " "
       << scale[2]
       << " "
       << getPerspectiveZooming(viewNumber);
       
   const QString s(str.str().c_str());

   return s;
}

/**
 * set transformations from string (3 trans, 16 rot, 1 (or 3) scale, 1 persp fov).
 */
void 
BrainModel::setTransformationsAsString(const int viewNumber, const QString s)
{
   std::vector<QString> tokens;
   StringUtilities::token(s, " ", tokens);
   
   QString s2(s);
   QTextStream textStream(&s2, QIODevice::ReadOnly);
   
   float trans[3];
   textStream >> trans[0] >> trans[1] >> trans[2];
   setTranslation(viewNumber, trans);

   float mat[16];
   for (int i = 0; i < 16; i++) {
      textStream >> mat[i];
   }
   setRotationMatrix(viewNumber, mat);
   
   float scale[3] = { 1.0, 1.0, 1.0 };
   textStream >> scale[0];
   if (tokens.size() == 23) {
      textStream >> scale[1];
      textStream >> scale[2];
   }
   else {
      scale[1] = scale[0];
      scale[2] = scale[0];
   }
   setScaling(viewNumber, scale);
   
   float perZoom;
   textStream >> perZoom;
   setPerspectiveZooming(viewNumber, perZoom);
}
      
/**
 * Set the surface to a standard view
 */
void 
BrainModel::setToStandardView(const int viewNumber, const STANDARD_VIEWS view)
{
   float m[16];
   bool valid = true;
   
   const Structure structure = brainSet->getStructure();
   
   switch(view) {
      case VIEW_LATERAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  0.0;
           m[1]  =  0.0;
           m[2]  = -1.0;
           m[3]  =  0.0;
           m[4]  = -1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  0.0;
           m[1]  =  0.0;
           m[2]  =  1.0;
           m[3]  =  0.0;
           m[4]  =  1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_MEDIAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  0.0;       
           m[1]  =  0.0;
           m[2]  =  1.0;
           m[3]  =  0.0;  
           m[4]  =  1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  0.0;
           m[1]  =  0.0;
           m[2]  = -1.0;
           m[3]  =  0.0;
           m[4]  = -1.0;
           m[5]  =  0.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_POSTERIOR:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  = -1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  = -1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_ANTERIOR:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  =  1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  0.0;
           m[6]  =  1.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  1.0;
           m[10] =  0.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_VENTRAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] = -1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  = -1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] = -1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_DORSAL:
         if (structure.getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] =  1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         else {
           m[0]  =  1.0;
           m[1]  =  0.0;
           m[2]  =  0.0;
           m[3]  =  0.0;
           m[4]  =  0.0;
           m[5]  =  1.0;
           m[6]  =  0.0;
           m[7]  =  0.0;
           m[8]  =  0.0;
           m[9]  =  0.0;
           m[10] =  1.0;
           m[11] =  0.0;
           m[12] =  0.0;
           m[13] =  0.0;
           m[14] =  0.0;
           m[15] =  1.0;
         }
         break;
      case VIEW_RESET:
         m[0]  =  1.0;
         m[1]  =  0.0;
         m[2]  =  0.0;
         m[3]  =  0.0;
         m[4]  =  0.0;
         m[5]  =  1.0;
         m[6]  =  0.0;
         m[7]  =  0.0;
         m[8]  =  0.0;
         m[9]  =  0.0;
         m[10] =  1.0;
         m[11] =  0.0;
         m[12] =  0.0;
         m[13] =  0.0;
         m[14] =  0.0;
         m[15] =  1.0;
         perspectiveZooming[viewNumber] = 100.0; //defaultPerspectiveZooming;
         scaling[viewNumber][0] = 1.0;
         scaling[viewNumber][1] = 1.0;
         scaling[viewNumber][2] = 1.0;
         translation[viewNumber][0] = 0.0;
         translation[viewNumber][1] = 0.0;
         translation[viewNumber][2] = 0.0;
         break;
      case VIEW_ROTATE_X_90:
         rotationMatrix[viewNumber]->RotateX(90.0);
         valid = false;
         break;
      case VIEW_ROTATE_Y_90:
         rotationMatrix[viewNumber]->RotateY(90.0);
         valid = false;
         break;
      case VIEW_ROTATE_Z_90:
         rotationMatrix[viewNumber]->RotateZ(-90.0);
         valid = false;
         break;
      case VIEW_NONE:
      default:
         valid = false;
         break;
   }
   
   if (valid) {
      setRotationMatrix(viewNumber, m);
   }
}

