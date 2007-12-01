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

#include "CommandVolumeConvertVectorToVolume.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VectorFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeConvertVectorToVolume::CommandVolumeConvertVectorToVolume()
   : CommandBase("-volume-convert-vector-to-volume",
                 "VOLUME CONVERT A VECTOR FILE TO A VOLUME FILE")
{
}

/**
 * destructor.
 */
CommandVolumeConvertVectorToVolume::~CommandVolumeConvertVectorToVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeConvertVectorToVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Vector File Name", FileFilters::getVolumeVectorFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeVectorFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeConvertVectorToVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-vector-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Convert a vector file into a vector type volume file.\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeConvertVectorToVolume::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString vectorFileName =
      parameters->getNextParameterAsString("Input Vector File Name");
   QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File Name");
   QString outputVolumeFileLabel;
   splitOutputVolumeNameIntoNameAndLabel(outputVolumeFileName, 
                                         outputVolumeFileLabel);
                                            
   VectorFile vectorFile;
   vectorFile.readFile(vectorFileName);

   VolumeFile volumeFile(vectorFile);
   
   writeVolumeFile(volumeFile, 
                   outputVolumeFileName, 
                   outputVolumeFileLabel);
}

      

