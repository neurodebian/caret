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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROIAssignMetricNodeArea.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CommandSurfaceRoiNodeAreas.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandSurfaceRoiNodeAreas::CommandSurfaceRoiNodeAreas()
   : CommandBase("-surface-roi-node-areas",
                 "SURFACE ROI NODE AREAS")
{
}

/**
 * destructor.
 */
CommandSurfaceRoiNodeAreas::~CommandSurfaceRoiNodeAreas()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceRoiNodeAreas::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric/Shape File", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Output Metric/Shape File", FileFilters::getMetricShapeFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRoiNodeAreas::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-metric-or-shape-file-name>\n"
       + indent9 + "<output-metric-or-shape-file-name>\n"
       + indent9 + "[-roi  region-of-interest-file-name]\n"
       + indent9 + "[-percentage]\n"
       + indent9 + "\n"
       + indent9 + "Generate a new metric or shape file column containing\n"
       + indent9 + "the surface area for each node.  While nodes technically\n"
       + indent9 + "do not have area, triangles do.  Since a triangle is made\n"
       + indent9 + "from three nodes, assign one-third of the triangle's\n"
       + indent9 + "surface area to each node and sum these amounts for each\n"
       + indent9 + "node.\n"
       + indent9 + "\n"
       + indent9 + "If an ROI is provided, the area measurements are limited\n"
       + indent9 + "to that region of the surface and nodes not in the ROI are\n"
       + indent9 + "assigned a value of zero.\n"
       + indent9 + "\n"
       + indent9 + "If the \"-percentage\" option is specified, the value \n"
       + indent9 + "assigned to each node is the percentage of total surface \n"
       + indent9 + "area contributed by that node.\n"
       + indent9 + "  Value = ((node-surface-area / total-surface-area) * 100.0)\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceRoiNodeAreas::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString coordinateFileName = 
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topoFileName = 
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputMetricShapeFileName = 
      parameters->getNextParameterAsString("Input Metric/Surface Shape File Name");
   const QString outputMetricShapeFileName = 
      parameters->getNextParameterAsString("Output Metric/Surface Shape File Name");
   
   QString roiFileName;
   bool percentageFlag = false;
   while (parameters->getParametersAvailable()) {
      const QString paramName = 
         parameters->getNextParameterAsString("Optional parameter");
      if (paramName == "-roi") {
         roiFileName = 
            parameters->getNextParameterAsString("Region of Interest File Name");
         if (roiFileName.isEmpty()) {
            throw CommandException("Region of Interest File Name is missing.");
         }
      }
      else if (paramName == "-percentage") {
         percentageFlag = true;
      }
      else {
         throw CommandException("Unrecognized parameter \""
                                + paramName
                                + "\".");
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topoFileName,
                     coordinateFileName,
                     "");
                     
   //
   // Find the surface
   //
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Unable to find surface after reading files.");
   }
   if (bms->getTopologyFile() == NULL) {
      throw CommandException("Unable to find topology after reading files.");
   }
   
   //
   // Read input metric/shape file
   //
   MetricFile mf;
   if (inputMetricShapeFileName.isEmpty() == false) {
      mf.readFile(inputMetricShapeFileName);
   }

   //
   // Get the ROI
   //
   BrainModelSurfaceROINodeSelection* surfaceROI = 
      brainSet.getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (roiFileName.isEmpty() == false) {
      NodeRegionOfInterestFile nroi;
      nroi.readFile(roiFileName);
      surfaceROI->getRegionOfInterestFromFile(nroi);
   }
   else {
      surfaceROI->selectAllNodes(bms);
   }
   
   QString columnName = "Node Area";
   if (percentageFlag) {
      columnName += " Percentage";
   }
   
   //
   // Get the areas
   //
   BrainModelSurfaceROIAssignMetricNodeArea
      roiOp(&brainSet,
            bms,
            surfaceROI,
            &mf,
            -1,
            "Node Area Percentage",
            percentageFlag);
   roiOp.execute();
   
   //
   // Write the metric file
   //
   mf.writeFile(outputMetricShapeFileName);
}

      

