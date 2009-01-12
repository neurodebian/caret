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

#include <algorithm>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceCellAttributeAssignment.h"
#include "BrainModelSurfacePointLocator.h"
#include "CellProjectionFile.h"
#include "MathUtilities.h"
#include "PaintFile.h"

/**
 * constructor.
 */
BrainModelSurfaceCellAttributeAssignment::BrainModelSurfaceCellAttributeAssignment(
                                BrainSet* brainSetIn,
                                const BrainModelSurface* leftSurfaceIn,
                                const BrainModelSurface* rightSurfaceIn,
                                const BrainModelSurface* cerebellumSurfaceIn,
                                CellProjectionFile* cellProjectionFileIn,
                                const PaintFile* paintFileIn,
                                const std::vector<bool>& paintColumnsSelectedIn,
                                const float maximumDistanceFromSurfaceIn,
                                const ASSIGN_ATTRIBUTE assignAttributeIn,
                                const ASSIGNMENT_METHOD assignmentMethodIn,
                                const QString attributeIDIn,
                                const bool optionIgnoreUnknownValuesFlagIn)
   : BrainModelAlgorithm(brainSetIn),
     leftSurface(leftSurfaceIn),
     rightSurface(rightSurfaceIn),
     cerebellumSurface(cerebellumSurfaceIn),
     cellProjectionFile(cellProjectionFileIn),
     paintFile(paintFileIn),
     paintColumnsSelected(paintColumnsSelectedIn),
     maximumDistanceFromSurface(maximumDistanceFromSurfaceIn),
     assignAttribute(assignAttributeIn),
     assignmentMethod(assignmentMethodIn),
     attributeID(attributeIDIn),
     optionIgnoreUnknownValuesFlag(optionIgnoreUnknownValuesFlagIn)
{
   leftPointLocator = NULL;
   rightPointLocator = NULL;
   cerebellumPointLocator = NULL;
}

/**
 * destructor.
 */
BrainModelSurfaceCellAttributeAssignment::~BrainModelSurfaceCellAttributeAssignment()
{
   if (leftPointLocator != NULL) {
      delete leftPointLocator;
      leftPointLocator = NULL;
   }
   if (rightPointLocator != NULL) {
      delete rightPointLocator;
      rightPointLocator = NULL;
   }
   if (cerebellumPointLocator != NULL) {
      delete cerebellumPointLocator;
      cerebellumPointLocator = NULL;
   }
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceCellAttributeAssignment::execute() throw (BrainModelAlgorithmException)
{
   if ((leftSurface == NULL) &&
       (rightSurface == NULL)  &&
       (cerebellumSurface == NULL)) {
      throw BrainModelAlgorithmException("Attribute Assignment: All surfaces are invalid.");
   }
   
   if (cellProjectionFile == NULL) {
      throw BrainModelAlgorithmException("Attribute Assignment: Cell/Foci file is invalid.");
   }
   
   const int numCells = cellProjectionFile->getNumberOfCellProjections();
   if (numCells <= 0) {
      throw BrainModelAlgorithmException("Attribute Assignment: There are no cells/foci.");
   }            

   //
   // See if paints are selected
   //
   int numPaintCols = 0;
   if (paintFile != NULL) {
      numPaintCols = paintFile->getNumberOfColumns();
   }
   
   if (assignmentMethod != ASSIGNMENT_METHOD_CLEAR) {
      if ((paintFile == NULL) || (numPaintCols <= 0)) {
         throw BrainModelAlgorithmException("There are no paint columns.");
      }
      else if (std::count(paintColumnsSelected.begin(),
                         paintColumnsSelected.end(),
                         true) <= 0) {
         throw BrainModelAlgorithmException("There are no paint columns selected.");
      }
   }
   //
   // Determine node nearest to each cell
   //
   std::vector<float> cellsNearestLeftNodeDistance(numCells, -1.0);
   std::vector<float> cellsNearestRightNodeDistance(numCells, -1.0);
   std::vector<float> cellsNearestCerebellumNodeDistance(numCells, -1.0);
   std::vector<int> cellsNearestLeftNode(numCells, -1);
   std::vector<int> cellsNearestRightNode(numCells, -1);
   std::vector<int> cellsNearestCerebellumNode(numCells, -1);
   if (leftSurface != NULL) {
      leftPointLocator = new BrainModelSurfacePointLocator(leftSurface, true);
   }
   if (rightSurface != NULL) {
      rightPointLocator = new BrainModelSurfacePointLocator(rightSurface, true);
   }
   if (cerebellumSurface != NULL) {
      cerebellumPointLocator = new BrainModelSurfacePointLocator(cerebellumSurface, true);
   }
   
   for (int i = 0; i < numCells; i++) {
      CellProjection* cp = cellProjectionFile->getCellProjection(i);
      
      float xyz[3];
      cp->getXYZ(xyz);
      
      switch (cp->getCellStructure()) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:         
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
            if (leftSurface != NULL) {
               if (cp->getProjectedPosition(leftSurface->getCoordinateFile(),
                                            leftSurface->getTopologyFile(),
                                            leftSurface->getIsFiducialSurface(),
                                            leftSurface->getIsFlatSurface(),
                                            false,
                                            xyz)) {
                  cellsNearestLeftNode[i] = leftPointLocator->getNearestPoint(xyz);
                  cellsNearestLeftNodeDistance[i] = 
                     MathUtilities::distance3D(xyz, 
                         leftSurface->getCoordinateFile()->getCoordinate(cellsNearestLeftNode[i]));
               }
            }
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
            if (rightSurface != NULL) {
               if (cp->getProjectedPosition(rightSurface->getCoordinateFile(),
                                            rightSurface->getTopologyFile(),
                                            rightSurface->getIsFiducialSurface(),
                                            rightSurface->getIsFlatSurface(),
                                            false,
                                            xyz)) {
                  cellsNearestRightNode[i] = rightPointLocator->getNearestPoint(xyz);
                  cellsNearestRightNodeDistance[i] = 
                     MathUtilities::distance3D(xyz, 
                         rightSurface->getCoordinateFile()->getCoordinate(cellsNearestRightNode[i]));
               }
            }
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:       
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
            if (cerebellumSurface != NULL) {
               if (cp->getProjectedPosition(cerebellumSurface->getCoordinateFile(),
                                            cerebellumSurface->getTopologyFile(),
                                            cerebellumSurface->getIsFiducialSurface(),
                                            cerebellumSurface->getIsFlatSurface(),
                                            false,
                                            xyz)) {
                  cellsNearestCerebellumNode[i] = cerebellumPointLocator->getNearestPoint(xyz);
                  cellsNearestCerebellumNodeDistance[i] = 
                     MathUtilities::distance3D(xyz, 
                         cerebellumSurface->getCoordinateFile()->getCoordinate(cellsNearestCerebellumNode[i]));
               }
            }
            break;
         case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_SUBCORTICAL:
         case Structure::STRUCTURE_TYPE_ALL:
         case Structure::STRUCTURE_TYPE_INVALID:
            break;
      }
   }
   
   //
   // Process the cells
   //
   for (int i = 0; i < numCells; i++) {
      //
      // Load cell data into the table
      //
      CellProjection* cd = cellProjectionFile->getCellProjection(i);
      
      //
      // Load paint into the table
      //
      int node = -1;
      float distance = 100000000.0;
      if (cellsNearestLeftNode[i] >= 0) {
         node = cellsNearestLeftNode[i];
         distance = cellsNearestLeftNodeDistance[i];
      }
      else if (cellsNearestRightNode[i] >= 0) {
         node = cellsNearestRightNode[i];
         distance = cellsNearestRightNodeDistance[i];
      }
      else if (cellsNearestCerebellumNode[i] >= 0) {
         node = cellsNearestCerebellumNode[i];
         distance = cellsNearestCerebellumNodeDistance[i];
      }
      if ((node >= 0) &&
          (distance <= maximumDistanceFromSurface)) {
         QString valueString;
         switch (assignAttribute) {
            case ASSIGN_ATTRIBUTE_AREA:
               valueString = cd->getArea();
               break;
            case ASSIGN_ATTRIBUTE_GEOGRAPHY:
               valueString = cd->getGeography();
               break;
            case ASSIGN_ATTRIBUTE_REGION_OF_INTEREST:
               valueString = cd->getRegionOfInterest();
               break;
         }
      
         bool doAssignmentFlag = false;
         switch (assignmentMethod) {
            case ASSIGNMENT_METHOD_APPEND:
               doAssignmentFlag = true;
               break; 
            case ASSIGNMENT_METHOD_CLEAR:
               valueString = "";
               break; 
            case ASSIGNMENT_METHOD_REPLACE:
               doAssignmentFlag = true;
               valueString = "";
               break; 
         }
      
         if (doAssignmentFlag) {
            for (int j = 0; j < numPaintCols; j++) {
               if (paintColumnsSelected[j]) {
                  const int paintIndex = paintFile->getPaint(node, j);
                  QString paintName = paintFile->getPaintNameFromIndex(paintIndex);
                  if (valueString.isEmpty() == false) {
                     valueString += "; ";
                  }
                  
                  if (optionIgnoreUnknownValuesFlag) {
                     if (paintName.startsWith("?")) {
                        paintName = " ";
                     }
                  }
                  
                  valueString += paintName;
               }
            }
         }

         switch (assignAttribute) {
            case ASSIGN_ATTRIBUTE_AREA:
               cd->setArea(valueString);
               break;
            case ASSIGN_ATTRIBUTE_GEOGRAPHY:
               cd->setGeography(valueString);
               break;
            case ASSIGN_ATTRIBUTE_REGION_OF_INTEREST:
               cd->setRegionOfInterest(valueString);
               break;
         }
         
         cd->setAttributeID(attributeID);
         
      } // surface found
   } // for (i = 0; i < numCells...
}
      
/**
 * get attribute names and values.
 */
void 
BrainModelSurfaceCellAttributeAssignment::getAttributeNamesAndValues(
                                       std::vector<QString>& namesOut,
                                       std::vector<ASSIGN_ATTRIBUTE>& valuesOut)
{
   namesOut.clear();
   valuesOut.clear();
   
   namesOut.push_back("Area");  
      valuesOut.push_back(ASSIGN_ATTRIBUTE_AREA);
   namesOut.push_back("Geography");  
      valuesOut.push_back(ASSIGN_ATTRIBUTE_GEOGRAPHY);
   namesOut.push_back("RegionOfInterest");  
      valuesOut.push_back(ASSIGN_ATTRIBUTE_REGION_OF_INTEREST);
}
                                 
/**
 * get assignment names and values.
 */
void 
BrainModelSurfaceCellAttributeAssignment::getAssignmentNamesAndValues(
                                    std::vector<QString>& namesOut,
                                    std::vector<ASSIGNMENT_METHOD>& valuesOut)
{
   namesOut.clear();
   valuesOut.clear();
   
   namesOut.push_back("Append");  
      valuesOut.push_back(ASSIGNMENT_METHOD_APPEND);
   namesOut.push_back("Clear");  
      valuesOut.push_back(ASSIGNMENT_METHOD_CLEAR);
   namesOut.push_back("Replace");  
      valuesOut.push_back(ASSIGNMENT_METHOD_REPLACE);
}
                                    

