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

#include <QDir>
#include <QFile>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainSet.h"
#include "CommandSurfacesToSegmentationVolumeMask.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandSurfacesToSegmentationVolumeMask::CommandSurfacesToSegmentationVolumeMask()
   : CommandBase("-surfaces-to-segmentation-volume-mask",
                 "SURFACES TO SEGMENTATION VOLUME MASK")
{
}

/**
 * destructor.
 */
CommandSurfacesToSegmentationVolumeMask::~CommandSurfacesToSegmentationVolumeMask()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfacesToSegmentationVolumeMask::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Spec File", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Output Volume File", FileFilters::getVolumeSegmentationFileFilter());
   paramsOut.addVariableListOfParameters("Optional Parameters");
}

/**
 * get full help information.
 */
QString 
CommandSurfacesToSegmentationVolumeMask::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-spec-file>\n"
       + indent9 + "<output-volume-file>\n"
       + indent9 + "[-dilate  number-of-iterations]\n"
       + indent9 + "\n"
       + indent9 + "For each fiducial surface listed in the spec file, convert it\n"
       + indent9 + "to a segmentation volume and then merge (union) all of these\n"
       + indent9 + "segmentation volumes into a single volume.  Perform the specified\n"
       + indent9 + "number of dilation iterations on the output volume prior to \n"
       + indent9 + "saving it.\n"
       + indent9 + "\n"
       + indent9 + "The output volume must exist and a volume may be created by this\n"
       + indent9 + "program using the \"-volume-create-in-stereotaxic-space\" option.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfacesToSegmentationVolumeMask::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputSpecFileName = 
      parameters->getNextParameterAsString("Input Spec File");
                                          
   const QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File");
   
   int dilationIterations = 0;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Param");
      if (paramName == "dilate") {
         dilationIterations = parameters->getNextParameterAsInt("Dilation Iterations");
      }
   }
   
   //
   // Save directory
   //
   const QString currentDirectoryName = QDir::currentPath();
   
   //
   // Create a spec file
   //
   SpecFile specFile;
   specFile.readFile(inputSpecFileName);
   specFile.setAllFileSelections(SpecFile::SPEC_TRUE);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      throw CommandException("ERROR: Reading spec file: "
                             + errorMessage.toAscii());
   }
   
   //
   // Reading brain set will change directory to one containing spec file
   //
   QDir::setCurrent(currentDirectoryName);
   
   //
   // Read the volume file and clear it
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
   // Loop through the surfaces
   //
   std::vector<QString> segmentationVolumeFileNames;
   for (int i = 0; i < brainSet.getNumberOfBrainModels(); i++) {
      //
      // See if model is fiducial surface
      //
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
         //
         // Convert the surface to a segmentation volume
         //
         const float surfaceOffset[3] = { 0.0, 0.0, 0.0 };
         const float innerBoundary = -1.5;
         const float outerBoundary =  1.5;
         const float intersectionStep = 0.5;
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
                                                 BrainModelSurfaceToVolumeConverter::CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES);
         bmsv.execute();
         
         VolumeFile* vf = bmsv.getOutputVolume();
         if (vf != NULL) {
            const QString name("TempSegmentVolume_" 
                               + QString::number(i)
                               + SpecFile::getNiftiVolumeFileExtension());
            vf->writeFile(name);
            segmentationVolumeFileNames.push_back(name);
            brainSet.deleteVolumeFile(vf);
         }
         else {
            throw CommandException("ERROR: Failed to create volume for surface:"
                                   + bms->getFileName());
         }
      }
   }
   
   //
   // Create the mask
   //
   VolumeFile::createSegmentationMask(outputVolumeFileName,
                                      segmentationVolumeFileNames,
                                      dilationIterations);
   
   //
   // If not debug delete temp volumes
   //
   if (DebugControl::getDebugOn() == false) {
      for (unsigned int i = 0; i < segmentationVolumeFileNames.size(); i++) {
         QFile::remove(segmentationVolumeFileNames[i]);
      }
   }
}

      

