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

#include <algorithm>
#include <cmath>
#include <sstream>


#include "BrainModelSurface.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainModelVolumeToSurfaceMapper.h"
#include "BrainSet.h"
#include "CaretVersion.h"
#include "DateAndTime.h"
#include "FileUtilities.h"
#include "GaussianComputation.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"

#include "vtkMath.h"

/**
 * Constructor for a volume file in memory
 */
BrainModelVolumeToSurfaceMapper::BrainModelVolumeToSurfaceMapper(
                                                BrainSet* bs,
                             BrainModelSurface* surfaceIn,
                             VolumeFile* volumeFileIn,
                             GiftiNodeDataFile* dataFileIn,
                             const BrainModelVolumeToSurfaceMapperAlgorithmParameters algorithmIn,
                             const int dataFileColumnIn,
                             const QString& dataFileColumnNameIn)
   : BrainModelAlgorithm(bs)
{
   metricFile = NULL;
   paintFile  = NULL;
   
   volumeMode = MODE_VOLUME_IN_MEMORY;
   surface    = surfaceIn;
   volumeFile = volumeFileIn;
   dataFile   = dataFileIn;
   dataFileColumn = dataFileColumnIn;
   dataFileColumnName = dataFileColumnNameIn;
   algorithmParameters = algorithmIn;
}
                                       
/**
 * Constructor for a volume file that needs to be read.
 */
BrainModelVolumeToSurfaceMapper::BrainModelVolumeToSurfaceMapper(
                                                BrainSet* bs,
                             BrainModelSurface* surfaceIn,
                             const QString& volumeFileNameIn,
                             GiftiNodeDataFile* dataFileIn,
                             const BrainModelVolumeToSurfaceMapperAlgorithmParameters algorithmIn,
                             const int dataFileColumnIn,
                             const QString& dataFileColumnNameIn)
   : BrainModelAlgorithm(bs)
{
   metricFile = NULL;
   paintFile  = NULL;
   
   volumeFile = NULL;
   volumeMode = MODE_VOLUME_ON_DISK;
   surface    = surfaceIn;
   volumeFileName = volumeFileNameIn;
   dataFile       = dataFileIn;
   dataFileColumn = dataFileColumnIn;
   dataFileColumnName = dataFileColumnNameIn;
   algorithmParameters = algorithmIn;
}
                                       
/**
 * Destructor
 */
BrainModelVolumeToSurfaceMapper::~BrainModelVolumeToSurfaceMapper()
{
}

/**
 * execute the algorithm
 */
void 
BrainModelVolumeToSurfaceMapper::execute() throw (BrainModelAlgorithmException)
{
   //
   // Get surface stuff
   //   
   if (surface == NULL) {
      throw BrainModelAlgorithmException("No surface provided.");
   }
   
   const CoordinateFile* cf = surface->getCoordinateFile();
   numberOfNodes = cf->getNumberOfCoordinates();
   if (numberOfNodes <= 0) {
      throw BrainModelAlgorithmException("Surface contains no nodes.");
   }
   const float* allCoords = cf->getCoordinate(0);
   
   //
   // Get the topology file
   //
   TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw BrainModelAlgorithmException("Surface has no topology.");
   }
   
   //
   // Verify paint/metric
   //
   if (dataFile == NULL) {
      throw BrainModelAlgorithmException("Metric/Paint file is invalid.");
   }
   switch (algorithmParameters.getAlgorithm()) {
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_NODES:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_ENCLOSING_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_GAUSSIAN:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_INTERPOLATED_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MAXIMUM_VOXEL:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MCW_BRAINFISH:
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_STRONGEST_VOXEL:
         metricFile = dynamic_cast<MetricFile*>(dataFile);
         if (metricFile == NULL) {
            throw BrainModelAlgorithmException("A metric file must be provided for metric mapping.");
         }
         break;
      case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_PAINT_ENCLOSING_VOXEL:
         paintFile = dynamic_cast<PaintFile*>(dataFile);
         if (paintFile == NULL) {
            throw BrainModelAlgorithmException("A metric file must be provided for metric mapping.");
         }
         break;
   }
   
   
   //
   // Create topology helper so that only connected nodes are mapped
   //
   topologyHelper = (TopologyHelper*)tf->getTopologyHelper(false, true, false);
   
   //
   // Reset metric column if necessary
   //
   dataFileColumnNumber = 0;
   if ((dataFileColumn < 0) || (dataFileColumn >= dataFile->getNumberOfColumns())) {
      dataFileColumn = -1;
   }
   if (dataFileColumn >= 0) {
      dataFileColumnNumber = dataFileColumn;
   }
   int columnsToAdd = 0;
   
   //
   // Mapping using a file that needs to be read
   //
   VolumeFile volumeFileOnDisk;
   int numberOfSubVolumes = 1;
   switch (volumeMode) {
      case MODE_VOLUME_ON_DISK:
         //
         // Determine the number of sub volumes in the volume by reading the header
         //
         volumeFile = &volumeFileOnDisk;
         try {
            volumeFile->readFile(volumeFileName,
                                 VolumeFile::VOLUME_READ_HEADER_ONLY);
            numberOfSubVolumes = volumeFile->getNumberOfSubVolumes();
            
            //
            // Determine the number of columns that need to be added to the metric file
            //
            if (dataFileColumn < 0) {
               columnsToAdd = numberOfSubVolumes;
               dataFileColumnNumber = dataFile->getNumberOfColumns();
            }
            else {
               columnsToAdd = (dataFileColumn + numberOfSubVolumes)
                           - dataFile->getNumberOfColumns();
               dataFileColumnNumber = dataFileColumn;
            }
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e.whatQString());
         }
         break;
      case MODE_VOLUME_IN_MEMORY:
         if (volumeFile == NULL) {
            throw BrainModelAlgorithmException("No volume provided.");
         }
   
         if (dataFileColumn < 0) {
            columnsToAdd = 1;
            dataFileColumnNumber = dataFile->getNumberOfColumns();
         }
         break;
   }
   
   //
   // Paint file region names
   //
   if (paintFile != NULL) {
      paintQuestionNameIndex = paintFile->addPaintName("???");
      
      //
      // Process the regions
      //
      paintVolumeIndexToPaintFileNameIndex.clear();
      const int numRegionNames = volumeFile->getNumberOfRegionNames();
      for (int i = 0; i < numRegionNames; i++) {
         QString name(volumeFile->getRegionNameFromIndex(i));
         if (name.isEmpty()) {
            name = ("Unknown_name_" + QString::number(i));
         }
         paintVolumeIndexToPaintFileNameIndex.push_back(paintFile->addPaintName(name));
      }
   }
   
   //
   // Add space, if needed, to the metric file
   //
   if (columnsToAdd > 0) {
      //
      // Add a column onto the metric file
      //
      if (dataFile->getNumberOfColumns() == 0) {
         dataFile->setNumberOfNodesAndColumns(numberOfNodes, columnsToAdd);
      }
      else {
         dataFile->addColumns(columnsToAdd);
      }
   }
   
   //
   // Get volume information
   //
   volumeFile->getOrigin(volumeOrigin);
   volumeFile->getSpacing(volumeVoxelSize);
   volumeFile->getDimensions(volumeDimensions);
   
   for (int j = 0; j < numberOfSubVolumes; j++) {
      switch (volumeMode) {
         case MODE_VOLUME_ON_DISK:
            //
            // Read the sub volume
            //
            try {
               volumeFile->readFile(volumeFileName, j);
            }
            catch (FileException& e) {
               throw BrainModelAlgorithmException(e.whatQString());
            }
            break;
         case MODE_VOLUME_IN_MEMORY:
            break;
      }
      int dim[3];
      volumeFile->getDimensions(dim);
      if ((dim[0] <= 0) || (dim[1] <= 0) || (dim[2] <= 0)) {
         throw BrainModelAlgorithmException("Volume is empty - dimensions all zero.");
      }
      
      QString columnName(dataFileColumnName);
      if (volumeMode == MODE_VOLUME_ON_DISK) {
         if (columnName.isEmpty()) {
            std::vector<QString> subVolumeNames;
            if (j < static_cast<int>(subVolumeNames.size())) {
               columnName = subVolumeNames[j];
            }
         }
         if (columnName.isEmpty()) {
            std::ostringstream str;
            str << " #" << (j + 1);
            columnName.append(str.str().c_str());
         }
      }
      dataFile->setColumnName(dataFileColumnNumber, columnName);
      
      //
      // Map the volume to the surface
      //
      switch (algorithmParameters.getAlgorithm()) {
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_NODES:
            algorithmMetricAverageNodes(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_AVERAGE_VOXEL:
            algorithmMetricAverageVoxel(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_ENCLOSING_VOXEL:
            algorithmMetricEnclosingVoxel(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_GAUSSIAN:
            algorithmMetricGaussian(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_INTERPOLATED_VOXEL:
            algorithmMetricInterpolatedVoxel(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MAXIMUM_VOXEL:
            algorithmMetricMaximumVoxel(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_MCW_BRAINFISH:
            algorithmMetricMcwBrainFish(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_METRIC_STRONGEST_VOXEL:
            algorithmMetricStrongestVoxel(allCoords);
            break;
         case BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM_PAINT_ENCLOSING_VOXEL:
            algorithmPaintEnclosingVoxel(allCoords);
            break;
      }
      
      //
      // Set the metadata link
      //
      dataFile->setColumnStudyMetaDataLinkSet(dataFileColumnNumber, 
                                              volumeFile->getStudyMetaDataLinkSet());
      
      //
      // Set the comment
      //
      QString comment;
      comment.append("CARET v");
      comment.append(CaretVersion::getCaretVersionAsString());
      comment.append("\n");
      comment.append("Mapped to surface: ");
      comment.append(FileUtilities::basename(cf->getFileName()));
      comment.append("\n");
      comment.append("Mapped from volume: ");
      comment.append(FileUtilities::basename(volumeFile->getFileName()));
      comment.append("\n");
      comment.append("Subvolume: ");
      comment.append(StringUtilities::fromNumber(j));
      comment.append("\n");
      comment.append(algorithmParameters.getAlgorithmParametersForComment());
      
      //
      // Might have been a NIFTI file
      //
      GiftiMetaData* md = dataFile->getDataArray(dataFileColumnNumber)->getMetaData();
      QString niftiIntentAndParamString;
      QString niftiIntentName;
      int intentCode; 
      float intentParameter1, intentParameter2, intentParameter3;
      float niftiTR;
      volumeFile->getNiftiInfo(niftiIntentAndParamString,
                               niftiIntentName,
                               intentCode, 
                               intentParameter1, 
                               intentParameter2, 
                               intentParameter3, 
                               niftiTR);
      if (niftiIntentAndParamString.isEmpty() == false) {
         comment.append("intent: " + niftiIntentAndParamString + "\n");
         md->set("intent", niftiIntentAndParamString);
         
         comment.append("intent_code: " + QString::number(intentCode) + "\n");
         md->set("intent_code", QString::number(intentCode));
         
         comment.append("intent_p1: " + QString::number(intentParameter1, 'f', 3) + "\n");
         md->set("intent_p1", QString::number(intentParameter1, 'f', 3));
         comment.append("intent_p2: " + QString::number(intentParameter2, 'f', 3) + "\n");
         md->set("intent_p2", QString::number(intentParameter2, 'f', 3));
         comment.append("intent_p3: " + QString::number(intentParameter3, 'f', 3) + "\n");
         md->set("intent_p3", QString::number(intentParameter3, 'f', 3));
            
         GiftiDataArray* gda = dataFile->getDataArray(dataFileColumnNumber);
         gda->setIntent(niftiIntentName);
      }
      
      if (niftiTR != 0.0) {
         comment.append("slice_duration: " + QString::number(niftiTR) + "\n");
         md->set("TimeStep", niftiTR);
      }
      
      comment.append("\n");
      comment.append("Date Mapped: ");
      comment.append(DateAndTime::getDateAndTimeAsString());
      comment.append("\n");
      dataFile->setColumnComment(dataFileColumnNumber, comment);
      
      //
      // Prepare for mapping next column
      //
      dataFileColumnNumber++;
   }
   
   //
   // Add paint names for anything that is missing
   //
   if (paintFile != NULL) {
      addPaintNamesForIndicesWithoutNames();
   }
   
   //
   // Store algorithm parameters in preferences file
   //
   PreferencesFile* pf = brainSet->getPreferencesFile();
   algorithmParameters.transferParametersToPreferncesFile(pf, true);
}

/**
 * Run the Metric Average Nodes algorithm.
 * The metric for each node is the average of the node and its neighbors voxel values.
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricAverageNodes(const float* allCoords)
{
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         int ijk[3];
         float pcoords[3];
         if (volumeFile->convertCoordinatesToVoxelIJK((float*)&allCoords[i*3], 
                                                                  ijk, pcoords)) {
            value = volumeFile->getVoxel(ijk, 0); 
            
            float numContribute = 1.0;
            int numNeighbors = 0;
            const int* neighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
            
            for (int j = 0; j < numNeighbors; j++) {
               const int n = neighbors[j];
               if (volumeFile->convertCoordinatesToVoxelIJK((float*)&allCoords[n*3],
                                                                        ijk, pcoords)) {
                  value += volumeFile->getVoxel(ijk, 0); 
                  numContribute += 1.0;
               }
            }
            
            value /= numContribute;
         }
      }
      
      metricFile->setValue(i, dataFileColumnNumber, value);       
   }
}

/**
 * Run the Metric Average Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricAverageVoxel(const float* allCoords)
{
   float averageVoxelNeighbors = 1.0;
   algorithmParameters.getAlgorithmMetricAverageVoxelParameters(averageVoxelNeighbors);
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      float numValueCounter = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         int iMin, iMax, jMin, jMax, kMin, kMax;
         if (getNeighborsSubVolume(&allCoords[i*3], iMin, iMax, jMin, jMax, kMin, kMax, 
                                   averageVoxelNeighbors)) {
            
            for (int ii = iMin; ii <= iMax; ii++) {
               for (int jj = jMin; jj <= jMax; jj++) {
                  for (int kk = kMin; kk <= kMax; kk++) {
                     int ijk[3] = { ii, jj, kk };
                     value += volumeFile->getVoxel(ijk, 0); 
                     numValueCounter += 1.0;   
                  }
               }
            }
         }
      }
      
      if (numValueCounter > 0.0) {
         value /= numValueCounter;
      }
      
      metricFile->setValue(i, dataFileColumnNumber, value);       
   }
}

/**
 * Run the Metric Enclosing Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricEnclosingVoxel(const float* allCoords)
{
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         int ijk[3];
         float pcoords[3];
         if (volumeFile->convertCoordinatesToVoxelIJK((float*)&allCoords[i*3], 
                                                                  ijk, pcoords)) {
            value = volumeFile->getVoxel(ijk, 0);  
         }
      }
      
      metricFile->setValue(i, dataFileColumnNumber, value);       
   }
}

/* SAVED
void
BrainModelVolumeToSurfaceMapper::algorithmGaussian(const float* allCoords)
{
   const float halfVoxelX = volumeVoxelSize[0] * 0.5;
   const float halfVoxelY = volumeVoxelSize[1] * 0.5;
   const float halfVoxelZ = volumeVoxelSize[2] * 0.5;
   
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         float voxelSum  = 0.0;
         float weightSum = 0.0;
         
         int iMin, iMax, jMin, jMax, kMin, kMax;
         if (getNeighborsSubVolume(&allCoords[i*3], iMin, iMax, jMin, jMax, kMin, kMax, 
                                  gaussianNeighbors)) {
            
            const float* nodeNormal = surface->getNormal(i);
            
            for (int ii = iMin; ii <= iMax; ii++) {
               for (int jj = jMin; jj <= jMax; jj++) {
                  for (int kk = kMin; kk <= kMax; kk++) {
                     int ijk[3] = { ii, jj, kk };
                     vtkIdType pointID = structuredPointsVolume->ComputePointId(ijk);
                     
                     //
                     // Voxel position - probably should add 1/2 voxel to get voxel center
                     //
                     const float voxelPos[3] = {
                        (ii * volumeVoxelSize[0] + volumeOrigin[0]) + halfVoxelX,
                        (jj * volumeVoxelSize[1] + volumeOrigin[1]) + halfVoxelY,
                        (kk * volumeVoxelSize[2] + volumeOrigin[2]) + halfVoxelZ
                     };
                     const float v = voxelDataArray->GetComponent(pointID, 0); 
                     
                     //if (v != 0.0) {
                        gaussian(&allCoords[i*3],
                                 nodeNormal,
                                 voxelPos,
                                 v,
                                 voxelSum,
                                 weightSum);
                     //}
                  }
               }
            }
         }
         
         if (weightSum != 0.0) {
            value = voxelSum / weightSum;
         }
      }

      metricFile->setValue(i, metricColumnNumber, value);       
   }
}
 */
 
/**
 * Run the Metric Gaussian algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricGaussian(const float* allCoords)
{
   float gaussianNeighbors;
   float gaussianSigmaNorm;
   float gaussianSigmaTang;
   float gaussianNormBelowCutoff;
   float gaussianNormAboveCutoff;
   float gaussianTangCutoff;
   algorithmParameters.getAlgorithmMetricGaussianParameters(gaussianNeighbors,
                                                             gaussianSigmaNorm,
                                                             gaussianSigmaTang,
                                                             gaussianNormBelowCutoff,
                                                             gaussianNormAboveCutoff,
                                                             gaussianTangCutoff);
                                                             
   const float halfVoxelX = volumeVoxelSize[0] * 0.5;
   const float halfVoxelY = volumeVoxelSize[1] * 0.5;
   const float halfVoxelZ = volumeVoxelSize[2] * 0.5;
   
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         
         int iMin, iMax, jMin, jMax, kMin, kMax;
         if (getNeighborsSubVolume(&allCoords[i*3], iMin, iMax, jMin, jMax, kMin, kMax, 
                                  gaussianNeighbors)) {
            
            std::vector<GaussianComputation::Point3D> gaussPoints;
            
            for (int ii = iMin; ii <= iMax; ii++) {
               for (int jj = jMin; jj <= jMax; jj++) {
                  for (int kk = kMin; kk <= kMax; kk++) {
                     int ijk[3] = { ii, jj, kk };
                     
                     //
                     // Voxel position - probably should add 1/2 voxel to get voxel center
                     //
                     const float voxelPos[3] = {
                        (ii * volumeVoxelSize[0] + volumeOrigin[0]) + halfVoxelX,
                        (jj * volumeVoxelSize[1] + volumeOrigin[1]) + halfVoxelY,
                        (kk * volumeVoxelSize[2] + volumeOrigin[2]) + halfVoxelZ
                     };
                     const float v = volumeFile->getVoxel(ijk, 0); 
                     
                     gaussPoints.push_back(GaussianComputation::Point3D(voxelPos, v));
                  }
               }
            }
            
            GaussianComputation gauss(gaussianNormBelowCutoff,
                                      gaussianNormAboveCutoff,
                                      gaussianSigmaNorm,
                                      gaussianSigmaTang,
                                      gaussianTangCutoff);
            value = gauss.evaluate(&allCoords[i*3],
                                   surface->getNormal(i),
                                   gaussPoints);
         }
      }

      metricFile->setValue(i, dataFileColumnNumber, value);       
   }
}

/**
 * Run the Metric Interpolated Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricInterpolatedVoxel(const float* allCoords)
{
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
         volumeFile->getInterpolatedVoxel(&allCoords[i*3], value);
      }
      
      metricFile->setValue(i, dataFileColumnNumber, value);       
   }
}

/**
 * Run the Metric Maximum Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricMaximumVoxel(const float* allCoords)
{
   float maximumVoxelNeighbors = 1.0;
   algorithmParameters.getAlgorithmMetricMaximumVoxelParameters(maximumVoxelNeighbors);
   
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         int iMin, iMax, jMin, jMax, kMin, kMax;
         if (getNeighborsSubVolume(&allCoords[i*3], iMin, iMax, jMin, jMax, kMin, kMax, 
                                   maximumVoxelNeighbors)) {            
            bool firstTime = true;
            for (int ii = iMin; ii <= iMax; ii++) {
               for (int jj = jMin; jj <= jMax; jj++) {
                  for (int kk = kMin; kk <= kMax; kk++) {
                     int ijk[3] = { ii, jj, kk };
                     const float v = volumeFile->getVoxel(ijk, 0); 
                     if (firstTime) {
                        value = v;
                     }
                     else {
                        value = std::max(v, value);
                     }
                     firstTime = false;
                  }
               }
            }
         }
      }
      
      metricFile->setValue(i, dataFileColumnNumber, value);       
   }
}

/**
 * Run the Metric Strongest Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricStrongestVoxel(const float* allCoords)
{
   float strongestVoxelNeighbors = 1.0;
   algorithmParameters.getAlgorithmMetricStrongestVoxelParameters(strongestVoxelNeighbors);
   
   for (int i = 0; i < numberOfNodes; i++) {
      float absVoxel = 0.0;
      float signedVoxel = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         int iMin, iMax, jMin, jMax, kMin, kMax;
         if (getNeighborsSubVolume(&allCoords[i*3], iMin, iMax, jMin, jMax, kMin, kMax, 
                                   strongestVoxelNeighbors)) {            
            for (int ii = iMin; ii <= iMax; ii++) {
               for (int jj = jMin; jj <= jMax; jj++) {
                  for (int kk = kMin; kk <= kMax; kk++) {
                     int ijk[3] = { ii, jj, kk };
                     const float signedTemp = volumeFile->getVoxel(ijk, 0); 
                     const float absTemp = std::fabs(signedTemp);
                     if (absTemp > absVoxel) {
                        absVoxel = absTemp;
                        signedVoxel = signedTemp;
                     }
                  }
               }
            }
         }
      }
      
      metricFile->setValue(i, dataFileColumnNumber, signedVoxel);       
   }
}

/**
 * Run the Metric MCW Brain Fish algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmMetricMcwBrainFish(const float* allCoords)
{
   float mcwBrainFishMaxDistance;
   int mcwBrainFishSplatFactor;
   algorithmParameters.getAlgorithmMetricMcwBrainFishParameters(mcwBrainFishMaxDistance,
                                                                mcwBrainFishSplatFactor);
   
   float surfaceBounds[6];
   surface->getBounds(surfaceBounds);

   //
   // convert surface bounds into voxel indices
   //
   float voxelBounds[6];
   voxelBounds[0] = floor((surfaceBounds[0] - volumeOrigin[0]) / volumeVoxelSize[0]);
   voxelBounds[1] = floor((surfaceBounds[1] - volumeOrigin[0]) / volumeVoxelSize[0]);
   voxelBounds[2] = floor((surfaceBounds[2] - volumeOrigin[1]) / volumeVoxelSize[1]);
   voxelBounds[3] = floor((surfaceBounds[3] - volumeOrigin[1]) / volumeVoxelSize[1]);
   voxelBounds[4] = floor((surfaceBounds[4] - volumeOrigin[2]) / volumeVoxelSize[2]);
   voxelBounds[5] = floor((surfaceBounds[5] - volumeOrigin[2]) / volumeVoxelSize[2]);
   
   //
   // Limit search of volume to voxels around surface's bounds
   //
   int xmin = static_cast<int>(voxelBounds[0] - mcwBrainFishMaxDistance - 1);
   int xmax = static_cast<int>(voxelBounds[1] + mcwBrainFishMaxDistance + 1);
   if (xmin < 0) xmin = 0;
   if (xmax >= volumeDimensions[0]) xmax = volumeDimensions[0] - 1;
   int ymin = static_cast<int>(voxelBounds[2] - mcwBrainFishMaxDistance - 1);
   int ymax = static_cast<int>(voxelBounds[3] + mcwBrainFishMaxDistance + 1);
   if (ymin < 0) ymin = 0;
   if (ymax >= volumeDimensions[1]) ymax = volumeDimensions[1] - 1;
   int zmin = static_cast<int>(voxelBounds[4] - mcwBrainFishMaxDistance - 1);
   int zmax = static_cast<int>(voxelBounds[5] + mcwBrainFishMaxDistance + 1);
   if (zmin < 0) zmin = 0;
   if (zmax >= volumeDimensions[2]) zmax = volumeDimensions[2] - 1;
   
   if (DebugControl::getDebugOn()) {
      //printf("ROI: xmin=%d; xmax=%d; ymin=%d; ymax=%d; zmin=%d; zmax=%d\n",
      //       xmin, xmax, ymin, ymax, zmin, zmax);
   }
     
   float* activity = new float[numberOfNodes];
   bool*  assigned = new bool[numberOfNodes];
   bool*  assignedToo = new bool[numberOfNodes];
   for (int m = 0; m < numberOfNodes; m++) {
      assigned[m] = false;
      assignedToo[m] = false;
      activity[m] = 0.0;
   }
   
   //
   // Use a point locator to speed nearby node queries
   //
   BrainModelSurfacePointLocator* pointLocator = new BrainModelSurfacePointLocator(
                                                           surface,
                                                           true);
   //
   // loop through the voxels
   //
   for (int k = zmin; k < zmax; k++) {
      for (int j = ymin; j < ymax; j++) {
         for (int i = xmin; i < xmax; i++) {
            int ijk[3] = { i, j, k };
            
            //
            // voxel at (i, j, k)
            //
            const float voxel = volumeFile->getVoxel(ijk, 0);
            //printf("voxel (%d,%d,%d) = %f\n", i, j, k, voxel);
            
            //
            // Only do voxels that have some activity
            //
            if (voxel != 0.0) {
               int nearestNode = -1;

               //
               // Center of voxel
               //
               float voxelPos[3] = {
                  volumeOrigin[0] + ijk[0] * volumeVoxelSize[0] + volumeVoxelSize[0] * 0.5,
                  volumeOrigin[1] + ijk[1] * volumeVoxelSize[1] + volumeVoxelSize[1] * 0.5,
                  volumeOrigin[2] + ijk[2] * volumeVoxelSize[2] + volumeVoxelSize[2] * 0.5
               };
         
               // 
               // Find the node that is nearest to the voxel
               //
               const int nodeFound = pointLocator->getNearestPoint(voxelPos);
               if (nodeFound >= 0) {
                  
                  //
                  // Make sure node is within the brain fish max distance
                  //
                  const float* xyz = &allCoords[nodeFound*3];
                  if ((xyz[0] > (voxelPos[0] - mcwBrainFishMaxDistance)) &&
                      (xyz[0] < (voxelPos[0] + mcwBrainFishMaxDistance))) {
                     if ((xyz[1] > (voxelPos[1] - mcwBrainFishMaxDistance)) &&
                         (xyz[1] < (voxelPos[1] + mcwBrainFishMaxDistance))) {
                        if ((xyz[2] > (voxelPos[2] - mcwBrainFishMaxDistance)) &&
                            (xyz[2] < (voxelPos[2] + mcwBrainFishMaxDistance))) {
                           nearestNode = nodeFound;
                        }
                     }
                  }
               }

               //
               // Allow positive activity to override negative activity
               // Negative only overrides "less negative" 
               //
               if (nearestNode >= 0) {
                  assigned[nearestNode] = true;
                  const float nodeValue = activity[nearestNode];
                  if (voxel > 0.0) {
                     if (voxel > nodeValue) {
                        activity[nearestNode] = voxel;
                     }
                  }
                  else if (nodeValue < 0.0) {
                     if (voxel < nodeValue) {
                        activity[nearestNode] = voxel;
                     }
                  }
               }
            }
         }
      }
   }
   
   //
   // Process the splat factor.  For each node that is assigned, check any
   // of its neighbors that are not assigned.  If the neighbor is not assigned,
   // assign the neighbor the average of its assigned neighbors.
   //
   if (mcwBrainFishSplatFactor > 0) {
      for (int m = 0; m < numberOfNodes; m++) {
         assignedToo[m] = assigned[m];
      }
      
      //
      // Number of Splat levels
      //
      for (int k = 0; k < mcwBrainFishSplatFactor; k++) {
         for (int m = 0; m < numberOfNodes; m++) {
            //
            // Is this node assigned ?
            //
            if (assigned[m]) {
            
               //
               // Get this node's neighbors
               //
               std::vector<int> neighbors;
               topologyHelper->getNodeNeighbors(m, neighbors);
               for (unsigned int j = 0; j < neighbors.size(); j++) {
                  const int neighbor = neighbors[j];
                  
                  //
                  // Is neighbor unassigned ?
                  //
                  if (assigned[neighbor] == false) {
                     float sum = 0.0;
                     int cnt = 0;
                     
                     //
                     // Get this neighbor's neighbors
                     //
                     std::vector<int> neighbors2;
                     topologyHelper->getNodeNeighbors(neighbor, neighbors2);
                     for (unsigned int n = 0; n < neighbors2.size(); n++) {
                     
                        //
                        // Is the neighbor's neighbor assigned ?
                        //
                        const int neighbor2 = neighbors2[n];
                        if (assigned[neighbor2]) {
                           sum += activity[neighbor2];
                           cnt++;
                        }
                     }
                     
                     //
                     // Set this neighbors activity, if any from its neighbors
                     //
                     if (cnt > 0) {
                        activity[neighbor] = sum/cnt;
                        assignedToo[neighbor] = true;
                     }
                  }
               }
            }
         }
         for (int mm = 0; mm < numberOfNodes; mm++) {
            assigned[mm] = assignedToo[mm];
         }
      }
   }
   
   //
   // Update the metric file with the activity
   //
   for (int mm = 0; mm < numberOfNodes; mm++) {
      metricFile->setValue(mm, dataFileColumnNumber, activity[mm]);
   }
   
   delete[] assigned;
   delete[] assignedToo;
   delete[] activity;
}

/**
 * Get the valid subvolume for neighbor mapping algorithms.
 * Returns true if the neighbors subvolume is valid (within the volume)
 */
bool
BrainModelVolumeToSurfaceMapper::getNeighborsSubVolume(const float xyz[3],
                                                             int& iMin, int& iMax,
                                                             int& jMin, int& jMax,
                                                             int& kMin, int& kMax,
                                                             const float neighborsCubeSize) const
{
   int ijk[3];
   if (volumeFile->convertCoordinatesToVoxelIJK(xyz, ijk) == false) {
      return false;
   }
   
   //
   // Half cube
   //
   const float halfCubeSize = neighborsCubeSize * 0.5;
   
   //
   // Min corner
   //   
   const float minCorner[3] = {
      xyz[0] - halfCubeSize,
      xyz[1] - halfCubeSize,
      xyz[2] - halfCubeSize
   };
   int ijkMin[3];
   volumeFile->convertCoordinatesToVoxelIJK(minCorner, ijkMin);

   //
   // Max corner
   //   
   const float maxCorner[3] = {
      xyz[0] + halfCubeSize,
      xyz[1] + halfCubeSize,
      xyz[2] + halfCubeSize
   };
   int ijkMax[3];
   volumeFile->convertCoordinatesToVoxelIJK(maxCorner, ijkMax);

   //
   // Limit dimensions
   //
   int dim[3];
   volumeFile->getDimensions(dim);
   for (int i = 0; i < 3; i++) {
      ijkMin[i] = std::max(ijkMin[i], 0);
      ijkMax[i] = std::min(ijkMax[i], dim[i] - 1);
   }
   
   iMin = ijkMin[0];
   jMin = ijkMin[1];
   kMin = ijkMin[2];
   iMax = ijkMax[0];
   jMax = ijkMax[1];
   kMax = ijkMax[2];

   return true;
}

/**
 * Get the valid subvolume for neighbor mapping algorithms.
 * Returns true if the neighbors subvolume is valid (within the volume)
 *
bool
BrainModelVolumeToSurfaceMapper::getNeighborsSubVolume(const float xyz[3],
                                                             int& iMin, int& iMax,
                                                             int& jMin, int& jMax,
                                                             int& kMin, int& kMax,
                                                             const float neighborsCubeSize) const
{
   //
   // See where the node is located inside the volume
   //
   int ijk[3];
   float pcoords[3];
   if (volumeFile->convertCoordinatesToVoxelIJK(xyz, ijk, pcoords)) {
      //
      // Default to the voxel containing the node
      //
      iMin = ijk[0];
      iMax = ijk[0];
      jMin = ijk[1];
      jMax = ijk[1];
      kMin = ijk[2];
      kMax = ijk[2];
      
      if (neighborsCubeSize > 0.0) {
         //
         // half of the neighbor cube
         //
         const float halfCube = neighborsCubeSize * 0.5;
         
         //
         // maximum voxel indices
         //
         const int maxDimI = volumeDimensions[0] - 1;
         const int maxDimJ = volumeDimensions[1] - 1;
         const int maxDimK = volumeDimensions[2] - 1;

         //
         // Half voxel sizes
         //
         //const float halfVoxelI = volumeVoxelSize[0] * 0.5;
         //const float halfVoxelJ = volumeVoxelSize[1] * 0.5;
         //const float halfVoxelK = volumeVoxelSize[2] * 0.5;
         
         //
         // Look for voxels in positive X
         //
         const float maxX = xyz[0] + halfCube;
         for (int i = ijk[0] + 1; i < maxDimI; i++) {
            float coord[3];
            volumeFile->getVoxelCoordinate(i, ijk[1], ijk[2], coord);
            const float x = coord[0];
            //const float x = i * volumeVoxelSize[0] + volumeOrigin[0] + halfVoxelI;
            if (x <= maxX) {
               iMax = i;
            }
            else {
               break;
            }
         }
         
         //
         // Look for voxels in negative X
         //
         const float minX = xyz[0] - halfCube;
         for (int i = ijk[0] - 1; i >= 0; i--) {
            float coord[3];
            volumeFile->getVoxelCoordinate(i, ijk[1], ijk[2], coord);
            const float x = coord[0];
            //const float x = i * volumeVoxelSize[0] + volumeOrigin[0] + halfVoxelI;
            if (x >= minX) {
               iMin = i;
            }
            else {
               break;
            }
         }
         
         //
         // Look for voxels in positive Y
         //
         const float maxY = xyz[1] + halfCube;
         for (int j = ijk[1] + 1; j < maxDimJ; j++) {
            float coord[3];
            volumeFile->getVoxelCoordinate(ijk[0], j, ijk[2], coord);
            const float y = coord[1];
            //const float y = j * volumeVoxelSize[1] + volumeOrigin[1] + halfVoxelJ;
            if (y <= maxY) {
               jMax = j;
            }
            else {
               break;
            }
         }
         
         //
         // Look for voxels in negative Y
         //
         const float minY = xyz[1] - halfCube;
         for (int j = ijk[1] - 1; j >= 0; j--) {
            float coord[3];
            volumeFile->getVoxelCoordinate(ijk[0], j, ijk[2], coord);
            const float y = coord[1];
            //const float y = j * volumeVoxelSize[1] + volumeOrigin[1] + halfVoxelJ;
            if (y >= minY) {
               jMin = j;
            }
            else {
               break;
            }
         }
      
         //
         // Look for voxels in positive Z
         //
         const float maxZ = xyz[2] + halfCube;
         for (int k = ijk[2] + 1; k < maxDimK; k++) {
            float coord[3];
            volumeFile->getVoxelCoordinate(ijk[0], ijk[1], k, coord);
            const float z = coord[2];
            //const float z = k * volumeVoxelSize[2] + volumeOrigin[2] + halfVoxelK;
            if (z <= maxZ) {
               kMax = k;
            }
            else {
               break;
            }
         }
         
         //
         // Look for voxels in negative Z
         //
         const float minZ = xyz[2] - halfCube;
         for (int k = ijk[2] - 1; k >= 0; k--) {
            float coord[3];
            volumeFile->getVoxelCoordinate(ijk[0], ijk[1], k, coord);
            const float z = coord[2];
            //const float z = k * volumeVoxelSize[2] + volumeOrigin[2] + halfVoxelK;
            if (z >= minZ) {
               kMin = k;
            }
            else {
               break;
            }
         }
         
      }
      return true;
   }
   
   return false;
}
*/

/**
 * Run the Paint Enclosing Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMapper::algorithmPaintEnclosingVoxel(const float* allCoords)
{
   const int numPaintIndices = static_cast<int>(paintVolumeIndexToPaintFileNameIndex.size());
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
            
         int ijk[3];
         float pcoords[3];
         if (volumeFile->convertCoordinatesToVoxelIJK((float*)&allCoords[i*3], 
                                                                  ijk, pcoords)) {
            value = volumeFile->getVoxel(ijk, 0);  
         }
      }
      
      //
      // Convert the paint volume name index to a paint file name index
      // if the index is out of range a name is created for it by the 
      // method named addPaintNamesForIndicesWithoutNames()
      //
      int paintIndex = static_cast<int>(value);
      if (paintIndex < 0) {
         paintIndex = paintQuestionNameIndex;
      }
      else if (paintIndex < numPaintIndices) {
         paintIndex = paintVolumeIndexToPaintFileNameIndex[paintIndex];
      }
      paintFile->setPaint(i, dataFileColumnNumber, paintIndex);       
   }
   
}

/**
 * add paint names for paint indices without names.
 */
void 
BrainModelVolumeToSurfaceMapper::addPaintNamesForIndicesWithoutNames()
{
   //
   // Find maximum paint index
   //
   const int numNodes = paintFile->getNumberOfNodes();
   const int numCols  = paintFile->getNumberOfColumns();
   int maximumPaintIndex = -1;
   for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numCols; j++) {
         maximumPaintIndex = std::max(paintFile->getPaint(i, j),
                                      maximumPaintIndex);
      }
   }
   
   //
   // Add paint names as needed
   //
   const int numPaintNames = paintFile->getNumberOfPaintNames();
   if (maximumPaintIndex >= numPaintNames) {
      for (int i = numPaintNames; i <= maximumPaintIndex; i++) {
         paintFile->addPaintName("Unknown_name_" + QString::number(i));
      }
   }
}

/*
 * Determine the gaussian for a node
 * Inputs:
 *   nodeXYZ is the position of the node.
 *   nodeNormal is the normal vector (magnitude = 1.0) for the node.
 *   voxelXYZ is the position of the voxel (probably should be the center of the voxel.
 *   voxelValue is the voxel whose metricSum and weightSum are being determined
 *                        
void                          
BrainModelVolumeToSurfaceMapper::gaussian(const float nodeXYZ[3],
                                                const float nodeNormal[3],
                                                const float voxelXYZ[3],
                                                const float voxelValue,
                                                float& metricSum,
                                                float& weightSum) const
{         
   // 
   // "DistanceVector" is the offset vector of a voxel from the origin of the gaussian. 
   //
   const float distanceVector[3] = {
                                      voxelXYZ[0] - nodeXYZ[0],
                                      voxelXYZ[1] - nodeXYZ[1],
                                      voxelXYZ[2] - nodeXYZ[2]
                                   };
   //
   // "dNorm" is"distanceVector" projected onto the normal.
   // See "Computer Graphics" 2nd ed pages 1096-1097
   //
   //   nodeNormal
   //      /|\          x
   //       |
   //       | dTang
   //       ------#       # = voxelXYZ
   //     d ||    /
   //     N ||   /
   //     o ||  / distanceVector
   //     r || /
   //     m ||/
   //       *              * = nodeXYZ
   //
   const float dNorm = MathUtilities::dotProduct((float*)nodeNormal, (float*)distanceVector);
   
   //
   // See if the voxel of interest is within the allowable distances along the normal vector.
   //
   float Wnorm = 0.0;               
   if ((dNorm > -gaussianNormBelowCutoff) &&
       (dNorm < gaussianNormAboveCutoff)) {
      //
      // Wnorm is the weighting for the distance along the normal vector.
      // The weighting is inversely related to the distance from the node.
      //
      Wnorm = exp (-(dNorm*dNorm)/(gaussianSigmaNorm*gaussianSigmaNorm));
   }                    

   if (Wnorm > 0.0)  {
      //
      // Tang is the distance vector components from the voxel 
      // to the nearest point on infinitely long normal vector.
      //
      float Tang[3];
      for (int i = 0; i < 3; i++) {
         Tang[i] = distanceVector[i] - dNorm*nodeNormal[i];
      }
      
      //
      // dTang is the distance from the end of the voxel to the closest
      // point on the infinitely long normal vector.
      //
      const float dTang = std::sqrt(Tang[0]*Tang[0] + Tang[1]*Tang[1] +
                               Tang[2]*Tang[2]);
                               
      //
      // See if voxel distance tangentially is within the limits.
      //
      float Wtang = 0.0;
      if (dTang < gaussianTangCutoff) {
         //
         // Wtang is the weighting for the distance orthogonal to the 
         // normal vector.  The weighting is inversely related to the 
         // distance from the origin.
         //
         Wtang = exp (-((dTang*dTang)
                       / (gaussianSigmaTang * gaussianSigmaTang)));
      }
      
      if (Wtang > 0.0) {
         //
         // Combine the weights and update the metric and weight sums.
         //
         const float weight = Wnorm * Wtang;
         metricSum += voxelValue * weight;
         weightSum += weight;
      }
   }
}
*/
