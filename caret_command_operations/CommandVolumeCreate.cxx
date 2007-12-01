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

#include "CommandVolumeCreate.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeCreate::CommandVolumeCreate()
   : CommandBase("-volume-create",
                 "VOLUME CREATE")
{
}

/**
 * destructor.
 */
CommandVolumeCreate::~CommandVolumeCreate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addInt("X-Dimension");
   paramsOut.addInt("X-Dimension");
   paramsOut.addInt("X-Dimension");
   paramsOut.addFile("Output Volume File", 
                         FileFilters::getVolumeGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeCreate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<x-dimension>\n"
       + indent9 + "<y-dimension>\n"
       + indent9 + "<z-dimension>\n"
       + indent9 + "<output-volume-file-name-and-label>\n"
       + indent9 + "\n"
       + indent9 + "Create a volume using the specified dimensions.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeCreate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const int dimensions[3] = {
      parameters->getNextParameterAsInt("X-Dimension"),
      parameters->getNextParameterAsInt("Y-Dimension"),
      parameters->getNextParameterAsInt("Z-Dimension")
   };      
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name",
                                                        outputVolumeFileName,
                                                        outputVolumeFileLabel);

   const float origin[3] = { 0.0, 0.0, 0.0 };
   const float spacing[3] = { 1.0, 1.0, 1.0 };
   const VolumeFile::ORIENTATION orient[3] = {
      VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
      VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
      VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR
   };

   VolumeFile volume;
   volume.initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT,
                     dimensions,
                     orient,
                     origin,
                     spacing);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

