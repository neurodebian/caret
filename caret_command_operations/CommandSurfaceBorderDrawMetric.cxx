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

#include <QFile>

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceROICreateBorderUsingMetricShape.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderDrawMetric.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "FociProjectionFile.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "NodeRegionOfInterestFile.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceBorderDrawMetric::CommandSurfaceBorderDrawMetric()
   : CommandBase("-surface-border-draw-metric",
                 "SURFACE BORDER DRAWING ALONG METRIC")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderDrawMetric::~CommandSurfaceBorderDrawMetric()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderDrawMetric::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> searchValues, searchDescriptions;
      searchValues.push_back("POSITIVE");  searchDescriptions.push_back("Search Positive Nodes");
      searchValues.push_back("NEGATIVE");  searchDescriptions.push_back("Search Negative Nodes");
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Metric or Shape File Name", 
                     FileFilters::getMetricShapeFileFilter());
   paramsOut.addString("Metric or Shape Column");
   paramsOut.addListOfItems("Search Mode", searchValues, searchDescriptions);
   paramsOut.addFile("Foci Projection File Name", 
                     FileFilters::getFociProjectionFileFilter());
   paramsOut.addString("Border Start Focus Name");
   paramsOut.addString("Border End Focus Name");
   paramsOut.addFile("Input Border Projection File Name", 
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File Name", 
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addString("Border Name");
   paramsOut.addFloat("Sampling Interval", 1.0, 0.0, 100000.0);
   paramsOut.addVariableListOfParameters("Border Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderDrawMetric::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>  \n"
       + indent9 + "<topology-file-name>  \n"
       + indent9 + "<metric-shape-file-name> \n"
       + indent9 + "<metric-shape-column> \n"
       + indent9 + "<METRIC-SHAPE-SEARCH-MODE> \n"
       + indent9 + "<foci-projection-file-name>  \n"
       + indent9 + "<name-of-focus-for-border-start>  \n"
       + indent9 + "<name-of-focus-for-border-end>  \n"
       + indent9 + "<input-border-projection-file-name>  \n"
       + indent9 + "<output-border-projection-file-name>  \n"
       + indent9 + "<output-border-name>  \n"
       + indent9 + "<output-border-sampling>  \n"
       + indent9 + "[-roi-file  roi-file-name] \n"
       + indent9 + "\n"
       + indent9 + "Draw a border along nodes in the region of interest\n"
       + indent9 + "from the node nearest the starting focus to the \n"
       + indent9 + "ending focus.  Each successive node is the next node\n"
       + indent9 + "connected to the current node that has the most positive\n"
       + indent9 + "or negative value and is closer to the ending node than\n"
       + indent9 + "the current node.\n"
       + indent9 + "\n"
       + indent9 + "Uses the last focus for starting and stopping foci.\n"
       + indent9 + "\n"
       + indent9 + "\"METRIC-SHAPE-SEARCH-MODE\" is one of:\n"
       + indent9 + "   POSITIVE      - searches along most positive nodes.\n"
       + indent9 + "   NEGATIVE      - searches along most negative nodes.\n"
       + indent9 + "\n"
       + indent9 + "If a region of interest file is supplied, only nodes that \n"
       + indent9 + "are in the region of interest will be searched.\n"
       + indent9 + "\n"
       + indent9 + "\"metric-shape-column\" is either the number of the column,\n"
       + indent9 + "which starts at one, or the name of the column.  If a \n"
       + indent9 + "name contains spaces, it must be enclosed in double \n"
       + indent9 + "quotes.  Name has priority over number. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderDrawMetric::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString coordinateFileName = 
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName = 
      parameters->getNextParameterAsString("Topology File Name");
   const QString metricFileName = 
      parameters->getNextParameterAsString("Metric/Shape File File Name");
   const QString metricColumnString = 
      parameters->getNextParameterAsString("Metric/Shape Column");
   const QString metricSearchModeString = 
      parameters->getNextParameterAsString("Metric/Shape Search Mode");
   const QString fociProjectionFileName = 
      parameters->getNextParameterAsString("Foci Projection File Name");
   const QString focusNameForBorderStart = 
      parameters->getNextParameterAsString("Name of Focus for Border Start");
   const QString focusNameForBorderEnd = 
      parameters->getNextParameterAsString("Name of Focus for Border End");
   const QString inputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Output Border Projection File Name");
   const QString borderName = 
      parameters->getNextParameterAsString("Output Border Name");
   const float borderSampling = 
      parameters->getNextParameterAsFloat("Output Border Sampling");

   //
   // Optional append file names
   //
   QString regionOfInterestFileName;
   
   //
   // Process the parameters for node selection
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get the selection type
      //
      const QString paramName =
         parameters->getNextParameterAsString("Optional Border Draw Parameters");
         
      if (paramName == "-roi-file") {
         regionOfInterestFileName = parameters->getNextParameterAsString("Region of Interest File Name");
      }
      else {
         throw CommandException("Unrecognized parameter = \""
                                + paramName
                                + "\".");
      }
   }
   
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     "",
                     true);
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read the metric file
   //
   MetricFile metricFile;
   metricFile.readFile(metricFileName);
   
   //
   // Get the metric shape column
   //
   const int metricColumnNumber = metricFile.getColumnFromNameOrNumber(metricColumnString, false);
   
   //
   // Check mode
   //
   BrainModelSurfaceROICreateBorderUsingMetricShape::MODE searchMode;
   if (metricSearchModeString == "NEGATIVE") {
      searchMode = BrainModelSurfaceROICreateBorderUsingMetricShape::MODE_FOLLOW_MOST_NEGATIVE;
   }
   else if (metricSearchModeString == "POSITIVE") {
      searchMode = BrainModelSurfaceROICreateBorderUsingMetricShape::MODE_FOLLOW_MOST_POSITIVE;
   }
   else {
      throw CommandException("invalid metric mode "
                             + metricSearchModeString);
   }
   
   //
   // Set the region of interest for the brain set
   //
   BrainModelSurfaceROINodeSelection* roi = 
      brainSet.getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (regionOfInterestFileName.isEmpty() == false) {
      //
      // Get region of interest from file
      //
      NodeRegionOfInterestFile roiFile;
      roiFile.readFile(regionOfInterestFileName);
      roi->getRegionOfInterestFromFile(roiFile);
   }
   else {
      //
      // Put all nodes in region of interest
      //
      roi->selectAllNodes(bms);
   }
   
   //
   // Read the foci projection file
   //
   FociProjectionFile fociProjectionFile;
   fociProjectionFile.readFile(fociProjectionFileName);
   
   //
   // Get the start and end foci
   //
   const CellProjection* startFocus = 
      fociProjectionFile.getLastCellProjectionWithName(focusNameForBorderStart);
   if (startFocus == NULL) {
      throw CommandException("unable to find a focus named \""
                             + focusNameForBorderStart
                             + "\" in the file "
                             + FileUtilities::basename(fociProjectionFileName));
   }
   const CellProjection* endFocus = 
      fociProjectionFile.getLastCellProjectionWithName(focusNameForBorderEnd);
   if (endFocus == NULL) {
      throw CommandException("unable to find a focus named \""
                             + focusNameForBorderEnd
                             + "\" in the file "
                             + FileUtilities::basename(fociProjectionFileName));
   }
   
   //
   // Unproject the foci to the surface
   //
   const bool fiducialSurfaceFlag = 
      (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   const bool flatSurfaceFlag = 
      ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT) ||
       (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR));
   float startXYZ[3], endXYZ[3];
   if (startFocus->getProjectedPosition(bms->getCoordinateFile(),
                                        bms->getTopologyFile(),
                                        fiducialSurfaceFlag,
                                        flatSurfaceFlag,
                                        false,
                                        startXYZ) == false) {
      throw CommandException("unable to get projected position for "
                             + focusNameForBorderStart);
   }
   if (endFocus->getProjectedPosition(bms->getCoordinateFile(),
                                        bms->getTopologyFile(),
                                        fiducialSurfaceFlag,
                                        flatSurfaceFlag,
                                        false,
                                        endXYZ) == false) {
      throw CommandException("unable to get projected position for "
                             + focusNameForBorderEnd);
   }

   //if (DebugControl::getDebugOn()) {
      std::cout << "Start XYZ: " 
                << startXYZ[0] << " "
                << startXYZ[1] << " "
                << startXYZ[2] << std::endl;
      std::cout << "End XYZ:   "
                << endXYZ[0] << " "
                << endXYZ[1] << " "
                << endXYZ[2] << std::endl;
   //}

   //
   // Get the nodes nearest the start and end foci
   //
   const int startNode = bms->getNodeClosestToPoint(startXYZ);
   if (startNode < 0) {
      throw CommandException("unable to find node closest to start focus.");
   }
   const int endNode = bms->getNodeClosestToPoint(endXYZ);
   if (endNode < 0) {
      throw CommandException("unable to find node closest to end focus.");
   }
   
   //if (DebugControl::getDebugOn()) {
      std::cout << "Border Start Node: " << startNode << std::endl;
      std::cout << "Border End Node:   " << endNode << std::endl;
   //}
   
   //
   // Force the starting and ending nodes to be in the ROI and connected
   //
   roi->expandSoNodesAreWithinAndConnected(bms,
                                           startNode,
                                           endNode);
                                           
   //
   // Draw the border
   //
   BrainModelSurfaceROICreateBorderUsingMetricShape createBorder(&brainSet,
                                                 bms,
                                                 roi,
                                                 searchMode,
                                                 &metricFile,
                                                 metricColumnNumber,
                                                 borderName,
                                                 startNode,
                                                 endNode,
                                                 borderSampling);
   createBorder.execute();
   const Border border = createBorder.getBorder();
   if (border.getNumberOfLinks() <= 0) {
      throw ("Border drawn as "
             + borderName
             + "\"has no links.");
   }
   BorderFile borderFile;
   borderFile.addBorder(border);
   
   //
   // Project the border
   //
   BorderProjectionFile bpf;
   BorderFileProjector projector(bms, true);
   projector.projectBorderFile(&borderFile,
                               &bpf,
                               NULL);
                               
   //
   // Write the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   if (inputBorderProjectionFileName.isEmpty() == false) {
      if (QFile::exists(inputBorderProjectionFileName)) {
         borderProjectionFile.readFile(inputBorderProjectionFileName);
      }
   }
   borderProjectionFile.append(bpf);
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
}

      

