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

#include <QTime>

#include "BrainModelVolumeTopologyGraphCorrector.h"
#include "BrainSet.h"
#include "CommandVolumeTopologyCorrector.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeTopologyCorrector::CommandVolumeTopologyCorrector()
   : CommandBase("-volume-topology-corrector",
                 "VOLUME TOPOLOGY CORRECTOR (in development - do not use)")
{
}

/**
 * destructor.
 */
CommandVolumeTopologyCorrector::~CommandVolumeTopologyCorrector()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeTopologyCorrector::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> mode;
   mode.push_back("NORMAL");
   mode.push_back("MINIMAL");
   
   paramsOut.clear();
   paramsOut.addFile("Input Segmentation Volume File Name",
                     FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addFile("Output Corrected Segmentation Volume File Name",
                     FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addListOfItems("Mode", mode, mode);
   paramsOut.addFile("Paint Volume File Showing Corrections",
                     FileFilters::getVolumePaintFileFilter(),
                     "corrections.nii.gz",
                     "-correction-paint-volume");
}

/**
 * get full help information.
 */
QString 
CommandVolumeTopologyCorrector::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-segmentation-volume-file-name>\n"
       + indent9 + "<output-corrected-segmentation-volume-file-name>\n"
       + indent9 + "<correction-mode>\n"
       + indent9 + "[-correction-paint-volume <paint-volume-file-name>]\n"
       + indent9 + "\n"
       + indent9 + "Correct segmentation volume topology.\n"
       + indent9 + "\n"
       + indent9 + "correction-mode is one of:\n"
       + indent9 + "   MINIMAL    fix minimal number of voxels\n"
       + indent9 + "   NORMAL     normal correction\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeTopologyCorrector::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString segmentationVolumeFileName =
      parameters->getNextParameterAsString("Input Segmentation Volume File Name");
   const QString correctedSegmentationVolumeFileName =
      parameters->getNextParameterAsString("Corrected Output Segmentation Volume File Name");
   const QString correctionModeString =
      parameters->getNextParameterAsString("Corrected Mode");

   QString correctionsPaintVolumeFileName;
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString("Optional Parameter");
      if (paramName == "-correction-paint-volume") {
         correctionsPaintVolumeFileName =
            parameters->getNextParameterAsString("Paint Volume File Showing Corrections Name");
      }
      else {
         throw CommandException("Unrecognized Parameter: " + paramName);
      }
   }
   
   //
   // Set the correction mode
   //
   BrainModelVolumeTopologyGraphCorrector::CORRECTION_MODE correctionMode =
      BrainModelVolumeTopologyGraphCorrector::CORRECTION_MODE_NORMAL;
   if (correctionModeString == "MINIMAL") {
      correctionMode =
         BrainModelVolumeTopologyGraphCorrector::CORRECTION_MODE_MINIMAL;
   }
   else if (correctionModeString == "NORMAL") {
      correctionMode =
         BrainModelVolumeTopologyGraphCorrector::CORRECTION_MODE_NORMAL;
   }
   else {
      throw CommandException("Invalid correction mode: " + correctionModeString);
   }
   
   //
   // Read volume
   //
   VolumeFile segmentationVolumeFile;
   segmentationVolumeFile.readFile(segmentationVolumeFileName);
   
   //
   // Create graph of foreground
   //
   BrainSet bs;
   BrainModelVolumeTopologyGraphCorrector corrector(&bs,
                                                 correctionMode,
                                                 &segmentationVolumeFile);
   QTime timer;
   timer.start();
   corrector.execute();
   const float totalTime = (static_cast<float>(timer.elapsed()) * 0.001);
   
   //
   // Save corrected volume file
   //
   if (corrector.getCorrectedSegmentationVolumeFile() != NULL) {
      VolumeFile correctedVolumeFile(*corrector.getCorrectedSegmentationVolumeFile());
      correctedVolumeFile.writeFile(correctedSegmentationVolumeFileName);
   }

   if (correctionsPaintVolumeFileName.isEmpty() == false) {
      VolumeFile* paintVol = new VolumeFile(*corrector.getShowingCorrectionsPaintVolumeFile());
      if (paintVol != NULL) {
         paintVol->writeFile(correctionsPaintVolumeFileName);
         delete paintVol;
      }
   }
   
   //
   // Print percent changed
   //
   const int numVoxels = segmentationVolumeFile.getTotalNumberOfVoxels();
   const int numChanged = corrector.getNumberOfVoxelsChanged();
   const float percentChanged = 
      (static_cast<float>(numChanged)
         / static_cast<float>(numVoxels))
      * 100.0;
   std::cout << numChanged
             << " of "
             << numVoxels
             << " ("
             << percentChanged
             << "%) voxels changed in correction."
             << std::endl;

   std::cout << "Time to correct volume topology: "
             << totalTime
             << " seconds."
             << std::endl;   
}
