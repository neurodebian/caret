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

#include "CommandVolumeSetSpacing.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeSetSpacing::CommandVolumeSetSpacing()
   : CommandBase("-volume-set-spacing",
                 "VOLUME SET SPACING")
{
}

/**
 * destructor.
 */
CommandVolumeSetSpacing::~CommandVolumeSetSpacing()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSetSpacing::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFloat("X-Axis-Spacing");
   paramsOut.addFloat("Y-Axis-Spacing");
   paramsOut.addFloat("Z-Axis-Spacing");
}

/**
 * get full help information.
 */
QString 
CommandVolumeSetSpacing::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<x-axis-spacing>\n"
       + indent9 + "<y-axis-spacing>\n"
       + indent9 + "<z-axis-spacing>\n"
       + indent9 + "\n"
       + indent9 + "Set the spacing for the volume file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSetSpacing::executeCommand() throw (BrainModelAlgorithmException,
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
   const float spacing[3] = {
      parameters->getNextParameterAsFloat("X-Axis-Spacing"),
      parameters->getNextParameterAsFloat("Y-Axis-Spacing"),
      parameters->getNextParameterAsFloat("Z-Axis-Spacing")
   };
   checkForExcessiveParameters();
   
   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // set the spacing
   //
   volume.setSpacing(spacing);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

