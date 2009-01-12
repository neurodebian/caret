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

#include "CommandVolumeTopologyReport.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeTopologyReport::CommandVolumeTopologyReport()
   : CommandBase("-volume-topology-report",
                 "VOLUME TOPOLOGY REPORT")
{
}

/**
 * destructor.
 */
CommandVolumeTopologyReport::~CommandVolumeTopologyReport()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeTopologyReport::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Segmentation Volume File",
                     FileFilters::getVolumeSegmentationFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeTopologyReport::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<segmentation-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "List topology information about the volume by measuring\n"
       + indent9 + "the topology on a surface generated from the input\n"
       + indent9 + "segmentation volume file.\n"
       + indent9 + "\n"
       + indent9 + "Note: any cavities are filled prior to determining the\n"
       + indent9 + "Euler Count, Objects, and Holes (handles).\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeTopologyReport::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputSegmentationVolumeFileName =
      parameters->getNextParameterAsString("Segmentation Volume File Name");
      
   VolumeFile segmentationVolumeFile;
   segmentationVolumeFile.readFile(inputSegmentationVolumeFileName);
   
   int numberOfObjects, numberOfCavities, numberOfHoles, eulerCount;
   segmentationVolumeFile.getSegmentationTopologyInformation(
      numberOfObjects, 
      numberOfCavities, 
      numberOfHoles, 
      eulerCount);
      
   std::cout << "Objects:         " << numberOfObjects << std::endl;
   std::cout << "Cavities:        " << numberOfCavities << std::endl; 
   std::cout << "Holes (handles): " << numberOfHoles << std::endl;
   std::cout << "Euler Count:     " << eulerCount << std::endl;
}

      

