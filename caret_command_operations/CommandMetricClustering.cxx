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

#include "BrainModelSurfaceMetricClustering.h"
#include "BrainSet.h"
#include "CommandMetricClustering.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricClustering::CommandMetricClustering()
   : CommandBase("-metric-clustering",
                 "METRIC CLUSTERING")
{
}

/**
 * destructor.
 */
CommandMetricClustering::~CommandMetricClustering()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricClustering::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("ANY_SIZE");   descriptions.push_back("Any Size");
   values.push_back("MINIMUM_NUMBER_OF_NODES");   descriptions.push_back("Minimum Number of Nodes");
   values.push_back("MINIMUM_SURFACE_AREA");   descriptions.push_back("Minimum Surface Area");

   paramsOut.clear();
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFloat("Minimum Negative Threshold", -5.0);
   paramsOut.addFloat("Maximum Negative Threshold", -10.0);
   paramsOut.addFloat("Minimum Positive Threshold", 5.0);
   paramsOut.addFloat("Maximum Positive Threshold", 10.0);
   paramsOut.addListOfItems("Cluster Mode", values, descriptions);
   paramsOut.addInt("Minimum Number of Nodes", 25);
   paramsOut.addFloat("Minimum Surface Area", 25.0);
}

/**
 * get full help information.
 */
QString 
CommandMetricClustering::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<minimum-negative-threshold>\n"
       + indent9 + "<maximum-negative-threshold>\n"
       + indent9 + "<minimum-positive-threshold>\n"
       + indent9 + "<maximum-positive-threshold>\n"
       + indent9 + "<cluster-mode>\n"
       + indent9 + "<cluster-minimum-number-of-node>\n"
       + indent9 + "<cluster-minimum-surface-area>\n"
       + indent9 + "\n"
       + indent9 + "Perform clustering on all columns in a metric file.\n"
       + indent9 + "\n"
       + indent9 + "\"cluster-mode\" must be one of:\n"
       + indent9 + "   ANY_SIZE\n"
       + indent9 + "   MINIMUM_NUMBER_OF_NODES\n"
       + indent9 + "   MINIMUM_SURFACE_AREA\n"
       + indent9 + "\n"
       + indent9 + "Note: The \"maximum-negative-threshold\" should be\n"
       + indent9 + "LESS THAN (more negative) than the \n"
       + indent9 + "\"minimum-negative-threshold\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricClustering::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   const float minimumNegativeThreshold =
      parameters->getNextParameterAsFloat("Minimum Negative Threshold");
   const float maximumNegativeThreshold =
      parameters->getNextParameterAsFloat("Maximum Negative Threshold");
   const float minimumPositiveThreshold =
      parameters->getNextParameterAsFloat("Minimum Positive Threshold");
   const float maximumPositiveThreshold =
      parameters->getNextParameterAsFloat("Maximum Positive Threshold");
   const QString modeName =
      parameters->getNextParameterAsString("Cluster Mode Name");
   const int minimumNumberOfNodes =
      parameters->getNextParameterAsInt("Minimum Number of Nodes");
   const float minimumSurfaceArea =
      parameters->getNextParameterAsFloat("Minimum Surface Area");
   checkForExcessiveParameters();
   
   //
   // Get algorithm
   //
   BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM clusterAlgorithm;
   if (modeName == "ANY_SIZE") {
      clusterAlgorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_ANY_SIZE;
   }
   else if (modeName == "MINIMUM_SURFACE_AREA") {
      clusterAlgorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA;
   }
   else if (modeName == "MINIMUM_NUMBER_OF_NODES") {
      clusterAlgorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES;
   }
   else {
      throw CommandException("Invalid mode: \""
                             + modeName
                             + "\"");
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     "",
                     true);
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read input metric file
   //
   MetricFile metricFile;
   metricFile.readFile(inputMetricFileName);

   //
   // Perform clustering
   //
   const int numberOfColumns = metricFile.getNumberOfColumns();
   for (int i = 0; i < numberOfColumns; i++) {
      BrainModelSurfaceMetricClustering clustering(&brainSet,
                                                   surface,
                                                   &metricFile,
                                                   clusterAlgorithm,
                                                   i,
                                                   i,
                                                   metricFile.getColumnName(i),
                                                   minimumNumberOfNodes,
                                                   minimumSurfaceArea,
                                                   minimumNegativeThreshold,
                                                   maximumNegativeThreshold,
                                                   minimumPositiveThreshold,
                                                   maximumPositiveThreshold,
                                                   true);
      clustering.execute();
   }
   
   //
   // Write the metric file
   //
   metricFile.writeFile(outputMetricFileName);
}

      

