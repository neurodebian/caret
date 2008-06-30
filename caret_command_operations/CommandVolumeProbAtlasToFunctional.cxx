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

#include "BrainModelVolume.h"
#include "BrainModelVolumeProbAtlasToFunctional.h"
#include "BrainSet.h"
#include "CommandVolumeProbAtlasToFunctional.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeProbAtlasToFunctional::CommandVolumeProbAtlasToFunctional()
   : CommandBase("-volume-prob-atlas-to-functional",
                 "VOLUME PROB ATLAS TO FUNCTIONAL VOLUMES")
{
}

/**
 * destructor.
 */
CommandVolumeProbAtlasToFunctional::~CommandVolumeProbAtlasToFunctional()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeProbAtlasToFunctional::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Prob Atlas Volume",
                     FileFilters::getVolumeProbAtlasFileFilter());
   paramsOut.addString("Functional Volume Name Prefix");
   paramsOut.addString("Functional Volume Name Suffix");
}

/**
 * get full help information.
 */
QString 
CommandVolumeProbAtlasToFunctional::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-prob-atlas-volume-file-name>\n"
       + indent9 + "<output-functional-volume-prefix-name>\n"
       + indent9 + "<output-functional-volume-suffix-name>\n"
       + indent9 + "\n"
       + indent9 + "For each region name that is not ???, GYRAL, or GYRUS,\n"
       + indent9 + "create a functional volume where each voxel is the\n"
       + indent9 + "number volumes that have the region identified for\n"
       + indent9 + "the voxel.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeProbAtlasToFunctional::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get inputs
   //
   const QString inputProbAtlasVolumeFileName =
      parameters->getNextParameterAsString("Input Prob Atlas Volume File Name");
   const QString functionalVolumePrefixName =
      parameters->getNextParameterAsString("Output Functional Volume File Name Prefix");
   const QString functionalVolumeSuffixName =
      parameters->getNextParameterAsString("Output Functional Volume File Name Suffix");


   //
   // Create and read the prob atlas volume
   //
   BrainSet bs;
   bs.readVolumeFile(inputProbAtlasVolumeFileName,
                     VolumeFile::VOLUME_TYPE_PROB_ATLAS,
                     false,
                     false);
   BrainModelVolume* bmv = bs.getBrainModelVolume();
   if (bmv == NULL) {
      throw CommandException("Unable to find volumes after reading files.");
   }
   if (bs.getNumberOfVolumeProbAtlasFiles() <= 0) {
      throw CommandException("No prob atlas volume files were read.");
   }
   VolumeFile* firstProbAtlasVolumeFile = NULL;
   if (bs.getNumberOfVolumeProbAtlasFiles() > 0) {
      firstProbAtlasVolumeFile = bs.getVolumeProbAtlasFile(0);
   }
   
   //
   // Create the functional volume 
   //
   VolumeFile functionalVolume(*bs.getVolumeProbAtlasFile(0));
   functionalVolume.setVolumeType(VolumeFile::VOLUME_TYPE_FUNCTIONAL);
   
   //
   // Process each prob atlas name
   //
   DisplaySettingsProbabilisticAtlas* dspa = bs.getDisplaySettingsProbabilisticAtlasVolume();
   const int numNames = firstProbAtlasVolumeFile->getNumberOfRegionNames();
   for (int i = 0; i < numNames; i++) {
      //
      // Get the name
      //
      const QString areaName = firstProbAtlasVolumeFile->getRegionNameFromIndex(i);
      if ((areaName != "???") &&
          (areaName != "GYRAL") &&
          (areaName != "GYRUS")) {
         //
         // Turn on only this name
         //
         for (int j = 0; j < numNames; j++) {
            if (i == j) {
               dspa->setAreaSelected(j, true);
            }
            else {
               dspa->setAreaSelected(j, false);
            }
         }
         
         //
         // Name for functional volume
         //
         const QString funcVolumeName =
            functionalVolumePrefixName
            + areaName
            + functionalVolumeSuffixName;
            
         //
         // Convert to functional volume
         //
         BrainModelVolumeProbAtlasToFunctional paf(&bs,
                                                   &functionalVolume,
                                                   funcVolumeName,
                                                   areaName);
         paf.execute();
         
         //
         // Write the functional volume
         //
         functionalVolume.writeFile(funcVolumeName);
      }
   }
   
   
}

      

