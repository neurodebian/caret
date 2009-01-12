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

#include "CommandVolumeTFCE.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelVolumeTFCE.h"
#include "BrainSet.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
CommandVolumeTFCE::CommandVolumeTFCE()
   : CommandBase("-volume-TFCE",
                 "VOLUME THRESHHOLD FREE CLUSTER ENHANCEMENT")
{
}

/**
 * destructor.
 */
CommandVolumeTFCE::~CommandVolumeTFCE()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeTFCE::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input functional volume file name", FileFilters::getVolumeFunctionalFileFilter());
   paramsOut.addFile("Output funtional volume file name", FileFilters::getVolumeFunctionalFileFilter());
   paramsOut.addString("Output volume label");
   paramsOut.addInt("Number of steps to approximate integral", BrainModelVolumeTFCE::defaultNumSteps(), 1);
   paramsOut.addFloat("E (power to raise cluster mass to)", BrainModelVolumeTFCE::defaultE());
   paramsOut.addFloat("H (power to raise threshhold to)", BrainModelVolumeTFCE::defaultH());
}

/**
 * get full help information.
 */
QString 
CommandVolumeTFCE::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-anatomy-volume-file-name>\n"
       + indent9 + "<output-segmentation-volume-file-name>\n"
       + indent9 + "<output-volume-label>\n"
       + indent9 + "[number-of-steps = " + StringUtilities::fromNumber(BrainModelVolumeTFCE::defaultNumSteps()) + "]\n"
       + indent9 + "[E = " + StringUtilities::fromNumber(BrainModelVolumeTFCE::defaultE()) + "]\n"
       + indent9 + "[H = " + StringUtilities::fromNumber(BrainModelVolumeTFCE::defaultH()) + "]\n"
       + indent9 + "\n"
       + indent9 + "Enhance clusterlike signal using Threshhold Free Cluster Enhancement\n"
       + indent9 + "\n"
       + indent9 + "   Optional parameters: (default value specified above)\n"
       + indent9 + "      number-of-steps  number of pieces used to approximate the integral\n"
       + indent9 + "                        using the value at the center of a piece as the\n"
       + indent9 + "                        height of the piece.\n"
       + indent9 + "\n"
       + indent9 + "      E  the power to raise the cluster mass (number of voxels) to in the\n"
       + indent9 + "          integral.\n"
       + indent9 + "\n"
       + indent9 + "      H  the power to raise the threshhold to in the integral.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeTFCE::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Functional Volume File Name");
   const QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Functional Volume File Name");
   const QString outputVolumeLabel =
      parameters->getNextParameterAsString("Output Functional Volume Label");
   int numSteps = BrainModelVolumeTFCE::defaultNumSteps();
   float E = BrainModelVolumeTFCE::defaultE(),
         H = BrainModelVolumeTFCE::defaultH();
   if (parameters->getParametersAvailable())
   {
      numSteps =
         parameters->getNextParameterAsInt("Number Of Steps (optional)");
   }
   if (parameters->getParametersAvailable())
   {
      E =
         parameters->getNextParameterAsFloat("E (optional)");
   }
   if (parameters->getParametersAvailable())
   {
      H =
         parameters->getNextParameterAsFloat("H (optional)");
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
   VolumeFile outVolume(inputVolume);

   //
   // Create the Ligase object
   //
   BrainModelVolumeTFCE TFCEObject(&brainSet,
                                                   &inputVolume,
                                                   &outVolume,
                                                   outputVolumeFileName,
                                                   outputVolumeLabel,
                                                   numSteps,
                                                   E,
                                                   H);
   
   //
   // Execute Ligase
   //
   TFCEObject.execute();
   
   //
   // Write the file
   //
   outVolume.writeFile(outputVolumeFileName);
   
   const QString warningMessages = TFCEObject.getWarningMessages();
   if (warningMessages.isEmpty() == false) {
      std::cout << "TFCE Warnings: " << warningMessages.toAscii().constData() << std::endl;
   }
}

      

