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

#include "BrainModelSurfaceROIMetricGradient.h"
#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VectorFile.h"
#include "MetricFile.h"
#include <cmath>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

void BrainModelSurfaceROIMetricGradient::crossProd(const float in1[3], const double in2[3], double out[3])
{//avoid loops for speed
   out[0] = in1[1] * in2[2] - in1[2] * in2[1];
   out[1] = in1[2] * in2[0] - in1[0] * in2[2];
   out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}

void BrainModelSurfaceROIMetricGradient::crossProd(const double in1[3], const double in2[3], double out[3])
{//avoid loops for speed
   out[0] = in1[1] * in2[2] - in1[2] * in2[1];
   out[1] = in1[2] * in2[0] - in1[0] * in2[2];
   out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}

double BrainModelSurfaceROIMetricGradient::dotProd(const double in1[3], const double in2[3])
{
   return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
}

double BrainModelSurfaceROIMetricGradient::dotProd(const float in1[3], const double in2[3])
{
   return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
}

void BrainModelSurfaceROIMetricGradient::normalize(double in[3])
{
   double mag = sqrt(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);
   in[0] /= mag;
   in[1] /= mag;
   in[2] /= mag;
}

void BrainModelSurfaceROIMetricGradient::coordDiff(const float* coord1, const float* coord2, double out[3])
{
   out[0] = coord1[0] - coord2[0];
   out[1] = coord1[1] - coord2[1];
   out[2] = coord1[2] - coord2[2];
}

void BrainModelSurfaceROIMetricGradient::calcrref(double* matrix[], int rows, int cols)
{//assumes more cols than rows
   int i, j, k, temp;
   double tempd, tempd2;
   for (i = 0; i < rows; ++i)
   {
      tempd = std::abs(matrix[i][i]);//search for pivot
      temp = i;
      for (j = i + 1; j < rows; ++j)
      {
         tempd2 = std::abs(matrix[j][i]);
         if (tempd2 > tempd)
         {
            tempd = tempd2;
            temp = j;
         }
      }
      if (i != temp)
      {
         for (j = i; j < cols; ++j)
         {//skip the waste that will end up 0's and 1's
            tempd = matrix[i][j];
            matrix[i][j] = matrix[temp][j];
            matrix[temp][j] = tempd;
         }
      }
      tempd = matrix[i][i];//pivot
      for (j = i + 1; j < cols; ++j)
      {//again, skip the 0's and 1's
         matrix[i][j] /= tempd;
         for (k = 0; k < i; ++k)
         {
            matrix[k][j] -= matrix[k][i] * matrix[i][j];
         }
         for (++k; k < rows; ++k)
         {
            matrix[k][j] -= matrix[k][i] * matrix[i][j];
         }
      }
   }//rref complete for all cols >= rows, just pretend rowsXrows is I
}

/**
 * Constructor.
 */
BrainModelSurfaceROIMetricGradient::BrainModelSurfaceROIMetricGradient(
                                               BrainSet* bs,
                                               BrainModelSurface* surfaceIn,
                                               MetricFile* roiIn,
                                               MetricFile* valuesIn,
                                               int metricIndexIn,
                                               VectorFile* gradOutIn,
                                               MetricFile* gradMagOutIn,
                                               int magOutIndexIn,
                                               bool avgNormalsIn)
   : BrainModelAlgorithm(bs)
{
   initialize();
   
   m_surface = surfaceIn;
   values     = valuesIn;
   roi = roiIn;
   metricIndex = metricIndexIn;
   gradOut    = gradOutIn;
   gradMagOut = gradMagOutIn;
   magOutIndex = magOutIndexIn;
   avgNormals = avgNormalsIn;
   
}
                                      
/**
 * Constructor for execution of all columns.
 */
BrainModelSurfaceROIMetricGradient::BrainModelSurfaceROIMetricGradient(BrainSet* bs,
                                  BrainModelSurface* surfaceIn,
                                  MetricFile* roiIn,
                                  MetricFile* valuesIn,
                                  bool avgNormalsIn,
                                  bool parallelFlagIn) 
   : BrainModelAlgorithm(bs)
{
   initialize();
   
   m_surface = surfaceIn;
   roi = roiIn;
   values     = valuesIn;
   gradMagOut = valuesIn;
   avgNormals = avgNormalsIn;   
   allColumnsFlag = true;
   parallelFlag = parallelFlagIn;
}

/**
 * Destructor.
 */
BrainModelSurfaceROIMetricGradient::~BrainModelSurfaceROIMetricGradient()
{
}

void 
BrainModelSurfaceROIMetricGradient::initialize()
{
   allColumnsFlag = false;
   parallelFlag = false;
   haveWarned = false;
   haveFailed = false;
}
                                            
/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceROIMetricGradient::execute() throw (BrainModelAlgorithmException)
{
   if (allColumnsFlag) {
       executeAllColumns();
   }
   else {
       executeSingleColumn();
   }
}

/**
 * execute all the metric columns
 */
void 
BrainModelSurfaceROIMetricGradient::executeAllColumns() throw (BrainModelAlgorithmException)
{
   //
   // Verify files exist, are valid, etc.
   //
   CoordinateFile* source = m_surface->getCoordinateFile();//reference
   TopologyFile* topo = m_surface->getTopologyFile();
   const TopologyHelper* myhelper = topo->getTopologyHelper(false, true, false);
   m_surface->computeNormals(NULL, avgNormals);
   m_surface->orientNormalsOut();
   if (source == NULL) {
      throw BrainModelAlgorithmException("Invalid coordinate file.");
   }
   if (values == NULL) {
      throw BrainModelAlgorithmException("Invalid metric file.");
   }
   if (roi == NULL) {
      throw BrainModelAlgorithmException("Invalid roi file.");
   }
   if (source->getNumberOfCoordinates() < 1)
   {
      throw BrainModelAlgorithmException("Not enough nodes in coordinate file.");
   }
   if (source->getNumberOfCoordinates() != values->getNumberOfNodes())
   {
      throw BrainModelAlgorithmException("Coordinate file contains different number of nodes than metric file.");
   }
   if (source->getNumberOfCoordinates() != roi->getNumberOfNodes())
   {
      throw BrainModelAlgorithmException("Coordinate file contains different number of nodes than ROI file.");
   } 

   int numNodes = source->getNumberOfCoordinates();
   int i, j;
   float* sourceData = new float[numNodes * 3];
   source->getAllCoordinates(sourceData);
   float* roiValues = new float[numNodes];
   float* magData = NULL;

   std::vector<int> neighbors;
   magData = new float[numNodes];
   
   roi->getColumnForAllNodes(0,roiValues);
   float* allnormals = new float[numNodes * 3];
   for (i = 0; i < numNodes; ++i)
   {
      j = i * 3;
      const float* mynormal = m_surface->getNormal(i);
      allnormals[j] = mynormal[0];
      allnormals[j + 1] = mynormal[1];
      allnormals[j + 2] = mynormal[2];
   }
   /*if (avgNormals) //TSC: this is taken care of by BrainModelSurface::computeNormals(NULL, true)
   {//assume normals have identical length
      for (i = 0; i < numNodes; ++i)
      {
         const float* mynormal = m_surface->getNormal(i);
         if(roiValues[i] == 0.0f) continue;
         //should these be filtered or not?,  I'm thinking for now that
         //normals should not be filtered
         
         myhelper->getNodeNeighbors(i, neighbors);
         numNeigh = neighbors.size();
         for (j = 0; j < numNeigh; ++j)
         {
               k = neighbors[j] * 3;
               allnormals[k] += mynormal[0];
               allnormals[k + 1] += mynormal[1];
               allnormals[k + 2] += mynormal[2];
         }
      }
   }//WARNING: normals are not normalized, nor identical length - below algorithm normalizes after every use of a normal though   
*/
   int numColumns = values->getNumberOfColumns();
   if (parallelFlag) {
      #pragma omp parallel for
      for (int i = 0; i < numColumns; i++) {         
         processSingleColumn(myhelper,
                             allnormals,
                             sourceData,
                             roiValues,
                             i,
                             numNodes);
      }
   }
   else {
      for (int i = 0; i < numColumns; i++) {
         processSingleColumn(myhelper,
                             allnormals,                             
                             sourceData,
                             roiValues,
                             i,
                             numNodes);
      }
   }
   delete[] allnormals;
   delete[] roiValues;
   delete[] sourceData;

}
/**
 * process single column.
 */
void 
BrainModelSurfaceROIMetricGradient::processSingleColumn(const TopologyHelper* myhelper,
                                                     const float* allnormals, 
                                                     const float* sourceData,
                                                     const float* roiValues,
                                                     const int columnIndex,
                                                     const int numNodes) throw (BrainModelAlgorithmException)
{   

   int i, j, k, numNeigh, whichnode;
   double xhat[3], yhat[3], somevec[3];
   float nodemetric;
   double xmag, ymag,tempd, sanity;
   double* rrefb[3];
   double *savedVec = new double[numNodes * 3];
   float* metricData = new float[numNodes];
   float* magData = new float[numNodes];
   
   std::vector<int> neighbors;


   values->getColumnForAllNodes(columnIndex, metricData);
   rrefb[0] = new double[4];
   rrefb[1] = new double[4];
   rrefb[2] = new double[4];   
   
   //std::vector<int> RejectedNodes; //Nodes that for whatever reason were throw out of the calculation (too few neighbors, etc.)
   for (i = 0; i < numNodes; ++i)
   {
      if(roiValues[i] == 0.0f)
      {
         savedVec[i*3]=0.0;
         savedVec[i*3+1]=0.0;
         savedVec[i*3+2]=0.0;
         
         magData[i] = 0.0f;        
         continue;
      }
      const float* mynormal = allnormals + i * 3;
      somevec[2] = 0.0;
      if (mynormal[0] > mynormal[1])
      {//generate a vector not parallel to normal
         somevec[0] = 0.0;
         somevec[1] = 1.0;
      } else {
         somevec[0] = 1.0;
         somevec[1] = 0.0;
      }
      crossProd(mynormal, somevec, xhat);
      normalize(xhat);//use cross products to generate a 2d coordinate system orthogonal to normal
      crossProd(mynormal, xhat, yhat);
      normalize(yhat);//xhat, yhat are orthogonal unit vectors describing the coord system with k = surface normal
      for (j = 0; j < 4; ++j)
      {
         rrefb[0][j] = 0.0;
         rrefb[1][j] = 0.0;
         rrefb[2][j] = 0.0;
      }
      nodemetric = metricData[i];//uses metric difference for 2 reasons: makes rref cumulate smaller values, and makes root node not matter to calculation
      //use function below to filter node neighbors based on ROI
      myhelper->getNodeNeighborsInROI(i, neighbors,roiValues);//intelligently detects depth == 1 (and invalid depth)
      numNeigh = neighbors.size();
      /*unsigned int MinimumNeighbors = 3;//let fallback handle this by calculating gradient vectors to neighbors and averaging
      if(neighbors.size() < MinimumNeighbors)
      {
         //cout << "node number is " << i << endl;
         //cout << "neighbor count is " << neighbors.size() << endl;
         RejectedNodes.push_back(i);
         continue;       
      }*/
      if (numNeigh >= 2)//dont attempt regression if the system is underdetermined
      {
         for (j = 0; j < numNeigh; ++j)
         {
            whichnode = neighbors[j];
            tempd = metricData[whichnode] - nodemetric;//metric difference
            coordDiff(&sourceData[whichnode * 3], &sourceData[i * 3], somevec);//position delta vector
            xmag = dotProd(xhat, somevec);//project to 2d plane tangent to surface
            ymag = dotProd(yhat, somevec);
            rrefb[0][0] += xmag * xmag;//gather A'A and A'b sums for regression
            rrefb[0][1] += xmag * ymag;
            rrefb[0][2] += xmag;
            rrefb[1][1] += ymag * ymag;
            rrefb[1][2] += ymag;
            rrefb[2][2] += 1.0f;
            rrefb[0][3] += xmag * tempd;
            rrefb[1][3] += ymag * tempd;
            rrefb[2][3] += tempd;
         }
         rrefb[1][0] = rrefb[0][1];//complete the symmetric elements
         rrefb[2][0] = rrefb[0][2];
         rrefb[2][1] = rrefb[1][2];
         rrefb[2][2] += 1.0f;//include center (metric and coord differences will be zero, so this is all that is needed)
         calcrref(rrefb, 3, 4);
         somevec[0] = xhat[0] * rrefb[0][3] + yhat[0] * rrefb[1][3];
         somevec[1] = xhat[1] * rrefb[0][3] + yhat[1] * rrefb[1][3];
         somevec[2] = xhat[2] * rrefb[0][3] + yhat[2] * rrefb[1][3];//somevec is now our surface gradient
         sanity = (float)(somevec[0] + somevec[1] + somevec[2]);
      }
      if (numNeigh < 2 || !(sanity == sanity))
      {
      	//expect this to happen with ROI gradient
         //if (!haveWarned) std::cerr << "WARNING: gradient calculation found a NaN/inf with regression method" << endl;
         //haveWarned = true;
         if (numNeigh != 0)
         {
            somevec[0] = 0.0;
            somevec[1] = 0.0;
            somevec[2] = 0.0;
            for (j = 0; j < numNeigh; ++j)
            {
               whichnode = neighbors[j];
               coordDiff(&sourceData[whichnode * 3], &sourceData[i * 3], xhat);//xhat repurposed as a temp variable
               for (k = 0; k < 3; ++k)
                  yhat[k] = xhat[k];
               normalize(xhat);
               tempd = (metricData[whichnode] - nodemetric) / sqrt(yhat[0] * yhat[0] + yhat[1] * yhat[1] + yhat[2] * yhat[2]);//difference over distance gives gradient magnitude
               for (k = 0; k < 3; ++k)
                  somevec[k] += tempd * xhat[k];//magnitude times unit vector parallel to distance vector gives crude estimate
            }
            for (j = 0; j < 3; ++j)
            {
               somevec[j] /= numNeigh;//now we have the 3d gradient, time to subtract the dot product with normal
               xhat[j] = mynormal[j];
            }
            normalize(xhat);//for sanity, in case your normals aren't unit vectors somehow
            tempd = dotProd(somevec, xhat);
            sanity = 0.0;
            for (j = 0; j < 3; ++j)
               sanity += (float)((somevec[j] -= tempd * xhat[j]));
         }
         if (numNeigh == 0 || !(sanity == sanity))
         {
            if (!haveFailed)
            {
               std::cerr << "WARNING: gradient calculation found a NaN/inf with fallback method, outputting ZERO" << endl;
               std::cerr << "check your coordinate/topo files for isolated nodes and nodes with identical coords" << endl;
            }
            haveFailed = true;
            somevec[0] = 0.0;
            somevec[1] = 0.0;
            somevec[2] = 0.0;
         }
      }
      //saveVec
      savedVec[i*3]=somevec[0];
      savedVec[i*3+1]=somevec[1];
      savedVec[i*3+2]=somevec[2];      
      
      magData[i] = (float)sqrt(somevec[0] * somevec[0] + somevec[1] * somevec[1] + somevec[2] * somevec[2]);
      
   }
   //For nodes that have less than 3 neighbors, we calculate the average gradient vector of that node's neighbors
   //This gives us a crude estimate of what that node's gradient vector should be.
   /*for(unsigned int i = 0;i<RejectedNodes.size();i++)
   {
      //In the case of nodes with a small number of neighbors, the gradient magnitude and size are the gaussian weighted average
      //of the Neighbors' vector and magnitude
      //currently it's just the average, look into making this gaussian weighted (weighting probably isn't necessary since
      //we're dealing with first order neighbors
      //This relies on the fact that nodes with less than 3 neighbors are always connected to nodes with more than two neighbors
      //in the ROI (and as a result, have a proper vector and magnitude).
      int Node = RejectedNodes[i];
      myhelper->getNodeNeighborsInROI(Node, neighbors,roiValues);
   
      double averageVec[3] = { 0.0, 0.0, 0.0 };
      //calculate average Vector
      
      if (neighbors.size() > 0)
      {
         for(unsigned int j = 0;j<neighbors.size();j++)
         {  
            averageVec[0] += savedVec[neighbors[j]*3];
            averageVec[1] += savedVec[neighbors[j]*3+1];
            averageVec[2] += savedVec[neighbors[j]*3+2];
         }
         averageVec[0] /= (double)neighbors.size();
         averageVec[1] /= (double)neighbors.size(); 
         averageVec[2] /= (double)neighbors.size(); 
         savedVec[Node*3]=averageVec[0];
         savedVec[Node*3+1]=averageVec[1];
         savedVec[Node*3+2]=averageVec[2];
      
         magData[Node] = (float) sqrt(averageVec[0] * averageVec[0] + averageVec[1] * averageVec[1] + averageVec[2] * averageVec[2]);
      } else {
         savedVec[Node * 3] = 0.0;
         savedVec[Node * 3 + 1] = 0.0;
         savedVec[Node * 3 + 2] = 0.0;
         magData[Node] = 0.0f;
      }
   }*/ //don't use this, the fallback method already calculates via neighbors, zero neighbors already outputs zero

   gradMagOut->setColumnForAllNodes(columnIndex, magData);
   gradMagOut->setColumnName(columnIndex, "surface gradient");   
   delete[] savedVec;
   delete[] magData;
   delete[] metricData;
   delete[] rrefb[0];
   delete[] rrefb[1];
   delete[] rrefb[2];

}

/**
 * execute the algorithm on a single metric column.
 */
void 
BrainModelSurfaceROIMetricGradient::executeSingleColumn() throw (BrainModelAlgorithmException)
{
   //
   // Verify files exist, are valid, etc.
   //
   CoordinateFile* source = m_surface->getCoordinateFile();//reference
   TopologyFile* topo = m_surface->getTopologyFile();
   const TopologyHelper* myhelper = topo->getTopologyHelper(false, true, false);
   m_surface->computeNormals();
   m_surface->orientNormalsOut();
   if (source == NULL) {
      throw BrainModelAlgorithmException("Invalid coordinate file.");
   }
   if (values == NULL) {
      throw BrainModelAlgorithmException("Invalid metric file.");
   }
   if (roi == NULL) {
      throw BrainModelAlgorithmException("Invalid roi file.");
   }
   if (source->getNumberOfCoordinates() < 1)
   {
      throw BrainModelAlgorithmException("Not enough nodes in coordinate file.");
   }
   if (source->getNumberOfCoordinates() != values->getNumberOfNodes())
   {
      throw BrainModelAlgorithmException("Coordinate file contains different number of nodes than metric file.");
   }
   if (source->getNumberOfCoordinates() != roi->getNumberOfNodes())
   {
      throw BrainModelAlgorithmException("Coordinate file contains different number of nodes than ROI file.");
   }
   //
   // Check output files exist
   //
   bool saveMag = true, saveVec = true;
   if (gradMagOut == NULL) {
      saveMag = false;
   } else {
      if (source->getNumberOfCoordinates() != gradMagOut->getNumberOfNodes())
      {
         gradMagOut->setNumberOfNodesAndColumns(source->getNumberOfCoordinates(), 1);//clear existing data because node numbers don't match
         magOutIndex = 0;
      }
      if (magOutIndex < 0 || magOutIndex >= gradMagOut->getNumberOfColumns())
      {
         magOutIndex = gradMagOut->getNumberOfColumns();
         gradMagOut->addColumns(1);
      }
      gradMagOut->setColumnName(magOutIndex, "surface gradient");
   }
   if (gradOut == NULL) {
      saveVec = false;
      gradOut = new VectorFile();//need to create one anyway as a place to store vectors
      gradOut->setNumberOfVectors(source->getNumberOfCoordinates());
   } else {
      if (source->getNumberOfCoordinates() != gradOut->getNumberOfVectors())
      {
         gradOut->setNumberOfVectors(source->getNumberOfCoordinates());
      }
   }
   if (!saveMag && !saveVec)
   {
      throw BrainModelAlgorithmException("No valid output file.");
   }
   int numNodes = source->getNumberOfCoordinates();
   int i, j, k, numNeigh, whichnode;
   double xhat[3], yhat[3], somevec[3];
   float nodemetric, somevecf[3];
   double xmag, ymag,tempd, sanity;
   double* rrefb[3];
   float* sourceData = new float[numNodes * 3];
   source->getAllCoordinates(sourceData);
   float* metricData = new float[numNodes];
   float* roiValues = new float[numNodes];
   float* magData = NULL;
   std::vector<int> neighbors;
   if (saveMag)
   {
      magData = new float[numNodes];
   }
   values->getColumnForAllNodes(metricIndex, metricData);
   roi->getColumnForAllNodes(0,roiValues);
   rrefb[0] = new double[4];
   rrefb[1] = new double[4];
   rrefb[2] = new double[4];
   float* allnormals = new float[numNodes * 3];
   for (i = 0; i < numNodes; ++i)
   {
      j = i * 3;
      const float* mynormal = m_surface->getNormal(i);
      allnormals[j] = mynormal[0];
      allnormals[j + 1] = mynormal[1];
      allnormals[j + 2] = mynormal[2];
   }
   if (avgNormals)
   {//assume normals have identical length
      for (i = 0; i < numNodes; ++i)
      {
         const float* mynormal = m_surface->getNormal(i);
         if(roiValues[i] == 0.0f) continue;
         //should these be filtered or not?,  I'm thinking for now that
         //normals should not be filtered
         
         myhelper->getNodeNeighbors(i, neighbors);
         numNeigh = neighbors.size();
         for (j = 0; j < numNeigh; ++j)
         {
               k = neighbors[j] * 3;
               allnormals[k] += mynormal[0];
               allnormals[k + 1] += mynormal[1];
               allnormals[k + 2] += mynormal[2];
         }
      }
   }//WARNING: normals are not normalized, nor identical length - below algorithm normalizes after every use of a normal though
   std::vector<int> RejectedNodes; //Nodes that for whatever reason were throw out of the calculation (too few neighbors, etc.)
   for (i = 0; i < numNodes; ++i)
   {
      if(roiValues[i] == 0.0f)
      {
         //saveVec
         gradOut->setVectorOrigin(i, &sourceData[i * 3]);
         const float xyzVectorIn[3] = {0.0f,0.0f, 0.0f};
         gradOut->setVectorUnitComponents(i, xyzVectorIn);//gradient complete!
         
         if (saveMag)
         {
            magData[i] = 0.0f;
         }
         continue;
      }
      float* mynormal = allnormals + i * 3;
      somevec[2] = 0.0;
      if (mynormal[0] > mynormal[1])
      {//generate a vector not parallel to normal
         somevec[0] = 0.0;
         somevec[1] = 1.0;
      } else {
         somevec[0] = 1.0;
         somevec[1] = 0.0;
      }
      crossProd(mynormal, somevec, xhat);
      normalize(xhat);//use cross products to generate a 2d coordinate system orthogonal to normal
      crossProd(mynormal, xhat, yhat);
      normalize(yhat);//xhat, yhat are orthogonal unit vectors describing the coord system with k = surface normal
      for (j = 0; j < 4; ++j)
      {
         rrefb[0][j] = 0.0;
         rrefb[1][j] = 0.0;
         rrefb[2][j] = 0.0;
      }
      nodemetric = metricData[i];//uses metric difference for 2 reasons: makes rref cumulate smaller values, and makes root node not matter to calculation
      //use function below to filter node neighbors based on ROI
      myhelper->getNodeNeighborsInROI(i, neighbors,roiValues);//intelligently detects depth == 1 (and invalid depth)
      numNeigh = neighbors.size();
      /*unsigned int MinimumNeighbors = 3;//let fallback handle this by calculating gradient vectors to neighbors and averaging
      if(neighbors.size() < MinimumNeighbors)
      {
         //cout << "node number is " << i << endl;
         //cout << "neighbor count is " << neighbors.size() << endl;
         RejectedNodes.push_back(i);
         continue;       
      }*/
      if (numNeigh >= 2)//dont attempt regression if the system is underdetermined
      {
         for (j = 0; j < numNeigh; ++j)
         {
            whichnode = neighbors[j];
            tempd = metricData[whichnode] - nodemetric;//metric difference
            coordDiff(&sourceData[whichnode * 3], &sourceData[i * 3], somevec);//position delta vector
            xmag = dotProd(xhat, somevec);//project to 2d plane tangent to surface
            ymag = dotProd(yhat, somevec);
            rrefb[0][0] += xmag * xmag;//gather A'A and A'b sums for regression
            rrefb[0][1] += xmag * ymag;
            rrefb[0][2] += xmag;
            rrefb[1][1] += ymag * ymag;
            rrefb[1][2] += ymag;
            rrefb[2][2] += 1.0f;
            rrefb[0][3] += xmag * tempd;
            rrefb[1][3] += ymag * tempd;
            rrefb[2][3] += tempd;
         }
         rrefb[1][0] = rrefb[0][1];//complete the symmetric elements
         rrefb[2][0] = rrefb[0][2];
         rrefb[2][1] = rrefb[1][2];
         rrefb[2][2] += 1.0f;//include center (metric and coord differences will be zero, so this is all that is needed)
         calcrref(rrefb, 3, 4);
         somevecf[0] = (float)(somevec[0] = xhat[0] * rrefb[0][3] + yhat[0] * rrefb[1][3]);
         somevecf[1] = (float)(somevec[1] = xhat[1] * rrefb[0][3] + yhat[1] * rrefb[1][3]);
         somevecf[2] = (float)(somevec[2] = xhat[2] * rrefb[0][3] + yhat[2] * rrefb[1][3]);//somevec is now our surface gradient
         sanity = somevec[0] + somevec[1] + somevec[2];
      }

      if (numNeigh < 2 || !(sanity == sanity))
      {
      	//expect this to happen with ROI gradient
         //if (!haveWarned) std::cerr << "WARNING: gradient calculation found a NaN/inf with regression method" << endl;
         //haveWarned = true;
         if (numNeigh != 0)
         {
            somevecf[0] = 0.0f;
            somevecf[1] = 0.0f;
            somevecf[2] = 0.0f;
            somevec[0] = 0.0;
            somevec[1] = 0.0;
            somevec[2] = 0.0;
            for (j = 0; j < numNeigh; ++j)
            {
               whichnode = neighbors[j];
               coordDiff(&sourceData[whichnode * 3], &sourceData[i * 3], xhat);//xhat repurposed as a temp variable
               for (k = 0; k < 3; ++k)
                  yhat[k] = xhat[k];
               normalize(xhat);
               tempd = (metricData[whichnode] - nodemetric) / sqrt(yhat[0] * yhat[0] + yhat[1] * yhat[1] + yhat[2] * yhat[2]);//difference over distance gives gradient magnitude
               for (k = 0; k < 3; ++k)
                  somevec[k] += tempd * xhat[k];//magnitude times unit vector parallel to distance vector gives crude estimate
            }
            for (j = 0; j < 3; ++j)
            {
               somevec[j] /= numNeigh;//now we have the 3d gradient, time to subtract the dot product with normal
               xhat[j] = mynormal[j];
            }
            normalize(xhat);//for sanity, in case your normals aren't unit vectors somehow
            tempd = dotProd(somevec, xhat);
            sanity = 0.0;
            for (j = 0; j < 3; ++j)
               sanity += (somevecf[j] = (float)(somevec[j] -= tempd * xhat[j]));
         }
         if (numNeigh == 0 || !(sanity == sanity))
         {
            if (!haveFailed)
            {
               std::cerr << "WARNING: gradient calculation found a NaN/inf with fallback method, outputting ZERO" << endl;
               std::cerr << "check your coordinate/topo files for isolated nodes and nodes with identical coords" << endl;
            }
            haveFailed = true;
            somevecf[0] = 0.0f;
            somevecf[1] = 0.0f;
            somevecf[2] = 0.0f;
            somevec[0] = 0.0;
            somevec[1] = 0.0;
            somevec[2] = 0.0;
         }
      }
      //saveVec
      gradOut->setVectorOrigin(i, &sourceData[i * 3]);
      gradOut->setVectorUnitComponents(i, somevecf);//gradient complete!
      
      if (saveMag)
      {
         magData[i] = (float)sqrt(somevec[0] * somevec[0] + somevec[1] * somevec[1] + somevec[2] * somevec[2]);
      }
   }
   //For nodes that have less than 3 neighbors, we calculate the average gradient vector of that node's neighbors
   //This gives us a crude estimate of what that node's gradient vector should be.
   /*for(unsigned int i = 0;i<RejectedNodes.size();i++)
   {
      //In the case of nodes with a small number of neighbors, the gradient magnitude and size are the gaussian weighted average
      //of the Neighbors' vector and magnitude
      //currently it's just the average, look into making this gaussian weighted (weighting probably isn't necessary since
      //we're dealing with first order neighbors
      //This relies on the fact that nodes with less than 3 neighbors are always connected to nodes with more than two neighbors
      //in the ROI (and as a result, have a proper vector and magnitude).
      int Node = RejectedNodes[i];
      myhelper->getNodeNeighborsInROI(Node, neighbors,roiValues);
   
      double averageVec[3] = { 0.0, 0.0, 0.0 };
      //calculate average Vector
      
      if (neighbors.size() > 0)
      {
         for(unsigned int j = 0;j<neighbors.size();j++)
         {  
            averageVec[0] += savedVec[neighbors[j]*3];
            averageVec[1] += savedVec[neighbors[j]*3+1];
            averageVec[2] += savedVec[neighbors[j]*3+2];
         }
         averageVec[0] /= (double)neighbors.size();
         averageVec[1] /= (double)neighbors.size(); 
         averageVec[2] /= (double)neighbors.size(); 
         savedVec[Node*3]=averageVec[0];
         savedVec[Node*3+1]=averageVec[1];
         savedVec[Node*3+2]=averageVec[2];
      } else {
         savedVec[Node * 3] = 0.0;
         savedVec[Node * 3 + 1] = 0.0;
         savedVec[Node * 3 + 2] = 0.0;
      }
      if (saveMag)
      {
         magData[Node] = (float)sqrt(averageVecf[0] * averageVecf[0] + averageVecf[1] * averageVecf[1] + averageVecf[2] * averageVecf[2]);
      }
   }*/ //don't use this, the fallback method already calculates via neighbors, zero neighbors already outputs zero
   if (saveMag)
   {
      gradMagOut->setColumnForAllNodes(magOutIndex, magData);
      delete[] magData;
   }
   
   if(!saveVec) delete gradOut;
   delete[] sourceData;
   delete[] metricData;
   delete[] rrefb[0];
   delete[] rrefb[1];
   delete[] rrefb[2];
   delete[] allnormals;
}                        


