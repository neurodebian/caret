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
#include "BrainModelSurfaceDistortion.h"
#include "BrainSet.h"
#include "CommandSurfaceDistortion.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SurfaceShapeFile.h"

/**
 * constructor.
 */
CommandSurfaceDistortion::CommandSurfaceDistortion()
   : CommandBase("-surface-distortion",
                 "SURFACE DISTORTION")
{
}

/**
 * destructor.
 */
CommandSurfaceDistortion::~CommandSurfaceDistortion()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceDistortion::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Fiducial Coordinate File Name", 
                     FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Surface Shape File Name", 
                     FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addFile("Output Surface Shape File Name", 
                     FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addVariableListOfParameters("Distortion\nOptions");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceDistortion::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<fiducial-coordinate-file-name>\n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<closed-topology-file-name>\n"
       + indent9 + "<input-surface-shape-file-name>\n"
       + indent9 + "<output-surface-shape-file-name>\n"
       + indent9 + "[-generate-areal-distortion]\n"
       + indent9 + "[-generate-linear-distortion]\n"
       + indent9 + "[-areal-distortion-column-name  name]\n"
       + indent9 + "[-linear-distortion-column-name  name]\n"
       + indent9 + "\n"
       + indent9 + "Generate distortion measurements for the surfaces.\n"
       + indent9 + "\n"
       + indent9 + "The column names are optional, and, if not specified \n"
       + indent9 + "are " + SurfaceShapeFile::arealDistortionColumnName + "\n"
       + indent9 + "and " + SurfaceShapeFile::linearDistortionColumnName + " \n"
       + indent9 + "respectively.\n"
       + indent9 + "\n"
       + indent9 + "The \"input-surface-shape-file-name\" does not needs\n"
       + indent9 + "to exist.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceDistortion::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString fiducialCoordinateFileName =
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputSurfaceShapeFileName =
      parameters->getNextParameterAsString("Input Surface Shape File Name");
   const QString outputSurfaceShapeFileName =
      parameters->getNextParameterAsString("Output Surface Shape File Name");
      
   bool doArealFlag = false;
   bool doLinearFlag = false;
   QString arealDistortionColumnName(SurfaceShapeFile::arealDistortionColumnName);
   QString linearDistortionColumnName(SurfaceShapeFile::linearDistortionColumnName);
   
   //
   // Check optional parameters
   //
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("SURFACE DISTORTION parameter");
      if (paramName == "-generate-areal-distortion") {
         doArealFlag = true;
      }
      else if (paramName == "-generate-linear-distortion") {
         doLinearFlag = true;
      }
      else if (paramName == "-areal-distortion-column-name") {
         arealDistortionColumnName = 
            parameters->getNextParameterAsString("Areal Distortion column name");
      }
      else if (paramName == "-linear-distortion-column-name") {
         linearDistortionColumnName = 
            parameters->getNextParameterAsString("Linear Distortion column name");
      }
      else {
         throw CommandException("unknown parameter: "
                                + paramName);
      }
   }

   //
   // Check inputs
   //
   if ((doArealFlag == false) &&
       (doLinearFlag == false)) {
      throw CommandException("At least one of "
                             "\"-generate-areal-distortion\" or "
                             "\"-generate-linear-distortion\" must be provided.");
   }
   
   // 
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     fiducialCoordinateFileName,
                     coordinateFileName,
                     true); 
   BrainModelSurface* fiducialBMS = brainSet.getBrainModelSurface(0);
   if (fiducialBMS == NULL) {
      throw CommandException("unable to find fiducial surface.");
   }
   TopologyFile* tf = fiducialBMS->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = fiducialBMS->getNumberOfNodes();
   if (numNodes == 0) {      
      throw CommandException("surface contains not nodes.");  
   }
   BrainModelSurface* surfaceBMS = brainSet.getBrainModelSurface(1);
   if (surfaceBMS == NULL) {
      throw CommandException("unable to find second surface.");
   }

   //
   // Read input surface shape file
   //
   SurfaceShapeFile surfaceShapeFile;
   if (QFile::exists(inputSurfaceShapeFileName)) {
      surfaceShapeFile.readFile(inputSurfaceShapeFileName);
   }
   
   //
   // Set areal distortion column number
   //
   int arealColumnNumber = BrainModelSurfaceDistortion::DISTORTION_COLUMN_DO_NOT_GENERATE;
   if (doArealFlag) {
      arealColumnNumber = BrainModelSurfaceDistortion::DISTORTION_COLUMN_CREATE_NEW;
      const int col = surfaceShapeFile.getColumnWithName(arealDistortionColumnName);
      if (col >= 0) {
         arealColumnNumber = col;
      }
   }
   
   //
   // Set linear distortion column number
   //
   int linearColumnNumber = BrainModelSurfaceDistortion::DISTORTION_COLUMN_DO_NOT_GENERATE;
   if (doLinearFlag) {
      linearColumnNumber = BrainModelSurfaceDistortion::DISTORTION_COLUMN_CREATE_NEW;
      const int col = surfaceShapeFile.getColumnWithName(linearDistortionColumnName);
      if (col >= 0) {
         linearColumnNumber = col;
      }
   }
   
   //
   // Generate curvature
   //
   BrainModelSurfaceDistortion bmsd(&brainSet,
                                   surfaceBMS,
                                   fiducialBMS,
                                   tf,
                                   &surfaceShapeFile,
                                   arealColumnNumber,
                                   linearColumnNumber,
                                   arealDistortionColumnName,
                                   linearDistortionColumnName);
   bmsd.execute();
   
   //
   // Write the surface shape file
   //
   surfaceShapeFile.writeFile(outputSurfaceShapeFileName);
}

      

