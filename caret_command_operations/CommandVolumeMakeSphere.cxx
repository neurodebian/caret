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

#include "CommandVolumeMakeSphere.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeMakeSphere::CommandVolumeMakeSphere()
   : CommandBase("-volume-make-sphere",
                 "VOLUME MAKE SPHERE")
{
}

/**
 * destructor.
 */
CommandVolumeMakeSphere::~CommandVolumeMakeSphere()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMakeSphere::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("center-i");
   paramsOut.addInt("center-j");
   paramsOut.addInt("center-k");
   paramsOut.addFloat("Radius");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMakeSphere::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<center-i>\n"
       + indent9 + "<center-j>\n"
       + indent9 + "<center-k>\n"
       + indent9 + "<radius>\n"
       + indent9 + "\n"
       + indent9 + "Make a sphere within the volume.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeMakeSphere::executeCommand() throw (BrainModelAlgorithmException,
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
   const int center[3] = {
      parameters->getNextParameterAsInt("center-i"),
      parameters->getNextParameterAsInt("center-j"),
      parameters->getNextParameterAsInt("center-k")
   };
   const float radius =
      parameters->getNextParameterAsInt("Radius");
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // make plane
   //
   volume.makeSphere(center, radius);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

