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

#include "BrainModelVolumeNearToPlane.h"
#include "BrainSet.h"
#include "CommandVolumeNearToPlane.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SureFitVectorFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeNearToPlane::CommandVolumeNearToPlane()
   : CommandBase("-volume-near-to-plane",
                 "VOLUME NEAR TO PLANE")
{
}

/**
 * destructor.
 */
CommandVolumeNearToPlane::~CommandVolumeNearToPlane()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeNearToPlane::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Mask Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Input Vector File Name", FileFilters::getSureFitVectorFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFloat("Sigma-N");
   paramsOut.addFloat("Sigma-W");
   paramsOut.addFloat("Offset");
   paramsOut.addInt("Grad Sign");
   paramsOut.addBoolean("Down Flag");
   paramsOut.addBoolean("Mask Flag");
}

/**
 * get full help information.
 */
QString 
CommandVolumeNearToPlane::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-mask-volume-file-name>\n"
       + indent9 + "<input-vector-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<Sigma-N>\n"
       + indent9 + "<Sigma-W>\n"
       + indent9 + "<Offset>\n"
       + indent9 + "<Grad-Sign>\n"
       + indent9 + "<Down-Flag>\n"
       + indent9 + "<Mask-Flag>\n"
       + indent9 + "\n"
       + indent9 + "Near to plane operation.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeNearToPlane::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputMaskVolumeFileName =
      parameters->getNextParameterAsString("Input Mask Volume File Name");
   const QString inputVectorFileName =
      parameters->getNextParameterAsString("Input Vector File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name/Label",
                                                        outputVolumeFileName, 
                                                        outputVolumeFileLabel);
   const float sigmaN =
      parameters->getNextParameterAsFloat("Sigma N");
   const float sigmaW =
      parameters->getNextParameterAsFloat("Sigma W");
   const float offset =
      parameters->getNextParameterAsFloat("Offset");
   const int gradSign =
      parameters->getNextParameterAsInt("Grad Sign");
   const bool downFlag =
      parameters->getNextParameterAsFloat("Down Flag");
   const bool maskFlag =
      parameters->getNextParameterAsFloat("Mask Flag");
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile maskVolume;
   maskVolume.readFile(inputMaskVolumeFileName);
   
   //
   // Read the vector file
   //
   SureFitVectorFile vectorFile;
   vectorFile.readFile(inputVectorFileName);
   
   VolumeFile outputVolume = maskVolume;
   
   //
   // perform near to plane
   //
   BrainSet bs;
   BrainModelVolumeNearToPlane np(&bs,
                                 &vectorFile,
                                 sigmaN,
                                 sigmaW,
                                 offset,
                                 downFlag,
                                 gradSign,
                                 maskFlag,
                                 &maskVolume,
                                 &outputVolume);
   np.execute();
   
   //
   // Write the volume
   //
   writeVolumeFile(outputVolume, outputVolumeFileName, outputVolumeFileLabel);
}

      

