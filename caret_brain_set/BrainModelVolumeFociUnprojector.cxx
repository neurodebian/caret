
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
#include "BrainModelVolumeFociUnprojector.h"
#include "FociProjectionFile.h"

/**
 * constructor.
 */
BrainModelVolumeFociUnprojector::BrainModelVolumeFociUnprojector(BrainSet* bsIn,
                                BrainModelSurface* leftSurfaceIn,
                                BrainModelSurface* rightSurfaceIn,
                                BrainModelSurface* cerebellumSurfaceIn,
                                FociProjectionFile* fociProjectionFileIn)
   : BrainModelAlgorithm(bsIn)
{
   leftSurface = leftSurfaceIn;
   rightSurface = rightSurfaceIn;
   cerebellumSurface = cerebellumSurfaceIn;
   fociProjectionFile = fociProjectionFileIn;
}
                                
/**
 * destructor.
 */
BrainModelVolumeFociUnprojector::~BrainModelVolumeFociUnprojector()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeFociUnprojector::execute() throw (BrainModelAlgorithmException)
{
   if ((leftSurface == NULL) &&
       (rightSurface == NULL)) {
      throw BrainModelAlgorithmException("Both surfaces are invalid.");
   }
   if (fociProjectionFile == NULL) {
      throw BrainModelAlgorithmException("The foci projection file is invalid.");
   }
   
   const int numFoci = fociProjectionFile->getNumberOfCellProjections();
   if (numFoci <= 0) {
      throw BrainModelAlgorithmException("The foci projection file contains no foci.");
   }
   
   //
   // Loop through the foci
   //
   for (int i = 0; i < numFoci; i++) {
      //
      // Get the focus
      //
      CellProjection* focus = fociProjectionFile->getCellProjection(i);
      
      //
      // Get the original position of the focus
      // 
      float xyz[3];
      focus->getXYZ(xyz);

      //
      // Determine which surface to use
      //
      bool leftFlag = false;
      bool rightFlag = false;
      bool cerebellumFlag = false;
      switch (focus->getCellStructure()) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
            leftFlag = true;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
            rightFlag = true;
            break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
            cerebellumFlag = true;
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
            break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
            leftFlag = true;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
            rightFlag = true;
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
         case Structure::STRUCTURE_TYPE_INVALID:
            if (xyz[0] > 0.0) {
               rightFlag = true;
            }
            else if (xyz[0] < 0.0) {
               leftFlag = true;
            }
            break;
      }
      
      //
      // get projected position using appropriate surface
      //
      if (leftFlag &&
          (leftSurface != NULL)) {
         float leftXYZ[3];
         if (focus->getProjectedPosition(leftSurface->getCoordinateFile(),
                                         leftSurface->getTopologyFile(),
                                         leftSurface->getIsFiducialSurface(),
                                         leftSurface->getIsFlatSurface(),
                                         false,
                                         leftXYZ)) {
            xyz[0] = leftXYZ[0];
            xyz[1] = leftXYZ[1];
            xyz[2] = leftXYZ[2];
         }
      }
      else if (rightFlag &&
               (rightSurface != NULL)) {
         float rightXYZ[3];
         if (focus->getProjectedPosition(rightSurface->getCoordinateFile(),
                                         rightSurface->getTopologyFile(),
                                         rightSurface->getIsFiducialSurface(),
                                         rightSurface->getIsFlatSurface(),
                                         false,
                                         rightXYZ)) {
            xyz[0] = rightXYZ[0];
            xyz[1] = rightXYZ[1];
            xyz[2] = rightXYZ[2];
         }
      }
      else if (cerebellumFlag &&
               (cerebellumSurface != NULL)) {
         if (focus->getProjectedPosition(cerebellumSurface->getCoordinateFile(),
                                         cerebellumSurface->getTopologyFile(),
                                         cerebellumSurface->getIsFiducialSurface(),
                                         cerebellumSurface->getIsFlatSurface(),
                                         false,
                                         xyz)) {
         }
      }
      
      //
      // Set the position of the focus in the volume
      //
      focus->setVolumeXYZ(xyz);
   }
}

