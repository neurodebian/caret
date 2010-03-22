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

#include <QFileInfo>

#include "BrainModelSurfaceDeformationSpherical.h"
#include "BrainSet.h"
#include "CommandSurfaceRegistrationSpherical.h"
#include "DeformationMapFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSurfaceRegistrationSpherical::CommandSurfaceRegistrationSpherical()
   : CommandBase("-surface-register-sphere",
                 "SURFACE SPHERICAL REGISTRATION")
{
}

/**
 * destructor.
 */
CommandSurfaceRegistrationSpherical::~CommandSurfaceRegistrationSpherical()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceRegistrationSpherical::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addString("Options", "N");
   paramsOut.addFile("Deformation Map File Name", FileFilters::getDeformationMapFileFilter());
   paramsOut.addFile("Individual Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Individual Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Individual Closed Topology File Name", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Individual Cut Topology File Name", FileFilters::getTopologyCutFileFilter());
   paramsOut.addFile("Individual Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Individual Spherical Coordinate File Name", FileFilters::getCoordinateSphericalFileFilter());
   paramsOut.addFile("Individual Flat Coordinate File Name", FileFilters::getCoordinateFlatFileFilter());
   paramsOut.addFile("Atlas Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Atlas Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Atlas Closed Topology File Name", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Atlas Cut Topology File Name", FileFilters::getTopologyCutFileFilter());
   paramsOut.addFile("Atlas Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Atlas Spherical Coordinate File Name", FileFilters::getCoordinateSphericalFileFilter());
   paramsOut.addFile("Atlas Flat Coordinate File Name", FileFilters::getCoordinateFlatFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRegistrationSpherical::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<options>\n"
       + indent9 + "<deformation-map-file-name-containing-registration-parameters>\n"
       + indent9 + "<individual-spec-file-name>\n"
       + indent9 + "<atlas-spec-file-name>\n"
       + indent9 + "\n"
       + indent9 + "The above method requires that the each of the spec file have one and only\n"
       + indent9 + "one border projection file, closed topology file,fiducial coordinate file,\n"
       + indent9 + "and spherical coordinate file.  The spec files may have zero or one\n"
       + indent9 + "cut topology file and flat coordinate file.\n"
       + indent9 + "\n"
       + indent9 + "Each of the individual or atlas data files MUST be listed\n"
       + indent9 + "in the respective individual or atlas spec files.\n"
       + indent9 + "\n"
       + indent9 + "<options>\n"
       + indent9 + "<deformation-map-file-name-containing-registration-parameters>\n"
       + indent9 + "<individual-spec-file-name>\n"
       + indent9 + "<individual-border-projection-file-name>\n"
       + indent9 + "<individual-closed-topology-file-name>\n"
       + indent9 + "<individual-cut-topology-file-name>\n"
       + indent9 + "<individual-fiducial-coordinate-file-name>\n"
       + indent9 + "<individual-spherical-coordinate-file-name>\n"
       + indent9 + "<individual-flat-coordiante-file-name>\n"
       + indent9 + "<atlas-spec-file-name>\n"
       + indent9 + "<atlas-border-projection-file-name>\n"
       + indent9 + "<atlas-closed-topology-file-name>\n"
       + indent9 + "<atlas-cut-topology-file-name>\n"
       + indent9 + "<atlas-fiducial-coordinate-file-name>\n"
       + indent9 + "<atlas-spherical-coordinate-file-name>\n"
       + indent9 + "<atlas-flat-coordiante-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Peform spherical registration.\n"
       + indent9 + "\n"
       + indent9 + "Some files, such as the flat coordinate files or deformation map file\n"
       + indent9 + "are optional and in this case the name should be specified with two\n"
       + indent9 + "consecutive double quotes (eg: \"\").\n"
       + indent9 + "\n"
       + indent9 + "options - a single character either 'Y' or 'N'  \n"
       + indent9 + "   Y - deform the individual to the atlas and the atlas to the individual.\n"
       + indent9 + "   N - deform the individual to the atlas only.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceRegistrationSpherical::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString options = 
      parameters->getNextParameterAsString("Options");
   const QString defMapFileName = 
      parameters->getNextParameterAsString("Deformation Map File Name");
   QString individualSpecFileName = 
      parameters->getNextParameterAsString("Individual Spec File Name");
   const QString indivBorderFileName = 
      parameters->getNextParameterAsString("Individual Border Projection File Name");
   const QString indivClosedTopoFileName = 
      parameters->getNextParameterAsString("Individual Closed Topology File Name");
   const QString indivCutTopologyFileName = 
      parameters->getNextParameterAsString("Individual Cut Topology File Name");
   const QString indivFiducialCoordFileName = 
      parameters->getNextParameterAsString("Individual Fiducial Coordinate File Name");
   const QString indivSphericalCoordFileName = 
      parameters->getNextParameterAsString("Individual Spherical Coordinate File Name");
   const QString indivFlatCoordFileName = 
      parameters->getNextParameterAsString("Individual Flat Coordinate File Name");
   QString atlasSpecFileName = 
      parameters->getNextParameterAsString("Atlas Spec File Name");
   const QString atlasBorderFileName = 
      parameters->getNextParameterAsString("Atlas Border Projection File Name");
   const QString atlasClosedTopoFileName = 
      parameters->getNextParameterAsString("Atlas Closed Topology File Name");
   const QString atlasCutTopologyFileName = 
      parameters->getNextParameterAsString("Atlas Cut Topology File Name");
   const QString atlasFiducialCoordFileName = 
      parameters->getNextParameterAsString("Atlas Fiducial Coordinate File Name");
   const QString atlasSphericalCoordFileName = 
      parameters->getNextParameterAsString("Atlas Spherical Coordinate File Name");
   const QString atlasFlatCoordFileName = 
      parameters->getNextParameterAsString("Atlas Flat Coordinate File Name");

   //
   // Read in the deformation map (if specified)
   //
   DeformationMapFile defMapFile;
   if (defMapFileName.isEmpty() == false) {
      defMapFile.readFile(defMapFileName);
   }
   
   //
   // Process options
   //   
   defMapFile.setDeformBothWays(false);
   if (options.length() > 0) {
      if (options[0] == 'Y') {
         defMapFile.setDeformBothWays(true);
      }
   }
   
   //
   // Spherical deformation
   //
   defMapFile.setFlatOrSphereSelection(DeformationMapFile::DEFORMATION_TYPE_SPHERE);

   //
   // Make paths absolute
   //   
   if (QFileInfo(individualSpecFileName).isRelative()) {
      individualSpecFileName = QFileInfo(individualSpecFileName).absoluteFilePath();
   }
   if (QFileInfo(atlasSpecFileName).isRelative()) {
      atlasSpecFileName = QFileInfo(atlasSpecFileName).absoluteFilePath();
   }
      
   //
   // Set the files
   //
   defMapFile.setSourceSpecFileName(individualSpecFileName);
   defMapFile.setSourceBorderFileName(indivBorderFileName, DeformationMapFile::BORDER_FILE_PROJECTION);
   defMapFile.setSourceClosedTopoFileName(indivClosedTopoFileName);
   defMapFile.setSourceCutTopoFileName(indivCutTopologyFileName);
   defMapFile.setSourceFiducialCoordFileName(indivFiducialCoordFileName);
   defMapFile.setSourceSphericalCoordFileName(indivSphericalCoordFileName);
   defMapFile.setSourceDeformedFlatCoordFileName(indivFlatCoordFileName);
   defMapFile.setTargetSpecFileName(atlasSpecFileName);
   defMapFile.setTargetBorderFileName(0, atlasBorderFileName, DeformationMapFile::BORDER_FILE_PROJECTION);
   defMapFile.setTargetClosedTopoFileName(atlasClosedTopoFileName);
   defMapFile.setTargetCutTopoFileName(atlasCutTopologyFileName);
   defMapFile.setTargetFiducialCoordFileName(atlasFiducialCoordFileName);
   defMapFile.setTargetSphericalCoordFileName(atlasSphericalCoordFileName);
   defMapFile.setTargetFlatCoordFileName(atlasFlatCoordFileName);
   
   //
   // Run the deformation
   //
   BrainSet bs;
   BrainModelSurfaceDeformationSpherical alg(&bs,
                                             &defMapFile);
   alg.execute();
}

      

