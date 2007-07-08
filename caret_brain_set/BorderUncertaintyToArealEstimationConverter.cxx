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
#define NOMINMAX
#endif

#include <cmath>
#include <limits>

#include "ArealEstimationFile.h"
#include "BorderFile.h"
#include "BorderUncertaintyToArealEstimationConverter.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "PaintFile.h"

/**
 * Constructor.
 */
BorderUncertaintyToArealEstimationConverter::BorderUncertaintyToArealEstimationConverter(
                                       BrainSet* bs,
                                       BrainModelSurface* surfaceIn,
                                       ArealEstimationFile* arealEstimationFileIn,
                                       BorderFile* borderFileIn,
                                       PaintFile* paintFileIn,
                                       const MODE modeIn,
                                       const int arealEstimationFileColumnIn,
                                       const QString& arealEstimationFileColumnNameIn,
                                       const QString& longNameIn,
                                       const QString& commentIn,
                                       const int paintColumnIn,
                                       const QString paintMatchNameIn,
                                       const bool overrideBorderUncertaintyIn,
                                       const float overrideBorderUncertaintyValueIn)
   : BrainModelAlgorithm(bs)
{
   surface = surfaceIn;
   arealEstimationFile = arealEstimationFileIn;
   borderFile = borderFileIn;
   paintFile = paintFileIn;
   mode = modeIn;
   arealEstimationFileColumn = arealEstimationFileColumnIn;
   arealEstimationFileColumnName = arealEstimationFileColumnNameIn;
   longName = longNameIn;
   comment = commentIn;
   paintColumn = paintColumnIn;
   paintMatchName = paintMatchNameIn;
   overrideBorderUncertainty = overrideBorderUncertaintyIn;
   overrideBorderUncertaintyValue = overrideBorderUncertaintyValueIn;

   comment.append("\nBorder File: ");
   comment.append("\nUncertainty override: ");
   comment.append(FileUtilities::basename(borderFile->getFileName()));
   if (overrideBorderUncertainty) {
      comment.append(QString::number(overrideBorderUncertaintyValue, 'f', 2));
   }
   else {
      comment.append("use values from border file.");
   }
}

//
// Find closest link in all borders from pos in X-Y plane.
//
void
BorderUncertaintyToArealEstimationConverter::findClosestBorderLinks2D(const float* pos,
                                                   std::set<BorderDist>& closestBorders)
{
   closestBorders.clear();
   const int numBorders = borderFile->getNumberOfBorders();
   for (int j = 0; j < numBorders; j++) {
      int closestLink = -1;
      float distanceToLink = std::numeric_limits<float>::max();

      Border* b = borderFile->getBorder(j);
      if (b->getDisplayFlag()) {
         const int numLinks = b->getNumberOfLinks();
         for (int i = 0; i < numLinks; i++) {
            const float* borderPos = b->getLinkXYZ(i);
            const float dx = pos[0] - borderPos[0];
            const float dy = pos[1] - borderPos[1];
            const float dist = dx*dx + dy*dy;
            if (dist < distanceToLink) {
               distanceToLink = dist;
               closestLink = i;
            }
         }

         if (closestLink >= 0) {
            distanceToLink = std::sqrt(distanceToLink);
            BorderDist bd(j, closestLink, distanceToLink);
            closestBorders.insert(bd);
         }
      }
   }
}

float
BorderUncertaintyToArealEstimationConverter::determineSignedDistance(const float* pos, 
                                                                     const BorderDist& bd)
{
   Border* border = borderFile->getBorder(bd.borderNumber);
   const float* borderPos = border->getLinkXYZ(bd.borderNearestLink);

   //
   // vector from border point to node
   //
   float bpToNode[3];
   MathUtilities::subtractVectors((float*)pos, borderPos, bpToNode);
   const float bpMag = std::sqrt(bpToNode[0]*bpToNode[0] + bpToNode[1]*bpToNode[1]);
   if (bpMag != 0.0) {
      bpToNode[0] /= bpMag;
      bpToNode[1] /= bpMag;
   }
   else {
      return 0.0;
   }
   
   //
   // signed distance of node from border point
   // Note: both bpToNode and the border normal are unit vectors so the dot
   // product of bpToNode and the border normal give the cosine of the angle
   // formed by them.
   //
   const float* flatNormal = border->getLinkFlatNormal(bd.borderNearestLink);
   const float cosAngle = bpToNode[0] * flatNormal[0] 
                        + bpToNode[1] * flatNormal[1];
                        
   const float signedDistance = (cosAngle * bpMag);
   return signedDistance;  
}

/**
 * Destructor.
 */
BorderUncertaintyToArealEstimationConverter::~BorderUncertaintyToArealEstimationConverter()
{
}

/**
 * execute the algorithm.
 */
void 
BorderUncertaintyToArealEstimationConverter::execute() throw (BrainModelAlgorithmException)
{
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface contains nodes.");
   }
   
   //
   // Make sure we have some paints
   //
   const int numPaintColumns = paintFile->getNumberOfColumns();
   if (numPaintColumns == 0) {
      if (mode == MODE_NODES_WITH_PAINT) {
         throw BrainModelAlgorithmException("The paint file is empty.");
      }
   }
   
   //
   // Make sure there are some borders
   //
   const int numBorders = borderFile->getNumberOfBorders();
   if (numBorders <= 0) {
      throw BrainModelAlgorithmException("The border file is empty.");
   }
   
   //
   // Compute normals for flat borders
   //
   borderFile->computeFlatNormals();
   
   //
   // Determine which paint names match the name entered by the user
   //
   std::vector<bool> matchingPaints;
   switch (mode) {
      case MODE_ALL_NODES:
         break;
      case MODE_NODES_WITH_PAINT:
         {
            const int numPaintNames = paintFile->getNumberOfPaintNames();
            if (numPaintNames <= 0) {
               throw BrainModelAlgorithmException("There are no paint names.");
            }
            matchingPaints.resize(numPaintNames, false);
            for (int j = 0; j < numPaintNames; j++) {
               if (paintFile->getPaintNameFromIndex(j) == paintMatchName) {
                  matchingPaints[j] = true;
               }
            }
         }
         break;
   }
   
   //
   // Get access to surface's Areal Estimation File
   //
   if (arealEstimationFile->getNumberOfColumns() <= 0) {
      arealEstimationFile->setNumberOfNodesAndColumns(numNodes, 1);
      arealEstimationFileColumn = arealEstimationFile->getNumberOfColumns() - 1;
   }
   else {
      if ((arealEstimationFileColumn < 0) || 
          (arealEstimationFileColumn >= arealEstimationFile->getNumberOfColumns())) {
         arealEstimationFile->addColumns(1);
         arealEstimationFileColumn = arealEstimationFile->getNumberOfColumns() - 1;
      }
   }
   arealEstimationFile->setColumnName(arealEstimationFileColumn, arealEstimationFileColumnName);
   
   arealEstimationFile->setLongName(arealEstimationFileColumn, longName);
   arealEstimationFile->setColumnComment(arealEstimationFileColumn, comment);
   
   //
   // Search through the nodes
   //
   const CoordinateFile* cf = surface->getCoordinateFile();
   for (int i = 0; i < numNodes; i++) {
      //
      // See if this node should be used
      //
      bool useNode = false;
      switch (mode) {
         case MODE_ALL_NODES:
            useNode = true;
            break;
         case MODE_NODES_WITH_PAINT:
            useNode = matchingPaints[paintFile->getPaint(i, paintColumn)];
            break;
      }
      
      if (useNode) {
         std::set<BorderDist> closestBorders;
         findClosestBorderLinks2D(cf->getCoordinate(i), closestBorders);
         
         int ctr = 0;
         QString names[4] = { "NULL", "NULL", "NULL", "NULL" };
         float probabilities[4] = { 0.0, 0.0, 0.0, 0.0 };
         float sumProbability = 0.0;
         for (std::set<BorderDist>::iterator cbi = closestBorders.begin();
              cbi != closestBorders.end(); cbi++) {
            const int borderNumber = cbi->borderNumber;
            Border* border = borderFile->getBorder(borderNumber);
            const QString borderName(border->getName());
            
            //
            // Make sure border with same name is not already found
            //
            bool useIt = true;
            for (int ib = 0; ib < ctr; ib++) {
               if (borderName == names[ib]) {
                  useIt = false;
                  break;
               }
            }
            
            if (useIt) {
               names[ctr] = borderName;
               const float signedDistance = determineSignedDistance(
                                                   cf->getCoordinate(i), *cbi);
               float borderUncertainty = border->getArealUncertainty();
               if (overrideBorderUncertainty) {
                  borderUncertainty = overrideBorderUncertaintyValue;
               }
               
               float temp = ((signedDistance / borderUncertainty) - 1.0) * 0.5;
               if (temp < -10.0) {
                  temp = -10.0;
               }
               probabilities[ctr] = exp(temp);
               if (probabilities[ctr] > 1.0) {
                  probabilities[ctr] = 1.0;
               }
               
               sumProbability += probabilities[ctr];
               
               ctr++;
               if (ctr >= 4) {
                  break;
               }
            }
         }
         
         std::set<ProbabilitySort> probSort;
         for (int m = 0; m < ctr; m++) {
            ProbabilitySort ps(names[m], probabilities[m] / sumProbability);
            probSort.insert(ps);
         }
         
         int pctr =0;
         for (std::set<ProbabilitySort>::iterator pi = probSort.begin();
              pi != probSort.end(); pi++) {
            names[pctr] = pi->name;
            probabilities[pctr] = pi->probability;
            pctr++;
         }
         
         arealEstimationFile->setNodeData(i, arealEstimationFileColumn, names, probabilities);
         
      }
   }
}
