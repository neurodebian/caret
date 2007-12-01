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

#include "CommandVolumeSetOrientation.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeSetOrientation::CommandVolumeSetOrientation()
   : CommandBase("-volume-set-orientation",
                 "VOLUME SET ORIENTATION")
{
}

/**
 * destructor.
 */
CommandVolumeSetOrientation::~CommandVolumeSetOrientation()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSetOrientation::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("ANTERIOR");   descriptions.push_back("Anterior-to-Posterior");
   values.push_back("INFERIOR");   descriptions.push_back("Interior-to-Superior");
   values.push_back("LEFT");   descriptions.push_back("Left-to-Right");
   values.push_back("POSTERIOR");   descriptions.push_back("Posterior-to-Inferior");
   values.push_back("RIGHT");   descriptions.push_back("Right-to-Left");
   values.push_back("SUPERIOR");   descriptions.push_back("Superior-to-Inferior");
   values.push_back("UNKNOWN");   descriptions.push_back("Unknown");
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addListOfItems("X-Axis-Orientation", values, descriptions);
   paramsOut.addListOfItems("Y-Axis-Orientation", values, descriptions);
   paramsOut.addListOfItems("Z-Axis-Orientation", values, descriptions);
}

/**
 * get full help information.
 */
QString 
CommandVolumeSetOrientation::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<x-axis-orientation>\n"
       + indent9 + "<y-axis-orientation>\n"
       + indent9 + "<z-axis-orientation>\n"
       + indent9 + "\n"
       + indent9 + "Set the orientation for the volume file\n"
       + indent9 + "\n"
       + indent9 + "\"axis-orientation\" values and descriptions:\n"
       + indent9 + "   Value-Entered   Description\n"
       + indent9 + "   -------------   -----------\n"
       + indent9 + "   ANTERIOR        Anterior-to-Posterior\n"
       + indent9 + "   INFERIOR        Interior-to-Superior\n"
       + indent9 + "   LEFT            Left-to-Right\n"
       + indent9 + "   POSTERIOR       Posterior-to-Inferior\n"
       + indent9 + "   RIGHT           Right-to-Left\n"
       + indent9 + "   SUPERIOR        Superior-to-Inferior\n"
       + indent9 + "   UNKNOWN         Unknown\n"
       + indent9 + "   \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSetOrientation::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString orientString[3] = {
      parameters->getNextParameterAsString("X-Axis-Orientation"),
      parameters->getNextParameterAsString("Y-Axis-Orientation"),
      parameters->getNextParameterAsString("Z-Axis-Orientation")
   };
   checkForExcessiveParameters();
   
   //
   // Convert orientation strings
   //
   VolumeFile::ORIENTATION orientation[3];
   for (int i = 0; i < 3; i++) {
      if (orientString[i] == "UNKNOWN") {
         orientation[i] = VolumeFile::ORIENTATION_UNKNOWN;
      }
      else if (orientString[i] == "RIGHT") {
         orientation[i] = VolumeFile::ORIENTATION_RIGHT_TO_LEFT;
      }
      else if (orientString[i] == "LEFT") {
         orientation[i] = VolumeFile::ORIENTATION_LEFT_TO_RIGHT;
      }
      else if (orientString[i] == "POSTERIOR") {
         orientation[i] = VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR;
      }
      else if (orientString[i] == "ANTERIOR") {
         orientation[i] = VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR;
      }
      else if (orientString[i] == "INFERIOR") {
         orientation[i] = VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR;
      }
      else if (orientString[i] == "SUPERIOR") {
         orientation[i] = VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR;
      }
      else {
         throw CommandException("Invalid orientation axis \""
                                + orientString[i]
                                + "\"");
      }
   }
   
   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // set the orientation
   //
   volume.setOrientation(orientation);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

