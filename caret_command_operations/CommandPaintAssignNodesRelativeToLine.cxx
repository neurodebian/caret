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
#include "BrainModelSurfacePaintAssignRelativeToLine.h"
#include "BrainSet.h"
#include "CommandPaintAssignNodesRelativeToLine.h"
#include "FileFilters.h"
#include "FociProjectionFile.h"
#include "MathUtilities.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPaintAssignNodesRelativeToLine::CommandPaintAssignNodesRelativeToLine()
   : CommandBase("-paint-assign-nodes-relative-to-line",
                 "PAINT ASSIGN NODES RELATIVE TO LINE")
{
}

/**
 * destructor.
 */
CommandPaintAssignNodesRelativeToLine::~CommandPaintAssignNodesRelativeToLine()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandPaintAssignNodesRelativeToLine::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Foci Projection File Name", FileFilters::getFociProjectionFileFilter());
   paramsOut.addString("Name of Focus at Line Start");
   paramsOut.addString("Name of Focus at Line End");
   paramsOut.addFile("Input Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addString("Paint File Column");
   paramsOut.addString("Region Paint Name");
   paramsOut.addString("New Paint Name");
   paramsOut.addFloat("Minimum Distance From Line");
   paramsOut.addFloat("Maximum Distance From Line");
   paramsOut.addVariableListOfParameters("Line Info");
}

      
/**
 * get full help information.
 */
QString 
CommandPaintAssignNodesRelativeToLine::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<foci-projection-file-name>\n"
       + indent9 + "<name-of-focus-at-line-start>\n"
       + indent9 + "<name-of-focus-at-line-end>\n"
       + indent9 + "<input-paint-file-name>\n"
       + indent9 + "<output-paint-file-name>\n"
       + indent9 + "<paint-file-column>\n"
       + indent9 + "<region-paint-name> \n"
       + indent9 + "<new-paint-name>\n"
       + indent9 + "<minimum-distance-from-line> \n"
       + indent9 + "<maximum-distance-from-line> \n"
       + indent9 + "[-limit-to-nodes-with-current-paint-name  name]\n"
       + indent9 + "[-node-x-min  value]\n"
       + indent9 + "[-node-x-max  value]\n"
       + indent9 + "[-node-y-min  value]\n"
       + indent9 + "[-node-y-max  value]\n"
       + indent9 + "[-node-z-min  value]\n"
       + indent9 + "[-node-z-max  value\n"
       + indent9 + "\n"
       + indent9 + "Create an infinite line that goes through the two specified\n"
       + indent9 + "foci.  Assign the new paint name to nodes that are within\n"
       + indent9 + "the minimum and maximum distances on the right side of the \n"
       + indent9 + "line.  Options limit the nodes assigned to those with an   \n"
       + indent9 + "existing paint name and those nodes whose coordinates are \n"
       + indent9 + "within geometric specified limits.  The initial nodes are\n"
       + indent9 + "those adjacent to \"region-paint-name\" and this regions\n"
       + indent9 + "is incrementally expanded until there no additional nodes\n"
       + indent9 + "have their paint set.\n"
       + indent9 + "\n"
       + indent9 + "\"paint-file-column\" is either the number of the column, \n"
       + indent9 + "which starts at one, or the name of the column.  If a\n"
       + indent9 + "name contains spaces, it must be enclosed in double\n"
       + indent9 + "quotes.  Name has priority over number.\n"
       + indent9 + "\n"
       + indent9 + "The input paint file must exist.  If the input paint column\n"
       + indent9 + "is not found and its name is non-numeric, a new column will \n"
       + indent9 + "be added to the paint file if the paint file contains nodes.\n"
       + indent9 + "\n"
       + indent9 + "Uses the last foci with specified names.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandPaintAssignNodesRelativeToLine::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString fociProjectionFileName =
      parameters->getNextParameterAsString("Projection File Name");
   const QString focusLineStartName =
      parameters->getNextParameterAsString("Focus Name for Line Start");
   const QString focusLineEndName =
      parameters->getNextParameterAsString("Focus Name for Line End");
   const QString inputPaintFileName =
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName =
      parameters->getNextParameterAsString("Output Paint File Name");
   const QString paintFileColumn =
      parameters->getNextParameterAsString("Paint File Column");
   const QString regionPaintName =
      parameters->getNextParameterAsString("Region Paint Name");
   const QString newPaintName =
      parameters->getNextParameterAsString("New Paint Name");
   const float minimumDistanceFromLine =
      parameters->getNextParameterAsFloat("Minimum Distance From Line");
   const float maximumDistanceFromLine =
      parameters->getNextParameterAsFloat("Maximum Distance From Line");
     
   //
   // Optional parameter names
   //
   std::vector<QString> existingPaintNames;
   float xMin = -std::numeric_limits<float>::max();
   float xMax =  std::numeric_limits<float>::max();
   float yMin = -std::numeric_limits<float>::max();
   float yMax =  std::numeric_limits<float>::max();
   float zMin = -std::numeric_limits<float>::max();
   float zMax =  std::numeric_limits<float>::max();

   //
   // Get optional parameters
   //
   while (parameters->getParametersAvailable()) {
      const QString paramName = 
         parameters->getNextParameterAsString("ASSIGN PAINT RELATIVE TO LINE parameter");
      if (paramName == "-limit-to-nodes-with-current-paint-name") {
         existingPaintNames.push_back(
            parameters->getNextParameterAsString("Existing paint name"));
      }
      else if (paramName == "-node-x-min") {
         xMin = parameters->getNextParameterAsFloat("Node X Minimum");
      }
      else if (paramName == "-node-x-max") {
         xMax = parameters->getNextParameterAsFloat("Node X Maximum");
      }
      else if (paramName == "-node-y-min") {
         yMin = parameters->getNextParameterAsFloat("Node Y Minimum");
      }
      else if (paramName == "-node-y-max") {
         yMax = parameters->getNextParameterAsFloat("Node Y Maximum");
      }
      else if (paramName == "-node-z-min") {
         zMin = parameters->getNextParameterAsFloat("Node Z Minimum");
      }
      else if (paramName == "-node-z-max") {
         zMax = parameters->getNextParameterAsFloat("Node Z Maximum");
      }
      else {
         throw CommandException("unexpected "
                                "parameter: "
                                + paramName);
      }
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
   const CoordinateFile* cf = surface->getCoordinateFile();
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read the input focus projection file
   //
   FociProjectionFile fociProjectionFile;
   fociProjectionFile.readFile(fociProjectionFileName);
   
   //
   // Get the foci for starting and ending the line
   //
   const CellProjection* focusStart = 
      fociProjectionFile.getLastCellProjectionWithName(focusLineStartName);
   if (focusStart == NULL) {
      throw CommandException("Cannot find focus "
                             "for start of line " + focusLineStartName);
   }
   const CellProjection* focusEnd = 
      fociProjectionFile.getLastCellProjectionWithName(focusLineEndName);
   if (focusEnd == NULL) {
      throw CommandException("Cannot find focus "
                             "for end of line " + focusLineEndName);
   }
   
   //
   // Unproject the Foci
   //
   const bool fiducialSurfaceFlag =
      (surface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   const bool flatSurfaceFlag =
      ((surface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT) ||
       (surface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR));
   float focusStartXYZ[3];
   focusStart->getProjectedPosition(cf,
                                    tf,
                                    fiducialSurfaceFlag,
                                    flatSurfaceFlag,
                                    false,
                                    focusStartXYZ);
   float focusEndXYZ[3];
   focusEnd->getProjectedPosition(cf,
                                    tf,
                                    fiducialSurfaceFlag,
                                    flatSurfaceFlag,
                                    false,
                                    focusEndXYZ);
   //
   // Open paint file and find column
   //
   PaintFile paintFile;
   paintFile.readFile(inputPaintFileName);
   //
   // Get the column number
   //
   const int paintFileColumnNumber = paintFile.getColumnFromNameOrNumber(paintFileColumn, true);
   const int regionPaintNameIndex = paintFile.getPaintIndexFromName(regionPaintName);
   if (regionPaintNameIndex < 0) {
      throw CommandException("Paint name "
                             + regionPaintName
                             + " not found in paint file.");
   }
   const int newPaintNameIndex = paintFile.addPaintName(newPaintName);
   
   //
   // Find indices of existing paint names
   //
   std::vector<int> existingPaintIndices;
   const int numExistingPaintNames = static_cast<int>(existingPaintNames.size());
   for (int i = 0; i < numExistingPaintNames; i++) {
      const int indx = paintFile.getPaintIndexFromName(existingPaintNames[i]);
      if (indx < 0) {
         throw CommandException("existing paint "
                                "name not found "
                                + existingPaintNames[i]);
      }
      existingPaintIndices.push_back(indx);
   }
 
   //
   // Assign nodes
   //  
   const float nodeExtent[6] = {
      xMin,
      xMax,
      yMin,
      yMax,
      zMin,
      zMax
   };
   BrainModelSurfacePaintAssignRelativeToLine paintAssign(&brainSet,
                                                          surface,
                                                          &paintFile,
                                                          paintFileColumnNumber,
                                                          regionPaintNameIndex,
                                                          newPaintNameIndex,
                                                          focusStartXYZ,
                                                          focusEndXYZ,
                                                          minimumDistanceFromLine,
                                                          maximumDistanceFromLine,
                                                          existingPaintIndices,
                                                          nodeExtent);
  
   paintAssign.execute();

   //
   // Write paint file
   //
   paintFile.writeFile(outputPaintFileName);
}


