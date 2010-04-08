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

#include "CommandVolumeReplaceVoxelsWithVectorMagnitude.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SureFitVectorFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeReplaceVoxelsWithVectorMagnitude::CommandVolumeReplaceVoxelsWithVectorMagnitude()
   : CommandBase("-volume-copy-vector-mag",
                 "VOLUME COPY VECTOR FILE MAGNITUDE INTO VOXELS")
{
}

/**
 * destructor.
 */
CommandVolumeReplaceVoxelsWithVectorMagnitude::~CommandVolumeReplaceVoxelsWithVectorMagnitude()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeReplaceVoxelsWithVectorMagnitude::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Vector File Name", FileFilters::getVolumeVectorFileFilter());
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Ooutput Volume File Name", FileFilters::getVolumeGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeReplaceVoxelsWithVectorMagnitude::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<vector-file>\n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Replace the volume's voxels with the vector file's magnitude.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeReplaceVoxelsWithVectorMagnitude::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString vectorFileName =
      parameters->getNextParameterAsString("Vector File Name");
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File Name");
   QString outputVolumeFileLabel;
   splitOutputVolumeNameIntoNameAndLabel(outputVolumeFileName, 
                                         outputVolumeFileLabel);
                                            
   VolumeFile volumeFile;
   volumeFile.readFile(inputVolumeFileName);
 
 
   SureFitVectorFile vectorFile;
   vectorFile.readFile(vectorFileName);
      
   vectorFile.copyMagnitudeToVolume(&volumeFile);
   
   writeVolumeFile(volumeFile, 
                   outputVolumeFileName, 
                   outputVolumeFileLabel);
}

      

