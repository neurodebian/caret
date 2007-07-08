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

#include "BrainModelStandardSurfaceReplacement.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainSet.h"
#include "DeformationMapFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelStandardSurfaceReplacement::BrainModelStandardSurfaceReplacement(
                                                 BrainSet* bs,
                                                 BrainModelSurface* sphericalSurfaceIn)
   : BrainModelAlgorithm(bs)
{
   sphericalSurface = sphericalSurfaceIn;
}

/**
 * Destructor.
 */
BrainModelStandardSurfaceReplacement::~BrainModelStandardSurfaceReplacement()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelStandardSurfaceReplacement::execute() throw (BrainModelAlgorithmException)
{
   //
   // Get the radius of the input sphere
   //
   const float sphereRadius = sphericalSurface->getSphericalSurfaceRadius();
   
   //
   // Read in the standard sphere
   //
   QString standardSphereSpecName(brainSet->getCaretHomeDirectory());
   standardSphereSpecName.append("/REGISTER.SPHERE/sphere.v5.6.spec");
   
   //
   // Read in the standard sphere spec file
   //
   BrainSet standardBrain(standardSphereSpecName, true, false);
   if (standardBrain.getNumberOfBrainModels() < 0) {
      QString msg("Unable to read standard sphere from spec: ");
      msg.append(standardSphereSpecName);
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Get the standard spherical surface
   //
   BrainModelSurface* standardSphere = standardBrain.getBrainModelSurface(0);
   if (standardSphere == NULL) {
      throw BrainModelAlgorithmException("Unable to find standard sphere after reading it");
   }
   
   //
   // Set the standard sphere to the radius of the input sphere
   //
   standardSphere->convertToSphereWithRadius(sphereRadius);
   
   //
   // Get the standard sphere coordinate file
   //
   const CoordinateFile* standardCoord = standardSphere->getCoordinateFile();
   const int numStandardNodes = standardSphere->getNumberOfNodes();
   
   //
   // Create a point projector for projecting standard sphere coords to input sphere
   //
   BrainModelSurfacePointProjector bmspp(sphericalSurface,
                       BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                       false);
                       
   //
   // Use a deformation map to store the projection information
   //
   DeformationMapFile defMapFile;
   defMapFile.setNumberOfNodes(numStandardNodes);
   
   //
   // Get the standard sphere's topology file
   //
   const TopologyFile* tf = standardSphere->getTopologyFile();
   
   //
   // Get a topology helper for connected nodes
   //
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Project the standard sphere to the input sphere
   //
   for (int i = 0; i < numStandardNodes; i++) {
      int tileNodes[3] = { -1, -1, -1 };
      float tileAreas[3];
      if (th->getNodeHasNeighbors(i)) {
         
         //
         // Project node from standard sphere
         //
         int nearestNode = -1;
         float xyz[3];
         standardCoord->getCoordinate(i, xyz);
         bmspp.projectBarycentric(xyz,
                                  nearestNode,
                                  tileNodes,
                                  tileAreas,
                                  true);
         if (tileNodes[0] < 0) {
            //
            // Move point slightly and try again
            //
            std::cout << "Trying agin for node : " << i << std::endl;
            const float delta = 0.001;
            xyz[0] += delta;
            xyz[1] += delta;
            xyz[2] += delta;
            bmspp.projectBarycentric(xyz,
                                     nearestNode,
                                     tileNodes,
                                     tileAreas,
                                     true);
            if (tileNodes[0] < 0) {
               std::cout << "Moving to nearest node" << std::endl;
               if (nearestNode >= 0) {
                  tileNodes[0] = nearestNode;
                  tileNodes[1] = nearestNode;
                  tileNodes[2] = nearestNode;
                  tileAreas[0] = 1.0;
                  tileAreas[1] = 0.0;
                  tileAreas[2] = 0.0;
               }
               else {
                  std::cout << "Node " << i << " did not project.  Nearest: "
                         << nearestNode << std::endl;
               }
            }
         }
      }
      //
      // set deform data for node
      //
      defMapFile.setDeformDataForNode(i, tileNodes, tileAreas);
   }
   
   //
   // Copy the standard sphere's topology file
   //
   TopologyFile* topologyFile = new TopologyFile(*tf);
   
   //
   // Keep track of new surfaces
   //
   std::vector<BrainModelSurface*> surfaces;
   
   //
   // Deform to each of input brain set's surfaces
   //
   for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      if (bms != NULL) {
         //
         // Create a new brain model surface, get coord file, set nodes
         //
         BrainModelSurface* newSurface = new BrainModelSurface(brainSet);
         newSurface->setSurfaceType(bms->getSurfaceType());
         CoordinateFile* newCoord = newSurface->getCoordinateFile();
         newCoord->setNumberOfCoordinates(numStandardNodes);
         
         //
         // Keep track of nodes that need to be smoothed
         //
         std::vector<bool> smoothNode(numStandardNodes, false);
         
         //
         // Unproject nodes to create new surface
         //
         const CoordinateFile* oldCoord = bms->getCoordinateFile();
         for (int j = 0; j < numStandardNodes; j++) {
            int nodes[3];
            float areas[3];
            defMapFile.getDeformDataForNode(j, nodes, areas);
            
            float xyz[3] = { 0.0, 0.0, 0.0 };
            if (nodes[0] >= 0) {
               BrainModelSurfacePointProjector::unprojectPoint(nodes, 
                                                               areas,
                                                               oldCoord,
                                                               xyz);
               if ((xyz[0] == 0.0) && (xyz[1] == 0.0) && (xyz[2] == 0.0)) {
                  smoothNode[j] = true;
               }
            }
            newCoord->setCoordinate(j, xyz);
         }
         
         //
         // Add topology
         //
         newSurface->setTopologyFile(topologyFile);
         
         //
         // Smooth connected nodes that are at origin
         //
         newSurface->arealSmoothing(1.0, 25, 0, &smoothNode);
         
         //
         // Smooth surface one iteration
         //
         int sphereIter = 0;
         if (newSurface->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
            sphereIter = 1;
         }
         newSurface->arealSmoothing(1.0, 1, sphereIter);
         
         //
         // Keep track of the surfaces
         //
         surfaces.push_back(newSurface);
      }
   }
   
   Structure structure = brainSet->getStructure();
   
   //
   // Reset the current brain set
   //
   brainSet->reset();
   brainSet->setStructure(structure);
   brainSet->addTopologyFile(topologyFile);
   for (unsigned int i = 0; i < surfaces.size(); i++) {
      surfaces[i]->computeNormals();
      surfaces[i]->setStructure(structure);
      brainSet->addBrainModel(surfaces[i], true);
   }
   
   //
   // Reset node coloring
   //
   BrainModelSurfaceNodeColoring* nodeColoring = brainSet->getNodeColoring();
   nodeColoring->setPrimaryOverlay(-1, BrainModelSurfaceNodeColoring::OVERLAY_NONE);
   nodeColoring->setSecondaryOverlay(-1, BrainModelSurfaceNodeColoring::OVERLAY_NONE);
   nodeColoring->setUnderlay(-1, BrainModelSurfaceNodeColoring::UNDERLAY_NONE);
   
   brainSet->postSpecFileReadInitializations();
}      
