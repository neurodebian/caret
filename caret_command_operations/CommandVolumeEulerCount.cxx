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

#include "CommandVolumeEulerCount.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeEulerCount::CommandVolumeEulerCount()
   : CommandBase("-volume-euler",
                 "VOLUME EULER COUNT")
{
}

/**
 * destructor.
 */
CommandVolumeEulerCount::~CommandVolumeEulerCount()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeEulerCount::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("Min X", 1, 0, 100000);
   paramsOut.addInt("Max X", 1, 0, 100000);
   paramsOut.addInt("Min Y", 1, 0, 100000);
   paramsOut.addInt("Max Y", 1, 0, 100000);
   paramsOut.addInt("Min Z", 1, 0, 100000);
   paramsOut.addInt("Max Z", 1, 0, 100000);
}

/**
 * get full help information.
 */
QString 
CommandVolumeEulerCount::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<min-x> \n"
       + indent9 + "<max-x> \n"
       + indent9 + "<min-y> \n"
       + indent9 + "<max-y> \n"
       + indent9 + "<min-z> \n"
       + indent9 + "<max-z> \n"
       + indent9 + "\n"
       + indent9 + "Perform and print the Euler Counts for a segmentation volume\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeEulerCount::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   const int minX =
      parameters->getNextParameterAsInt("Minimum X");
   const int maxX =
      parameters->getNextParameterAsInt("Maximum X");
   const int minY =
      parameters->getNextParameterAsInt("Minimum Y");
   const int maxY =
      parameters->getNextParameterAsInt("Maximum Y");
   const int minZ =
      parameters->getNextParameterAsInt("Minimum Z");
   const int maxZ =
      parameters->getNextParameterAsInt("Maximum Z");
   checkForExcessiveParameters();
   
   //
   // Read the input volume file
   //
   VolumeFile vf;
   vf.readFile(inputVolumeFileName);
   
   //
   // Do the euler count
   //
   const int extent[6] = {
      minX, maxX, minY, maxY, minZ, maxZ
   };
   int NumObjects, NumCavities, NumHandles, eulerCount;
   vf.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                 NumCavities,
                                                 NumHandles,
                                                 eulerCount,
                                                 extent);

   //
   // Print the counts
   //
   std::cout << "Num Objects  " << NumObjects << std::endl;
   std::cout << "Num Cavities " << NumCavities << std::endl;
   std::cout << "Num Handles  " << NumHandles << std::endl;
   std::cout << "Euler Count  " << eulerCount << std::endl;
}

      

