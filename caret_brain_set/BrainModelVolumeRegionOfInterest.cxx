
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
#include <iostream>
#include <limits>
#include <set>
#include <sstream>

#include "BrainModelVolumeRegionOfInterest.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelVolumeRegionOfInterest::BrainModelVolumeRegionOfInterest(BrainSet* brainSetIn)
{
   brainSet = brainSetIn;
   roiVolume = new VolumeFile;
   displayVolumeROI = false;
}

/**
 * destructor.
 */
BrainModelVolumeRegionOfInterest::~BrainModelVolumeRegionOfInterest()
{
   if (roiVolume != NULL) {
      delete roiVolume;
      roiVolume = NULL;
   }
}

/**
 * get the number of voxels in the ROI.
 */
int 
BrainModelVolumeRegionOfInterest::getNumberOfVoxelsInROI() const
{
   const int numVoxels = roiVolume->getTotalNumberOfVoxelElements();
   const float* voxels = roiVolume->getVoxelData();
   
   int cnt = 0;
   for (int i = 0; i < numVoxels; i++) {
      if (voxels[i] != 0.0) {
         cnt++;
      }
   }
   
   return cnt;
}
      
/**
 * assign paint id to paint volume for ROI voxels.
 */
void 
BrainModelVolumeRegionOfInterest::operationAssignPaintVolumeID(
                         VolumeFile* paintVolume,
                         const QString& paintName) const throw (BrainModelAlgorithmException)
{
   if (getNumberOfVoxelsInROI() <= 0) {
      throw BrainModelAlgorithmException("The region of interest contains no voxels.");
   }
   
   //
   // See which voxels are in the ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = determineVoxelsWithinVolumeROI(paintVolume, 
                                                             -std::numeric_limits<int>::min(),
                                                             std::numeric_limits<int>::max(),
                                                             voxelInROI);
   if (numVoxelsInROI <= 0) {
      throw BrainModelAlgorithmException("No voxels from the functional volume are within the ROI volume.\n"
                                         "Are the stereotaxic coordinates properly set?");
   }
   
   //
   // Index of paint being assigned
   //
   const int paintIndex = paintVolume->addRegionName(paintName);
   
   //
   // Determine the COG of the voxels in the ROI
   //
   int dim[3];
   paintVolume->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = paintVolume->getVoxelDataIndex(i, j, k);
            if (voxelInROI[flatIndex]) {
               paintVolume->setVoxel(i, j, k, 0, paintIndex);
            }
         }
      }
   }
}
 
/**
 * assign function value to functional volume for ROI voxels.
 */
void 
BrainModelVolumeRegionOfInterest::operationAssignFunctionalVolumeValue(
                              VolumeFile* functionalVolume,
                              const float value) const throw (BrainModelAlgorithmException)
{
   if (getNumberOfVoxelsInROI() <= 0) {
      throw BrainModelAlgorithmException("The region of interest contains no voxels.");
   }
   
   //
   // See which voxels are in the ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = determineVoxelsWithinVolumeROI(functionalVolume, 
                                                             -std::numeric_limits<int>::min(),
                                                             std::numeric_limits<int>::max(),
                                                             voxelInROI);
   if (numVoxelsInROI <= 0) {
      throw BrainModelAlgorithmException("No voxels from the functional volume are within the ROI volume.\n"
                                         "Are the stereotaxic coordinates properly set?");
   }
   
   //
   // Determine the COG of the voxels in the ROI
   //
   int dim[3];
   functionalVolume->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = functionalVolume->getVoxelDataIndex(i, j, k);
            if (voxelInROI[flatIndex]) {
               functionalVolume->setVoxel(i, j, k, 0, value);
            }
         }
      }
   }
}

/**
 * paint region center of gravity report.
 */
void 
BrainModelVolumeRegionOfInterest::operationPaintRegionCenterOfGravity(
                                const VolumeFile* paintVolume,
                                QString& reportTextOut) const throw (BrainModelAlgorithmException)
{
   reportTextOut = "";
   
   if (getNumberOfVoxelsInROI() <= 0) {
      throw BrainModelAlgorithmException("The region of interest contains no voxels.");
   }
   //
   // See which voxels are in the ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = determineVoxelsWithinVolumeROI(paintVolume, 
                                                             0,
                                                             std::numeric_limits<int>::max(),
                                                             voxelInROI);
   if (numVoxelsInROI <= 0) {
      throw BrainModelAlgorithmException("No voxels from the paint volume are within the ROI volume.\n"
                                         "Are the stereotaxic coordinates properly set?");
   }
   
   //
   // Get number of paint names
   //
   const int numPaintNames = paintVolume->getNumberOfRegionNames();
   if (numPaintNames <= 0) {
      throw BrainModelAlgorithmException("There are no paint regions in the volume.");
      return;
   }
   const int totalSize = numPaintNames * 3;
   double* voxelCOG = new double[totalSize];
   double* coordCOG = new double[totalSize];
   double* counter  = new double[numPaintNames];
   for (int i = 0; i < totalSize; i++) {
      voxelCOG[i] = 0.0;
      coordCOG[i] = 0.0;
      if (i < numPaintNames) {
         counter[i] = 0.0;
      }
   }
   
   //
   // Determine the COG of the voxels in the ROI
   //
   int dim[3];
   paintVolume->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = paintVolume->getVoxelDataIndex(i, j, k);
            if (voxelInROI[flatIndex]) {
               const int paintIndex = static_cast<int>(paintVolume->getVoxel(i, j, k));
               const int paintIndex3 = paintIndex * 3;
               voxelCOG[paintIndex3 + 0] += i;
               voxelCOG[paintIndex3 + 1] += j;
               voxelCOG[paintIndex3 + 2] += k;
               
               float xyz[3];
               paintVolume->getVoxelCoordinate(i, j, k, xyz);
               coordCOG[paintIndex3 + 0] += xyz[0];
               coordCOG[paintIndex3 + 1] += xyz[1];
               coordCOG[paintIndex3 + 2] += xyz[2];

               counter[paintIndex] += 1.0;
            }
         }
      }
   }
   
   std::ostringstream str;
   str.precision(2);
   str.setf(std::ios::fixed);
   std::vector<QString> sortedInfo;
   
   for (int i = 0; i < numPaintNames; i++) {
      const int i3 = i * 3;
      str.str("");
      
      const QString name(paintVolume->getRegionNameFromIndex(i));
      const double count = counter[i];
      str << name.toAscii().constData() << " has " << static_cast<int>(count) << " matching voxels.\n";
      if (count > 0) {         
         str << "   Voxel IJK C.O.G.: ("
             << voxelCOG[i3 + 0] / count << ", "
             << voxelCOG[i3 + 1] / count << ", "
             << voxelCOG[i3 + 2] / count << ")\n";
         
         str << "   Voxel Coordinate C.O.G.: ("
             << coordCOG[i3 + 0] / count << ", "
             << coordCOG[i3 + 1] / count << ", "
             << coordCOG[i3 + 2] / count << ")\n";
      }
      str << "\n";
      
      sortedInfo.push_back(str.str().c_str());
   }
   
   std::sort(sortedInfo.begin(), sortedInfo.end());
      
   QString operationText;
   
   for (int i = 0; i < static_cast<int>(sortedInfo.size()); i++) {
      operationText += sortedInfo[i];
   }
   
   delete[] voxelCOG;
   delete[] coordCOG;
   delete[] counter;
   
   const int totalNumberOfVoxels = dim[0] * dim[1] * dim[2];
   reportTextOut = createReport(paintVolume,
                                "Paint Identification Center of Gravity",
                                operationText,
                                totalNumberOfVoxels,
                                numVoxelsInROI);                                
}

/**
 * paint percentage report.
 */
void 
BrainModelVolumeRegionOfInterest::operationPaintPercentageReport(
                           const VolumeFile* paintVolume,
                           QString& reportTextOut) const throw (BrainModelAlgorithmException)
{
   reportTextOut = "";
   
   if (getNumberOfVoxelsInROI() <= 0) {
      throw BrainModelAlgorithmException("The region of interest contains no voxels.");
   }
   
   //
   // Use first paint volume to determine voxels in ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = determineVoxelsWithinVolumeROI(paintVolume, 
                                                             0,
                                                             std::numeric_limits<int>::max(),
                                                             voxelInROI);
   if (numVoxelsInROI <= 0) {
      throw BrainModelAlgorithmException("No voxels from the paint volume are within the ROI volume.\n"
                                         "Are the stereotaxic coordinates properly set?");
   }

   //
   // Get number of paint names
   //
   const int numPaintNames = paintVolume->getNumberOfRegionNames();
   if (numPaintNames <= 0) {
      throw BrainModelAlgorithmException("There are no paint regions in the volume.");
   }
   double* counter  = new double[numPaintNames];
   for (int i = 0; i < numPaintNames; i++) {
      counter[i] = 0.0;
   }
   
   //
   // Determine the COG of the voxels in the ROI
   //
   int dim[3];
   paintVolume->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int flatIndex = paintVolume->getVoxelDataIndex(i, j, k);
            if (voxelInROI[flatIndex]) {
               const int paintIndex = static_cast<int>(paintVolume->getVoxel(i, j, k));
               counter[paintIndex] += 1.0;
            }
         }
      }
   }
   
   std::ostringstream str;
   str.precision(2);
   str.setf(std::ios::fixed);
   std::vector<QString> sortedInfo;
   
   for (int i = 0; i < numPaintNames; i++) {
      str.str("");
      
      const double count = counter[i];
      str << paintVolume->getRegionNameFromIndex(i).toAscii().constData() 
          << "; "
          << static_cast<int>(count)
          << "; "
          << (count / numVoxelsInROI) * 100.0
          << "\n";
      
      sortedInfo.push_back(str.str().c_str());
   }
      
   std::sort(sortedInfo.begin(), sortedInfo.end());
      
   QString operationText;
   operationText += "Name; Voxels; Percent\n";
   
   for (int i = 0; i < static_cast<int>(sortedInfo.size()); i++) {
      operationText += sortedInfo[i];
      operationText += "\n";
   }
   
   delete[] counter;

   const int totalNumberOfVoxels = dim[0] * dim[1] * dim[2];
   reportTextOut = createReport(paintVolume,
                                "Paint Percentage Report",
                                operationText,
                                totalNumberOfVoxels,
                                numVoxelsInROI);                                
}

/**
 * probabilistic paint volume overlap analysis.
 */
void 
BrainModelVolumeRegionOfInterest::operationPaintOverlapAnalysis(
                          const std::vector<VolumeFile*>& paintVolumes,
                          QString& reportTextOut) const throw (BrainModelAlgorithmException) 
{
   reportTextOut = "";
   
   if (getNumberOfVoxelsInROI() <= 0) {
      throw BrainModelAlgorithmException("The region of interest contains no voxels.");
   }
   //
   // Make sure all volumes are of the same dimensions
   //
   const int numVolumes = static_cast<int>(paintVolumes.size());
   int dim[3];
   for (int i = 0; i < numVolumes; i++) {
      const VolumeFile* vf = paintVolumes[i];
      if (i == 0) {
         vf->getDimensions(dim);
      }
      else {
         int dim2[3];
         vf->getDimensions(dim2);
         for (int j = 0; j < 3; j++) {
            if (dim[j] != dim2[j]) {
               VolumeFile* vf0 = paintVolumes[0];
               std::ostringstream str;
               str << "All volumes must be the same dimensions.\n"
                   << "Volume " << FileUtilities::basename(vf0->getFileName()).toAscii().constData() << " dimensions: "
                   << "(" << dim[0] << ", " << dim[1] << ", " << dim[2] << ")\n"
                   << "Volume " << FileUtilities::basename(vf->getFileName()).toAscii().constData() << " dimensions: "
                   << "(" << dim2[0] << ", " << dim2[1] << ", " << dim2[2] << ").\n";
               throw BrainModelAlgorithmException(QString(str.str().c_str()));
            }
         }
      }
   }

   //
   // Use first paint volume to determine voxels in ROI
   //
   std::vector<int> voxelInROI;
   const int numVoxelsInROI = determineVoxelsWithinVolumeROI(paintVolumes[0], 
                                                             0,
                                                             std::numeric_limits<int>::max(),
                                                             voxelInROI);
   if (numVoxelsInROI <= 0) {
      throw BrainModelAlgorithmException("No voxels from the first paint volume are within the ROI volume.\n"
                                         "Are the stereotaxic coordinates properly set?");
   }

   //
   // Get all paint names in sorted order
   //
   std::set<QString> paintNamesSorted;
   for (int i = 0; i < numVolumes; i++) {
      VolumeFile* vf = paintVolumes[i];
      const int numNames = vf->getNumberOfRegionNames();
      for (int j = 0; j < numNames; j++) {
         paintNamesSorted.insert(vf->getRegionNameFromIndex(j));
      }
   }
   std::vector<QString> paintNames(paintNamesSorted.begin(), paintNamesSorted.end());
   const int numPaintNames = static_cast<int>(paintNames.size());
   
   //
   // Counts for each paint index
   //
   const int numColumns = numVolumes + 1;
   const int totalCountSize = numPaintNames * numColumns;
   std::vector<int> totalCounts(totalCountSize, 0);
   
   //
   // Get conversion from each paint volume index to all paint names
   //   
   std::vector< std::vector<int> > paintIndexToAllPaintNames;
   for (int i = 0; i < numVolumes; i++) {
      VolumeFile* vf = paintVolumes[i];
      const int numNames = vf->getNumberOfRegionNames();
      std::vector<int> indexConversion;
      for (int j = 0; j < numNames; j++) {
         int indx = -1;
         const QString name(vf->getRegionNameFromIndex(j));
         for (int k = 0; k < numPaintNames; k++) {
            if (name == paintNames[k]) {
               indx = k;
            }
         }
         if (indx == -1) {
            std::cout << "PROGRAM ERROR: Invalid paint index volume paint report" << std::endl;
         }
         indexConversion.push_back(indx);
      }
      paintIndexToAllPaintNames.push_back(indexConversion);
   }
   
   //
   // Get the number of voxels
   //
   const int numVoxels = paintVolumes[0]->getTotalNumberOfVoxels();
   for (int i = 0; i < numVoxels; i++) {
      //
      // Is this voxel in the ROI
      //
      if (voxelInROI[i]) {
         //
         // Get the paint names used by this voxel in all volumes
         //
         std::vector<int> indicesUsedByThisVoxel;
         for (int j = 0; j < numVolumes; j++) {
            std::vector<int>& indexConversion = paintIndexToAllPaintNames[j];
            indicesUsedByThisVoxel.push_back(
               indexConversion[static_cast<int>(
                  paintVolumes[j]->getVoxelWithFlatIndex(i))]);
         }   
         std::sort(indicesUsedByThisVoxel.begin(), indicesUsedByThisVoxel.end());
          
         //
         // Get unique list of paint indices used by this voxel in all volumes
         //
         std::set<int> uniqueIndices(indicesUsedByThisVoxel.begin(),
                                     indicesUsedByThisVoxel.end());
                          
         //
         // Update counts
         //
         for (std::set<int>::iterator iter = uniqueIndices.begin();
              iter != uniqueIndices.end(); iter++) {
            const int n = *iter;
            int cnt = 0;
            for (int p = 0; p < numVolumes; p++) {
               if (n == indicesUsedByThisVoxel[p]) {
                  cnt++;
               }
            }
            totalCounts[n * numColumns + cnt]++;
         }
      }
   }
   
   const QString separator(";");
   
   QString operationText;
   
   //
   // Create the report for each paint name
   //
   for (int i = 0; i < numPaintNames; i++) {
      const int offset = i * numColumns;
      
      //
      // Determine number of zero entries
      //
      int numZero = numVoxelsInROI;
      for (int j = 1; j < numColumns; j++) {
         numZero -= totalCounts[offset + j];
      }
      totalCounts[offset] = numZero;
      
      //
      // Print counts
      //
      std::ostringstream str;
      str << paintNames[i].toAscii().constData() << " - number of voxels with:\n";
      for (int j = 0; j < numColumns; j++) {
         str << j << " entries" << separator.toAscii().constData();
      }
      str << "\n";
      for (int j = 0; j < numColumns; j++) {
         str << totalCounts[offset + j] << separator.toAscii().constData();
      }
      str << "\n";
      operationText += QString(str.str().c_str());
      
      //
      // Print percentages
      //
      str.str("");
      str.precision(2);
      str.setf(std::ios::fixed);
      str << paintNames[i].toAscii().constData() << " - percent of voxels with:\n";
      for (int j = 0; j < numColumns; j++) {
         str << j << " entries" << separator.toAscii().constData();
      }
      str << "\n";
      for (int j = 0; j < numColumns; j++) {
         const float pct = (static_cast<float>(totalCounts[offset + j]) / 
                            static_cast<float>(numVoxelsInROI)) * 100.0;
         str << pct << separator.toAscii().constData();
      }
      str << "\n";
      operationText += QString(str.str().c_str());
   }

   const int totalNumberOfVoxels = dim[0] * dim[1] * dim[2];
   reportTextOut = createReport(paintVolumes,
                                "Probabilistic Atlas Overlap Analysis",
                                operationText,
                                totalNumberOfVoxels,
                                numVoxelsInROI);                                
}

/**
 * determine which voxels are in the ROI volume.
 */
int 
BrainModelVolumeRegionOfInterest::determineVoxelsWithinVolumeROI(const VolumeFile* volume,
                                                                 const float minValue,
                                                                 const float maxValue,
                                                                 std::vector<int>& voxelInROI) const
{
   voxelInROI.resize(volume->getTotalNumberOfVoxels());
   std::fill(voxelInROI.begin(), voxelInROI.end(), false);
   
   int dim[3];
   volume->getDimensions(dim);
   
   int ctr = 0;
   
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const float voxel = volume->getVoxel(i, j, k);
            if ((voxel >= minValue) &&
                (voxel <= maxValue)) {
               //
               // Get stereotaxic coordinate of volume's voxel
               //
               float xyz[3];
               volume->getVoxelCoordinate(i, j, k, xyz);
               
               //
               // See if voxel is in the ROI using the stereotaxic coordinate
               //
               if (insideVolumeROI(xyz)) {
                  const int flatIndex = volume->getVoxelDataIndex(i, j, k);
                  voxelInROI[flatIndex] = true;
                  ctr++;
               }
            }
         }
      }
   }
   
   return ctr;
}
                                      
/**
 * segmentation region center of gravity report.
 */
void 
BrainModelVolumeRegionOfInterest::operationSegmentationRegionCenterOfGravity(
                                       const VolumeFile *segmentVolume,
                                       QString& reportTextOut) const throw (BrainModelAlgorithmException)
{
   reportTextOut = "";
   
   if (getNumberOfVoxelsInROI() <= 0) {
      throw BrainModelAlgorithmException("The region of interest contains no voxels.");
   }
   
   //
   //
   // Determine the COG of the voxels in the ROI
   //
   double voxelCOG[3] = { 0.0, 0.0, 0.0 };
   double coordCOG[3] = { 0.0, 0.0, 0.0 };
   double counter = 0;
   int dim[3];
   segmentVolume->getDimensions(dim);
   
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const float voxel = segmentVolume->getVoxel(i, j, k);
            if (voxel != 0.0) {
               //
               // Get stereotaxic coordinate of segmentation voxel
               //
               float xyz[3];
               segmentVolume->getVoxelCoordinate(i, j, k, xyz);
               
               //
               // See if voxel is in the ROI using the stereotaxic coordinate
               //
               if (insideVolumeROI(xyz)) {
                  voxelCOG[0] += i;
                  voxelCOG[1] += j;
                  voxelCOG[2] += k;
                  
                  coordCOG[0] += xyz[0];
                  coordCOG[1] += xyz[1];
                  coordCOG[2] += xyz[2];

                  counter += 1.0;
               }
            }
         }
      }
   }
   
   QString operationText;
   
   if (counter > 0) {
      const QString s1 =  
         ("Voxel IJK C.O.G.: ("
          + QString::number(voxelCOG[0] / counter, 'f', 0) + ", "
          + QString::number(voxelCOG[1] / counter, 'f', 0) + ", "
          + QString::number(voxelCOG[2] / counter, 'f', 0) + ")\n");
      operationText += s1;
      
      const QString s2 = 
         ("Voxel Coordinate C.O.G.: ("
          + QString::number(coordCOG[0] / counter, 'f', 3) + ", "
          + QString::number(coordCOG[1] / counter, 'f', 3) + ", "
          + QString::number(coordCOG[2] / counter, 'f', 3) + ")\n");
      operationText += s2;
   }
   else {
      operationText += "There are no non-zero segmentation voxels in the ROI.\n";
   }

   const int totalNumberOfVoxels = dim[0] * dim[1] * dim[2];
   reportTextOut = createReport(segmentVolume,
                                "Segmentation Center of Gravity",
                                operationText,
                                totalNumberOfVoxels,
                                static_cast<int>(counter));                                
}

/**
 * see if a stereotaxic coordinate is inside the ROI.
 */
bool 
BrainModelVolumeRegionOfInterest::insideVolumeROI(const float xyz[3]) const
{
   int ijk[3];
   if (roiVolume->convertCoordinatesToVoxelIJK(xyz, ijk)) {
      if (roiVolume->getVoxel(ijk) != 0.0) {
         return true;
      }
   }
   return false;
}

/**
 * reset the ROI volume.
 */
void 
BrainModelVolumeRegionOfInterest::resetROIVolume(const VolumeFile* vf,
                                                 const bool colorTheVolumesVoxelsFlag)
{
   if (vf != NULL) {
      //
      // Delete existing volume
      //
      if (roiVolume != NULL) {
         delete roiVolume;
         roiVolume = NULL;
      }
      
      //
      // Copy the user's volume
      //
      roiVolume = new VolumeFile(*vf);
      
      //
      // If voxels should be colored
      //
      if (colorTheVolumesVoxelsFlag) {
         //
         // Color all voxels
         //
         BrainModelVolumeVoxelColoring* voxelColoring = brainSet->getVoxelColoring();
         voxelColoring->colorAllOfTheVolumesVoxels(roiVolume);
         
         //
         // Change the volume type so that the volume gets colored in the ROI color
         // Need to do this so after coloring copied volume so that the voxel display flags
         // are set but before changing the type to ROI for ROI coloring
         //
         roiVolume->setVolumeType(VolumeFile::VOLUME_TYPE_ROI);
   
         //
         // If any voxels have color set the voxel value to 1.0 else 0.0 if voxel color invalid
         // and set voxel color invalid so that it will get colored with the proper ROI color
         //
         const unsigned char invalidColor[4] = { 0, 0, 0, VolumeFile::VOXEL_COLOR_STATUS_INVALID };
         int dim[3] = { 0, 0, 0 };
         roiVolume->getDimensions(dim);
         for (int i = 0; i < dim[0]; i++) {
            for (int j = 0; j < dim[1]; j++) {
               for (int k = 0; k < dim[2]; k++) {
                  unsigned char rgb[4];
                  roiVolume->getVoxelColor(i, j, k, rgb); 
                  if (rgb[3] == VolumeFile::VolumeFile::VOXEL_COLOR_STATUS_VALID) {
                     roiVolume->setVoxel(i, j, k, 0, 1.0);
                  }
                  else {
                     roiVolume->setVoxel(i, j, k, 0, 0.0);
                  }
                  roiVolume->setVoxelColor(i, j, k, invalidColor);
               }
            }
         }
      }
   }
   else {
      //
      // create an empty volume
      //
      int dim[3] = { 0, 0, 0 };
      VolumeFile::ORIENTATION orient[3];
      float origin[3] = { 0.0, 0.0, 0.0 };
      float spacing[3] = { 1.0, 1.0, 1.0 };
      roiVolume->initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT,
                            dim,
                            orient,
                            origin,
                            spacing,
                            true,  // clear the voxels,
                            true); // allocate voxel data
   }
   
   //
   // Change the volume type so that the volume gets colored in the ROI color
   //
   roiVolume->setVolumeType(VolumeFile::VOLUME_TYPE_ROI);
   
   reportROIVolumeInfoText = "";
}

/**
 * set the volume ROI to all voxels in volume.
 */
void 
BrainModelVolumeRegionOfInterest::setVolumeROIToAllVoxels(const VolumeFile* vf)
{
   resetROIVolume(vf, false);
   if (vf == NULL) {
      return;
   }

   //
   // Set all voxels to one
   //   
   int dim[3];
   roiVolume->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            roiVolume->setVoxel(i, j, k, 0, 1.0);
         }
      }
   }
   
   reportROIVolumeInfoText = "Selection Mode: All voxels within volume "
                       + FileUtilities::basename(vf->getFileName())
                       + "\n";
}

/**
 * set the volume ROI to all displayed voxels in volume.
 */
void 
BrainModelVolumeRegionOfInterest::setVolumeROIToAllDisplayedVoxels(const VolumeFile* vf)
{
   resetROIVolume(vf, true);
   if (vf == NULL) {
      return;
   }
      
   reportROIVolumeInfoText = "Selection Mode: All displayed voxels within volume "
                       + FileUtilities::basename(vf->getFileName())
                       + "\n";
}

/**
 * set the volume ROI to all paint voxels with the specified name.
 */
void 
BrainModelVolumeRegionOfInterest::setVolumeROIToPaintIDVoxels(const VolumeFile* vf,
                                                              const QString& paintName)
{
   resetROIVolume(vf, false);
   if (vf == NULL) {
      return;
   }

   const int paintIndex = vf->getRegionIndexFromName(paintName);
   
   //
   // Set all voxels with proper paint ID to one
   //
   int dim[3];
   vf->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const int value = static_cast<int>(vf->getVoxel(i, j, k, 0));
            if (value == paintIndex) {
               roiVolume->setVoxel(i, j, k, 0, 1.0);
            }
            else {
               roiVolume->setVoxel(i, j, k, 0, 0.0);
            }
         }
      }
   }
   
   reportROIVolumeInfoText = "Selection Mode: All voxels with label "
                       + paintName
                       + " in volume "
                       + FileUtilities::basename(vf->getFileName())
                       + "\n";
}
                                       
/**
 * set the volume ROI to all voxels within a range in volume.
 */
void 
BrainModelVolumeRegionOfInterest::setVolumeROIToVoxelsInRangeOfValues(const VolumeFile* vf,
                                                                      const float minValue,
                                                                      const float maxValue)
{
   resetROIVolume(vf, false);
   if (vf == NULL) {
      return;
   }

   //
   // Set voxels within range to one, others to zero
   //
   int dim[3];
   vf->getDimensions(dim);
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const float value = vf->getVoxel(i, j, k, 0);
            if ((value >= minValue) &&
                (value <= maxValue)) {
               roiVolume->setVoxel(i, j, k, 0, 1.0);
            }
            else {
               roiVolume->setVoxel(i, j, k, 0, 0.0);
            }
         }
      }
   }
   
   reportROIVolumeInfoText = "Selection Mode: All voxels with value in range ("
                       + QString::number(minValue, 'f', 3)
                       + " , "
                       + QString::number(maxValue, 'f', 3)
                       + ") in volume "
                       + FileUtilities::basename(vf->getFileName())
                       + "\n";
}

/**
 * create the report.
 */
QString 
BrainModelVolumeRegionOfInterest::createReport(const VolumeFile* operatingVolume,
                                               const QString& description,
                                               const QString& operationText,
                                               const int totalNumberOfVoxels,
                                               const int numVoxelsInROI) const
{
   std::vector<VolumeFile*> volumeVector;
   volumeVector.push_back((VolumeFile*)operatingVolume);
   return createReport(volumeVector,
                description,
                operationText,
                totalNumberOfVoxels,
                numVoxelsInROI);
}

/**
 * create the report.
 */
QString 
BrainModelVolumeRegionOfInterest::createReport(const std::vector<VolumeFile*> operatingVolumes,
                                               const QString& description,
                                               const QString& operationText,
                                               const int totalNumberOfVoxels,
                                               const int numVoxelsInROI) const
{
   QString txt = 
         "\n"
       + description
       + "\n\n"
       + reportROIVolumeInfoText;
   for (unsigned int i = 0; i < operatingVolumes.size(); i++) {
       + "Operating on volume: "
       + FileUtilities::basename(operatingVolumes[i]->getFileName())
       + "\n"
       + "              label: "
       + operatingVolumes[i]->getDescriptiveLabel()
       + "\n";
   }
   txt +=
         "\n"
       + QString::number(numVoxelsInROI)
       + " of "
       + QString::number(totalNumberOfVoxels)
       + " voxels are in the ROI."
       + "\n\n"
       + operationText;
       
   return txt;
}
