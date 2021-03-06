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

#include "CommandVolumeResample.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeResample::CommandVolumeResample()
   : CommandBase("-volume-resample",
                 "VOLUME RESAMPLE")
{
}

/**
 * destructor.
 */
CommandVolumeResample::~CommandVolumeResample()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeResample::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> methodValues, methodDescriptions;
   methodValues.push_back("INTERP_CUBIC");
      methodDescriptions.push_back("Cubic (best quality)");
   methodValues.push_back("INTERP_LINEAR");
      methodDescriptions.push_back("Linear");
   methodValues.push_back("INTERP_NEAREST_NEIGHBOR");
      methodDescriptions.push_back("Nearest Neighbors (use for paint and probabilistic volumes");

   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFloat("New Spacing X");
   paramsOut.addFloat("New Spacing Y");
   paramsOut.addFloat("New Spacing Z");
   paramsOut.addListOfItems("Interpolation Mode",
                            methodValues,
                            methodDescriptions);
}

/**
 * get full help information.
 */
QString 
CommandVolumeResample::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<new-spacing-x>\n"
       + indent9 + "<new-spacing-y>\n"
       + indent9 + "<new-spacing-z>\n"
       + indent9 + "<interpolation-mode>\n"
       + indent9 + "\n"
       + indent9 + "Resample a volume to the specified spacing.\n"
       + indent9 + "\n"
       + indent9 + "\"interpolation-mode\" is one of:\n"
       + indent9 + "   INTERP_CUBIC\n"
       + indent9 + "   INTERP_LINEAR\n"
       + indent9 + "   INTERP_NEAREST_NEIGHBOR\n"
       + indent9 + "\n"
       + indent9 + "For paint, probabilistic atlas, and segmentation volumes,\n"
       + indent9 + " you MUST use INTERP_NEAREST_NEIGHBOR for the interpolation.\n"
       + indent9 + "mode.  INTERP_CUBIC is recommended for all other volume file\n"
       + indent9 + "types.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeResample::executeCommand() throw (BrainModelAlgorithmException,
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
   const float spacing[3] = {
      parameters->getNextParameterAsFloat("New Spacing X"),
      parameters->getNextParameterAsFloat("New Spacing Y"),
      parameters->getNextParameterAsFloat("New Spacing Z")
   };   
   const QString interpolationName =
      parameters->getNextParameterAsString("Interpolation");
   checkForExcessiveParameters();

   VolumeFile::INTERPOLATION_TYPE interpolationType =
      VolumeFile::INTERPOLATION_TYPE_CUBIC;
   if (interpolationName == "INTERP_CUBIC") {
      interpolationType = VolumeFile::INTERPOLATION_TYPE_CUBIC;
   }
   else if (interpolationName == "INTERP_LINEAR") {
      interpolationType = VolumeFile::INTERPOLATION_TYPE_LINEAR;
   }
   else if (interpolationName == "INTERP_NEAREST_NEIGHBOR") {
      interpolationType = VolumeFile::INTERPOLATION_TYPE_NEAREST_NEIGHBOR;
   }
   else {
      throw CommandException("Invalid interpolation value \""
                             + interpolationName
                             + "\"");
   }
   
   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // set the spacing
   //
   volume.resampleToSpacing(spacing,
                            interpolationType);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

