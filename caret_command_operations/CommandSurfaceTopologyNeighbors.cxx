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

#include "CommandSurfaceTopologyNeighbors.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
CommandSurfaceTopologyNeighbors::CommandSurfaceTopologyNeighbors()
   : CommandBase("-surface-topology-neighbors",
                 "SURFACE TOPOLOGY NEIGHBORS")
{
}

/**
 * destructor.
 */
CommandSurfaceTopologyNeighbors::~CommandSurfaceTopologyNeighbors()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceTopologyNeighbors::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.clear();
   paramsOut.addFile("Input Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Text File Name", FileFilters::getTextFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceTopologyNeighbors::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<output-text-file-name>\n"
       + indent9 + "\n"
       + indent9 + "For each node in the input topology file, generate a list\n"
       + indent9 + "of the node's neighbors.  Each line in the output text file\n"
       + indent9 + "contains the node number followed by the node's neighbors.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceTopologyNeighbors::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString outputTextFileName =
      parameters->getNextParameterAsString("Output Text File Name");
   //
   // Read input topology file
   //
   TopologyFile topologyFile;
   topologyFile.readFile(inputTopologyFileName);
   
   TextFile outputTextFile;
   
   //
   // Get the topology helper             
   //                                     
   const TopologyHelper* th = topologyFile.getTopologyHelper(true, true, true);
   const int numNodes = th->getNumberOfNodes();
   const QString blank(" ");              
   for (int i = 0; i < numNodes; i++) {
      QString nodeLine(QString::number(i));
      std::vector<int> neighbors;
      th->getNodeNeighbors(i, neighbors);
      for (unsigned int j = 0; j < neighbors.size(); j++) {
         nodeLine += (blank + QString::number(neighbors[j]));
      }
      outputTextFile.appendLine(nodeLine);
   }

   //
   // Write the neighbors file
   //
   outputTextFile.writeFile(outputTextFileName);
}

      

