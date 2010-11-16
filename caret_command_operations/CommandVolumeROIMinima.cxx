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
#include <fstream>

#include "CommandVolumeROIMinima.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelVolumeROIMinima.h"
#include "BrainSet.h"
#include "StringUtilities.h"
#include "FileUtilities.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeROIMinima::CommandVolumeROIMinima()
   : CommandBase("-volume-roi-minima",
                 "VOLUME REGION OF INTEREST MINIMA")
{
}

/**
 * destructor.
 */
CommandVolumeROIMinima::~CommandVolumeROIMinima()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeROIMinima::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File", FileFilters::getVolumeFunctionalFileFilter());
   paramsOut.addFile("Input Volume ROI File", FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addFile("Output Volume File", FileFilters::getVolumeFunctionalFileFilter());
   paramsOut.addFloat("Distance", 1.0, 0.0);
}

/**
 * get full help information.
 */
QString 
CommandVolumeROIMinima::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<values-volume>\n"
       + indent9 + "<region-volume>\n"
       + indent9 + "<output-volume>\n"
       + indent9 + "<distance>\n"
       + indent9 + "\n"
       + indent9 + "Find the minima of part of a volume file.  Does not use information\n"
       + indent9 + "from voxels that are 0 or negative in region-volume.\n"
       + indent9 + "\n"
       + indent9 + "      values-volume      the input file\n"
       + indent9 + "\n"
       + indent9 + "      region-volume      the region file\n"
       + indent9 + "\n"
       + indent9 + "      output-volume      the output file\n"
       + indent9 + "\n"
       + indent9 + "      distance           distance cutoff for range to search for\n"
       + indent9 + "                       smaller values\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeROIMinima::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString values =
      parameters->getNextParameterAsString("Input Volume File");
   const QString region =
      parameters->getNextParameterAsString("Input Region File");
   const QString output =
      parameters->getNextParameterAsString("Output Volume File");
   const float dist =
      parameters->getNextParameterAsFloat("Distance");
   BrainSet mybs;
   VolumeFile valueFile, regionFile, outFile;
   valueFile.readFile(values);
   regionFile.readFile(region);
   BrainModelVolumeROIMinima mymin(&mybs, &valueFile, &regionFile, &outFile, dist);
   mymin.execute();
   outFile.writeFile(output);
}
