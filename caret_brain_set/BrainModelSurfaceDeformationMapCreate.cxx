
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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformationMapCreate.h"
#include "BrainModelSurfacePointProjector.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceDeformationMapCreate::BrainModelSurfaceDeformationMapCreate(
                                      BrainSet* bs,
                                      const BrainModelSurface* sourceSurfaceIn,
                                      const BrainModelSurface* targetSurfaceIn,
                                      DeformationMapFile* deformationMapFileIn,
                                      const DEFORMATION_SURFACE_TYPE deformationSurfaceTypeIn)
   : BrainModelAlgorithm(bs),
     sourceSurfaceIn(sourceSurfaceIn),
     targetSurfaceIn(targetSurfaceIn)
{
   deformationMapFile = deformationMapFileIn;
   deformationSurfaceType = deformationSurfaceTypeIn;
}

/**
 * destructor.
 */
BrainModelSurfaceDeformationMapCreate::~BrainModelSurfaceDeformationMapCreate()
{
   if (this->sourceSurface != NULL) {
      delete this->sourceSurface;
      this->sourceSurface = NULL;
   }
   if (this->targetSurface != NULL) {
      delete this->targetSurface;
      this->targetSurface = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceDeformationMapCreate::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (sourceSurfaceIn == NULL) {
      throw BrainModelAlgorithmException("Source surface is invalid.");
   }
   if (targetSurfaceIn == NULL) {
      throw BrainModelAlgorithmException("Target surface is invalid.");
   }
   if (deformationMapFile == NULL) {
      throw BrainModelAlgorithmException("Deformation Map is invalid.");
   }
   
   if (sourceSurfaceIn->getNumberOfNodes() <= 0) {
      throw BrainModelAlgorithmException("Source surface contains no nodes.");
   }
   if (targetSurfaceIn->getNumberOfNodes() <= 0) {
      throw BrainModelAlgorithmException("Target surface contains no nodes.");
   }
   
   //
   // Verify there is topology
   //
   const TopologyFile* sourceTopologyFile = sourceSurfaceIn->getTopologyFile();
   if (sourceTopologyFile == NULL) {
      throw BrainModelAlgorithmException("Source surface contains no topology.");
   }
   const TopologyFile* targetTopologyFile = targetSurfaceIn->getTopologyFile();
   if (targetTopologyFile == NULL) {
      throw BrainModelAlgorithmException("Target surface contains no topology.");
   }
   
   //
   // Copy the source and target surfaces so that they can be modified
   //
   this->sourceSurface = new BrainModelSurface(*(this->sourceSurfaceIn));
   this->targetSurface = new BrainModelSurface(*(this->targetSurfaceIn));
   this->sourceSurface->getCoordinateFile()->setFileName(
      this->sourceSurfaceIn->getCoordinateFile()->getFileName());
   this->targetSurface->getCoordinateFile()->setFileName(
      this->targetSurfaceIn->getCoordinateFile()->getFileName());
   
   //
   // Clear the deformation map
   //
   deformationMapFile->clear();

   //
   // Create the deformation map
   //
   switch (deformationSurfaceType) {
      case DEFORMATION_SURFACE_TYPE_SPHERE:
         createSphericalDeformationMap();
         break;
   }
   
   //
   // File names for deformation map
   //
   const CoordinateFile* sourceCoord = sourceSurface->getCoordinateFile();
   QString sourceDirName = FileUtilities::dirname(sourceCoord->getFileName());
   if (sourceDirName == ".") {
      sourceDirName = QDir::currentPath();
   }
   deformationMapFile->setSourceDirectory(sourceDirName);
   const CoordinateFile* targetCoord = targetSurface->getCoordinateFile();
   QString targetDirName = FileUtilities::dirname(targetCoord->getFileName());
   if (targetDirName == ".") {
      targetDirName = QDir::currentPath();
   }
   deformationMapFile->setTargetDirectory(targetDirName);
   
   switch (deformationSurfaceType) {
      case DEFORMATION_SURFACE_TYPE_SPHERE:
         deformationMapFile->setSourceSphericalCoordFileName(
            FileUtilities::basename(sourceCoord->getFileName()));
         deformationMapFile->setSourceDeformedSphericalCoordFileName(
            FileUtilities::basename(sourceCoord->getFileName()));
         deformationMapFile->setSourceClosedTopoFileName(
            FileUtilities::basename(sourceTopologyFile->getFileName()));
         deformationMapFile->setTargetSphericalCoordFileName(
            FileUtilities::basename(targetCoord->getFileName()));
         deformationMapFile->setTargetClosedTopoFileName(
            FileUtilities::basename(targetTopologyFile->getFileName()));
         break;
   }
}

/**
 * create the spherical deformation map.
 */
void 
BrainModelSurfaceDeformationMapCreate::createSphericalDeformationMap()
{
   //
   // Make sure source surface is same radius as target sphere
   // and at origin
   //
   this->sourceSurface->translateToCenterOfMass();
   this->targetSurface->translateToCenterOfMass();
   this->sourceSurface->convertToSphereWithRadius(targetSurface->getSphericalSurfaceRadius());
      
   //
   // Get the coordinate files for the surfaces
   //
   const CoordinateFile* targetCoords = targetSurface->getCoordinateFile();
   const int numCoords = targetCoords->getNumberOfCoordinates();
       
   //
   // Create a Point Projector source surface.
   //
   BrainModelSurfacePointProjector bspp(sourceSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           false);
   
   //
   // Set number of nodes in deformation map file
   //
   deformationMapFile->setNumberOfNodes(numCoords);
   
   //
   // Project each point from target onto source 
   //
   for (int i = 0; i < numCoords; i++) {
      //
      // Get a target coordinate
      //
      float xyz[3];
      targetCoords->getCoordinate(i, xyz);
      
      //
      // Project target node onto source surface
      //
      int nearestNode = -1;
      int tileNodes[3];
      float tileAreas[3];
      const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                               tileNodes, tileAreas, true);
                                               
      //
      // Projected to tile?
      //
      if (tile >= 0) {
         deformationMapFile->setDeformDataForNode(i,
                                                  tileNodes,
                                                  tileAreas);
      }
      else if (nearestNode >= 0) {
         tileNodes[0] = nearestNode;
         tileNodes[1] = nearestNode;
         tileNodes[2] = nearestNode;
         tileAreas[0] = 0.33;
         tileAreas[1] = 0.33;
         tileAreas[2] = 0.33;
         deformationMapFile->setDeformDataForNode(i,
                                                  tileNodes,
                                                  tileAreas);
      }
   }
}
      

