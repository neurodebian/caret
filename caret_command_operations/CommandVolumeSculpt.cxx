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

#include "CommandVolumeSculpt.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeSculpt::CommandVolumeSculpt()
   : CommandBase("-volume-sculpt",
                 "VOLUME SCULPT")
{
}

/**
 * destructor.
 */
CommandVolumeSculpt::~CommandVolumeSculpt()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSculpt::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("AND");  descriptions.push_back("AND");
   values.push_back("SEED-AND");  descriptions.push_back("SEED-AND");
   values.push_back("AND-NOT");  descriptions.push_back("AND-NOT");
   values.push_back("SEED-AND-NOT");  descriptions.push_back("SEED-AND-NOT");
   
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Other Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("i-minimum");
   paramsOut.addInt("i-maximum");
   paramsOut.addInt("j-minimum");
   paramsOut.addInt("j-maximum");
   paramsOut.addInt("k-minimum");
   paramsOut.addInt("k-maximum");
   paramsOut.addInt("i-seed");
   paramsOut.addInt("j-seed");
   paramsOut.addInt("k-seed");
   paramsOut.addListOfItems("Mode", values, descriptions);
   paramsOut.addInt("Number Of Steps");
}

/**
 * get full help information.
 */
QString 
CommandVolumeSculpt::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<input-other-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<i-minimum>\n"
       + indent9 + "<i-maximum>\n"
       + indent9 + "<j-minimum>\n"
       + indent9 + "<j-maximum>\n"
       + indent9 + "<k-minimum>\n"
       + indent9 + "<k-maximum>\n"
       + indent9 + "<i-seed>\n"
       + indent9 + "<j-seed>\n"
       + indent9 + "<k-seed>\n"
       + indent9 + "<mode>\n"
       + indent9 + "<num-steps>\n"
       + indent9 + "\n"
       + indent9 + "Sculpt a volume.\n"
       + indent9 + "\n"
       + indent9 + "\"mode\" is one of:\n"
       + indent9 + "   AND\n"
       + indent9 + "   SEED-AND\n"
       + indent9 + "   AND-NOT\n"
       + indent9 + "   SEED-AND-NOT\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSculpt::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   const QString otherVolumeFileName =
      parameters->getNextParameterAsString("Other Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name/Label",
                                                        outputVolumeFileName, 
                                                        outputVolumeFileLabel);
   int extent[6] = {
      parameters->getNextParameterAsInt("i-minimum"),
      parameters->getNextParameterAsInt("i-maximum"),
      parameters->getNextParameterAsInt("j-minimum"),
      parameters->getNextParameterAsInt("j-maximum"),
      parameters->getNextParameterAsInt("k-minimum"),
      parameters->getNextParameterAsInt("k-maximum")
   };
   int seed[3] = {
      parameters->getNextParameterAsInt("i-seed"),
      parameters->getNextParameterAsInt("j-seed"),
      parameters->getNextParameterAsInt("k-seed")
   };
   const QString modeString =
      parameters->getNextParameterAsString("Mode");
   const int numberOfSteps =
      parameters->getNextParameterAsInt("Number of Steps");
   checkForExcessiveParameters();

   VolumeFile::SCULPT_MODE mode;
   if (modeString == "AND") {
      mode = VolumeFile::SCULPT_MODE_AND;
   }
   else if (modeString == "SEED-AND") {
      mode = VolumeFile::SCULPT_MODE_SEED_AND;
   }
   else if (modeString == "AND-NOT") {
      mode = VolumeFile::SCULPT_MODE_AND_NOT;
   }
   else if (modeString == "SEED-AND-NOT") {
      mode = VolumeFile::SCULPT_MODE_SEED_AND_NOT;
   }
   else {
      throw CommandException("Invalid sculpt mode: " + modeString);
   }

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // Read the other volume
   //
   VolumeFile otherVolume;
   otherVolume.readFile(otherVolumeFileName);
   
   //
   // sculpt
   //
   volume.sculptVolume(mode, &otherVolume, numberOfSteps, seed, extent);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

