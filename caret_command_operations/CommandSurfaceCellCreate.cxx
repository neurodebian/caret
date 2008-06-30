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

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CellFile.h"
#include "CellFileProjector.h"
#include "CellProjectionFile.h"
#include "CommandSurfaceCellCreate.h"
#include "FileFilters.h"
#include "FociProjectionFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceCellCreate::CommandSurfaceCellCreate(const QString& operationSwitchIn,
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
CommandSurfaceCellCreate::~CommandSurfaceCellCreate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceCellCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   QString s = "Cell";
   if (fociFlag) s = "Foci";
   
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input " + s + " Projection File Name", FileFilters::getCellProjectionFileFilter());
   paramsOut.addFile("Output " + s + " Projection File Name", FileFilters::getCellProjectionFileFilter());
   paramsOut.addVariableListOfParameters(s + " Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceCellCreate::getHelpInformation() const
{
   QString s = "cell";
   QString s2 = "cell";
   if (fociFlag) {
      s = "foci";
      s2 = "focus";
   }
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-" + s + "-projection-file-name>\n"
       + indent9 + "<output-" + s + "-projection-file-name>\n"
       + indent9 + "[-at-node " + s2 + " -name  node-number]\n"
       + indent9 + "[-" + s2 + "    " + s2 + "-name  X  Y  Z]\n"
       + indent9 + "[-" + s2 + "-offset  " + s2 + "-name  " + s + "-offset-name  off-X off-Y off-Z]\n"
       + indent9 + "[-" + s2 + "-offset-xyz  " + s2 + "-name \n"
       + indent9 + "    x-" + s2 + "-offset-name  offset-X \n"
       + indent9 + "    y-" + s2 + "-offset-name  offset-Y \n"
       + indent9 + "    z-" + s2 + "-offset-name  offset-Z] \n"
       + indent9 + "\n"
       + indent9 + "Create new " + s2 + " located at either an XYZ coordinate or on\n"
       + indent9 + "a node.  The input " + s + " projection file does not need to exist.\n"
       + indent9 + "\n"
       + indent9 + "The offset mode creates a new " + s2 + " that is offset from \n"
       + indent9 + "the \"" + s2 + "-offset-name\" by (off-X, off-Y, off-Z).\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceCellCreate::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString inputCellProjectionFileName =
      parameters->getNextParameterAsString("Input " + s + " Projection File Name");
   const QString outputCellProjectionFileName =
      parameters->getNextParameterAsString("Output " + s + " Projection File Name");
      
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
   }   const int numNodes = bms->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Always assume surface is a fiducial
   //
   bms->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   
   //
   // Read input cell projection file
   //
   CellProjectionFile* cellProjectionFile = NULL;
   if (fociFlag) {
      cellProjectionFile = new FociProjectionFile;
   }
   else {
      cellProjectionFile = new CellProjectionFile;
   }
   if (QFile::exists(inputCellProjectionFileName)) {
      cellProjectionFile->readFile(inputCellProjectionFileName);
   }
   
   //
   // File for new cells
   //
   CellFile cellFile;
   
   //
   // Loop through options
   //
   while (parameters->getParametersAvailable()) {
      const QString paramName = 
         parameters->getNextParameterAsString("Create " + s + " Parameter");
      if ((paramName == "-cell") ||
          (paramName == "-focus")) {
         const QString cellName = parameters->getNextParameterAsString(s + " Name");
         const float x = parameters->getNextParameterAsFloat(s + " X");
         const float y = parameters->getNextParameterAsFloat(s + " Y");
         const float z = parameters->getNextParameterAsFloat(s + " Z");
         CellData cd(cellName, x, y, z);
         cellFile.addCell(cd);
      }
      else if ((paramName == "-cell-offset") ||
               (paramName == "-focus-offset")) {
         const QString cellName = parameters->getNextParameterAsString(s + " Name");
         const QString cellOffsetName = parameters->getNextParameterAsString(s + "Offset Name");
         const float xOffset = parameters->getNextParameterAsFloat(s + " Off-X");
         const float yOffset = parameters->getNextParameterAsFloat(s + " Off-Y");
         const float zOffset = parameters->getNextParameterAsFloat(s + " Off-Z");
         
         //
         // Find offset cell
         //
         const CellProjection* cp = cellProjectionFile->getLastCellProjectionWithName(cellOffsetName);
         if (cp == NULL) {
            throw CommandException(" offset " + s + " named "
                                   + cellOffsetName + " not found.");
         }
         float xyz[3];
         if (cp->getProjectedPosition(bms->getCoordinateFile(),
                                      tf,
                                      bms->getIsFiducialSurface(),
                                      bms->getIsFlatSurface(),
                                      false,
                                      xyz) == false) {
            throw CommandException("offset " + s + " named "
                                   + cellOffsetName + " does not have valid projected position.");
         }
         
         //
         // Create new cell with offset
         //
         const float x = xyz[0] + xOffset;
         const float y = xyz[1] + yOffset;
         const float z = xyz[2] + zOffset;
         CellData cd(cellName, x, y, z);
         cellFile.addCell(cd);
      }
      else if ((paramName == "-cell-offset-xyz") ||
               (paramName == "-focus-offset-xyz")) {
         const QString cellName = parameters->getNextParameterAsString(s + " Name");
         const QString cellXOffsetName = parameters->getNextParameterAsString(s + " X-Offset Name");
         const float xOffset = parameters->getNextParameterAsFloat(s + " Off-X");
         const QString cellYOffsetName = parameters->getNextParameterAsString(s + " X-Offset Name");
         const float yOffset = parameters->getNextParameterAsFloat(s + " Off-Y");
         const QString cellZOffsetName = parameters->getNextParameterAsString(s + " X-Offset Name");
         const float zOffset = parameters->getNextParameterAsFloat(s + " Off-Z");
         
         //
         // Find offset cell X
         //
         const CellProjection* cpX = cellProjectionFile->getLastCellProjectionWithName(cellXOffsetName);
         if (cpX == NULL) {
            throw CommandException(" offset " + s + " named "
                                   + cellXOffsetName + " not found.");
         }
         float xyz[3];
         if (cpX->getProjectedPosition(bms->getCoordinateFile(),
                                      tf,
                                      bms->getIsFiducialSurface(),
                                      bms->getIsFlatSurface(),
                                      false,
                                      xyz) == false) {
            throw CommandException("offset " + s + " named "
                                   + cellXOffsetName + " does not have valid projected position.");
         }
         const float newCellX = xyz[0] + xOffset;
         
         
         //
         // Find offset cell Y
         //
         const CellProjection* cpY = cellProjectionFile->getLastCellProjectionWithName(cellYOffsetName);
         if (cpY == NULL) {
            throw CommandException(" offset " + s + " named "
                                   + cellYOffsetName + " not found.");
         }
         if (cpY->getProjectedPosition(bms->getCoordinateFile(),
                                      tf,
                                      bms->getIsFiducialSurface(),
                                      bms->getIsFlatSurface(),
                                      false,
                                      xyz) == false) {
            throw CommandException("offset " + s + " named "
                                   + cellYOffsetName + " does not have valid projected position.");
         }
         const float newCellY = xyz[1] + yOffset;
         
         //
         // Find offset cell Z
         //
         const CellProjection* cpZ = cellProjectionFile->getLastCellProjectionWithName(cellZOffsetName);
         if (cpZ == NULL) {
            throw CommandException(" offset " + s + " named "
                                   + cellZOffsetName + " not found.");
         }
         if (cpZ->getProjectedPosition(bms->getCoordinateFile(),
                                      tf,
                                      bms->getIsFiducialSurface(),
                                      bms->getIsFlatSurface(),
                                      false,
                                      xyz) == false) {
            throw CommandException("offset " + s + " named "
                                   + cellZOffsetName + " does not have valid projected position.");
         }
         const float newCellZ = xyz[2] + zOffset;
        
         //
         // Create new cell with offset
         //
         CellData cd(cellName, newCellX, newCellY, newCellZ);
         cellFile.addCell(cd);
      }
      else if (paramName == "-at-node") {
         const QString cellName = parameters->getNextParameterAsString(s + " Name");
         const int nodeNumber = parameters->getNextParameterAsFloat(s + " Node Number");
         if ((nodeNumber >= 0) &&
             (nodeNumber < numNodes)) {
            const float* xyz = bms->getCoordinateFile()->getCoordinate(nodeNumber);
            CellData cd(cellName, xyz[0], xyz[1], xyz[2]);
            cellFile.addCell(cd);
         }
         else {
           throw CommandException("invalid node number: " 
                                  + QString::number(nodeNumber));
         }
      }
      else {
         throw CommandException("Create unknown parameter: " 
                                + paramName);
      }
   }

   if (cellFile.getNumberOfCells() <= 0) {
      throw CommandException("No " + s + " were created.");
   }   
   
   //
   // Project the cells
   //
   CellProjectionFile projCellFile;
   projCellFile.appendFiducialCellFile(cellFile);
   CellFileProjector cfp(bms);
   cfp.projectFile(&projCellFile,
                   0,
                   CellFileProjector::PROJECTION_TYPE_ALL,
                   0.0,
                   false,
                   NULL);
                   
   //
   // Append new cells
   //
   cellProjectionFile->append(projCellFile);
   
   //
   // Write cell projection file
   //
   cellProjectionFile->writeFile(outputCellProjectionFileName);
   
   delete cellProjectionFile;
}


      

