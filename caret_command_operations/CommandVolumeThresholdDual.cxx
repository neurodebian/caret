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

#include "CommandVolumeThresholdDual.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeThresholdDual::CommandVolumeThresholdDual()
   : CommandBase("-volume-threshold-dual",
                 "VOLUME THRESHOLDING DUAL")
{
}

/**
 * destructor.
 */
CommandVolumeThresholdDual::~CommandVolumeThresholdDual()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeThresholdDual::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFloat("Threshold Low Value", 127);
   paramsOut.addFloat("Threshold High Value", 128);
}

/**
 * get full help information.
 */
QString 
CommandVolumeThresholdDual::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<threshold-low-value>\n"
       + indent9 + "<threshold-high-value>\n"
       + indent9 + "\n"
       + indent9 + "Threshold a volume file.\n"
       + indent9 + "\n"
       + indent9 + "Voxels between the low and high threshold values are set \n"
       + indent9 + "to 255 and all other voxels are set to zero.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeThresholdDual::executeCommand() throw (BrainModelAlgorithmException,
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
   const float thresholdLow = parameters->getNextParameterAsFloat("Low Threshold");
   const float thresholdHigh = parameters->getNextParameterAsFloat("High Threshold");
   
   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // Threshold the volume
   //
   volume.dualThresholdVolume(thresholdLow, thresholdHigh);
      
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

