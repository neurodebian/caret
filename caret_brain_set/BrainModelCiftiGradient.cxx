/*
 *  BrainModelCiftiGradient.cxx
 *  caret_brain_set
 *
 *  Created by Tim Coalson on 7/5/11.
 *  Copyright 2010 Washington University School of Medicine. All rights reserved.
 *
 */

#include <cmath>
#include <iostream>

using namespace std;

#ifdef _OPENMP
#include "omp.h"
#endif

#include "BrainModelCiftiGradient.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceROIMetricGradient.h"
#include "BrainModelSurfaceROIMetricSmoothing.h"
#include "BrainModelVolumeROIGradient.h"
#include "BrainSet.h"
#include "CiftiFile.h"
#include "CiftiXML.h"
#include "DebugControl.h"
#include "MetricFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */ 
BrainModelCiftiGradient::BrainModelCiftiGradient(
           BrainSet* bs,
           BrainModelSurface* leftSurf,
           BrainModelSurface* rightSurf,
           CiftiFile* inputCiftiFile,
           float surfaceKernel,
           float volumeKernel,
           bool averageNormals,
           bool individualOutput,
           bool parallelFlag)
   : BrainModelAlgorithm(bs),
     m_leftSurf(leftSurf),
     m_rightSurf(rightSurf),
     m_inputCiftiFile(inputCiftiFile)
{
   m_surfaceKernel = surfaceKernel;
   m_averageNormals = averageNormals;
   m_volumeKernel = volumeKernel;
   m_individualOutput = individualOutput;
   m_parallelFlag = parallelFlag;
}

/**
 * execute the algorithm.
 */                                                                                               
void 
BrainModelCiftiGradient::execute() throw (BrainModelAlgorithmException)
{
   //m_inputCiftiFile->setCopyData(false);//no accessor yet
   CiftiMatrix* matrix=  m_inputCiftiFile->getCiftiMatrix();
   matrix->setCopyData(false);
   vector <int> dimensions;
   matrix->getMatrixData(m_dataValues,dimensions);
   Nifti2Header header;
   nifti_2_header head;
   this->m_inputCiftiFile->getHeader(header);
   header.getHeaderStruct(head);
   long long rows = head.dim[5];
   long long cols = head.dim[6];
   if(DebugControl::getDebugOn()) cout << "Input Number of Rows is: " << rows << endl;
   if(DebugControl::getDebugOn()) cout << "Input Number of Columns is: " << cols << endl;
   if ((rows <= 0) || (cols <= 0)) {
      throw BrainModelAlgorithmException("Input Cifti file is empty");
   }
   CiftiRootElement myrootXML;
   m_inputCiftiFile->getCiftiXML()->getXMLRoot(myrootXML);//copy of xml
   if (myrootXML.m_numberOfMatrices < 1)//this traversal really should be done in CiftiFile to associate values with the dimensions they apply to
   {
      throw BrainModelAlgorithmException("Input Cifti file has no information on the matrix");
   }
   CiftiMatrixElement& mymatXML = myrootXML.m_matrices[0];
   vector<TransformationMatrixVoxelIndicesIJKtoXYZElement>& mytransvec = mymatXML.m_volume[0].m_transformationMatrixVoxelIndicesIJKtoXYZ;
   if (mytransvec.size() < 1)
   {
      throw BrainModelAlgorithmException("Input Cifti file has no volume spacing information");
   }
   float volSpace[4][4];
   for (int i = 0; i < 4; ++i)
   {
      for (int j = 0; j < 4; ++j)
      {
         volSpace[i][j] = mytransvec[0].m_transform[(i << 2) + j];//equals i * 4 + j
      }
   }
   float origin[3] = {volSpace[0][3], volSpace[1][3], volSpace[2][3]};//not really used, we allocate smaller volumes so the origin will be wrong unless we apply the offset
   float spacing[3] = {volSpace[0][0] + volSpace[1][0] + volSpace[2][0],
                       volSpace[0][1] + volSpace[1][1] + volSpace[2][1],
                       volSpace[0][2] + volSpace[1][2] + volSpace[2][2]
                      };//if axes aren't orthogonal, then shame on you, expect something weird
   vector<CiftiMatrixIndicesMapElement>& maparray = mymatXML.m_matrixIndicesMap;
   if (maparray.size() < 1)
   {
      throw BrainModelAlgorithmException("Input Cifti file has no mapping information");
   }
   bool found = false;
   int firstmatch;
   for (int i = 0; !found && i < (int)(maparray.size()); ++i)
   {
      for (int j = 0; j < (int)(maparray[i].m_appliesToMatrixDimension.size()); ++j)
      {
         if (maparray[i].m_appliesToMatrixDimension[j] == 0)
         {
            found = true;
            firstmatch = i;
            break;
         }
      }
   }
   vector<CiftiBrainModelElement>& mymodels = maparray[firstmatch].m_brainModels;
   MetricFile tempLeftMetric;
   MetricFile tempRightMetric;
   MetricFile* leftROI = new MetricFile();
   MetricFile* rightROI = new MetricFile();
   leftROI->setNumberOfNodesAndColumns(m_leftSurf->getNumberOfNodes(), 1);
   rightROI->setNumberOfNodesAndColumns(m_rightSurf->getNumberOfNodes(), 1);
   tempLeftMetric.setNumberOfNodesAndColumns(m_leftSurf->getNumberOfNodes(), cols);
   tempRightMetric.setNumberOfNodesAndColumns(m_rightSurf->getNumberOfNodes(), cols);
   for (int i = 0; i < (int)mymodels.size(); ++i)
   {
      if (mymodels[i].m_modelType == CIFTI_MODEL_TYPE_SURFACE)
      {
         bool rightFlag;
         if (mymodels[i].m_brainStructure.endsWith("_LEFT"))
         {
            rightFlag = false;
         } else {
            if (mymodels[i].m_brainStructure.endsWith("_RIGHT"))
            {
               rightFlag = true;
            } else {
               cerr << "skipping unknown surface structure, mapping " << i << endl;
               continue;
            }
         }
         MetricFile* tempROI, *process;
         BrainModelSurface* surfaceUsed;
         if (rightFlag)
         {
            tempROI = rightROI;
            process = &tempRightMetric;
            surfaceUsed = m_rightSurf;
         } else {
            tempROI = leftROI;
            process = &tempLeftMetric;
            surfaceUsed = m_leftSurf;
         }
         const int numNodes = surfaceUsed->getNumberOfNodes();
         if (process->getNumberOfColumns() != cols)
         {
            process->setNumberOfNodesAndColumns(surfaceUsed->getNumberOfNodes(), cols);
         }
         if (numNodes != (int)(mymodels[i].m_surfaceNumberOfNodes))
         {
            throw BrainModelAlgorithmException("Input Cifti file mapping gives a different number of nodes");
         }
         tempROI->setColumnAllNodesToScalar(0, 0.0f);
         vector<unsigned long long>* myindices = &(mymodels[i].m_nodeIndices);
         vector<unsigned long long> fallbackIndices;
         if (myindices->size() != mymodels[i].m_indexCount)
         {
            if (myindices->size() == 0 && (int)mymodels[i].m_indexCount == numNodes)
            {
               fallbackIndices.resize(numNodes);
               for (long i = 0; i < numNodes; ++i)
               {
                  fallbackIndices[i] = i;
               }
               myindices = &fallbackIndices;
            } else {
               throw BrainModelAlgorithmException("Input Cifti file indices mapping is inconsistent");
            }
         }
         for (int j = 0; j < (int)myindices->size(); ++j)
         {
            tempROI->setValue((*myindices)[j], 0, 1.0f);
            process->setAllColumnValuesForNode((*myindices)[j], m_dataValues + cols * (mymodels[i].m_indexOffset + j));
         }//finally, we can process it
         BrainModelSurfaceROIMetricSmoothing smoothobj(NULL,
               surfaceUsed,
               process,
               tempROI,
               1.0f,//unused
               1,//should always be 1
               0.0f,//unused
               0.0f,//unused
               0.0f,//unused
               0.0f,//unused
               0.0f,//unused
               m_surfaceKernel,
               m_parallelFlag);
         smoothobj.execute();
         BrainModelSurfaceROIMetricGradient gradobj(NULL,
                                                   surfaceUsed,
                                                   tempROI,
                                                   process,
                                                   m_averageNormals,
                                                   m_parallelFlag);
         gradobj.execute();//now, copy it back into the cifti
         for (int j = 0; j < (int)myindices->size(); ++j)
         {//note: index mappings must not overlap, or some areas will get done twice
            process->getAllColumnValuesForNode((*myindices)[j], m_dataValues + cols * (mymodels[i].m_indexOffset + j));
         }
      }
      if (mymodels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
      {
         vector<voxelIndexType> &myvoxels = mymodels[i].m_voxelIndicesIJK;//really just unsigned long long
         if (myvoxels.size() != mymodels[i].m_indexCount * 3)
         {
            throw BrainModelAlgorithmException("Input Cifti file indices mapping is inconsistent");
         }
         voxelIndexType minmax[6];
         minmax[0] = minmax[1] = myvoxels[0];
         minmax[2] = minmax[3] = myvoxels[1];
         minmax[4] = minmax[5] = myvoxels[2];
         for (int j = 3; j < (int)myvoxels.size(); j += 3)
         {
            if (myvoxels[j] < minmax[0]) minmax[0] = myvoxels[j];
            if (myvoxels[j] > minmax[1]) minmax[1] = myvoxels[j];
            if (myvoxels[j + 1] < minmax[2]) minmax[2] = myvoxels[j + 1];
            if (myvoxels[j + 1] > minmax[3]) minmax[3] = myvoxels[j + 1];
            if (myvoxels[j + 2] < minmax[4]) minmax[4] = myvoxels[j + 2];
            if (myvoxels[j + 2] > minmax[5]) minmax[5] = myvoxels[j + 2];
         }
         int voxextent[3] = {minmax[1] - minmax[0] + 1, minmax[3] - minmax[2] + 1, minmax[5] - minmax[4] + 1};
         int voxoffset[3] = {minmax[0], minmax[2], minmax[4]};
         VolumeFile::ORIENTATION orient[3] = {VolumeFile::ORIENTATION_RIGHT_TO_LEFT,
                                             VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
                                             VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR};
#ifdef _OPENMP
         if (m_parallelFlag && omp_get_max_threads() > 1)
         {
#pragma omp parallel
            {
               VolumeFile* processVol, *tempROIVol, *tempOutVol;
               processVol = new VolumeFile();//private memory that they don't continuously reallocate
               processVol->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, voxextent, orient, origin, spacing);
               tempROIVol = new VolumeFile(*processVol);//takes a reference, not a pointer
               tempOutVol = new VolumeFile(*processVol);
#pragma omp for
               for (int k = 0; k < cols; ++k)
               {
                  tempROIVol->setAllVoxels(-1.0f);
                  int j1 = 0;//so we don't need a divide every copy
                  long jcols = 0;//ditto for multiply
                  long indexbase = cols * mymodels[i].m_indexOffset;//and to make sure this multiply gets optimized out
                  for (int j = 0; j < (int)myvoxels.size(); j += 3, ++j1, jcols += cols)
                  {
                     tempROIVol->setVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2], 0, 1.0f);
                     processVol->setVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2], 0, m_dataValues[indexbase + jcols + k]);
                  }//volume is copied, now run gradient
                  BrainModelVolumeROIGradient volgradobj(NULL,
                                                         processVol,
                                                         tempROIVol,
                                                         tempOutVol,
                                                         m_volumeKernel);
                  volgradobj.execute();
                  j1 = 0;
                  jcols = 0;
                  for (int j = 0; j < (int)myvoxels.size(); j += 3, ++j1, jcols += cols)
                  {
                     m_dataValues[indexbase + jcols + k] = tempOutVol->getVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2]);
                  }
               }
               delete processVol;
               delete tempROIVol;
               delete tempOutVol;
            }//omp parallel
         } else {
            VolumeFile* processVol, *tempROIVol, *tempOutVol;
            processVol = new VolumeFile();
            processVol->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, voxextent, orient, origin, spacing);
            tempROIVol = new VolumeFile(*processVol);//takes a reference, not a pointer
            tempOutVol = new VolumeFile(*processVol);
            for (int k = 0; k < cols; ++k)
            {
               tempROIVol->setAllVoxels(-1.0f);
               int j1 = 0;//so we don't need a divide every copy
               long jcols = 0;//ditto for multiply
               long indexbase = cols * mymodels[i].m_indexOffset;//and to make sure this multiply gets optimized out
               for (int j = 0; j < (int)myvoxels.size(); j += 3, ++j1, jcols += cols)
               {
                  tempROIVol->setVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2], 0, 1.0f);
                  processVol->setVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2], 0, m_dataValues[indexbase + jcols + k]);
               }//volume is copied, now run gradient
               BrainModelVolumeROIGradient volgradobj(NULL,
                                                      processVol,
                                                      tempROIVol,
                                                      tempOutVol,
                                                      m_volumeKernel);
               volgradobj.execute();
               j1 = 0;
               jcols = 0;
               for (int j = 0; j < (int)myvoxels.size(); j += 3, ++j1, jcols += cols)
               {
                  m_dataValues[indexbase + jcols + k] = tempOutVol->getVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2]);
               }
            }
            delete processVol;
            delete tempROIVol;
            delete tempOutVol;
         }
#else
         VolumeFile* processVol, *tempROIVol, *tempOutVol;
         processVol = new VolumeFile();
         processVol->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, voxextent, orient, origin, spacing);
         tempROIVol = new VolumeFile(*processVol);//takes a reference, not a pointer
         tempOutVol = new VolumeFile(*processVol);
         for (int k = 0; k < cols; ++k)
         {
            tempROIVol->setAllVoxels(-1.0f);
            int j1 = 0;//so we don't need a divide every copy
            long jcols = 0;//ditto for multiply
            long indexbase = cols * mymodels[i].m_indexOffset;//and to make sure this multiply gets optimized out
            for (int j = 0; j < myvoxels.size(); j += 3, ++j1, jcols += cols)
            {
               tempROIVol->setVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2], 0, 1.0f);
               processVol->setVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2], 0, m_dataValues[indexbase + jcols + k]);
            }//volume is copied, now run gradient
            BrainModelVolumeROIGradient volgradobj(NULL,
                                                   processVol,
                                                   tempROIVol,
                                                   tempOutVol,
                                                   m_volumeKernel);
            volgradobj.execute();
            j1 = 0;
            jcols = 0;
            for (int j = 0; j < myvoxels.size(); j += 3, ++j1, jcols += cols)
            {
               m_dataValues[indexbase + jcols + k] = tempOutVol->getVoxel(myvoxels[j] - voxoffset[0], myvoxels[j + 1] - voxoffset[1], myvoxels[j + 2] - voxoffset[2]);
            }
         }
         delete processVol;
         delete tempROIVol;
         delete tempOutVol;
#endif
      }
   }
   delete leftROI;
   delete rightROI;
   if (!m_individualOutput)
   {
      float* output = new float[rows];
      dimensions.clear();
      dimensions.push_back(rows);
      dimensions.push_back(1);
      dimensions.push_back(1);
#ifdef _OPENMP
      if (m_parallelFlag && omp_get_max_threads() > 1)
      {
#pragma omp parallel for
         for (long long i = 0; i < rows; ++i)
         {
            output[i] = 0.0f;
            long long indexbase = i * cols;
            for (long long j = 0; j < cols; ++j)
            {
               output[i] += m_dataValues[indexbase + j];
            }
            output[i] /= cols;
         }
      } else {
         for (long long i = 0; i < rows; ++i)
         {
            long long indexbase = i * cols;
            output[i] = 0.0f;
            for (long long j = 0; j < cols; ++j)
            {
               output[i] += m_dataValues[indexbase + j];
            }
            output[i] /= cols;
         }
      }
#else
      for (long long i = 0; i < rows; ++i)
      {
         long long indexbase = i * cols;
         output[i] = 0.0f;
         for (long long j = 0; j < cols; ++j)
         {
            output[i] += m_dataValues[indexbase + j];
         }
         output[i] /= cols;
      }
#endif
      matrix->setMatrixData(output, dimensions);//takes ownership of the pointer and deletes old matrix
      for (int i = 0; i < (int)maparray.size(); ++i)
      {//search for and remove all mappings for dimension 1 in matrix 0
         for (int j = 0; j < (int)maparray[i].m_appliesToMatrixDimension.size(); ++j)
         {
            if (maparray[i].m_appliesToMatrixDimension[j] == 1)
            {
               maparray[i].m_appliesToMatrixDimension.erase(maparray[i].m_appliesToMatrixDimension.begin() + j);
               --j;//move back one place so we don't skip anything
            }
         }
         if (maparray[i].m_appliesToMatrixDimension.size() == 0)
         {
            maparray.erase(maparray.begin() + i);
            --i;//move back
         }
      }
      m_inputCiftiFile->getCiftiXML()->setXMLRoot(myrootXML);//maparray was a reference in this, so it should be changed now
      m_inputCiftiFile->setCiftiMatrix(*matrix);//because getMatrix sets matrix to NULL for some reason
   } else {
      m_inputCiftiFile->setCiftiMatrix(*matrix);
   }
}
