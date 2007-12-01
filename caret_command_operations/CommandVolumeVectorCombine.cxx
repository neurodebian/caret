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

#include "CommandVolumeVectorCombine.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VectorFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeVectorCombine::CommandVolumeVectorCombine()
   : CommandBase("-volume-vector-combine",
                 "VOLUME VECTOR FILES COMBINE")
{
}

/**
 * destructor.
 */
CommandVolumeVectorCombine::~CommandVolumeVectorCombine()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeVectorCombine::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("DOT_SQRT_RECT_MINUS");   descriptions.push_back("DOT_SQRT_RECT_MINUS");
   values.push_back("2_VEC_NORMAL");   descriptions.push_back("2_VEC_NORMAL");
   values.push_back("2_VEC");   descriptions.push_back("2_VEC");
   
   paramsOut.clear();
   paramsOut.addFile("Input Vector File 1 Name", FileFilters::getVectorFileFilter());
   paramsOut.addFile("Input Vector File 2 Name", FileFilters::getVectorFileFilter());
   paramsOut.addFile("Output Vector File Name", FileFilters::getVectorFileFilter());
   paramsOut.addFile("Mask Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addListOfItems("Operation", values, descriptions);
   paramsOut.addBoolean("Mask Flag");
}

/**
 * get full help information.
 */
QString 
CommandVolumeVectorCombine::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-vector-file-1-name>\n"
       + indent9 + "<input-vector-file-2-name>\n"
       + indent9 + "<output-vector-file-name>\n"
       + indent9 + "<mask-volume-file-name>\n"
       + indent9 + "<operation>\n"
       + indent9 + "<mask-flag>\n"
       + indent9 + "\n"
       + indent9 + "Combine vector files.\n"
       + indent9 + "   \"operation\" is one of:\n"
       + indent9 + "      DOT_SQRT_RECT_MINUS\n"
       + indent9 + "      2_VEC_NORMAL\n"
       + indent9 + "      2_VEC\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeVectorCombine::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVectorFile1Name =
      parameters->getNextParameterAsString("Input Vector File 1 Name");
   const QString inputVectorFile2Name =
      parameters->getNextParameterAsString("Input Vector File 2 Name");
   const QString outputVectorFileName =
      parameters->getNextParameterAsString("Output Vector File Name");
   const QString maskVolumeFileName =
      parameters->getNextParameterAsString("Mask Volume File Name");
   const QString operationString =
      parameters->getNextParameterAsString("Operation").toUpper();
   const bool maskFlag =
      parameters->getNextParameterAsBoolean("Mask Flag");
   checkForExcessiveParameters();
      
   VectorFile::COMBINE_OPERATION operation;
   if (operationString == "DOT_SQRT_RECT_MINUS") {
      operation = VectorFile::COMBINE_OPERATION_DOT_SQRT_RECT_MINUS;
   }
   else if (operationString == "2_VEC_NORMAL") {
      operation = VectorFile::COMBINE_OPERATION_2_VEC_NORMAL;
   }
   else if (operationString == "2_VEC") {
      operation = VectorFile::COMBINE_OPERATION_2_VEC;
   }
   else {
      throw CommandException("Invalid operation \"" + operationString + "\"");
   }
   
   //
   // Read the input files
   //
   VectorFile vector1;
   vector1.readFile(inputVectorFile1Name);
   VectorFile vector2;
   vector2.readFile(inputVectorFile2Name);
   VolumeFile maskVolume;
   maskVolume.readFile(maskVolumeFileName);
   
   //
   // Create the output vector file
   //
   VectorFile outputVector = vector1;
   
   //
   // combine vectors
   //
   VectorFile::combineVectorFiles(maskFlag,
                                     operation,
                                     &vector1,
                                     &vector2,
                                     &maskVolume,
                                     &outputVector);
   //
   // Write the vector file
   //
   outputVector.writeFile(outputVectorFileName);
}

      

