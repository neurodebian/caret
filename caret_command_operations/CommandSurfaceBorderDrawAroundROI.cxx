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

#include <QFile>

#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceClusterToBorderConverter.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderDrawAroundROI.h"
#include "FileFilters.h"
#include "FociProjectionFile.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderDrawAroundROI::CommandSurfaceBorderDrawAroundROI()
   : CommandBase("-surface-border-draw-around-roi",
                 "SURFACE BORDER DRAW AROUND ROI")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderDrawAroundROI::~CommandSurfaceBorderDrawAroundROI()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderDrawAroundROI::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Region of Interest File Name", FileFilters::getRegionOfInterestFileFilter());
   paramsOut.addFile("Input Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addString("Border Name");
   paramsOut.addVariableListOfParameters("Draw Border ROI Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderDrawAroundROI::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<region-of-interest-file-name>\n"
       + indent9 + "<input-border-projection-file-name>\n"
       + indent9 + "<output-border-projection-file-name>\n"
       + indent9 + "<border-name>\n"
       + indent9 + "[-output-all-borders]\n"
       + indent9 + "[-start-near-focus  foci-projection-file-name  focus-name]\n"
       + indent9 + "\n"
       + indent9 + "Draw a border around a surface region of interest.  \n"
       + indent9 + "\n"
       + indent9 + "By default, if there is more than one disjoint region in\n"
       + indent9 + "the ROI, only the border with the greatest number of links\n"
       + indent9 + "is output. \n"
       + indent9 + "\n"
       + indent9 + "If the \"-output-all-borders\" option is specified, the\n"
       + indent9 + " borders for ALL disjoint regions are output.\n"
       + indent9 + "\n"
       + indent9 + "If the \"-start-near-focus\" option is specified, all \n"
       + indent9 + "borders are set up so that the first link in the border is\n"
       + indent9 + "the link nearest the focus.  This option is only applied if\n"
       + indent9 + "\"-output-all-borders\" is NOT specified.\n"
       + indent9 + "\n"
       + indent9 + "Note: The input border projection file does not need to exist.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderDrawAroundROI::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString roiFileName = 
      parameters->getNextParameterAsString("Input Region of Interest File Name");
   const QString inputBorderProjectionFileName =
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName =
      parameters->getNextParameterAsString("Output Border Projection File Name");
   const QString borderName =
      parameters->getNextParameterAsString("Border Name");

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
   const CoordinateFile* cf = bms->getCoordinateFile();
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains not nodes.");
   }
   
   //
   // Optional starting location of border
   //
   float startXYZ[3];
   bool startXYZValid = false;
   
   //
   // Check options parameters
   // 
   bool outputAllBordersFlag = false;  
   while (parameters->getParametersAvailable()) {
      const QString paramName(parameters->getNextParameterAsString("Draw Border Option"));
      if (paramName == "-start-near-focus") {
         const QString fociProjectionFileName =
            parameters->getNextParameterAsString("Foci Projection File Name");
         const QString focusName =
            parameters->getNextParameterAsString("Focus Name");
         
         FociProjectionFile fociProjectionFile;
         fociProjectionFile.readFile(fociProjectionFileName);
         const CellProjection* focus = 
            fociProjectionFile.getLastCellProjectionWithName(focusName);
         if (focus == NULL) {
            throw CommandException("Focus named " + focusName +
                                   " not found in file " + fociProjectionFileName);
         }
         
         if (focus->getProjectedPosition(cf,
                                         tf,
                                         bms->getIsFiducialSurface(),
                                         bms->getIsFlatSurface(),
                                         false,
                                         startXYZ)) {
            startXYZValid = true;
         }
      }
      else if (paramName == "-output-all-borders") {
         outputAllBordersFlag = true;
      }
      else {
         throw CommandException("Unrecognized option: " + paramName);
      }
   }
   
   //
   // Get the region of interest node selection
   //
   BrainModelSurfaceROINodeSelection* roi = 
      brainSet.getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->deselectAllNodes();
   
   //
   // Read input ROI file
   //
   NodeRegionOfInterestFile roiFile;
   roiFile.readFile(roiFileName);
   roi->getRegionOfInterestFromFile(roiFile);
   if (roi->getNumberOfNodesSelected() <= 0) {
      throw CommandException("No nodes are selected in the ROI.");
   }
   
   //
   // Create borders around ROI clusters
   //
   BrainModelSurfaceClusterToBorderConverter roiToBorders(&brainSet,
                                                          bms,
                                                          bms->getTopologyFile(),
                                                          borderName,
                                                          roi,
                                                          true);
   roiToBorders.execute();
   
   //
   // Get the borders that were created
   //
   BrainModelBorderSet* bmbs = brainSet.getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   if (numBorders <= 0) {
      throw CommandException("No borders were created around the ROI.");
   }
   
   //
   // Get new borders as border projection
   //
   BorderProjectionFile newBorderProjectionFile;
   bmbs->copyBordersToBorderProjectionFile(newBorderProjectionFile);
   
   //
   // Read input border projection file
   //
   BorderProjectionFile outputBorderProjectionFile;
   if (inputBorderProjectionFileName.isEmpty() == false) {
      if (QFile::exists(inputBorderProjectionFileName)) {
         outputBorderProjectionFile.readFile(inputBorderProjectionFileName);
      }
   }
      
   //
   // Output all borders ?
   //
   if (outputAllBordersFlag) {
      //
      // Append new borders
      //
      outputBorderProjectionFile.append(newBorderProjectionFile);
   }
   else {
      //
      // Get border projection with the largest number of links
      //
      BorderProjection* borderProjection =
         newBorderProjectionFile.getBorderProjectionWithLargestNumberOfLinks();
        
      //
      // Should border start in a specified location
      //
      if (startXYZValid) {
         borderProjection->changeStartingLinkOfClosedBorderToBeNearPoint(cf, startXYZ);
      }
      
      //
      // Append new projections
      //
      outputBorderProjectionFile.addBorderProjection(*borderProjection);
   }
   
   //
   // Write border projections
   //
   outputBorderProjectionFile.writeFile(outputBorderProjectionFileName);
}

      

