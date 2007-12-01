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

#include "CommandVolumeReplaceVectorMagnitudeWithVolume.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VectorFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeReplaceVectorMagnitudeWithVolume::CommandVolumeReplaceVectorMagnitudeWithVolume()
   : CommandBase("-volume-vector-replace-mag-volume",
                 "VOLUME VECTOR MAGNITUDE - REPLACE WITH VOLUME")
{
}

/**
 * destructor.
 */
CommandVolumeReplaceVectorMagnitudeWithVolume::~CommandVolumeReplaceVectorMagnitudeWithVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeReplaceVectorMagnitudeWithVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("REPLACE");   descriptions.push_back("REPLACE");
   values.push_back("MULTIPLY");   descriptions.push_back("MULTIPLY");
   paramsOut.clear();
   paramsOut.addFile("Input Vector File Name", FileFilters::getVectorFileFilter());
   paramsOut.addFile("Output Vector File Name", FileFilters::getVectorFileFilter());
   paramsOut.addFile("Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addListOfItems("Operation", values, descriptions);
}

/**
 * get full help information.
 */
QString 
CommandVolumeReplaceVectorMagnitudeWithVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-vector-file-name>\n"
       + indent9 + "<output-vector-file-name>\n"
       + indent9 + "<volume-file-name>\n"
       + indent9 + "<operation>\n"
       + indent9 + "\n"
       + indent9 + "Replace vector file's magnitude.\n"
       + indent9 + "\n"
       + indent9 + "   \"operation\" is one of:\n"
       + indent9 + "      REPLACE  - replace magnitude with volume's voxel\n"
       + indent9 + "      MULTIPLY - multiply magnitude with volume's voxel\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeReplaceVectorMagnitudeWithVolume::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVectorFileName =
      parameters->getNextParameterAsString("Input Vector File Name");
   const QString outputVectorFileName =
      parameters->getNextParameterAsString("Output Vector File Name");
   const QString volumeFileName =
      parameters->getNextParameterAsString("Volume File Name");
   const QString operationString =
      parameters->getNextParameterAsString("Operation").toUpper();
   checkForExcessiveParameters();

   VectorFile::COMBINE_VOLUME_OPERATION operation;
   if (operationString == "REPLACE") {
      operation = VectorFile::COMBINE_VOLUME_REPLACE_MAGNITUDE_WITH_VOLUME;
   }
   else if (operationString == "MULTIPLY") {
      operation = VectorFile::COMBINE_VOLUME_MULTIPLY_MAGNITUDE_WITH_VOLUME;
   }
   else {
      throw CommandException("Invalid operation \"" + operationString + "\"");
   }
   
   //
   // Read the input files
   //
   VectorFile vector;
   vector.readFile(inputVectorFileName);
   VolumeFile volume;
   volume.readFile(volumeFileName);
   
   //
   // replace magnitude with volume's voxels
   //
   vector.combineWithVolumeOperation(operation,
                                     &volume);
   
   //
   // Write the vector file
   //
   vector.writeFile(outputVectorFileName);
}

      

