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

#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROISurfaceXYZMeansReport.h"
#include "BrainSet.h"
#include "CommandSurfaceRoiCoordReport.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "TextFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
CommandSurfaceRoiCoordReport::CommandSurfaceRoiCoordReport()
   : CommandBase("-surface-roi-coord-report",
                 "SURFACE ROI COORD REPORT")
{
}

/**
 * destructor.
 */
CommandSurfaceRoiCoordReport::~CommandSurfaceRoiCoordReport()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceRoiCoordReport::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addString("Paint Column Name or Number");
   paramsOut.addString("Paint Name");
   paramsOut.addFile("Output Text File Name", FileFilters::getTextFileFilter());
   paramsOut.addMultipleFiles("Coordinate File Names", FileFilters::getCoordinateGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRoiCoordReport::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<input-paint-file-name>\n"
       + indent9 + "<input-paint-file-column-name-or-number>\n"
       + indent9 + "<input-paint-name>\n"
       + indent9 + "<output-file-name.txt>\n"
       + indent9 + "<one-or-more-coordinate-file-names>\n"
       + indent9 + "\n"
       + indent9 + "Print a report listing the mean X, Y, and Z coordinates for\n"
       + indent9 + "the nodes identified by the paint name.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceRoiCoordReport::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString topoFileName = 
      parameters->getNextParameterAsString("Topology File Name");
   const QString paintFileName = 
      parameters->getNextParameterAsString("Paint File Name");
   const QString paintColumnNameOrNumber = 
      parameters->getNextParameterAsString("Paint Column Number");
   const QString paintName = 
      parameters->getNextParameterAsString("Paint Name");   
   const QString outputTextFileName = 
      parameters->getNextParameterAsString("Output Text File Name");

   //
   // Coordinate files
   //
   std::vector<QString> inputCoordFileNames;
   while (parameters->getParametersAvailable()) {
      inputCoordFileNames.push_back(parameters->getNextParameterAsString("Coordinate File Names"));
   }
   const int numInputCoordFiles = static_cast<int>(inputCoordFileNames.size());
   if (numInputCoordFiles <= 0) {
      throw CommandException("No coordinate files are specified.");
   }

   //
   // Read the spec file into a brain set
   //
   SpecFile specFile;
   for (int i = 0; i < numInputCoordFiles; i++) {
      specFile.addToSpecFile(SpecFile::getFiducialCoordFileTag(),
                             inputCoordFileNames[i],
                             "",
                             false);
   }
   specFile.addToSpecFile(SpecFile::getClosedTopoFileTag(),
                          topoFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::getPaintFileTag(),
                          paintFileName,
                          "",
                          false);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   brainSet.setIgnoreTopologyFileInCoordinateFileHeaderFlag(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      throw CommandException("Reading spec file: " +
                             errorMessage);
   }

   //
   // Find the paint file column with the proper name
   //
   PaintFile* paintFile = brainSet.getPaintFile();
   const int paintColumnNumber = 
      paintFile->getColumnFromNameOrNumber(paintColumnNameOrNumber, false);   
      
   //
   // Find the index of the paint name
   //
   const int paintIndex = paintFile->getPaintIndexFromName(paintName);
   if (paintIndex < 0) {
      throw CommandException("Unable to find a paint named " 
                             + paintName
                             + " in the paint file.");
   }
   
   //
   // Find the coordinate files
   //
   std::vector<CoordinateFile*> coordFiles;
   for (int i = 0; i < brainSet.getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms != NULL) {
         coordFiles.push_back(bms->getCoordinateFile());
      }
   }
   const int numCoordFiles = static_cast<int>(coordFiles.size());
   if (numCoordFiles <= 0) {
      throw CommandException("ERROR: No surfaces were read.");
   }

   //
   // Get topology file
   //
   if (brainSet.getNumberOfTopologyFiles() < 0) {
      throw CommandException("ERROR: No topology file was read.");
   }
   else if (brainSet.getNumberOfTopologyFiles() > 1) {
      throw CommandException("ERROR: There is more than one topology file.");
   }
   const TopologyFile* topoFile = brainSet.getTopologyFile(0);
   
   //
   // Verify surface has nodes
   //
   const int numNodes = brainSet.getNumberOfNodes();
   if (numNodes <= 0) {
      throw CommandException("The surfaces contain no nodes.");
   }
   
   //
   // Create a topology helper
   //
   const TopologyHelper* topologyHelper = topoFile->getTopologyHelper(false, true, false);
   
   //
   // Flag the nodes for the ROI
   //
   BrainModelSurfaceROINodeSelection surfaceROINodeSelection(&brainSet);
   surfaceROINodeSelection.update();
   surfaceROINodeSelection.deselectAllNodes();
   for (int i = 0; i < numNodes; i++) {
      if (topologyHelper->getNodeHasNeighbors(i)) {
         if (paintFile->getPaint(i, paintColumnNumber) == paintIndex) {
            surfaceROINodeSelection.setNodeSelected(i, 1);
         }
      }
   }

   //
   // Generate the report
   //   
   BrainModelSurfaceROISurfaceXYZMeansReport surfaceROI(&brainSet,
                                                brainSet.getBrainModelSurface(0),
                                                &surfaceROINodeSelection,
                                                coordFiles);
   surfaceROI.execute();
   
   //
   // File for text report
   //
   TextFile textReportFile;
   textReportFile.setText(surfaceROI.getReportText());

   //
   // Write the text file
   //
   textReportFile.writeFile(outputTextFileName);
}

      

