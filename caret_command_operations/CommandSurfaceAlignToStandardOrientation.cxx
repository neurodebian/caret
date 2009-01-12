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

#include <iostream>

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceAlignToStandardOrientation.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceAlignToStandardOrientation::CommandSurfaceAlignToStandardOrientation()
   : CommandBase("-surface-align-to-standard-orientation",
                 "SURFACE ALIGN TO STANDARD ORIENTATION")
{
}

/**
 * destructor.
 */
CommandSurfaceAlignToStandardOrientation::~CommandSurfaceAlignToStandardOrientation()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceAlignToStandardOrientation::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Fiducial Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Sphere or Flat Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Output Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Border Projection File Name", 
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addString("Central Sulcus Border Name", 
                       "LANDMARK.CentralSulcus");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceAlignToStandardOrientation::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-fiducial-coordinate-file-name> \n"
       + indent9 + "<input-sphere-or-flat-coordinate-file-name> \n"
       + indent9 + "<output-coordinate-file-name> \n"
       + indent9 + "<topology-file-name> \n"
       + indent9 + "<border-projection-file-name> \n"
       + indent9 + "<central-sulcus-border-name> \n"
       + indent9 + "\n"
       + indent9 + "Align a surface to standard orienation.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceAlignToStandardOrientation::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputFiducialCoordinateFileName =
      parameters->getNextParameterAsString("Input Fiducial Coordinate File Name");
   const QString inputSphereOrFlatCoordinateFileName =
      parameters->getNextParameterAsString("Input Sphere or Flat Coordinate File Name");
   const QString outputCoordinateFileName =
      parameters->getNextParameterAsString("Output Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString borderProjectionFileName =
      parameters->getNextParameterAsString("Border Projection File Name");
   const QString centralSulcusBorderName =
      parameters->getNextParameterAsString("Central Sulcus Border Name");
   checkForExcessiveParameters();
   
   // 
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     inputFiducialCoordinateFileName,
                     inputSphereOrFlatCoordinateFileName,
                     true); 
   BrainModelSurface* fiducialBMS = 
      brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialBMS == NULL) {
      throw CommandException("unable to find fiducial surface.");
   }
   BrainModelSurface* bms = 
      brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (bms == NULL) {
      bms = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
   }
   if (bms == NULL) {
      bms = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR);
   }
   if (bms == NULL) {
      throw CommandException("unable to find spherical or flat surface.");
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
   // Check surface type
   //
   switch (bms->getSurfaceType()) {
      case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
      case BrainModelSurface::SURFACE_TYPE_FLAT:
      case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
         break;
      case BrainModelSurface::SURFACE_TYPE_RAW:
      case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
      case BrainModelSurface::SURFACE_TYPE_INFLATED:
      case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
      case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
      case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
      case BrainModelSurface::SURFACE_TYPE_HULL:
      case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
      case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
         throw CommandException("The surface type must be flat or spherical but is \""
                                + bms->getSurfaceTypeName()
                                + "\"");
         break;
   }
   
   // 
   // Check structure
   //
   switch (bms->getStructure().getType()) {
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_BOTH: 
      case Structure::STRUCTURE_TYPE_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_SUBCORTICAL:
      case Structure::STRUCTURE_TYPE_ALL:
      case Structure::STRUCTURE_TYPE_INVALID:
         throw CommandException("Structure must be left or right but is \""
                                + bms->getStructure().getTypeAsString()
                                + "\"");
         break;
   }
   
   //
   // Get the border
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(borderProjectionFileName);
   const BorderProjection* centralSulcusBorderProjection =
      borderProjectionFile.getFirstBorderProjectionByName(centralSulcusBorderName);
   if (centralSulcusBorderProjection == NULL) {
      throw CommandException("No border projection named \""
                             + centralSulcusBorderName
                             + "\" was found in the border projection file.");
   }
   
   //
   // Align the surface
   //
   bms->alignToStandardOrientation(fiducialBMS,
                                   centralSulcusBorderProjection,
                                   false,
                                   true);
                                   
   //
   // Write the coordinate file
   //
   bms->getCoordinateFile()->writeFile(outputCoordinateFileName);
}

      

