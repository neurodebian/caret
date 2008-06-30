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

#include <QStringList>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROISurfaceXYZMeansReport.h"
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "FileUtilities.h"

/**
 * constructor.
 */
BrainModelSurfaceROISurfaceXYZMeansReport::BrainModelSurfaceROISurfaceXYZMeansReport(BrainSet* bs,
                                                 BrainModelSurface* bmsIn,
                                                 BrainModelSurfaceROINodeSelection* surfaceROIIn,
                                                 std::vector<CoordinateFile*>& coordFilesIn)
   : BrainModelSurfaceROIOperation(bs, bmsIn, surfaceROIIn)
{
   surfaceMeansCoordFiles = coordFilesIn;
}                                  

/**
 * destructor.
 */
BrainModelSurfaceROISurfaceXYZMeansReport::~BrainModelSurfaceROISurfaceXYZMeansReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROISurfaceXYZMeansReport::executeOperation() throw (BrainModelAlgorithmException)
{
   const int numCoordFiles = static_cast<int>(surfaceMeansCoordFiles.size());
   if (numCoordFiles <= 0) {
      throw BrainModelAlgorithmException("ERROR: There are no coord files.");
   }
   
   //
   // Determine surface means
   //
   const int numNodes = brainSet->getNumberOfNodes();
   for (int j = 0; j < numCoordFiles; j++) {
      double meanX = 0.0;
      double meanY = 0.0;
      double meanZ = 0.0;
      double nodeCount = 0.0;
      
      const CoordinateFile* coordFile = surfaceMeansCoordFiles[j];
      for (int i = 0; i < numNodes; i++) {
         if (operationSurfaceROI->getNodeSelected(i)) {
            float xyz[3];
            coordFile->getCoordinate(i, xyz);
            meanX += xyz[0];
            meanY += xyz[1];
            meanZ += xyz[2];
            nodeCount += 1.0;
         }
      }
      
      meanX /= nodeCount;
      meanY /= nodeCount;
      meanZ /= nodeCount;
      
      const QString s = 
         (FileUtilities::basename(coordFile->getFileName())
          + " "
          + QString::number(meanX, 'f', 6)
          + " "
          + QString::number(meanY, 'f', 6)
          + " "
          + QString::number(meanZ, 'f', 6)
          + "\n");
          
      reportText.append(s);
   }
}
      
