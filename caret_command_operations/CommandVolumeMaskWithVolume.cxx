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

#include "CommandVolumeMaskWithVolume.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"
/**
 * constructor.
 */
CommandVolumeMaskWithVolume::CommandVolumeMaskWithVolume()
   : CommandBase("-volume-mask-with-volume",
                 "VOLUME MASK WITH VOLUME")
{
}

/**
 * destructor.
 */
CommandVolumeMaskWithVolume::~CommandVolumeMaskWithVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMaskWithVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Input Mask Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("Dilation Iterations");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMaskWithVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<input-mask-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<dilation-iterations>\n"
       + indent9 + "\n"
       + indent9 + "Clear all of the voxels in the input volume that do map into\n"
       + indent9 + "a non-zero voxel in the mask volume.  The mapping is performed\n"
       + indent9 + "using stereotaxic coordinates so the volumes do not need to have\n"
       + indent9 + "have the same dimensions but must overlap in stereotaxic\n"
       + indent9 + "coordinates.  Create a mask volume using this program with the\n"
       + indent9 + "\"-volume-segment-mask-creation\" option.\n"
       + indent9 + "\n"
       + indent9 + "If \"dilation-iterations\" is non-zero, the mask will be dilated\n"
       + indent9 + "the specified number of times prior to the application of the mask.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeMaskWithVolume::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   const QString inputMaskVolumeFileName =
      parameters->getNextParameterAsString("Input MaskVolume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name/Label",
                                                        outputVolumeFileName, 
                                                        outputVolumeFileLabel);
   const int dilationIterations =
      parameters->getNextParameterAsInt("Dilation Iterations");
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // Read the input mask file
   //
   VolumeFile maskVolume;
   maskVolume.readFile(inputMaskVolumeFileName);
   
   //
   // Dilate mask volume
   if (dilationIterations > 0) {
      maskVolume.doVolMorphOps(dilationIterations, 0);
   }

   //
   // mask the volume
   //
   volume.maskWithVolume(&maskVolume);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

