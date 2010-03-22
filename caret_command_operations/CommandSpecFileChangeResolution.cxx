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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <BrainModelSurfaceDeformDataFile.h>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformation.h"
#include "BrainModelSurfaceDeformationMapCreate.h"
#include "BrainModelSurfacePolyhedron.h"
#include "BrainModelSurfaceStandardSphere.h"
#include "BrainSet.h"
#include "CommandSpecFileChangeResolution.h"
#include "DeformationMapFile.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSpecFileChangeResolution::CommandSpecFileChangeResolution()
   : CommandBase("-spec-file-change-resolution",
                 "SPEC FILE CHANGE RESOLUTION")
{
}

/**
 * destructor.
 */
CommandSpecFileChangeResolution::~CommandSpecFileChangeResolution()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSpecFileChangeResolution::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();

   paramsOut.addFile("Input Spec File Name",
                     FileFilters::getSpecFileFilter());
   paramsOut.addDirectory("Output Directory Name");
   paramsOut.addInt("Number of Nodes", 2562);
}

/**
 * get full help information.
 */
QString 
CommandSpecFileChangeResolution::getHelpInformation() const
{
   BrainModelSurfacePolyhedron poly(NULL, 0);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-spec-file-name>\n"
       + indent9 + "<output-directory-name>\n"
       + indent9 + "<number-of-nodes>\n"
       + indent9 + "\n"
       + indent9 + "Change (typically reduce) the resolution of the surface \n"
       + indent9 + "files in the spec file.  This is accomplished by replacing\n"
       + indent9 + "spec file's spherical coordinate file with a spherical  \n"
       + indent9 + "coordinate file generated by iteratively subdividing an \n"
       + indent9 + "icosahedron.  If there is not a spherical surface, a \n"
       + indent9 + "spherical surface is created from the fiducial surface\n"
       + indent9 + "via inflation and smoothing.\n"
       + indent9 + "\n"
       + indent9 + "The output directory must not exist and will be created\n"
       + indent9 + "by this program.\n"
       + indent9 + "\n"
       + indent9 + "This command should be run from the directory containing\n"
       + indent9 + "the input spec file.\n"
       + indent9 + "\n"
       + indent9 + "Number of Nodes   Number of Triangles\n"
       + indent9 + "---------------   -------------------\n");

   std::vector<int> iterations, nodes, triangles;
   BrainModelSurfacePolyhedron::getNumberOfNodesAndTrianglesFromIterations(
                                                iterations, nodes, triangles);
   int numIters = static_cast<int>(iterations.size());
   for (int i = 0; i < numIters; i++) {
      helpInfo += (indent9 +
                   QString("%1%2\n").arg(nodes[i], 15).arg(triangles[i], 22));
   }

   BrainModelSurfaceStandardSphere::getNumberOfNodesAndTriangles(nodes, triangles);
   const int num = static_cast<int>(nodes.size());
   for (int i = 0; i < num; i++) {
      helpInfo += (indent9 +
                   QString("%1%2\n").arg(nodes[i], 15).arg(triangles[i], 22));
   }

   helpInfo += "\n";

   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSpecFileChangeResolution::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString originalPath = QDir::currentPath();

   QString deformedPrefix = ""; //LowRes_";

   //
   // Get the inputs
   //
   const QString usersSpecFileName =
      parameters->getNextParameterAsString("Spec File Name");
   const QString outputDirectoryName =
      parameters->getNextParameterAsString("Directory Name");
   const int numberOfNodes =
      parameters->getNextParameterAsInt("Number of Nodes");
   checkForExcessiveParameters();

   //
   // Validate number of nodes
   //
   std::vector<int> iterations, nodes, triangles;
   BrainModelSurfacePolyhedron::getNumberOfNodesAndTrianglesFromIterations(
                                                iterations, nodes, triangles);
   int numberOfIterations = -1;
   for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
      if (numberOfNodes == nodes[i]) {
         numberOfIterations = i;
         break;
      }
   }

   int stdSphereNumberOfNodes = -1;
   if (numberOfIterations < 0) {
      BrainModelSurfaceStandardSphere::getNumberOfNodesAndTriangles(
                                                 nodes, triangles);
      for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
         if (numberOfNodes == nodes[i]) {
            stdSphereNumberOfNodes = numberOfNodes;
            break;
         }
      }
   }
   if ((numberOfIterations < 0) &&
       (stdSphereNumberOfNodes < 0)) {
      throw CommandException("Invalid number of nodes: "
                             + QString::number(numberOfNodes));
   }

   //
   // create the output directory
   //
   this->createOutputDirectory(outputDirectoryName);

   //
   // Create the spherical target
   //
   BrainSet brainSet;
   if (numberOfIterations >= 0) {
      BrainModelSurfacePolyhedron tetra(&brainSet, numberOfIterations);
      tetra.execute();
   }
   else if (stdSphereNumberOfNodes > 0) {
      BrainModelSurfaceStandardSphere sphere(&brainSet, stdSphereNumberOfNodes);
      sphere.execute();
      brainSet.getLoadedFilesSpecFile()->clear();
      QDir::setCurrent(originalPath);
   }
   else {
      throw CommandException("Invalid number of nodes: "
                             + QString::number(numberOfNodes));
   }

   BrainModelSurface* targetSphericalSurface = brainSet.getBrainModelSurface(0);
   if (targetSphericalSurface == NULL) {
      throw CommandException("Failed to create resampled spherical surface.");
   }
   TopologyFile* tf = targetSphericalSurface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("Created spherical surface has no topology.");
   }

   //
   // Prefix for deformation sphere files
   //
   const QString defTargetPrefix = "def_sphere";
   const QString defTargetCoordName = defTargetPrefix
                                    + SpecFile::getCoordinateFileExtension();
   const QString defTargetTopoName = defTargetPrefix
                                    + SpecFile::getTopoFileExtension();
   const QString defTargetSpecName = deformedPrefix
                                   + FileUtilities::basename(usersSpecFileName);
   const QString defTargetDefMapName = defTargetPrefix
                                   + SpecFile::getDeformationMapFileExtension();

   //
   // Read the user's brain set
   //
   BrainSet* usersBrainSet = this->readUsersSpecFile(usersSpecFileName);

   BrainModelSurface* fiducialSurface =
           usersBrainSet->getBrainModelSurfaceOfType(
                   BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   QString fiducialSurfaceName;
   if (fiducialSurface != NULL) {
      fiducialSurfaceName = fiducialSurface->getFileName();
   }
   BrainModelSurface* usersSphericalSurface =
           usersBrainSet->getBrainModelSurfaceOfType(
                   BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (usersSphericalSurface == NULL) {
      usersSphericalSurface = createSphereFromUsersFiducial(usersBrainSet);
   }

   //
   // Set the radius of the target sphere
   //
   targetSphericalSurface->convertToSphereWithRadius(
                       usersSphericalSurface->getSphericalSurfaceRadius());

   //
   // Update items in target spec
   //
   brainSet.setSpecies(usersBrainSet->getSpecies());
   brainSet.setStructure(usersBrainSet->getStructure());
   brainSet.setSubject(usersBrainSet->getSubject());
   brainSet.setStereotaxicSpace(usersBrainSet->getStereotaxicSpace());
   
   //
   // Write out sphere for debugging
   //
   QDir::setCurrent(outputDirectoryName);
   brainSet.setSpecFileName(defTargetSpecName, false);
   CoordinateFile* cf = targetSphericalSurface->getCoordinateFile();
   brainSet.writeCoordinateFile(defTargetCoordName,
                                 targetSphericalSurface->getSurfaceType(),
                                 cf,
                                 true);
   brainSet.writeTopologyFile(defTargetTopoName,
                               tf->getTopologyType(),
                               tf);
   QDir::setCurrent(originalPath);

   //
   // Deformation Map File
   //
   DeformationMapFile defMapFile;
   
   //
   // Execute the deformation 
   //
   BrainModelSurfaceDeformationMapCreate bmsdmc(&brainSet,
                                                usersSphericalSurface,
                                                targetSphericalSurface,
                                                &defMapFile,
                                                BrainModelSurfaceDeformationMapCreate::DEFORMATION_SURFACE_TYPE_SPHERE);
   bmsdmc.execute();

   //
   // Write the deformation map file
   //
   if (QFileInfo(usersSpecFileName).isAbsolute()) {
      defMapFile.setSourceSpecFileName(usersSpecFileName);
   }
   else {
      defMapFile.setSourceSpecFileName(QDir::currentPath()
                                       + QDir::separator()
                                       + usersSpecFileName);
   }
   defMapFile.setSourceFiducialCoordFileName(fiducialSurfaceName);
   defMapFile.setSourceDirectory(QDir::currentPath());
   QDir::setCurrent(outputDirectoryName);
   defMapFile.setTargetDirectory(QDir::currentPath());
   if (QFileInfo(defTargetSpecName).isAbsolute()) {
      defMapFile.setTargetSpecFileName(defTargetSpecName);
   }
   else {
      defMapFile.setTargetSpecFileName(QDir::currentPath()
                                       + QDir::separator()
                                       + defTargetSpecName);
   }
   defMapFile.setOutputSpecFileName(defMapFile.getTargetSpecFileName());
   defMapFile.setDeformedFileNamePrefix(deformedPrefix);
   defMapFile.setDeformedColumnNamePrefix("");
   defMapFile.writeFile(defTargetDefMapName);
   QDir::setCurrent(originalPath);

   //
   // Create the deformed fiducial surface and load the fiducial surface
   // This allows border and foci projections to deform
   //
   SpecFile userSpecFile;
   userSpecFile.readFile(usersSpecFileName);
   if (userSpecFile.fiducialCoordFile.getNumberOfFiles() > 0) {
       userSpecFile.readFile(usersSpecFileName);
       QString deformErrorMessage;
       BrainModelSurfaceDeformDataFile::deformCoordinateFiles(&defMapFile,
                                                              userSpecFile.fiducialCoordFile,
                                                              deformErrorMessage);
       if (deformErrorMessage.isEmpty()) {
          QDir::setCurrent(outputDirectoryName);
          SpecFile tgtSpec;
          tgtSpec.readFile(defMapFile.getOutputSpecFileName());
          if (tgtSpec.fiducialCoordFile.getNumberOfFiles() > 0) {
              const QString fiducialCoordFileName =
                 tgtSpec.fiducialCoordFile.getFileName(0);
              brainSet.readCoordinateFile(fiducialCoordFileName,
                                          BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
                                          false,
                                          true,
                                          true);
              defMapFile.setTargetFiducialCoordFileName(fiducialCoordFileName);
          }
          QDir::setCurrent(originalPath);
       }
   }

   //
   // Remove the std sphere from the output spec file
   //
   QDir::setCurrent(outputDirectoryName);
   SpecFile outSpecFile;
   outSpecFile.readFile(defMapFile.getOutputSpecFileName());
   outSpecFile.sphericalCoordFile.clear(false);
   outSpecFile.writeFile(defMapFile.getOutputSpecFileName());
   QDir::setCurrent(originalPath);

   //
   // Deform the data files
   //
   QString errorMessage;
   BrainModelSurfaceDeformation::deformDataFiles(
           usersBrainSet,
           &brainSet,
           usersBrainSet->getSpecFileName(),
           &defMapFile,
           true,
           false, // already did fiducial
           true,
           true,
           true,
           false,
           errorMessage);

   if (errorMessage.isEmpty() == false) {
      throw CommandException(errorMessage);
   }
}

/**
 * create a spherical surface from a fiducial surface.
 */
BrainModelSurface*
CommandSpecFileChangeResolution::createSphereFromUsersFiducial(BrainSet* bs)
{
   BrainModelSurface* bms = bs->getBrainModelSurfaceOfType(
           BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (bms == NULL) {
      throw CommandException("Users spec file contains neither a fiducial "
                             "nor a spherical surface.");
   }

   //
   // Create a spherical surface
   //
   bms->createInflatedAndEllipsoidFromFiducial(false,
                                               false,
                                               false,
                                               true,
                                               false,
                                               false,
                                               true,
                                               0,
                                               NULL);

   bms = bs->getBrainModelSurfaceOfType(
           BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (bms == NULL) {
      throw CommandException("Failed to create spherical surface from users "
                             "fiducial surface.");
   }
   
   return bms;
}

/**
 * create the output directory.
 */
void
CommandSpecFileChangeResolution::createOutputDirectory(const QString& directoryName)
{
   if (QFile::exists(directoryName)) {
      throw CommandException("Ouput directory \""
                             + directoryName
                             + "\" exists.  The output directory must not "
                             + "and will be created by this program.");
   }

   QDir currentDir;
   if (currentDir.mkdir(directoryName) == false) {
      throw CommandException("Unable to create directory \""
                             + directoryName
                             + "\".");
   }
}

/**
 * read the user's spec file.
 */
BrainSet*
CommandSpecFileChangeResolution::readUsersSpecFile(const QString& usersSpecFileName)
{
   SpecFile sf;
   sf.readFile(usersSpecFileName);
   sf.setAllFileSelections(SpecFile::SPEC_TRUE);
   BrainSet* bs = new BrainSet();
   QString errorMessage;
   bs->readSpecFile(sf, usersSpecFileName, errorMessage);

   if (errorMessage.isEmpty() == false) {
      throw CommandException(errorMessage);
   }

   return bs;
}
      
