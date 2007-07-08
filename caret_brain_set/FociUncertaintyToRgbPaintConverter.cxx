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


#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX  // needed for min/max in algorithm & numeric_limits
#endif

#include <algorithm>
#include <cmath>
#include <limits>

#include <QApplication>
#include <QProgressDialog>

#include "vtkMath.h"

#include "BrainSet.h"
#include "DisplaySettingsFoci.h"
#include "FociFile.h"
#include "FociColorFile.h"
#include "FociProjectionFile.h"
#include "FociUncertaintyToRgbPaintConverter.h"
#include "RgbPaintFile.h"

/**
 * Constructor
 */
FociUncertaintyToRgbPaintConverter::FociUncertaintyToRgbPaintConverter()
{
}

/**
 * Destructor
 */
FociUncertaintyToRgbPaintConverter::~FociUncertaintyToRgbPaintConverter()
{
}

/**
 * Convert foci uncertainty to rgb paint (returns true if successful).
 */
bool
FociUncertaintyToRgbPaintConverter::convert(BrainSet* brain,
                   const float lowerLimit, const float middleLimit, const float upperLimit,
                   const int rgbPaintColumnIn, const QString& rgbPaintColumnNewName,
                   QWidget* progressDialogParent, QString& errorMessage)
{
   //
   // Get the fiducial surface & its coordinate file
   //
   BrainModelSurface* bms = brain->getActiveFiducialSurface();
   if (bms == NULL) {
      errorMessage = "There is no active fiducial surface.";
      return true;
   }
   errorMessage = "";
   
   const float upperLimitSquared = upperLimit*upperLimit;
   
   //
   // Get fiducial foci file
   //
   FociProjectionFile* fpf = brain->getFociProjectionFile();
   FociFile fociFile;
   fpf->getCellFile(bms->getCoordinateFile(),
                   bms->getTopologyFile(),
                   true,
                   fociFile);

   const int numFoci = fociFile.getNumberOfCells();
   if (numFoci <= 0) {
      errorMessage = "There are no fiducial foci.  Do you need to project foci ?";
      return true;
   }
   
   //
   // Get number of foci classes
   //
   const int numClasses = fociFile.getNumberOfCellClasses();
   if (numClasses <= 0) {
      errorMessage = "ERROR: There are no foci classes.";
      return true;
   }
   
   const CoordinateFile* cf = bms->getCoordinateFile();
   const int numCoords = cf->getNumberOfCoordinates();
   const bool leftHemisphere = (bms->getStructure() == Structure::STRUCTURE_TYPE_CORTEX_LEFT);
   
   //
   // Storage for number of nodes in each class
   //
   std::vector<int> nodeCount(numClasses, 0);
   
   //
   // Check left right selections
   //
   DisplaySettingsFoci* dsf = brain->getDisplaySettingsFoci();
   const bool correctHemOnly = dsf->getDisplayCellsOnCorrectHemisphereOnly();
   //const bool doRightFoci = dsf->getDisplayRightHemisphereCells();
   //const bool doLeftFoci  = dsf->getDisplayLeftHemisphereCells();
   
   //
   // Initialize color to foreground color
   //
   unsigned char foregroundColor[3];
   PreferencesFile* pref = brain->getPreferencesFile();
   pref->getSurfaceForegroundColor(foregroundColor[0], foregroundColor[1], foregroundColor[2]);
   
   //
   // Add a column to the rgb paint file
   //
   RgbPaintFile* rgbPaintFile = brain->getRgbPaintFile();
   int rgbPaintColumn = rgbPaintColumnIn;
   if (rgbPaintFile->getNumberOfNodes() == 0) {
      rgbPaintFile->setNumberOfNodesAndColumns(bms->getNumberOfNodes(), 1);
      rgbPaintColumn = 0;
      rgbPaintFile->setColumnName(rgbPaintColumn, rgbPaintColumnNewName);
   }
   else if ((rgbPaintColumn < 0) ||
            (rgbPaintColumn >= rgbPaintFile->getNumberOfColumns())) {
      rgbPaintColumn = rgbPaintFile->getNumberOfColumns();
      rgbPaintFile->addColumns(1);
      rgbPaintFile->setColumnName(rgbPaintColumn, rgbPaintColumnNewName);
   }
   rgbPaintFile->setScaleRed(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setScaleGreen(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setScaleBlue(rgbPaintColumn, 0, 255.0);
   rgbPaintFile->setColumnComment(rgbPaintColumn, 
                                  "Created from foci uncertainty");
   
   ColorFile* fociColorFile = brain->getFociColorFile();
   
   //
   // Create a progress dialog 
   //
   QProgressDialog* progressDialog = NULL;
   if (progressDialogParent != NULL) {
      progressDialog = new QProgressDialog("Mapping Uncertainty",
                                             0,
                                             0,
                                             numCoords,
                                             progressDialogParent);
      progressDialog->setWindowTitle("Mapping Uncertainty");
      progressDialog->setValue(0);
      progressDialog->show();
   }
   
   //
   // Loop through fiducial coordinates
   //
   for (int i = 0; i < numCoords; i++) {
      if (progressDialog != NULL) {
         if (progressDialog->wasCanceled()) {
            errorMessage = "Cancelled by user";
            delete progressDialog;
            return true;
         }
         progressDialog->setValue(i);
         qApp->processEvents(); // note: qApp is global in QApplication
      }

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
            const CellData* foci = fociFile.getCell(k);
            if (foci->getDisplayFlag()) {
               //
               //  Get the foci's class and see if it matches the current foci class
               //
               const int classNumber = foci->getClassIndex();
               if (classNumber == fociClass) {
                  //
                  // Get the foci's position and adjust for hemisphere
                  //
                  bool useFocus = false;
                  float fociPos[3];
                  foci->getXYZ(fociPos);
                  if (fociPos[0] > 0.0) {
                     if (correctHemOnly) {
                        if (leftHemisphere == false) {
                           useFocus = true;
                        }
                     }
                     else {
                        useFocus = true;
                        if (leftHemisphere) {
                           fociPos[0] = -fociPos[0];
                        }
                     }
                  }
                  else {
                     if (correctHemOnly) {
                        if (leftHemisphere) {
                           useFocus = true;
                        }
                     }
                     else {
                        if (leftHemisphere == false) {
                           fociPos[0] = -fociPos[0];
                        }
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
         int colorIndex = fociFile.getCell(nearbyFoci[0])->getColorIndex();
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
         colorIndex = fociFile.getCell(nearbyFoci[1])->getColorIndex();
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
         const int colorIndex = fociFile.getCell(nearbyFoci[0])->getColorIndex();
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
   
   //
   // Remove the progress dialog
   //
   if (progressDialog != NULL) {
      progressDialog->setValue(numCoords + 1);
      delete progressDialog;
   }
   
   return false;
}

