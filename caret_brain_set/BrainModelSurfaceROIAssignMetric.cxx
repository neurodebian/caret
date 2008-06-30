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
#include "BrainModelSurfaceROIAssignMetric.h"
#include "MetricFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceROIAssignMetric::BrainModelSurfaceROIAssignMetric(BrainSet* bs,
                                 BrainModelSurface* bmsIn,
                                 BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                 MetricFile* metricFileIn,
                                 const int metricColumnNumberIn,
                                 const QString& metricColumnNameIn,
                                 const float valueIn)
   : BrainModelSurfaceROIOperation(bs,
                                   bmsIn,
                                   surfaceROIIn)
{
   metricFile = metricFileIn;
   metricColumnNumber = metricColumnNumberIn;
   metricColumnName = metricColumnNameIn;
   value = valueIn;
}
                                 
/**
 * destructor.
 */
BrainModelSurfaceROIAssignMetric::~BrainModelSurfaceROIAssignMetric()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIAssignMetric::executeOperation() throw (BrainModelAlgorithmException)
{
   const int numNodes = bms->getNumberOfNodes();

   if (metricFile->getNumberOfColumns() == 0) {
      metricFile->setNumberOfNodesAndColumns(numNodes, 1);
      metricColumnNumber = metricFile->getNumberOfColumns() - 1;
   }
   else if ((metricColumnNumber < 0) ||
            (metricColumnNumber >= metricFile->getNumberOfColumns())) {
      metricFile->addColumns(1);
      metricColumnNumber = metricFile->getNumberOfColumns() - 1;
   }
   
   //
   // Set column name
   //
   metricFile->setColumnName(metricColumnNumber, metricColumnName);
   
   //
   // Assign the metric index to the nodes
   //
   for (int i = 0; i < numNodes; i++) {
      if (operationSurfaceROI->getNodeSelected(i)) {
         metricFile->setValue(i, metricColumnNumber, value);
      }
   }
}
