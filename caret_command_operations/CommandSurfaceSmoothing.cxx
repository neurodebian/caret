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
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CommandSurfaceSmoothing.h"
#include "FileFilters.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceSmoothing::CommandSurfaceSmoothing()
   : CommandBase("-surface-smoothing",
                 "SURFACE SMOOTHING")
{
}

/**
 * destructor.
 */
CommandSurfaceSmoothing::~CommandSurfaceSmoothing()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceSmoothing::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Output Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFloat("Strength", 1.0, 0.0, 1.0);
   paramsOut.addInt("Iterations", 10, 0, 100000);
   paramsOut.addInt("Edge Iterations", 0, 0, 100000);
   paramsOut.addVariableListOfParameters("Smoothing Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceSmoothing::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <input-coordinate-file-name>\n"
       + indent9 + "   <output-coordinate-file-name>\n"
       + indent9 + "   <topology-file-name>\n"
       + indent9 + "   <smoothing-strength>\n"
       + indent9 + "   <smoothing-iterations>\n"
       + indent9 + "   <smoothing-edge-iterations>\n"
       + indent9 + "   [-project-to-sphere   every-x-iterations]\n"
       + indent9 + "   [-roi-file  roi-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Smooth a surface.\n"
       + indent9 + "\n"
       + indent9 + "Use \"-project-to-sphere\" to project the surface to a\n"
       + indent9 + "sphere \"every-x-iterations\".\n"
       + indent9 + "\n"
       + indent9 + "Use \"-roi-file\" to limit smoothing to a region of \n"
       + indent9 + "interest (subset of the surface).\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceSmoothing::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get coord, topo, and roi file names
   //
   const QString inputCoordinateFileName = 
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString outputCoordinateFileName = 
      parameters->getNextParameterAsString("Output Coordinate File Name");
   const QString topoFileName = 
      parameters->getNextParameterAsString("Input Topology File Name");
   const float smoothingStrength = 
      parameters->getNextParameterAsFloat("Smoothing Strength");
   const int smoothingIterations = 
      parameters->getNextParameterAsInt("Smoothing Iterations");
   const int smoothingEdgeIterations = 
      parameters->getNextParameterAsInt("Smoothing Edge Iterations");
     
   //
   // Create a brain set
   //
   BrainSet brainSet(topoFileName,
                     inputCoordinateFileName,
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
      throw CommandException("surface contains not nodes.");
   }

   //
   // Create an ROI of all nodes
   //
   BrainModelSurfaceROINodeSelection roi(&brainSet);
   roi.selectAllNodes(bms);
   
   int projectToSphereIterations = -1;
   
   //
   // Process the parameters for node selection
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get the parameter
      //
      const QString parameterName =
         parameters->getNextParameterAsString("SURFACE SMOOTHING option");
         
      if (parameterName == "-project-to-sphere") {
         projectToSphereIterations = 
            parameters->getNextParameterAsInt("SURFACE SMOOTHING Project to Sphere Every X Iterations");
      }
      else if (parameterName == "-roi-file") {
         const QString roiFileName =
            parameters->getNextParameterAsString("SURFACE SMOOTHING roi-file-name");
         NodeRegionOfInterestFile roiFile;
         roiFile.readFile(roiFileName);
         roi.getRegionOfInterestFromFile(roiFile);
      }
      else {
         throw CommandException("Unrecognized operation = \""
                                + parameterName
                                + "\".");
      }
   }
   
   //
   // Set nodes for smoothing
   //
   std::vector<bool> nodesToBeSmoothed(numNodes);
   for (int i = 0; i < numNodes; i++) {
      nodesToBeSmoothed[i] = roi.getNodeSelected(i);
   }
   
   //
   // Smooth the surface
   //
   bms->arealSmoothing(smoothingStrength,
                       smoothingIterations,
                       smoothingEdgeIterations,
                       &nodesToBeSmoothed,
                       projectToSphereIterations);

   //
   // Write the coordinate file
   //
   CoordinateFile* outputCoordFile = bms->getCoordinateFile();
   outputCoordFile->writeFile(outputCoordinateFileName);
}

      

