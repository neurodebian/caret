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

#include "CommandVolumeAnatomyPeaks.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StatisticHistogram.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeAnatomyPeaks::CommandVolumeAnatomyPeaks()
   : CommandBase("-volume-anatomy-peaks",
                 "VOLUME ANATOMY PEAKS")
{
}

/**
 * destructor.
 */
CommandVolumeAnatomyPeaks::~CommandVolumeAnatomyPeaks()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeAnatomyPeaks::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Anatomy Volume File", FileFilters::getVolumeAnatomyFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeAnatomyPeaks::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<anatomy-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Estimate the peaks in an anatomy volume file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeAnatomyPeaks::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString anatomyVolumeFileName =
      parameters->getNextParameterAsString("Anatomy Volume File Name");
      
   //
   // Read the anatomy volume file
   //
   VolumeFile volumeFile;
   volumeFile.readFile(anatomyVolumeFileName);

   //
   // Get the histogram and the estimated peaks
   //
   const StatisticHistogram* histo = volumeFile.getHistogram();
   histo->printHistogramPeaks(std::cout);

   delete histo;
   histo = NULL;
}

      

