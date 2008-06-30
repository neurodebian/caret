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

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformationSphericalSlits.h"
#include "BrainSet.h"
#include "CommandSurfaceRegistrationPrepareSlits.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSurfaceRegistrationPrepareSlits::CommandSurfaceRegistrationPrepareSlits()
   : CommandBase("-surface-register-prepare-slits",
                 "SURFACE SPHERICAL REGISTRATION PREPARE SLIT LANDMARKS")
{
}

/**
 * destructor.
 */
CommandSurfaceRegistrationPrepareSlits::~CommandSurfaceRegistrationPrepareSlits()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceRegistrationPrepareSlits::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Input Spherical Coordinate File Name", FileFilters::getCoordinateSphericalFileFilter());
   paramsOut.addFile("Input Closed Topology File Name", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Input Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Cut Spherical Coordinate File Name", FileFilters::getCoordinateSphericalFileFilter());
   paramsOut.addFile("Output Cut Topology File Name", FileFilters::getTopologyCutFileFilter());
   paramsOut.addFile("Output Closed Smoothed Spherical Coordinate File Name", FileFilters::getCoordinateSphericalFileFilter());
   paramsOut.addFile("Output Closed Topology File Name", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Output Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRegistrationPrepareSlits::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "<input-spherical-coordinate-file-name>\n"
       + indent9 + "<input-closed-topology-file-name>\n"
       + indent9 + "<input-border-projection-file-name>\n"
       + indent9 + "<output-cut-spherical-coordinate-file-name>\n"
       + indent9 + "<output-cut-topology-file-name>\n"
       + indent9 + "<output-closed-smoothed-spherical-coordinate-file-name>\n"
       + indent9 + "<output-closed-topology-file-name>.\n"
       + indent9 + "<output-border-projection-file-name>\n"
       + indent9 + "\n"
       + indent9 + "For all borders that begin with the name \""
                 + BrainModelSurfaceDeformationSphericalSlits::getSlitLandmarkBorderNamePrefix() + "\"\n"
       + indent9 + "in the input border projection file, use the border to cut\n"
       + indent9 + "the input spherical surface.  The sphere is then smoothed\n"
       + indent9 + "to open up the holes caused by the cuts.  For each of the\n"
       + indent9 + "slit borders, two new borders are created around the hole \n"
       + indent9 + "with the suffixes \"PatchA\" and \"PatchB\".  The slit \n"
       + indent9 + "borders are deleted.  Use the output-closed-smoothed-\n"
       + indent9 + "spherical-coordinate-file, the output-closed-topology-file,\n"
       + indent9 + "and the output-border-projection-file as inputs to the\n"
       + indent9 + "spherical registration command.\n"
       + indent9 + "\n"
       + indent9 + "When performing registration between species, there may\n"
       + indent9 + "be functional areas that are present in one species but \n"
       + indent9 + "not in the other.  In this case, draw a border at the\n"
       + indent9 + "location of the \"missing\" area and use this command\n"
       + indent9 + "to open up that region.  On the target subject, draw\n"
       + indent9 + "two corresponding borders, with the counter-clockwise\n"
       + indent9 + "half ending in \"PatchA\" and the clockwise half ending\n"
       + indent9 + "in \"PatchB\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceRegistrationPrepareSlits::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the input parameters
   //
   const QString specFileName =
      parameters->getNextParameterAsString("Spec File Name");
   const QString inputSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Input Spherical Coordinate File Name");
   const QString inputClosedTopologyFileName =
      parameters->getNextParameterAsString("Input Closed Topology File Name");
   const QString inputBorderProjectionFileName =
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputCutSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Output Cut Spherical Coordinate File Name");
   const QString outputCutTopologyFileName =
      parameters->getNextParameterAsString("Output Cut Topology File Name");
   const QString outputClosedSmoothedSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Output Closed Smoothed Spherical Coordinate File Name");
   const QString outputClosedTopologyFileName =
      parameters->getNextParameterAsString("Output Closed Topology File Name");
   const QString outputBorderProjectionFileName =
      parameters->getNextParameterAsString("Output Border Projection File Name");
   checkForExcessiveParameters();
   
   //
   // Create a brain set
   //
   BrainSet brainSet(inputClosedTopologyFileName,
                     inputSphericalCoordinateFileName,
                     "",
                     true);
   
   //
   // Verify files read
   //
   BrainModelSurface* inputSphericalSurface = 
      brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (inputSphericalSurface == NULL) {
      throw CommandException("Problem loading spherical surface.");
   }
   if (inputSphericalSurface->getTopologyFile() == NULL) {
      throw CommandException("Topology is missing.");
   }
   
   //
   // Read tje border projection file
   //
   brainSet.readBorderProjectionFile(inputBorderProjectionFileName, false, false);
   
   BorderProjectionFile bpf;
   BrainModelBorderSet* bmbs = brainSet.getBorderSet();
   bmbs->copyBordersToBorderProjectionFile(bpf);
   if (bpf.getNumberOfBorderProjections() <= 0) {
      throw CommandException("Border projection file is empty.");
   }
      
   //
   // Set the name of the spec file
   //
   brainSet.setSpecFileName(specFileName);
   
   //
   // Perform the deformation
   //
   BrainModelSurfaceDeformationSphericalSlits bmsdss(&brainSet,
                                                     inputSphericalSurface,
                                                     &bpf);
   bmsdss.execute();

   //
   // Write the output files
   // Do topology first so it gets the proper name that is used by coord writing
   //
   TopologyFile* outputCutTopologyFile = bmsdss.getCutSphericalSurfaceTopologyFile();
   if (outputCutTopologyFile != NULL) {
      if (outputCutTopologyFileName.isEmpty() == false) {
         brainSet.writeTopologyFile(outputCutTopologyFileName,
                              TopologyFile::TOPOLOGY_TYPE_CUT,
                              outputCutTopologyFile);
      }
   }
   BrainModelSurface* outputSphericalBMS = bmsdss.getCutSphericalSurface();
   if (outputSphericalBMS != NULL) {
      if (outputCutSphericalCoordinateFileName.isEmpty() == false) {
         brainSet.writeCoordinateFile(outputCutSphericalCoordinateFileName,
                                BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                outputSphericalBMS->getCoordinateFile(),
                                true);
      }
   }
   TopologyFile* outputClosedTopologyFile = bmsdss.getSmoothedClosedSphericalSurfaceTopologyFile();
   if (outputClosedTopologyFile != NULL) {
      if (outputClosedTopologyFileName.isEmpty() == false) {
         brainSet.writeTopologyFile(outputClosedTopologyFileName,
                              TopologyFile::TOPOLOGY_TYPE_CLOSED,
                              outputClosedTopologyFile);
      }
   }
   BrainModelSurface* outputSmoothedSphericalBMS = bmsdss.getSmoothedClosedSphericalSurface();
   if (outputSmoothedSphericalBMS != NULL) {
      if (outputClosedSmoothedSphericalCoordinateFileName.isEmpty() == false) {
         brainSet.writeCoordinateFile(outputClosedSmoothedSphericalCoordinateFileName,
                                BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                                outputSmoothedSphericalBMS->getCoordinateFile(),
                                true);
      }
   }
   
   //
   // Write the output border projection file
   //
   if (outputBorderProjectionFileName.isEmpty() == false) {
      brainSet.writeBorderProjectionFile(outputBorderProjectionFileName,
                                   "",
                                   "",
                                   false);
   }
}

      

