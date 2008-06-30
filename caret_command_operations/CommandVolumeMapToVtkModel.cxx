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

#include "BrainModelVolumeToVtkSurfaceMapper.h"
#include "BrainSet.h"
#include "CommandVolumeMapToVtkModel.h"
#include "FileFilters.h"
#include "PaletteFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"
#include "VtkModelFile.h"

/**
 * constructor.
 */
CommandVolumeMapToVtkModel::CommandVolumeMapToVtkModel()
   : CommandBase("-volume-map-to-vtk-model",
                 "VOLUME MAP TO VTK MODEL")
{
}

/**
 * destructor.
 */
CommandVolumeMapToVtkModel::~CommandVolumeMapToVtkModel()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMapToVtkModel::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input VTK Model File", FileFilters::getVtkModelFileFilter());
   paramsOut.addFile("Output VTK Model File", FileFilters::getVtkModelFileFilter());
   paramsOut.addFile("Input Volume File", FileFilters::getVolumeFunctionalFileFilter());
   paramsOut.addInt("Input Sub-Volume Number", 1);
   paramsOut.addString("Palette Name or Number", "1");
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMapToVtkModel::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-vtk-model-file-name>\n"
       + indent9 + "<output-vtk-model-file-name>\n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<input-volume-file-sub-volume-number>\n"
       + indent9 + "<input-palette-name-or-number>\n"
       + indent9 + "[-palette-file input-palette_file-name]\n"
       + indent9 + "\n"
       + indent9 + "Map the specified volume to the VTK model and color the\n"
       + indent9 + "model using the specified palette.\n"
       + indent9 + "\n"
       + indent9 + "The sub-volume number is the index of the sub-volume in\n"
       + indent9 + "the volume file and the numbers start at 1.\n"
       + indent9 + "\n"
       + indent9 + "If a number is used for the palette, the numbers start at 1.\n"
       + indent9 + "\n"
       + indent9 + "If a palette file is NOT specified, the following palettes\n"
       + indent9 + "are available:\n");

   PaletteFile pf;
   for (int i = 0; i < pf.getNumberOfPalettes(); i++) {
      helpInfo += (indent9 + "   " + pf.getPalette(i)->getName());
   }

   helpInfo += (
       indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeMapToVtkModel::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputVtkModelFileName =
      parameters->getNextParameterAsString("Input VTK Model File Name");
   const QString outputVtkModelFileName =
      parameters->getNextParameterAsString("Output VTK Model File Name");
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   const int inputSubVolumeNumber = 
      parameters->getNextParameterAsInt("Input Sub-Volume Number");
   const QString inputPaletteNameOrNumber =
      parameters->getNextParameterAsString("Input Palette Name or Number");
   QString inputPaletteFileName;
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString("Optional parameter");
      if (paramName == "-palette-file") {
         inputPaletteFileName = 
            parameters->getNextParameterAsString("Palette File Name");
      }
   }
      
   //
   // Read the VTK Model
   //
   VtkModelFile vtkModelFile;
   vtkModelFile.readFile(inputVtkModelFileName);
   
   //
   // Read the volume file
   //
   std::vector<VolumeFile*> volumeFiles;
   VolumeFile::readFile(inputVolumeFileName,
                        -1,
                        volumeFiles);
   const int numberOfVolumesRead = static_cast<int>(volumeFiles.size());
                     
   //
   // Get the volume for mapping
   //
   VolumeFile* mappingVolume;
   if ((inputSubVolumeNumber > 0) &&
       (inputSubVolumeNumber <= numberOfVolumesRead)) {
      mappingVolume = volumeFiles[inputSubVolumeNumber - 1];
   }
   else {
      throw CommandException("The Sub-Volume Number for the input volume must be between 1 and "
                             + QString::number(numberOfVolumesRead)
                             + ".");
   }
   
   //
   // Read the palette file
   //
   PaletteFile paletteFile;
   if (inputPaletteFileName.isEmpty() == false) {
      paletteFile.readFile(inputPaletteFileName);
   }
   
   //
   // Get the index of the palette
   //
   const int paletteIndex =
      paletteFile.getPaletteIndexFromNameOrNumber(inputPaletteNameOrNumber);

   //
   // Map the volume
   //
   BrainSet brainSet;
   BrainModelVolumeToVtkSurfaceMapper mapper(&brainSet,
                                             &vtkModelFile,
                                             mappingVolume,
                                             &paletteFile,
                                             paletteIndex);
   mapper.execute();
   
   //
   // Write the VTK model file
   //
   vtkModelFile.writeFile(outputVtkModelFileName);
   
   //
   // free the volumes in memory
   //
   for (int i = 0; i < numberOfVolumesRead; i++) {
      delete volumeFiles[i];
   }
}

      

