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
#include "BrainModelSurfaceCurvature.h"
#include "BrainSet.h"
#include "CommandSurfaceCurvature.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SurfaceShapeFile.h"

/**
 * constructor.
 */
CommandSurfaceCurvature::CommandSurfaceCurvature()
   : CommandBase("-surface-curvature",
                 "SURFACE CURVATURE")
{
}

/**
 * destructor.
 */
CommandSurfaceCurvature::~CommandSurfaceCurvature()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceCurvature::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Surface Shape File Name", 
                     FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addFile("Output Surface Shape File Name", 
                     FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addVariableListOfParameters("Curvature Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceCurvature::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<fiducial-coordinate-file-name>\n"
       + indent9 + "<closed-topology-file-name>\n"
       + indent9 + "<input-surface-shape-file-name>\n"
       + indent9 + "<output-surface-shape-file-name>\n"
       + indent9 + "[-generate-mean-curvature]\n"
       + indent9 + "[-generate-gaussian-curvature]\n"
       + indent9 + "[-mean-column-name  name]\n"
       + indent9 + "[-gaussian-column-name  name]\n"
       + indent9 + "\n"
       + indent9 + "Generate curvature measurements for the surface.\n"
       + indent9 + "\n"
       + indent9 + "The column names are optional, and, if not specified \n"
       + indent9 + "are " + SurfaceShapeFile::meanCurvatureColumnName + "\n"
       + indent9 + "and " + SurfaceShapeFile::gaussianCurvatureColumnName + " \n"
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
CommandSurfaceCurvature::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputSurfaceShapeFileName =
      parameters->getNextParameterAsString("Input Surface Shape File Name");
   const QString outputSurfaceShapeFileName =
      parameters->getNextParameterAsString("Output Surface Shape File Name");
      
   bool doMean = false;
   bool doGaussian = false;
   QString meanCurvatureColumnName(SurfaceShapeFile::meanCurvatureColumnName);
   QString gaussianCurvatureColumnName(SurfaceShapeFile::gaussianCurvatureColumnName);
   
   //
   // Check optional parameters
   //
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("SURFACE CURVATURE parameter");
      if (paramName == "-generate-mean-curvature") {
         doMean = true;
      }
      else if (paramName == "-generate-gaussian-curvature") {
         doGaussian = true;
      }
      else if (paramName == "-mean-column-name") {
         meanCurvatureColumnName = 
            parameters->getNextParameterAsString("SURFACE CURVATURE mean column name");
      }
      else if (paramName == "-gaussian-column-name") {
         gaussianCurvatureColumnName = 
            parameters->getNextParameterAsString("SURFACE CURVATURE gaussian column name");
      }
      else {
         throw CommandException("unknown parameter: "
                                + paramName);
      }
   }

   //
   // Check inputs
   //
   if ((doMean == false) &&
       (doGaussian == false)) {
      throw CommandException("At least one of "
                             "\"-generate-mean-curvature\" or "
                             "\"-generate-gaussian-curvature\" must be provided.");
   }
   
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
   // Read input surface shape file
   //
   SurfaceShapeFile surfaceShapeFile;
   if (QFile::exists(inputSurfaceShapeFileName)) {
      surfaceShapeFile.readFile(inputSurfaceShapeFileName);
   }
   
   //
   // Set mean column number
   //
   int meanColumnNumber = BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE;
   if (doMean) {
      meanColumnNumber = BrainModelSurfaceCurvature::CURVATURE_COLUMN_CREATE_NEW;
      const int col = surfaceShapeFile.getColumnWithName(meanCurvatureColumnName);
      if (col >= 0) {
         meanColumnNumber = col;
      }
   }
   
   //
   // Set gaussian column number
   //
   int gaussianColumnNumber = BrainModelSurfaceCurvature::CURVATURE_COLUMN_DO_NOT_GENERATE;
   if (doGaussian) {
      gaussianColumnNumber = BrainModelSurfaceCurvature::CURVATURE_COLUMN_CREATE_NEW;
      const int col = surfaceShapeFile.getColumnWithName(gaussianCurvatureColumnName);
      if (col >= 0) {
         gaussianColumnNumber = col;
      }
   }
   
   //
   // Generate curvature
   //
   BrainModelSurfaceCurvature bmsc(&brainSet,
                                   bms,
                                   &surfaceShapeFile,
                                   meanColumnNumber,
                                   gaussianColumnNumber,
                                   meanCurvatureColumnName,
                                   gaussianCurvatureColumnName);
   bmsc.execute();
   
   //
   // Write the surface shape file
   //
   surfaceShapeFile.writeFile(outputSurfaceShapeFileName);
}

      

