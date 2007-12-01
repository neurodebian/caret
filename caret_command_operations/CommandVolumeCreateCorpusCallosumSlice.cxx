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

#include "BrainModelVolumeSureFitSegmentation.h"
#include "CommandVolumeCreateCorpusCallosumSlice.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "Structure.h"

/**
 * constructor.
 */
CommandVolumeCreateCorpusCallosumSlice::CommandVolumeCreateCorpusCallosumSlice()
   : CommandBase("-volume-create-corpus-callosum-slice",
                 "VOLUME CREATE CORPUS CALLOSUM SLICE")
{
}

/**
 * destructor.
 */
CommandVolumeCreateCorpusCallosumSlice::~CommandVolumeCreateCorpusCallosumSlice()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeCreateCorpusCallosumSlice::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes, structureNames, false);
      
   paramsOut.clear();
   paramsOut.addFile("Anatomy Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addListOfItems("Structure", structureNames, structureNames);
   paramsOut.addVariableListOfParameters("Callosum Slice Options");
}

/**
 * get full help information.
 */
QString 
CommandVolumeCreateCorpusCallosumSlice::getHelpInformation() const
{
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes, structureNames, false);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <anatomy-volume-file-name>\n"
       + indent9 + "   <output-volume-file-name>\n"
       + indent9 + "   <structure>\n"
       + indent9 + "   [-gray   gray-matter-peak]\n"
       + indent9 + "   [-white  white-matter-peak]\n"
       + indent9 + "\n"
       + indent9 + "Create the corpus callosum slice from an anatomical volume.\n"
       + indent9 + "If the gray and white matter peaks are not specified, they\n"
       + indent9 + "will be estimated.\n"
       + indent9 + "\n");
       
       helpInfo += (indent9 + "Examples of \"structure\" are: \n");
          for (int i = 0; i < static_cast<int>(structureNames.size()); i++) {
             helpInfo += (indent9 + "   " + structureNames[i] + "\n");
          }
          
       helpInfo +=
        (indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeCreateCorpusCallosumSlice::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Anatomy Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name",
                                                        outputVolumeFileName,
                                                        outputVolumeFileLabel);
   const Structure structure = parameters->getNextParameterAsStructure("Structure");
   
   //
   // Optional parameters
   //
   float grayMatterPeak = -1.0;
   float whiteMatterPeak = -1.0;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Volume Create Corpus Callosum Option");
      if (paramName == "-gray") {
         grayMatterPeak = parameters->getNextParameterAsFloat("Gray Matter Peak");
      }
      else if (paramName == "-white") {
         whiteMatterPeak = parameters->getNextParameterAsFloat("White Matter Peak");
      }
      else {
         throw CommandException("Volume Create Corpus Callosum invalid parameter "
                                + paramName);
      }
   }
   
   //
   // Read input file
   //
   VolumeFile anatomyVolume;
   anatomyVolume.readFile(inputVolumeFileName);
   
   //
   // Generate the corpus callosum slice volume
   //
   VolumeFile corpusCallosumVolume;
   BrainModelVolumeSureFitSegmentation::generateCorpusCallosumSlice(
                                                             anatomyVolume,
                                                             corpusCallosumVolume,
                                                             structure,
                                                             grayMatterPeak,
                                                             whiteMatterPeak);
   
   //
   // Write the volume file
   //
   corpusCallosumVolume.setDescriptiveLabel(outputVolumeFileLabel);
   corpusCallosumVolume.writeFile(outputVolumeFileName);
}

      

