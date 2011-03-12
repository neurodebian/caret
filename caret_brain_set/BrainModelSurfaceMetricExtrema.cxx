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

#include "BrainModelVolume.h"
#include "BrainModelSurfaceMetricExtrema.h"
#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "MetricFile.h"
#include <cmath>
#include <iostream>

/**
 * Constructor.
 */
BrainModelSurfaceMetricExtrema::BrainModelSurfaceMetricExtrema(
                                               BrainSet* bs,
                                               int bsIndexIn,
                                               MetricFile* valuesIn,
                                               int metricIndexIn,
                                               MetricFile* extremaOutIn,
                                               int exOutIndexIn,
                                               int depthIn)
   : BrainModelAlgorithm(bs)
{
   setIndex   = bsIndexIn;
   values     = valuesIn;
   metricIndex = metricIndexIn;
   extremaOut    = extremaOutIn;
   exOutIndex = exOutIndexIn;
   depth = depthIn;
}
                                      
/**
 * Destructor.
 */
BrainModelSurfaceMetricExtrema::~BrainModelSurfaceMetricExtrema()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceMetricExtrema::execute() throw (BrainModelAlgorithmException)
{
   //
   // Verify files exist, are valid, etc.
   //
   BrainModelSurface* mysurf = brainSet->getBrainModelSurface(setIndex);//reference
   CoordinateFile* source = mysurf->getCoordinateFile();//reference
   TopologyFile* topo = mysurf->getTopologyFile();
   const TopologyHelper* myhelper = topo->getTopologyHelper(false, true, false);
   if (depth < 1)
   {
      depth = 1;
   }
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
   if (extremaOut == NULL) {
      throw BrainModelAlgorithmException("No valid output file.");
   }
   if (extremaOut->getNumberOfNodes() != source->getNumberOfCoordinates())
   {
      extremaOut->setNumberOfNodesAndColumns(source->getNumberOfCoordinates(), 1);
      exOutIndex = 0;
   }
   if (exOutIndex < 0 || exOutIndex >= extremaOut->getNumberOfColumns())
   {
      exOutIndex = extremaOut->getNumberOfColumns();
      extremaOut->addColumns(1);
   }
   extremaOut->setColumnName(exOutIndex, QString("extrema depth=") + QString::number(depth));
   int numNodes = source->getNumberOfCoordinates();
   int i, j, numNeigh, whichnode;
   float nodemetric;
   float* sourceData = new float[numNodes * 3];
   source->getAllCoordinates(sourceData);
   float* metricData = new float[numNodes];
   values->getColumnForAllNodes(metricIndex, metricData);
   std::vector<int> neighbors;
   float* exData = new float[numNodes];
   int* markmax = new int[numNodes], *markmin = new int[numNodes];
   for (i = 0; i < numNodes; ++i)
   {
      exData[i] = 0.0f;
      markmax[i] = 0;
      markmin[i] = 0;
   }
   bool isMax, isMin;
   for (i = 0; i < numNodes; ++i)
   {
      isMax = !markmax[i];//skip previously compared nodes for extra speed
      isMin = !markmin[i];//also allows it to not request neighbors, even more speed
      if (isMax || isMin)
      {
         nodemetric = metricData[i];
         myhelper->getNodeNeighborsToDepth(i, depth, neighbors);
         numNeigh = neighbors.size();
         for (j = 0; (isMax || isMin) && j < numNeigh; ++j)
         {
            whichnode = neighbors[j];
            if (nodemetric < metricData[whichnode])
            {
               isMax = false;
               markmin[whichnode] = 1;
            }
            if (nodemetric > metricData[whichnode])
            {
               isMin = false;
               markmax[whichnode] = 1;
            }
         }
         if (isMin)
         {
            exData[i] = -1.0f;
         }
         if (isMax)
         {
            exData[i] = 1.0f;
         }
      }
   }
   extremaOut->setColumnForAllNodes(exOutIndex, exData);
   delete[] exData;
   delete[] sourceData;
   delete[] metricData;
   delete[] markmax;
   delete[] markmin;
}
