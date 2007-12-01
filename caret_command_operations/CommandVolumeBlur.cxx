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

#include "CommandVolumeBlur.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeBlur::CommandVolumeBlur()
   : CommandBase("-volume-blur",
                 "VOLUME BLUR")
{
}

/**
 * destructor.
 */
CommandVolumeBlur::~CommandVolumeBlur()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeBlur::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File", FileFilters::getVolumeAnatomyFileFilter());
   paramsOut.addFile("Output Volume File", FileFilters::getVolumeAnatomyFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeBlur::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Blur the volume.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeBlur::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File Name");
   QString outputVolumeFileLabel;
   splitOutputVolumeNameIntoNameAndLabel(outputVolumeFileName, outputVolumeFileLabel);
   checkForExcessiveParameters();
   
   VolumeFile volumeFile;
   volumeFile.readFile(inputVolumeFileName);
   
   volumeFile.blur();
                                 
   writeVolumeFile(volumeFile, 
                   outputVolumeFileName,
                   outputVolumeFileLabel);
}

      

