
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

#include <fstream>
#include <iostream>
#include <sstream>

#include <QDateTime>
#include <QDir>
#include <QFile>

#include "BrainModelSurface.h"
#include "BrainModelVolumeSureFitErrorCorrection.h"
#include "BrainModelVolumeToSurfaceConverter.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"

/**
 * Constructor.
 */
BrainModelVolumeSureFitErrorCorrection::BrainModelVolumeSureFitErrorCorrection(
                                                          BrainSet* bs,
                                                          VolumeFile* segmentationVolumeIn,
                                                          VolumeFile* radialPositionMapVolumeIn,
                                                          const VolumeFile::FILE_READ_WRITE_TYPE typeOfVolumeFilesToWriteIn,
                                                          const int acIJKIn[3],
                                                          const bool leftHemFlagIn,
                                                          const bool saveIntermediateFilesIn)
   : BrainModelAlgorithm(bs)
{
   //
   // Setting true will reduce runtime by about 5% but does
   // require plenty of memory
   //
   keepIntermediateFilesInMemoryFlag = false; // true;
   
   segmentationVolume = new VolumeFile(*segmentationVolumeIn);
   radialPositionMapVolume = new VolumeFile(*radialPositionMapVolumeIn);
   typeOfVolumeFilesToWrite = typeOfVolumeFilesToWriteIn;
   leftHemFlag = leftHemFlagIn;
   acIJK[0] = acIJKIn[0];
   acIJK[1] = acIJKIn[1];
   acIJK[2] = acIJKIn[2];
   
   intermediateFilesSubDirectory = "ERROR_CORRECTION_INTERMEDIATES";
   saveIntermediateFiles = saveIntermediateFilesIn;
   
   if (saveIntermediateFiles) {
      keepIntermediateFilesInMemoryFlag = false;
   }
   
   outputVolume = NULL;
}

/**
 * Destructor.
 */
BrainModelVolumeSureFitErrorCorrection::~BrainModelVolumeSureFitErrorCorrection()
{
   if (segmentationVolume != NULL) {
      delete segmentationVolume;
   }
   if (radialPositionMapVolume != NULL) {
      delete radialPositionMapVolume;
   }
   
   if (saveIntermediateFiles == false) {
      for (unsigned int i = 0; i < intermediateFileNames.size(); i++) {
         QFile::remove(intermediateFileNames[i]);
      }
      
      for (std::map<QString,VolumeFile*>::iterator iter = 
                      intermediateVolumeFilesInMemory.begin();
           iter != intermediateVolumeFilesInMemory.end();
           iter++) {
         delete iter->second;
      }
      
      QDir dir;
      dir.rmdir(intermediateFilesSubDirectory);
   }
}

/**
 * execute the algorithm
 */
void 
BrainModelVolumeSureFitErrorCorrection::execute() throw (BrainModelAlgorithmException)
{
   if (segmentationVolume == NULL) {
      throw BrainModelAlgorithmException("Input segmentation is invalid");
   }
   segmentationVolume->getDimensions(xDim, yDim, zDim);
   if ((xDim <= 0) || (yDim <= 0) || (zDim <= 0)) {
      throw BrainModelAlgorithmException("Input segmentation is invalid");
   }
   
   if (radialPositionMapVolume == NULL) {
      throw BrainModelAlgorithmException("Input radial position map volume is invalid");
   }
   int raddim[3];
   radialPositionMapVolume->getDimensions(raddim);
   if ((raddim[0] != xDim) || (raddim[1] != yDim) || (raddim[2] != zDim)) {
      throw BrainModelAlgorithmException(
         "Input Radial Position Map Volume has different dimensions than segmentation volume.");
   }
   
   try {
      //
      // Create the intermediate files directory
      //
      QDir intermedDir(intermediateFilesSubDirectory);
      if (intermedDir.exists() == false) {
         QDir temp(".");
         temp.mkdir(intermediateFilesSubDirectory);
      }
      
      //
      // Create measurements
      //
      QTime genTimer;
      genTimer.start();
      generateSurfaceAndMeasurements(segmentationVolume);
      const float genTime = genTimer.elapsed() * 0.001;
      
      //
      // Correct the errors
      //
      QTime errorTimer;
      errorTimer.start();
      correctErrors();
      const float errorTime = errorTimer.elapsed() * 0.001;
      
      if (DebugControl::getDebugOn()) {
         std::cout << "Surface and measurements time: " << genTime << std::endl;
         std::cout << "Correct errors time: " << errorTime << std::endl;
      }
   }
   catch (BrainModelAlgorithmException& e) {
      removeProgressDialog();
      throw e;
   }
   
   removeProgressDialog();
}

/**
 * Generate a surface from the specified volume.
 */
void
BrainModelVolumeSureFitErrorCorrection::generateSurfaceAndMeasurements(const VolumeFile* vf)
    throw (BrainModelAlgorithmException)
{
   //
   // Make a copy of the segmentation volume since it will be added to a temporary brain set
   //
   VolumeFile* segVol = new VolumeFile(*vf);

   //
   // Fill any holes in the segmentation and write it
   //
   segVol->fillSegmentationCavities();
   writeIntermediateVolume(segVol, "Segment.BeforePatch");
   
   //
   // Create a brain set and add the segmentation volume to it
   //
   BrainSet bs;
   bs.addVolumeFile(VolumeFile::VOLUME_TYPE_SEGMENTATION,
                    segVol,
                    "",
                    false,
                    false);
                    
   //
   // convert the segmentation volume to a surface
   //
   BrainModelVolumeToSurfaceConverter bmvsc(&bs,
                                            segVol,
                BrainModelVolumeToSurfaceConverter::RECONSTRUCTION_MODE_SUREFIT_SURFACE,
                                            (leftHemFlag == false),
                                            leftHemFlag);
   bmvsc.execute();
   
   //
   // Find the raw and fiducial surfaces
   //
   BrainModelSurface* rawSurface = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_RAW);
   if (rawSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find raw surface for surface measurements.");
   }
   BrainModelSurface* fiducialSurface = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find fiducial surface for surface measurements.");
   }
   
   //
   // Metric file for surface measurements
   //
   MetricFile metricMeasurements;
   
   //
   // Generate the elliposoid surface
   //
   fiducialSurface->createInflatedAndEllipsoidFromFiducial(false,  // inflated
                                                           false,  // very inflated
                                                           true,   // ellipsoid
                                                           false,  // sphere
                                                           true,   // finger smoothing
                                                           false,  // scale to fiducial
                                                           1.0,    // No iteration scaling
                                                           &metricMeasurements);
                                                           
   //
   // Output measurements for debugging
   //
   if (DebugControl::getDebugOn()) {
      try {
         metricMeasurements.writeFile("ErrorCorrectionMeasurements.metric");
      }
      catch (FileException&) {
      }
   }
   
   //
   // Find the ellipsoid surface
   //
   BrainModelSurface* ellipsoidSurface = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL);
   if (ellipsoidSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find ellipsoid surface for surface measurements.");
   }

   //
   // Find the compressed or stretched column
   //
   const int compressedStretchedColumn = metricMeasurements.getColumnWithName("Ellipsoid_CompressedOrStretched");
   if (compressedStretchedColumn < 0) {
      throw BrainModelAlgorithmException("Unable to find metric column named Ellipsoid_CompressedOrStretched");
   }
   
   //
   // Create a volume containing the compressed/stretched for the ellipsoid surface
   //
   VolumeFile* compressStretchVolume = convertMetricToVolume(rawSurface,
                                                        &metricMeasurements,
                                                        compressedStretchedColumn,
                                                        1.0,
                                                        1.5);
   if(compressStretchVolume == NULL) {
      throw BrainModelAlgorithmException("Unable to create compress stretch ellipsoid volume.");
   }  
   writeIntermediateVolume(compressStretchVolume, "Ellipsoid_CompressedOrStretched");
   delete compressStretchVolume; // 7/18/2005
   compressStretchVolume = NULL; // 7/18/2005

   //
   // Do a crossover check on the ellipsoid surface
   //
   ellipsoidSurface->crossoverCheckSureFitEllipsoid();
                                    
   //
   // Create a metric file containing the crossovers
   //
   const int numNodes = ellipsoidSurface->getNumberOfNodes();
   MetricFile crossoversMetric;
   crossoversMetric.setNumberOfNodesAndColumns(numNodes, 1);
   const QString metricCrossoversColumnName("crossovers");
   const int crossoversColumn = 0;
   crossoversMetric.setColumnName(crossoversColumn, metricCrossoversColumnName);
   for (int i = 0; i < numNodes; i++) {
      const BrainSetNodeAttribute* bna = bs.getNodeAttributes(i);
      if (bna->getCrossover() == BrainSetNodeAttribute::CROSSOVER_YES) {
         crossoversMetric.setValue(i, crossoversColumn, 1.0);
      }
   }
   
   //
   // Create a volume containing the crossovers and write it
   //
   VolumeFile* crossoversVolume = convertMetricToVolume(rawSurface,
                                                        &crossoversMetric,
                                                        crossoversColumn,
                                                        1.0,
                                                        1.5);
   if(crossoversVolume == NULL) {
      throw BrainModelAlgorithmException("Unable to create crossovers volume.");
   }  
   writeIntermediateVolume(crossoversVolume, "Crossovers");
 
   //
   // Metric file for near crossovers
   //
   MetricFile nearCrossoversMetric;
   nearCrossoversMetric.setNumberOfNodesAndColumns(numNodes, 1);
   const int nearCrossoversColumn = 0;
   nearCrossoversMetric.setColumnName(nearCrossoversColumn, "Near Crossovers");
    
   //
   // Find the non-crossover nodes near the crossovers
   //
   crossoverProjection(ellipsoidSurface,
                     &crossoversMetric,
                     crossoversColumn,
                     &metricMeasurements,
                     compressedStretchedColumn,
                     &nearCrossoversMetric,
                     nearCrossoversColumn,
                     1.0);
                        
   //
   // Create a volume containing the near crossovers and write it
   //
   VolumeFile* nearCrossoversVolume = convertMetricToVolume(rawSurface,
                                                        &nearCrossoversMetric,
                                                        nearCrossoversColumn,
                                                        1.0,
                                                        1.5);
   if(nearCrossoversVolume == NULL) {
      throw BrainModelAlgorithmException("Unable to create near crossovers volume.");
   }  
   writeIntermediateVolume(nearCrossoversVolume, "NearCrossovers");
   delete nearCrossoversVolume; // 7/18/2005
   nearCrossoversVolume = NULL; // 7/18/2005
   
   //
   // Find the gaussian neg for low smoothed surface column
   //
   const int gaussNegLowSmoothColumn = metricMeasurements.getColumnWithName("gaussian_neg.LowSmooth");
   if (gaussNegLowSmoothColumn < 0) {
      throw BrainModelAlgorithmException("Unable to find metric column named gaussian_neg.LowSmooth");
   }
   
   //
   // Create a volume containing the gaussian neg for low smoothed surface
   //
   VolumeFile* gaussNegLowSmoothVolume = convertMetricToVolume(rawSurface,
                                                        &metricMeasurements,
                                                        gaussNegLowSmoothColumn,
                                                        1.0,
                                                        1.5);
   if(gaussNegLowSmoothVolume == NULL) {
      throw BrainModelAlgorithmException("Unable to create gauss neg low smooth volume.");
   }  
   writeIntermediateVolume(gaussNegLowSmoothVolume, "Gauss_neg");
   delete gaussNegLowSmoothVolume; // 7/18/2005
   gaussNegLowSmoothVolume = NULL; // 7/18/2005
   
   //
   // Find the compressed for high smoothed surface column
   //
   const int compressHighSmoothColumn = metricMeasurements.getColumnWithName("compressed.HighSmooth");
   if (compressHighSmoothColumn < 0) {
      throw BrainModelAlgorithmException("Unable to find metric column named compressed.HighSmooth");
   }
   
   //
   // Create a volume containing the compressed for high smoothed surface
   //
   VolumeFile* compressHighSmoothVolume = convertMetricToVolume(rawSurface,
                                                        &metricMeasurements,
                                                        compressHighSmoothColumn,
                                                        1.0,
                                                        1.5);
   if(compressHighSmoothVolume == NULL) {
      throw BrainModelAlgorithmException("Unable to create gauss neg low smooth volume.");
   }  
   writeIntermediateVolume(compressHighSmoothVolume, "Compression.HighSmooth");
   delete compressHighSmoothVolume; // 7/18/2005
   compressHighSmoothVolume = NULL; // 7/18/2005
   
   //
   //
   // Create a thresholded volume of crossovers and write it
   //  
   crossoversVolume->thresholdVolume(75);
   writeIntermediateVolume(crossoversVolume, "Crossovers.thresh");
   
   delete crossoversVolume;  // 7/18/2005
   crossoversVolume = NULL;  // 7/18/2005

}

/** 
 * Find the nearest node that is not a crossover and has a compressed 
 * value less than the threshold.
 */
void
BrainModelVolumeSureFitErrorCorrection::crossoverProjection(const BrainModelSurface* bms,
                    const MetricFile* crossoversMetric,
                    const int crossoverColumn,
                    const MetricFile* compressedMetric,
                    const int compressedColumn,
                    MetricFile* outputMetric,
                    const int outputColumn,
                    const float thresholdValue)  throw (BrainModelAlgorithmException)
{
   //
   // Get coordinates
   //
   const CoordinateFile* cf = bms->getCoordinateFile();
   const int numNodes = bms->getNumberOfNodes();
   
   //
   // Get topology helper
   //
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      std::ostringstream str;
      str << "Surface "
          << FileUtilities::basename(bms->getCoordinateFile()->getFileName()).toAscii().constData()
          << " has no topology";
      throw BrainModelAlgorithmException(str.str().c_str());
   }
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Clear output metric column
   //   
   for (int i = 0; i < numNodes; i++) {
      outputMetric->setValue(i, outputColumn, 0.0);
   }
   
   //
   // find closest non-crossover and non-stretched node to each crossover node
   //
   for (int i = 0; i < numNodes; i++) {
      //
      // Is this node a crossover node
      //
      if (crossoversMetric->getValue(i, crossoverColumn) != 0) {
         //
         // Get this node's coordinates
         //
         float x, y, z;
         cf->getCoordinate(i, x, y, z);
         
         //
         // Find nearest node that is not a crossover and has compression below the threshold
         //
         float nearestDistSQ = -1.0;
         int nearestNode = -1;
         for (int j = 0; j < numNodes; j++) {
            if ((crossoversMetric->getValue(j, crossoverColumn) == 0.0) &&
                (compressedMetric->getValue(j, compressedColumn) <= thresholdValue) &&
                th->getNodeHasNeighbors(j)) {
               float nodeX, nodeY, nodeZ;
               cf->getCoordinate(j, nodeX, nodeY, nodeZ);
               const float dx = nodeX - x;
               const float dy = nodeY - y;
               const float dz = nodeZ - z;
               const float distSQ = dx*dx + dy*dy + dz*dz;
               if (nearestNode < 0) {
                  nearestDistSQ = distSQ;
                  nearestNode = j;
               }
               else if (distSQ < nearestDistSQ) {
                  nearestDistSQ = distSQ;
                  nearestNode = j;
               }
            }
         }
         
         if (nearestNode < 0) {
            std::cout << "ERROR: Cannot find nearest node to crossover node "
                      << i << endl;
         }
         else {
            outputMetric->setValue(nearestNode, outputColumn, 1.0);
         }
      }
   }
}

/**
 * convert metric surface data into a volume.
 */
VolumeFile* 
BrainModelVolumeSureFitErrorCorrection::convertMetricToVolume(
                                                 const BrainModelSurface* bms,
                                                 const MetricFile* mf,
                                                 const int metricColumn,
                                                 const float scaling,
                                                 const float voxelBoxSize)
{
   //
   // Check inputs
   //
   if (bms == NULL) {
      return NULL;
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      return NULL;
   }
   if ((metricColumn < 0) || (metricColumn >= mf->getNumberOfColumns())) {
      return NULL;
   }
   
   //
   // Get half of the voxel box size
   //
   const float halfVoxelSize = voxelBoxSize * 0.5;
   
   //
   // Get the surface's coordinate file
   //
   const CoordinateFile* cf = bms->getCoordinateFile();

   //
   // Create the volume file and clear its voxels
   //
   VolumeFile* vf = new VolumeFile(*segmentationVolume);
   vf->makeDefaultFileName("metric-to-vol");
   vf->setVolumeType(VolumeFile::VOLUME_TYPE_FUNCTIONAL);
   vf->setAllVoxels(-1.0);
   
   //
   // Process each node
   //
   for (int m = 0; m < numNodes; m++) {
      //
      // Get the metric value
      //
      const float metricValue = mf->getValue(m, metricColumn);

      //
      // Get coordinate of node
      //
      const float* xyz = cf->getCoordinate(m);
      
      //
      // Convert coordinate to voxel IJK index and set the value for this voxel
      //
      int ijk[3];
      vf->convertCoordinatesToVoxelIJK(xyz, ijk);
      if (vf->getVoxelIndexValid(ijk)) {
         if (metricValue > vf->getVoxel(ijk)) {
            vf->setVoxel(ijk, 0, metricValue);
         }
      }
      
      //
      // Is the voxel being "fuzzed"
      //
      if (voxelBoxSize > 1.0) {
         for (float x = (xyz[0] - halfVoxelSize); x <= (xyz[0] + halfVoxelSize); x += 1.0) {
            for (float y = (xyz[1] - halfVoxelSize); y <= (xyz[1] + halfVoxelSize); y += 1.0) {
               for (float z = (xyz[2] - halfVoxelSize); z <= (xyz[2] + halfVoxelSize); z += 1.0) {
                  const float tempxyz[3] = { x, y, z };
                  int ijk[3];
                  vf->convertCoordinatesToVoxelIJK(tempxyz, ijk);
                  if (vf->getVoxelIndexValid(ijk)) {
                     if (metricValue > vf->getVoxel(ijk)) {
                        vf->setVoxel(ijk, 0, metricValue);
                     }
                  }
               }
            }
         }
      }      
   }
   
   if (scaling == 1.0) {
      const int numVoxels = vf->getTotalNumberOfVoxels();
      for (int i = 0; i < numVoxels; i++) {
         float voxel = vf->getVoxelWithFlatIndex(i);
         if (voxel < -1.0) {
            voxel = 0.0;
         }
         else if (voxel > 4.0) {
            voxel = 255.0;
         }
         else {
            voxel = 50.0 * (voxel + 1.0);
         }
         voxel = std::min(voxel, 255.0f);
         vf->setVoxelWithFlatIndex(i, 0, voxel);
      }
   }
   
   return vf;
}

/**
 * Correct errors in segmetation.
 */
int 
BrainModelVolumeSureFitErrorCorrection::correctErrors() 
   throw (BrainModelAlgorithmException)
{
   if (DebugControl::getDebugOn()) {
   	std::cout << "#### CorrectErrors begin ####" << std::endl;
   }
   
	/*TO-DO: CheckRPM
		find RadialPositionMap_pad.mnc or RadialPositionMap.mnc
		make sure its dims match the segmentations
	*/

	/*
	TEMPORARY 
	GenerateSurfaceInflateCrossoverCheck (segfname, ellipsoidvtkname, rawvtkfname, lp);
	*/

	//START FINGER PATCHING
	//%int i, seed[3]={0,0,0}, extent[6];
	//%int xdim=lp.xdim;
	//%int ydim=lp.ydim;
	//%int zdim=lp.zdim;
	//%int num_voxels=xdim*ydim*zdim;
	//%unsigned char* voxdataflat=new unsigned char [num_voxels];
	//%unsigned char* hidata=new unsigned char [num_voxels];
	//%unsigned char* lodata=new unsigned char [num_voxels];
	//%read_minc_file ("Compression.HighSmooth.mnc", hidata);
	//%for ( i=0 ; i<num_voxels ; i++ ) lodata[i]=hidata[i];
   VolumeFile hidata;
   readIntermediateVolume(hidata, "Compression.HighSmooth");
   VolumeFile lodata = hidata;
   
	//Thresh.py Compression.HighSmooth.mnc CompressedRegions.HiThresh 150
	//%ThresholdVolume (hidata, 150, xdim,ydim,zdim);
	//%write_minc ("CompressedRegions.HiThresh.mnc", hidata, xdim, ydim, zdim);
	//%unsigned char* crhdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) crhdata[i]=hidata[i];
   hidata.thresholdVolume(150);
   writeIntermediateVolume(hidata, "CompressedRegions.HiThresh");
   VolumeFile crhdata = hidata;

	//Thresh.py Compression.HighSmooth.mnc CompressedRegions.LowThresh 100
	//%ThresholdVolume (lodata, 100, xdim,ydim,zdim);
	//%write_minc ("CompressedRegions.LowThresh.mnc", lodata, xdim, ydim, zdim);
   lodata.thresholdVolume(100);
   writeIntermediateVolume(lodata, "CompressedRegions.LowThresh");
   
	//Sculpt.py 0 8 0 $ncol 0 $nrow 0 $nslices CompressedRegions.HiThresh.mnc CompressedRegions.LowThresh.mnc CompressedRegions.sculpt
	//%extent[0]=0;
	//%extent[1]=xdim;
	//%extent[2]=0;
	//%extent[3]=ydim;
	//%extent[4]=0;
	//%extent[5]=zdim;
	//%Sculpt (0, 8, seed, extent, hidata, lodata, xdim, ydim, zdim);
	//%write_minc ("CompressedRegions.sculpt.mnc", hidata, xdim, ydim, zdim);
	//%unsigned char* crsdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) crsdata[i]=hidata[i];
   int extent[6] = {
      0,
      xDim,
      0,
      yDim,
      0,
      zDim
   };
   int seed[3] = { 0, 0, 0 };
   hidata.sculptVolume(0, &lodata, 8, seed, extent);
   writeIntermediateVolume(hidata, "CompressedRegions.sculpt");
   VolumeFile crsdata = hidata;
   
	//IDENTIFY AND PATCH EVAGINATIONS

	//VolMorphOps.py 2 4 Segment.BeforePatch.mnc SegmentationHull.erode
	//unsigned char* sbpdata=new unsigned char [num_voxels];
	//unsigned char* voxdataflat2=new unsigned char [num_voxels];
	//read_minc_file ("Segment.BeforePatch.mnc", sbpdata);
	//for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=voxdataflat2[i]=sbpdata[i];
	//DoVolMorphOps (xdim, ydim, zdim, voxdataflat, 2, 4);
	//write_minc ("SegmentationHull.erode.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile sbpdata;
   readIntermediateVolume(sbpdata, "Segment.BeforePatch");
   VolumeFile voxdataflat = sbpdata;
   VolumeFile voxdataflat2 = sbpdata;
   voxdataflat.doVolMorphOps(2, 4);
   writeIntermediateVolume(voxdataflat, "SegmentationHull.erode");
   
	//FillBiggestObject.py SegmentationHull.erode.mnc SegmentationCore 0 $ncol 0 $nrow 0 $nslices
	//%FindBiggestObjectWithinMask (voxdataflat, xdim, ydim, zdim,
	//%	0, xdim, 0, ydim, 0, zdim, seed);
	//%vtkFloodFill (seed, voxdataflat, 255, 255, 0, xdim, ydim, zdim);
	//%write_minc ("SegmentationCore.mnc", voxdataflat, xdim, ydim, zdim); 
   voxdataflat.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
   voxdataflat.floodFillWithVTK(seed, 255, 255, 0);
   writeIntermediateVolume(voxdataflat, "SegmentationCore");
   
	//VolMorphOps.py 2 0 SegmentationCore.mnc SegmentationCore.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, voxdataflat, 2, 0);
	//%write_minc ("SegmentationCore.dilate.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.doVolMorphOps(2, 0);
   writeIntermediateVolume(voxdataflat, "SegmentationCore.dilate");
   
	//CombineVols.py subrect CompressedRegions.sculpt.mnc SegmentationCore.dilate.mnc EvaginationCores
	//%CombineVols ("subrect", hidata, voxdataflat, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("EvaginationCores.mnc", hidata, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &hidata,
                                            &voxdataflat,
                                            &voxdataflat,
                                            &hidata);
   hidata.stretchVoxelValues();
   writeIntermediateVolume(hidata, "EvaginationCores");

	//VolMorphOps.py 2 2 EvaginationCores.mnc Evaginations
	//%DoVolMorphOps (xdim, ydim, zdim, hidata, 2, 2);
	//%write_minc ("CompositeEvaginations.mnc", hidata, xdim, ydim, zdim);
	hidata.doVolMorphOps(2, 2);
   writeIntermediateVolume(hidata, "CompositeEvaginations");
   
	//Omit until we use the counts for something...
	//NewEulerSubVolume.py Evaginations.mnc Evaginations.euler 0 $ncol 0 $nrow 0 $nslices
	//NewEulerSubVolume (hidata, "Evaginations.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	
	//CombineVols.py subrect Segment.BeforePatch.mnc Evaginations.mnc Segmentation_noEvaginations.mnc
	//%CombineVols ("subrect", voxdataflat2, hidata, hidata, xdim, ydim, zdim);
	//%write_minc ("Segmentation_noEvaginations.mnc", voxdataflat2, xdim, ydim, zdim);
	//%unsigned char* snedata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) snedata[i]=voxdataflat2[i];
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat2,
                                            &hidata,
                                            &hidata,
                                            &voxdataflat2);
   voxdataflat2.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat2, "Segmentation_noEvaginations");
   VolumeFile snedata = voxdataflat2;

	//VolMorphOps.py 2 0 Evaginations.mnc Evaginations.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, hidata, 2, 0);
	//%write_minc ("Evaginations.dilate.mnc", hidata, xdim, ydim, zdim);
	hidata.doVolMorphOps(2, 0);
   writeIntermediateVolume(hidata, "Evaginations.dilate");

	//IDENTIFY (BUT DON'T PATCH) SMALL POCKETS

	//VolMorphOps.py 1 1 Segmentation_noEvaginations.mnc Segment_noEvagination.DilateErode
	//%DoVolMorphOps (xdim, ydim, zdim, voxdataflat2, 1, 1);
	//%write_minc ("Segment_noEvagination.DilateErode.mnc", voxdataflat2, xdim, ydim, zdim);
   voxdataflat2.doVolMorphOps(1, 1);
   writeIntermediateVolume(voxdataflat2, "Segment_noEvagination.DilateErode");

	//CombineVols.py subrect Segment_noEvagination.DilateErode.mnc Segmentation_noEvaginations.mnc PossibleSmallPockets
	//%CombineVols ("subrect", voxdataflat2, snedata, snedata, xdim, ydim, zdim);
	//%write_minc ("PossibleSmallPockets.mnc", voxdataflat2, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat2,
                                            &snedata,
                                            &snedata,
                                            &voxdataflat2);
   voxdataflat2.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat2, "PossibleSmallPockets");
   
	//CombineVols.py mult PossibleSmallPockets.mnc CompressedRegions.LowThresh.mnc CompressedSmallPockets
	//%CombineVols ("mult", voxdataflat2, lodata, lodata, xdim, ydim, zdim);
	//%write_minc ("CompressedSmallPockets.mnc", voxdataflat2, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_MULTIPLY,
                                            &voxdataflat2,
                                            &lodata,
                                            &lodata,
                                            &voxdataflat2);
   voxdataflat2.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat2, "CompressedSmallPockets");
   
	//VolMorphOps.py 1 0 CompressedSmallPockets.mnc SmallPockets.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, voxdataflat2, 1, 0);
	//%write_minc ("SmallPockets.dilate.mnc", voxdataflat2, xdim, ydim, zdim);
	//%unsigned char* spddata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) spddata[i]=voxdataflat2[i];
	voxdataflat2.doVolMorphOps(1, 0);
   writeIntermediateVolume(voxdataflat2, "SmallPockets.dilate");
   VolumeFile spddata = voxdataflat2;
   
	//Omit until we use the counts for something...
	// EulerSubVolume.py SmallPockets.dilate.mnc SmallPockets.euler 0 $ncol 0 $nrow 0 $nslices
	//countfname = "%s/%s" % (ECDirectory, "SmallPockets.euler")
	//NewEulerSubVolume (fname, countfname, extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	
	//NEW NEXT TWO STEPS - AUG 11
	//%if (lp.Hem == 0 ) {
	//%	//MaskVol.py SmallPockets.dilate.mnc SmallPockets.mask `expr $ACx - 20` `expr $ACx + 0` `expr $ACy - 50` `expr $ACy + 30` 0 `expr $ACz + 30`
	//%	extent[0]=lp.ACx-20;
	//%	extent[1]=lp.ACx;
	//%} else {
	//%	//MaskVol.py SmallPockets.dilate.mnc SmallPockets.mask `expr $ACx - 0` `expr $ACx + 20` `expr $ACy - 50` `expr $ACy + 30` 0 `expr $ACz + 30`
	//%	extent[0]=lp.ACx;
	//%	extent[1]=lp.ACx+20;
	//%}
	//%extent[2]=lp.ACy-50;
	//%extent[3]=lp.ACy+30;
	//%extent[4]=0;
	//%extent[5]=lp.ACz+30;
	//%MaskVolume (voxdataflat2, xdim, ydim, zdim, extent);
	//%write_minc ("SmallPockets.mask.mnc", voxdataflat2, xdim, ydim, zdim);
   if (leftHemFlag) {
      extent[0] = acIJK[0] - 20;
      extent[1] = acIJK[0];
   }
   else {
      extent[0] = acIJK[0];
      extent[1] = acIJK[0] + 20;
   }
   extent[2] = acIJK[1] - 50;
   extent[3] = acIJK[1] + 30;
   extent[4] = 0;
   extent[5] = acIJK[2] + 30;
   voxdataflat.maskVolume(extent);
   writeIntermediateVolume(voxdataflat2, "SmallPockets.mask");
   
	//CombineVols.py OR  Segmentation_noEvaginations.mnc  SmallPockets.mask.mnc Segmentation_noEvaginations_orSubCortInvaginations
	//%CombineVols ("OR", voxdataflat2, snedata, snedata, xdim, ydim, zdim);
	//%write_minc ("Segmentation_noEvaginations_orSubCortInvaginations.mnc", voxdataflat2, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat2,
                                            &snedata,
                                            &snedata,
                                            &voxdataflat2);
   voxdataflat2.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat2, "Segmentation_noEvaginations_orSubCortInvaginations");
	
	//IDENTIFY AND PATCH LARGE INVAGINATIONS

	//VolMorphOps.py 4 12 Segmentation_noEvaginations.mnc SegmentationDeepCore
	//%unsigned char* sdcdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) sdcdata[i]=snedata[i];
	//%DoVolMorphOps (xdim, ydim, zdim, sdcdata, 4, 12);
	//%write_minc ("SegmentationDeepCore.mnc", sdcdata, xdim, ydim, zdim);
   VolumeFile sdcdata = snedata;
   sdcdata.doVolMorphOps(4, 12);
   writeIntermediateVolume(sdcdata, "SegmentationDeepCore");

	//Thresh.py Gauss_neg.mnc Gauss_neg.thresh 30
	//%read_minc_file ("Gauss_neg.mnc", voxdataflat);
	//%ThresholdVolume (voxdataflat, 30, xdim,ydim,zdim);
	//%write_minc ("Gauss_neg.thresh.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "Gauss_neg");
   voxdataflat.thresholdVolume(30);
   writeIntermediateVolume(voxdataflat, "Gauss_neg.thresh");
   
	//CombineVols.py subrect Gauss_neg.thresh.mnc CompressedRegions.HiThresh.mnc PossibleInvaginationPlugs
	//%CombineVols ("subrect", voxdataflat, crhdata, crhdata, xdim, ydim, zdim);
	//%write_minc ("PossibleInvaginationPlugs.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &crhdata,
                                            &crhdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "PossibleInvaginationPlugs");
   
	//Sculpt.py 2 1 0 $ncol 0 $nrow 0 $nslices PossibleInvaginationPlugs.mnc Segmentation_noEvaginations.mnc PossibleInvaginationPlugs.sculpt
	//%extent[0]=0;
	//%extent[1]=xdim;
	//%extent[2]=0;
	//%extent[3]=ydim;
	//%extent[4]=0;
	//%extent[5]=zdim;
	//%Sculpt (2, 1, seed, extent, voxdataflat, snedata, xdim, ydim, zdim);
	//%write_minc ("PossibleInvaginationPlugs.sculpt.mnc", voxdataflat, xdim, ydim, zdim);
	extent[0] = 0;
	extent[1] = xDim;
	extent[2] = 0;
	extent[3] = yDim;
	extent[4] = 0;
	extent[5] = zDim;
   voxdataflat.sculptVolume(2, &snedata, 1, seed, extent);
   writeIntermediateVolume(voxdataflat, "PossibleInvaginationPlugs.sculpt");
   
	//CombineVols.py OR Segmentation_noEvaginations.mnc PossibleInvaginationPlugs.sculpt.mnc Segmentation_InvaginationPlugged 
	//%CombineVols ("OR", voxdataflat, snedata, snedata, xdim, ydim, zdim);
	//%write_minc ("Segmentation_InvaginationPlugged.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &snedata,
                                            &snedata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "Segmentation_InvaginationPlugged");
   
	//ADDED LINES 13 AUG: CombineVols.py subrect CompressedRegions.LowThresh.mnc Segmentation_InvaginationPlugged.mnc CompressedRegions.LowThresh_notInvagPlug
	//%CombineVols ("subrect", lodata, voxdataflat, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("CompressedRegions.LowThresh_notInvagPlug.mnc", lodata, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &lodata,
                                            &voxdataflat,
                                            &voxdataflat,
                                            &lodata);
   lodata.stretchVoxelValues();
   writeIntermediateVolume(lodata, "CompressedRegions.LowThresh_notInvagPlug");
   
	//Sculpt.py 0 8 0 $ncol 0 $nrow 0 $nslices CompressedRegions.HiThresh.mnc CompressedRegions.LowThresh_notInvagPlug.mnc CompressedRegions.sculpt_noInvagPlug
	//%Sculpt (0, 8, seed, extent, crhdata, lodata, xdim, ydim, zdim);
	//%write_minc ("CompressedRegions.sculpt_noInvagPlug.mnc", crhdata, xdim, ydim, zdim);
	//%delete [] lodata;
   crhdata.sculptVolume(0, &lodata, 8, seed, extent);
   writeIntermediateVolume(crhdata, "CompressedRegions.sculpt_noInvagPlug");

	//CombineVols.py mult CompressedRegions.sculpt_noInvagPlug.mnc SegmentationDeepCore.mnc CompressedRegions_withinCore
	//%CombineVols ("mult", crhdata, sdcdata, sdcdata, xdim, ydim, zdim);
	//%write_minc ("CompressedRegions_withinCore.mnc", crhdata, xdim, ydim, zdim);
	//%delete [] sdcdata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_MULTIPLY,
                                            &crhdata,
                                            &sdcdata,
                                            &sdcdata,
                                            &crhdata);
   crhdata.stretchVoxelValues();
   writeIntermediateVolume(crhdata, "CompressedRegions_withinCore");
   
	//CombineVols.py subrect CompressedRegions_withinCore.mnc SmallPockets.dilate.mnc CompressedCoreRegions_noPockets
	//%CombineVols ("subrect", crhdata, spddata, spddata, xdim, ydim, zdim);
	//%write_minc ("CompressedCoreRegions_noPockets.mnc", crhdata, xdim, ydim, zdim);
	//%delete [] spddata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &crhdata,
                                            &spddata,
                                            &spddata,
                                            &crhdata);
   crhdata.stretchVoxelValues();
   writeIntermediateVolume(crhdata, "CompressedCoreRegions_noPockets");

	//VolMorphOps.py 2 3 CompressedCoreRegions_noPockets.mnc LargeInvaginationCores
	//%DoVolMorphOps (xdim, ydim, zdim, crhdata, 2, 3);
	//%write_minc ("LargeInvaginationCores.mnc", crhdata, xdim, ydim, zdim);
   crhdata.doVolMorphOps(2, 3);
   writeIntermediateVolume(crhdata, "LargeInvaginationCores");
   
	//CombineVols.py subrect LargeInvaginationCores.mnc Segmentation_InvaginationPlugged.mnc LargeInvagination_notSegmentation
	//%unsigned char* licdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) licdata[i]=crhdata[i];
	//%CombineVols ("subrect", crhdata, voxdataflat, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("LargeInvagination_notSegmentation.mnc", crhdata, xdim, ydim, zdim);
	//%delete [] crhdata;
   VolumeFile licdata = crhdata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &crhdata,
                                            &voxdataflat,
                                            &voxdataflat,
                                            &crhdata);
   crhdata.stretchVoxelValues();
   writeIntermediateVolume(crhdata, "LargeInvagination_notSegmentation");
   
	//Sculpt.py 2 10 0 $ncol 0 $nrow 0 $nslices LargeInvaginationCores.mnc Segmentation_InvaginationPlugged.mnc LargeInvaginations.sculpt
	//%Sculpt (2, 10, seed, extent, licdata, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("LargeInvaginations.sculpt.mnc", licdata, xdim, ydim, zdim);
   licdata.sculptVolume(2, &voxdataflat, 10, seed, extent);
   writeIntermediateVolume(licdata, "LargeInvaginations.sculpt");
   
	//Sculpt.py 2 2 0 $ncol 0 $nrow 0 $nslices  LargeInvaginations.sculpt.mnc Segmentation_noEvaginations.mnc LargeInvaginations.sculpt2
	//%Sculpt (2, 2, seed, extent, licdata, snedata, xdim, ydim, zdim);
	//%write_minc ("LargeInvaginations.sculpt2.mnc", licdata, xdim, ydim, zdim);
   licdata.sculptVolume(2, &snedata, 2, seed, extent);
   writeIntermediateVolume(licdata, "LargeInvaginations.sculpt2");
   
	//cp LargeInvaginations.sculpt2.mnc CompositeInvaginations.mnc
	//%write_minc ("CompositeInvaginations.mnc", licdata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches.mnc", licdata, xdim, ydim, zdim);
	//%delete [] snedata;
   writeIntermediateVolume(licdata, "CompositeInvaginations");
   writeIntermediateVolume(licdata, "CompositeAllPatches");

	//CombineVols.py OR Segmentation_noEvaginations_orSubCortInvaginations.mnc LargeInvaginations.sculpt2.mnc Segmentation_LargeInvaginations_noEvaginations
	//%CombineVols ("OR", voxdataflat2, licdata, licdata, xdim, ydim, zdim);
	//%write_minc ("Segmentation_LargeInvaginations_noEvaginations.mnc", voxdataflat2, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat2,
                                            &licdata,
                                            &licdata,
                                            &voxdataflat2);
   writeIntermediateVolume(voxdataflat2, "Segmentation_LargeInvaginations_noEvaginations");

	//FillBiggestObject.py Segmentation_LargeInvaginations_noEvaginations.mnc Segment_PatchedFingers 0 $ncol 0 $nrow 0 $nslices 
	//%FindBiggestObjectWithinMask (voxdataflat2, xdim, ydim, zdim,
	//%	0, xdim, 0, ydim, 0, zdim, seed);
	//%vtkFloodFill (seed, voxdataflat2, 255, 255, 0, xdim, ydim, zdim);
	//%write_minc ("Segment_PatchedFingers.mnc", voxdataflat2, xdim, ydim, zdim);   
	//%unsigned char* spfdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) spfdata[i]=voxdataflat2[i];
   voxdataflat2.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
   voxdataflat2.floodFillWithVTK(seed, 255, 255, 0);
   writeIntermediateVolume(voxdataflat2, "Segment_PatchedFingers");
   VolumeFile spfdata = voxdataflat2;

	//FillHoles.py Segment_PatchedFingers.mnc Segment_PatchedFingers.fill
	//%NewDoFillHoles (xdim, ydim, zdim, voxdataflat2);
	//%write_minc ("Segment_PatchedFingers.fill.mnc", voxdataflat2, xdim, ydim, zdim);
   voxdataflat2.fillSegmentationCavities();
   writeIntermediateVolume(voxdataflat2, "Segment_PatchedFingers.fill");
   
	//cp Segment_PatchedFingers.fill.mnc Segment.BeforePatch.mnc
	//%for ( i=0 ; i<num_voxels ; i++ ) sbpdata[i]=voxdataflat2[i];
	//%write_minc ("Segment.BeforePatch.mnc", voxdataflat2, xdim, ydim, zdim);
	//%delete [] voxdataflat2;
	sbpdata = voxdataflat2;
   writeIntermediateVolume(voxdataflat2, "Segment.BeforePatch");
   
   // Do if use counts
	// EulerSubVolume.py Segment_PatchedFingers.fill.mnc Segment_PatchedFingers.euler 0 $ncol 0 $nrow 0 $nslices
	//countfname = "%s/%s" % (ECDirectory, "Segment_PatchedFingers.euler")
	//NewEulerSubVolume (fname, countfname, extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles)
	
	//CombineVols.py subrect Segment_PatchedFingers.mnc $Input_fname.mnc PatchedFingers
	//%unsigned char* segdata=new unsigned char [num_voxels];
	//%read_minc_file (segfname, segdata);
	//%CombineVols ("subrect", spfdata, segdata, segdata, xdim, ydim, zdim);
	//%write_minc ("PatchedFingers.mnc", spfdata, xdim, ydim, zdim);
   VolumeFile segdata = *segmentationVolume; // NOTE: segmentationVolume is passed to constructor
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &spfdata,
                                            &segdata,
                                            &segdata,
                                            &spfdata);
   spfdata.stretchVoxelValues();
   writeIntermediateVolume(spfdata, "PatchedFingers");
   
	//VolMorphOps.py 2 0 PatchedFingers.mnc PatchedFingers.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, spfdata, 2, 0);
	//%write_minc ("PatchedFingers.dilate.mnc", spfdata, xdim, ydim, zdim);
	//%delete [] spfdata;
   spfdata.doVolMorphOps(2, 0);
   writeIntermediateVolume(spfdata, "PatchedFingers.dilate");
   
	//SET UP FOR MAIN LOOP
	//%unsigned char* rpmdata=new unsigned char [num_voxels];
	//%read_minc_file (rpmfname, rpmdata);
	//%int threshes[8]={70,90,110,120,180,190,210,230}, thresh;
	//%char filename[256];
	//%int j;
	//%for ( j=0 ; j<8 ; j++ ) {
	//%	thresh=int(threshes[j]);
	//%	for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=rpmdata[i];
	//%	ThresholdVolume (voxdataflat, thresh, xdim,ydim,zdim);
	//%	sprintf (filename, "Segmentation.Thresh.%d.mnc", thresh);
	//%	write_minc (filename, voxdataflat, xdim, ydim, zdim);
	//%	if ( thresh <= 110) {
	//%		CombineVols ("subrect", voxdataflat, segdata, segdata, xdim, ydim, zdim);
	//%		sprintf (filename, "SegmentationDiff.%d.mnc", thresh);
	//%		write_minc (filename, voxdataflat, xdim, ydim, zdim);
	//%	}
	//%}
	//%delete [] rpmdata;
   // NOTE: radial position map volume now passed to constructor
   //VolumeFile rpmdata;
   //readIntermediateVolume(rpmdata, rpmfname);
	int threshes[8]={ 70, 90, 110, 120, 180, 190, 210, 230 };
   int thresh;
   QString filename;
   for (int j = 0; j < 8; j++) {
      thresh = threshes[j];
      voxdataflat = *radialPositionMapVolume;  //rpmdata;
      voxdataflat.thresholdVolume(thresh);
      std::ostringstream str;
      str << "Segmentation.Thresh."
          << thresh;
      filename = str.str().c_str();
      writeIntermediateVolume(voxdataflat, filename);
      if (thresh <= 110) {
         VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                                  &voxdataflat,
                                                  &segdata,
                                                  &segdata,
                                                  &voxdataflat);
         voxdataflat.stretchVoxelValues();
         str.str("");
         str << "SegmentationDiff."
             << thresh;
         filename = str.str().c_str();
         writeIntermediateVolume(voxdataflat, filename);
         
      }
   }

	//
	//%unsigned char* nulldata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) nulldata[i]=0;
   VolumeFile nulldata = voxdataflat;
   nulldata.setFileName("");
   nulldata.setAllVoxels(0);
   
	//Initializing composite volumes to null volume
	//%write_minc ("CompositeObjectsPatched.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CompositeUncorrectedObjects.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("UncorrectedObject.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CompositeExoHandlesPatched.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CompositeEndoHandlesPatched.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CumulativeTestObjects.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches.Dilate.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CompositeCavitiesFilled.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CompositeInvaginationPatches.mnc", nulldata, xdim, ydim, zdim);
	//%write_minc ("CorrectedObject.sculpt.mnc", nulldata, xdim, ydim, zdim);
	//%delete [] nulldata;
	//%int NumObjects=0, NumCavities=0, NumHandles=0,CurrentHandles, StartingHandles;
	writeIntermediateVolume(nulldata, "CompositeObjectsPatched");
	writeIntermediateVolume(nulldata, "CompositeUncorrectedObjects");
	writeIntermediateVolume(nulldata, "UncorrectedObject");
	writeIntermediateVolume(nulldata, "CompositeExoHandlesPatched");
	writeIntermediateVolume(nulldata, "CompositeEndoHandlesPatched");
	writeIntermediateVolume(nulldata, "CumulativeTestObjects");
	writeIntermediateVolume(nulldata, "CompositeAllPatches.Dilate");
	writeIntermediateVolume(nulldata, "CompositeCavitiesFilled");
	writeIntermediateVolume(nulldata, "CompositeInvaginationPatches");
	writeIntermediateVolume(nulldata, "CorrectedObject.sculpt");
   int eulerCount      = 0;
	int NumObjects      = 0;
   int NumCavities     = 0;
   int NumHandles      = 0;
   int CurrentHandles  = 0;
   int StartingHandles = 0;
   
	//EulerSubVolume.py Segment.BeforePatch.mnc BeforePatch.euler 0 $ncol 0 $nrow 0 $nslices 
	//%NewEulerSubVolume (sbpdata, "BeforePatch.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%StartingHandles=CurrentHandles=NumHandles;
	//%printf("NumObjects = %d ; NumCavities = %d\n",NumObjects, NumCavities);
	//%printf("CurrentHandles = %d ; StartingHandles = %d\n",CurrentHandles, StartingHandles);
   sbpdata.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                  NumCavities,
                                                  NumHandles,
                                                  eulerCount,
                                                  extent);
   StartingHandles = NumHandles;
   CurrentHandles  = NumHandles;
   
   if (DebugControl::getDebugOn()) {
      std::cout << "NumObjects = " << NumObjects 
                << " ; NumCavities = " << NumCavities
                << std::endl;
      std::cout << "CurrentHandles = " << CurrentHandles
                << " ; StartingHandles = " << StartingHandles
                << std::endl;
   }
   
	//CHANGES 11 AUG IN NEXT 5 STEPS:
	//VolMorphOps.py 1 0 CompressedRegions.sculpt.mnc CompressedRegions.sculpt.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, crsdata, 1, 0);
	//%write_minc ("CompressedRegions.sculpt.dilate.mnc", crsdata, xdim, ydim, zdim);
   crsdata.doVolMorphOps(1, 0);
   writeIntermediateVolume(crsdata, "CompressedRegions.sculpt.dilate");
   
	//VolMorphOps.py 1 0 LargeInvaginations.sculpt2.mnc LargeInvaginations.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, licdata, 1, 0);
	//%write_minc ("LargeInvaginations.dilate.mnc", licdata, xdim, ydim, zdim);
   licdata.doVolMorphOps(1, 0);
   writeIntermediateVolume(licdata, "LargeInvaginations.dilate");

	//CombineVols.py subrect Crossovers.thresh.mnc CompressedRegions.sculpt.dilate.mnc Crossovers_notHighlyCompressed
	//%unsigned char* ctdata=new unsigned char [num_voxels];
	//%read_minc_file ("Crossovers.thresh.mnc", ctdata);
	//%CombineVols ("subrect", ctdata, crsdata, crsdata, xdim, ydim, zdim);
	//%write_minc ("Crossovers_notHighlyCompressed.mnc", ctdata, xdim, ydim, zdim);
	//%delete [] crsdata;
   VolumeFile ctdata;
   readIntermediateVolume(ctdata, "Crossovers.thresh");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &ctdata,
                                            &crsdata,
                                            &crsdata,
                                            &ctdata);
   ctdata.stretchVoxelValues();
   writeIntermediateVolume(ctdata, "Crossovers_notHighlyCompressed");

	//CombineVols.py subrect Crossovers_notHighlyCompressed.mnc Evaginations.dilate.mnc Crossovers_notHighlyCompressedOrEvaginations
	//%CombineVols ("subrect", ctdata, hidata, hidata, xdim, ydim, zdim);
	//%write_minc ("Crossovers_notHighlyCompressedOrEvaginations.mnc", ctdata, xdim, ydim, zdim);
	//%delete [] hidata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &ctdata,
                                            &hidata,
                                            &hidata,
                                            &ctdata);
   ctdata.stretchVoxelValues();
   writeIntermediateVolume(ctdata, "Crossovers_notHighlyCompressedOrEvaginations");

	//CombineVols.py subrect Crossovers_notHighlyCompressedOrEvaginations.mnc LargeInvaginations.dilate.mnc Crossovers_notCompressed_OrEvag_orInvag
	//%CombineVols ("subrect", ctdata, licdata, licdata, xdim, ydim, zdim);
	//%write_minc ("Crossovers_notCompressed_OrEvag_orInvag.mnc", ctdata, xdim, ydim, zdim);
	//%delete [] licdata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &ctdata,
                                            &licdata,
                                            &licdata,
                                            &ctdata);
   ctdata.stretchVoxelValues();
   writeIntermediateVolume(ctdata, "Crossovers_notCompressed_OrEvag_orInvag");

	//Thresh.py Ellipsoid_CompressedOrStretched.mnc CompressedOrStretched.thresh 100
	//%read_minc_file ("Ellipsoid_CompressedOrStretched.mnc", voxdataflat);
	//%ThresholdVolume (voxdataflat, 100, xdim,ydim,zdim);
	//%write_minc ("CompressedOrStretched.thresh.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "Ellipsoid_CompressedOrStretched");
   voxdataflat.thresholdVolume(100);
   writeIntermediateVolume(voxdataflat, "CompressedOrStretched.thresh");
   
	//CombineVols.py subrect Crossovers_notCompressed_OrEvag_orInvag.mnc CompressedOrStretched.thresh.mnc Crossovers_notCompressedOrStretched_OrEvag_orInvag
	//%CombineVols ("subrect", ctdata, voxdataflat, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("Crossovers_notCompressedOrStretched_OrEvag_orInvag.mnc", ctdata, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &ctdata,
                                            &voxdataflat,
                                            &voxdataflat,
                                            &ctdata);
   ctdata.stretchVoxelValues();
   writeIntermediateVolume(ctdata, "Crossovers_notCompressedOrStretched_OrEvag_orInvag");

	//Thresh.py Gauss_neg.mnc Gauss_neg.LowThresh 30
	//%read_minc_file ("Gauss_neg.mnc", voxdataflat);
	//%ThresholdVolume (voxdataflat, 30, xdim,ydim,zdim);
	//%write_minc ("Gauss_neg.LowThresh.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "Gauss_neg");
   voxdataflat.thresholdVolume(30);
   writeIntermediateVolume(voxdataflat, "Gauss_neg.LowThresh");
   
	//CombineVols.py mult Crossovers_notCompressedOrStretched_OrEvag_orInvag.mnc Gauss_neg.LowThresh.mnc Crossovers_GaussNeg
	//%CombineVols ("mult", ctdata, voxdataflat, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("Crossovers_GaussNeg.mnc", ctdata, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_MULTIPLY,
                                            &ctdata,
                                            &voxdataflat,
                                            &voxdataflat,
                                            &ctdata);
   ctdata.stretchVoxelValues();
   writeIntermediateVolume(ctdata, "Crossovers_GaussNeg");
   
	//Thresh.py NearCrossovers.mnc NearCrossovers.thresh 75
	//%read_minc_file ("NearCrossovers.mnc", voxdataflat);
	//%ThresholdVolume (voxdataflat, 75, xdim,ydim,zdim);
	//%write_minc ("NearCrossovers.thresh.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "NearCrossovers");
   voxdataflat.thresholdVolume(75);
   writeIntermediateVolume(voxdataflat, "NearCrossovers.thresh");
   
	//VolMorphOps.py 1 0 NearCrossovers.thresh.mnc NearCrossovers.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, voxdataflat, 1, 0);
	//%write_minc ("NearCrossovers.dilate.mnc", voxdataflat, xdim, ydim, zdim);
	//%delete [] voxdataflat;
   voxdataflat.doVolMorphOps(1, 0);
   writeIntermediateVolume(voxdataflat, "NearCrossovers.dilate");
   
	//VolMorphOps.py 2 2 Crossovers_GaussNeg.mnc TestObjects
	//%DoVolMorphOps (xdim, ydim, zdim, ctdata, 2, 2);
	//%write_minc ("TestObjects.mnc", ctdata, xdim, ydim, zdim);
	//%write_minc ("CompositeTestObjects.mnc", ctdata, xdim, ydim, zdim);
   ctdata.doVolMorphOps(2, 2);
   writeIntermediateVolume(ctdata, "TestObjects");
   writeIntermediateVolume(ctdata, "CompositeTestObjects");
   
	//EulerSubVolume.py TestObjects.mnc TestObjects.euler 0 $ncol 0 $nrow 0 $nslices 
	//%NewEulerSubVolume (ctdata, "TestObjects.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%int TestObjectNum=NumObjects;
	//%printf("***** Number of crossover areas: %d\n", TestObjectNum);
   ctdata.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                 NumCavities,
                                                 NumHandles,
                                                 eulerCount,
                                                 extent);
   int TestObjectNum = NumObjects;

   if (DebugControl::getDebugOn()) {
      std::cout << "**** Number of crossover areas: " << TestObjectNum << std::endl;
   }
   
	// sh -x PATCH.OBJECT.BATCH.sh
	//%if ( StartingHandles + NumObjects > 0 ) {
	//%	ErrorStats es=PatchObjectBatch (ctdata, sbpdata, CurrentHandles, TestObjectNum, lp);
	//%	es.Print();
	//%	CurrentHandles=es.CurrentHandles;
	//%}
   if ((StartingHandles + NumObjects) > 0) {
      ErrorStats es = patchObjectBatch(ctdata, sbpdata, CurrentHandles, TestObjectNum);
      if (DebugControl::getDebugOn()) {
         es.print();
      }
      CurrentHandles = es.getCurrentHandles();
   }

	//cp Segment.BeforePatch.mnc Segment_Corrected.mnc
	//%read_minc_file ("Segment.BeforePatch.mnc", sbpdata);
	//%write_minc ("Segment_Corrected.mnc", sbpdata, xdim, ydim, zdim);
	//%printf("STARTING HANDLES = %d\n", StartingHandles);
	//%printf("FINAL HANDLES = %d\n", CurrentHandles);
	//%if ( CurrentHandles == 0 ) printf("CurrentHandles=0; PATCHING COMPLETE.\n");
   readIntermediateVolume(sbpdata, "Segment.BeforePatch");
   writeIntermediateVolume(sbpdata, "Segment_Corrected");

   if (DebugControl::getDebugOn()) {
      std::cout << "STARTING HANDLES = " << StartingHandles << std::endl;
      std::cout << "FINAL HANDLES = " << CurrentHandles << std::endl;
      if (CurrentHandles == 0) {
         std::cout << "CurrentHandles=0; PATCHING COMPLETE." << std::endl;
      }
   }
   
   //
   // Create the output volume
   //   
   outputVolume = new VolumeFile(sbpdata);
   outputVolume->makeDefaultFileName("Segment_ErrorCorrected");
   outputVolume->setDescriptiveLabel("Segment_ErrorCorrected");

   //
	//Added 5/25/2001:
	//%CombineVols ("subrect", sbpdata, segdata, segdata, xdim, ydim, zdim);
	//%write_minc ("Segment_Corrected_Minus_InitialSegmentation.mnc.mnc", sbpdata, xdim, ydim, zdim);
	//%delete [] segdata;
	//%delete [] sbpdata;
	//%delete [] ctdata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &sbpdata,
                                            &segdata,
                                            &segdata,
                                            &sbpdata);
   sbpdata.stretchVoxelValues();
   writeIntermediateVolume(sbpdata, "Segment_Corrected_Minus_InitialSegmentation");
  
   if (DebugControl::getDebugOn()) {
   	std::cout << "#### CorrectErrors end ####" << std::endl;
   }
   
   return 0;
}




/**
 * Patch an object
 */
BrainModelVolumeSureFitErrorCorrection::ErrorStats 
BrainModelVolumeSureFitErrorCorrection::patchObjectBatch(VolumeFile& rodata, 
                                                         VolumeFile& sbpdata, 
                                                         const int CurrentHandles, 
                                                         const int TestObjectNum)
      throw (BrainModelAlgorithmException)
{
   if (DebugControl::getDebugOn()) {
   	std::cout << "#### PatchObjectBatch begin ####" << std::endl;
   }
   
	//START CYCLE
	ErrorStats es;
	es.setStartingHandles(CurrentHandles);
	es.setTestObjectNum(TestObjectNum);
	es.setCurrentHandles(CurrentHandles);
	es.setRemainingObjectNum(TestObjectNum);

	//if ( true ) return es;
	//%int NumObjects=0, NumCavities=0, NumHandles=0;
	//%int xdim=lp.xdim;
	//%int ydim=lp.ydim;
	//%int zdim=lp.zdim;
	//%int num_voxels=xdim*ydim*zdim;
	//%int i, seed[3]={0,0,0};
	//%int extent[6]={0,xdim,0,ydim,0,zdim}, ctoextent[6]={0,0,0,0,0,0};
   int NumObjects = 0;
   int NumCavities = 0;
   int NumHandles = 0;
   int seed[3] = { 0, 0, 0 };
   int extent[6] = { 0, xDim, 0, yDim, 0, zDim };
   int ctoextent[6] = { 0, 0, 0, 0, 0, 0 };

	//cp TestObjects.mnc RemainingObjects.mnc
	//%write_minc ("RemainingObjects.mnc", rodata, xdim, ydim, zdim);
   writeIntermediateVolume(rodata, "RemainingObjects");
   
	//EulerSubVolume.py RemainingObjects.mnc RemainingObjects.euler 0 $ncol 0 $nrow 0 $nslices 
	//%NewEulerSubVolume (rodata, "RemainingObjects.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%printf("PatchObjectBatch post-euler NumHandles=%d; NumObjects=%d\n", NumHandles, NumObjects);
	//%es.setRemainingObjectNum(NumObjects);
	//%printf("RemainingObjectNum=%d\n", es.RemainingObjectNum);
	//%printf("RemainingObjectNum=%d\n", NumObjects);
   int eulerCount;
   rodata.getEulerCountsForSegmentationSubVolume(NumObjects, 
                                              NumCavities,
                                              NumHandles,
                                              eulerCount,
                                              extent);
   es.setRemainingObjectNum(NumObjects);
   
   if (DebugControl::getDebugOn()) {
      std::cout << "PatchObjectBatch post-euler NumHandles="
                << NumHandles
                << "; NumObjects="
                << NumObjects
                << std::endl;
      std::cout << "RemainingObjectNum=" << es.getRemainingObjectNum() << std::endl;
      std::cout << "RemainingObjectNum=" << NumObjects << std::endl;
   }
   
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices CompositeAllPatches.Dilate.mnc Crossovers.thresh.mnc CompositeAllPatches_CrossoverSculpt
	//%unsigned char* capddata=new unsigned char [num_voxels];
	//%unsigned char* voxdataflat=new unsigned char [num_voxels];
	//%read_minc_file ("CompositeAllPatches.Dilate.mnc", capddata);
	//%for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=capddata[i];
   VolumeFile capddata;
   readIntermediateVolume(capddata, "CompositeAllPatches.Dilate");
   VolumeFile voxdataflat = capddata;
   
	//TO-DO: get vtkfname, errorsfname based on segfname
	//%unsigned char* errorsdata=new unsigned char [num_voxels];
	//%read_minc_file ("Crossovers.thresh.mnc", errorsdata);
	//%Sculpt (0, 2, seed, extent, voxdataflat, errorsdata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches_CrossoverSculpt.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile errorsdata;
   readIntermediateVolume(errorsdata, "Crossovers.thresh");
   voxdataflat.sculptVolume(0, &errorsdata, 2, seed, extent);
   writeIntermediateVolume(voxdataflat, "CompositeAllPatches_CrossoverSculpt");
   
	//CombineVols.py subrect Crossovers.thresh.mnc CompositeAllPatches_CrossoverSculpt.mnc Crossovers_noPatches
	//%CombineVols ("subrect", errorsdata, voxdataflat, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("Crossovers_noPatches.mnc", errorsdata, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &errorsdata,
                                            &voxdataflat,
                                            &voxdataflat,
                                            &errorsdata);
   errorsdata.stretchVoxelValues();
   writeIntermediateVolume(errorsdata, "Crossovers_noPatches");
   
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices CompositeAllPatches.Dilate.mnc NearCrossovers.thresh.mnc CompositeAllPatches_NearCrossoverSculpt
	//%read_minc_file ("NearCrossovers.thresh.mnc", voxdataflat);
	//%Sculpt (0, 2, seed, extent, capddata, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches_NearCrossoverSculpt.mnc", capddata, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "NearCrossovers.thresh");
   capddata.sculptVolume(0, &voxdataflat, 2, seed, extent);
   writeIntermediateVolume(capddata, "CompositeAllPatches_NearCrossoverSculpt");
   
	//CombineVols.py subrect NearCrossovers.thresh.mnc CompositeAllPatches_NearCrossoverSculpt.mnc NearCrossovers_noPatches
	//%CombineVols ("subrect", voxdataflat, capddata, capddata, xdim, ydim, zdim);
	//%write_minc ("NearCrossovers_noPatches.mnc", voxdataflat, xdim, ydim, zdim);
	//%delete [] capddata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &capddata,
                                            &capddata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "NearCrossovers_noPatches");
   

	//while RemainingObjectNum > 0 and CurrentHandles > 0:
	//%FILE *errorlimfile;
	//%unsigned char* ctodata=new unsigned char [num_voxels];
	//%errorlimfile=fopen ("ErrorLimits.xyz", "w");
   const QString errorlimfileName("ErrorLimits.xyz");
   std::ofstream errorlimfile(errorlimfileName.toAscii().constData());
   if (! errorlimfile) {
      throw BrainModelAlgorithmException("Unable to open ErrorLimits.xyz for writing.");
   }
   VolumeFile ctodata;
   
	//resorted to this because couldn't es.set* in subroutines
	//%int TestStats[3]={es.DisconnectFlag,es.CurrentHandles,es.CavitiesPatched};
	//%while ((es.RemainingObjectNum>0) && (es.CurrentHandles>0)) {
	int TestStats[3] = { es.getDisconnectFlag(),
                        es.getCurrentHandles(),
                        es.getCavitiesPatched() };
	while ((es.getRemainingObjectNum() > 0) && (es.getCurrentHandles() > 0)) {
      allowEventsToProcess();
      
      {
         std::ostringstream str;
         str << "Processing handle "
             << es.getCurrentTestNum()
             << " of "
             << es.getStartingHandles();
         if (DebugControl::getDebugOn()) {
            std::cout << str.str() << std::endl;
         }
      }

      if (DebugControl::getDebugOn()) {
   		std::cout << "Iteration: " << es.getCurrentTestNum() << std::endl;
      }
      
		//FillBiggestObject.py RemainingObjects.mnc CurrentTestObject 0 $ncol 0 $nrow 0 $nslices
		//%for ( i=0 ; i<num_voxels ; i++ ) ctodata[i]=rodata[i];
		//%FindBiggestObjectWithinMask (ctodata, xdim, ydim, zdim, 0, xdim, 0, ydim, 0, zdim, seed);
		//%vtkFloodFill (seed, ctodata, 255, 255, 0, xdim, ydim, zdim);
		//%write_minc ("CurrentTestObject.mnc", ctodata, xdim, ydim, zdim);
      ctodata = rodata;
      ctodata.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
      ctodata.floodFillWithVTK(seed, 255, 255, 0);
      writeIntermediateVolume(ctodata, "CurrentTestObject");
      
		//FindLimits.py CurrentTestObject.mnc CurrentTestObject.limits
		//%FindLimits (xdim, ydim, zdim, ctodata, "CurrentTestObject.limits", ctoextent);
		//%fprintf(errorlimfile,"%d-%d, %d-%d, %d-%d\n",ctoextent[0],ctoextent[1],ctoextent[2],ctoextent[3],ctoextent[4],ctoextent[5]);
      ctodata.findLimits("CurrentTestObject.limits",
                         ctoextent);
      errorlimfile << ctoextent[0] << "-" << ctoextent[1] << ", "
                   << ctoextent[2] << "-" << ctoextent[3] << ", "
                   << ctoextent[4] << "-" << ctoextent[5];

		//NewEulerSubVolume.py Segment.BeforePatch.mnc BeforePatch.mask.euler `expr $LimitXmin - 20` `expr $LimitXmin + 20` `expr $LimitYmin - 20` `expr $LimitYmin + 20` `expr $LimitZmin - 20` `expr $LimitZmin + 20`
		//%read_minc_file ("Segment.BeforePatch.mnc", voxdataflat);
		//%ctoextent[1]=ctoextent[0]+20;
		//%ctoextent[3]=ctoextent[2]+20;
		//%ctoextent[5]=ctoextent[4]+20;
		//%ctoextent[0]=ctoextent[0]-20;
		//%ctoextent[2]=ctoextent[2]-20;
		//%ctoextent[4]=ctoextent[4]-20;
		//%NewEulerSubVolume (voxdataflat, "BeforePatch.mask.euler", ctoextent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
		//%es.setMaskHandlesBefore(NumHandles);
		//%es.setMaskCavitiesBefore(NumCavities);
      readIntermediateVolume(voxdataflat, "Segment.BeforePatch");
		ctoextent[1] = ctoextent[0] + 20;
		ctoextent[3] = ctoextent[2] + 20;
		ctoextent[5] = ctoextent[4] + 20;
		ctoextent[0] = ctoextent[0] - 20;
		ctoextent[2] = ctoextent[2] - 20;
		ctoextent[4] = ctoextent[4] - 20;
      voxdataflat.clampVoxelDimension(VolumeFile::VOLUME_AXIS_X, ctoextent[0]);
      voxdataflat.clampVoxelDimension(VolumeFile::VOLUME_AXIS_X, ctoextent[1]);
      voxdataflat.clampVoxelDimension(VolumeFile::VOLUME_AXIS_Y, ctoextent[2]);
      voxdataflat.clampVoxelDimension(VolumeFile::VOLUME_AXIS_Y, ctoextent[3]);
      voxdataflat.clampVoxelDimension(VolumeFile::VOLUME_AXIS_Z, ctoextent[4]);
      voxdataflat.clampVoxelDimension(VolumeFile::VOLUME_AXIS_Z, ctoextent[5]);
      voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                         NumCavities,
                                                         NumHandles,
                                                         eulerCount,
                                                         ctoextent);
		es.setMaskHandlesBefore(NumHandles);
		es.setMaskCavitiesBefore(NumCavities);

		//sh -x PATCH.CYCLE.forOBJECT.sh
		//%PatchCycle (ctodata, errorsdata, sbpdata, ctoextent, TestStats, lp, es);
      patchCycle(ctodata, errorsdata, sbpdata, ctoextent, TestStats, es);
		if ( TestStats[0] == 1 ) {
			es.setDisconnectFlag(1); // no real reason to do this
			es.setCurrentHandles(TestStats[1]);
			es.setCavitiesPatched(TestStats[2]);
		} else {
			//sh -x UNCORRECTED.OBJECT.sh
			//%UncorrectedObject (lp, es);
         uncorrectedObject();
		}
      
		//CombineVols.py subrect RemainingObjects.mnc CorrectedObject.sculpt.mnc RemainingObjects
		//%read_minc_file ("CorrectedObject.sculpt.mnc", voxdataflat);
		//%CombineVols ("subrect", rodata, voxdataflat, voxdataflat, xdim, ydim, zdim);
		//%write_minc ("RemainingObjects.mnc", rodata, xdim, ydim, zdim);
      readIntermediateVolume(voxdataflat, "CorrectedObject.sculpt");
      VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                               &rodata,
                                               &voxdataflat,
                                               &voxdataflat,
                                               &rodata);
      rodata.stretchVoxelValues();
      writeIntermediateVolume(rodata, "RemainingObjects");
      
		//CombineVols.py subrect RemainingObjects.mnc UncorrectedObject.mnc RemainingObjects
		//%read_minc_file ("UncorrectedObject.mnc", voxdataflat);
		//%CombineVols ("subrect", rodata, voxdataflat, voxdataflat, xdim, ydim, zdim);
		//%write_minc ("RemainingObjects.mnc", rodata, xdim, ydim, zdim);
      readIntermediateVolume(voxdataflat, "UncorrectedObject");
      VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                               &rodata,
                                               &voxdataflat,
                                               &voxdataflat,
                                               &rodata);
      rodata.stretchVoxelValues();
      writeIntermediateVolume(rodata, "RemainingObjects");

		//EulerSubVolume.py RemainingObjects.mnc RemainingObjects.euler 0 $ncol 0 $nrow 0 $nslices
		//%NewEulerSubVolume (rodata, "RemainingObjects.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
		//%es.setRemainingObjectNum(NumObjects);
		//%es.setDisconnectFlag(0);
		//%TestStats[0]=0;
		//%es.setCurrentTestNum(es.CurrentTestNum+1);
		//%es.setCumulativeTestNum(es.CumulativeTestNum+1);
      rodata.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                     NumCavities,
                                                     NumHandles,
                                                     eulerCount,
                                                     extent);
      es.setRemainingObjectNum(NumObjects);
      es.setDisconnectFlag(0);
      TestStats[0] = 0;
		es.setCurrentTestNum(es.getCurrentTestNum() + 1);
		es.setCumulativeTestNum(es.getCumulativeTestNum() + 1);

		//EulerSubVolume.py CompositeObjectsPatched.mnc ObjectsPatched.euler 0 $ncol 0 $nrow 0 $nslices 
		//%read_minc_file ("CompositeObjectsPatched.mnc", voxdataflat);
		//%NewEulerSubVolume (voxdataflat, "ObjectsPatched.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
		//%es.setObjectsPatched(NumObjects);
		//%printf("ObjectsPatched=%d\n", es.ObjectsPatched);
      readIntermediateVolume(voxdataflat, "CompositeObjectsPatched");
      voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                     NumCavities,
                                                     NumHandles,
                                                     eulerCount,
                                                     extent);
      es.setObjectsPatched(NumObjects);
      
      if (DebugControl::getDebugOn()) {
         std::cout << "ObjectsPatched=" << es.getObjectsPatched() << std::endl;
      }
      
		//EulerSubVolume.py CompositeUncorrectedObjects.mnc.mnc UncorrectedObjects.euler 0 $ncol 0 $nrow 0 $nslices 
		//%read_minc_file ("CompositeUncorrectedObjects.mnc", voxdataflat);
		//%NewEulerSubVolume (voxdataflat, "UncorrectedObjects.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
		//%es.setUncorrectedObjects(NumObjects);
		//%printf("UncorrectedObjects=%d\n", es.UncorrectedObjects);
      readIntermediateVolume(voxdataflat, "CompositeUncorrectedObjects");
      voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                     NumCavities,
                                                     NumHandles,
                                                     eulerCount,
                                                     extent);
      es.setUncorrectedObjects(NumObjects);

      if (DebugControl::getDebugOn()) {
         std::cout << "UncorrectedObjects=" << es.getUncorrectedObjects() << std::endl;
      }
	}

	//%fclose (errorlimfile);
	//%delete [] ctodata;
	//%delete [] errorsdata;
	//%delete [] voxdataflat;
   errorlimfile.close();
   if (DebugControl::getDebugOn() == false) {
      QFile::remove(errorlimfileName);
   }

   if (DebugControl::getDebugOn()) {
	   std::cout << "#### PatchObjectBatch end ####" << std::endl;
   }
   
	return es;
}

/**
 *
 */
void 
BrainModelVolumeSureFitErrorCorrection::uncorrectedObject()  throw (BrainModelAlgorithmException)
{
   if (DebugControl::getDebugOn()) {
	   std::cout << "#### UncorrectedObject begin ####" << std::endl;
   }
   
	//%int xdim=lp.xdim;
	//%int ydim=lp.ydim;
	//%int zdim=lp.zdim;
	//%int num_voxels=xdim*ydim*zdim;
	//%unsigned char* voxdataflat=new unsigned char [num_voxels];
	//%unsigned char* maskdata=new unsigned char [num_voxels];
   
	//cp CurrentTestObject.mnc UncorrectedObject.mnc
	//%read_minc_file ("CurrentTestObject.mnc", voxdataflat);
	//%write_minc ("UncorrectedObject.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile voxdataflat;
   readIntermediateVolume(voxdataflat, "CurrentTestObject");
   writeIntermediateVolume(voxdataflat, "UncorrectedObject");
   
	//CombineVols.py OR UncorrectedObject.mnc CompositeUncorrectedObjects.mnc CompositeUncorrectedObjects  
	//%read_minc_file ("CompositeUncorrectedObjects.mnc", maskdata);
	//%CombineVols ("OR", maskdata, voxdataflat, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("CompositeUncorrectedObjects.mnc", maskdata, xdim, ydim, zdim);
   VolumeFile maskdata;
   readIntermediateVolume(maskdata, "CompositeUncorrectedObjects");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &voxdataflat,
                                            &voxdataflat,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeUncorrectedObjects");
   
	//%delete [] voxdataflat;
	//%delete [] maskdata;

   if (DebugControl::getDebugOn()) {
	   std::cout << "#### UncorrectedObject end ####" << std::endl;
   }
}

/**
 * Perform a cycle of patching
 */
void 
BrainModelVolumeSureFitErrorCorrection::patchCycle(VolumeFile& ctodata, 
                                                   VolumeFile& errorsdata, 
                                                   VolumeFile& sbpdata, 
                                                   int ctoextent[6], 
                                                   int TestStats[3], 
                                                   ErrorStats es) throw (BrainModelAlgorithmException)
{
   if (DebugControl::getDebugOn()) {
	   std::cout << "#### PatchCycle begin ####" << std::endl;
   }
	if (TestStats[1] == 0) {
      if (DebugControl::getDebugOn()) {
		   std::cout << "CurrentHandles=0; SKIPPING TEST" << std::endl;
      }
		return;
	}
	if (TestStats[0] == 1) {
      if (DebugControl::getDebugOn()) {
		   std::cout << "SKIPPING EXO-HANDLE TEST" << std::endl;
      }
		return;
	}
   if (DebugControl::getDebugOn()) {
   	es.print();
   }
   
	//%int xdim=lp.xdim;
	//%int ydim=lp.ydim;
	//%int zdim=lp.zdim;
	//%int num_voxels=xdim*ydim*zdim;
	//%int i, seed[3]={0,0,0}, extent[6]={0,xdim,0,ydim,0,zdim};
   int seed[3] = { 0, 0, 0 };
   int extent[6] = { 0, xDim, 0, yDim, 0, zDim };

	//%unsigned char* voxdataflat=new unsigned char [num_voxels];
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices CurrentTestObject.mnc Crossovers_noPatches.mnc TestObject.InCrossover
	//%for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=ctodata[i];
	//%Sculpt (0, 2, seed, extent, voxdataflat, errorsdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.InCrossover.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile voxdataflat = ctodata;
   voxdataflat.sculptVolume(0, &errorsdata, 2, seed, extent);
   writeIntermediateVolume(voxdataflat, "TestObject.InCrossover");

	//CombineVols.py mult TestObject.InCrossover.mnc Segment.BeforePatch.mnc TestObject.BeginExoSculpt
	//%CombineVols ("mult", voxdataflat, sbpdata, sbpdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.BeginExoSculpt.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_MULTIPLY,
                                            &voxdataflat,
                                            &sbpdata,
                                            &sbpdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "TestObject.BeginExoSculpt");

	//cp Segmentation.Thresh.190.mnc CurrentSegmentationThresh.mnc
	//%unsigned char* cstdata=new unsigned char [num_voxels];
	//%read_minc_file ("Segmentation.Thresh.190.mnc", cstdata);
	//%write_minc ("CurrentSegmentationThresh.mnc", cstdata, xdim, ydim, zdim);
   VolumeFile cstdata;
   readIntermediateVolume(cstdata, "Segmentation.Thresh.190");
   writeIntermediateVolume(cstdata, "CurrentSegmentationThresh");

	//EXO-HANDLE TEST 1: sh -x PATCH.EXO.HANDLE.sh
	//%int exo_count=1;
	//%PatchExoHandle (sbpdata, ctoextent, TestStats, lp, es,exo_count);
   int exoCount = 1;
   patchExoHandle(sbpdata, ctoextent, TestStats, es, exoCount);
   
	//ALTERNATE BETWEEN ENDO and EXO HANDLES
	//VolMorphOps.py 1 0 CurrentTestObject.mnc CurrentTestObject.dilate
	//%for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=ctodata[i];
	//%DoVolMorphOps (xdim, ydim, zdim, voxdataflat, 1, 0);
	//%write_minc ("CurrentTestObject.dilate.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat = ctodata;
   voxdataflat.doVolMorphOps(1, 0);
   writeIntermediateVolume(voxdataflat, "CurrentTestObject.dilate");
   
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices CurrentTestObject.dilate.mnc NearCrossovers.dilate.mnc TestObject.BeginEndoSculpt.mnc
	//%unsigned char* maskdata=new unsigned char [num_voxels];
	//%read_minc_file ("NearCrossovers.dilate.mnc", maskdata);
	//%Sculpt (0, 2, seed, extent, voxdataflat, maskdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.BeginEndoSculpt.mnc", voxdataflat, xdim, ydim, zdim);
	//%write_minc ("TestObject.NearCrossover.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile maskdata;
   readIntermediateVolume(maskdata, "NearCrossovers.dilate");
   voxdataflat.sculptVolume(0, &maskdata, 2, seed, extent);
   writeIntermediateVolume(voxdataflat, "TestObject.BeginEndoSculpt");
   writeIntermediateVolume(voxdataflat, "TestObject.NearCrossover");

	//CombineVols.py subrect SegmentationDiff.110.mnc CompositeAllPatches.Dilate.mnc CurrentSegmentationDiff.mnc
	//%read_minc_file ("SegmentationDiff.110.mnc", voxdataflat);
	//%read_minc_file ("CompositeAllPatches.Dilate.mnc", maskdata);
	//%CombineVols ("subrect", voxdataflat, maskdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("CurrentSegmentationDiff.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "SegmentationDiff.110");
   readIntermediateVolume(maskdata, "CompositeAllPatches.Dilate");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &maskdata,
                                            &maskdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "CurrentSegmentationDiff");
   
	//ENDO-HANDLE TEST 1: sh -x PATCH.ENDO.HANDLE.sh
	//%int endo_count=1;
	//%PatchEndoHandle (sbpdata, ctoextent, TestStats, lp, es, endo_count);
   int endoCount = 1;
   patchEndoHandle(sbpdata, ctoextent, TestStats, es, endoCount);

	//sh -x PATCH.INVAGINATION.sh
	//%PatchInvagination (sbpdata, TestStats, lp, es);
   patchInvagination(sbpdata, TestStats, es);
   
	//cp Segmentation.Thresh.210.mnc CurrentSegmentationThresh.mnc
	//%read_minc_file ("Segmentation.Thresh.210.mnc", cstdata);
	//%write_minc ("CurrentSegmentationThresh.mnc", cstdata, xdim, ydim, zdim);
   readIntermediateVolume(cstdata, "Segmentation.Thresh.210");
   writeIntermediateVolume(cstdata, "CurrentSegmentationThresh");
   
	//EXO-HANDLE TEST 2: sh -x PATCH.EXO.HANDLE.sh
	//%exo_count++;
	//%PatchExoHandle (sbpdata, ctoextent, TestStats, lp, es, exo_count);
   exoCount++;
   patchExoHandle(sbpdata, ctoextent, TestStats, es, exoCount);
   
	//CombineVols.py subrect SegmentationDiff.90.mnc CompositeAllPatches.Dilate.mnc CurrentSegmentationDiff.mnc
	//%read_minc_file ("SegmentationDiff.90.mnc", voxdataflat);
	//%CombineVols ("subrect", voxdataflat, maskdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("CurrentSegmentationDiff.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "SegmentationDiff.90");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &maskdata,
                                            &maskdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "CurrentSegmentationDiff");
   
	//ENDO-HANDLE TEST 2: sh -x PATCH.ENDO.HANDLE.sh
	//%endo_count++;
	//%PatchEndoHandle (sbpdata, ctoextent, TestStats, lp, es, endo_count);
   endoCount++;
   patchEndoHandle(sbpdata, ctoextent, TestStats, es, endoCount);
   
	//sh -x PATCH.INVAGINATION.sh
	//%PatchInvagination (sbpdata, TestStats, lp, es);
   patchInvagination(sbpdata, TestStats, es);
   
	//EXO-HANDLE TEST 3: sh -x PATCH.EXO.HANDLE.sh
	//%exo_count++;
	//%PatchExoHandle (sbpdata, ctoextent, TestStats, lp, es, exo_count);
   exoCount++;
   patchExoHandle(sbpdata, ctoextent, TestStats, es, exoCount);
   
	//ENDO-HANDLE TEST 3: sh -x PATCH.ENDO.HANDLE.sh
	//%endo_count++;
	//%PatchEndoHandle (sbpdata, ctoextent, TestStats, lp, es, endo_count);
   endoCount++;
   patchEndoHandle(sbpdata, ctoextent, TestStats, es, endoCount);
   
	//sh -x PATCH.INVAGINATION.sh
	//%PatchInvagination (sbpdata, TestStats, lp, es);
   patchInvagination(sbpdata, TestStats, es);
   
	//cp Segmentation.Thresh.230.mnc CurrentSegmentationThresh.mnc
	//%read_minc_file ("Segmentation.Thresh.230.mnc", cstdata);
	//%write_minc ("CurrentSegmentationThresh.mnc", cstdata, xdim, ydim, zdim);
   readIntermediateVolume(cstdata, "Segmentation.Thresh.230");
   writeIntermediateVolume(cstdata, "CurrentSegmentationThresh");
   
	//EXO-HANDLE TEST 4: sh -x PATCH.EXO.HANDLE.sh
	//%exo_count++;
	//%PatchExoHandle (sbpdata, ctoextent, TestStats, lp, es, exo_count);
   exoCount++;
   patchExoHandle(sbpdata, ctoextent, TestStats, es, exoCount);
   
	//CombineVols.py subrect SegmentationDiff.70.mnc CompositeAllPatches.Dilate.mnc CurrentSegmentationDiff.mnc
	//%read_minc_file ("SegmentationDiff.70.mnc", voxdataflat);
	//%CombineVols ("subrect", voxdataflat, maskdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("CurrentSegmentationDiff.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "SegmentationDiff.70");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &maskdata,
                                            &maskdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "CurrentSegmentationDiff");
   
   
	//ENDO-HANDLE TEST 4: sh -x PATCH.ENDO.HANDLE.sh
	//%endo_count++;
	//%PatchEndoHandle (sbpdata, ctoextent, TestStats, lp, es, endo_count);
   endoCount++;
   patchEndoHandle(sbpdata, ctoextent, TestStats, es, endoCount);
   
	//sh -x PATCH.INVAGINATION.sh
	//%PatchInvagination (sbpdata, TestStats, lp, es);
   patchInvagination(sbpdata, TestStats, es);

	//%delete [] cstdata;
	//%delete [] maskdata;
	//%delete [] voxdataflat;

   if (DebugControl::getDebugOn()) {
	   std::cout << "#### PatchCycle end ####" << std::endl;
   }
}

/**
 * patch an "Exo" handle.
 */
void 
BrainModelVolumeSureFitErrorCorrection::patchExoHandle(VolumeFile& sbpdata, 
                                                       int ctoextent[6], 
                                                       int TestStats[3], 
                                                       ErrorStats es, 
                                                       const int exoCount) throw (BrainModelAlgorithmException)
{
   if (DebugControl::getDebugOn()) {
	   std::cout << "#### PatchExoHandle begin ####" << std::endl;
   }
	if (( TestStats[1] == 0) || (TestStats[0] == 1)) {
      if (DebugControl::getDebugOn()) {
   		std::cout << "SKIPPING EXO-HANDLE TEST" << std::endl;
      }
      return;
	}
   if (DebugControl::getDebugOn()) {
	   std::cout << "CurrentTestNum = "
             << es.getCurrentTestNum()
             << "; EXO-HANDLE TEST "
             << exoCount
             << std::endl;
   }
  
   
	//%int NumObjects=0, NumCavities=0, NumHandles=0;
	//%int xdim=lp.xdim;
	//%int ydim=lp.ydim;
	//%int zdim=lp.zdim;
	//%int i, seed[3]={0,0,0}, extent[6]={0,xdim,0,ydim,0,zdim};
	//%int num_voxels=xdim*ydim*zdim;
	//%unsigned char* voxdataflat=new unsigned char [num_voxels];
	//%unsigned char* maskdata=new unsigned char [num_voxels];
   
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices TestObject.BeginExoSculpt.mnc Segment.BeforePatch.mnc TestObject.Exosculpt
	//%read_minc_file ("TestObject.BeginExoSculpt.mnc", voxdataflat);
	//%for ( i=0 ; i<num_voxels ; i++ ) maskdata[i]=sbpdata[i];
	//%Sculpt (0, 2, seed, extent, voxdataflat, maskdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.Exosculpt.mnc", voxdataflat, xdim, ydim, zdim);
	//%unsigned char* toesdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) toesdata[i]=voxdataflat[i];
   VolumeFile voxdataflat;
   readIntermediateVolume(voxdataflat, "TestObject.BeginExoSculpt");
   VolumeFile maskdata = sbpdata;
   int seed[3] = { 0, 0, 0 };
   int extent[6] = { 0, xDim, 0, yDim, 0, zDim };
   voxdataflat.sculptVolume(0, &maskdata, 2, seed, extent);
   writeIntermediateVolume(voxdataflat, "TestObject.Exosculpt");
   VolumeFile toesdata = voxdataflat;
   
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices TestObject.InCrossover.mnc Crossovers_noPatches.mnc TestObject.InCrossover
	//%read_minc_file ("TestObject.InCrossover.mnc", voxdataflat);
	//%read_minc_file ("Crossovers_noPatches.mnc", maskdata);
	//%Sculpt (0, 2, seed, extent, voxdataflat, maskdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.InCrossover.mnc", voxdataflat, xdim, ydim, zdim);
	//%unsigned char* toicdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) toicdata[i]=voxdataflat[i];
   readIntermediateVolume(voxdataflat, "TestObject.InCrossover");
   readIntermediateVolume(maskdata, "Crossovers_noPatches");
   voxdataflat.sculptVolume(0, &maskdata, 2, seed, extent);
   writeIntermediateVolume(voxdataflat, "TestObject.InCrossover");
   VolumeFile toicdata = voxdataflat;
   
	//CombineVols.py OR TestObject.InCrossover.mnc TestObject.Exosculpt.mnc TestObject.InCrossover_Exosculpt
	//%CombineVols ("OR", voxdataflat, toesdata, toesdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.InCrossover_Exosculpt.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &toesdata,
                                            &toesdata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "TestObject.InCrossover_Exosculpt");

	//CombineVols.py subrect TestObject.InCrossover_Exosculpt.mnc CurrentSegmentationThresh.mnc TestObject.Exosculpt.final
	//%read_minc_file ("CurrentSegmentationThresh.mnc", maskdata);
	//%CombineVols ("subrect", voxdataflat, maskdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.Exosculpt.final.mnc", voxdataflat, xdim, ydim, zdim);
	//%unsigned char* toesfdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) toesfdata[i]=voxdataflat[i];
   readIntermediateVolume(maskdata, "CurrentSegmentationThresh");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &maskdata,
                                            &maskdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "TestObject.Exosculpt.final");
   VolumeFile toesfdata = voxdataflat;
   
	//VolMorphOps.py 0 1 TestObject.Exosculpt.final.mnc TestObject.erode
	//DoVolMorphOps (xdim, ydim, zdim, voxdataflat, 0, 1);
	//write_minc ("TestObject.erode.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.doVolMorphOps(0, 1);
   writeIntermediateVolume(voxdataflat, "TestObject.erode");
   
	//VolMorphOps.py 1 0 TestObject.erode.mnc TestObject.erodeDilate
	//%DoVolMorphOps (xdim, ydim, zdim, voxdataflat, 1, 0);
	//%write_minc ("TestObject.erodeDilate.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.doVolMorphOps(1, 0);
   writeIntermediateVolume(voxdataflat, "TestObject.erodeDilate");
   
	//CombineVols.py OR TestObject.InCrossover.mnc TestObject.erodeDilate.mnc TestObject.erodeDilate_InCrossover
	//%CombineVols ("OR", voxdataflat, toicdata, toicdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.erodeDilate_InCrossover.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &toicdata,
                                            &toicdata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "TestObject.erodeDilate_InCrossover");
   
	//FillBiggestObject.py TestObject.erodeDilate_InCrossover.mnc TestObject.BeginExoSculpt 0 $ncol 0 $nrow 0 $nslices
	//%FindBiggestObjectWithinMask (voxdataflat, xdim, ydim, zdim, extent[0],extent[1],extent[2],extent[3],extent[4],extent[5], seed);
	//%vtkFloodFill (seed, voxdataflat, 255, 255, 0, xdim, ydim, zdim);
	//%write_minc ("TestObject.BeginExoSculpt.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
   voxdataflat.floodFillWithVTK(seed, 255, 255, 0);
   writeIntermediateVolume(voxdataflat, "TestObject.BeginExoSculpt");
   
	//CombineVols.py subrect Segment.BeforePatch.mnc TestObject.Exosculpt.final.mnc Segment.AfterPatch
	//%for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=sbpdata[i];
	//%CombineVols ("subrect", voxdataflat, toesfdata, toesfdata, xdim, ydim, zdim);
	//%write_minc ("Segment.AfterPatch.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat = sbpdata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &toesfdata,
                                            &toesfdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "Segment.AfterPatch");
   
	//FillHoles.py Segment.AfterPatch.mnc Segment.AfterPatch.fill
	//%NewDoFillHoles (xdim, ydim, zdim, voxdataflat);
	//%write_minc ("Segment.AfterPatch.fill.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.fillSegmentationCavities();
   writeIntermediateVolume(voxdataflat, "Segment.AfterPatch.fill");
   
	//NewEulerSubVolume.py Segment.AfterPatch.fill.mnc AfterPatch.mask.euler
	//%NewEulerSubVolume (voxdataflat, "AfterPatch.mask.euler", ctoextent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%int MaskHandlesAfter=NumHandles;
	//%printf("MaskHandlesBefore=%d; MaskHandlesAfter=%d\n", es.MaskHandlesBefore, MaskHandlesAfter);
	//%if ( MaskHandlesAfter >= es.MaskHandlesBefore ) {
	//%	printf("EXO-HANDLE TEST %d: NO HANDLE REDUCTION\n", exo_count);
	//%	delete [] toicdata;
	//%	delete [] toesfdata;
	//%	delete [] toesdata;
	//%	delete [] maskdata;
	//%	delete [] voxdataflat;
	//%	return;
	//%}
   int NumObjects, NumCavities, NumHandles, eulerCount;
   voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                   NumCavities,
                                                   NumHandles,
                                                   eulerCount,
                                                   ctoextent);
   int MaskHandlesAfter = NumHandles;
   if (DebugControl::getDebugOn()) {
      std::cout << "MaskHandlesBefore = "
             << es.getMaskHandlesBefore()
             << "; MaskHandlesAfter = "
             << MaskHandlesAfter
             << std::endl;
   }
   if (MaskHandlesAfter >= es.getMaskHandlesBefore()) {
      if (DebugControl::getDebugOn()) {
         std::cout << "EXO-HANDLE TEST "
                   << exoCount
                   << ": NO HANDLE REDUCTION"
                   << std::endl;
      }
      return;
   }
   
	//FillBiggestObject.py Segment.AfterPatch.fill.mnc Segment.AfterPatch.flood 0 $ncol 0 $nrow 0 $nslices
	//%FindBiggestObjectWithinMask (voxdataflat, xdim, ydim, zdim, extent[0],extent[1],extent[2],extent[3],extent[4],extent[5], seed);
	//%vtkFloodFill (seed, voxdataflat, 255, 255, 0, xdim, ydim, zdim);
	//%write_minc ("Segment.AfterPatch.flood.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
   voxdataflat.floodFillWithVTK(seed, 255, 255, 0);
   writeIntermediateVolume(voxdataflat, "Segment.AfterPatch.flood");

	//NewEulerSubVolume.py Segment.AfterPatch.flood.mnc AfterPatch.euler 0 $ncol 0 $nrow 0 $nslices
	//%NewEulerSubVolume (voxdataflat, "AfterPatch.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%if ( NumHandles >= es.CurrentHandles ) {
	//%	printf("FALSE POSITIVE - HANDLES NOT DECREASED\n");
	//%	delete [] toicdata;
	//%	delete [] toesfdata;
	//%	delete [] toesdata;
	//%	delete [] maskdata;
	//%	delete [] voxdataflat;
	//%	return;
	//%}   
   voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                   NumCavities,
                                                   NumHandles,
                                                   eulerCount,
                                                   extent);
   if (NumHandles >= es.getCurrentHandles()) {
      if (DebugControl::getDebugOn()) {
         std::cout << "FALSE POSITIVE - HANDLES NOT DECREASED" << std::endl;
      }
      return;
   }
   
	//cp Segment.AfterPatch.flood.mnc Segment.BeforePatch.mnc
	//%for ( i=0 ; i<num_voxels ; i++ ) sbpdata[i]=voxdataflat[i];
	//%char filename[256];
	//%sprintf (filename, "Segment.BeforePatch.%d.mnc", NumHandles);
	//%write_minc (filename, sbpdata, xdim, ydim, zdim);
	//%write_minc ("Segment.BeforePatch.mnc", sbpdata, xdim, ydim, zdim);
   sbpdata = voxdataflat;
   std::ostringstream str;
   str << "Segment.BeforePatch."
       << NumHandles;
   writeIntermediateVolume(sbpdata, str.str().c_str());
   writeIntermediateVolume(sbpdata, "Segment.BeforePatch");
   
	//cp  TestObject.Exosculpt.mnc HandlePatched.mnc 
	//%write_minc ("HandlePatched.mnc", toesdata, xdim, ydim, zdim);
	//%printf("EXO-HANDLE DISCONNECTED\n");
   writeIntermediateVolume(toesdata, "HandlePatched");
   
   if (DebugControl::getDebugOn()) {
      std::cout << "EXO-HANDLE DISCONNECTED" << std::endl;
   }
   
	//CombineVols.py OR CurrentTestObject.mnc CompositeObjectsPatched.mnc CompositeObjectsPatched
	//%read_minc_file ("CompositeObjectsPatched.mnc", voxdataflat);
	//%read_minc_file ("CurrentTestObject.mnc", maskdata);
	//%CombineVols ("OR", voxdataflat, maskdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("CompositeObjectsPatched.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "CompositeObjectsPatched");
   readIntermediateVolume(maskdata, "CurrentTestObject");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &maskdata,
                                            &maskdata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "CompositeObjectsPatched");

	//VolMorphOps.py 1 0 CurrentTestObject.mnc CorrectedObject.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, maskdata, 1, 0);
	//%write_minc ("CorrectedObject.dilate.mnc", maskdata, xdim, ydim, zdim);
   maskdata.doVolMorphOps(1, 0);
   writeIntermediateVolume(maskdata, "CorrectedObject.dilate");
   
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices CorrectedObject.dilate.mnc RemainingObjects.mnc CorrectedObject.sculpt
	//%read_minc_file ("RemainingObjects.mnc", voxdataflat);
	//%Sculpt (0, 2, seed, extent, maskdata, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("CorrectedObject.sculpt.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "RemainingObjects");
   maskdata.sculptVolume(0, &voxdataflat, 2, seed, extent);
   writeIntermediateVolume(maskdata, "CorrectedObject.sculpt");

	//CombineVols.py OR HandlePatched.mnc CompositeExoHandlesPatched.mnc CompositeExoHandlesPatched.mnc
	//%read_minc_file ("CompositeExoHandlesPatched.mnc", maskdata);
	//%CombineVols ("OR", maskdata, toesdata, toesdata, xdim, ydim, zdim);
	//%write_minc ("CompositeExoHandlesPatched.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeExoHandlesPatched");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &toesdata,
                                            &toesdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeExoHandlesPatched");

	//CombineVols.py OR HandlePatched.mnc CompositeAllPatches.mnc CompositeAllPatches.mnc 
	//%read_minc_file ("CompositeAllPatches.mnc", maskdata);
	//%CombineVols ("OR", maskdata, toesdata, toesdata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeAllPatches");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &toesdata,
                                            &toesdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeAllPatches");

	//VolMorphOps.py 2 0 HandlePatched.mnc HandlePatched.Dilate
	//%DoVolMorphOps (xdim, ydim, zdim, toesdata, 2, 0);
	//%write_minc ("HandlePatched.dilate.mnc", toesdata, xdim, ydim, zdim);
   toesdata.doVolMorphOps(2, 0);
   writeIntermediateVolume(toesdata, "HandlePatched.dilate");
   
	//CombineVols.py OR HandlePatched.Dilate.mnc CompositeAllPatches.Dilate.mnc CompositeAllPatches.Dilate.mnc 
	//%read_minc_file ("CompositeAllPatches.Dilate.mnc", maskdata);
	//%CombineVols ("OR", maskdata, toesdata, toesdata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches.Dilate.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeAllPatches.Dilate");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &toesdata,
                                            &toesdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeAllPatches.Dilate");
   
	//%TestStats[1]=NumHandles;
	//%TestStats[0]=1;
	//%delete [] toicdata;
	//%delete [] toesfdata;
	//%delete [] toesdata;
	//%delete [] maskdata;
	//%delete [] voxdataflat;

	TestStats[1] = NumHandles;
	TestStats[0] = 1;

   if (DebugControl::getDebugOn()) {
	   std::cout << "#### PatchExoHandle end ####" << std::endl;
   }
}

/**
 * Patch an "endo" handle.
 */
void 
BrainModelVolumeSureFitErrorCorrection::patchEndoHandle(VolumeFile& sbpdata, 
                                                        int ctoextent[6], 
                                                        int TestStats[3], 
                                                        ErrorStats es, 
                                                        const int endoCount) throw (BrainModelAlgorithmException)
{
   if (DebugControl::getDebugOn()) {
	   std::cout << "#### PatchEndoHandle begin ####" << std::endl;
   }
	if (( TestStats[1] == 0) || (TestStats[0] == 1)) {
      if (DebugControl::getDebugOn()) {
   		std::cout << "SKIPPING ENDO-HANDLE TEST" << std::endl;
      }
		return;
	}
   
   if (DebugControl::getDebugOn()) {
	   std::cout << "CurrentTestNum=" << es.getCurrentTestNum()
                << "; ENDO-HANDLE TEST " << endoCount << std::endl;
   }
   
	//%int NumObjects=0, NumCavities=0, NumHandles=0;
	//%int xdim=lp.xdim;
	//%int ydim=lp.ydim;
	//%int zdim=lp.zdim;
	//%int i, seed[3]={0,0,0}, extent[6]={0,xdim,0,ydim,0,zdim};
	//%int num_voxels=xdim*ydim*zdim;
	//%unsigned char* voxdataflat=new unsigned char [num_voxels];
	//%unsigned char* maskdata=new unsigned char [num_voxels];
	//%unsigned char* toesdata=new unsigned char [num_voxels];
   
	//Sculpt.py 0 2 0  $ncol 0 $nrow 0 $nslices   TestObject.BeginEndoSculpt.mnc CurrentSegmentationDiff.mnc TestObject.Endosculpt
	//%read_minc_file ("TestObject.BeginEndoSculpt.mnc", toesdata);
	//%read_minc_file ("CurrentSegmentationDiff.mnc", maskdata);
	//%Sculpt (0, 2, seed, extent, toesdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.Endosculpt.mnc", toesdata, xdim, ydim, zdim);
   VolumeFile toesdata;
   readIntermediateVolume(toesdata, "TestObject.BeginEndoSculpt");
   VolumeFile maskdata;
   readIntermediateVolume(maskdata, "CurrentSegmentationDiff");
   int extent[6] = { 0, xDim, 0, yDim, 0, zDim };
   int seed[3] = { 0, 0, 0 };
   toesdata.sculptVolume(0, &maskdata, 2, seed, extent);
   writeIntermediateVolume(toesdata, "TestObject.Endosculpt");
   
	//Sculpt.py 0 6 0 $ncol 0 $nrow 0 $nslices TestObject.NearCrossover.mnc NearCrossovers_noPatches.mnc TestObject.NearCrossoverSculpt
	//%read_minc_file ("TestObject.NearCrossover.mnc", voxdataflat);
	//%read_minc_file ("NearCrossovers_noPatches.mnc", maskdata);
	//%Sculpt (0, 6, seed, extent, voxdataflat, maskdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.NearCrossoverSculpt.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile voxdataflat;
   readIntermediateVolume(voxdataflat, "TestObject.NearCrossover");
   readIntermediateVolume(maskdata, "NearCrossovers_noPatches");
   voxdataflat.sculptVolume(0, &maskdata, 6, seed, extent);
   writeIntermediateVolume(voxdataflat, "TestObject.NearCrossoverSculpt");

	//cp TestObject.NearCrossoverSculpt.mnc TestObject.NearCrossover.mnc
	//write_minc ("TestObject.NearCrossover.mnc", voxdataflat, xdim, ydim, zdim);
   writeIntermediateVolume(voxdataflat, "TestObject.NearCrossover");
   
	//CombineVols.py OR TestObject.NearCrossoverSculpt.mnc TestObject.Endosculpt.mnc TestObject.BeginEndoSculpt
	//%CombineVols ("OR", voxdataflat, toesdata, toesdata, xdim, ydim, zdim);
	//%write_minc ("TestObject.BeginEndoSculpt.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &toesdata,
                                            &toesdata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "TestObject.BeginEndoSculpt");
   
	//CombineVols.py OR TestObject.Endosculpt.mnc Segment.BeforePatch.mnc Segment.AfterPatch
	//%for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=toesdata[i];
	//%CombineVols ("OR", voxdataflat, sbpdata, sbpdata, xdim, ydim, zdim);
	//%write_minc ("Segment.AfterPatch.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat = toesdata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &sbpdata,
                                            &sbpdata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "Segment.AfterPatch");
   
	//FillHoles.py Segment.AfterPatch.mnc Segment.AfterPatch.fill
	//%NewDoFillHoles (xdim, ydim, zdim, voxdataflat);
	//%write_minc ("Segment.AfterPatch.fill.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.fillSegmentationCavities();
   writeIntermediateVolume(voxdataflat, "Segment.AfterPatch.fill");
   
	//NewEulerSubVolume.py Segment.AfterPatch.fill.mnc AfterPatch.mask.euler x1 x2 y1 y2 z1 z2
	//%NewEulerSubVolume (voxdataflat, "AfterPatch.mask.euler", ctoextent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%int MaskHandlesAfter=NumHandles;
	//%printf("MaskHandlesBefore=%d; MaskHandlesAfter=%d\n", es.MaskHandlesBefore, MaskHandlesAfter);
	//%if ( MaskHandlesAfter >= es.MaskHandlesBefore ) {
	//%	printf("ENDO-HANDLE TEST %d: NO HANDLE REDUCTION\n", endo_count);
	//%	delete [] toesdata;
	//%	delete [] maskdata;
	//%	delete [] voxdataflat;
	//%	return;
	//%}
   int NumObjects, NumCavities, NumHandles, eulerCount;
   voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                   NumCavities,
                                                   NumHandles,
                                                   eulerCount,
                                                   ctoextent);
   const int MaskHandlesAfter = NumHandles;
   if (DebugControl::getDebugOn()) {
      std::cout << "MaskHandlesBefore = "
             << es.getMaskHandlesBefore()
             << "; MaskHandlesAfter = "
             << MaskHandlesAfter
             << std::endl;
   }
   if (MaskHandlesAfter >= es.getMaskHandlesBefore()) {
      if (DebugControl::getDebugOn()) {
         std::cout << "ENDO-HANDLE TEST "
                << endoCount
                << ": NO HANDLE REDUCTION"
                << std::endl;
      }
      return;
   }
   
	//FillBiggestObject.py Segment.AfterPatch.fill.mnc Segment.AfterPatch.flood 0 $ncol 0 $nrow 0 $nslices
	//%FindBiggestObjectWithinMask (voxdataflat, xdim, ydim, zdim, extent[0],extent[1],extent[2],extent[3],extent[4],extent[5], seed);
	//%vtkFloodFill (seed, voxdataflat, 255, 255, 0, xdim, ydim, zdim);
	//%write_minc ("Segment.AfterPatch.flood.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
   voxdataflat.floodFillWithVTK(seed, 255, 255, 0);
   writeIntermediateVolume(voxdataflat, "Segment.AfterPatch.flood");
   
	//EulerSubVolume.py Segment.AfterPatch.flood.mnc AfterPatch.euler 0 $ncol 0 $nrow 0 $nslices
	//%NewEulerSubVolume (voxdataflat, "AfterPatch.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%if ( NumHandles >= es.CurrentHandles ) {
	//%	printf("FALSE POSITIVE - HANDLES NOT DECREASED\n");
	//%	delete [] toesdata;
	//%	delete [] maskdata;
	//%	delete [] voxdataflat;
	//%	return;
	//%}
   voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                   NumCavities,
                                                   NumHandles,
                                                   eulerCount,
                                                   extent);
	if ( NumHandles >= es.getCurrentHandles() ) {
      if (DebugControl::getDebugOn()) {
	   	std::cout << "FALSE POSITIVE - HANDLES NOT DECREASED" << std::endl;
      }
      return;
   }

	//cp Segment.AfterPatch.flood.mnc Segment.BeforePatch.mnc
	//%for ( i=0 ; i<num_voxels ; i++ ) sbpdata[i]=voxdataflat[i];
	//%char filename[256];
	//%sprintf (filename, "Segment.BeforePatch.%d.mnc", NumHandles);
	//%write_minc (filename, sbpdata, xdim, ydim, zdim);
	//%write_minc ("Segment.BeforePatch.mnc", sbpdata, xdim, ydim, zdim);
	//%printf("ENDO-HANDLE FILLED\n");
   sbpdata = voxdataflat;
   std::ostringstream str;
   str << "Segment.BeforePatch."
       << NumHandles;
   writeIntermediateVolume(sbpdata, str.str().c_str());
   writeIntermediateVolume(sbpdata, "Segment.BeforePatch");
   
   if (DebugControl::getDebugOn()) {
      std::cout << "ENDO-HANDLE FILLED" << std::endl;
   }
   
	//CombineVols.py OR CurrentTestObject.mnc CompositeObjectsPatched.mnc CompositeObjectsPatched
	//%read_minc_file ("CompositeObjectsPatched.mnc", voxdataflat);
	//%read_minc_file ("CurrentTestObject.mnc", maskdata);
	//%CombineVols ("OR", voxdataflat, maskdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("CompositeObjectsPatched.mnc", voxdataflat, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "CompositeObjectsPatched");
   readIntermediateVolume(maskdata, "CurrentTestObject");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &maskdata,
                                            &maskdata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "CompositeObjectsPatched");

	//VolMorphOps.py 1 0 CurrentTestObject.mnc CorrectedObject.dilate
	//%DoVolMorphOps (xdim, ydim, zdim, maskdata, 1, 0);
	//%write_minc ("CorrectedObject.dilate.mnc", maskdata, xdim, ydim, zdim);
   maskdata.doVolMorphOps(1, 0);
   writeIntermediateVolume(maskdata, "CorrectedObject.dilate");
   
	//Sculpt.py 0 2 0 $ncol 0 $nrow 0 $nslices CorrectedObject.dilate.mnc RemainingObjects.mnc CorrectedObject.sculpt
	//%read_minc_file ("RemainingObjects.mnc", voxdataflat);
	//%Sculpt (0, 2, seed, extent, maskdata, voxdataflat, xdim, ydim, zdim);
	//%write_minc ("CorrectedObject.sculpt.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(voxdataflat, "RemainingObjects");
   maskdata.sculptVolume(0, &voxdataflat, 2, seed, extent);
   writeIntermediateVolume(maskdata, "CorrectedObject.sculpt");

	//cp TestObject.Endosculpt.mnc HandlePatched.mnc 
	//%write_minc ("HandlePatched.mnc", toesdata, xdim, ydim, zdim);
   writeIntermediateVolume(toesdata, "HandlePatched");

	//CombineVols.py OR HandlePatched.mnc CompositeEndoHandlesPatched.mnc CompositeEndoHandlesPatched.mnc
	//%read_minc_file ("CompositeEndoHandlesPatched.mnc", maskdata);
	//%CombineVols ("OR", maskdata, toesdata, toesdata, xdim, ydim, zdim);
	//%write_minc ("CompositeEndoHandlesPatched.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeEndoHandlesPatched");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &toesdata,
                                            &toesdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeEndoHandlesPatched");
   
	//CombineVols.py OR HandlePatched.mnc CompositeAllPatches.mnc CompositeAllPatches.mnc 
	//%read_minc_file ("CompositeAllPatches.mnc", maskdata);
	//%CombineVols ("OR", maskdata, toesdata, toesdata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeAllPatches");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &toesdata,
                                            &toesdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeAllPatches");

	//VolMorphOps.py 2 0 HandlePatched.mnc HandlePatched.Dilate
	//%DoVolMorphOps (xdim, ydim, zdim, toesdata, 2, 0);
	//%write_minc ("HandlePatched.dilate.mnc", toesdata, xdim, ydim, zdim);
   toesdata.doVolMorphOps(2, 0);
   writeIntermediateVolume(toesdata, "HandlePatched.dilate");

	//CombineVols.py OR HandlePatched.Dilate.mnc CompositeAllPatches.Dilate.mnc CompositeAllPatches.Dilate.mnc 
	//%read_minc_file ("CompositeAllPatches.Dilate.mnc", maskdata);
	//%CombineVols ("OR", maskdata, toesdata, toesdata, xdim, ydim, zdim);
   //%write_minc ("CompositeAllPatches.Dilate.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeAllPatches.Dilate");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &toesdata,
                                            &toesdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeAllPatches.Dilate");

	//%TestStats[1]=NumHandles;
	//%TestStats[0]=1;
	//%delete [] toesdata;
	//%delete [] maskdata;
	//%delete [] voxdataflat;
   
   TestStats[1] = NumHandles;
   TestStats[2] = 1;

   if (DebugControl::getDebugOn()) {
   	std::cout << "#### PatchEndoHandle end ####" << std::endl;
   }
}

/**
 * Patch invaginations
 */
void 
BrainModelVolumeSureFitErrorCorrection::patchInvagination(VolumeFile& sbpdata, 
                                                          int TestStats[3], 
                                                          ErrorStats es) throw (BrainModelAlgorithmException)
{
	if (( TestStats[1] == 0) || (TestStats[0] == 1)) {
      if (DebugControl::getDebugOn()) {
   		std::cout << "SKIPPING INVAGINATION TEST" << std::endl;
      }
		return;
	}
   if (DebugControl::getDebugOn()) {
	   std::cout << "#### PatchInvagination begin ####" << std::endl;
   }
   
	//%int seed[3]={0,0,0}, extent[6]={0,xdim,0,ydim,0,zdim};
	//%int num_voxels=xdim*ydim*zdim;
	//%unsigned char* voxdataflat=new unsigned char [num_voxels];
	//%unsigned char* maskdata=new unsigned char [num_voxels];
	//%unsigned char* sapdata=new unsigned char [num_voxels];
	//FillBiggestObject.py Segment.AfterPatch.mnc Segment.AfterPatch.flood 0 $ncol 0 $nrow 0 $nslices
	//%for ( i=0 ; i<num_voxels ; i++ ) voxdataflat[i]=sapdata[i];
	//%FindBiggestObjectWithinMask (voxdataflat, xdim, ydim, zdim, extent[0],extent[1],extent[2],extent[3],extent[4],extent[5], seed);
	//vtkFloodFill (seed, voxdataflat, 255, 255, 0, xdim, ydim, zdim);
	//%writeIntermediateVolume("Segment.AfterPatch.flood.mnc", voxdataflat, xdim, ydim, zdim);
   int extent[6] = { 0, xDim, 0, yDim, 0, zDim };
   VolumeFile sapdata;
	readIntermediateVolume(sapdata, "Segment.AfterPatch");
   VolumeFile voxdataflat = sapdata;
   VolumeFile::VoxelIJK seed;
   voxdataflat.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
   voxdataflat.floodFillWithVTK(seed, 255, 255, 0);
	writeIntermediateVolume(voxdataflat, "Segment.AfterPatch.flood");
   
	//EulerSubVolume.py Segment.AfterPatch.flood.mnc AfterPatch.euler 0 $ncol 0 $nrow 0 $nslices
	//%NewEulerSubVolume (voxdataflat, "AfterPatch.euler", extent, xdim, ydim, zdim, &NumObjects, &NumCavities, &NumHandles);
	//%int MaskCavitiesAfter=NumCavities;
	//%printf("MaskCavitiesBefore=%d; MaskCavitiesAfter=%d\n", es.MaskCavitiesBefore, MaskCavitiesAfter);
	//%if ( MaskCavitiesAfter <= es.MaskCavitiesBefore ) {
	//%	printf("CAVITIES UNCHANGED\n");
	//%	delete [] sapdata;
	//%	delete [] maskdata;
	//%	delete [] voxdataflat;
	//%	return;
	//%}
	int NumObjects = 0;
   int NumCavities = 0;
   int NumHandles = 0;
   int eulerCount = 0;
   voxdataflat.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                    NumCavities,
                                                    NumHandles,
                                                    eulerCount,
                                                    extent);
	int MaskCavitiesAfter = NumCavities;
   if (DebugControl::getDebugOn()) {
   	std::cout << "MaskCavitiesBefore = " << es.getMaskCavitiesBefore() 
                << "; MaskCavitiesAfter = " << MaskCavitiesAfter << std::endl;
   }
	if (MaskCavitiesAfter <= es.getMaskCavitiesBefore() ) {
      if (DebugControl::getDebugOn()) {
   		std::cout << "CAVITIES UNCHANGED" << std::endl;
      }
		return;
	}


	//FillHoles.py Segment.AfterPatch.flood.mnc  Segment.AfterPatch.fill
	//%NewDoFillHoles (xdim, ydim, zdim, voxdataflat);
	//%write_minc ("Segment.AfterPatch.fill.mnc", voxdataflat, xdim, ydim, zdim);
	//%unsigned char* sapfdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) sapfdata[i]=voxdataflat[i];
   voxdataflat.fillSegmentationCavities();
   writeIntermediateVolume(voxdataflat, "Segment.AfterPatch.fill");
   VolumeFile sapfdata = voxdataflat;

	//CombineVols.py subrect Segment.AfterPatch.fill.mnc Segment.AfterPatch.mnc TestCavityFilled
	//%CombineVols ("subrect", voxdataflat, sapdata, maskdata, xdim, ydim, zdim);
	//%write_minc ("TestCavityFilled.mnc", voxdataflat, xdim, ydim, zdim);
	//%delete [] sapdata;
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
                                            &voxdataflat,
                                            &sapdata,
                                            NULL, // maskdata,
                                            &voxdataflat);
   voxdataflat.stretchVoxelValues();
   writeIntermediateVolume(voxdataflat, "TestCavityFilled");

	//FillBiggestObject.py TestCavityFilled.mnc TestCavityFilled.flood 0 $ncol 0 $nrow 0 $nslices
	//FindBiggestObjectWithinMask (voxdataflat, xdim, ydim, zdim, extent[0],extent[1],extent[2],extent[3],extent[4],extent[5], seed);
	//vtkFloodFill (seed, voxdataflat, 255, 255, 0, xdim, ydim, zdim);
	//write_minc ("TestCavityFilled.flood.mnc", voxdataflat, xdim, ydim, zdim);
   voxdataflat.findBiggestObjectWithinMask(extent, 255.0, 255.0, seed);
   voxdataflat.floodFillWithVTK(seed, 255, 255, 0);
   writeIntermediateVolume(voxdataflat, "TestCavityFilled.flood");

	//FindLimits.py TestCavityFilled.flood.mnc TestCavityLimits
	//%int limits[6]={0,0,0,0,0,0};
	//%FindLimits (xdim, ydim, zdim, voxdataflat, "CurrentTestObject.limits", limits);
	//%int CavitySize=limits[1]-limits[0]+limits[3]-limits[2]+limits[5]-limits[4];
	//%if ( CavitySize <= 10) {
	//%	printf("PATCHED CAVITY LESS THAN 10; NOT ACCEPTED\n");
	//%	delete [] sapfdata;
	//%	delete [] maskdata;
	//%	delete [] voxdataflat;
	//%	return;
	//%}
	//%printf("INVAGINATION PATCHED\n");
   int limits[6] = { 0, 0, 0, 0, 0, 0 };
   voxdataflat.findLimits("CurrentTestObject.limits", limits);
	int CavitySize=limits[1]-limits[0]+limits[3]-limits[2]+limits[5]-limits[4];
	if ( CavitySize <= 10) {
      if (DebugControl::getDebugOn()) {
		   std::cout << "PATCHED CAVITY LESS THAN 10; NOT ACCEPTED" << std::endl;
      }
		return;
	}

   if (DebugControl::getDebugOn()) {
   	std::cout << "INVAGINATION PATCHED" << std::endl;
   }
   
	//cp Segment.AfterPatch.fill.mnc Segment.BeforePatch.mnc
	//%for ( i=0 ; i<num_voxels ; i++ ) sbpdata[i]=sapfdata[i];
	//%char filename[256];
	//%sprintf (filename, "Segment.BeforePatch.%d.mnc", NumHandles);
	//%write_minc (filename, sbpdata, xdim, ydim, zdim);
	//%write_minc ("Segment.BeforePatch.mnc", sbpdata, xdim, ydim, zdim);
	//%delete [] sapfdata;
   sbpdata = sapfdata;
   std::ostringstream str;
   str << "Segment.BeforePatch" << NumHandles;
   writeIntermediateVolume(sbpdata, str.str().c_str());
   writeIntermediateVolume(sbpdata, "Segment.BeforePatch");
   
	//cp TestCavityFilled.flood.mnc InvaginationPatched.mnc
	//%unsigned char* tcffdata=new unsigned char [num_voxels];
	//%for ( i=0 ; i<num_voxels ; i++ ) tcffdata[i]=voxdataflat[i];
	//%write_minc ("InvaginationPatched.mnc", tcffdata, xdim, ydim, zdim);
   VolumeFile tcffdata = voxdataflat;
   writeIntermediateVolume(tcffdata, "InvaginationPatched");
   
	//CombineVols.py OR CurrentTestObject.mnc CompositeObjectsPatched.mnc CompositeObjectsPatched
	//read_minc_file ("CompositeObjectsPatched.mnc", voxdataflat);
	//read_minc_file ("CurrentTestObject.mnc", maskdata);
	//CombineVols ("OR", voxdataflat, maskdata, maskdata, xdim, ydim, zdim);
	//write_minc ("CompositeObjectsPatched.mnc", voxdataflat, xdim, ydim, zdim);
   VolumeFile maskdata;
   readIntermediateVolume(voxdataflat, "CompositeObjectsPatched");
   readIntermediateVolume(maskdata, "CurrentTestObject");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &voxdataflat,
                                            &maskdata,
                                            &maskdata,
                                            &voxdataflat);
   writeIntermediateVolume(voxdataflat, "CompositeObjectsPatched");

	//CombineVols.py OR InvaginationPatched.mnc CompositeInvaginationPatches.mnc CompositeInvaginationPatches.mnc
	//%read_minc_file ("CompositeInvaginationPatches.mnc", maskdata);
	//%CombineVols ("OR", maskdata, tcffdata, tcffdata, xdim, ydim, zdim);
	//%write_minc ("CompositeInvaginationPatches.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeInvaginationPatches");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &tcffdata,
                                            &tcffdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeInvaginationPatches");
   
	//CombineVols.py OR TestCavityFilled.mnc CompositeCavitiesFilled.mnc CompositeCavitiesFilled.mnc
	//%read_minc_file ("CompositeCavitiesFilled.mnc", maskdata);
	//%CombineVols ("OR", maskdata, tcffdata, tcffdata, xdim, ydim, zdim);
	//%write_minc ("CompositeCavitiesFilled.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeCavitiesFilled");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &tcffdata,
                                            &tcffdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeCavitiesFilled");

	//CombineVols.py OR TestCavityFilled.mnc CompositeAllPatches.mnc CompositeAllPatches.mnc 
	//%read_minc_file ("CompositeAllPatches.mnc", maskdata);
	//%CombineVols ("OR", maskdata, tcffdata, tcffdata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeAllPatches");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &tcffdata,
                                            &tcffdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeAllPatches");
   
	//VolMorphOps.py 2 0 TestCavityFilled.mnc CavityFilled.Dilate
	//%DoVolMorphOps (xdim, ydim, zdim, tcffdata, 2, 0);
	//%write_minc ("CavityFilled.Dilate.mnc", tcffdata, xdim, ydim, zdim);
   tcffdata.doVolMorphOps(2, 0);
   writeIntermediateVolume(tcffdata, "CavityFilled.Dilate");
   
	//CombineVols.py OR CavityFilled.Dilate.mnc CompositeAllPatches.Dilate.mnc CompositeAllPatches.Dilate.mnc 
	//%read_minc_file ("CompositeAllPatches.Dilate.mnc", maskdata);
	//%CombineVols ("OR", maskdata, tcffdata, tcffdata, xdim, ydim, zdim);
	//%write_minc ("CompositeAllPatches.Dilate.mnc", maskdata, xdim, ydim, zdim);
   readIntermediateVolume(maskdata, "CompositeAllPatches.Dilate");
   VolumeFile::performMathematicalOperation(VolumeFile::VOLUME_MATH_OPERATION_OR,
                                            &maskdata,
                                            &tcffdata,
                                            &tcffdata,
                                            &maskdata);
   writeIntermediateVolume(maskdata, "CompositeAllPatches.Dilate");
   
	//%TestStats[2]=es.CavitiesPatched+MaskCavitiesAfter-es.MaskCavitiesBefore;
	//%TestStats[0]=1; //DisconnectFlag true
	//%delete [] tcffdata;
	//%delete [] maskdata;
	//%delete [] voxdataflat;
 
   TestStats[2] = es.getCavitiesPatched()
                + MaskCavitiesAfter
                - es.getMaskCavitiesBefore();
	TestStats[0] = 1; //DisconnectFlag true

   if (DebugControl::getDebugOn()) {
   	std::cout << "#### PatchInvagination end ####" << std::endl;
   }
}

/**
 * read a volume.
 */
void 
BrainModelVolumeSureFitErrorCorrection::readIntermediateVolume(VolumeFile* vf,
                const QString& nameIn) throw (BrainModelAlgorithmException)
{
   readIntermediateVolume(*vf, nameIn);
}
                         
/**
 * read a volume.
 */
void 
BrainModelVolumeSureFitErrorCorrection::readIntermediateVolume(VolumeFile& vf,
                const QString& nameIn) throw (BrainModelAlgorithmException)
{
   if (keepIntermediateFilesInMemoryFlag) {
      //
      // Make sure volume is in memory
      //
      std::map<QString,VolumeFile*>::iterator iter = 
         intermediateVolumeFilesInMemory.find(nameIn);
      if (iter == intermediateVolumeFilesInMemory.end()) {
         throw BrainModelAlgorithmException(
            "PROGRAM ERROR: Unable to find volume named "
            + nameIn
            + " in intermediate volume files in memory.");
      }
      
      //
      // copy the volume to output
      //
      vf = *(iter->second);
      
      return;
   }
   
   try {
      QString name;
      QDir intermedDir(intermediateFilesSubDirectory);
      if (intermedDir.exists()) {
         name.append(intermediateFilesSubDirectory);
         name.append("/");
      }
      name.append(nameIn);
      name.append("+orig");
      switch (typeOfVolumeFilesToWrite) {
         case VolumeFile::FILE_READ_WRITE_TYPE_RAW:
            throw FileException("ERROR: Intermediate volume wants to be read in RAW");
            break;
         case VolumeFile::FILE_READ_WRITE_TYPE_AFNI:
           name.append(SpecFile::getAfniVolumeFileExtension());
            break;
         case VolumeFile::FILE_READ_WRITE_TYPE_ANALYZE:
            name.append(SpecFile::getAnalyzeVolumeFileExtension());
            break;
         case VolumeFile::FILE_READ_WRITE_TYPE_NIFTI:
            {
               name.append(SpecFile::getNiftiVolumeFileExtension());
               const QString compressName = name + ".gz";
               if (QFile::exists(compressName)) {
                  name = compressName;
               }
            }
            break;
         case VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX: 
            name.append(SpecFile::getAnalyzeVolumeFileExtension());
            break;
         case VolumeFile::FILE_READ_WRITE_TYPE_WUNIL:
            name.append(SpecFile::getWustlVolumeFileExtension());
            break;
         case VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN:
            throw FileException("ERROR: Intermediate Volume wants to be read in UNKNOWN");
            break;
      }
      vf.readFile(name);
      if (DebugControl::getDebugOn()) {
         std::cout << "Read Volume File: " << name.toAscii().constData() << std::endl;
      }
   }
   catch (FileException& e) {
      throw (BrainModelAlgorithmException(e.whatQString()));
   }
}      

/**
 * Write a volume.
 */
void 
BrainModelVolumeSureFitErrorCorrection::writeIntermediateVolume(VolumeFile& vf,
                              const QString& nameIn) throw (BrainModelAlgorithmException)
{
   writeIntermediateVolume(&vf, nameIn);
}
                               
/** 
 * Write a volume.
 */
void
BrainModelVolumeSureFitErrorCorrection::writeIntermediateVolume(VolumeFile* vf,
                               const QString& nameIn) throw (BrainModelAlgorithmException)
{
   if (keepIntermediateFilesInMemoryFlag) {
      //
      // If volume with name exists, free its memory and remove from map
      //
      std::map<QString,VolumeFile*>::iterator iter = 
         intermediateVolumeFilesInMemory.find(nameIn);
      if (iter != intermediateVolumeFilesInMemory.end()) {
         delete iter->second;
         intermediateVolumeFilesInMemory.erase(iter);
      }
      
      //
      // Keep file in memory
      //
      intermediateVolumeFilesInMemory[nameIn] = new VolumeFile(*vf);
      
      return;
   }
   
   try {
      vf->setDescriptiveLabel(nameIn);
      QString name;
      QDir intermedDir(intermediateFilesSubDirectory);
      if (intermedDir.exists()) {
         name.append(intermediateFilesSubDirectory);
         name.append("/");
      }
      name.append(nameIn);

      QString fileNameWritten, dataFileNameWritten;
      VolumeFile::writeVolumeFile(vf,
                                  typeOfVolumeFilesToWrite,
                                  name,
                                  false,
                                  fileNameWritten,
                                  dataFileNameWritten);

/*
      name.append("+orig");  // so readable by AFNI
      QString brickName(name);
      brickName.append(".BRIK");
      name.append(SpecFile::getAfniVolumeFileExtension());
      vf->writeFile(name);
*/
      if (DebugControl::getDebugOn()) {
         std::cout << "Write Volume File: " << fileNameWritten.toAscii().constData() << std::endl;
      }
      intermediateFileNames.push_back(fileNameWritten);
      if (dataFileNameWritten.isEmpty() == false) {
         intermediateFileNames.push_back(dataFileNameWritten);
      }
   }
   catch (FileException& e) {
      throw (BrainModelAlgorithmException(e.whatQString()));
   }
}     

