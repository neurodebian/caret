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
#include "CommandSurfaceRegistrationSphericalSpecOnly.h"
#include "DeformationMapFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSurfaceRegistrationSphericalSpecOnly::CommandSurfaceRegistrationSphericalSpecOnly()
   : CommandBase("-surface-register-sphere-spec-only",
                 "SURFACE SPHERICAL REGISTRATION SPEC ONLY")
{
}

/**
 * destructor.
 */
CommandSurfaceRegistrationSphericalSpecOnly::~CommandSurfaceRegistrationSphericalSpecOnly()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceRegistrationSphericalSpecOnly::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addString("Options", "N");
   paramsOut.addFile("Deformation Map File Name", FileFilters::getDeformationMapFileFilter());
   paramsOut.addFile("Individual Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Atlas Spec File Name", FileFilters::getSpecFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRegistrationSphericalSpecOnly::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<options>\n"
       + indent9 + "<deformation-map-file-name-containing-registration-parameters>\n"
       + indent9 + "<individual-spec-file-name>\n"
       + indent9 + "<atlas-spec-file-name>\n"
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
CommandSurfaceRegistrationSphericalSpecOnly::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   QString indivBorderFileName;
   QString indivClosedTopoFileName;
   QString indivCutTopologyFileName;
   QString indivFiducialCoordFileName;
   QString indivSphericalCoordFileName;
   QString indivFlatCoordFileName;
   QString atlasBorderFileName;
   QString atlasClosedTopoFileName;
   QString atlasCutTopologyFileName;
   QString atlasFiducialCoordFileName;
   QString atlasSphericalCoordFileName;
   QString atlasFlatCoordFileName;
   
   const QString options = 
      parameters->getNextParameterAsString("Options");
   const QString defMapFileName = 
      parameters->getNextParameterAsString("Deformation Map File Name");
   QString individualSpecFileName = 
      parameters->getNextParameterAsString("Individual Spec File Name");
   QString atlasSpecFileName = 
      parameters->getNextParameterAsString("Atlas Spec File Name");
   
   QString errorMessage;
   
   SpecFile individualSpecFile;
   individualSpecFile.readFile(individualSpecFileName);
   
   if (individualSpecFile.borderProjectionFile.getNumberOfFiles() == 1) {
      indivBorderFileName = individualSpecFile.borderProjectionFile.getFileName(0);
   }
   else {
      errorMessage.append("Indiv spec file does not have exactly one border projection file.\n");
   }
   if (individualSpecFile.closedTopoFile.getNumberOfFiles() == 1) {
      indivClosedTopoFileName = individualSpecFile.closedTopoFile.getFileName(0);
   }
   else {
      errorMessage.append("Indiv spec file does not have exactly one closed topology file.\n");
   }
   if (individualSpecFile.cutTopoFile.getNumberOfFiles() > 1) {
      errorMessage.append("Indiv spec file has more than one cut topology file.\n");
   }
   else if (individualSpecFile.cutTopoFile.getNumberOfFiles() == 1) {
      indivCutTopologyFileName = individualSpecFile.cutTopoFile.getFileName(0);
   }
   if (individualSpecFile.fiducialCoordFile.getNumberOfFiles() == 1) {
      indivFiducialCoordFileName = individualSpecFile.fiducialCoordFile.getFileName(0);
   }
   else {
      errorMessage.append("Indiv spec file does not have exactly one fiducial coordinate file.\n");
   }
   if (individualSpecFile.sphericalCoordFile.getNumberOfFiles() == 1) {
      indivSphericalCoordFileName = individualSpecFile.sphericalCoordFile.getFileName(0);
   }
   else {
      errorMessage.append("Indiv spec file does not have exactly one spherical coordinate file.\n");
   }
   if (individualSpecFile.flatCoordFile.getNumberOfFiles() > 1) {
      errorMessage.append("Indiv spec file has more than one flat coordinate file.\n");
   }
   else if (individualSpecFile.flatCoordFile.getNumberOfFiles() == 1) {
      indivFlatCoordFileName = individualSpecFile.flatCoordFile.getFileName(0);
   }

   SpecFile atlasSpecFile;
   atlasSpecFile.readFile(atlasSpecFileName);
   
   if (atlasSpecFile.borderProjectionFile.getNumberOfFiles() == 1) {
      atlasBorderFileName = atlasSpecFile.borderProjectionFile.getFileName(0);
   }
   else {
      errorMessage.append("atlas spec file does not have exactly one border projection file.\n");
   }
   if (atlasSpecFile.closedTopoFile.getNumberOfFiles() == 1) {
      atlasClosedTopoFileName = atlasSpecFile.closedTopoFile.getFileName(0);
   }
   else {
      errorMessage.append("atlas spec file does not have exactly one closed topology file.\n");
   }
   if (atlasSpecFile.cutTopoFile.getNumberOfFiles() > 1) {
      errorMessage.append("atlas spec file has more than one cut topology file.\n");
   }
   else if (atlasSpecFile.cutTopoFile.getNumberOfFiles() == 1) {
      atlasCutTopologyFileName = atlasSpecFile.cutTopoFile.getFileName(0);
   }
   if (atlasSpecFile.fiducialCoordFile.getNumberOfFiles() == 1) {
      atlasFiducialCoordFileName = atlasSpecFile.fiducialCoordFile.getFileName(0);
   }
   else {
      errorMessage.append("atlas spec file does not have exactly one fiducial coordinate file.\n");
   }
   if (atlasSpecFile.sphericalCoordFile.getNumberOfFiles() == 1) {
      atlasSphericalCoordFileName = atlasSpecFile.sphericalCoordFile.getFileName(0);
   }
   else {
      errorMessage.append("atlas spec file does not have exactly one spherical coordinate file.\n");
   }
   if (atlasSpecFile.flatCoordFile.getNumberOfFiles() > 1) {
      errorMessage.append("atlas spec file has more than one flat coordinate file.\n");
   }
   else if (atlasSpecFile.flatCoordFile.getNumberOfFiles() == 1) {
      atlasFlatCoordFileName = atlasSpecFile.flatCoordFile.getFileName(0);
   }

   if (errorMessage.isEmpty() == false) {
      throw CommandException(errorMessage);
   }

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

      

