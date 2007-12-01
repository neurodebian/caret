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
#include "BrainModelSurfaceSulcalDepthWithNormals.h"
#include "BrainSet.h"
#include "CommandSurfaceSulcalDepth.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SurfaceShapeFile.h"

/**
 * constructor.
 */
CommandSurfaceSulcalDepth::CommandSurfaceSulcalDepth()
   : CommandBase("-surface-sulcal-depth",
                 "SURFACE SULCAL DEPTH")
{
}

/**
 * destructor.
 */
CommandSurfaceSulcalDepth::~CommandSurfaceSulcalDepth()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceSulcalDepth::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Fiducial Coordinate File", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Closed Topology File", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Hull VTK File", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Input Surface Shape File", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Output Surface Shape File", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Output Hull Coordinate File", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addVariableListOfParameters("Surface Sulcal Depth Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceSulcalDepth::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<fiducial-coordinate-file-name>\n"
       + indent9 + "<closed-topology-file-name>\n"
       + indent9 + "<hull-vtk-file-name>\n"
       + indent9 + "<input-surface-shape-file-name>\n"
       + indent9 + "<output-surface-shape-file-name>\n"
       + indent9 + "<output-hull-coordinate-file-name>\n"
       + indent9 + "[-depth-column-name  name]\n"
       + indent9 + "[-smoothed-depth-column-name  name]\n"
       + indent9 + "[-no-depth]\n"
       + indent9 + "[-no-smoothed-depth]\n"
       + indent9 + "\n"
       + indent9 + "Generate sulcal depth for a surface.\n"
       + indent9 + "\n"
       + indent9 + "\"" + getDefaultDepthColumnName() + "\" is the default name for the depth column.\n"
       + indent9 + "\"" + getDefaultSmoothedDepthColumnName() + "\" is the default name for the depth column.\n"
       + indent9 + "\n"
       + indent9 + "Note: The input surface shape file does not need to exist.\n"
       + indent9 + "If a column exists in the input surface shape file with the\n"
       + indent9 + "name of the depth or smoothed depth column name, the column\n"
       + indent9 + "is overwritten.\n"
       + "\n");
   
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceSulcalDepth::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get required parameters
   //
   const QString fiducialCoordinateFileName =
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
   const QString closedTopologyFileName =
      parameters->getNextParameterAsString("Closed Topology File Name");
   const QString hullVtkFileName =
      parameters->getNextParameterAsString("Hull VTK File Name");
   const QString inputSurfaceShapeFileName =
      parameters->getNextParameterAsString("Input Surface Shape File Name");
   const QString outputSurfaceShapeFileName =
      parameters->getNextParameterAsString("Output Surface Shape File Name");
   const QString outputHullCoordinateFileName = 
      parameters->getNextParameterAsString("Output Hull Coordinate File Name");

   //
   // Depth columns
   //
   int depthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_CREATE_NEW;
   int smoothedDepthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_CREATE_NEW;
   QString depthColumnName = getDefaultDepthColumnName();
   QString smoothedDepthColumnName = getDefaultSmoothedDepthColumnName();
   
   //
   // Check optional parameters
   //
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Sulcal Depth Param");
      if (paramName == "-depth-column-name") {
         depthColumnName = parameters->getNextParameterAsString("Depth Column Name");
      }
      else if (paramName == "-no-depth") {
         depthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE;
      }
      else if (paramName == "-no-smoothed-depth") {
         smoothedDepthColumn = BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE;
      }
      else if (paramName == "-smoothed-depth-column-name") {
         smoothedDepthColumnName = parameters->getNextParameterAsString("Depth Column Name");
      }
      else {
         throw CommandException("unknown parameter: "
                                + paramName);
      }
   }
   
   // Create a brain set
   //
   BrainSet brainSet(closedTopologyFileName,
                     fiducialCoordinateFileName,
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
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Surface Shape File
   //
   SurfaceShapeFile surfaceShapeFile;
   if (inputSurfaceShapeFileName.isEmpty() == false) {
      if (QFile::exists(inputSurfaceShapeFileName)) {
         surfaceShapeFile.readFile(inputSurfaceShapeFileName);
         
         //
         // Does depth column exist, if so, reuse it
         //
         if (depthColumn != BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE) {
            const int depCol = surfaceShapeFile.getColumnWithName(depthColumnName);
            if (depCol >= 0) {
               depthColumn = depCol;
            }
         }
         
         //
         // Does smoothed depth column exist, if so, reuse it
         //
         if (smoothedDepthColumn != BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_DO_NOT_GENERATE) {
            const int depCol = surfaceShapeFile.getColumnWithName(smoothedDepthColumnName);
            if (depCol >= 0) {
               smoothedDepthColumn = depCol;
            }
         }
      }
   }
   
   //
   // Do sulcal depth
   // 
   CoordinateFile hullCoordinateFile;
   BrainModelSurfaceSulcalDepthWithNormals sd(&brainSet,
                                              bms,
                                              hullVtkFileName,
                                              &surfaceShapeFile,
                                              5,
                                              100,
                                              depthColumn,
                                              smoothedDepthColumn,
                                              depthColumnName,
                                              smoothedDepthColumnName, 
                                              &hullCoordinateFile);
   sd.execute();   
   
   //
   // Write the output files
   //
   surfaceShapeFile.writeFile(outputSurfaceShapeFileName);
   hullCoordinateFile.writeFile(outputHullCoordinateFileName);
}

      

