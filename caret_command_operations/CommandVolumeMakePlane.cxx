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

#include "CommandVolumeMakePlane.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeMakePlane::CommandVolumeMakePlane()
   : CommandBase("-volume-make-plane",
                 "VOLUME MAKE PLANE")
{
}

/**
 * destructor.
 */
CommandVolumeMakePlane::~CommandVolumeMakePlane()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMakePlane::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("X-slope");
   paramsOut.addInt("X-offset");
   paramsOut.addInt("Y-slope");
   paramsOut.addInt("Y-offset");
   paramsOut.addInt("Z-slope");
   paramsOut.addInt("Z-offset");
   paramsOut.addInt("Offset");
   paramsOut.addInt("Thickness");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMakePlane::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<X-slope>\n"
       + indent9 + "<X-offset>\n"
       + indent9 + "<Y-slope>\n"
       + indent9 + "<Y-offset>\n"
       + indent9 + "<Z-slope>\n"
       + indent9 + "<Z-offset>\n"
       + indent9 + "<offset>\n"
       + indent9 + "<thickness>\n"
       + indent9 + "\n"
       + indent9 + "Make a plane.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeMakePlane::executeCommand() throw (BrainModelAlgorithmException,
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
   const int xSlope =
      parameters->getNextParameterAsInt("X-Slope");
   const int xOffset =
      parameters->getNextParameterAsInt("X-Offset");
   const int ySlope =
      parameters->getNextParameterAsInt("Y-Slope");
   const int yOffset =
      parameters->getNextParameterAsInt("Y-Offset");
   const int zSlope =
      parameters->getNextParameterAsInt("Z-Slope");
   const int zOffset =
      parameters->getNextParameterAsInt("Z-Offset");
   const int offset =
      parameters->getNextParameterAsInt("Offset");
   const int thickness =
      parameters->getNextParameterAsInt("Thickness");
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // make plane
   //
   volume.makePlane(xSlope, xOffset, ySlope, yOffset, zSlope, zOffset, offset, thickness);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

