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

#include <QDateTime>

#include "BrainModelSurface.h"
#include "BrainModelSurfacePointLocator.h"
#include "BrainModelVolumeToSurfaceMetricMapper.h"
#include "CaretVersion.h"
#include "FileUtilities.h"
#include "GaussianComputation.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"

#include "vtkMath.h"

/**
 * Constructor for a volume file in memory
 */
BrainModelVolumeToSurfaceMetricMapper::BrainModelVolumeToSurfaceMetricMapper(
                                                BrainSet* bs,
                                                const ALGORITHM algorithmIn,
                                                BrainModelSurface* surfaceIn,
                                                VolumeFile* volumeFileIn,
                                                MetricFile* metricFileIn,
                                                const int metricColumnIn,
                                                const QString& metricColumnNameIn)
   : BrainModelAlgorithm(bs)
{
   gaussianNeighbors       = 6.0;
   averageVoxelNeighbors   = 0.0;
   maximumVoxelNeighbors   = 3.0;
   gaussianSigmaNorm       = 2.0;
   gaussianSigmaTang       = 1.0;
   gaussianNormBelowCutoff = 2.0;
   gaussianNormAboveCutoff = 2.0;
   gaussianTangCutoff      = 3.0;
   mcwBrainFishMaxDistance = 1.0;
   mcwBrainFishSplatFactor = 1;
   
   volumeMode = MODE_VOLUME_IN_MEMORY;
   algorithm  = algorithmIn;
   surface    = surfaceIn;
   volumeFile = volumeFileIn;
   metricFile = metricFileIn;
   metricColumn = metricColumnIn;
   metricColumnName = metricColumnNameIn;
}
                                       
/**
 * Constructor for a volume file that needs to be read.
 */
BrainModelVolumeToSurfaceMetricMapper::BrainModelVolumeToSurfaceMetricMapper(
                                                BrainSet* bs,
                                                const ALGORITHM algorithmIn,
                                                BrainModelSurface* surfaceIn,
                                                const QString& volumeFileNameIn,
                                                MetricFile* metricFileIn,
                                                const int metricColumnIn,
                                                const QString& metricColumnNameIn)
   : BrainModelAlgorithm(bs)
{
   volumeMode = MODE_VOLUME_ON_DISK;
   algorithm  = algorithmIn;
   surface    = surfaceIn;
   volumeFileName = volumeFileNameIn;
   metricFile = metricFileIn;
   metricColumn = metricColumnIn;
   metricColumnName = metricColumnNameIn;
}
                                       
/**
 * Destructor
 */
BrainModelVolumeToSurfaceMetricMapper::~BrainModelVolumeToSurfaceMetricMapper()
{
}

/**
 * execute the algorithm
 */
void 
BrainModelVolumeToSurfaceMetricMapper::execute() throw (BrainModelAlgorithmException)
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
   // Create topology helper so that only connected nodes are mapped
   //
   topologyHelper = (TopologyHelper*)tf->getTopologyHelper(false, true, false);
   
   //
   // Reset metric column if necessary
   //
   metricColumnNumber = 0;
   if ((metricColumn < 0) || (metricColumn >= metricFile->getNumberOfColumns())) {
      metricColumn = -1;
   }
   if (metricColumn >= 0) {
      metricColumnNumber = metricColumn;
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
            if (metricColumn < 0) {
               columnsToAdd = numberOfSubVolumes;
               metricColumnNumber = metricFile->getNumberOfColumns();
            }
            else {
               columnsToAdd = (metricColumn + numberOfSubVolumes)
                           - metricFile->getNumberOfColumns();
               metricColumnNumber = metricColumn;
            }
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e.whatQString());
         }
         break;
      case MODE_VOLUME_IN_MEMORY:
         if (metricColumn < 0) {
            columnsToAdd = 1;
            metricColumnNumber = metricFile->getNumberOfColumns();
         }
         break;
   }
   
   //
   // Get volume information
   //
   if (volumeFile == NULL) {
      throw BrainModelAlgorithmException("No volume provided.");
   }
   
   //
   // Add space, if needed, to the metric file
   //
   if (columnsToAdd > 0) {
      //
      // Add a column onto the metric file
      //
      if (metricFile->getNumberOfColumns() == 0) {
         metricFile->setNumberOfNodesAndColumns(numberOfNodes, columnsToAdd);
      }
      else {
         metricFile->addColumns(columnsToAdd);
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
      
      QString columnName(metricColumnName);
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
      metricFile->setColumnName(metricColumnNumber, columnName);
      
      //
      // Map the volume to the surface
      //
      switch (algorithm) {
         case ALGORITHM_AVERAGE_NODES:
            algorithmAverageNodes(allCoords);
            break;
         case ALGORITHM_AVERAGE_VOXEL:
            algorithmAverageVoxel(allCoords);
            break;
         case ALGORITHM_ENCLOSING_VOXEL:
            algorithmEnclosingVoxel(allCoords);
            break;
         case ALGORITHM_GAUSSIAN:
            algorithmGaussian(allCoords);
            break;
         case ALGORITHM_INTERPOLATED_VOXEL:
            algorithmInterpolatedVoxel(allCoords);
            break;
         case ALGORITHM_MAXIMUM_VOXEL:
            algorithmMaximumVoxel(allCoords);
            break;
         case ALGORITHM_MCW_BRAINFISH:
            algorithmMcwBrainFish(allCoords);
            break;
      }
      
      //
      // Set the metadata link
      //
      metricFile->setColumnStudyMetaDataLink(metricColumnNumber, 
                                             volumeFile->getStudyMetaDataLink());
      
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
      switch (algorithm) {
         case ALGORITHM_AVERAGE_NODES:
            comment.append("Algorithm: Average Nodes\n");
            break;
         case ALGORITHM_AVERAGE_VOXEL:
            comment.append("Algorithm: Average Voxel\n");
            comment.append("   neighbors=");
            comment.append(StringUtilities::fromNumber(averageVoxelNeighbors));
            comment.append("mm\n");
            break;
         case ALGORITHM_ENCLOSING_VOXEL:
            comment.append("Algorithm: Enclosing Voxel\n");
            break;
         case ALGORITHM_GAUSSIAN:
            comment.append("Algorithm: Gaussian\n");
            comment.append("   neighbors=");
            comment.append(StringUtilities::fromNumber(gaussianNeighbors));
            comment.append("mm   sigma-norm=");
            comment.append(StringUtilities::fromNumber(gaussianSigmaNorm));
            comment.append("   sigma-tang=");
            comment.append(StringUtilities::fromNumber(gaussianSigmaTang));
            comment.append("   norm-below-cutoff=");
            comment.append(StringUtilities::fromNumber(gaussianNormBelowCutoff));
            comment.append("   norm-above-cutoff=");
            comment.append(StringUtilities::fromNumber(gaussianNormAboveCutoff));
            comment.append("   tang-cutoff=");
            comment.append(StringUtilities::fromNumber(gaussianTangCutoff));
            comment.append("\n");
            break;
         case ALGORITHM_INTERPOLATED_VOXEL:
            comment.append("Algorithm: Interpolated Voxel\n");
            break;
         case ALGORITHM_MAXIMUM_VOXEL:
            comment.append("Algorithm: Maximum Voxel\n");
            comment.append("   neighbors=");
            comment.append(StringUtilities::fromNumber(maximumVoxelNeighbors));
            comment.append("mm\n");
            break;
         case ALGORITHM_MCW_BRAINFISH:
            comment.append("Algorithm: MCW Brain Fish\n");
            comment.append("   max-distance=");
            comment.append(StringUtilities::fromNumber(mcwBrainFishMaxDistance));
            comment.append("   splat-factor=");
            comment.append(StringUtilities::fromNumber(mcwBrainFishSplatFactor));
            comment.append("\n");
            break;
      }
      
      //
      // Might have been a NIFTI file
      //
      GiftiMetaData* md = metricFile->getDataArray(metricColumnNumber)->getMetaData();
      QString niftiIntent;
      float niftiTR;
      volumeFile->getNiftiInfo(niftiIntent, niftiTR);
      if (niftiIntent.isEmpty() == false) {
         comment.append("intent_code: " + niftiIntent + "\n");
         md->set("intent_code", niftiIntent);
      }
      if (niftiTR != 0.0) {
         comment.append("slice_duration: " + QString::number(niftiTR) + "\n");
         md->set("slice_duration", niftiTR);
      }
      
      comment.append("\n");
      comment.append("Date Mapped: ");
      comment.append(QDateTime::currentDateTime().toString(Qt::TextDate));
      comment.append("\n");
      metricFile->setColumnComment(metricColumnNumber, comment);
      
      //
      // Prepare for mapping next column
      //
      metricColumnNumber++;
   }
}

/**
 * get the average voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::getAlgorithmAverageVoxelParameters(float neighborsOut) const
{
   neighborsOut = averageVoxelNeighbors;
}

/**
 * set the average voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::setAlgorithmAverageVoxelParameters(const float neighborsIn)
{
   averageVoxelNeighbors = neighborsIn;
}

/**
 * get the maximum voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::getAlgorithmMaximumVoxelParameters(float neighborsOut) const
{
   neighborsOut = maximumVoxelNeighbors;
}

/**
 * set the maximum voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::setAlgorithmMaximumVoxelParameters(const float neighborsIn)
{
  maximumVoxelNeighbors = neighborsIn;
}

/**
 * set gaussian algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::getAlgorithmGaussianParameters(
                                                            float& gaussianNeighborsOut,
                                                            float& gaussianSigmaNormOut,
                                                            float& gaussianSigmaTangOut,
                                                            float& gaussianNormBelowCutoffOut,
                                                            float& gaussianNormAboveCutoffOut,
                                                            float& gaussianTangCutoffOut) const
{
   gaussianNeighborsOut       = gaussianNeighbors;
   gaussianSigmaNormOut       = gaussianSigmaNorm;
   gaussianSigmaTangOut       = gaussianSigmaTang;
   gaussianNormBelowCutoffOut = gaussianNormBelowCutoff;
   gaussianNormAboveCutoffOut = gaussianNormAboveCutoff;
   gaussianTangCutoffOut      = gaussianTangCutoff;
}
                                    
/**
 * set gaussian algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::setAlgorithmGaussianParameters(
                                                         const float gaussianNeighborsIn,
                                                         const float gaussianSigmaNormIn,
                                                         const float gaussianSigmaTangIn,
                                                         const float gaussianNormBelowCutoffIn,
                                                         const float gaussianNormAboveCutoffIn,
                                                         const float gaussianTangCutoffIn)
{
   gaussianNeighbors       = gaussianNeighborsIn;
   gaussianSigmaNorm       = gaussianSigmaNormIn;
   gaussianSigmaTang       = gaussianSigmaTangIn;
   gaussianNormBelowCutoff = gaussianNormBelowCutoffIn;
   gaussianNormAboveCutoff = gaussianNormAboveCutoffIn;
   gaussianTangCutoff      = gaussianTangCutoffIn;
}
 
/**
 * get the mcw brainfish algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::getAlgorithmMcwBrainFishParameters(
                                                    float& mcwBrainFishMaxDistanceOut,
                                                    int& mcwBrainFishSplatFactorOut) const
{
   mcwBrainFishMaxDistanceOut = mcwBrainFishMaxDistance;
   mcwBrainFishSplatFactorOut = mcwBrainFishSplatFactor;
}
                                        
/**
 * set the mcw brainfish algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMetricMapper::setAlgorithmMcwBrainFishParameters(
                                        const float mcwBrainFishMaxDistanceIn,
                                        const int mcwBrainFishSplatFactorIn)
{
   mcwBrainFishMaxDistance = mcwBrainFishMaxDistanceIn;
   mcwBrainFishSplatFactor = mcwBrainFishSplatFactorIn;
}                                              
   
/**
 * Run the Average Nodes algorithm.
 * The metric for each node is the average of the node and its neighbors voxel values.
 */
void
BrainModelVolumeToSurfaceMetricMapper::algorithmAverageNodes(const float* allCoords)
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
      
      metricFile->setValue(i, metricColumnNumber, value);       
   }
}

/**
 * Run the Average Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMetricMapper::algorithmAverageVoxel(const float* allCoords)
{
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
      
      metricFile->setValue(i, metricColumnNumber, value);       
   }
}

/**
 * Run the Enclosing Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMetricMapper::algorithmEnclosingVoxel(const float* allCoords)
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
      
      metricFile->setValue(i, metricColumnNumber, value);       
   }
}

/* SAVED
void
BrainModelVolumeToSurfaceMetricMapper::algorithmGaussian(const float* allCoords)
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
 * Run the Gaussian algorithm
 */
void
BrainModelVolumeToSurfaceMetricMapper::algorithmGaussian(const float* allCoords)
{
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

      metricFile->setValue(i, metricColumnNumber, value);       
   }
}

/**
 * Run the Interpolated Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMetricMapper::algorithmInterpolatedVoxel(const float* allCoords)
{
   for (int i = 0; i < numberOfNodes; i++) {
      float value = 0.0;
      
      if (topologyHelper->getNodeHasNeighbors(i)) {
         volumeFile->getInterpolatedVoxel(&allCoords[i*3], value);
      }
      
      metricFile->setValue(i, metricColumnNumber, value);       
   }
}

/**
 * Run the Maximum Voxel algorithm
 */
void
BrainModelVolumeToSurfaceMetricMapper::algorithmMaximumVoxel(const float* allCoords)
{
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
      
      metricFile->setValue(i, metricColumnNumber, value);       
   }
}

/**
 * Run the MCW Brain Fish algorithm
 */
void
BrainModelVolumeToSurfaceMetricMapper::algorithmMcwBrainFish(const float* allCoords)
{
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
      metricFile->setValue(mm, metricColumnNumber, activity[mm]);
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
BrainModelVolumeToSurfaceMetricMapper::getNeighborsSubVolume(const float xyz[3],
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
         const float halfVoxelI = volumeVoxelSize[0] * 0.5;
         const float halfVoxelJ = volumeVoxelSize[1] * 0.5;
         const float halfVoxelK = volumeVoxelSize[2] * 0.5;
         
         //
         // Look for voxels in positive X
         //
         const float maxX = xyz[0] + halfCube;
         for (int i = ijk[0] + 1; i < maxDimI; i++) {
            const float x = i * volumeVoxelSize[0] + volumeOrigin[0] + halfVoxelI;
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
            const float x = i * volumeVoxelSize[0] + volumeOrigin[0] + halfVoxelI;
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
            const float y = j * volumeVoxelSize[1] + volumeOrigin[1] + halfVoxelJ;
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
            const float y = j * volumeVoxelSize[1] + volumeOrigin[1] + halfVoxelJ;
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
            const float z = k * volumeVoxelSize[2] + volumeOrigin[2] + halfVoxelK;
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
            const float z = k * volumeVoxelSize[2] + volumeOrigin[2] + halfVoxelK;
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

/*
 * Determine the gaussian for a node
 * Inputs:
 *   nodeXYZ is the position of the node.
 *   nodeNormal is the normal vector (magnitude = 1.0) for the node.
 *   voxelXYZ is the position of the voxel (probably should be the center of the voxel.
 *   voxelValue is the voxel whose metricSum and weightSum are being determined
 *                        
void                          
BrainModelVolumeToSurfaceMetricMapper::gaussian(const float nodeXYZ[3],
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
