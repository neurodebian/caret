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

#include <algorithm>

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderResample.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderResample::CommandSurfaceBorderResample()
   : CommandBase("-surface-border-resample",
                 "SURFACE BORDER RESAMPLE")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderResample::~CommandSurfaceBorderResample()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderResample::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
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
   paramsOut.addFloat("Sampling Interval", 1.0, 0.000, 100000.0);
   paramsOut.addVariableListOfParameters("Border Resample Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderResample::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-border-projection-file-name>\n"
       + indent9 + "<output-border-projection-file-name>\n"
       + indent9 + "<resampling-interval>\n"
       + indent9 + "[-all]\n"
       + indent9 + "[-border-name  border-name]\n"
       + indent9 + "\n"
       + indent9 + "Resample borders.\n"
       + indent9 + "\n"
       + indent9 + "Either \"-all\", which resample all borders in the file, or\n"
       + indent9 + "\"-border-name\", which resamples borders with specific names\n"
       + indent9 + "MUST be specified.  \"-border-name\" may be used multiple\n"
       + indent9 + "times to resample additional borders.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderResample::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputBorderProjectionFileName =
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName =
      parameters->getNextParameterAsString("Output Border Projection File Name");
   const float resamplingInterval = 
      parameters->getNextParameterAsFloat("Resampling Interval");
      
   // 
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
   // Check optional parameters
   //
   bool resampleAllFlag = false;
   std::vector<QString> borderNames;
   QString roiFileName;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("parameter");
      if (paramName == "-all") {
         resampleAllFlag = true;;
      }
      else if (paramName == "-border-name") {
         const QString nameString = 
            parameters->getNextParameterAsString("border name");
         borderNames.push_back(nameString);
      }
      else {
         throw CommandException("unknown parameter: "
                                + paramName);
      }
   }

   const int numberOfBorderNames = static_cast<int>(borderNames.size());
   if ((resampleAllFlag == false) &&
       (numberOfBorderNames <= 0)) {
      throw CommandException("either \"-all\" or "
                             "\"-border-name\" must be specified to indicate "
                             "which borders should be resampled.");
   }
   
   //
   // Read input file
   //
   BorderProjectionFile inputBorderProjectionFile;
   inputBorderProjectionFile.readFile(inputBorderProjectionFileName);
   
   BorderProjectionFile outputBorderProjectionFile;
   outputBorderProjectionFile.setHeader(inputBorderProjectionFile.getHeader());

   //
   // Loop through the border projections
   //
   const int num = inputBorderProjectionFile.getNumberOfBorderProjections();
   for (int i = 0; i < num; i++) {
      //
      // Get the border projection
      //
      BorderProjection* bp = inputBorderProjectionFile.getBorderProjection(i);
      
      //
      // Determine if the border projection should be resampled
      //
      bool resampleIt = false;
      if (resampleAllFlag) {
         resampleIt = true;
      }
      else {
         if (std::find(borderNames.begin(),
                       borderNames.end(),
                       bp->getName()) != borderNames.end()) {
            resampleIt = true;
         }
      }
      
      //
      // Should border projection be resampled
      //
      if (resampleIt) {
         //
         // Place the border projection in a temporary file
         //
         BorderProjectionFile tempBorderProjectionFile;
         tempBorderProjectionFile.addBorderProjection(*bp);
         
         //
         // Unproject the border
         //
         BorderProjectionUnprojector unprojector;
         BorderFile tempBorderFile;
         unprojector.unprojectBorderProjections(*cf,
                                                tempBorderProjectionFile,
                                                tempBorderFile);
                                                
         //
         // Get border
         //
         if (tempBorderFile.getNumberOfBorders() <= 0) {
            throw CommandException("unprojection error.");
         }
         
         //
         // Resample the border
         //
         Border* b = tempBorderFile.getBorder(0);
         int newNumLinks = 0;
         b->resampleBorderToDensity(resamplingInterval, 2, newNumLinks);
         
         //
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
         outputBorderProjectionFile.addBorderProjection(
                        *tempBorderProjectionFile.getBorderProjection(0));
      }
      else {
         //
         // Add border projection to output file
         //
         outputBorderProjectionFile.addBorderProjection(*bp);
      }
   }
   
   //
   // Write output border projection file
   //
   outputBorderProjectionFile.writeFile(outputBorderProjectionFileName);
}

      

