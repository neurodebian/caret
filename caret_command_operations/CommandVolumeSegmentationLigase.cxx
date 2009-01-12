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

#include <iostream>

#include "CommandVolumeSegmentationLigase.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelVolumeLigaseSegmentation.h"
#include "BrainSet.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
CommandVolumeSegmentationLigase::CommandVolumeSegmentationLigase()
   : CommandBase("-volume-segment-ligase",
                 "VOLUME SEGMENTATION LIGASE")
{
}

/**
 * destructor.
 */
CommandVolumeSegmentationLigase::~CommandVolumeSegmentationLigase()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSegmentationLigase::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input anatomy volume file name", FileFilters::getVolumeAnatomyFileFilter());
   paramsOut.addFile("Output segmentation volume file name", FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addString("Output volume label");
   paramsOut.addInt("X index of seed", 128, 0);
   paramsOut.addInt("Y index of seed", 128, 0);
   paramsOut.addInt("Z index of seed", 128, 0);
   paramsOut.addFloat("White minimum", 140.0f);
   paramsOut.addFloat("White peak", 170.0f);
   paramsOut.addFloat("White maximum", 200.0f);
   paramsOut.addFloat("Difference cutoff base value", BrainModelVolumeLigaseSegmentation::defaultDiff());
   paramsOut.addFloat("Gradient cutoff base value", BrainModelVolumeLigaseSegmentation::defaultGrad());
   paramsOut.addFloat("Above peak probability minimum", BrainModelVolumeLigaseSegmentation::defaultHighBias());
   paramsOut.addFloat("Below peak probability minimum", BrainModelVolumeLigaseSegmentation::defaultLowBias());
}

/**
 * get full help information.
 */
QString 
CommandVolumeSegmentationLigase::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-anatomy-volume-file-name>\n"
       + indent9 + "<output-segmentation-volume-file-name>\n"
       + indent9 + "<output-volume-label>\n"
       + indent9 + "<x-seed>\n"
       + indent9 + "<y-seed>\n"
       + indent9 + "<z-seed>\n"
       + indent9 + "<white-min>\n"
       + indent9 + "<white-peak>\n"
       + indent9 + "<white-max>\n"
       + indent9 + "[diff-base = " + StringUtilities::fromNumber(BrainModelVolumeLigaseSegmentation::defaultDiff()) + "]\n"
       + indent9 + "[grad-base = " + StringUtilities::fromNumber(BrainModelVolumeLigaseSegmentation::defaultGrad()) + "]\n"
       + indent9 + "[high-bias = " + StringUtilities::fromNumber(BrainModelVolumeLigaseSegmentation::defaultHighBias()) + "]\n"
       + indent9 + "[low-bias = " + StringUtilities::fromNumber(BrainModelVolumeLigaseSegmentation::defaultLowBias()) + "]\n"
       + indent9 + "\n"
       + indent9 + "Use LIGASE to segment the white matter.\n"
       + indent9 + "\n"
       + indent9 + "      x-seed, y-seed, z-seed  specify the voxel for LIGASE to grow\n"
       + indent9 + "                               from, make sure it is in the white\n"
       + indent9 + "                               matter.\n"
       + indent9 + "\n"
       + indent9 + "      white-min  specifies the minimum intensity of the white matter in\n"
       + indent9 + "                  the anatomy volume. \n"
       + indent9 + " \n"
       + indent9 + "      white-peak  specifies the intensity of the white matter peak in\n"
       + indent9 + "                  the anatomy volume. \n"
       + indent9 + " \n"
       + indent9 + "      white-max  specifies the maximum intensity of the white matter in\n"
       + indent9 + "                  the anatomy volume. \n"
       + indent9 + "\n"
       + indent9 + "   Optional parameters: (default value specified above)\n"
       + indent9 + "      diff-base  specifies how much to rely on the difference from one\n"
       + indent9 + "                  voxel to the next when growing, higher numbers allow\n"
       + indent9 + "                  more growth. \n"
       + indent9 + "\n"
       + indent9 + "      grad-base  specifies how much to rely on the magnitude of the\n"
       + indent9 + "                  local 3D gradient when growing, higher numbers allow\n"
       + indent9 + "                  more growth.\n"
       + indent9 + "\n"
       + indent9 + "      high-bias  specifies how low a probability based on intensity\n"
       + indent9 + "                  is considered as no chance of being white matter\n"
       + indent9 + "                  when intensity is above peak, smaller numbers allow\n"
       + indent9 + "                  more growth.\n"
       + indent9 + "\n"
       + indent9 + "      low-bias  specifies how low a probability based on intensity\n"
       + indent9 + "                  is considered as no chance of being white matter\n"
       + indent9 + "                  when intensity is below peak, smaller numbers allow\n"
       + indent9 + "                  more growth.\n"
       + indent9 + "\n"
       + indent9 + "Note: This algorithm is generally conservative in estimating the\n"
       + indent9 + "       gray/white boundary, try dilating the output if changing\n"
       + indent9 + "       parameters doesn't give the desired results.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSegmentationLigase::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Anatomy Volume File Name");
   const QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Segmentation Volume File Name");
   const QString outputVolumeLabel =
      parameters->getNextParameterAsString("Output Segmentation Volume Label");
   const int xSeed =
      parameters->getNextParameterAsInt("Seed Point X Index");
   const int ySeed =
      parameters->getNextParameterAsInt("Seed Point Y Index");
   const int zSeed =
      parameters->getNextParameterAsInt("Seed Point Z Index");
   const float whiteMin =
      parameters->getNextParameterAsFloat("White Minimum");
   const float whitePeak =
      parameters->getNextParameterAsFloat("White Peak");
   const float whiteMax =
      parameters->getNextParameterAsFloat("White Maximum");
   float diffBase = BrainModelVolumeLigaseSegmentation::defaultDiff(),
         gradBase = BrainModelVolumeLigaseSegmentation::defaultGrad(),
         highBias = BrainModelVolumeLigaseSegmentation::defaultHighBias(),
         lowBias = BrainModelVolumeLigaseSegmentation::defaultLowBias();
   if (parameters->getParametersAvailable())
   {
      diffBase =
         parameters->getNextParameterAsFloat("Difference Cutoff Base (optional)");
   }
   if (parameters->getParametersAvailable())
   {
      gradBase =
         parameters->getNextParameterAsFloat("Gradient Cutoff Base (optional)");
   }
   if (parameters->getParametersAvailable())
   {
      highBias =
         parameters->getNextParameterAsFloat("High Probability Bias (optional)");
   }
   if (parameters->getParametersAvailable())
   {
      lowBias =
         parameters->getNextParameterAsFloat("Low Probability Bias (optional)");
   }
   checkForExcessiveParameters();
   
   //
   // Create a brain set
   //
   BrainSet brainSet;
   
   //
   // Read the volume file
   //
   VolumeFile inputVolume;
   inputVolume.readFile(inputVolumeFileName);
   
   //
   // Create output volume file
   //
   VolumeFile segVolume(inputVolume);
   segVolume.setVolumeType(VolumeFile::VOLUME_TYPE_SEGMENTATION);

   //
   // Create the Ligase object
   //
   BrainModelVolumeLigaseSegmentation ligaseObject(&brainSet,
                                                   &inputVolume,
                                                   &segVolume,
                                                   outputVolumeFileName,
                                                   outputVolumeLabel,
                                                   xSeed,
                                                   ySeed,
                                                   zSeed,
                                                   whiteMin,
                                                   whitePeak,
                                                   whiteMax,
                                                   diffBase,
                                                   gradBase,
                                                   highBias,
                                                   lowBias);
   
   //
   // Execute Ligase
   //
   ligaseObject.execute();
   
   //
   // Write the file
   //
   segVolume.writeFile(outputVolumeFileName);
   
   const QString warningMessages = ligaseObject.getWarningMessages();
   if (warningMessages.isEmpty() == false) {
      std::cout << "Segmentation Warnings: " << warningMessages.toAscii().constData() << std::endl;
   }
}

      

