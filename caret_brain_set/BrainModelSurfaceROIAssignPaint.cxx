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
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROIAssignPaint.h"
#include "PaintFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceROIAssignPaint::BrainModelSurfaceROIAssignPaint(BrainSet* bs,
                                 BrainModelSurface* bmsIn,
                                 BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                 PaintFile* paintFileIn,
                                 const int paintColumnNumberIn,
                                 const QString& paintColumnNameIn,
                                 const QString& paintNameIn)
   : BrainModelSurfaceROIOperation(bs,
                                   bmsIn,
                                   surfaceROIIn)
{
   paintFile = paintFileIn;
   paintColumnNumber = paintColumnNumberIn;
   paintColumnName = paintColumnNameIn;
   paintName = paintNameIn;
}
                                 
/**
 * destructor.
 */
BrainModelSurfaceROIAssignPaint::~BrainModelSurfaceROIAssignPaint()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIAssignPaint::executeOperation() throw (BrainModelAlgorithmException)
{
   if (paintName.isEmpty()) {
      throw BrainModelAlgorithmException("Paint name is empty.");
   }
   
   const int numNodes = bms->getNumberOfNodes();

   if (paintFile->getNumberOfColumns() == 0) {
      paintFile->setNumberOfNodesAndColumns(numNodes, 1);
      paintColumnNumber = paintFile->getNumberOfColumns() - 1;
   }
   else if ((paintColumnNumber < 0) ||
            (paintColumnNumber >= paintFile->getNumberOfColumns())) {
      paintFile->addColumns(1);
      paintColumnNumber = paintFile->getNumberOfColumns() - 1;
   }
   
   //
   // Set column name
   //
   paintFile->setColumnName(paintColumnNumber, paintColumnName);
   
   //
   // Get the index of the paint name
   //
   const int paintIndex = paintFile->addPaintName(paintName);
   
   //
   // Assign the paint index to the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      if (operationSurfaceROI->getNodeSelected(i)) {
         paintFile->setPaint(i, paintColumnNumber, paintIndex);
      }
   }
}
