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

#include "CommandVolumeBiasCorrection.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeBiasCorrection::CommandVolumeBiasCorrection()
   : CommandBase("-volume-bias-correction",
                 "VOLUME BIAS CORRECTION")
{
}

/**
 * destructor.
 */
CommandVolumeBiasCorrection::~CommandVolumeBiasCorrection()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeBiasCorrection::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addInt("Gray Minimum", 0);
   paramsOut.addInt("White Maximum", 255);
   paramsOut.addInt("Iterations", 5);
   paramsOut.addFile("Input Volume File", FileFilters::getVolumeAnatomyFileFilter());
   paramsOut.addFile("Output Volume File", FileFilters::getVolumeAnatomyFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeBiasCorrection::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<gray-minimum-value>\n"
       + indent9 + "<white-maximum-value>\n"
       + indent9 + "<iterations>\n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Perform bias (non-uniformity) correction on an anatomy\n"
       + indent9 + "volume.\n"
       + indent9 + "\n"
       + indent9 + "\"gray-minimum-value\" is an integer that is the minimum\n"
       + indent9 + "value of the gray matter voxels. \n"
       + indent9 + "\n"
       + indent9 + "\"white-maximum-value\" is an integer that is the maximum\n"
       + indent9 + "value of the white matter voxels. \n"
       + indent9 + "\n"
       + indent9 + "\"iterations\" is the number of iterations used in the bias\n"
       + indent9 + "correction algorithm.  A value of 5 is sufficient in most\n"
       + indent9 + "cases. \n"
       + indent9 + "\n"
       + indent9 + "This algorithm (and code) is taken directly from AFNI's  \n"
       + indent9 + "3dUniformize program. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeBiasCorrection::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const int grayMinimum = 
      parameters->getNextParameterAsInt("Gray Minimum Value");
   const int whiteMaximum = 
      parameters->getNextParameterAsInt("White Maximum Value");
   const int iterations = 
      parameters->getNextParameterAsInt("Iterations");
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File Name");
   QString outputVolumeFileLabel;
   splitOutputVolumeNameIntoNameAndLabel(outputVolumeFileName, outputVolumeFileLabel);
   checkForExcessiveParameters();
   
   VolumeFile volumeFile;
   volumeFile.readFile(inputVolumeFileName);
   
   volumeFile.biasCorrectionWithAFNI(grayMinimum,
                                     whiteMaximum,
                                     iterations);
                                 
   writeVolumeFile(volumeFile, 
                   outputVolumeFileName,
                   outputVolumeFileLabel);
}

      

