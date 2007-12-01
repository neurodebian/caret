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
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "CommandSurfaceCrossoverCheck.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceCrossoverCheck::CommandSurfaceCrossoverCheck()
   : CommandBase("-surface-crossover-check",
                 "SURFACE CROSSOVER CHECK")
{
}

/**
 * destructor.
 */
CommandSurfaceCrossoverCheck::~CommandSurfaceCrossoverCheck()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceCrossoverCheck::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addVariableListOfParameters("Crossover Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceCrossoverCheck::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "[-roi-file      roi-file-name]\n"
       + indent9 + "[-surface-type  SURFACE-TYPE]\n"
       + indent9 + "\n"
       + indent9 + "Perform a crossover check on a surface.\n"
       + indent9 + "\n"
       + indent9 + "If a region of interest file name is supplied, the ROI is\n"
       + indent9 + "set to the nodes identified as crossovers.\n"
       + indent9 + "\n"
       + indent9 + "If a SURFACE-TYPE is specified it must be one of the \n"
       + indent9 + "types listed below.  If no SURFACE-TYPE is supplied, the\n"
       + indent9 + "type contained in coordinate file will be used.\n");
       
       std::vector<BrainModelSurface::SURFACE_TYPES> surfaceTypes;
       std::vector<QString> surfaceTypeNames;
       BrainModelSurface::getSurfaceTypesAndNames(surfaceTypes,
                                                  surfaceTypeNames);
       for (int i = 0; i < static_cast<int>(surfaceTypeNames.size()); i++) {
          helpInfo += (indent9 
                       + "   "
                       + surfaceTypeNames[i]
                       + "\n");
       }
   helpInfo += "\n";
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceCrossoverCheck::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");

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
   QString roiFileName;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("SURFACE CROSSOVER CHECK parameter");
      if (paramName == "-roi-file") {
         roiFileName = 
            parameters->getNextParameterAsString("SURFACE CROSSOVER ROI file name");
      }
      else if (paramName == "-surface-type") {
         const QString surfaceTypeString = 
            parameters->getNextParameterAsString("SURFACE CROSSOVER SURFACE-TYPE");
         const BrainModelSurface::SURFACE_TYPES surfaceType =
            BrainModelSurface::getSurfaceTypeFromConfigurationID(surfaceTypeString);
         bms->setSurfaceType(surfaceType);
      }
      else {
         throw CommandException("unknown parameter: "
                                + paramName);
      }
   }
   
   //
   // Do the crossover check
   //
   int tileCrossovers = 0;
   int nodeCrossovers = 0;
   bms->crossoverCheck(tileCrossovers,
                       nodeCrossovers,
                       bms->getSurfaceType());
   
   //
   // Create optional ROI file
   //
   if (roiFileName.isEmpty() == false) {
      NodeRegionOfInterestFile roiFile;
      roiFile.setNumberOfNodes(numNodes);
      for (int i = 0; i < numNodes; i++) {
         if (brainSet.getNodeAttributes(i)->getCrossover() ==
             BrainSetNodeAttribute::CROSSOVER_YES) {
            roiFile.setNodeSelected(i, true);
         }
      }
      roiFile.writeFile(roiFileName);
   }
   
   //
   // Print results
   //
   std::cout << "Crossover Check for "
             << FileUtilities::basename(coordinateFileName).toAscii().constData() << " "
             << FileUtilities::basename(topologyFileName).toAscii().constData() << std::endl;
   std::cout << "   Tile Crossovers: " << tileCrossovers << std::endl;
   std::cout << "   Node Crossovers: " << nodeCrossovers << std::endl;
}

      

