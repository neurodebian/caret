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
#ifdef Q_OS_WIN32     // required for M_PI in <cmath>
#define _USE_MATH_DEFINES
#define NOMINMAX
#endif

#include <QApplication>
#include <QDateTime>
#include <QFile>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>

#include <QDir>

#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurfaceDistortion.h"
#include "BrainModelSurfaceFlatHexagonalSubsample.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "StatisticDataGroup.h"
#include "StringUtilities.h"
#include "TopologyHelper.h"
#include "TransformationMatrixFile.h"
#include "vtkMath.h"
#include "vtkTriangle.h"

/**
 * Constructor
 */
BrainModelSurfaceMultiresolutionMorphing::BrainModelSurfaceMultiresolutionMorphing(
                                             BrainSet* brainSetIn,
                                             BrainModelSurface* referenceSurfaceIn,
                                             BrainModelSurface* morphingSurfaceIn,
               const BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingSurfaceTypeIn,
               const BorderProjection* centralSulcusBorderProjectionIn)
   : BrainModelAlgorithm(brainSetIn),
     referenceSurface(referenceSurfaceIn),
     morphingSurface(morphingSurfaceIn),
     morphingSurfaceType(morphingSurfaceTypeIn),
     centralSulcusBorderProjection(centralSulcusBorderProjectionIn)
{   
   autoSaveFilesFlag = true;
   
   currentCycle = std::numeric_limits<int>::max();
   
   brainModelSurfaceType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
   
   
   for (int i = 0; i < MAXIMUM_NUMBER_OF_CYCLES; i++) {
      switch (morphingSurfaceType) {
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
            iterationsPerLevel[i][0] = 0;
            iterationsPerLevel[i][1] = 10;
            iterationsPerLevel[i][2] = 20;
            iterationsPerLevel[i][3] = 40;
            iterationsPerLevel[i][4] = 60;
            iterationsPerLevel[i][5] = 80;
            iterationsPerLevel[i][6] = 100;
            smoothingStrength[i]       = 1.0;
            smoothingEdgeIterations[i] = 10;
            smoothingIterations[i]     = 50;
            linearForce[i]      = 0.50;
            angularForce[i]     = 0.40;
            stepSize[i]         = 0.5;
            break;
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
            iterationsPerLevel[i][0] =  0;
            iterationsPerLevel[i][1] = 25;
            iterationsPerLevel[i][2] = 10;
            iterationsPerLevel[i][3] = 10;
            iterationsPerLevel[i][4] =  5;
            iterationsPerLevel[i][5] =  2;
            iterationsPerLevel[i][6] =  1;
            smoothingStrength[i]       = 1.0;
            smoothingEdgeIterations[i] = 10;
            smoothingIterations[i]     = 10;
            linearForce[i]      = 0.50;
            angularForce[i]     = 0.30;
            stepSize[i]         = 0.50;
            break;
      }
   }
   
   switch (morphingSurfaceType) {
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
         brainModelSurfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
         numberOfCycles = 5;   
         numberOfLevels = 7;
         crossoverSmoothAtEndOfEachCycle = true;
         crossoverSmoothStrength = 1.0;
         crossoverSmoothCycles   = 10;
         crossoverSmoothIterations = 50;
         crossoverSmoothEdgeIterations = 10;
         crossoverSmoothProjectToSphereIterations = 0;
         crossoverSmoothNeighborDepth = 5;

         iterationsPerLevel[0][0] = 0;
         iterationsPerLevel[0][1] = 10;
         iterationsPerLevel[0][2] = 20;
         iterationsPerLevel[0][3] = 40;
         iterationsPerLevel[0][4] = 60;
         iterationsPerLevel[0][5] = 70;
         iterationsPerLevel[0][6] = 60;
         linearForce[0]           = 0.50;
         angularForce[0]          = 0.50;
         stepSize[0]              = 0.50;
         
         iterationsPerLevel[1][0] = 0;
         iterationsPerLevel[1][1] = 10;
         iterationsPerLevel[1][2] = 20;
         iterationsPerLevel[1][3] = 40;
         iterationsPerLevel[1][4] = 60;
         iterationsPerLevel[1][5] = 60;
         iterationsPerLevel[1][6] = 50;
         linearForce[1]           = 0.50;
         angularForce[1]          = 0.50;
         stepSize[1]              = 0.50;
         
         iterationsPerLevel[2][0] = 0;
         iterationsPerLevel[2][1] = 10;
         iterationsPerLevel[2][2] = 20;
         iterationsPerLevel[2][3] = 40;
         iterationsPerLevel[2][4] = 60;
         iterationsPerLevel[2][5] = 60;
         iterationsPerLevel[2][6] = 50;
         linearForce[2]           = 0.50;
         angularForce[2]          = 0.50;
         stepSize[2]              = 0.50;
         
         iterationsPerLevel[3][0] = 0;
         iterationsPerLevel[3][1] = 20;
         iterationsPerLevel[3][2] = 20;
         iterationsPerLevel[3][3] = 40;
         iterationsPerLevel[3][4] = 60;
         iterationsPerLevel[3][5] = 50;
         iterationsPerLevel[3][6] = 30;
         linearForce[3]           = 0.50;
         angularForce[3]          = 0.50;
         stepSize[3]              = 0.50;
         
         iterationsPerLevel[4][0] = 0;
         iterationsPerLevel[4][1] = 20;
         iterationsPerLevel[4][2] = 20;
         iterationsPerLevel[4][3] = 40;
         iterationsPerLevel[4][4] = 60;
         iterationsPerLevel[4][5] = 50;
         iterationsPerLevel[4][6] = 30;
         linearForce[4]           = 0.40;
         angularForce[4]          = 0.50;
         stepSize[4]              = 0.50;
         
         break;
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
         brainModelSurfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
         numberOfCycles = 4;   
         numberOfLevels = 6;
         
         iterationsPerLevel[0][0] = 2;
         iterationsPerLevel[0][1] = 3;
         iterationsPerLevel[0][2] = 5;
         iterationsPerLevel[0][3] = 20;
         iterationsPerLevel[0][4] = 20;
         iterationsPerLevel[0][5] = 15;
         iterationsPerLevel[0][6] = 0;
         
         iterationsPerLevel[1][0] = 2;
         iterationsPerLevel[1][1] = 3;
         iterationsPerLevel[1][2] = 15;
         iterationsPerLevel[1][3] = 20;
         iterationsPerLevel[1][4] = 15;
         iterationsPerLevel[1][5] = 0;
         iterationsPerLevel[1][6] = 0;
         
         iterationsPerLevel[2][0] = 2;
         iterationsPerLevel[2][1] = 5;
         iterationsPerLevel[2][2] = 10;
         iterationsPerLevel[2][3] = 10;
         iterationsPerLevel[2][4] = 10;
         iterationsPerLevel[2][5] = 0;
         iterationsPerLevel[2][6] = 0;
         
         iterationsPerLevel[3][0] = 2;
         iterationsPerLevel[3][1] = 5;
         iterationsPerLevel[3][2] = 10;
         iterationsPerLevel[3][3] = 20;
         iterationsPerLevel[3][4] = 5;
         iterationsPerLevel[3][5] = 0;
         iterationsPerLevel[3][6] = 0;
         
         crossoverSmoothAtEndOfEachCycle = true;
         crossoverSmoothStrength = 1.0;
         crossoverSmoothCycles   = 10;
         crossoverSmoothIterations = 10;
         crossoverSmoothEdgeIterations = 0;
         crossoverSmoothProjectToSphereIterations = 10;
         crossoverSmoothNeighborDepth = 30;
         break;
   }
   outputFileNamePrefix = "";
   outputFileNameSuffix = "";
   
   deleteIntermediateFiles = true;
   intermediateFiles.clear();
   
   smoothOutFlatSurfaceOverlap = true;
   pointSphericalTilesOutward  = false;
   
   //
   // Get name of original coordinate file
   //
   if (morphingSurface != NULL) {
      const CoordinateFile* origCoord = morphingSurface->getCoordinateFile();
      origCoordFileName = origCoord->getFileName();
   }
}

/**
 * Destructor
 */
BrainModelSurfaceMultiresolutionMorphing::~BrainModelSurfaceMultiresolutionMorphing()
{
}

/** 
 * Copy parameters from another object
 */
void
BrainModelSurfaceMultiresolutionMorphing::copyParameters(
                                          const BrainModelSurfaceMultiresolutionMorphing& bmsm)
{
   numberOfCycles = bmsm.numberOfCycles;
   numberOfLevels = bmsm.numberOfLevels;
   
   for (int i = 0; i < MAXIMUM_NUMBER_OF_CYCLES; i++) {
      for (int j = 0; j < MAXIMUM_NUMBER_OF_LEVELS; j++) {
         iterationsPerLevel[i][j] = bmsm.iterationsPerLevel[i][j];
      }
      
      linearForce[i] = bmsm.linearForce[i];
      angularForce[i] = bmsm.angularForce[i];
      stepSize[i] = bmsm.stepSize[i];
      smoothingStrength[i] = bmsm.smoothingStrength[i];
      smoothingIterations[i] = bmsm.smoothingIterations[i];
      smoothingEdgeIterations[i] = bmsm.smoothingEdgeIterations[i];
   }
   
   deleteIntermediateFiles = bmsm.deleteIntermediateFiles;
   smoothOutFlatSurfaceOverlap = bmsm.smoothOutFlatSurfaceOverlap;
   pointSphericalTilesOutward = bmsm.pointSphericalTilesOutward;
   crossoverSmoothAtEndOfEachCycle = bmsm.crossoverSmoothAtEndOfEachCycle;
}

/**
 * get morphing parameters
 */
void 
BrainModelSurfaceMultiresolutionMorphing::getMorphingParameters(
                                            const int cycleNumber,
                                            float& linearForceOut,
                                            float& angularForceOut,
                                            float& stepSizeOut) const
{
   linearForceOut  = linearForce[cycleNumber];
   angularForceOut = angularForce[cycleNumber];
   stepSizeOut     = stepSize[cycleNumber];
}
                           
/**
 * set morphing parameters
 */
void 
BrainModelSurfaceMultiresolutionMorphing::setMorphingParameters(
                                            const int cycleNumber,
                                            const float linearForceIn,
                                            const float angularForceIn,
                                            const float stepSizeIn)
{
   linearForce[cycleNumber]  = linearForceIn;
   angularForce[cycleNumber] = angularForceIn;
   stepSize[cycleNumber]     = stepSizeIn;
}
   
/**
 * Set the number of cycles
 */
void
BrainModelSurfaceMultiresolutionMorphing::setNumberOfCycles(const int num)
{
   numberOfCycles = num;
   numberOfCycles = std::min(numberOfCycles, static_cast<int>(MAXIMUM_NUMBER_OF_CYCLES));
   numberOfCycles = std::max(numberOfCycles, 1);
}

/**
 * get the iterations per level
 */
void 
BrainModelSurfaceMultiresolutionMorphing::getIterationsPerLevel(
                                            const int cycleNumber,
                                            int iter[MAXIMUM_NUMBER_OF_LEVELS]) const
{
   for (int i = 0; i < MAXIMUM_NUMBER_OF_LEVELS; i++) {
      iter[i] = iterationsPerLevel[cycleNumber][i];
   }
}

/**
 * set the iterations per level
 */
void 
BrainModelSurfaceMultiresolutionMorphing::setIterationsPerLevel(
                                            const int cycleNumber,
                                            const int iter[MAXIMUM_NUMBER_OF_LEVELS])
{
   for (int i = 0; i < MAXIMUM_NUMBER_OF_LEVELS; i++) {
      iterationsPerLevel[cycleNumber][i] = iter[i];
   }
}

/**
 * get smoothing parameters
 */
void 
BrainModelSurfaceMultiresolutionMorphing::getSmoothingParameters(
                                                            const int cycleNumber,
                                                            float& strength,
                                                            int& iterations,
                                                            int& edgeIterations) const
{
   strength = smoothingStrength[cycleNumber];
   iterations = smoothingIterations[cycleNumber];
   edgeIterations = smoothingEdgeIterations[cycleNumber];
}

/**
 * set smoothing parameters
 */
void 
BrainModelSurfaceMultiresolutionMorphing::setSmoothingParameters(
                                                            const int cycleNumber,
                                                            const float strength,
                                                            const int iterations,
                                                            const int edgeIterations)
{
   smoothingStrength[cycleNumber]       = strength;
   smoothingIterations[cycleNumber]     = iterations;
   smoothingEdgeIterations[cycleNumber] = edgeIterations;
}

/**
 * Setup the the output file name prefix and suffix
 */
void
BrainModelSurfaceMultiresolutionMorphing::setUpOutputFileNaming()
{
   outputFileNamePrefix = "";
   outputFileNameSuffix = "";
   
   //
   // Get the name of the input coordinate file
   //
   const CoordinateFile* cf = morphingSurface->getCoordinateFile();
   QString morphFileName(cf->getFileName());
   if (morphFileName.isEmpty()) {
      const TopologyFile* tf = morphingSurface->getTopologyFile();
      morphFileName = tf->getFileName();
   }
   if (morphFileName.isEmpty()) {
      morphFileName = brainSet->getSpecFileName();
   }
   
   if (morphFileName.isEmpty()) {
      outputFileNamePrefix = "morphing_output";
      outputFileNameSuffix = SpecFile::getCoordinateFileExtension();
   }
   else {
      //
      // Parse the file name
      //
      QString directory, species, casename, anatomy, hemisphere, description, descriptionNoType;
      QString theDate, numNodes, extension;
      if (FileUtilities::parseCaretDataFileName(morphFileName,
                                                directory,
                                                species,
                                                casename,
                                                anatomy,
                                                hemisphere,
                                                description,
                                                descriptionNoType,
                                                theDate,
                                                numNodes,
                                                extension)) {
                                    
         if (species.isEmpty() == false) {
            outputFileNamePrefix.append(species);
            outputFileNamePrefix.append(".");
         }
         if (casename.isEmpty() == false) {
            outputFileNamePrefix.append(casename);
            outputFileNamePrefix.append(".");
         }
         if (anatomy.isEmpty() == false) {
            outputFileNamePrefix.append(anatomy);
            outputFileNamePrefix.append(".");
         }
         if (hemisphere.isEmpty() == false) {
            outputFileNamePrefix.append(hemisphere);
            outputFileNamePrefix.append(".");
         }
         if (descriptionNoType.isEmpty() == false) {
            outputFileNamePrefix.append(descriptionNoType);
            outputFileNamePrefix.append(".");
         }

         if (theDate.isEmpty() == false) {
            outputFileNameSuffix.append(theDate);
            outputFileNameSuffix.append(".");
         }
         if (numNodes.isEmpty() == false) {
            outputFileNameSuffix.append(numNodes);
         }
         // extension has period at beginning     if (outputFileNameSuffix.isEmpty()) {
         //   outputFileNameSuffix.append(".");
         //}
         outputFileNameSuffix.append(SpecFile::getCoordinateFileExtension());
      }
      else {
         outputFileNamePrefix = FileUtilities::filenameWithoutExtension(morphFileName);
         if (StringUtilities::endsWith(outputFileNamePrefix, ".") == false) {
            outputFileNamePrefix.append(".");
         }
         outputFileNameSuffix = FileUtilities::filenameExtension(morphFileName);
      }
   }
   
   if (outputFileNameSuffix.startsWith('.') == false) {
      outputFileNameSuffix.insert(0, '.');
   }
}

/**
 * run the multiresolution morphing
 */
void 
BrainModelSurfaceMultiresolutionMorphing::execute() throw (BrainModelAlgorithmException)
{
   QString morphTypeString;
   switch (morphingSurfaceType) {
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
         morphTypeString = "Flat ";
         break;
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
         morphTypeString = "Spherical ";
         break;
   }
   std::ostringstream str;
   str << "\n"
       << "Multiresolution Morphing "
       << morphTypeString.toAscii().constData()
       << ": "
       << " reference surface="
       << FileUtilities::basename(referenceSurface->getFileName()).toAscii().constData()
       << "\n"
       << " morphing surface="
       << FileUtilities::basename(morphingSurface->getFileName()).toAscii().constData()
       << "\n";
   for (int j = 0; j < numberOfCycles; j++) {
      str << "cycle "
          << j
          << ":"
          << " iterations=";
      for (int i = 0; i < MAXIMUM_NUMBER_OF_LEVELS; i++) {    
         str << iterationsPerLevel[j][i] << " ";
      }
      str << "\n"
         << " linear force=" << linearForce[j]
         << " angular force=" << angularForce[j]
         << " step size=" << stepSize[j]
         << "\n"
         << "smoothing strength=" << smoothingStrength[j]
         << " iterations=" << smoothingIterations[j]
         << " edge iterations=" << smoothingEdgeIterations[j]
         << "\n";
   }
   const QString fileComment(str.str().c_str());
   
   //
   // Surfaces that will be scaled after the process is finished
   //
   std::vector<BrainModelSurface*> surfacesToScale;
   
   //
   // Get modified status of existing brain models
   //
   std::vector<unsigned long> brainModelModified;
   for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      if (bms != NULL) {
         CoordinateFile* cf = bms->getCoordinateFile();
         brainModelModified.push_back(cf->getModified());
      }
      else {
         brainModelModified.push_back(0);
      }
   }

   //
   // Check for stragglers
   //   
   std::vector<int> cornerTiles;
   morphingSurface->getTopologyFile()->findCornerTiles(2, cornerTiles);
   if (cornerTiles.empty() == false) {
      throw BrainModelAlgorithmException(
         "Straggler tile(s) were found (tiles that have two nodes that are\n"
         "only used by a single tile).  These tiles will cause problems\n"
         "with Multi-resolution Morphing.  Use the Surface:Topology:\n"
         "Remove Corner and Straggler Tiles menu item with \"Delete Stragglers\n"
         "Only\" to eliminate them.  After doing so, save both the Topology \n"
         "and Coordinate files.");
   }
   
   //
   // Create the progress dialog
   //
   QString title("Flat Multiresolution Morphing");
   if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
      title = "Spherical Multiresolution Morphing";
   }
   createProgressDialog(title,
                        numberOfCycles + 1,
                        "multiresMorphProgressDialog");
   
   try {
      //
      // Setup filename prefix and suffix for output file naming
      //
      setUpOutputFileNaming();
      
      //
      // Save the original topology file for spherical morphing since it might get modified
      //
      TopologyFile* originalTopologyFile = NULL;
      TopologyFile* newTopologyFile = NULL;
      switch (morphingSurfaceType) {
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
            break;
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
            if (pointSphericalTilesOutward) {
               originalTopologyFile = morphingSurface->getTopologyFile();
               newTopologyFile = new TopologyFile(*originalTopologyFile);
               morphingSurface->setTopologyFile(newTopologyFile);
            }
            break;
      }
      
      //
      // Use the morphing surface's topology file
      //
      TopologyFile* topologyFile = morphingSurface->getTopologyFile();
      
      //
      // Classify the nodes using the morphing surface's topology file
      //
      
      brainSet->clearNodeAttributes();
      brainSet->classifyNodes(topologyFile);
      
      const int numTiles = topologyFile->getNumberOfTiles();
      
      std::vector<int> dummy1, dummy2, dummy3;
      const int numPiecesOfSurface = topologyFile->findIslands(dummy1, dummy2, dummy3);
      if (numPiecesOfSurface > 1) {
         throw BrainModelAlgorithmException(
            "There are multiple pieces of surface.  Use Surface: Topology: Remove Islands\n"
            "to remove them and verify that the surface remains correct.");
      }
      
      //
      // Set the references surface area
      //
      const float referenceSurfaceArea = referenceSurface->getSurfaceArea(topologyFile);
      
      switch (morphingSurfaceType) {
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
            break;
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
            //
            // Scale surface of sphere 02/26/2004
            //
            morphingSurface->convertToSphereWithSurfaceArea(referenceSurfaceArea);
            break;
      }
      //
      // Get the radius of the spherical morphing surface
      //
      const float sphericalRadius = morphingSurface->getSphericalSurfaceRadius();
                  
      
      //
      // Measure the surface
      //
      measureSurface(-1, 0.0);
      
      //
      // Get the hemisphere being morphed
      //
      brainStruct = brainSet->getStructure().getType();
         
      if (DebugControl::getDebugOn()) {
         try {
            BrainModelSurface tempr(*referenceSurface);
            tempr.getCoordinateFile()->writeFile("debug_morph_input_refererence.coord");
            
            BrainModelSurface tempm(*morphingSurface);
            tempm.getCoordinateFile()->writeFile("debug_morph_input_morphing.coord");
            
            TopologyFile tempt(*topologyFile);
            tempt.writeFile("debug_morph_input_topology.topo");
            
            SpecFile sp;
            sp.addToSpecFile(SpecFile::getFiducialCoordFileTag(), 
                             tempr.getCoordinateFile()->getFileName(), "", false);
            
            switch (morphingSurfaceType) {
               case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
                  sp.addToSpecFile(SpecFile::getFlatCoordFileTag(),
                                   tempm.getCoordinateFile()->getFileName(), "", false);
                  sp.addToSpecFile(SpecFile::getCutTopoFileTag(),
                                   tempt.getFileName(), "", false);
                  break;
               case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
                  sp.addToSpecFile(SpecFile::getSphericalCoordFileTag(), 
                                   tempm.getCoordinateFile()->getFileName(), "", false);
                  sp.addToSpecFile(SpecFile::getClosedTopoFileTag(),
                                   tempt.getFileName(), "", false);
                  break;
            }
            sp.writeFile("debug_morph_input.spec");
         }
         catch (FileException&) {
         }
      }
      
      //
      // Loop number of cycles
      //
      for (currentCycle = 0; currentCycle < numberOfCycles; currentCycle++) {
         const bool lastCycleFlag = (currentCycle == (numberOfCycles - 1));
   
         //
         // Set progress dialog
         //
         {
            std::ostringstream str;
            QString typeString("Flat");
            if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
               typeString = "Spherical";
            }
            str << "Running "
               << typeString.toAscii().constData()
               << " Cycle "
               << (currentCycle + 1)
               << " of "
               << numberOfCycles;
            updateProgressDialog(str.str().c_str(), currentCycle + 1);
         }
         
         //
         // Start a timer
         // 
         QTime timer;
         timer.start();
         
         //
         // set the prefix for naming intermediate files and spec file naming
         //
         QString morphTypeString("none");
         switch (morphingSurfaceType) {
            case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
               morphTypeString = "flatmorph";
               break;
            case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
               morphTypeString = "spheremorph";
               break;
         }
         for (int i = 0; i < numberOfLevels; i++) {
            std::ostringstream ostr;
            ostr << morphTypeString.toAscii().constData()
               << ".cycle"
               << (currentCycle + 1)
               << ".level"
               << (i + 1);
            intermediateFileNamePrefix[i] = ostr.str().c_str();
            intermediateCoordFileNamePrefix[i] = intermediateFileNamePrefix[i];
            switch(morphingSurfaceType) {
               case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
                  intermediateCoordFileNamePrefix[i].append(".flat");
                  break;
               case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
                  intermediateCoordFileNamePrefix[i].append(".sphere");
                  break;
            }
            intermediateSpecFileNames[i] = intermediateFileNamePrefix[i];
            intermediateSpecFileNames[i].append(".spec");
            intermediateFiles.push_back(intermediateSpecFileNames[i]);
         }
         
         std::vector<BrainSet*> brains;
         brains.push_back(brainSet);  // one passed to this objects constructor
         
         switch (morphingSurfaceType) {
            case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
               {
                  //
                  // Set resampling number of tiles
                  //
                  const int numberOfResamplingTiles = static_cast<int>(numTiles * 0.3);
               
                  //
                  //  Create a flat hexagonal subsampled surface
                  //
                  BrainModelSurfaceFlatHexagonalSubsample hexSubSample(brainSet,
                                                                     referenceSurface,
                                                                     morphingSurface,
                                                                     numberOfResamplingTiles);
                  hexSubSample.execute();
                  BrainSet* subSampledBrainSet = hexSubSample.getSubsampledBrainSet();
                  if (subSampledBrainSet != NULL) {
                     //
                     // Create the non-morphed surface
                     //
                     BrainModelSurface* bms = subSampledBrainSet->getBrainModelSurface(1);
                     subSampledBrainSet->setStructure(brainStruct);
                     subSampledBrainSet->addBrainModel(new BrainModelSurface(*bms));
                  }
                  
                  if (subSampledBrainSet == NULL) {
                     throw BrainModelAlgorithmException("Failed to create equilateral grid surface");
                  }
                  
                  brains.push_back(subSampledBrainSet);
                  
                  //
                  // multiresolution downsample the surface
                  //
                  multiresolutionDownsample(brains);
                  
                  //
                  // Write the multiresolution surfaces 
                  //
                  writeMultiresolutionSurfaces(brains);
                  
                  //
                  // Morph the surfaces
                  //
                  multiresolutionMorph(brains);
                  
                  //
                  // smooth to eliminate crossovers
                  //
                  smoothOutCrossovers(morphingSurface, 0.0);
                  
                  //
                  // Update surface normals
                  //
                  morphingSurface->computeNormals();
                  
                  //
                  // Translate to center of mass and scale to fit the window
                  //
                  morphingSurface->translateToCenterOfMass();
                  morphingSurface->updateForDefaultScaling();
                  
               }
               break;
               
            case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
               {
                  //
                  // Create the multiresolution spheres and mappings between them
                  //
                  constructTemplateSpheres(brains);
                  createSphereDownsampleMapping(brains);
                  
                  //
                  // Write the multiresolution surfaces 
                  //
                  writeMultiresolutionSurfaces(brains);
                  
                  //
                  // Morph the surfaces
                  //
                  multiresolutionMorph(brains);
                  
                  //
                  // smooth to eliminate crossovers
                  //
                  smoothOutCrossovers(morphingSurface, sphericalRadius);
                  
                  //
                  // Update surface normals
                  //
                  morphingSurface->computeNormals();
               }
               break;
         }  // switch(morphingSurfaceType)
         
         //
         // Free memory of multiresolution surfaces.  Note that index 0 is
         // the brain passed to this objects constructor so do not delete it.
         //
         for (int i = 1; i < static_cast<int>(brains.size()); i++) {
            delete brains[i];
         }
         
         //
         // if flat surface 
         //
         if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
            //
            // Save the coordinates
            //
            morphingSurface->pushCoordinates();
            
            //
            // scale to match reference surface area
            //
            morphingSurface->scaleSurfaceToArea(referenceSurfaceArea, true);
            
            //
            // Add to surfaces that should be scaled
            //
            surfacesToScale.push_back(morphingSurface);
         }
         
         //
         // If spherical surface
         //
         if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
            morphingSurface->convertToSphereWithRadius(sphericalRadius);
         }
         
         //
         // Measure the surface
         //
         measureSurface(currentCycle, (timer.elapsed() * 0.001));
         
         if (currentCycle == 0) {
            CoordinateFile* cf = morphingSurface->getCoordinateFile();
            cf->appendToFileComment(fileComment);
            cf->appendSoftwareVersionToFileComment("Multiresolution Morphing with");
         }
         
         //
         // Write the morphed coordinate file (but do not add to spec file just yet)
         //
         QString cycleType("FLAT_CYCLE");
         if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
            cycleType = "SPHERE_CYCLE";
         }
         std::ostringstream str;
         str << outputFileNamePrefix.toAscii().constData()
            << cycleType.toAscii().constData()
            << (currentCycle + 1)
            //<< "."
            << outputFileNameSuffix.toAscii().constData()
            << std::ends;
         if (autoSaveFilesFlag) {
            try {
               CoordinateFile* cf = morphingSurface->getCoordinateFile();
               brainSet->writeCoordinateFile(str.str().c_str(), morphingSurface->getSurfaceType(), cf, true);    
            }
            catch (FileException& e) {
               throw BrainModelAlgorithmException(e.whatQString());
            }
         }
         else {
            CoordinateFile* cf = morphingSurface->getCoordinateFile();
            cf->setFileName(str.str().c_str());
         }
         
         // if flat surface 
         //
         if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
            //
            // Restore the coordinates
            //
            morphingSurface->popCoordinates();
         }
                  
         //
         // If doing flat registration and smoothing of flat surface overlap 
         // enabled and doing the last cycle
         //
         if ((morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) &&
            smoothOutFlatSurfaceOverlap &&
            lastCycleFlag) {
            
            //
            // Copy the morphing surface
            //
            BrainModelSurface* smoothingSurface = new BrainModelSurface(*morphingSurface);
            
            //
            // If there was smoothing of overlapped areas
            //
            if (smoothingSurface->smoothOutFlatSurfaceOverlap()) {
               //
               // Save the coordinates
               //
               smoothingSurface->pushCoordinates();
               
               //
               // scale to match reference surface area
               //
               smoothingSurface->scaleSurfaceToArea(referenceSurfaceArea, true);

               //
               // Add to surfaces that should be scaled
               //
               surfacesToScale.push_back(smoothingSurface);
               
               //
               // Write the smoothed out overlap surface
               //
               morphingSurface = smoothingSurface;
               brainSet->addBrainModel(morphingSurface);
               CoordinateFile* cf = morphingSurface->getCoordinateFile();
               std::ostringstream str;
               str << outputFileNamePrefix.toAscii().constData()
                  << "FLAT_CYCLE"
                  << (currentCycle + 1)
                  << "_OVERLAP_SMOOTH"
                  //<< "."
                  << outputFileNameSuffix.toAscii().constData()
                  << std::ends;
               if (autoSaveFilesFlag) {
                  try {
                     brainSet->writeCoordinateFile(str.str().c_str(), morphingSurface->getSurfaceType(), cf, false);    
                  }
                  catch (FileException& e) {
                     throw BrainModelAlgorithmException(e.whatQString());
                  }
               }
               else {
                  cf->setFileName(str.str().c_str());
               }
               
               //
               // Measure the overlap smoothed surface
               //
               measureSurface(1000, (timer.elapsed() * 0.001));
               
               //
               // if flat surface 
               //
               if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
                  //
                  // Restore the coordinates
                  //
                  smoothingSurface->popCoordinates();
               }
            }
            else {
               delete smoothingSurface;
            }
         }
         
         //
         // if this is the last cycle
         //
         if (lastCycleFlag) {
            //
            // Add the coordinate file to the spec file
            //
            if (autoSaveFilesFlag) {
               brainSet->addToSpecFile(
                  BrainModelSurface::getCoordSpecFileTagFromSurfaceType(morphingSurface->getSurfaceType()),
                  morphingSurface->getFileName());
            }
               
            //
            // If surface should be aligned
            //
            if (centralSulcusBorderProjection != NULL) {
               if (centralSulcusBorderProjection->getNumberOfLinks() > 1) {
                  const bool flatFlag =
                     (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT);
                  BrainModelSurface* alignmentSurface = new BrainModelSurface(*morphingSurface);
                  alignmentSurface->alignToStandardOrientation(referenceSurface,
                                                               centralSulcusBorderProjection,
                                                               false,
                                                               false);
                  brainSet->addBrainModel(alignmentSurface);
                  
                  if (flatFlag) {
                     alignmentSurface->scaleSurfaceToArea(referenceSurfaceArea, true);
                  }

                  const QString alignmentComment =
                     "\nAligned to Standard Orientation using "
                     + centralSulcusBorderProjection->getName()
                     + " from "
                     + brainSet->getBorderSet()->getBorderProjectionFileInfo()->getFileName()
                     + ".\n";
                  CoordinateFile* cf = alignmentSurface->getCoordinateFile();
                  cf->appendToFileComment(alignmentComment);
                  
                  const QString name(
                       outputFileNamePrefix
                     + (flatFlag
                        ? "FLAT_ALIGNED"
                        : "SPHERE_ALIGNED")
                     + outputFileNameSuffix);
                  if (autoSaveFilesFlag) {
                     try {
                        brainSet->writeCoordinateFile(name, alignmentSurface->getSurfaceType(), cf, true);    
                     }
                     catch (FileException& e) {
                        throw BrainModelAlgorithmException(e.whatQString());
                     }
                  }
                  else {
                     cf->setFileName(name);
                  }
                  
                  morphingSurface = alignmentSurface;
                  
                  //
                  // Measure the overlap smoothed surface
                  //
                  measureSurface(2000, (timer.elapsed() * 0.001));
               }
            }
         }
         
         //
         // Update the displayed brain model
         //
         brainSet->drawBrainModel(morphingSurface);
      }
      
      //
      // If original topology file was saved, restore it to the surfaces.
      // This is used in spherical morphing since the topology may be modified
      //
      if ((originalTopologyFile != NULL) && (newTopologyFile != NULL)) {
         //
         // Restore the topology file
         //
         const int numBrainModels = brainSet->getNumberOfBrainModels();
         for (int i = 0; i < numBrainModels; i++) {
            BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
            if (bms != NULL) {
               if (bms->getTopologyFile() == newTopologyFile) {
                  bms->setTopologyFile(originalTopologyFile);
               }
            }
         }
         delete newTopologyFile;
      }
         
      //
      // Delete intermediate files is user wants them deleted
      //
      if (deleteIntermediateFiles) {
         for (int i = 0; i < static_cast<int>(intermediateFiles.size()); i++) {
            QFile::remove(intermediateFiles[i]);
         }
      }
      
      //
      // Retain modified status of existing brain models
      //
      for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
         BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
         if (bms != NULL) {
            if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
               if (std::find(surfacesToScale.begin(), surfacesToScale.end(), bms) 
                  != surfacesToScale.end()) {
                  bms->scaleSurfaceToArea(referenceSurfaceArea, true);
                  
                  if (autoSaveFilesFlag) {
                     CoordinateFile* cf = bms->getCoordinateFile();
                     brainSet->writeCoordinateFile(cf->getFileName(),
                                                   bms->getSurfaceType(),
                                                   cf);
                  }
               }
            }  
            CoordinateFile* cf = bms->getCoordinateFile();
            if (i < static_cast<int>(brainModelModified.size())) {
                  cf->setModifiedCounter(brainModelModified[i]);
            }
            //else {
            //      cf->clearModified();
            //}
         }
      }
   }
   catch (BrainModelAlgorithmException& e) {
      removeProgressDialog();
      throw e;
   }
   
   removeProgressDialog();
}

/**
 * Make measurements of surface
 */
void
BrainModelSurfaceMultiresolutionMorphing::measureSurface(const int cycleNumber, 
                                                         const float elapsedTime)
{
   //
   // Set the names of the distortion measurements
   //
   QString arealDistortName("Areal Distortion ");
   QString linearDistortName("Linear Distortion ");
   QString cycleName;
   if (cycleNumber == -1) {
      arealDistortName.append("Before Morphing");
      linearDistortName.append("Before Morphing");
      cycleName = "Before Morphing";
   }
   else {
      std::ostringstream str;
      if (cycleNumber == 2000) {
         str << "Aligned";
      }
      else if (cycleNumber == 1000) {
         str << "Overlap Smoothed";
      }
      else {
         str << " Cycle "
             << (cycleNumber + 1);
      }
      arealDistortName.append(str.str().c_str());
      linearDistortName.append(str.str().c_str());
      cycleName = str.str().c_str();
   }
   
   //
   // Make the distortion measurements
   //
   BrainModelSurfaceDistortion bmsd(brainSet, morphingSurface, referenceSurface,
                                    morphingSurface->getTopologyFile(), 
                                    &shapeMeasurementsFile,
                                    BrainModelSurfaceDistortion::DISTORTION_COLUMN_CREATE_NEW,
                                    BrainModelSurfaceDistortion::DISTORTION_COLUMN_CREATE_NEW,
                                    arealDistortName,
                                    linearDistortName);
   bmsd.execute();
   
   //
   // Save the distortion measurements file.
   //
   QString filename;
   switch(morphingSurfaceType) {
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
         filename = "flat_morph_distortion";
         break;
      case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
         filename = "spherical_morph_distortion";
         break;
   }
   if (filename.isEmpty() == false) {
      filename.append(SpecFile::getSurfaceShapeFileExtension());
      try {
         shapeMeasurementsFile.writeFile(filename);
      }
      catch (FileException& e) {
         throw BrainModelAlgorithmException(e.whatQString());
      }
      if (cycleNumber < 0) {
         //brainSet->addToSpecFile(SpecFile::surfaceShapeFileTag, filename);
      }
   }
   
   //
   // Get columns in surface shape file
   //
   const int arealDistortColumn = shapeMeasurementsFile.getColumnWithName(arealDistortName);
   const int linearDistortColumn = shapeMeasurementsFile.getColumnWithName(linearDistortName);
   if (arealDistortColumn < 0) {
      std::cout << "PROGRAM ERROR: invalid areal distortion column at "
                << __LINE__ << " in " << __FILE__ << std::endl;
      return;
   }
   if (linearDistortColumn < 0) {
      std::cout << "PROGRAM ERROR: invalid linear distortion column at "
                << __LINE__ << " in " << __FILE__ << std::endl;
      return;
   }
   
   const int numNodes = morphingSurface->getNumberOfNodes();
   std::vector<float> linearDistortion(numNodes);
   std::vector<float> arealDistortion(numNodes);
   for (int i = 0; i < numNodes; i++) {
      arealDistortion[i] = shapeMeasurementsFile.getValue(i, arealDistortColumn);
      linearDistortion[i] = shapeMeasurementsFile.getValue(i, linearDistortColumn);
   }
   
   //
   // Statistics for areal distortion
   //
   StatisticsUtilities::DescriptiveStatistics arealDistortionStats;
   StatisticsUtilities::computeStatistics(arealDistortion,
                                                     true,
                                                     arealDistortionStats);
/*
   StatisticDataGroup arealGroup(&arealDistortion,
                                 StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticsUtilities arealDS(StatisticDescriptiveStatistics::DATA_TYPE_SAMPLE);
   arealDS.addDataGroup(&arealGroup);
   try {
      arealDS.execute();
   }
   catch (StatisticException&) {
   }
   const StatisticDescriptiveStatistics::DescriptiveStatistics
                                arealDistortionStats = arealDS.getDescriptiveStatistics();
*/
   //
   // Statistics for linear distortion
   //
   StatisticsUtilities::DescriptiveStatistics linearDistortionStats;
   StatisticsUtilities::computeStatistics(linearDistortion,
                                                     true,
                                                     linearDistortionStats);
/*
   StatisticDataGroup linearGroup(&linearDistortion,
                                 StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDescriptiveStatistics linearDS(StatisticDescriptiveStatistics::DATA_TYPE_SAMPLE);
   linearDS.addDataGroup(&linearGroup);
   try {
      linearDS.execute();
   }
   catch (StatisticException&) {
   }
   const StatisticDescriptiveStatistics::DescriptiveStatistics
                                linearDistortionStats = linearDS.getDescriptiveStatistics();
*/

   int numTileCrossovers, numNodeCrossovers; 
   morphingSurface->crossoverCheck(numTileCrossovers, numNodeCrossovers, brainModelSurfaceType);
   
   MorphingMeasurements mm(cycleName,
                           arealDistortionStats,
                           linearDistortionStats,
                           numNodeCrossovers, numTileCrossovers, elapsedTime);
   measurements.push_back(mm);
}

/**
 * Smooth to eliminate crossovers.
 */
void
BrainModelSurfaceMultiresolutionMorphing::smoothOutCrossovers(BrainModelSurface* bms,
                                                              const float sphereRadius)
{
   if (DebugControl::getDebugOn()) {
      try {
         BrainModelSurface temp(*bms);
         temp.getCoordinateFile()->writeFile("debug_morph_before_any_smoothing.coord");
      } catch (FileException&) {
      }
   }
   
   int numNodeCrossovers = 10;
   
   int iterCount = 0;
   while ((numNodeCrossovers > 2) && (iterCount < smoothingIterations[currentCycle])) {
      int numIters = 10;
      if ((smoothingIterations[currentCycle] - iterCount) < numIters) {
         numIters = (smoothingIterations[currentCycle] - iterCount);
      }
      if (numIters > 0) {
         bms->arealSmoothing(smoothingStrength[currentCycle], numIters, 
                             smoothingEdgeIterations[currentCycle]);
         iterCount += numIters; 
      }
      else {
         break;
      }
      
      //
      // Push nodes on spherical surface back to the sphere
      //
      if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
         bms->convertToSphereWithRadius(sphereRadius);
         
         //
         // If normals should be pointed outward
         //
         if (pointSphericalTilesOutward) {
            bms->orientTilesOutward(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
         }
      }
      
      int numTileCrossovers;
      bms->crossoverCheck(numTileCrossovers, numNodeCrossovers, brainModelSurfaceType);
   }

   if (DebugControl::getDebugOn()) {
      try {
         BrainModelSurface temp2(*bms);
         temp2.getCoordinateFile()->writeFile("debug_morph_after_general_smoothing.coord");
      } catch (FileException&) {
      }
   }
   
   
   if (crossoverSmoothAtEndOfEachCycle) {
      //
      // Set project back to sphere if sphere
      //
      int projToSphereEveryIter = -1;
      switch (morphingSurfaceType) {
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
            projToSphereEveryIter = -1;
            break;
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
            projToSphereEveryIter = 5;
            break;
      }
      
      //
      // Smooth out crossovers
      //
//      bms->smoothOutSurfaceCrossovers(1.0, 10, 50, 10, projToSphereEveryIter, 3);
      bms->smoothOutSurfaceCrossovers(crossoverSmoothStrength,
                                      crossoverSmoothCycles,
                                      crossoverSmoothIterations,
                                      crossoverSmoothEdgeIterations,
                                      crossoverSmoothProjectToSphereIterations,
                                      crossoverSmoothNeighborDepth,
                                      brainModelSurfaceType);
      
      //  
      // Project back to sphere
      //
      switch (morphingSurfaceType) {
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
            break;
         case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
            bms->convertToSphereWithRadius(sphereRadius);
            break;
      }
      int numTileCrossovers;
      bms->crossoverCheck(numTileCrossovers, numNodeCrossovers, brainModelSurfaceType);
   }
   
   if (DebugControl::getDebugOn()) {
      try {
         BrainModelSurface temp3(*bms);
         temp3.getCoordinateFile()->writeFile("debug_morph_crossover_smoothing.coord");
      } catch (FileException&) {
      }
   }
   
   if (DebugControl::getDebugOn()) {
      if (numNodeCrossovers > 0) {
         std::cout << "At end of smoothing there are " << numNodeCrossovers
                   << " node crossovers." << std::endl;
      }
   } 
}

/**
 * Write multiresolution surfaces.  Note that spec file is written automatically any
 * time a data file is saved.
 */
void
BrainModelSurfaceMultiresolutionMorphing::writeMultiresolutionSurfaces(std::vector<BrainSet*>& brains)
{
   //
   // surface with index 0 is surface being morphed and do not need to write it
   //
   const int numBrainSets = static_cast<int>(brains.size());
   for (int i = (numBrainSets - 1); i > 0; i--) {
      BrainSet* bs = brains[i];
      
      bs->setSpecFileName(intermediateSpecFileNames[i]);
      
      try {
         //
         // Write the topology file
         //
         QString topoName(intermediateFileNamePrefix[i]);
         topoName.append(SpecFile::getTopoFileExtension());
         TopologyFile* tf = bs->getTopologyFile(0);
         try {
            bs->writeTopologyFile(topoName, tf->getTopologyType(), tf);
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e.whatQString());
         }
         intermediateFiles.push_back(topoName);
         
         //
         // Write the fidicual coordinate file
         //
         QString fiducialName(intermediateFileNamePrefix[i]);
         fiducialName.append(".fiducial");
         fiducialName.append(SpecFile::getCoordinateFileExtension());
         BrainModelSurface* bms = bs->getBrainModelSurface(SURFACE_FIDUCIAL_INDEX);
         CoordinateFile* cf = bms->getCoordinateFile();
         try {
            bs->writeCoordinateFile(fiducialName, bms->getSurfaceType(), cf);
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e.whatQString());
         }
         intermediateFiles.push_back(fiducialName);
         
         //
         // Write the flat/spherical coordinate file
         //
         QString flatName(intermediateCoordFileNamePrefix[i]);
         flatName.append(SpecFile::getCoordinateFileExtension());
         bms = bs->getBrainModelSurface(SURFACE_MORPHED_INDEX);
         cf = bms->getCoordinateFile();
         try {
            bs->writeCoordinateFile(flatName, bms->getSurfaceType(), cf);
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e.whatQString());
         }
         intermediateFiles.push_back(flatName);

         //
         // Convert the surface to a border file to facilitate overlaying surface
         // on the original high resolution surface
         //
         {
            QString borderFileName(intermediateCoordFileNamePrefix[i]);
            borderFileName.append(SpecFile::getBorderFileExtension());
            BorderFile bf(bms->getTopologyFile(), bms->getCoordinateFile());
            switch(morphingSurfaceType) {
               case BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT:
                  bf.setHeaderTag(AbstractFile::headerTagConfigurationID,
                                  SpecFile::getFlatBorderFileTagName());
                  bs->addToSpecFile(SpecFile::getFlatBorderFileTag(), borderFileName);
                  break;
               case BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL:
                  bf.setHeaderTag(AbstractFile::headerTagConfigurationID,
                                  SpecFile::getSphericalBorderFileTagName());
                  bs->addToSpecFile(SpecFile::getSphericalBorderFileTag(), borderFileName);
                  break;
            }
            bf.writeFile(borderFileName);
            intermediateFiles.push_back(borderFileName);
         }
      }
      catch (FileException& e) {
         std::cerr << "File Write Error: " << e.whatQString().toAscii().constData() << std::endl;
      }
   }
}

/**
 * Create downsampled multiresolution surface(s).
 */
void
BrainModelSurfaceMultiresolutionMorphing::multiresolutionDownsample(
                                               std::vector<BrainSet*>& brains)
{
   int numNodes = 1000;
   
   //
   // create subsampled surfaces until number of nodes is less than or equal to 100
   //
   while (numNodes > 100) {
      const int brainIndex = brains.size() - 1;
      const int prevNumNodes = brains[brainIndex]->getNumberOfNodes();
      BrainSet* bs = downsampleEquilateralGridSurface(brains[brainIndex]);
      if (bs != NULL) {
         brains.push_back(bs);
         numNodes = bs->getNumberOfNodes();
         
         if (DebugControl::getDebugOn()) {
            std::cout << "Downsampled surface " << brainIndex << " to " << (brainIndex + 1)
                      << " nodes reduced from " << prevNumNodes << " to " 
                      << numNodes << std::endl;
         }   
      }
      else {
         throw BrainModelAlgorithmException("Failed to downsample surface");
      }
      
      //
      // limit to maximum number of levels
      //
      if (brains.size() == MAXIMUM_NUMBER_OF_LEVELS) {
         break;
      }
   }
}

/**
 * Downsample an equilateral grid surface by selecting every other node in each row and column.
 */
BrainSet*
BrainModelSurfaceMultiresolutionMorphing::downsampleEquilateralGridSurface(BrainSet* brainIn)
{
   //
   // Surfaces and coordinate files of input brain
   //
   BrainModelSurface* fiducialSurfaceIn = brainIn->getBrainModelSurface(SURFACE_FIDUCIAL_INDEX);
   CoordinateFile* fiducialCoordIn = fiducialSurfaceIn->getCoordinateFile();
   BrainModelSurface* flatSurfaceIn = brainIn->getBrainModelSurface(SURFACE_MORPHED_INDEX);
   CoordinateFile* flatCoordIn = flatSurfaceIn->getCoordinateFile();
   
   //
   // Output brain set
   //
   BrainSet* brainOut = new BrainSet;
   brainOut->setStructure(brainStruct);
   
   //
   // Output brain surfaces (fiducial, flat, and a flat that does not get modified)
   //
   BrainModelSurface* fiducialSurfaceOut = new BrainModelSurface(brainOut);
   fiducialSurfaceOut->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   fiducialSurfaceOut->setStructure(brainStruct);
   BrainModelSurface* flatSurfaceOut = new BrainModelSurface(brainOut);
   flatSurfaceOut->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FLAT);
   fiducialSurfaceOut->setStructure(brainStruct);
   BrainModelSurface* flatNotMorphSurfaceOut = new BrainModelSurface(brainOut);
   flatNotMorphSurfaceOut->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FLAT);
   flatNotMorphSurfaceOut->setStructure(brainStruct);
   
   //
   // Add brain surfaces to brain
   //
   brainOut->addBrainModel(fiducialSurfaceOut);
   brainOut->addBrainModel(flatSurfaceOut);
   brainOut->addBrainModel(flatNotMorphSurfaceOut);
   
   //
   // Loop through node of input flat surface for subsampling
   //
   const int numNodes = flatSurfaceIn->getNumberOfNodes();
   for (int nm = 0; nm < numNodes; nm++) {
      const BrainSetNodeAttribute* bna = brainIn->getNodeAttributes(nm);
      int row, column, node;
      bna->getFlatMorphAttributes(row, column, node);
      
      //
      // Use node in every other row and column
      //
      if ( ((row % 2) == 0) && ((column %2) == 0) ) {
         flatSurfaceOut->addNode(flatCoordIn->getCoordinate(nm));
         flatNotMorphSurfaceOut->addNode(flatCoordIn->getCoordinate(nm));
         fiducialSurfaceOut->addNode(fiducialCoordIn->getCoordinate(nm));
         brainOut->resetNodeAttributes();
         BrainSetNodeAttribute* bna = brainOut->getNodeAttributes(flatSurfaceOut->getNumberOfNodes() - 1);
         bna->setFlatMorphAttributes((row/2), (column/2), nm);
      }
   }
   
   //
   // Unable to subsample ?
   //
   if (flatSurfaceOut->getNumberOfNodes() <= 0) {
      delete fiducialSurfaceOut;
      delete flatSurfaceOut;
      delete flatNotMorphSurfaceOut;
      return NULL;
   }
   
   //
   // Create the topology 
   //
   TopologyFile* topology = createEquilateralGridTopology(brainOut, flatSurfaceOut);
   if (topology != NULL) {
      fiducialSurfaceOut->setTopologyFile(topology);
      flatSurfaceOut->setTopologyFile(topology);
      flatNotMorphSurfaceOut->setTopologyFile(topology);
      brainOut->addTopologyFile(topology);
   }
   
   return brainOut;
}

/**
 * Create the topology for an equilateral grid.
 */
TopologyFile*
BrainModelSurfaceMultiresolutionMorphing::createEquilateralGridTopology(BrainSet* brain,
                                                                       BrainModelSurface* surface)
{
   const int numNodes = surface->getNumberOfNodes();
   TopologyFile *topology = new TopologyFile();
   topology->setTopologyType(TopologyFile::TOPOLOGY_TYPE_CUT);
   
   for (int i = 0; i < numNodes; i++) {
      BrainSetNodeAttribute* bna = brain->getNodeAttributes(i);
      int row, col, node;
      bna->getFlatMorphAttributes(row, col, node);
      const int n1 = brain->getNodeWithMorphRowColumn(row, col + 1, i);
      const int n2 = brain->getNodeWithMorphRowColumn(row + 1, col, i);
      const int n3 = brain->getNodeWithMorphRowColumn(row + 1, col - 1, i);
      
      if ((n1 >= 0) && (n2 >= 0)) {
         topology->addTile(i, n1, n2);
      }
      if ((n2 >= 0) && (n3 >= 0)) {
         topology->addTile(i, n2, n3);
      }
   }
   
   if (topology->getNumberOfTiles() <= 0) {
      delete topology;
      throw BrainModelAlgorithmException("Failed to create topology");
   }
   
   return topology;
}

/**
 * Peform the multi-resolution morphing
 */
void
BrainModelSurfaceMultiresolutionMorphing::multiresolutionMorph(std::vector<BrainSet*>& brains)
{
   const int numBrains = static_cast<int>(brains.size());
   
   //
   // Limit to available levels
   //
   int startNum = numBrains - 1;
   if (startNum >= MAXIMUM_NUMBER_OF_LEVELS) {
      startNum = MAXIMUM_NUMBER_OF_LEVELS - 1;
   }
   
   //
   // Start with the lowest resolution surface
   //
   for (int bi = startNum; bi > 0; bi--) {
      //
      // Get the flat and fiducial surfaces
      //
      BrainSet* bs = brains[bi];
      BrainModelSurface* fiducialSurface = bs->getBrainModelSurface(SURFACE_FIDUCIAL_INDEX);
      BrainModelSurface* flatSurface = bs->getBrainModelSurface(SURFACE_MORPHED_INDEX);
      
      //
      // Update normals on the flat and fiducial surfaces
      //
      fiducialSurface->computeNormals();
      flatSurface->computeNormals();
      
      if (DebugControl::getDebugOn()) {
         std::cout << std::endl << "*** Morphing Level surface " << bi
                   << ": nodes " << bs->getNumberOfNodes() 
                   << ", iterations " << iterationsPerLevel[currentCycle][bi] << std::endl;
      }
    
      //
      // Morph the surface for the specified iterations
      //
      BrainModelSurfaceMorphing bsm(bs, fiducialSurface, flatSurface, morphingSurfaceType);
      bsm.setMorphingParameters(iterationsPerLevel[currentCycle][bi],
                                linearForce[currentCycle], 
                                angularForce[currentCycle], 
                                stepSize[currentCycle]);
      bsm.execute();
      
      //
      // Write the morphed file
      //
      std::ostringstream str;
      str << intermediateCoordFileNamePrefix[bi].toAscii().constData()
          << ".Morph"
          << iterationsPerLevel[currentCycle][bi];
      intermediateCoordFileNamePrefix[bi] = str.str().c_str();
      QString coordName(intermediateCoordFileNamePrefix[bi]);
      coordName.append(SpecFile::getCoordinateFileExtension());
      try {
         bs->writeCoordinateFile(coordName, flatSurface->getSurfaceType(),
                                 flatSurface->getCoordinateFile());
      }
      catch (FileException& e) {
         throw BrainModelAlgorithmException(e.whatQString());
      }
      intermediateFiles.push_back(coordName);

      //
      // Convert the morphed surface to a border file for help in debugging
      //
      {
         QString borderFileName(coordName);
         borderFileName.append(SpecFile::getBorderFileExtension());
         BorderFile bf(flatSurface->getTopologyFile(), flatSurface->getCoordinateFile());
         if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
            bf.setHeaderTag(AbstractFile::headerTagConfigurationID,
                            SpecFile::getFlatBorderFileTagName());
         }
         else {
            bf.setHeaderTag(AbstractFile::headerTagConfigurationID,
                            SpecFile::getSphericalBorderFileTagName());
         }
         bf.writeFile(borderFileName);
         intermediateFiles.push_back(borderFileName);
      }
      
      if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
         //
         // If upsampling to original surface, we want sphericalUpsample to update
         // a copy of the morphing surface since morphingSurface is initially the 
         // input surface that should no longer be modified.
         //
         if (bi == 1) {
            //
            // Make a copy of the input surface (do not want to modify it)
            //
            BrainModelSurface* bms = new BrainModelSurface(*morphingSurface);
            brains[0]->addBrainModel(bms);
            morphingSurface = bms;
         }
         else {
            //
            // smooth crossovers
            //
            const int numNodes = bs->getNumberOfNodes();
            int iters = 0;
            int depth = 0;
            if (numNodes < 100) {
               iters = 1;
               depth = 1;
            }
            else if (numNodes < 1000) {
               iters = 3;
               depth = 2;
            }
            else if (numNodes < 2000) {
               iters = 5;
               depth = 2;
            }
            else if (numNodes < 5000) {
               iters = 10;
               depth = 3;
            }
            else {
               iters = 20;
               depth = 3;
            }
            
            if (iters > 0) {
//               flatSurface->smoothOutSurfaceCrossovers(1.0, 1, iters, 0, iters, depth);
            }
         }
         
         //
         // upsample to next sphere
         //
         sphericalUpsample(brains, bi - 1);
         
         //
         // Write the upsampled coordinate file
         //
         std::ostringstream str;
         str << intermediateCoordFileNamePrefix[bi-1].toAscii().constData()
            << ".Morph.Up";
         intermediateCoordFileNamePrefix[bi-1] = str.str().c_str();
         QString coordName(intermediateCoordFileNamePrefix[bi-1]);
         coordName.append(SpecFile::getCoordinateFileExtension());
         BrainModelSurface* surf = brains[bi-1]->getBrainModelSurface(SURFACE_MORPHED_INDEX);
         if (bi != 1) {
            try {
               brains[bi-1]->writeCoordinateFile(coordName, surf->getSurfaceType(),
                                              surf->getCoordinateFile());
               //
               // Convert the morphed surface to a border file for help in debugging
               //
               {
                  QString borderFileName(coordName);
                  borderFileName.append(SpecFile::getBorderFileExtension());
                  BorderFile bf(flatSurface->getTopologyFile(), flatSurface->getCoordinateFile());
                  if (morphingSurfaceType == BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT) {
                     bf.setHeaderTag(AbstractFile::headerTagConfigurationID,
                                    SpecFile::getFlatBorderFileTagName());
                  }
                  else {
                     bf.setHeaderTag(AbstractFile::headerTagConfigurationID,
                                    SpecFile::getSphericalBorderFileTagName());
                  }
                  bf.writeFile(borderFileName);
                  intermediateFiles.push_back(borderFileName);
               }
            }
            catch (FileException& e) {
               throw BrainModelAlgorithmException(e.whatQString());
            }
         }
         intermediateFiles.push_back(coordName);
      
         //
         // morph final surface
         //
         if (bi == 1) {
            //
            // Morph the surface for the specified iterations
            //
            BrainModelSurfaceMorphing bsm(brains[0]  /*bs*/, referenceSurface, morphingSurface, morphingSurfaceType);
            bsm.setMorphingParameters(iterationsPerLevel[currentCycle][0], 
                                      linearForce[currentCycle], 
                                      angularForce[currentCycle], 
                                      stepSize[currentCycle]);
            bsm.execute();
         }
      }  
      else {   // flat morphing
         if (bi > 1) {
            //
            // Upsample one resolution to another
            //
            flatUpsample(brains[bi],
                     brains[bi - 1], 
                     brains[bi - 1]->getBrainModelSurface(SURFACE_MORPHED_INDEX),
                     false);
                     
            intermediateCoordFileNamePrefix[bi - 1].append(".U");
            QString coordName(intermediateCoordFileNamePrefix[bi - 1]);
            coordName.append(SpecFile::getCoordinateFileExtension());
            BrainModelSurface* bms = brains[bi - 1]->getBrainModelSurface(SURFACE_MORPHED_INDEX);
            try {
               brains[bi - 1]->writeCoordinateFile(coordName, bms->getSurfaceType(),
                                       bms->getCoordinateFile());
            }
            catch (FileException& e) {
               throw BrainModelAlgorithmException(e.whatQString());
            }
            intermediateFiles.push_back(coordName);
         }
         else if (bi == 1) {
            //
            // Make a copy of the input surface (do not want to modify it)
            //
            BrainModelSurface* bms = new BrainModelSurface(*morphingSurface);
            brains[0]->addBrainModel(bms);
            morphingSurface = bms;
                     
            //
            // Backsample into original surface
            //
            flatUpsample(brains[bi], brains[0], morphingSurface, true);
            
            //
            // Morph the surface for the specified iterations
            //
            BrainModelSurfaceMorphing bsm(brains[0]  /*bs*/, referenceSurface, morphingSurface, morphingSurfaceType);
            bsm.setMorphingParameters(iterationsPerLevel[currentCycle][0], 
                                      linearForce[currentCycle], 
                                      angularForce[currentCycle], 
                                      stepSize[currentCycle]);
            bsm.execute();
         }
      }
   } // for (int bi = ...
}

/**
 * Upsample from one flat brain surface to another
 */
void
BrainModelSurfaceMultiresolutionMorphing::flatUpsample(BrainSet* fromBrain, BrainSet* toBrain,
                                                       BrainModelSurface* toSurface,
                                                       const bool backsampleFlag)
{
   //
   // "from" brain's surface and coordinate files
   //
   const BrainModelSurface* fromSurface = fromBrain->getBrainModelSurface(SURFACE_MORPHED_INDEX);
   const CoordinateFile* fromCoords = fromSurface->getCoordinateFile();
   const BrainModelSurface* fromNotMorphedSurface = fromBrain->getBrainModelSurface(SURFACE_NOT_MORPHED_INDEX);
   const int numFromNodes = fromNotMorphedSurface->getNumberOfNodes();
   
   //
   // Topology for "from" surface
   //
   const TopologyHelper* fromTopology = 
      fromNotMorphedSurface->getTopologyFile()->getTopologyHelper(false, true, false);
   
   //
   // "to" brain's coordinate file
   //
   CoordinateFile* toCoords = toSurface->getCoordinateFile();
   const int numToNodes = toSurface->getNumberOfNodes();
   
   //
   // Mark all of "to" surface's nodes as unvisited
   //
   toBrain->setAllNodesVisited(false);
   
   if (backsampleFlag == false) {
      //
      // In the "fromSurface" the morphNode contains the node number in the "toSurface" from
      // which this node was downsampled
      //
      for (int i = 0; i < numFromNodes; i++) {
         if (fromTopology->getNodeHasNeighbors(i)) {
            //
            // Transfer the coordinates from the morphed "from" surface
            //
            BrainSetNodeAttribute* bna = fromBrain->getNodeAttributes(i);
            toCoords->setCoordinate(bna->morphNode, fromCoords->getCoordinate(i));
            
            //
            // Use the visited flag to mark this node as being updated
            //
            BrainSetNodeAttribute* hiResAttr = toBrain->getNodeAttributes(bna->morphNode);
            hiResAttr->setVisited(true);
         }
      }
   }
   
   //
   // Point Projector to project new surface points into original surface
   //
   BrainModelSurfacePointProjector* bspp = new BrainModelSurfacePointProjector(fromNotMorphedSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_FLAT,
                           false);
   
   //
   // process all nodes in the "to" surface that were not downsampled
   //
   for (int i = 0; i < numToNodes; i++) {
      //
      // Was node NOT downsampled ?
      //
      if (toBrain->getNodeAttributes(i)->getVisited() == false) {
         //
         // project using non-morphed "from" surface
         //
         int nearestTile = -1;
         int tileNodes[3];
         float tileAreas[3];
         const int node = bspp->projectBarycentricBestTile2D(toCoords->getCoordinate(i),
                                                             nearestTile,
                                                             tileNodes,
                                                             tileAreas);
         if (node >= 0) {
            float xyz[3];
            BrainModelSurfacePointProjector::unprojectPoint(tileNodes, tileAreas, fromCoords, xyz);
            toCoords->setCoordinate(i, xyz);   
         }
         else if (node >= 0) {
            std::cout << "WARNING: Using nearest node when upsampling for " << i << std::endl;
            toCoords->setCoordinate(i, fromCoords->getCoordinate(node));
         }
      }
   }
}

/**
 * Read in the standard spheres and then construct a fiducial surface for each of them.
 */
 void
 BrainModelSurfaceMultiresolutionMorphing::constructTemplateSpheres(
                           std::vector<BrainSet*>& brains)  throw (BrainModelAlgorithmException)
 {
    //
    // Get radius of the morphing surface
    //
    const float morphingSurfaceRadius = morphingSurface->getSphericalSurfaceRadius();
    
    //
    // Create a point projector for the morphing surface
    //
    BrainModelSurfacePointProjector mspp(morphingSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           false);
   
    //
    // Get the center of gravity of the reference surface
    //
    float referenceCOG[3];
    referenceSurface->getCenterOfMass(referenceCOG);
    
    //
    // User's reference surface coordinate file
    //
    const CoordinateFile* referenceCoordinateFile = referenceSurface->getCoordinateFile();
       
    //
    // Do for each level
    //
    for (int lvl = (numberOfLevels - 1); lvl > 0; lvl--) {
       //
       // Create name of standard sphere spec file
       //
       std::ostringstream ostr;
       ostr << brainSet->getCaretHomeDirectory().toAscii().constData()
            << "/data_files/CONSTRUCT.SPHERE/"
            << "sphere.v5."
            << lvl
            << ".spec";
       const QString specFileName(ostr.str().c_str());
       
       //
       // Save name of current directory since reading a spec file sets the 
       // current path to the spec file's directory.
       //
       const QString savedPath(QDir::currentPath());
       
       //
       // Read the spec file and select all files in the spec file
       //
       SpecFile sf;
       try {
          sf.readFile(specFileName);
          sf.setAllFileSelections(SpecFile::SPEC_TRUE);
       }
       catch (FileException& /*e*/) {
          std::ostringstream msg;
          msg << "Unable to read file: "
              << specFileName.toAscii().constData()
              << std::endl;
          throw BrainModelAlgorithmException(msg.str().c_str());
       }
              
       //
       // Create a new brain set and load the files listed in the spec file
       //
       BrainSet* sphereBrain = new BrainSet;
       std::vector<QString> errorMessages;
       sphereBrain->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                 sf, specFileName, errorMessages, NULL, NULL);
       if (errorMessages.size() > 0) {
          QString msg("Error reading data files listed in spec file: ");
          msg.append(specFileName);
          msg.append("\n");
          for (unsigned int i = 0; i < errorMessages.size(); i++) {
             msg.append(errorMessages[i]);
             msg.append("\n");
          }
          throw BrainModelAlgorithmException(msg);
       }
       else if (sphereBrain->getNumberOfBrainModels() < 2) {
          QString msg(specFileName);
          msg.append(" contains\nless than two surface.  Should have fiducial and spherical");
          throw BrainModelAlgorithmException(msg);
       }
       
       //
       // Restore the current directory
       //
       QDir::setCurrent(savedPath);
       
       //
       // Get the standard sphere fiducial surface
       //
       BrainModelSurface* sphereFiducialSurface = sphereBrain->getBrainModelSurface(0);
       if (sphereFiducialSurface == NULL) {
          QString msg("Spec file: ");
          msg.append(specFileName);
          msg.append("\nDoes not contains any surfaces.");
          throw BrainModelAlgorithmException(msg);
       }
       CoordinateFile* sphereFiducialCoords = sphereFiducialSurface->getCoordinateFile();

       //
       // Get the standard sphere surface
       //
       BrainModelSurface* sphereSurface = sphereBrain->getBrainModelSurface(1);
       if (sphereSurface == NULL) {
          QString msg("Spec file: ");
          msg.append(specFileName);
          msg.append("\nDoes not contains less than 1 surface.");
          throw BrainModelAlgorithmException(msg);
       }
       
       //
       // Set the view of this standard sphere surface so that it is the
       // same as the surface being morphed (04/27/2009).
       //
       sphereSurface->copyTransformations(this->morphingSurface,
                                      BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                                      BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW);
       
       //
       // Set the sphere to have the same area as the reference surface
       //
       sphereSurface->convertToSphereWithRadius(morphingSurfaceRadius);
       
       //
       // Rotate the sphere just a little.  Higher resolution spheres have points directly
       // on edges of the lower resolution sphere which creates a degenerate condition
       // when mapping.
       //
       TransformationMatrix tm;
       tm.rotate(TransformationMatrix::ROTATE_X_AXIS, 0.25 * lvl);
       tm.rotate(TransformationMatrix::ROTATE_Y_AXIS, 0.25 * lvl);
       tm.rotate(TransformationMatrix::ROTATE_Z_AXIS, 0.25 * lvl);
       sphereSurface->applyTransformationMatrix(tm);
       
       //
       // Project nodes in sphere surface onto morphing surface
       //
       const int numSphereNodes = sphereSurface->getNumberOfNodes();
       if (numSphereNodes != sphereFiducialCoords->getNumberOfCoordinates()) {
          QString msg(specFileName);
          msg.append("\nfiducial and sphere have a different number of coordinates.\n");
          msg.append("Sphere Fiducial number of coordinates: ");
          msg.append(StringUtilities::fromNumber(sphereFiducialCoords->getNumberOfCoordinates()));
          msg.append("\nSphere number of coordinates: ");
          msg.append(StringUtilities::fromNumber(numSphereNodes));
          throw BrainModelAlgorithmException(msg);
       }
       CoordinateFile* sphereCoords = sphereSurface->getCoordinateFile();
       std::vector<bool> nodeNotProjected(numSphereNodes, false);
       bool haveNotProjectedNodes = false;
       for (int i = 0; i < numSphereNodes; i++) {
          float xyz[3];
          sphereCoords->getCoordinate(i, xyz);
          //
          // Project point to input surface
          //
          int nearestNode = -1;
          int tileNodes[3];
          float tileAreas[3];
          const int tile = mspp.projectBarycentric(xyz, nearestNode,
                                                    tileNodes, tileAreas, true);
         
          //
          // Did point project into the surface
          //
          if (tile >= 0) {
             //
             // Set the sphere's reference surface coordinate
             //
             float refXYZ[3];
             BrainModelSurfacePointProjector::unprojectPoint(tileNodes, tileAreas, 
                                                             referenceCoordinateFile, refXYZ);
             sphereFiducialCoords->setCoordinate(i, refXYZ);
          }
          else {
             //
             // Medial wall may be missing from user's reference sphere so mark nodes
             // as not projected and place them at the origin
             //
             nodeNotProjected[i] = true;
             haveNotProjectedNodes = true;
             sphereFiducialCoords->setCoordinate(i, referenceCOG);
          }
       } // for (i = 0; i < numSphereNodes...
       
       //
       // If there are nodes that were not projected.  Typically these nodes are in
       // the medial wall so smoothing them pulls them from the origin.
       //
       if (haveNotProjectedNodes) {
          sphereFiducialSurface->linearSmoothing(1.0, 300, 0, &nodeNotProjected);
       }
       
       //
       // save this sphere brain
       //
       brains.push_back(sphereBrain);
       
    } // for (lvl...
 }
 
/**
 * Map each brain to brain + 1
 */
void
BrainModelSurfaceMultiresolutionMorphing::createSphereDownsampleMapping(
                           std::vector<BrainSet*>& brains)  throw (BrainModelAlgorithmException)
{
   const int numBrains = static_cast<int>(brains.size());
    
   //
   // Map each brain to the next brain
   //
   for (int i = 0; i < (numBrains - 1); i++) {
      //
      // Get the brains
      //
      BrainSet* currentBrain = brains[i];
      BrainSet* nextBrain    = brains[i + 1];
       
      //
      // Get the current sphere's surface and coordinate file
      //
      BrainModelSurface* currentSphereSurface = NULL;
      if (i == 0) {
         currentSphereSurface = morphingSurface;
      }
      else {
         currentSphereSurface = currentBrain->getBrainModelSurface(SURFACE_MORPHED_INDEX);
      }
      const CoordinateFile* currentSphereCoords = currentSphereSurface->getCoordinateFile();
      const int numCoords = currentSphereCoords->getNumberOfCoordinates();
      
      //
      // Topology helper for current surface
      //
      const TopologyHelper* currentSphereTopologyHelper = new TopologyHelper(
                                                      currentSphereSurface->getTopologyFile(),
                                                      false, true, false);
      //
      // Get the next sphere's surface
      //
      BrainModelSurface* nextSphereSurface = nextBrain->getBrainModelSurface(SURFACE_MORPHED_INDEX);
                                 
      //
      // Create a point projector for the next sphere surface
      //
      BrainModelSurfacePointProjector sspp(nextSphereSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE,
                           false);
      
      //
      // Project each node from current sphere to next sphere
      //
      for (int i = 0; i < numCoords; i++) {
         float xyz[3];
         currentSphereCoords->getCoordinate(i, xyz);
         //
         // Project point to original surface
         //
         int nearestNode = -1;
         int tileNodes[3];
         float tileAreas[3];
         int tile = -1;
         
         //
         // Only try to project if node has neighbors
         //
         if (currentSphereTopologyHelper->getNodeHasNeighbors(i)) {
            //
            // Project current sphere node to next sphere
            //
            tile = sspp.projectBarycentric(xyz, nearestNode,
                                            tileNodes, tileAreas, true);
         }
         BrainSetNodeAttribute* bna = currentBrain->getNodeAttributes(i);
         bna->setSphericalMorphingAttributes(nearestNode, tile, tileNodes, tileAreas);
      }
      
   }
}
 
/**
 * Upsample brain + 1 to brain
 */
void
BrainModelSurfaceMultiresolutionMorphing::sphericalUpsample(std::vector<BrainSet*>& brains,
                                                            const int targetBrainIndex)  
                                                            throw (BrainModelAlgorithmException)
{
   //
   // Get the brains
   //
   BrainSet* currentBrain = brains[targetBrainIndex];
   BrainSet* nextBrain    = brains[targetBrainIndex + 1];
      
   //
   // Get the current sphere's surface and coordinate file
   //
   BrainModelSurface* currentSphereSurface = NULL;
   if (targetBrainIndex == 0) {
      currentSphereSurface = morphingSurface;
   }
   else {
      currentSphereSurface = currentBrain->getBrainModelSurface(SURFACE_MORPHED_INDEX);
   }
   CoordinateFile* currentSphereCoords = currentSphereSurface->getCoordinateFile();
   const int numCoords = currentSphereCoords->getNumberOfCoordinates();
   
   //
   // Get the radius of the sphere
   //
   const float sphereRadius = currentSphereSurface->getSphericalSurfaceRadius();
   
   //
   // Get the next sphere's surface
   //
   BrainModelSurface* nextSphereSurface = nextBrain->getBrainModelSurface(SURFACE_MORPHED_INDEX);
   const CoordinateFile* nextSphereCoords = nextSphereSurface->getCoordinateFile();
   
   //
   // Make the next sphere the same radius as the current one
   //
   nextSphereSurface->convertToSphereWithRadius(sphereRadius);
   
   //
   // Update the position of each target coordinate
   //
   for (int i = 0; i < numCoords; i++) {
      //
      // get the node's morphing information
      int nearestNode = -1;
      int tileNodes[3];
      float tileAreas[3];
      int tile = -1;
      BrainSetNodeAttribute* bna = currentBrain->getNodeAttributes(i);
      bna->getSphericalMorphingAttributes(nearestNode, tile, tileNodes, tileAreas);
      
      if (tile >= 0) {
         //
         // Unproject using tile information
         //
         float xyz[3];
         BrainModelSurfacePointProjector::unprojectPoint(tileNodes, tileAreas, nextSphereCoords,
                                                         xyz);
         currentSphereCoords->setCoordinate(i, xyz);
      }
      else if (nearestNode >= 0) {
         //
         // unproject to nearest node's position
         //
         currentSphereCoords->setCoordinate(i, nextSphereCoords->getCoordinate(nearestNode));
      }
   }
   
   //
   // Force nodes to be on the sphere
   //
   currentSphereSurface->convertToSphereWithRadius(sphereRadius);
}

/**
 * constructor
 */
MorphingMeasurements::MorphingMeasurements(const QString& nameIn,
                                    const StatisticsUtilities::DescriptiveStatistics& arealDistortionStatisticsIn,
                                    const StatisticsUtilities::DescriptiveStatistics& linearDistortionStatisticsIn,
                                    const int numberOfNodeCrossoversIn,
                                    const int numberOfTileCrossoversIn,
                                    const float elapsedTimeIn)
{
   arealDistortionStatistics  = arealDistortionStatisticsIn;
   linearDistortionStatistics = linearDistortionStatisticsIn;
   name                       = nameIn;
   numberOfNodeCrossovers     = numberOfNodeCrossoversIn;
   numberOfTileCrossovers     = numberOfTileCrossoversIn;
   elapsedTime                = elapsedTimeIn;
}
                     
/**
 * get the measurements
 */
void
MorphingMeasurements::get(QString& nameOut,
                          StatisticsUtilities::DescriptiveStatistics& arealDistortionStatisticsOut,
                          StatisticsUtilities::DescriptiveStatistics& linearDistortionStatisticsOut,
                          int& numberOfNodeCrossoversOut,
                          int& numberOfTileCrossoversOut,
                          float& elapsedTimeOut) const
{
   nameOut                       = name;
   arealDistortionStatisticsOut  = arealDistortionStatistics;
   linearDistortionStatisticsOut = linearDistortionStatistics;
   numberOfNodeCrossoversOut  = numberOfNodeCrossovers;
   numberOfTileCrossoversOut  = numberOfTileCrossovers;
   elapsedTimeOut             = elapsedTime;
}
       
