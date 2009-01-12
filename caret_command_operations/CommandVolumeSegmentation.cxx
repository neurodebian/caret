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

#include <iostream>

#include "BrainModelSurfaceBorderLandmarkIdentification.h"
#include "BrainModelVolumeSureFitSegmentation.h"
#include "BrainSet.h"
#include "CommandVolumeSegmentation.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StringUtilities.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeSegmentation::CommandVolumeSegmentation()
   : CommandBase("-volume-segment",
                 "VOLUME SEGMENTATION")
{
}

/**
 * destructor.
 */
CommandVolumeSegmentation::~CommandVolumeSegmentation()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSegmentation::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("NIFTI");   descriptions.push_back("NIFTI");
   values.push_back("AFNI");   descriptions.push_back("AFNI");
   values.push_back("SPM");   descriptions.push_back("SPM");
   values.push_back("WUNIL");   descriptions.push_back("WUNIL");
   
   std::vector<QString> structValues, structDescriptions;
   structValues.push_back("LEFT");   structDescriptions.push_back("LEFT");
   structValues.push_back("RIGHT");   structDescriptions.push_back("RIGHT");
   
   std::vector<BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD> errorCorrectionValues;
   std::vector<QString> errorCorrectionNames;
   BrainModelVolumeSureFitSegmentation::getErrorCorrectionMethodsAndNames(
      errorCorrectionNames, errorCorrectionValues);
      
   paramsOut.clear();
   paramsOut.addFile("Input Anatomy Volume File Name", FileFilters::getVolumeAnatomyFileFilter());
   paramsOut.addFile("Input Segmentation Volume File Name", FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addFile("Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addString("Operation Code", "YYYYYYYYYNYYYYYY");
   paramsOut.addFloat("Gray Peak", 100.0);
   paramsOut.addFloat("White Peak", 170.0);
   paramsOut.addString("Padding Code", "NNNNNN");
   paramsOut.addListOfItems("Structure", structValues, structDescriptions);
   paramsOut.addListOfItems("Error Correction", errorCorrectionNames, errorCorrectionNames);
   paramsOut.addListOfItems("Volume Write Type", values, descriptions);
}

/**
 * get full help information.
 */
QString 
CommandVolumeSegmentation::getHelpInformation() const
{
   std::vector<BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD> errorCorrectionValues;
   std::vector<QString> errorCorrectionNames;
   BrainModelVolumeSureFitSegmentation::getErrorCorrectionMethodsAndNames(
      errorCorrectionNames, errorCorrectionValues);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-anatomy-volume-file-name>\n"
       + indent9 + "<input-segmentation-volume-file-name>\n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "<operation-code>\n"
       + indent9 + "<gray-peak>\n"
       + indent9 + "<white-peak>\n"
       + indent9 + "<padding-code>\n"
       + indent9 + "<structure>\n"
       + indent9 + "<error-correction-method>\n"
       + indent9 + "<write-volume-type>\n"
       + indent9 + " \n"
       + indent9 + "Perform segmentation operations.\n"
       + indent9 + " \n"
       + indent9 + "      Operation_Code characters \n"
       + indent9 + "         Specify each with either a \"Y\" or \"N\". \n"
       + indent9 + "         All characters must be specified. \n"
       + indent9 + "         Character   Operation Description \n"
       + indent9 + "         ---------   --------------------- \n"
       + indent9 + "                 1   Disconnect Eye and Skull \n"
       + indent9 + "                 2   Disconnect Hindbrain \n"
       + indent9 + "                 3   Use High Threshold for Hindbrain disconnection \n"
       + indent9 + "                 4   Cut Corpus Callossum \n"
       + indent9 + "                 5   Generate Segmentation \n"
       + indent9 + "                 6   Fill Ventricles \n"
       + indent9 + "                 7   Generate Raw and Fiducial Surfaces \n"
       + indent9 + "                 8   Reduce polygons in surfaces \n"
       + indent9 + "                 9   Correct topological errors in surfaces \n"
       + indent9 + "                10   Generate Inflated Surface \n"
       + indent9 + "                11   Generate Very Inflated Surface \n"
       + indent9 + "                12   Generate Ellipsoid Surface (For Flattening) \n"
       + indent9 + "                13   Generate Spherical Surface \n"
       + indent9 + "                14   Generate Comp Med Wall Surface \n"
       + indent9 + "                15   Generate Hull Surface  \n"
       + indent9 + "                16   Generate Curvature, Depth, and Paint Attributes \n"
       + indent9 + "                17   Generate Registration and Flattening Landmark Borders\n"
       + indent9 + " \n"
       + indent9 + "      gray-peak  specifies the intensity of the gray matter peak in the  \n"
       + indent9 + "                 anatomy volume. \n"
       + indent9 + " \n"
       + indent9 + "      white-peak  specifies the intensity of the white matter peak in the \n"
       + indent9 + "                  anatomy volume. \n"
       + indent9 + " \n"
       + indent9 + "      padding-code \n"
       + indent9 + "         Specify padding for any cut faces when segmenting a partial hemisphere. \n"
       + indent9 + "         Specify each with either a \"Y\" for padding or \"N\" for no padding. \n"
       + indent9 + "         All characters must be specified. \n"   
       + indent9 + "         Character   Padding Description \n"
       + indent9 + "         ---------   ------------------- \n"
       + indent9 + "                 1   Pad Negative X \n"
       + indent9 + "                 2   Pad Positive X \n"
       + indent9 + "                 3   Pad Posterior Y \n"
       + indent9 + "                 4   Pad Anterior Y \n"
       + indent9 + "                 5   Pad Inferior Z \n"
       + indent9 + "                 6   Pad Superior Z \n"
       + indent9 + " \n"
       + indent9 + "      structure  Specifies the brain structure. \n"
       + indent9 + "                  Acceptable values are RIGHT or LEFT \n"
       + indent9 + " \n"
       + indent9 + "      spec-file-name  Name of specification file. \n"
       + indent9 + " \n"
       + indent9 + "      input-anatomy-volume-file-name   \n"
       + indent9 + "         If there is not an anatomy volume file, leave this\n"
       + indent9 + "         item blank (two consecutive double quotes).\n"
       + indent9 + " \n"
       + indent9 + "      input-segmentation-volume-file-name   \n"
       + indent9 + "         If there is not a segmentation volume file, leave this\n"
       + indent9 + "         item blank (two consecutive double quotes).\n"
       + indent9 + " \n"
       + indent9 + "      error-correction-method\n");
       for (unsigned int i = 0; i < errorCorrectionNames.size(); i++) {
          helpInfo += (
               indent9 + indent9 + errorCorrectionNames[i] + "\n");
       }
   helpInfo += ("\n"
       + indent9 + "      write-volume-type   Type of volume files to write. \n"
       + indent9 + "         Specifies the type of the volume files that will be written \n"
       + indent9 + "         during the segmentation process.  Valid values are \n"
       + indent9 + "            AFNI \n"
       + indent9 + "            NIFTI  \n"
       + indent9 + "            NIFTI_GZIP   (RECOMMENDED!!!!) \n"
       + indent9 + "            SPM \n"
       + indent9 + "            WUNIL \n"
       + indent9 + " \n"
       + indent9 + "      All input volumes must be in a Left-Posterior-Inferior orientation \n"
       + indent9 + "      and their stereotaxic coordinates must be set so that the origin is  \n"
       + indent9 + "      at the anterior commissure. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSegmentation::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputAnatomyVolumeFileName =
      parameters->getNextParameterAsString("Input Anatomy Volume File Name");
   const QString inputSegmentationVolumeFileName =
      parameters->getNextParameterAsString("Input Segmentation Volume File Name");
   const QString specFileName =
      parameters->getNextParameterAsString("Spec File Name");
   const QString operationCode =
      parameters->getNextParameterAsString("Operation Code");
   const float grayPeak =
      parameters->getNextParameterAsFloat("Gray Peak");
   const float whitePeak =
      parameters->getNextParameterAsFloat("White Peak");
   const QString paddingCode =
      parameters->getNextParameterAsString("Padding Code");
   const QString structureName =
      parameters->getNextParameterAsString("Structure Name").toUpper();
   const QString errorCorrectionName =
      parameters->getNextParameterAsString("Error Correction Name");
   const QString writeVolumeTypeString =
      parameters->getNextParameterAsString("Write Volume Type");
   checkForExcessiveParameters();
   
   VolumeFile::FILE_READ_WRITE_TYPE writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI_GZIP;
   if (writeVolumeTypeString == "AFNI") {
      writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
   }
   else if (writeVolumeTypeString == "NIFTI") {
      writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
   }
   else if (writeVolumeTypeString == "NIFTI_GZIP") {
      writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI_GZIP;
   }
   else if (writeVolumeTypeString == "SPM") {
      writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX;
   }
   else if (writeVolumeTypeString == "WUNIL") {
      writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_WUNIL;
   }
   else  {
      throw CommandException("Invalid volume file write type: "
                             + writeVolumeTypeString);
   }
   //
   // Check the operation code
   //
   const int operationCodeLength = 17;
   if (operationCode.length() != operationCodeLength) {
      throw CommandException("Operation code must contain exactly "
                             + QString::number(operationCodeLength)
                             + " characters.");
   }
   for (int i = 0; i < operationCodeLength; i++) {
      if ((operationCode[i] != 'Y') && (operationCode[i] != 'N')) {
         throw CommandException("Operation code character "
                                + QString::number(i + 1)
                                + " must be \"Y\" or \"N\".");
      }
   }
   
   const bool disconnectEyeFlag          = (operationCode[0] == 'Y');
   const bool disconnectHindbrainFlag    = (operationCode[1] == 'Y');
   const bool disconnectHindHiThreshFlag = (operationCode[2] == 'Y');
   const bool cutCorpusCallossumFlag     = (operationCode[3] == 'Y');
   const bool generateSegmentationFlag   = (operationCode[4] == 'Y');
   const bool fillVentriclesFlag         = (operationCode[5] == 'Y');
   const bool rawFiducialSurfaceFlag     = (operationCode[6] == 'Y');
   const bool reduceSurfacePolygonsFlag  = (operationCode[7] == 'Y');
   const bool topologicalCorrectFlag     = (operationCode[8] == 'Y');
   const bool inflatedSurfaceFlag        = (operationCode[9] == 'Y');
   const bool veryInflatedSurfaceFlag    = (operationCode[10] == 'Y');
   const bool ellipsoidSurfaceFlag       = (operationCode[11] == 'Y');
   const bool sphericalSurfaceFlag       = (operationCode[12] == 'Y');
   const bool compMedWallSurfaceFlag     = (operationCode[13] == 'Y');
   const bool hullSurfaceFlag            = (operationCode[14] == 'Y');
   const bool depthCurveGeographyFlag    = (operationCode[15] == 'Y');
   const bool landmarksFlag              = (operationCode[16] == 'Y');
   
   //
   // Get the structure
   //
   Structure::STRUCTURE_TYPE structure;
   if (structureName == "RIGHT") {
      structure = Structure::STRUCTURE_TYPE_CORTEX_RIGHT;
   }
   else if (structureName == "LEFT") {
      structure = Structure::STRUCTURE_TYPE_CORTEX_LEFT;
   }
   else {
      throw CommandException("Structure must be either RIGHT or LEFT.\n"
                             "       Value entered \"" + structureName + "\"");
   }
   
   //
   // Determine padding
   //
   const int paddingCodeLength = 6;
   if (paddingCode.length() != paddingCodeLength) {
      throw CommandException("Padding code must have exactly "
                             + QString::number(paddingCodeLength)
                             + " characters.");
   }
   int paddingAmount[6] = { 0, 0, 0, 0, 0, 0 };
   for (int i = 0; i < paddingCodeLength; i++) {
      if (paddingCode[i] == 'Y') {
         paddingAmount[i] = 30;
      }
      else if (paddingCode[i] == 'N') {
         paddingAmount[i] = 0;
      }
      else {
         throw CommandException("Padding code character "
                                + QString::number(i + 1)
                                + " must be \"Y\" or \"N\".");
      }
   }
   
   //
   // Get error correction
   //
   std::vector<BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD> errorCorrectionValues;
   std::vector<QString> errorCorrectionNames;
   BrainModelVolumeSureFitSegmentation::getErrorCorrectionMethodsAndNames(
      errorCorrectionNames, errorCorrectionValues);
   
   BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD errorCorrectionMethod =
      BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD_NONE;
   bool foundErrorCorrectionFlag = false;
   for (unsigned int i = 0; i < errorCorrectionNames.size(); i++) {
       if (errorCorrectionName == errorCorrectionNames[i]) {
          errorCorrectionMethod = errorCorrectionValues[i];
          foundErrorCorrectionFlag = true;
          break;
       }
   }
   if (foundErrorCorrectionFlag == false) {
      throw CommandException("Invalid error correction: " + errorCorrectionName);
   }
   
   //
   // Check if generating landmarks is valid
   //
   if (landmarksFlag) {
      SpecFile sf;
      sf.readFile(specFileName);
      const StereotaxicSpace space = sf.getSpace();
      const bool validLandmarkSpaceFlag =
         BrainModelSurfaceBorderLandmarkIdentification::isStereotaxicSpaceSupported(space);
      if (validLandmarkSpaceFlag == false) {
         throw CommandException("Generation of flattening/landmark landmark borders "
                                "not supported for stereotaxic space: "
                                + space.getName());
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(specFileName, false, true);

   //
   // For anterior commissure voxel index
   //
   const float zeros[3] = { 0.0, 0.0, 0.0 };
   int acIJK[3];

   //
   // Read the anatomy volume file
   //
   VolumeFile inputAnatomyVolume;
   if (inputAnatomyVolumeFileName.isEmpty() == false) {
      inputAnatomyVolume.readFile(inputAnatomyVolumeFileName);
      inputAnatomyVolume.convertCoordinatesToVoxelIJK(zeros, acIJK);
   }
   
   //
   // Read the segmentation volume file
   //
   VolumeFile inputSegmentationVolume;
   if (inputSegmentationVolumeFileName.isEmpty() == false) {
      inputSegmentationVolume.readFile(inputSegmentationVolumeFileName);
      inputSegmentationVolume.convertCoordinatesToVoxelIJK(zeros, acIJK);
   }
   
   
   //
   // Create the segmentation object
   //
   BrainModelVolumeSureFitSegmentation 
      segmentationObject(&brainSet,
                         &inputAnatomyVolume,
                         &inputSegmentationVolume,
                         writeVolumeType,
                         acIJK,
                         paddingAmount,
                         whitePeak,
                         grayPeak,
                         0.0,
                         structure,
                         disconnectEyeFlag,
                         disconnectHindbrainFlag,
                         disconnectHindHiThreshFlag,
                         cutCorpusCallossumFlag,
                         generateSegmentationFlag,
                         fillVentriclesFlag,
                         errorCorrectionMethod,
                         rawFiducialSurfaceFlag,
                         (reduceSurfacePolygonsFlag == false),
                         topologicalCorrectFlag,
                         inflatedSurfaceFlag,
                         veryInflatedSurfaceFlag,
                         ellipsoidSurfaceFlag,
                         sphericalSurfaceFlag,
                         compMedWallSurfaceFlag,
                         hullSurfaceFlag,
                         depthCurveGeographyFlag,
                         landmarksFlag,
                         true);
   
   //
   // Execute the segmentation
   //
   segmentationObject.execute();
   
   const QString warningMessages = segmentationObject.getWarningMessages();
   if (warningMessages.isEmpty() == false) {
      std::cout << "Segmentation Warnings: " << warningMessages.toAscii().constData() << std::endl;
   }
}

      

