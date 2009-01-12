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

#include "BrainModelVolumeSureFitSegmentation.h"
#include "BrainSet.h"
#include "CommandSurfaceIdentifySulci.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandSurfaceIdentifySulci::CommandSurfaceIdentifySulci()
   : CommandBase("-surface-identify-sulci",
                 "SURFACE IDENTIFY SULCI")
{
}

/**
 * destructor.
 */
CommandSurfaceIdentifySulci::~CommandSurfaceIdentifySulci()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceIdentifySulci::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("NIFTI");   descriptions.push_back("NIFTI");
   values.push_back("AFNI");   descriptions.push_back("AFNI");
   values.push_back("SPM");   descriptions.push_back("SPM");
   values.push_back("WUNIL");   descriptions.push_back("WUNIL");
   
   std::vector<QString> structValues, structDescriptions;
   structValues.push_back("LEFT");   structDescriptions.push_back("LEFT");
   structValues.push_back("RIGHT");   structDescriptions.push_back("RIGHT");
   
   paramsOut.clear();
   paramsOut.addFile("Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addListOfItems("Structure", structValues, structDescriptions);
   paramsOut.addFile("Segmentation Volume File Name", FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addFile("Closed Topology File Name", FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Raw Coordinate File Name", FileFilters::getCoordinateRawFileFilter());
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addListOfItems("Volume Write Type", values, descriptions);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceIdentifySulci::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "<structure>\n"
       + indent9 + "<segmentation-volume-file-name>\n"
       + indent9 + "<closed-topology-file-name>\n"
       + indent9 + "<raw-coordinate-file-name>\n"
       + indent9 + "<fiducial-coordinate-file-name>\n"
       + indent9 + "<volume-write-type>\n"
       + indent9 + "\n"
       + indent9 + "Identify Sulci with shape and paint.\n"
       + indent9 + "\n"
       + indent9 + "Create a surface shape file containing depth and curvature measurements,\n"
       + indent9 + "a paint file identifying the sulci, and an area color file.  If there\n"
       + indent9 + "is no raw coordinate file, specify fiducial coordinate file instead.\n"
       + indent9 + "\n"
       + indent9 + "NOTE: This command MUST be run in the directory containing the files.\n"
       + indent9 + "\n"
       + indent9 + "structure  Specifies the brain structure. \n"
       + indent9 + "   Acceptable values are RIGHT or LEFT \n"
       + indent9 + " \n"
       + indent9 + "write-volume-type   Type of volume files to write. \n"
       + indent9 + "   Specifies the type of the volume files that will be written \n"
       + indent9 + "    during the segmentation process.  Valid values are: \n"
       + indent9 + "       AFNI \n"
       + indent9 + "       NIFTI   \n"
       + indent9 + "       NIFTI_GZIP (RECOMMENDED!!!!) \n"
       + indent9 + "       SPM \n"
       + indent9 + "       WUNIL \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceIdentifySulci::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString specFileName =
      parameters->getNextParameterAsString("Spec File Name");
   const QString structureName =
      parameters->getNextParameterAsString("Structure Name").toUpper();
   const QString segmentationVolumeFileName =
      parameters->getNextParameterAsString("Segmentation Volume File Name");
   const QString closedTopologyFileName =
      parameters->getNextParameterAsString("Closed Topology File Name");
   const QString rawCoordinateFileName =
      parameters->getNextParameterAsString("Raw Coordinate File Name");
   const QString fiducialCoordinateFileName =
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
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
      throw CommandException("Invalid volume file write type: " +
                             + writeVolumeType);
   }

   //
   // Load the segmentation volume to get the data file name
   //
   VolumeFile volTest;
   QString segmentationVolumeDataFileName;
   volTest.readFileMetaDataOnly(segmentationVolumeFileName);
   segmentationVolumeDataFileName = volTest.getDataFileName();
   
   //
   // Read spec file
   //
   SpecFile specFile;
   specFile.readFile(specFileName);

   //
   // Set the selected files
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getVolumeSegmentationFileTag(), segmentationVolumeFileName, 
                          segmentationVolumeDataFileName, SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getClosedTopoFileTag(), closedTopologyFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getRawCoordFileTag(), rawCoordinateFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getFiducialCoordFileTag(), fiducialCoordinateFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, specFileName, errorMessage)) {
      throw CommandException("Reading spec file: " +  errorMessage);
   }
   
   //
   // Identify the sulci
   //
   Structure structure(structureName);
   BrainModelVolumeSureFitSegmentation bmsf(&brainSet,
                                            structure.getType(),
                                            writeVolumeType,
                                            true);
   bmsf.executeIdentifySulci();
}

      

