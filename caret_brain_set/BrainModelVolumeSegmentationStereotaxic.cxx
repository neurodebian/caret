
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

#include <QDir>
#include <QFileInfo>

#include "BrainModelVolumeSegmentationStereotaxic.h"
#include "BrainSet.h"
#include "CommaSeparatedValueFile.h"
#include "FileUtilities.h"
#include "SegmentationMaskListFile.h"
#include "SpecFile.h"
#include "StatisticHistogram.h"
#include "StringTable.h"
#include "Structure.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelVolumeSegmentationStereotaxic::BrainModelVolumeSegmentationStereotaxic(
   BrainSet* brainSetIn,
   const VolumeFile* anatomicalVolumeFileIn,
   const int uniformityIterationsIn,
   const bool disconnectEyeFlagIn,
   const BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD errorCorrectVolumeMethodIn,
   const bool errorCorrectSurfaceFlagIn,
   const bool maxPolygonsFlagIn,
   const bool flatteningFilesFlagIn)
   : BrainModelAlgorithm(brainSetIn),
     anatomicalVolumeFile(anatomicalVolumeFileIn)
{
   disconnectEyeFlag = disconnectEyeFlagIn;
   uniformityIterations = uniformityIterationsIn;
   errorCorrectVolumeMethod = errorCorrectVolumeMethodIn;
   errorCorrectSurfaceFlag = errorCorrectSurfaceFlagIn;
   maxPolygonsFlag = maxPolygonsFlagIn;
   flatteningFilesFlag = flatteningFilesFlagIn;
}
                                        
/**
 * destructor.
 */
BrainModelVolumeSegmentationStereotaxic::~BrainModelVolumeSegmentationStereotaxic()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeSegmentationStereotaxic::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (brainSet == NULL) {
      throw BrainModelAlgorithmException("BrainSet is NULL.");
   }
   if (anatomicalVolumeFile == NULL) {
      throw BrainModelAlgorithmException("Anatomical volume file is NULL.");
   }
   const QString& stereotaxicSpaceName = brainSet->getStereotaxicSpace().getName();
   if (stereotaxicSpaceName.isEmpty()) {
      throw BrainModelAlgorithmException("SpecFile has no stereotaxic space.");
   }
   
   //
   // Only do left and right structures
   //
   const Structure::STRUCTURE_TYPE structure = brainSet->getStructure().getType();
   QString structureName;
   switch (structure) {
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
         structureName = "LEFT";
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         structureName = "RIGHT";
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
         throw BrainModelAlgorithmException("Structure BOTH not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_CEREBELLUM:
         throw BrainModelAlgorithmException("Structure CEREBELLUM not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
         throw BrainModelAlgorithmException("Structure CEREBELLUM OR LEFT not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
         throw BrainModelAlgorithmException("Structure CEREBELLUM OR RIGHT not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
         throw BrainModelAlgorithmException("Structure LEFT CEREBELLUM not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
         throw BrainModelAlgorithmException("Structure RIGHT CEREBELLUM not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
         throw BrainModelAlgorithmException("Structure CEREBRUM CEREBELLUM not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_SUBCORTICAL:
         throw BrainModelAlgorithmException("Structure SUBCORTICAL not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_ALL:
         throw BrainModelAlgorithmException("Structure ALL not allowed, must be RIGHT or LEFT");
         break;
      case Structure::STRUCTURE_TYPE_INVALID:
         throw BrainModelAlgorithmException("Structure in SpecFile is invalid");
         break;
   }
   
   //
   // Find the names of the mask volumes
   //
   QString maskVolumeName;
   getSegmentationMaskVolumeFileNames(stereotaxicSpaceName,
                                      structureName,
                                      maskVolumeName);
   
   //
   // Generate segmentation
   //
   generateSegmentation(maskVolumeName);
}

/**
 * generate the segmentation.
 */
void 
BrainModelVolumeSegmentationStereotaxic::generateSegmentation(
              const QString& maskVolumeFileName) throw (BrainModelAlgorithmException)
{
   //
   // Read the mask volume
   //
   VolumeFile maskVolume;
   try {
      maskVolume.readFile(maskVolumeFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
   //
   // Get extent of the non-zero voxels in the max volume
   //
   int maskExtent[6];
   float coordExtent[6];
   maskVolume.getNonZeroVoxelExtent(maskExtent, coordExtent);
   const int padExtent = 1;
   maskExtent[0] -= padExtent;
   maskExtent[1] += padExtent;
   maskExtent[2] -= padExtent;
   maskExtent[3] += padExtent;
   maskExtent[4] -= padExtent;
   maskExtent[5] += padExtent;
   
   //
   // Convert mask extent to stereotaxic coordinates
   //
   const int minMaskExtent[3] = { maskExtent[0], maskExtent[2], maskExtent[4] };
   const int maxMaskExtent[3] = { maskExtent[1], maskExtent[3], maskExtent[5] };
   float minExtentXYZ[3], maxExtentXYZ[3];
   maskVolume.getVoxelCoordinate(minMaskExtent, minExtentXYZ);
   maskVolume.getVoxelCoordinate(maxMaskExtent, maxExtentXYZ);

   //
   // Add anatomical volume name to spec file
   //
   brainSet->addToSpecFile(SpecFile::getVolumeAnatomyFileTag(),
                           anatomicalVolumeFile->getFileName());
   
   //
   // Copy anatomical volume
   //
   VolumeFile volume(*anatomicalVolumeFile);
   
   //
   // Convert mask extent stereotaxic coordinates into input volume slices
   //
   int minCropping[3], maxCropping[3];
   volume.convertCoordinatesToVoxelIJK(minExtentXYZ,
                                       minCropping);
   volume.convertCoordinatesToVoxelIJK(maxExtentXYZ,
                                       maxCropping);
   const int cropping[6] = {
      minCropping[0],
      maxCropping[0],
      minCropping[1],
      maxCropping[1],
      minCropping[2],
      maxCropping[2]
   };
   
   //
   // Crop the input volume
   //
   volume.resize(cropping);
   writeDebugVolume(volume, "anatomy_cropped");
   
   //
   // Mask the volume to extract the hemisphere
   // DO NOT DO THIS FOR NOW as it causes a problem with cutting the corpus callosum
   //   
   //volume.maskWithVolume(&maskVolume);
   
   //
   // Stretch the voxels to 0 to 255
   //
   volume.stretchVoxelValues();
   writeDebugVolume(volume, "anatomy_0_255");
   
   //
   // Get a histogram for the volume
   //
   StatisticHistogram uniformHistogram = *(volume.getHistogram());
   
   //
   // get histogram measurements
   //
   int csfPeakBucketNumber;
   int grayPeakBucketNumber;
   int whitePeakBucketNumber;
   int grayMinimumBucketNumber;
   int whiteMaximumBucketNumber;
   int grayWhiteBoundaryBucketNumber;
   uniformHistogram.getGrayWhitePeakEstimates(grayPeakBucketNumber,
                                              whitePeakBucketNumber,
                                              grayMinimumBucketNumber,
                                              whiteMaximumBucketNumber,
                                              grayWhiteBoundaryBucketNumber,
                                              csfPeakBucketNumber);
                                              
   //
   // Perform non-uniformity correction on the volume
   //
   if (uniformityIterations > 0) {
      const float uniformGrayMin = uniformHistogram.getDataValueForBucket(grayMinimumBucketNumber);
      const float uniformWhiteMax = uniformHistogram.getDataValueForBucket(whiteMaximumBucketNumber);
      std::cout << "Bias correction Gray Min: " << uniformGrayMin
                << " White Max: " << uniformWhiteMax << std::endl;
      if ((uniformGrayMin > 0) &&
          (uniformWhiteMax > 0)) {      
         volume.biasCorrectionWithAFNI(static_cast<int>(uniformGrayMin),
                                       static_cast<int>(uniformWhiteMax),
                                       uniformityIterations);
                   
         writeDebugVolume(volume, "bias_corrected");
      }
      else {
         std::cout << "Uniformity correction skipped due to invalid gray/white." << std::endl;
      }
   }
                                    
   //
   // Get histogram for gray/white peaks
   //
   StatisticHistogram grayWhiteHistogram = *(volume.getHistogram());
   grayWhiteHistogram.getGrayWhitePeakEstimates(grayPeakBucketNumber,
                                              whitePeakBucketNumber,
                                              grayMinimumBucketNumber,
                                              whiteMaximumBucketNumber,
                                              grayWhiteBoundaryBucketNumber,
                                              csfPeakBucketNumber);
   const float whiteMatterMaximum = grayWhiteHistogram.getDataValueForBucket(whiteMaximumBucketNumber);
   std::cout << "White matter maximum for segmentation: "
             << whiteMatterMaximum << std::endl;
             
   //
   // Get the peaks and adjust the gray peak a little bit low
   //
   const int grayOffset = 0; //-3;
   const float grayPeak = grayWhiteHistogram.getDataValueForBucket(grayPeakBucketNumber
                                                                   + grayOffset);
   const float whitePeak = grayWhiteHistogram.getDataValueForBucket(whitePeakBucketNumber);
   std::cout << "Gray Peak: " << grayPeak
             << " White Peak: " << whitePeak << std::endl;
   if ((grayPeak <= 0) ||
       (whitePeak <= 0)) {
      throw BrainModelAlgorithmException("ERROR: Invalid gray or white peak.");
   }
   
   //
   // Get anterior commissure position
   //
   const float zeros[3] = { 0.0, 0.0, 0.0 };
   int acIJK[3];
   volume.convertCoordinatesToVoxelIJK(zeros, acIJK);
   
   //
   // Segment the volume
   //
   const int padding[6] = { 0, 0, 0, 0, 0, 0 };
   BrainModelVolumeSureFitSegmentation sureFit(brainSet,
                                               &volume,
                                               NULL,
                                               VolumeFile::FILE_READ_WRITE_TYPE_NIFTI_GZIP,
                                               acIJK,
                                               padding,
                                               whitePeak,
                                               grayPeak,
                                               0.0,
                                               brainSet->getStructure().getType(),
                                               disconnectEyeFlag,  // eye
                                               true,  // hindbrain
                                               true,  // high thresh
                                               true,   // cut corpus callosum
                                               true,   // segment anatomy
                                               true,   // fill ventricles
                                               errorCorrectVolumeMethod, // correct volume 
                                               true,   // generate surfaces
                                               maxPolygonsFlag,  // max polygons
                                               errorCorrectSurfaceFlag, // correct surface
                                               true,   // inflated
                                               flatteningFilesFlag,  // very inflated
                                               flatteningFilesFlag,  // ellipsoid
                                               flatteningFilesFlag,  // sphere
                                               flatteningFilesFlag,  // CMW
                                               flatteningFilesFlag,  // hull,
                                               flatteningFilesFlag,  // Depth, Curve, Geography
                                               false,  // Landmarks
                                               true);   // auto save
   sureFit.setVolumeMask(&maskVolume);
   //sureFit.setWhiteMatterMaximum(whiteMatterMaximum);
   sureFit.execute();
}

/**
 * get the segmentation mask volume file names.
 */
void 
BrainModelVolumeSegmentationStereotaxic::getSegmentationMaskVolumeFileNames(
                                       const QString& spaceNameIn,
                                       const QString& structureNameIn,
                                       QString& maskNameOut) throw (BrainModelAlgorithmException)
{               
   maskNameOut = "";
   
   //
   // Read in segmentation masks list file
   //
   const QString maskVolumesDirectory(BrainSet::getCaretHomeDirectory()
                                      + "/data_files/segmentation_masks/");
   const QString maskVolumeListFileName(maskVolumesDirectory
                                        + "mask_list.txt.csv");
   //CommaSeparatedValueFile maskVolumeListFile;
   SegmentationMaskListFile maskVolumeListFile;
   try {
      maskVolumeListFile.readFile(maskVolumeListFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
   
/*
   //
   // Get the Mask Volumes String Table
   //
   const QString maskTableName("MaskVolumes");
   const StringTable* maskVolumesTable = maskVolumeListFile.getDataSectionByName(maskTableName);
   if (maskVolumesTable == NULL) {
      throw BrainModelAlgorithmException("Unable to find table named "
                                         + maskTableName
                                         + " in "
                                         + maskVolumeListFileName);
   }
   
   //
   // Find columns numbers of data
   //
   const int spaceCol = maskVolumesTable->getColumnIndexFromName("Space");
   const int structureCol = maskVolumesTable->getColumnIndexFromName("Structure");
   const int volumeCol = maskVolumesTable->getColumnIndexFromName("MaskVolume");
   if ((spaceCol < 0) ||
       (structureCol < 0) ||
       (volumeCol < 0)) {
      throw BrainModelAlgorithmException("Missing required columns in " +
                                         maskVolumeListFileName);
   }
   
   //
   // find the mask volume names
   //
   const QString& spaceName(spaceNameIn.toLower());
   const QString& structureName(structureNameIn.toLower());
   const int numRows = maskVolumesTable->getNumberOfRows();
   for (int i = 0; i < numRows; i++) {
      if (spaceName == maskVolumesTable->getElement(i, spaceCol).toLower()) {
         if (structureName == maskVolumesTable->getElement(i, structureCol).toLower()) {
            maskNameOut = maskVolumesDirectory
                          + maskVolumesTable->getElement(i, volumeCol);
         }
      }
   }
*/
   maskNameOut = maskVolumeListFile.getSegmentationMaskFileName(spaceNameIn,
                                                                structureNameIn);
   if (maskNameOut.isEmpty()) {
      QString msg("Unable to find mask volume for space \""
                  + spaceNameIn
                  + "\" and structure \""
                  + structureNameIn
                  + "\".\n"
                  + "Available masks are: \n"
                  + maskVolumeListFile.getAvailableMasks("   "));

/*      for (int i = 0; i < numRows; i++) {
         msg += (maskVolumesTable->getElement(i, spaceCol).toLower()
                 + "   "
                 + maskVolumesTable->getElement(i, structureCol).toLower()
                 + "\n");
      }
 */
      throw BrainModelAlgorithmException(msg);
   }
   
   if (QFile::exists(maskNameOut) == false) {
      throw BrainModelAlgorithmException("Mask Volume " 
                                         + maskNameOut
                                         + " is missing.");
   }
}

/**
 * write debugging volume.
 */
void 
BrainModelVolumeSegmentationStereotaxic::writeDebugVolume(VolumeFile& vf,
                                                          const QString& fileNameIn)
{
   //if (DebugControl::getDebugOn()) {
      const QString name("seg_debug_" 
                         + fileNameIn 
                         + SpecFile::getNiftiGzipVolumeFileExtension());
      try {
         vf.writeFile(name);
      }
      catch (FileException& e) {
         std::cout << "Writing debug volume: " << e.whatQString().toAscii().constData() << std::endl;
      }
   //}
}


