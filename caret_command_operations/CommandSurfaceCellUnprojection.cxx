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
#include "CellProjectionUnprojector.h"
#include "CellProjectionFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "CommandSurfaceCellUnprojection.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceCellUnprojection::CommandSurfaceCellUnprojection(const QString& operationSwitchIn,
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
CommandSurfaceCellUnprojection::~CommandSurfaceCellUnprojection()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceCellUnprojection::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   if (fociFlag) {
      paramsOut.addFile("Input Foci Projection File",
                        FileFilters::getFociProjectionFileFilter());
      paramsOut.addFile("Output Foci File",
                        FileFilters::getFociFileFilter());
   }
   else {
      paramsOut.addFile("Input Cell Projection File",
                        FileFilters::getCellProjectionFileFilter());
      paramsOut.addFile("Output Cell File",
                        FileFilters::getCellFileFilter());
   }
}

/**
 * get full help information.
 */
QString 
CommandSurfaceCellUnprojection::getHelpInformation() const
{
   QString s = "cell";
   if (fociFlag) s = "foci";
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-" + s + "-projection-file-name>\n"
       + indent9 + "<output-" + s + "-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Unproject the " + s + " projections to the surface and save\n"
       + indent9 + "the " + s + " file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceCellUnprojection::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString cellProjectionFileName =
      parameters->getNextParameterAsString(s + " Projection File Name");
   const QString cellFileName =
      parameters->getNextParameterAsString(s + " File Name");
      
   checkForExcessiveParameters();
   
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
   // Load the cell projection file
   //
   CellProjectionFile* cellProjectionFile = NULL;
   if (fociFlag) {
      cellProjectionFile = new FociProjectionFile;
   }
   else {
      cellProjectionFile = new CellProjectionFile;
   }
   cellProjectionFile->readFile(cellProjectionFileName);
   
   //
   // Unproject the cells
   //
   CellFile* cellFile = NULL;
   if (fociFlag) {
      cellFile = new FociFile;
   }
   else {
      cellFile = new CellFile;
   }
   CellProjectionUnprojector unprojector;
   unprojector.unprojectCellProjections(*cellProjectionFile,
                                        bms,
                                        *cellFile,
                                        0);
   //
   // Write the cell file
   //
   cellFile->writeFile(cellFileName);
   
   //
   // Clean up
   //
   delete cellFile;
   delete cellProjectionFile;
}

      

