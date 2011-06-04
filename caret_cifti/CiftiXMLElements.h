/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
#ifndef __CIFTI_XML_ELEMENTS
#define __CIFTI_XML_ELEMENTS
#include <QtCore>
#include <vector>
#include "nifti2.h"
/* Cifti Defines */

enum ModelType {
   CIFTI_MODEL_TYPE_SURFACE=1,
   CIFTI_MODEL_TYPE_VOXELS=2
};

enum IndicesMapToDataType {
   CIFTI_INDEX_TYPE_BRAIN_MODELS=1,
   CIFTI_INDEX_TYPE_FIBERS=2,
   CIFTI_INDEX_TYPE_PARCELS=3,
   CIFTI_INDEX_TYPE_TIME_POINTS=4
};

typedef unsigned long long voxelIndexType;

class CiftiBrainModelElement {
public:
   //CiftiBrainModelElement();

   unsigned long long m_indexOffset;
   unsigned long long m_indexCount;
   ModelType m_modelType;
   QString m_brainStructure;
   unsigned long long m_surfaceNumberOfNodes;
//children
   std::vector<unsigned long long> m_nodeIndices;
   std::vector<voxelIndexType> m_voxelIndicesIJK;   
};

class CiftiMatrixIndicesMapElement
{
public:
   CiftiMatrixIndicesMapElement()
   {
      m_timeStep = -1.0;
      m_timeStepUnits = -1;
   }

   std::vector<int> m_appliesToMatrixDimension;
   IndicesMapToDataType m_indicesMapToDataType;
   double m_timeStep;
   int m_timeStepUnits;
   std::vector<CiftiBrainModelElement> m_brainModels;
};

class CiftiLabelElement {
public:
   CiftiLabelElement()
   {
      m_red = m_green = m_blue = m_alpha = m_x = m_y = m_z = 0.0;
   }
   unsigned long long m_key;
   float m_red;
   float m_green;
   float m_blue;
   float m_alpha;
   float m_x;
   float m_y;
   float m_z;
   QString m_text;
};

class TransformationMatrixVoxelIndicesIJKtoXYZ
{
public:
   unsigned long m_dataSpace;
   unsigned long m_transformedSpace;
   unsigned long m_unitsXYZ;
   float m_transform[16];
};

class CiftiVolume {
public:
   std::vector<TransformationMatrixVoxelIndicesIJKtoXYZ> m_transformationMatrixVoxelIndicesIJKtoXYZ;
   unsigned int m_volumeDimensions[3];
};

class CiftiMatrixElement {
public:
   std::vector<CiftiLabelElement> m_labelTable;//TODO, this may be better as a hash
   QHash<QString, QString> m_userMetaData; //user meta data
   std::vector<CiftiMatrixIndicesMapElement> m_matrixIndicesMap;
   std::vector<CiftiVolume> m_volume;    
};

class CiftiRootElement {
public:
   QString m_version;
   unsigned long m_numberOfMatrices;
   std::vector<CiftiMatrixElement> m_matrices;
};


#endif //__CIFTI_ELEMENTS