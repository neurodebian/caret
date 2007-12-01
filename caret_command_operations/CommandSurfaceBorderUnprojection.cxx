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
#include "CommandSurfaceBorderUnprojection.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderUnprojection::CommandSurfaceBorderUnprojection()
   : CommandBase("-surface-border-unprojection",
                 "SURFACE BORDER UNPROJECTION")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderUnprojection::~CommandSurfaceBorderUnprojection()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderUnprojection::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border File",
                     FileFilters::getBorderGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderUnprojection::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-border-projection-file>\n"
       + indent9 + "<output-border-file>\n"
       + indent9 + "\n"
       + indent9 + "Unproject the borders using the specified surface.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderUnprojection::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString borderFileName =
      parameters->getNextParameterAsString("Border File Name");
      
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
   // Read the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(borderProjectionFileName);
   
   //
   // Project the border file
   //
   BorderFile borderFile;
   BorderProjectionUnprojector unprojector;
   unprojector.unprojectBorderProjections(*(bms->getCoordinateFile()), 
                                          borderProjectionFile, 
                                          borderFile);
   
   //
   // Write the border projection file
   //
   borderFile.writeFile(borderFileName);
}

      

