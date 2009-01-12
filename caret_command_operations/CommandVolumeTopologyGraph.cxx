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

#include "BrainModelVolumeTopologyGraph.h"
#include "BrainSet.h"
#include "CommandVolumeTopologyGraph.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeTopologyGraph::CommandVolumeTopologyGraph()
   : CommandBase("-volume-topology-graph",
                 "VOLUME TOPOLOGY GRAPH (in development - do not use)")
{
}

/**
 * destructor.
 */
CommandVolumeTopologyGraph::~CommandVolumeTopologyGraph()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeTopologyGraph::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> xyz;
   xyz.push_back("X");
   xyz.push_back("Y");
   xyz.push_back("Z");
   
   std::vector<QString> neighbors;
   neighbors.push_back("6");
   neighbors.push_back("18");
   neighbors.push_back("26");
   
   std::vector<QString> foregroundBackground;
   foregroundBackground.push_back("FG");
   foregroundBackground.push_back("BG");
   
   paramsOut.clear();
   paramsOut.addFile("Input Segmentation Volume File Name",
                     FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addListOfItems("AXIS (X/Y/Z)", xyz, xyz);
   paramsOut.addListOfItems("Neighbors", neighbors, neighbors);
   paramsOut.addListOfItems("Foreground/Background", foregroundBackground, foregroundBackground);
   paramsOut.addFile("GraphViz File",
                     "GraphViz File (*.dot)",
                     "graph.dot",
                     "-graphviz-dot-file");
   paramsOut.addFile("Graph Paint File",
                     FileFilters::getVolumePaintFileFilter(),
                     "graph.nii.gz",
                     "-graph-paint-volume");
   paramsOut.addFile("Handles Paint File",
                     FileFilters::getVolumePaintFileFilter(),
                     "handles.nii.gz",
                     "-handles-paint-volume");
}

/**
 * get full help information.
 */
QString 
CommandVolumeTopologyGraph::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-segmentation-volume-file-name>\n"
       + indent9 + "<search-axis>\n"
       + indent9 + "<neighbor-connectivity>\n"
       + indent9 + "<foreground-or-background>\n"
       + indent9 + "[-graphviz-dot-file  <file-name>]\n"
       + indent9 + "[-graph-paint-volume <file-name>]\n"
       + indent9 + "[-handles-paint-volume <file-name>]\n"
       + indent9 + "\n"
       + indent9 + "Generaete segmentation volume topology graph.\n"
       + indent9 + "\n"
       + indent9 + "Search-Axis is one of:\n"
       + indent9 + "   X\n"
       + indent9 + "   Y\n"
       + indent9 + "   Z\n"
       + indent9 + "\n"
       + indent9 + "Neighbor-Connectivity is one of:\n"
       + indent9 + "    6\n"
       + indent9 + "   18\n"
       + indent9 + "   26\n"
       + indent9 + "\n"
       + indent9 + "Foreground-or-Background is one of\n"
       + indent9 + "   FG     Compute graph of volume.\n"
       + indent9 + "   BG     Compute graph of inverse volume.\n"
       + indent9 + "\n"
       + indent9 + "\"-graphviz-dot-file\" allows the creation of a \".dot\"\n"
       + indent9 + "for use with a GraphVis program\n"
       + indent9 + "   (http::graphviz.org).\n"
       + indent9 + "Run \"neato -Tpdf -o graph.pdf graph.dot\" to create a\n"
       + indent9 + "PDF file containing the graph.\n"
       + indent9 + "\n"
       + indent9 + "\"-graph-paint-volume\" allows the creation of a paint\n"
       + indent9 + "volume with each graph vertex given an paint identifier.\n"
       + indent9 + "\n"
       + indent9 + "\"-handles-paint-volume\" allows the creation of a paint\n"
       + indent9 + "volume identifying the \"handle-part\" of the handle.\n"
       + indent9 + "\n"
       + indent9 + "Note: Names assigned to vertices are identical for the\n"
       + indent9 + " \"-graphviz-dot-file\" and -graph-paint-volume\" options.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeTopologyGraph::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString segmentationVolumeFileName =
      parameters->getNextParameterAsString("Input Segmentation Volume File Name");
   const QString axisName =
      parameters->getNextParameterAsString("Search Axis Name");
   const QString neighborConnectivity =
      parameters->getNextParameterAsString("Neighbor Connectivity");
   const QString foregroundBackground =
      parameters->getNextParameterAsString("Foreground/Background");
   
   QString graphvizFileName;
   QString graphVolumeFileName;
   QString handlesVolumeFileName;
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString("Optional Parameter");
      if (paramName == "-graphviz-dot-file") {
         graphvizFileName =
            parameters->getNextParameterAsString("GraphViz File Name");
      }
      else if (paramName == "-graph-paint-volume") {
         graphVolumeFileName =
            parameters->getNextParameterAsString("Graph Paint Volume File Name");
      }
      else if (paramName == "-handles-paint-volume") {
         handlesVolumeFileName =
            parameters->getNextParameterAsString("Handles Paint Volume File Name");
      }
      else {
         throw CommandException("Unrecognized Parameter: " + paramName);
      }
   }
   
   //
   // Set search axis
   //
   BrainModelVolumeTopologyGraph::SEARCH_AXIS searchAxis =
      BrainModelVolumeTopologyGraph::SEARCH_AXIS_Z;
   if (axisName == "X") {
      searchAxis = BrainModelVolumeTopologyGraph::SEARCH_AXIS_X;   
   }
   else if (axisName == "Y") {
      searchAxis = BrainModelVolumeTopologyGraph::SEARCH_AXIS_Y;   
   }
   else if (axisName == "Z") {
      searchAxis = BrainModelVolumeTopologyGraph::SEARCH_AXIS_Z;   
   }
   else {
      throw CommandException("Invalid Search Axis: " + axisName);
   }
   
   //
   // Neighbor connectivity
   //
   BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY neighbors =
      BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_6;
   if (neighborConnectivity == "6") {
      neighbors = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_6;   
   }
   else if (neighborConnectivity == "18") {
      neighbors = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_18;   
   }
   else if (neighborConnectivity == "26") {
      neighbors = BrainModelVolumeTopologyGraph::VOXEL_NEIGHBOR_CONNECTIVITY_26;   
   }
   else {
      throw CommandException("Invalid Neighbor Connectivity: " + neighborConnectivity);
   }

   //
   // Foreground/Background
   //
   bool doForegroundFlag = true;
   if (foregroundBackground == "FG") {
      doForegroundFlag = true;
   }
   else if (foregroundBackground == "BG") {
      doForegroundFlag = false;
   }
   else {
      throw CommandException("Invalid Foreground/Background: " + foregroundBackground);
   }
   
   //
   // Read volume
   //
   VolumeFile segmentationVolumeFile;
   segmentationVolumeFile.readFile(segmentationVolumeFileName);
   if (doForegroundFlag == false) {
      segmentationVolumeFile.invertSegmentationVoxels();
   }
   
   //
   // Create graph of foreground
   //
   BrainSet bs;
   BrainModelVolumeTopologyGraph graph(&bs,
                                       &segmentationVolumeFile,
                                       searchAxis,
                                       neighbors);
   QTime timer;
   timer.start();
   graph.execute();
   const float totalTime = (static_cast<float>(timer.elapsed()) * 0.001);

   graph.printResults();
   
   if (graphvizFileName.isEmpty() == false) {
      graph.writeGraphVizDotFile(graphvizFileName);
   }
   
   if (graphVolumeFileName.isEmpty() == false) {
      graph.writeGraphToPaintVolumeFile(graphVolumeFileName);
   }
   if (handlesVolumeFileName.isEmpty() == false) {
      VolumeFile handlesVolume;
      graph.createHandlesPaintVolumeFile(handlesVolume);
      handlesVolume.writeFile(handlesVolumeFileName);
   }
   
   std::cout << "Time to compute graph: "
             << totalTime
             << " seconds."
             << std::endl;   
}
