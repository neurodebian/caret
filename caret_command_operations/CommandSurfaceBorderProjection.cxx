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
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderProjection.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderProjection::CommandSurfaceBorderProjection()
   : CommandBase("-surface-border-projection",
                 "SURFACE BORDER PROJECTION")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderProjection::~CommandSurfaceBorderProjection()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderProjection::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addFile("Output Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderProjection::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-border-file>\n"
       + indent9 + "<output-border-projection-file>\n"
       + indent9 + "\n"
       + indent9 + "Project the borders using the specified surface.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderProjection::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString borderFileName =
      parameters->getNextParameterAsString("Border File Name");
   const QString borderProjectionFileName =
      parameters->getNextParameterAsString("Border Projection File Name");
      
   checkForExcessiveParameters();
      
   // 
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     "",
                     true); 
   const BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
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
   // Read the border file
   //
   BorderFile borderFile;
   borderFile.readFile(borderFileName);
   
   //
   // Project the border file
   //
   BorderProjectionFile borderProjectionFile;
   BorderFileProjector projector(bms, true);
   projector.projectBorderFile(&borderFile, &borderProjectionFile, NULL);
   
   //
   // Write the border projection file
   //
   borderProjectionFile.writeFile(borderProjectionFileName);
}

      

