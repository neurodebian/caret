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

#include <iostream>
#include <fstream>

#include "CommandSurfaceGeodesic.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainSet.h"
#include "StringUtilities.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "GeodesicHelper.h"

/**
 * constructor.
 */
CommandSurfaceGeodesic::CommandSurfaceGeodesic()
   : CommandBase("-surface-geodesic",
                 "SURFACE GEODESIC DISTANCE")
{
}

/**
 * destructor.
 */
CommandSurfaceGeodesic::~CommandSurfaceGeodesic()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceGeodesic::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addBoolean("Smoothing", false);
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceGeodesic::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<surface-coord>\n"
       + indent9 + "<surface-topo>\n"
       + indent9 + "<output-metric>\n"
       + indent9 + "<smoothed>\n"
       + indent9 + "[-node  node-number]\n"
       + indent9 + "\n"
       + indent9 + "Generate the geodesic distance from all nodes to all other nodes\n"
       + indent9 + "unless a node number is specified in which case the distances are\n"
       + indent9 + "output for the specified node to all nodes.\n"
       + indent9 + "Uses dijkstra's algorithm with reuse of information from previous\n"
       + indent9 + "root nodes.  If smoothed is 'true', it traces paths over adjacent\n"
       + indent9 + "triangles in order to generate distances for some 2hop neighbors.\n"
       + indent9 + "This causes the contours of equal distance to be closer to circular,\n"
       + indent9 + "and also makes the distances more consistent for a mesh which has\n"
       + indent9 + "been streched nonuniformly.  It is almost as fast as unsmoothed.\n"
       + indent9 + "\n"
       + indent9 + "      surface-coord      the surface coord file\n"
       + indent9 + "\n"
       + indent9 + "      surface-topo       the surface topo file\n"
       + indent9 + "\n"
       + indent9 + "      output-metric      output metric file for geodesic distance\n"
       + indent9 + "\n"
       + indent9 + "      smoothed           output smoothed distances by using 2hop neighbors\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceGeodesic::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coord =
      parameters->getNextParameterAsString("Input Coordinate File");
   const QString topo =
      parameters->getNextParameterAsString("Input Topology File");
   const QString metricName =
      parameters->getNextParameterAsString("Output Metric File");
   const bool smooth =
      parameters->getNextParameterAsBoolean("Smoothing");
   int nodeNumber = -1;
   while (parameters->getParametersAvailable()) {
      QString paramName = parameters->getNextParameterAsString("Geodesic Parameter");
      if (paramName == "-node") {
         nodeNumber = parameters->getNextParameterAsInt("Node Number");
      }
      else {
         throw CommandException("Invalid Parameter: " + paramName);
      }   
   }
   BrainSet mybs(topo, coord);//yes, its hideous, but the BrainSet constructor is such an easy way to load them
   BrainModelSurface* mysurf = mybs.getBrainModelSurface(0);
   int numNodes = mysurf->getCoordinateFile()->getNumberOfNodes();
   GeodesicHelper gh(mysurf->getCoordinateFile(), mysurf->getTopologyFile());
   if (nodeNumber >= 0) {
#ifdef _USE_STL_FOR_DATA_
      std::vector<int> allNodeIndices;
      allNodeIndices.reserve(numNodes);
      for (int i = 0; i < numNodes; i++) {
         allNodeIndices.push_back(i);
      }
      std::vector<float> distances;
      gh.getGeoToTheseNodes(nodeNumber, allNodeIndices, distances, smooth);
      if (static_cast<int>(distances.size()) == numNodes) {
         MetricFile mymetric;
         mymetric.setNumberOfNodesAndColumns(numNodes, 1);
         mymetric.setColumnForAllNodes(0, distances);
         mymetric.setColumnName(0, "Node " + QString::number(nodeNumber));
         mymetric.writeFile(metricName);
      }
      else {
         std::cerr << "getGeoToTheseNodes() failed." << std::endl;
      }
#else
      float* distances = new float[numNodes];
      gh.getGeoFromNode(nodeNumber, distances, smooth);
      MetricFile mymetric;
      mymetric.setNumberOfNodesAndColumns(numNodes, 1);
      mymetric.setColumnForAllNodes(0, distances);
      mymetric.setColumnName(0, "Node " + QString::number(nodeNumber));
      mymetric.writeFile(metricName);
#endif
   }
   else {
      float** result = gh.getGeoAllToAll(smooth);//PASSES BACK ALLOCATED MEMORY, we must delete[] all of it
      //was more convinient to write the code to catch malloc failures into that function, does not crash if failed, merely deallocates
      //everything that it succeeded in allocating, then returns NULL
      if (result != NULL)//happens if memory runs out
      {
         std::cout << "saving to " << metricName.toLocal8Bit().constData() << "..." << std::endl;
         MetricFile mymetric;
         mymetric.setNumberOfNodesAndColumns(numNodes, numNodes);
         for (int i = 0; i < numNodes; ++i)
         {
            mymetric.setColumnForAllNodes(i, result[i]);
            delete[] result[i];
         }
         delete[] result;
         mymetric.writeFile(metricName);
      } else {
         std::cerr << "getGeoAllToAll() failed." << std::endl;
      }
   }
}
