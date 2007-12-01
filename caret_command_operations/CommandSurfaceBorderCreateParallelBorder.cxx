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

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderCreateParallelBorder.h"
#include "CoordinateFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderCreateParallelBorder::CommandSurfaceBorderCreateParallelBorder()
   : CommandBase("-surface-border-create-parallel-border",
                 "SURFACE BORDER CREATE PARALLEL BORDER")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderCreateParallelBorder::~CommandSurfaceBorderCreateParallelBorder()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderCreateParallelBorder::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addString("Input Border Name");
   paramsOut.addString("Output Parallel Border Name");
   paramsOut.addVariableListOfParameters("Axes Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderCreateParallelBorder::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-border-file-name>\n"
       + indent9 + "<output-border-file-name>\n"
       + indent9 + "<input-border-name>\n"
       + indent9 + "<output-parallel-border-name>\n"
       + indent9 + "[-separation-axis  AXIS-NAME  AXIS-VALUE]\n"
       + indent9 + "\n"
       + indent9 + "Create a new border that is parallel to an existing border.\n"
       + indent9 + "\n"
       + indent9 + "\"AXIS-NAME\" is one of:\n"
       + indent9 + "   X\n"
       + indent9 + "   Y\n"
       + indent9 + "   Z\n"
       + indent9 + "   LATERAL\n"
       + indent9 + "   MEDIAL\n"
       + indent9 + "\n"
       + indent9 + "Multiple axes may be specified.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderCreateParallelBorder::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get coord, topo, and roi file names
   //
   const QString coordFileName = 
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString topoFileName = 
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString inputBorderProjectionFileName =
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName =
      parameters->getNextParameterAsString("Output Border Projection File Name");
   const QString inputBorderName =
      parameters->getNextParameterAsString("Input Border Name");
   const QString outputBorderName =
      parameters->getNextParameterAsString("Output Parallel Border Name");
   std::vector<QString> axesNames;
   std::vector<float> axesValues;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Parallel Border Axes");
      if (paramName == "-separation-axis") {
         axesNames.push_back(parameters->getNextParameterAsString("AXIS-NAME"));
         axesValues.push_back(parameters->getNextParameterAsFloat("AXIS-VALUE"));
      }
      else {
         throw CommandException("Unrecognized option: " + paramName);
      }
   }
   
   //
   // Check input data
   //
   const int numAxes = static_cast<int>(axesNames.size());
   if (numAxes <= 0) {
      throw CommandException("No axes were specified.");
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topoFileName,
                     coordFileName,
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
   // Process the offsets
   //
   float offset[3] = { 0.0, 0.0, 0.0 };
   for (int i = 0; i < numAxes; i++) {
      const QString separationAxisName = axesNames[i];
      const float   separationValue    = axesValues[i];
      if (separationAxisName == "X") {
         offset[0] += separationValue;
      }
      else if (separationAxisName == "Y") {
         offset[1] += separationValue;
      }
      else if (separationAxisName == "Z") {
         offset[2] += separationValue;
      }
      else if (separationAxisName == "LATERAL") {
         if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
            offset[0] -= separationValue;
         }
         else if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
            offset[0] += separationValue;
         }
         else {
            throw CommandException("Structure is neither right nor left for for "
                                   "lateral offset.");
         }
      }
      else if (separationAxisName == "MEDIAL") {
         if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
            offset[0] += separationValue;
         }
         else if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
            offset[0] -= separationValue;
         }
         else {
            throw CommandException("Structure is neither right nor left for for "
                                   "lateral offset.");
         }
      }
      else {
         throw CommandException("Separation Axis \""
                                + separationAxisName
                                + "\" is invalid.");
      }
   }
   
   //
   // Read the input border projection file
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(inputBorderProjectionFileName);
   
   //
   // Find the input border
   //
   BorderProjection* borderProjection = 
      borderProjectionFile.getLastBorderProjectionByName(inputBorderName);
   if (borderProjection == NULL) {
      throw CommandException("Unable to find border projection named \""
                             + inputBorderName
                             +"\"");
   }

   //
   // Place the border projection in a temporary file
   //
   BorderProjectionFile tempBorderProjectionFile;
   tempBorderProjectionFile.addBorderProjection(*borderProjection);
   
   //
   // Unproject the border
   //
   BorderProjectionUnprojector unprojector;
   BorderFile tempBorderFile;
   unprojector.unprojectBorderProjections(*(bms->getCoordinateFile()),
                                          tempBorderProjectionFile,
                                          tempBorderFile);
    
   //
   // Get border
   //
   if (tempBorderFile.getNumberOfBorders() <= 0) {
      throw CommandException("unprojection error.");
   }
   
   //
   // Get the border
   //
   Border* border = tempBorderFile.getBorder(0);
   
   //
   // Make the parallel border
   //
   border->setName(outputBorderName);
   for (int i = 0; i < border->getNumberOfLinks(); i++) {
      float xyz[3];
      border->getLinkXYZ(i, xyz);
      xyz[0] += offset[0];
      xyz[1] += offset[1];
      xyz[2] += offset[2];
      border->setLinkXYZ(i, xyz);
   }
   
   // Reproject the border
   //
   tempBorderProjectionFile.clear();
   BorderFileProjector projector(bms, true);
   projector.projectBorderFile(&tempBorderFile,
                               &tempBorderProjectionFile,
                               NULL);

   //
   // Add border projection to output file
   //
   if (tempBorderProjectionFile.getNumberOfBorderProjections() <= 0) {
      throw CommandException("reprojection error.");
   }
   
   borderProjectionFile.addBorderProjection(
                  *tempBorderProjectionFile.getBorderProjection(0));
                  
   //
   // Write the output file
   //
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
}

      

