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

#include "CommandVolumeClassifyIntensities.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeClassifyIntensities::CommandVolumeClassifyIntensities()
   : CommandBase("-volume-classify-intensity",
                 "VOLUME CLASSIFY INTENSITIES")
{
}

/**
 * destructor.
 */
CommandVolumeClassifyIntensities::~CommandVolumeClassifyIntensities()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeClassifyIntensities::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFloat("Mean", 128);
   paramsOut.addFloat("Low", 0);
   paramsOut.addFloat("High", 255);
   paramsOut.addFloat("Signum", 1);
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeAnatomyFileFilter());
   paramsOut.addFile("Ooutput Volume File Name", FileFilters::getVolumeAnatomyFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeClassifyIntensities::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<mean>\n"
       + indent9 + "<low>\n"
       + indent9 + "<high>\n"
       + indent9 + "<signum>\n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Classify intensities.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeClassifyIntensities::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const float mean =
      parameters->getNextParameterAsFloat("Mean");
   const float low =
      parameters->getNextParameterAsFloat("Low");
   const float high =
      parameters->getNextParameterAsFloat("High");
   const float signum =
      parameters->getNextParameterAsFloat("Signum");
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File Name");
   QString outputVolumeFileLabel;
   splitOutputVolumeNameIntoNameAndLabel(outputVolumeFileName, 
                                         outputVolumeFileLabel);
                                            
   VolumeFile volumeFile;
   volumeFile.readFile(inputVolumeFileName);
 
   //
   // classify
   //
   volumeFile.classifyIntensities(mean, low, high, signum);
   
   writeVolumeFile(volumeFile, 
                   outputVolumeFileName, 
                   outputVolumeFileLabel);
}

      

