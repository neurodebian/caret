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
#include "BrainModelSurfaceFindExtremum.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CellFileProjector.h"
#include "CommandSurfacePlaceFociAtExtremum.h"
#include "FileFilters.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfacePlaceFociAtExtremum::CommandSurfacePlaceFociAtExtremum()
   : CommandBase("-surface-place-focus-at-extremum",
                 "SURFACE PLACE FOCUS AT EXTREMUM")
{
}

/**
 * destructor.
 */
CommandSurfacePlaceFociAtExtremum::~CommandSurfacePlaceFociAtExtremum()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfacePlaceFociAtExtremum::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> extremums;
   extremums.push_back("X-LATERAL");
   extremums.push_back("X-MEDIAL");
   extremums.push_back("X-NEG");
   extremums.push_back("X-POS");
   extremums.push_back("Y-NEG");
   extremums.push_back("Y-POS");
   extremums.push_back("Z-NEG");
   extremums.push_back("Z-POS");

   paramsOut.clear();
   paramsOut.addFile("Fiducial Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Extremum Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Foci Projection File Name", 
                     FileFilters::getFociProjectionFileFilter());
   paramsOut.addString("Input Focus Name");
   paramsOut.addFile("Output Foci Projection File Name", 
                     FileFilters::getFociProjectionFileFilter());
   paramsOut.addString("Output Focus Name");
   paramsOut.addListOfItems("Direction", extremums, extremums);
   paramsOut.addFloat("Maximum X Movement", 10000000.0, 0.0, 1000000.0);
   paramsOut.addFloat("Maximum Y Movement", 10000000.0, 0.0, 1000000.0);
   paramsOut.addFloat("Maximum Z Movement", 10000000.0, 0.0, 1000000.0);
   paramsOut.addVariableListOfParameters("Information Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfacePlaceFociAtExtremum::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<fiducial-coord-file-name>  \n"
       + indent9 + "<extremum-coord-file-name>  \n"
       + indent9 + "<input-topology-file-name>  \n"
       + indent9 + "<input-foci-projection-file-name>  \n"
       + indent9 + "<input-focus-name>  \n"
       + indent9 + "<output-foci-projection-file-name>  \n"
       + indent9 + "<output-focus-name>  \n"
       + indent9 + "<EXTREMUM-DIRECTION>\n"
       + indent9 + "<maximum-x-movement>\n"
       + indent9 + "<maximum-y-movement>\n"
       + indent9 + "<maximum-z-movement>\n"
       + indent9 + "[-create-roi-from-path  roi-file-name.roi] \n"
       + indent9 + "[-place-foci-along-path]\n"
       + indent9 + "[-start-offset  X  Y  Z]\n"
       + indent9 + "\n"
       + indent9 + "Starting at the position of the input focus, move in the\n"
       + indent9 + "specified direction along the node mesh until one can move\n"
       + indent9 + "no further and place a focus at that location.\n"
       + indent9 + "\n"
       + indent9 + "EXTREMUM-DIRECTION is one of:\n"
       + indent9 + "   X-LATERAL\n"
       + indent9 + "   X-MEDIAL\n"
       + indent9 + "   X-NEG\n"
       + indent9 + "   X-POS\n"
       + indent9 + "   Y-NEG\n"
       + indent9 + "   Y-POS\n"
       + indent9 + "   Z-NEG\n"
       + indent9 + "   Z-POS\n"
       + indent9 + "\n"
       + indent9 + "Uses the last focus with \"input-focus-name\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfacePlaceFociAtExtremum::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString fiducialCoordinateFileName =
      parameters->getNextParameterAsString("Input Fiducial Coordinate File Name");
   const QString extremumCoordinateFileName =
      parameters->getNextParameterAsString("Input Extremum Coordinate File Name");
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString inputFociProjectionFileName = 
      parameters->getNextParameterAsString("Input Foci Projection File");
   const QString inputFocusName =
      parameters->getNextParameterAsString("Input Focus Name");
   const QString outputFociProjectionFileName =
      parameters->getNextParameterAsString("Output Foci Projection File Name");
   const QString outputFocusName =
      parameters->getNextParameterAsString("Output Focus Name");
   const QString extremumDirection =
      parameters->getNextParameterAsString("Extremum Direction");

   //
   // set the search direction
   //
   BrainModelSurfaceFindExtremum::DIRECTION searchDirection = 
      BrainModelSurfaceFindExtremum::DIRECTION_INVALID;
   if (extremumDirection == "X-LATERAL") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_LATERAL;
   }
   else if (extremumDirection == "X-MEDIAL") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_MEDIAL;
   }
   else if (extremumDirection == "X-NEG") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_X_NEGATIVE;
   }
   else if (extremumDirection == "X-POS") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_X_POSITIVE;
   }
   else if (extremumDirection == "Y-NEG") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_Y_NEGATIVE;
   }
   else if (extremumDirection == "Y-POS") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_Y_POSITIVE;
   }
   else if (extremumDirection == "Z-NEG") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_Z_NEGATIVE;
   }
   else if (extremumDirection == "Z-POS") {
      searchDirection = BrainModelSurfaceFindExtremum::DIRECTION_Z_POSITIVE;
   }
   else {
      throw CommandException("EXTREMUM DIRECTION value invalid: "
                             + extremumDirection);
   }
   
   //
   // Maximum movements
   //
   const float maximumMovementX = 
      parameters->getNextParameterAsFloat("Maximum X Movement");
   const float maximumMovementY = 
      parameters->getNextParameterAsFloat("Maximum Y Movement");
   const float maximumMovementZ = 
      parameters->getNextParameterAsFloat("Maximum Z Movement");
      
   //
   // Optional append file names
   //
   QString regionOfInterestFileName;
   bool createROIFlag = false;
   
   //
   // Process the parameters for node selection
   //
   bool placeFociAlongPathFlog = false;
   float startOffset[3] = { 0.0, 0.0, 0.0 };
   while (parameters->getParametersAvailable()) {
      //
      // Get the next parameter and process it
      //
      const QString paramName = parameters->getNextParameterAsString("Next Operation");
      if (paramName == "-create-roi-from-path") {
         regionOfInterestFileName = 
            parameters->getNextParameterAsString("Region of Interest File Name");
         createROIFlag = true;
      }
      else if (paramName == "-place-foci-along-path") {
         placeFociAlongPathFlog = true;
      }
      else if (paramName == "-start-offset") {
         startOffset[0] = parameters->getNextParameterAsFloat("Start Offset X");
         startOffset[1] = parameters->getNextParameterAsFloat("Start Offset Y");
         startOffset[2] = parameters->getNextParameterAsFloat("Start Offset X");
      }
      else {
         throw CommandException("Unrecognized operation: "
                                + paramName);
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(inputTopologyFileName,
                     fiducialCoordinateFileName,
                     extremumCoordinateFileName,
                     true);
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurface(0);
   if (fiducialSurface == NULL) {
      throw CommandException("unable to find fiducial surface.");
   }
   BrainModelSurface* extremumSurface = brainSet.getBrainModelSurface(1);
   if (extremumSurface == NULL) {
      if (extremumCoordinateFileName == fiducialCoordinateFileName) {
         extremumSurface = fiducialSurface;
      }
      else {
         throw CommandException("unable to find extremum surface.");
      }
   }
   const TopologyFile* tf = extremumSurface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = extremumSurface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read the input focus projection file
   //
   FociProjectionFile fociProjectionFile;
   fociProjectionFile.readFile(inputFociProjectionFileName);
   
   //
   // Get the focus with the specified name
   //
   const CellProjection* inputFocus = 
      fociProjectionFile.getLastCellProjectionWithName(inputFocusName);
   if (inputFocus == NULL) {
      throw CommandException("Cannot find focus "
                             "with name " + inputFocusName);
   }
   
   //
   // Get position of input focus
   //
   const bool fiducialSurfaceFlag =
      (extremumSurface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   const bool flatSurfaceFlag =
      ((extremumSurface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT) ||
       (extremumSurface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR));
   float inputFocusXYZ[3];
   if (inputFocus->getProjectedPosition(extremumSurface->getCoordinateFile(),
                                        extremumSurface->getTopologyFile(),
                                        fiducialSurfaceFlag,
                                        flatSurfaceFlag,
                                        false,
                                        inputFocusXYZ) == false) {
      throw CommandException("unable to get projected position for "
                             + inputFocusName);
   }
   
   //
   // Add start offset to
   //
   inputFocusXYZ[0] += startOffset[0];
   inputFocusXYZ[1] += startOffset[1];
   inputFocusXYZ[2] += startOffset[2];
   
   //
   // Get the extremum
   //
   BrainModelSurfaceFindExtremum bmsfe(&brainSet,
                                       extremumSurface,
                                       searchDirection,
                                       inputFocusXYZ,
                                       maximumMovementX,
                                       maximumMovementY,
                                       maximumMovementZ);
   bmsfe.execute();
   
   //
   // Get the coordinate of the extremum node
   //
   const int extremumNode = bmsfe.getExtremumNode();
   if (extremumNode < 0) {
      throw CommandException("search for extremum node failed.");
   }
   
   // if (DebugControl::getDebugOn()) {
      const CoordinateFile* extremumCoordFile = extremumSurface->getCoordinateFile();
      const float* extremumXYZ = extremumCoordFile->getCoordinate(extremumNode);
      std::cout << "Extremum (" 
                << outputFocusName.toAscii().constData()
                << ") start: " 
                << inputFocusXYZ[0] << " "
                << inputFocusXYZ[1] << " "
                << inputFocusXYZ[2] << " "
                << " end: "
                << extremumXYZ[0] << " "
                << extremumXYZ[1] << " "
                << extremumXYZ[2] << std::endl;
   //}
             
   const CoordinateFile* fiducialCoordFile = fiducialSurface->getCoordinateFile();
   //
   // Foci projection file
   //
   FociProjectionFile outputFociProjectionFile;
   
   //
   // Add focus projection
   //
   fociProjectionFile.addCellProjection(CellProjection(outputFocusName,
                                                       fiducialCoordFile,
                                                       extremumNode,
                                                       fiducialSurface->getStructure()));
   
   //
   // Place foci along the path from start to extremum node?
   //
   if (placeFociAlongPathFlog) {
      // 
      // Get nodes in path
      //
      std::vector<int> nodesInPath;
      bmsfe.getNodeInPathToExtremum(nodesInPath); 
      
      //
      // Skip start and final nodes
      //
      const int startNode = 1;
      const int endNode = static_cast<int>(nodesInPath.size()) - 1;
      
      for (int i = startNode; i < endNode; i++) {
         //
         // Place a focus at the node in the path
         //
         fociProjectionFile.addCellProjection(CellProjection(outputFocusName,
                                                             fiducialCoordFile,
                                                             nodesInPath[i],
                                                             fiducialSurface->getStructure()));
      }
   }
   
   //
   // Create a region of interest from the path
   //
   if (createROIFlag) {
      BrainModelSurfaceROINodeSelection roi(&brainSet);
      bmsfe.setRegionOfInterestToNodesInPath(roi);
      NodeRegionOfInterestFile roiFile;
      roi.setRegionOfInterestIntoFile(roiFile);
      roiFile.writeFile(regionOfInterestFileName);
   }
   
   //
   // Write the projection file
   //
   fociProjectionFile.writeFile(outputFociProjectionFileName);
   
}

      

