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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceToVolumeSegmentationConverter.h"
#include "BrainSet.h"
#include "CommandSurfaceToSegmentationVolume.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandSurfaceToSegmentationVolume::CommandSurfaceToSegmentationVolume()
   : CommandBase("-surface-to-segmentation-volume",
                 "SURFACE TO SEGMENTATION VOLUME")
{
}

/**
 * destructor.
 */
CommandSurfaceToSegmentationVolume::~CommandSurfaceToSegmentationVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceToSegmentationVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Volume File", FileFilters::getVolumeGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceToSegmentationVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coordinate-file-name>\n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Intersect a surface with a volume and create a segmentation\n"
       + indent9 + "volume.\n"
       + indent9 + "\n"
       + indent9 + "The output volume file must exist and it must be in the \n"
       + indent9 + "same stereotaxic space as the surface.  A volume file may\n"
       + indent9 + "be created by using the \"-volume-create\" command.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceToSegmentationVolume::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Required parameters
   //
   const QString coordinateFileName = 
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName = 
      parameters->getNextParameterAsString("Topology File Name");
   QString outputVolumeFileName = 
      parameters->getNextParameterAsString("Output Volume File Name and Label");
   QString outputVolumeFileLabel;
   splitOutputVolumeNameIntoNameAndLabel(outputVolumeFileName, outputVolumeFileLabel);

   //
   // Read the volume file
   //
   VolumeFile outputVolumeFile;
   outputVolumeFile.readFile(outputVolumeFileName);

   //
   // Create a brain set from coord and topo
   //
   BrainSet brain(topologyFileName, coordinateFileName);
   BrainModelSurface* bms = brain.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Unable to find surface after reading files.");
   }
   
   //
   // Create the segmentation volume
   //
   BrainModelSurfaceToVolumeSegmentationConverter bmssc(&brain,
                                                        bms,
                                                        &outputVolumeFile,
                                                        false,
                                                        false);
   bmssc.execute();
   writeVolumeFile(outputVolumeFile, outputVolumeFileName, outputVolumeFileLabel);      
}

      

