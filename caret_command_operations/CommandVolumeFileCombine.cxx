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

#include "CommandVolumeFileCombine.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeFileCombine::CommandVolumeFileCombine()
   : CommandBase("-volume-file-combine",
                 "VOLUME FILE COMBINE")
{
}

/**
 * destructor.
 */
CommandVolumeFileCombine::~CommandVolumeFileCombine()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeFileCombine::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addMultipleFiles("Input Volume File Name(s)", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addVariableListOfParameters("Optional Parameters");
}

/**
 * get full help information.
 */
QString 
CommandVolumeFileCombine::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<input-volume-file-1-name>\n"
       + indent9 + "[additional-input-volume-files]\n"
       + indent9 + "[-paint]\n"
       + indent9 + "Combine the volumes into a single, multi-volume file.\n"
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
CommandVolumeFileCombine::executeCommand() throw (BrainModelAlgorithmException,
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
   std::vector<VolumeFile*> allVolumes;
   for (unsigned int i = 0; i < inputVolumeFileNames.size(); i++) {
      std::vector<VolumeFile*> volumesRead;
      VolumeFile::readFile(inputVolumeFileNames[i],
                           -1,
                           volumesRead);
      allVolumes.insert(allVolumes.end(),
                        volumesRead.begin(), volumesRead.end());
   }
   if (allVolumes.empty()) {
      throw CommandException("No volumes were read.");
   }
   
   //
   // Handle paint names
   //
   VolumeFile::VOLUME_TYPE volumeType = allVolumes[0]->getVolumeType();
   if (paintFlag) {
      volumeType = VolumeFile::VOLUME_TYPE_PAINT;
   }
   
   //
   // Write the output volume file
   //
   VolumeFile::writeFile(outputVolumeFileName,
                         volumeType,
                         allVolumes[0]->getVoxelDataType(),
                         allVolumes);
                         
   //
   // Free memory
   //
   for (unsigned int i = 0; i < allVolumes.size(); i++) {
      delete allVolumes[i];
   }
}

      

