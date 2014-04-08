/*
 *  BrainModelCiftiROIDenseConnectomeGradient.cxx
 *  caret_brain_set
 *
 *  Created by Tim Coalson on 9/12/11.
 *  Copyright 2010 Washington University School of Medicine. All rights reserved.
 *
 */

#include <cmath>
#include <iostream>

using namespace std;

#ifdef _OPENMP
#include "omp.h"
#endif

#include "BrainModelCiftiROIDenseConnectomeGradient.h"
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

BrainModelCiftiROIDenseConnectomeGradient::BrainModelCiftiROIDenseConnectomeGradient(
                           BrainSet* bs,
                           BrainModelSurface* leftSurf,
                           BrainModelSurface* rightSurf,
                           CiftiFile* inputCiftiFile,
                           MetricFile* leftROI,
                           MetricFile* rightROI,
                           float surfaceKernel,
                           float volumeKernel,
                           bool averageNormals,
                           bool debug)
   : BrainModelAlgorithm(bs),
      m_leftSurf(leftSurf),
      m_rightSurf(rightSurf),
      m_inputCiftiFile(inputCiftiFile),
      m_leftROI(leftROI),
      m_rightROI(rightROI)
{
   m_surfaceKernel = surfaceKernel;
   m_volumeKernel = volumeKernel;
   m_averageNormals = averageNormals;
   m_debug = debug;
}

void 
BrainModelCiftiROIDenseConnectomeGradient::execute() throw (BrainModelAlgorithmException)
{
   if (m_leftROI == NULL && m_rightROI == NULL)
   {
      throw BrainModelAlgorithmException("No ROI specified");
   }
   CiftiMatrix* matrix=  m_inputCiftiFile->getCiftiMatrix();
   matrix->setCopyData(false);
   vector <int> dimensions;
   float* dataValues;
   matrix->getMatrixData(dataValues,dimensions);
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
   CiftiMatrixElement& mymatXML = myrootXML.m_matrices[0];
   vector<CiftiMatrixIndicesMapElement>& maparray = mymatXML.m_matrixIndicesMap;
   if (myrootXML.m_numberOfMatrices < 1)//this traversal really should be done in CiftiFile to associate values with the dimensions they apply to
   {
      throw BrainModelAlgorithmException("Input Cifti file has no information on the matrix");
   }
   /*vector<TransformationMatrixVoxelIndicesIJKtoXYZElement>& mytransvec = mymatXML.m_volume[0].m_transformationMatrixVoxelIndicesIJKtoXYZ;
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
      volSpace[0][2] + volSpace[1][2] + volSpace[2][2]};//if axes aren't orthogonal, then shame on you, expect something weird
   //*/ //don't check volume stuff until we support volume ROI
   if (maparray.size() < 1)
   {
      throw BrainModelAlgorithmException("Input Cifti file has no mapping information");
   }
   int leftOffset, rightOffset;
   bool found = false;
   int firstmatch = 0;
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
   vector<int> myLeftIndexList, myRightIndexList;
   vector<unsigned long long> myLeftNodeList, myRightNodeList;
   vector<float> myLeftOutput, myRightOutput;
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
         MetricFile* tempROI;
         MetricFile process;
         vector<int>* myIndexList;
         vector<unsigned long long>* myNodeList;
         vector<float>* myOutput;
         BrainModelSurface* surfaceUsed;
         if (rightFlag)
         {
            tempROI = m_rightROI;
            surfaceUsed = m_rightSurf;
            myIndexList = &myRightIndexList;
            myNodeList = &myRightNodeList;
            myOutput = &myRightOutput;
            rightOffset = mymodels[i].m_indexOffset;
         } else {
            tempROI = m_leftROI;
            surfaceUsed = m_leftSurf;
            myIndexList = &myLeftIndexList;
            myNodeList = &myLeftNodeList;
            myOutput = &myLeftOutput;
            leftOffset = mymodels[i].m_indexOffset;
         }
         if (tempROI != NULL)
         {
            const int numNodes = surfaceUsed->getNumberOfNodes();
            if (numNodes != (int)(mymodels[i].m_surfaceNumberOfNodes) || numNodes != tempROI->getNumberOfNodes())
            {
               throw BrainModelAlgorithmException("Input Cifti file mapping gives a different number of nodes");
            }
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
            //TODO: get the index list for the ROI
            const int indexSize = (int)myindices->size();
            myIndexList->clear();
            myNodeList->clear();
            for (int j = 0; j < indexSize; ++j)
            {
               if (tempROI->getValue((*myindices)[j], 0) != 0.0f)
               {
                  myIndexList->push_back(j);
                  myNodeList->push_back((*myindices)[j]);
               }
            }
            const int roiSize = (int)myIndexList->size();
            vector<double> means, stdevs;
            process.setNumberOfNodesAndColumns(numNodes, roiSize);
            means.resize(roiSize);
            stdevs.resize(roiSize);
#ifdef _OPENMP
#pragma omp parallel for
#endif
            for (int j = 0; j < roiSize; ++j)
            {
               double jAvg = 0.0, jStdev = 0.0;//first, lets precompute all row means and stdevs
               float* rowReference = dataValues + cols * (mymodels[i].m_indexOffset + (*myIndexList)[j]);
               for (int k = 0; k < cols; ++k)
               {
                  jAvg += rowReference[k];
               }
               jAvg /= (double)cols;
               for (int k = 0; k < cols; ++k)
               {
                  double tempd = rowReference[k] - jAvg;
                  jStdev += tempd * tempd;
               }
               jStdev /= (double)cols;//maximum likelihood formula, as opposed to sample standard deviation
               jStdev = sqrt(jStdev);
               means[j] = jAvg;
               stdevs[j] = jStdev;
            }
            long long complete = 0;
            long long total = roiSize * (roiSize - 1) >> 1;
            int lastdots = 0, thisdots;
            cout << "computing correlation for ";
            if (rightFlag)
            {
               cout << "right";
            } else {
               cout << "left";
            }
            cout << " surface" << endl;
            cout << "|0%                 complete                 100%|" << endl;
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1)
#endif
            for (int j = 0; j < roiSize; ++j)
            {
               int myJ = (*myIndexList)[j];
               int myjnode = (*myNodeList)[j];
               process.setValue(myjnode, j, 1.0f);
               float* jRef = dataValues + cols * (mymodels[i].m_indexOffset + myJ);
               double jAvg = means[j], jStdev = stdevs[j];
               for (int k = j + 1; k < roiSize; ++k)
               {
                  int myK = (*myIndexList)[k];
                  int myknode = (*myNodeList)[k];
                  float* kRef = dataValues + cols * (mymodels[i].m_indexOffset + myK);
                  double correlation = 0.0;
                  double kAvg = means[k];
                  for (int m = 0; m < cols; ++m)
                  {
                     correlation += (jRef[m] - jAvg) * (kRef[m] - kAvg);
                  }
                  correlation /= (double)cols * jStdev * stdevs[k];
                  process.setValue(myjnode, k, correlation);
                  process.setValue(myknode, j, correlation);
               }
#ifdef _OPENMP
#pragma omp critical
#endif
               {
                  complete += roiSize - j - 1;
                  thisdots = 50 * complete / total;
                  while (lastdots < thisdots)
                  {
                     cout << '.';
                     cout.flush();
                     ++lastdots;
                  }
               }
            }
            cout << endl;
            if (m_debug)
            {
               if (rightFlag)
               {
                  process.writeFile(QString("debugRightCorrelation.metric"));
               } else {
                  process.writeFile(QString("debugLeftCorrelation.metric"));
               }
            }
            BrainModelSurfaceROIMetricSmoothing smoothobj(NULL,
                                                         surfaceUsed,
                                                         &process,
                                                         tempROI,
                                                         1.0f,//unused
                                                         1,//should always be 1
                                                         0.0f,//unused
                                                         0.0f,//unused
                                                         0.0f,//unused
                                                         0.0f,//unused
                                                         0.0f,//unused
                                                         m_surfaceKernel,
                                                         true);
            smoothobj.execute();
            if (m_debug)
            {
               if (rightFlag)
               {
                  process.writeFile(QString("debugRightSmoothedCorrelation.metric"));
               } else {
                  process.writeFile(QString("debugLeftSmoothedCorrelation.metric"));
               }
            }
            BrainModelSurfaceROIMetricGradient gradobj(NULL,
                                                      surfaceUsed,
                                                      tempROI,
                                                      &process,
                                                      m_averageNormals,
                                                      true);
            gradobj.execute();
            if (m_debug)
            {
               if (rightFlag)
               {
                  process.writeFile(QString("debugRightGradient.metric"));
               } else {
                  process.writeFile(QString("debugLeftGradient.metric"));
               }
            }
            myOutput->resize(roiSize);
#ifdef _OPENMP
#pragma omp parallel for
#endif
            for (int j = 0; j < roiSize; ++j)
            {
               int myjnode = (*myNodeList)[j];
               double avg = 0.0;
               for (int k = 0; k < roiSize; ++k)
               {
                  avg += process.getValue(myjnode, k);
               }
               (*myOutput)[j] = (float)(avg / roiSize);
            }
         }
      }
      if (mymodels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
      {//not currently supported
         /*vector<voxelIndexType> &myvoxels = mymodels[i].m_voxelIndicesIJK;//really just unsigned long long
         if (myvoxels.size() != mymodels[i].m_indexCount * 3)
         {
            throw BrainModelAlgorithmException("Input Cifti file indices mapping is inconsistent");
         }
         unsigned long numVoxels = mymodels[i].m_indexCount;
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
         vector<double> means, stdevs, output;
         vector<vector<double> > correlations;
         correlations.resize(numVoxels);
         means.resize(numVoxels);
         stdevs.resize(numVoxels);
         output.resize(numVoxels);
         VolumeFile* tempROIVol;
         tempROIVol = new VolumeFile();
         tempROIVol->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT, voxextent, orient, origin, spacing);
         tempROIVol->setAllVoxels(-1.0f);
#ifdef _OPENMP
#pragma omp parallel for
#endif
         for (unsigned long j = 0; j < numVoxels; ++j)
         {
            int ijk[3];
            correlations[j].resize(numVoxels);
            unsigned long base = j * 3;
            ijk[0] = myvoxels[base] - voxoffset[0];
            ijk[1] = myvoxels[base + 1] - voxoffset[1];
            ijk[2] = myvoxels[base + 2] - voxoffset[2];
            tempROIVol->setVoxel(ijk, 0, 1.0f);
            output[j] = 0.0;
            double jAvg = 0.0, jStdev = 0.0;
            float* rowReference = dataValues + cols * (mymodels[i].m_indexOffset + j);
            for (int k = 0; k < cols; ++k)
            {
               jAvg += rowReference[k];
            }
            jAvg /= (double)numVoxels;
            for (int k = 0; k < cols; ++k)
            {
               double tempd = rowReference[k] - jAvg;
               jStdev += tempd * tempd;
            }
            jStdev /= (double)numVoxels;//maximum likelihood formula, as opposed to sample standard deviation
            jStdev = sqrt(jStdev);
            means[j] = jAvg;
            stdevs[j] = jStdev;
         }
#ifdef _OPENMP
#pragma omp parallel
#endif
         {
            VolumeFile* processVol, *tempOutVol;
            processVol = new VolumeFile(*tempROIVol);
            tempOutVol = new VolumeFile(*tempROIVol);
#ifdef _OPENMP
#pragma omp for schedule(dynamic, 1)
#endif
            for (unsigned long j = 0; j < numVoxels; ++j)
            {
               correlations[j][j] = 1.0;
               float* jRef = dataValues + cols * (mymodels[i].m_indexOffset + j);
               double jAvg = means[j], jStdev = stdevs[j];
               for (unsigned long k = j + 1; k < numVoxels; ++k)
               {
                  float* kRef = dataValues + cols * (mymodels[i].m_indexOffset + k);
                  double correlation = 0.0;
                  for (int m = 0; m < cols; ++m)
                  {
                     correlation += (jRef[m] - jAvg) * (kRef[m] - means[k]);
                  }
                  correlation /= (double)cols * jStdev * stdevs[k];
                  correlations[j][k] = correlation;
                  correlations[k][j] = correlation;
               }
            }
#ifdef _OPENMP
#pragma omp for
#endif
            for (unsigned long j = 0; j < numVoxels; ++j)
            {
               int ijk[3];
               for (unsigned long k = 0; k < numVoxels; ++k)
               {
                  unsigned long base = k * 3;
                  ijk[0] = myvoxels[base] - voxoffset[0];
                  ijk[1] = myvoxels[base + 1] - voxoffset[1];
                  ijk[2] = myvoxels[base + 2] - voxoffset[2];
                  processVol->setVoxel(ijk, 0, correlations[j][k]);
               }
               BrainModelVolumeROIGradient volgradobj(NULL,
                                                      processVol,
                                                      tempROIVol,
                                                      tempOutVol,
                                                      m_volumeKernel);
               volgradobj.execute();
#ifdef _OPENMP
#pragma omp critical
#endif
               {//don't let two threads try to accumulate at once
                  for (unsigned long k = 0; k < numVoxels; ++k)
                  {
                     unsigned long base = k * 3;
                     ijk[0] = myvoxels[base] - voxoffset[0];
                     ijk[1] = myvoxels[base + 1] - voxoffset[1];
                     ijk[2] = myvoxels[base + 2] - voxoffset[2];
                     output[k] += tempOutVol->getVoxel(ijk, 0);
                  }
               }
            }
#ifdef _OPENMP
#pragma omp for
#endif
            for (unsigned long j = 0; j < numVoxels; ++j)
            {
               outputValues[mymodels[i].m_indexOffset + j] = (float)(output[j] / numVoxels);
            }
            delete processVol;
            delete tempOutVol;
         }
         delete tempROIVol;//*/ //we don't support volumes here yet, so skip it completely
      }
   }
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
   /*CiftiMatrixIndicesMapElement tempMapping;//old code that modified old mapping instead of building a new one
   tempMapping.m_appliesToMatrixDimension.push_back(1);
   tempMapping.m_indicesMapToDataType = CIFTI_INDEX_TYPE_TIME_POINTS;
   tempMapping.m_timeStep = 1.0;
   tempMapping.m_timeStepUnits = NIFTI_UNITS_SEC;
   maparray.push_back(tempMapping);
   dimensions.clear();
   dimensions.push_back(rows);
   dimensions.push_back(1);
   dimensions.push_back(1);
   int leftdim = (int)myLeftIndexList.size(), rightdim = (int)myRightIndexList.size();
   float* outputValues = new float[rows];
   for (int i = 0; i < rows; ++i)
   {
      outputValues[i] = 0.0f;
   }
   for (int i = 0; i < leftdim; ++i)
   {
      outputValues[myLeftIndexList[i] + leftOffset] = myLeftOutput[i];
   }
   for (int i = 0; i < rightdim; ++i)
   {
      outputValues[myRightIndexList[i] + rightOffset] = myRightOutput[i];
   }
   matrix->setMatrixData(outputValues, dimensions);//takes ownership of the pointer and deletes old matrix
   CiftiXML mycxml(myrootXML);
   m_inputCiftiFile->setCiftiXML(mycxml);
   m_inputCiftiFile->setCiftiMatrix(*matrix);//*///because getMatrix sets matrix to NULL for some reason
   
   int leftdim = (int)myLeftIndexList.size(), rightdim = (int)myRightIndexList.size();
   maparray.erase(maparray.begin(), maparray.end());//remove all existing mappings from the XML
   CiftiMatrixIndicesMapElement tempMapping;
   tempMapping.m_appliesToMatrixDimension.push_back(0);
   tempMapping.m_indicesMapToDataType = CIFTI_INDEX_TYPE_BRAIN_MODELS;
   CiftiBrainModelElement tempModel;
   tempModel.m_brainStructure = QString("CIFTI_STRUCTURE_CORTEX_LEFT");
   tempModel.m_indexCount = leftdim;
   tempModel.m_indexOffset = 0;
   tempModel.m_modelType = CIFTI_MODEL_TYPE_SURFACE;
   tempModel.m_nodeIndices = myLeftNodeList;
   tempModel.m_surfaceNumberOfNodes = m_leftSurf->getNumberOfNodes();
   if (m_leftROI != NULL)
   {
      tempMapping.m_brainModels.push_back(tempModel);
   }
   tempModel.m_brainStructure = QString("CIFTI_STRUCTURE_CORTEX_RIGHT");
   tempModel.m_indexCount = rightdim;
   tempModel.m_indexOffset = leftdim;
   tempModel.m_nodeIndices = myRightNodeList;
   tempModel.m_surfaceNumberOfNodes = m_rightSurf->getNumberOfNodes();
   if (m_rightROI != NULL)
   {
      tempMapping.m_brainModels.push_back(tempModel);
   }
   maparray.push_back(tempMapping);
   tempMapping.m_appliesToMatrixDimension[0] = 1;
   tempMapping.m_brainModels.clear();
   tempMapping.m_indicesMapToDataType = CIFTI_INDEX_TYPE_TIME_POINTS;
   tempMapping.m_timeStep = 1.0;
   tempMapping.m_timeStepUnits = NIFTI_UNITS_SEC;
   maparray.push_back(tempMapping);
   CiftiXML mycxml(myrootXML);
   m_inputCiftiFile->setCiftiXML(mycxml);
   dimensions.clear();
   int mydim = leftdim + rightdim;
   dimensions.push_back(mydim);
   dimensions.push_back(1);
   dimensions.push_back(1);
   float* outputValues = new float[mydim];
   for (int i = 0; i < leftdim; ++i)
   {
      outputValues[i] = myLeftOutput[i];
   }
   for (int i = 0; i < rightdim; ++i)
   {
      outputValues[i + leftdim] = myRightOutput[i];
   }
   matrix->setMatrixData(outputValues, dimensions);//takes ownership of the pointer and deletes old matrix
   m_inputCiftiFile->setCiftiMatrix(*matrix);//because getMatrix sets matrix to NULL for some reason
}
