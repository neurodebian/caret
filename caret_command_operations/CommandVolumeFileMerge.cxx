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

#include "CommandVolumeFileMerge.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeFileMerge::CommandVolumeFileMerge()
   : CommandBase("-volume-file-merge",
                 "VOLUME FILE MERGE LEFT and RIGHT")
{
}

/**
 * destructor.
 */
CommandVolumeFileMerge::~CommandVolumeFileMerge()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeFileMerge::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Left Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Right Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addVariableListOfParameters("Optional Parameters");
}

/**
 * get full help information.
 */
QString 
CommandVolumeFileMerge::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<left-input-volume-filename>\n"
       + indent9 + "<right-input-volume-filename>\n"
       + indent9 + "[other-input-volume-files]\n"
       + indent9 + "[-paint]\n"
       + indent9 + "\n"
       + indent9 + "Merge the volumes into a single volume file.\n"
       + indent9 + "\n"
       + indent9 + "The purpose of this command is to merge left and right \n"
       + indent9 + "hemisphere volumes into a single volume file.  The input\n"
       + indent9 + "volumes must all have the same dimensions.  If the input\n"
       + indent9 + "volumes are multi-brick volumes, each input file must \n"
       + indent9 + "contain the same number of bricks, each brick is merged\n"
       + indent9 + "with the corresponding bricks in the other input volume\n"
       + indent9 + "files, and the output volume file will contain the same\n"
       + indent9 + "number of bricks.\n"
       + indent9 + "\n"
       + indent9 + "When merging the voxels, the output voxel is set to the \n"
       + indent9 + "largest of the corresponding input voxels.\n"
       + indent9 + "\n"
       + indent9 + "If the volumes are paint or probabilistic atlas volume\n"
       + indent9 + "files, the \"-paint\" option must be specified so that\n"
       + indent9 + "paint name indices are synchronized throughout all of\n"
       + indent9 + "the volumes.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeFileMerge::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File Name");
   std::vector<QString> inputVolumeFileNames;
   inputVolumeFileNames.push_back(
      parameters->getNextParameterAsString("Left Input Volume File Name"));
   inputVolumeFileNames.push_back(
      parameters->getNextParameterAsString("Right Input Volume File Name"));
   bool paintFlag = false;
   while (parameters->getParametersAvailable()) {
      const QString name = parameters->getNextParameterAsString("Optional parameter");
      if (name == "-paint") {
         paintFlag = true;
      }
      else {
         inputVolumeFileNames.push_back(name);
      }
   }
   
   //
   // Read all of the input volumes
   //
   std::vector<VolumeFile*> allInputVolumes;
   std::vector<std::vector<VolumeFile*> > inputVolumes;
   for (unsigned int i = 0; i < inputVolumeFileNames.size(); i++) {
      std::vector<VolumeFile*> volumesRead;
      VolumeFile::readFile(inputVolumeFileNames[i],
                           -1,
                           volumesRead);
      allInputVolumes.insert(allInputVolumes.end(),
                        volumesRead.begin(), volumesRead.end());
      inputVolumes.push_back(volumesRead);
   }
   
   //
   // Verify all volumes are the same dimensions
   //
   const VolumeFile* firstVolume = allInputVolumes[0];
   int dim[3];
   firstVolume->getDimensions(dim);
   const int totalNumberOfVolumes = static_cast<int>(allInputVolumes.size());
   for (int i = 1; i < totalNumberOfVolumes; i++) {
      int dimOther[3];
      allInputVolumes[i]->getDimensions(dimOther);
      if ((dim[0] != dimOther[0]) ||
          (dim[1] != dimOther[1]) ||
          (dim[2] != dimOther[2])) {
         throw CommandException("The input volumes are of different dimensions");
      }
   }
   
   //
   // Verify that the input volumes contain the same number of bricks
   //
   const int numberOfInputVolumes = static_cast<int>(inputVolumes.size());
   if (numberOfInputVolumes <= 1) {
      throw CommandException("There must be at least two input volume files.");
   }
   const int numBricksPerVolume = static_cast<int>(inputVolumes[0].size());
   for (int i = 1; i < numberOfInputVolumes; i++) {
      if (static_cast<int>(inputVolumes[i].size()) != numBricksPerVolume) {
      }
   }
   
   //
   // Handle paint names
   //
   VolumeFile::VOLUME_TYPE volumeType = allInputVolumes[0]->getVolumeType();
   if (paintFlag) {
      volumeType = VolumeFile::VOLUME_TYPE_PAINT;
   }
   if (volumeType == VolumeFile::VOLUME_TYPE_PAINT) {
      VolumeFile::synchronizeRegionNames(allInputVolumes);
   }
   
   //
   // Loop through the bricks
   //
   for (int brickNum = 0; brickNum < numBricksPerVolume; brickNum++) {
      //
      // Loop through the voxels
      //
      for (int i = 0; i < dim[0]; i++) {
         for (int j = 0; j < dim[1]; j++) {
            for (int k = 0; k < dim[2]; k++) {
               //
               // Get voxel from first volume
               //
               float voxel = inputVolumes[0][brickNum]->getVoxel(i, j, k, 0);

               //
               // Get the maximum value for the voxel from all corresponding bricks
               //
               for (int volumeNum = 1; volumeNum < numberOfInputVolumes; volumeNum++) {
                  voxel = std::max(voxel,
                        inputVolumes[volumeNum][brickNum]->getVoxel(i, j, k, 0));
               }
               
               //
               // Save the voxel into the first volume
               //
               inputVolumes[0][brickNum]->setVoxel(i, j, k, 0, voxel);
            }
         }
      }
   }

   //
   // Append the comments
   //
   QString comment;
   for (int volumeNum = 0; volumeNum < numberOfInputVolumes; volumeNum++) {
      comment += inputVolumes[volumeNum][0]->getFileComment() + "\n";
   }
   inputVolumes[0][0]->setFileComment(comment);
   
   //
   // Append the descriptive labels
   //
   for (int brickNum = 0; brickNum < numBricksPerVolume; brickNum++) {
      QString label;
      for (int volumeNum = 0; volumeNum < numberOfInputVolumes; volumeNum++) {
         label += inputVolumes[volumeNum][brickNum]->getDescriptiveLabel() + " ";
      }
      inputVolumes[0][brickNum]->setDescriptiveLabel(label);
   }
   
   //
   // Write the output volume file
   //
   VolumeFile::writeFile(outputVolumeFileName,
                         volumeType,
                         inputVolumes[0][0]->getVoxelDataType(),
                         inputVolumes[0]);
                         
   //
   // Free memory
   //
   for (unsigned int i = 0; i < allInputVolumes.size(); i++) {
      delete allInputVolumes[i];
   }
}

      

