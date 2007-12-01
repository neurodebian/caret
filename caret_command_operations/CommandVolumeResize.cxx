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

#include "CommandVolumeResize.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeResize::CommandVolumeResize()
   : CommandBase("-volume-resize",
                 "VOLUME RESIZE")
{
}

/**
 * destructor.
 */
CommandVolumeResize::~CommandVolumeResize()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeResize::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("x-min", 0);
   paramsOut.addInt("x-max", 0);
   paramsOut.addInt("y-min", 0);
   paramsOut.addInt("y-max", 0);
   paramsOut.addInt("z-min", 0);
   paramsOut.addInt("z-max", 0);
}

/**
 * get full help information.
 */
QString 
CommandVolumeResize::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<x-min>\n"
       + indent9 + "<x-max>\n"
       + indent9 + "<y-min>\n"
       + indent9 + "<y-max>\n"
       + indent9 + "<z-min>\n"
       + indent9 + "<z-max>\n"
       + indent9 + "\n"
       + indent9 + "Resize a volume.\n"
       + indent9 + "\n"
       + indent9 + "If \"min\" values are less than zero, voxels will be\n"
       + indent9 + "added to that side of the volume\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeResize::executeCommand() throw (BrainModelAlgorithmException,
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
   const int cropping[6] = {
      parameters->getNextParameterAsInt("x-min"),
      parameters->getNextParameterAsInt("x-max"),
      parameters->getNextParameterAsInt("y-min"),
      parameters->getNextParameterAsInt("y-max"),
      parameters->getNextParameterAsInt("z-min"),
      parameters->getNextParameterAsInt("z-max")
   };

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // resize the volume
   //
   volume.resize(cropping);
      
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

