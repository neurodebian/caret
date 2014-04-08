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
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainSet.h"
#include "CommandSurfaceToVolume.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandSurfaceToVolume::CommandSurfaceToVolume()
   : CommandBase("-surface-to-volume",
                 "SURFACE TO VOLUME")
{
}

/**
 * destructor.
 */
CommandSurfaceToVolume::~CommandSurfaceToVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceToVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Metric/Paint/Shape File", FileFilters::getAnyFileFilter());
   paramsOut.addString("Metric/Paint/Shape Column");
   paramsOut.addFile("Output Volume File", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceToVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coordinate-file-name>\n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<input-metric-paint-or-shape-file-name>\n"
       + indent9 + "<input-metric-paint-or-shape-column>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "[-inner  inner-boundary]\n"
       + indent9 + "[-outer  outer-boundary]\n"
       + indent9 + "[-step   intersection-step]\n"
       + indent9 + "\n"
       + indent9 + "Intersect a surface with a volume and assign the specified\n"
       + indent9 + "column's data of the metric, paint, or shape file to the\n"
       + indent9 + "volume.\n"
       + indent9 + "\n"
       + indent9 + "The output volume file must exist and it must be in the \n"
       + indent9 + "same stereotaxic space as the surface.  A volume file may\n"
       + indent9 + "be created by using the \"-volume-create\" or \n"
       + indent9 + "\"-volume-create-in-stereotaxic-space\" commands.\n"
       + indent9 + "\n"
       + indent9 + "The default inner boundary, outer boundar, and step size\n"
       + indent9 + "are -1.5, 1.5, and 0.5 respectively.\n"
       + indent9 + "\n"
       + indent9 + "The metric, paint, or shape file column is either the \n"
       + indent9 + "number of the column, which starts at one, or the name of\n"
       + indent9 + "the column.  If a name contains spaces, it must be \n"
       + indent9 + "enclosed in double quotes.  Name has priority over number.\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceToVolume::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString nodeAttributeFileName = 
      parameters->getNextParameterAsString("Metric/Paint/Shape File Name");
   const QString nodeAttributeColumnIdentifier = 
      parameters->getNextParameterAsString("Metric/Paint/Shape File Column");
   QString outputVolumeFileName = 
      parameters->getNextParameterAsString("Output Volume File Name and Label");
   QString outputVolumeFileLabel;
   splitOutputVolumeNameIntoNameAndLabel(outputVolumeFileName, outputVolumeFileLabel);
   
   //
   // Optional parameters
   //
   float innerBoundary = -1.5;
   float outerBoundary =  1.5;
   float intersectionStep = 0.5;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Surface to Volume Options");
      if (paramName == "-inner") {
         innerBoundary = parameters->getNextParameterAsFloat("Inner Boundary");
      }
      else if (paramName == "-outer") {
         outerBoundary = parameters->getNextParameterAsFloat("Outer Boundary");
      }
      else if (paramName == "-step") {
         intersectionStep = parameters->getNextParameterAsFloat("Intersection Step Size");
      }
      else {
         throw CommandException("Unrecognized option: " + paramName);
      }  
   }
   
   //
   // Create a spec file
   //
   SpecFile specFile;
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getClosedTopoFileTag(), topologyFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getFiducialCoordFileTag(), coordinateFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   int inputDataFileColumnNumber = -1;
   BrainModelSurfaceToVolumeConverter::CONVERSION_MODE conversionMode;
   if (nodeAttributeFileName.endsWith(SpecFile::getMetricFileExtension())
       || nodeAttributeFileName.endsWith(SpecFile::getGiftiFunctionalFileExtension())) {
      specFile.addToSpecFile(SpecFile::getMetricFileTag(), nodeAttributeFileName, 
                             "", SpecFile::SPEC_FALSE);
      conversionMode = BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE;
      
      MetricFile file;
      file.readFile(nodeAttributeFileName);
      inputDataFileColumnNumber = file.getColumnFromNameOrNumber(nodeAttributeColumnIdentifier, false);
   }
   else if (nodeAttributeFileName.endsWith(SpecFile::getPaintFileExtension())
            || nodeAttributeFileName.endsWith(SpecFile::getGiftiLabelFileExtension())) {
      specFile.addToSpecFile(SpecFile::getPaintFileTag(), nodeAttributeFileName, 
                             "", SpecFile::SPEC_FALSE);
      conversionMode = BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_PAINT;
      
      PaintFile file;
      file.readFile(nodeAttributeFileName);
      inputDataFileColumnNumber = file.getColumnFromNameOrNumber(nodeAttributeColumnIdentifier, false);
   }
   else if (nodeAttributeFileName.endsWith(SpecFile::getSurfaceShapeFileExtension())
            || nodeAttributeFileName.endsWith(SpecFile::getGiftiShapeFileExtension())) {
      specFile.addToSpecFile(SpecFile::getSurfaceShapeFileTag(), nodeAttributeFileName, 
                             "", SpecFile::SPEC_FALSE);
      conversionMode = BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE;
      
      SurfaceShapeFile file;
      file.readFile(nodeAttributeFileName);
      inputDataFileColumnNumber = file.getColumnFromNameOrNumber(nodeAttributeColumnIdentifier, false);
   }
   else {
      throw CommandException("Metric/Paint/Shape file extension not valid.");
   }
   
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      throw CommandException("ERROR: Reading spec file: "
                              + errorMessage);
   }
   if (errorMessage.length() > 0) {
      throw CommandException("ERROR: Reading spec file: "
                              + errorMessage);
   }
   
   //
   // Get the surface
   //
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Unable to find surface, check file names.");
   }
   
   //
   // Read the volume file
   //
   VolumeFile outputVolumeFile;
   outputVolumeFile.readFile(outputVolumeFileName);
   int dimensions[3];
   outputVolumeFile.getDimensions(dimensions);
   float spacing[3];
   outputVolumeFile.getSpacing(spacing);
   float origin[3];
   outputVolumeFile.getOrigin(origin);

   //
   // Convert the surface to a segmentation volume
   //
   const float surfaceOffset[3] = { 0.0, 0.0, 0.0 };
   BrainModelSurfaceToVolumeConverter bmsv(&brainSet,
                                           bms,
                                           StereotaxicSpace::SPACE_UNKNOWN,
                                           surfaceOffset,
                                           dimensions,
                                           spacing,
                                           origin,
                                           innerBoundary,
                                           outerBoundary,
                                           intersectionStep,
                                           conversionMode);
   bmsv.setNodeAttributeColumn(inputDataFileColumnNumber);
   bmsv.execute();
      
   VolumeFile* vf = bmsv.getOutputVolume();
   if (vf != NULL) {
      writeVolumeFile(*vf, outputVolumeFileName, outputVolumeFileLabel);
   }
   else {
      throw CommandException("Failed to create volume.");
   }
}

      

