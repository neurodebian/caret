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

#include <limits>

#include <QDir>

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderCutter.h"
#include "BrainModelSurfaceClusterToBorderConverter.h"
#include "BrainModelSurfaceDeformation.h"
#include "BrainModelSurfaceDeformDataFile.h"
#include "BrainModelSurfaceDeformationMapCreate.h"
#include "BrainModelSurfaceDeformationSphericalSlits.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceSphericalTessellator.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "PaintFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * constructor.
 */
BrainModelSurfaceDeformationSphericalSlits::BrainModelSurfaceDeformationSphericalSlits(
                           BrainSet* brainSetIn,
                           const BrainModelSurface* sphericalSurfaceIn,
                           const BorderProjectionFile* slitLandmarkBorderProjectionFileIn)
   : BrainModelAlgorithm(brainSetIn),
     inputSphericalSurface(sphericalSurfaceIn),
     inputSlitLandmarkBorderProjectionFile(slitLandmarkBorderProjectionFileIn)
{
   cutSphericalSurface = NULL;
   cutSphericalSurfaceTopologyFile     = NULL;
   smoothedClosedSphericalSurface = NULL;
   smoothedClosedSphericalSurfaceTopologyFile = NULL;
}

/**
 * destructor.
 */
BrainModelSurfaceDeformationSphericalSlits::~BrainModelSurfaceDeformationSphericalSlits()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceDeformationSphericalSlits::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (inputSphericalSurface == NULL) {
      throw BrainModelAlgorithmException("Input spherical surface is invalid.");
   }
   if (inputSlitLandmarkBorderProjectionFile == NULL) {
      throw BrainModelAlgorithmException("Input border projections are invalid.");
   }
   
   //
   // Find the border projections that are slits
   //
   BorderProjectionFile slitBorderProjections;
   for (int i = 0; 
        i < inputSlitLandmarkBorderProjectionFile->getNumberOfBorderProjections();
        i++) {
      const BorderProjection* bp = inputSlitLandmarkBorderProjectionFile->getBorderProjection(i);
      if (bp->getName().startsWith(getSlitLandmarkBorderNamePrefix())) {
         slitBorderProjections.addBorderProjection(*bp);
      }
   }
   if (slitBorderProjections.getNumberOfBorderProjections() <= 0) {
      throw BrainModelAlgorithmException("No borders with name prefix "
                                         + getSlitLandmarkBorderNamePrefix()
                                         + " were found.");
   }
   
   //
   // Surface/Topology that will be cut and add to brain set
   //
   TopologyFile* closedTopologyFile = inputSphericalSurface->getTopologyFile();
   cutSphericalSurface = new BrainModelSurface(*inputSphericalSurface);
   cutSphericalSurfaceTopologyFile = new TopologyFile(*closedTopologyFile);
   cutSphericalSurface->setTopologyFile(cutSphericalSurfaceTopologyFile);
   brainSet->addTopologyFile(cutSphericalSurfaceTopologyFile);
   brainSet->addBrainModel(cutSphericalSurface);
   
   //
   // Make the cuts
   //
   BrainModelSurfaceBorderCutter cutter(brainSet,
                                        cutSphericalSurface,
                                        &slitBorderProjections,
                                        BrainModelSurfaceBorderCutter::CUTTING_MODE_SPHERICAL_SURFACE,
                                        false);
   cutter.execute();
                                        
   //
   // Classify nodes
   //
   brainSet->classifyNodes(cutSphericalSurfaceTopologyFile, false);
   
   //
   // Create an ROI from the edge nodes
   //
   BrainModelSurfaceROINodeSelection roi(brainSet);
   roi.selectNodesThatAreEdges(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                               cutSphericalSurface);
   
   //
   // Convert the ROIs to borders
   //
   const QString borderAroundSlitName("SLIT.ENCLOSING.BORDER");
   BrainModelSurfaceClusterToBorderConverter bmscbc(brainSet,
                                                    cutSphericalSurface,
                                                    cutSphericalSurfaceTopologyFile,
                                                    borderAroundSlitName,
                                                    &roi,
                                                    true);
   bmscbc.execute();
   
   //
   // Get ALL of the borders on the sphere
   //
   BrainModelBorderSet* bmbs = brainSet->getBorderSet();
   BorderProjectionFile slitEnclosingBorderProjectionFile;
   bmbs->copyBordersToBorderProjectionFile(slitEnclosingBorderProjectionFile);

   //
   // Track names of borders that will be deleted later
   //                                           
   std::vector<QString> borderNamesToDelete;
   borderNamesToDelete.push_back(borderAroundSlitName);

   //
   // Loop through the slit borders that were input by the user
   //
   for (int i = 0; i < slitBorderProjections.getNumberOfBorderProjections(); i++) {
      //
      // Get XYZ of first link using INPUT CLOSED SPHERE
      //
      const BorderProjection* slitBP = slitBorderProjections.getBorderProjection(i);
      if (slitBP->getNumberOfLinks() > 1) {
         //
         // Get XYZ of first and last links
         //
         const BorderProjectionLink* slitStartBPL = slitBP->getBorderProjectionLink(0);
         float firstLinkXYZ[3];
         slitStartBPL->unprojectLink(inputSphericalSurface->getCoordinateFile(),
                                     firstLinkXYZ);
         const BorderProjectionLink* slitEndBPL = 
            slitBP->getBorderProjectionLink(slitBP->getNumberOfLinks() - 1);
         float lastLinkXYZ[3];
         slitEndBPL->unprojectLink(inputSphericalSurface->getCoordinateFile(),
                                   lastLinkXYZ);
                                
         //
         // Find enclosing slit that corresponds
         //
         int nearestEnclosingSlitBorderIndex = -1;
         int nearestEnclosingSlitBorderLinkNumber = -1;
         float nearestEnclosingSlitBorderDistance = std::numeric_limits<float>::max();
         for (int j = 0; 
              j < slitEnclosingBorderProjectionFile.getNumberOfBorderProjections();
              j++) {
            const BorderProjection* bp = 
               slitEnclosingBorderProjectionFile.getBorderProjection(j);
            if (bp->getName() == borderAroundSlitName) {
               const int linkNum = bp->getLinkNumberNearestToCoordinate(
                  cutSphericalSurface->getCoordinateFile(),
                  firstLinkXYZ);
               if (linkNum >= 0) {
                  const BorderProjectionLink* bpl = bp->getBorderProjectionLink(linkNum);
                  float linkXYZ[3];
                  bpl->unprojectLink(cutSphericalSurface->getCoordinateFile(),
                                     linkXYZ);
                  const float distSQ = 
                     MathUtilities::distanceSquared3D(firstLinkXYZ,
                                                      linkXYZ);
                  if (distSQ < nearestEnclosingSlitBorderDistance) {
                     nearestEnclosingSlitBorderDistance = distSQ;
                     nearestEnclosingSlitBorderIndex = j;
                     nearestEnclosingSlitBorderLinkNumber = linkNum;
                  }
               }
            }
         }
         
         //
         // Was the corresponding border that encloses the slit found?
         //
         if (nearestEnclosingSlitBorderIndex >= 0) {
            BorderProjection* bp = 
               slitEnclosingBorderProjectionFile.getBorderProjection(nearestEnclosingSlitBorderIndex);

            const int lastLinkNumber =
               bp->getLinkNumberNearestToCoordinate(
                  cutSphericalSurface->getCoordinateFile(),
                  lastLinkXYZ);
            //
            // Split it into two border projections
            //
            BorderProjection bp1, bp2;
            bp->splitClosedBorderProjection(cutSphericalSurface->getCoordinateFile(),
                                            nearestEnclosingSlitBorderLinkNumber,
                                            (slitBP->getName() + ".Patch"),
                                            bp1,
                                            bp2,
                                            lastLinkNumber);
            bp1.setName(slitBP->getName() + ".PatchA");
            bp2.setName(slitBP->getName() + ".PatchB");
            bp2.reverseOrderOfBorderProjectionLinks();
            BorderProjectionFile tempBP;
            tempBP.addBorderProjection(bp1);
            tempBP.addBorderProjection(bp2);
            bmbs->copyBordersFromBorderProjectionFile(&tempBP);
            bmbs->projectBorders(cutSphericalSurface);
            
            borderNamesToDelete.push_back(slitBP->getName());
            
            //
            // Update name of the enclosing slit border name
            //
            bp->setName(slitBP->getName() + ".Patch");
         }
      }
   }
   
   //
   // Remove the enclosing borders
   //
   bmbs->deleteBordersWithNames(borderNamesToDelete);
   
   //
   // Classify nodes
   //
   brainSet->classifyNodes(cutSphericalSurfaceTopologyFile, false);
   
   //
   // Create the smoothed spherical coordinate file that will receive closed topology
   //
   BrainModelSurface* smoothedSphericalSurface = new BrainModelSurface(*cutSphericalSurface);
   brainSet->addBrainModel(smoothedSphericalSurface);
   
   //
   // Smooth the sphere
   //
   smoothedSphericalSurface->arealSmoothing(1.0, 1000, 10, NULL, 10);
   smoothedSphericalSurface->arealSmoothing(1.0, 1000,  0, NULL, 10);
   
   //
   // Close the sphere by retessellating it
   //
   const int numNodes = smoothedSphericalSurface->getNumberOfNodes();
   const TopologyHelper* th = cutSphericalSurfaceTopologyFile->getTopologyHelper(false, true, false);
   std::vector<bool> useNodeInTessellationFlag(numNodes);
   for (int i = 0; i < numNodes; i++) {
      useNodeInTessellationFlag[i] = th->getNodeHasNeighbors(i);
   }
   BrainModelSurfaceSphericalTessellator bmsst(brainSet, 
                                               smoothedSphericalSurface,
                                               useNodeInTessellationFlag);
   bmsst.execute();
   
   //
   // Get newly tessellated closed spherical surface and topology file
   //
   smoothedClosedSphericalSurface = bmsst.getPointerToNewSphericalSurface();
   smoothedClosedSphericalSurfaceTopologyFile = smoothedClosedSphericalSurface->getTopologyFile();
   
   //
   // Remove the smoothed cut sphere from the brain set
   //
   brainSet->deleteBrainModel(smoothedSphericalSurface);
   smoothedSphericalSurface = NULL;
}
