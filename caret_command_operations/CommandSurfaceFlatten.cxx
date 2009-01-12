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

#include "AreaColorFile.h"
#include "BorderProjectionFile.h"
#include "BrainSet.h"
#include "BrainModelSurfaceFlattenHemisphere.h"
#include "CommandSurfaceFlatten.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSurfaceFlatten::CommandSurfaceFlatten()
   : CommandBase("-surface-flatten",
                 "SURFACE FLATTEN")
{
}

/**
 * destructor.
 */
CommandSurfaceFlatten::~CommandSurfaceFlatten()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceFlatten::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandSurfaceFlatten::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-fiduical-coordinate-file-name>\n"
       + indent9 + "<input-ellispoid-or-spherical-coordinate-file-name>\n"
       + indent9 + "<input-closed-topology-file-name>\n"
       + indent9 + "<input-flatten-border-projection-file-name>\n"
       + indent9 + "<output-fiducial-smoothed-medial-wall-coordinate-file-name>\n"
       + indent9 + "<output-spherical-coordinate-file-name>\n"
       + indent9 + "<output-initial-flat-surface-coordinate-file-name>\n"
       + indent9 + "<output-open-topology-file-name>\n"
       + indent9 + "<output-cut-topology-file-name>\n"
       + indent9 + "<input-paint-file-name>\n"
       + indent9 + "<output-paint-file-name>\n"
       + indent9 + "<input-area-color-file-name>\n"
       + indent9 + "<output-area-color-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Flatten the ellipsoid/spherical surface.\n"
       + indent9 + "\n"
       + indent9 + "The input border projection file must contain a border \n"
       + indent9 + "projection that identifies the medial wall (its name \n"
       + indent9 + "must be \"FLATTEN.HOLE.MedialWall\") and cuts (names of \n"
       + indent9 + "cuts start with \"FLATTEN.CUT.Std.\".)\n"
       + indent9 + "\n"
       + indent9 + "All of the output file names are optional.  If you do not\n"
       + indent9 + "want an output file written, specify the file's name as\n"
       + indent9 + "two consecutive double quotes (\"\", an empty string).\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceFlatten::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString fiducialCoordinateFileName =
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
   const QString ellipsoidSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Ellipsoid/Spherical Coordinate File Name");
   const QString closedTopologyFileName =
      parameters->getNextParameterAsString("Closed Topology File Name");
   const QString flattenBorderProjectionFileName =
      parameters->getNextParameterAsString("Flatten Border Projection File Name");
   const QString outputFiducialSmoothedMedialWallCoordinateFileName =
      parameters->getNextParameterAsString("Output Fiducial Smoothed Medial Wall Coordinate File Name");
   const QString outputSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Output Spherical Coordinate File Name");
   const QString outputInitialFlatCoordinateFileName =
      parameters->getNextParameterAsString("Output Initial Flat Coordinate File Name");
   const QString outputOpenTopologyFileName =
      parameters->getNextParameterAsString("Output Open Topology File Name");
   const QString outputCutTopologyFileName =   
      parameters->getNextParameterAsString("Output Cut Topology File Name");
   const QString inputPaintFileName =
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName =
      parameters->getNextParameterAsString("Output Paint File Name");
   const QString inputAreaColorFileName =
      parameters->getNextParameterAsString("Input Area Color File Name");
   const QString outputAreaColorFileName =
      parameters->getNextParameterAsString("Output Area Color File Name");
   checkForExcessiveParameters();
   
   //
   // Read spec file
   //
   SpecFile specFile;

   //
   // Set the selected files
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getClosedTopoFileTag(), closedTopologyFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getFiducialCoordFileTag(), fiducialCoordinateFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getEllipsoidCoordFileTag(), ellipsoidSphericalCoordinateFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, "Temp.spec", errorMessage)) {
      throw CommandException("Reading spec file: " + errorMessage);
   }
   
   //
   // Find the fiducial surface
   //
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialSurface == NULL) {
      throw CommandException("Unable to find fiducial surface.");
   }
   
   //
   // Find the flat surface
   //
   BrainModelSurface* ellipsoidSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL);
   if (ellipsoidSurface == NULL) {
      throw CommandException("Unable to find ellipsoid/spherical surface.");
   }
   
   //
   // Read the borders
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(flattenBorderProjectionFileName);
   
   //
   // Read the paint file
   //
   PaintFile paintFile;
   if (inputPaintFileName.isEmpty() == false) {
      paintFile.readFile(inputPaintFileName);
   }
   
   //
   // Read the area color file
   //
   AreaColorFile areaColorFile;
   if (inputAreaColorFileName.isEmpty() == false) {
      areaColorFile.readFile(inputAreaColorFileName);
   }
   
   //
   // Execute the algorithm
   //
   BrainModelSurfaceFlattenHemisphere bmsfh(&brainSet,
                                            fiducialSurface,
                                            ellipsoidSurface,
                                            &borderProjectionFile,
                                            &paintFile,
                                            &areaColorFile,
         (outputFiducialSmoothedMedialWallCoordinateFileName.isEmpty() == false),
                                            false);
   bmsfh.execute();
   
   //
   // Write paint file
   //
   if (outputPaintFileName.isEmpty() == false) {
      paintFile.writeFile(outputPaintFileName);
   }

   //
   // Write area color file
   //
   if (outputAreaColorFileName.isEmpty() == false) {
      areaColorFile.writeFile(outputAreaColorFileName);
   }
   
   //
   // Write the fiducial surface with smoothed medial wall name surface
   //
   if (outputFiducialSmoothedMedialWallCoordinateFileName.isEmpty() == false) {
      BrainModelSurface* bms = bmsfh.getFiducialSurfaceWithSmoothedMedialWall();
      if (bms != NULL) {
         CoordinateFile* cf = bms->getCoordinateFile();
         cf->writeFile(outputFiducialSmoothedMedialWallCoordinateFileName);
      }
   }
   
   //
   // Write the spherical surface
   //
   if (outputSphericalCoordinateFileName.isEmpty() == false) {
      BrainModelSurface* bms = bmsfh.getSphericalSurface();
      if (bms != NULL) {
         CoordinateFile* cf = bms->getCoordinateFile();
         cf->writeFile(outputSphericalCoordinateFileName);
      }
   }
   
   //
   // Write the initial flat surface
   //
   if (outputInitialFlatCoordinateFileName.isEmpty() == false) {
      BrainModelSurface* bms = bmsfh.getInitialFlatSurface();
      if (bms != NULL) {
         CoordinateFile* cf = bms->getCoordinateFile();
         cf->writeFile(outputInitialFlatCoordinateFileName);
      }
   }

   //
   // Write the open topology file
   //
   if (outputOpenTopologyFileName.isEmpty() == false) {
      TopologyFile* tf = bmsfh.getOpenTopologyFile();
      if (tf != NULL) {
         tf->writeFile(outputOpenTopologyFileName);
      }
   }

   //
   // Write the cut topology file
   //
   if (outputCutTopologyFileName.isEmpty() == false) {
      TopologyFile* tf = bmsfh.getCutTopologyFile();
      if (tf != NULL) {
         tf->writeFile(outputCutTopologyFileName);
      }
   }
}

      

