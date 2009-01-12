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

#include "CommandVolumeSetOrigin.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeSetOrigin::CommandVolumeSetOrigin()
   : CommandBase("-volume-set-origin",
                 "VOLUME SET ORIGIN")
{
}

/**
 * destructor.
 */
CommandVolumeSetOrigin::~CommandVolumeSetOrigin()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSetOrigin::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFloat("X-Axis-Origin (Center of First Voxel)");
   paramsOut.addFloat("Y-Axis-Origin (Center of First Voxel)");
   paramsOut.addFloat("Z-Axis-Origin (Center of First Voxel)");
}

/**
 * get full help information.
 */
QString 
CommandVolumeSetOrigin::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<x-axis-origin-at-center-of-first-voxel>\n"
       + indent9 + "<y-axis-origin-at-center-of-first-voxel>\n"
       + indent9 + "<z-axis-origin-at-center-of-first-voxel>\n"
       + indent9 + "\n"
       + indent9 + "Set the origin for the volume file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSetOrigin::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get inputs
   //
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name/Label",
                                                        outputVolumeFileName, 
                                                        outputVolumeFileLabel);
   const float origin[3] = {
      parameters->getNextParameterAsFloat("X-Axis-Origin-At-Center-of-First-Voxel"),
      parameters->getNextParameterAsFloat("Y-Axis-Origin-At-Center-of-First-Voxel"),
      parameters->getNextParameterAsFloat("Z-Axis-Origin-At-Center-of-First-Voxel")
   };
   checkForExcessiveParameters();
   
   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // set the origin
   //
   volume.setOrigin(origin);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

