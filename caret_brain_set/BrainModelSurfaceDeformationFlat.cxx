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

#include <sstream>

#include <QDir>

#include "BorderFile.h"
#include "BrainModelRunExternalProgram.h"
#include "BrainModelSurfaceDeformationFlat.h"
#include "BrainModelSurfaceFlatHexagonalSubsample.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "DeformationMapFile.h"

/**
 * Constructor.
 */
BrainModelSurfaceDeformationFlat::BrainModelSurfaceDeformationFlat(
                                 BrainSet* brainSetIn,
                                 DeformationMapFile* deformationMapFileIn)
   : BrainModelSurfaceDeformation(brainSetIn, deformationMapFileIn)
{
}

/**
 * Destructor.
 */
BrainModelSurfaceDeformationFlat::~BrainModelSurfaceDeformationFlat()
{
}

/**
 * Check the borders to make sure the names are valid.
 */
void
BrainModelSurfaceDeformationFlat::checkBorderNames(const QString& borderFileName) throw (BrainModelAlgorithmException)
{
   BorderFile bf;
   try {
      bf.readFile(borderFileName);
      const int num = bf.getNumberOfBorders();
      for (int i = 0; i < num; i++) {
         const Border* b = bf.getBorder(i);
         const QString name(b->getName());
         
         //
         // Is this a valid border name ?
         //
         if ((name.left(6) == "morph.") ||
             (name.left(6) == "MORPH.") ||
             (name.left(8) == "LANDMARK") ||
             (name.left(8) == "landmark")) {
            return;
         }
      }
      
      QString msg("ERROR: at least one of the border files has no borders beginning with:\n");
      msg.append("   \"morph.\"\n");
      msg.append("   \"MORPH.\"\n");
      msg.append("   \"LANDMARK\"\n");
      msg.append("   \"landmark\"\n");
      throw BrainModelAlgorithmException(msg);
   }
   catch (FileException& e) {
      QString msg("ERROR checking border names for validity\n");
      msg.append(e.whatQString());
      throw BrainModelAlgorithmException(msg);
   }
}

/**
 * Execute the deformation.
 */
void 
BrainModelSurfaceDeformationFlat::executeDeformation() throw (BrainModelAlgorithmException)
{
   //
   // Get the flat parameters
   //
   int subSampleTiles = 900;
   int numberOfIterations = 0;
   float beta = 0.0;
   float varMult = 0.0;
   deformationMapFile->getFlatParameters(subSampleTiles, beta, varMult, numberOfIterations);
   
   //
   // Create a subsampled hexagaonal grid surface
   //
   BrainModelSurfaceFlatHexagonalSubsample fhs(sourceBrainSet,
                                               sourceFiducialSurface,
                                               sourceSurface,
                                               subSampleTiles);
   fhs.execute();
   
   //
   // Get the output (subsampled brain set) from the hexagonal surface generator
   //
   BrainSet* subSampledBrainSet = fhs.getSubsampledBrainSet();
   BrainModelSurface* fiducialSubSampledSurface = 
                            subSampledBrainSet->getBrainModelSurface(0);
   BrainModelSurface* flatSubSampledSurface = 
                            subSampledBrainSet->getBrainModelSurface(1);
   CoordinateFile* fiducialCoords = fiducialSubSampledSurface->getCoordinateFile();
   CoordinateFile* flatCoords = flatSubSampledSurface->getCoordinateFile();
   TopologyFile*   flatTopology = flatSubSampledSurface->getTopologyFile();
   
   //
   // Add resampled file names to deformation map file
   //
   deformationMapFile->setSourceResampledCutTopoFileName("flat_deform_resampled.topo");
   deformationMapFile->setSourceResampledFlatCoordFileName("flat_deform_resampled.FLAT.coord");
   deformationMapFile->setSourceResampledDeformedFlatCoordFileName("deformed_flat_deform_resampled.FLAT.coord");
   
   //
   // Write the subsampled brain set
   //
   subSampledBrainSet->setSpecFileName("flat_deform_resampled.spec");
   intermediateFiles.push_back("flat_deform_resampled.spec");
   subSampledBrainSet->writeTopologyFile(deformationMapFile->getSourceResampledCutTopoFileName(),
                                         flatTopology->getTopologyType(),
                                         flatTopology);
   intermediateFiles.push_back(deformationMapFile->getSourceResampledCutTopoFileName());
   subSampledBrainSet->writeCoordinateFile("flat_deform_resampled.FIDUCIAL.coord",
                                           fiducialSubSampledSurface->getSurfaceType(),
                                           fiducialCoords);
   intermediateFiles.push_back("flat_deform_resampled.FIDUCIAL.coord");
   subSampledBrainSet->writeCoordinateFile(deformationMapFile->getSourceResampledFlatCoordFileName(),
                                           flatSubSampledSurface->getSurfaceType(),
                                           flatCoords);
   intermediateFiles.push_back(deformationMapFile->getSourceResampledFlatCoordFileName());
   
   //
   // Convert the subsampled flat coord file into a border file since
   // flat_fluid operates on a border file.
   //
   const QString coordsAsBorderFileName("resampled_coords_as_border.border");
   const int maxLinksPerBorder = 100;
   BorderFile coordsAsBorder(flatCoords, maxLinksPerBorder);
   coordsAsBorder.writeFile(coordsAsBorderFileName);
   intermediateFiles.push_back(coordsAsBorderFileName);
   
   checkBorderNames(targetBorderResampledName);
   checkBorderNames(sourceBorderResampledName);
   
   //
   // Assemble arguments for flat_fluid program
   //
   QStringList str;
   str << targetBorderResampledName
       << sourceBorderResampledName
       << coordsAsBorderFileName
       << QString::number(beta, 'f', 6)
       << QString::number(varMult, 'f', 6)
       << QString::number(numberOfIterations)
       << "junk.image"
       << QString::number(0)
       << QString::number(0);
     
   if (DebugControl::getDebugOn()) {
      std::cout << std::endl;
      std::cout << "flat_fluid will be executed with the parameters: " << std::endl;
      std::cout << "   " << str.join(" ").toAscii().constData() << std::endl;
      std::cout << std::endl;
   }
   
   //
   // Execute flat_fluid 
   //
   BrainModelRunExternalProgram cup("flat_fluid", 
                                    str, 
                                    true);
   cup.execute();
   outputOfFlatFluid = cup.getOutputText();
   
   //
   // Keep track of files created by flat fluid
   //
   intermediateFiles.push_back("arch.deform.dat");
   intermediateFiles.push_back("arch.orig.dat");
   intermediateFiles.push_back("iter.dat");
   intermediateFiles.push_back("source.dat");
   intermediateFiles.push_back("source.deform.dat");
   intermediateFiles.push_back("source.orig.dat");
   intermediateFiles.push_back("source.points.deform.dat");
   intermediateFiles.push_back("target.dat");
   intermediateFiles.push_back("target.deform.dat");
   intermediateFiles.push_back("target.orig.dat");
   intermediateFiles.push_back("target.points.deform.dat");

   //
   // Read the deformed border 
   //
   BorderFile deformedCoordsAsBorder;
   try {
      deformedCoordsAsBorder.readFile("arch.deform.dat");
   }
   catch (FileException& e) {
      QString msg("Unable to read \"arch.deform.dat\", which is the output of flat_fluid.\n");
      msg += e.whatQString();
      throw BrainModelAlgorithmException(msg);
   }
   if (deformedCoordsAsBorder.getTotalNumberOfLinks() <= 0) {
      QString msg("The output of flat_fluid, \"arch.deform.dat\", contains no links.\n"
                  "This means that flat_fluid was unable to deform the input data.");
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Convert the deformed border links to a BrainModelSurface
   //
   BrainModelSurface* deformedFlatSubSampledSurface =
      new BrainModelSurface(subSampledBrainSet);
   deformedFlatSubSampledSurface->setSurfaceType(flatSubSampledSurface->getSurfaceType());
   CoordinateFile* deformedFlatCoords = deformedFlatSubSampledSurface->getCoordinateFile();
   deformedCoordsAsBorder.copyLinksToCoordinateFile(deformedFlatCoords);
   deformedFlatSubSampledSurface->setTopologyFile(flatTopology);
   subSampledBrainSet->addBrainModel(deformedFlatSubSampledSurface);
   
   //
   // Write the deformed subsampled coordinates
   //
   subSampledBrainSet->writeCoordinateFile(deformationMapFile->getSourceResampledDeformedFlatCoordFileName(),
                                       deformedFlatSubSampledSurface->getSurfaceType(),
                                       deformedFlatCoords);
   intermediateFiles.push_back(deformationMapFile->getSourceResampledDeformedFlatCoordFileName());

   //
   // Copy the user's input surface to a new deformed surface
   //
   deformedSourceSurface = new BrainModelSurface(*sourceSurface);
   sourceBrainSet->addBrainModel(deformedSourceSurface);
   
   //
   // Create a Point Projector for the flat subsampled surface.
   //
   BrainModelSurfacePointProjector bspp(flatSubSampledSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                           false);
   
   //
   // Get the coordinate files for the user's surface and its
   // new deformed surface.
   //
   CoordinateFile* sourceCoords = sourceSurface->getCoordinateFile();
   CoordinateFile* deformedSourceCoords = deformedSourceSurface->getCoordinateFile();
   const int numCoords = sourceCoords->getNumberOfCoordinates();
   
   //
   // See if X coordinate will need to be flipped
   //
   const bool diffHemFlag = 
          (sourceBrainSet->getStructure() != targetBrainSet->getStructure());
          
   //
   // Project each point in the users input surface to its deformed surface
   //
   for (int i = 0; i < numCoords; i++) {
      float xyz[3];
      sourceCoords->getCoordinate(i, xyz);
      if (diffHemFlag) {
         xyz[0] = -xyz[0];
      }
      
      //
      // Project source node onto subsampled surface
      //
      int nearestNode = -1;
      int tileNodes[3];
      float tileAreas[3];
      const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                               tileNodes, tileAreas, true);
                                               
      //
      // Unproject using the deformed subsampled coordinate file
      //
      if (tile >= 0) {
         BrainModelSurfacePointProjector::unprojectPoint(tileNodes, 
                                                         tileAreas,
                                                         deformedFlatCoords, 
                                                         xyz);
      }
      else if (nearestNode >= 0) {
         // JWH 08/08/03  2:15pm deformedSourceCoords->getCoordinate(nearestNode, xyz);
         deformedFlatCoords->getCoordinate(nearestNode, xyz);
      }
      deformedSourceCoords->setCoordinate(i, xyz);
   }
   
   //
   // Write the source deformed coordinates
   //
   QDir::setCurrent(sourceDirectory);
   QString defCoordName(deformationMapFile->getDeformedFileNamePrefix());
   defCoordName.append(sourceCoords->getFileName());
   deformationMapFile->setSourceDeformedFlatCoordFileName(defCoordName);
   deformedSourceCoords->writeFile(defCoordName);
   //sourceBrainSet->writeCoordinateFile(defCoordName,
   //                                    deformedSourceSurface->getSurfaceType(),
   //                                    deformedSourceCoords);
   QDir::setCurrent(originalDirectory);
   
}
      
