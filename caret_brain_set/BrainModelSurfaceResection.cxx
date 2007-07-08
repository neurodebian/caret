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

#include <algorithm>
#include <limits>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceResection.h"
#include "SectionFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "vtkTransform.h"

/**
 * Constructor
 */
BrainModelSurfaceResection::BrainModelSurfaceResection(
                                          BrainSet* brainSetIn,
                                          BrainModelSurface* bmsIn,
                                          vtkTransform* rotationMatrixIn,
                                          const SECTION_AXIS sectionAxisIn,
                                          const SECTION_TYPE sectionTypeIn,
                                          SectionFile* sectionFileIn,
                                          const int columnNumberIn,
                                          const QString& columnNameIn,
                                          const float thicknessIn,
                                          const int numSectionsIn)
   : BrainModelAlgorithm(brainSetIn)
{
   bms            = bmsIn;
   rotationMatrix = rotationMatrixIn;
   sectionAxis    = sectionAxisIn;
   sectionType    = sectionTypeIn;
   sectionFile    = sectionFileIn;
   columnNumber   = columnNumberIn;
   thickness      = thicknessIn;
   numSections    = numSectionsIn;
   columnName = columnNameIn;
}

/**
 * destructor.
 */
BrainModelSurfaceResection::~BrainModelSurfaceResection()
{
}

/**
 * execute the algorithm
 */
void 
BrainModelSurfaceResection::execute() throw (BrainModelAlgorithmException)
{
   //
   // Get a topology helper 
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw BrainModelAlgorithmException("Surface has no topology file.");
   }
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   vtkMatrix4x4* matrix = vtkMatrix4x4::New();
   if (rotationMatrix != NULL) {
      rotationMatrix->GetMatrix(matrix);
      //matrix->Transpose();
   }
   
   //
   //
   // Coordinate component based upon the axis being used and 
   // get min and max points
   //
   float ptMin = std::numeric_limits<float>::max();
   float ptMax = std::numeric_limits<float>::min();
   const int numNodes = bms->getNumberOfNodes();
   const CoordinateFile* coords = bms->getCoordinateFile();
   std::vector<float> ptXYZ(numNodes);
   for (int i = 0; i < numNodes; i++) {
      float xyzw[4];
      coords->getCoordinate(i, xyzw);
      switch (sectionAxis) {
         case SECTION_AXIS_X:
            ptXYZ[i] = xyzw[0];
            break;
         case SECTION_AXIS_Y:
            ptXYZ[i] = xyzw[1];
            break;
         case SECTION_AXIS_Z:
            ptXYZ[i] = xyzw[2];
            break;
         case SECTION_AXIS_Z_WITH_ROTATION_MATRIX:
            {
               float pt[4];
               xyzw[3] = 1.0;
               matrix->MultiplyPoint(xyzw, pt);
               ptXYZ[i] = pt[2];
            }
            break;
      }
      
      //
      // Update min and max
      //
      ptMin = std::min(ptMin, ptXYZ[i]);
      ptMax = std::max(ptMax, ptXYZ[i]);
   }
   
   matrix->Delete();

   //
   // If computing sections based upon thickness
   // convert to number of sections
   //
   switch (sectionType) {
      case SECTION_TYPE_THICKNESS:
         numSections = static_cast<int>(((ptMax - ptMin) / thickness) + 0.5);
         break;
      case SECTION_TYPE_NUM_SECTIONS:
         break;
   }
   
   //
   // Add space to the section file if needed and set column name
   //
   if ((columnNumber < 0) || (columnNumber >= sectionFile->getNumberOfColumns())) {
      if (sectionFile->getNumberOfColumns() <= 0) {
         sectionFile->setNumberOfNodesAndColumns(numNodes, 1);
      }
      else {
         sectionFile->addColumns(1);
      }
      columnNumber = sectionFile->getNumberOfColumns() - 1;
   }
   sectionFile->setColumnName(columnNumber, columnName);
   
   //
   // determine the sections
   //
   const float scale = static_cast<float>(numSections) / (ptMax - ptMin);
   for (int i = 0; i < numNodes; i++) {
      int sectionNumber = 0;
      if (th->getNodeHasNeighbors(i)) {
         sectionNumber = static_cast<int>(scale * (ptXYZ[i] - ptMin));
      }
         sectionFile->setSection(i, columnNumber, sectionNumber);
   }
   
   //
   // Update minimum and maximum sections
   //
   sectionFile->postColumnCreation(columnNumber);
}

