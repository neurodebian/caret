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

#include "CommandVolumeScalePercent0to255.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeScalePercent0to255::CommandVolumeScalePercent0to255()
   : CommandBase("-volume-scale-percent-min-max-255",
                 "VOLUME SCALE VOXELS 0 to 255 WITH PERCENTAGE OF MIN/MAX")
{
}

/**
 * destructor.
 */
CommandVolumeScalePercent0to255::~CommandVolumeScalePercent0to255()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeScalePercent0to255::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFloat("Percent Minimum", 2.0);
   paramsOut.addFloat("Percent Maximum", 2.0);
}

/**
 * get full help information.
 */
QString 
CommandVolumeScalePercent0to255::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<percent-minimum> \n"
       + indent9 + "<percent-maximum> \n"
       + indent9 + "\n"
       + indent9 + "Scale voxels.\n"
       + indent9 + "\n"
       + indent9 + "Scale voxels so that they are in the range 0 to 255 but\n"
       + indent9 + "map the first \"percent-minimum\" voxels to 0 and map the\n"
       + indent9 + "last \"percent-maximum\" voxels to 255.\n"
       + indent9 + "\n"
       + indent9 + "The \"percent-minimum\" and \"percent-maximum\" values should\n"
       + indent9 + "range from 0.0 to 100.0.  Setting the \"percent-minimum\" to\n"
       + indent9 + "2.0 and the \"percent-maximum\" to 3.0 will result in the first\n"
       + indent9 + "two percent of voxel values being mapped to zero and the last\n"
       + indent9 + "three percent of voxel values being mapped to two hundred\n"
       + indent9 + "fifty five.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeScalePercent0to255::executeCommand() throw (BrainModelAlgorithmException,
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
   const float percentMinimum =
      parameters->getNextParameterAsFloat("Percent Minimum");
   const float percentMaximum =
      parameters->getNextParameterAsFloat("Percent Maximum");
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // rescale
   //
   volume.stretchVoxelValuesExcludePercentage(percentMinimum, percentMaximum);
      
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

