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

#include "BrainModelSurfaceMetricGradient.h"
#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VectorFile.h"
#include "MetricFile.h"
#include <cmath>
#include <iostream>

void BrainModelSurfaceMetricGradient::crossProd(const float in1[3], const double in2[3], double out[3])
{//avoid loops for speed
   out[0] = in1[1] * in2[2] - in1[2] * in2[1];
   out[1] = in1[2] * in2[0] - in1[0] * in2[2];
   out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}

void BrainModelSurfaceMetricGradient::crossProd(const double in1[3], const double in2[3], double out[3])
{//avoid loops for speed
   out[0] = in1[1] * in2[2] - in1[2] * in2[1];
   out[1] = in1[2] * in2[0] - in1[0] * in2[2];
   out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}

double BrainModelSurfaceMetricGradient::dotProd(const double in1[3], const double in2[3])
{
   return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
}

double BrainModelSurfaceMetricGradient::dotProd(const float in1[3], const double in2[3])
{
   return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
}

void BrainModelSurfaceMetricGradient::normalize(double in[3])
{
   double mag = sqrt(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);
   in[0] /= mag;
   in[1] /= mag;
   in[2] /= mag;
}

void BrainModelSurfaceMetricGradient::coordDiff(const float* coord1, const float* coord2, double out[3])
{
   out[0] = coord1[0] - coord2[0];
   out[1] = coord1[1] - coord2[1];
   out[2] = coord1[2] - coord2[2];
}

void BrainModelSurfaceMetricGradient::calcrref(double* matrix[], int rows, int cols)
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

double BrainModelSurfaceMetricGradient::det3(double* matrix[], int column)
{//diagonals trick for 3x3, unrolled
   int column1 = column + 1, column2 = column + 2;
   double ret = matrix[0][column] * matrix[1][column1] * matrix[2][column2];
   ret += matrix[1][column] * matrix[2][column1] * matrix[0][column2];
   ret += matrix[2][column] * matrix[0][column1] * matrix[1][column2];
   ret -= matrix[2][column] * matrix[1][column1] * matrix[0][column2];
   ret -= matrix[1][column] * matrix[0][column1] * matrix[2][column2];
   ret -= matrix[0][column] * matrix[2][column1] * matrix[1][column2];
   return ret;
}

/**
 * Constructor.
 */
BrainModelSurfaceMetricGradient::BrainModelSurfaceMetricGradient(
                                               BrainSet* bs,
                                               int bsIndexIn,
                                               MetricFile* valuesIn,
                                               int metricIndexIn,
                                               VectorFile* gradOutIn,
                                               MetricFile* gradMagOutIn,
                                               int magOutIndexIn,
                                               bool avgNormalsIn)
   : BrainModelAlgorithm(bs)
{
   setIndex   = bsIndexIn;
   values     = valuesIn;
   metricIndex = metricIndexIn;
   gradOut    = gradOutIn;
   gradMagOut = gradMagOutIn;
   magOutIndex = magOutIndexIn;
   avgNormals = avgNormalsIn;
}
                                      
/**
 * Destructor.
 */
BrainModelSurfaceMetricGradient::~BrainModelSurfaceMetricGradient()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricGradient::execute() throw (BrainModelAlgorithmException)
{
   //
   // Verify files exist, are valid, etc.
   //
   BrainModelSurface* mysurf = brainSet->getBrainModelSurface(setIndex);//reference
   CoordinateFile* source = mysurf->getCoordinateFile();//reference
   TopologyFile* topo = mysurf->getTopologyFile();
   const TopologyHelper* myhelper = topo->getTopologyHelper(false, true, false);
   mysurf->computeNormals();
   mysurf->orientNormalsOut();
   if (source == NULL) {
      throw BrainModelAlgorithmException("Invalid coordinate file.");
   }
   if (values == NULL) {
      throw BrainModelAlgorithmException("Invalid metric file.");
   }
   if (source->getNumberOfCoordinates() < 1)
   {
      throw BrainModelAlgorithmException("Not enough nodes in coordinate file.");
   }
   if (source->getNumberOfCoordinates() != values->getNumberOfNodes())
   {
      throw BrainModelAlgorithmException("Node numbers do not match.");
   }
   //
   // Check output files exist
   //
   bool saveMag = true, saveVec = true, haveWarned = false, haveFailed = false;
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
   float* magData = NULL;
   std::vector<int> neighbors;
   if (saveMag)
   {
      magData = new float[numNodes];
   }
   values->getColumnForAllNodes(metricIndex, metricData);
   rrefb[0] = new double[4];
   rrefb[1] = new double[4];
   rrefb[2] = new double[4];
   float* allnormals = new float[numNodes * 3];
   for (i = 0; i < numNodes; ++i)
   {
      j = i * 3;
      const float* mynormal = mysurf->getNormal(i);
      allnormals[j] = mynormal[0];
      allnormals[j + 1] = mynormal[1];
      allnormals[j + 2] = mynormal[2];
   }
   if (avgNormals)
   {//assume normals have identical length
      for (i = 0; i < numNodes; ++i)
      {
         const float* mynormal = mysurf->getNormal(i);
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
   for (i = 0; i < numNodes; ++i)
   {
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
      myhelper->getNodeNeighbors(i, neighbors);//intelligently detects depth == 1 (and invalid depth)
      numNeigh = neighbors.size();
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
      calcrref(rrefb, 3, 4);
      somevecf[0] = (float)(somevec[0] = xhat[0] * rrefb[0][3] + yhat[0] * rrefb[1][3]);
      somevecf[1] = (float)(somevec[1] = xhat[1] * rrefb[0][3] + yhat[1] * rrefb[1][3]);
      somevecf[2] = (float)(somevec[2] = xhat[2] * rrefb[0][3] + yhat[2] * rrefb[1][3]);//somevec is now our surface gradient
      sanity = somevec[0] + somevec[1] + somevec[2];
      if (sanity != sanity)
      {
         if (!haveWarned) std::cerr << "WARNING: gradient calculation found a NaN/inf with regression method" << endl;
         haveWarned = true;
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
            tempd = (metricData[whichnode] - nodemetric) / (yhat[0] * yhat[0] + yhat[1] * yhat[1] + yhat[2] * yhat[2]);//difference over distance gives gradient magnitude
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
         if (sanity != sanity)
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
      if (saveVec)
      {
         gradOut->setVectorOrigin(i, &sourceData[i * 3]);
         gradOut->setVectorUnitComponents(i, somevecf);//gradient complete!
      }
      if (saveMag)
      {
         magData[i] = (float)sqrt(somevec[0] * somevec[0] + somevec[1] * somevec[1] + somevec[2] * somevec[2]);
      }
   }
   if (saveMag)
   {
      gradMagOut->setColumnForAllNodes(magOutIndex, magData);
      delete[] magData;
   }
   delete[] sourceData;
   delete[] metricData;
   delete[] rrefb[0];
   delete[] rrefb[1];
   delete[] rrefb[2];
   delete[] allnormals;
}
