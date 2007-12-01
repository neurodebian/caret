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

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderNibbler.h"
#include "FileFilters.h"
#include "FociProjectionFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderNibbler::CommandSurfaceBorderNibbler()
   : CommandBase("-surface-border-nibbler",
                 "SURFACE BORDER NIBBLER")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderNibbler::~CommandSurfaceBorderNibbler()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderNibbler::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addString("Border Name");
   paramsOut.addFile("Foci Projection File Name", 
                     FileFilters::getFociProjectionFileFilter());
   paramsOut.addString("Focus Name");
   paramsOut.addVariableListOfParameters("Border Nibble Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderNibbler::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-border-projection-file-name>\n"
       + indent9 + "<output-border-projection-file-name>\n"
       + indent9 + "<input-border-name>\n"
       + indent9 + "output-border-name>\n"
       + indent9 + "<foci-projection-file>\n"
       + indent9 + "<focus-name>\n"
       + indent9 + "[-greater-than-x  x-offset] \n"
       + indent9 + "[-greater-than-y  y-offset] \n"
       + indent9 + "[-greater-than-z  z-offset] \n"
       + indent9 + "[-less-than-x  x-offset] \n"
       + indent9 + "[-less-than-y  y-offset] \n"
       + indent9 + "[-less-than-z  z-offset] \n"
       + indent9 + "[-remove-after] \n"
       + indent9 + "[-remove-before] \n"
       + indent9 + "[-within-x-distance  x-distance]\n"
       + indent9 + "[-within-y-distance  y-distance]\n"
       + indent9 + "[-within-z-distance  z-distance]\n"
       + indent9 + "[-within-linear-distance  linear-distance]\n"
       + indent9 + "\n"
       + indent9 + "When specifying a \"within\" distance, any borders points\n"
       + indent9 + "whose distance is LESS THAN a \"linear distance\" from the\n"
       + indent9 + "focus are removed.\n"
       + indent9 + "\n"
       + indent9 + "\"-greater-than-X/Y/Z\" removes border points whose X/Y/Z-coordinate\n"
       + indent9 + "is greater than the sum of X/Y/Z-coordinate of the focus plus the \n"
       + indent9 + "\"x/y/z-offset\". \n"
       + indent9 + "\n"
       + indent9 + "\"-less-than-X/Y/Z\" removes border points whose X/Y/Z-coordinate\n"
       + indent9 + "is less than the sum of X/Y/Z-coordinate of the focus plus the \n"
       + indent9 + "\"x/y/z-offset\". \n"
       + indent9 + "\n"
       + indent9 + "\"-remove-after\" removes all links from the border starting\n"
       + indent9 + "at the link closest to the focus to the end of the border.\n"
       + indent9 + "\n"
       + indent9 + "\"-remove-before\" removes all links from the start of the border\n"
       + indent9 + "to the border link closest to the focus.\n"
       + indent9 + "\n"
       + indent9 + "If the input and output border names are different, a new \n"
       + indent9 + "border is created.  \n"
       + indent9 + "\n"
       + indent9 + "Uses the last focus with \"focus-name\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderNibbler::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputCoordinateFileName =
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString inputBorderProjectionFileName =
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName =
      parameters->getNextParameterAsString("Output Border Projection File Name");
   const QString inputBorderName =
      parameters->getNextParameterAsString("Input Border Name");
   const QString outputBorderName =
      parameters->getNextParameterAsString("Output Border Name");
   const QString inputFociProjectionFileName =
      parameters->getNextParameterAsString("Input Foci Projection File Name");
   const QString inputFocusName =
      parameters->getNextParameterAsString("Input Focus Name");

   bool lessGreaterThanFlag = false;
   float greaterThanOffset[3] = { 0.0, 0.0, 0.0 };
   bool greaterThanOffsetValid[3] = { false, false, false };
   float lessThanOffset[3] = { 0.0, 0.0, 0.0 };
   bool lessThanOffsetValid[3] = { false, false, false };
   float withinXDistance = -1.0;
   float withinYDistance = -1.0;
   float withinZDistance = -1.0;
   float withinLinearDistance = -1.0;
   bool withinDistanceFlag = false;
   bool removeAfterFlag = false;
   bool removeBeforeFlag = false;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Distance Parameter");
      if (paramName == "-greater-than-x") {
         greaterThanOffset[0] = parameters->getNextParameterAsFloat("Greater Than X-Offset");
         greaterThanOffsetValid[0] = true;
         lessGreaterThanFlag = true;
      }
      else if (paramName == "-greater-than-y") {
         greaterThanOffset[1] = parameters->getNextParameterAsFloat("Greater Than Y-Offset");
         greaterThanOffsetValid[1] = true;
         lessGreaterThanFlag = true;
      }
      else if (paramName == "-greater-than-z") {
         greaterThanOffset[2] = parameters->getNextParameterAsFloat("Greater Than Z-Offset");
         greaterThanOffsetValid[2] = true;
         lessGreaterThanFlag = true;
      }
      else if (paramName == "-less-than-x") {
         lessThanOffset[0] = parameters->getNextParameterAsFloat("Less Than X-Offset");
         lessThanOffsetValid[0] = true;
         lessGreaterThanFlag = true;
      }
      else if (paramName == "-less-than-y") {
         lessThanOffset[1] = parameters->getNextParameterAsFloat("Less Than Y-Offset");
         lessThanOffsetValid[1] = true;
         lessGreaterThanFlag = true;
      }
      else if (paramName == "-less-than-z") {
         lessThanOffset[2] = parameters->getNextParameterAsFloat("Less Than Z-Offset");
         lessThanOffsetValid[2] = true;
         lessGreaterThanFlag = true;
      }
      else if (paramName == "-remove-after") {
         removeAfterFlag = true;
      }
      else if (paramName == "-remove-before") {
         removeBeforeFlag = true;
      }
      else if (paramName == "-within-x-distance") {
         withinXDistance = parameters->getNextParameterAsFloat("Within-X-Distance");
         if (withinXDistance < 0.0) {
            throw CommandException("Within X Distance must be greater than or equal to zero.");
         }
         withinDistanceFlag = true;
      }
      else if (paramName == "-within-y-distance") {
         withinYDistance = parameters->getNextParameterAsFloat("Within-Y-Distance");
         if (withinYDistance < 0.0) {
            throw CommandException("Within Y Distance must be greater than or equal to zero.");
         }
         withinDistanceFlag = true;
      }
      else if (paramName == "-within-z-distance") {
         withinZDistance = parameters->getNextParameterAsFloat("Within-Z-Distance");
         if (withinZDistance < 0.0) {
            throw CommandException("Within Z Distance must be greater than or equal to zero.");
         }
         withinDistanceFlag = true;
      }
      else if (paramName == "-within-linear-distance") {
         withinLinearDistance = parameters->getNextParameterAsFloat("Within-Linear-Distance");
         if (withinLinearDistance < 0.0) {
            throw CommandException("Within Linear Distance must be greater than or equal to zero.");
         }
         withinDistanceFlag = true;
      }
      else {
         throw CommandException("Unrecognized distance: "
                                + paramName);
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(inputTopologyFileName,
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
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(inputBorderProjectionFileName);
   
   //
   // Read the foci projection file
   //
   FociProjectionFile inputFociProjectionFile;
   inputFociProjectionFile.readFile(inputFociProjectionFileName);
   
   //
   // Get the focus with the specified name
   //
   const CellProjection* inputFocus = 
      inputFociProjectionFile.getLastCellProjectionWithName(inputFocusName);
   if (inputFocus == NULL) {
      throw CommandException("Cannot find focus "
                             "with name " + inputFocusName);
   }
   
   //
   // Get position of input focus
   //
   const bool fiducialSurfaceFlag =
      (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   const bool flatSurfaceFlag =
      ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT) ||
       (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR));
   float inputFocusXYZ[3];
   if (inputFocus->getProjectedPosition(bms->getCoordinateFile(),
                                        bms->getTopologyFile(),
                                        fiducialSurfaceFlag,
                                        flatSurfaceFlag,
                                        false,
                                        inputFocusXYZ) == false) {
      throw CommandException("unable to get projected position for "
                             + inputFocusName);
   }
   
   //
   // Get the border with the specified name
   //
   BorderProjection* bp = borderProjectionFile.getLastBorderProjectionByName(inputBorderName);
   if (bp == NULL) {
      throw CommandException("Unable to find border named \""
                             + inputBorderName
                             + "\"");
   }

   //
   // If the input and output border names are different, create a new border
   //
   if (inputBorderName != outputBorderName) {
      borderProjectionFile.addBorderProjection(*bp);
      bp = borderProjectionFile.getBorderProjection(borderProjectionFile.getNumberOfBorderProjections() - 1);
      bp->setName(outputBorderName);
   }
   
   //
   // nibble the border projection
   //
   if (withinDistanceFlag) {
      bp->removeLinksNearPoint(bms->getCoordinateFile(),
                               inputFocusXYZ,
                               withinXDistance,
                               withinYDistance,
                               withinZDistance,
                               withinLinearDistance);
   }
   
   //
   // Do nibbling of border points outside extent
   //
   if (lessGreaterThanFlag) {
      float extent[6] = {
         -std::numeric_limits<float>::max(),
         std::numeric_limits<float>::max(),
         -std::numeric_limits<float>::max(),
         std::numeric_limits<float>::max(),
         -std::numeric_limits<float>::max(),
         std::numeric_limits<float>::max()
      };
      if (greaterThanOffsetValid[0]) {
         extent[1] = inputFocusXYZ[0] + greaterThanOffset[0];
      }
      if (greaterThanOffsetValid[1]) {
         extent[3] = inputFocusXYZ[1] + greaterThanOffset[1];
      }
      if (greaterThanOffsetValid[2]) {
         extent[5] = inputFocusXYZ[2] + greaterThanOffset[2];
      }
      if (lessThanOffsetValid[0]) {
         extent[0] = inputFocusXYZ[0] + lessThanOffset[0];
      }
      if (lessThanOffsetValid[1]) {
         extent[2] = inputFocusXYZ[1] + lessThanOffset[1];
      }
      if (lessThanOffsetValid[0]) {
         extent[4] = inputFocusXYZ[2] + lessThanOffset[2];
      }
      bp->removeLinksOutsideExtent(bms->getCoordinateFile(),
                                   extent);
   }
   
   //
   // Remove before and after
   //
   if (removeAfterFlag ||
       removeBeforeFlag) {
      bp->removeLinksBeforeAfterLinkNearestPoint(bms->getCoordinateFile(),
                                                 inputFocusXYZ,
                                                 removeAfterFlag,
                                                 removeBeforeFlag);
   }
   
   //
   // Write the border projection file
   //
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
}

      

