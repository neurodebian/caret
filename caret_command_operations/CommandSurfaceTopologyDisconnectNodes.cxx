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

#include "CommandSurfaceTopologyDisconnectNodes.h"
#include "FileFilters.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceTopologyDisconnectNodes::CommandSurfaceTopologyDisconnectNodes()
   : CommandBase("-surface-topology-disconnect-nodes",
                 "SURFACE TOPOLOGY DISCONNECT NODES")
{
}

/**
 * destructor.
 */
CommandSurfaceTopologyDisconnectNodes::~CommandSurfaceTopologyDisconnectNodes()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceTopologyDisconnectNodes::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Region of Interest File Name", FileFilters::getRegionOfInterestFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceTopologyDisconnectNodes::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<output-topology-file-name>\n"
       + indent9 + "<region-of-interest-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Disconnect all nodes that are in the region of interest.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceTopologyDisconnectNodes::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString outputTopologyFileName =
      parameters->getNextParameterAsString("Output Topology File Name");
   const QString regionOfInterestFileName =
      parameters->getNextParameterAsString("Region of Interest File Name");
   
   checkForExcessiveParameters();
   
   //
   // Read input topology file
   //
   TopologyFile topologyFile;
   topologyFile.readFile(inputTopologyFileName);
   
   //
   // Read the region of interest file
   //
   NodeRegionOfInterestFile roiFile;
   roiFile.readFile(regionOfInterestFileName);
   
   //
   // Disconnec nodes
   //
   topologyFile.disconnectNodesInRegionOfInterest(roiFile);
   
   //
   // Write the topology file
   //
   topologyFile.writeFile(outputTopologyFileName);
}

      

