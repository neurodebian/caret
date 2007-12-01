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
#include "BrainSet.h"
#include "CommandVolumeMapToSurfaceROIFile.h"
#include "FileFilters.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TransformationMatrixFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeMapToSurfaceROIFile::CommandVolumeMapToSurfaceROIFile()
   : CommandBase("-volume-map-to-surface-roi-file",
                 "VOLUME MAP TO SURFACE ROI FILE")
{
}

/**
 * destructor.
 */
CommandVolumeMapToSurfaceROIFile::~CommandVolumeMapToSurfaceROIFile()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMapToSurfaceROIFile::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Input Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Region of Interest File Name", FileFilters::getRegionOfInterestFileFilter());
   paramsOut.addVariableListOfParameters("Volume Map Options");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMapToSurfaceROIFile::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<input-coordinate-file-name> \n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<output-region-of-interest-file-name>\n"
       + indent9 + "[-surface-offset  X  Y  Z]\n"
       + indent9 + "\n"
       + indent9 + "Intersect a volume with a surface and for all nodes that are\n"
       + indent9 + "within a non-zero voxel, assign the node as selcted in the\n"
       + indent9 + "region of interest file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeMapToSurfaceROIFile::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   const QString inputCoordinateFileName =
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString outputROIFileName =
      parameters->getNextParameterAsString("Output Node Region of Interest File Name");
      
   //
   // Get optional parmameters
   //
   float surfaceOffset[3] = { 0.0, 0.0, 0.0 };
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Volume Map to Surface ROI Parameter");
      if (paramName == "-surfaceOffset") {
         surfaceOffset[0] = parameters->getNextParameterAsFloat("Surface Offset X");
         surfaceOffset[1] = parameters->getNextParameterAsFloat("Surface Offset Y");
         surfaceOffset[2] = parameters->getNextParameterAsFloat("Surface Offset Z");
      }
      else {
         throw CommandException("unknown parameter "
                                + paramName);
      }
   }

   //
   // Create a brain set
   //
   BrainSet brainSet(inputTopologyFileName,
                     inputCoordinateFileName,
                     "",
                     true);
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read the volume file
   //
   VolumeFile volumeFile;
   volumeFile.readFile(inputVolumeFileName);
   
   //
   // Apply the surface translation
   //
   TransformationMatrix matrix;
   matrix.translate(surfaceOffset);
   surface->applyTransformationMatrix(matrix);
   
   //
   // Create the region of interest file
   //
   NodeRegionOfInterestFile roiFile;
   roiFile.setNumberOfNodes(numNodes);

   //
   // Assign the roi
   //
   roiFile.assignSelectedNodesWithVolumeFile(&volumeFile,
                                             surface->getCoordinateFile(),
                                             surface->getTopologyFile());
                                             
   //
   // Write the roi file
   //
   roiFile.writeFile(outputROIFileName);
}

      

