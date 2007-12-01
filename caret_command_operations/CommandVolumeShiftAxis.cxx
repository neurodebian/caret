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

#include "CommandVolumeShiftAxis.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeShiftAxis::CommandVolumeShiftAxis()
   : CommandBase("-volume-shift-axis",
                 "VOLUME SHIFT AXIS")
{
}

/**
 * destructor.
 */
CommandVolumeShiftAxis::~CommandVolumeShiftAxis()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeShiftAxis::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> axisValues, axisNames;
   axisValues.push_back("X");   axisNames.push_back("Parasagittal (X)");
   axisValues.push_back("Y");   axisNames.push_back("Coronal (Y)");
   axisValues.push_back("Z");   axisNames.push_back("Horizontal (Z)");

   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addListOfItems("Axis", axisValues, axisNames);
   paramsOut.addInt("Offset");
}

/**
 * get full help information.
 */
QString 
CommandVolumeShiftAxis::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<axis>\n"
       + indent9 + "<offset>\n"
       + indent9 + "\n"
       + indent9 + "Shift along an axis.\n"
       + indent9 + "\n"
       + indent9 + "\"axis\" is one of \"X\", \"Y\", or \"Z\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeShiftAxis::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString axisString = parameters->getNextParameterAsString("Axis");
   const VolumeFile::VOLUME_AXIS axis = VolumeFile::getAxisFromString(axisString);
   const int offset = parameters->getNextParameterAsInt("Offset");
   checkForExcessiveParameters();
   
   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // shift
   //
   volume.shiftAxis(axis, offset);
      
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

