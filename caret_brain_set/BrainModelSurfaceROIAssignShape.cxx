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
#include "BrainModelSurfaceROIAssignShape.h"
#include "SurfaceShapeFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceROIAssignShape::BrainModelSurfaceROIAssignShape(BrainSet* bs,
                                 BrainModelSurface* bmsIn,
                                 BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                 SurfaceShapeFile* shapeFileIn,
                                 const int shapeColumnNumberIn,
                                 const QString& shapeColumnNameIn,
                                 const float valueIn)
   : BrainModelSurfaceROIOperation(bs,
                                   bmsIn,
                                   surfaceROIIn)
{
   shapeFile = shapeFileIn;
   shapeColumnNumber = shapeColumnNumberIn;
   shapeColumnName = shapeColumnNameIn;
   value = valueIn;
}
                                 
/**
 * destructor.
 */
BrainModelSurfaceROIAssignShape::~BrainModelSurfaceROIAssignShape()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIAssignShape::executeOperation() throw (BrainModelAlgorithmException)
{
   const int numNodes = bms->getNumberOfNodes();

   if (shapeFile->getNumberOfColumns() == 0) {
      shapeFile->setNumberOfNodesAndColumns(numNodes, 1);
      shapeColumnNumber = shapeFile->getNumberOfColumns() - 1;
   }
   else if ((shapeColumnNumber < 0) ||
            (shapeColumnNumber >= shapeFile->getNumberOfColumns())) {
      shapeFile->addColumns(1);
      shapeColumnNumber = shapeFile->getNumberOfColumns() - 1;
   }
   
   //
   // Set column name
   //
   shapeFile->setColumnName(shapeColumnNumber, shapeColumnName);
   
   //
   // Assign the shape index to the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      if (operationSurfaceROI->getNodeSelected(i)) {
         shapeFile->setValue(i, shapeColumnNumber, value);
      }
   }
}
