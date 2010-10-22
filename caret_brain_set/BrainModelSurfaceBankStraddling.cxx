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
#include "BrainModelSurfaceBankStraddling.h"
#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "MetricFile.h"
#include "GeodesicHelper.h"
#include <cmath>
#include <iostream>
#include <vector>

/**
 * Constructor.
 */
BrainModelSurfaceBankStraddling::BrainModelSurfaceBankStraddling(
                                               BrainSet* bs,
                                               int bsIndexIn,
                                               MetricFile* metricOutIn,
                                               int metricOutIndexIn,
                                               float voxdimIn[3],
                                               bool interpIn)
   : BrainModelAlgorithm(bs)
{
   setIndex   = bsIndexIn;
   metricOut    = metricOutIn;
   metricOutIndex = metricOutIndexIn;
   interp = interpIn;
   voxdim[0] = voxdimIn[0];
   voxdim[1] = voxdimIn[1];
   voxdim[2] = voxdimIn[2];
}
                                      
/**
 * Destructor.
 */
BrainModelSurfaceBankStraddling::~BrainModelSurfaceBankStraddling()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceBankStraddling::execute() throw (BrainModelAlgorithmException)
{
   //
   // Verify files exist, are valid, etc.
   //
   BrainModelSurface* mysurf = brainSet->getBrainModelSurface(setIndex);//reference
   if (!mysurf)
   {
      throw BrainModelAlgorithmException("No input surface.");
   }
   CoordinateFile* source = mysurf->getCoordinateFile();//references
   TopologyFile* topo = mysurf->getTopologyFile();
   GeodesicHelper mygeo(source, topo);
   if (voxdim[0] <= 0.0f || voxdim[1] <= 0.0f || voxdim[2] <= 0.0f)
   {
      throw BrainModelAlgorithmException("Invalid voxel dimensions.");
   }
   if (source == NULL)
   {
      throw BrainModelAlgorithmException("Invalid coordinate file.");
   }
   if (source->getNumberOfCoordinates() < 1)
   {
      throw BrainModelAlgorithmException("Not enough nodes in coordinate file.");
   }
   //
   // Check output files exist
   //
   if (metricOut == NULL) {
      throw BrainModelAlgorithmException("No valid output file.");
   }
   if (metricOut->getNumberOfNodes() != source->getNumberOfCoordinates())
   {
      metricOut->setNumberOfNodesAndColumns(source->getNumberOfCoordinates(), 1);
      metricOutIndex = 0;
   }
   if (metricOutIndex < 0 || metricOutIndex >= metricOut->getNumberOfColumns())
   {
      metricOutIndex = metricOut->getNumberOfColumns();
      metricOut->addColumns(1);
   }
   metricOut->setColumnName(metricOutIndex, QString("Bank Straddling ") + QString::number(voxdim[0]) + QString("x") +
                              QString::number(voxdim[0]) + QString("x") + QString::number(voxdim[0]) + QString("mm, ") +
                              (interp ? QString("interpolated") : QString("enclosing")));
   int numNodes = source->getNumberOfCoordinates();
   int i, j, numInrange, whichnode, coordbase;//, worstnode;
   float rootCoord[3], selectrange[3];
   float tempf, tempf2, tempf3;
   if (interp)
   {
      selectrange[0] = 2.0f * voxdim[0];
      selectrange[1] = 2.0f * voxdim[1];
      selectrange[2] = 2.0f * voxdim[2];
   } else {
      selectrange[0] = voxdim[0];
      selectrange[1] = voxdim[1];
      selectrange[2] = voxdim[2];
   }
   std::vector<int> inrange;
   std::vector<float> distances;
   float* outData = new float[numNodes];
   float* sourceData = new float[numNodes * 3];
   source->getAllCoordinates(sourceData);
   for (i = 0; i < numNodes; ++i)
   {
      coordbase = i * 3;
      rootCoord[0] = sourceData[coordbase];
      rootCoord[1] = sourceData[coordbase + 1];
      rootCoord[2] = sourceData[coordbase + 2];
      inrange.clear();
      for (j = 0; j < numNodes; ++j)
      {
         coordbase = j * 3;
         if (std::abs(rootCoord[0] - sourceData[coordbase]) < selectrange[0] &&
               std::abs(rootCoord[1] - sourceData[coordbase + 1]) < selectrange[1] &&
               std::abs(rootCoord[2] - sourceData[coordbase + 2]) < selectrange[2])
         {
            inrange.push_back(j);
         }
      }
      mygeo.getGeoToTheseNodes(i, inrange, distances, true);
      numInrange = inrange.size();
      tempf = -1.0f;
      if (interp)
      {
         for (j = 0; j < numInrange; ++j)
         {
            whichnode = inrange[j];
            coordbase = whichnode * 3;
            tempf2 = (1 - std::abs(rootCoord[0] - sourceData[coordbase]) / selectrange[0]) *
                        (1 - std::abs(rootCoord[1] - sourceData[coordbase + 1]) / selectrange[1]) *
                        (1 - std::abs(rootCoord[2] - sourceData[coordbase + 2]) / selectrange[2]);
            tempf3 = 1 - tempf2;
            tempf2 *= tempf2;
            tempf3 *= tempf3;
            tempf2 /= (tempf2 + tempf3);//this is r^2 / (r^2 + (1 - r)^2), derived forced correlation
            tempf2 *= distances[j];//times geodesic distance
            if (tempf2 > tempf)
            {
               //worstnode = whichnode;
               tempf = tempf2;
            }
         }
      } else {
         for (j = 0; j < numInrange; ++j)
         {
            tempf2 = distances[j];
            if (tempf2 > tempf)
            {
               //worstnode = inrange[j];
               tempf = tempf2;
            }
         }
      }
      //std::cout << "node " << i << ": " << worstnode << endl;
      outData[i] = tempf;
   }
   metricOut->setColumnForAllNodes(metricOutIndex, outData);
   delete[] sourceData;
   delete[] outData;
}
