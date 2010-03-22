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

#include "BrainModelSurfaceStandardSphere.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "SpecFile.h"

/**
 * Constructor.
 */
BrainModelSurfaceStandardSphere::BrainModelSurfaceStandardSphere(
                                                 BrainSet* bs,
                                                 const int numberOfNodesIn)
   : BrainModelAlgorithm(bs),
     numberOfNodes(numberOfNodesIn)
{
}

/**
 * Destructor.
 */
BrainModelSurfaceStandardSphere::~BrainModelSurfaceStandardSphere()
{
}

/**
 * get number of nodes and triangles from the number of iterations.
 */
void
BrainModelSurfaceStandardSphere::getNumberOfNodesAndTriangles(
                                        std::vector<int>& numNodesOut,
                                        std::vector<int>& numTrianglesOut)
{
   numNodesOut.clear();
   numTrianglesOut.clear();

   //
   // sphere.v5.1.spec
   //
   numNodesOut.push_back(74);  numTrianglesOut.push_back(144);

   //
   // sphere.v5.2.spec
   //
   numNodesOut.push_back(290);  numTrianglesOut.push_back(576);

   //
   // sphere.v5.3.spec
   //
   numNodesOut.push_back(1154);  numTrianglesOut.push_back(2304);

   //
   // sphere.v5.4.spec
   //
   numNodesOut.push_back(4610);  numTrianglesOut.push_back(9216);

   //
   // sphere.v5.5.spec
   //
   numNodesOut.push_back(18434);  numTrianglesOut.push_back(36864);

   //
   // sphere.v5.6.spec
   //
   numNodesOut.push_back(73730);  numTrianglesOut.push_back(147456);
}

/**
 * execute the algorithm.
 */
void 
BrainModelSurfaceStandardSphere::execute() throw (BrainModelAlgorithmException)
{
   //
   // Read in the standard sphere
   //
   QString standardSphereSpecName(brainSet->getCaretHomeDirectory());
   switch(numberOfNodes) {
      case 74:
         standardSphereSpecName.append("/data_files/REGISTER.SPHERE/sphere.v5.1.spec");
         break;
      case 290:
         standardSphereSpecName.append("/data_files/REGISTER.SPHERE/sphere.v5.2.spec");
         break;
      case 1154:
         standardSphereSpecName.append("/data_files/REGISTER.SPHERE/sphere.v5.3.spec");
         break;
      case 4610:
         standardSphereSpecName.append("/data_files/REGISTER.SPHERE/sphere.v5.4.spec");
         break;
      case 18434:
         standardSphereSpecName.append("/data_files/REGISTER.SPHERE/sphere.v5.5.spec");
         break;
      case 73730:
         standardSphereSpecName.append("/data_files/REGISTER.SPHERE/sphere.v5.6.spec");
         break;
   }
   
   //
   // Read in the standard sphere spec file
   //
   try {
      SpecFile sf;
      sf.readFile(standardSphereSpecName);
      sf.setAllFileSelections(SpecFile::SPEC_TRUE);
      QString errorMessage;
      brainSet->readSpecFile(sf, standardSphereSpecName, errorMessage);
      if (errorMessage.isEmpty() == false) {
         throw BrainModelAlgorithmException(errorMessage);
      }
      
      //
      // Get the standard spherical surface
      //
      BrainModelSurface* standardSphere = brainSet->getBrainModelSurface(0);
      if (standardSphere == NULL) {
         throw BrainModelAlgorithmException(
                     "Unable to find standard sphere after reading it");
      }
   }
   catch (FileException e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
}      
