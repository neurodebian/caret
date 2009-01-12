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
#include "CellFile.h"
#include "CellFileProjector.h"
#include "CellProjectionFile.h"
#include "CommandSurfaceCellProjection.h"
#include "FileFilters.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceCellProjection::CommandSurfaceCellProjection(const QString& operationSwitchIn,
                                                           const QString& shortDescriptionIn,
                                                           const bool fociFlagIn)
   : CommandBase(operationSwitchIn,
                 shortDescriptionIn)
{
   fociFlag = fociFlagIn;
}

/**
 * destructor.
 */
CommandSurfaceCellProjection::~CommandSurfaceCellProjection()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceCellProjection::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   if (fociFlag) {
      paramsOut.addFile("Input Foci File",
                        FileFilters::getFociFileFilter());
      paramsOut.addFile("Output Foci Projection File",
                        FileFilters::getFociProjectionFileFilter());
   }
   else {
      paramsOut.addFile("Input Cell File",
                        FileFilters::getCellFileFilter());
      paramsOut.addFile("Output Cell Projection File",
                        FileFilters::getCellProjectionFileFilter());
   }
}

/**
 * get full help information.
 */
QString 
CommandSurfaceCellProjection::getHelpInformation() const
{
   QString s = "cell";
   if (fociFlag) s = "foci";
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-" + s + "-file-name>\n"
       + indent9 + "<output-" + s + "-projection-file-name>\n"
       + indent9 + "[-project-onto-surface  onto-surface-above-distance]\n"
       + indent9 + "\n"
       + indent9 + "Project the " + s + " to the surface and save into the " + s + "\n"
       + indent9 + "projection file.\n"
       + indent9 + "\n"
       + indent9 + "\"-onto-surface-flag\" is used to project the " + s + " so that\n"
       + indent9 + "they are a specified distance above the surface.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceCellProjection::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   QString s = "Cell";
   if (fociFlag) s = "Foci";

   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString cellFileName =
      parameters->getNextParameterAsString(s + " File Name");
   const QString cellProjectionFileName =
      parameters->getNextParameterAsString(s + " Projection File Name");
      
   //
   // Process optional parameters
   //
   bool projectToSurfaceFlag = false;
   float surfaceAboveDistance = 0.0;
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString(s + " Projection Parameter");
      if (paramName == "-project-onto-surface") {
         surfaceAboveDistance = 
            parameters->getNextParameterAsFloat(s + " Projection Parameter: Above surface distance");
      }
      else {
         throw CommandException("unrecognized option");
      }
   }
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
      throw CommandException("surface contains no nodes.");  
   }
   
   //
   // Read the cell file
   //
   CellFile* cellFile = NULL;
   if (fociFlag) {
      cellFile = new FociFile;
   }
   else {
      cellFile = new CellFile;
   }
   cellFile->readFile(cellFileName);
   
   //
   // Project the cell file
   //
   CellProjectionFile* cellProjectionFile = NULL;
   if (fociFlag) {
      cellProjectionFile = new FociProjectionFile;
   }
   else {
      cellProjectionFile = new CellProjectionFile;
   }
   cellProjectionFile->appendFiducialCellFile(*cellFile);
   CellFileProjector projector(bms);
   projector.projectFile(cellProjectionFile, 
                         0,
                         CellFileProjector::PROJECTION_TYPE_ALL,
                         surfaceAboveDistance,
                         projectToSurfaceFlag,
                         NULL);
   
   //
   // Write the cell projection file
   //
   cellProjectionFile->writeFile(cellProjectionFileName);
}

      

