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

#include "CommandVolumeRescaleVoxels.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeRescaleVoxels::CommandVolumeRescaleVoxels()
   : CommandBase("-volume-rescale-voxels",
                 "VOLUME RESCALE VOXELS")
{
}

/**
 * destructor.
 */
CommandVolumeRescaleVoxels::~CommandVolumeRescaleVoxels()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeRescaleVoxels::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFloat("Input Minimum Value", 0);
   paramsOut.addFloat("Input Maximum Value", 255);
   paramsOut.addFloat("Output Minimum Value", 0);
   paramsOut.addFloat("Output Maximum Value", 255);
}

/**
 * get full help information.
 */
QString 
CommandVolumeRescaleVoxels::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<input-minimum-value>\n"
       + indent9 + "<input-maximum-value>\n"
       + indent9 + "<output-minimum-value>\n"
       + indent9 + "<output-maximum-value>\n"
       + indent9 + "\n"
       + indent9 + "Rescale a volume's voxels.\n"
       + indent9 + "\"input-min\" and below are mapped to \"output-min\".\n"
       + indent9 + "\"input-max\" and above are mapped to \"output-max\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeRescaleVoxels::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name/Label",
                                                        outputVolumeFileName, 
                                                        outputVolumeFileLabel);
   const float inputMinimum =
      parameters->getNextParameterAsFloat("Input Minimum");
   const float inputMaximum =
      parameters->getNextParameterAsFloat("Input Maximum");
   const float outputMinimum =
      parameters->getNextParameterAsFloat("Output Minimum");
   const float outputMaximum =
      parameters->getNextParameterAsFloat("Output Maximum");
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // set the orientation
   //
   volume.rescaleVoxelValues(inputMinimum, inputMaximum, outputMinimum, outputMaximum);
      
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

