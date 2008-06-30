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

#include <cmath>

#include <QProgressDialog>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceFociUncertaintyToRgbPaint.h"
#include "BrainSet.h"
#include "FociProjectionFile.h"
#include "FociColorFile.h"
#include "MathUtilities.h"
#include "PreferencesFile.h"
#include "RgbPaintFile.h"
#include "vtkMath.h"

/**
 * Constructor.
 */
BrainModelSurfaceFociUncertaintyToRgbPaint::BrainModelSurfaceFociUncertaintyToRgbPaint(
                               BrainSet* bsIn,
                               const BrainModelSurface* leftSurfaceIn,
                               const BrainModelSurface* rightSurfaceIn,
                               RgbPaintFile* rgbPaintFileIn,
                               const int leftRgbPaintFileColumnIn,
                               const QString& leftRgbPaintFileColumnNameIn,
                               const int rightRgbPaintFileColumnIn,
                               const QString& rightRgbPaintFileColumnNameIn,
                               const FociProjectionFile* fociProjectionFileIn,
                               const FociColorFile* fociColorFileIn,
                               const float lowerLimitIn, 
                               const float middleLimitIn, 
                               const float upperLimitIn)
  : BrainModelAlgorithm(bsIn),
   leftSurface(leftSurfaceIn),
   rightSurface(rightSurfaceIn),
   rgbPaintFile(rgbPaintFileIn),
   leftRgbPaintFileColumn(leftRgbPaintFileColumnIn),
   leftRgbPaintFileColumnName(leftRgbPaintFileColumnNameIn),
   rightRgbPaintFileColumn(rightRgbPaintFileColumnIn),
   rightRgbPaintFileColumnName(rightRgbPaintFileColumnNameIn),
   fociProjectionFile(fociProjectionFileIn),
   fociColorFile(fociColorFileIn),
   lowerLimit(lowerLimitIn),
   middleLimit(middleLimitIn),
   upperLimit(upperLimitIn)
{
}

/**
 * Destructor.
 */
BrainModelSurfaceFociUncertaintyToRgbPaint::~BrainModelSurfaceFociUncertaintyToRgbPaint()
{
}

/**
 * execute the algorithm.
 */
void
BrainModelSurfaceFociUncertaintyToRgbPaint::execute() throw (BrainModelAlgorithmException)
{
   if ((leftSurface == NULL) &&
       (rightSurface == NULL)) {
      throw BrainModelAlgorithmException("Both surfaces are invalid.");
   }
   if (rgbPaintFile == NULL) {
      throw BrainModelAlgorithmException("The RGB Paint File is invalid.");
   }
   if (fociProjectionFile == NULL) {
      throw BrainModelAlgorithmException("The Foci Projection File is invalid.");
   }
   if (fociColorFile == NULL) {
      throw BrainModelAlgorithmException("The Foci Color File is invalid.");
   }
   if (fociProjectionFile->getNumberOfCellProjections() <= 0) {
      throw BrainModelAlgorithmException("The Foci Projection File contains no foci.");
   }
   if (fociProjectionFile->getNumberOfCellClasses() <= 0) {
      throw BrainModelAlgorithmException("There are no foci classes.");
   }
   
   int numNodes = 0;
   if (leftSurface != NULL) {
      numNodes += leftSurface->getNumberOfNodes();
   }
   if (rightSurface != NULL) {
      numNodes += rightSurface->getNumberOfNodes();
   }
   
   createProgressDialog("Determining Foci Uncertainty",
                        numNodes,
                        "fociUncertainty");
                        
   int progressSteps = 0;
  
   try { 
      if (leftSurface != NULL) {
         generateLimits(leftSurface,
                        leftRgbPaintFileColumn,
                        leftRgbPaintFileColumnName,
                        progressSteps);
      }
      
      if (rightSurface != NULL) {
         generateLimits(rightSurface,
                        rightRgbPaintFileColumn,
                        rightRgbPaintFileColumnName,
                        progressSteps);
      }
   }
   catch (BrainModelAlgorithmException& e) {
      removeProgressDialog();
      throw e;
   }
   
   removeProgressDialog();
}


/**
 * Convert foci uncertainty to rgb paint (returns true if successful).
 */
void
BrainModelSurfaceFociUncertaintyToRgbPaint::generateLimits(const BrainModelSurface* bms,
                                                    const int rgbPaintColumnIn, 
                                                    const QString& rgbPaintColumnNewName,
                                                    int& progressSteps)
                                          throw (BrainModelAlgorithmException)
{
   //
   const float upperLimitSquared = upperLimit*upperLimit;
   
   //
   // Get number of foci classes
   //
   const int numClasses = fociProjectionFile->getNumberOfCellClasses();
   const int numFoci = fociProjectionFile->getNumberOfCellProjections();
   const TopologyFile* tf = bms->getTopologyFile();
   const CoordinateFile* cf = bms->getCoordinateFile();
   const int numCoords = cf->getNumberOfCoordinates();
   if (numCoords <= 0) {
      throw BrainModelAlgorithmException("The surface has no coordinates.");
   }
   if (tf == NULL) {
      throw BrainModelAlgorithmException("The surface has no topology.");
   }
   if (tf->getNumberOfTiles() <= 0) {
      throw BrainModelAlgorithmException("The surface's topology contains no triangles.");
   }
   
   //
   // Storage for number of nodes in each class
   //
   std::vector<int> nodeCount(numClasses, 0);
   
   //
   // Initialize color to foreground color
   //
   unsigned char foregroundColor[3];
   PreferencesFile* pref = brainSet->getPreferencesFile();
   pref->getSurfaceForegroundColor(foregroundColor[0], foregroundColor[1], foregroundColor[2]);
   
   //
   // Add a column to the rgb paint file
   //
   int rgbPaintColumn = rgbPaintColumnIn;
   if (rgbPaintFile->getNumberOfNodes() == 0) {
      rgbPaintFile->setNumberOfNodesAndColumns(bms->getNumberOfNodes(), 1);
      rgbPaintColumn = 0;
   }
   else if ((rgbPaintColumn < 0) ||
            (rgbPaintColumn >= rgbPaintFile->getNumberOfColumns())) {
      rgbPaintColumn = rgbPaintFile->getNumberOfColumns();
      rgbPaintFile->addColumns(1);
   }
   rgbPaintFile->setColumnName(rgbPaintColumn, rgbPaintColumnNewName);
   rgbPaintFile->setScaleRed(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setScaleGreen(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setScaleBlue(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setColumnComment(rgbPaintColumn, 
                                  "Created from foci uncertainty");
   
   const bool fiducialSurfaceFlag = bms->getIsFiducialSurface();
   const bool flatSurfaceFlag = bms->getIsFlatSurface();
   const Structure::STRUCTURE_TYPE surfaceStructureType = 
                                      bms->getStructure().getType();
      
   //
   // Loop through fiducial coordinates
   //
   for (int i = 0; i < numCoords; i++) {
      //
      // Update progress
      //
      if (progressDialog != NULL) {
         const float pctComplete = (static_cast<float>(progressSteps) 
                                    / progressDialog->maximum()) * 100.0;
         const QString pctString(QString::number(pctComplete, 'f', 1)
                                 + "% complete.");
         updateProgressDialog(pctString, progressSteps);
      }
      else {
         updateProgressDialog("Determining Uncertainty", progressSteps);
      }
      progressSteps++;
      
      //
      // Keep track of nodes in the various classes nearest to this node.
      //
      std::vector<int> focusNearestNode(numClasses, -1);
      std::vector<float> focusDistanceToNode(numClasses, 
                                             std::numeric_limits<float>::max());
      
      //
      // Get the position of the node
      //
      const float* nodePos = cf->getCoordinate(i);
      
         //
         // Loop through foci
         //
         for (int k = 0; k < numFoci; k++) {
            //
            // Get the foci's and see if it is displayed
            //
            const CellProjection* focus = fociProjectionFile->getCellProjection(k);
            if (focus->getDisplayFlag()) {
               //
               //  Get the foci's class and see if it matches the current foci class
               //
               const int classNumber = focus->getClassIndex();
               if (classNumber >= 0) {
                  //
                  // Get the foci's position and adjust for hemisphere
                  //
                  bool useFocus = false;
                  float fociPos[3];
                  if (focus->getProjectedPosition(cf,
                                                   tf,
                                                   fiducialSurfaceFlag,
                                                   flatSurfaceFlag,
                                                   false,
                                                   fociPos)) {
                     if (focus->getCellStructure() == surfaceStructureType) {
                        useFocus = true;
                     }
                  }

                  if (useFocus) {
                     //
                     // See how close this focus is to coordinate
                     //
                     const float distSQ = MathUtilities::distanceSquared3D(fociPos,
                                                                           nodePos);
                     if ((distSQ < upperLimitSquared) &&
                         (distSQ < focusDistanceToNode[classNumber])) {
                        focusNearestNode[classNumber] = k;
                        focusDistanceToNode[classNumber] = distSQ;
                     }
                  }
               }
            }
         }
         
      for (int m = 0; m < numClasses; m++) {
         focusDistanceToNode[m] = std::sqrt(focusDistanceToNode[m]);
      }
      
      //
      // Node coloring
      //
      float nodeColor[3] = { -1.0, -1.0, -1.0 };
      std::vector<int> nearbyFoci;
      std::vector<float> nearbyFociDistances;
      
      //
      // Loop through foci classes
      //
      for (int j = 0; j < numClasses; j++) {
         if (focusNearestNode[j] >= 0) {
            if ((focusDistanceToNode[j] >= lowerLimit) &&
                (focusDistanceToNode[j] <= upperLimit)) {
               nearbyFoci.push_back(focusNearestNode[j]);
               nearbyFociDistances.push_back(focusDistanceToNode[j]);
            }
         }
      }
      
      const int nearbyFociCount = static_cast<int>(nearbyFoci.size());
      
      if (nearbyFociCount > 2) {
         //
         // Paint node gray
         //
         nodeColor[0] = 50.0;
         nodeColor[1] = 50.0;
         nodeColor[2] = 50.0;
      }
      else if (nearbyFociCount == 2) {
         //
         // Get color of foci nearest the node
         //
         int colorIndex = fociProjectionFile->getCellProjection(nearbyFoci[0])->getColorIndex();
         unsigned char r, g, b;
         if (colorIndex >= 0) {
            fociColorFile->getColorByIndex(colorIndex, r, g, b);
         }
         else {
            //
            // Use foreground color
            //
            r = foregroundColor[0];
            g = foregroundColor[1];
            b = foregroundColor[2];
         }
         const float r1 = r;
         const float g1 = g;
         const float b1 = b;
         
         //
         // Get color of foci second nearest the node
         //
         colorIndex = fociProjectionFile->getCellProjection(nearbyFoci[1])->getColorIndex();
         if (colorIndex >= 0) {
            fociColorFile->getColorByIndex(colorIndex, r, g, b);
         }
         else {
            //
            // Use foreground color
            //
            r = foregroundColor[0];
            g = foregroundColor[1];
            b = foregroundColor[2];
         }
         const float r2 = r;
         const float g2 = g;
         const float b2 = b;
         
         const float m = std::max(r1+r2, std::max(g1+g2, b1+b2));
         nodeColor[0] = 255.0 * (r1 + r2) / m;
         nodeColor[1] = 255.0 * (g1 + g2) / m;
         nodeColor[2] = 255.0 * (b1 + b2) / m;
         
      }
      else if (nearbyFociCount == 1) {
         //
         // Use the foci's color
         //
         const int colorIndex = fociProjectionFile->getCellProjection(nearbyFoci[0])->getColorIndex();
         if (colorIndex >= 0) {
            unsigned char r, g, b;
            fociColorFile->getColorByIndex(colorIndex, r, g, b);
            nodeColor[0] = r;
            nodeColor[1] = g;
            nodeColor[2] = b;
         }
         else {
            //
            // Use foreground color
            //
            nodeColor[0] = foregroundColor[0];
            nodeColor[1] = foregroundColor[1];
            nodeColor[2] = foregroundColor[2];
         }
         
         //
         // If within middle limit, whiten the node to produce a halo effect
         //
         if (nearbyFociDistances[0] < middleLimit) {
            nodeColor[0] = 127.5 + 0.5 * nodeColor[0];
            nodeColor[1] = 127.5 + 0.5 * nodeColor[1];
            nodeColor[2] = 127.5 + 0.5 * nodeColor[2];
         }
      }
      
      //
      // Add to rgb paint file
      //
      const float max255 = 255.0;
      nodeColor[0] = std::min(nodeColor[0], max255);
      nodeColor[1] = std::min(nodeColor[1], max255);
      nodeColor[2] = std::min(nodeColor[2], max255);      
      rgbPaintFile->setRgb(i, rgbPaintColumn, nodeColor[0], nodeColor[1], nodeColor[2]);
   }
}

/**
 * Convert foci uncertainty to rgb paint (returns true if successful).
 */
void
BrainModelSurfaceFociUncertaintyToRgbPaint::generateLimitsOLD(const BrainModelSurface* bms,
                                                    const int rgbPaintColumnIn, 
                                                    const QString& rgbPaintColumnNewName)
                                          throw (BrainModelAlgorithmException)
{
   //
   const float upperLimitSquared = upperLimit*upperLimit;
   
   //
   // Get number of foci classes
   //
   const int numClasses = fociProjectionFile->getNumberOfCellClasses();
   const int numFoci = fociProjectionFile->getNumberOfCellProjections();
   const TopologyFile* tf = bms->getTopologyFile();
   const CoordinateFile* cf = bms->getCoordinateFile();
   const int numCoords = cf->getNumberOfCoordinates();
   if (numCoords <= 0) {
      throw BrainModelAlgorithmException("There surface has no coordinates.");
   }
   if (tf == NULL) {
      throw BrainModelAlgorithmException("There surface has no topology.");
   }
   if (tf->getNumberOfTiles() <= 0) {
      throw BrainModelAlgorithmException("There surface's topology contains no triangles.");
   }
   
   //
   // Storage for number of nodes in each class
   //
   std::vector<int> nodeCount(numClasses, 0);
   
   //
   // Initialize color to foreground color
   //
   unsigned char foregroundColor[3];
   PreferencesFile* pref = brainSet->getPreferencesFile();
   pref->getSurfaceForegroundColor(foregroundColor[0], foregroundColor[1], foregroundColor[2]);
   
   //
   // Add a column to the rgb paint file
   //
   int rgbPaintColumn = rgbPaintColumnIn;
   if (rgbPaintFile->getNumberOfNodes() == 0) {
      rgbPaintFile->setNumberOfNodesAndColumns(bms->getNumberOfNodes(), 1);
      rgbPaintColumn = 0;
   }
   else if ((rgbPaintColumn < 0) ||
            (rgbPaintColumn >= rgbPaintFile->getNumberOfColumns())) {
      rgbPaintColumn = rgbPaintFile->getNumberOfColumns();
      rgbPaintFile->addColumns(1);
   }
   rgbPaintFile->setColumnName(rgbPaintColumn, rgbPaintColumnNewName);
   rgbPaintFile->setScaleRed(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setScaleGreen(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setScaleBlue(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setColumnComment(rgbPaintColumn, 
                                  "Created from foci uncertainty");
   
   const bool fiducialSurfaceFlag = bms->getIsFiducialSurface();
   const bool flatSurfaceFlag = bms->getIsFlatSurface();
   const Structure::STRUCTURE_TYPE surfaceStructureType = 
                                      bms->getStructure().getType();
      
   //
   // Loop through fiducial coordinates
   //
   for (int i = 0; i < numCoords; i++) {
      //
      // Keep track of nodes in the various classes nearest to this node.
      //
      std::vector<int> focusNearestNode(numClasses, -1);
      std::vector<float> focusDistanceToNode(numClasses, 0.0);
      
      //
      // Get the position of the node
      //
      const float* nodePos = cf->getCoordinate(i);
      
      //
      // Loop through foci classes
      //
      for (int fociClass = 0; fociClass < numClasses; fociClass++) {
         int closestFocus = -1;
         float closestFocusDistance = std::numeric_limits<float>::max();
         
         //
         // Loop through foci
         //
         for (int k = 0; k < numFoci; k++) {
            //
            // Get the foci's and see if it is displayed
            //
            const CellProjection* focus = fociProjectionFile->getCellProjection(k);
            if (focus->getDisplayFlag()) {
               //
               //  Get the foci's class and see if it matches the current foci class
               //
               const int classNumber = focus->getClassIndex();
               if (classNumber == fociClass) {
                  //
                  // Get the foci's position and adjust for hemisphere
                  //
                  bool useFocus = false;
                  float fociPos[3];
                  if (focus->getProjectedPosition(cf,
                                                   tf,
                                                   fiducialSurfaceFlag,
                                                   flatSurfaceFlag,
                                                   false,
                                                   fociPos)) {
                     if (focus->getCellStructure() == surfaceStructureType) {
                        useFocus = true;
                     }
                  }

                  if (useFocus) {
                     //
                     // See how close this focus is to coordinate
                     //
                     const float distSquared = vtkMath::Distance2BetweenPoints(fociPos, nodePos);
                     if ((distSquared < upperLimitSquared) && (distSquared < closestFocusDistance)) {
                        closestFocus = k;
                        closestFocusDistance = distSquared;
                     }
                  }
               }
            }
         }
         
         if (closestFocus >= 0) {
            focusNearestNode[fociClass] = closestFocus;
            focusDistanceToNode[fociClass] = std::sqrt(closestFocusDistance);
         }
      }
      
      //
      // Node coloring
      //
      float nodeColor[3] = { 0.0, 0.0, 0.0 };
      std::vector<int> nearbyFoci;
      std::vector<float> nearbyFociDistances;
      
      //
      // Loop through foci classes
      //
      for (int j = 0; j < numClasses; j++) {
         if (focusNearestNode[j] >= 0) {
            if ((focusDistanceToNode[j] >= lowerLimit) &&
                (focusDistanceToNode[j] <= upperLimit)) {
               nearbyFoci.push_back(focusNearestNode[j]);
               nearbyFociDistances.push_back(focusDistanceToNode[j]);
            }
         }
      }
      
      const int nearbyFociCount = static_cast<int>(nearbyFoci.size());
      
      if (nearbyFociCount > 2) {
         //
         // Paint node gray
         //
         nodeColor[0] = 50.0;
         nodeColor[1] = 50.0;
         nodeColor[2] = 50.0;
      }
      else if (nearbyFociCount == 2) {
         //
         // Get color of foci nearest the node
         //
         int colorIndex = fociProjectionFile->getCellProjection(nearbyFoci[0])->getColorIndex();
         unsigned char r, g, b;
         if (colorIndex >= 0) {
            fociColorFile->getColorByIndex(colorIndex, r, g, b);
         }
         else {
            //
            // Use foreground color
            //
            r = foregroundColor[0];
            g = foregroundColor[1];
            b = foregroundColor[2];
         }
         const float r1 = r;
         const float g1 = g;
         const float b1 = b;
         
         //
         // Get color of foci second nearest the node
         //
         colorIndex = fociProjectionFile->getCellProjection(nearbyFoci[1])->getColorIndex();
         if (colorIndex >= 0) {
            fociColorFile->getColorByIndex(colorIndex, r, g, b);
         }
         else {
            //
            // Use foreground color
            //
            r = foregroundColor[0];
            g = foregroundColor[1];
            b = foregroundColor[2];
         }
         const float r2 = r;
         const float g2 = g;
         const float b2 = b;
         
         const float m = std::max(r1+r2, std::max(g1+g2, b1+b2));
         nodeColor[0] = 255.0 * (r1 + r2) / m;
         nodeColor[1] = 255.0 * (g1 + g2) / m;
         nodeColor[2] = 255.0 * (b1 + b2) / m;
         
      }
      else if (nearbyFociCount == 1) {
         //
         // Use the foci's color
         //
         const int colorIndex = fociProjectionFile->getCellProjection(nearbyFoci[0])->getColorIndex();
         if (colorIndex >= 0) {
            unsigned char r, g, b;
            fociColorFile->getColorByIndex(colorIndex, r, g, b);
            nodeColor[0] = r;
            nodeColor[1] = g;
            nodeColor[2] = b;
         }
         else {
            //
            // Use foreground color
            //
            nodeColor[0] = foregroundColor[0];
            nodeColor[1] = foregroundColor[1];
            nodeColor[2] = foregroundColor[2];
         }
         
         //
         // If within middle limit, whiten the node to produce a halo effect
         //
         if (nearbyFociDistances[0] < middleLimit) {
            nodeColor[0] = 127.5 + 0.5 * nodeColor[0];
            nodeColor[1] = 127.5 + 0.5 * nodeColor[1];
            nodeColor[2] = 127.5 + 0.5 * nodeColor[2];
         }
      }
      
      //
      // Add to rgb paint file
      //
      const float max255 = 255.0;
      nodeColor[0] = std::min(nodeColor[0], max255);
      nodeColor[1] = std::min(nodeColor[1], max255);
      nodeColor[2] = std::min(nodeColor[2], max255);      
      rgbPaintFile->setRgb(i, rgbPaintColumn, nodeColor[0], nodeColor[1], nodeColor[2]);
   }
}

