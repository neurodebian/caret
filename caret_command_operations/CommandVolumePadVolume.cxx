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

#include "CommandVolumePadVolume.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumePadVolume::CommandVolumePadVolume()
   : CommandBase("-volume-pad-volume",
                 "VOLUME PAD A VOLUME")
{
}

/**
 * destructor.
 */
CommandVolumePadVolume::~CommandVolumePadVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumePadVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("Pad Neg X", 0, 0);
   paramsOut.addInt("Pad Pos X", 0, 0);
   paramsOut.addInt("Pad Neg Y", 0, 0);
   paramsOut.addInt("Pad Pos Y", 0, 0);
   paramsOut.addInt("Pad Neg Z", 0, 0);
   paramsOut.addInt("Pad Pos Z", 0, 0);
}

/**
 * get full help information.
 */
QString 
CommandVolumePadVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<pad-neg-x>\n"
       + indent9 + "<pad-pos-x>\n"
       + indent9 + "<pad-neg-y>\n"
       + indent9 + "<pad-pos-y>\n"
       + indent9 + "<pad-neg-z>\n"
       + indent9 + "<pad-pos-z>\n"
       + indent9 + "\n"
       + indent9 + "Add padding (empty voxels) around a volume.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumePadVolume::executeCommand() throw (BrainModelAlgorithmException,
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
   const int padding[6] = {
      parameters->getNextParameterAsInt("pad-neg-x"),
      parameters->getNextParameterAsInt("pad-pos-x"),
      parameters->getNextParameterAsInt("pad-neg-y"),
      parameters->getNextParameterAsInt("pad-pos-y"),
      parameters->getNextParameterAsInt("pad-neg-z"),
      parameters->getNextParameterAsInt("pad-pos-z")
   };
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // pad the volume
   //
   volume.padSegmentation(padding);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

