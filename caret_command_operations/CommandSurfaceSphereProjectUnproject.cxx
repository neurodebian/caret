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

#include "BrainSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfacePointProjector.h"
#include "CommandSurfaceSphereProjectUnproject.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
CommandSurfaceSphereProjectUnproject::CommandSurfaceSphereProjectUnproject()
   : CommandBase("-surface-sphere-project-unproject",
                 "SURFACE SPHERE PROJECT/UNPROJECT")
{
}

/**
 * destructor.
 */
CommandSurfaceSphereProjectUnproject::~CommandSurfaceSphereProjectUnproject()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceSphereProjectUnproject::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandSurfaceSphereProjectUnproject::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-spherical-coordinate-file-name>\n"
       + indent9 + "<input-spherical-topology-file-name>\n"
       + indent9 + "<output-spherical-coordinate-file-name>\n"
       + indent9 + "<project-to-spherical-coordinate-file-name>\n"
       + indent9 + "<unproject-to-spherical-coordinate-file-name>\n"
       + indent9 + "<project-unproject-spherical-topology-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Transform a spherical surface.\n"
       + indent9 + "\n"
       + indent9 + "The input spherical surface is projected to the \n"
       + indent9 + "\"project-to\" surface using barycentric projections\n"
       + indent9 + "and then unprojected using the \"unproject-to\" \n"
       + indent9 + "spherical surface.  Typically the \"unproject-to\"\n"
       + indent9 + "surface is created during surface-based registration\n"
       + indent9 + "of the \"project-to\" spherical surface to some\n"
       + indent9 + "target.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceSphereProjectUnproject::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Input Spherical Coordinate File Name");
   const QString inputSphericalTopologyFileName =
      parameters->getNextParameterAsString("Input Spherical Topology File Name");
   const QString outputSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Output Spherical Coordinate File Name");
   const QString projectToSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Project To Spherical Coordinate File Name");
   const QString unprojectToSphericalCoordinateFileName =
      parameters->getNextParameterAsString("Unproject To Spherical Coordinate File Name");
   const QString projectUnprojectSphericalTopologyFileName =
      parameters->getNextParameterAsString("Project/Unproject Spherical Topology File Name");

   checkForExcessiveParameters();

   // 
   // Create a brain set for user surface
   //
   BrainSet brainSet(inputSphericalTopologyFileName,
                     inputSphericalCoordinateFileName,
                     "",
                     true); 
   BrainModelSurface* inputSphericalSurface = brainSet.getBrainModelSurface(0);
   if (inputSphericalSurface == NULL) {
      throw CommandException("unable to find input spherical surface after reading files.");
   }
   const TopologyFile* inputSphericalTopologyFile = inputSphericalSurface->getTopologyFile();
   if (inputSphericalTopologyFile == NULL) {
      throw CommandException("unable to find input spherical topology after reading files.");
   }
   const int inputSphereNumNodes = inputSphericalSurface->getNumberOfNodes();
   if (inputSphereNumNodes == 0) {      
      throw CommandException("input spherical surface contains no nodes.");  
   }
   
   //
   // Create a brain set for project/unproject surfaces
   //
   BrainSet projectUnprojectBrainSet(projectUnprojectSphericalTopologyFileName,
                     projectToSphericalCoordinateFileName,
                     unprojectToSphericalCoordinateFileName,
                     true); 
   BrainModelSurface* projectSphericalSurface = projectUnprojectBrainSet.getBrainModelSurface(0);
   if (projectSphericalSurface == NULL) {
      throw CommandException("unable to find project to spherical surface after reading files.");
   }
   BrainModelSurface* unprojectSphericalSurface = projectUnprojectBrainSet.getBrainModelSurface(1);
   if (unprojectSphericalSurface == NULL) {
      throw CommandException("unable to find unproject to spherical surface after reading files.");
   }
   const CoordinateFile* unprojectCoordinateFile = unprojectSphericalSurface->getCoordinateFile();
   const TopologyFile* projectTopologyFile = projectSphericalSurface->getTopologyFile();
   if (projectTopologyFile == NULL) {
      throw CommandException("unable to find project/unproject spherical topology after reading files.");
   }
   const int projectSphereNumNodes = projectSphericalSurface->getNumberOfNodes();
   if (projectSphereNumNodes == 0) {      
      throw CommandException("input project to surface contains no nodes.");  
   }
   
   //
   // Topology Helper for input sphere
   //
   const TopologyHelper* inputSphereTopologyHelper = 
       inputSphericalTopologyFile->getTopologyHelper(false, true, false);
   CoordinateFile* inputOutputCoordFile = inputSphericalSurface->getCoordinateFile();
       
   //
   // Make sure both surfaces are centered and of the same radius and unproject surface
   //
   unprojectSphericalSurface->translateMidpointToOrigin();
   const float radius = unprojectSphericalSurface->getSphericalSurfaceRadius();
   projectSphericalSurface->translateMidpointToOrigin();
   projectSphericalSurface->convertToSphereWithRadius(radius);
   inputSphericalSurface->translateMidpointToOrigin();
   inputSphericalSurface->convertToSphereWithRadius(radius);
   
   //
   // Use a point projector to transform the surface nodes
   //
   BrainModelSurfacePointProjector pp(projectSphericalSurface,
                                      BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                                      false);
   for (int i = 0; i < inputSphereNumNodes; i++) {
      if (inputSphereTopologyHelper->getNodeHasNeighbors(i)) {
         float xyz[3];
         inputOutputCoordFile->getCoordinate(i, xyz);
         
         int nearestNode = -1;
         int baryNodes[3];
         float baryAreas[3];
         int triangle = pp.projectBarycentric(xyz,
                                              nearestNode,
                                              baryNodes,
                                              baryAreas,
                                              false);
         if (triangle >= 0) {
            BrainModelSurfacePointProjector::unprojectPoint(baryNodes,
                                                            baryAreas,
                                                            unprojectCoordinateFile,
                                                            xyz);
         }
         else if (nearestNode >= 0) {
            unprojectCoordinateFile->getCoordinate(nearestNode, xyz);
         }
         else {
            throw CommandException("Sphere Project/Unproject failed for node index: "
                                   + QString::number(i));
         }
         
         inputOutputCoordFile->setCoordinate(i, xyz);
      }
   }

   //
   // Make spherical again
   //
   inputSphericalSurface->translateMidpointToOrigin();
   inputSphericalSurface->convertToSphereWithRadius(radius);

   //
   // Write the output coordinate file
   //
   inputOutputCoordFile->writeFile(outputSphericalCoordinateFileName);
}

      

