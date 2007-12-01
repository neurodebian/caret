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

#include "CommandVolumeFillSlice.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeFillSlice::CommandVolumeFillSlice()
   : CommandBase("-volume-fill-slice",
                 "VOLUME FILL SLICE")
{
}

/**
 * destructor.
 */
CommandVolumeFillSlice::~CommandVolumeFillSlice()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeFillSlice::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> axisValues, axisNames;
   axisValues.push_back("X");   axisNames.push_back("Parasagittal View");
   axisValues.push_back("Y");   axisNames.push_back("Coronal View");
   axisValues.push_back("Z");   axisNames.push_back("Horizontal (axial) View");

   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addListOfItems("Axis", axisValues, axisNames);
   paramsOut.addInt("Seed X", 1, 0, 100000);
   paramsOut.addInt("Seed Y", 1, 0, 100000);
   paramsOut.addInt("Seed Z", 1, 0, 100000);
}

/**
 * get full help information.
 */
QString 
CommandVolumeFillSlice::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file>\n"
       + indent9 + "<output-volume-file>\n"
       + indent9 + "<AXIS>\n"
       + indent9 + "<seed-X>\n"
       + indent9 + "<seed-Y>\n"
       + indent9 + "<seed-Z>\n"
       + indent9 + "\n"
       + indent9 + "Flood fill a single slice.\n"
       + indent9 + "\n"
       + indent9 + "      axis is one of:  \n"
       + indent9 + "         X    => x-axis (parasagittal view)\n"
       + indent9 + "         Y    => y-axis (coronal view)\n"
       + indent9 + "         Z    => z-axis (horizontal/axial view)\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeFillSlice::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name",
                                                        outputVolumeFileName,
                                                        outputVolumeFileLabel);
   VolumeFile::VOLUME_AXIS axis =
      VolumeFile::getAxisFromString(parameters->getNextParameterAsString("AXIS"));
   const int seedX =
      parameters->getNextParameterAsInt("Seed X");
   const int seedY =
      parameters->getNextParameterAsInt("Seed Y");
   const int seedZ =
      parameters->getNextParameterAsInt("Seed Z");

   //
   // Read the input volume file
   //
   VolumeFile vf;
   vf.readFile(inputVolumeFileName);
   
   //
   // Find the biggest object and remove any islands.
   //
   const int seed[3] = {
      seedX, seedY, seedZ
   };
   vf.floodFillSliceWithVTK(axis, seed, 255, 255, 0);
   
   //
   // Write the volume file
   //
   writeVolumeFile(vf, outputVolumeFileName, outputVolumeFileLabel);
}

      

