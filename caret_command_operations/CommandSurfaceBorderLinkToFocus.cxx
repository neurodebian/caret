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
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CellFileProjector.h"
#include "CommandSurfaceBorderLinkToFocus.h"
#include "FileFilters.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderLinkToFocus::CommandSurfaceBorderLinkToFocus()
   : CommandBase("-surface-border-link-to-focus",
                 "SURFACE BORDER LINK TO FOCUS")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderLinkToFocus::~CommandSurfaceBorderLinkToFocus()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderLinkToFocus::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addString("Border Name");
   paramsOut.addFile("Input Foci Projection File Name", FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Output Foci Projection File Name", FileFilters::getFociProjectionFileFilter());
   paramsOut.addVariableListOfParameters("Border Link to Focus Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderLinkToFocus::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<border-projection-file-name>\n"
       + indent9 + "<border-name>\n"
       + indent9 + "<input-foci-projection-file-name>\n"
       + indent9 + "<output-foci-projection-file-name>\n"
       + indent9 + "[-first-link  focus-name]\n"
       + indent9 + "[-last-link   focus-name]\n"
       + indent9 + "\n"
       + indent9 + "Create foci at border link locations.\n"
       + indent9 + "\n"
       + indent9 + "Note: The input foci projection file need not exist.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderLinkToFocus::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString borderProjectionFileName = 
      parameters->getNextParameterAsString("Border Projection File Name");
   const QString borderName = 
      parameters->getNextParameterAsString("Border Name");
   const QString inputFociProjectionFileName = 
      parameters->getNextParameterAsString("Input Foci Projection File Name");
   const QString outputFociProjectionFileName = 
      parameters->getNextParameterAsString("Output Foci Projection File Name");

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
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Surface MUST be fiducial for projection to work in some cases
   //
   surface->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   
   //
   // Read the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(borderProjectionFileName);
   
   //
   // Find the border projection and place it in a file by itself
   // so only this border is unprojected
   //
   const BorderProjection* bp = borderProjectionFile.getLastBorderProjectionByName(borderName);
   if (bp == NULL) {
      throw CommandException("Unable to find border named: " + borderName);
   }
   BorderProjectionFile tempBorderProjFile;
   tempBorderProjFile.addBorderProjection(*bp);
   
   //
   // Unproject the borders
   //
   BorderFile borderFile;
   BorderProjectionUnprojector borderUnprojector;
   borderUnprojector.unprojectBorderProjections(*(surface->getCoordinateFile()),
                                                tempBorderProjFile,
                                                borderFile);
   const Border* border = borderFile.getBorder(0);
   if (border == NULL) {
      throw CommandException("border unprojection failed.");
   }
   if (border->getNumberOfLinks() <= 0) {
      throw CommandException("Border named \""
                             + borderName
                             + " has no links after unprojection.");
   }
  
   //
   // File for new foci
   // 
   FociFile fociFile;
   
   //
   // Search optional parameters
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get the next parameter and process it
      //
      const QString paramName = parameters->getNextParameterAsString("Border Link to Focus");
      if (paramName == "-first-link") {
         const QString focusName = parameters->getNextParameterAsString("Focus Name");
         const float* xyz = border->getLinkXYZ(0);
         
         // Create a new focus
         //
         CellData focus(focusName, xyz[0], xyz[1], xyz[2]);
         fociFile.addCell(focus);
      }
      else if (paramName == "-last-link") {
         const QString focusName = parameters->getNextParameterAsString("Focus Name");
         const float* xyz = border->getLinkXYZ(border->getNumberOfLinks() - 1);
         
         // Create a new focus
         //
         CellData focus(focusName, xyz[0], xyz[1], xyz[2]);
         fociFile.addCell(focus);
      }
      else {
         throw CommandException("Unrecognized parameter: "
                                + paramName);
      }
   }
   
   //
   // Project the foci
   //
   FociProjectionFile fociFileProjected;
   fociFileProjected.append(fociFile);
   CellFileProjector cellProjector(surface);
   cellProjector.projectFile(&fociFileProjected,
                             0,
                             CellFileProjector::PROJECTION_TYPE_ALL,
                             0.0,
                             false,
                             NULL);
   
   //
   // Read the input focus projection file
   //
   FociProjectionFile fociProjectionFile;
   if (inputFociProjectionFileName.isEmpty() == false) {
      if (QFile::exists(inputFociProjectionFileName)) {
         fociProjectionFile.readFile(inputFociProjectionFileName);
      }
   }
   
   //
   // Append new foci
   //
   fociProjectionFile.append(fociFileProjected);
   
   //
   // Write new foci file
   //
   fociProjectionFile.writeFile(outputFociProjectionFileName);
}

      

