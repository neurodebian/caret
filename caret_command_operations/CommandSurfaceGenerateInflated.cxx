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
#include "BrainSet.h"
#include "CommandSurfaceGenerateInflated.h"
#include "CoordinateFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceGenerateInflated::CommandSurfaceGenerateInflated()
   : CommandBase("-surface-generate-inflated",
                 "SURFACE GENERATE INFLATED AND OTHER SURFACES")
{
}

/**
 * destructor.
 */
CommandSurfaceGenerateInflated::~CommandSurfaceGenerateInflated()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceGenerateInflated::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addVariableListOfParameters("Inflate Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceGenerateInflated::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-fiducial-coordinate-file-name> \n"
       + indent9 + "<input-closed-topology-file-name> \n"
       + indent9 + "[-compression-factor value] \n"
       + indent9 + "[-iterations-scale  value] \n"
       + indent9 + "[-generate-inflated] \n"
       + indent9 + "[-generate-very-inflated] \n"
       + indent9 + "[-generate-ellipsoid] \n"
       + indent9 + "[-generate-sphere] \n"
       + indent9 + "[-generate-cmw] \n"
       + indent9 + "[-output-spec    output-spec-file-name]\n"
       + indent9 + "[-output-inflated-file-name  output-inflated-coordinate-file-name]\n"
       + indent9 + "[-output-very-inflated-file-name  output-very-inflated-coordinate-file-name]\n"
       + indent9 + "[-output-ellipsoid-file-name output-ellipsoid-coordinate-file-name]\n"
       + indent9 + "[-output-sphere-file-name    output-spherical-coordinate-file-name]\n"
       + indent9 + "[-output-cmw-file-name       output-cmw-coordinate-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Generate inflated, very-inflated, ellipsoid, spherical,\n"
       + indent9 + "and compresed medial wall coordinate files.  The input \n"
       + indent9 + "fiducial surface should be free of topological defects.\n"
       + indent9 + "Run the command \"-surface-topology-report\" to examine\n"
       + indent9 + "a surface's topology.\n"
       + indent9 + "\n"
       + indent9 + "NOTE: At this time, generation of the compressed medial \n"
       + indent9 + "wall surface from the command line is not supported due\n"
       + indent9 + "to the algorithm's use of OpenGL for rotations.\n"
       + indent9 + "\n"
       + indent9 + "If the file name for a surface is not specified, the name\n"
       + indent9 + "of the output file will be automatically generated.\n"
       + indent9 + "\n"
       + indent9 + "Use the \"-iterations-scale\" to scale the iterations\n"
       + indent9 + "during the inflation processes.  In most cases, it is\n"
       + indent9 + "not necessary to use this option such as when the \n"
       + indent9 + "surface has been generated using Caret.  However, \n"
       + indent9 + "surfaces produced by FreeSurfer often contain a large\n"
       + indent9 + "number of nodes, 150,000 or more.  In this case, try\n"
       + indent9 + "an \"iterations-scale\" of 2.5.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceGenerateInflated::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString fiducialCoordinateFileName = 
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
   const QString closedTopologyFileName = 
      parameters->getNextParameterAsString("Closed Topology File Name");
      
   //
   // Process the parameters for output files
   //
   bool createInflatedFlag = false;
   bool createVeryInflatedFlag = false;
   bool createEllipsoidFlag = false;
   bool createSphereFlag = false; 
   bool createCompMedWallFlag = false;
   float iterationsScale = 1.0;  
   float compressionFactor = 0.95;
   QString outputSpecFileName;
   QString inflatedCoordinateFileName;
   QString veryInflateCoordinateFileName;
   QString ellipsoidCoordinateFileName;
   QString sphericalCoordinateFileName;
   QString compMedWallCoordinateFileName;
   while (parameters->getParametersAvailable()) {
      //
      // Get the next parameter and process it
      //
      const QString paramName = parameters->getNextParameterAsString("Next Surface Option");
      if (paramName == "-output-spec") {
         outputSpecFileName = 
            parameters->getNextParameterAsString("Output Spec File Name");
      }
      else if (paramName == "-compression-factor") {
          compressionFactor = parameters->getNextParameterAsFloat("Compression Factor");
      }
      else if (paramName == "-iterations-scale") {
         iterationsScale = parameters->getNextParameterAsFloat("Iterations Scale Value");
      }
      else if (paramName == "-generate-inflated") {
         createInflatedFlag = true;
      }
      else if (paramName == "-generate-very-inflated") {
         createVeryInflatedFlag = true;
      }
      else if (paramName == "-generate-ellipsoid") {
         createEllipsoidFlag = true;
      }
      else if (paramName == "-generate-sphere") {
         createSphereFlag = true;
      }
      else if (paramName == "-generate-cmw") {
         createCompMedWallFlag = true;
      }
      else if (paramName == "-output-inflated-file-name") {
         inflatedCoordinateFileName = 
            parameters->getNextParameterAsString("Inflated Coordinate File Name");
      }
      else if (paramName == "-output-very-inflated-file-name") {
         veryInflateCoordinateFileName = 
            parameters->getNextParameterAsString("Very Inflated Coordinate File Name");
      }
      else if (paramName == "-output-ellipsoid-file-name") {
         ellipsoidCoordinateFileName = 
            parameters->getNextParameterAsString("Ellipsoid Coordinate File Name");
      }
      else if (paramName == "-output-sphere-file-name") {
         sphericalCoordinateFileName = 
            parameters->getNextParameterAsString("Sphere Coordinate File Name");
      }
      else if (paramName == "-output-cmw-file-name") {
         compMedWallCoordinateFileName = 
            parameters->getNextParameterAsString("Comp Med Wall Coordinate File Name");
      }
      else {
         throw CommandException("Unrecognized surface option: "
                                + paramName);
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(closedTopologyFileName,
                     fiducialCoordinateFileName,
                     "",
                     true);
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurface(0);
   if (fiducialSurface == NULL) {
      throw CommandException("unable to find fiducial surface.");
   }
   const TopologyFile* tf = fiducialSurface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = fiducialSurface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Generate the surfaces
   //
   const bool smoothFingersFlag = false;
   const bool scaleToMatchFiduciaFlag = false;
   fiducialSurface->createInflatedAndEllipsoidFromFiducial(createInflatedFlag,
                                                           createVeryInflatedFlag,
                                                           createEllipsoidFlag,
                                                           createSphereFlag,
                                                           createCompMedWallFlag,
                                                           smoothFingersFlag,
                                                           scaleToMatchFiduciaFlag,
                                                           iterationsScale,
                                                           NULL,
                                                           compressionFactor);
   //
   // Write the output files
   //
   if (createInflatedFlag) {
      BrainModelSurface* surface = 
         brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_INFLATED);
      if (surface == NULL) {
         throw CommandException("inflated surface generation failed.");
      }
      writeCoordUpdateSpec(surface,
                           inflatedCoordinateFileName,
                           outputSpecFileName,
                           SpecFile::getInflatedCoordFileTag());
   }
   if (createVeryInflatedFlag) {
      BrainModelSurface* surface = 
         brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_VERY_INFLATED);
      if (surface == NULL) {
         throw CommandException("very inflated surface generation failed.");
      }
      writeCoordUpdateSpec(surface,
                           veryInflateCoordinateFileName,
                           outputSpecFileName,
                           SpecFile::getVeryInflatedCoordFileTag());
   }
   if (createEllipsoidFlag) {
      BrainModelSurface* surface = 
         brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL);
      if (surface == NULL) {
         throw CommandException("ellipsoid surface generation failed.");
      }
      writeCoordUpdateSpec(surface,
                           ellipsoidCoordinateFileName,
                           outputSpecFileName,
                           SpecFile::getEllipsoidCoordFileTag());
   }
   if (createSphereFlag) {
      BrainModelSurface* surface = 
         brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
      if (surface == NULL) {
         throw CommandException("spherical surface generation failed.");
      }
      writeCoordUpdateSpec(surface,
                           sphericalCoordinateFileName,
                           outputSpecFileName,
                           SpecFile::getSphericalCoordFileTag());
   }
   if (createCompMedWallFlag) {
      BrainModelSurface* surface = 
         brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL);
      if (surface == NULL) {
         throw CommandException("compressed medial wall surface generation failed.");
      }
      writeCoordUpdateSpec(surface,
                           compMedWallCoordinateFileName,
                           outputSpecFileName,
                           SpecFile::getCompressedCoordFileTag());
   }
}

/**
 * write the coordinate file and update the spec file.
 */
void 
CommandSurfaceGenerateInflated::writeCoordUpdateSpec(BrainModelSurface* surface,
                                      const QString& filenameIn,
                                      const QString& specFileName,
                                      const QString& specFileTag) throw (FileException)
{
   QString filename = filenameIn;
   CoordinateFile* cf = surface->getCoordinateFile();
   if (filename.isEmpty()) {
      filename = cf->getFileName();
   }
   cf->writeFile(filename);
   if (specFileName.isEmpty() == false) {
      SpecFile sf;
      sf.readFile(specFileName);
      sf.addToSpecFile(specFileTag,
                       filename,
                       "",
                       false);
      sf.writeFile(specFileName);
   }
}

      

