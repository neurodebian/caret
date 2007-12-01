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

#include "CommandVolumeSmearAxis.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeSmearAxis::CommandVolumeSmearAxis()
   : CommandBase("-volume-smear-axis",
                 "VOLUME SMEAR AXIS")
{
}

/**
 * destructor.
 */
CommandVolumeSmearAxis::~CommandVolumeSmearAxis()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSmearAxis::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> axisValues, axisNames;
   axisValues.push_back("X");   axisNames.push_back("Parasagittal View");
   axisValues.push_back("Y");   axisNames.push_back("Coronal View");
   axisValues.push_back("Z");   axisNames.push_back("Horizontal (axial) View");
   
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addListOfItems("Axis", axisValues, axisNames);
   paramsOut.addInt("Mag", 0, -10000.0, 10000.0);
   paramsOut.addInt("Sign", 0, -10000.0, 10000.0);
   paramsOut.addInt("Core", 0, -10000.0, 10000.0);
}
      
/**
 * get full help information.
 */
QString 
CommandVolumeSmearAxis::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<AXIS>\n"
       + indent9 + "<mag>\n"
       + indent9 + "<sign>\n"
       + indent9 + "<core>\n"
       + indent9 + "\n"
       + indent9 + "Smear a volume along an axis.\n"
       + indent9 + "\n"
       + indent9 + "\"axis\" is one of \"X\", \"Y\", or \"Z\".\n"
       + indent9 + "\n"  //EXPLAIN HOW IT WORKS/WHAT IT DOES
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSmearAxis::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name",
                                                        outputVolumeFileName,
                                                        outputVolumeFileLabel);
   const QString axisString = parameters->getNextParameterAsString("Axis");
   const VolumeFile::VOLUME_AXIS axis = VolumeFile::getAxisFromString(axisString);
   const int mag =  
      parameters->getNextParameterAsInt("Mag");
   const int sign =  
      parameters->getNextParameterAsInt("Sign");
   const int core =  
      parameters->getNextParameterAsInt("Core");
      
   VolumeFile vf;
   vf.readFile(inputVolumeFileName);
   
   vf.smearAxis(axis,
                mag,
                sign,
                core);
            
   vf.setDescriptiveLabel(outputVolumeFileLabel);
   vf.writeFile(outputVolumeFileName);
}


