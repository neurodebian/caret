
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

#include <set>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceClusterToBorderConverter.h"
#include "BrainModelSurfaceConnectedSearchPaint.h"
#include "BrainModelSurfacePaintToBorderConverter.h"
#include "BrainSet.h"
#include "PaintFile.h"

/**
 * constructor.
 */
BrainModelSurfacePaintToBorderConverter::BrainModelSurfacePaintToBorderConverter(
                                                          BrainSet* brainSetIn,
                                                          BrainModelSurface* bmsIn,
                                                          PaintFile* paintFileIn,
                                                          const int paintFileColumnIn)
   : BrainModelAlgorithm(brainSetIn)
{
   bms = bmsIn;
   paintFile       = paintFileIn;
   paintFileColumn = paintFileColumnIn;
}
                                        
/**
 * destructor.
 */
BrainModelSurfacePaintToBorderConverter::~BrainModelSurfacePaintToBorderConverter()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfacePaintToBorderConverter::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (bms == NULL) {
      throw BrainModelAlgorithmException("ERROR: No surface provided.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("ERROR: The surface contains no nodes.");
   }
   TopologyFile* topologyFile = bms->getTopologyFile();
   if (topologyFile == NULL) {
      throw BrainModelAlgorithmException("ERROR: Surface has no topology.");
   }
   if (paintFile == NULL) {
      throw BrainModelAlgorithmException("ERROR: No Paint file provided.");
   }
   if ((paintFileColumn < 0) ||
       (paintFileColumn >= paintFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("ERROR: Invalid paint file column.");
   }
   
   //
   // Keep track of nodes that have been searched
   //
   std::vector<bool> nodeHasBeenSearched(numNodes, false);
   
   //
   // Loop through the nodes to find the paints that are to be searched
   // exclude ???
   //
   const int questionIndex = paintFile->getPaintIndexFromName("???");
   std::set<int> paintIndicesForSearching;
   for (int i = 0; i < numNodes; i++) {
      const int indx = paintFile->getPaint(i, paintFileColumn);
      if (indx != questionIndex) {
         paintIndicesForSearching.insert(indx);
      }
   }
   
   //
   // Will keep track of border colors that are needed
   //
   std::set<QString> borderColorsNeeded;
   
   //
   // Loop through the paint indices
   //
   for (std::set<int>::iterator iter = paintIndicesForSearching.begin();
        iter != paintIndicesForSearching.end();
        iter++) {
      //
      // Index of paint for which we will search
      //
      const int paintIndex = *iter;
      
      //
      // Loop through the nodes
      //
      for (int i = 0; i < numNodes; i++) {
         //
         // Has this node NOT been checked
         //
         if (nodeHasBeenSearched[i] == false) {
            //
            // Does it have the paint index we are seeking
            //
            if (paintFile->getPaint(i, paintFileColumn) == paintIndex) {
               //
               // Mark this node searched
               //
               nodeHasBeenSearched[i] = true;
               
               //
               // Do a connected search 
               //
               BrainModelSurfaceConnectedSearchPaint paintSearch(brainSet,
                                                                 bms,
                                                                 i,
                                                                 paintFile,
                                                                 paintFileColumn,
                                                                 paintIndex);
               paintSearch.execute();
               
               //
               // Get the connected nodes found
               //
               bool atLeastOneNodeInCluster = false;
               std::vector<bool> nodeInPaintCluster(numNodes, false);
               for (int j = 0; j < numNodes; j++) {
                  if (paintSearch.getNodeConnected(j)) {
                     //
                     // Have at least on node in the cluster
                     //
                     atLeastOneNodeInCluster = true;
                     
                     //
                     // Mark node has been searched
                     //
                     nodeHasBeenSearched[j] = true;
                     
                     //
                     // Mark node in the cluster
                     //
                     nodeInPaintCluster[j] = true;
                  }
               }
               
               //
               // If nodes are found
               //
               if (atLeastOneNodeInCluster) {
                  //
                  // Create a border around the cluster
                  //
                  BrainModelSurfaceClusterToBorderConverter borderGenerator(brainSet,
                                                                            bms,
                                                                            topologyFile,
                                                                            paintFile->getPaintNameFromIndex(paintIndex),
                                                                            nodeInPaintCluster);
                  borderGenerator.execute();
                  
                  //
                  // Will need a border colors
                  //
                  borderColorsNeeded.insert(paintFile->getPaintNameFromIndex(paintIndex));
               }
            }
         }
      }
   }
   
   //
   // Add any needed border colors
   //
   AreaColorFile* acf = brainSet->getAreaColorFile();
   BorderColorFile* bcf = brainSet->getBorderColorFile();
   for (std::set<QString>::iterator iter = borderColorsNeeded.begin();
        iter != borderColorsNeeded.end(); 
        iter++) {
      const QString colorName(*iter);
      
      //
      // Find area color
      //
      bool areaColorMatch = false;
      const int areaColorIndex = acf->getColorIndexByName(colorName, areaColorMatch);
      
      //
      // Find border color
      //
      bool borderColorMatch = false;
      const int borderColorIndex = bcf->getColorIndexByName(colorName, borderColorMatch);
      
      //
      // If border color does not exist
      //
      if (borderColorIndex < 0) {
         //
         // If there is an area color
         //
         if (areaColorIndex >= 0) {
            //
            // Transfer the area color to the border color
            //
            float pointSize, lineSize;
            acf->getPointLineSizeByIndex(areaColorIndex, pointSize, lineSize);
            unsigned char r, g, b, a;
            acf->getColorByIndex(areaColorIndex, r, g, b, a);
            
            bcf->addColor(colorName, r, g, b, a, pointSize, lineSize);
         }
      }
   }
}
            
